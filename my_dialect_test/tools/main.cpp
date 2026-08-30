#include "tens/Dialect.h"

#include "mlir/IR/MLIRContext.h"
#include "mlir/IR/Builders.h"
#include "mlir/IR/BuiltinOps.h"
#include "mlir/IR/BuiltinTypes.h"
#include "mlir/Dialect/Func/IR/FuncOps.h"
#include "mlir/IR/Verifier.h"
#include "tens/Passes.h"
#include "mlir/Dialect/Linalg/IR/Linalg.h"
#include "mlir/Dialect/Tensor/IR/Tensor.h"
#include "mlir/Pass/PassManager.h"

int main() {
    mlir::MLIRContext context;
    context.getOrLoadDialect<mlir::tens::TensDialect>();
    context.getOrLoadDialect<mlir::func::FuncDialect>();
    context.getOrLoadDialect<mlir::linalg::LinalgDialect>();
    context.getOrLoadDialect<mlir::tensor::TensorDialect>();

    mlir::OpBuilder builder(&context);
    auto loc = builder.getUnknownLoc();

    mlir::ModuleOp module = mlir::ModuleOp::create(loc);
    builder.setInsertionPointToEnd(module.getBody());

    auto tensorType = mlir::RankedTensorType::get({2, 2}, builder.getF64Type());

    // https://mlir.llvm.org/doxygen/classmlir_1_1Builder.html#a5e44a1083e200c0aea501f30f4ddc62c
    auto funcType = builder.getFunctionType({}, {tensorType});
    auto funcOp = mlir::func::FuncOp::create(builder, loc, "main", funcType);
    builder.setInsertionPointToStart(funcOp.addEntryBlock());

    auto onesOp = mlir::tens::OnesOp::create(builder, loc, tensorType);
    auto squareOp = mlir::tens::SquareOp::create(builder, loc, onesOp.getResult());
    auto matmulOp = mlir::tens::MatMulOp::create(builder, loc, onesOp.getResult(), squareOp.getResult());
    
    mlir::func::ReturnOp::create(builder, loc, matmulOp.getResult());

    if (llvm::failed(mlir::verify(module))) {
        llvm::errs() << "module verification failed\n";
        return 1;
    }

    mlir::PassManager pm(&context);
    pm.addPass(mlir::tens::createLowerToLinalgPass());
    if (failed(pm.run(module))) {
    llvm::errs() << "pass failed\n";
        return 1;
    }

    module.print(llvm::outs());
    llvm::outs() << "\n";
    return 0;
}