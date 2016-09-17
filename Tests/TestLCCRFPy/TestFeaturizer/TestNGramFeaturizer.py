#!/usr/bin/env python

import os,sys
from NGramFeaturizer import NGramFeaturizer
#from LCCRFPy import *
import unittest

class TestCaseNGramFeaturizer(unittest.TestCase):

    def setUp(self):
        pass

    def tearDown(self):
        pass

    def test_GenerateNGrams(self):
        queryFile = "./query.txt"
        ngramFile = "./2gram.txt"
        grammarFile = './en-us.datetime.grammar.xml'
        featurizer = NGramFeaturizer.GenerateNGrams(queryFile, 0, ngramFile, 2, False)
        ngramCount = len(open(ngramFile).readlines())
        self.assertEqual(ngramCount, 8)

if __name__ == "__main__":
    unittest.main()
