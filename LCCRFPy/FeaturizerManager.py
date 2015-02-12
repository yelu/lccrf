#!/usr/bin/env python

import os
import sys
import json
import pickle
from log import *
from LCCRFPy import X,Y,XSample,YSample

class FeaturizerManager(object):
    def __init__(self):
        self._featurizers = {}
        self._features = {}
        self._nextFeatureId = 0
        self._nextTagId = 0
        self._tags = {}
        self._idToTag = {}
    
    def AddFeaturizer(self, name, featurizer, shift = 0, unigram = True, bigram = True):
        self._featurizers[name] = {"unigram" : unigram, "bigram" : bigram, "shift" : shift, \
                                   "instance":featurizer, "type" : type(featurizer)}
    
    def Fit(self, xs, ys):
        xys = zip(xs, ys)
        for key, value in self._featurizers.items():
            name = key
            featurizer = value["instance"]
            isUnigram = value["unigram"]
            isBigram = value["bigram"]
            shift = value["shift"]
            for xy in xys:
                x, y = xy
                # collect all tags for future use.
                for tag in y:
                    if tag not in self._tags:
                        self._tags[tag] = self._nextTagId
                        self._nextTagId += 1
                # 1. extract features on X.
                features = featurizer.Fit(x)
                # matching features on X to Y
                for feature in features:
                    start, end = feature[0] - shift, feature[1] - shift
                    if start < 0 or start >= len(xy[0]) or \
                       end < 0 or end >= len(xy[0]):
                        continue
                    # 2. generate key based on X.
                    featureOfX = (name, shift, feature[2])
                    # 3. combine X and Y
                    featuresOfY = set()
                    if isUnigram:
                        featuresOfY.add(("unigram", (y[end],)))
                    if isBigram and end - 1 >= 0:
                        featuresOfY.add(("bigram", (y[end - 1], y[end])))

                    if featureOfX not in self._features:
                        self._features[featureOfX] = {}
                    for featureOfY in featuresOfY:
                        if featureOfY not in self._features[featureOfX]:
                            self._features[featureOfX][featureOfY] = self._nextFeatureId
                            self._nextFeatureId += 1
        # swap key and value in self._tags to reconstruct self._idToTag.
        self._idToTag = {value:key for key, value in self._tags.items()}

    def TransformY(self, ys):
        yInner = Y()
        for i, y in enumerate(ys):
            yInner.Append(YSample())
            for idx, tag in enumerate(y):
                yInner[-1, idx] = self._tags[tag]
            log.debug("transforming y ... [%d/%d] " % (i + 1, len(ys)))
        return yInner
        
    def TransformX(self, xs):
        xInner = X()
        for i, x in enumerate(xs):
            xInner.Append(XSample(len(x)))
            for key, value in self._featurizers.items():
                name = key
                featurizer = value["instance"]
                shift = value["shift"]
                features = featurizer.Transform(x)
                for feature in features:
                    start, end = feature[0] - shift, feature[1] - shift
                    if start < 0 or start >= len(x) or \
                       end < 0 or end > len(x):
                        continue
                    featureOfX = (name, shift, feature[-1])
                    if featureOfX not in self._features:
                        continue
                    featuresOfY = self._features[featureOfX]
                    for featureOfY, featureId in featuresOfY.items():
                        # if it is unigram
                        if featureOfY[0] == "unigram":
                            prevTags = self._tags.values()
                            if end == 0:
                                prevTags = [-1]
                            currTag = featureOfY[1][0]
                            for prevTag in prevTags:
                                xInner[-1][end, prevTag, self._tags[currTag], featureId] = 1.0

                        # if it is bigram
                        if featureOfY[0] == "bigram" and end - 1 >= 0:
                            prevTag, currTag = featureOfY[1]
                            xInner[-1][end, self._tags[prevTag], self._tags[currTag], featureId] = 1.0

            log.debug("transforming x ... [%d/%d] " % (i + 1, len(xs)))
        return xInner
        
    @property
    def FeatureCount(self):
        return self._nextFeatureId
        
    @property
    def TagCount(self):
        return self._nextTagId
        
    def ReadableFeatures(self):
        allFeatures = {}
        for featureOfX, featuresOfY in self._features.items():
            for featureOfY, id in featuresOfY.items():
                allFeatures[id] = (featureOfX, featureOfY)
        return allFeatures

    @staticmethod
    def Serialize(obj, directory):
        # serialize featurizers.
        for name, featurizer in obj._featurizers.items():
            instance = featurizer["instance"]
            # if featurizer don't have a serialize method.
            # use pickle to serialize it. file name is name of
            # the featurizer.
            fileDir = os.path.join(directory, name)
            if not os.path.exists(fileDir):
                os.makedirs(fileDir)
            if "Serialize" not in dir(instance):
                filePath = os.path.join(fileDir, name + ".bin")
                with open(filePath, 'w') as f:
                    pickle.dump(instance, f)
            else:
                getattr(instance, "Serialize")(instance, fileDir)
        
        # remove featurizer instance and pickle the fm. then recover it.
        featurizersBackup = {}
        for name, featurizer in obj._featurizers.items():
            featurizersBackup[name] = featurizer["instance"]
            featurizer["instance"] = None

        with open(os.path.join(directory, "fm.bin"), 'w') as f:
            pickle.dump(obj, f)

        for name, featurizer in obj._featurizers.items():
            featurizer["instance"] = featurizersBackup[name]
   
    @staticmethod
    def Deserialize(directory):
        fm = FeaturizerManager()
        with open(os.path.join(directory, "fm.bin"), 'r') as f:
            fm = pickle.load(f)

        # deserialize featurizers.
        for name, featurizer in fm._featurizers.items():
            fileDir = os.path.join(directory, name)
            if "Deserialize" in dir(featurizer["type"]):
                featurizer["instance"] = getattr(featurizer["type"], "Deserialize")(fileDir)
            else:
                with open(os.path.join(fileDir, name + ".bin")) as f:
                    featurizer["instance"] = pickle.load(f)

        return fm

