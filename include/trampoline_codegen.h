#ifndef TRAMPOLINE_CODEGEN_H
#define TRAMPOLINE_CODEGEN_H

#include <stdint.h>
#include <string.h>

// movabs rax, imm64
#define EMIT_MOVABS_RAX_IMM64(code, addr) do { \
    *(code)++ = 0x48; *(code)++ = 0xB8; \
    uintptr_t tmp_addr = (uintptr_t)(addr); \
    memcpy((code), &tmp_addr, sizeof(tmp_addr)); \
    (code) += sizeof(tmp_addr); \
} while (0)

// call rax
#define EMIT_CALL_RAX(code) do { \
    *(code)++ = 0xFF; *(code)++ = 0xD0; \
} while (0)

// ret
#define EMIT_RET(code) do { \
    *(code)++ = 0xC3; \
} while (0)

#endif // TRAMPOLINE_CODEGEN_H