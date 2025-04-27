# Makefile for jminus project

# Compiler
CC = gcc

# Compiler flags
CFLAGS = -std=c99 -Wall -Wextra -g

# Source files
SRC = main.c lexer.c parser.c compiler.c vm.c interpreter.c environment.c

# REPL source
REPL_SRC = repl.c lexer.c parser.c compiler.c vm.c interpreter.c environment.c

# Executable names
MAIN_EXE = jminus.exe
REPL_EXE = jminus-repl.exe

# Default target
all: $(MAIN_EXE) $(REPL_EXE)

# Build main executable (runs start.jminus files)
$(MAIN_EXE): $(SRC)
	$(CC) $(CFLAGS) $(SRC) -o $(MAIN_EXE)

# Build REPL
$(REPL_EXE): $(REPL_SRC)
	$(CC) $(CFLAGS) $(REPL_SRC) -o $(REPL_EXE)

# Clean build artifacts
clean:
	rm -f $(MAIN_EXE) $(REPL_EXE)

# Run tests (using your test script)
test:
	./scripts/run_tests.sh

# Convenience targets
rebuild: clean all

.PHONY: all clean test rebuild
