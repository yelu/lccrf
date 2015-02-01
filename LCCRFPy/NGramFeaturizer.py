#!/usr/bin/env python

class NGramFeaturizer:
    def __init__(self, ngram = 2):
        self._n = ngram
        self._features = set([])
    
    def GetFeatures(self):
        return self._features
        
    def Fit(self, doc):
        for i in range(self._n - 1, len(doc)):
            gram = tuple(doc[i + 1 - self._n : i + 1])
            self._features.add(gram)
        return self.Transform(doc)

    def Transform(self, doc):
        res = []
        for i in range(self._n - 1, len(doc)):
            gram = tuple(doc[i + 1 - self._n : i + 1])
            if gram in self._features:
                res.append((i + 1 - self._n, i, gram))
        return res

    @property
    def size(self):
        return len(self._features)
        
    def readable(self):
        readable_features = set([])
        for f in self._features:
            readable_features.add("ngram%d %s"%(self._n, f))
        return readable_features

