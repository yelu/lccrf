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
    
    def test_Fit(self):
        queryFile = "./query.txt"
        queries = open(queryFile, 'r').readlines()
        tokenizedQueries = [query.strip().split() for query in queries]
        featurizer = NGramFeaturizer(2)
        featurizer.Fit(tokenizedQueries)
        savedFile = "./2gram.txt"
        featurizer.Serialize(savedFile)

    def test_Featurize(self):
        self.test_Fit()
        savedFile = "./2gram.txt"
        f = NGramFeaturizer.Deserialize(savedFile)
        doc1 = 'a b c'.split()
        doc2 = 'what is the weather'.split()
        res = f.Featurize([doc1, doc2], 0)
        print res

if __name__ == "__main__":
    unittest.main()
   
        
