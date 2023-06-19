/*
  Copyright (c), 2001-2022, Shenshu Tech. Co., Ltd.
 */

#ifndef __HI_COMMON_SYS_H__
#define __HI_COMMON_SYS_H__

#include "hi_common_video.h"
#include "hi_debug.h"
#include "hi_errno.h"
#include "ot_common_sys.h"


#ifdef __cplusplus
extern "C" {
#endif

#define HI_GPS_EXPRESS_NUM OT_GPS_EXPRESS_NUM
#define HI_GPS_GRP_NUM OT_GPS_GRP_NUM
#define HI_UNIQUE_ID_NUM OT_UNIQUE_ID_NUM
#define HI_ERR_SYS_NULL_PTR OT_ERR_SYS_NULL_PTR
#define HI_ERR_SYS_NOT_READY OT_ERR_SYS_NOT_READY
#define HI_ERR_SYS_NOT_PERM OT_ERR_SYS_NOT_PERM
#define HI_ERR_SYS_NO_MEM OT_ERR_SYS_NO_MEM
#define HI_ERR_SYS_ILLEGAL_PARAM OT_ERR_SYS_ILLEGAL_PARAM
#define HI_ERR_SYS_BUSY OT_ERR_SYS_BUSY
#define HI_ERR_SYS_NOT_SUPPORT OT_ERR_SYS_NOT_SUPPORT
#define HI_SCALE_RANGE_0 OT_SCALE_RANGE_0
#define HI_SCALE_RANGE_1 OT_SCALE_RANGE_1
#define HI_SCALE_RANGE_2 OT_SCALE_RANGE_2
#define HI_SCALE_RANGE_3 OT_SCALE_RANGE_3
#define HI_SCALE_RANGE_4 OT_SCALE_RANGE_4
#define HI_SCALE_RANGE_5 OT_SCALE_RANGE_5
#define HI_SCALE_RANGE_6 OT_SCALE_RANGE_6
#define HI_SCALE_RANGE_7 OT_SCALE_RANGE_7
#define HI_SCALE_RANGE_8 OT_SCALE_RANGE_8
#define HI_SCALE_RANGE_9 OT_SCALE_RANGE_9
#define HI_SCALE_RANGE_10 OT_SCALE_RANGE_10
#define HI_SCALE_RANGE_11 OT_SCALE_RANGE_11
#define HI_SCALE_RANGE_12 OT_SCALE_RANGE_12
#define HI_SCALE_RANGE_13 OT_SCALE_RANGE_13
#define HI_SCALE_RANGE_14 OT_SCALE_RANGE_14
#define HI_SCALE_RANGE_15 OT_SCALE_RANGE_15
#define HI_SCALE_RANGE_16 OT_SCALE_RANGE_16
#define HI_SCALE_RANGE_17 OT_SCALE_RANGE_17
#define HI_SCALE_RANGE_18 OT_SCALE_RANGE_18
#define HI_SCALE_RANGE_BUTT OT_SCALE_RANGE_BUTT
typedef ot_scale_range_type hi_scale_range_type;
#define HI_COEF_LEVEL_0 OT_COEF_LEVEL_0
#define HI_COEF_LEVEL_1 OT_COEF_LEVEL_1
#define HI_COEF_LEVEL_2 OT_COEF_LEVEL_2
#define HI_COEF_LEVEL_3 OT_COEF_LEVEL_3
#define HI_COEF_LEVEL_4 OT_COEF_LEVEL_4
#define HI_COEF_LEVEL_5 OT_COEF_LEVEL_5
#define HI_COEF_LEVEL_6 OT_COEF_LEVEL_6
#define HI_COEF_LEVEL_7 OT_COEF_LEVEL_7
#define HI_COEF_LEVEL_8 OT_COEF_LEVEL_8
#define HI_COEF_LEVEL_9 OT_COEF_LEVEL_9
#define HI_COEF_LEVEL_10 OT_COEF_LEVEL_10
#define HI_COEF_LEVEL_11 OT_COEF_LEVEL_11
#define HI_COEF_LEVEL_12 OT_COEF_LEVEL_12
#define HI_COEF_LEVEL_13 OT_COEF_LEVEL_13
#define HI_COEF_LEVEL_14 OT_COEF_LEVEL_14
#define HI_COEF_LEVEL_15 OT_COEF_LEVEL_15
#define HI_COEF_LEVEL_16 OT_COEF_LEVEL_16
#define HI_COEF_LEVEL_17 OT_COEF_LEVEL_17
#define HI_COEF_LEVEL_18 OT_COEF_LEVEL_18
#define HI_COEF_LEVEL_BUTT OT_COEF_LEVEL_BUTT
typedef ot_coef_level hi_coef_level;
typedef ot_mpp_sys_cfg hi_mpp_sys_cfg;
typedef ot_sys_virt_mem_info hi_sys_virt_mem_info;
typedef ot_mpp_bind_dst hi_mpp_bind_dst;
typedef ot_scale_coef_level hi_scale_coef_level;
typedef ot_scale_range hi_scale_range;
typedef ot_scale_coef_info hi_scale_coef_info;
typedef ot_unique_id hi_unique_id;
typedef ot_gps_info hi_gps_info;
#define HI_VI_OFFLINE_VPSS_OFFLINE OT_VI_OFFLINE_VPSS_OFFLINE
#define HI_VI_OFFLINE_VPSS_ONLINE OT_VI_OFFLINE_VPSS_ONLINE
#define HI_VI_ONLINE_VPSS_OFFLINE OT_VI_ONLINE_VPSS_OFFLINE
#define HI_VI_ONLINE_VPSS_ONLINE OT_VI_ONLINE_VPSS_ONLINE
#define HI_VI_PARALLEL_VPSS_OFFLINE OT_VI_PARALLEL_VPSS_OFFLINE
#define HI_VI_PARALLEL_VPSS_PARALLEL OT_VI_PARALLEL_VPSS_PARALLEL
#define HI_VI_VPSS_MODE_BUTT OT_VI_VPSS_MODE_BUTT
typedef ot_vi_vpss_mode_type hi_vi_vpss_mode_type;
typedef ot_vi_vpss_mode hi_vi_vpss_mode;
#define HI_VI_VIDEO_MODE_NORM OT_VI_VIDEO_MODE_NORM
#define HI_VI_VIDEO_MODE_ADVANCED OT_VI_VIDEO_MODE_ADVANCED
#define HI_VI_VIDEO_MODE_BUTT OT_VI_VIDEO_MODE_BUTT
typedef ot_vi_video_mode hi_vi_video_mode;
#define HI_EXT_CHN_SRC_TYPE_TAIL OT_EXT_CHN_SRC_TYPE_TAIL
#define HI_EXT_CHN_SRC_TYPE_BEFORE_FISHEYE OT_EXT_CHN_SRC_TYPE_BEFORE_FISHEYE
#define HI_EXT_CHN_SRC_TYPE_HEAD OT_EXT_CHN_SRC_TYPE_HEAD
#define HI_EXT_CHN_SRC_TYPE_BUTT OT_EXT_CHN_SRC_TYPE_BUTT
typedef ot_ext_chn_src_type hi_ext_chn_src_type;
typedef ot_raw_frame_compress_param hi_raw_frame_compress_param;

#ifdef __cplusplus
}
#endif
#endif /* __HI_COMMON_SYS_H__ */
