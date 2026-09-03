"""Python port of tools/main.cpp.

Builds

    func.func @main() -> tensor<2x2xf64> {
      %0 = tens.ones
      %1 = tens.square %0
      %2 = tens.matmul %0, %1
      return %2
    }

verifies it, runs the `-tens-lower-to-linalg` pass, and prints the module
before and after lowering.

Run (after building the TensPythonModules target):

    PYTHONPATH=build/python_packages/tens python3 examples/lower_tens.py
"""

import mlir_tens.ir as ir
from mlir_tens.dialects import func
from mlir_tens.dialects import tens_nanobind as tens
from mlir_tens.passmanager import PassManager


def main() -> None:
    with ir.Context(), ir.Location.unknown():
        tens.register_dialects()
        tens.register_passes()

        module = ir.Module.create()
        f64 = ir.F64Type.get()
        mat2x2 = ir.RankedTensorType.get([2, 2], f64)

        with ir.InsertionPoint(module.body):
            fn = func.FuncOp("main", ([], [mat2x2]))
            with ir.InsertionPoint(fn.add_entry_block()):
                ones = tens.ones(mat2x2)
                squared = tens.square(mat2x2, ones)
                product = tens.matmul(mat2x2, ones, squared)
                func.ReturnOp([product])

        if not module.operation.verify():
            raise SystemExit("module verification failed")

        print("=== tens IR ===")
        print(module)

        pm = PassManager.parse("builtin.module(tens-lower-to-linalg)")
        pm.run(module.operation)

        print("=== after -tens-lower-to-linalg ===")
        print(module)

        # Prove we got here and the lowering did its job.
        body = module.body.operations[0].regions[0].blocks[0]
        lowered_ops = sorted({op.operation.name for op in body.operations})
        print("lowered op set:", lowered_ops)
        assert not any(name.startswith("tens.") for name in lowered_ops), lowered_ops
        assert "linalg.matmul" in lowered_ops, lowered_ops
        print("OK: tens.* fully lowered to linalg/tensor/arith")


if __name__ == "__main__":
    main()
