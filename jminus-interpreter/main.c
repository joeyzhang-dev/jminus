// main.c
#include <stdio.h>      // Include standard input/output library for basic IO functions
#include <stdlib.h>     // Include standard library for memory allocation and exit functions
#include <string.h>     // Include string library for string manipulation functions
#include "lexer.h"      // Include our custom lexer header for tokenizing source code
#include "parser.h"     // Include our custom parser header for creating abstract syntax tree
#include "compiler.h"   // Include our custom compiler header for bytecode generation
#include "vm.h"         // Include our custom virtual machine header for code execution

// Read entire file contents
// This function reads a file into memory and returns it as a string
char* read_file(const char* filename) {
    // Open the file in binary mode to avoid issues with different newline representations
    FILE* file = fopen(filename, "rb"); // Binary mode to avoid newline issues
    
    // Check if the file opened successfully, exit with error message if not
    if (!file) {
        perror("Failed to open file");  // Print system error message
        exit(1);                        // Terminate program with error code
    }
    
    // Move file pointer to the end to determine file size
    fseek(file, 0, SEEK_END);
    // Get the current position which equals the file size in bytes
    long length = ftell(file);
    // Reset file pointer back to the beginning of the file
    rewind(file);
    
    // Allocate memory buffer with extra space for null terminator and potential additions
    char* buffer = malloc(length + 2);
    // Check if memory allocation succeeded
    if (!buffer) {
        perror("Failed to allocate buffer");  // Print system error message
        exit(1);                             // Terminate program with error code
    }
    
    // Read the entire file into our allocated buffer
    size_t read_size = fread(buffer, 1, length, file);
    // Add null terminator to make it a valid C string
    buffer[read_size] = '\0'; // Proper null termination
    
    // Close the file since we're done with it
    fclose(file);
    
    // Return the buffer containing the file contents
    return buffer;
}

// Remove trailing whitespace
// This function cleans up the source code by removing trailing whitespace characters
void clean_source(char* src) {
    // Get length of the source string
    int len = strlen(src);
    
    // Keep removing trailing whitespace characters until we hit non-whitespace or the beginning
    while (len > 0 && (src[len - 1] == ' ' || src[len - 1] == '\n' || src[len - 1] == '\r' || src[len - 1] == '\t')) {
        src[len - 1] = '\0';  // Replace whitespace with null terminator
        len--;                // Decrement length to check next character
    }
}

int main(int argc, char* argv[]) {
    // Default filename if none is provided through command line
    const char* filename = "start.jminus";
    // Debug flag, set to 0 initially (off)
    int debug = 0;
    
    // Process command-line arguments
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--debug") == 0) {
            // If argument is "--debug", turn on debug mode
            debug = 1;
        } else {
            // Otherwise treat the argument as a filename
            filename = argv[i];
        }
    }
    
    // Read the entire source file into memory
    char* source = read_file(filename);
    
    // Clean up any trailing whitespace in the source
    clean_source(source);
    
    // Print the source code for debugging purposes
    printf("---- SOURCE START ----\n%s\n---- SOURCE END ----\n", source);
    
    // Variable to store the count of tokens found during lexical analysis
    int token_count;
    
    // Perform lexical analysis (tokenization) on the source code
    Token* tokens = tokenize(source, &token_count);
    
    // If debug mode is enabled, print all tokens
    if (debug) {
        printf("\n--- Tokens ---\n");
        for (int i = 0; i < token_count; i++) {
            // Print each token's line number, type, and lexeme (text content)
            printf("[Line %d] %-15s %s\n", tokens[i].line, token_type_to_string(tokens[i].type), tokens[i].lexeme);
        }
    }
    
    // Variable to store the count of statements in our AST
    int stmt_count;
    
    // Parse the tokens into an Abstract Syntax Tree (AST)
    Stmt** stmts = parse(tokens, token_count, &stmt_count);
    
    // If debug mode is enabled, print the AST
    if (debug) {
        printf("\n--- AST ---\n");
        for (int i = 0; i < stmt_count; i++) {
            // Print each statement in the AST with indentation level 0
            print_stmt(stmts[i], 0);
        }
    }
    
    // Compile the AST into bytecode for our virtual machine
    Bytecode* bytecode = compile(stmts, stmt_count);
   
    // Execute the bytecode in our virtual machine
    run(bytecode);
    
    // Clean up memory - free the AST
    free_ast(stmts, stmt_count);
    
    // Clean up memory - free the tokens
    free_tokens(tokens, token_count);
    
    // Clean up memory - free the bytecode
    free_bytecode(bytecode);
    
    // Clean up memory - free the source code string
    free(source);
    
    // Return success code to operating system
    return 0;
}