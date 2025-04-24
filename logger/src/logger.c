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
#include <sys/stat.h>
#include <fcntl.h>

#include "logger.h"

#define LOG_STRING_LEN 32

static syscall_hook_fn_t real_syscall = NULL;

static void escape_string(char* out, const char* buf, ssize_t len);
static void format_sockaddr(const struct sockaddr *addr, socklen_t len, char *out, size_t outlen);

static void log_openat(int64_t dirfd, const char *path, int64_t flags, int64_t mode, int64_t ret);
static void log_read(int fd, const void* buf, size_t count, ssize_t ret);
static void log_write(int fd, const void* buf, size_t count, ssize_t ret);
static void log_connect(int fd, const struct sockaddr *addr, socklen_t addrlen, int ret);
static void log_execve(const char* filename, const void* argv, const void* envp);

int64_t logger_hook(int64_t arg1, int64_t arg2, int64_t arg3,
                    int64_t arg4, int64_t arg5, int64_t arg6,
                    int64_t syscall_no) {
    int64_t ret;

    switch (syscall_no) {
        case SYS_openat:
            ret = real_syscall(arg1, arg2, arg3, arg4, arg5, arg6, syscall_no);
            log_openat(arg1, (const char*)arg2, arg3, arg4, ret);
            break;

        case SYS_read:
            ret = real_syscall(arg1, arg2, arg3, arg4, arg5, arg6, syscall_no);
            log_read(arg1, (const void*)arg2, arg3, ret);
            break;

        case SYS_write:
            ret = real_syscall(arg1, arg2, arg3, arg4, arg5, arg6, syscall_no);
            log_write(arg1, (const void*)arg2, arg3, ret);
            break;

        case SYS_connect:
            ret = real_syscall(arg1, arg2, arg3, arg4, arg5, arg6, syscall_no);
            log_connect(arg1, (const struct sockaddr*)arg2, arg3, ret);
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

static void escape_string(char* out, const char* buf, ssize_t len) {
    size_t out_i = 0;
    len = (len > LOG_STRING_LEN) ? LOG_STRING_LEN : len;
    for (size_t i = 0; i < len; ++i) {
        unsigned char c = buf[i];
        if (c == '\n') {
            out[out_i++] = '\\'; out[out_i++] = 'n';
        } else if (c == '\t') {
            out[out_i++] = '\\'; out[out_i++] = 't';
        } else if (c == '\r') {
            out[out_i++] = '\\'; out[out_i++] = 'r';
        } else if (c >= 32 && c <= 126) {
            out[out_i++] = c;
        } else {
            sprintf(out + out_i, "\\x%02x", c);
            out_i += 4;
        }
    }
    out[out_i] = '\0';
}

static void format_sockaddr(const struct sockaddr *addr, socklen_t len, char *out, size_t outlen) {
    if (!addr || len == 0) {
        snprintf(out, outlen, "UNKNOWN");
        return;
    }

    if (addr->sa_family == AF_INET) {
        const struct sockaddr_in *in = (const struct sockaddr_in *)addr;
        char ip[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &(in->sin_addr), ip, sizeof(ip));
        snprintf(out, outlen, "%s:%d", ip, ntohs(in->sin_port));
    } else if (addr->sa_family == AF_INET6) {
        const struct sockaddr_in6 *in6 = (const struct sockaddr_in6 *)addr;
        char ip[INET6_ADDRSTRLEN];
        inet_ntop(AF_INET6, &(in6->sin6_addr), ip, sizeof(ip));
        snprintf(out, outlen, "%s:%d", ip, ntohs(in6->sin6_port));
    } else if (addr->sa_family == AF_UNIX) {
        const struct sockaddr_un *un = (const struct sockaddr_un *)addr;
        snprintf(out, outlen, "UNIX:%s", un->sun_path[0] ? un->sun_path : "(anonymous)");
    } else {
        snprintf(out, outlen, "UNKNOWN");
    }
}

void log_openat(int64_t dirfd_raw, const char *path, int64_t flags, int64_t mode, int64_t ret) {
    int dirfd = (int)dirfd_raw;
    char dirfd_str[32];
    snprintf(dirfd_str, sizeof(dirfd_str), "%d", dirfd);
    fprintf(stderr, "[logger] openat(%s, \"%s\", 0x%lx, %#o) = %ld\n",
            (dirfd == AT_FDCWD) ? "AT_FDCWD" : dirfd_str,
            (char*)path, (unsigned long)flags, (unsigned int)mode, ret);
}

void log_read(int fd, const void* buf, size_t count, ssize_t ret) {
    char escaped_buf[256];
    char truncated_string[4];

    escape_string(escaped_buf, (const char*)buf, ret);
    sprintf(truncated_string, "%s", (ret > LOG_STRING_LEN) ? "..." : "");
    fprintf(stderr, "[logger] read(%d, \"%s\"%s, %zu) = %zd\n",
            fd, escaped_buf, truncated_string, count, ret);
}

void log_write(int fd, const void* buf, size_t count, ssize_t ret) {
    char escaped_buf[256];
    char truncated_string[4];

    escape_string(escaped_buf, (const char*)buf, ret);
    sprintf(truncated_string, "%s", (ret > LOG_STRING_LEN) ? "..." : "");
    fprintf(stderr, "[logger] write(%d, \"%s\"%s, %zu) = %zd\n",
            fd, escaped_buf, truncated_string, count, ret);
}

void log_connect(int fd, const struct sockaddr *addr, socklen_t addrlen, int ret) {
    char addr_buf[128];
    format_sockaddr(addr, addrlen, addr_buf, sizeof(addr_buf));

    fprintf(stderr, "[logger] connect(%d, \"%s\", %d) = %d\n", fd, addr_buf, addrlen, ret);
}

static void log_execve(const char* filename, const void* argv, const void* envp) {
    fprintf(stderr, "[logger] execve(\"%s\", %p, %p)\n", filename, argv, envp);
}