#ifndef INTERPRETER_H
#define INTERPRETER_H

#include "parser.h"

void interpret(Stmt** stmts, int count);

int lookup_variable(const char* name);
void assign_variable(const char* name, int value);
void define_variable(const char* name, int value);

#endif
