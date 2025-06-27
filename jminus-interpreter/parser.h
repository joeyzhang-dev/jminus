/**
 * @file parser.h
 * @brief Parser and Abstract Syntax Tree (AST) for the jminus language
 * @author Joey Zhang
 * @version 1.0.0
 * 
 * The parser converts a stream of tokens into an Abstract Syntax Tree (AST).
 * The AST represents the syntactic structure of the program in a tree format
 * that can be easily traversed and analyzed.
 * 
 * Parser Architecture:
 * - Uses recursive descent parsing technique
 * - Each grammar rule corresponds to a parsing function
 * - Top-down approach: starts with program, works down to expressions
 * - Error recovery: continues parsing despite individual errors
 * 
 * AST Structure:
 * The AST consists of two main node types:
 * - Expressions: Compute values (literals, variables, operations)
 * - Statements: Perform actions (declarations, control flow, blocks)
 * 
 * Parsing Strategy:
 * 1. Program → Statement* (zero or more statements)
 * 2. Statement → Let | Yap | If | While | Block | Expression
 * 3. Expression → Binary | Unary | Primary
 * 4. Primary → Literal | Variable | Grouped
 * 
 * Error Handling:
 * - Syntax errors are reported with line numbers
 * - Parser attempts to recover and continue
 * - Invalid constructs are skipped when possible
 * - Error tokens are handled gracefully
 */

#ifndef PARSER_H
#define PARSER_H

#include "lexer.h"

// Forward declarations for AST node types
typedef struct Expr Expr;
typedef struct Stmt Stmt;

/**
 * @brief Represents a literal value in the AST
 * 
 * Literals are constant values that appear directly in the source code.
 * Currently supports only integer literals.
 */
typedef struct {
    Token value; ///< The token containing the literal value
} LiteralExpr;

/**
 * @brief Represents a variable reference in the AST
 * 
 * Variable expressions refer to previously declared variables.
 * The name field contains the variable identifier.
 */
typedef struct {
    Token name; ///< The token containing the variable name
} VariableExpr;

/**
 * @brief Represents a binary operation in the AST
 * 
 * Binary expressions combine two sub-expressions with an operator.
 * Examples: a + b, x * y, condition == true
 */
typedef struct {
    Expr* left;   ///< Left-hand side expression
    Token op;     ///< The binary operator token
    Expr* right;  ///< Right-hand side expression
} BinaryExpr;

/**
 * @brief Represents a function call in the AST
 * 
 * Function calls invoke functions with arguments.
 * Currently only supports the built-in 'yap' function.
 */
typedef struct {
    Expr* callee;      ///< The function being called
    Expr** args;       ///< Array of argument expressions
    int arg_count;     ///< Number of arguments
} CallExpr;

/**
 * @brief Union of all possible expression types
 * 
 * The Expr struct uses a tagged union to represent different
 * kinds of expressions while maintaining a single interface.
 */
typedef struct Expr {
    enum {
        EXPR_LITERAL,  ///< Constant value (number)
        EXPR_VARIABLE, ///< Variable reference
        EXPR_BINARY,   ///< Binary operation
        EXPR_CALL      ///< Function call
    } type;            ///< Tag indicating the expression type
    
    union {
        LiteralExpr literal;   ///< Literal expression data
        VariableExpr variable; ///< Variable expression data
        BinaryExpr binary;     ///< Binary expression data
        CallExpr call;         ///< Function call data
    };
} Expr;

/**
 * @brief Represents an expression statement in the AST
 * 
 * Expression statements evaluate an expression for its side effects.
 * Examples: x = 5; (assignment), yap(x); (function call)
 */
typedef struct {
    Expr* expression; ///< The expression to evaluate
} ExprStmt;

/**
 * @brief Represents a variable declaration in the AST
 * 
 * Let statements declare new variables with initial values.
 * Example: let x = 42;
 */
typedef struct {
    Token name;        ///< Variable name token
    Expr* initializer; ///< Initial value expression
} LetStmt;

/**
 * @brief Represents a print statement in the AST
 * 
 * Yap statements print the result of an expression.
 * Example: yap(x + 1);
 */
typedef struct {
    Expr* expression; ///< Expression to print
} YapStmt;

