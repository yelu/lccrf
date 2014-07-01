# distutils: language = c++
# distutils: sources = ../LCCRF/Types.cpp ../LCCRF/FWBW.cpp ../LCCRF/Viterbi.cpp ../LCCRF/LCCRF.cpp 

from libcpp.list cimport list
from libcpp.vector cimport vector
from libcpp.pair cimport pair

cdef extern from "Types.h":
    cdef cppclass XType:
        XType() except +
        XType(int) except +
        void AddFeature(int, int, int, int)
        void SetLength(int)
        int Length()
    
    cdef cppclass YType:
        YType() except +
        void AddTag(int, int)

    cdef cppclass XListType:
        XListType() except +
        void AddFeature(int, int, int, int, int)
        void GetAllFeatures(list[list[pair[list[int], list[int]]]]& res)
        XType& At(int)
        void Append(XType&)

    cdef cppclass YListType:
        YListType() except +
        void AddTag(int, int, int)
        void GetAllTags(list[list[int]]& res)
        YType& At(int)
        void Append(YType&)
        

cdef extern from "LCCRF.h":
    cdef cppclass LCCRF:
        LCCRF(int, int, double) except +
        void Fit(XListType&, YListType&, double, int, int)
        void Predict(XListType&, YListType&)
        vector[double]& GetWeights()
        pair[list[list[pair[int, double]]], double] Debug(XType&, YType&)

cdef class XItem:
    cdef XType* thisptr
    cdef int ownership
    def __cinit__(self, int length, ownership = 1):
        self.ownership = ownership
        if self.ownership == 1:
            self.thisptr = new XType(length)
    def __dealloc__(self):
        if self.ownership == 1:
            del self.thisptr
    def __setitem__(self, idx, value):
        self.thisptr.AddFeature(idx[0], idx[1], idx[2], idx[3]) 
    
    property length:
        def __get__(self):
            return self.thisptr.Length()
        def __set__(self, value):
            self.thisptr.SetLength(value)
        
cdef class YItem:
    cdef YType* thisptr
    cdef int ownership
    def __cinit__(self, ownership = 1):     
        self.ownership = ownership
        if self.ownership == 1:
            self.thisptr = new YType()
    def __dealloc__(self):
        if self.ownership == 1:
            del self.thisptr
    def __setitem__(self, idx, value):
        self.thisptr.AddTag(idx, value)

cdef class X:
    cdef XListType* thisptr
    def __cinit__(self):
        self.thisptr = new XListType()
    def __dealloc__(self):
        del self.thisptr
    def __getitem__(self, idx):
        res = XItem(0, 0)
        res.thisptr = &(self.thisptr.At(idx))
        return res
    def __setitem__(self, idx, value):
        self.thisptr.AddFeature(idx[0], idx[1], idx[2], idx[3], idx[4])
    def append(self, XItem x):
        self.thisptr.Append(x.thisptr[0])
    def get_all_features(self):
        cdef list[list[pair[list[int], list[int]]]] res
        self.thisptr.GetAllFeatures(res)
        return res
        
cdef class Y:
    cdef YListType* thisptr
    def __cinit__(self):
        self.thisptr = new YListType()
    def __dealloc__(self):
        del self.thisptr
    def append(self, YItem y):
        self.thisptr.Append(y.thisptr[0])
    def __getitem__(self, idx):
        res = YItem(0)
        res.thisptr = &(self.thisptr.At(idx))
        return res
    def __setitem__(self, idx, value):
        self.thisptr.AddTag(idx[0], idx[1], value)
    def get_all_tags(self):
        cdef list[list[int]] res
        self.thisptr.GetAllTags(res)
        return res
        
cdef class LinearChainCRF:
    cdef LCCRF* thisptr
    def __cinit__(self, int featureCount, int labelCount, float l = 1):
        self.thisptr = new LCCRF(featureCount, labelCount, l)
    def __dealloc__(self):
        del self.thisptr
    def fit(self, X x, Y y, learningRate = 0.01, batch = 1, maxIteration = 1):
        self.thisptr.Fit(x.thisptr[0], y.thisptr[0], learningRate, batch, maxIteration)
    def predict(self, X x, Y y):
        self.thisptr.Predict(x.thisptr[0], y.thisptr[0])
    def get_weights(self):
        return self.thisptr.GetWeights()
    def debug(self, XItem x, YItem y):
        return self.thisptr.Debug(x.thisptr[0], y.thisptr[0])
