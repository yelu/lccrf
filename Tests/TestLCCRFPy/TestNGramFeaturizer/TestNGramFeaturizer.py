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
    
    def test_SelectNGramToFile(self):
        docsFile = "./query.txt"
        ngramFile = "./ngram.txt"
        
        NGramFeaturizer.SelectNGramToFile(docsFile, ngramFile, 2)

    def test_Featurize(self):
        ngramFile = "./ngram.txt"
        f1 = NGramFeaturizer(ngramFile, 2)
        doc = ['a', 'b', 'c']
        res = f1.Featurize(doc)
        print res

if __name__ == "__main__":
    unittest.main()
   
        