/**
 * @brief Represents an if statement in the AST
 * 
 * If statements provide conditional execution.
 * Example: if (x > 0) { yap(x); } else { yap(0); }
 */
typedef struct {
    Expr* condition;     ///< Boolean condition expression
    Stmt* then_branch;   ///< Statement to execute if condition is true
    Stmt* else_branch;   ///< Statement to execute if condition is false (optional)
} IfStmt;

/**
 * @brief Represents a while loop in the AST
 * 
 * While statements provide repeated execution.
 * Example: while (i < 10) { i = i + 1; }
 */
typedef struct {
    Expr* condition; ///< Boolean condition expression
    Stmt* body;      ///< Statement to execute repeatedly
} WhileStmt;

/**
 * @brief Represents a block of statements in the AST
 * 
 * Block statements group multiple statements together.
 * They also create new variable scopes.
 * Example: { let x = 1; yap(x); }
 */
typedef struct {
    Stmt** statements; ///< Array of statements in the block
    int count;         ///< Number of statements
} BlockStmt;

/**
 * @brief Union of all possible statement types
 * 
 * The Stmt struct uses a tagged union to represent different
 * kinds of statements while maintaining a single interface.
 */
typedef struct Stmt {
    enum {
        STMT_EXPR,   ///< Expression statement
        STMT_LET,    ///< Variable declaration
        STMT_YAP,    ///< Print statement
        STMT_IF,     ///< Conditional statement
        STMT_WHILE,  ///< Loop statement
        STMT_BLOCK   ///< Block of statements
    } type;          ///< Tag indicating the statement type
    
    union {
        ExprStmt expr;    ///< Expression statement data
        LetStmt let;      ///< Let statement data
        YapStmt yap;      ///< Yap statement data
        IfStmt if_stmt;   ///< If statement data
        WhileStmt while_stmt; ///< While statement data
        BlockStmt block;  ///< Block statement data
    };
} Stmt;

/**
 * @brief Parses a stream of tokens into an Abstract Syntax Tree
 * @param token_array Array of tokens to parse
 * @param token_count Number of tokens in the array
 * @param stmt_count Pointer to store the number of statements parsed
 * @return Array of statement nodes (caller must free with free_ast())
 * 
 * This function is the main entry point for parsing. It:
 * 1. Initializes the parser state with the token stream
 * 2. Parses the entire program as a sequence of statements
 * 3. Returns an array of top-level statement nodes
 * 4. Reports the number of statements parsed
 * 
 * Parsing Process:
 * - Each statement is parsed according to its grammar rule
 * - Expressions are parsed with operator precedence
 * - Error recovery allows parsing to continue after errors
 * - The resulting AST represents the complete program structure
 * 
 * Memory Management:
 * - Returns dynamically allocated array of statement nodes
 * - Caller is responsible for freeing with free_ast()
 * - All AST nodes are properly allocated and linked
 * 
 * Error Handling:
 * - Syntax errors are reported to stderr
 * - Parser attempts to recover and continue
 * - Returns NULL if parsing fails completely
 */
Stmt** parse(Token* token_array, int token_count, int* stmt_count);

/**
 * @brief Frees all memory allocated for the AST
 * @param stmts Array of statement nodes to free
 * @param stmt_count Number of statements in the array
 * 
 * This function recursively deallocates all AST nodes:
 * - Statement nodes and their data
 * - Expression nodes and their data
 * - Arrays of statements in blocks
 * - Arrays of expressions in function calls
 * 
 * Memory is freed in post-order traversal to avoid
 * accessing freed memory during cleanup.
 */
void free_ast(Stmt** stmts, int stmt_count);

/**
 * @brief Prints a human-readable representation of the AST
 * @param stmt The statement node to print
 * @param indent Current indentation level for pretty printing
 * 
 * This function is used for debugging and development.
 * It prints the AST structure with proper indentation
 * to show the hierarchical relationships between nodes.
 * 
 * Output Format:
 * - Each node type is clearly labeled
 * - Nested structures are indented
 * - Token values are shown for literals and identifiers
 * - The structure mirrors the actual AST layout
 */
void print_stmt(Stmt* stmt, int indent);

#endif // PARSER_H 