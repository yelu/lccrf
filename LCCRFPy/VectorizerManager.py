#!/usr/bin/env python

import sys
import json
from LCCRFPy import X,Y,XSample,YSample

class VectorizerManager:
    def __init__(self):
        self._featurizers = {}
        self._features = {}
        self._features_to_ids = {}
        self._ids_to_features = {}
        self._maxFeatureId = 0
        self._maxTagId = 0
        self._tags = {}
    
    def allocate_featureid(self):
        self.next_featureid += 1
        return self.next_featureid - 1
        
    def get_or_allocate_tagid(self, tagname):
        if tagname not in self.tagname_to_tagid:
            self.tagname_to_tagid[tagname] = self.next_tagid
            self.tagid_to_tagname[self.next_tagid] = tagname
            self.next_tagid += 1
        return self.tagname_to_tagid[tagname]
    
    def get_tagname(self, tagid):
        if tagid in self.tagid_to_tagname:
            return self.tagid_to_tagname[tagid]
        else:
            return ""
          
    def AddFeaturizer(self, name, featurizer, unigram = True, bigram = True, shift = 0):
        self._featurizers[name] = {"unigram" : unigram, "bigram" : bigram, "shift" : shift, "instance":featurizer}
    
    def allocate_tagid(self, docs):
        for doc in docs:
            for _, tag in doc:
                self.get_or_allocate_tagid(tag)

    def _GenCombinedUnigramFeature(featureOfX, allTags):
        idx = featureOfX[1]
        tagRange = allTags
        res = []
        if idx == 0:
            tagRange = ['']
        for i in tagRange:
            res.append((featureOfX[0], featureOfX[1], "unigram", \
                        featureOfX[2], (i, )))
        return res

    def _GenCombinedBigramFeature(featureOfX, allTags):
        idx = featureOfX[1]
        tagRange = allTags
        res = []
        if idx == 0:
            tagRange = ['']
        for i in tagRange:
            res.append((featureOfX[0], featureOfX[1], "unigram", \
                        featureOfX[2], (i, )))
        return res
       
    def Fit(self, xs, ys):
        xys = zip(xs, ys)
        for key, value in self._featurizers.items():
            name = key
            featurizer = value["instance"]
            isUnigram = value["unigram"]
            isBigram = value["bigram"]
            shift = value["shift"]
            # extract features on X.
            for xy in xys:
                x, y = xy
                # collect all tags for future use.
                for tag in y:
                    if tag not in self._tags:
                        self._maxTagId += 1
                        self._tags[tag] = self._maxTagId
                features = featurizer.Fit(x)
                # matching features on X to Y
                for feature in features:
                    start, end = feature[0] - shift, feature[1] - shift
                    if start < 0 or start >= len(xy[0]) or \
                       end < 0 or end >= len(xy[0]):
                        continue
                    # 1. get shifted features on X
                    featureOfX = (name, shift, feature[2])
                    # 2. combine X and Y
                    featuresOfY = set()
                    if isUnigram:
                        featuresOfY.add(("unigram", (y[end],)))
                    if isBigram and end - 1 >= 0:
                        featuresOfY.add(("bigram", (y[end - 1], y[end])))

                    if featureOfX not in self._features:
                        self._features[featureOfX] = {}
                    for featureOfY in featuresOfY:
                        if featureOfY not in self._features[featureOfX]:
                            self._maxFeatureId += 1
                            self._features[featureOfX][featureOfY] = self._maxFeatureId
            
    def Transform(self, xs, ys):
        xInner = X()
        yInner = Y()
        xys = zip(xs, ys)
        for i, xy in enumerate(xys):
            x, y = xy
            xInner.append(XSample(len(x)))
            for key, value in self.__featurizers.items():
                name = key
                featurizer = value["instance"]
                shift = value["shift"]
                features = featurizer.Transform(x)
                for feature in features:
                    start, end = feature[0] - shift, feature[1] - shift
                    if start < 0 or start >= len(x) or \
                       end < 0 or end > len(x):
                        continue
                    featureOfX = (name, shift, feature)
                    if featureOfX not in self._features:
                        continue
                    featuresOfY = self._features[featureOfX]
                    for featureOfY, featureId in featuresOfY.items():
                        # if it is unigram
                        if featureOfY[0] == "unigram":
                            prevTags = self._tags.value()
                            if idx == 0:
                                prevTags = [-1]
                            currTag = featureOfY[1](0)
                            for prevTag in prevTags:
                                x[-1][end, prevTag, self._tags[currTag], featureId] = 1.0

                        # if it is bigram
                        if featureOfY[0] == "bigram":
                            prevTag, currTag = featureOfY[1]
                            x[-1][end, self._tags[prevTag], self._tags[currTag], featureId] = 1.0
                           
            yInner.append(YSample())
            for idx, tag in enumerate(y):
                y[-1, idx] = self._tags(tag)
            print >> sys.stderr, "transforming... [%d/%d]\r" % (i + 1, len(xs)),
                
        return (xInner, yInner)
        
    @property
    def feature_count(self):
        return self.next_featureid
        
    @property
    def tag_count(self):
        return len(self.tagid_to_tagname)
        
    def readable_features(self):
        all_features = {}
        for v in self.__vectorizer:
            all_features.update(v.readable_features())
        return all_features
