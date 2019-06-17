/******************************************************************************

  Copyright (C), 2001-2011, Hisilicon Tech. Co., Ltd.

 ******************************************************************************
  File Name     : hi_comm_3a.h
  Version       : Initial Draft
  Author        : Hisilicon multimedia software group
  Created       : 2012/12/18
  Description   : 
  History       :
  1.Date        : 2012/12/18
    Author      : n00168968
    Modification: Created file

******************************************************************************/

#ifndef __HI_COMM_3A_H__
#define __HI_COMM_3A_H__

#include "hi_common.h"
#include "hi_comm_isp.h"


#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* End of #ifdef __cplusplus */

#define IS_LINEAR_MODE(mode)      (WDR_MODE_NONE == (mode))
#define IS_WDR_MODE(mode)         ((!IS_LINEAR_MODE(mode)) && ((mode) < WDR_MODE_BUTT))
#define IS_BUILT_IN_WDR_MODE(mode) (WDR_MODE_BUILT_IN == (mode))
#define IS_FS_WDR_MODE(mode)      ((!IS_LINEAR_MODE(mode))&& (!IS_BUILT_IN_WDR_MODE(mode)) && ((mode) < WDR_MODE_BUTT))
#define IS_2to1_WDR_MODE(mode)    ((WDR_MODE_2To1_FRAME == (mode)) || (WDR_MODE_2To1_FRAME_FULL_RATE == (mode)) || (WDR_MODE_2To1_LINE == (mode)))
#define IS_3to1_WDR_MODE(mode)    ((WDR_MODE_3To1_FRAME == (mode)) || (WDR_MODE_3To1_FRAME_FULL_RATE == (mode)) || (WDR_MODE_3To1_LINE == (mode)))
#define IS_4to1_WDR_MODE(mode)    ((WDR_MODE_4To1_FRAME == (mode)) || (WDR_MODE_4To1_FRAME_FULL_RATE == (mode)) || (WDR_MODE_4To1_LINE == (mode)))
#define IS_FULL_WDR_MODE(mode)    ((WDR_MODE_2To1_FRAME_FULL_RATE == (mode)) || (WDR_MODE_3To1_FRAME_FULL_RATE == (mode)) || (WDR_MODE_4To1_FRAME_FULL_RATE == (mode)))
#define IS_HALF_WDR_MODE(mode)    ((WDR_MODE_2To1_FRAME == (mode)) || (WDR_MODE_3To1_FRAME == (mode)) || (WDR_MODE_4To1_FRAME == (mode)))
#define IS_LINE_WDR_MODE(mode)    ((WDR_MODE_2To1_LINE == (mode)) || (WDR_MODE_3To1_LINE == (mode)) || (WDR_MODE_4To1_LINE == (mode)))

#define IS_MAIN_CHANNAL(IspDev)   (0 == (IspDev))

#define MAX_REGISTER_ALG_LIB_NUM 2

typedef enum hiISP_ALG_MOD_E
{
    ISP_ALG_AE = 0,
    ISP_ALG_AF,
    ISP_ALG_AWB,
    //ISP_ALG_ANTIFOG,
    ISP_ALG_BLC,
    ISP_ALG_DP,
    ISP_ALG_DRC,
    ISP_ALG_DEMOSAIC,
    ISP_ALG_GAMMA,
    ISP_ALG_GAMMAFE,
    ISP_ALG_GAMMABE,
    ISP_ALG_GE,
    //ISP_ALG_NEW_ANTIFOG,
    ISP_ALG_NR,
    ISP_ALG_SHARPEN,
    ISP_ALG_SHADING,
    ISP_ALG_FrameWDR,
    ISP_ALG_FPN,
    ISP_ALG_DEHAZE,
    ISP_ALG_ACM,
    ISP_ALG_IF,
    ISP_ALG_BUTT,
} ISP_ALG_MOD_E;

typedef enum hiISP_CTRL_CMD_E
{
    ISP_WDR_MODE_SET = 8000,
    ISP_PROC_WRITE,

    ISP_AE_FPS_BASE_SET,  

    ISP_AWB_ISO_SET,  /* set iso, change saturation when iso change */
    ISP_CHANGE_IMAGE_MODE_SET,
    ISP_UPDATE_INFO_GET,
    ISP_FRAMEINFO_GET,
    ISP_AWB_INTTIME_SET,
    ISP_PROTRIGGER_SET,

    ISP_CTRL_CMD_BUTT,
} ISP_CTRL_CMD_E;

typedef struct hiISP_CTRL_PROC_WRITE_S
{
    HI_CHAR *pcProcBuff;
    HI_U32   u32BuffLen;
    HI_U32   u32WriteLen;   /* The len count should contain '\0'. */
} ISP_CTRL_PROC_WRITE_S;

