.intel_syntax noprefix
.global trampoline_entry
.type trampoline_entry, @function

trampoline_entry:
    // Maintain the context of the caller
    push rdi
    push rsi
    push rdx
    push r10
    push r8
    push r9
    push rax

    // Move the syscall number and arguments to the appropriate registers for C calling convention
    mov rcx, r10

    call syscall_handler

    // Restore the context of the caller
    add rsp, 8 // Skip RAX
    pop r9
    pop r8
    pop r10
    pop rdx
    pop rsi
    pop rdi

    ret