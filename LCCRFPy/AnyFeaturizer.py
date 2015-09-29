#!/usr/bin/env python
import pickle

class AnyFeaturizer(object):
    def __init__(self):
        self._features = set([''])
    
    def GetFeatures(self):
        return self._features
        
    def Featurize(self, words):
        res = []
        for i in range(0, len(words)):
            res.append((i, i, ('')))
        return res

