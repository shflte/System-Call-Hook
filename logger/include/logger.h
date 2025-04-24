#ifndef LOGGER_H
#define LOGGER_H

#include "hook.h"

void __hook_init(const syscall_hook_fn_t trigger_syscall,
                 syscall_hook_fn_t* hooked_syscall);

#endif // LOGGER_H