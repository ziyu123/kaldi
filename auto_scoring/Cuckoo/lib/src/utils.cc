#include "base/kaldi-common.h"
#include "util/common-utils.h"
#include "fstext/fstext-lib.h"
#include "utils.h"

namespace kaldi {

namespace nnet1 {
BaseFloat ComputeConfidence(const TransitionModel &trans_model, 
                            const std::vector<int32> &alignment,
                            const Matrix<BaseFloat> &loglikes,
                            const Vector<BaseFloat> &priors, 
							int *frame_array,
                            std::vector<PhoneConfidence> *conf_outputs) {
    int n = alignment.size();
    int frame = 0;
    BaseFloat posts = 0.0;
    int last_phone = 0;
	int start = 0,end = 0;
    BaseFloat sub_posts = 0.0, sub_count = 0;
	
//	int hb_frame_array[EXP_LEN];
//	memset(hb_frame_array,0x00,EXP_LEN*sizeof(int));
    for (std::vector<int32>::const_iterator i = alignment.begin(); i < alignment.end(); ++i, ++frame) 
	{
        int32 phone = trans_model.TransitionIdToPhone(*i);
        int32 pdf = trans_model.TransitionIdToPdf(*i);
        BaseFloat like = loglikes(frame, pdf)/0.12, prior = priors(pdf), post = like - prior;
        if (phone != last_phone)
		{
            if (last_phone && conf_outputs) 
			{
                PhoneConfidence pc(last_phone, expf(sub_posts/sub_count),start,end);
                conf_outputs->push_back(pc);
            }

			sub_posts = 0.0;
			sub_count = 0;
			start = end;
		}
        sub_posts += post;
        ++sub_count;
        last_phone = phone;
		++end;
		
		int32 idx = 0;
		BaseFloat frm_conf = expf(post);
		//have debug it will be useless speech frame in frame_array
		while(frm_conf*10<=1)
		{
			frm_conf *= 10;
			++idx;
		}
	//	if(idx>=EXP_LEN) idx = EXP_LEN-1;
	//	frame_array[idx] ++;	
		//add test hubo
		{
			//if(phone_syms)
			{
			//	std::string phone_name = phone_syms->Find(phone);
				if(phone!=1 && phone!=2 && phone!=3 && phone!=4 && phone!=5)
				{
					if(idx>=EXP_LEN) idx = EXP_LEN-1;
					++frame_array[idx];
				}
			}
		}
		//end test
    }
	//add test hubo
	/*
	for(int hb=0;hb<EXP_LEN;hb++)
	{
		printf("%3d ",frame_array[hb]);
	}
	printf("\n");
	int total=0;
	for(int hb=0;hb<EXP_LEN;hb++)
	{
		total+=hb_frame_array[hb];
		printf("%3d ",hb_frame_array[hb]);
	}
	printf("\n");
	FILE *fp_h=NULL;
	fp_h=fopen("mlp_nonsil_fea","a");
	fprintf(fp_h,"%8.6f %8.6f %8.6f %8.6f %8.6f %8.6f %8.6f %8.6f %8.6f %8.6f\n",
			hb_frame_array[0]*1.0/total,
			hb_frame_array[1]*1.0/total,
			hb_frame_array[2]*1.0/total,
			hb_frame_array[3]*1.0/total,
			hb_frame_array[4]*1.0/total,
			hb_frame_array[5]*1.0/total,
			hb_frame_array[6]*1.0/total,
			hb_frame_array[7]*1.0/total,
			hb_frame_array[8]*1.0/total,
			hb_frame_array[9]*1.0/total,
			hb_frame_array[10]*1.0/total
			);
	fclose(fp_h);
	//end test
	*/
    if (last_phone && conf_outputs) 
	{
        PhoneConfidence pc(last_phone, expf(sub_posts/sub_count),start,end);
        conf_outputs->push_back(pc);
    }
/*no use
	int32 idx = EXP_LEN-2;
	while(idx>0)
	{//we don't use the array[0] it must be 1
		frame_array[idx] += frame_array[idx+1];
		--idx;
	}
  */
  	return 0.0f;
}

}
} //namespace
