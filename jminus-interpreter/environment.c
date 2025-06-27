/**
 * @file environment.c
 * @brief Variable environment and scope management for jminus
 * @author Joey Zhang
 * @version 1.0.0
 *
 * This file implements a simple environment system for variable storage.
 * Environments support nested scopes via parent pointers, enabling block scoping.
 *
 * Features:
 * - Fixed-size array for variable entries (128 per environment)
 * - Linear search for variable lookup and assignment
 * - Variable names are duplicated (strdup) for safe storage
 * - Parent pointer enables lexical scoping (block scope)
 *
 * Error Handling:
 * - Lookup/assignment of undefined variables triggers error and exit
 * - Variable redefinition in the same scope updates the value
 * - All errors are reported with descriptive messages
 */

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "environment.h"

/**
 * @brief Creates a new environment with an optional parent
 * @param parent Pointer to parent environment (NULL for global)
 * @return Pointer to the new environment (caller must free)
 */
Environment* new_environment(Environment* parent) {
    Environment* env = malloc(sizeof(Environment));
    env->count = 0;
    env->parent = parent;
    return env;
}

/**
 * @brief Frees all memory used by an environment
 * @param env Pointer to the environment to free
 *
 * Only frees the environment structure itself (not parent).
 * Variable names are not freed (assumed to be managed elsewhere).
 */
void free_environment(Environment* env) {
    free(env);
}

/**
 * @brief Defines a new variable in the environment
 * @param env Pointer to the environment
 * @param name Variable name (string is duplicated)
 * @param value Initial value
 *
 * If the variable already exists in this scope, its value is updated.
 */
void define_var(Environment* env, const char* name, int value) {
    // Check if variable already exists in this scope
    for (int i = env->count - 1; i >= 0; i--) {
        if (strcmp(env->entries[i].name, name) == 0) {
            // Variable exists, just update its value
            env->entries[i].value = value;
            return;
        }
    }
    // Variable doesn't exist, add it to this scope
    env->entries[env->count].name = strdup(name);  // Duplicate the name
    env->entries[env->count].value = value;
    env->count++;
}

/**
 * @brief Looks up the value of a variable by name
 * @param env Pointer to the environment
 * @param name Variable name (null-terminated string)
 * @return Value of the variable
 *
 * Searches the current environment and all parents.
 * Exits with error if variable is not found.
 */
int lookup_var(Environment* env, const char* name) {
    for (int i = env->count - 1; i >= 0; i--) {
        if (strcmp(env->entries[i].name, name) == 0) {
            return env->entries[i].value;
        }
    }
    if (env->parent) {
        return lookup_var(env->parent, name);
    }
    fprintf(stderr, "Undefined variable: %s\n", name);
    exit(1);
}

/**
 * @brief Assigns a value to an existing variable
 * @param env Pointer to the environment
 * @param name Variable name (null-terminated string)
 * @param value Value to assign
 *
 * Searches the current environment and all parents.
 * Exits with error if variable is not found.
 */
void assign_var(Environment* env, const char* name, int value) {
    for (int i = env->count - 1; i >= 0; i--) {
        if (strcmp(env->entries[i].name, name) == 0) {
            env->entries[i].value = value;
            return;
        }
    }
    if (env->parent) {
        assign_var(env->parent, name, value);
        return;
    }
    fprintf(stderr, "Undefined variable: %s\n", name);
    exit(1);
} 