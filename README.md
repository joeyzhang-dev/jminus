# jminus - A Simple C-Based Interpreter and Virtual Machine

[![Build Status](https://img.shields.io/badge/Build-Passing-brightgreen?style=flat-square)]()
[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg?style=flat-square)]()
[![Language: C](https://img.shields.io/badge/language-C-blue?style=flat-square)]()
[![Tests](https://img.shields.io/badge/Tests-Passing-brightgreen?style=flat-square)]()
[![Version](https://img.shields.io/badge/Version-1.0.0-blueviolet?style=flat-square)]()

---

## 📚 About jminus

**jminus** is a small educational programming language featuring:

- A custom lexer, parser, bytecode compiler, and virtual machine
- Basic variable handling (`let` + variable assignment), `yap` (print output), `if`, `while`, and `{}` blocks
- Dual evaluation: **interpreted** and **compiled bytecode VM**
- Fully written in C with minimal dependencies
- REPL (Read-Eval-Print Loop) for quick testing
- Basic error handling and diagnostics

This project was built from scratch to learn:

- Compiler phases (lexing -> parsing -> IR/bytecode construction)
- Bytecode generation and execution
- Stack machine architecture
- Manual memory management (malloc/free)
- Simple variable scope handling (environments)

---

## ⚙️ How to Build and Run

### 1. Build the project

```bash
make // run in interpreter folder with MSYS2 Bash
```

### 2. Start the REPL (interactive shell)

```bash
./jminus-repl.exe
```

### 3. Run a `.jminus` program from a file

```bash
./jminus.exe start.jminus
```

### 4. Run the automated test suite

```bash
./scripts/run_tests.sh
```

---

## ✨ Features of the Language

- **Variables**: 
  - `let x = 5;`
  - `x = x + 1;`
- **Printing**:
  - `yap(x);`
- **Conditionals**:
  - `if (x > 0) { yap(1); } else { yap(0); }`
- **Loops**:
  - `while (x < 10) { x = x + 1; }`
- **Blocks**:
  - `{ let y = 3; yap(y); }`

---

## 📂 Project Structure

| File/Folder           | Purpose                          |
|------------------------|----------------------------------|
| `lexer.c`, `lexer.h`    | Lexical analysis (tokenizer)     |
| `parser.c`, `parser.h`  | Parses tokens into AST           |
| `compiler.c`, `compiler.h` | Compiles AST to bytecode      |
| `vm.c`, `vm.h`          | Virtual machine to execute bytecode |
| `interpreter.c`, `interpreter.h` | Direct interpreter (no VM) |
| `environment.c`, `environment.h` | Variable environments |
| `repl.c`                | REPL shell for quick testing    |
| `main.c`                | Entry point to run a program    |
| `scripts/run_tests.sh`  | Automated test runner           |
| `tests/`                | Unit tests for compiler, lexer, parser |

---

## 🛠 Technologies

- C99 Standard
- Manual memory management (no libraries like Flex/Bison)
- Minimalistic design for educational clarity

---

## 📝 License

Licensed under the [MIT License](https://opensource.org/licenses/MIT).
