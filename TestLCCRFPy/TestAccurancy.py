#!/usr/bin/env python

import os,sys
lib_path = os.path.abspath('../LCCRFPy')
sys.path.append(lib_path)
from LCCRFPy import *
from VectorizerManager import VectorizerManager
from NGramVectorizer import NGramVectorizer
from TransitionVectorizer import TransitionVectorizer
import unittest
import re

class TestAccurancy(unittest.TestCase):

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
        
        vm = VectorizerManager()
        vm.add_vectorizer(NGramVectorizer(1))
        vm.add_vectorizer(TransitionVectorizer())
        
        vm.fit(docs)
        x, y = vm.transform(docs)
        #x, y = x.get_all_features(), y.get_all_tags()
        print "feature extraction completed."
        #print x.get_all_features()
        print y.get_all_tags()
        crf = LinearChainCRF(vm.feature_count, vm.tag_count, 0.1)
        crf.fit(x, y, 0.1, 1, 20)
        weights = crf.get_weights()
        #input_A = input("Input: ")
        #print input_A
        testX, refY = vm.transform(self.ParseInput('./data/test.tsv'))
        print testX.get_all_features()
        testY = Y()
        crf.predict(testX, testY)
        print testY.get_all_tags()
        print "features:%d, tags:%d, weights:%d" % (vm.feature_count, vm.tag_count, len(weights))
        vm.visual_features(weights, './features')
        
    def tearDown(self):
        pass
    
    def test_fit(self):
        pass
        
if __name__ == "__main__":
    unittest.main()
    
    
    
    
