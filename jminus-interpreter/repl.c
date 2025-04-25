// repl.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lexer.h"
#include "parser.h"
#include "compiler.h"
#include "vm.h"
#include "interpreter.h"

// --- Color macros ---
#define COLOR_RESET   "\x1b[0m"
#define COLOR_BOLD    "\x1b[1m"
#define COLOR_GREEN   "\x1b[32m"
#define COLOR_YELLOW  "\x1b[33m"
#define COLOR_CYAN    "\x1b[36m"
#define COLOR_RED     "\x1b[31m"

#define LINE_BUF 1024

void print_help() {
    printf(COLOR_CYAN "Available commands:\n" COLOR_GREEN);
    printf("  :help         Show this help message\n");
    printf("  :exit         Exit the REPL\n");
    printf("  :interp;      Interpreter mode\n");
    printf("  :vm;          Compile + VM mode\n");
    printf("  let x = 3;    Declare variables\n");
    printf("  yap(x);       Print variables or expressions\n");
    printf("  Supports: if, while, blocks {}\n\n " COLOR_RESET);
}

void clean_line(char* line) {
    size_t len = strlen(line);
    while (len && (line[len - 1] == '\n' || line[len - 1] == '\r')) {
        line[--len] = '\0';
    }
}

int main(void) {
    char line[LINE_BUF];

    printf(COLOR_GREEN COLOR_BOLD "Welcome to jminus REPL 🚀\n" COLOR_RESET);

    printf(COLOR_CYAN "Type :help for available commands.\n\n" COLOR_RESET);

    while (1) {
        fputs(COLOR_YELLOW "jminus> " COLOR_RESET, stdout);
        if (!fgets(line, LINE_BUF, stdin)) break;

        clean_line(line);
        if (strcmp(line, ":exit") == 0 || strcmp(line, "exit") == 0) break;
        if (strcmp(line, ":help") == 0) {
            print_help();
            continue;
        }
        if (strlen(line) == 0) continue;

        int token_count = 0;
        Token* tokens = tokenize(line, &token_count);

        int stmt_count = 0;
        Stmt** stmts = NULL;

        // parse with basic error handling
        stmts = parse(tokens, token_count, &stmt_count);
        if (!stmts) {
            free_tokens(tokens, token_count);
            continue;
        }

        // interpreter (debug output)
        interpret(stmts, stmt_count);

        // bytecode compiler + VM execution
        Bytecode* bc = compile(stmts, stmt_count);
        run(bc);

        // cleanup
        free_tokens(tokens, token_count);
        free_ast(stmts, stmt_count);
        free_bytecode(bc);
    }

    puts(COLOR_GREEN "\nGoodbye 👋" COLOR_RESET);
    return 0;
}
