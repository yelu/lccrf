#!/usr/bin/env python

from Vectorizer import Vectorizer
from LCCRFPy import X,Y

class TransitionVectorizer(Vectorizer):
    def __init__(self, ngram = 2):
        self.__ngram = ngram
        self.features = {}
        self.tags = Vectorizer.tagid_to_tagname
    
    def get_features(self):
        return self.features
        
    def __make_transition(self, s1, s2):
        tr = s1 + " " + s2
        return tr
    
    def fit(self, doc):
        for i in range(1, len(doc)):
            tr = self.__make_transition(doc[i-1][1], doc[i][1])
            if tr not in self.features:
                new_featureid = Vectorizer.allocate_featureid()
                self.features[tr] = new_featureid

    def transform(self, doc, x):
        for j in range(1, len(doc)):
            for s2 in self.tags.keys():
                for s1 in self.tags.keys():
                    tr = self.__make_transition(self.tags[s1], self.tags[s2])                   
                    if tr in self.features:
                        x[j, s1, s2, self.features[tr]] = 1
        
        return x
    
    @property
    def feature_count(self):
        return len(self.features)
        
    
    def readable_features(self):
        new_dict = dict(zip(self.features.values(), self.features.keys()))
        return new_dict
        