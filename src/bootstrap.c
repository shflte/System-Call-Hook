#include "trampoline.h"
#include "hook.h"
#include <stdlib.h> // todo: remove

__attribute__((constructor))
void bootstrap_init() {
    if (getenv("ZDEBUG")) asm("int3"); // todo: remove

    init_trampoline();
    init_hook_library();
    rewrite_syscall();
}