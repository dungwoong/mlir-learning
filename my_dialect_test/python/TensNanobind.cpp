//===- TensNanobind.cpp - `tens` dialect Python extension ------------------===//
//
// Nanobind extension for the `tens` dialect. It exposes just enough of the
// C API to drive the dialect from Python:
//
//   * register_dialects() - append the `tens` dialect plus all upstream
//     dialects *and their extensions* (the bufferization external interface
//     models the lowering pipeline needs) to a context.
//   * register_passes()   - register `-tens-lower-to-linalg` plus all upstream
//     passes so a textual pipeline can be handed to PassManager.parse.
//
// This pulls in a lot of MLIR (via MLIRCAPIRegisterEverything); a project that
// cares about binary size would register only the specific dialects, extensions
// and passes it uses instead.
//
//===----------------------------------------------------------------------===//

#include "tens-c/Dialects.h"
#include "tens-c/Passes.h"

#include "mlir-c/Dialect/Arith.h"
#include "mlir-c/Dialect/Func.h"
#include "mlir-c/IR.h"
#include "mlir-c/RegisterEverything.h"
#include "mlir/Bindings/Python/IRCore.h"
#include "mlir/Bindings/Python/Nanobind.h"
#include "mlir/Bindings/Python/NanobindAdaptors.h"

namespace nb = nanobind;

NB_MODULE(_tensDialectsNanobind, m) {
  auto tensM = m.def_submodule("tens");

  tensM.def(
      "register_dialects",
      [](mlir::python::MLIR_BINDINGS_PYTHON_DOMAIN::DefaultingPyMlirContext
             context,
         bool load) {
        MlirContext ctx = context.get()->get();

        MlirDialectRegistry registry = mlirDialectRegistryCreate();
        mlirRegisterAllDialects(registry); // upstream dialects + extensions
        mlirDialectHandleInsertDialect(mlirGetDialectHandle__tens__(), registry);
        mlirContextAppendDialectRegistry(ctx, registry);
        mlirDialectRegistryDestroy(registry);

        if (load) {
          // Eagerly load the handful the example builds ops in; everything
          // else loads on demand as passes touch it.
          mlirDialectHandleLoadDialect(mlirGetDialectHandle__tens__(), ctx);
          mlirDialectHandleLoadDialect(mlirGetDialectHandle__func__(), ctx);
          mlirDialectHandleLoadDialect(mlirGetDialectHandle__arith__(), ctx);
        }
      },
      nb::arg("context").none() = nb::none(), nb::arg("load") = true);

  tensM.def("register_passes", []() {
    mlirRegisterAllPasses();
    mlirRegisterTensPasses();
  });

  // Smoke-test hook: proves the compiled extension is importable and callable.
  tensM.def("smoke", []() { return "tens extension OK"; });
}
