#!/usr/bin/env python

from NGramVectorizer import *

if __name__ == "__main__":
    
    doc = [[['a'], 'tag1'], [['b'], 'tag2']]
    Vectorizer.get_or_allocate_tagid('tag1')
    Vectorizer.get_or_allocate_tagid('tag2')
    v = NGramVectorizer(1)
    v.fit(doc)
    print v.get_features()
    
    x = X()
    y = Y()
    x.push_back(2)
    y.push_back()
    v.transform(doc, x)
    print x.get_all_features()
    #print y.get_all_tags()
    
        