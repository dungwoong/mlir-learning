#include "tens/Dialects.h"
#include "mlir-c/Dialect/Arith.h"
#include "mlir/Bindings/Python/IRCore.h"
#include "mlir/Bindings/Python/IRTypes.h"
#include "mlir/Bindings/Python/Nanobind.h"
#include "mlir/Bindings/Python/NanobindAdaptors.h"

namespace nb = nanobind;

struct PyCustomType
    : mlir::python::MLIR_BINDINGS_PYTHON_DOMAIN::PyConcreteType<PyCustomType> {
  static constexpr IsAFunctionTy isaFunction = mlirTensTypeIsACustomType;
  static constexpr GetTypeIDFunctionTy getTypeIdFunction =
      mlirTensCustomTypeGetTypeID;
  static constexpr const char *pyClassName = "CustomType";
  using PyConcreteType::PyConcreteType;

  static void bindDerived(ClassTy &c) {
    c.def_static(
        "get",
        [](const std::string &value,
           mlir::python::MLIR_BINDINGS_PYTHON_DOMAIN::DefaultingPyMlirContext
               context) {
          return PyCustomType(
              context->getRef(),
              mlirTensCustomTypeGet(
                  context.get()->get(),
                  mlirStringRefCreateFromCString(value.c_str())));
        },
        nb::arg("value"), nb::arg("context").none() = nb::none());
  }
};

NB_MODULE(_tensDialectsNanobind, m) {
  auto tensM = m.def_submodule("tens");

  PyCustomType::bind(tensM);

  tensM.def(
      "register_dialects",
      [](mlir::python::MLIR_BINDINGS_PYTHON_DOMAIN::DefaultingPyMlirContext
             context,
         bool load) {
        MlirDialectHandle arithHandle = mlirGetDialectHandle__arith__();
        MlirDialectHandle standaloneHandle =
            mlirGetDialectHandle__standalone__();
        MlirContext context_ = context.get()->get();
        mlirDialectHandleRegisterDialect(arithHandle, context_);
        mlirDialectHandleRegisterDialect(standaloneHandle, context_);
        if (load) {
          mlirDialectHandleLoadDialect(arithHandle, context_);
          mlirDialectHandleRegisterDialect(standaloneHandle, context_);
        }
      },
      nb::arg("context").none() = nb::none(), nb::arg("load") = true);

  tensM.def(
      "print_fp_type",
      [](const mlir::python::MLIR_BINDINGS_PYTHON_DOMAIN::PyF16Type &,
         nb::handle stderr_obj) {
        nb::print("this is a fp16 type", /*end=*/nb::handle(), stderr_obj);
      });
  standaloneM.def(
      "print_fp_type",
      [](const mlir::python::MLIR_BINDINGS_PYTHON_DOMAIN::PyF32Type &,
         nb::handle stderr_obj) {
        nb::print("this is a fp32 type", /*end=*/nb::handle(), stderr_obj);
      });
  standaloneM.def(
      "print_fp_type",
      [](const mlir::python::MLIR_BINDINGS_PYTHON_DOMAIN::PyF64Type &,
         nb::handle stderr_obj) {
        nb::print("this is a fp64 type", /*end=*/nb::handle(), stderr_obj);
      });
}
