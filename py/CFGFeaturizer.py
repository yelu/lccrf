#!/usr/bin/env python
import os
import argparse
from LCCRFPy import CFGParserPy

class CFGFeaturizer(object):
    def __init__(self, args, cwd):
        self.args = args
        self.inputs = self.args.input.strip().split(',')
        self.output = self.args.output
        self._cfgParser = CFGParserPy()
        grammarFile = os.path.join(cwd, self.args.grammarFile)
        self._cfgParser.LoadXml(grammarFile)

    @staticmethod
    def GetArgParser():
        argParser = argparse.ArgumentParser(description = 'CFG featurizer')
        argParser.add_argument('--input', action="store", dest="input", default="")
        argParser.add_argument('--output', action="store", dest="output", default="")
        argParser.add_argument('--grammarFile', action="store", dest="grammarFile", default="")
        return argParser

    def Process(self, input):
        query = input[0]
        matches = self._cfgParser.Parse(query)
        output = []
        for name, poses in matches.items():
            output += [(pos[0], pos[1], name) for pos in poses]
        return output
