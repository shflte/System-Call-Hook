.intel_syntax noprefix
.global trigger_syscall
.type trigger_syscall, @function

trigger_syscall:
    mov rax, [rsp + 8]
    mov r10, rcx

    syscall
    ret