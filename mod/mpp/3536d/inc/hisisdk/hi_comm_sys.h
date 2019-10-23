/******************************************************************************
Copyright (C), 2001-2011, Hisilicon Tech. Co., Ltd.
******************************************************************************
File Name     : hi35xx_sys.h
Version       : Initial Draft
Author        : Hi35xx MPP Team
Created       : 2007/1/30
Last Modified :
Description   : Hi35xx chip specific configure data structure
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
extern "C"{
#endif
#endif /* End of #ifdef __cplusplus */

#define HI_TRACE_SYS(level, fmt...) HI_TRACE(level, HI_ID_SYS,##fmt)
typedef struct hiMPP_SYS_CONF_S
{
    /* stride of picture buffer must be aligned with this value.
     * you can choose a value from 1 to 1024, and it must be multiple of 16.
     */
    HI_U32 u32AlignWidth;  

}MPP_SYS_CONF_S;

typedef enum hiSCALE_RANGE_E
{
    SCALE_RANGE_0 = 0,      /* scale range <  1/4 */
    SCALE_RANGE_1,          /* scale range >= 1/4 */
    SCALE_RANGE_2,          /* scale range >= 1/3 */
    SCALE_RANGE_3,          /* scale range >= 1/2 */
    SCALE_RANGE_4,          /* scale range >= 3/4 */
    SCALE_RANGE_5,          /* scale range == 1/1 */
    SCALE_RANGE_6,          /* scale range >  1   */
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


#define HI_ERR_SYS_NULL_PTR         HI_DEF_ERR(HI_ID_SYS, EN_ERR_LEVEL_ERROR, EN_ERR_NULL_PTR)
#define HI_ERR_SYS_NOTREADY         HI_DEF_ERR(HI_ID_SYS, EN_ERR_LEVEL_ERROR, EN_ERR_SYS_NOTREADY)
#define HI_ERR_SYS_NOT_PERM         HI_DEF_ERR(HI_ID_SYS, EN_ERR_LEVEL_ERROR, EN_ERR_NOT_PERM)
#define HI_ERR_SYS_NOMEM            HI_DEF_ERR(HI_ID_SYS, EN_ERR_LEVEL_ERROR, EN_ERR_NOMEM)
#define HI_ERR_SYS_ILLEGAL_PARAM    HI_DEF_ERR(HI_ID_SYS, EN_ERR_LEVEL_ERROR, EN_ERR_ILLEGAL_PARAM)
#define HI_ERR_SYS_BUSY             HI_DEF_ERR(HI_ID_SYS, EN_ERR_LEVEL_ERROR, EN_ERR_BUSY)

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif  /* __HI_COMM_SYS_H__ */

