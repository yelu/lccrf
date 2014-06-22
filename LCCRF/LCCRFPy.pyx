# distutils: language = c++
# distutils: sources = Types.cpp FWBW.cpp Viterbi.cpp LCCRF.cpp 

from libcpp.list cimport list
from libcpp.vector cimport vector

cdef extern from "Types.h":
	cdef cppclass X:
		X(int) except +
		void SetFeature(int, int, int, int)
		
	cdef cppclass Y:
		Y() except +
		void AppendTag(int)

cdef extern from "LCCRF.h":
	cdef cppclass LCCRF:
		LCCRF(int, int, double) except +
		void Fit(const list[X]&, const list[Y]&, double, int, int)
		void Predict(const X&, Y&)
		
cdef class PyX:
	cdef X* thisptr
	def __cinit__(self, int length):
		self.thisptr = new X(length)
	def __dealloc__(self):
		del self.thisptr
	def set_feature(self, j, s1, s2, f):
		self.thisptr.SetFeature(j, s1, s2, f)
		
cdef class PyY:
	cdef Y* thisptr
	def __cinit__(self):
		self.thisptr = new Y()
	def __dealloc__(self):
		del self.thisptr
	def append_tag(self, j):
		self.thisptr.AppendTag(j)

cdef class PyLCCRF:
	cdef LCCRF* thisptr
	def __cinit__(self, int featureCount, int labelCount, float l = 1):
		self.thisptr = new LCCRF(featureCount, labelCount, l)
	def __dealloc__(self):
		del self.thisptr
	cdef Fit(self, list[X]& xs, list[Y]& ys, learningRate = 0.01, batch = 1, maxIteration = 1):
		self.thisptr.Fit(xs, ys, learningRate, batch, maxIteration)
	cdef Predict(self, X& doc, Y& tags):
		self.thisptr.Predict(doc, tags)
