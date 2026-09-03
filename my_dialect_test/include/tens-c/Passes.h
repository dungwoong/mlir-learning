#ifndef TENS_C_PASSES_H
#define TENS_C_PASSES_H

#include "mlir-c/Support.h"

#ifdef __cplusplus
extern "C" {
#endif

/// Registers the `tens` dialect passes (currently just
/// `-tens-lower-to-linalg`) with the global pass registry so they can be
/// named in a textual pass pipeline.
MLIR_CAPI_EXPORTED void mlirRegisterTensPasses(void);

#ifdef __cplusplus
}
#endif

#endif // TENS_C_PASSES_H