/********************************  AE  *************************************/
/* the init param of ae alg */
typedef struct hiISP_AE_PARAM_S
{
    SENSOR_ID SensorId;
    HI_U8 u8WDRMode;
    HI_FLOAT f32Fps;
    
    HI_S32 s32Rsv;
} ISP_AE_PARAM_S;

/* the statistics of ae alg */
typedef struct hiISP_AE_STAT_1_S
{
    HI_U8   au8MeteringHistThresh[HIST_THRESH_NUM];
    HI_U16  au16MeteringHist[HIST_5_NUM];
} ISP_AE_STAT_1_S;

/* Hi3519 not support */
typedef struct hiISP_AE_STAT_2_S
{
    HI_U8   au8MeteringHistThresh[HIST_THRESH_NUM];
    HI_U16  au16MeteringMemArrary[AE_ZONE_ROW][AE_ZONE_COLUMN][HIST_5_NUM];
} ISP_AE_STAT_2_S;

typedef struct hiISP_AE_STAT_3_S
{    
    HI_U32  au32HistogramMemArray[HIST_1024_NUM];
} ISP_AE_STAT_3_S;

typedef struct hiISP_AE_STAT_4_S
{    
    HI_U16  u16GlobalAvgR;
    HI_U16  u16GlobalAvgGr;
    HI_U16  u16GlobalAvgGb;
    HI_U16  u16GlobalAvgB; 
} ISP_AE_STAT_4_S;

typedef struct hiISP_AE_STAT_5_S
{    
    HI_U16  au16ZoneAvg[AE_ZONE_ROW][AE_ZONE_COLUMN][ISP_BAYER_CHN_NUM];    
} ISP_AE_STAT_5_S;

typedef struct hiISP_AE_STAT_6_S
{    
    HI_U32  au32PreHistogram0[HIST_256_NUM];   /* After TPG, channel 1 256 bin histogram */
    HI_U32  au32PreHistogram1[HIST_256_NUM];   /* After TPG, channel 2 256 bin histogram */
    HI_U32  au32PreHistogram2[HIST_256_NUM];   /* After TPG, channel 3 256 bin histogram */
    HI_U32  au32PreHistogram3[HIST_256_NUM];   /* After TPG, channel 4 256 bin histogram */
} ISP_AE_STAT_6_S;


typedef struct hiISP_AE_INFO_S
{
    HI_U32  u32FrameCnt;    /* the counting of frame */

    ISP_AE_STAT_1_S *pstAeStat1;
    ISP_AE_STAT_2_S *pstAeStat2;
    ISP_AE_STAT_3_S *pstAeStat3;
    ISP_AE_STAT_4_S *pstAeStat4;
    ISP_AE_STAT_5_S *pstAeStat5;
    ISP_AE_STAT_6_S *pstAeStat6;
} ISP_AE_INFO_S;

typedef struct hiISP_AE_STAT_ATTR_S
{
    HI_BOOL bChange;

    HI_U8   au8MeteringHistThresh[HIST_THRESH_NUM];
    HI_U8   au8WeightTable[AE_ZONE_ROW][AE_ZONE_COLUMN];
} ISP_AE_STAT_ATTR_S;

/* the final calculate of ae alg */
#define AE_INT_TIME_NUM     (4)
typedef struct hiISP_AE_RESULT_S
{
    HI_U32  u32IntTime[AE_INT_TIME_NUM];
    HI_U32  u32IspDgain;
    HI_U32  u32Iso;
    HI_U8   u8AERunInterval;

    HI_BOOL bPirisValid;
    HI_S32  s32PirisPos;
    HI_U32  u32PirisGain;

    ISP_FSWDR_MODE_E enFSWDRMode;
	HI_U32  u32WDRGain[4];
    
    ISP_AE_STAT_ATTR_S stStatAttr;
} ISP_AE_RESULT_S;

typedef struct hiISP_AE_EXP_FUNC_S
{
    HI_S32 (*pfn_ae_init)(HI_S32 s32Handle, const ISP_AE_PARAM_S *pstAeParam);
    HI_S32 (*pfn_ae_run)(HI_S32 s32Handle,
        const ISP_AE_INFO_S *pstAeInfo,
        ISP_AE_RESULT_S *pstAeResult,
        HI_S32 s32Rsv
        );
    HI_S32 (*pfn_ae_ctrl)(HI_S32 s32Handle, HI_U32 u32Cmd, HI_VOID *pValue);
    HI_S32 (*pfn_ae_exit)(HI_S32 s32Handle);
} ISP_AE_EXP_FUNC_S;

typedef struct hiISP_AE_REGISTER_S
{
    ISP_AE_EXP_FUNC_S stAeExpFunc;
} ISP_AE_REGISTER_S;

