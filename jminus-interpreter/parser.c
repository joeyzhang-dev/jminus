#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "parser.h"

// ----------------------------
// Internal State
// ----------------------------
static Token* tokens;
static int current;
static int total;

// ----------------------------
// Helpers
// ----------------------------
static Token peek() {
    return tokens[current];
}

static Token previous() {
    return tokens[current - 1];
}

static int is_at_end() {
  return tokens[current].type == TOKEN_EOF;
}

static Token advance() {
    if (!is_at_end()) current++;
    return previous();
}

static int check(TokenType type) {
    return !is_at_end() && tokens[current].type == type;
}

static int match(TokenType type) {
    if (check(type)) {
        advance();
        return 1;
    }
    return 0;
}



void* allocate(size_t size) {
    void* node = malloc(size);
    if (!node) {
        fprintf(stderr, "Memory allocation failed\n");
        exit(1);
    }
    memset(node, 0, size);
    return node;
}

// ----------------------------
// Forward declarations
// ----------------------------
Expr* parse_expression();
Expr* parse_equality();
Expr* parse_comparison();
Expr* parse_term();
Expr* parse_factor();
Expr* parse_primary();
Stmt* parse_statement();
Stmt* parse_block_statement();
Stmt* parse_if_statement();
Stmt* parse_while_statement();
Stmt* parse_let_statement();
Stmt* parse_yap_statement();

// ----------------------------
// Expression Parsing
// ----------------------------
Expr* parse_primary() {
    if (match(TOKEN_INT)) {
        LiteralExpr lit = { previous() };
        Expr* expr = allocate(sizeof(Expr));
        expr->type = EXPR_LITERAL;
        expr->literal = lit;
        return expr;
    }

    if (match(TOKEN_IDENTIFIER)) {
        VariableExpr var = { previous() };
        Expr* expr = allocate(sizeof(Expr));
        expr->type = EXPR_VARIABLE;
        expr->variable = var;
        return expr;
    }

    if (match(TOKEN_LPAREN)) {
        Expr* expr = parse_expression();
        if (!match(TOKEN_RPAREN)) {
            fprintf(stderr, "Expected ')' after expression\n");
            exit(1);
        }
        return expr;
    }

    fprintf(stderr, "Unexpected token: %s\n", peek().lexeme);
    exit(1);
}

Expr* parse_binary(Expr* (*next_fn)(), TokenType ops[], int op_count) {
    Expr* left = next_fn();

    while (!is_at_end()) {
        TokenType type = peek().type;
        int matched = 0;
        for (int i = 0; i < op_count; i++) {
            if (type == ops[i]) {
                matched = 1;
                break;
            }
        }
        if (!matched) break;

        Token op = advance();
        Expr* right = next_fn();

        BinaryExpr bin = { left, op, right };
        Expr* expr = allocate(sizeof(Expr));
        expr->type = EXPR_BINARY;
        expr->binary = bin;
        left = expr;
    }

    return left;
}

Expr* parse_factor() {
    TokenType ops[] = { TOKEN_STAR, TOKEN_SLASH };
    return parse_binary(parse_primary, ops, 2);
}

Expr* parse_term() {
    TokenType ops[] = { TOKEN_PLUS, TOKEN_MINUS };
    return parse_binary(parse_factor, ops, 2);
}

Expr* parse_comparison() {
    TokenType ops[] = { TOKEN_LESS, TOKEN_LESS_EQUAL, TOKEN_GREATER, TOKEN_GREATER_EQUAL };
    return parse_binary(parse_term, ops, 4);
}

Expr* parse_equality() {
    TokenType ops[] = { TOKEN_EQUAL, TOKEN_BANG_EQUAL };
    return parse_binary(parse_comparison, ops, 2);
}

