lccrf
=====

A fast linear chain conditional random field(lccrf) implemention in C++. The resolver is l1-regularized sgd. Perceptron resolver is in progress.

This distribution includes:

  * a api lib. It can be embedded into your own code easily and gracefully, compared to calling a standalone binary.
  * a console exe. For detailed usage, see following sections.

## Build

### Windows

```cmd
REM build dependent libraries
cd superbuild
cmake -S . -B build -G "Visual Studio 16 2019" -A x64 -DCMAKE_TOOLCHAIN_FILE=%cd%/../cmake/x64-windows.cmake
cmake --build build

REM build
cd ..
cmake -S . -B build -G "Visual Studio 16 2019" -A x64 -DCMAKE_TOOLCHAIN_FILE=%cd%/cmake/x64-windows.cmake
cmake --build build
```

## Get Started

**sample command**

lccrf.exe -d "data.txt" -m "model.bin" -i 10 -s 0.01 -l 0.01

**arguments**

-d Data file to train the model. 

A blank line sperates query from query. Within each query, each line is a word token. Within each line, the first integer is the label id and the remaining integers are features triggered at current word token.

    [label id] [f1] [f2] ...             <- query #0, word #0
    [label id] [f1] [f2] [f3] ...        <- query #0, word #1
                                         <- a blank line seperates query #0 and #1
    [label id] [f1] [f2] [f3] ...        <- query #1, word #0
    [label id] [f1] [f2] ...             <- query #1, word #1
    [label id] [f1] [f2] [f3] [f4] ...   <- query $1, word #2
    
An example:
        
    0 0 1 2 3 4 5 6
    0 7 8 9 10 11 12 13 14

    0 15 16 17 18 19 4 20 21
    0 22 23 24 25 26 27 28 12 29 30 31
    0 32 33 34 35 36 37 38 39 40 41 42 43
    5 44 45 46 47 48 49 50 51 52 53 54 55 6 56
    2 57 58 59 60 61 62 63 64 13 65


-m Target path to save the model file.

-i Max iteration number. There will be an early stop if the loss improvment between too epoches is lower than a threshold.

-s Learning step size.

-l L1 regularization strength.
