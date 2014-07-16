#!/usr/bin/env python

import os,sys
lib_path = os.path.abspath('../LCCRFPy')
sys.path.append(lib_path)
from NGramVectorizer import NGramVectorizer
from VectorizerManager import VectorizerManager
from LCCRFPy import *
import unittest

class TestCaseNGramVectorizer(unittest.TestCase):
    
    def setUp(self):
        pass
        
    def tearDown(self):
        pass
    
    def test_(self):
        doc = [[['a'], 'tag1'], [['b'], 'tag2']]
        vm = VectorizerManager()
        v = NGramVectorizer(vm, 1)
        vm.add_vectorizer(v)
        vm.fit([doc])
        print v.readable_features()
        
        x, y = vm.transform([doc])
        x = x.to_array()
        
        self.assertEqual(len(x), 1)
        self.assertEqual(x[0][0][0], [0,-1,0])
        self.assertEqual(x[0][0][1], [0])
        self.assertEqual(x[0][1][0], [1,0,1])
        self.assertEqual(x[0][1][1], [1])
        self.assertEqual(x[0][2][0], [1,1,1])
        self.assertEqual(x[0][2][1], [1])       
        
if __name__ == "__main__":
    unittest.main()
   
        