#include <stdio.h>     // Include standard input/output for printf, fprintf functions
#include <stdlib.h>    // Include standard library for memory management (malloc, free)
#include <string.h>    // Include string library for string operations like memset
#include "parser.h"    // Include our custom parser header with necessary data structures

// ----------------------------
// Internal State
// ----------------------------
static Token* tokens;   // Static array to hold all tokens from lexical analysis
static int current;     // Current position in the tokens array
static int total;       // Total number of tokens in the array

// ----------------------------
// Helpers
// ----------------------------
static Token peek() {
    // Return the current token without advancing the position
    return tokens[current];
}

static Token previous() {
    // Return the token right before the current position
    return tokens[current - 1];
}

static int is_at_end() {
    // Check if we've reached the end of the token stream
    return tokens[current].type == TOKEN_EOF;
}

static Token advance() {
    // Move to the next token if not at the end, and return the previous token
    if (!is_at_end()) current++;
    return previous();
}

static int check(TokenType type) {
    // Check if the current token matches the expected type
    return !is_at_end() && tokens[current].type == type;
}

static int match(TokenType type) {
    // Check if current token matches type, and advance if it does
    if (check(type)) {
        advance();
        return 1;  // Return true if matched
    }
    return 0;      // Return false if not matched
}

void* allocate(size_t size) {
    // Allocate memory of given size and initialize it to zeros
    void* node = malloc(size);
    // Check if allocation was successful
    if (!node) {
        fprintf(stderr, "Memory allocation failed\n");
        exit(1);  // Exit program if memory allocation fails
    }
    // Initialize allocated memory to all zeros
    memset(node, 0, size);
    return node;
}

// ----------------------------
// Memory cleanup
// ----------------------------
static void free_expr(Expr* expr) {
    // Recursively free memory for expression nodes
    if (!expr) return;  // Return if null pointer
    
    // Handle different expression types differently
    switch (expr->type) {
        case EXPR_BINARY:
            // Free both operands of binary expression
            free_expr(expr->binary.left);
            free_expr(expr->binary.right);
            break;
        case EXPR_CALL:
            // Free callee and all arguments of function call
            free_expr(expr->call.callee);
            for (int i = 0; i < expr->call.arg_count; i++) {
                free_expr(expr->call.args[i]);
            }
            // Free the array of argument pointers
            free(expr->call.args);
            break;
        case EXPR_LITERAL:
        case EXPR_VARIABLE:
            // These don't have any sub-expressions to free
            break;
    }
    // Free the expression node itself
    free(expr);
}

static void free_stmt(Stmt* stmt) {
    // Recursively free memory for statement nodes
    if (!stmt) return;  // Return if null pointer
    
    // Handle different statement types differently
    switch (stmt->type) {
        case STMT_LET:
            // Free the initializer expression of let statement
            free_expr(stmt->let.initializer);
            break;
        case STMT_EXPR:
            // Free the expression in expression statement
            free_expr(stmt->expr.expression);
            break;
        case STMT_YAP:
            // Free the expression in yap statement (print)
            free_expr(stmt->yap.expression);
            break;
        case STMT_IF:
            // Free condition and both branches of if statement
            free_expr(stmt->if_stmt.condition);
            free_stmt(stmt->if_stmt.then_branch);
            free_stmt(stmt->if_stmt.else_branch);
            break;
        case STMT_WHILE:
            // Free condition and body of while statement
            free_expr(stmt->while_stmt.condition);
            free_stmt(stmt->while_stmt.body);
            break;
        case STMT_BLOCK:
            // Free all statements in a block
            for (int i = 0; i < stmt->block.count; i++) {
                free_stmt(stmt->block.statements[i]);
            }
            // Free the array of statement pointers
            free(stmt->block.statements);
            break;
    }
    // Free the statement node itself
    free(stmt);
}

void free_ast(Stmt** stmts, int stmt_count) {
    // Free the entire abstract syntax tree
    for (int i = 0; i < stmt_count; i++) {
        // Free each top-level statement
        free_stmt(stmts[i]);
    }
    // Free the array of statement pointers
    free(stmts);
}

