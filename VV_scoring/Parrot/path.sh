export KALDI_ROOT=`pwd`/../..
export KALDI_ROOT=/work7/tangzy/github/VV_scoring_20170828
[ -f $KALDI_ROOT/tools/env.sh ] && . $KALDI_ROOT/tools/env.sh
export PATH=$PWD/utils/:$KALDI_ROOT/tools/openfst/bin:$PWD:$PATH
[ ! -f $KALDI_ROOT/tools/config/common_path.sh ] && echo >&2 "The standard file $KALDI_ROOT/tools/config/common_path.sh is not present -> Exit!" && exit 1
. $KALDI_ROOT/tools/config/common_path.sh
export LC_ALL=C

# add path of VV_scoring bins
export PATH=\
$PWD/../src/likelihoodbin:\
$PWD/../src/phonelenbin:\
$PWD/../src/pitchbin:\
$PATH
