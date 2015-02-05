#!/usr/bin/env python

import os,sys
lib_path = os.path.abspath('../../LCCRFPy')
sys.path.append(lib_path)
from FeaturizerManager import FeaturizerManager
from NGramFeaturizer import NGramFeaturizer
from AnyFeaturizer import AnyFeaturizer
from LCCRFPy import *
import unittest

class TestCaseFeaturizerManager(unittest.TestCase):
    
    def setUp(self):
        pass
        
    def tearDown(self):
        pass
    
    def test_Fit(self):
        xs = [['a', 'b', 'c']]
        ys = [['tag1', 'tag2', 'tag3']]
    
        vm = FeaturizerManager()
        vm.AddFeaturizer("ngram2", NGramFeaturizer(2))
        
        vm.Fit(xs, ys)
        #print vm._features

    def test_Transform(self): 
        xs = [['a', 'a', 'b', 'c']]
        ys = [['tag1', 'tag1', 'tag2', 'tag3']]
    
        vm = FeaturizerManager()
        vm.AddFeaturizer("ngram2", NGramFeaturizer(2), -1)
        
        vm.Fit(xs, ys)
        #print vm._features
        x = vm.TransformX(xs)
        y = vm.TransformY(ys)
        xPy = x.ToArray()
        yPy = y.ToArray()
        self.assertEqual(xPy[0][0], ([2, 0, 1], [1, 0]))
        self.assertEqual(xPy[0][1], ([2, 1, 1], [0]))
        self.assertEqual(xPy[0][2], ([2, 2, 1], [0]))
        self.assertEqual(xPy[0][3], ([3, 0, 2], [3]))
        self.assertEqual(xPy[0][4], ([3, 1, 2], [3, 2]))
        self.assertEqual(xPy[0][5], ([3, 2, 2], [3]))
        self.assertEqual(yPy[0], [0, 0, 1, 2])

    def test_Transform_TransitionFeature(self): 
        xs = [['a', 'b', 'c']]
        ys = [['tag1', 'tag2', 'tag3']]
    
        vm = FeaturizerManager()
        vm.AddFeaturizer("any", AnyFeaturizer(), shift = 0, unigram = False, bigram = True)
        
        vm.Fit(xs, ys)
        x = vm.TransformX(xs)
        y = vm.TransformY(ys)
        xPy = x.ToArray()
        yPy = y.ToArray()
        print xPy
        print yPy
        self.assertEqual(xPy[0][0], ([1, 0, 1], [0]))
        self.assertEqual(xPy[0][1], ([1, 1, 2], [1]))
        self.assertEqual(xPy[0][2], ([2, 0, 1], [0]))
        self.assertEqual(xPy[0][3], ([2, 1, 2], [1]))
        self.assertEqual(yPy[0], [0, 1, 2])



        
if __name__ == "__main__":
    unittest.main()

        
