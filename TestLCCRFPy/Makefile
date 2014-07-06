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
all:
	$(MAKE) -C ../LCCRFPy

.PHONY:test
test:
	$(PYTHON) TestLCCRFPy.py >/dev/null
	$(PYTHON) TestNGramVectorizer.py >/dev/null
	$(PYTHON) TestVectorizerManager.py >/dev/null
	$(PYTHON) TestCRFTagger.py >/dev/null

.PHONY:clean
clean:
	rm -f ./*.pyc
	$(MAKE) -C ../LCCRFPy clean

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
