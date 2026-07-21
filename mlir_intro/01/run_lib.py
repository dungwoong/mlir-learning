import ctypes
from pathlib import Path

current_dir = Path(__file__).resolve().parent

module = ctypes.CDLL(current_dir / 'libsimple.so')
module.main.argtypes = []
module.main.restype = ctypes.c_int

print(module.main())