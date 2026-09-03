include(AddMLIRPython)

add_compile_definitions("MLIR_PYTHON_PACKAGE_PREFIX=${MLIR_PYTHON_PACKAGE_PREFIX}.")

declare_mlir_python_sources(TensPythonSources)

declare_mlir_dialect_python_bindings(
  ADD_TO_PARENT TensPythonSources
  ROOT_DIR "${CMAKE_CURRENT_SOURCE_DIR}/mlir_tens"
  TD_FILE dialects/TensOps.td
  SOURCES
    dialects/standalone_nanobind.py
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
    TensCAPI
)

add_mlir_python_common_capi_library(TensPythonCAPI
  INSTALL_COMPONENT TensPythonModules
  INSTALL_DESTINATION "${MLIR_BINDINGS_PYTHON_INSTALL_PREFIX}/_mlir_libs"
  OUTPUT_DIRECTORY "${MLIR_BINARY_DIR}/${MLIR_BINDINGS_PYTHON_INSTALL_PREFIX}/_mlir_libs"
  RELATIVE_INSTALL_ROOT "../../../.."
  DECLARED_SOURCES
    TensPythonSources
    MLIRPythonSources.Core
)

set(TensPythonModules_ROOT_PREFIX "${MLIR_BINARY_DIR}/${MLIR_BINDINGS_PYTHON_INSTALL_PREFIX}")

add_mlir_python_modules(TensPythonModules
  ROOT_PREFIX "${TensPythonModules_ROOT_PREFIX}"
  INSTALL_PREFIX "${MLIR_BINDINGS_PYTHON_INSTALL_PREFIX}"
  DECLARED_SOURCES
    TensPythonSources
    MLIRPythonSources.Core
  COMMON_CAPI_LINK_LIBS
    TensPythonCAPI
)