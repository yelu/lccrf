#!/usr/bin/env python

import os,sys
lib_path = os.path.abspath('../LCCRFPy')
sys.path.append(lib_path)
from CRFTagger import *
import unittest
import re

class TestCRFTagger(unittest.TestCase):

    def ParseInput(self, filePath):
        docs = []
        with open(filePath) as f:
            f.readline()
            for line in f:
                fields = line.strip().split('\t')
                if len(fields) < 2:
                    continue
                sentence = fields[1]
                words = sentence.split()
                doc = []
                tag = 'oos'
                has_tag = False
                for word in words:
                    if word.startswith('<') and not word.startswith('</'):
                        tag = word[1:-1]
                        has_tag = True
                        continue
                    if word.startswith('</'):
                        tag = 'oos'
                        continue
                    doc.append([[word], tag])
                #print doc
                if has_tag:
                    docs.append(doc)
        return docs
    
    def setUp(self):
        filePath = './data/train.tsv'
        docs = self.ParseInput(filePath)
        
        tagger = CRFTagger()
        tagger.fit(docs)
        
        testX = self.ParseInput('./data/test.tsv')
        testY = tagger.transform(testX[0:1])
        print testY

        debugRes1 = tagger.debug(testX[0])
        print debugRes1
        
    def tearDown(self):
        pass
    
    def test_fit(self):
        pass
        
if __name__ == "__main__":
    unittest.main()
    
    
    
    
