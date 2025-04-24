#ifndef COMMON_HOOK_H
#define COMMON_HOOK_H

#include <stdint.h>

typedef int64_t (*syscall_hook_fn_t)(
    int64_t, int64_t, int64_t,
    int64_t, int64_t, int64_t,
    int64_t
);

#endif // COMMON_HOOK_H