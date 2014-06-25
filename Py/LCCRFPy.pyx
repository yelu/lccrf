# distutils: language = c++
# distutils: sources = ../LCCRF/Types.cpp ../LCCRF/FWBW.cpp ../LCCRF/Viterbi.cpp ../LCCRF/LCCRF.cpp 

from libcpp.list cimport list
from libcpp.vector cimport vector

cdef extern from "Types.h":
	cdef cppclass XListType:
		XListType() except +
		void PushBack(int)
		void AddFeature(int, int, int, int)

	cdef cppclass YListType:
		YListType() except +
		void PushBack()
		void AddTag(int)
		int LengthOf(int)
		int TagOf(int, int)
		int Size()

cdef extern from "LCCRF.h":
	cdef cppclass LCCRF:
		LCCRF(int, int, double) except +
		void Fit(XListType&, YListType&, double, int, int)
		void Predict(XListType&, YListType&)
		vector[double]& GetWeights()
		
cdef class X:
	cdef XListType* thisptr
	def __cinit__(self):
		self.thisptr = new XListType()
	def __dealloc__(self):
		del self.thisptr
	def add_feature(self, j, s1, s2, f):
		self.thisptr.AddFeature(j, s1, s2, f)
	def push_back(self, i):
		self.thisptr.PushBack(i)
		
cdef class Y:
	cdef YListType* thisptr
	def __cinit__(self):
		self.thisptr = new YListType()
	def __dealloc__(self):
		del self.thisptr
	def push_back(self):
		self.thisptr.PushBack()
	def add_tag(self, j):
		self.thisptr.AddTag(j)
	def size(self):
		return self.thisptr.Size()
	def length_of(self, i):
		return self.thisptr.LengthOf(i)
	def tag_of(self, i, j):
		return self.thisptr.TagOf(i, j)
		
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
