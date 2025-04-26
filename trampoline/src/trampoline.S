.intel_syntax noprefix
.global trampoline
.type trampoline, @function

trampoline:
    // --- fastpath: rt_sigreturn ---
    cmp rax, 0xf                  // __NR_rt_sigreturn
    je .fastpath_rt_sigreturn

    // --- fastpath: vfork ---
    cmp rax, 0x3a                 // __NR_vfork
    jne .continue_trampoline

    add rsp, 0x8
    pop rsi
    syscall
    push rsi
    mov rsi, 0x20d
    push rsi
    ret

.fastpath_rt_sigreturn:
    add rsp, 0x10
    syscall
    ud2

.continue_trampoline:
    // Maintain the context of the caller
    push rdi
    push rsi
    push rdx
    push r10
    push r8
    push r9
    push rax

    // Original syscall return address
    mov rdi, [rsp + 0x30 + 0x10] // original rdi + 0x10
    call set_retptr

    // Move the syscall number and arguments to the appropriate registers for C calling convention
    mov rdi, [rsp + 0x30]
    mov rsi, [rsp + 0x28]
    mov rdx, [rsp + 0x20]
    mov rcx, [rsp + 0x18]
    mov r8, [rsp + 0x10]
    mov r9, [rsp + 0x8]

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