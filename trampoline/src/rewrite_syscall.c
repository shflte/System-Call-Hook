#include <string.h>
#include <sys/mman.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <capstone/capstone.h>

#include "trampoline.h"
#include "syscall_wrappers.h"

#define PROC_MAP_BUF_SIZE 0x10000

void rewrite_syscall() {
    int fd = SYS_OPENAT(AT_FDCWD, "/proc/self/maps", O_RDONLY, 0);
    if (fd < 0) return;

    char buf[PROC_MAP_BUF_SIZE];
    ssize_t total = 0, n;
    while (total < (sizeof(buf)-1) &&
           (n = SYS_READ(fd, buf + total, sizeof(buf)-1 - total)) > 0) {
        total += n;
    }

    SYS_CLOSE(fd);
    if (total <= 0) return;
    buf[total] = '\0';

    void* trampoline_start = (void*)trampoline;
    void* trampoline_end = trampoline_start + 0x50; // Length of trampoline()

    void* trigger_start = (void*)trigger_syscall;
    void* trigger_end = trigger_start + 0x1A; // Length of trigger_syscall()

    char* line = strtok(buf, "\n");
    while (line) {
        unsigned long start, end;
        char perm[5];
        char offset_str[20];
        char dev[20];
        char inode[20];
        char pathname[100];
        if (sscanf(line, "%lx-%lx %4s %s %s %s %s",
                   &start, &end, perm, offset_str, dev, inode, pathname) < 6) {
            line = strtok(NULL, "\n");
            continue;
        }

        if ((strstr(perm, "x") == NULL) ||
            (strcmp(pathname, "[vdso]") == 0) || (strcmp(pathname, "[vsyscall]") == 0) ||
            strstr(line, "libcapstone.so")) {
            line = strtok(NULL, "\n");
            continue;
        }

        size_t region_size = end - start;

        if (SYS_MPROTECT((void*)start, region_size, PROT_READ | PROT_WRITE | PROT_EXEC) != 0) {
            line = strtok(NULL, "\n");
            continue;
        }

        csh handle;
        if (cs_open(CS_ARCH_X86, CS_MODE_64, &handle) != CS_ERR_OK) {
            line = strtok(NULL, "\n");
            continue;
        }

        cs_option(handle, CS_OPT_DETAIL, CS_OPT_ON);
        uint8_t* code_ptr = (uint8_t*)start;
        size_t offset = 0;

        while (offset < region_size) {
            cs_insn* insn;
            size_t count = cs_disasm(handle, code_ptr + offset, region_size - offset,
                                     (uint64_t)(code_ptr + offset), 1, &insn);

            if (count == 0) {
                offset += 1;
                continue;
            }

            if (insn[0].id == X86_INS_SYSCALL) {
                uint8_t* addr = (uint8_t*)insn[0].address;

                if (((void*)addr < trigger_start || (void*)addr >= trigger_end) &&
                    ((void*)addr < trampoline_start || (void*)addr >= trampoline_end)) {
                    addr[0] = 0xFF;
                    addr[1] = 0xD0;
                    for (int j = 2; j < insn[0].size; j++) {
                        addr[j] = 0x90;
                    }
                }
            }

            offset += insn[0].size;
            cs_free(insn, count);
        }

        cs_close(&handle);
        line = strtok(NULL, "\n");
    }
}