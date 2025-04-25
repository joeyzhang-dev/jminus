// environment.h
#ifndef ENVIRONMENT_H
#define ENVIRONMENT_H

#include "parser.h"

typedef struct EnvEntry {
    const char* name;
    int value;
} EnvEntry;

typedef struct Environment {
    EnvEntry entries[128];
    int count;
    struct Environment* parent;
} Environment;

Environment* new_environment(Environment* parent);
void free_environment(Environment* env);

void define_var(Environment* env, const char* name, int value);
int lookup_var(Environment* env, const char* name);
void assign_var(Environment* env, const char* name, int value);

#endif
