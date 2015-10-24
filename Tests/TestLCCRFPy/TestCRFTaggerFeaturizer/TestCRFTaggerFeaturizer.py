#!/usr/bin/env python

import os,sys
from CRFTaggerFeaturizer import CRFTaggerFeaturizer
from NGramFeaturizer import NGramFeaturizer
from AnyFeaturizer import AnyFeaturizer
import unittest

class TestCaseFeaturizerManager(unittest.TestCase):
    
    def setUp(self):
        pass
        
    def tearDown(self):
        pass
    
    def test_Fit(self):
        xs = [['a', 'b', 'c']]
        ys = [['tag1', 'tag2', 'tag3']]
    
        f = CRFTaggerFeaturizer()
        f.AddFeaturizer("2gram", NGramFeaturizer(2), shift = 0, uniTag = True, biTag = True)
        
        f.Fit(xs, ys)

    def test_Transform(self): 
        xs = [['a', 'a', 'b', 'c']]
        ys = [['tag1', 'tag1', 'tag2', 'tag3']]
    
        f = CRFTaggerFeaturizer()
        f.AddFeaturizer("2gram", NGramFeaturizer(2), shift = -1, uniTag = True, biTag = True)
        
        f.Fit(xs, ys)
        x = f.Featurize(xs)
        self.assertEqual(x[0][0], (2, -1, 1, 0))
        self.assertEqual(x[0][1], (2, 0, 1, 1))
        self.assertEqual(x[0][2], (3, -1, 2, 2))
        self.assertEqual(x[0][3], (3, 1, 2, 3))

    def test_Transform_TransitionFeature(self): 
        xs = [['a', 'b', 'c']]
        ys = [['tag1', 'tag2', 'tag3']]
    
        f = CRFTaggerFeaturizer()
        f.AddFeaturizer("any", AnyFeaturizer(), shift = 0, uniTag = False, biTag = True)
        
        f.Fit(xs, ys)
        x = f.Featurize(xs)
        self.assertEqual(x[0][0], (1, 1, 2, 1))
        self.assertEqual(x[0][1], (1, 0, 1, 0))
        self.assertEqual(x[0][2], (2, 1, 2, 1))
        self.assertEqual(x[0][3], (2, 0, 1, 0))

    def test_Serialize(self):
        xs = [['a', 'a', 'b', 'c']]
        ys = [['tag1', 'tag1', 'tag2', 'tag3']]
    
        f = CRFTaggerFeaturizer()
        f.AddFeaturizer("2ngram", NGramFeaturizer(2), -1, uniTag = True, biTag = True)
        #fm.AddFeaturizer("any", AnyFeaturizer(), shift = 0, unigram = False, bigram = True)
        #fm.AddFeaturizer("ngram1", NGramFeaturizer(2), shift = 0, unigram = True, bigram = False)
        
        f.Fit(xs, ys)

        f.Serialize("./features.bin")
        
        f1 = CRFTaggerFeaturizer.Deserialize("./features.bin")
        x = f1.Featurize(xs)
        self.assertEqual(x[0][0], (2, -1, 1, 0))
        self.assertEqual(x[0][1], (2, 0, 1, 1))
        self.assertEqual(x[0][2], (3, -1, 2, 2))
        self.assertEqual(x[0][3], (3, 1, 2, 3))


        
if __name__ == "__main__":
    unittest.main()
