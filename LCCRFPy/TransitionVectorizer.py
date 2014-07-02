#!/usr/bin/env python

class TransitionVectorizer():
    def __init__(self, vm):
        self.features = {}
        self.tags = vm.tagid_to_tagname
        self.vm = vm
    
    def get_features(self):
        return self.features
        
    def __make_transition(self, s1, s2):
        tr = s1 + " " + s2
        return tr
    
    def fit(self, doc):
        for i in range(1, len(doc)):
            tr = self.__make_transition(doc[i-1][1], doc[i][1])
            if tr not in self.features:
                new_featureid = self.vm.allocate_featureid()
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
        readable_features = {}
        for key, value in self.features.items():
            readable_features[value] = "transition#%s"%(key.strip(),)
        return readable_features
        