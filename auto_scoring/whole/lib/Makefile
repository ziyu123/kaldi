
GXX = g++

LDFLAGS = -rdynamic -lm -ldl -lpthread

SOFLAGS = -shared -Wl,--no-undefined -Wl,--as-needed -Wl,-soname=$@



KALDI_PATH = /work7/tangzy/github/auto_scoring_20170726

#### build shared library from static library ####

#FST_LIBS = $(KALDI_PATH)/tools/openfst/lib/libfst.a
#
#MKL_LIBS = -Wl,--start-group /nfs/disk/perm/tools/intel/parallel_studio_xe_2013/composer_xe_2013_sp1.0.080/mkl/lib/intel64/libmkl_intel_lp64.a /nfs/disk/perm/tools/intel/parallel_studio_xe_2013/composer_xe_2013_sp1.0.080/mkl/lib/intel64/libmkl_core.a /nfs/disk/perm/tools/intel/parallel_studio_xe_2013/composer_xe_2013_sp1.0.080/mkl/lib/intel64/libmkl_sequential.a -Wl,--end-group
#
#KALDI_LIBS = $(KALDI_PATH)/src/nnet/kaldi-nnet.a \
#             $(KALDI_PATH)/src/decoder/kaldi-decoder.a \
#             $(KALDI_PATH)/src/hmm/kaldi-hmm.a \
#             $(KALDI_PATH)/src/feat/kaldi-feat.a \
#             $(KALDI_PATH)/src/transform/kaldi-transform.a \
#             $(KALDI_PATH)/src/tree/kaldi-tree.a \
#             $(KALDI_PATH)/src/lat/kaldi-lat.a \
#             $(KALDI_PATH)/src/cudamatrix/kaldi-cudamatrix.a \
#             $(KALDI_PATH)/src/matrix/kaldi-matrix.a \
#             $(KALDI_PATH)/src/util/kaldi-util.a \
#             $(KALDI_PATH)/src/base/kaldi-base.a



#### build shared library from shared library ####

FST_LIBS = -L$(KALDI_PATH)/tools/openfst/lib \
	   -Wl,-rpath=$(KALDI_PATH)/tools/openfst/lib \
	   -lfst

MKL_LIBS = -L/nfs/disk/perm/tools/intel/parallel_studio_xe_2013/composer_xe_2013_sp1.0.080/mkl/lib/intel64 \
	   -Wl,-rpath=/nfs/disk/perm/tools/intel/parallel_studio_xe_2013/composer_xe_2013_sp1.0.080/mkl/lib/intel64 \
	   -lmkl_intel_lp64 -lmkl_sequential -lmkl_core

KALDI_LIBS = -L$(KALDI_PATH)/src/lib \
             -Wl,-rpath=$(KALDI_PATH)/src/lib \
             -lkaldi-nnet -lkaldi-decoder -lkaldi-fstext -lkaldi-hmm \
	     -lkaldi-feat -lkaldi-transform -lkaldi-tree \
	     -lkaldi-lat -lkaldi-cudamatrix -lkaldi-matrix \
	     -lkaldi-util -lkaldi-base

LIBS = $(KALDI_LIBS) $(FST_LIBS) $(MKL_LIBS)



OBJFILES = mathlib.o stream-biglm-faster-decoder.o stream-feature-input.o utils.o \
           mlp.o stream-cmvn-input.o stream-hclg-decoder.o utl.o \
           relat_score.o stream-faster-decoder.o stream-nnet-decodable-score.o



all: libauto_score.so

libauto_score.so: $(OBJFILES)
	$(CXX) $(LDFLAGS) $(SOFLAGS) -o $@ $^ $(LIBS)
	
clean:
	-rm  libauto_score.so $(OBJFILES)
