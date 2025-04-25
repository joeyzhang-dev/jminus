#include <stdio.h>
#include <stdlib.h>
#include "lexer.h"
#include "parser.h"
#include "interpreter.h"


// Reads the entire contents of a file into a string
char* read_file(const char* filename) {
    FILE* file = fopen(filename, "r");
    if (!file) {
        perror("Failed to open file");
        exit(EXIT_FAILURE);
    }

    fseek(file, 0, SEEK_END);
    long length = ftell(file);
    rewind(file);

    char* buffer = malloc(length + 1);
    if (!buffer) {
        perror("Failed to allocate buffer");
        exit(EXIT_FAILURE);
    }

    fread(buffer, 1, length, file);
    buffer[length] = '\0';

    fclose(file);
    return buffer;
}

int main(int argc, char* argv[]) {
    const char* filename = "start.jminus";
    if (argc > 1) filename = argv[1];

    char* source = read_file(filename);
    printf("---- SOURCE START ----\n%s\n---- SOURCE END ----\n", source);

    int token_count;
    Token* tokens = tokenize(source, &token_count);

    int stmt_count;
    for (int i = 0; i < token_count; i++) {
        printf("[Line %d] %-15s %s\n", tokens[i].line, token_type_to_string(tokens[i].type), tokens[i].lexeme);
    }
    
    Stmt** stmts = parse(tokens, token_count, &stmt_count);
    interpret(stmts, stmt_count);

    for (int i = 0; i < stmt_count; i++) {
        print_stmt(stmts[i], 0);  // 🔥 AST printer
    }

    free_tokens(tokens, token_count);
    free(source);
    return 0;
}
