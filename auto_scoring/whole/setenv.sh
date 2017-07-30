#!/bin/bash
export PKG_NAME=pachira.score

export WORK_DIR=`pwd`
export KALDI_DIR_NAME=kaldi-20140727
export ATLAS_PATH=/usr/lib64/atlas
export MKL_PATH=/nfs/disk/perm/tools/intel/parallel_studio_xe_2013/composer_xe_2013_sp1.0.080/mkl


export CC_=gcc
export CXX_=g++
export AR_=ar
export RANLIB_=ranlib
export STRIP_=strip
export LD_=ld

#export CXXFLAGS_="-msse -msse2 -Wall -fPIC -DKALDI_DOUBLEPRECISION=0 -DHAVE_POSIX_MEMALIGN -Wno-sign-compare -Wno-unused-local-typedefs -Winit-self -DHAVE_EXECINFO_H=1 -rdynamic -DHAVE_CXXABI_H -DHAVE_ATLAS -fpermissive -w -std=c++0x -O3 -g"
export CXXFLAGS_="-msse -msse2 -Wall -fPIC -DKALDI_DOUBLEPRECISION=0 -DHAVE_POSIX_MEMALIGN -Wno-sign-compare -Wno-unused-local-typedefs -Winit-self -DHAVE_EXECINFO_H=1 -rdynamic -DHAVE_CXXABI_H -DHAVE_MKL -fpermissive -w -std=c++0x -O3 -g"

#export PACHIRA_SO="-shared  -rdynamic -Wl,-rpath=/work7/tangzy/github/kaldi_master_20170604/tools/openfst/lib -lm -lpthread  -ldl"
export PACHIRA_SO="-shared  -rdynamic -Wl,-rpath=/work7/tangzy/auto_score/linux/kaldi-20140727/tools/openfst/lib -lm -lpthread  -ldl"
