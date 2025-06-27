// compiler.h
#ifndef COMPILER_H
#define COMPILER_H

#include "parser.h"

// Bytecode instruction set
typedef enum {
    BC_CONST,
    BC_ADD,
    BC_SUB,
    BC_MUL,
    BC_DIV,
    BC_PRINT,
    BC_LOAD_VAR,
    BC_SET_VAR,
    BC_DEFINE_VAR,  // New instruction for variable definition
    BC_STORE_VAR,      
    BC_EQUAL,
    BC_NOT_EQUAL,
    BC_LESS,
    BC_LESS_EQUAL,
    BC_LOAD_CONST,
    BC_GREATER,
    BC_GREATER_EQUAL,
    BC_JUMP_IF_FALSE,
    BC_JUMP,
    BC_LOOP,
    BC_POP,            
    BC_HALT
} OpCode;


// Represents a bytecode instruction
typedef struct {
    OpCode opcode;
    int operand; // e.g., index into constants table or jump offset
} Instruction;

// Represents compiled bytecode
typedef struct {
    Instruction* instructions;
    int count;
    int capacity;
    int* constants;  // For literal numbers
    int const_count;
    int const_capacity;
} Bytecode;

Bytecode* compile(Stmt** stmts, int stmt_count);
void free_bytecode(Bytecode* bytecode);

#endif
