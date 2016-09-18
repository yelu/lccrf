#!/usr/bin/env python
import os,sys
from CRFTagger import *

if __name__ == "__main__":

    train_data = sys.argv[1]

    xs, ys = CRFTagger.LoadTrainData(train_data)
    config = {"modelDir":"./model", "features":{"cfg":{"grammarFile":"./en-us.datetime.grammar.xml"}}}
    tagger = CRFTagger(config)
    tagger.Train(xs, ys, maxIteration = 10, learningRate = 0.05, variance = 0.0008)

    tagger = CRFTagger.Load(config["modelDir"])

    #test on training set.
    stat = tagger.Evaluate(xs, ys)
    print json.dumps(stat, indent=4)
