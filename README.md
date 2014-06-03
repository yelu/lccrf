LCCRF
=====

A C++ implementation of Linear Chain Conditional Random Field. 

This implementation aims to:

* Be easy to understand. It follows the theoretical derivation and uses the same mathematical symbols in [Log-Linear Models, MEMMs, and CRFs](http://www.cs.columbia.edu/~mcollins/crf.pdf) and [The Forward-Backward Algorithm](http://www.cs.columbia.edu/~mcollins/fb.pdf) by [Michael Collins](http://www.cs.columbia.edu/~mcollins/).
* Be pure. The core algorithm(LCCRF) itself is strictly seperated from other distracting purposes of application, e.g. chunking, entity tagging and etc.
* Leave fetures flexible. LCCRF accepts a batch of hypothesises to extract features. So you can provide any kind of feature you want and apply it easily by expressing it in a function with signaure : double Hypothesis(X, y(j-1), y(j), j).
