#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <capstone/capstone.h>

#include "trampoline.h"

#define MAX_LINE 512

__attribute__((constructor))
void rewrite_syscall() {
    FILE* maps = fopen("/proc/self/maps", "r");
    if (!maps) {
        perror("fopen /proc/self/maps");
        return;
    }

    char line[MAX_LINE];
    while (fgets(line, sizeof(line), maps)) {
        unsigned long start, end;
        char perm[5];
        if (sscanf(line, "%lx-%lx %4s", &start, &end, perm) != 3)
            continue;

        if (strstr(perm, "x") == NULL || strstr(line, "[vdso]") || strstr(line, "[vsyscall]"))
            continue;

        size_t region_size = end - start;
        uint8_t* buffer = (uint8_t*)start;

        if (mprotect((void*)start, region_size, PROT_READ | PROT_WRITE | PROT_EXEC) != 0) {
            perror("mprotect");
            continue;
        }

        csh handle;
        cs_insn* insn;
        size_t count;

        if (cs_open(CS_ARCH_X86, CS_MODE_64, &handle) != CS_ERR_OK) {
            fprintf(stderr, "capstone init failed\n");
            continue;
        }

        cs_option(handle, CS_OPT_DETAIL, CS_OPT_ON);

        for (size_t i = 0; i < count; i++) {
            if (insn[i].id == X86_INS_SYSCALL) {
                uint8_t* addr = (uint8_t*)insn[i].address;

                // syscall -> call rax = FF D0
                addr[0] = 0xFF;
                addr[1] = 0xD0;
            }
        }

        cs_free(insn, count);
        cs_close(&handle);
    }

    fclose(maps);
}