#!/usr/bin/env python

import os,sys
#sys.path.append(os.path.abspath("../../"))
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

    def test_FitPredict(self):
        filePath = './train.tsv'
        xs, ys = self.ParseInput(filePath)
	config = {"modelDir":"./model", "features":{"cfg":{"grammarFile":"./en-us.datetime.grammar.xml"}}}
        tagger = CRFTagger(config)
        tagger.Train(xs, ys, maxIteration = 1000, learningRate = 0.05, variance = 0.0008)

        tagger = CRFTagger.Load(config["modelDir"])

        #test on training set.
        stat = tagger.Test(xs, ys)
        print json.dumps(stat, indent=4)

        xs = ["will it rain",
	      "what is the weather in shanghai",
              "what is the weather in a b c"]
        ys = []
        for x in xs:
            ys.append(tagger.Predict(x))
        print ys


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
                    xs.append(" ".join(x))
                    ys.append(" ".join(y))
        #shuffle(docs)
        return xs, ys

if __name__ == "__main__":
    unittest.main()
    #test = TestCRFTagger()
    #test.test_Fit()
