"""Smoke test for the `tens` Python bindings.

Run after building the `TensPythonModules` target:

    PYTHONPATH=build/python_packages/tens python3 test/smoketest.py

It only checks that the CMake / CAPI / nanobind plumbing works end to end:
the packages import, the compiled extension loads, the dialect registers, and
a `tens` op round-trips through the textual parser/printer.
"""

import sys

print("Testing mlir_tens package", file=sys.stderr)

import mlir_tens.ir
from mlir_tens.dialects import tens_nanobind as tens_d

# Compiled extension is importable and callable.
print(tens_d.smoke(), file=sys.stderr)

with mlir_tens.ir.Context() as ctx:
    tens_d.register_dialects()

    # Only `tens` + `arith` are registered by register_dialects(), so keep the
    # smoke IR to those dialects (no `func.func` wrapper).
    module = mlir_tens.ir.Module.parse(
        """
        %0 = "tens.ones"() : () -> tensor<2x2xf64>
        %1 = "tens.square"(%0) : (tensor<2x2xf64>) -> tensor<2x2xf64>
        %2 = "tens.matmul"(%0, %1)
               : (tensor<2x2xf64>, tensor<2x2xf64>) -> tensor<2x2xf64>
        """
    )
    print(module, file=sys.stderr)

# The upstream `mlir` core package is reachable through the same prefix.
from mlir_tens.ir import F64Type

print("smoke test OK", file=sys.stderr)
