/*
  Copyright (c), 2001-2024, Shenshu Tech. Co., Ltd.
 */

#ifndef OT_SAMPLE_DIS_H
#define OT_SAMPLE_DIS_H

#include "hi_type.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

hi_bool sample_dis_get_save_stream(void);
hi_void sample_dis_set_save_stream(hi_bool save_stream);
hi_bool sample_dis_get_send_data_statue(hi_void);
hi_void sample_dis_set_send_data_statue(hi_bool send_data);
hi_void sample_dis_set_venc_chn_size(hi_size img_size);
hi_void sample_dis_pause(hi_char *s);
hi_s32 sample_dis_start_sample(sample_vi_cfg *vi_cfg, sample_vo_cfg *vo_cfg, hi_size *img_size);
hi_s32 sample_dis_stop_sample(sample_vi_cfg *vi_cfg, sample_vo_cfg *vo_cfg);
hi_void sample_dis_stop_sample_without_sys_exit(sample_vi_cfg *vi_cfg, sample_vo_cfg *vo_cfg);

#ifdef __cplusplus
}
#endif /* End of __cplusplus */

#endif /* OT_SAMPLE_DIS_H */