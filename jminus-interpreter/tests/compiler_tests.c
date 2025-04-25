// tests/compiler_tests.c

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../lexer.h"
#include "../parser.h"
#include "../compiler.h"

static void assert_bool(int cond, const char* msg) {
    if (!cond) {
        fprintf(stderr, "Assertion failed: %s\n", msg);
        exit(1);
    }
}

int main(void) {
    // --------
    // Test 1: let-statement
    //   let x = 42;
    // --------
    {
        const char* src = "let x = 42;";
        int tcount;
        Token* tokens = tokenize(src, &tcount);
        int scount;
        Stmt** stmts = parse(tokens, tcount, &scount);
        Bytecode* bc = compile(stmts, scount);

        assert_bool(bc->const_count == 1,          "const_count should be 1");
        assert_bool(bc->constants[0] == 42,        "constant[0] should be 42");

        assert_bool(bc->count >= 3,                "should have at least 3 instructions");
        assert_bool(bc->instructions[0].opcode == BC_CONST,   "instr0 must be BC_CONST");
        assert_bool(bc->instructions[0].operand ==  0,        "instr0 operand must be 0");
        assert_bool(bc->instructions[1].opcode == BC_SET_VAR, "instr1 must be BC_SET_VAR");
        assert_bool(bc->instructions[2].opcode == BC_HALT,    "instr2 must be BC_HALT");

        free_bytecode(bc);
        free_tokens(tokens, tcount);
    }

    // --------
    // Test 2: yap-expression
    //   yap(1+2);
    // --------
    {
        const char* src = "yap(1+2);";
        int tcount;
        Token* tokens = tokenize(src, &tcount);
        int scount;
        Stmt** stmts = parse(tokens, tcount, &scount);
        Bytecode* bc = compile(stmts, scount);

        assert_bool(bc->const_count == 2,          "const_count should be 2");
        assert_bool(bc->constants[0] == 1,         "constant[0] should be 1");
        assert_bool(bc->constants[1] == 2,         "constant[1] should be 2");

        // now index the actual instructions:
        assert_bool(bc->instructions[0].opcode == BC_CONST,  "instr0 must be BC_CONST");
        assert_bool(bc->instructions[1].opcode == BC_CONST,  "instr1 must be BC_CONST");
        assert_bool(bc->instructions[2].opcode == BC_ADD,    "instr2 must be BC_ADD");
        assert_bool(bc->instructions[3].opcode == BC_PRINT,  "instr3 must be BC_PRINT");
        assert_bool(bc->instructions[4].opcode == BC_HALT,   "instr4 must be BC_HALT");

        free_bytecode(bc);
        free_tokens(tokens, tcount);
    }

    printf("✅ compiler_tests passed!\n");
    return 0;
}
