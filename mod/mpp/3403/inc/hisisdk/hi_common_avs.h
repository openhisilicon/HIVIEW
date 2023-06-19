/*
  Copyright (c), 2001-2022, Shenshu Tech. Co., Ltd.
 */

#ifndef __HI_COMMON_AVS_H__
#define __HI_COMMON_AVS_H__

#include "hi_type.h"
#include "hi_common.h"
#include "hi_common_video.h"
#include "ot_common_avs.h"


#ifdef __cplusplus
extern "C" {
#endif

#define HI_ERR_AVS_NULL_PTR OT_ERR_AVS_NULL_PTR
#define HI_ERR_AVS_NOT_READY OT_ERR_AVS_NOT_READY
#define HI_ERR_AVS_INVALID_DEVID OT_ERR_AVS_INVALID_DEVID
#define HI_ERR_AVS_INVALID_PIPEID OT_ERR_AVS_INVALID_PIPEID
#define HI_ERR_AVS_INVALID_CHNID OT_ERR_AVS_INVALID_CHNID
#define HI_ERR_AVS_EXIST OT_ERR_AVS_EXIST
#define HI_ERR_AVS_UNEXIST OT_ERR_AVS_UNEXIST
#define HI_ERR_AVS_NOT_SUPPORT OT_ERR_AVS_NOT_SUPPORT
#define HI_ERR_AVS_NOT_PERM OT_ERR_AVS_NOT_PERM
#define HI_ERR_AVS_NO_MEM OT_ERR_AVS_NO_MEM
#define HI_ERR_AVS_NO_BUF OT_ERR_AVS_NO_BUF
#define HI_ERR_AVS_ILLEGAL_PARAM OT_ERR_AVS_ILLEGAL_PARAM
#define HI_ERR_AVS_BUSY OT_ERR_AVS_BUSY
#define HI_ERR_AVS_BUF_EMPTY OT_ERR_AVS_BUF_EMPTY
#define HI_AVS_LUT_ACCURACY_HIGH OT_AVS_LUT_ACCURACY_HIGH
#define HI_AVS_LUT_ACCURACY_LOW OT_AVS_LUT_ACCURACY_LOW
#define HI_AVS_LUT_ACCURACY_BUTT OT_AVS_LUT_ACCURACY_BUTT
typedef ot_avs_lut_accuracy hi_avs_lut_accuracy;
typedef ot_avs_lut hi_avs_lut;
#define HI_AVS_PROJECTION_EQUIRECTANGULAR OT_AVS_PROJECTION_EQUIRECTANGULAR
#define HI_AVS_PROJECTION_RECTILINEAR OT_AVS_PROJECTION_RECTILINEAR
#define HI_AVS_PROJECTION_CYLINDRICAL OT_AVS_PROJECTION_CYLINDRICAL
#define HI_AVS_PROJECTION_CUBE_MAP OT_AVS_PROJECTION_CUBE_MAP
#define HI_AVS_PROJECTION_BUTT OT_AVS_PROJECTION_BUTT
typedef ot_avs_projection_mode hi_avs_projection_mode;
#define HI_AVS_GAIN_MODE_MANUAL OT_AVS_GAIN_MODE_MANUAL
#define HI_AVS_GAIN_MODE_AUTO OT_AVS_GAIN_MODE_AUTO
#define HI_AVS_GAIN_MODE_BUTT OT_AVS_GAIN_MODE_BUTT
typedef ot_avs_gain_mode hi_avs_gain_mode;
#define HI_AVS_MODE_BLEND OT_AVS_MODE_BLEND
#define HI_AVS_MODE_NOBLEND_VER OT_AVS_MODE_NOBLEND_VER
#define HI_AVS_MODE_NOBLEND_HOR OT_AVS_MODE_NOBLEND_HOR
#define HI_AVS_MODE_NOBLEND_QR OT_AVS_MODE_NOBLEND_QR
#define HI_AVS_MODE_BUTT OT_AVS_MODE_BUTT
typedef ot_avs_mode hi_avs_mode;
typedef ot_avs_gain_attr hi_avs_gain_attr;
typedef ot_avs_rotation hi_avs_rotation;
typedef ot_avs_fov hi_avs_fov;
typedef ot_avs_split_attr hi_avs_split_attr;
typedef ot_avs_cube_map_attr hi_avs_cube_map_attr;
typedef ot_avs_out_attr hi_avs_out_attr;
typedef ot_avs_grp_attr hi_avs_grp_attr;
typedef ot_avs_chn_attr hi_avs_chn_attr;
typedef ot_avs_mod_param hi_avs_mod_param;

#ifdef __cplusplus
}
#endif
#endif /* __HI_COMMON_AVS_H__ */