/********************************  AWB  *************************************/
typedef enum hiAWB_CTRL_CMD_E
{
    AWB_CCM_CONFIG_SET,
    AWB_CCM_CONFIG_GET,
    
    AWB_DEBUG_ATTR_SET,
    AWB_DEBUG_ATTR_GET,

    AWB_CTRL_BUTT,
} AWB_CTRL_CMD_E;

typedef struct hiAWB_CCM_CONFIG_S
{
    HI_BOOL bAWBBypassEn;
    HI_BOOL bManualSatEn;
    HI_BOOL bManualTempEn;

    HI_U32  u32ManualSatValue;
    HI_U32  u32ManualTempValue;	
    HI_U16  u16CCMSpeed;

}AWB_CCM_CONFIG_S;

/* the init param of awb alg */
typedef struct hiISP_AWB_PARAM_S
{
    SENSOR_ID SensorId;
    HI_U8 u8WDRMode;
    HI_U8 u8AWBZoneRow;
    HI_U8 u8AWBZoneCol;
    HI_S8 s8Rsv;
} ISP_AWB_PARAM_S;

/* the statistics of awb alg */
typedef struct hiISP_AWB_STAT_1_S
{
    HI_U16  u16MeteringAwbRg;
    HI_U16  u16MeteringAwbBg;
    HI_U32  u32MeteringAwbSum;
} ISP_AWB_STAT_1_S;

typedef struct hiISP_AWB_STAT_2_S
{
    HI_U16  au16MeteringMemArrayRg[AWB_ZONE_NUM];
    HI_U16  au16MeteringMemArrayBg[AWB_ZONE_NUM];
    HI_U16  au16MeteringMemArraySum[AWB_ZONE_NUM];
} ISP_AWB_STAT_2_S;

typedef struct hiISP_AWB_STAT_3_S
{
    HI_U16  u16MeteringAwbAvgR;
    HI_U16  u16MeteringAwbAvgG;
    HI_U16  u16MeteringAwbAvgB;
    HI_U16  u16MeteringAwbCountAll;
    HI_U16  u16MeteringAwbCountMin;
    HI_U16  u16MeteringAwbCountMax;
} ISP_AWB_STAT_3_S;

typedef struct hiISP_AWB_STAT_4_S
{
    HI_U16  au16MeteringMemArrayAvgR[AWB_ZONE_NUM];
    HI_U16  au16MeteringMemArrayAvgG[AWB_ZONE_NUM];
    HI_U16  au16MeteringMemArrayAvgB[AWB_ZONE_NUM];
    HI_U16  au16MeteringMemArrayCountAll[AWB_ZONE_NUM];
    HI_U16  au16MeteringMemArrayCountMin[AWB_ZONE_NUM];
    HI_U16  au16MeteringMemArrayCountMax[AWB_ZONE_NUM];
} ISP_AWB_STAT_4_S;

typedef struct hiISP_AWB_INFO_S
{
    HI_U32  u32FrameCnt;

    ISP_AWB_STAT_1_S *pstAwbStat1;
    ISP_AWB_STAT_2_S *pstAwbStat2;
    ISP_AWB_STAT_3_S *pstAwbStat3;
    ISP_AWB_STAT_4_S *pstAwbStat4;
} ISP_AWB_INFO_S;

/* the statistics's attr of awb alg */
typedef struct hiISP_AWB_STAT_ATTR_S
{
    HI_BOOL bChange;

    HI_U16  u16MeteringWhiteLevelAwb;
    HI_U16  u16MeteringBlackLevelAwb;
    HI_U16  u16MeteringCrRefMaxAwb;
    HI_U16  u16MeteringCbRefMaxAwb;
    HI_U16  u16MeteringCrRefMinAwb;
    HI_U16  u16MeteringCbRefMinAwb;
    HI_U16  u16MeteringCrRefHighAwb;
    HI_U16  u16MeteringCrRefLowAwb;
    HI_U16  u16MeteringCbRefHighAwb;
    HI_U16  u16MeteringCbRefLowAwb;
} ISP_AWB_STAT_ATTR_S;

typedef struct hiISP_AWB_RAW_STAT_ATTR_S
{
    HI_BOOL bChange;
    HI_BOOL bAboveWhiteLevelClip;
    HI_BOOL bBelowBlackLevelClip;

    HI_U16  u16MeteringWhiteLevelAwb;
    HI_U16  u16MeteringBlackLevelAwb;
    HI_U16  u16MeteringCrRefMaxAwb;
    HI_U16  u16MeteringCbRefMaxAwb;
    HI_U16  u16MeteringCrRefMinAwb;
    HI_U16  u16MeteringCbRefMinAwb;
    HI_U16  u16MeteringCrRefHighAwb;
    HI_U16  u16MeteringCrRefLowAwb;
    HI_U16  u16MeteringCbRefHighAwb;
    HI_U16  u16MeteringCbRefLowAwb;
} ISP_AWB_RAW_STAT_ATTR_S;

