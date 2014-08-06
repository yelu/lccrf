#!/usr/bin/env python

import os,sys
lib_path = os.path.abspath('../../LCCRFPy')
sys.path.append(lib_path)
from CRFTagger import *
import unittest
import re
import json
from random import shuffle

class TestCRFTagger(unittest.TestCase):

    def ParseInput(self, filePath):
        docs = []
        with open(filePath) as f:
            for line in f:
                fields = line.strip().split('\t')
                if len(fields) < 2:
                    continue
                sentence = fields[1].strip().replace("<", " <")
                sentence = sentence.replace(">", "> ")
                words = []
                for token in sentence.split():
                    if token != "":
                        words.append(token)
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
                if has_tag:
                    docs.append(doc)
        #shuffle(docs)
        return docs
    
    def setUp(self):
        filePath = './data/train.tsv'
        train_doc = self.ParseInput(filePath)
        
        tagger = CRFTagger()
        tagger.fit(train_doc)
        
        tagger.save('./tagger.model')
        
        test_doc = self.ParseInput('./data/train.tsv')
        res = tagger.test(test_doc)
        print json.dumps(res, sort_keys = True, indent = 4)
        
    def tearDown(self):
        pass
    
    def test_fit(self):
        pass
        
if __name__ == "__main__":
    unittest.main()
    
    
    
    
