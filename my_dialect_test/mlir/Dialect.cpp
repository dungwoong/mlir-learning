#include "tens/Dialect.h"

using namespace mlir;
using namespace mlir::tens;

#include "tens/Dialect.cpp.inc"

#define GET_OP_CLASSES
#include "tens/Ops.cpp.inc"

void TensDialect::initialize() {
    addOperations<
#define GET_OP_LIST
#include "tens/Ops.cpp.inc"
>();
}

void SquareOp::build(OpBuilder &builder, OperationState &state, Value x) {
  state.addOperands(x);
  state.addTypes(x.getType());
}

void MatMulOp::build(OpBuilder &builder, OperationState &state, Value lhs,
                      Value rhs) {
  state.addOperands({lhs, rhs});
  // placeholder result type for now — you'll want to actually compute
  // MxK * KxN -> MxN shape here later
  state.addTypes(lhs.getType());
}

LogicalResult MatMulOp::verify() {
  // placeholder — return success for now, add real shape checks later
  auto lhsType = llvm::dyn_cast<RankedTensorType>(getLhs().getType());
  auto rhsType = llvm::dyn_cast<RankedTensorType>(getRhs().getType());

  if (!lhsType || !rhsType) {
    return emitOpError("Operands must be ranked Tensors");
  }

  if (lhsType.getRank() != 2 || rhsType.getRank() != 2) {
    return emitOpError("Operands must be rank-2 tensors, got ranks ") <<
    lhsType.getRank() << " and " << rhsType.getRank();
  }
  int64_t lhsK = lhsType.getDimSize(1);
  int64_t rhsK = rhsType.getDimSize(0);
  if (lhsK != ShapedType::kDynamic && rhsK != ShapedType::kDynamic &&
      lhsK != rhsK)
    return emitOpError("inner dimensions must match, got ")
           << lhsK << " and " << rhsK;

  return success();
}