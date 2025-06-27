// vm.c
#include <stdio.h>
#include <stdlib.h>
#include "vm.h"
#include "interpreter.h"
#include "environment.h"

#define STACK_SIZE 1024

static int stack[STACK_SIZE];
static int sp = 0;  // stack pointer
static Environment* vm_env = NULL;  // VM's environment

// Output function pointer for BC_PRINT
void vm_default_output(int value) { printf("%d\n", value); }
void (*vm_output)(int value) = vm_default_output;

void run(Bytecode* bytecode) {
    Instruction* code = bytecode->instructions;
    int ip = 0;
    sp = 0;
    
    // Initialize VM environment if not already done
    if (!vm_env) {
        vm_env = new_environment(NULL);
    }

    while (1) {
        Instruction instr = code[ip++];

        switch (instr.opcode) {
            case BC_CONST: {
                // Push a literal constant
                int value = bytecode->constants[instr.operand];
                stack[sp++] = value;
                break;
            }

            case BC_ADD: {
                int b = stack[--sp];
                int a = stack[--sp];
                stack[sp++] = a + b;
                break;
            }
            case BC_SUB: {
                int b = stack[--sp];
                int a = stack[--sp];
                stack[sp++] = a - b;
                break;
            }
            case BC_MUL: {
                int b = stack[--sp];
                int a = stack[--sp];
                stack[sp++] = a * b;
                break;
            }
            case BC_DIV: {
                int b = stack[--sp];
                int a = stack[--sp];
                stack[sp++] = a / b;
                break;
            }

            case BC_EQUAL: {
                int b = stack[--sp];
                int a = stack[--sp];
                stack[sp++] = (a == b);
                break;
            }
            case BC_NOT_EQUAL: {
                int b = stack[--sp];
                int a = stack[--sp];
                stack[sp++] = (a != b);
                break;
            }
            case BC_LESS: {
                int b = stack[--sp];
                int a = stack[--sp];
                stack[sp++] = (a < b);
                break;
            }
            case BC_LESS_EQUAL: {
                int b = stack[--sp];
                int a = stack[--sp];
                stack[sp++] = (a <= b);
                break;
            }
            case BC_GREATER: {
                int b = stack[--sp];
                int a = stack[--sp];
                stack[sp++] = (a > b);
                break;
            }
            case BC_GREATER_EQUAL: {
                int b = stack[--sp];
                int a = stack[--sp];
                stack[sp++] = (a >= b);
                break;
            }

            case BC_LOAD_VAR: {
                // operand is the ASCII code of the single‐char var name
                int var_id = instr.operand;
                char name[2] = { (char)var_id, '\0' };
                int value = lookup_var(vm_env, name);
                stack[sp++] = value;
                break;
            }
            case BC_SET_VAR: {
                int var_id = instr.operand;
                int value = stack[--sp];
                char name[2] = { (char)var_id, '\0' };
                assign_var(vm_env, name, value);
                break;
            }
            case BC_DEFINE_VAR: {
                int var_id = instr.operand;
                int value = stack[--sp];
                char name[2] = { (char)var_id, '\0' };
                define_var(vm_env, name, value);
                break;
            }

            case BC_PRINT: {
                int value = stack[--sp];
                vm_output(value);
                break;
            }

            case BC_JUMP_IF_FALSE: {
                int target = instr.operand;
                int cond = stack[--sp];
                if (!cond) ip = target;
                break;
            }
            case BC_JUMP: {
                ip = instr.operand;
                break;
            }

            case BC_HALT:
                return;

            default:
                fprintf(stderr, "Unknown opcode: %d\n", instr.opcode);
                exit(1);
        }
    }
}