// ----------------------------
// Forward declarations
// ----------------------------
// These declarations allow functions to call each other even when defined later
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
    // Parse literals, variables, and parenthesized expressions
    
    if (match(TOKEN_INT)) {
        // Create a literal expression node for integer tokens
        LiteralExpr lit = { previous() };
        Expr* expr = allocate(sizeof(Expr));
        expr->type = EXPR_LITERAL;
        expr->literal = lit;
        return expr;
    }

    if (match(TOKEN_IDENTIFIER)) {
        // Create a variable expression node for identifier tokens
        VariableExpr var = { previous() };
        Expr* expr = allocate(sizeof(Expr));
        expr->type = EXPR_VARIABLE;
        expr->variable = var;
        return expr;
    }

    if (match(TOKEN_LPAREN)) {
        // Parse parenthesized expression and check for closing parenthesis
        Expr* expr = parse_expression();
        if (!expr || !match(TOKEN_RPAREN)) {
            fprintf(stderr, "Expected ')' after expression\n");
            return NULL;
        }
        return expr;
    }

    // Error if unexpected token encountered
    fprintf(stderr, "Unexpected token: %s\n", peek().lexeme);
    return NULL;
}

Expr* parse_binary(Expr* (*next_fn)(), TokenType ops[], int op_count) {
    // Generic binary expression parser used for different precedence levels
    
    // Parse the left-hand operand using the next higher precedence parser
    Expr* left = next_fn();
    if (!left) return NULL;

    // Continue parsing operators at this precedence level while possible
    while (!is_at_end()) {
        TokenType type = peek().type;
        int matched = 0;
        
        // Check if current token is one of the operators at this level
        for (int i = 0; i < op_count; i++) {
            if (type == ops[i]) {
                matched = 1;
                break;
            }
        }
        if (!matched) break;  // Exit if no operator at this level

        // Consume the operator token
        Token op = advance();
        
        // Parse the right-hand operand
        Expr* right = next_fn();
        if (!right) return NULL;

        // Create binary expression node with left, operator, and right
        BinaryExpr bin = { left, op, right };
        Expr* expr = allocate(sizeof(Expr));
        expr->type = EXPR_BINARY;
        expr->binary = bin;
        
        // For left-associativity, the new expression becomes the left operand
        left = expr;
    }

    return left;
}

Expr* parse_factor() {
    // Parse multiplication and division (highest precedence binary ops)
    TokenType ops[] = { TOKEN_STAR, TOKEN_SLASH };
    return parse_binary(parse_primary, ops, 2);
}

Expr* parse_term() {
    // Parse addition and subtraction (medium precedence binary ops)
    TokenType ops[] = { TOKEN_PLUS, TOKEN_MINUS };
    return parse_binary(parse_factor, ops, 2);
}

Expr* parse_comparison() {
    // Parse comparison operators (<, <=, >, >=)
    TokenType ops[] = { TOKEN_LESS, TOKEN_LESS_EQUAL, TOKEN_GREATER, TOKEN_GREATER_EQUAL };
    return parse_binary(parse_term, ops, 4);
}

Expr* parse_equality() {
    // Parse equality operators (==, !=)
    TokenType ops[] = { TOKEN_EQUAL, TOKEN_BANG_EQUAL };
    return parse_binary(parse_comparison, ops, 2);
}

Expr* parse_expression() {
    // Top-level expression parser that handles assignments
    
    // Parse left-hand side (could be variable or other expression)
    Expr* expr = parse_equality();
    if (!expr) return NULL;

    // Check if this is an assignment expression
    if (match(TOKEN_ASSIGN)) {
        Token equals = previous();
        
        // Parse right-hand side of assignment
        Expr* value = parse_expression();
        if (!value) return NULL;

        // Ensure left side is a valid assignment target (variable)
        if (expr->type != EXPR_VARIABLE) {
            fprintf(stderr, "Invalid assignment target.\n");
            return NULL;
        }

        // Create binary expression for assignment
        BinaryExpr bin = { expr, equals, value };
        Expr* assignment = allocate(sizeof(Expr));
        assignment->type = EXPR_BINARY;
        assignment->binary = bin;
        return assignment;
    }

    // Return the parsed expression if not an assignment
    return expr;
}

