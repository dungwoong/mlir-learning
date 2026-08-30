#include "tens/Dialect.h"
#include "tens/Passes.h"

#include "mlir/Dialect/Linalg/IR/Linalg.h"
#include "mlir/Dialect/Tensor/IR/Tensor.h"
#include "mlir/Dialect/Arith/IR/Arith.h"
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
};

struct OnesToLinalg : public OpRewritePattern<tens::OnesOp> {
    using OpRewritePattern::OpRewritePattern;

    LogicalResult matchAndRewrite(tens::OnesOp op, PatternRewriter &rewriter) const override {
        Location loc = op.getLoc();
        auto resultType = llvm::cast<RankedTensorType>(op.getType());
        Value one = arith::ConstantOp::create(rewriter, loc, rewriter.getF64FloatAttr(1.0));

        // value range isn't needed, need to look at .h.inc for default values
        Value init = tensor::SplatOp::create(rewriter, loc, one, resultType, ValueRange{});

        rewriter.replaceOp(op, init);
        return success();
    }
};

struct SquareToLinalg : public OpRewritePattern<tens::SquareOp> {
    using OpRewritePattern::OpRewritePattern;

    LogicalResult matchAndRewrite(tens::SquareOp op, PatternRewriter &rewriter) const override {
        Location loc = op.getLoc();
        auto resultType = llvm::cast<RankedTensorType>(op.getType());
        Value init = tensor::EmptyOp::create(rewriter, loc, resultType.getShape(),
                                              resultType.getElementType());
        auto mapOp = linalg::MapOp::create(
            rewriter, loc, ValueRange{op.getX()}, init,
            [&](OpBuilder &b, Location loc, ValueRange args) {
                Value squared = arith::MulFOp::create(b, loc, args[0], args[0]);
                linalg::YieldOp::create(b, loc, squared);
            });

        rewriter.replaceOp(op, mapOp.getResult());
        return success();
    }
};

struct LowerToLinalgPass
    : public PassWrapper<LowerToLinalgPass, OperationPass<ModuleOp>> {
  void runOnOperation() override {
    RewritePatternSet patterns(&getContext());
    patterns.add<MatMulToLinalg, OnesToLinalg, SquareToLinalg>(&getContext());
    if (failed(applyPatternsGreedily(getOperation(),
                                             std::move(patterns))))
      signalPassFailure();
  }
};
} // namespace

std::unique_ptr<mlir::Pass> mlir::tens::createLowerToLinalgPass() {
  return std::make_unique<LowerToLinalgPass>();
}