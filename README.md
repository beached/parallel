# Header Libraries ![](https://github.com/beached/parallel/workflows/MacOS/badge.svg) ![](https://github.com/beached/parallel/workflows/Ubuntu/badge.svg) ![](https://github.com/beached/parallel/workflows/Windows/badge.svg)

## Description
A set of header only parallel helpers. Most should have tests located in the [tests](tests/) folder

## Building
to build
```bash
mkdir build
cd build
cmake -DCMAKE_BUILD_TYPE=DEBUG ..
cmake --build . --target full --config Debug
```

## Testing
To run unit tests

```bash
ctest -C Debug
```

## Installing

```bash
cmake --install .
```
