from time import time
import ctypes
import numpy as np
from mlir.ir import *
from mlir.dialects import func, arith, memref, scf
from mlir.execution_engine import ExecutionEngine
from mlir.passmanager import PassManager
from mlir.runtime import get_ranked_memref_descriptor # can use this for numpy arrays instead of my thing

context = Context()
with context, Location.unknown():
    module = Module.create()
    f32 = F32Type.get()

    # Dynamic 1D MemRef type: memref<?xf32>
    memref_type = MemRefType.get([ShapedType.get_dynamic_size()], f32)
    with InsertionPoint(module.body):
        # function, input/output types
        f = func.FuncOp('dynamic_square', ([memref_type, memref_type], []))
        f.operation.attributes["llvm.emit_c_interface"] = UnitAttr.get()

        with InsertionPoint(f.add_entry_block()):
            inp, out = f.arguments[0], f.arguments[1]
            c0 = arith.ConstantOp.create_index(0)
            size = memref.DimOp(inp, c0) # dim of input at index 0

            lb = arith.ConstantOp.create_index(0)
            step = arith.ConstantOp.create_index(1)

            loop = scf.ForOp(lb, size, step)
            with InsertionPoint(loop.body):
                i = loop.induction_variable
                val = memref.LoadOp(inp, [i])
                squared = arith.MulFOp(val.result, val.result)
                memref.StoreOp(squared, out, [i])
                scf.YieldOp([])
            func.ReturnOp([])

    print(module)

    pm = PassManager.parse("builtin.module(convert-scf-to-cf, convert-to-llvm)")
    # pm = PassManager.parse(
    #     "builtin.module("
    #     "convert-scf-to-cf,"
    #     "finalize-memref-to-llvm,"
    #     "convert-func-to-llvm,"
    #     "convert-arith-to-llvm,"
    #     "convert-cf-to-llvm,"
    #     "reconcile-unrealized-casts)"
    # )
    pm.run(module.operation)
    engine = ExecutionEngine(module)

class CustomMemref1dF32(ctypes.Structure):
    _fields_ = [
        ("allocated", ctypes.c_void_p),
        ("aligned", ctypes.c_void_p),
        ("offset", ctypes.c_int64),
        ("sizes", ctypes.c_int64 * 1),
        ("strides", ctypes.c_int64 * 1),
    ]

    @classmethod
    def from_numpy(cls, arr: np.ndarray):
        assert arr.dtype == np.float32 and arr.ndim == 1, "Must be 1D float32 array"
        ptr = arr.ctypes.data_as(ctypes.c_void_p)
        stride_in_elements = arr.strides[0] // arr.itemsize
        return cls(
            allocated=ptr,
            aligned=ptr,
            offset=0,
            sizes=(ctypes.c_int64 * 1)(arr.shape[0]),
            strides=(ctypes.c_int64 * 1)(stride_in_elements),
        )

data = np.array([1, 2, 3, 4, 5, 6, 7, 8, 9], dtype=np.float32)

def square(data):
    res = np.empty_like(data)
    desc_data = CustomMemref1dF32.from_numpy(data)
    desc_res = CustomMemref1dF32.from_numpy(res)
    ptr_data = ctypes.pointer(ctypes.pointer(desc_data))
    ptr_res = ctypes.pointer(ctypes.pointer(desc_res))
    engine.invoke('dynamic_square', ptr_data, ptr_res)
    return res

def square2(data):
    res = np.empty_like(data)
    for i in range(len(data)):
        res[i] = data[i] * data[i]
    return res

def square3(data):
    return np.square(data)

arr = np.array([3 for _ in range(100000)], dtype=np.float32)

start = time()
for i in range(100):
    square(arr)
square_mlir_s = time() - start

start = time()
for i in range(100):
    square2(arr)
square_python_s = time() - start

start = time()
for i in range(100):
    square3(arr)
square_numpy_s = time() - start

print(f'{square_python_s=}')
print(f'{square_mlir_s=}({round(square_python_s / square_mlir_s, 2)}x)')
print(f'{square_numpy_s=}({round(square_python_s / square_numpy_s, 2)}x)')