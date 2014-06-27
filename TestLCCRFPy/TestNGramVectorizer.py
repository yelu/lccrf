#!/usr/bin/env python

import os,sys
lib_path = os.path.abspath('../LCCRFPy')
sys.path.append(lib_path)
from NGramVectorizer import NGramVectorizer
from Vectorizer import Vectorizer
from LCCRFPy import *
import unittest

class TestCaseNGramVectorizer(unittest.TestCase):
    
    def setUp(self):
        pass
        
    def tearDown(self):
        pass
    
    def test_(self):
        doc = [[['a'], 'tag1'], [['b'], 'tag2']]
        Vectorizer.get_or_allocate_tagid('tag1')
        Vectorizer.get_or_allocate_tagid('tag2')
        v = NGramVectorizer(1)
        v.fit(doc)
        print v.get_features()
        
        x = X()
        y = Y()
        x.append(XItem(2))
        v.transform(doc, x[-1])
        x = x.get_all_features()
        
        self.assertEqual(len(x), 1)
        self.assertEqual(x[0][0][0], [0,-1,0])
        self.assertEqual(x[0][0][1], [0])
        self.assertEqual(x[0][1][0], [1,0,1])
        self.assertEqual(x[0][1][1], [1])
        self.assertEqual(x[0][2][0], [1,1,1])
        self.assertEqual(x[0][2][1], [1])       
        
if __name__ == "__main__":
    unittest.main()
   
        