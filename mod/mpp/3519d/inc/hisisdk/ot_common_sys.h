/*
  Copyright (c), 2001-2024, Shenshu Tech. Co., Ltd.
 */

#ifndef OT_COMMON_SYS_H
#define OT_COMMON_SYS_H

#include "ot_common_video.h"
#include "ot_debug.h"
#include "ot_errno.h"

#ifdef __cplusplus
extern "C" {
#endif /* End of #ifdef __cplusplus */

#define OT_GPS_EXPRESS_NUM 3
#define OT_GPS_GRP_NUM 2
#define OT_UNIQUE_ID_NUM 6

#define OT_ERR_SYS_NULL_PTR      OT_DEFINE_ERR(OT_ID_SYS, OT_ERR_LEVEL_ERROR, OT_ERR_NULL_PTR)
#define OT_ERR_SYS_NOT_READY     OT_DEFINE_ERR(OT_ID_SYS, OT_ERR_LEVEL_ERROR, OT_ERR_NOT_READY)
#define OT_ERR_SYS_NOT_PERM      OT_DEFINE_ERR(OT_ID_SYS, OT_ERR_LEVEL_ERROR, OT_ERR_NOT_PERM)
#define OT_ERR_SYS_NO_MEM        OT_DEFINE_ERR(OT_ID_SYS, OT_ERR_LEVEL_ERROR, OT_ERR_NO_MEM)
#define OT_ERR_SYS_ILLEGAL_PARAM OT_DEFINE_ERR(OT_ID_SYS, OT_ERR_LEVEL_ERROR, OT_ERR_ILLEGAL_PARAM)
#define OT_ERR_SYS_BUSY          OT_DEFINE_ERR(OT_ID_SYS, OT_ERR_LEVEL_ERROR, OT_ERR_BUSY)
#define OT_ERR_SYS_NOT_SUPPORT   OT_DEFINE_ERR(OT_ID_SYS, OT_ERR_LEVEL_ERROR, OT_ERR_NOT_SUPPORT)

typedef enum  {
    OT_SCALE_RANGE_0 = 0, /* scale range <   8/64 */
    OT_SCALE_RANGE_1, /* scale range >=  8/64 */
    OT_SCALE_RANGE_2, /* scale range >= 10/64 */
    OT_SCALE_RANGE_3, /* scale range >= 15/64 */
    OT_SCALE_RANGE_4, /* scale range >= 19/64 */
    OT_SCALE_RANGE_5, /* scale range >= 24/64 */
    OT_SCALE_RANGE_6, /* scale range >= 29/64 */
    OT_SCALE_RANGE_7, /* scale range >= 33/64 */
    OT_SCALE_RANGE_8, /* scale range >= 35/64 */
    OT_SCALE_RANGE_9, /* scale range >= 38/64 */
    OT_SCALE_RANGE_10, /* scale range >= 42/64 */
    OT_SCALE_RANGE_11, /* scale range >= 45/64 */
    OT_SCALE_RANGE_12, /* scale range >= 48/64 */
    OT_SCALE_RANGE_13, /* scale range >= 51/64 */
    OT_SCALE_RANGE_14, /* scale range >= 53/64 */
    OT_SCALE_RANGE_15, /* scale range >= 55/64 */
    OT_SCALE_RANGE_16, /* scale range >= 57/64 */
    OT_SCALE_RANGE_17, /* scale range >= 60/64 */
    OT_SCALE_RANGE_18, /* scale range >  1     */
    OT_SCALE_RANGE_BUTT,
} ot_scale_range_type;

typedef enum  {
    OT_COEF_LEVEL_0 = 0, /* coefficient level 0 */
    OT_COEF_LEVEL_1, /* coefficient level 1 */
    OT_COEF_LEVEL_2, /* coefficient level 2 */
    OT_COEF_LEVEL_3, /* coefficient level 3 */
    OT_COEF_LEVEL_4, /* coefficient level 4 */
    OT_COEF_LEVEL_5, /* coefficient level 5 */
    OT_COEF_LEVEL_6, /* coefficient level 6 */
    OT_COEF_LEVEL_7, /* coefficient level 7 */
    OT_COEF_LEVEL_8, /* coefficient level 8 */
    OT_COEF_LEVEL_9, /* coefficient level 9 */
    OT_COEF_LEVEL_10, /* coefficient level 10 */
    OT_COEF_LEVEL_11, /* coefficient level 11 */
    OT_COEF_LEVEL_12, /* coefficient level 12 */
    OT_COEF_LEVEL_13, /* coefficient level 13 */
    OT_COEF_LEVEL_14, /* coefficient level 14 */
    OT_COEF_LEVEL_15, /* coefficient level 15 */
    OT_COEF_LEVEL_16, /* coefficient level 16 */
    OT_COEF_LEVEL_17, /* coefficient level 17 */
    OT_COEF_LEVEL_18, /* coefficient level 18 */
    OT_COEF_LEVEL_19, /* coefficient level 19 */
    OT_COEF_LEVEL_BUTT,
} ot_coef_level;

typedef struct {
    td_u32 align;
} ot_mpp_sys_cfg;

typedef struct {
    ot_coef_level hor_luma; /* horizontal luminance   coefficient level */
    ot_coef_level hor_chroma; /* horizontal chrominance coefficient level */
    ot_coef_level ver_luma; /* vertical   luminance   coefficient level */
    ot_coef_level ver_chroma; /* vertical   chrominance coefficient level */
} ot_scale_coef_level;

typedef struct {
    ot_scale_range_type hor;
    ot_scale_range_type ver;
} ot_scale_range;

typedef struct {
    ot_scale_range scale_range;
    ot_scale_coef_level scale_coef_level;
} ot_scale_coef_info;

typedef struct {
    td_u32 id[OT_UNIQUE_ID_NUM];
} ot_unique_id;

typedef struct {
    td_char gps_latitude_ref; /* GPS latitude_ref indicates whether the latitude is north or south latitude,
                               * 'N'/'S', default 'N' */
    td_u32 gps_latitude[OT_GPS_EXPRESS_NUM][OT_GPS_GRP_NUM];
                               /* GPS latitude is expressed as degrees, minutes and seconds, a typical format
                                   * like "dd/1, mm/1, ss/1", default 0/0, 0/0, 0/0 */
    td_char gps_longitude_ref; /* GPS longitude_ref indicates whether the longitude is east or west longitude,
                                * 'E'/'W', default 'E' */
    td_u32 gps_longitude[OT_GPS_EXPRESS_NUM][OT_GPS_GRP_NUM];
                                /* GPS longitude is expressed as degrees, minutes and seconds, a typical format
                                    * like "dd/1, mm/1, ss/1", default 0/0, 0/0, 0/0 */
    td_u8 gps_altitude_ref; /* GPS altitude_ref indicates the reference altitude used, 0 - above sea level,
                             * 1 - below sea level default 0 */
    td_u32 gps_altitude[OT_GPS_GRP_NUM];
                             /* GPS altitude_ref indicates the altitude based on the reference gps_altitude_ref,
                                * the reference unit is meters, default 0/0 */
} ot_gps_info;

typedef enum {
    OT_VI_OFFLINE_VPSS_OFFLINE = 0,
    OT_VI_OFFLINE_VPSS_ONLINE,
    OT_VI_ONLINE_VPSS_OFFLINE,
    OT_VI_ONLINE_VPSS_ONLINE,
    OT_VI_PARALLEL_VPSS_OFFLINE,
    OT_VI_PARALLEL_VPSS_PARALLEL,
    OT_VI_VPSS_MODE_BUTT
} ot_vi_vpss_mode_type;

typedef struct {
    ot_vi_vpss_mode_type mode[OT_VI_MAX_PIPE_NUM];
} ot_vi_vpss_mode;

typedef enum {
    OT_VI_AIISP_MODE_DEFAULT = 0,
    OT_VI_AIISP_MODE_AIDRC,
    OT_VI_AIISP_MODE_AIDM,
    OT_VI_AIISP_MODE_AI3DNR,
    OT_VI_AIISP_MODE_BUTT
} ot_vi_aiisp_mode;

typedef enum {
    OT_EXT_CHN_SRC_TYPE_TAIL = 0,
    OT_EXT_CHN_SRC_TYPE_BEFORE_FISHEYE,
    OT_EXT_CHN_SRC_TYPE_HEAD,
    OT_EXT_CHN_SRC_TYPE_BUTT
} ot_ext_chn_src_type;

typedef struct {
    td_u32 comp_ratio_10_bit; /* compression ratio of 10 bit */
    td_u32 comp_ratio_12_bit; /* compression ratio of 12 bit */
    td_u32 comp_ratio_14_bit; /* compression ratio of 14 bit */
} ot_raw_frame_compress_param;


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* OT_COMMON_SYS_H */
