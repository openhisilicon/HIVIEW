/*
 * Copyright (c) Hisilicon Technologies Co., Ltd. 2012-2019. All rights reserved.
 * Description: Function of hi_sns_ctrl_adapt.h
 * Author: ISP SW
 * Create: 2012/06/28
 */

#ifndef __HI_SNS_CTRL_ADAPT_H__
#define __HI_SNS_CTRL_ADAPT_H__

#include "hi_type.h"
#include "hi_comm_3a_adapt.h"
#include "hi_sns_ctrl.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* End of #ifdef __cplusplus */

typedef struct {
    hi_bool      init;                   /* HI_TRUE: Sensor init */
    hi_bool      sync_init;              /* HI_TRUE: Sync Reg init */
    hi_u8        img_mode;
    hi_u8        hdr;                 /* HI_TRUE: HDR enbale */
    hi_wdr_mode  wdr_mode;

    hi_isp_sns_regs_info regs_info[ISP_SNS_SAVE_INFO_MAX]; /* [0]: Sensor reg info of cur-frame;
                                                              [1]: Sensor reg info of pre-frame ; */

    hi_u32      fl[ISP_SNS_SAVE_INFO_MAX];                /* [0]: FullLines of cur-frame;
                                                             [1]: Pre FullLines of pre-frame */
    hi_u32      fl_std;                    /* FullLines std */
    hi_u32      wdr_int_time[WDR_MAX_FRAME_NUM];
    hi_u32      sensor_wb_gain[ISP_BAYER_CHN_NUM];
} hi_isp_sns_state;

typedef ISP_SNS_MIRRORFLIP_TYPE_E hi_isp_sns_mirrorflip_type;

typedef struct {
    hi_s32  (*pfn_register_callback)(hi_vi_pipe vi_pipe, hi_isp_alg_lib *ae_lib, hi_isp_alg_lib *awb_lib);
    hi_s32  (*pfn_un_register_callback)(hi_vi_pipe vi_pipe, hi_isp_alg_lib *ae_lib, hi_isp_alg_lib *awb_lib);
    hi_s32  (*pfn_set_bus_info)(hi_vi_pipe vi_pipe, hi_isp_sns_commbus sns_bus_info);
    hi_void (*pfn_standby)(hi_vi_pipe vi_pipe);
    hi_void (*pfn_restart)(hi_vi_pipe vi_pipe);
    hi_void (*pfn_mirror_flip)(hi_vi_pipe vi_pipe, hi_isp_sns_mirrorflip_type sns_mirror_flip);
    hi_s32  (*pfn_write_reg)(hi_vi_pipe vi_pipe, hi_s32 addr, hi_s32 data);
    hi_s32  (*pfn_read_reg)(hi_vi_pipe vi_pipe, hi_s32 addr);
    hi_s32  (*pfn_set_init)(hi_vi_pipe vi_pipe, hi_isp_init_attr *init_attr);
} hi_isp_sns_obj;

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */

#endif /* __HI_SNS_CTRL_ADAPT_H__ */
