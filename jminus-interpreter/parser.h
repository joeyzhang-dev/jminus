#ifndef PARSER_H
#define PARSER_H

#include "lexer.h"

// --------------------
// Expression Types
// --------------------
typedef enum {
  EXPR_LITERAL,      // e.g., 5
  EXPR_VARIABLE,     // e.g., x
  EXPR_BINARY,       // e.g., x + y
  EXPR_CALL          // e.g., yap(x)
} ExprType;

// Forward declare
struct Expr;

// A binary expression: left + right
typedef struct {
  struct Expr* left;
  Token op;
  struct Expr* right;
} BinaryExpr;

// A function call: yap(x, y)
typedef struct {
  struct Expr* callee;
  struct Expr** args;
  int arg_count;
} CallExpr;

// Literal: numbers
typedef struct {
  Token value;
} LiteralExpr;

// Variable usage: e.g. x
typedef struct {
  Token name;
} VariableExpr;

// Main expression node
typedef struct Expr {
  ExprType type;
  union {
    BinaryExpr binary;
    CallExpr call;
    LiteralExpr literal;
    VariableExpr variable;
  };
} Expr;

// --------------------
// Statement Types
// --------------------
typedef enum {
  STMT_LET,
  STMT_EXPR,
  STMT_BLOCK,
  STMT_YAP,
  STMT_IF,
  STMT_WHILE
} StmtType;

// Forward declare
struct Stmt;

// Let statement: let x = expr;
typedef struct {
  Token name;
  struct Expr* initializer;
} LetStmt;

// Expression statement: just a value (like `x + 1;`)
typedef struct {
  struct Expr* expression;
} ExprStmt;

// Block statement: { stmt; stmt; }
typedef struct {
  struct Stmt** statements;
  int count;
} BlockStmt;

// While statement
typedef struct {
  struct Expr* condition;
  struct Stmt* body;
} WhileStmt;

// If statement
typedef struct {
  struct Expr* condition;
  struct Stmt* then_branch;
  struct Stmt* else_branch; // NULL if no else
} IfStmt;

// Print/Yap statement
typedef struct {
  struct Expr* expression;
} YapStmt;

// Main statement node
typedef struct Stmt {
  StmtType type;
  union {
    LetStmt let;
    ExprStmt expr;
    BlockStmt block;
    WhileStmt while_stmt;
    IfStmt if_stmt;
    YapStmt yap;
  };
} Stmt;

// --------------------
// Parse API
// --------------------

// Entry point for the parser
Stmt** parse(Token* tokens, int token_count, int* stmt_count);

// AST debug printer declarations
void print_expr(Expr* expr, int indent);
void print_stmt(Stmt* stmt, int indent);

#endif
