#!/usr/bin/env python

import os,sys
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
    
        fm = FeaturizerManager()
        fm.AddFeaturizer("ngram2", NGramFeaturizer(2))
        
        fm.Fit(xs, ys)

    def test_Transform(self): 
        xs = [['a', 'a', 'b', 'c']]
        ys = [['tag1', 'tag1', 'tag2', 'tag3']]
    
        fm = FeaturizerManager()
        fm.AddFeaturizer("ngram2", NGramFeaturizer(2), -1)
        
        fm.Fit(xs, ys)
        x = fm.TransformX(xs)
        y = fm.TransformY(ys)
        xPy = x.ToArray()
        yPy = y.ToArray()
        self.assertEqual(xPy[0][0], ([2, -1, 1], [0]))
        self.assertEqual(xPy[0][1], ([2, 0, 1], [1]))
        self.assertEqual(xPy[0][2], ([3, -1, 2], [3]))
        self.assertEqual(xPy[0][3], ([3, 1, 2], [2]))
        self.assertEqual(yPy[0], [0, 0, 1, 2])

    def test_Transform_TransitionFeature(self): 
        xs = [['a', 'b', 'c']]
        ys = [['tag1', 'tag2', 'tag3']]
    
        fm = FeaturizerManager()
        fm.AddFeaturizer("any", AnyFeaturizer(), shift = 0, unigram = False, bigram = True)
        
        fm.Fit(xs, ys)
        x = fm.TransformX(xs)
        y = fm.TransformY(ys)
        xPy = x.ToArray()
        yPy = y.ToArray()
        self.assertEqual(xPy[0][0], ([1, 0, 1], [0]))
        self.assertEqual(xPy[0][1], ([1, 1, 2], [1]))
        self.assertEqual(xPy[0][2], ([2, 0, 1], [0]))
        self.assertEqual(xPy[0][3], ([2, 1, 2], [1]))
        self.assertEqual(yPy[0], [0, 1, 2])


    def test_Serialize(self):
        xs = [['a', 'a', 'b', 'c']]
        ys = [['tag1', 'tag1', 'tag2', 'tag3']]
    
        fm = FeaturizerManager()
        fm.AddFeaturizer("ngram2", NGramFeaturizer(2), -1)
        #fm.AddFeaturizer("any", AnyFeaturizer(), shift = 0, unigram = False, bigram = True)
        #fm.AddFeaturizer("ngram1", NGramFeaturizer(2), shift = 0, unigram = True, bigram = False)
        
        fm.Fit(xs, ys)

        FeaturizerManager.Serialize(fm, "./model/")
        
        fm1 = FeaturizerManager.Deserialize("./model")
        x = fm1.TransformX(xs)
        y = fm1.TransformY(ys)
        xPy = x.ToArray()
        yPy = y.ToArray()
        self.assertEqual(xPy[0][0], ([2, -1, 1], [0]))
        self.assertEqual(xPy[0][1], ([2, 0, 1], [1]))
        self.assertEqual(xPy[0][2], ([3, -1, 2], [3]))
        self.assertEqual(xPy[0][3], ([3, 1, 2], [2]))
        self.assertEqual(yPy[0], [0, 0, 1, 2])


        
if __name__ == "__main__":
    unittest.main()

        
