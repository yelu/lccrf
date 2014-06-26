#!/usr/bin/env python

from VectorizerManager import VectorizerManager
from NGramVectorizer import NGramVectorizer
from TransitionVectorizer import TransitionVectorizer
from LCCRFPy import X,Y

if __name__ == "__main__":
    
    doc = [[['a'], 'tag1'], [['b'], 'tag2']]
    
    vm = VectorizerManager()
    vm.add_vectorizer(NGramVectorizer(1))
    vm.add_vectorizer(TransitionVectorizer())
    
    vm.fit([doc])
    x, y = vm.transform([doc])
    
    print x.get_all_features()
    print y.get_all_tags()
    
        