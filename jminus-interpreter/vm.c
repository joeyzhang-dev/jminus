// vm.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "vm.h"
#include "interpreter.h"

#define STACK_SIZE 1024

static int stack[STACK_SIZE];
static int sp = 0;  // stack pointer

void run(Bytecode* bytecode) {
    int ip = 0;  // instruction pointer index
    while (1) {
        Instruction instr = bytecode->instructions[ip++];

        switch (instr.opcode) {
            case BC_CONST: {
                int value = bytecode->constants[instr.operand];
                stack[sp++] = value;
                break;
            }

            case BC_LOAD_VAR: {
                // opcode operand is ASCII code of variable name
                char varname[2] = { (char)instr.operand, '\0' };
                int value = lookup_variable(varname);
                stack[sp++] = value;
                break;
            }

            case BC_STORE_VAR:
            case BC_SET_VAR: {
                // Both let-stmt and assignment opcodes
                char varname[2] = { (char)instr.operand, '\0' };
                int value = stack[--sp];
                assign_variable(varname, value);
                break;
            }

            case BC_PRINT: {
                int value = stack[--sp];
                printf("%d\n", value);
                break;
            }

            case BC_POP: {
                // discard top of stack
                if (sp > 0) sp--;
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

            case BC_JUMP_IF_FALSE: {
                int offset = instr.operand;
                int cond = stack[--sp];
                if (!cond) {
                    ip = offset;
                }
                break;
            }

            case BC_JUMP: {
                ip = instr.operand;
                break;
            }

            case BC_LOOP: {
                ip -= instr.operand;
                break;
            }

            case BC_HALT: {
                return;
            }

            default: {
                fprintf(stderr, "Unknown opcode: %d\n", instr.opcode);
                exit(1);
            }
        }
    }
}
