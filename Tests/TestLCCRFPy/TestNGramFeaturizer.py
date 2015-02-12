#!/usr/bin/env python

import os,sys
from NGramFeaturizer import NGramFeaturizer
from LCCRFPy import *
import unittest

class TestCaseNGramFeaturizer(unittest.TestCase):
    
    def setUp(self):
        pass
        
    def tearDown(self):
        pass
    
    def test_(self):
        f1 = NGramFeaturizer(2)
        docs = ['a', 'b', 'c']
        print f1.Fit(docs)
        features = list(f1.GetFeatures())
        features = f1.GetFeatures()
        self.assertEqual(features, set([('a', 'b'), ('b', 'c')]))
        
        f2 = NGramFeaturizer(3)
        docs = ['a', 'b', 'c']
        print f2.Fit(docs)
        features = f2.GetFeatures()
        self.assertEqual(features, set([('a', 'b', 'c')]))
        
        f3 = NGramFeaturizer(1)
        docs = ['a', 'b', 'c']
        print f3.Fit(docs)
        features = f3.GetFeatures()
        self.assertEqual(features, set([('a', ), ('b', ), ('c', )]))
    
    def test_Serialize(self):
        f1 = NGramFeaturizer(2)
        docs = ['a', 'b', 'c']
        f1.Fit(docs)
        print f1.Readable()

        fileDir = "./model/ngram"
        if not os.path.exists(fileDir):
            os.makedirs(fileDir)
        NGramFeaturizer.Serialize(f1, fileDir)

        f2 = NGramFeaturizer.Deserialize(fileDir)
        print f2.Readable()

        self.assertEqual(f1._n, f2._n)
        self.assertEqual(f1._features, f2._features)

        
if __name__ == "__main__":
    unittest.main()
   
        
