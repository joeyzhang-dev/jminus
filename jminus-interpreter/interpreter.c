#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "interpreter.h"

// Simple variable storage
typedef struct {
    const char* name;
    int value;
} Variable;

#define MAX_VARS 128
static Variable vars[MAX_VARS];
static int var_count = 0;

int lookup_variable(const char* name) {
    for (int i = 0; i < var_count; i++) {
        if (strcmp(vars[i].name, name) == 0) {
            return vars[i].value;
        }
    }
    fprintf(stderr, "Undefined variable: %s\n", name);
    exit(1);
}

void assign_variable(const char* name, int value) {
    for (int i = 0; i < var_count; i++) {
        if (strcmp(vars[i].name, name) == 0) {
            vars[i].value = value;
            return;
        }
    }
    vars[var_count].name = strdup(name);
    vars[var_count].value = value;
    var_count++;
}

// ------------------
// Expression evaluation
// ------------------
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
void exec_stmt(Stmt* stmt) {
    if (!stmt) return;

    switch (stmt->type) {
        case STMT_LET: {
            const char* name = stmt->let.name.lexeme;
            int value = eval_expr(stmt->let.initializer);
            assign_variable(name, value);
            break;
        }

        case STMT_YAP: {
            int value = eval_expr(stmt->yap.expression);
            printf("%d\n", value);
            break;
        }

        case STMT_EXPR: {
            eval_expr(stmt->expr.expression);
            break;
        }

        default:
            printf("Unhandled statement type\n");
    }
}

void interpret(Stmt** stmts, int count) {
    for (int i = 0; i < count; i++) {
        exec_stmt(stmts[i]);
    }
}
