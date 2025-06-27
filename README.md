# jminus - A Simple C-Based Interpreter and Virtual Machine

[![Build Status](https://img.shields.io/badge/Build-Passing-brightgreen?style=flat-square)]()
[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg?style=flat-square)]()
[![Language: C](https://img.shields.io/badge/language-C-blue?style=flat-square)]()
[![Tests](https://img.shields.io/badge/Tests-Passing-brightgreen?style=flat-square)]()
[![Version](https://img.shields.io/badge/Version-1.0.0-blueviolet?style=flat-square)]()

---

## 📋 Table of Contents

- [📚 About jminus](#-about-jminus)
  - [🎯 What You'll Learn](#-what-youll-learn)
  - [🏗 Architecture Overview](#-architecture-overview)
- [🚀 Quick Start](#-quick-start)
  - [Prerequisites](#prerequisites)
  - [1. Clone and Build](#1-clone-and-build)
  - [2. Try the REPL](#2-try-the-repl)
  - [3. Run a Program File](#3-run-a-program-file)
  - [4. Run Tests](#4-run-tests)
- [📖 Language Features](#-language-features)
  - [Variables and Assignment](#variables-and-assignment)
  - [Arithmetic Operations](#arithmetic-operations)
  - [Comparison Operators](#comparison-operators)
  - [Output](#output)
  - [Control Flow](#control-flow)
    - [If Statements](#if-statements)
    - [While Loops](#while-loops)
  - [Blocks and Scope](#blocks-and-scope)
- [📁 Project Structure](#-project-structure)
  - [Core Components](#core-components)
- [🔧 Development Guide](#-development-guide)
  - [Building from Source](#building-from-source)
  - [Running Tests](#running-tests)
  - [Debug Mode](#debug-mode)
  - [REPL Commands](#repl-commands)
- [🏗 Architecture Deep Dive](#-architecture-deep-dive)
  - [Lexical Analysis](#lexical-analysis)
  - [Parsing](#parsing)
  - [Code Generation](#code-generation)
  - [Virtual Machine](#virtual-machine)
  - [Bytecode Instructions](#bytecode-instructions)
- [🧪 Testing](#-testing)
  - [Test Philosophy](#test-philosophy)
  - [Running Tests](#running-tests-1)
  - [Adding Tests](#adding-tests)
- [🐛 Debugging](#-debugging)
  - [Common Issues](#common-issues)
  - [Debug Tools](#debug-tools)
  - [Debug Output](#debug-output)
- [🤝 Contributing](#-contributing)
  - [Getting Started](#getting-started)
  - [Development Workflow](#development-workflow)
  - [Code Style](#code-style)
  - [Areas for Contribution](#areas-for-contribution)
- [📄 License](#-license)
- [🙏 Acknowledgments](#-acknowledgments)
- [📞 Support](#-support)

---

## 📚 About jminus

**jminus** is a small, educational programming language designed to demonstrate compiler construction principles. It features a complete toolchain from source code to execution, including:

- **Lexical Analysis**: Custom tokenizer that converts source code into tokens
- **Parsing**: Recursive descent parser that builds an Abstract Syntax Tree (AST)
- **Code Generation**: Bytecode compiler that translates AST into stack-based instructions
- **Execution**: Virtual machine that interprets bytecode, plus a direct interpreter mode
- **REPL**: Interactive shell for quick testing and experimentation

### 🎯 What You'll Learn

This project demonstrates:
- **Compiler phases** (lexing → parsing → code generation → execution)
- **Stack-based virtual machine** architecture
- **Manual memory management** in C (malloc/free)
- **Variable scope handling** with environment chains
- **Error handling** and diagnostics
- **Testing strategies** for language implementations

### 🏗 Architecture Overview

```
Source Code → Lexer → Parser → AST → Compiler → Bytecode → VM → Output
                                    ↓
                              Interpreter → Output
```

---

## 🚀 Quick Start

### Prerequisites
- **GCC** or **Clang** compiler
- **Make** build system
- **Bash** shell (for running tests)

### 1. Clone and Build

```bash
git clone https://github.com/joeyzhang-dev/jminus.git
cd jminus/jminus-interpreter
make
```

### 2. Try the REPL

```bash
./jminus-repl.exe
```

Example session:
```
Welcome to jminus REPL 🚀
Type :help for available commands.

jminus> let x = 5;
jminus> yap(x);
5
jminus> :interp
Switched to interpreter mode
jminus> let y = 10;
Defined variable y = 10
jminus> yap(y);
Yap output: 10
jminus> :exit
Goodbye 👋
```

### 3. Run a Program File

```bash
./jminus.exe start.jminus
```

### 4. Run Tests

```bash
make test
```

Expected output:
```
=== Compiling and running all tests ===

[*] Building compiler_tests...
[*] Running compiler_tests...
✅ let-statement compiles to BC_DEFINE_VAR
✅ assignment compiles to BC_SET_VAR
✅ yap-expression compiles to BC_PRINT
✅ if-statement compiles to jumps
✅ while-statement compiles to jumps

🎉 All compiler tests passed!

[*] Building lexer_tests...
[*] Running lexer_tests...
✅ lexer_tests passed

[*] Building parser_tests...
[*] Running parser_tests...
✅ parser_tests passed

[*] Building vm_tests...
[*] Running vm_tests...
✅ arithmetic and print
✅ variable define/assign
✅ if-statement (true branch)

🎉 All VM tests passed!

=== All tests passed! ===
```

---

## 📖 Language Features

### Variables and Assignment

```jminus
// Variable declaration
let x = 42;

// Variable assignment
x = 99;

// Multiple variables
let a = 1;
let b = 2;
let c = a + b;
```

### Arithmetic Operations

```jminus
let x = 10;
let y = 3;

// Basic arithmetic
let sum = x + y;      // 13
let diff = x - y;     // 7
let product = x * y;  // 30
let quotient = x / y; // 3
```

### Comparison Operators

```jminus
let a = 5;
let b = 10;

// Comparisons
let eq = a == b;      // false (0)
let ne = a != b;      // true (1)
let lt = a < b;       // true (1)
let le = a <= b;      // true (1)
let gt = a > b;       // false (0)
let ge = a >= b;      // false (0)
```

### Output

```jminus
// Print values
yap(42);           // prints: 42
yap(x + y);        // prints: 13
yap(a == b);       // prints: 0 (false)
```

### Control Flow

#### If Statements

```jminus
let x = 10;

if (x > 5) {
    yap(1);  // prints: 1
} else {
    yap(0);
}

// Nested if
if (x > 15) {
    yap("big");
} else if (x > 5) {
    yap("medium");  // prints: medium
} else {
    yap("small");
}
```

#### While Loops

```jminus
let i = 0;
while (i < 3) {
    yap(i);
    i = i + 1;
}
// Output:
// 0
// 1
// 2
```

### Blocks and Scope

```jminus
let x = 1;
yap(x);  // prints: 1

{
    let x = 2;  // shadows outer x
    yap(x);     // prints: 2
}

yap(x);  // prints: 1 (outer x is back in scope)
```

---

## 📁 Project Structure

```
jminus-interpreter/
├── main.c                 # Entry point for file execution
├── repl.c                 # Interactive REPL shell
├── lexer.c/h             # Tokenization (source → tokens)
├── parser.c/h            # Parsing (tokens → AST)
├── compiler.c/h          # Code generation (AST → bytecode)
├── vm.c/h                # Virtual machine (bytecode → execution)
├── interpreter.c/h       # Direct interpretation (AST → execution)
├── environment.c/h       # Variable scope and environments
├── start.jminus          # Example program file
├── Makefile              # Build configuration
├── scripts/
│   └── run_tests.sh      # Test runner
└── tests/
    ├── compiler_tests.c  # Compiler unit tests
    ├── lexer_tests.c     # Lexer unit tests
    ├── parser_tests.c    # Parser unit tests
    └── vm_tests.c        # VM unit tests
```

### Core Components

| Component | Purpose | Key Files |
|-----------|---------|-----------|
| **Lexer** | Converts source code into tokens | `lexer.c/h` |
| **Parser** | Builds Abstract Syntax Tree | `parser.c/h` |
| **Compiler** | Generates bytecode from AST | `compiler.c/h` |
| **VM** | Executes bytecode instructions | `vm.c/h` |
| **Interpreter** | Direct AST execution | `interpreter.c/h` |
| **Environment** | Variable scope management | `environment.c/h` |

---

## 🔧 Development Guide

### Building from Source

```bash
# Clean build
make clean
make

# Rebuild everything
make rebuild

# Build with debug symbols
make CFLAGS="-g -O0"
```

### Running Tests

```bash
# Run all tests
make test

# Run specific test
./build/tests/compiler_tests.exe
./build/tests/vm_tests.exe
```

### Debug Mode

```bash
# Run with debug output
./jminus.exe --debug start.jminus
```

This shows:
- Source code
- Token stream
- Abstract Syntax Tree
- Execution output

### REPL Commands

| Command | Description |
|---------|-------------|
| `:help` | Show available commands |
| `:exit` | Exit the REPL |
| `:interp` | Switch to interpreter mode |
| `:vm` | Switch to VM mode (default) |

---

## 🏗 Architecture Deep Dive

### Lexical Analysis

The lexer (`lexer.c`) scans source code character by character, recognizing:
- **Keywords**: `let`, `if`, `else`, `while`, `yap`
- **Identifiers**: Variable names
- **Literals**: Numbers (integers only)
- **Operators**: `+`, `-`, `*`, `/`, `=`, `==`, `!=`, `<`, `<=`, `>`, `>=`
- **Delimiters**: `(`, `)`, `{`, `}`, `;`

### Parsing

The parser (`parser.c`) uses recursive descent parsing to build an AST:
- **Statements**: `let`, `yap`, `if`, `while`, blocks, expressions
- **Expressions**: literals, variables, binary operations
- **Error recovery**: Graceful handling of syntax errors

### Code Generation

The compiler (`compiler.c`) translates AST nodes into bytecode:
- **Constants**: Stored in a constants table
- **Variables**: Single-character names (ASCII codes)
- **Control flow**: Jump instructions for if/while
- **Stack operations**: Push, pop, arithmetic

### Virtual Machine

The VM (`vm.c`) executes bytecode using a stack-based architecture:
- **Stack**: Operand stack for calculations
- **Environment**: Variable storage and lookup
- **Instruction pointer**: Current execution position
- **Constants table**: Access to literal values

### Bytecode Instructions

| Instruction | Description | Operand |
|-------------|-------------|---------|
| `BC_CONST` | Push constant | Index into constants table |
| `BC_ADD` | Add top two stack values | None |
| `BC_SUB` | Subtract top two stack values | None |
| `BC_MUL` | Multiply top two stack values | None |
| `BC_DIV` | Divide top two stack values | None |
| `BC_PRINT` | Print top stack value | None |
| `BC_LOAD_VAR` | Load variable value | Variable name (ASCII) |
| `BC_SET_VAR` | Assign to variable | Variable name (ASCII) |
| `BC_DEFINE_VAR` | Define new variable | Variable name (ASCII) |
| `BC_JUMP` | Unconditional jump | Target instruction index |
| `BC_JUMP_IF_FALSE` | Conditional jump | Target instruction index |
| `BC_HALT` | Stop execution | None |

---

## 🧪 Testing

### Test Philosophy

- **Unit tests** for each component (lexer, parser, compiler, VM)
- **Integration tests** for complete programs
- **Clear feedback** with descriptive pass/fail messages
- **Comprehensive coverage** of language features

### Running Tests

```bash
# All tests
make test

# Individual test suites
./build/tests/compiler_tests.exe
./build/tests/lexer_tests.exe
./build/tests/parser_tests.exe
./build/tests/vm_tests.exe
```

### Adding Tests

1. **Compiler tests**: Add to `tests/compiler_tests.c`
2. **VM tests**: Add to `tests/vm_tests.c`
3. **Lexer/Parser tests**: Add to respective test files

Test structure:
```c
// Test description
{
    const char* src = "let x = 42;";
    // ... test setup ...
    // ... assertions ...
    print_pass("test description");
}
```

---

## 🐛 Debugging

### Common Issues

1. **Memory leaks**: Use `valgrind` or address sanitizer
2. **Segmentation faults**: Check array bounds and null pointers
3. **Incorrect output**: Verify bytecode generation and VM execution

### Debug Tools

```bash
# Build with debug symbols
make CFLAGS="-g -O0"

# Run with debug output
./jminus.exe --debug program.jminus

# Use gdb for debugging
gdb ./jminus.exe
```

### Debug Output

The `--debug` flag shows:
```
---- SOURCE START ----
let x = 42;
yap(x);
---- SOURCE END ----

--- Tokens ---
[Line 1] LET             let
[Line 1] IDENTIFIER      x
[Line 1] ASSIGN          =
[Line 1] INT             42
[Line 1] SEMICOLON       ;
[Line 2] YAP             yap
[Line 2] LPAREN          (
[Line 2] IDENTIFIER      x
[Line 2] RPAREN          )
[Line 2] SEMICOLON       ;
[Line 2] EOF             

--- AST ---
LetStmt: x
  Literal: 42
YapStmt:
  Variable: x

42
```

---

## 🤝 Contributing

### Getting Started

1. **Fork** the repository
2. **Clone** your fork
3. **Create** a feature branch
4. **Make** your changes
5. **Test** thoroughly
6. **Submit** a pull request

### Development Workflow

```bash
# Create feature branch
git checkout -b feature/new-feature

# Make changes
# ... edit files ...

# Test changes
make test

# Commit changes
git add .
git commit -m "Add new feature"

# Push to your fork
git push origin feature/new-feature
```

### Code Style

- **C99 standard** compliance
- **Consistent indentation** (4 spaces)
- **Descriptive variable names**
- **Function documentation** for public APIs
- **Error handling** for all operations

### Areas for Contribution

- **Language features**: New operators, data types, control structures
- **Performance**: Optimize VM execution, reduce memory usage
- **Error handling**: Better error messages, recovery strategies
- **Documentation**: Improve examples, add tutorials
- **Testing**: More test cases, edge case coverage

---

## 📄 License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

---

## 🙏 Acknowledgments

- Inspired by educational language implementations
- Built for learning compiler construction principles
- Thanks to the C programming community for resources and inspiration

---

## 📞 Support

- **Issues**: Report bugs and feature requests on GitHub
- **Discussions**: Ask questions and share ideas
- **Contributions**: Pull requests welcome!

---

*Happy coding with jminus! 🚀*
