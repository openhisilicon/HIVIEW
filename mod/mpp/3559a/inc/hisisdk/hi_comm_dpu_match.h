/******************************************************************************

  Copyright (C), 2017-2018, Hisilicon Tech. Co., Ltd.

 ******************************************************************************
  File Name     : hi_comm_match.h
  Version       : Initial Draft
  Author        : Hisilicon multimedia software group
  Created       : 2017/07/22
  Description   :
  History       :
  1.Date        : 2017/07/22
    Modification: Created file

******************************************************************************/

#ifndef __HI_COMM_MATCH_H__
#define __HI_COMM_MATCH_H__


#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */
#include "hi_common.h"
#include "hi_errno.h"
#include "hi_defines.h"
#include "hi_comm_video.h"

#define DPU_MATCH_LEFT_PIPE 0
#define DPU_MATCH_RIGHT_PIPE 1

typedef   HI_S32 DPU_MATCH_GRP;
typedef   HI_S32 DPU_MATCH_PIPE;
typedef   HI_S32 DPU_MATCH_CHN;

typedef enum hiEN_DPU_MATCH_ERR_CODE_E
{
    ERR_DPU_MATCH_SYS_TIMEOUT    = 0x40,   /* DPU MATCH process timeout */
    ERR_DPU_MATCH_OPEN_FILE      = 0x41,   /* DPU MATCH open file error */
    ERR_DPU_MATCH_READ_FILE      = 0x42,   /* DPU MATCH read file error */
    ERR_DPU_MATCH_WRITE_FILE     = 0x43,   /* DPU MATCH write file error */

    ERR_DPU_MATCH_BUTT
}EN_DPU_MATCH_ERR_CODE_E;

#define HI_ERR_DPU_MATCH_NULL_PTR        HI_DEF_ERR(HI_ID_DPU_MATCH, EN_ERR_LEVEL_ERROR, EN_ERR_NULL_PTR)
#define HI_ERR_DPU_MATCH_NOTREADY        HI_DEF_ERR(HI_ID_DPU_MATCH, EN_ERR_LEVEL_ERROR, EN_ERR_SYS_NOTREADY)
#define HI_ERR_DPU_MATCH_INVALID_DEVID   HI_DEF_ERR(HI_ID_DPU_MATCH, EN_ERR_LEVEL_ERROR, EN_ERR_INVALID_DEVID)
#define HI_ERR_DPU_MATCH_INVALID_CHNID   HI_DEF_ERR(HI_ID_DPU_MATCH, EN_ERR_LEVEL_ERROR, EN_ERR_INVALID_CHNID)
#define HI_ERR_DPU_MATCH_EXIST           HI_DEF_ERR(HI_ID_DPU_MATCH, EN_ERR_LEVEL_ERROR, EN_ERR_EXIST)
#define HI_ERR_DPU_MATCH_UNEXIST         HI_DEF_ERR(HI_ID_DPU_MATCH, EN_ERR_LEVEL_ERROR, EN_ERR_UNEXIST)
#define HI_ERR_DPU_MATCH_NOT_SUPPORT     HI_DEF_ERR(HI_ID_DPU_MATCH, EN_ERR_LEVEL_ERROR, EN_ERR_NOT_SUPPORT)
#define HI_ERR_DPU_MATCH_NOT_PERM        HI_DEF_ERR(HI_ID_DPU_MATCH, EN_ERR_LEVEL_ERROR, EN_ERR_NOT_PERM)
#define HI_ERR_DPU_MATCH_NOMEM           HI_DEF_ERR(HI_ID_DPU_MATCH, EN_ERR_LEVEL_ERROR, EN_ERR_NOMEM)
#define HI_ERR_DPU_MATCH_NOBUF           HI_DEF_ERR(HI_ID_DPU_MATCH, EN_ERR_LEVEL_ERROR, EN_ERR_NOBUF)
#define HI_ERR_DPU_MATCH_ILLEGAL_PARAM   HI_DEF_ERR(HI_ID_DPU_MATCH, EN_ERR_LEVEL_ERROR, EN_ERR_ILLEGAL_PARAM)
#define HI_ERR_DPU_MATCH_BUSY            HI_DEF_ERR(HI_ID_DPU_MATCH, EN_ERR_LEVEL_ERROR, EN_ERR_BUSY)
#define HI_ERR_DPU_MATCH_BUF_EMPTY       HI_DEF_ERR(HI_ID_DPU_MATCH, EN_ERR_LEVEL_ERROR, EN_ERR_BUF_EMPTY)

#define HI_ERR_DPU_MATCH_SYS_TIMEOUT       HI_DEF_ERR(HI_ID_DPU_MATCH, EN_ERR_LEVEL_ERROR, ERR_DPU_MATCH_SYS_TIMEOUT)
#define HI_ERR_DPU_MATCH_OPEN_FILE         HI_DEF_ERR(HI_ID_DPU_MATCH, EN_ERR_LEVEL_ERROR, ERR_DPU_MATCH_OPEN_FILE)
#define HI_ERR_DPU_MATCH_READ_FILE         HI_DEF_ERR(HI_ID_DPU_MATCH, EN_ERR_LEVEL_ERROR, ERR_DPU_MATCH_READ_FILE)
#define HI_ERR_DPU_MATCH_WRITE_FILE        HI_DEF_ERR(HI_ID_DPU_MATCH, EN_ERR_LEVEL_ERROR, ERR_DPU_MATCH_WRITE_FILE)


