#include <string.h>
#include <dlfcn.h>
#include <link.h>

#include "syscall_wrappers.h"

void z_perror(const char* msg) {
    const char *error_msg = "[-] Error: ";
    SYS_WRITE(2, error_msg, strlen(error_msg));
    SYS_WRITE(2, msg, strlen(msg));
    SYS_WRITE(2, "\n", 1);
}