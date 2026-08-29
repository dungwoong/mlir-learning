#include "tens/Dialect.h"
#include "mlir/IR/MLIRContext.h"
#include "mlir/IR/Builders.h"
#include "mlir/IR/BuiltinTypes.h"

int main() {
    mlir::MLIRContext context;
    context.getOrLoadDialect<mlir::tens::TensDialect>();

    mlir::OpBuilder builder(&context);
    auto tensorType = mlir::RankedTensorType::get({2, 2}, builder.getF64Type());
    auto op = builder.create<mlir::tens::OnesOp>(builder.getUnknownLoc(), tensorType);

    op->print(llvm::outs());
    llvm::outs() << "\n";
    return 0;
}