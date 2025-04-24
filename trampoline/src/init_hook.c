#include <stdio.h>
#include <stdint.h>
#include <dlfcn.h>
#include <stdlib.h>
#include <string.h>

#include "init_hook.h"
#include "trampoline.h"
#include "syscall_wrappers.h"

syscall_hook_fn_t hooked_syscall = trigger_syscall;

void init_hook() {
    const char* hook_path = getenv("LIBZPHOOK");
    if (!hook_path) {
        z_perror("LIBZPHOOK not set");
        return;
    }

    // Load hook library in a new namespace to prevent recursion
    void* handle = dlmopen(LM_ID_NEWLM, hook_path, RTLD_NOW | RTLD_LOCAL);
    if (!handle) {
        z_perror("dlmopen failed");
        return;
    }

    // Resolve the hook init function
    void (*hook_init)(const syscall_hook_fn_t, syscall_hook_fn_t*);
    *(void**)(&hook_init) = dlsym(handle, "__hook_init");
    if (!hook_init) {
        z_perror("dlsym failed");
        return;
    }

    hook_init(trigger_syscall, &hooked_syscall);
}