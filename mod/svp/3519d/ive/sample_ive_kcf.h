//#include "sample_ive_main.h"

#include "hi_buffer.h"
#include "hi_common.h"
#include "hi_common_sys.h"
#include "hi_common_svp.h"
#include "hi_common_vpss.h"
#include "sample_comm.h"
#include "sample_common_svp.h"
#include "sample_common_svp_npu.h"
#include "sample_common_svp_npu_model.h"
#include "sample_common_ive.h"
#include "sample_ive_queue.h"
#include "svp_acl_ext.h"


#define HI_SAMPLE_IVE_KCF_SVP_NPU_MAX_ROI_NUM_OF_CLASS     50
#define HI_SAMPLE_IVE_KCF_SVP_NPU_MAX_CLASS_NUM            50
#define HI_SAMPLE_IVE_KCF_SVP_NPU_MIN_CLASS_NUM            1

typedef struct {
    hi_u32 class_num;
    hi_u32 total_num;
    hi_u32 roi_num_array[HI_SAMPLE_IVE_KCF_SVP_NPU_MAX_CLASS_NUM];
    hi_sample_svp_rect rect[HI_SAMPLE_IVE_KCF_SVP_NPU_MAX_CLASS_NUM][HI_SAMPLE_IVE_KCF_SVP_NPU_MAX_ROI_NUM_OF_CLASS];
} hi_sample_svp_ive_rect_arr;

hi_s32 sample_ive_kcf_init2(void);
hi_void sample_ive_kcf_deinit2(void);

hi_sample_svp_rect_info *sample_ive_kcf_tracking2(const hi_video_frame_info *frame, hi_sample_svp_ive_rect_arr *rect_arr);

