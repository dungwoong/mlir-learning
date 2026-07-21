#!/bin/bash

LLVM_BASE=$1
FILE_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"


# We can do these passes to convert to LLVM
${LLVM_BASE}/build/bin/mlir-opt ${FILE_ROOT}/simple.mlir \
    --convert-func-to-llvm \
    --convert-math-to-llvm \
    --convert-index-to-llvm \
    --convert-scf-to-cf \
    --convert-cf-to-llvm \
    --convert-arith-to-llvm \
    --reconcile-unrealized-casts \
    -o ${FILE_ROOT}/simple_opt.mlir

echo Expected: 45

${LLVM_BASE}/build/bin/mlir-runner -e main -entry-point-result=i32 ${FILE_ROOT}/simple_opt.mlir

// this is in the llvm dialect, not actual LLVM!