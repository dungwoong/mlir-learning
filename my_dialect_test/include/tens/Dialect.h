#ifndef TENS_DIALECT
#define TENS_DIALECT

#include "mlir/Bytecode/BytecodeOpInterface.h"
#include "mlir/IR/Dialect.h"
#include "mlir/IR/SymbolTable.h"
#include "mlir/Interfaces/CallInterfaces.h"
#include "mlir/Interfaces/FunctionInterfaces.h"
#include "mlir/Interfaces/SideEffectInterfaces.h"

// this header file contains dialect decl
#include "tens/Dialect.h.inc"

// this contains decls of the operations
#define GET_OP_CLASSES
#include "tens/Ops.h.inc"

#endif