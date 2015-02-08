LCCRF
=====

A Linear Chain Conditional Random Field Library implemented in C++. Python interface provided.

This implementation features:

* Be easy to use. It is an API library, thus can be embeded into your own code more easily and gracefully compared to calling a standalone binary.
* Rich features. NGram, Transition, Regex(in progress), Bag-of-Words(in progress), Contextual-Free-Grammar(cfg, in progress) features are built-in.
* Extensible featurizer. It allows you to add any features you need freely by implementing self-defined featurizers, e.g. a featurizer checking if two consecutive tags are the same. It is difficult to add such kind of features by using CRF++, crfpp or crfsuite.
* Be easy to understand. It follows the theoretical derivation and uses the same mathematical symbols in [Log-Linear Models, MEMMs, and CRFs](http://www.cs.columbia.edu/~mcollins/crf.pdf) and [The Forward-Backward Algorithm](http://www.cs.columbia.edu/~mcollins/fb.pdf) by [Michael Collins](http://www.cs.columbia.edu/~mcollins/).

## Get Started

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
tagger.AddFeaturizer("ngram1", NGramFeaturizer(1), shift = 0, unigram = True, bigram = False)
# add bigram(on x) featurizer : current bi-word together with current tag.
tagger.AddFeaturizer("ngram2", NGramFeaturizer(2), shift = 0, unigram = True, bigram = False)
# add an any featurizer : trigger a feature at any position of x, 
# thus a transition featurizer purely on y.
tagger.AddFeaturizer("any", AnyFeaturizer(), shift = 0, unigram = False, bigram = True)
# 2. train a crf model.
tagger.Fit(trainXs, trainYs)
print >> sys.stderr, "Feature Count : %d    Tag Count : %d" % (tagger.fm.FeatureCount, \
                                                               tagger.fm.TagCount)

xs = ["what is the weather in bei jing".split()]
# 3. inference on new data.
ys = tagger.Transform(xs)
print ys
```

