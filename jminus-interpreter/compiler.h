/**
 * @file compiler.h
 * @brief Bytecode compiler for the jminus language
 * @author Joey Zhang
 * @version 1.0.0
 * 
 * The compiler translates an Abstract Syntax Tree (AST) into bytecode
 * instructions that can be executed by the virtual machine.
 * 
 * Compiler Architecture:
 * - Tree-walking code generator
 * - Stack-based instruction set
 * - Constant folding and optimization
 * - Jump target resolution for control flow
 * 
 * Bytecode Design:
 * The bytecode uses a stack-based architecture where:
 * - Instructions operate on an operand stack
 * - Values are pushed onto and popped from the stack
 * - Binary operations consume two operands and produce one result
 * - Control flow uses absolute jump targets
 * 
 * Instruction Format:
 * Each instruction consists of:
 * - Opcode: The operation to perform
 * - Operand: Additional data (constant index, jump target, etc.)
 * 
 * Compilation Process:
 * 1. Traverse AST nodes in post-order
 * 2. Generate instructions for each node type
 * 3. Resolve jump targets for control flow
 * 4. Build constant table for literals
 * 5. Produce linear bytecode sequence
 * 
 * Memory Management:
 * - Bytecode structures are dynamically allocated
 * - Constant table grows as needed
 * - Instruction array expands automatically
 * - All memory is properly freed on cleanup
 */

#ifndef COMPILER_H
#define COMPILER_H

#include "parser.h"

/**
 * @brief Enumeration of all bytecode instruction types
 * 
 * Instructions are organized by category:
 * - Constants: Loading literal values
 * - Arithmetic: Mathematical operations
 * - Variables: Loading and storing variables
 * - Control Flow: Jumps and conditional execution
 * - I/O: Input and output operations
 * - Stack: Stack manipulation
 * - Special: Program termination
 */
typedef enum {
    // Constants - Loading literal values
    BC_CONST,        ///< Push constant from constants table
    
    // Arithmetic Operations - Mathematical computations
    BC_ADD,          ///< Add top two stack values: b + a
    BC_SUB,          ///< Subtract top two stack values: b - a
    BC_MUL,          ///< Multiply top two stack values: b * a
    BC_DIV,          ///< Divide top two stack values: b / a
    
    // I/O Operations - Input and output
    BC_PRINT,        ///< Print top stack value to stdout
    
    // Variable Operations - Loading and storing variables
    BC_LOAD_VAR,     ///< Load variable value onto stack
    BC_SET_VAR,      ///< Store top stack value into variable
    BC_DEFINE_VAR,   ///< Define new variable with top stack value
    BC_STORE_VAR,    ///< Alias for BC_SET_VAR (legacy)
    
    // Comparison Operations - Boolean comparisons
    BC_EQUAL,        ///< Compare top two values: b == a
    BC_NOT_EQUAL,    ///< Compare top two values: b != a
    BC_LESS,         ///< Compare top two values: b < a
    BC_LESS_EQUAL,   ///< Compare top two values: b <= a
    BC_GREATER,      ///< Compare top two values: b > a
    BC_GREATER_EQUAL, ///< Compare top two values: b >= a
    
    // Constants (legacy)
    BC_LOAD_CONST,   ///< Alias for BC_CONST (legacy)
    
    // Control Flow - Jumps and conditional execution
    BC_JUMP_IF_FALSE, ///< Jump if top stack value is false
    BC_JUMP,         ///< Unconditional jump to target
    BC_LOOP,         ///< Jump back to loop start (legacy)
    
    // Stack Operations - Stack manipulation
    BC_POP,          ///< Remove top value from stack
    
    // Special - Program control
    BC_HALT          ///< Stop program execution
} OpCode;

/**
 * @brief Represents a single bytecode instruction
 * 
 * Each instruction consists of an opcode and an optional operand.
 * The operand meaning depends on the opcode:
 * - BC_CONST: Index into constants table
 * - BC_LOAD_VAR/BC_SET_VAR/BC_DEFINE_VAR: Variable name (ASCII code)
 * - BC_JUMP/BC_JUMP_IF_FALSE: Target instruction index
 * - Other instructions: Unused (typically 0)
 */
typedef struct {
    OpCode opcode; ///< The operation to perform
    int operand;   ///< Additional data for the operation
} Instruction;

/**
 * @brief Complete bytecode representation of a compiled program
 * 
 * The Bytecode structure contains all information needed to execute
 * a compiled program, including instructions and constant values.
 * 
 * Memory Layout:
 * - Instructions are stored in a linear array
 * - Constants are stored in a separate table
 * - Both arrays grow dynamically as needed
 * - Capacity fields track allocated space
 * - Count fields track actual usage
 */
typedef struct {
    Instruction* instructions; ///< Array of bytecode instructions
    int count;                 ///< Number of instructions
    int capacity;              ///< Allocated instruction capacity
    
    int* constants;            ///< Table of constant values
    int const_count;           ///< Number of constants
    int const_capacity;        ///< Allocated constant capacity
} Bytecode;

/**
 * @brief Compiles an Abstract Syntax Tree into bytecode
 * @param stmts Array of statement nodes to compile
 * @param stmt_count Number of statements to compile
 * @return Compiled bytecode (caller must free with free_bytecode())
 * 
 * This function is the main entry point for compilation. It:
 * 1. Initializes the bytecode structure
 * 2. Compiles each statement in the AST
 * 3. Resolves jump targets for control flow
 * 4. Builds the constant table
 * 5. Returns the complete bytecode
 * 
 * Compilation Strategy:
 * - Tree-walking approach: traverse AST nodes
 * - Post-order evaluation: compile children before parents
 * - Stack-based code generation: push operands, apply operations
 * - Forward references: resolve jump targets after generation
 * 
 * Instruction Generation:
 * - Expressions: Generate code to compute values on stack
 * - Statements: Generate code for side effects and control flow
 * - Variables: Use ASCII codes for single-character names
 * - Constants: Store in table, reference by index
 * 
 * Control Flow:
 * - If statements: conditional jumps around branches
 * - While loops: conditional jumps with backward references
 * - Blocks: sequential execution of contained statements
 * 
 * Memory Management:
 * - Returns dynamically allocated Bytecode structure
 * - Caller is responsible for freeing with free_bytecode()
 * - All internal arrays are properly allocated
 * 
 * Error Handling:
 * - Compilation errors are reported to stderr
 * - Invalid constructs cause compilation to fail
 * - Returns NULL if compilation fails completely
 */
Bytecode* compile(Stmt** stmts, int stmt_count);

/**
 * @brief Frees all memory allocated for bytecode
 * @param bytecode The bytecode structure to free
 * 
 * This function deallocates all memory used by the bytecode:
 * - Instruction array
 * - Constants table
 * - Bytecode structure itself
 * 
 * Memory is freed in the correct order to avoid
 * accessing freed memory during cleanup.
 */
void free_bytecode(Bytecode* bytecode);

#endif // COMPILER_H 