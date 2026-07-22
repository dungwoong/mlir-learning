import ctypes
from mlir.ir import *
from mlir.dialects import func, arith
from mlir.execution_engine import ExecutionEngine
from mlir.passmanager import PassManager


# https://mlir.llvm.org/docs/Bindings/Python/#context-management
# MLIRContext is like LLVMContext

# https://mlir.llvm.org/docs/Bindings/Python/#insertion-points-and-locations
# You can attach this in a with, or in specific ops

# Some docs
# - Location: https://mlir.llvm.org/python-bindings/autoapi/mlir/_mlir_libs/_mlir/ir/index.html#mlir._mlir_libs._mlir.ir.Location
context = Context()
with context, Location.unknown():
    module = Module.create()
    f32 = F32Type.get()

    # You can add ops like this
    with InsertionPoint(module.body):
        # 1. Create the function
        f = func.FuncOp('my_add', ([f32, f32], [f32]))
        
        # 2. Attach the attribute directly to the operation
        f.operation.attributes["llvm.emit_c_interface"] = UnitAttr.get()
        
        with InsertionPoint(f.add_entry_block()):
            b = arith.AddFOp(f.arguments[0], f.arguments[1])
            func.ReturnOp([b])

    # 3. Lower to LLVM
    pm = PassManager.parse("builtin.module(convert-to-llvm)")
    pm.run(module.operation)

    # 4. Create JIT Engine
    engine = ExecutionEngine(module)

def run(a: float, b: float) -> float:
    arg1, arg2, res = ctypes.c_float(a), ctypes.c_float(b), ctypes.c_float(0.0)
    # byref is like pass by reference e.g. &x
    engine.invoke("my_add", ctypes.byref(arg1), ctypes.byref(arg2), ctypes.byref(res))
    return res.value

print(module)

print(f"{run(3, 2)}")