// changed to allow assignment
Expr* parse_expression() {
  Expr* expr = parse_equality();

  // Handle assignment like `x = ...`
  if (match(TOKEN_ASSIGN)) {
      Token equals = previous();
      Expr* value = parse_expression();

      if (expr->type != EXPR_VARIABLE) {
          fprintf(stderr, "Invalid assignment target.\n");
          exit(1);
      }

      // Build a binary '=' expression
      BinaryExpr bin = { expr, equals, value };
      Expr* assignment = allocate(sizeof(Expr));
      assignment->type = EXPR_BINARY;
      assignment->binary = bin;
      return assignment;
  }

  return expr;
}


// ----------------------------
// Statement Parsing
// ----------------------------
Stmt* parse_let_statement() {
    if (!match(TOKEN_IDENTIFIER)) {
        fprintf(stderr, "Expected variable name after 'let'\n");
        exit(1);
    }
    Token name = previous();

    if (!match(TOKEN_ASSIGN)) {
        fprintf(stderr, "Expected '=' after variable name\n");
        exit(1);
    }

    Expr* initializer = parse_expression();

    if (!match(TOKEN_SEMICOLON)) {
        fprintf(stderr, "Expected ';' after let initializer\n");
        exit(1);
    }

    LetStmt let = { name, initializer };
    Stmt* stmt = allocate(sizeof(Stmt));
    stmt->type = STMT_LET;
    stmt->let = let;
    return stmt;
}

Stmt* parse_yap_statement() {
    if (!match(TOKEN_LPAREN)) {
        fprintf(stderr, "Expected '(' after 'yap'\n");
        exit(1);
    }

    Expr* expr = parse_expression();

    if (!match(TOKEN_RPAREN)) {
        fprintf(stderr, "Expected ')' after yap expression\n");
        exit(1);
    }

    if (!match(TOKEN_SEMICOLON)) {
        fprintf(stderr, "Expected ';' after yap statement\n");
        exit(1);
    }

    YapStmt yap = { expr };
    Stmt* stmt = allocate(sizeof(Stmt));
    stmt->type = STMT_YAP;
    stmt->yap = yap;
    return stmt;
}

Stmt* parse_if_statement() {
    if (!match(TOKEN_LPAREN)) {
        fprintf(stderr, "Expected '(' after 'if'\n");
        exit(1);
    }

    Expr* condition = parse_expression();

    if (!match(TOKEN_RPAREN)) {
        fprintf(stderr, "Expected ')' after condition\n");
        exit(1);
    }

    Stmt* then_branch = parse_statement();
    Stmt* else_branch = NULL;

    if (match(TOKEN_ELSE)) {
        else_branch = parse_statement();
    }

    IfStmt if_stmt = { condition, then_branch, else_branch };
    Stmt* stmt = allocate(sizeof(Stmt));
    stmt->type = STMT_IF;
    stmt->if_stmt = if_stmt;
    return stmt;
}

Stmt* parse_while_statement() {
    if (!match(TOKEN_LPAREN)) {
        fprintf(stderr, "Expected '(' after 'while'\n");
        exit(1);
    }

    Expr* condition = parse_expression();

    if (!match(TOKEN_RPAREN)) {
        fprintf(stderr, "Expected ')' after while condition\n");
        exit(1);
    }

    Stmt* body = parse_statement();

    WhileStmt while_stmt = { condition, body };
    Stmt* stmt = allocate(sizeof(Stmt));
    stmt->type = STMT_WHILE;
    stmt->while_stmt = while_stmt;
    return stmt;
}

Stmt* parse_block_statement() {
    Stmt** statements = allocate(sizeof(Stmt*) * 128);
    int count = 0;

    while (!check(TOKEN_RBRACE) && !is_at_end()) {
        statements[count++] = parse_statement();
    }

    if (!match(TOKEN_RBRACE)) {
        fprintf(stderr, "Expected '}' after block\n");
        exit(1);
    }

    BlockStmt block = { statements, count };
    Stmt* stmt = allocate(sizeof(Stmt));
    stmt->type = STMT_BLOCK;
    stmt->block = block;
    return stmt;
}

