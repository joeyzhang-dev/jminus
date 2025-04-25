#ifndef PARSER_H
#define PARSER_H

#include "lexer.h"

// ----------------------------
// Expression Types
// ----------------------------
typedef enum {
    EXPR_LITERAL,
    EXPR_VARIABLE,
    EXPR_BINARY,
    EXPR_CALL
} ExprType;

typedef struct Expr Expr;  // Forward declare for self-referencing

typedef struct {
    Token value;  // e.g., a number like 5
} LiteralExpr;

typedef struct {
    Token name;  // e.g., a variable like x
} VariableExpr;

typedef struct {
    Expr* left;
    Token op;
    Expr* right;
} BinaryExpr;

typedef struct {
    Expr* callee;
    Expr** args;
    int arg_count;
} CallExpr;

struct Expr {
    ExprType type;
    union {
        LiteralExpr literal;
        VariableExpr variable;
        BinaryExpr binary;
        CallExpr call;
    };
};

// ----------------------------
// Statement Types
// ----------------------------
typedef enum {
    STMT_EXPR,
    STMT_LET,
    STMT_YAP,
    STMT_IF,
    STMT_WHILE,
    STMT_BLOCK
} StmtType;

typedef struct Stmt Stmt;

typedef struct {
    Token name;
    Expr* initializer;
} LetStmt;

typedef struct {
    Expr* expression;
} ExprStmt;

typedef struct {
    Expr* expression;
} YapStmt;

typedef struct {
    Expr* condition;
    Stmt* then_branch;
    Stmt* else_branch;  // optional
} IfStmt;

typedef struct {
    Expr* condition;
    Stmt* body;
} WhileStmt;

typedef struct {
    Stmt** statements;
    int count;
} BlockStmt;

struct Stmt {
    StmtType type;
    union {
        LetStmt let;
        ExprStmt expr;
        YapStmt yap;
        IfStmt if_stmt;
        WhileStmt while_stmt;
        BlockStmt block;
    };
};

// ----------------------------
// Functions
// ----------------------------
Stmt** parse(Token* tokens, int token_count, int* stmt_count);
void print_stmt(Stmt* stmt, int indent);

void free_ast(Stmt** stmts, int stmt_count);
#endif // PARSER_H
