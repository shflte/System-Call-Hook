#ifndef TRAMPOLINE_CODEGEN_H
#define TRAMPOLINE_CODEGEN_H

#include <stdint.h>
#include <string.h>

// movabs r11, imm64
#define EMIT_MOVABS_R11_IMM64(code, addr) do { \
    *(code)++ = 0x49; *(code)++ = 0xBB; /* mov r11, imm64 */ \
    uintptr_t tmp_addr = (uintptr_t)(addr); \
    memcpy((code), &tmp_addr, sizeof(tmp_addr)); \
    (code) += sizeof(tmp_addr); \
} while (0)

// call r11
#define EMIT_CALL_R11(code) do { \
    *(code)++ = 0x41; *(code)++ = 0xFF; *(code)++ = 0xD3; \
} while (0)

// ret
#define EMIT_RET(code) do { \
    *(code)++ = 0xC3; \
} while (0)

#endif // TRAMPOLINE_CODEGEN_H