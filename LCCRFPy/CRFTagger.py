#!/usr/bin/env python

import os,sys
from log import *
from LCCRFPy import *
from FeaturizerManager import FeaturizerManager
from NGramFeaturizer import NGramFeaturizer
from AnyFeaturizer import AnyFeaturizer
import json

class CRFTagger:

    def __init__(self):
        self.fm = FeaturizerManager()
        self.crf = None

    def AddFeaturizer(self, name, featurizer, shift = 0, unigram = True, bigram = True):
        self.fm.AddFeaturizer(name, featurizer, shift, unigram, bigram)
    
    def Fit(self, xs, ys, \
            maxIteration = 1, \
            learningRate = 0.001, \
            variance = 0.001):
        self.fm.Fit(xs, ys)
        log.debug("fm.fit finished.")
        x = self.fm.TransformX(xs)
        y = self.fm.TransformY(ys)
        log.debug("fm.Transform finished.")
        self.crf = LCCRFPy(self.fm.FeatureCount, self.fm.TagCount)
        self.crf.Fit(x, y, maxIteration, learningRate, variance)
        log.debug("Fit finished.")
        return
        
    def Transform(self, xs):
        xInner = self.fm.TransformX(xs)        
        yInner = self.crf.Predict(xInner)
        ys = yInner.ToArray()
        ysWithOriginalTag = []
        for y in ys:
            ysWithOriginalTag.append(map(lambda x:self.fm._idToTag[x], y))
        return ysWithOriginalTag

    @staticmethod
    def Serialize(obj, directory):
        FeaturizerManager.Serialize(obj.fm, directory)
        if obj.crf != None:
            obj.crf.Save(directory + "/lccrf.weights.txt")

    @staticmethod
    def Deserialize(directory):
        tagger = CRFTagger()
        tagger.fm = FeaturizerManager.Deserialize(directory)
        tagger.crf = LCCRFPy()
        tagger.crf.Load(directory + "/lccrf.weights.txt")
        return tagger
        
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
        predictedTags = self.Transform(xs)
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
        
