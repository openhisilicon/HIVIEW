/*
  Copyright (c), 2001-2022, Shenshu Tech. Co., Ltd.
 */

#ifndef __HI_COMMON_DIS_H__
#define __HI_COMMON_DIS_H__

#include "hi_common_video.h"
#include "ot_common_dis.h"


#ifdef __cplusplus
extern "C" {
#endif

#define HI_ERR_DIS_NO_BUF OT_ERR_DIS_NO_BUF
#define HI_ERR_DIS_BUF_EMPTY OT_ERR_DIS_BUF_EMPTY
#define HI_ERR_DIS_NULL_PTR OT_ERR_DIS_NULL_PTR
#define HI_ERR_DIS_ILLEGAL_PARAM OT_ERR_DIS_ILLEGAL_PARAM
#define HI_ERR_DIS_BUF_FULL OT_ERR_DIS_BUF_FULL
#define HI_ERR_DIS_SYS_NOT_READY OT_ERR_DIS_SYS_NOT_READY
#define HI_ERR_DIS_NOT_SUPPORT OT_ERR_DIS_NOT_SUPPORT
#define HI_ERR_DIS_NOT_PERM OT_ERR_DIS_NOT_PERM
#define HI_ERR_DIS_BUSY OT_ERR_DIS_BUSY
#define HI_ERR_DIS_INVALID_CHN_ID OT_ERR_DIS_INVALID_CHN_ID
#define HI_ERR_DIS_CHN_UNEXIST OT_ERR_DIS_CHN_UNEXIST
#define HI_DIS_MODE_4_DOF_GME OT_DIS_MODE_4_DOF_GME
#define HI_DIS_MODE_6_DOF_GME OT_DIS_MODE_6_DOF_GME
#define HI_DIS_MODE_GYRO OT_DIS_MODE_GYRO
#define HI_DIS_MODE_BUTT OT_DIS_MODE_BUTT
typedef ot_dis_mode hi_dis_mode;
#define HI_DIS_MOTION_LEVEL_LOW OT_DIS_MOTION_LEVEL_LOW
#define HI_DIS_MOTION_LEVEL_NORM OT_DIS_MOTION_LEVEL_NORM
#define HI_DIS_MOTION_LEVEL_HIGH OT_DIS_MOTION_LEVEL_HIGH
#define HI_DIS_MOTION_LEVEL_BUTT OT_DIS_MOTION_LEVEL_BUTT
typedef ot_dis_motion_level hi_dis_motion_level;
#define HI_DIS_PDT_TYPE_IPC OT_DIS_PDT_TYPE_IPC
#define HI_DIS_PDT_TYPE_DV OT_DIS_PDT_TYPE_DV
#define HI_DIS_PDT_TYPE_DRONE OT_DIS_PDT_TYPE_DRONE
#define HI_DIS_PDT_TYPE_BUTT OT_DIS_PDT_TYPE_BUTT
typedef ot_dis_pdt_type hi_dis_pdt_type;
typedef ot_dis_param hi_dis_param;
typedef ot_dis_attr hi_dis_attr;
typedef ot_dis_cfg hi_dis_cfg;

#ifdef __cplusplus
}
#endif
#endif /* __HI_COMMON_DIS_H__ */
