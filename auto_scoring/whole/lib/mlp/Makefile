
GXX = g++

CXXFLAGS = -std=c++11 \
           -Wall -Wno-sign-compare -Wno-unused-local-typedefs -Wno-deprecated-declarations -Winit-self \
           -DKALDI_DOUBLEPRECISION=0 -DHAVE_EXECINFO_H=1 -DHAVE_CXXABI_H -DHAVE_MKL -DHAVE_CUDA \
           -m64 -msse -msse2 -pthread \
           -g -fPIC -c



INCLUDE = -I.

OBJFILES = mathlib.o  mlp.o  utl.o


all: $(OBJFILES)


%.o: %.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDE) -o $@ $^

clean:
	-rm $(OBJFILES)
