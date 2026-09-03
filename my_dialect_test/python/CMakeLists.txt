include(AddMLIRPython)

add_compile_definitions("MLIR_PYTHON_PACKAGE_PREFIX=${MLIR_PYTHON_PACKAGE_PREFIX}.")

declare_mlir_python_sources(TensPythonSources)

declare_mlir_dialect_python_bindings(
  ADD_TO_PARENT TensPythonSources
  ROOT_DIR "${CMAKE_CURRENT_SOURCE_DIR}/mlir_tens"
  TD_FILE dialects/TensOps.td
  SOURCES
    dialects/tens_nanobind.py
  DIALECT_NAME tens)

declare_mlir_python_extension(TensPythonSources.NanobindExtension
  MODULE_NAME _tensDialectsNanobind
  ADD_TO_PARENT TensPythonSources
  SOURCES
    TensNanobind.cpp
  PRIVATE_LINK_LIBS
    LLVMSupport
  EMBED_CAPI_LINK_LIBS
    MLIRCAPIIR
    MLIRCAPIArith
    MLIRCAPIFunc
    # Match upstream's RegisterEverything extension: the core _mlir module's
    # Rewrite.cpp needs the DialectConversion CAPI from MLIRCAPITransforms.
    MLIRCAPITransforms
    MLIRCAPIConversion
    MLIRCAPIRegisterEverything
    TensCAPI
)

# Python-side sources that ship in the `mlir_tens` package. `func` is pulled in
# because the example builds `func.func` / `func.return` from Python; `linalg`,
# `tensor` and `arith` only need to be *registered* in the context (done via the
# C API in TensNanobind.cpp) for the lowered IR to print, so their Python op
# wrappers are intentionally left out to keep the build light. ExecutionEngine
# brings `mlir_tens.execution_engine` + `mlir_tens.runtime` for JIT + numpy.
set(_tens_declared_sources
  TensPythonSources
  MLIRPythonSources.Core
  MLIRPythonSources.Dialects.func
  MLIRPythonSources.ExecutionEngine
)

add_mlir_python_common_capi_library(TensPythonCAPI
  INSTALL_COMPONENT TensPythonModules
  INSTALL_DESTINATION "${MLIR_BINDINGS_PYTHON_INSTALL_PREFIX}/_mlir_libs"
  OUTPUT_DIRECTORY "${MLIR_BINARY_DIR}/${MLIR_BINDINGS_PYTHON_INSTALL_PREFIX}/_mlir_libs"
  RELATIVE_INSTALL_ROOT "../../../.."
  DECLARED_SOURCES
    ${_tens_declared_sources}
)

set(TensPythonModules_ROOT_PREFIX "${MLIR_BINARY_DIR}/${MLIR_BINDINGS_PYTHON_INSTALL_PREFIX}")

add_mlir_python_modules(TensPythonModules
  ROOT_PREFIX "${TensPythonModules_ROOT_PREFIX}"
  INSTALL_PREFIX "${MLIR_BINDINGS_PYTHON_INSTALL_PREFIX}"
  DECLARED_SOURCES
    ${_tens_declared_sources}
  COMMON_CAPI_LINK_LIBS
    TensPythonCAPI
)