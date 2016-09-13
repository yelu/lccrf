# distutils: language = c++
# distutils: sources = ../LCCRF/Types.cpp ../LCCRF/FWBW.cpp ../LCCRF/Viterbi.cpp ../LCCRF/LCCRF.cpp ../LCCRF/SGDL1.cpp ../LCCRF/MurmurHash3.cpp

from libcpp.list cimport list
from libcpp.string cimport string
from libcpp.vector cimport vector
from libcpp.pair cimport pair

cdef extern from "Types.h":
    cdef cppclass XSampleType:
        XSampleType() except +
        XSampleType(int) except +
        void SetFeature(int, int, int, int)
        void SetLength(int)
        int Length()
    
    cdef cppclass YSampleType:
        YSampleType() except +
        void SetTag(int, int)

    cdef cppclass XType:
        XType() except +
        void SetFeature(int, int, int, int, int)
        void ToArray(list[list[pair[list[int], list[int]]]]& res)
        XSampleType& At(int)
        void Append(XSampleType&)

    cdef cppclass YType:
        YType() except +
        void SetTag(int, int, int)
        void ToArray(list[list[int]]& res)
        YSampleType& At(int)
        void Append(YSampleType&)
        

cdef extern from "LCCRF.h":
    cdef cppclass LCCRF:
        LCCRF() except +
        LCCRF(int, int) except +
        void Fit(XType&, YType&, int, double, double) except +
        void Predict(XType&, YType&)
        vector[double]& GetWeights()
        pair[list[list[pair[int, double]]], double] Debug(XSampleType&, YSampleType&)
        void Save(string&)
        void Load(string&)

cdef class XSample:
    cdef XSampleType* thisptr
    cdef int ownership
    def __cinit__(self, int length, ownership = 1):
        self.ownership = ownership
        if self.ownership == 1:
            self.thisptr = new XSampleType(length)
    def __dealloc__(self):
        if self.ownership == 1:
            del self.thisptr
    def __setitem__(self, idx, value):
        self.thisptr.SetFeature(idx[0], idx[1], idx[2], idx[3]) 
    
    property length:
        def __get__(self):
            return self.thisptr.Length()
        def __set__(self, value):
            self.thisptr.SetLength(value)
        
cdef class YSample:
    cdef YSampleType* thisptr
    cdef int ownership
    def __cinit__(self, ownership = 1):     
        self.ownership = ownership
        if self.ownership == 1:
            self.thisptr = new YSampleType()
    def __dealloc__(self):
        if self.ownership == 1:
            del self.thisptr
    def __setitem__(self, idx, value):
        self.thisptr.SetTag(idx, value)

cdef class X:
    cdef XType* thisptr
    def __cinit__(self):
        self.thisptr = new XType()
    def __dealloc__(self):
        del self.thisptr
    def __getitem__(self, idx):
        res = XSample(0, 0)
        res.thisptr = &(self.thisptr.At(idx))
        return res
    def __setitem__(self, idx, value):
        self.thisptr.SetFeature(idx[0], idx[1], idx[2], idx[3], idx[4])
    def Append(self, XSample x):
        self.thisptr.Append(x.thisptr[0])
    def ToArray(self):
        cdef list[list[pair[list[int], list[int]]]] res
        self.thisptr.ToArray(res)
        return res
        
cdef class Y:
    cdef YType* thisptr
    def __cinit__(self):
        self.thisptr = new YType()
    def __dealloc__(self):
        del self.thisptr
    def Append(self, YSample y):
        self.thisptr.Append(y.thisptr[0])
    def __getitem__(self, idx):
        res = YSample(0)
        res.thisptr = &(self.thisptr.At(idx))
        return res
    def __setitem__(self, idx, value):
        self.thisptr.SetTag(idx[0], idx[1], value)
    def ToArray(self):
        cdef list[list[int]] res
        self.thisptr.ToArray(res)
        return res
        
cdef class LCCRFPy:
    cdef LCCRF* thisptr
    def __cinit__(self, featureCount = None, labelCount = None):
        if featureCount == None or labelCount == None:
            self.thisptr = new LCCRF()
        else:
            self.thisptr = new LCCRF(featureCount, labelCount)
    def __dealloc__(self):
        del self.thisptr
    def Fit(self, X x, Y y, maxIteration = 1, learningRate = 0.001, variance = 0.001):
        self.thisptr.Fit(x.thisptr[0], y.thisptr[0], maxIteration, learningRate, variance)
    def Predict(self, X x):
        y = Y()
        self.thisptr.Predict(x.thisptr[0], y.thisptr[0])
        return y
    def GetWeights(self):
        return self.thisptr.GetWeights()
    def Debug(self, XSample x, YSample y):
        return self.thisptr.Debug(x.thisptr[0], y.thisptr[0])
    def Save(self, path):
        self.thisptr.Save(path)
    def Load(self, path):
        self.thisptr.Load(path)


