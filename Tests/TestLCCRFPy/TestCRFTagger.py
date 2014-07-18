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
            f.readline()
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
                #print doc
                if has_tag:
                    docs.append(doc)
        #shuffle(docs)
        print >> sys.stderr, "%d sentences processed." % len(docs)
        return docs
    
    def setUp(self):
        filePath = './data/train.tsv'
        train_doc = self.ParseInput(filePath)
        
        tagger = CRFTagger()
        tagger.fit(train_doc)
        #return
        
        for feature in tagger.readable_features_and_weights():
            print "%d\t%s\t%f" % (feature[0], feature[1], feature[2])
        
        test_doc = self.ParseInput('./data/train.tsv')
        test_y = tagger.transform(test_doc)
        #print test_y
        for i, doc in enumerate(test_doc):
            mismatch = False
            for j, token in enumerate(doc):
                if token[1] != test_y[i][j]:
                    mismatch = True
                    break
            if mismatch:
                print doc
                print test_y[i]
                print ""
        #print test_y

        #debugRes1 = tagger.debug(test_doc[0])
        #print json.dumps(debugRes1, sort_keys = True, indent = 4)
        
        # change tag and get another path result.
        #for idx, i in enumerate(test_doc[0]):
        #    i[1] = test_y[0][idx]
        #debugRes2 = tagger.debug(test_doc[0])
        #print json.dumps(debugRes2, sort_keys = True, indent = 4)
        
    def tearDown(self):
        pass
    
    def test_fit(self):
        pass
        
if __name__ == "__main__":
    unittest.main()
    
    
    
    
