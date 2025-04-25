#include <stdint.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <linux/sched.h>

#include "trampoline.h"
#include "init_hook.h"

static __thread uint64_t retptr_tls;

void set_retptr(uint64_t retptr) {
    retptr_tls = retptr;
}

int64_t syscall_handler(int64_t rdi, int64_t rsi, int64_t rdx,
                        int64_t r10, int64_t r8, int64_t r9,
                        int64_t num)
{
    uint64_t retptr = retptr_tls;

    if (num == __NR_clone3) {
        struct clone_args* ca = (struct clone_args*)rdi;
        if (ca->flags & CLONE_VM) {
            ca->stack_size -= sizeof(uint64_t);
            uint8_t* child_sp = (uint8_t*)ca->stack + ca->stack_size;
            *(uint64_t*)child_sp = retptr;
        }
    }

    if (num == __NR_clone) {
        int64_t flags = rdi;
        if (flags & CLONE_VM) {
            uint8_t* child_sp = (uint8_t*)rsi;
            child_sp -= sizeof(uint64_t);
            *(uint64_t*)child_sp = retptr;
            rsi = (int64_t)child_sp;
        }
    }

    return hooked_syscall(rdi, rsi, rdx, r10, r8, r9, num);
}