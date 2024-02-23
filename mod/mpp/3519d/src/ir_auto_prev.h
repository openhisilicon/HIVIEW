/*
  Copyright (c), 2001-2022, Shenshu Tech. Co., Ltd.
 */

#ifndef _HI_IR_AUTO_PREV_H_
#define _HI_IR_AUTO_PREV_H_

typedef struct {
    sample_vi_cfg vi_config;
    sample_vo_cfg vo_config;
    hi_vo_dev vo_dev;
    hi_vo_chn vo_chn;
    hi_vi_pipe vi_pipe;
    hi_vi_chn vi_chn;
} ir_auto_prev;

#endif
