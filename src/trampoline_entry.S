.intel_syntax noprefix
.global trampoline_entry
.type trampoline_entry, @function

trampoline_entry:
    sub rsp, 8
    mov [rsp], r9

    mov r9, r8
    mov r8,  r10
    mov rcx, rdx
    mov rdx, rsi
    mov rsi, rdi
    mov rdi, rax

    call syscall_hook_handler
    add rsp, 8

    ret
