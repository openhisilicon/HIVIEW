/*
 * Copyright (c) Hisilicon Technologies Co., Ltd. 2019-2020. All rights reserved.
 * Description: chip specific configure data structure of sys
 * Author: Hisilicon multimedia software group
 * Create: 2019/07/15
 */

#ifndef __HI_COMMON_SYS_H__
#define __HI_COMMON_SYS_H__

#include "hi_common_video.h"
#include "hi_debug.h"
#include "hi_errno.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* End of #ifdef __cplusplus */

#define HI_GPS_EXPRESS_NUM 3
#define HI_GPS_GRP_NUM 2
#define HI_UNIQUE_ID_NUM 6

#define HI_ERR_SYS_NULL_PTR      HI_DEFINE_ERR(HI_ID_SYS, HI_ERR_LEVEL_ERROR, HI_ERR_NULL_PTR)
#define HI_ERR_SYS_NOT_READY     HI_DEFINE_ERR(HI_ID_SYS, HI_ERR_LEVEL_ERROR, HI_ERR_NOT_READY)
#define HI_ERR_SYS_NOT_PERM      HI_DEFINE_ERR(HI_ID_SYS, HI_ERR_LEVEL_ERROR, HI_ERR_NOT_PERM)
#define HI_ERR_SYS_NO_MEM        HI_DEFINE_ERR(HI_ID_SYS, HI_ERR_LEVEL_ERROR, HI_ERR_NO_MEM)
#define HI_ERR_SYS_ILLEGAL_PARAM HI_DEFINE_ERR(HI_ID_SYS, HI_ERR_LEVEL_ERROR, HI_ERR_ILLEGAL_PARAM)
#define HI_ERR_SYS_BUSY          HI_DEFINE_ERR(HI_ID_SYS, HI_ERR_LEVEL_ERROR, HI_ERR_BUSY)
#define HI_ERR_SYS_NOT_SUPPORT   HI_DEFINE_ERR(HI_ID_SYS, HI_ERR_LEVEL_ERROR, HI_ERR_NOT_SUPPORT)

typedef enum  {
    HI_SCALE_RANGE_0 = 0, /* scale range <   8/64 */
    HI_SCALE_RANGE_1, /* scale range >=  8/64 */
    HI_SCALE_RANGE_2, /* scale range >= 10/64 */
    HI_SCALE_RANGE_3, /* scale range >= 15/64 */
    HI_SCALE_RANGE_4, /* scale range >= 19/64 */
    HI_SCALE_RANGE_5, /* scale range >= 24/64 */
    HI_SCALE_RANGE_6, /* scale range >= 29/64 */
    HI_SCALE_RANGE_7, /* scale range >= 33/64 */
    HI_SCALE_RANGE_8, /* scale range >= 35/64 */
    HI_SCALE_RANGE_9, /* scale range >= 38/64 */
    HI_SCALE_RANGE_10, /* scale range >= 42/64 */
    HI_SCALE_RANGE_11, /* scale range >= 45/64 */
    HI_SCALE_RANGE_12, /* scale range >= 48/64 */
    HI_SCALE_RANGE_13, /* scale range >= 51/64 */
    HI_SCALE_RANGE_14, /* scale range >= 53/64 */
    HI_SCALE_RANGE_15, /* scale range >= 55/64 */
    HI_SCALE_RANGE_16, /* scale range >= 57/64 */
    HI_SCALE_RANGE_17, /* scale range >= 60/64 */
    HI_SCALE_RANGE_18, /* scale range >  1     */
    HI_SCALE_RANGE_BUTT,
} hi_scale_range_type;

typedef enum  {
    HI_COEF_LEVEL_0 = 0, /* coefficient level 0 */
    HI_COEF_LEVEL_1, /* coefficient level 1 */
    HI_COEF_LEVEL_2, /* coefficient level 2 */
    HI_COEF_LEVEL_3, /* coefficient level 3 */
    HI_COEF_LEVEL_4, /* coefficient level 4 */
    HI_COEF_LEVEL_5, /* coefficient level 5 */
    HI_COEF_LEVEL_6, /* coefficient level 6 */
    HI_COEF_LEVEL_7, /* coefficient level 7 */
    HI_COEF_LEVEL_8, /* coefficient level 8 */
    HI_COEF_LEVEL_9, /* coefficient level 9 */
    HI_COEF_LEVEL_10, /* coefficient level 10 */
    HI_COEF_LEVEL_11, /* coefficient level 11 */
    HI_COEF_LEVEL_12, /* coefficient level 12 */
    HI_COEF_LEVEL_13, /* coefficient level 13 */
    HI_COEF_LEVEL_14, /* coefficient level 14 */
    HI_COEF_LEVEL_15, /* coefficient level 15 */
    HI_COEF_LEVEL_16, /* coefficient level 16 */
    HI_COEF_LEVEL_17, /* coefficient level 17 */
    HI_COEF_LEVEL_18, /* coefficient level 18 */
    HI_COEF_LEVEL_BUTT,
} hi_coef_level;

typedef struct {
    hi_u32 align;
} hi_mpp_sys_cfg;

typedef struct {
    hi_phys_addr_t phys_addr;
    hi_bool is_cached;
} hi_sys_virt_mem_info;

typedef struct {
    hi_u32 num;
    hi_mpp_chn mpp_chn[HI_MAX_BIND_DST_NUM];
} hi_mpp_bind_dst;

typedef struct {
    hi_coef_level hor_luma; /* horizontal luminance   coefficient level */
    hi_coef_level hor_chroma; /* horizontal chrominance coefficient level */
    hi_coef_level ver_luma; /* vertical   luminance   coefficient level */
    hi_coef_level ver_chroma; /* vertical   chrominance coefficient level */
} hi_scale_coef_level;

typedef struct {
    hi_scale_range_type hor;
    hi_scale_range_type ver;
} hi_scale_range;

typedef struct {
    hi_scale_range scale_range;
    hi_scale_coef_level scale_coef_level;
} hi_scale_coef_info;

typedef struct {
    hi_u32 id[HI_UNIQUE_ID_NUM];
} hi_unique_id;

typedef struct {
    hi_char gps_latitude_ref; /* GPS latitude_ref indicates whether the latitude is north or south latitude,
                               * 'N'/'S', default 'N' */
    hi_u32 gps_latitude[HI_GPS_EXPRESS_NUM][HI_GPS_GRP_NUM];
                               /* GPS latitude is expressed as degrees, minutes and seconds, a typical format
                                   * like "dd/1, mm/1, ss/1", default 0/0, 0/0, 0/0 */
    hi_char gps_longitude_ref; /* GPS longitude_ref indicates whether the longitude is east or west longitude,
                                * 'E'/'W', default 'E' */
    hi_u32 gps_longitude[HI_GPS_EXPRESS_NUM][HI_GPS_GRP_NUM];
                                /* GPS longitude is expressed as degrees, minutes and seconds, a typical format
                                    * like "dd/1, mm/1, ss/1", default 0/0, 0/0, 0/0 */
    hi_u8 gps_altitude_ref; /* GPS altitude_ref indicates the reference altitude used, 0 - above sea level,
                             * 1 - below sea level default 0 */
    hi_u32 gps_altitude[HI_GPS_GRP_NUM];
                             /* GPS altitude_ref indicates the altitude based on the reference gps_altitude_ref,
                                * the reference unit is meters, default 0/0 */
} hi_gps_info;


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* __HI_COMMON_SYS_H__ */

