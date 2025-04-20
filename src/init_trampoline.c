#include <stdio.h>
#include <stdint.h>
#include <sys/mman.h>

#include "trampoline_codegen.h"
#include "trampoline.h"
#include "syscall_wrappers.h"

#define PAGE_SIZE 4096

void* get_trampoline_address() {
    return (void*)TRAMPOLINE_ADDR;
}

__attribute__((constructor))
void init_trampoline() {
    void* addr = (void*) SYS_MMAP((void*)0x0, PAGE_SIZE,
                                  PROT_READ | PROT_WRITE | PROT_EXEC,
                                  MAP_ANONYMOUS | MAP_PRIVATE | MAP_FIXED,
                                  -1, 0);
    if ((int64_t)addr < 0) {
        SYS_WRITE(2, "mmap @0x0 failed\n", 18);
        return;
    }

    memset(addr, 0x90, 512);

    uint8_t* code = (uint8_t*)addr + 512;
    EMIT_MOVABS_R11_IMM64(code, trampoline_entry);
    EMIT_CALL_R11(code);
    EMIT_RET(code);
}