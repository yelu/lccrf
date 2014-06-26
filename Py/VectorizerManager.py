#!/usr/bin/env python

from Vectorizer import Vectorizer
from LCCRFPy import X,Y

class VectorizerManager:
    def __init__(self):
        self.__vectorizer = []
    
    def add_vectorizer(self, v):
        self.__vectorizer.append(v)
    
    def get_all_tags(self, docs):
        for doc in docs:
            for _, tag in doc:
                Vectorizer.get_or_allocate_tagid(tag)
        
    def fit(self, docs):
        self.get_all_tags(docs)
        for doc in docs:
            for v in self.__vectorizer:
                v.fit(doc)
        
    def transform(self, docs):
        x = X()
        y = Y()
        for doc in docs:
            x.push_back(len(doc))
            for v in self.__vectorizer:
                v.transform(doc, x)
            y.push_back()
            for _, tag in doc:
                y.add_tag(Vectorizer.get_or_allocate_tagid(tag))
                
        return (x, y)