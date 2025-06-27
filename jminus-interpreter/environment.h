/**
 * @file environment.h
 * @brief Variable environment and scope management for jminus
 * @author Joey Zhang
 * @version 1.0.0
 *
 * The environment module provides a simple variable storage system
 * with support for nested scopes (parent environments).
 *
 * Environment Features:
 * - Stores variable name/value pairs
 * - Supports variable definition, lookup, and assignment
 * - Allows for nested environments (block scopes, future extensions)
 * - Used by both the interpreter and VM
 *
 * Design Decisions:
 * - Fixed-size array for variable storage (128 entries)
 * - Linear search for variable lookup (sufficient for small programs)
 * - Variable names are stored as dynamically allocated strings
 * - Parent pointer enables lexical scoping (block scope)
 *
 * Error Handling:
 * - Lookup/assignment of undefined variables triggers error and exit
 * - Variable redefinition in the same scope updates the value
 * - All errors are reported with descriptive messages
 */

#ifndef ENVIRONMENT_H
#define ENVIRONMENT_H

#include "parser.h"

/**
 * @brief Represents a single variable entry in the environment
 *
 * Each entry stores a variable name and its integer value.
 */
typedef struct EnvEntry {
    const char* name; ///< Variable name (dynamically allocated string)
    int value;       ///< Variable value
} EnvEntry;

/**
 * @brief Represents a variable environment (scope)
 *
 * The environment contains an array of variable entries and a pointer
 * to a parent environment (for nested scopes).
 */
typedef struct Environment {
    EnvEntry entries[128]; ///< Array of variable entries (fixed size)
    int count;            ///< Number of variables in this environment
    struct Environment* parent; ///< Pointer to parent environment (NULL for global)
} Environment;

/**
 * @brief Creates a new environment with an optional parent
 * @param parent Pointer to parent environment (NULL for global)
 * @return Pointer to the new environment (caller must free)
 */
Environment* new_environment(Environment* parent);

/**
 * @brief Frees all memory used by an environment
 * @param env Pointer to the environment to free
 *
 * Only frees the environment structure itself (not parent).
 * Variable names are not freed (assumed to be managed elsewhere).
 */
void free_environment(Environment* env);

/**
 * @brief Defines a new variable in the environment
 * @param env Pointer to the environment
 * @param name Variable name (string is duplicated)
 * @param value Initial value
 *
 * If the variable already exists in this scope, its value is updated.
 */
void define_var(Environment* env, const char* name, int value);

/**
 * @brief Looks up the value of a variable by name
 * @param env Pointer to the environment
 * @param name Variable name (null-terminated string)
 * @return Value of the variable
 *
 * Searches the current environment and all parents.
 * Exits with error if variable is not found.
 */
int lookup_var(Environment* env, const char* name);

/**
 * @brief Assigns a value to an existing variable
 * @param env Pointer to the environment
 * @param name Variable name (null-terminated string)
 * @param value Value to assign
 *
 * Searches the current environment and all parents.
 * Exits with error if variable is not found.
 */
void assign_var(Environment* env, const char* name, int value);

#endif 