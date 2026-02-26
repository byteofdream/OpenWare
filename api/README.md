# OpenWare API

This directory contains API examples in C and Python.

## Structure

- `c/` - C API library (`openware_api`)
- `python/` - Python HTTP API server using the C API via `ctypes`

## Build the C API

From the project root:

```sh
cmake -S . -B build
cmake --build build
```

After building, the `openware_api` library will be generated in the `build` directory.

## Run the Python API

```sh
python3 api/python/server.py
```

Available endpoints:

- `GET /health`
- `GET /version`
- `GET /sum?a=2&b=3`

If the C API library is not found automatically, set the path manually:

```sh
OPENWARE_API_LIB=/abs/path/to/libopenware_api.so python3 api/python/server.py
```
