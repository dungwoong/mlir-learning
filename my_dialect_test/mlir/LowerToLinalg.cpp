#include "tens/Dialect.h"
#include "tens/Passes.h"

#include "mlir/Dialect/Linalg/IR/Linalg.h"
#include "mlir/Dialect/Tensor/IR/Tensor.h"
#include "mlir/IR/PatternMatch.h"
#include "mlir/Pass/Pass.h"
#include "mlir/Transforms/GreedyPatternRewriteDriver.h"

using namespace mlir;

namespace {
struct MatMulToLinalg : public OpRewritePattern<tens::MatMulOp> {
    using OpRewritePattern::OpRewritePattern; // inherit constructors from base class

    LogicalResult matchAndRewrite(tens::MatMulOp op, PatternRewriter &rewriter) const override {
        Location loc = op.getLoc();
        auto resultType = llvm::cast<RankedTensorType>(op.getType());

        Value init = tensor::EmptyOp::create(rewriter, loc, resultType.getShape(), resultType.getElementType());

        auto matmul = linalg::MatmulOp::create(
            rewriter, loc, resultType, ValueRange{op.getLhs(), op.getRhs()}, ValueRange{init}
        );

        rewriter.replaceOp(op, matmul.getResult(0));
        return success();
    }
}

struct LowerToLinalgPass
    : public PassWrapper<LowerToLinalgPass, OperationPass<ModuleOp>> {
  void runOnOperation() override {
    RewritePatternSet patterns(&getContext());
    patterns.add<MatMulToLinalg>(&getContext());
    if (failed(applyPatternsAndFoldGreedily(getOperation(),
                                             std::move(patterns))))
      signalPassFailure();
  }
};
} // namespace

std::unique_ptr<mlir::Pass> mlir::tens::createLowerToLinalgPass() {
  return std::make_unique<LowerToLinalgPass>();
}