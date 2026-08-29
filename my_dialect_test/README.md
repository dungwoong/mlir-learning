# Dialect
- I want it to be like a linalg dialect, kinda like what Toy has available

# Tblgen
llvm-project/build/bin/mlir-tblgen -gen-op-decls mlir-learning/my_dialect_test/include/tens/Ops.td -I llvm-project/mlir/include/

also -gen-op-defs for definitions of the functions

# Adding files
- `include/tens` has Dialect.h and Ops.td
- Op.td creates an h.inc and cpp.inc file.

In CMakeLists, do this:

```
mlir_tablegen(include/tens/TensOpsDialect.h.inc -gen-dialect-decls)
mlir_tablegen(include/tens/TensOpsDialect.cpp.inc -gen-dialect-defs)
```

- dialect.h basically includes all the stuff in Ops.td with additional headers to make it work
- in `mlir/Dialect.cpp` you can use the cpp inc to declare stuff.

- build: `cmake -G Ninja .. -DMLIR_DIR=$LLVM_PATH/build/lib/cmake/mlir -DLLVM_DIR=$LLVM_PATH/build/lib/cmake/llvm ninja MLIRTensDialect`
- then just `ninja`

- In the Dialect.cpp, we put functions that correspond to the verifier or whatever. Need to actually inspect the .inc files to understand what we are doing.