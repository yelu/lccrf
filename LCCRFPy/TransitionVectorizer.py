#!/usr/bin/env python

from Vectorizer import Vectorizer
from LCCRFPy import X,Y

class TransitionVectorizer(Vectorizer):
    def __init__(self, ngram = 2):
        self.__ngram = ngram
        self.__featurename_to_featureid = {}
    
    def get_features(self):
        return self.__featurename_to_featureid
        
    def __make_transition(self, s1, s2):
        tr = s1 + " " + s2
        return tr
    
    def fit(self, doc):
        self.__tag_count = len(Vectorizer.tagid_to_tagname)
        for i in range(1, len(doc)):
            tr = self.__make_transition(doc[i-1][1], doc[i][1])
            if tr not in self.__featurename_to_featureid:
                new_featureid = Vectorizer.allocate_featureid()
                self.__featurename_to_featureid[tr] = new_featureid

    def transform(self, doc, x):
        for j in range(1, len(doc)):
            for s2 in range(0, self.__tag_count):
                for s1 in range(0, self.__tag_count):
                    tr = self.__make_transition(Vectorizer.get_tagname(s1), Vectorizer.get_tagname(s2))                   
                    if tr in self.__featurename_to_featureid:
                        x[j, s1, s2, self.__featurename_to_featureid[tr]] = 1
        
        return x
        