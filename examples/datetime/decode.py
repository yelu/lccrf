#!/usr/bin/env python
import os,sys
from CRFTagger import *

if __name__ == "__main__":

    modelDir = sys.argv[1]

    tagger = CRFTagger.Load(modelDir)

    while 1:
        query = sys.stdin.readline()
        tokenizedQuery = query.strip().split()
        if len(tokenizedQuery) == 0:
            continue    
        tags = tagger.Predict(tokenizedQuery)
        print tags

