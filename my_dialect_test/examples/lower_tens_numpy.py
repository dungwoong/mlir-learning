"""End-to-end: build the `tens` program from Python, lower it all the way to
the LLVM dialect, JIT it with the MLIR ExecutionEngine, and read the result
back into a NumPy array.

    @main() -> tensor<2x2xf64>
      ones   = [[1, 1], [1, 1]]
      square = ones * ones          = [[1, 1], [1, 1]]
      result = matmul(ones, square) = [[2, 2], [2, 2]]

`-buffer-results-to-out-params` turns the tensor return into a caller-provided
`memref<2x2xf64>` out-parameter, which is exactly the memref-descriptor calling
convention used in mlir_intro/02/emit_mlir_3.py.

Run (after building the TensPythonModules target):

    PYTHONPATH=build/python_packages/tens python3 examples/lower_tens_numpy.py
"""

import ctypes

import numpy as np

import mlir_tens.ir as ir
from mlir_tens.dialects import func
from mlir_tens.dialects import tens_nanobind as tens
from mlir_tens.execution_engine import ExecutionEngine
from mlir_tens.passmanager import PassManager
from mlir_tens.runtime import get_ranked_memref_descriptor


# tens -> linalg-on-tensors -> buffers -> loops -> LLVM dialect.
#
# buffer-results-to-out-params turns `@main` from "returns a tensor" into
# "takes the result memref as an out-param" (the emit_mlir_3.py convention).
# modify-public-functions is required because `@main` is public; hoist-static-
# allocs makes it write straight into the caller's buffer with no extra copy.
LOWER_TO_LLVM = "builtin.module(" + ", ".join(
    [
        "tens-lower-to-linalg",
        "empty-tensor-to-alloc-tensor",
        "one-shot-bufferize{bufferize-function-boundaries "
        "function-boundary-type-conversion=identity-layout-map}",
        "buffer-results-to-out-params{hoist-static-allocs modify-public-functions}",
        "convert-linalg-to-loops",
        "convert-scf-to-cf",
        "expand-strided-metadata",
        "finalize-memref-to-llvm",
        "convert-func-to-llvm",
        "convert-arith-to-llvm",
        "convert-cf-to-llvm",
        "reconcile-unrealized-casts",
    ]
) + ")"


def build_module() -> ir.Module:
    module = ir.Module.create()
    f64 = ir.F64Type.get()
    mat2x2 = ir.RankedTensorType.get([2, 2], f64)

    with ir.InsertionPoint(module.body):
        fn = func.FuncOp("main", ([], [mat2x2]))
        # Needed for the ExecutionEngine's C calling convention wrapper.
        fn.attributes["llvm.emit_c_interface"] = ir.UnitAttr.get()
        with ir.InsertionPoint(fn.add_entry_block()):
            ones = tens.ones(mat2x2)
            squared = tens.square(mat2x2, ones)
            product = tens.matmul(mat2x2, ones, squared)
            func.ReturnOp([product])
    return module


def main() -> None:
    with ir.Context(), ir.Location.unknown():
        tens.register_dialects()
        tens.register_passes()

        module = build_module()
        if not module.operation.verify():
            raise SystemExit("verification failed")
        print("=== tens IR ===")
        print(module)

        PassManager.parse(LOWER_TO_LLVM).run(module.operation)
        print("=== LLVM dialect ===")
        print(module)

        engine = ExecutionEngine(module, opt_level=2)

        # `main` now takes the result memref as an out-param (void return).
        result = np.zeros((2, 2), dtype=np.float64)
        result_ptr = ctypes.pointer(
            ctypes.pointer(get_ranked_memref_descriptor(result))
        )
        engine.invoke("main", result_ptr)

        print("=== result (numpy) ===")
        print(result)
        expected = np.array([[2.0, 2.0], [2.0, 2.0]])
        np.testing.assert_array_equal(result, expected)
        print("OK: JIT result matches expected [[2, 2], [2, 2]]")


if __name__ == "__main__":
    main()
