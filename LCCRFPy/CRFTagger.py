#!/usr/bin/env python

import os,sys
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
    
    def SaveXYToFile(self, x, y):
        xArray = x.to_array()
        yArray = y.to_array()
        with open('x', 'w') as f:
            for idx, i in enumerate(xArray):
                for j, features in i:
                    for feature in features:
                        feature_tuple = map(str, [idx,] + j + [feature,])
                        print >> f, "\t".join(feature_tuple)
        
        with open('y', 'w') as f:
            for idx, i in enumerate(yArray):
                for j, t in enumerate(i):
                    print >> f, "\t".join(map(str, [idx, j, t]))
    
    def Fit(self, xs, ys):
        self.fm.Fit(xs, ys)
        print >> sys.stderr, "vm.fit finished."
        x = self.fm.TransformX(xs)
        y = self.fm.TransformY(ys)
        print >> sys.stderr, "vm.Transform finished."
        #self.SaveXYToFile(x, y)
        #print >> sys.stderr, x.ToArray()
        #print >> sys.stderr, y.ToArray()
        self.crf = LCCRFPy(self.fm.FeatureCount, self.fm.TagCount)
        self.crf.Fit(x, y, 1000, 0.05, 0.0008)
        print >> sys.stderr, "Fit finished."
        return
        
    def Transform(self, xs, ys):
        x, _ = self.fm.Transform(xs)        
        y = Y()
        y = self.crf.Predict(x)
        yArray = y.toArray()
        for case in yArray:
            for i in range(0, len(case)):
                case[i] = self.tags[case[i]]
        return yArray
        
    def ReadableFeaturesAndWeights(self):
        res = []
        if self.crf != None:
            weights = self.crf.GetWeights()
        else:
            weights = [0.0] * self.fm.FeatureCount
        readableFeatures = self.fm.ReadableFeatures()
        for key, value in readableFeatures.items():
            res.append([key, value, weights[key - 1]])
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
        
    def test(self, docs):
        predicted_tages = self.Transform(docs)
        tags_map = {}
        total = 0
        right_tags = 0
        for i, doc in enumerate(docs):
            for j, token in enumerate(doc):
                total += 1
                true_tag = token[1]
                predicted_tag = predicted_tages[i][j]
                if true_tag not in tags_map:
                    tags_map[true_tag] = {"tp":0,"fp":0,"fn":0,"tn":0}
                if predicted_tag not in tags_map:
                    tags_map[predicted_tag] = {"tp":0,"fp":0,"fn":0,"tn":0}
                if predicted_tag == true_tag:
                    tags_map[true_tag]["tp"] += 1
                    right_tags += 1
                else:
                    tags_map[predicted_tag]["fp"] += 1
                    tags_map[true_tag]["fn"] += 1
        for key, value in tags_map.items():
            value["precision"] = 0.0
            if value["tp"] + value["fp"] != 0:
                value["precision"] = float(value["tp"]) / float(value["tp"] + value["fp"])
            value["recall"] = 0.0
            if value["tp"] + value["fn"] != 0:
                value["recall"] = float(value["tp"]) / float(value["tp"] + value["fn"])
            value["f1"] = 0.0
            if abs(value["precision"] + value["recall"]) > 1e-6:
                value["f1"] = 2 * value["precision"] * value["recall"] / float(value["precision"] + value["recall"])
        if right_tags == 0:
            tags_map["accuracy"] = 0.0
        else:
            tags_map["accuracy"] = float(right_tags) / float(total)
        return tags_map
        
