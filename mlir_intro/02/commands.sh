python3 -m venv ~/.venv/mlirdev
source ~/.venv/mlirdev/bin/activate
pip install -r llvm-project/mlir/python/requirements.txt

# go to llvm build folder
cmake -G Ninja ../llvm \
    -DCMAKE_SKIP_RPATH=ON \
    -DLLVM_ENABLE_PROJECTS="mlir" \
    -DLLVM_BUILD_EXAMPLES=ON \
    -DMLIR_ENABLE_BINDINGS_PYTHON=ON \
    -DPython3_EXECUTABLE=$(which python) \
    -DLLVM_TARGETS_TO_BUILD="Native;NVPTX;AMDGPU" \
    -DCMAKE_BUILD_TYPE=Release \
    -DLLVM_ENABLE_ASSERTIONS=ON \
    -DCMAKE_C_COMPILER=clang-15 \
    -DCMAKE_CXX_COMPILER=clang++-15 \
    -DLLVM_ENABLE_LLD=ON \
    -DCMAKE_BUILD_WITH_INSTALL_RPATH=ON \
    -DMLIR_INCLUDE_INTEGRATION_TESTS=ON \
    -DCMAKE_C_VISIBILITY_PRESET=hidden \
    -DCMAKE_CXX_VISIBILITY_PRESET=hidden \
    -DCMAKE_VISIBILITY_INLINES_HIDDEN=ON


# This one works https://github.com/pyccel/pyccel/discussions/1539
cmake -G Ninja ../llvm \
   -DLLVM_ENABLE_PROJECTS=mlir \
   -DLLVM_BUILD_EXAMPLES=ON \
   -DLLVM_TARGETS_TO_BUILD="Native;NVPTX;AMDGPU" \
   -DCMAKE_BUILD_TYPE=Release \
   -DLLVM_ENABLE_ASSERTIONS=ON \
   -DMLIR_ENABLE_BINDINGS_PYTHON=TRUE \
   -DPython3_EXECUTABLE="Please specify python path using 'which python'"
