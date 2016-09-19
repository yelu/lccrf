#!/usr/bin/env python

import os,sys
from LCCRFFeaturizer import LCCRFFeaturizer
from NGramFeaturizer import NGramFeaturizer
import unittest

class TestCaseFeaturizerManager(unittest.TestCase):

    def setUp(self):
        pass

    def tearDown(self):
        pass

    def test_Process(self):
        queryFile = "./query.txt"
        tokenizedQueries = []
        with open(queryFile) as f:
            for line in f:
                fields = line.strip().split()
                tokenizedQueries.append(fields)
        ngramFile = "./2gram.txt"
        NGramFeaturizer.GenerateNGrams(tokenizedQueries, ngramFile, 2, False)
        crfFeaturizer = LCCRFFeaturizer("./pipeline.txt", ["ngrams", 'cfgs'])
        features = crfFeaturizer.Featurize("what is the weather on september 8 2016".split())
        print features
        self.assertEqual(len(features), 5)
        self.assertEqual(features[0], (1, 0))
        self.assertEqual(features[1], (2, 1))
        self.assertEqual(features[2], (3, 2))
        self.assertEqual(features[3], (6, 3))
        self.assertEqual(features[4], (7, 4))


if __name__ == "__main__":
    unittest.main()
