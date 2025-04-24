#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/un.h>

#include "logger.h"

static syscall_hook_fn_t real_syscall = NULL;

static void escape_and_print(const char* buf, size_t len, FILE* stream);
static void log_connect(int64_t fd, const void* addr, int64_t addrlen, int64_t ret);
static void log_execve(const char* filename, const void* argv, const void* envp);

int64_t logger_hook(int64_t arg1, int64_t arg2, int64_t arg3,
                    int64_t arg4, int64_t arg5, int64_t arg6,
                    int64_t syscall_no) {
    int64_t ret;

    switch (syscall_no) {
        case SYS_openat:
            fprintf(stderr, "[logger] openat\n");
            ret = real_syscall(arg1, arg2, arg3, arg4, arg5, arg6, syscall_no);
            // fprintf(stderr, "[logger] openat(%s, \"%s\", 0x%lx, %#o) = %ld\n",
            //     (arg1 == AT_FDCWD) ? "AT_FDCWD" : (char[]){0}, // TODO: print int if not AT_FDCWD
            //     (char*)arg2, (unsigned long)arg3, (unsigned int)arg4, ret);
            break;

        case SYS_read:
            fprintf(stderr, "[logger] read\n");
            ret = real_syscall(arg1, arg2, arg3, arg4, arg5, arg6, syscall_no);
            // fprintf(stderr, "[logger] read(%ld, \"", arg1);
            // escape_and_print((const char*)arg2, ret > 32 ? 32 : ret, stderr);
            // if (ret > 32) fprintf(stderr, "...");
            // fprintf(stderr, "\", %ld) = %ld\n", arg3, ret);
            break;

        case SYS_write:
            fprintf(stderr, "[logger] write\n");
            ret = real_syscall(arg1, arg2, arg3, arg4, arg5, arg6, syscall_no);
            // fprintf(stderr, "[logger] write(%ld, \"", arg1);
            // escape_and_print((const char*)arg2, ret > 32 ? 32 : ret, stderr);
            // if (ret > 32) fprintf(stderr, "...");
            // fprintf(stderr, "\", %ld) = %ld\n", arg3, ret);
            break;

        case SYS_connect:
            fprintf(stderr, "[logger] connect\n");
            ret = real_syscall(arg1, arg2, arg3, arg4, arg5, arg6, syscall_no);
            // log_connect(arg1, (const void*)arg2, arg3, ret);
            break;

        case SYS_execve:
            log_execve((const char*)arg1, (void*)arg2, (void*)arg3); // log before syscall
            ret = real_syscall(arg1, arg2, arg3, arg4, arg5, arg6, syscall_no);
            break;

        default:
            ret = real_syscall(arg1, arg2, arg3, arg4, arg5, arg6, syscall_no);
            break;
    }

    return ret;
}

void __hook_init(const syscall_hook_fn_t trigger_syscall,
                 syscall_hook_fn_t* hooked_syscall) {
    real_syscall = trigger_syscall;
    *hooked_syscall = logger_hook;
}

// --- Helper functions ---

static void escape_and_print(const char* buf, size_t len, FILE* stream) {
    for (size_t i = 0; i < len; ++i) {
        unsigned char c = buf[i];
        if (c == '\n') fprintf(stream, "\\n");
        else if (c == '\t') fprintf(stream, "\\t");
        else if (c == '\r') fprintf(stream, "\\r");
        else if (c >= 32 && c <= 126) fputc(c, stream);
        else fprintf(stream, "\\x%02x", c);
    }
}

static void log_connect(int64_t fd, const void* addr, int64_t addrlen, int64_t ret) {
    const struct sockaddr* sa = (const struct sockaddr*)addr;

    if (sa->sa_family == AF_INET) {
        char ip[INET_ADDRSTRLEN];
        struct sockaddr_in* sin = (struct sockaddr_in*)addr;
        inet_ntop(AF_INET, &sin->sin_addr, ip, sizeof(ip));
        fprintf(stderr, "[logger] connect(%ld, \"%s:%d\", %ld) = %ld\n",
                fd, ip, ntohs(sin->sin_port), addrlen, ret);
    } else if (sa->sa_family == AF_INET6) {
        char ip[INET6_ADDRSTRLEN];
        struct sockaddr_in6* sin6 = (struct sockaddr_in6*)addr;
        inet_ntop(AF_INET6, &sin6->sin6_addr, ip, sizeof(ip));
        fprintf(stderr, "[logger] connect(%ld, \"%s:%d\", %ld) = %ld\n",
                fd, ip, ntohs(sin6->sin6_port), addrlen, ret);
    } else if (sa->sa_family == AF_UNIX) {
        struct sockaddr_un* sun = (struct sockaddr_un*)addr;
        fprintf(stderr, "[logger] connect(%ld, \"UNIX:%s\", %ld) = %ld\n",
                fd, sun->sun_path, addrlen, ret);
    } else {
        fprintf(stderr, "[logger] connect(%ld, \"UNKNOWN\", %ld) = %ld\n",
                fd, addrlen, ret);
    }
}

static void log_execve(const char* filename, const void* argv, const void* envp) {
    fprintf(stderr, "[logger] execve(\"%s\", %p, %p)\n", filename, argv, envp);
}