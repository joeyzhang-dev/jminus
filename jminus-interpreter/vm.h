/**
 * @file vm.h
 * @brief Virtual machine for executing jminus bytecode
 * @author Joey Zhang
 * @version 1.0.0
 * 
 * The virtual machine executes compiled bytecode instructions.
 * It implements a stack-based architecture where instructions
 * operate on an operand stack and use an environment for variables.
 * 
 * VM Architecture:
 * - Stack-based execution model
 * - Environment-based variable storage
 * - Instruction pointer for program flow
 * - Constants table for literal values
 * - Configurable output function for testing
 * 
 * Execution Model:
 * The VM uses a simple fetch-decode-execute cycle:
 * 1. Fetch: Read instruction at current instruction pointer
 * 2. Decode: Determine operation and operands
 * 3. Execute: Perform operation on stack/environment
 * 4. Advance: Move to next instruction (unless jumped)
 * 
 * Stack Operations:
 * - Values are pushed onto and popped from the operand stack
 * - Binary operations consume two operands and produce one result
 * - Stack underflow is prevented by instruction validation
 * - Stack overflow is prevented by fixed stack size
 * 
 * Variable Management:
 * - Variables are stored in an environment structure
 * - Single-character variable names (ASCII codes)
 * - Support for variable definition and assignment
 * - Environment lookup with parent chain support
 * 
 * Control Flow:
 * - Absolute jumps for if statements and loops
 * - Conditional jumps based on stack values
 * - Forward and backward jump support
 * - Jump target validation
 * 
 * Error Handling:
 * - Invalid opcodes cause immediate termination
 * - Stack underflow is detected and reported
 * - Variable lookup errors are handled gracefully
 * - Division by zero protection
 */

#ifndef VM_H
#define VM_H

#include "compiler.h"

/**
 * @brief Executes compiled bytecode on the virtual machine
 * @param bytecode The compiled program to execute
 * 
 * This function is the main entry point for VM execution. It:
 * 1. Initializes the VM state (stack, environment, instruction pointer)
 * 2. Enters the main execution loop
 * 3. Fetches, decodes, and executes each instruction
 * 4. Handles control flow and variable operations
 * 5. Produces program output via the configured output function
 * 
 * Execution Process:
 * - Each instruction is processed in sequence
 * - Stack operations maintain proper operand order
 * - Variable operations use the environment system
 * - Control flow instructions modify the instruction pointer
 * - Program terminates when BC_HALT is encountered
 * 
 * Stack Behavior:
 * - Operands are pushed in left-to-right order
 * - Binary operations pop operands in reverse order (right, then left)
 * - Results are pushed back onto the stack
 * - Stack state is maintained across instruction boundaries
 * 
 * Variable Operations:
 * - BC_LOAD_VAR: Pushes variable value onto stack
 * - BC_SET_VAR: Stores top stack value into existing variable
 * - BC_DEFINE_VAR: Creates new variable with top stack value
 * - Variable names are single ASCII characters
 * 
 * Control Flow:
 * - BC_JUMP: Sets instruction pointer to absolute target
 * - BC_JUMP_IF_FALSE: Jumps only if top stack value is false (0)
 * - Jump targets are validated to prevent out-of-bounds access
 * - Loops use backward jumps, conditionals use forward jumps
 * 
 * Output Handling:
 * - BC_PRINT uses the configured output function
 * - Default output function prints to stdout
 * - Tests can override output function for verification
 * - Output function receives the value to print
 * 
 * Error Conditions:
 * - Unknown opcodes cause immediate termination
 * - Stack underflow is detected and reported
 * - Invalid jump targets are prevented
 * - Division by zero is handled gracefully
 * 
 * Performance Characteristics:
 * - O(1) stack operations (push/pop)
 * - O(n) variable lookup in environment
 * - O(1) instruction dispatch
 * - Linear time complexity for most operations
 */
void run(Bytecode* bytecode);

/**
 * @brief Function pointer for VM output operations
 * 
 * This function pointer controls how the VM handles BC_PRINT instructions.
 * By default, it points to a function that prints to stdout.
 * Tests can override this to capture output for verification.
 * 
 * Function Signature:
 * - Parameter: int value - The value to output
 * - Return: void - No return value needed
 * 
 * Usage:
 * - Default: Prints values to stdout with newline
 * - Testing: Captures values in test arrays
 * - Custom: Can be set to any output function
 * 
 * Thread Safety:
 * - This is a global variable, not thread-safe
 * - Should be set once at program start
 * - Concurrent access should be avoided
 */
extern void (*vm_output)(int value);

#endif // VM_H 