typedef struct hiDPU_MATCH_MEM_INFO_S
{
	HI_U64  u64PhyAddr;
	HI_U64  u64VirAddr;
	HI_U32  u32Size;
}DPU_MATCH_MEM_INFO_S;

typedef enum hiDPU_MATCH_MASK_MODE_E
{
	DPU_MATCH_MASK_DEFAULT_MODE   =  0x0,
	DPU_MATCH_MASK_1X1_MODE  =  0x1,
	DPU_MATCH_MASK_3X3_MODE  =  0x2,
	DPU_MATCH_MASK_5X5_MODE  =  0x3,
	DPU_MATCH_MASK_7X7_MODE  =  0x4,
	DPU_MATCH_MASK_9X9_MODE  =  0x5,
	DPU_MATCH_MASK_MODE_BUTT
}DPU_MATCH_MASK_MODE_E;

typedef enum hiDPU_MATCH_DENS_ACCU_MODE_E
{
	DPU_MATCH_DENS_ACCU_MODE_D0_A9  =  0x0,
	DPU_MATCH_DENS_ACCU_MODE_D1_A8  =  0x1,
	DPU_MATCH_DENS_ACCU_MODE_D2_A7  =  0x2,
	DPU_MATCH_DENS_ACCU_MODE_D3_A6  =  0x3,
	DPU_MATCH_DENS_ACCU_MODE_D4_A5  =  0x4,
	DPU_MATCH_DENS_ACCU_MODE_D5_A4  =  0x5,
	DPU_MATCH_DENS_ACCU_MODE_D6_A3  =  0x6,
	DPU_MATCH_DENS_ACCU_MODE_D7_A2  =  0x7,
	DPU_MATCH_DENS_ACCU_MODE_D8_A1  =  0x8,
	DPU_MATCH_DENS_ACCU_MODE_D9_A0  =  0x9,
	DPU_MATCH_DENS_ACCU_MODE_BUTT
}DPU_MATCH_DENS_ACCU_MODE_E;

typedef enum hiDPU_MATCH_SPEED_ACCU_MODE_E
{
	DPU_MATCH_SPEED_ACCU_MODE_SPEED  =  0x0,
	DPU_MATCH_SPEED_ACCU_MODE_ACCU  =  0x1,
	DPU_MATCH_SPEED_ACCU_MODE_BUTT
}DPU_MATCH_SPEED_ACCU_MODE_E;


typedef enum hiDPU_MATCH_DISP_SUBPIXEL_E
{
	DPU_MATCH_DISP_SUBPIXEL_DISABLE  =  0x0,
	DPU_MATCH_DISP_SUBPIXEL_ENABLE  =  0x1,
	DPU_MATCH_DISP_SUBPIXEL_BUTT
}DPU_MATCH_DISP_SUBPIXEL_E;

typedef struct hiDPU_MATCH_GRP_ATTR_S
{
    SIZE_S stLeftImageSize; /* Left image size. */
    SIZE_S stRightImageSize; /* Right image size. */
    DPU_MATCH_MASK_MODE_E enMatchMaskMode; /* Aggregation mask mode. */
	DPU_MATCH_DENS_ACCU_MODE_E enDensAccuMode; /* Adjust density-accuracy trade-off. */
	DPU_MATCH_SPEED_ACCU_MODE_E	enSpeedAccuMode; /* Adjust speed-accuracy trade-off. */
	DPU_MATCH_DISP_SUBPIXEL_E enDispSubpixelEn;	/* Calculate subpixel disparity or not. */
    HI_U16 u16DispNum; /* The number of disparity, when the value is less than or equal to 128,
                         it must be the multiple of 16, and when the value is greater than 128,
                         it must be the multiple of 32. Range:[16,256] */
	HI_U16 u16DispStartPos;	 /* Minimum disparity, it must be the multiple of 2. Range:[0,126] */
    HI_U32 u32Depth;  /* The depth of user image queue for getting Match output image,
                           it can not be changed dynamic. Range:[0,8] */
    HI_BOOL bNeedSrcFrame; /* The flag of getting source videoframe.It will effect when bind dpu rect. */
    DPU_MATCH_MEM_INFO_S stAssistBuf; /* Assistance buffer. */
    FRAME_RATE_CTRL_S stFrameRate;  /* Grp frame rate contrl. */
}DPU_MATCH_GRP_ATTR_S;

typedef struct hiDPU_MATCH_CHN_ATTR_S
{
    SIZE_S stImageSize;  /* Output image size. */
}DPU_MATCH_CHN_ATTR_S;

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */


#endif /* __HI_COMM_MATCH_H__ */

