#!/usr/bin/env python

class AnyFeaturizer(object):
    def __init__(self):
        pass

    def Dim(self):
        return 1

    def GetFeatures(self):
        return self._features
    
    def Fit(self, queries):
        pass

    def Featurize(self, queries, idShift):
        res = []
        for query in queries:
            features = []
            for i in range(0, len(query)):
                features.append((i, i, idShift))
            res.append(features)
        return res

    def Serialize(self, dstFile):
        pass

    @staticmethod
    def Deserialize(srcFile):
        featurizer = AnyFeaturizer()
        return featurizer


