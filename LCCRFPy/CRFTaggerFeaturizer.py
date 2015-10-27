#!/usr/bin/env python

import os
import sys
import json
from log import *

class CRFTaggerFeaturizer(object):
    def __init__(self):
        self._featurizers = []
        self._features = {}
        self._nextFeatureId = 0
        self._nextTagId = 0
        self._tags = {}
        self._idToTag = {}
    
    def AddFeaturizer(self, name, featurizer, shift = 0, uniTag = True, biTag = True):
        self._featurizers.append({"name" : name, "uniTag" : uniTag, "biTag" : biTag, \
                                   "shift" : shift, "instance": featurizer})
    
    def Fit(self, queries, tags):
        # collect all tags for future use.
        for tag in tags:
            for i in tag:
                if i not in self._tags:
                    self._tags[i] = self._nextTagId
                    self._nextTagId += 1
        # swap key and value in self._tags to reconstruct self._idToTag.
        self._idToTag = {value:key for key, value in self._tags.items()}

        xys = zip(queries, tags)
        for item in self._featurizers:
            name, featurizer, uniTag, biTag, shift = \
            item["name"], item["instance"], item["uniTag"], item["biTag"], item["shift"]
            # extract features on X.
            featurizer.Fit(queries)
            for xy in xys:
                x, y = xy
                # 1. extract features on X.
                featuresOfX = featurizer.Featurize([x], 0)[0]
                # matching features on X to Y
                for featureOfX in featuresOfX:
                    start, end = featureOfX[0] - shift, featureOfX[1] - shift
                    if start < 0 or start >= len(x) or \
                       end < 0 or end >= len(x):
                        continue
                    # 2. generate key based on X.
                    key = (name, shift, featureOfX[2])
                    # 3. combine X and Y
                    featuresOfY = set()
                    if uniTag:
                        featuresOfY.add(("uniTag", (self._tags[y[end]],)))
                    if biTag and end - 1 >= 0:
                        featuresOfY.add(("biTag", (self._tags[y[end - 1]], self._tags[y[end]])))

                    if key not in self._features:
                        self._features[key] = {}
                    for featureOfY in featuresOfY:
                        if featureOfY not in self._features[key]:
                            self._features[key][featureOfY] = self._nextFeatureId
                            self._nextFeatureId += 1

    def Featurize(self, queries, tags = None):
        for i, x in enumerate(queries):
            res =[]
            for item in self._featurizers:
                name, featurizer, shift = item["name"], item["instance"], item["shift"]
                featuresOfX = featurizer.Featurize([x], 0)[0]
                for featureOfX in featuresOfX:
                    start, end = featureOfX[0] - shift, featureOfX[1] - shift
                    if start < 0 or start >= len(x) or \
                       end < 0 or end > len(x):
                        continue
                    key = (name, shift, featureOfX[2])
                    if key not in self._features:
                        continue
                    featuresOfY = self._features[key]
                    for featureOfY, featureId in featuresOfY.items():
                        # if it is unigram
                        if featureOfY[0] == "uniTag":
                            prevTag = -1
                            currTag = featureOfY[1][0]
                            if not tags or self._tags[tags[i][end]] == currTag:
                                res.append((end, prevTag, currTag, featureId, featurizer.__class__.__name__))

                        # if it is bigram
                        if featureOfY[0] == "biTag" and end - 1 >= 0:
                            prevTag, currTag = featureOfY[1]
                            if not tags or (self._tags[tags[i][end]] == currTag and \
                                            self._tags[tags[i][end - 1]] == prevTag):
                                res.append((end, prevTag, currTag, featureId, featurizer.__class__.__name__))

            yield res
        
    def Tags(self):
        return self._tags
        
    def Features(self):
        allFeatures = {}
        for featureOfX, featuresOfY in self._features.items():
            for featureOfY, id in featuresOfY.items():
                allFeatures[id] = (featureOfX, featureOfY)
        return allFeatures

    def FeatureCount(self):
        return self._nextFeatureId

    def Serialize(self, dstFile):
        with open(dstFile, 'w') as f:
            config = {"tags" : self._nextTagId, "features" : self._nextFeatureId}
            print >> f, config
            featurizers = []
            for featurizer in self._featurizers:
                featurizer["instance"].Serialize(featurizer["name"])
                featurizers.append({"name" : featurizer["name"], \
                                    "uniTag" : featurizer["uniTag"], \
                                    "biTag" : featurizer["biTag"], \
                                    "shift" : featurizer["shift"], \
                                    "instance" : featurizer["instance"].__module__ + "." + featurizer["instance"].__class__.__name__, \
                                    "file" : featurizer["name"]})
            print >> f, featurizers
            print >> f, self._tags
            for featureOfX, featuresOfY in self._features.items():
                for featureOfY, id in featuresOfY.items():
                    print >> f, (id, featureOfX, featureOfY)

    @staticmethod
    def GetClass( kls ):
        parts = kls.split('.')
        module = ".".join(parts[:-1])
        m = __import__( module )
        for comp in parts[1:]:
            m = getattr(m, comp)
        return m

    @staticmethod
    def Deserialize(srcFile):
        with open(srcFile, 'r') as sf:
            f = CRFTaggerFeaturizer()
            config = eval(sf.readline().strip())
            f._nextFeatureId, f._nextTagId = config["features"], config["tags"]
            featurizers = eval(sf.readline().strip())
            for featurizer in featurizers:
                filePath = os.path.join(os.path.dirname(srcFile), featurizer["name"])
                f._featurizers.append({"name" : featurizer["name"], \
                                       "uniTag" : featurizer["uniTag"], \
                                       "biTag" : featurizer["biTag"], \
                                       "shift" : featurizer["shift"], \
                                       "instance" : CRFTaggerFeaturizer.GetClass(featurizer["instance"]).Deserialize(filePath)})
            f._tags = eval(sf.readline().strip())
            f._idToTag = {value:key for key, value in f._tags.items()}
            for line in sf:
                line = line.strip()
                if len(line) == 0:
                    continue
                id, featureX, featureY = eval(line)
                if featureX not in f._features:
                    f._features[featureX] = {}
                f._features[featureX][featureY] = id
                    
            return f

