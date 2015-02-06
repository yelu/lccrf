#!/usr/bin/env bash

cat ../TestLCCRFPy/data/testset.tsv | python GenDataForCrfSuite.py > input.crfsuite.tsv
cat input.crfsuite.tsv | python ./chunking_unigram.py > features.crfsuite.tsv
cat input.crfsuite.tsv | python ./chunking_bigram.py >> features.crfsuite.tsv
