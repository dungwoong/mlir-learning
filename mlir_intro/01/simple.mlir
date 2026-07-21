// produces output of type <index> (built-in type)
// index is also a dialect so you can do index.constant
func.func @loop_add() -> (index) {
    %init = index.constant 0
    %lb = index.constant 0
    %ub = index.constant 10
    %step = index.constant 1

    // more like a JS array.reduce()
    // - store final result into sum
    // %iv is the variable, goes from lb to ub
    // Initial %acc = init, return a type <index> as %sum
    %sum = scf.for %iv = %lb to %ub step %step iter_args(%acc = %init) -> (index) {
        // add %iv + %acc
        %sum_next = arith.addi %acc, %iv : index

        // this yield becomes %acc for the next cycle, and is the return value at the end
        scf.yield %sum_next : index
    }

    return %sum : index
}

func.func @main() -> i32 {
    %out = call @loop_add() : () -> index
    %out_i32 = arith.index_cast %out : index to i32
    func.return %out_i32 : i32
}