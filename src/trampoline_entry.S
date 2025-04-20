.intel_syntax noprefix
.global trampoline_entry
.type trampoline_entry, @function

trampoline_entry:
    // Maintain the context of the caller
    push rax
    push rdi
    push rsi
    push rdx
    push r10
    push r8
    push r9

    // Move the syscall number and arguments to the appropriate registers for C calling convention
    mov rdi, [rsp + 0x30]
    mov rsi, [rsp + 0x28]
    mov rdx, [rsp + 0x20]
    mov rcx, [rsp + 0x18]
    mov r8,  [rsp + 0x10]
    mov r9,  [rsp + 0x8]

    call syscall_hook_handler

    // Restore the context of the caller
    pop r9
    pop r8
    pop r10
    pop rdx
    pop rsi
    pop rdi
    pop rax

    ret
