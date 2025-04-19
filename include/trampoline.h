#ifndef TRAMPOLINE_H
#define TRAMPOLINE_H

#define TRAMPOLINE_ADDR 0x200

void* trampoline_entry();
void* get_trampoline_address();
int64_t syscall_hook_handler(int64_t syscall_no, int64_t arg1, int64_t arg2, int64_t arg3,
                             int64_t arg4, int64_t arg5, int64_t arg6);
int64_t trigger_syscall(int64_t syscall_no, int64_t arg1, int64_t arg2, int64_t arg3,
                        int64_t arg4, int64_t arg5, int64_t arg6);

#endif // TRAMPOLINE_H