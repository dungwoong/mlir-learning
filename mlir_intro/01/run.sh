LLVM_ROOT=$1
FILE_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
${FILE_ROOT}/mlir_to_llvm.sh $LLVM_ROOT
${FILE_ROOT}/compile_llvm.sh $LLVM_ROOT