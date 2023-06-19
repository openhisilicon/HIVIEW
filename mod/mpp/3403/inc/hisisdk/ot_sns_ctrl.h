/*
  Copyright (c), 2001-2022, Shenshu Tech. Co., Ltd.
 */

#ifndef OT_SNS_CTRL_H
#define OT_SNS_CTRL_H

#include "ot_type.h"
#include "ot_common_3a.h"
#include "ot_debug.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* End of #ifdef __cplusplus */

#define ISP_SNS_SAVE_INFO_MAX 2
typedef struct {
    td_bool      init;                   /* TD_TRUE: Sensor init */
    td_bool      sync_init;              /* TD_TRUE: Sync Reg init */
    td_u8        img_mode;
    td_u8        hdr;                 /* TD_TRUE: HDR enable */
    ot_wdr_mode  wdr_mode;

    ot_isp_sns_regs_info regs_info[ISP_SNS_SAVE_INFO_MAX]; /* [0]: Sensor reg info of cur-frame;
                                                              [1]: Sensor reg info of pre-frame ; */

    td_u32      fl[ISP_SNS_SAVE_INFO_MAX];                /* [0]: FullLines of cur-frame;
                                                             [1]: Pre FullLines of pre-frame */
    td_u32      fl_std;                    /* FullLines std */
    td_u32      wdr_int_time[OT_ISP_WDR_MAX_FRAME_NUM];
    td_u32      sensor_wb_gain[OT_ISP_BAYER_CHN_NUM];
} ot_isp_sns_state;

typedef enum {
    ISP_SNS_NORMAL      = 0,
    ISP_SNS_MIRROR      = 1,
    ISP_SNS_FLIP        = 2,
    ISP_SNS_MIRROR_FLIP = 3,
    ISP_SNS_BUTT
} ot_isp_sns_mirrorflip_type;

typedef struct {
    char bus_addr;
} ot_isp_sns_bus_ex;

typedef struct {
    td_bool blc_clamp_en; /* TD_TRUE: sensor black level correction enable */
} ot_isp_sns_blc_clamp;

typedef struct {
    td_s32  (*pfn_register_callback)(ot_vi_pipe vi_pipe, ot_isp_3a_alg_lib *ae_lib, ot_isp_3a_alg_lib *awb_lib);
    td_s32  (*pfn_un_register_callback)(ot_vi_pipe vi_pipe, ot_isp_3a_alg_lib *ae_lib, ot_isp_3a_alg_lib *awb_lib);
    td_s32  (*pfn_set_bus_info)(ot_vi_pipe vi_pipe, ot_isp_sns_commbus sns_bus_info);
    td_s32  (*pfn_set_bus_ex_info)(ot_vi_pipe vi_pipe, ot_isp_sns_bus_ex *serdes_info);
    td_void (*pfn_standby)(ot_vi_pipe vi_pipe);
    td_void (*pfn_restart)(ot_vi_pipe vi_pipe);
    td_void (*pfn_mirror_flip)(ot_vi_pipe vi_pipe, ot_isp_sns_mirrorflip_type sns_mirror_flip);
    td_void (*pfn_set_blc_clamp)(ot_vi_pipe vi_pipe, ot_isp_sns_blc_clamp sns_blc_clamp);
    td_s32  (*pfn_write_reg)(ot_vi_pipe vi_pipe, td_u32 addr, td_u32 data);
    td_s32  (*pfn_read_reg)(ot_vi_pipe vi_pipe, td_u32 addr);
    td_s32  (*pfn_set_init)(ot_vi_pipe vi_pipe, ot_isp_init_attr *init_attr);
} ot_isp_sns_obj;

extern ot_isp_sns_obj g_sns_imx327_obj;
extern ot_isp_sns_obj g_sns_os08a20_obj;
extern ot_isp_sns_obj g_sns_os08a20_slave_obj;
extern ot_isp_sns_obj g_sns_imx485_obj;
extern ot_isp_sns_obj g_sns_imx347_slave_obj;
extern ot_isp_sns_obj g_sns_os05a10_2l_slave_obj;
extern ot_isp_sns_obj g_sns_imx334_obj;
extern ot_isp_sns_obj g_sns_os04a10_obj;
extern ot_isp_sns_obj g_sns_os08b10_obj;

#define sensor_check_pointer_return(ptr) \
    do { \
        if ((ptr) == TD_NULL) { \
            isp_err_trace("Null Pointer!\n"); \
            return OT_ERR_ISP_NULL_PTR; \
        } \
    } while (0)

#define sensor_check_pointer_void_return(ptr) \
    do { \
        if ((ptr) == TD_NULL) { \
            isp_err_trace("Null Pointer!\n"); \
            return; \
        } \
    } while (0)

#define sensor_free(ptr) \
    do { \
        if ((ptr) != TD_NULL) { \
            free(ptr); \
            (ptr) = TD_NULL; \
        } \
    } while (0)

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */

#endif /* OT_SNS_CTRL_H */