typedef struct hiISP_ACM_GAIN_ATTR_S
{
    HI_BOOL bChange;
    HI_U16 u16GainLuma;
    HI_U16 u16GainHue;
    HI_U16 u16GainSat;
} ISP_ACM_GAIN_ATTR_S;

/* the final calculate of awb alg */
typedef struct hiISP_AWB_RESULT_S
{
    HI_U32  au32WhiteBalanceGain[ISP_BAYER_CHN_NUM];
    HI_U16  au16ColorMatrix[CCM_MATRIX_SIZE];

    HI_U32  u32ColorTemp;
    
    ISP_AWB_STAT_ATTR_S stStatAttr;
    ISP_AWB_RAW_STAT_ATTR_S stRawStatAttr;
    ISP_ACM_GAIN_ATTR_S stAcmGainAttr; 
} ISP_AWB_RESULT_S;

typedef struct hiISP_AWB_EXP_FUNC_S
{
    HI_S32 (*pfn_awb_init)(HI_S32 s32Handle, const ISP_AWB_PARAM_S *pstAwbParam);
    HI_S32 (*pfn_awb_run)(HI_S32 s32Handle,
        const ISP_AWB_INFO_S *pstAwbInfo,
        ISP_AWB_RESULT_S *pstAwbResult,
        HI_S32 s32Rsv
        );
    HI_S32 (*pfn_awb_ctrl)(HI_S32 s32Handle, HI_U32 u32Cmd, HI_VOID *pValue);
    HI_S32 (*pfn_awb_exit)(HI_S32 s32Handle);
} ISP_AWB_EXP_FUNC_S;

typedef struct hiISP_AWB_REGISTER_S
{    
    ISP_AWB_EXP_FUNC_S stAwbExpFunc;
} ISP_AWB_REGISTER_S;

/********************************  AF  *************************************/

/* the init param of af alg */
typedef struct hiISP_AF_PARAM_S
{
    SENSOR_ID SensorId;
    HI_U8 u8WDRMode;
    
    HI_S32 s32Rsv;
} ISP_AF_PARAM_S;

/* the statistics of af alg */
typedef struct hiISP_AF_ZONE_S
{
    HI_U16  u16v1;
    HI_U16  u16h1;
    HI_U16  u16v2;
    HI_U16  u16h2;
    HI_U16  u16y;
    HI_U16  u16HlCnt;
} ISP_AF_ZONE_S;
typedef struct hiISP_AF_STAT_S
{
    ISP_AF_ZONE_S stZoneMetrics[AF_ZONE_ROW][AF_ZONE_COLUMN]; /*RO, The zoned measure of contrast*/
} ISP_AF_STAT_S;

typedef struct hiISP_AF_INFO_S
{
    HI_U32  u32FrameCnt;
    
    ISP_AF_STAT_S   *pstAfStat;
} ISP_AF_INFO_S;

/* the final calculate of af alg */
typedef struct hiISP_AF_RESULT_S
{
    HI_S32 s32Rsv;
} ISP_AF_RESULT_S;

typedef struct hiISP_AF_EXP_FUNC_S
{
    HI_S32 (*pfn_af_init)(HI_S32 s32Handle, const ISP_AF_PARAM_S *pstAfParam);
    HI_S32 (*pfn_af_run)(HI_S32 s32Handle,
        const ISP_AF_INFO_S *pstAfInfo,
        ISP_AF_RESULT_S *pstAfResult,
        HI_S32 s32Rsv
        );
    HI_S32 (*pfn_af_ctrl)(HI_S32 s32Handle, HI_U32 u32Cmd, HI_VOID *pValue);
    HI_S32 (*pfn_af_exit)(HI_S32 s32Handle);
} ISP_AF_EXP_FUNC_S;

typedef struct hiISP_AF_REGISTER_S
{    
    ISP_AF_EXP_FUNC_S stAfExpFunc;
} ISP_AF_REGISTER_S;

#define ALG_LIB_NAME_SIZE_MAX   (20)
typedef struct hiALG_LIB_S
{
    HI_S32  s32Id;
    HI_CHAR acLibName[ALG_LIB_NAME_SIZE_MAX];
} ALG_LIB_S;

typedef struct hiISP_BIND_ATTR_S
{
    SENSOR_ID   SensorId;
    ALG_LIB_S   stAeLib;
    ALG_LIB_S   stAfLib;
    ALG_LIB_S   stAwbLib;
} ISP_BIND_ATTR_S;

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */

#endif /*__HI_COMM_SNS_H__ */

