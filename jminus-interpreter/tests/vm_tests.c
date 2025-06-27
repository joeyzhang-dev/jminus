#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include "../compiler.h"
#include "../vm.h"

static int test_output[32];
static int test_output_count = 0;

void test_vm_output(int value) {
    test_output[test_output_count++] = value;
}

static void assert_int(int actual, int expected, const char* msg) {
    if (actual != expected) {
        fprintf(stderr, "❌ Assertion failed: %s (got %d, expected %d)\n", msg, actual, expected);
        exit(1);
    }
}

static void print_pass(const char* msg) {
    printf("✅ %s\n", msg);
}

int main(void) {
    // Set VM output to our test function
    vm_output = test_vm_output;

    // Test 1: Arithmetic and print
    {
        test_output_count = 0;
        Bytecode* bc = malloc(sizeof(Bytecode));
        bc->instructions = malloc(sizeof(Instruction) * 5);
        bc->constants = malloc(sizeof(int) * 2);
        bc->capacity = 5;
        bc->const_capacity = 2;
        bc->count = 0;
        bc->const_count = 0;
        bc->constants[0] = 7;
        bc->constants[1] = 3;
        bc->instructions[0] = (Instruction){BC_CONST, 0}; // push 7
        bc->instructions[1] = (Instruction){BC_CONST, 1}; // push 3
        bc->instructions[2] = (Instruction){BC_ADD, 0};   // add
        bc->instructions[3] = (Instruction){BC_PRINT, 0}; // print
        bc->instructions[4] = (Instruction){BC_HALT, 0};
        bc->count = 5;
        bc->const_count = 2;
        run(bc);
        assert_int(test_output_count, 1, "arithmetic: should print once");
        assert_int(test_output[0], 10, "arithmetic: 7+3 should print 10");
        print_pass("arithmetic and print");
        free_bytecode(bc);
    }

    // Test 2: Variable definition and assignment
    {
        test_output_count = 0;
        Bytecode* bc = malloc(sizeof(Bytecode));
        bc->instructions = malloc(sizeof(Instruction) * 8);
        bc->constants = malloc(sizeof(int) * 2);
        bc->capacity = 8;
        bc->const_capacity = 2;
        bc->count = 0;
        bc->const_count = 0;
        bc->constants[0] = 5;
        bc->constants[1] = 9;
        bc->instructions[0] = (Instruction){BC_CONST, 0}; // push 5
        bc->instructions[1] = (Instruction){BC_DEFINE_VAR, 'x'}; // let x = 5
        bc->instructions[2] = (Instruction){BC_CONST, 1}; // push 9
        bc->instructions[3] = (Instruction){BC_SET_VAR, 'x'}; // x = 9
        bc->instructions[4] = (Instruction){BC_LOAD_VAR, 'x'}; // load x
        bc->instructions[5] = (Instruction){BC_PRINT, 0}; // print x
        bc->instructions[6] = (Instruction){BC_HALT, 0};
        bc->count = 7;
        bc->const_count = 2;
        run(bc);
        assert_int(test_output_count, 1, "var: should print once");
        assert_int(test_output[0], 9, "var: x should be 9 after assignment");
        print_pass("variable define/assign");
        free_bytecode(bc);
    }

    // Test 3: If-statement (simulated)
    {
        test_output_count = 0;
        Bytecode* bc = malloc(sizeof(Bytecode));
        bc->instructions = malloc(sizeof(Instruction) * 8);
        bc->constants = malloc(sizeof(int) * 2);
        bc->capacity = 8;
        bc->const_capacity = 2;
        bc->count = 0;
        bc->const_count = 0;
        bc->constants[0] = 1;
        bc->constants[1] = 42;
        bc->instructions[0] = (Instruction){BC_CONST, 0}; // push 1 (true)
        bc->instructions[1] = (Instruction){BC_JUMP_IF_FALSE, 5}; // if false, jump to 5
        bc->instructions[2] = (Instruction){BC_CONST, 1}; // push 42
        bc->instructions[3] = (Instruction){BC_PRINT, 0}; // print 42
        bc->instructions[4] = (Instruction){BC_JUMP, 6}; // jump to halt (index 6)
        bc->instructions[5] = (Instruction){BC_HALT, 0}; // else branch would start here if present
        bc->instructions[6] = (Instruction){BC_HALT, 0}; // ensure halt at jump target
        bc->count = 7;
        bc->const_count = 2;
        run(bc);
        assert_int(test_output_count, 1, "if: should print once");
        assert_int(test_output[0], 42, "if: should print 42");
        print_pass("if-statement (true branch)");
        free_bytecode(bc);
    }

    printf("\n🎉 All VM tests passed!\n");
    return 0;
}
