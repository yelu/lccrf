#!/usr/bin/env python
import os
import argparse

class FeatureShifter(object):
    def __init__(self, args, cwd):
        self.args = args
        self.inputs = self.args.input.strip().split(',')
        self.output = self.args.output
        self._shift = self.args.shift

    @staticmethod
    def GetArgParser():
        argParser = argparse.ArgumentParser(description = 'Feature shifter')
        argParser.add_argument('--input', action="store", dest="input", default="")
        argParser.add_argument('--output', action="store", dest="output", default="")
        argParser.add_argument('--shift', action="store", dest="shift", default="0", type=int)
        return argParser

    def Process(self, input):
        output = []
        for s,e,fid in input:
            output.append((s + self._shift, e + self._shift, fid))
        return output