// ----------------------------
// Statement Parsing
// ----------------------------
Stmt* parse_let_statement() {
    // Parse variable declarations (let statements)
    
    // Expect variable name after 'let' keyword
    if (!match(TOKEN_IDENTIFIER)) {
        fprintf(stderr, "Expected variable name after 'let'\n");
        return NULL;
    }
    Token name = previous();

    // Expect assignment operator after variable name
    if (!match(TOKEN_ASSIGN)) {
        fprintf(stderr, "Expected '=' after variable name\n");
        return NULL;
    }

    // Parse initialization expression
    Expr* initializer = parse_expression();
    if (!initializer) return NULL;

    // Expect semicolon after expression
    if (!match(TOKEN_SEMICOLON)) {
        fprintf(stderr, "Expected ';' after let initializer\n");
        return NULL;
    }

    // Create let statement node
    LetStmt let = { name, initializer };
    Stmt* stmt = allocate(sizeof(Stmt));
    stmt->type = STMT_LET;
    stmt->let = let;
    return stmt;
}

Stmt* parse_yap_statement() {
    // Parse print statements (yap statements)
    
    // Expect opening parenthesis after 'yap' keyword
    if (!match(TOKEN_LPAREN)) {
        fprintf(stderr, "Expected '(' after 'yap'\n");
        return NULL;
    }

    // Parse expression to be printed
    Expr* expr = parse_expression();
    if (!expr) return NULL;

    // Expect closing parenthesis after expression
    if (!match(TOKEN_RPAREN)) {
        fprintf(stderr, "Expected ')' after yap expression\n");
        return NULL;
    }

    // Expect semicolon after closing parenthesis
    if (!match(TOKEN_SEMICOLON)) {
        fprintf(stderr, "Expected ';' after yap statement\n");
        return NULL;
    }

    // Create yap statement node
    YapStmt yap = { expr };
    Stmt* stmt = allocate(sizeof(Stmt));
    stmt->type = STMT_YAP;
    stmt->yap = yap;
    return stmt;
}

Stmt* parse_if_statement() {
    // Parse if statements with optional else branch
    
    // Expect opening parenthesis after 'if' keyword
    if (!match(TOKEN_LPAREN)) {
        fprintf(stderr, "Expected '(' after 'if'\n");
        return NULL;
    }

    // Parse condition expression
    Expr* condition = parse_expression();
    if (!condition) return NULL;

    // Expect closing parenthesis after condition
    if (!match(TOKEN_RPAREN)) {
        fprintf(stderr, "Expected ')' after condition\n");
        return NULL;
    }

    // Parse then branch (statement to execute if condition is true)
    Stmt* then_branch = parse_statement();
    if (!then_branch) return NULL;

    // Parse optional else branch
    Stmt* else_branch = NULL;
    if (match(TOKEN_ELSE)) {
        else_branch = parse_statement();
        if (!else_branch) return NULL;
    }

    // Create if statement node
    IfStmt if_stmt = { condition, then_branch, else_branch };
    Stmt* stmt = allocate(sizeof(Stmt));
    stmt->type = STMT_IF;
    stmt->if_stmt = if_stmt;
    return stmt;
}

Stmt* parse_while_statement() {
    // Parse while loop statements
    
    // Expect opening parenthesis after 'while' keyword
    if (!match(TOKEN_LPAREN)) {
        fprintf(stderr, "Expected '(' after 'while'\n");
        return NULL;
    }

    // Parse loop condition expression
    Expr* condition = parse_expression();
    if (!condition) return NULL;

    // Expect closing parenthesis after condition
    if (!match(TOKEN_RPAREN)) {
        fprintf(stderr, "Expected ')' after while condition\n");
        return NULL;
    }

    // Parse loop body statement
    Stmt* body = parse_statement();
    if (!body) return NULL;

    // Create while statement node
    WhileStmt while_stmt = { condition, body };
    Stmt* stmt = allocate(sizeof(Stmt));
    stmt->type = STMT_WHILE;
    stmt->while_stmt = while_stmt;
    return stmt;
}

Stmt* parse_block_statement() {
    // Parse block of statements enclosed in braces
    
    // Allocate array for statements (max 128 statements per block)
    Stmt** statements = allocate(sizeof(Stmt*) * 128);
    int count = 0;

    // Parse statements until closing brace or end of file
    while (!check(TOKEN_RBRACE) && !is_at_end()) {
        Stmt* stmt = parse_statement();
        if (!stmt) return NULL;
        statements[count++] = stmt;
    }

    // Expect closing brace after all statements
    if (!match(TOKEN_RBRACE)) {
        fprintf(stderr, "Expected '}' after block\n");
        return NULL;
    }

    // Create block statement node
    BlockStmt block = { statements, count };
    Stmt* stmt = allocate(sizeof(Stmt));
    stmt->type = STMT_BLOCK;
    stmt->block = block;
    return stmt;
}

