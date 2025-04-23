#include <stdint.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <sys/syscall.h>

#include "trampoline.h"

static char leet_to_char(char c) {
    switch (c) {
        case '0': return 'o';
        case '1': return 'i';
        case '2': return 'z';
        case '3': return 'e';
        case '4': return 'a';
        case '5': return 's';
        case '6': return 'g';
        case '7': return 't';
        default:  return c;
    }
}

static char decoded[4096];

int64_t syscall_hook_handler(int64_t num, int64_t arg1, int64_t arg2, int64_t arg3,
                             int64_t arg4, int64_t arg5, int64_t arg6) {
    if (num == SYS_write && arg1 == STDOUT_FILENO) {
        char* buf = (char*)arg2;
        size_t len = (size_t)arg3;

        // Copy and decode buffer
        if (len >= sizeof(decoded)) len = sizeof(decoded) - 1;
        for (size_t i = 0; i < len; ++i) {
            decoded[i] = leet_to_char(buf[i]);
        }
        decoded[len] = '\0';

        // Replace buffer and length
        arg2 = (int64_t)decoded;
        arg3 = (int64_t)strlen(decoded);
    }

    return trigger_syscall(num, arg1, arg2, arg3, arg4, arg5, arg6);
}
