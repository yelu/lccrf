#!/usr/bin/env python

import os,sys
lib_path = os.path.abspath('../../LCCRFPy')
sys.path.append(lib_path)
from CRFTagger import *
import unittest
import re
import json
from random import shuffle

def GenDataForCrfSuite(input):
    for line in input:
        fields = line.strip().split('\t')
        if len(fields) < 2:
            continue
        sentence = fields[1].strip().replace("<", " <")
        sentence = sentence.replace(">", "> ")
        words = []
        for token in sentence.split():
            if token != "":
                words.append(token)
        tag = 'oos'
        for word in words:
            if word.startswith('<') and not word.startswith('</'):
                tag = word[1:-1]
                continue
            if word.startswith('</'):
                tag = 'oos'
                continue
            print >> sys.stdout, "%s\t%s" % (word, tag)
        print >> sys.stdout, ""

if __name__ == "__main__":
   GenDataForCrfSuite(sys.stdin) 
    
    
    
