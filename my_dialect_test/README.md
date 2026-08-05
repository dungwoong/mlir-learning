# Dialect
- I want it to be like a linalg dialect, kinda like what Toy has available

# Tblgen
llvm-project/build/bin/mlir-tblgen -gen-op-decls mlir-learning/my_dialect_test/include/tens/Ops.td -I llvm-project/mlir/include/

also -gen-op-defs for definitions of the functions