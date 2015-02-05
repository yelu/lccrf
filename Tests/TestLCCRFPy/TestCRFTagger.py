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
#class TestCRFTagger:

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
    
    def setUp(self):
        #testDoc = self.ParseInput('./data/train.tsv')
        #res = tagger.test(testDoc)
        #print json.dumps(res, sort_keys = True, indent = 4)
        pass

    def test_Fit(self):
        filePath = './data/testset.tsv'
        trainXs, trainYs = self.ParseInput(filePath)
        print trainXs
        print trainYs
        
        tagger = CRFTagger()
        tagger.AddFeaturizer("ngram1", NGramFeaturizer(1), shift = 0, unigram = True, bigram = True)
        tagger.AddFeaturizer("ngram2", NGramFeaturizer(2), shift = 0, unigram = True, bigram = True)
        tagger.AddFeaturizer("any", AnyFeaturizer(), shift = 0, unigram = False, bigram = True)
        tagger.Fit(trainXs, trainYs)
        print sys.stderr, "Feature Count : %d    Tag Count : %d" % (tagger.fm.FeatureCount, \
                                                                    tagger.fm.TagCount)
        tagger.SaveReadableFeaturesAndWeights("./tagger.model")
        #tagger.Save('./tagger.model')

        
        
    def tearDown(self):
        pass
    
    def test_fit(self):
        pass
        
if __name__ == "__main__":
    unittest.main()
    #test = TestCRFTagger()
    #test.setUp()
    
    
    
    
