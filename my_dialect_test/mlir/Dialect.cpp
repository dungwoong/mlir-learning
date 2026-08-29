#include "tens/Dialect.h"

using namespace mlir;
using namespace mlir::tens;

#include "tens/Dialect.cpp.inc"

#define GET_OP_CLASSES
#include "tens/Ops.cpp.inc"

void TensDialect::initialize() {
    addOperations
#define GET_OP_LIST
#include "tens/Ops.cpp.inc"
>();
}