#export KALDI_ROOT=`pwd`/../../..
export KALDI_ROOT=/work4/tzy/github/kaldi_master_20170206
[ -f $KALDI_ROOT/tools/env.sh ] && . $KALDI_ROOT/tools/env.sh
export PATH=$PWD/utils/:$KALDI_ROOT/tools/openfst/bin:$KALDI_ROOT/tools/sctk/bin:$PWD:$PATH:/nfs/disk/perm/tools/linux-x86_64-glibc212/bin:/nfs/disk/perm/tools/srilm/bin/i686-m64
[ ! -f $KALDI_ROOT/tools/config/common_path.sh ] && echo >&2 "The standard file $KALDI_ROOT/tools/config/common_path.sh is not present -> Exit!" && exit 1
. $KALDI_ROOT/tools/config/common_path.sh
export LC_ALL=C
