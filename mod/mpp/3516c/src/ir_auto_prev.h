/*
  Copyright (c), 2001-2024, Shenshu Tech. Co., Ltd.
 */

#ifndef _HI_IR_AUTO_PREV_H_
#define _HI_IR_AUTO_PREV_H_

typedef struct {
    sample_vi_cfg vi_config;
    hi_vi_pipe vi_pipe;
    hi_vi_chn vi_chn;
    hi_vpss_grp vpss_grp;
    hi_venc_chn venc_chn;
    sample_comm_venc_chn_param venc_param;
} ir_auto_prev;

#endif
