#!/usr/bin/env python
import pickle

class AnyFeaturizer(object):
    def __init__(self):
        self._features = set([''])
    
    def GetFeatures(self):
        return self._features
        
    def Fit(self, doc):
        return self.Transform(doc)

    def Transform(self, doc):
        res = []
        for i in range(0, len(doc)):
            res.append((i, i, ('')))
        return res

