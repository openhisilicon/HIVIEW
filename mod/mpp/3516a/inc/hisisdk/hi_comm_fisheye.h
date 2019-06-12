/******************************************************************************

  Copyright (C), 2013-2033, Hisilicon Tech. Co., Ltd.

 ******************************************************************************
  File Name     : hi_comm_fisheye.h
  Version       :
  Author        : Hisilicon Hi35xx MPP Team
  Created       : 2015/06/15
  Last Modified :
  Description   : common struct definition for FISHEYE
  Function List :
  History       :
******************************************************************************/
#ifndef __HI_COMM_FISHEYE_H__
#define __HI_COMM_FISHEYE_H__

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

#include "hi_type.h"
#include "hi_common.h"
#include "hi_errno.h"
#include "hi_comm_video.h"

#define MAKE_DWORD(high,low) (((low)&0x0000ffff)|((high)<<16))
#define HIGH_WORD(x) (((x)&0xffff0000)>>16)
#define LOW_WORD(x) ((x)&0x0000ffff)

/* failure caused by malloc buffer */
#define HI_ERR_FISHEYE_NOBUF           HI_DEF_ERR(HI_ID_FISHEYE, EN_ERR_LEVEL_ERROR, EN_ERR_NOBUF)
#define HI_ERR_FISHEYE_BUF_EMPTY       HI_DEF_ERR(HI_ID_FISHEYE, EN_ERR_LEVEL_ERROR, EN_ERR_BUF_EMPTY)
#define HI_ERR_FISHEYE_NULL_PTR        HI_DEF_ERR(HI_ID_FISHEYE, EN_ERR_LEVEL_ERROR, EN_ERR_NULL_PTR)
#define HI_ERR_FISHEYE_ILLEGAL_PARAM   HI_DEF_ERR(HI_ID_FISHEYE, EN_ERR_LEVEL_ERROR, EN_ERR_ILLEGAL_PARAM)
#define HI_ERR_FISHEYE_BUF_FULL        HI_DEF_ERR(HI_ID_FISHEYE, EN_ERR_LEVEL_ERROR, EN_ERR_BUF_FULL)
#define HI_ERR_FISHEYE_SYS_NOTREADY    HI_DEF_ERR(HI_ID_FISHEYE, EN_ERR_LEVEL_ERROR, EN_ERR_SYS_NOTREADY)
#define HI_ERR_FISHEYE_NOT_SUPPORT     HI_DEF_ERR(HI_ID_FISHEYE, EN_ERR_LEVEL_ERROR, EN_ERR_NOT_SUPPORT)
#define HI_ERR_FISHEYE_NOT_PERMITTED   HI_DEF_ERR(HI_ID_FISHEYE, EN_ERR_LEVEL_ERROR, EN_ERR_NOT_PERM)

typedef HI_S32      FISHEYE_HANDLE;

typedef struct hiFISHEYE_TASK_ATTR_S
{
    VIDEO_FRAME_INFO_S      stImgIn;             /* input picture */
    VIDEO_FRAME_INFO_S      stImgOut;            /* output picture */
    HI_U32                  au32privateData[4];  /* task's private data */
    HI_U32                  reserved;            /* save current picture's state while debug */
} FISHEYE_TASK_ATTR_S;

typedef struct hiFISHEYE_ADD_PMF_S
{
    HI_S32      				as32PMFCoef[9];
} FISHEYE_ADD_PMF_S;


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* __HI_COMM_FISHEYE_H__ */
