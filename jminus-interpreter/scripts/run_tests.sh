#!/usr/bin/env bash
set -euo pipefail

SRC_DIR="."
TEST_DIR="tests"
BIN_DIR="build/tests"

mkdir -p "$BIN_DIR"

echo "=== Compiling and running all tests ==="
for test_file in "$TEST_DIR"/*_tests.c; do
  test_name=$(basename "$test_file" .c)
  out="$BIN_DIR/$test_name.exe"

  echo
  echo "[*] Building $test_name..."
  gcc -std=c99 -Wall \
      "$SRC_DIR"/lexer.c \
      "$SRC_DIR"/parser.c \
      "$SRC_DIR"/compiler.c \
      "$SRC_DIR"/vm.c \
      "$SRC_DIR"/interpreter.c \
      "$SRC_DIR"/winmain.c \
      "$test_file" \
      -o "$out"

  echo "[*] Running $test_name..."
  "$out"
done

echo
echo "=== All tests passed! ==="
