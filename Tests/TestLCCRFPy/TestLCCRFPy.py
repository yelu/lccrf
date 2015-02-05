#!/usr/bin/env python

import os,sys
lib_path = os.path.abspath('../../LCCRFPy')
sys.path.append(lib_path)
from LCCRFPy import *
import unittest

class TestCaseLinearChainCRF(unittest.TestCase):
    
    def setUp(self):
        self.x = X()
        self.x[0].length = 2
        print self.x[0].length
        self.x[0, 0, -1, 0, 0] = 1
        self.x[0, 1, 0, 1, 1] = 1
        self.x[0, 1, 1, 1, 1] = 1
        print self.x.to_array()
        
        self.y = Y()
        self.y[0, 0] = 0
        self.y[0, 1] = 1
        print self.y.to_array()
        
        self.crf = LinearChainCRF(2, 2)
        
    def tearDown(self):
        pass
    
    def test_fit(self):
        self.crf.fit(self.x, self.y, 1000, 0.1, 0.1)
        weights = self.crf.get_weights()
        self.assertAlmostEqual(weights[0], 1.63906, 4)
        self.assertAlmostEqual(weights[1], 1.63906, 4)
        
if __name__ == "__main__":
    unittest.main()
    
    
    
    
