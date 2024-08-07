/*
  Copyright (c), 2001-2022, Shenshu Tech. Co., Ltd.
 */

#ifndef SAMPLE_AIISP_COMMON_H
#define SAMPLE_AIISP_COMMON_H

#include <signal.h>
#include "sample_comm.h"
#include "hi_sns_ctrl.h"
#include "hi_common_aiisp.h"
#include "hi_common_aibnr.h"
#include "hi_mpi_aibnr.h"
#include "hi_mpi_ae.h"

/* exps is true, return ret */
#define sample_aiisp_check_exps_return(exps) \
do { \
    if ((exps)) { \
        printf("realpath file failed\n"); \
        return HI_FAILURE; \
    } \
} while (0) \

hi_void sample_aiisp_handle_sig(hi_s32 signo);
hi_void sample_get_char(hi_char *s);
sample_sns_type sample_aiisp_get_wdr_sns_type(sample_sns_type sns_type);
hi_s32 sample_aiisp_load_mem(hi_aiisp_mem_info *mem, hi_char *model_file);
hi_void sample_aiisp_unload_mem(hi_aiisp_mem_info *param_mem);
hi_s32 sample_aiisp_sys_init(hi_size *in_size, hi_u32 vb_cnt);
hi_s32 sample_aiisp_start_vpss(hi_vpss_grp grp, hi_size *in_size);
hi_s32 sample_aiisp_start_vo(sample_vo_mode vo_mode);
hi_void sample_aiisp_stop_vo(hi_void);
hi_s32 sample_aiisp_start_venc(hi_venc_chn venc_chn[], hi_u32 venc_chn_len, hi_u32 chn_num, hi_size *size);
hi_s32 sample_aiisp_start_venc_and_vo(hi_vpss_grp vpss_grp[], hi_u32 grp_num, hi_size *in_size);
hi_void sample_aiisp_stop_vpss(hi_vpss_grp grp);
hi_void sample_aiisp_stop_venc(hi_venc_chn venc_chn[], hi_u32 venc_chn_len, hi_u32 chn_num);
hi_void sample_aiisp_stop_venc_and_vo(hi_vpss_grp vpss_grp[], hi_u32 grp_num);
hi_s32 sample_aiisp_set_long_frame_mode(hi_vi_pipe vi_pipe, hi_bool is_wdr_mode);
hi_void sample_aiisp_get_default_cfg(sample_sns_type sns_type, hi_vi_pipe vi_pipe,
    hi_size *size, sample_vi_cfg *vi_cfg);
hi_void sample_aiisp_sensor_cfg(hi_vi_pipe vi_pipe, sample_sns_type sns_type);

#endif
