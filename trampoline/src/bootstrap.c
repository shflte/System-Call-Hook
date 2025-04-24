#include "trampoline.h"
#include "init_hook.h"

__attribute__((constructor))
void bootstrap_init() {
    init_trampoline();
    rewrite_syscall();
    init_hook();
}