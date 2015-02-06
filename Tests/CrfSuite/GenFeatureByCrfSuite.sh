#!/usr/bin/env bash

cat ../TestLCCRFPy/data/testset.tsv | python GenDataForCrfSuite.py | \
python ./chunking.py | \
awk 'BEGIN{FS=OFS="\t"}{if(NF==2){print $1,$2} else if(NF==3){print $1,$2;print $1,$3}}' > input.crfsuite.tsv
cat input.crfsuite.tsv | sort | uniq > features.crfsuite.tsv

