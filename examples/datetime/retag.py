#!/usr/bin/env python

import xml.etree.ElementTree as ET
import os,sys
import re

class Query:
    def __init__(self, q):
        self.slots = []
        # add O tag
        startPattern = re.compile(r'(<[a-zA-Z0-9_]+>)')
        xmlStr = startPattern.sub(r'</O>\1', q)
        endPattern = re.compile(r'(</[a-zA-Z0-9_]+>)')
        xmlStr = endPattern.sub(r'\1<O>', xmlStr)
        xmlStr = xmlStr.replace('</O><O>', '</O>')
        xmlStr = '<root><O>' + xmlStr + '</O></root>'
        tree = ET.ElementTree(ET.fromstring(xmlStr))
        for child in tree.getroot():
            if child.text != None and len(child.text.strip()) != 0:
                self.slots.append([child.tag.strip(), child.text.strip()])

    @staticmethod
    def ToString(query):
        res = ""
        for slot in query.slots:
            if slot[0] == 'O':
                res += "%s " % slot[1]
            else:
                res += "<%s> %s </%s> " % (slot[0], slot[1], slot[0])
        res.strip()
        return res

if __name__ == "__main__":

    idx = 1
    for line in sys.stdin:
        line = line.strip()
        if len(line) == 0:
            continue     
        try:
            q = Query(line)
            hasDateTime = False
            for slot in q.slots:
                if slot[0].endswith('time'):
                    slot[0] = 'time'
                    hasDateTime = True
                elif slot[0].endswith('date'):
                    slot[0] = 'date'
                    hasDateTime = True
                else:
                    slot[0] = 'O'
            if hasDateTime:
                qStr = Query.ToString(q).lower()
                print "%s\t%s" % (idx, qStr)
                idx += 1
        except:
            print "Badline : %s" % line
