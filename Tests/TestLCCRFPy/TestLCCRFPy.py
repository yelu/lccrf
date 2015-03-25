#!/usr/bin/env python

import os,sys
from LCCRFPy import *
import unittest

class TestCaseLCCRF(unittest.TestCase):
    
    def setUp(self):
        self.x = X()
        self.x[0].length = 2
        print self.x[0].length
        self.x[0, 0, -1, 0, 0] = 1
        self.x[0, 1, -1, 1, 1] = 1
        print self.x.ToArray()
        
        self.y = Y()
        self.y[0, 0] = 0
        self.y[0, 1] = 1
        print self.y.ToArray()
        
        self.crf = LCCRFPy(2, 2)
        
    def tearDown(self):
        pass
    
    def test_fit(self):
        self.crf.Fit(self.x, self.y, 1000, 0.1, 0.1)
        weights = self.crf.GetWeights()
        self.assertAlmostEqual(weights[0], 2.18198, 4)
        self.assertAlmostEqual(weights[1], 2.18198, 4)
        
if __name__ == "__main__":
    unittest.main()
    
    
    
    
