#ifndef TRAMPOLINE_H
#define TRAMPOLINE_H

#include <stdbool.h>

#define TRAMPOLINE_ADDR 0x200

// Trampoline Setup
void* trampoline_entry();
void* get_trampoline_address();
int64_t syscall_hook_handler(int64_t syscall_no, int64_t arg1, int64_t arg2, int64_t arg3,
                             int64_t arg4, int64_t arg5, int64_t arg6);
int64_t trigger_syscall(int64_t syscall_no, int64_t arg1, int64_t arg2, int64_t arg3,
                        int64_t arg4, int64_t arg5, int64_t arg6);

// Hooking Library
typedef int64_t (*syscall_hook_fn_t)(int64_t, int64_t, int64_t, int64_t,
                                     int64_t, int64_t, int64_t);
extern syscall_hook_fn_t hooked_syscall;
extern bool using_hook_library;
syscall_hook_fn_t get_hooked_syscall();
void init_hook_library();

#endif // TRAMPOLINE_H