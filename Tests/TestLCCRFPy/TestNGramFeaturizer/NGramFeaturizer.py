#!/usr/bin/env python
import os
import pickle

class NGramFeaturizer(object):
    def __init__(self, ngramFile, n, addBosEos = False):
        self._n = n
        self._addBosEos = addBosEos
        self._ngrams = set()
        with open(ngramFile, 'r') as f:
            for line in f:
                words = line.strip().split()
                self._ngrams.add(tuple(words))
    
    def GetFeatures(self):
        return self._features

    @staticmethod
    def SelectNGramToFile(docsFile, dstNGramFile, n, addBosEos = False):
        with open(docsFile, 'r') as inFile, open(dstNGramFile, 'w') as outFile:
            ngramSet = set()
            for line in inFile:
                words = list(line.strip().split())
                if addBosEos:
                    words.insert(0, "BOS")
                    words.append("EOS")
                for i in range(n - 1, len(words)):
                    ngram = tuple(words[i + 1 - n : i + 1])
                    if ngram not in ngramSet:
                        ngramSet.add(ngram)
                        print >> outFile, " ".join(list(ngram))

    def Featurize(self, words):
        res = []
        if self._addBosEos:
            words.insert(0, "BOS")
            words.append("EOS")
        for i in range(self._n - 1, len(words)):
            gram = tuple(words[i + 1 - self._n : i + 1])
            if gram in self._ngrams:
                res.append((i + 1 - self._n, i, gram))
        return res

