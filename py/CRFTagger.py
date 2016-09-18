#!/usr/bin/env python

import os,sys
from collections import defaultdict
from operator import itemgetter
import json
import shutil
from log import *
from LCCRFFeaturizer import LCCRFFeaturizer
from NGramFeaturizer import NGramFeaturizer
from LCCRFPy import LCCRFPy

class CRFTagger:
    def __init__(self, config = None):
	if config != None:
	    self.config = config
            self._lccrfpy = None
            self._lccrfFeaturizer = None
            self._tagToId = None
            self._idToTag = None

    @staticmethod
    def Load(modelDir):
	crfTagger = CRFTagger()
        weightsFile = os.path.join(modelDir, "lccrf.weights.txt")
        crfTagger._lccrfpy = LCCRFPy()
        crfTagger._lccrfpy.Load(weightsFile)
        crfTagger._lccrfFeaturizer = LCCRFFeaturizer.Load(modelDir)
        tagFile = os.path.join(modelDir, "lccrf.tags.txt")
        with open(tagFile, 'r') as f:
            crfTagger._tagToId = eval(f.readline())
            crfTagger._idToTag = dict((v,k) for k,v in crfTagger._tagToId.iteritems())
        return crfTagger

    def Train(self, xs, ys, \
              maxIteration = 1, \
              learningRate = 0.001, \
              variance = 0.001):

        modelDir = self.config["modelDir"]

        if not os.path.exists(modelDir):
            os.makedirs(modelDir)

        # generate ngrams
        unigramFile = os.path.join(modelDir, "1gram.txt")
        NGramFeaturizer.GenerateNGrams(xs, unigramFile, 1, False)
        bigramFile = os.path.join(modelDir, "2gram.txt")
        NGramFeaturizer.GenerateNGrams(xs, bigramFile, 2, False)
        trigramFile = os.path.join(modelDir, "3gram.txt")
        NGramFeaturizer.GenerateNGrams(xs, trigramFile, 3, False)
        pipelineFile = os.path.join(modelDir, "lccrf.pipeline.txt")
        self.GeneratePipelineFile(self.config, pipelineFile)

        #outputs = ["1gram", "2gram", "3gram", "cfg"]
        outputs = ["1gram", "2gram", "3gram", \
                   "1gram_n1", "1gram_p1", "1gram_n2", "1gram_p2", "1gram_n3", "1gram_p3", \
                   "2gram_n1", "2gram_p1", "2gram_n2", "2gram_p2", \
                   "cfg"]
        self._lccrfFeaturizer = LCCRFFeaturizer(pipelineFile, outputs)
        self._tagToId = {}
        lccrfQueryBinFile = os.path.join(modelDir, 'query.featurized.bin')
        with open(lccrfQueryBinFile, 'w') as f:
            for x, y in zip(xs, ys):
                res = {}
                # save tags
                tags = y
                for idx, tag in enumerate(tags):
                    if tag not in self._tagToId:
                        self._tagToId[tag] = len(self._tagToId)
                    res[idx] = [self._tagToId[tag], [-1]]

                xFeatures = self._lccrfFeaturizer.Featurize(x)
                for xFeature in xFeatures:
                    end, xFid = xFeature
                    res[end][1].append(xFid)
                print >> f, len(tags)
                for key in sorted(res):
                    print >> f, "\t".join([str(key), str(res[key][0]), ','.join(str(x) for x in res[key][1])])
                print >> f,""

        self._lccrfFeaturizer.Save(modelDir)

        ## save tags.
        with open(os.path.join(modelDir, 'lccrf.tags.txt'), 'w') as f:
            print >> f, self._tagToId
        self._idToTag = dict((v,k) for k,v in self._tagToId.iteritems())
        log.debug("tags saved.")

        self._lccrfpy = LCCRFPy()
        self._lccrfpy.Fit(lccrfQueryBinFile, maxIteration, learningRate, variance)
        weightsFile = os.path.join(modelDir, "lccrf.weights.txt")
        self._lccrfpy.Save(weightsFile)
        log.debug("weights saved.")

    @staticmethod
    def LoadTrainData(filePath):
        xs = []
        ys = []
        with open(filePath) as f:
            for line in f:
                fields = line.strip().split('\t')
                if len(fields) < 2:
                    continue
                sentence = fields[1].strip().replace("<", " <")
                sentence = sentence.replace(">", "> ")
                words = []
                for token in sentence.split():
                    if token != "":
                        words.append(token)
                x = []
                y = []
                tag = 'o'
                has_tag = False
                for word in words:
                    if word.startswith('<') and not word.startswith('</'):
                        tag = word[1:-1]
                        has_tag = True
                        continue
                    if word.startswith('</'):
                        tag = 'o'
                        continue
                    x.append(word)
                    y.append(tag)
                if has_tag:
                    xs.append(x)
                    ys.append(y)
        #shuffle(docs)
        return xs, ys

    def GeneratePipelineFile(self, config, pipelineFile):
        modelDir = os.path.dirname(pipelineFile)
        with open(pipelineFile, 'w') as f:
            # ngram
            print >> f, "NGramFeaturizer --input=query --output=1gram --n=1 --ngramFile=./1gram.txt"
            print >> f, "NGramFeaturizer --input=query --output=2gram --n=2 --ngramFile=./2gram.txt"
            print >> f, "NGramFeaturizer --input=query --output=3gram --n=3 --ngramFile=./3gram.txt"
            # previous/next ngram
            print >> f, "FeatureShifter --input=1gram --output=1gram_n1 --shift=1"
            print >> f, "FeatureShifter --input=1gram --output=1gram_p1 --shift=-1"
            print >> f, "FeatureShifter --input=1gram --output=1gram_n2 --shift=2"
            print >> f, "FeatureShifter --input=1gram --output=1gram_p2 --shift=-2"
            print >> f, "FeatureShifter --input=1gram --output=1gram_n3 --shift=3"
            print >> f, "FeatureShifter --input=1gram --output=1gram_p3 --shift=-3"
            print >> f, "FeatureShifter --input=2gram --output=2gram_n1 --shift=1"
            print >> f, "FeatureShifter --input=2gram --output=2gram_p1 --shift=-1"
            print >> f, "FeatureShifter --input=2gram --output=2gram_n2 --shift=2"
            print >> f, "FeatureShifter --input=2gram --output=2gram_p2 --shift=-2"
            pipelineFile = os.path.join(modelDir, "cfg.grammar.xml")
            if "grammarFile" not in config["features"]["cfg"]:
                with open(pipelineFile) as f:
                    print >> f, '''<?xml version="1.0" encoding="utf-8"?><root></root>'''
            else:
                shutil.copyfile(config["features"]["cfg"]["grammarFile"], pipelineFile)
            print >> f, "CFGFeaturizer --input=query --output=cfg --grammarFile=./cfg.grammar.xml"

    def Predict(self, tokenizedQuery):
        xFeatures = self._lccrfFeaturizer.Featurize(tokenizedQuery, lock = True)
        x = []
        for item in xFeatures:
            x.append((item[1], item[0]))
        xLen = len(tokenizedQuery)
        y = self._lccrfpy.Predict(x, xLen)
        tags = [self._idToTag[id] for id in y]
        return tags

    def Evaluate(self, xs, ys):
        predictedTags = []
        for i, x in enumerate(xs):
            y = self.Predict(x)
            predictedTags.append(y)
        stat = {}
        totalTags = 0
        rightTag = 0
        xys = zip(xs, ys)
        for i, xy in enumerate(xys):
            x, y = xy[0], xy[1]
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
