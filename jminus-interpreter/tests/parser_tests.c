// tests/parser_tests.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "../lexer.h"
#include "../parser.h"

// Helper to free the AST; you should implement this in your parser module.
extern void free_ast(Stmt** stmts, int count);

static void test_let_statement() {
    const char* src = "let foo = 123;";
    int token_count;
    Token* tokens = tokenize(src, &token_count);

    int stmt_count;
    Stmt** stmts = parse(tokens, token_count, &stmt_count);
    assert(stmt_count == 1);

    Stmt* s = stmts[0];
    assert(s->type == STMT_LET);
    assert(strcmp(s->let.name.lexeme, "foo") == 0);

    Expr* init = s->let.initializer;
    assert(init->type == EXPR_LITERAL);
    assert(atoi(init->literal.value.lexeme) == 123);

    free_tokens(tokens, token_count);
    free_ast(stmts, stmt_count);
}

static void test_yap_statement() {
    const char* src = "yap(4+5*6);";
    int token_count;
    Token* tokens = tokenize(src, &token_count);

    int stmt_count;
    Stmt** stmts = parse(tokens, token_count, &stmt_count);
    assert(stmt_count == 1);

    Stmt* s = stmts[0];
    assert(s->type == STMT_YAP);

    // Inside a YapStmt we expect an expression: 4 + (5 * 6)
    Expr* e = s->yap.expression;
    assert(e->type == EXPR_BINARY);
    // operator should be '+'
    assert(strcmp(e->binary.op.lexeme, "+") == 0);

    // left literal 4
    assert(e->binary.left->type == EXPR_LITERAL);
    assert(atoi(e->binary.left->literal.value.lexeme) == 4);

    // right is another binary (*)
    Expr* right = e->binary.right;
    assert(right->type == EXPR_BINARY);
    assert(strcmp(right->binary.op.lexeme, "*") == 0);
    assert(atoi(right->binary.left->literal.value.lexeme) == 5);
    assert(atoi(right->binary.right->literal.value.lexeme) == 6);

    free_tokens(tokens, token_count);
    free_ast(stmts, stmt_count);
}

int main(void) {
    test_let_statement();
    test_yap_statement();
    printf("✅  parser_tests passed\n");
    return 0;
}
