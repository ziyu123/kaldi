

all:


include ../../../src/kaldi.mk

CXXFLAGS += -I../../../src

TESTFILES =

OBJFILES = simple-score.o

LIBNAME = kaldi-vvscore

ADDLIBS = ../vvutil/kaldi-vvutil.a \
		  ../../../src/nnet3/kaldi-nnet3.a ../../../src/chain/kaldi-chain.a \
          ../../../src/cudamatrix/kaldi-cudamatrix.a ../../../src/decoder/kaldi-decoder.a \
          ../../../src/lat/kaldi-lat.a ../../../src/lm/kaldi-lm.a \
		  ../../../src/fstext/kaldi-fstext.a ../../../src/hmm/kaldi-hmm.a \
		  ../../../src/feat/kaldi-feat.a \
          ../../../src/transform/kaldi-transform.a ../../../src/gmm/kaldi-gmm.a \
          ../../../src/tree/kaldi-tree.a ../../../src/util/kaldi-util.a \
          ../../../src/matrix/kaldi-matrix.a ../../../src/base/kaldi-base.a

include ../../../src/makefiles/default_rules.mk

