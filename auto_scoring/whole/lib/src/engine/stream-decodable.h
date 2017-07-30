#ifndef __STREAM_STREAM_DECODABLE_H__
#define __STREAM_STREAM_DECODABLE_H__
#include "base/kaldi-types.h"
#include "itf/decodable-itf.h"
#include "matrix/kaldi-matrix.h"
#include "utils.h"

namespace kaldi {

class StreamDecodableInterface: public DecodableInterface {
public:
  virtual int32 SetData(const char *data, uint32 len, int chunk_no) = 0;
  virtual bool GetResult(const std::vector<int32> &phone,std::vector<nnet1::PhoneConfidence> &phone_confs
  		,int *frame_array,Matrix<BaseFloat>**features) = 0;
  virtual void Reset() = 0;
  virtual int LoadStat(const Matrix<double> &in) = 0;
  virtual int SaveStat(Matrix<double> &out) = 0;
  virtual inline BaseFloat LogLikelihood(int32 frame, int32 index) = 0;
  virtual bool IsLastFrame(int32 frame) const = 0;
  virtual int32 NumIndices() const = 0;
  virtual ~StreamDecodableInterface() {};
};

} // namespace kaldi

#endif
