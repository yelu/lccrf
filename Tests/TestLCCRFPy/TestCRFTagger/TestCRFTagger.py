#!/usr/bin/env python

import os,sys
from CRFTagger import *
from CRFTaggerFeaturizer import CRFTaggerFeaturizer
from NGramFeaturizer import NGramFeaturizer
from AnyFeaturizer import AnyFeaturizer
import unittest
import re
import json
from random import shuffle

class TestCRFTagger(unittest.TestCase):
   
    def setUp(self):
        pass

    def tearDown(self):
        pass

    def test_Fit(self):
        filePath = './train.tsv'
        trainXs, trainYs = self.ParseInput(filePath)
        
        # 1. instantiate a CRFTagger and add three featurizers.
        featurizer = CRFTaggerFeaturizer()
        # add bigram(on x) featurizer : current bi-word together with current tag.
        featurizer.AddFeaturizer("2gram", NGramFeaturizer(2), shift = 0, uniTag = True, biTag = False)
        # add unigram(on x) featurizer : current word together with current tag.
        featurizer.AddFeaturizer("1gram", NGramFeaturizer(1), shift = 0, uniTag = True, biTag = False)
        # add any featurizer : trigger a feature at any position of x, 
        # the final feature depends purely on transitions of
        featurizer.AddFeaturizer("any", AnyFeaturizer(), shift = 0, uniTag = False, biTag = True)

        tagger = CRFTagger(featurizer)
        modelDir = "./"
        tagger.Train(trainXs, trainYs, modelDir, \
                   maxIteration = 1000,\
                   learningRate = 0.05, \
                   variance = 0.0008)
        #tagger.SaveReadableFeaturesAndWeights("./output/crftagger/tagger.model")
        #print >> sys.stderr, "Feature Count : %d    Tag Count : %d" % (tagger.fm.FeatureCount, \
        #                                                            tagger.fm.TagCount)
        #self.assertEqual(tagger.fm.FeatureCount, 134)

    def test_Predict(self):
        filePath = './train.tsv'
        trainXs, trainYs = self.ParseInput(filePath)
        
        modelDir = "./"
        tagger = CRFTagger.Deserialize(modelDir)
        
        tagger.SaveReadableFeaturesAndWeights('./features.txt')
        
        xs = ["what is the weather in shang hai".split(),
              "what is the weather in a b c".split()]
        ys = tagger.Predict(xs)
        print ys

        #test on training set.
        stat = tagger.Test(trainXs, trainYs)
        print json.dumps(stat, indent=4)

    def ParseInput(self, filePath):
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
                tag = 'oos'
                has_tag = False
                for word in words:
                    if word.startswith('<') and not word.startswith('</'):
                        tag = word[1:-1]
                        has_tag = True
                        continue
                    if word.startswith('</'):
                        tag = 'oos'
                        continue
                    x.append(word)
                    y.append(tag)
                if has_tag:
                    xs.append(x)
                    ys.append(y)
        #shuffle(docs)
        return xs, ys
    
if __name__ == "__main__":
    unittest.main()
    #test = TestCRFTagger()
    #test.test_Fit()
    
