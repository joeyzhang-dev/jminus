#ifndef VM_H
#define VM_H

#include "compiler.h"

void run(Bytecode* bytecode); // <- Make sure it's void, not int!

// Output function pointer for BC_PRINT
extern void (*vm_output)(int value);

#endif
