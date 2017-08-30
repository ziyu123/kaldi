
all:
EXTRA_CXXFLAGS = -Wno-sign-compare
include ../../../src/kaldi.mk

CXXFLAGS += -I../../../src
LDFLAGS += $(CUDA_LDFLAGS)
LDLIBS += $(CUDA_LDLIBS)

BINFILES = nnet3-align-compiled ll-compare-with-referred-text-wav

OBJFILES =

# Add this dependency to force cuda-compiled.o to be rebuilt when we reconfigure.
cuda-compiled.o: ../../../src/kaldi.mk

TESTFILES =

ADDLIBS = ../../../src/nnet3/kaldi-nnet3.a ../../../src/chain/kaldi-chain.a \
          ../../../src/cudamatrix/kaldi-cudamatrix.a ../../../src/decoder/kaldi-decoder.a \
          ../../../src/lat/kaldi-lat.a ../../../src/fstext/kaldi-fstext.a ../../../src/hmm/kaldi-hmm.a \
          ../../../src/transform/kaldi-transform.a ../../../src/gmm/kaldi-gmm.a \
          ../../../src/tree/kaldi-tree.a ../../../src/util/kaldi-util.a \
          ../../../src/matrix/kaldi-matrix.a ../../../src/base/kaldi-base.a

include ../../../src/makefiles/default_rules.mk
