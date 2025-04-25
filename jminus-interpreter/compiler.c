#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "compiler.h"
#include "vm.h"

static Bytecode* bytecode;

static void emit(OpCode opcode, int operand) {
    if (bytecode->count >= bytecode->capacity) {
        bytecode->capacity *= 2;
        bytecode->instructions = realloc(bytecode->instructions, sizeof(Instruction) * bytecode->capacity);
    }
    bytecode->instructions[bytecode->count++] = (Instruction){ opcode, operand };
}

static int add_constant(int value) {
    if (bytecode->const_count >= bytecode->const_capacity) {
        bytecode->const_capacity *= 2;
        bytecode->constants = realloc(bytecode->constants, sizeof(int) * bytecode->const_capacity);
    }
    bytecode->constants[bytecode->const_count] = value;
    return bytecode->const_count++;
}

static void compile_expr(Expr* expr) {
    switch (expr->type) {
        case EXPR_LITERAL: {
            int value = atoi(expr->literal.value.lexeme);
            int idx = add_constant(value);
            emit(BC_CONST, idx);
            break;
        }
        case EXPR_VARIABLE: {
            int var_id = expr->variable.name.lexeme[0];
            emit(BC_LOAD_VAR, var_id);
            break;
        }
        case EXPR_BINARY: {
            const char* op = expr->binary.op.lexeme;

            if (strcmp(op, "=") == 0) {
                if (expr->binary.left->type != EXPR_VARIABLE) {
                    fprintf(stderr, "Invalid assignment target\n");
                    exit(1);
                }
                compile_expr(expr->binary.right);
                int var_id = expr->binary.left->variable.name.lexeme[0];
                emit(BC_SET_VAR, var_id);
                return;
            }

            compile_expr(expr->binary.left);
            compile_expr(expr->binary.right);

            if (strcmp(op, "+") == 0) emit(BC_ADD, 0);
            else if (strcmp(op, "-") == 0) emit(BC_SUB, 0);
            else if (strcmp(op, "*") == 0) emit(BC_MUL, 0);
            else if (strcmp(op, "/") == 0) emit(BC_DIV, 0);
            else if (strcmp(op, "==") == 0) emit(BC_EQUAL, 0);
            else if (strcmp(op, "!=") == 0) emit(BC_NOT_EQUAL, 0);
            else if (strcmp(op, "<") == 0) emit(BC_LESS, 0);
            else if (strcmp(op, "<=") == 0) emit(BC_LESS_EQUAL, 0);
            else if (strcmp(op, ">") == 0) emit(BC_GREATER, 0);
            else if (strcmp(op, ">=") == 0) emit(BC_GREATER_EQUAL, 0);
            else {
                fprintf(stderr, "Unknown binary operator: %s\n", op);
                exit(1);
            }
            break;
        }
        default:
            fprintf(stderr, "Unknown expression type\n");
            exit(1);
    }
}

static void compile_stmt(Stmt* stmt) {
    switch (stmt->type) {
        case STMT_LET: {
            compile_expr(stmt->let.initializer);
            int var_id = stmt->let.name.lexeme[0];
            emit(BC_SET_VAR, var_id);
            break;
        }
        case STMT_YAP: {
            compile_expr(stmt->yap.expression);
            emit(BC_PRINT, 0);
            break;
        }
        case STMT_EXPR: {
            compile_expr(stmt->expr.expression);
            break;
        }
        case STMT_IF: {
            compile_expr(stmt->if_stmt.condition);
            int jump_if_false = bytecode->count;
            emit(BC_JUMP_IF_FALSE, 0); // Placeholder

            compile_stmt(stmt->if_stmt.then_branch);

            if (stmt->if_stmt.else_branch) {
                int jump_end = bytecode->count;
                emit(BC_JUMP, 0); // Placeholder
                bytecode->instructions[jump_if_false].operand = bytecode->count;
                compile_stmt(stmt->if_stmt.else_branch);
                bytecode->instructions[jump_end].operand = bytecode->count;
            } else {
                bytecode->instructions[jump_if_false].operand = bytecode->count;
            }
            break;
        }
        case STMT_WHILE: {
            int loop_start = bytecode->count;
            compile_expr(stmt->while_stmt.condition);
            int jump_out = bytecode->count;
            emit(BC_JUMP_IF_FALSE, 0); // Placeholder

            compile_stmt(stmt->while_stmt.body);

            emit(BC_JUMP, loop_start);
            bytecode->instructions[jump_out].operand = bytecode->count;
            break;
        }
        case STMT_BLOCK: {
            for (int i = 0; i < stmt->block.count; i++) {
                compile_stmt(stmt->block.statements[i]);
            }
            break;
        }
        default:
            fprintf(stderr, "Unhandled statement type\n");
            exit(1);
    }
}

Bytecode* compile(Stmt** stmts, int stmt_count) {
    bytecode = malloc(sizeof(Bytecode));
    bytecode->instructions = malloc(sizeof(Instruction) * 128);
    bytecode->capacity = 128;
    bytecode->count = 0;

    bytecode->constants = malloc(sizeof(int) * 128);
    bytecode->const_capacity = 128;
    bytecode->const_count = 0;

    for (int i = 0; i < stmt_count; i++) {
        compile_stmt(stmts[i]);
    }

    emit(BC_HALT, 0);
    return bytecode;
}

void free_bytecode(Bytecode* bc) {
    free(bc->instructions);
    free(bc->constants);
    free(bc);
}
