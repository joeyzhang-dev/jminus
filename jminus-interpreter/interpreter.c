/**
 * @file interpreter.c
 * @brief Direct AST interpreter for the jminus language
 * @author Joey Zhang
 * @version 1.0.0
 *
 * This file implements a recursive AST interpreter for jminus.
 * It walks the AST, evaluates expressions, and executes statements
 * directly, using a global environment for variable storage.
 *
 * Execution Model:
 * - Each statement is executed in order
 * - Expressions are evaluated recursively
 * - Variables are managed via the environment module
 * - Control flow (if/while) is handled with C control structures
 *
 * Error Handling:
 * - Undefined variable access triggers error and exit
 * - Invalid assignments are detected
 * - All errors are reported with descriptive messages
 *
 * Debugging:
 * - Prints variable assignments, yap output, and control flow
 * - Useful for tracing program execution in REPL mode
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "interpreter.h"
#include "environment.h"

// Global environment for the interpreter (single scope for now)
static Environment* global_env = NULL;

int lookup_variable(const char* name) {
    if (!global_env) {
        global_env = new_environment(NULL);
    }
    return lookup_var(global_env, name);
}

void assign_variable(const char* name, int value) {
    if (!global_env) {
        global_env = new_environment(NULL);
    }
    assign_var(global_env, name, value);
}

void define_variable(const char* name, int value) {
    if (!global_env) {
        global_env = new_environment(NULL);
    }
    define_var(global_env, name, value);
}

// ------------------
// Expression evaluation
// ------------------

/**
 * @brief Recursively evaluates an expression AST node
 * @param expr Pointer to the expression node
 * @return The computed integer value
 *
 * Handles literals, variables, and binary operations.
 * Exits on unknown expression types or errors.
 */
int eval_expr(Expr* expr) {
    switch (expr->type) {
        case EXPR_LITERAL:
            return atoi(expr->literal.value.lexeme);

        case EXPR_VARIABLE:
            return lookup_variable(expr->variable.name.lexeme);

        case EXPR_BINARY: {
            int left = eval_expr(expr->binary.left);
            int right = eval_expr(expr->binary.right);
            const char* op = expr->binary.op.lexeme;

            if (strcmp(op, "+") == 0) return left + right;
            if (strcmp(op, "-") == 0) return left - right;
            if (strcmp(op, "*") == 0) return left * right;
            if (strcmp(op, "/") == 0) return left / right;
            if (strcmp(op, "==") == 0) return left == right;
            if (strcmp(op, "!=") == 0) return left != right;
            if (strcmp(op, "<")  == 0) return left < right;
            if (strcmp(op, "<=") == 0) return left <= right;
            if (strcmp(op, ">")  == 0) return left > right;
            if (strcmp(op, ">=") == 0) return left >= right;

            fprintf(stderr, "Unknown binary operator: %s\n", op);
            exit(1);
        }

        default:
            fprintf(stderr, "Unknown expression type\n");
            exit(1);
    }
}

// ------------------
// Statement execution
// ------------------

/**
 * @brief Executes a single statement AST node
 * @param stmt Pointer to the statement node
 *
 * Handles all statement types: let, yap, expr, if, while, block.
 * Recursively executes nested statements and blocks.
 */
void exec_stmt(Stmt* stmt) {
    if (!stmt) return;

    switch (stmt->type) {
        case STMT_LET: {
            const char* name = stmt->let.name.lexeme;
            int value = eval_expr(stmt->let.initializer);
            define_variable(name, value);
            printf("Defined variable %s = %d\n", name, value);  // Debugging output
            break;
        }

        case STMT_YAP: {
            int value = eval_expr(stmt->yap.expression);
            printf("Yap output: %d\n", value);  // Debugging output
            break;
        }

        case STMT_EXPR: {
            Expr* expr = stmt->expr.expression;

            // Handle assignment: x = ...
            if (expr->type == EXPR_BINARY &&
                strcmp(expr->binary.op.lexeme, "=") == 0 &&
                expr->binary.left->type == EXPR_VARIABLE) {

                const char* name = expr->binary.left->variable.name.lexeme;
                int value = eval_expr(expr->binary.right);
                assign_variable(name, value);
                printf("Re-assigned variable %s = %d\n", name, value);  // Debugging output
            } else {
                eval_expr(expr);  // Regular expression
            }
            break;
        }

        case STMT_IF: {
            int condition = eval_expr(stmt->if_stmt.condition);
            printf("If condition: %d\n", condition);  // Debugging output
            if (condition) {
                exec_stmt(stmt->if_stmt.then_branch);
            } else if (stmt->if_stmt.else_branch) {
                exec_stmt(stmt->if_stmt.else_branch);
            }
            break;
        }

        case STMT_WHILE: {
            while (eval_expr(stmt->while_stmt.condition)) {
                printf("While condition true\n");  // Debugging output
                exec_stmt(stmt->while_stmt.body);
            }
            break;
        }

        case STMT_BLOCK: {
            for (int i = 0; i < stmt->block.count; i++) {
                exec_stmt(stmt->block.statements[i]);
            }
            break;
        }

        default:
            printf("Unhandled statement type\n");
    }
}

/**
 * @brief Executes an array of AST statements
 * @param stmts Array of statement nodes
 * @param count Number of statements
 *
 * Calls exec_stmt on each statement in order.
 */
void interpret(Stmt** stmts, int count) {
    for (int i = 0; i < count; i++) {
        exec_stmt(stmts[i]);
    }
} 