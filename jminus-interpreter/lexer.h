/**
 * @file lexer.h
 * @brief Lexical analyzer (tokenizer) for the jminus language
 * @author Joey Zhang
 * @version 1.0.0
 * 
 * The lexer converts source code text into a stream of tokens.
 * Each token represents a meaningful unit of the language:
 * - Keywords (let, if, while, etc.)
 * - Identifiers (variable names)
 * - Literals (numbers)
 * - Operators (+, -, *, /, etc.)
 * - Delimiters (parentheses, braces, semicolons)
 * 
 * Token Structure:
 * - type: The kind of token (keyword, identifier, etc.)
 * - lexeme: The actual text that was tokenized
 * - line: Source line number for error reporting
 * 
 * Lexical Analysis Process:
 * 1. Scan source code character by character
 * 2. Identify token boundaries based on language rules
 * 3. Classify each token according to its content
 * 4. Track line numbers for error reporting
 * 5. Handle whitespace and comments appropriately
 * 
 * Error Handling:
 * - Invalid characters are reported with line numbers
 * - Unterminated strings/literals are detected
 * - Lexical errors don't stop the entire process
 */

#ifndef LEXER_H
#define LEXER_H

/**
 * @brief Enumeration of all possible token types in jminus
 * 
 * Token types are organized into categories:
 * - Keywords: Language-specific reserved words
 * - Literals: Constant values (numbers, strings)
 * - Operators: Mathematical and logical operators
 * - Delimiters: Punctuation that structures the language
 * - Special: End-of-file and error markers
 */
typedef enum {
    // Keywords
    TOKEN_LET,
    TOKEN_FN,
    TOKEN_YAP,
    TOKEN_RETURN,
    TOKEN_IF,
    TOKEN_WHILE,
    TOKEN_ELSE,

    // Identifiers and literals
    TOKEN_IDENTIFIER,
    TOKEN_INT,
    TOKEN_FLOAT,

    // Operators
    TOKEN_ASSIGN,      // =
    TOKEN_PLUS,        // +
    TOKEN_MINUS,       // -
    TOKEN_STAR,        // *
    TOKEN_SLASH,       // /
    TOKEN_EQUAL,       // ==
    TOKEN_BANG,        // !
    TOKEN_BANG_EQUAL,  // !=
    TOKEN_NOT_EQUAL,   // != (alias)
    TOKEN_LESS,        // <
    TOKEN_LESS_EQUAL,  // <=
    TOKEN_GREATER,     // >
    TOKEN_GREATER_EQUAL, // >=

    // Delimiters
    TOKEN_LPAREN,      // (
    TOKEN_RPAREN,      // )
    TOKEN_LBRACE,      // {
    TOKEN_RBRACE,      // }
    TOKEN_SEMICOLON,   // ;
    TOKEN_COMMA,       // ,

    // Special
    TOKEN_EOF,
    TOKEN_UNKNOWN,     // Unknown/invalid token
    TOKEN_ERROR        // Error token for invalid input
} TokenType;

/**
 * @brief Represents a single token in the source code
 * 
 * A token is the smallest meaningful unit of the language.
 * The lexer produces a stream of tokens that the parser consumes.
 * 
 * Fields:
 * - type: The category of the token (keyword, operator, etc.)
 * - lexeme: The actual text that was tokenized
 * - line: Source line number for error reporting and debugging
 */
typedef struct {
    TokenType type;     ///< The type/category of this token
    char* lexeme;       ///< The actual text that was tokenized
    int line;           ///< Source line number (1-indexed)
} Token;

/**
 * @brief Converts source code string into an array of tokens
 * @param source The source code to tokenize
 * @param token_count Pointer to store the number of tokens generated
 * @return Array of tokens (caller must free with free_tokens())
 * 
 * This function performs lexical analysis on the source code:
 * 1. Scans the source string character by character
 * 2. Identifies token boundaries based on language rules
 * 3. Creates Token structures for each meaningful unit
 * 4. Tracks line numbers for error reporting
 * 5. Returns a dynamically allocated array of tokens
 * 
 * Tokenization Rules:
 * - Keywords are recognized by exact string matches
 * - Identifiers start with a letter and contain letters/digits
 * - Numbers are sequences of digits (integers only)
 * - Operators are single or double characters
 * - Whitespace separates tokens but is not tokenized
 * - Line numbers are tracked for error reporting
 * 
 * Memory Management:
 * - Returns dynamically allocated array of tokens
 * - Caller is responsible for freeing with free_tokens()
 * - Lexeme strings point into the original source (no copying)
 * 
 * Error Handling:
 * - Invalid characters produce TOKEN_ERROR tokens
 * - Line numbers are preserved for error reporting
 * - Tokenization continues despite individual errors
 */
Token* tokenize(const char* source, int* token_count);

/**
 * @brief Frees memory allocated for token array
 * @param tokens Array of tokens to free
 * @param count Number of tokens in the array
 * 
 * This function deallocates the memory used by the token array.
 * Note that lexeme strings are not freed since they point into
 * the original source code string.
 */
void free_tokens(Token* tokens, int count);

/**
 * @brief Converts a token type to a human-readable string
 * @param type The token type to convert
 * @return String representation of the token type
 * 
 * This function is primarily used for debugging and error reporting.
 * It provides a readable name for each token type.
 */
const char* token_type_to_string(TokenType type);

#endif // LEXER_H 