/**
 * @file interpreter.h
 * @brief Direct AST interpreter for the jminus language
 * @author Joey Zhang
 * @version 1.0.0
 *
 * The interpreter executes the Abstract Syntax Tree (AST) directly,
 * without compiling to bytecode. It is useful for debugging, REPL,
 * and educational purposes.
 *
 * Interpreter Features:
 * - Walks the AST recursively
 * - Evaluates expressions and executes statements
 * - Manages variables using an environment
 * - Supports let, assignment, if, while, blocks, and yap
 *
 * Variable Management:
 * - Variables are stored in an environment structure
 * - Functions provided for lookup, assignment, and definition
 * - Global environment is used for all variables (no local scopes yet)
 *
 * Error Handling:
 * - Undefined variable access triggers error and exit
 * - Invalid assignments are detected
 * - All errors are reported with descriptive messages
 */

#ifndef INTERPRETER_H
#define INTERPRETER_H

#include "parser.h"

/**
 * @brief Executes an array of AST statements directly
 * @param stmts Array of statement nodes to execute
 * @param count Number of statements in the array
 *
 * This function walks the AST and executes each statement in order.
 * It handles all statement types (let, yap, if, while, block, expr).
 *
 * Memory Management:
 * - Does not free the AST; caller is responsible
 *
 * Error Handling:
 * - Exits on runtime errors (undefined variable, etc.)
 */
void interpret(Stmt** stmts, int count);

/**
 * @brief Looks up the value of a variable by name
 * @param name Variable name (null-terminated string)
 * @return Value of the variable
 *
 * Exits with error if variable is not defined.
 */
int lookup_variable(const char* name);

/**
 * @brief Assigns a value to an existing variable
 * @param name Variable name (null-terminated string)
 * @param value Value to assign
 *
 * Exits with error if variable is not defined.
 */
void assign_variable(const char* name, int value);

/**
 * @brief Defines a new variable with a value
 * @param name Variable name (null-terminated string)
 * @param value Initial value
 *
 * If the variable already exists, its value is updated.
 */
void define_variable(const char* name, int value);

#endif 