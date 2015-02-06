#!/usr/bin/env python

import sys
import json
from LCCRFPy import X,Y,XSample,YSample

class FeaturizerManager:
    def __init__(self):
        self._featurizers = {}
        self._features = {}
        self._nextFeatureId = 0
        self._nextTagId = 0
        self._tags = {}
    
    def AddFeaturizer(self, name, featurizer, shift = 0, unigram = True, bigram = True):
        self._featurizers[name] = {"unigram" : unigram, "bigram" : bigram, "shift" : shift, "instance":featurizer}
    
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

    def TransformY(self, ys):
        yInner = Y()
        for i, y in enumerate(ys):
            yInner.Append(YSample())
            for idx, tag in enumerate(y):
                yInner[-1, idx] = self._tags[tag]
            print >> sys.stderr, "\rtransforming y ... [%d/%d] " % (i + 1, len(ys)),
        print >> sys.stderr, "done."
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

            print >> sys.stderr, "\rtransforming x ... [%d/%d] " % (i + 1, len(xs)),
        print >> sys.stderr, "done."
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
