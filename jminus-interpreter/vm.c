// vm.c
#include <stdio.h>
#include <stdlib.h>
#include "vm.h"
#include "compiler.h"
#include "interpreter.h" // For lookup_variable and assign_variable

void run(Bytecode* bytecode) {
    int stack[256];
    int sp = 0;

    int ip = 0; // Instruction Pointer
    while (1) {
        Instruction instr = bytecode->instructions[ip++];
        switch (instr.opcode) {
            case BC_CONST:
                stack[sp++] = bytecode->constants[instr.operand];
                break;

            case BC_LOAD_VAR: {
                char varname[2] = { instr.operand, '\0' };
                stack[sp++] = lookup_variable(varname);
                break;
            }

            case BC_SET_VAR: {
                char varname[2] = { instr.operand, '\0' };
                assign_variable(varname, stack[--sp]);
                break;
            }

            case BC_STORE_VAR: {
                char varname[2] = { instr.operand, '\0' };
                assign_variable(varname, stack[--sp]);
                break;
            }

            case BC_ADD:
                stack[sp - 2] = stack[sp - 2] + stack[sp - 1];
                sp--;
                break;

            case BC_SUB:
                stack[sp - 2] = stack[sp - 2] - stack[sp - 1];
                sp--;
                break;

            case BC_MUL:
                stack[sp - 2] = stack[sp - 2] * stack[sp - 1];
                sp--;
                break;

            case BC_DIV:
                stack[sp - 2] = stack[sp - 2] / stack[sp - 1];
                sp--;
                break;

            case BC_EQUAL:
                stack[sp - 2] = (stack[sp - 2] == stack[sp - 1]);
                sp--;
                break;

            case BC_NOT_EQUAL:
                stack[sp - 2] = (stack[sp - 2] != stack[sp - 1]);
                sp--;
                break;

            case BC_LESS:
                stack[sp - 2] = (stack[sp - 2] < stack[sp - 1]);
                sp--;
                break;

            case BC_LESS_EQUAL:
                stack[sp - 2] = (stack[sp - 2] <= stack[sp - 1]);
                sp--;
                break;

            case BC_GREATER:
                stack[sp - 2] = (stack[sp - 2] > stack[sp - 1]);
                sp--;
                break;

            case BC_GREATER_EQUAL:
                stack[sp - 2] = (stack[sp - 2] >= stack[sp - 1]);
                sp--;
                break;

            case BC_PRINT:
                printf("%d\n", stack[--sp]);
                break;

            case BC_POP:
                sp--;
                break;

            case BC_JUMP_IF_FALSE: {
                int condition = stack[--sp];
                if (!condition) {
                    ip = instr.operand;
                }
                break;
            }

            case BC_JUMP:
                ip = instr.operand;
                break;

            case BC_HALT:
                return;

            default:
                fprintf(stderr, "Unknown opcode: %d\n", instr.opcode);
                exit(1);
        }
    }
}
