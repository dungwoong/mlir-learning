# Generated with Gemini for now
cmake_minimum_required(VERSION 3.20)
project(tens-dialect CXX C)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 1. Locate installed MLIR and LLVM packages
find_package(MLIR REQUIRED CONFIG)

list(APPEND CMAKE_MODULE_PATH "${MLIR_CMAKE_DIR}")
list(APPEND CMAKE_MODULE_PATH "${LLVM_CMAKE_DIR}")
include(TableGen)
include(AddLLVM)
include(AddMLIR)

include_directories(${LLVM_INCLUDE_DIRS} ${MLIR_INCLUDE_DIRS} ${CMAKE_CURRENT_BINARY_DIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
add_definitions(${LLVM_DEFINITIONS})

# 2. Automate TableGen code generation from FooOps.td
set(LLVM_TARGET_DEFINITIONS include/tens/Ops.td)
mlir_tablegen(include/tens/Dialect.h.inc -gen-dialect-decls)
mlir_tablegen(include/tens/Dialect.cpp.inc -gen-dialect-defs)
mlir_tablegen(include/tens/Ops.h.inc -gen-op-decls)
mlir_tablegen(include/tens/Ops.cpp.inc -gen-op-defs)
add_public_tablegen_target(TensOpsIncGen)

# 3. Create the C++ Dialect Library
add_mlir_dialect_library(MLIRTensDialect
  mlir/Dialect.cpp

  DEPENDS
  TensOpsIncGen

  LINK_LIBS
  MLIRIR
  MLIRSupport
)