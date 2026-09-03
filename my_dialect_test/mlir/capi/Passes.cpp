#include "tens-c/Passes.h"
#include "tens/Passes.h"

void mlirRegisterTensPasses(void) { mlir::tens::registerTensPasses(); }
