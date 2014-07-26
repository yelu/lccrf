#!/usr/bin/env python

import os,sys
from LCCRFPy import *
from VectorizerManager import VectorizerManager
from NGramVectorizer import NGramVectorizer
from TransitionVectorizer import TransitionVectorizer
import json

class CRFTagger:

    def __init__(self):
        self.vm = VectorizerManager()
        self.vm.add_vectorizer(NGramVectorizer(self.vm, 1))
        self.vm.add_vectorizer(TransitionVectorizer(self.vm))
        self.crf = None
    
    def SaveXYToFile(self, x, y):
        x_array = x.to_array()
        y_array = y.to_array()
        with open('x', 'w') as f:
            for idx, i in enumerate(x_array):
                for j, features in i:
                    for feature in features:
                        feature_tuple = map(str, [idx,] + j + [feature,])
                        print >> f, "\t".join(feature_tuple)
        
        with open('y', 'w') as f:
            for idx, i in enumerate(y_array):
                for j, t in enumerate(i):
                    print >> f, "\t".join(map(str, [idx, j, t]))
    
    def fit(self, docs):
        self.vm.fit(docs)
        print >> sys.stderr, "tagger.fit finished."
        x, y = self.vm.transform(docs)
        print >> sys.stderr, "tagger.transform finished."
        self.SaveXYToFile(x, y)
        #print x.to_array()
        #print y.to_array()
        self.crf = LinearChainCRF(self.vm.feature_count, self.vm.tag_count)
        self.crf.fit(x, y, 500, 0.05, 0.001)
        self.weights = self.crf.get_weights()
        self.readable_features = self.vm.readable_features()
        self.tags = self.vm.tagid_to_tagname
        
        #print json.dumps(self.vm.readable_features(), sort_keys = True, indent = 4)
        
    def transform(self, docs):
        x, _ = self.vm.transform(docs)
        
        y = Y()
        y = self.crf.predict(x)
        y_array = y.to_array()
        for case in y_array:
            for i in range(0, len(case)):
                case[i] = self.tags[case[i]]
        return y_array
        
    def readable_features_and_weights(self):
        res = []
        for key, value in self.vm.readable_features().items():
            res.append([key, value, self.weights[key]])
        return res
     
    def debug(self, doc):
        x, y = self.vm.transform([doc])
        debugInfo = self.crf.debug(x[0], y[0])
        res = {"path":{}, "score":debugInfo[1]}
        for idx, step in enumerate(debugInfo[0]):
            res["path"]["position %s"%idx] = {"features":[], "tag":doc[idx][1]}
            for feature in step:
                res["path"]["position %s"%idx]["features"].append((self.readable_features[feature[0]], feature[1]))
        return res