cmake_minimum_required(VERSION 3.20)
project(tens-dialect CXX C)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(MLIR REQUIRED CONFIG)

list(APPEND CMAKE_MODULE_PATH "${MLIR_CMAKE_DIR}")
list(APPEND CMAKE_MODULE_PATH "${LLVM_CMAKE_DIR}")
include(TableGen)
include(AddLLVM)
include(AddMLIR)

# MLIRConfig.cmake doesn't export this in standalone (find_package) mode —
# the example project sets it manually too.
set(MLIR_BINARY_DIR ${CMAKE_BINARY_DIR})

# The static dialect / C-API libraries get pulled into the shared
# libTensPythonCAPI.so that the Python extension links against, so every object
# needs to be position-independent. (The upstream standalone example gets this
# transitively via include(HandleLLVMOptions).)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

include_directories(${LLVM_INCLUDE_DIRS} ${MLIR_INCLUDE_DIRS} ${CMAKE_CURRENT_BINARY_DIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
add_definitions(${LLVM_DEFINITIONS})

# --- TableGen ---
set(LLVM_TARGET_DEFINITIONS include/tens/Ops.td)
mlir_tablegen(tens/Dialect.h.inc -gen-dialect-decls)
mlir_tablegen(tens/Dialect.cpp.inc -gen-dialect-defs)
mlir_tablegen(tens/Ops.h.inc -gen-op-decls)
mlir_tablegen(tens/Ops.cpp.inc -gen-op-defs)
add_public_tablegen_target(TensOpsIncGen)

# --- Dialect library ---
add_mlir_dialect_library(MLIRTensDialect
  mlir/Dialect.cpp
  mlir/LowerToLinalg.cpp

  DEPENDS
  TensOpsIncGen

  LINK_LIBS
  MLIRIR
  MLIRSupport
  MLIRPass
  MLIRTransformUtils
  MLIRLinalgDialect
  MLIRTensorDialect
  MLIRArithDialect
)

# --- C API library ---
add_mlir_public_c_api_library(TensCAPI
  mlir/capi/Dialects.cpp
  mlir/capi/Passes.cpp

  LINK_LIBS PUBLIC
  MLIRTensDialect
  MLIRPass
  MLIRCAPIIR
)

# --- Sample program ---
add_llvm_executable(tens-test tools/main.cpp)
llvm_update_compile_flags(tens-test)
target_link_libraries(tens-test PRIVATE
  MLIRTensDialect MLIRIR MLIRFuncDialect MLIRLinalgDialect MLIRTensorDialect MLIRArithDialect)

# --- C API smoke test ---
add_executable(tens-capi-test tools/capi_test.c)
target_link_libraries(tens-capi-test PRIVATE TensCAPI MLIRCAPIIR)

# --- Python bindings ---
if(MLIR_ENABLE_BINDINGS_PYTHON)
  include(MLIRDetectPythonEnv)
  mlir_configure_python_dev_packages()

  if(NOT MLIR_PYTHON_PACKAGE_PREFIX)
    set(MLIR_PYTHON_PACKAGE_PREFIX "mlir_tens" CACHE STRING "" FORCE)
  endif()
  if(NOT MLIR_BINDINGS_PYTHON_NB_DOMAIN)
    set(MLIR_BINDINGS_PYTHON_NB_DOMAIN "mlir_tens" CACHE STRING "" FORCE)
  endif()
  if(NOT MLIR_BINDINGS_PYTHON_INSTALL_PREFIX)
    set(MLIR_BINDINGS_PYTHON_INSTALL_PREFIX "python_packages/tens/${MLIR_PYTHON_PACKAGE_PREFIX}" CACHE STRING "" FORCE)
  endif()

  add_subdirectory(python)
endif()