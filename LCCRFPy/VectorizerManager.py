#!/usr/bin/env python

import sys
import json
from LCCRFPy import X,Y,XSample,YSample

class VectorizerManager:
    def __init__(self):
        self.__vectorizer = []       
        self.next_featureid = 0
        self.next_tagid = 0
        self.tagid_to_tagname = {}
        self.tagname_to_tagid = {}
    
    def allocate_featureid(self):
        self.next_featureid += 1
        return self.next_featureid - 1
        
    def get_or_allocate_tagid(self, tagname):
        if tagname not in self.tagname_to_tagid:
            self.tagname_to_tagid[tagname] = self.next_tagid
            self.tagid_to_tagname[self.next_tagid] = tagname
            self.next_tagid += 1
        return self.tagname_to_tagid[tagname]
    
    def get_tagname(self, tagid):
        if tagid in self.tagid_to_tagname:
            return self.tagid_to_tagname[tagid]
        else:
            return ""
          
    def add_vectorizer(self, v):
        self.__vectorizer.append(v)
    
    def allocate_tagid(self, docs):
        for doc in docs:
            for _, tag in doc:
                self.get_or_allocate_tagid(tag)
        
    def fit(self, docs):
        self.allocate_tagid(docs)
        for doc in docs:
            for v in self.__vectorizer:
                v.fit(doc)
               
    def transform(self, docs):
        x = X()
        y = Y()
        count = 0
        for doc in docs:
            x.append(XSample(len(doc)))
            for v in self.__vectorizer:
                v.transform(doc, x[-1])
            y.append(YSample())
            for idx, v in enumerate(doc):
                y[-1, idx] = self.get_or_allocate_tagid(v[1])
            count += 1
            print >> sys.stderr, "[%d] transformed." % (count, )
            sys.stderr.flush()
                
        return (x, y)
        
    @property
    def feature_count(self):
        return self.next_featureid
        
    @property
    def tag_count(self):
        return len(self.tagid_to_tagname)
        
    def readable_features(self):
        all_features = {}
        for v in self.__vectorizer:
            all_features.update(v.readable_features())
        return all_features