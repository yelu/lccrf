#!/usr/bin/env python

class Vectorizer:
    next_featureid = 0
    next_tagid = 0
    tagid_to_tagname = {}
    tagname_to_tagid = {}
    
    @classmethod
    def allocate_featureid(cls):
        cls.next_featureid += 1
        return cls.next_featureid - 1
        
    @classmethod
    def get_or_allocate_tagid(cls, tagname):
        if tagname not in cls.tagname_to_tagid:
            cls.tagname_to_tagid[tagname] = cls.next_tagid
            cls.tagid_to_tagname[cls.next_tagid] = tagname
            cls.next_tagid += 1
        return cls.tagname_to_tagid[tagname]
    
    @classmethod
    def get_tagname(cls, tagid):
        if tagid in cls.tagid_to_tagname:
            return cls.tagid_to_tagname[tagid]
        else:
            return ""
