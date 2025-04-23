.intel_syntax noprefix
.global trigger_syscall
.type trigger_syscall, @function

trigger_syscall:
    mov rax, rdi
    mov rdi, rsi
    mov rsi, rdx
    mov rdx, rcx
    mov r10, r8
    mov r8,  r9
    mov r9,  [rsp + 8]

    syscall
    ret
