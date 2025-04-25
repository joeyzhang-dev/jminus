#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "parser.h"

static Token* tokens;
static int current;
static int total;

static Token peek() {
  return tokens[current];
}

static Token advance() {
  if (current < total) current++;
  return tokens[current - 1];
}

static Token previous() {
  return tokens[current - 1];
}

static int match(TokenType type) {
  if (tokens[current].type == type) {
    advance();
    return 1;
  }
  return 0;
}

static int check(TokenType type) {
  return tokens[current].type == type;
}

static int is_at_end() {
  return tokens[current].type == TOKEN_EOF;
}

// Allocates a new node of any AST type
void* allocate(size_t size) {
  void* node = malloc(size);
  if (!node) {
    fprintf(stderr, "Memory allocation failed\n");
    exit(1);
  }
  memset(node, 0, size);
  return node;
}

Expr* parse_expression(); // Forward declare
Stmt* parse_statement();
Stmt* parse_let_statement();
Stmt* parse_yap_statement();

// ----------------------------
// Primary: numbers and variables
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

// ----------------------------
// Binary operations with precedence
// ----------------------------
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

Expr* parse_term() {
  TokenType ops[] = { TOKEN_STAR, TOKEN_SLASH };
  return parse_binary(parse_primary, ops, 2);
}

Expr* parse_expression() {
  TokenType ops[] = { TOKEN_PLUS, TOKEN_MINUS };
  return parse_binary(parse_term, ops, 2);
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
    fprintf(stderr, "Expected '=' after variable name in let\n");
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

Stmt* parse_statement() {
  if (match(TOKEN_LET)) return parse_let_statement();
  if (match(TOKEN_YAP)) return parse_yap_statement();

  Expr* expr = parse_expression();
  if (!match(TOKEN_SEMICOLON)) {
    fprintf(stderr, "Expected ';' after expression\n");
    exit(1);
  }

  ExprStmt e = { expr };
  Stmt* stmt = allocate(sizeof(Stmt));
  stmt->type = STMT_EXPR;
  stmt->expr = e;
  return stmt;
}

// ----------------------------
// Entry point
// ----------------------------
Stmt** parse(Token* token_array, int token_count, int* stmt_count) {
  tokens = token_array;
  current = 0;
  total = token_count;

  int capacity = 128;
  Stmt** stmts = allocate(sizeof(Stmt*) * capacity);
  int count = 0;

  while (!is_at_end()) {
    Stmt* stmt = parse_statement();
    stmts[count++] = stmt;

    if (count >= capacity) {
      capacity *= 2;
      stmts = realloc(stmts, sizeof(Stmt*) * capacity);
    }
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
