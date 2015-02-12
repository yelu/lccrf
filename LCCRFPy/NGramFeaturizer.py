#!/usr/bin/env python
import os
import pickle

class NGramFeaturizer(object):
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

    @staticmethod
    def Serialize(obj, fileDir):
        filePath = os.path.join(fileDir, "ngram.bin")
        with open(filePath, 'w') as f:
            pickle.dump(obj, f)

    @staticmethod
    def Deserialize(fileDir):
        filePath = os.path.join(fileDir, "ngram.bin")
        with open(filePath, 'r') as f:
            obj = pickle.load(f)
            return obj

    def Readable(self):
        readable_features = set([])
        for f in self._features:
            readable_features.add("ngram%d %s"%(self._n, f))
        return readable_features

