if [[ -z "$1" ]]; then
    echo "Error: First argument is required."
    exit 1
fi

LLVM_BASE=$1
export PATH=$PATH:"$LLVM_BASE/build/bin"
export PYTHONPATH="$PYTHONPATH:$LLVM_BASE/build/tools/mlir/python_packages/mlir_core"