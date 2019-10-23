/******************************************************************************
Copyright (C), 2001-2011, Hisilicon Tech. Co., Ltd.
******************************************************************************
File Name     : hi3511_sys.h
Version       : Initial Draft
Author        : Hi3511 MPP Team
Created       : 2007/1/30
Last Modified :
Description   : Hi3511 chip specific configure data structure
Function List :
History       :
 1.Date        : 2007/1/30
   Author      : c42025
   Modification: Created file

 2.Date        : 2007/11/30
   Author      : c42025
   Modification: modify according review comments

 3.Date        : 2008/03/03
   Author      : c42025
   Modification: modify HI_TRACE_SYS

 4.Date        : 2008/03/05
   Author      : c42025
   Modification: modify 'HI_LOG_LEVEL_ERROR' to 'EN_ERR_LEVEL_ERROR'

******************************************************************************/
#ifndef __HI_COMM_SYS_H__
#define __HI_COMM_SYS_H__

#include "hi_type.h"
#include "hi_errno.h"
#include "hi_debug.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* End of #ifdef __cplusplus */

#define HI_TRACE_SYS(level, fmt...)\
do{ \
        HI_TRACE(level, HI_ID_SYS,"[Func]:%s [Line]:%d [Info]:", __FUNCTION__, __LINE__);\
        HI_TRACE(level,HI_ID_SYS,##fmt);\
}while(0)


typedef struct hiMPP_SYS_CONF_S
{
    /* stride of picture buffer must be aligned with this value.
     * you can choose a value from 1 to 1024, and it must be multiple of 16.
     */
    HI_U32 u32AlignWidth;

} MPP_SYS_CONF_S;

typedef struct hiSYS_VIRMEM_INFO_S
{
    HI_U32  u32PhyAddr;
    HI_BOOL bCached;
} SYS_VIRMEM_INFO_S;

typedef enum hiSCALE_RANGE_E
{
    SCALE_RANGE_0 = 0,      /* scale range <  8/64 */ 
    SCALE_RANGE_1,          /* scale range >= 8/64 */ 
    SCALE_RANGE_2,          /* scale range >= 10/64 */ 
    SCALE_RANGE_3,          /* scale range >= 15/64 */ 
    SCALE_RANGE_4,          /* scale range >= 19/64 */ 
    SCALE_RANGE_5,          /* scale range >= 24/64 */ 
    SCALE_RANGE_6,          /* scale range >= 29/64 */ 
    SCALE_RANGE_7,          /* scale range >= 33/64 */
    SCALE_RANGE_8,          /* scale range >= 35/64 */
    SCALE_RANGE_9,          /* scale range >= 38/64 */
    SCALE_RANGE_10,         /* scale range >= 42/64 */
    SCALE_RANGE_11,         /* scale range >= 45/64 */
    SCALE_RANGE_12,         /* scale range >= 48/64 */
    SCALE_RANGE_13,         /* scale range >= 51/64 */
    SCALE_RANGE_14,         /* scale range >= 53/64 */
    SCALE_RANGE_15,         /* scale range >= 55/64 */
    SCALE_RANGE_16,         /* scale range >= 57/64*/
    SCALE_RANGE_17,         /* scale range >= 60/64 */
    SCALE_RANGE_18,         /* scale range >  1     */
    SCALE_RANGE_BUTT,
} SCALE_RANGE_E;

typedef enum hiCOEFF_LEVEL_E
{
    COEFF_LEVEL_0 = 0,      /* coefficient level 0 */
    COEFF_LEVEL_1,          /* coefficient level 1 */
    COEFF_LEVEL_2,          /* coefficient level 2 */
    COEFF_LEVEL_3,          /* coefficient level 3 */
    COEFF_LEVEL_4,          /* coefficient level 4 */
    COEFF_LEVEL_5,          /* coefficient level 5 */
    COEFF_LEVEL_6,          /* coefficient level 6 */
    COEFF_LEVEL_7,          /* coefficient level 7 */
    COEFF_LEVEL_8,          /* coefficient level 8 */
    COEFF_LEVEL_9,          /* coefficient level 9 */
    COEFF_LEVEL_10,         /* coefficient level 10 */
    COEFF_LEVEL_11,         /* coefficient level 11 */
    COEFF_LEVEL_12,         /* coefficient level 12 */
    COEFF_LEVEL_13,         /* coefficient level 13 */
    COEFF_LEVEL_14,         /* coefficient level 14 */
    COEFF_LEVEL_15,         /* coefficient level 15 */
    COEFF_LEVEL_16,         /* coefficient level 16 */
    COEFF_LEVEL_17,         /* coefficient level 17 */
    COEFF_LEVEL_18,         /* coefficient level 18 */
    COEFF_LEVEL_BUTT,
} COEFF_LEVEL_E;


typedef struct hiSCALE_COEFF_LEVEL_S
{
    COEFF_LEVEL_E enHorLum; /* horizontal luminance   coefficient level */    
    COEFF_LEVEL_E enHorChr; /* horizontal chrominance coefficient level */    
    COEFF_LEVEL_E enVerLum; /* vertical   luminance   coefficient level */    
    COEFF_LEVEL_E enVerChr; /* vertical   chrominance coefficient level */    
} SCALE_COEFF_LEVEL_S;

typedef struct hiSCALE_RANGE_S
{
    SCALE_RANGE_E enHorizontal;
    SCALE_RANGE_E enVertical;   
} SCALE_RANGE_S;

typedef struct hiSCALE_COEFF_INFO_S
{
    SCALE_RANGE_S stScaleRange;
    SCALE_COEFF_LEVEL_S stScaleCoeffLevel;   
} SCALE_COEFF_INFO_S;

typedef struct hiGPS_INFO_S
{
    HI_CHAR  chGPSLatitudeRef;         /* GPS LatitudeRef Indicates whether the latitude is north or south latitude, 
                                    * 'N'/'S', default 'N' */
    HI_U32   au32GPSLatitude[3][2];    /* GPS Latitude is expressed as degrees, minutes and seconds,a typical format like "dd/1,mm/1,ss/1", 
                                    *  default 0/1, 0/1, 0/1 */ 
    HI_CHAR  chGPSLongitudeRef;        /* GPS LongitudeRef Indicates whether the longitude is east or west longitude, 
                                    * 'E'/'W', default 'E' */
    HI_U32   au32GPSLongitude[3][2];   /* GPS Longitude is expressed as degrees, minutes and seconds,a typical format like "dd/1,mm/1,ss/1", 
                                    *  default 0/1, 0/1, 0/1 */ 
	HI_U8    u8GPSAltitudeRef;         /* GPS AltitudeRef Indicates the reference altitude used, 0 - above sea level, 1 - below sea level
                                    * default 0 */
    HI_U32   au32GPSAltitude[2];       /* GPS AltitudeRef Indicates the altitude based on the reference u8GPSAltitudeRef, the reference unit is meters,
                                    * default 0/1 */
} GPS_INFO_S;


#define HI_ERR_SYS_NULL_PTR         HI_DEF_ERR(HI_ID_SYS, EN_ERR_LEVEL_ERROR, EN_ERR_NULL_PTR)
#define HI_ERR_SYS_NOTREADY         HI_DEF_ERR(HI_ID_SYS, EN_ERR_LEVEL_ERROR, EN_ERR_SYS_NOTREADY)
#define HI_ERR_SYS_NOT_PERM         HI_DEF_ERR(HI_ID_SYS, EN_ERR_LEVEL_ERROR, EN_ERR_NOT_PERM)
#define HI_ERR_SYS_NOMEM            HI_DEF_ERR(HI_ID_SYS, EN_ERR_LEVEL_ERROR, EN_ERR_NOMEM)
#define HI_ERR_SYS_ILLEGAL_PARAM    HI_DEF_ERR(HI_ID_SYS, EN_ERR_LEVEL_ERROR, EN_ERR_ILLEGAL_PARAM)
#define HI_ERR_SYS_BUSY             HI_DEF_ERR(HI_ID_SYS, EN_ERR_LEVEL_ERROR, EN_ERR_BUSY)
#define HI_ERR_SYS_NOT_SUPPORT      HI_DEF_ERR(HI_ID_SYS, EN_ERR_LEVEL_ERROR, EN_ERR_NOT_SUPPORT)


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif  /* __HI_COMM_SYS_H__ */

