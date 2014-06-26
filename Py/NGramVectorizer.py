#!/usr/bin/env python

from Vectorizer import Vectorizer
from LCCRFPy import X,Y

class NGramVectorizer(Vectorizer):
    def __init__(self, ngram = 2):
        self.__ngram = ngram
        self.__tag_count = len(Vectorizer.tagid_to_tagname)
        self.__featurename_to_featureid = {}
    
    def get_features(self):
        return self.__featurename_to_featureid
        
    def __make_gram(self, doc, end_pos, tag):
        gram = tag + " "
        for j in range(end_pos - self.__ngram  + 1, end_pos + 1):
            gram += doc[j][0][0]
            gram += "\t"
        return gram
    
    def fit(self, doc):
        self.__tag_count = len(Vectorizer.tagid_to_tagname)
        for i in range(self.__ngram - 1, len(doc)):
            gram = self.__make_gram(doc, i, doc[i][1])
            if gram not in self.__featurename_to_featureid:
                new_featureid = Vectorizer.allocate_featureid()
                self.__featurename_to_featureid[gram] = new_featureid

    def transform(self, doc, x):
        if self.__ngram == 1:
            for s2 in range(0, self.__tag_count):
                gram = self.__make_gram(doc, 0, Vectorizer.get_tagname(s2))
                if gram in self.__featurename_to_featureid:
                    x.add_feature(0, -1, s2, self.__featurename_to_featureid[gram])
            
            for j in range(1, len(doc)):
                for s2 in range(0, self.__tag_count):
                    for s1 in range(0, self.__tag_count):
                        gram = self.__make_gram(doc, j, Vectorizer.get_tagname(s2))
                        if gram in self.__featurename_to_featureid:
                            x.add_feature(j, s1, s2, self.__featurename_to_featureid[gram])
        
        else:    
            for j in range(self.__ngram - 1, len(doc)):
                for s2 in range(0, self.__tag_count):
                    for s1 in range(0, self.__tag_count):
                        gram = self.__make_gram(doc, j, Vectorizer.get_tagname(s2))
                        if gram in self.__featurename_to_featureid:
                            x.add_feature(j, s1, s2, self.__featurename_to_featureid[gram])
        return x
        