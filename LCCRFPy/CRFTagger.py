#!/usr/bin/env python

import os,sys
from collections import defaultdict
from log import *
from CRFTaggerFeaturizer import CRFTaggerFeaturizer
from AnyFeaturizer import AnyFeaturizer
import json

class CRFTagger:

    def __init__(self, crfFeaturizer = None, tmpDir = "./"):
        self.featurizer = crfFeaturizer
        self.tmpDir = tmpDir
        if not os.path.exists(tmpDir):
            os.makedirs(tmpDir)
        self.weights = {}
        self.lccrfBin = os.path.join(os.path.dirname(os.path.abspath(__file__)), 'lccrf.exe')

    def Train(self, queries, tags, modelDir = "./", \
            maxIteration = 1, \
            learningRate = 0.001, \
            variance = 0.001):
        
        if not os.path.exists(modelDir):
            os.makedirs(modelDir)
            
        self.featurizer.Fit(queries, tags)
        self.featurizer.Serialize(os.path.join(modelDir, 'features.bin'))
        log.debug("feature extracted.")
                
        queriesBinFile = os.path.join(self.tmpDir, "queries.lccrf.bin")
        with open(queriesBinFile, 'w') as f:
            print >> f, "%d    %d" % (len(self.featurizer.Features()), len(self.featurizer.Tags()))
            print >> f, ""
            featurizedQueries = self.featurizer.Featurize(queries)
            for i, q in enumerate(featurizedQueries):
                # save tags.
                print >> f, len(tags[i])
                print >> f, " ".join([str(self.featurizer._tags[x]) for x in tags[i]])
                # save features.
                print >> f, len(q)
                for feat in q:
                    print >> f, " ".join([str(x) for x in list(feat)])
                print >> f, ""

            with open('x.txt', 'w') as f1:
                for i, q in enumerate(featurizedQueries):
                    for feat in q:
                        print >> f1, " ".join([str(i)] + [str(x) for x in list(feat)])

        # train crf model.
        cmd = "\"%s\" -d \"%s\" -i %s -s %s -l %s -m \"%s\" >%s 2>&1" % \
               (self.lccrfBin, queriesBinFile, maxIteration, learningRate, variance, \
                os.path.join(modelDir, 'weights.txt'), \
                os.path.join(self.tmpDir, 'train_lccrf.log'))
        
        import commands
        log.info(cmd)
        (status, output) = commands.getstatusoutput(cmd)
        log.info("%s return with %s"% (cmd, status))
        if status:
            raise RuntimeError, "%s failed. [ret:%s][%s]."%(cmd, status, output)
        return

    @staticmethod
    def Deserialize(modelDir):
        modelFile = os.path.join(modelDir, "weights.txt")
        tagger = CRFTagger()
        with open(modelFile, 'r') as f:
            next(f)
            for line in f:
                fields = line.strip().split()
                if len(fields) != 2:
                    continue
                tagger.weights[int(fields[0])] = float(fields[1])

        tagger.featurizer = CRFTaggerFeaturizer.Deserialize(os.path.join(modelDir, "features.bin"))
        return tagger

    def Predict(self, xs):
        res = []
        for x in xs:
            xFeatures = self.featurizer.Featurize([x])[0]
            edges, nodes = self.MakeEdgesAndNodes(xFeatures, self.weights)
            yIDs = self.VeterbiDecode(edges, nodes, len(x), len(self.featurizer.Tags()))
            yWithOriginalTag = map(lambda x:self.featurizer._idToTag[x], yIDs)
            res.append(yWithOriginalTag)
        return res

    def MakeEdgesAndNodes(self, xFeatures, weights):
        edges = defaultdict(lambda : 0.0)
        nodes = defaultdict(lambda : 0.0)
        for feature in xFeatures:
            pos, preTag, curTag, featureID = feature[0], feature[1], feature[2], feature[3]
            if preTag >= 0:
                key = (pos, preTag, curTag)
                edges[key] += weights[featureID]
            else:
                key = (pos, curTag)
                nodes[key] += weights[featureID]
        return (edges, nodes)

    def VeterbiDecode(self, edges, nodes, nStep, nState):
        phi = [0.0] * nState
        for i in range(nState):
            phi[i] = nodes[(0, i)]

        newPhi = [float('-inf')] * nState

        backtraceMatrix = defaultdict(lambda : -1)
        for j in range(1, nStep):
            for s1 in range(0, nState):
                for s2 in range(0, nState):
                    d = phi[s2] + edges[(j, s2, s1)] + nodes[(j, s1)]
                    if d > newPhi[s1]:
                        backtraceMatrix[(j, s1)] = s2
                        newPhi[s1] = d
            phi, newPhi = newPhi, phi
        
        # find the optimal path
        maxJ = -1
        res = [-1] * nStep
        maxPath = float('-inf')
        for s in range(0, nState):
            if phi[s] > maxPath:
                maxPath = phi[s]
                maxJ = s
        
        res[nStep - 1] = maxJ
        for j in range(nStep - 2, -1, -1):
            res[j] = backtraceMatrix[(j + 1, res[j + 1])]

        return res

    
    def Debug(self, x):
        x, y = self.fm.Transform([x])
        debugInfo = self.crf.debug(x[0], y[0])
        res = {"path":{}, "score":debugInfo[1]}
        for idx, step in enumerate(debugInfo[0]):
            res["path"]["position %s"%idx] = {"features":[], "tag":doc[idx][1]}
            for feature in step:
                res["path"]["position %s"%idx]["features"].append((self.readable_features[feature[0]], feature[1]))
        return res
        
    def Test(self, xs, ys):
        predictedTags = self.Predict(xs)
        stat = {}
        totalTags = 0
        rightTag = 0
        xys = zip(xs, ys)
        for i, xy in enumerate(xys):
            x, y = xy
            for j, word in enumerate(x):
                totalTags += 1
                expectedTag = y[j]
                predictedTag = predictedTags[i][j]
                if expectedTag not in stat:
                    stat[expectedTag] = {"tp":0,"fp":0,"fn":0,"tn":0}
                if predictedTag not in stat:
                    stat[predictedTag] = {"tp":0,"fp":0,"fn":0,"tn":0}
                if predictedTag == expectedTag:
                    stat[expectedTag]["tp"] += 1
                    rightTag += 1
                else:
                    stat[predictedTag]["fp"] += 1
                    stat[expectedTag]["fn"] += 1
        for key, value in stat.items():
            value["precision"] = 0.0
            if value["tp"] + value["fp"] != 0:
                value["precision"] = float(value["tp"]) / float(value["tp"] + value["fp"])
            value["recall"] = 0.0
            if value["tp"] + value["fn"] != 0:
                value["recall"] = float(value["tp"]) / float(value["tp"] + value["fn"])
            value["f1"] = 0.0
            if abs(value["precision"] + value["recall"]) > 1e-6:
                value["f1"] = 2 * value["precision"] * value["recall"] / float(value["precision"] + value["recall"])
        if rightTag == 0:
            stat["accuracy"] = 0.0
        else:
            stat["accuracy"] = float(rightTag) / float(totalTags)
        return stat

    def ReadableFeaturesAndWeights(self):
        res = []
        features = self.featurizer.Features()
        for key, value in features.items():
            res.append((key, value, self.weights[key]))
        return res
    
    def SaveReadableFeaturesAndWeights(self, filePath):
        featureWeight = self.ReadableFeaturesAndWeights()
        with open(filePath, 'w') as f:
            for featureId, featureName, weight in featureWeight:
                print >> f, "%s\t%s\t%f" % (featureId, featureName, weight)
 
