#ifndef TENS_C_DIALECTS_H
#define TENS_C_DIALECTS_H

#include "mlir-c/IR.h"

#ifdef __cplusplus
extern "C" {
#endif

MLIR_DECLARE_CAPI_DIALECT_REGISTRATION(Tens, tens);

#ifdef __cplusplus
}
#endif

#endif