Stmt* parse_statement() {
    // Parse any kind of statement, dispatching to specific parsers
    
    // Check for specific statement keywords and dispatch accordingly
    if (match(TOKEN_LET)) return parse_let_statement();
    if (match(TOKEN_YAP)) return parse_yap_statement();
    if (match(TOKEN_IF)) return parse_if_statement();
    if (match(TOKEN_WHILE)) return parse_while_statement();
    if (match(TOKEN_LBRACE)) return parse_block_statement();

    // Default case: expression statement
    Expr* expr = parse_expression();
    if (!expr) return NULL;

    // Expect semicolon after expression
    if (!match(TOKEN_SEMICOLON)) {
        fprintf(stderr, "Expected ';' after expression\n");
        return NULL;
    }

    // Create expression statement node
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
    // Main entry point for the parser
    
    // Initialize parser state with token array
    tokens = token_array;
    current = 0;
    total = token_count;

    // Allocate array for top-level statements (max 128)
    Stmt** stmts = allocate(sizeof(Stmt*) * 128);
    int count = 0;

    // Parse statements until end of file
    while (!is_at_end()) {
        Stmt* stmt = parse_statement();
        if (!stmt) {
            // Parsing error happened, clean up and return NULL
            free_ast(stmts, count);
            return NULL;
        }
        stmts[count++] = stmt;
    }

    // Set output parameter to number of statements parsed
    *stmt_count = count;
    return stmts;
}

// ----------------------------
// Debug Printing
// ----------------------------
void print_indent(int indent) {
    // Print indentation spaces for AST pretty printing
    for (int i = 0; i < indent; i++) printf("  ");
}

void print_expr(Expr* expr, int indent) {
    // Print expression node with given indentation level
    if (!expr) return;

    switch (expr->type) {
        case EXPR_LITERAL:
            // Print literal value
            print_indent(indent);
            printf("Literal: %s\n", expr->literal.value.lexeme);
            break;
        case EXPR_VARIABLE:
            // Print variable name
            print_indent(indent);
            printf("Variable: %s\n", expr->variable.name.lexeme);
            break;
        case EXPR_BINARY:
            // Print binary operator and its operands (recursively)
            print_indent(indent);
            printf("Binary: %s\n", expr->binary.op.lexeme);
            print_expr(expr->binary.left, indent + 1);  // Left operand with increased indent
            print_expr(expr->binary.right, indent + 1); // Right operand with increased indent
            break;
        case EXPR_CALL:
            // Print function call and its arguments (recursively)
            print_indent(indent);
            printf("Call:\n");
            print_expr(expr->call.callee, indent + 1);  // Function being called
            for (int i = 0; i < expr->call.arg_count; i++) {
                print_expr(expr->call.args[i], indent + 1);  // Each argument
            }
            break;
    }
}

void print_stmt(Stmt* stmt, int indent) {
    // Print statement node with given indentation level
    if (!stmt) return;

    switch (stmt->type) {
        case STMT_EXPR:
            // Print expression statement
            print_indent(indent);
            printf("ExprStmt:\n");
            print_expr(stmt->expr.expression, indent + 1);
            break;
        case STMT_LET:
            // Print variable declaration
            print_indent(indent);
            printf("LetStmt: %s\n", stmt->let.name.lexeme);
            print_expr(stmt->let.initializer, indent + 1);
            break;
        case STMT_YAP:
            // Print yap (print) statement
            print_indent(indent);
            printf("YapStmt:\n");
            print_expr(stmt->yap.expression, indent + 1);
            break;
        case STMT_IF:
            // Print if statement with condition and branches
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
            // Print while statement with condition and body
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
            // Print block statement with all contained statements
            print_indent(indent);
            printf("BlockStmt:\n");
            for (int i = 0; i < stmt->block.count; i++) {
                print_stmt(stmt->block.statements[i], indent + 1);
            }
            break;
    }
}
