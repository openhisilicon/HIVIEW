/*
  Copyright (c), 2001-2024, Shenshu Tech. Co., Ltd.
 */

#ifndef OT_COMMON_SNAP_H
#define OT_COMMON_SNAP_H

#include "ot_common.h"
#include "ot_common_video.h"
#include "ot_common_isp.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define OT_ERR_SNAP_INVALID_PIPE_ID OT_DEFINE_ERR(OT_ID_SNAP, OT_ERR_LEVEL_ERROR, OT_ERR_INVALID_PIPE_ID)
#define OT_ERR_SNAP_ILLEGAL_PARAM   OT_DEFINE_ERR(OT_ID_SNAP, OT_ERR_LEVEL_ERROR, OT_ERR_ILLEGAL_PARAM)
#define OT_ERR_SNAP_NULL_PTR        OT_DEFINE_ERR(OT_ID_SNAP, OT_ERR_LEVEL_ERROR, OT_ERR_NULL_PTR)
#define OT_ERR_SNAP_NOT_SUPPORT     OT_DEFINE_ERR(OT_ID_SNAP, OT_ERR_LEVEL_ERROR, OT_ERR_NOT_SUPPORT)
#define OT_ERR_SNAP_NOT_PERM        OT_DEFINE_ERR(OT_ID_SNAP, OT_ERR_LEVEL_ERROR, OT_ERR_NOT_PERM)
#define OT_ERR_SNAP_NO_MEM          OT_DEFINE_ERR(OT_ID_SNAP, OT_ERR_LEVEL_ERROR, OT_ERR_NO_MEM)
#define OT_ERR_SNAP_NOT_READY       OT_DEFINE_ERR(OT_ID_SNAP, OT_ERR_LEVEL_ERROR, OT_ERR_NOT_READY)

typedef enum {
    OT_SNAP_TYPE_NORM, /* ZSL and none-ZSL type */
    OT_SNAP_TYPE_PRO,  /* Professional type support HDR, AEB, LongExposure */
    OT_SNAP_TYPE_BUTT
} ot_snap_type;

typedef struct {
    td_u32  frame_cnt;         /* RW; Set capture frame counts */
    td_u32  repeat_send_times; /*
                                * RW; Range: [0, 3];
                                * When FE-BE is offline, the first raw frame that is triggered
                                * should be sent repeatedly to the BE.
                                */

    td_bool zsl_en;            /* RW; statical attrs */
    td_u32  frame_depth;       /* RW; buffer depth: [1,8] */
    td_u32  rollback_ms;       /* RW; Rollback time, unit(ms), invalid when zsl_en is TD_FALSE  */
    td_u32  interval;          /* RW; For continuous capture, select frame every intercal frames */
} ot_snap_norm_attr;

typedef struct {
    td_u32 exp_time[OT_ISP_PRO_MAX_FRAME_NUM]; /*
                                                * RW; Range: [0x0, 0xFFFFFFFF], sensor exposure time (unit: us),
                                                * it's related to the specific sensor.
                                                */

    td_u32 sys_gain[OT_ISP_PRO_MAX_FRAME_NUM]; /*
                                                * RW; Range: [0x400, 0xFFFFFFFF],
                                                * system gain (unit: times, 10bit precision),
                                                * it's related to the specific sensor and ISP d_gain range.
                                                */
} ot_snap_pro_manual_param;

typedef struct {
    td_u16 exp_step[OT_ISP_PRO_MAX_FRAME_NUM]; /*
                                                    * RW; Range:[0x0, 0xFFFF],
                                                    * Professional exposure step (unit: times, 8bit precision).
                                                    */
} ot_snap_pro_auto_param;

typedef struct {
    ot_op_mode               op_mode;
    ot_snap_pro_auto_param   auto_param;
    ot_snap_pro_manual_param manual_param;
} ot_snap_pro_param;

typedef struct {
    td_u32  frame_cnt;         /* RW; Range: [1, OT_ISP_PRO_MAX_FRAME_NUM] */
    td_u32  repeat_send_times; /*
                                * RW; Range: [0, 3];
                                * When FE-BE is offline, the first raw frame that is triggered
                                * should be sent repeatedly to the BE.
                                */

    ot_snap_pro_param pro_param;
} ot_snap_pro_attr;

typedef struct {
    ot_snap_type snap_type;    /* snap type */
    td_bool      load_ccm_en;  /* TD_TRUE:use CCM of snap_isp_info, TD_FALSE: Algorithm calculate */
    union {
        ot_snap_norm_attr norm_attr; /* AUTO: ot_snap_type:OT_SNAP_TYPE_NORM. */
        ot_snap_pro_attr  pro_attr;  /* AUTO: ot_snap_type:OT_SNAP_TYPE_PRO. */
    };
} ot_snap_attr;

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* OT_COMMON_SNAP_H */
