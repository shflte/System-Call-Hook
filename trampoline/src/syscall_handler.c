#include <stdint.h>

#include "trampoline.h"
#include "init_hook.h"

int64_t syscall_handler(int64_t arg1, int64_t arg2, int64_t arg3,
                        int64_t arg4, int64_t arg5, int64_t arg6,
                        int64_t num)
{
    return hooked_syscall(arg1, arg2, arg3,
                          arg4, arg5, arg6,
                          num);
}