
all:
EXTRA_CXXFLAGS = -Wno-sign-compare
include ../../../src/kaldi.mk

CXXFLAGS += -I../../../src

BINFILES = pl-score-referred-text-wav


OBJFILES =

ADDLIBS = ../../../src/decoder/kaldi-decoder.a ../../../src/lat/kaldi-lat.a ../../../src/lm/kaldi-lm.a \
          ../../../src/fstext/kaldi-fstext.a ../../../src/hmm/kaldi-hmm.a \
          ../../../src/transform/kaldi-transform.a ../../../src/gmm/kaldi-gmm.a \
          ../../../src/tree/kaldi-tree.a ../../../src/util/kaldi-util.a \
          ../../../src/matrix/kaldi-matrix.a \
          ../vvutil/kaldi-vvutil.a \
          ../../../src/base/kaldi-base.a


TESTFILES =

include ../../../src/makefiles/default_rules.mk
