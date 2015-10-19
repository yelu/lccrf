#!/usr/bin/env python
import os

class NGramFeaturizer(object):
    def __init__(self, n, addBosEos = False):
        self._n = n
        self._addBosEos = addBosEos
        self._nextID = 0
        self._ngrams = {}
          
    def Dim(self):
        return len(self._ngrams)

    def Fit(self, queries):
        for query in queries:
            if self._addBosEos:
                query.insert(0, "BOS")
                query.append("EOS")
            for i in range(self._n - 1, len(query)):
                ngram = tuple(query[i + 1 - self._n : i + 1])
                if ngram not in self._ngrams:
                    self._ngrams[ngram] = self._nextID
                    self._nextID += 1

    def Serialize(self, dstFile):
        config = {"ngram" : self._n, "nextID" : self._nextID, "addBosEos" : self._addBosEos}
        with open(dstFile, 'w') as f:
            print >> f, config
            print >> f, self._ngrams

    @staticmethod
    def Deserialize(srcFile):
        with open(srcFile, 'r') as f:
            config = eval(f.readline().strip())
            featurizer = NGramFeaturizer(config["ngram"], config["addBosEos"])
            featurizer._nextID = config["nextID"]
            featurizer._ngrams = eval(f.readline().strip())
            return featurizer

    def Featurize(self, queries, idShift):
        res = []
        for query in queries:
            features = []
            if self._addBosEos:
                query.insert(0, "BOS")
                query.append("EOS")
            for i in range(self._n - 1, len(query)):
                ngram = tuple(query[i + 1 - self._n : i + 1])
                if ngram in self._ngrams:
                    features.append((i + 1 - self._n, i, self._ngrams[ngram] + idShift))
            res.append(features)
        return res

