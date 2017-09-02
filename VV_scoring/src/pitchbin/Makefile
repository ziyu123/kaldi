
all:
EXTRA_CXXFLAGS = -Wno-sign-compare
include ../../../src/kaldi.mk

CXXFLAGS += -I../../../src

BINFILES = pit-score-referred-wav

OBJFILES =

TESTFILES =

ADDLIBS = ../../../src/hmm/kaldi-hmm.a ../../../src/feat/kaldi-feat.a \
          ../../../src/transform/kaldi-transform.a ../../../src/gmm/kaldi-gmm.a \
          ../../../src/tree/kaldi-tree.a ../../../src/util/kaldi-util.a \
          ../../../src/matrix/kaldi-matrix.a \
          ../vvutil/kaldi-vvutil.a \
          ../../../src/base/kaldi-base.a 

include ../../../src/makefiles/default_rules.mk

