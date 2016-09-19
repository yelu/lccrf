#!/usr/bin/env python

import os,sys
#sys.path.append(os.path.abspath("../../"))
from CRFTagger import *
import unittest
import json
from random import shuffle

class TestCRFTagger(unittest.TestCase):

    def setUp(self):
        pass

    def tearDown(self):
        pass

    def test_FitPredict(self):
        filePath = './train.tsv'
        xs, ys = CRFTagger.LoadTrainData(filePath)
        config = {"modelDir":"./model", "features":{"cfg":{"grammarFile":"./cfg.xml"}}}
        tagger = CRFTagger(config)
        tagger.Train(xs, ys, maxIteration = 1000, learningRate = 0.05, variance = 0.0008)

        tagger = CRFTagger.Load(config["modelDir"])

        #test on training set.
        stat = tagger.Evaluate(xs, ys)
        print json.dumps(stat, indent=4)

        xs = ["will it rain".split(),
              "what is the weather in shanghai".split(),
              "what is the weather in a b c d e f g".split()]
        ys = []
        for x in xs:
            y = tagger.Predict(x)
            print y

if __name__ == "__main__":
    unittest.main()
    #test = TestCRFTagger()
    #test.test_Fit()
