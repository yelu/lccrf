[![Build Status](https://travis-ci.org/yelu/LCCRF.svg?branch=master)](https://travis-ci.org/yelu/LCCRF)

LCCRF
=====

A practical Linear Chain Conditional Random Field(LCCRF) Library implemented in C++, with python driver provided.

This implementation features:

* Rich features. NGram, Transition, Regex, Contextual-Free-Grammar(cfg) features are built-in.
* API library. It can be embedded into your own code more easily and gracefully compared to calling a standalone binary.
* Fast. The core is written in C++, thus has good training performance.

## Get Started

**Prerequisites:**

1.gcc 4.9.0 or higher (with C++11 regex support)

    // In case you are using ubuntu 14.04, here is a tested instruction:
    sudo add-apt-repository ppa:ubuntu-toolchain-r/test
    sudo apt-get update
    sudo apt-get install gcc-4.9 g++-4.9
    sudo update-alternatives --install /usr/bin/g++ g++ /usr/bin/g++-4.9 90
    sudo update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-4.9 90

2.cython

    sudo apt-get install cython


**Install LCCRFPy:**

```bash
cd ./py/
export CC=/usr/bin/gcc  # This is to make sure you are using the right gcc version(>4.9.0)
python Setup.py build
sudo python Setup.py install
```

**Hello World**

There is an example datetime tagger in "examples/datetime" for extracting date/time from input query.

To train such a tagger, run

    python train.py -d train.tsv -m ./model/

To decode using the model just trained, run

    python decode.py -m ./model/
