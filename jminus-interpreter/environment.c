// environment.c
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "environment.h"

Environment* new_environment(Environment* parent) {
    Environment* env = malloc(sizeof(Environment));
    env->count = 0;
    env->parent = parent;
    return env;
}

void free_environment(Environment* env) {
    free(env);
}

void define_var(Environment* env, const char* name, int value) {
    env->entries[env->count].name = name;
    env->entries[env->count].value = value;
    env->count++;
}

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
