#!/usr/bin/env python

import os,sys
from collections import defaultdict
from log import *
try:
    from LCCRFPy import *
except:
    pass
from FeaturizerManager import FeaturizerManager
from NGramFeaturizer import NGramFeaturizer
from AnyFeaturizer import AnyFeaturizer
import json

class CRFTagger:

    def __init__(self):
        self.fm = FeaturizerManager()
        self.crf = None
        self.weights = {}

    def Train(self, docs, tags, modelDir, \
            maxIteration = 1, \
            learningRate = 0.001, \
            variance = 0.001):
        if not os.path.exists(modelDir):
            os.makedirs(modelDir)

        # prepare featurizers.
        unigramFile = os.path.join(modelDir, "unigram.txt")
        bigramFile = os.path.join(modelDir, "bigram.txt")
        trigramFile = os.path.join(modelDir, "trigram.txt")
        NGramFeaturizer.SelectNGramToFile(docs, unigramFile, 1)
        NGramFeaturizer.SelectNGramToFile(docs, bigramFile, 2)
        NGramFeaturizer.SelectNGramToFile(docs, trigramFile, 3)

        unigramFeaturizer = NGramFeaturizer(unigramFile, 1)
        bigramFeaturizer = NGramFeaturizer(bigramFile, 2)
        trigramFeaturizer = NGramFeaturizer(trigramFile, 3)
        
        self.fm.AddFeaturizer("unigram", unigramFeaturizer, shift = 0, unigram = True, bigram = False)
        self.fm.AddFeaturizer("bigram", bigramFeaturizer, shift = 0, unigram = True, bigram = False)
        #self.fm.AddFeaturizer("trigram", trigramFeaturizer, shift = 0, unigram = True, bigram = False)
        self.fm.AddFeaturizer("any", AnyFeaturizer(), shift = 0, unigram = False, bigram = True)
        
        self.fm.Fit(docs, tags)
        log.debug("fm.fit finished.")
        
        featureFile = os.path.join(modelDir, "features.bin")
        self.fm.Dump(featureFile)

        # train crf model.
        x = self.fm.FeaturizeX(docs)
        y = self.fm.FeaturizeY(tags)
        self.crf = LCCRFPy(self.fm.FeatureCount, self.fm.TagCount)
        self.crf.Fit(x, y, maxIteration, learningRate, variance)
        log.debug("lccrf train finished.")
        modelFile = os.path.join(modelDir, "weights.txt")
        self.crf.Save(modelFile)
        return

    def Load(self, modelDir):
        featureFile = os.path.join(modelDir, "features.bin")
        self.fm = FeaturizerManager.Load(featureFile)

        modelFile = os.path.join(modelDir, "weights.txt")
        with open(modelFile, 'r') as f:
            next(f)
            for line in f:
                fields = line.strip().split()
                if len(fields) != 2:
                    continue
                self.weights[int(fields[0])] = float(fields[1])

        # prepare featurizers.
        unigramFile = os.path.join(modelDir, "unigram.txt")
        bigramFile = os.path.join(modelDir, "bigram.txt")
        trigramFile = os.path.join(modelDir, "trigram.txt")

        unigramFeaturizer = NGramFeaturizer(unigramFile, 1)
        bigramFeaturizer = NGramFeaturizer(bigramFile, 2)
        trigramFeaturizer = NGramFeaturizer(trigramFile, 3)
        
        self.fm.AddFeaturizer("unigram", unigramFeaturizer, shift = 0, unigram = True, bigram = False)
        self.fm.AddFeaturizer("bigram", bigramFeaturizer, shift = 0, unigram = True, bigram = False)
        #self.fm.AddFeaturizer("trigram", trigramFeaturizer, shift = 0, unigram = True, bigram = False)
        self.fm.AddFeaturizer("any", AnyFeaturizer(), shift = 0, unigram = False, bigram = True)

    def Predict(self, xs):
        res = []
        for x in xs:
            xFeatures = self.fm.FeaturizeXPy([x])[0]
            edges, nodes = self.MakeEdgesAndNodes(xFeatures, self.weights)
            yIDs = self.VeterbiDecode(edges, nodes, len(x), self.fm.TagCount)
            yWithOriginalTag = map(lambda x:self.fm._idToTag[x], yIDs)
            res.append(yWithOriginalTag)
        return res

    def MakeEdgesAndNodes(self, xFeatures, weights):
        edges = defaultdict(lambda : 0.0)
        nodes = defaultdict(lambda : 0.0)
        for feature in xFeatures:
            pos, preTag, curTag, featureID = feature[0], feature[1], feature[2], feature[3]
            if preTag >= 0:
                key = (pos, preTag, curTag)
                edges[key] += weights[featureID]
            else:
                key = (pos, curTag)
                nodes[key] += weights[featureID]
        return (edges, nodes)

    def VeterbiDecode(self, edges, nodes, nStep, nState):
        phi = [0.0] * nState
        for i in range(nState):
            phi[i] = nodes[(0, i)]

        newPhi = [float('-inf')] * nState

        backtraceMatrix = defaultdict(lambda : -1)
        for j in range(1, nStep):
            for s1 in range(0, nState):
                for s2 in range(0, nState):
                    d = phi[s2] + edges[(j, s2, s1)] + nodes[(j, s1)]
                    if d > newPhi[s1]:
                        backtraceMatrix[(j, s1)] = s2
                        newPhi[s1] = d
            phi, newPhi = newPhi, phi
        
        # find the optimal path
        maxJ = -1
        res = [-1] * nStep
        maxPath = float('-inf')
        for s in range(0, nState):
            if phi[s] > maxPath:
                maxPath = phi[s]
                maxJ = s
        
        res[nStep - 1] = maxJ
        for j in range(nStep - 2, -1, -1):
            res[j] = backtraceMatrix[(j + 1, res[j + 1])]

        return res

    def ReadableFeaturesAndWeights(self):
        res = []
        if self.crf != None:
            weights = self.crf.GetWeights()
        else:
            weights = [0.0] * self.fm.FeatureCount
        readableFeatures = self.fm.ReadableFeatures()
        for key, value in readableFeatures.items():
            res.append([key, value, weights[key]])
        return res
    
    def SaveReadableFeaturesAndWeights(self, filePath):
        featureWeight = self.ReadableFeaturesAndWeights()
        with open(filePath, 'w') as f:
            for featureId, featureName, weight in featureWeight:
                print >> f, "%s\t%s\t%f" % (featureId, featureName, weight)
     
    def Debug(self, x):
        x, y = self.fm.Transform([x])
        debugInfo = self.crf.debug(x[0], y[0])
        res = {"path":{}, "score":debugInfo[1]}
        for idx, step in enumerate(debugInfo[0]):
            res["path"]["position %s"%idx] = {"features":[], "tag":doc[idx][1]}
            for feature in step:
                res["path"]["position %s"%idx]["features"].append((self.readable_features[feature[0]], feature[1]))
        return res
        
    def Test(self, xs, ys):
        predictedTags = self.Predict(xs)
        stat = {}
        totalTags = 0
        rightTag = 0
        xys = zip(xs, ys)
        for i, xy in enumerate(xys):
            x, y = xy
            for j, word in enumerate(x):
                totalTags += 1
                expectedTag = y[j]
                predictedTag = predictedTags[i][j]
                if expectedTag not in stat:
                    stat[expectedTag] = {"tp":0,"fp":0,"fn":0,"tn":0}
                if predictedTag not in stat:
                    stat[predictedTag] = {"tp":0,"fp":0,"fn":0,"tn":0}
                if predictedTag == expectedTag:
                    stat[expectedTag]["tp"] += 1
                    rightTag += 1
                else:
                    stat[predictedTag]["fp"] += 1
                    stat[expectedTag]["fn"] += 1
        for key, value in stat.items():
            value["precision"] = 0.0
            if value["tp"] + value["fp"] != 0:
                value["precision"] = float(value["tp"]) / float(value["tp"] + value["fp"])
            value["recall"] = 0.0
            if value["tp"] + value["fn"] != 0:
                value["recall"] = float(value["tp"]) / float(value["tp"] + value["fn"])
            value["f1"] = 0.0
            if abs(value["precision"] + value["recall"]) > 1e-6:
                value["f1"] = 2 * value["precision"] * value["recall"] / float(value["precision"] + value["recall"])
        if rightTag == 0:
            stat["accuracy"] = 0.0
        else:
            stat["accuracy"] = float(rightTag) / float(totalTags)
        return stat
        
