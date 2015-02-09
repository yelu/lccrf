[![Build Status](https://travis-ci.org/yelu/LCCRF.svg?branch=master)](https://travis-ci.org/yelu/LCCRF)

LCCRF
=====

A practical Linear Chain Conditional Random Field(LC CRF) Library implemented in C++. Python interface provided.

This implementation features:

* Rich features. NGram, Transition, Regex(in progress), Dictionary(in progress), Contextual-Free-Grammar(cfg, in progress) features are built-in.
* Extensible featurizers. By implementing self-defined featurizers, you can add any feature you need freely, e.g. a feature checking if two consecutive tags are the same.
* API library. It can be embedded into your own code more easily and gracefully compared to calling a standalone binary.
* Be easy to understand. It follows the theoretical derivation and uses the same mathematical symbols in [Log-Linear Models, MEMMs, and CRFs](http://www.cs.columbia.edu/~mcollins/crf.pdf) and [The Forward-Backward Algorithm](http://www.cs.columbia.edu/~mcollins/fb.pdf) by [Michael Collins](http://www.cs.columbia.edu/~mcollins/).

## Get Started

### Installition

Prerequisites:

```bash
# 1. gcc 4.8 or higher (with full C++11 support)
# 2. cython
sudo apt-get install cython
```

Install LCCRFPy:

```bash
cd ./LCCRFPy/
python Setup.py build
sudo python Setup.py install
```

### Hello World

```python
import os,sys
from CRFTagger import *

# training samples.
trainXs = ["what is the weather in shang hai".split(),
           "what is the weather in hong kong".split()]
trainYs = [["O", "O", "O", "O", "O", "City", "City"],
           ["O", "O", "O", "O", "O", "City", "City"],
          ]

# 1. instantiate a CRFTagger and add three featurizers.
tagger = CRFTagger()
# add unigram(on x) featurizer : current word together with current tag.
tagger.AddFeaturizer("ngram1", NGramFeaturizer(1),\
                     shift = 0, unigram = True, bigram = False)
# add bigram(on x) featurizer : current bi-word together with current tag.
tagger.AddFeaturizer("ngram2", NGramFeaturizer(2), 
                     shift = 0, unigram = True, bigram = False)
# add any featurizer : trigger a feature at any position of x, 
# the final feature depends purely on transitions of y.
tagger.AddFeaturizer("any", AnyFeaturizer(), 
                      shift = 0, unigram = False, bigram = True)
# 2. train a crf model.
tagger.Fit(trainXs, trainYs, \
           maxIteration = 1000,\
           learningRate = 0.05, \
           variance = 0.0008)
print "Features : %d    Tags : %d" % (tagger.fm.FeatureCount, \
                                      tagger.fm.TagCount)

# 3. inference on new data.
xs = ["what is the weather in bei jing".split()]
ys = tagger.Transform(xs)
print ys
```

