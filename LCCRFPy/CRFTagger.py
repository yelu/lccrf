#!/usr/bin/env python

import os,sys
from LCCRFPy import *
from Vectorizer import Vectorizer
from VectorizerManager import VectorizerManager
from NGramVectorizer import NGramVectorizer
from TransitionVectorizer import TransitionVectorizer

class CRFTagger:

    def __init__(self):
        self.vm = VectorizerManager()
        self.vm.add_vectorizer(NGramVectorizer(1))
        self.vm.add_vectorizer(TransitionVectorizer())
        self.crf = None
        
    def fit(self, docs):
        self.vm.fit(docs)
        x, y = self.vm.transform(docs)
        self.crf = LinearChainCRF(self.vm.feature_count, self.vm.tag_count, 0.1)
        self.crf.fit(x, y, 0.1, 1, 20)
        self.weights = self.crf.get_weights()
        self.readable_features = self.vm.readable_features()
        self.tags = Vectorizer.tagid_to_tagname
    
    def transform(self, docs):
        refX, refY = self.vm.transform(docs)
        testY = Y()
        self.crf.predict(refX, testY)
        testY = testY.get_all_tags()
        for case in testY:
            for i in range(0, len(case)):
                case[i] = self.tags[case[i]]
        return testY
     
    def debug(self, doc):
        x, y = self.vm.transform([doc])
        res = self.crf.debug(x[0], y[0])
        res = list(res)
        for step in res[0]:
            for i in range(0, len(step)):
                step[i] = list(step[i])
                step[i][0] = self.readable_features[step[i][0]]
        return res