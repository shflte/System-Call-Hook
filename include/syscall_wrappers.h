#ifndef SYSCALL_WRAPPERS_H
#define SYSCALL_WRAPPERS_H

#include <unistd.h>
#include <stdint.h>
#include <sys/syscall.h>

#include "trampoline.h"

#define SYS_MMAP(addr, len, prot, flags, fd, offset) \
    trigger_syscall((int64_t)(addr), (len), (prot), (flags), (fd), (offset), SYS_mmap)

#define SYS_MPROTECT(addr, len, prot) \
    trigger_syscall((int64_t)(addr), (len), (prot), 0, 0, 0, SYS_mprotect)

#define SYS_OPENAT(dirfd, pathname, flags, mode) \
    trigger_syscall((dirfd), (int64_t)(pathname), (flags), (mode), 0, 0, SYS_openat)

#define SYS_CLOSE(fd) \
    trigger_syscall((fd), 0, 0, 0, 0, 0, SYS_close)

#define SYS_READ(fd, buf, len) \
    trigger_syscall((fd), (int64_t)(buf), (len), 0, 0, 0, SYS_read)

#define SYS_WRITE(fd, buf, len) \
    trigger_syscall((fd), (int64_t)(buf), (len), 0, 0, 0, SYS_write)

void z_perror(const char* msg);

#endif  // SYSCALL_WRAPPERS_H