#include <stdio.h>
#include <sys/mman.h>
#include <unistd.h>

#include "trampoline_codegen.h"
#include "trampoline.h"

#define PAGE_SIZE 4096

void* get_trampoline_address() {
    return (void*)TRAMPOLINE_ADDR;
}

__attribute__((constructor))
void init_trampoline() {
    void* addr = mmap((void*)0x0, PAGE_SIZE,
                      PROT_READ | PROT_WRITE | PROT_EXEC,
                      MAP_ANONYMOUS | MAP_PRIVATE | MAP_FIXED, -1, 0);
    if (addr == MAP_FAILED) {
        perror("mmap @0x0 failed");
        return;
    }

    memset(addr, 0x90, 512);
    uint8_t* code = (uint8_t*)addr + 512;

    EMIT_MOVABS_RAX_IMM64(code, trampoline_entry);
    EMIT_CALL_RAX(code);
    EMIT_RET(code);
}
