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
  return success();
}