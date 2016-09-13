#!/usr/bin/env python
import os
import argparse

class NGramFeaturizer(object):
    def __init__(self, args):
        self.args = args
        self._ngrams = {}
        with open(self.args.ngramFile, 'r') as f:
            for line in f:
                fields = line.strip().split('\t')
                if len(fields) < 2:
                    continue
                nGram, id = fields[1], fields[0]
                self._ngrams[nGram] = id

    @staticmethod GetArgParser():
        argParser = argparse.ArgumentParser(description = 'NGram featurizer')
        argParser.add_argument('--input', action="store", dest="input", default="")
        argParser.add_argument('--output', action="store", dest="output", default="")
        argParser.add_argument('--ngramFile', action="store", dest="ngramFile", default="")
        argParser.add_argument('--n', action="store", dest="n", default="1", type=int)
        argParser.add_argument('--addBosEos', action="store_true", dest="addBosEos", default=False)
        return argParser

    @staticmethod
    def GenerateNGrams(quries, ngramFile, n, addBosEos = False):
        nGrams = {}
        nextID = 0
        for query in queries:
            tokenizedQuery = query.trim().split()
            if addBosEos:
                tokenizedQuery.insert(0, "BOS")
                tokenizedQuery.append("EOS")
            for i in range(n - 1, len(tokenizedQuery)):
                ngram = tuple(tokenizedQuery[i + 1 - n : i + 1])
                if ngram not in nGrams:
                    nGrams[ngram] = 0
                    nextID += 1
        if not os.path.exists(os.path.dirname(ngramFile)):
            os.makedirs(os.path.dirname(ngramFile))
        with open(ngramFile) as f:
            for ngram, id in nGrams:
                print >> f, "%s\t%s" % (id, ngram)

    def Process(self, input):
        query = input[0]
        output = self.Featurize(query)
        return output

    def Featurize(self, queries):
        tokenizedQuery = query.trim().split()
        features = []
        if self.args.addBosEos:
            tokenizedQuery.insert(0, "BOS")
            tokenizedQuery.append("EOS")
        for i in range(self.args.n - 1, len(tokenizedQuery)):
            ngram = tuple(tokenizedQuery[i + 1 - self.args.n : i + 1])
            if ngram in self._ngrams:
                features.append((i + 1 - self.args.n, i + 1, self._ngrams[ngram]))
        return features
