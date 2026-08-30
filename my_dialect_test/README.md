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

# Adding Builders
- You have an OperationState, which defines the state of your thing before the operation is actually built.
- You also have a verify function.

- So first, you have to initialize context `mlir::MLIRContext context`
- then, you want to `getOrLoadDialect` for stuff like `mlir::tens::TensDialect` or `mlir::func::FuncDialect`
- Then you declare an `OpBuilder` given the context
- You have the stuff like module and then `setInsertionPoint...` operators
- Whenever you want to create some op, you use the `create` method e.g. `mlir::func::FuncOp::create(builder, loc, ...)`
- Note that this will create an empty state and call those `create` methods you manually implemented earlier.

# Adding matmul verifier
- inspect the `build/tens/Ops.h.inc` for operations. They have getLhs, getRhs
- Then, you just have to know the types of the stuff lol
- Then, I added a sample for what I want.

- VERIFICATION: `if llvm::failed(mlir::verify(module))`
- ERROR: `llvm::errs() << "...";`

# Transforms and passes
- I want to look at passes(e.g. combine ones + square into just ones)
- Then, I want to lower to some other dialect.

- in Passes.h, you need a `std::unique_ptr<mlir::Pass> createYourPass();`

In your passname e.g. `LowerToLinalg.cpp`, you make pattern rewriter and pass wrapper.

Pattern Rewriter
- Pattern rewriter has `matchAndRewrite`
- it calls once per MatmulOp
- You have to use rewriter to create stuff or replace ops.

PassWrapper
- CRTP curiously recurring template pattern: pass your own type as the template arg so in functions like copy() or whatever they know your type already.
- MLIR will handle scheduling and call your pass once per matching op
- We take our pattern and add it to the list so then you try to apply patterns to the operation basically.

To add more stuff, you can just add new rewriters to your pass, and add them to the PassWrapper. After, it just comes down to knowing how to instantiate the other ops, which is pretty difficult ngl. Next step is figuring out where Claude pulled that from.