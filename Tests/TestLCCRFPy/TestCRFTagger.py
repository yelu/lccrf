#!/usr/bin/env python

import os,sys
lib_path = os.path.abspath('../../LCCRFPy')
sys.path.append(lib_path)
from CRFTagger import *
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
        filePath = './data/testset.tsv'
        trainXs, trainYs = self.ParseInput(filePath)
        print trainXs
        print trainYs
        
        tagger = CRFTagger()
        tagger.AddFeaturizer("ngram1", NGramFeaturizer(1), shift = 0, unigram = True, bigram = False)
        tagger.AddFeaturizer("ngram2", NGramFeaturizer(2), shift = 0, unigram = True, bigram = False)
        #tagger.AddFeaturizer("any", AnyFeaturizer(), shift = 0, unigram = False, bigram = True)
        tagger.Fit(trainXs, trainYs, \
                   maxIteration = 1000,\
                   learningRate = 0.05, \
                   variance = 0.0008)
        print >> sys.stderr, "Feature Count : %d    Tag Count : %d" % (tagger.fm.FeatureCount, \
                                                                    tagger.fm.TagCount)
        tagger.SaveReadableFeaturesAndWeights("./tagger.model")
        self.assertEqual(tagger.fm.FeatureCount, 119)
        #tagger.Save('./tagger.model')

    def test_Transform(self):
        filePath = './data/testset.tsv'
        trainXs, trainYs = self.ParseInput(filePath)
        
        tagger = CRFTagger()
        tagger.AddFeaturizer("ngram1", NGramFeaturizer(1), shift = 0, unigram = True, bigram = False)
        tagger.AddFeaturizer("ngram2", NGramFeaturizer(2), shift = 0, unigram = True, bigram = False)
        tagger.AddFeaturizer("any", AnyFeaturizer(), shift = 0, unigram = False, bigram = True)
        tagger.Fit(trainXs, trainYs, \
                   maxIteration = 1000,\
                   learningRate = 0.05, \
                   variance = 0.0008)
        print >> sys.stderr, "Feature Count : %d    Tag Count : %d" % (tagger.fm.FeatureCount, \
                                                                    tagger.fm.TagCount)
        xs = ["what is the weather in shang hai".split(),
              "what is the weather in a b c".split()]
        ys = tagger.Transform(xs)
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
    #test.setUp()
    
