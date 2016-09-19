# distutils: language = c++
# distutils: sources = ../c/lib/CFGParser.cpp ../c/lib/Types.cpp ../c/lib/FWBW.cpp ../c/lib/Viterbi.cpp ../c/lib/LCCRF.cpp ../c/lib/SGDL1.cpp ../c/lib/MurmurHash3.cpp

from libcpp.list cimport list
from libcpp.string cimport string
from libcpp.vector cimport vector
from libcpp.pair cimport pair
from libcpp.map cimport map

cdef extern from "LCCRF.h":
    cdef cppclass LCCRF:
        LCCRF() except +
        void Fit(string&, int, double, double)
        vector[int] Predict(vector[pair[int, int]]&, int) except +
        vector[double]& GetWeights()
        void Save(string&)
        void Load(string&)

cdef class LCCRFPy:
    cdef LCCRF c_lccrf
    def __cinit__(self):
        self.c_lccrf = LCCRF()
    def Fit(self, dataFile, maxIteration = 1, learningRate = 0.001, variance = 0.001):
        self.c_lccrf.Fit(dataFile, maxIteration, learningRate, variance)
    def Predict(self, x, length):
        y = self.c_lccrf.Predict(x, length)
        return y
    def GetWeights(self):
        return self.c_lccrf.GetWeights()
    def Save(self, weightsFile):
        self.c_lccrf.Save(weightsFile)
    def Load(self, weightsFile):
        self.c_lccrf.Load(weightsFile)

cdef extern from "CFGParser.h":
    cdef cppclass CFGParser:
        CFGParser() except +
        void LoadXml(const string&)
        map[string, vector[pair[int, int]]] Parse(const vector[string]&)

cdef class CFGParserPy:
    cdef CFGParser c_cfgparser
    def __cinit__(self):
        self.c_cfgparser = CFGParser()
    def LoadXml(self, filePath):
        self.c_cfgparser.LoadXml(filePath)
    def Parse(self, tokenizedQuery):
        cdef map[string, vector[pair[int, int]]] res = self.c_cfgparser.Parse(tokenizedQuery)
        return res
