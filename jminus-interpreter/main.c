/**
 * @file main.c
 * @brief Main entry point for the jminus interpreter
 * @author Joey Zhang
 * @version 1.0.0
 * 
 * This file implements the complete jminus execution pipeline:
 * Source Code → Lexer → Parser → AST → Compiler → Bytecode → VM → Output
 * 
 * The main function orchestrates all components:
 * 1. Reads source code from a file
 * 2. Tokenizes the source into a stream of tokens
 * 3. Parses tokens into an Abstract Syntax Tree (AST)
 * 4. Compiles the AST into bytecode for the virtual machine
 * 5. Executes the bytecode and produces output
 * 
 * Architecture Overview:
 * - Lexer: Converts source text into structured tokens
 * - Parser: Builds a tree representation of the program structure
 * - Compiler: Translates the tree into stack-based bytecode
 * - VM: Executes bytecode instructions on a virtual stack machine
 * 
 * Error Handling:
 * - File I/O errors are handled with descriptive messages
 * - Memory allocation failures trigger immediate exit
 * - All allocated resources are properly freed before exit
 */

#include <stdio.h>      // Include standard input/output library for basic IO functions
#include <stdlib.h>     // Include standard library for memory allocation and exit functions
#include <string.h>     // Include string library for string manipulation functions
#include "lexer.h"      // Include our custom lexer header for tokenizing source code
#include "parser.h"     // Include our custom parser header for creating abstract syntax tree
#include "compiler.h"   // Include our custom compiler header for bytecode generation
#include "vm.h"         // Include our custom virtual machine header for code execution

/**
 * @brief Reads an entire file into memory as a null-terminated string
 * @param filename Path to the file to read
 * @return Pointer to allocated memory containing file contents
 * @note Caller is responsible for freeing the returned memory
 * @note Exits program on file I/O or memory allocation errors
 * 
 * This function:
 * - Opens the file in binary mode to avoid newline conversion issues
 * - Determines file size by seeking to end and getting position
 * - Allocates memory with extra space for null terminator
 * - Reads entire file content into the allocated buffer
 * - Ensures proper null termination for C string compatibility
 * 
 * Error Handling:
 * - Exits with error code 1 if file cannot be opened
 * - Exits with error code 1 if memory allocation fails
 * - Provides system error messages via perror()
 */
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

/**
 * @brief Removes trailing whitespace characters from source code
 * @param src Pointer to the source code string to clean
 * 
 * This function modifies the source string in-place by:
 * - Starting from the end of the string
 * - Replacing whitespace characters (space, newline, carriage return, tab) with null terminators
 * - Stopping when it encounters the first non-whitespace character
 * 
 * This ensures clean source code without trailing whitespace that could cause parsing issues.
 */
void clean_source(char* src) {
    // Get length of the source string
    int len = strlen(src);
    
    // Keep removing trailing whitespace characters until we hit non-whitespace or the beginning
    while (len > 0 && (src[len - 1] == ' ' || src[len - 1] == '\n' || src[len - 1] == '\r' || src[len - 1] == '\t')) {
        src[len - 1] = '\0';  // Replace whitespace with null terminator
        len--;                // Decrement length to check next character
    }
}

/**
 * @brief Main entry point for the jminus interpreter
 * @param argc Number of command-line arguments
 * @param argv Array of command-line argument strings
 * @return 0 on successful execution, 1 on error
 * 
 * Command-line usage:
 *   jminus.exe [--debug] [filename]
 * 
 * Arguments:
 *   --debug  Enable debug output (shows tokens and AST)
 *   filename Path to .jminus file to execute (defaults to "start.jminus")
 * 
 * Execution Pipeline:
 * 1. Parse command-line arguments
 * 2. Read and clean source code from file
 * 3. Tokenize source code into structured tokens
 * 4. Parse tokens into Abstract Syntax Tree (AST)
 * 5. Compile AST into bytecode instructions
 * 6. Execute bytecode on virtual machine
 * 7. Clean up all allocated resources
 * 
 * Debug Mode:
 * When --debug is specified, the program outputs:
 * - Complete source code
 * - Token stream with line numbers and types
 * - Abstract Syntax Tree representation
 * - Final execution output
 * 
 * Memory Management:
 * All dynamically allocated memory is properly freed:
 * - Source code buffer
 * - Token array
 * - AST nodes
 * - Bytecode structure
 */
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
    // This converts the source string into a structured array of tokens
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
    // This creates a tree structure representing the program's syntax
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
    // This translates the tree structure into linear bytecode instructions
    Bytecode* bytecode = compile(stmts, stmt_count);
   
    // Execute the bytecode in our virtual machine
    // This runs the compiled instructions and produces the program's output
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