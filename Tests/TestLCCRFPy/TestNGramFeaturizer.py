#!/usr/bin/env python

import os,sys
#lib_path = os.path.abspath('../../LCCRFPy')
#sys.path.append(lib_path)
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
        print f1.readable()
        features = list(f1.GetFeatures())
        features = f1.GetFeatures()
        self.assertEqual(features, set([('a', 'b'), ('b', 'c')]))
        
        f2 = NGramFeaturizer(3)
        docs = ['a', 'b', 'c']
        print f2.Fit(docs)
        print f2.readable()
        features = f2.GetFeatures()
        self.assertEqual(features, set([('a', 'b', 'c')]))
        
        f3 = NGramFeaturizer(1)
        docs = ['a', 'b', 'c']
        print f3.Fit(docs)
        print f3.readable()
        features = f3.GetFeatures()
        self.assertEqual(features, set([('a', ), ('b', ), ('c', )]))
        
if __name__ == "__main__":
    unittest.main()
   
        
