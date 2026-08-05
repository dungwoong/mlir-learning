#ifndef TENS_OPS
#define TENS_OPS

include "mlir/IR/OpBase.td"
include "mlir/Interfaces/FunctionInterfaces.td"
include "mlir/IR/SymbolInterfaces.td"
include "mlir/Interfaces/SideEffectInterfaces.td"


def Tens_Dialect : Dialect {
    let name = "tens";

    let summary = "tens dialect";
    let description = [{
        This will include matmul and elementwise and that's about it
    }];

    let cppNamespace = "::mlir::tens";
}


// name.mnemonic e.g. tens.matmul would be Tens_Op<"matmul">
// traits are properties attached to the operation
class Tens_Op<string mnemonic, list<Trait> traits = []> :
    Op<Tens_Dialect, mnemonic, traits>;

// what you can look for
// getResult method
// namespace ::mlir::tens around everything
// parse and print methods
// Pure: op doesn't read/write to memory, no hidden state, etc.
def OnesOp : Tens_Op<"ones", [Pure]> {
    let summary = "ones";
    let description = [{
        Ones operation
    }];

    // shape/type is conveyed by the result type
    let arguments = (ins);

    // AnyTensor is pre-defined in OpBase.td, just any kind of tensor
    // result is just the variable name of this thing, it will auto-gen getResult()
    let results = (outs F64Tensor:$result);

    // no builders required, since we infer shape from result
    // MLIR will generate default builders

    let assemblyFormat = "attr-dict `:` type($result)";
}

def SquareOp : Tens_Op<"square", [Pure]> {
    let summary = "elementwise square";
    let description = [{
        Square stuff.
    }];

    let arguments = (ins F64Tensor:$x);
    let results = (outs F64Tensor:$result);

    // you want to find that build() takes in a Value(mlir Value) x
    let builders = [
        OpBuilder<(ins "Value":$x)>
    ];

    // without type($result) you get an error result is not buildable and a buildable type cannot be inferred
    let assemblyFormat = "$x attr-dict `:` type($x) `->` type($result)";
}

// need stuff after endif to parse correctly
#endif
