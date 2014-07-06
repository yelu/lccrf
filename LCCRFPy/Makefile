#edit-mode: -*- Makefile -*-

MAKE=make
PYTHON="/cygdrive/c/Anaconda/python.exe"
CC=gcc
CXX=g++
CXXFLAGS=-g \
  -pipe \
  -W \
  -Wall \
  -fPIC
CFLAGS=-g \
  -pipe \
  -W \
  -Wall \
  -fPIC
CPPFLAGS=-Dprivate=public \
  -D_GNU_SOURCE \
  -D__STDC_LIMIT_MACROS \
INCPATH=
DEP_INCPATH=


.PHONY:all
all:py_module

.PHONY:py_module
py_module:
	export VS90COMNTOOLS=${$(VS110COMNTOOLS)// /\ }
	$(PYTHON) Setup.py build --compiler=msvc
	cp ./build/lib.*/LCCRFPy.pyd .

.PHONY:clean
clean:
	rm -f ./LCCRFPy.pyd
	rm -rf ./build
	rm -f ./*.pyc

.PHONY:dist
dist:
	tar czvf output.tar.gz output
	@echo "make dist done"

.PHONY:distclean
distclean:
	rm -f output.tar.gz
	@echo "make distclean done"

.PHONY:love
love:
	@echo "make love done"
