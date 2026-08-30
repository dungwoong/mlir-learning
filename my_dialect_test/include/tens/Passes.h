#ifndef TENS_PASSES
#define TENS_PASSES

#include "mlir/Pass/Pass.h"
#include <memory>

namespace mlir {
namespace tens {
std::unique_ptr<mlir::Pass> createLowerToLinalgPass();
}
}

#endif // TENS_PASSES