/******************************************************************************

  Copyright (C), 2001-2011, Hisilicon Tech. Co., Ltd.

 ******************************************************************************
  File Name     : hi_awb_comm.h
  Version       : Initial Draft
  Author        : Hisilicon multimedia software group
  Created       : 2012/12/19
  Description   : 
  History       :
  1.Date        : 2012/12/19
    Author      : n00168968
    Modification: Created file

******************************************************************************/
#ifndef __HI_ISP_DEHAZE_H__
#define __HI_ISP_DEHAZE_H__

#include "hi_type.h"

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* End of #ifdef __cplusplus */

#define FRAMECNT 12
#define DEHAZE_CURVE_NUM        16
#define DEHAZE_CURVE_MAX_NODE   256


typedef enum hiDehze_CTRL_CMD_E
{ 
    
    DEHAZE_DEBUG_ATTR_SET,
    DEHAZE_DEBUG_ATTR_GET,
    DEHAZE_DEBUG_WAIT_END,
    DEHAZE_CTRL_BUTT,
    
}DEHAZE_CTRL_CMD_E;

typedef struct hiDEHAZE_DBG_ATTR_S
{
    HI_U32 hblk_num;
    HI_U32 vblk_num;
    HI_U32 u32ImageHeight;
    HI_U32 u32ImageWidth;    
} DEHAZE_DBG_ATTR_S;

typedef struct hiDEHAZE_DBG_STATUS_S
{
    HI_U16  AirPre[FRAMECNT][3];
    HI_U16  maxdc;
    HI_U32  u32CurFrmNum;
    HI_U16  au8DehazeMinDout[DEFOG_ZONE_NUM]; 
    HI_U16  MinAdapt[DEFOG_ZONE_NUM];
    HI_U16  tmpr_min_flt[DEFOG_ZONE_NUM];
    HI_U16  v_blur_flt_dout[DEFOG_ZONE_NUM];
    HI_U16  pre_dc[DEFOG_ZONE_NUM];   
    HI_U16  flt_a[3];     
    HI_U16  defog_strength;
    HI_U16  curve_index;
    HI_U32  u32FrmNumBgn;
    HI_U32  u32FrmNumEnd; 
    HI_U32  CurrA[3];
    HI_U32  au32DehazeMaxStatDout[DEFOG_ZONE_NUM];
    HI_U8   Lut[DEHAZE_CURVE_MAX_NODE];
    
    
} DEHAZE_DBG_STATUS_S;


typedef struct hiDEHAZE_DBG_CTRL_S
{
    HI_BOOL bPreDebugEn;
    HI_U32  u32PrePhyAddr;
    HI_U32  u32PreDepth;
    HI_U32  u32PreSize;
    
    HI_BOOL bDebugEn;
    HI_BOOL bDebugOut;
    HI_BOOL bDbgCompleted;
    HI_U32  u32PhyAddr;
    HI_U32  u32Depth;    
    HI_U32  u32Size;
    HI_U32 u32DbgFrmCnt;
    
    DEHAZE_DBG_ATTR_S   *pstDbgAttr;
    DEHAZE_DBG_STATUS_S *pstDbgStatus;
    
} DEHAZE_DBG_CTRL_S;



#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */

#endif






