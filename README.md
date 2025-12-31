# PentaLedger


## Requirements

- C++20 compatible compiler (GCC 10+, Clang 12+, MSVC 2019+)
- CMake 3.15 or higher
- Google Test (automatically downloaded via FetchContent)

## Building

```bash
mkdir build
cd build
cmake ..
make
```


## Architecture

The library provides a low-level data file management system:

1. **DataFile**: Random access file with custom header containing record pointers and metadata

## Testing

Run the test suite:

```bash
cd build
ctest
```

Or run tests directly:

```bash
./tests/pentaledger_tests
```

## Examples

See `examples/basic_usage.cpp` for a complete example.

## License

GPL-2.0
