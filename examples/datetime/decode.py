#!/usr/bin/env python
import os,sys
import argparse
from CRFTagger import *

if __name__ == "__main__":

    parser = argparse.ArgumentParser()
    parser.add_argument("-m", "--model", required=True, action="store", dest="model", \
                        default="./model", help="directory for model files")
    args = parser.parse_args()

    tagger = CRFTagger.Load(args.model)

    while True:
        query = sys.stdin.readline()
        tokenizedQuery = query.strip().split()
        if len(tokenizedQuery) == 0:
            continue    
        tags = tagger.Predict(tokenizedQuery)
        print tags

