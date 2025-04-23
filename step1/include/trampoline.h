#ifndef TRAMPOLINE_H
#define TRAMPOLINE_H

#include <stdbool.h>

#define TRAMPOLINE_ADDR 0x200

void* trampoline_entry();

#endif // TRAMPOLINE_H