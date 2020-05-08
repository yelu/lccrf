lccrf
=====

A fast linear chain conditional random field(lccrf) implementation in C++ library. 

Resolver: [Averaged perceptron](http://www.cs.columbia.edu/~mcollins/papers/tagperc.pdf)

## Build

### Linux

```cmd
# build dependent libraries
cd superbuild
cmake -S . -B build -DCMAKE_TOOLCHAIN_FILE=`pwd`/../cmake/x64-linux.cmake
cmake --build build

# build
cd ..
cmake -S . -B build -DCMAKE_TOOLCHAIN_FILE=`pwd`/cmake/x64-linux.cmake
cmake --build build --target install
```

### Windows

```cmd
REM build dependent libraries
cd superbuild
cmake -S . -B build -G "Visual Studio 16 2019" -A x64 -DCMAKE_TOOLCHAIN_FILE=%cd%/../cmake/x64-windows.cmake
cmake --build build

REM build
cd ..
cmake -S . -B build -G "Visual Studio 16 2019" -A x64 -DCMAKE_TOOLCHAIN_FILE=%cd%/cmake/x64-windows.cmake
cmake --build build --target install
```

## Getting Started

### Data file format

It is a vw-like format. A blank line separates one query from another. Each line describes one word token.

```
[label id] ([importantce]) ([text]) | [f1](:[value]) [f2](:[value]) ...
```

For example:

```      
0 1.0 hello | 0:1.0
0 1.0 world | 1:1.0

0 1.0 i | 2:1.0
0 1.0 am | 3:1.0
1 1.0 crf | 4:1.0 5:2.0

```

API usage: [Test case](tests/test_perceptron.cpp)
