#ifndef INIT_HOOK_H
#define INIT_HOOK_H

#include "hook.h"

extern syscall_hook_fn_t hooked_syscall;
void init_hook();

#endif // INIT_HOOK_H