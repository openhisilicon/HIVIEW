/*
  Copyright (c), 2001-2022, Shenshu Tech. Co., Ltd.
 */

#ifndef HI_COMMON_VO_H
#define HI_COMMON_VO_H

#include "hi_type.h"
#include "hi_common.h"
#include "hi_errno.h"
#include "hi_common_video.h"
#include "hi_common_vo_dev.h"
#include "ot_common_vo.h"

#ifdef __cplusplus
extern "C" {
#endif

#define HI_VO_PARTITION_MODE_SINGLE OT_VO_PARTITION_MODE_SINGLE
#define HI_VO_PARTITION_MODE_MULTI OT_VO_PARTITION_MODE_MULTI
#define HI_VO_PARTITION_MODE_BUTT OT_VO_PARTITION_MODE_BUTT
typedef ot_vo_partition_mode hi_vo_partition_mode;
typedef ot_vo_video_layer_attr hi_vo_video_layer_attr;
typedef ot_vo_layer_hsharpen_param hi_vo_layer_hsharpen_param;
typedef ot_vo_layer_param hi_vo_layer_param;
typedef ot_vo_chn_attr hi_vo_chn_attr;
typedef ot_vo_chn_param hi_vo_chn_param;
#define HI_VO_ZOOM_IN_RECT OT_VO_ZOOM_IN_RECT
#define HI_VO_ZOOM_IN_RATIO OT_VO_ZOOM_IN_RATIO
#define HI_VO_ZOOM_IN_BUTT OT_VO_ZOOM_IN_BUTT
typedef ot_vo_zoom_in_type hi_vo_zoom_in_type;
typedef ot_vo_zoom_ratio hi_vo_zoom_ratio;
typedef ot_vo_zoom_attr hi_vo_zoom_attr;
typedef ot_vo_border hi_vo_border;
#define HI_VO_MIRROR_NONE OT_VO_MIRROR_NONE
#define HI_VO_MIRROR_HOR OT_VO_MIRROR_HOR
#define HI_VO_MIRROR_VER OT_VO_MIRROR_VER
#define HI_VO_MIRROR_BOTH OT_VO_MIRROR_BOTH
#define HI_VO_MIRROR_BUTT OT_VO_MIRROR_BUTT
typedef ot_vo_mirror_mode hi_vo_mirror_mode;
typedef ot_vo_chn_status hi_vo_chn_status;

#ifdef __cplusplus
}
#endif
#endif /* HI_COMMON_VO_H */
