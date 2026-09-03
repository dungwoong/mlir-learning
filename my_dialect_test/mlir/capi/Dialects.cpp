#include "Tens-c/Dialects.h"
#include "tens/Dialect.h"
#include "mlir/CAPI/Registration.h"

MLIR_DEFINE_CAPI_DIALECT_REGISTRATION(Tens, tens, mlir::tens::TensDialect)