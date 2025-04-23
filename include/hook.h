#ifndef HOOK_H
#define HOOK_H

#include <stdbool.h>
#include <stdint.h>

typedef int64_t (*syscall_hook_fn_t)(int64_t, int64_t, int64_t, int64_t,
                                     int64_t, int64_t, int64_t);
extern syscall_hook_fn_t hooked_syscall;
void init_hook_library();

#endif // HOOK_H