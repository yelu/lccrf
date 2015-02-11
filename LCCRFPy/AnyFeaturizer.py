#!/usr/bin/env python

class AnyFeaturizer:
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

    @staticmethod
    def Serialize(obj, filePath):
        with open(filePath, 'w') as f:
            pickle.dump(obj, f)
            #pickle.dump(self._n)
            #pickle.dump(self._features)

    @staticmethod
    def Deserialize(filePath):
        with open(filePath, 'r') as f:
            obj = pickle.load(f)
            return obj


