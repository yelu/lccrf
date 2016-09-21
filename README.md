[![Build Status](https://travis-ci.org/yelu/LCCRF.svg?branch=master)](https://travis-ci.org/yelu/LCCRF)

LC-CRF
=====

A practical Linear Chain Conditional Random Field(LC-CRF) implemented in C++, with python driver provided.

This implementation features:

* Rich feature templates. NGram, Transition, Regex, Contextual-Free-Grammar(cfg) features are built-in.
* Speed. The core training component is well optimazied for fast training.
* API library. It can be embedded into your own code more easily and gracefully, compared to calling a standalone binary.

## Get Started

**Prerequisites:**

1.gcc 4.9.0 or higher (with C++11 regex support)

    // In case you are using ubuntu 14.04, here is an instruction:
    sudo add-apt-repository ppa:ubuntu-toolchain-r/test
    sudo apt-get update
    sudo apt-get install gcc-4.9 g++-4.9

2.cython

    sudo apt-get install cython


**Install LCCRFPy:**

```bash
cd ./py/
export CC=g++-4.9  # This is to make sure you are using the right gcc version(>4.9.0)
python Setup.py build
sudo python Setup.py install
```

**Hello World**

There is an example datetime tagger in "examples/datetime" for extracting date/time from input query.

To train such a tagger, run

    python train.py -d train.tsv -m ./model/

To decode using the model just trained, run

    python decode.py -m ./model/
