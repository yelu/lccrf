#!/usr/bin/env python

import os
from LCCRFPy import *

if __name__ == "__main__":

	x = X()
	x.push_back(2)
	x.add_feature(0, -1, 0, 0)
	x.add_feature(1, 0, 1, 1)
	x.add_feature(1, 1, 1, 1)
	
	y = Y()
	y.push_back()
	y.add_tag(0)
	y.add_tag(1)
	
	crf = LinearChainCRF(2, 2, 0.1)
	crf.fit(x, y, 0.1, 1, 1000)
	weights = crf.get_weights()
	print weights