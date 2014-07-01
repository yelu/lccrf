#!/usr/bin/env python

from Vectorizer import Vectorizer
from LCCRFPy import X,Y,XItem,YItem

class VectorizerManager:
    def __init__(self):
        self.__vectorizer = []
    
    def add_vectorizer(self, v):
        self.__vectorizer.append(v)
    
    def allocate_tagid(self, docs):
        for doc in docs:
            for _, tag in doc:
                Vectorizer.get_or_allocate_tagid(tag)
        
    def fit(self, docs):
        self.allocate_tagid(docs)
        for doc in docs:
            for v in self.__vectorizer:
                v.fit(doc)
               
    def transform(self, docs):
        x = X()
        y = Y()
        for doc in docs:
            x.append(XItem(len(doc)))
            for v in self.__vectorizer:
                v.transform(doc, x[-1])
            y.append(YItem())
            for idx, v in enumerate(doc):
                y[-1, idx] = Vectorizer.get_or_allocate_tagid(v[1])
                
        return (x, y)
        
    @property
    def feature_count(self):
        count = 0
        for v in self.__vectorizer:
            count += v.feature_count
        return count
        
    @property
    def tag_count(self):
        return len(Vectorizer.tagid_to_tagname)
        
    def readable_features(self):
        all_features = {}
        with open(filePath, 'w') as f:
            for v in self.__vectorizer:
                all_features.update(v.readable_features())
        return all_features