#!/usr/bin/env python

class NGramVectorizer:
    def __init__(self, vm, ngram = 2):
        self.__ngram = ngram
        self.tags = vm.tagid_to_tagname
        self.vm = vm
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
                new_featureid = self.vm.allocate_featureid()
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
        readable_features = {}
        for key, value in self.features.items():
            readable_features[value] = "ngram%d#%s"%(self.__ngram, key.strip())
        return readable_features