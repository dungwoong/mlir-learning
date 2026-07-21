#!/bin/bash

LLVM_BASE=$1
FILE_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
CLANG_CMD=clang-11

${LLVM_BASE}/build/bin/mlir-translate ${FILE_ROOT}/simple_opt.mlir -mlir-to-llvmir -o ${FILE_ROOT}/simple.ll
llc -filetype=obj --relocation-model=pic ${FILE_ROOT}/simple.ll -o ${FILE_ROOT}/simple.o
$CLANG_CMD -shared -fPIC ${FILE_ROOT}/simple.o -o ${FILE_ROOT}/libsimple.so

echo Compiled, running in python

python3 $FILE_ROOT/run_lib.py