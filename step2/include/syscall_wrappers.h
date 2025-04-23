#ifndef SYSCALL_WRAPPERS_H
#define SYSCALL_WRAPPERS_H

#include <unistd.h>
#include <stdint.h>
#include <sys/syscall.h>

#include "trampoline.h"

#define SYS_MMAP(addr, len, prot, flags, fd, offset) \
    trigger_syscall(SYS_mmap, (int64_t)(addr), (len), (prot), (flags), (fd), (offset))

#define SYS_MPROTECT(addr, len, prot) \
    trigger_syscall(SYS_mprotect, (int64_t)(addr), (len), (prot), 0, 0, 0)

#define SYS_OPENAT(dirfd, pathname, flags, mode) \
    trigger_syscall(SYS_openat, (dirfd), (int64_t)(pathname), (flags), (mode), 0, 0)

#define SYS_CLOSE(fd) \
    trigger_syscall(SYS_close, (fd), 0, 0, 0, 0, 0)

#define SYS_READ(fd, buf, len) \
    trigger_syscall(SYS_read, (fd), (int64_t)(buf), (len), 0, 0, 0)

#define SYS_WRITE(fd, buf, len) \
    trigger_syscall(SYS_write, (fd), (int64_t)(buf), (len), 0, 0, 0)

#endif  // SYSCALL_WRAPPERS_H