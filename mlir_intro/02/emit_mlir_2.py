import ctypes
from mlir.ir import *
from mlir.dialects import func, arith, scf
from mlir.execution_engine import ExecutionEngine
from mlir.passmanager import PassManager

context = Context()
with context, Location.unknown():
    module = Module.create()
    f32 = F32Type.get()
    index_type = IndexType.get()

    with InsertionPoint(module.body):
        # f32 inputs (a, b) -> returns f32 result
        f = func.FuncOp('loop_sum', ([f32, f32], [f32]))
        f.operation.attributes["llvm.emit_c_interface"] = UnitAttr.get()

        with InsertionPoint(f.add_entry_block()):
            a, b = f.arguments[0], f.arguments[1]

            # 1. Compute upper bound: (a + b) converted to index
            total_f32 = arith.AddFOp(a, b)
            upper_bound_i32 = arith.FPToSIOp(IntegerType.get_signless(32), total_f32) # cast float to signed int, round down
            upper_bound = arith.IndexCastOp(index_type, upper_bound_i32) # Cast i32 to index

            # 2. Loop bounds: range(0, upper_bound, 1)
            lb = arith.ConstantOp.create_index(0)
            step = arith.ConstantOp.create_index(1)
            init_sum = arith.ConstantOp(f32, 0.0)

            # 3. Create scf.for loop with iter_args=[init_sum]
            # iter_args allows passing state through loop iterations
            loop = scf.ForOp(lb, upper_bound, step, [init_sum])
            
            with InsertionPoint(loop.body):
                iv = loop.induction_variable      # 'i' as index
                current_sum = loop.inner_iter_args[0] # accumulated sum

                # Convert loop index 'i' -> f32 so we can add it to current_sum
                i_i32 = arith.IndexCastOp(IntegerType.get_signless(32), iv)
                i_f32 = arith.SIToFPOp(f32, i_i32)

                # sum += i
                next_sum = arith.AddFOp(current_sum, i_f32)

                # Yield updated accumulator back to loop
                scf.YieldOp([next_sum])

            # 4. Return final sum from loop output
            func.ReturnOp([loop.results[0]])

    print('Module\n', module, sep="")

    # Lower SCF -> Control Flow -> LLVM Dialect
    pm = PassManager.parse("builtin.module(convert-scf-to-cf, convert-to-llvm)")
    pm.run(module.operation)

    print('After Passes\n', module, sep="")

    engine = ExecutionEngine(module)

# Wrapper function
def run_loop(a: float, b: float) -> float:
    arg1, arg2, res = ctypes.c_float(a), ctypes.c_float(b), ctypes.c_float(0.0)
    engine.invoke("loop_sum", ctypes.byref(arg1), ctypes.byref(arg2), ctypes.byref(res))
    return res.value

print(f"Result: {run_loop(5, 6)}")  # sum(0-10) = 55