
GXX = g++

CXXFLAGS = -std=c++11 \
           -Wall -Wno-sign-compare -Wno-unused-local-typedefs -Wno-deprecated-declarations -Winit-self \
           -DKALDI_DOUBLEPRECISION=0 -DHAVE_EXECINFO_H=1 -DHAVE_CXXABI_H -DHAVE_MKL -DHAVE_CUDA \
           -m64 -msse -msse2 -pthread \
           -g -fPIC -c

LDFLAGS = -rdynamic -lm -ldl -lpthread



KALDI_PATH = /work7/tangzy/github/auto_scoring_20170726

INCLUDE = -I$(KALDI_PATH)/src \
	  -I$(KALDI_PATH)/tools/openfst/include \
	  -I/nfs/disk/perm/tools/intel/parallel_studio_xe_2013/composer_xe_2013_sp1.0.080/mkl/include

KALDI_LIBS = -L$(KALDI_PATH)/src/lib \
	     -Wl,-rpath=$(KALDI_PATH)/src/lib \
	     -lkaldi-feat -lkaldi-matrix -lkaldi-util -lkaldi-base

MKL_LIBS = -L/nfs/disk/perm/tools/intel/parallel_studio_xe_2013/composer_xe_2013_sp1.0.080/mkl/lib/intel64 \
           -Wl,-rpath=/nfs/disk/perm/tools/intel/parallel_studio_xe_2013/composer_xe_2013_sp1.0.080/mkl/lib/intel64 \
           -lmkl_intel_lp64 -lmkl_sequential -lmkl_core

LDLIBS = $(KALDI_LIBS) $(MKL_LIBS)



all: compute-peak-from-mfcc

compute-peak-from-mfcc: compute-peak-from-mfcc.o
	$(GXX) $(LDFLAGS) $(LDLIBS) -o $@ $^

compute-peak-from-mfcc.o: compute-peak-from-mfcc.cc
	$(GXX) $(CXXFLAGS) $(INCLUDE) -o $@ $^

clean:
	-rm compute-peak-from-mfcc compute-peak-from-mfcc.o



