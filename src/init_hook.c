#include <dlfcn.h>
#include <link.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "trampoline.h"
#include "syscall_wrappers.h"

syscall_hook_fn_t hooked_syscall = trigger_syscall;

void init_hook_library() {
    const char* hook_path = getenv("LIBZPHOOK");
    if (!hook_path) {
        // No hook library specified
        return;
    }

    // Load hook library in a new namespace to prevent recursion
    void* handle = dlmopen(LM_ID_NEWLM, hook_path, RTLD_NOW);
    if (!handle) {
        // Failed to load the hook library
        return;
    }

    // Resolve the hook init function
    void (*hook_init)(const syscall_hook_fn_t, syscall_hook_fn_t*);
    *(void**)(&hook_init) = dlsym(handle, "__hook_init");
    if (!hook_init) {
        // Failed to resolve the hook init function
        return;
    }

    hook_init(trigger_syscall, &hooked_syscall);
}

// Expose hooked_syscall
syscall_hook_fn_t get_hooked_syscall() {
    return hooked_syscall;
}