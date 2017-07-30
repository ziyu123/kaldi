
GXX = g++

CXXFLAGS = -std=c++11 \
           -Wall -Wno-sign-compare -Wno-unused-local-typedefs -Wno-deprecated-declarations -Winit-self \
           -DKALDI_DOUBLEPRECISION=0 -DHAVE_EXECINFO_H=1 -DHAVE_CXXABI_H -DHAVE_MKL -DHAVE_CUDA \
           -m64 -msse -msse2 -pthread \
           -g -fPIC -c



KALDI_PATH = /work7/tangzy/github/auto_scoring_20170726

INCLUDE = -I. -I$(KALDI_PATH)/src \
          -I$(KALDI_PATH)/tools/openfst/include \
          -I/nfs/disk/perm/tools/intel/parallel_studio_xe_2013/composer_xe_2013_sp1.0.080/mkl/include \
	  -I/nfs/disk/work/users/zhaomy/cuda/cuda-8.0/include



OBJFILES = relat_score.o utils.o

all: $(OBJFILES)

%.o: %.cc
	$(GXX) $(CXXFLAGS) $(INCLUDE) -o $@ $<

#	arm-linux-androideabi-g++ $(CXXFLAGS) $(INCLUDE) -o $@ $^

clean:
	-rm $(OBJFILES)


