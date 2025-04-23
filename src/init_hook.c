#include <stdio.h>
#include <stdint.h>
#include <dlfcn.h>
#include <stdlib.h>

#include "trampoline.h"
#include "hook.h"

syscall_hook_fn_t hooked_syscall = trigger_syscall;

void init_hook_library() {
    const char* hook_path = getenv("LIBZPHOOK");
    if (!hook_path) {
        perror("[-] LIBZPHOOK not set");
        return;
    }

    // Load hook library in a new namespace to prevent recursion
    void* handle = dlmopen(LM_ID_NEWLM, hook_path, RTLD_NOW);
    if (!handle) {
        perror("[-] dlmopen");
        return;
    }

    // Resolve the hook init function
    void (*hook_init)(const syscall_hook_fn_t, syscall_hook_fn_t*);
    *(void**)(&hook_init) = dlsym(handle, "__hook_init");
    if (!hook_init) {
        perror("[-] dlsym");
        return;
    }

    hook_init(trigger_syscall, &hooked_syscall);
}