Stmt* parse_statement() {
    if (match(TOKEN_LET)) return parse_let_statement();
    if (match(TOKEN_YAP)) return parse_yap_statement();
    if (match(TOKEN_IF)) return parse_if_statement();
    if (match(TOKEN_WHILE)) return parse_while_statement();
    if (match(TOKEN_LBRACE)) return parse_block_statement();

    // Otherwise just an expression statement
    Expr* expr = parse_expression();
    if (!match(TOKEN_SEMICOLON)) {
        fprintf(stderr, "Expected ';' after expression\n");
        exit(1);
    }

    ExprStmt expr_stmt = { expr };
    Stmt* stmt = allocate(sizeof(Stmt));
    stmt->type = STMT_EXPR;
    stmt->expr = expr_stmt;
    return stmt;
}

// ----------------------------
// Entry point
// ----------------------------
Stmt** parse(Token* token_array, int token_count, int* stmt_count) {
    tokens = token_array;
    current = 0;
    total = token_count;

    Stmt** stmts = allocate(sizeof(Stmt*) * 128);
    int count = 0;

    while (!is_at_end()) {
        stmts[count++] = parse_statement();
    }

    *stmt_count = count;
    return stmts;
}

// ----------------------------
// Debug Printing
// ----------------------------
void print_indent(int indent) {
    for (int i = 0; i < indent; i++) printf("  ");
}

void print_expr(Expr* expr, int indent) {
    if (!expr) return;

    switch (expr->type) {
        case EXPR_LITERAL:
            print_indent(indent);
            printf("Literal: %s\n", expr->literal.value.lexeme);
            break;
        case EXPR_VARIABLE:
            print_indent(indent);
            printf("Variable: %s\n", expr->variable.name.lexeme);
            break;
        case EXPR_BINARY:
            print_indent(indent);
            printf("Binary: %s\n", expr->binary.op.lexeme);
            print_expr(expr->binary.left, indent + 1);
            print_expr(expr->binary.right, indent + 1);
            break;
        case EXPR_CALL:
            print_indent(indent);
            printf("Call:\n");
            print_expr(expr->call.callee, indent + 1);
            for (int i = 0; i < expr->call.arg_count; i++) {
                print_expr(expr->call.args[i], indent + 1);
            }
            break;
    }
}

void print_stmt(Stmt* stmt, int indent) {
    if (!stmt) return;

    switch (stmt->type) {
        case STMT_EXPR:
            print_indent(indent);
            printf("ExprStmt:\n");
            print_expr(stmt->expr.expression, indent + 1);
            break;
        case STMT_LET:
            print_indent(indent);
            printf("LetStmt: %s\n", stmt->let.name.lexeme);
            print_expr(stmt->let.initializer, indent + 1);
            break;
        case STMT_YAP:
            print_indent(indent);
            printf("YapStmt:\n");
            print_expr(stmt->yap.expression, indent + 1);
            break;
        case STMT_IF:
            print_indent(indent);
            printf("IfStmt:\n");
            print_indent(indent + 1);
            printf("Condition:\n");
            print_expr(stmt->if_stmt.condition, indent + 2);
            print_indent(indent + 1);
            printf("Then:\n");
            print_stmt(stmt->if_stmt.then_branch, indent + 2);
            if (stmt->if_stmt.else_branch) {
                print_indent(indent + 1);
                printf("Else:\n");
                print_stmt(stmt->if_stmt.else_branch, indent + 2);
            }
            break;
        case STMT_WHILE:
            print_indent(indent);
            printf("WhileStmt:\n");
            print_indent(indent + 1);
            printf("Condition:\n");
            print_expr(stmt->while_stmt.condition, indent + 2);
            print_indent(indent + 1);
            printf("Body:\n");
            print_stmt(stmt->while_stmt.body, indent + 2);
            break;
        case STMT_BLOCK:
            print_indent(indent);
            printf("BlockStmt:\n");
            for (int i = 0; i < stmt->block.count; i++) {
                print_stmt(stmt->block.statements[i], indent + 1);
            }
            break;
    }
}
