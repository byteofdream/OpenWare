import ctypes
import os
from pathlib import Path


def _candidate_library_names() -> list[str]:
    if os.name == "nt":
        return ["openware_api.dll"]
    if os.uname().sysname == "Darwin":
        return ["libopenware_api.dylib", "openware_api.dylib"]
    return ["libopenware_api.so", "openware_api.so"]


def _candidate_paths() -> list[Path]:
    here = Path(__file__).resolve()
    project_root = here.parents[2]
    build_dirs = ["build", "build_ninja", "cmake-build-debug", "cmake-build-release"]
    names = _candidate_library_names()

    paths: list[Path] = []
    for build_dir in build_dirs:
        for name in names:
            paths.append(project_root / build_dir / "api" / "c" / name)
            paths.append(project_root / build_dir / name)
    return paths


def load_c_api() -> ctypes.CDLL:
    override = os.getenv("OPENWARE_API_LIB")
    if override:
        return ctypes.CDLL(override)

    for path in _candidate_paths():
        if path.exists():
            return ctypes.CDLL(str(path))

    raise FileNotFoundError(
        "C API library was not found. Build the project first or set OPENWARE_API_LIB."
    )


def c_api_sum(a: int, b: int) -> int:
    lib = load_c_api()
    lib.ow_api_sum.argtypes = [ctypes.c_int, ctypes.c_int]
    lib.ow_api_sum.restype = ctypes.c_int
    return int(lib.ow_api_sum(a, b))

