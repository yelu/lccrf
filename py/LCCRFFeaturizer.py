#!/usr/bin/env python

import os
import sys
import json
import argparse
import importlib
import shutil
from log import *

class LCCRFFeaturizer(object):
    def __init__(self, pipelineFile = None, outputs = None):
        if pipelineFile != None and outputs != None:
	    self._pipelineFile = pipelineFile
	    self._outputs = outputs
	    self._variables = {}
	    self._featureIDMap = {}
	    self._nextId = 0
	    self.LoadPipeline(self._pipelineFile)

    @staticmethod
    def Load(srcDir):
        lccrfFeaturizer = LCCRFFeaturizer()
        lccrfFeaturizer._pipelineFile = os.path.join(srcDir, "lccrf.pipeline.txt")
        lccrfFeaturizer.LoadPipeline(lccrfFeaturizer._pipelineFile)

        outputsFile = os.path.join(srcDir, "lccrf.pipeline.outputs.txt")
        lccrfFeaturizer._outputs = []
        with open(outputsFile, 'r') as f:
	    for line in f:
		output = line.strip()
		if len(output) != 0:
                    lccrfFeaturizer._outputs.append(output)

        lccrfFeaturizer._featureIDMap = {}
        lccrfFeaturizer._nextId = 0
        featuresFile = os.path.join(srcDir, "lccrf.features.txt")
        with open(featuresFile, 'r') as f:
	    for line in f:
                line = line.strip()
                if len(line) != 0: feat = eval(line)
                lccrfFeaturizer._featureIDMap[feat[0]] = feat[1]
                if feat[1] >= lccrfFeaturizer._nextId : 
		    lccrfFeaturizer._nextId = feat[1] + 1

	lccrfFeaturizer._variables = {}
	return lccrfFeaturizer

    def LoadPipeline(self, pipelineFile):
        self._processors = []
        with open(pipelineFile) as f:
            for line in f:
                line = line.strip()
                if len(line) == 0 or line[0] == "#":
                    continue
                processorName = line.split()[0]
                processorClass = getattr(importlib.import_module(processorName), \
                                         processorName)
                argParser = processorClass.GetArgParser()
                args = argParser.parse_args(line.split()[1:])
                processor = processorClass(args, os.path.dirname(pipelineFile))
                self._processors.append(processor)

    def Featurize(self, query, lock = False):
        query = query.strip()
        self._variables['query'] = [query]
        ret = None
        for processor in self._processors:
            input = []
            for item in processor.input:
                input += self._variables[item]
            self._variables[processor.output] = processor.Process(input)
        ret = []
	print self._variables
        if self._outputs != None:
            for output in self._outputs:
                features = self._variables[output]
                # allocate an id for features.
                for feature in features:
                    featureKey = (output, feature[-1])
                    if not lock:
                        if featureKey not in self._featureIDMap:
                            self._featureIDMap[featureKey] = self._nextId
                            self._nextId += 1
                    if featureKey in self._featureIDMap:
                        ret.append((feature[-2] - 1, self._featureIDMap[featureKey]))
        return ret

    def Save(self, outDir):
	features = sorted(self._featureIDMap.items())
        with open(os.path.join(outDir, "lccrf.features.txt"), 'w') as f:
            for feature in features:
                print >> f, feature

        # copy pipeline file
        #pipelineFile = os.path.join(outDir, "lccrf.pipeline.txt")
        # shutil.copy(self._pipelineFile, pipelineFile)

        # save pipeline.outputs
        outputsFile = os.path.join(outDir, "lccrf.pipeline.outputs.txt")
        with open(outputsFile, 'w') as f:
            for output in self._outputs:
                print >> f,output
