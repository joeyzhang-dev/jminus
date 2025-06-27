// tests/compiler_tests.c

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../lexer.h"
#include "../parser.h"
#include "../compiler.h"

static void assert_bool(int cond, const char* msg) {
    if (!cond) {
        fprintf(stderr, "❌ Assertion failed: %s\n", msg);
        exit(1);
    }
}

static void print_pass(const char* msg) {
    printf("✅ %s\n", msg);
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

        assert_bool(bc->const_count == 1,          "let: const_count should be 1");
        assert_bool(bc->constants[0] == 42,        "let: constant[0] should be 42");
        assert_bool(bc->count >= 3,                "let: should have at least 3 instructions");
        assert_bool(bc->instructions[0].opcode == BC_CONST,      "let: instr0 must be BC_CONST");
        assert_bool(bc->instructions[0].operand ==  0,           "let: instr0 operand must be 0");
        assert_bool(bc->instructions[1].opcode == BC_DEFINE_VAR, "let: instr1 must be BC_DEFINE_VAR");
        assert_bool(bc->instructions[2].opcode == BC_HALT,       "let: instr2 must be BC_HALT");
        print_pass("let-statement compiles to BC_DEFINE_VAR");
        free_bytecode(bc);
        free_tokens(tokens, tcount);
    }

    // --------
    // Test 2: assignment
    //   let x = 1; x = 99;
    // --------
    {
        const char* src = "let x = 1; x = 99;";
        int tcount;
        Token* tokens = tokenize(src, &tcount);
        int scount;
        Stmt** stmts = parse(tokens, tcount, &scount);
        Bytecode* bc = compile(stmts, scount);
        // Should have BC_DEFINE_VAR for let, BC_CONST+BC_SET_VAR for assignment
        assert_bool(bc->instructions[1].opcode == BC_DEFINE_VAR, "assign: let uses BC_DEFINE_VAR");
        int found_set = 0;
        for (int i = 0; i < bc->count; i++) {
            if (bc->instructions[i].opcode == BC_SET_VAR) found_set = 1;
        }
        assert_bool(found_set, "assign: assignment uses BC_SET_VAR");
        print_pass("assignment compiles to BC_SET_VAR");
        free_bytecode(bc);
        free_tokens(tokens, tcount);
    }

    // --------
    // Test 3: yap-expression
    //   yap(1+2);
    // --------
    {
        const char* src = "yap(1+2);";
        int tcount;
        Token* tokens = tokenize(src, &tcount);
        int scount;
        Stmt** stmts = parse(tokens, tcount, &scount);
        Bytecode* bc = compile(stmts, scount);
        assert_bool(bc->const_count == 2,          "yap: const_count should be 2");
        assert_bool(bc->constants[0] == 1,         "yap: constant[0] should be 1");
        assert_bool(bc->constants[1] == 2,         "yap: constant[1] should be 2");
        assert_bool(bc->instructions[0].opcode == BC_CONST,  "yap: instr0 must be BC_CONST");
        assert_bool(bc->instructions[1].opcode == BC_CONST,  "yap: instr1 must be BC_CONST");
        assert_bool(bc->instructions[2].opcode == BC_ADD,    "yap: instr2 must be BC_ADD");
        assert_bool(bc->instructions[3].opcode == BC_PRINT,  "yap: instr3 must be BC_PRINT");
        assert_bool(bc->instructions[4].opcode == BC_HALT,   "yap: instr4 must be BC_HALT");
        print_pass("yap-expression compiles to BC_PRINT");
        free_bytecode(bc);
        free_tokens(tokens, tcount);
    }

    // --------
    // Test 4: if-statement
    //   if (1 == 1) { yap(123); } else { yap(456); }
    // --------
    {
        const char* src = "if (1 == 1) { yap(123); } else { yap(456); }";
        int tcount;
        Token* tokens = tokenize(src, &tcount);
        int scount;
        Stmt** stmts = parse(tokens, tcount, &scount);
        Bytecode* bc = compile(stmts, scount);
        int found_jump = 0, found_jump_if_false = 0;
        for (int i = 0; i < bc->count; i++) {
            if (bc->instructions[i].opcode == BC_JUMP) found_jump = 1;
            if (bc->instructions[i].opcode == BC_JUMP_IF_FALSE) found_jump_if_false = 1;
        }
        assert_bool(found_jump, "if: should emit BC_JUMP");
        assert_bool(found_jump_if_false, "if: should emit BC_JUMP_IF_FALSE");
        print_pass("if-statement compiles to jumps");
        free_bytecode(bc);
        free_tokens(tokens, tcount);
    }

    // --------
    // Test 5: while-statement
    //   let x = 0; while (x < 2) { x = x + 1; }
    // --------
    {
        const char* src = "let x = 0; while (x < 2) { x = x + 1; }";
        int tcount;
        Token* tokens = tokenize(src, &tcount);
        int scount;
        Stmt** stmts = parse(tokens, tcount, &scount);
        Bytecode* bc = compile(stmts, scount);
        int found_jump = 0, found_jump_if_false = 0;
        for (int i = 0; i < bc->count; i++) {
            if (bc->instructions[i].opcode == BC_JUMP) found_jump = 1;
            if (bc->instructions[i].opcode == BC_JUMP_IF_FALSE) found_jump_if_false = 1;
        }
        assert_bool(found_jump, "while: should emit BC_JUMP");
        assert_bool(found_jump_if_false, "while: should emit BC_JUMP_IF_FALSE");
        print_pass("while-statement compiles to jumps");
        free_bytecode(bc);
        free_tokens(tokens, tcount);
    }

    printf("\n🎉 All compiler tests passed!\n");
    return 0;
}
