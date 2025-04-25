// tests/lexer_tests.c

#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "../lexer.h"

int main(void) {
    const char* src = "let foo = 123 + bar; yap(foo)";
    int count;
    Token* tokens = tokenize(src, &count);

    // What we expect, in order:
    TokenType expected_types[] = {
        TOKEN_LET,
        TOKEN_IDENTIFIER,
        TOKEN_ASSIGN,
        TOKEN_INT,
        TOKEN_PLUS,
        TOKEN_IDENTIFIER,
        TOKEN_SEMICOLON,
        TOKEN_YAP,
        TOKEN_LPAREN,
        TOKEN_IDENTIFIER,
        TOKEN_RPAREN,
        TOKEN_EOF
    };
    const char* expected_lexemes[] = {
        "let",
        "foo",
        "=",
        "123",
        "+",
        "bar",
        ";",
        "yap",
        "(",
        "foo",
        ")",
        ""    // EOF has empty lexeme
    };

    int expected_count = sizeof(expected_types) / sizeof(expected_types[0]);
    assert(count == expected_count && "token count mismatch");

    for (int i = 0; i < expected_count; i++) {
        assert(tokens[i].type == expected_types[i] &&
               "token type mismatch");
        assert(strcmp(tokens[i].lexeme, expected_lexemes[i]) == 0 &&
               "token lexeme mismatch");
    }

    free_tokens(tokens, count);
    printf("✅ lexer_tests passed\n");
    return 0;
}
