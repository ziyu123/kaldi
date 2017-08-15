
GXX = g++

CXXFLAGS = -std=c++11 \
           -Wall -Wno-sign-compare -Wno-unused-local-typedefs -Wno-deprecated-declarations -Winit-self \
           -DKALDI_DOUBLEPRECISION=0 -DHAVE_EXECINFO_H=1 -DHAVE_CXXABI_H -DHAVE_MKL -DHAVE_CUDA \
           -m64 -msse -msse2 -pthread \
           -g -fPIC -c

LDFLAGS = -rdynamic -lm -ldl -lpthread

INCLUDE = -I. -I..


all: auto-scoring

auto-scoring: score_stream.o
	$(GXX) $(LDFLAGS) -o $@ $^ -L. -lauto_score \
	-Wl,-rpath=/work7/tangzy/github/auto_scoring_20170726/auto_scoring/Cuckoo/lib

score_stream.o: score_stream.cc
	$(GXX) $(CXXFLAGS) $(INCLUDE) -o $@ $<
clean:
	-rm score_stream.o auto-scoring
