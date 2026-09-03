#ifndef TENS_PASSES
#define TENS_PASSES

#include "mlir/Pass/Pass.h"
#include <memory>

namespace mlir {
namespace tens {
std::unique_ptr<mlir::Pass> createLowerToLinalgPass();

// Registers the `-tens-lower-to-linalg` pass so it can be referenced by name
// from a textual pass pipeline (e.g. from the Python `PassManager.parse`).
void registerLowerToLinalgPass();

inline void registerTensPasses() { registerLowerToLinalgPass(); }
}
}

#endif // TENS_PASSES
