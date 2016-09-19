#!/usr/bin/env python
import os,sys
import argparse
from CRFTagger import *

if __name__ == "__main__":

    parser = argparse.ArgumentParser()
    parser.add_argument("-d", "--data", required=True, action="store", dest="data", \
                        default="./train.tsv", help="directory of training data")
    parser.add_argument("-m", "--model", required=True, action="store", dest="model", \
                        default="./model", help="directory for model files")
    args = parser.parse_args()

    xs, ys = CRFTagger.LoadTrainData(args.data)
    config = {"modelDir":args.model, "features":{"cfg":{"grammarFile":"./en-us.datetime.cfg.xml"}}}
    tagger = CRFTagger(config)
    tagger.Train(xs, ys, maxIteration = 10, learningRate = 0.01, variance = 0.01)

    tagger = CRFTagger.Load(config["modelDir"])

    #test on training set.
    stat = tagger.Evaluate(xs, ys)
    print json.dumps(stat, indent=4)
