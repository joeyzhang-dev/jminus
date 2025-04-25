// main.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lexer.h"
#include "parser.h"
#include "compiler.h"
#include "vm.h"

// Read entire file contents
char* read_file(const char* filename) {
    FILE* file = fopen(filename, "rb"); // Binary mode to avoid newline issues
    if (!file) {
        perror("Failed to open file");
        exit(1);
    }

    fseek(file, 0, SEEK_END);
    long length = ftell(file);
    rewind(file);

    char* buffer = malloc(length + 2);
    if (!buffer) {
        perror("Failed to allocate buffer");
        exit(1);
    }

    size_t read_size = fread(buffer, 1, length, file);
    buffer[read_size] = '\0'; // Proper null termination
    fclose(file);
    return buffer;
}

// Remove trailing whitespace
void clean_source(char* src) {
    int len = strlen(src);
    while (len > 0 && (src[len - 1] == ' ' || src[len - 1] == '\n' || src[len - 1] == '\r' || src[len - 1] == '\t')) {
        src[len - 1] = '\0';
        len--;
    }
}

int main(int argc, char* argv[]) {
    const char* filename = "start.jminus";
    int debug = 0;

    // Command-line arguments
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--debug") == 0) {
            debug = 1;
        } else {
            filename = argv[i];
        }
    }

    char* source = read_file(filename);
    clean_source(source);

    printf("---- SOURCE START ----\n%s\n---- SOURCE END ----\n", source);

    int token_count;
    Token* tokens = tokenize(source, &token_count);

    if (debug) {
        printf("\n--- Tokens ---\n");
        for (int i = 0; i < token_count; i++) {
            printf("[Line %d] %-15s %s\n", tokens[i].line, token_type_to_string(tokens[i].type), tokens[i].lexeme);
        }
    }

    int stmt_count;
    Stmt** stmts = parse(tokens, token_count, &stmt_count);

    if (debug) {
        printf("\n--- AST ---\n");
        for (int i = 0; i < stmt_count; i++) {
            print_stmt(stmts[i], 0);
        }
    }

    Bytecode* bytecode = compile(stmts, stmt_count);
    
    run(bytecode);

    free_ast(stmts, stmt_count);

    free_tokens(tokens, token_count);
    free_bytecode(bytecode);
    free(source);

    return 0;
}
