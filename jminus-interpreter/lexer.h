#ifndef LEXER_H
#define LEXER_H

#include <stddef.h> // For size_t (used for lengths, arrays, etc.)

// -----------------------------
// Token Types
// -----------------------------
typedef enum {
  // Keywords
  TOKEN_LET,
  TOKEN_FN,
  TOKEN_RETURN,
  TOKEN_IF,
  TOKEN_ELSE,
  TOKEN_WHILE,
  TOKEN_YAP,

  // Literals
  TOKEN_IDENTIFIER, // Variable or function name
  TOKEN_INT,        // Integer value
  TOKEN_FLOAT,      // Float value (optional feature)

  // Operators
  TOKEN_ASSIGN,         // =
  TOKEN_PLUS,           // +
  TOKEN_MINUS,          // -
  TOKEN_STAR,           // *
  TOKEN_SLASH,          // /
  TOKEN_EQUAL,          // ==
  TOKEN_BANG,           // !
  TOKEN_BANG_EQUAL,     // !=
  TOKEN_LESS,           // <
  TOKEN_LESS_EQUAL,     // <=
  TOKEN_GREATER,        // >
  TOKEN_GREATER_EQUAL,  // >=

  // Delimiters
  TOKEN_LPAREN,         // (
  TOKEN_RPAREN,         // )
  TOKEN_LBRACE,         // {
  TOKEN_RBRACE,         // }
  TOKEN_COMMA,          // ,
  TOKEN_SEMICOLON,      // ;

  // Special
  TOKEN_EOF,            // End of file/input
  TOKEN_UNKNOWN         // For unrecognized tokens (error recovery)
} TokenType;

// -----------------------------
// Token Structure
// -----------------------------
typedef struct {
  TokenType type;  // What kind of token
  char* lexeme;    // The actual string (e.g., "let", "123", "+")
  int line;        // Line number in the source code
} Token;

// -----------------------------
// Tokenizer API
// -----------------------------

// Tokenize the source code into a dynamically allocated array of Tokens
// The number of tokens is stored in `token_count` (passed by reference)
Token* tokenize(const char* source, int* token_count);

// Frees the memory allocated for the token array
void free_tokens(Token* tokens, int count);

// Optional: Useful during debugging
const char* token_type_to_string(TokenType type); // Converts TokenType to readable string
void print_token(Token token);                   // Prints a single token's type and value

#endif
