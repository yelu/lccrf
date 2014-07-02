#!/usr/bin/env python

import os,sys
lib_path = os.path.abspath('../LCCRFPy')
sys.path.append(lib_path)
from VectorizerManager import VectorizerManager
from NGramVectorizer import NGramVectorizer
from TransitionVectorizer import TransitionVectorizer
from LCCRFPy import *
import unittest

class TestCaseVectorizerManager(unittest.TestCase):
    
    def setUp(self):
        pass
        
    def tearDown(self):
        pass
    
    def test_(self):
        doc = [[['a'], 'tag1'], [['b'], 'tag2']]
    
        vm = VectorizerManager()
        vm.add_vectorizer(NGramVectorizer(vm, 1))
        vm.add_vectorizer(TransitionVectorizer(vm))
        
        vm.fit([doc])
        x, y = vm.transform([doc])
        x, y = x.to_array(), y.to_array()
        
        self.assertEqual(len(x), 1)
        self.assertEqual(x[0][0][0], [0,-1,0])
        self.assertEqual(x[0][0][1], [0])
        self.assertEqual(x[0][1][0], [1,0,1])
        self.assertEqual(x[0][1][1], [1,2])
        self.assertEqual(x[0][2][0], [1,1,1])
        self.assertEqual(x[0][2][1], [1])
        
        self.assertEqual(len(y), 1)
        self.assertEqual(y[0], [0,1])
        
        
if __name__ == "__main__":
    unittest.main()

    

    
        