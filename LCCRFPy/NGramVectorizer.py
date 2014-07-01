#!/usr/bin/env python

from Vectorizer import Vectorizer
from LCCRFPy import X,Y,XItem

class NGramVectorizer(Vectorizer):
    def __init__(self, ngram = 2):
        self.__ngram = ngram
        self.tags = Vectorizer.tagid_to_tagname
        self.features = {}
    
    def get_features(self):
        return self.features
        
    def __make_gram(self, doc, end_pos, tag):
        gram = tag + " "
        for j in range(end_pos - self.__ngram  + 1, end_pos + 1):
            gram += doc[j][0][0]
            gram += "\t"
        return gram
    
    def fit(self, doc):
        for i in range(self.__ngram - 1, len(doc)):
            gram = self.__make_gram(doc, i, doc[i][1])
            if gram not in self.features:
                new_featureid = Vectorizer.allocate_featureid()
                self.features[gram] = new_featureid

    def transform(self, doc, x):
        for j in range(self.__ngram - 1, len(doc)):
            for s2 in self.tags.keys():
                if j == 0:
                    s1_range = [-1]
                else:
                    s1_range = self.tags.keys()
                for s1 in s1_range:
                    gram = self.__make_gram(doc, j, self.tags[s2])
                    if gram in self.features:
                        x[j, s1, s2, self.features[gram]] = 1
        return x
    
    @property
    def feature_count(self):
        return len(self.features)
        
    def readable_features(self):
        new_dict = dict(zip(self.features.values(), self.features.keys()))
        return new_dict