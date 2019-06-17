/******************************************************************************

  Copyright (C), 2001-2011, Hisilicon Tech. Co., Ltd.

 ******************************************************************************
  File Name     : hi_comm_isp.h
  Version       : Initial Draft
  Author        : Hisilicon multimedia software group
  Created       : 2010/12/20
  Description   : 
  History       :
  1.Date        : 2010/12/20
    Author      : x00100808
    Modification: Created file

  2.Date         : 2014/04/24
    Author       : y00246723
    Modification : Add ACM structure define

******************************************************************************/

#ifndef __HI_COMM_ISP_H__
#define __HI_COMM_ISP_H__

#include "hi_type.h"
#include "hi_errno.h"
#include "hi_common.h"
#include "hi_isp_debug.h"

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* End of #ifdef __cplusplus */


/****************************************************************************
 * MACRO DEFINITION                                                         *
 ****************************************************************************/

#define RES_WIDTH_MAX               (4608)
#define RES_WIDTH_MIN               (480)
#define RES_HEIGHT_MAX              (4608)
#define RES_HEIGHT_MIN              (240)
#define FRAME_RATE_MAX              (65535.0)

#define VREG_MAX_NUM                (16)

#define AE_ZONE_ROW                 (15)
#define AE_ZONE_COLUMN              (17)
#define AWB_ZONE_ROW                (32)
#define AWB_ZONE_COLUMN             (32)
#define AWB_ZONE_ROW_STITCH         (16)
#define AWB_ZONE_COLUMN_STITCH      (64)
#define AWB_ZONE_NUM                (AWB_ZONE_ROW * AWB_ZONE_COLUMN) 
#define AF_ZONE_ROW                 (15)
#define AF_ZONE_COLUMN              (17)
#define LIGHTSOURCE_NUM             (4)

#define DRC_TM_NODE_NUM             (65)
#define STATIC_DP_COUNT_MAX         (4095)
#define STATIC_DP_CAL_TIME_OUT      (0x640)
#define GAMMA_NODE_NUM              (257)
#define GAMMA_FE0_NODE_NUM          (33)
#define GAMMA_FE1_NODE_NUM          (257)

#define NP_NODE_NUM_MAX             (128)

#define SHADING_NODE_NUM_MAX        (129)
#define ISP_MESH_SHADING_NODE_NUM   (64*64)
#define ISP_MESH_SHADING_PLANE_NUM  (3)
#define ISP_MESH_LS_NUM_MAX         (4)

#define DEFOG_ZONE_ROW       (32)
#define DEFOG_ZONE_COLUMN    (32)
#define DEFOG_ZONE_NUM       (1024)
#define DEFOG_MAX_ZONE_NUM   (1024)
#define DEFOG_ZONE_ROW_STITCH (32)
#define DEFOG_ZONE_COL_STITCH (16)



#define ISP_AUTO_STENGTH_NUM (16)

#define ISP_MAX_SNS_REGS     (32)
#define ISP_MAX_DEV_NUM      (2)
#define ISP_MAX_UPDATEINFO_BUF_NUM  (2)
#define ISP_MAX_FRAMEINFO_BUF_NUM  (2)

#define ISP_REG_SIZE_BIN        (0x50000)
#define ISP_VREG_SIZE_BIN       (0x3000)
#define ALG_LIB_VREG_SIZE_BIN   (0x1000)

#define AI_MAX_STEP_FNO_NUM  (1024)

#define PRO_MAX_FRAME_NUM    (8)

#define ISP_BAYER_CHN_NUM   (4)
#define CCM_MATRIX_SIZE     (9)

/****************************************************************************
 * GENERAL STRUCTURES                                                       *
 ****************************************************************************/

typedef enum hiISP_ERR_CODE_E
{
    ERR_ISP_NOT_INIT                = 0x40,
    ERR_ISP_MEM_NOT_INIT            = 0x41,
    ERR_ISP_ATTR_NOT_CFG            = 0x42,
    ERR_ISP_SNS_UNREGISTER          = 0x43,
    ERR_ISP_INVALID_ADDR            = 0x44,
    ERR_ISP_NOMEM                   = 0x45,
    ERR_ISP_NO_INT                  = 0x46,
} ISP_ERR_CODE_E;

#define HI_ERR_ISP_NULL_PTR             HI_DEF_ERR(HI_ID_ISP, EN_ERR_LEVEL_ERROR, EN_ERR_NULL_PTR)
#define HI_ERR_ISP_ILLEGAL_PARAM        HI_DEF_ERR(HI_ID_ISP, EN_ERR_LEVEL_ERROR, EN_ERR_ILLEGAL_PARAM)
#define HI_ERR_ISP_NOT_SUPPORT          HI_DEF_ERR(HI_ID_ISP, EN_ERR_LEVEL_ERROR, EN_ERR_NOT_SUPPORT)

#define HI_ERR_ISP_NOT_INIT             HI_DEF_ERR(HI_ID_ISP, EN_ERR_LEVEL_ERROR, ERR_ISP_NOT_INIT)
#define HI_ERR_ISP_MEM_NOT_INIT         HI_DEF_ERR(HI_ID_ISP, EN_ERR_LEVEL_ERROR, ERR_ISP_MEM_NOT_INIT)
#define HI_ERR_ISP_ATTR_NOT_CFG         HI_DEF_ERR(HI_ID_ISP, EN_ERR_LEVEL_ERROR, ERR_ISP_ATTR_NOT_CFG)
#define HI_ERR_ISP_SNS_UNREGISTER       HI_DEF_ERR(HI_ID_ISP, EN_ERR_LEVEL_ERROR, ERR_ISP_SNS_UNREGISTER)
#define HI_ERR_ISP_INVALID_ADDR         HI_DEF_ERR(HI_ID_ISP, EN_ERR_LEVEL_ERROR, ERR_ISP_INVALID_ADDR)
#define HI_ERR_ISP_NOMEM                HI_DEF_ERR(HI_ID_ISP, EN_ERR_LEVEL_ERROR, ERR_ISP_NOMEM)
#define HI_ERR_ISP_NO_INT               HI_DEF_ERR(HI_ID_ISP, EN_ERR_LEVEL_ERROR, ERR_ISP_NO_INT)

typedef struct hiISP_MOD_PARAM_S
{
    HI_U32      proc_param;         /* RW */
    HI_U32      u32StatIntvl;       /* RW; Range:[0x1, 0xFFFFFFFF] */
    HI_U32      u32UpdatePos;       /* RW */
    HI_U32      u32IntBottomHalf;   /* RO */
    HI_U32      u32IntTimeOut;      /* RW */
    HI_U32      u32PwmNum;          /* RO */
}ISP_MOD_PARAM_S;

typedef enum hiISP_OP_TYPE_E
{
    OP_TYPE_AUTO    = 0,
    OP_TYPE_MANUAL  = 1,
    OP_TYPE_BUTT
} ISP_OP_TYPE_E;

typedef enum hiISP_MODE_E
{
    ISP_MODE_NORMAL = 0,
    ISP_MODE_STITCH,
    ISP_MODE_BUTT
} ISP_MODE_E;

typedef enum hiISP_STATE_E
{
    ISP_STATE_INIT     = 0, 
    ISP_STATE_SUCCESS  = 1,
    ISP_STATE_TIMEOUT  = 2,
    ISP_STATE_BUTT
} ISP_STATUS_E;

typedef struct hiISP_PIPE_DIFF_ATTR_S
{
    HI_S32 as32Offset[ISP_BAYER_CHN_NUM]; 
    HI_U32 au32Gain[ISP_BAYER_CHN_NUM];
    HI_U16 au16ColorMatrix[CCM_MATRIX_SIZE];
} ISP_PIPE_DIFF_ATTR_S;

typedef enum hiISP_BAYER_FORMAT_E
{
    BAYER_RGGB    = 0,
    BAYER_GRBG    = 1,
    BAYER_GBRG    = 2,
    BAYER_BGGR    = 3,
    BAYER_BUTT    
} ISP_BAYER_FORMAT_E;
typedef struct hiISP_PUB_ATTR_S
{
    RECT_S          stWndRect;      /* RW. */
    SIZE_S          stSnsSize;
    HI_FLOAT        f32FrameRate;   /* RW. */
    ISP_BAYER_FORMAT_E  enBayer;    /* RW. */
} ISP_PUB_ATTR_S;

/* Slave mode sensor sync signal generate module */
typedef struct hiISP_SLAVE_SNS_SYNC_S 
{
    union 
    {
        struct 
        {
            HI_U32  bit16HsDlyCyc     :  16;
            HI_U32  bitHInv           :  1;
            HI_U32  bitVInv           :  1;
            HI_U32  bit12Rsv          :  12;
            HI_U32  bitHEnable        :  1;
            HI_U32  bitVEnable        :  1;          
        } stBits;   
        HI_U32 u32Bytes; 
    } unCfg;

    HI_U32  u32VsTime;
    HI_U32  u32HsTime;
    HI_U32  u32VsCyc;
    HI_U32  u32HsCyc;    
}ISP_SLAVE_SNS_SYNC_S;

typedef enum hiISP_FMW_STATE_E
{
    ISP_FMW_STATE_RUN = 0,
    ISP_FMW_STATE_FREEZE,
    ISP_FMW_STATE_BUTT
} ISP_FMW_STATE_E;

typedef struct hiISP_WDR_MODE_S
{
    WDR_MODE_E  enWDRMode;
} ISP_WDR_MODE_S;

typedef union hiISP_MODULE_CTRL_U
{
    HI_U32  u32Key;
    struct
    {
        HI_U32  bitGammaFePosition      : 1 ;   /* [0] */
        HI_U32  bitISPDGainPosition     : 1 ;   /* [1] */
        HI_U32  bitBypassInputFormatter : 1 ;   /* [2] */
        HI_U32  bitChnSwitch            : 1 ;   /* [3] */
        HI_U32  bitBypassVideoTest      : 1 ;   /* [4] */
        HI_U32  bitBypassCrosstalkR     : 1 ;   /* [5] */
        HI_U32  bitBypassDPC            : 1 ;   /* [6] */
        HI_U32  bitBypassFSWDR          : 1 ;   /* [7] */
        HI_U32  bitBypassGammaFe        : 1 ;   /* [8] */
        HI_U32  bitBypassGammaFeDl      : 1 ;   /* [9] */
        HI_U32  bitBypassISPDGain       : 1 ;   /* [10] */
        HI_U32  bitFSChnSwitch          : 1 ;   /* [11] */
        HI_U32  bitBypassNR             : 1 ;   /* [12] */
        HI_U32  bitBypassGammaBe        : 1 ;   /* [13] */
        HI_U32  bitBypassGammaBeDl      : 1 ;   /* [14] */
        HI_U32  bitBypassWBGain         : 1 ;   /* [15] */
        HI_U32  bitBypassRadialShading  : 1 ;   /* [16] */
        HI_U32  bitBypassMeshShading    : 1 ;   /* [17] */
        HI_U32  bitBypassDRC            : 1 ;   /* [18] */
        HI_U32  bitBypassDemosaic       : 1 ;   /* [19] */
        HI_U32  bitBypassColorMatrix    : 1 ;   /* [20] */
        HI_U32  bitBypassGamma          : 1 ;   /* [21] */
        HI_U32  bit4Rsv1                : 4 ;   /* [22:25] */
        HI_U32  bitBypassCsConv         : 1 ;   /* [26] */
        HI_U32  bitBypassBalanceFe      : 1 ;   /* [27] */
        HI_U32  bitBypassFSBalanceFe    : 1 ;   /* [28] */
        HI_U32  bitBypassWDRGain        : 1 ;   /* [29] */
        HI_U32  bit2ChnSelect           : 2 ;   /* [30:31] */
    };
}ISP_MODULE_CTRL_U;


/****************** InputFormatter *******************/
#define IF_KNEE_POINT_NUM       (3)
#define IF_SLOPE_NUM            (4)

typedef struct hiISP_INPUT_FORMATTER_ATTR_S
{
    HI_U16 au16KneePoint[IF_KNEE_POINT_NUM];           /* RW, Built-In WDR sensor knee point, Range:[0x0, 0xFFFF] */   
    HI_U8  au8SlopeSelect[IF_SLOPE_NUM];               /* RW, Built-In WDR sensor compress curve slope select, Range:[0x0, 0xF] */  
} ISP_INPUT_FORMATTER_ATTR_S;


/****************** FSWDR *******************/
typedef enum hiISP_COMBINE_MODE_E
{
    FS_WDR_COMBINE_SHORT_FIRST = 0,                            /* Short exposure data is used when combining                                                           */
    FS_WDR_COMBINE_LONG_FIRST = 1,                             /* Long exposure data is used when combining                                                            */
    FS_WDR_COMBINE_BUTT                                        
} ISP_COMBINE_MODE_E;

typedef struct hiISP_WDR_FS_ATTR_S
{
    HI_BOOL bShortMotionComp;                                 /* RW, HI_TRUE: enable motion compensation; HI_FLASE: disable motion compensation                        */
    HI_U16  u16ShortShortThresh;                              /* RW, Range: [0x0, 0xFFF]. Data above this threshold will be taken from short exposure only...
                                                                 ...This value is normally not changed and should be left at the default value                         */
    HI_U16  u16ShortLongThresh;                               /* RW, Range: [0x0, u16ShortShortThresh]. Data below this threshold will be taken from long exposure only....
                                                                 ...This value is normally not changed and should be left at the default value                         */
    ISP_COMBINE_MODE_E enShortComMode;

    HI_BOOL bMedMotionComp;                                   /* RW, HI_TRUE: enable motion compensation; HI_FLASE: disable motion compensation                        */
    HI_U16  u16MedShortThresh;                                /* RW, Range: [0x0, 0xFFF]. Data above this threshold will be taken from short exposure only...
                                                                 ...This value is normally not changed and should be left at the default value                         */
    HI_U16  u16MedLongThresh;                                 /* RW, Range: [0x0, u16MedShortThresh]. Data below this threshold will be taken from long exposure only...
                                                                 ...This value is normally not changed and should be left at the default value                         */
    ISP_COMBINE_MODE_E enMedComMode;
    
    HI_BOOL bLongMotionComp;                                   /* RW, HI_TRUE: enable motion compensation; HI_FLASE: disable motion compensation                       */
    HI_U16  u16LongShortThresh;                                /* RW, Range: [0x0, 0xFFF]. Data above this threshold will be taken from short exposure only...
                                                                  ...This value is normally not changed and should be left at the default value                        */
    HI_U16  u16LongLongThresh;                                 /* RW, Range: [0x0, u16LongShortThresh]. Data below this threshold will be taken from long exposure only...
                                                                  ...This value is normally not changed and should be left at the default value                        */
    ISP_COMBINE_MODE_E enLongComMode;                        
} ISP_WDR_FS_ATTR_S;


/****************** DRC *******************/
typedef struct hiISP_DRC_MANUAL_ATTR_S
{
    HI_U32  u32Strength;          /* RW, Range: [0, 0x3FF]. Strength of dynamic range compression. 
                                     Higher values lead to higher differential gain between shadows and highlights. */
} ISP_DRC_MANUAL_ATTR_S;

typedef struct hiISP_DRC_AUTO_ATTR_S
{
    HI_U32  u32Strength;          /* RW, Range: [0, 0x3FF]. It is the base strength. The strength used in ISP is generated by firmware. 
                                        In linear mode, strength = f1(u32Strength, histogram, ISO)
                                        In sensor WDR mode: strength = f2(u32Strength, histogram, ISO)
                                        In FSWDR mode: strength = f3(ExpRatio, ISO) */
} ISP_DRC_AUTO_ATTR_S;

typedef enum hiISP_DRC_CURVE_TYPE_E
{
    DRC_CURVE_TYPE  = 0,
    DRC_CURVE_TYPE_EX  = 1,
    DRC_CURVE_TYPE_BUTT
} ISP_DRC_CURVE_TYPE_E;
/* Old DRC fine curve control */
typedef struct hiISP_DRC_CURVE_S 
{
    HI_U32  u32SlopeMax;          /* RW, Range: [0, 0xFF].  Restricts the maximum slope (gain). */
    HI_U32  u32SlopeMin;          /* RW, Range: [0, 0xFF].  Restricts the minimum slope (gain). */
    HI_U32  u32VarianceSpace;     /* RW, Range: [0, 0xF].   Degree of spatial sensitivity of the DRC algorithm*/
    HI_U32  u32VarianceIntensity; /* RW, Range: [0, 0xF].   Degree of intensity sensitivity of the DRC algorithm */
}ISP_DRC_CURVE_S;	
/* New DRC fine curve control */
typedef struct hiISP_DRC_CURVE_EX_S 
{
	HI_U32  u32Contrast;		  /* RW, Range: [0, 0xFF]. */
	HI_U32  u32BrightPr;	  	  /* RW, Range: [0, 0xFF]. */
	HI_U32  u32Svariance;		  /* RW, Range: [0, 0xF].  */
	HI_U32  u32DarkEnhance;		  /* RW, Range: [0, 0xFFFF]. */
}ISP_DRC_CURVE_EX_S;

typedef struct hiISP_DRC_ATTR_S
{
    HI_BOOL bEnable;
    HI_U32  u32WhiteLevel;        		            /* RW, Range: [0, 0xFFFFF]. Values above this are not processed and remain linear. */
    HI_U32  u32BlackLevel;        		            /* RW, Range: [0, 0xFFFFF]. Values below this are not processed and remain linear. */	

    HI_BOOL bUserTmEnable;                          /* RW, bUserTmEnable = 0: DRC tone curve is generated by u32Asymmetry and u32BrightEnhance; 
                                                                bUserTmEnable = 1: DRC tone curve is defined by au32ToneMappingValue.
                                                                Default value: 0. */
    
    HI_U32  u32Asymmetry;         		            /* RW, Range: [0, 0xFF].  The parameter0 of DRC tone curve. Default value: 0x8. */
    HI_U32  u32BrightEnhance;     		            /* RW, Range: [0, 0xFF].  The parameter1 of DRC tone curve. Default value: 0xE6. */	

    HI_U32  au32ToneMappingValue[DRC_TM_NODE_NUM];  /*RW, Range: [0x0, 0xFFFFF] */
    
	ISP_DRC_CURVE_TYPE_E FilterMux;	  	            /* RW,  Set FilterMux = 1 is New DRC curve shapes restrict, else Old. */
	ISP_DRC_CURVE_S  stDrcCurve;
	ISP_DRC_CURVE_EX_S stDrcCurveEx;
	
    ISP_OP_TYPE_E enOpType;
	ISP_DRC_MANUAL_ATTR_S stManual;
	ISP_DRC_AUTO_ATTR_S   stAuto;
} ISP_DRC_ATTR_S;


typedef enum hiISP_STATIC_DP_TYPE_E{
    ISP_STATIC_DP_BRIGHT = 0x0,  
    ISP_STATIC_DP_DARK,    
    ISP_STATIC_DP_BUTT
} ISP_STATIC_DP_TYPE_E;

typedef struct hiISP_DP_STATIC_CALIBRATE_S
{
    HI_BOOL bEnable;              /* RW. Enable/disable the static defect-pixel module.Default Value:1(0x1). */
    HI_BOOL bEnableDetect;        /* RW. Set 'HI_TRUE'to start static defect-pixel calibration, and firmware will set 'HI_FALSE' when finished. */
    
    ISP_STATIC_DP_TYPE_E enStaticDPType;    /* Select static bright/dark defect-pixel calibration. */
    HI_U8   u8StartThresh;        /* RW, Range: [1, 0xFF]. Start threshold for static defect-pixel calibraiton.Default Value:31(0x1f).. */
    HI_U16  u16CountMax;          /* RW, Range: [0, 0xFFF]. Limit of max number of static defect-pixel calibraiton.Default Value:256(0x100). */
    HI_U16  u16CountMin;          /* RW, Range: [0, u16CountMax]. Limit of min number of static defect-pixel calibraiton.Default Value:64(0x40). */
    HI_U16  u16TimeLimit;         /* RW, Range: [0x0, 0x640].Time limit for static defect-pixel calibraiton, in frame number.Default Value:1600(0x640). */

    HI_U32      u32Table[STATIC_DP_COUNT_MAX];  /* RO, Static defect-pixel calibraiton table,the first 13 bits represents the X coordinate of the defect pixel, the second 13 bits represent the Y coordinate of the defect pixel. */
    HI_U8       u8FinishThresh;   /* RO, Range: [0, 0xFF]. Finish threshold for static defect-pixel calibraiton. */    
    HI_U16      u16Count;         /* RO, Range: [0, 0xFFF]. Finish number for static defect-pixel calibraiton. */
    ISP_STATUS_E enStatus;        /* RO, Status of static defect-pixel calibraiton.Default Value:0(0x0). */
} ISP_DP_STATIC_CALIBRATE_S;

typedef struct hiISP_DP_DYNAMIC_ATTR_S
{
    HI_BOOL bEnable;
    
    HI_U16  u16Slope;             /* RW, Range: [0x0, 0xFFF] */
    HI_U16  u16Thresh;            /* RW, Range: [0x0, 0xFFF] */
    HI_U16  u16LineThresh;        /* RW, Range: [0x0, 0xFFF] */
    HI_U16  u16HpThresh;          /* RW, Range: [0x0, 0xFFF] */  
    HI_U8   u8BlendRatio;         /* RW, Range: [0x0, 0xFF] */
} ISP_DP_DYNAMIC_ATTR_S;

typedef struct hiISP_DP_STATIC_ATTR_S
{
    HI_BOOL bEnable;
    HI_U16  u16BrightCount;       /* RW, When used as input(W), indicate the number of static bright defect pixels; As output(R),indicate the number of static bright and dark defect pixels */
    HI_U16  u16DarkCount;         /* RW, When used as input(W), indicate the number of static dark defect pixels; As output(R), invalid value 0 */    
    HI_U32  au32BrightTable[STATIC_DP_COUNT_MAX];   /* RW, Range: [0x0, 0x3FFFFFF],the first 13 bits represents the X coordinate of the defect pixel, the second 13 bits represent the Y coordinate of the defect pixel
                                                                    Notice : When used as input(W), indicate static bright defect pixels table;  As output(R), indicate static bright and dark defect pixels table */
    HI_U32  au32DarkTable[STATIC_DP_COUNT_MAX];     /* RW, Range: [0x0, 0x3FFFFFF],the first 13 bits represents the X coordinate of the defect pixel, the second 13 bits represent the Y coordinate of the defect pixel
                                                                    Notice : When used as input(W), indicate static dark defect pixels table;  As output(R), invalid value */
    HI_BOOL bShow;
} ISP_DP_STATIC_ATTR_S;

typedef struct hiISP_DP_ATTR_S
{
    ISP_DP_DYNAMIC_ATTR_S stDynamicAttr;    
    ISP_DP_STATIC_ATTR_S  stStaticAttr;
} ISP_DP_ATTR_S;

typedef struct hiISP_DIS_ATTR_S
{
    HI_BOOL bEnable;
} ISP_DIS_ATTR_S;

typedef struct hiISP_RADIAL_SHADING_ATTR_S
{
    HI_BOOL              bEnable;
    HI_U16               au16OffCenter[ISP_MESH_SHADING_PLANE_NUM];                  /* RW, Range: [0x0, 0xFFFF] */
    POINT_S              astCenter[ISP_MESH_SHADING_PLANE_NUM];                      /* RW, Range: [0x0, 0xFFFF] */
    HI_U32               au32Table[ISP_MESH_SHADING_PLANE_NUM][SHADING_NODE_NUM_MAX];/* RW, Range: [0x0, 0xFFFF] */
    HI_U16               u16RadialNodeNum;                                           /* RW, Range: [0x0, SHADING_NODE_NUM_MAX] */
} ISP_RADIAL_SHADING_ATTR_S;

typedef enum hiISP_MESH_MODE_TYPE_E
{
    ISP_MESH_MODE_TYPE_1LS  = 0,
    ISP_MESH_MODE_TYPE_2LS  = 1,
    ISP_MESH_MODE_TYPE_4LS  = 2,

    ISP_MESH_MODE_TYPE_BUTT
}ISP_MESH_MODE_TYPE_E;

typedef struct hiISP_MESH_SHADING_ATTR_S
{
    HI_BOOL              bEnable;
    ISP_MESH_MODE_TYPE_E enMeshLSMode;      /* RW, Range: [0x0, 0x2] Select the number of light source illuminants 0:1LS 1:2LS 2:4LS*/
    HI_U8                u8MeshScale;       /* RW, Range: [0x0, 0x7] Selects the scale and precision for the 8 bit gain coefficients stored in mesh tables */
    HI_U16               u16MeshStrength;   /* RW, Range: [0x0, 0xFFFF] Mesh strength adjustment in 4.12 format.*/
    HI_U32               au32MeshColorTemp[ISP_MESH_LS_NUM_MAX]; /* RW, Range: [0x0, 0xFFFFFFFF] color temperature corresponding to difference light souce .*/
    HI_U8                au8MeshTable[ISP_MESH_SHADING_PLANE_NUM][ISP_MESH_SHADING_NODE_NUM];/* RW, Range: [0x0, 0xFF] Mesh Shading correction LUT*/
} ISP_MESH_SHADING_ATTR_S;

typedef struct hiISP_NR_MANUAL_ATTR_S
{
    HI_U8 u8Thresh;                            /* RW, Range: [0x0, 0xFF]. Noise reduction effect for high spacial frequencies. */
    HI_U8 u8ThreshLong;                        /* RW, Range: [0x0, 0xFF]. Noise reduction effect for high spacial frequencies. */
} ISP_NR_MANUAL_ATTR_S;
typedef struct hiISP_NR_AUTO_ATTR_S
{
    HI_U8 au8Thresh[ISP_AUTO_STENGTH_NUM];     /* RW, Range: [0x0, 0xFF]. Noise reduction target value array for different ISO. */
} ISP_NR_AUTO_ATTR_S;
typedef struct hiISP_NR_ATTR_S
{
    HI_BOOL bEnable;
    ISP_OP_TYPE_E enOpType;
    ISP_NR_MANUAL_ATTR_S stManual;
    ISP_NR_AUTO_ATTR_S   stAuto;
} ISP_NR_ATTR_S;

/* Noise profile table for ISP 2DNR */
typedef struct hiISP_NP_TABLE_S
{
    HI_U8 au8NPTable[NP_NODE_NUM_MAX];
} ISP_NP_TABLE_S;

typedef enum hiISP_GAMMA_CURVE_TYPE_E
{
    ISP_GAMMA_CURVE_DEFAULT = 0x0,  /* default Gamma curve */
    ISP_GAMMA_CURVE_SRGB,
    ISP_GAMMA_CURVE_USER_DEFINE,    /* user defined Gamma curve, Gamma Table must be correct */
    ISP_GAMMA_CURVE_BUTT
} ISP_GAMMA_CURVE_TYPE_E;
typedef struct hiISP_GAMMA_ATTR_S
{
    HI_BOOL bEnable;

    ISP_GAMMA_CURVE_TYPE_E enCurveType;
    HI_U16 u16Table[GAMMA_NODE_NUM];
} ISP_GAMMA_ATTR_S;

typedef struct hiISP_GAMMAFE_ATTR_S
{
    HI_BOOL bEnable;

    HI_U32 u32Table[GAMMA_FE0_NODE_NUM + GAMMA_FE1_NODE_NUM]; /* only for WDR sensor mode */
} ISP_GAMMAFE_ATTR_S;


/******** Bayer domin sharpen ********/
typedef enum hiISP_SHARPEN_TYPE_E
{
    SHARPEN_TYPE  = 0,
    SHARPEN_TYPE_EX  = 1,
    SHARPEN_TYPE_BUTT
} ISP_SHARPEN_TYPE_E;
/* Sharpen */
typedef struct hiISP_BAYER_SHARPEN_S
{
    HI_U8  u8SharpenD;       	/* RW,  Range:[0, 0xFF]. */
    HI_U8  u8SharpenUd;      	/* RW,  Range:[0, 0xFF]. */
    HI_U16 u16LumaThd;          /* RW,  Range:[0, 0xFFF]. */	
}ISP_BAYER_SHARPEN_S;
/* SharpenEx */	
typedef struct hiISP_BAYER_SHARPEN_EX_S
{
	HI_U8 u8SharpenHF;			/* RW,  Range:[0, 0xFF]. Strength parameter for high frequency components.*/
	HI_U8 u8SharpenMF;			/* RW,  Range:[0, 0xFF]. Strength parameter for medium frequency components.*/
	HI_U8 u8SharpenLF;			/* RW,  Range:[0, 0xFF]. Strength parameter for low frequency components.*/
	HI_U8 u8SadAmplifier;		/* RW,  Range:[0, 0xFF]. Amplifier for sad.*/
}ISP_BAYER_SHARPEN_EX_S;
	
typedef struct hiISP_BAYER_SHARPEN_MANUAL_ATTR_S
{
	ISP_BAYER_SHARPEN_S stShp;
	ISP_BAYER_SHARPEN_EX_S stShpEx;
} ISP_BAYER_SHARPEN_MANUAL_ATTR_S;


typedef struct hiISP_BAYER_SHARPEN_AUTO_ATTR_S
{
	ISP_BAYER_SHARPEN_S astShp[ISP_AUTO_STENGTH_NUM];
	ISP_BAYER_SHARPEN_EX_S astShpEx[ISP_AUTO_STENGTH_NUM];	
} ISP_BAYER_SHARPEN_AUTO_ATTR_S;

typedef struct hiISP_BAYER_SHARPEN_ATTR_S
{
    HI_BOOL bEnable;
    ISP_SHARPEN_TYPE_E enShpAlgSel;   		/* RW, Set enShpAlgSel = 1 is Sharpen, else SharpenEx. */		
    ISP_OP_TYPE_E enOpType;

    ISP_BAYER_SHARPEN_MANUAL_ATTR_S stManual;
    ISP_BAYER_SHARPEN_AUTO_ATTR_S   stAuto;
} ISP_BAYER_SHARPEN_ATTR_S;


/***** YUV domin sharpen *****/
#define ISP_YUV_SHPLUMA_NUM  (4)

typedef struct hiISP_YUV_SHARPEN_MANUAL_ATTR_S
{
    HI_BOOL bEnLowLumaShoot;
    HI_U8 u8TextureSt;    	 	 /* RW,  Sharpen strength of detail, range [0, 0xFF], equal to maxEdgeAmt in fw. */
    HI_U8 u8EdgeSt;		 		 /* RW,  Sharpen strength of Edge, range [0, 0xFF], equal to maxSharpAmt1 in fw */     
    HI_U8 u8OverShoot;		 	 /* RW,  Range:[0, 0xFF]. */	
    HI_U8 u8UnderShoot;		 	 /* RW,  Range:[0, 0xFF]. */	
    HI_U8 u8TextureThd;	 		 /* RW,  Range:[0, 0xFF]. */	
    HI_U8 u8EdgeThd;		 	 /* RW,  Range:[0, 0xFF]. */	
    HI_U8 u8JagCtrl;		 	 /* RW,  Range:[0, 0x3F]. Jag control strength. set to "0" is off */	
    HI_U8 u8SaltCtrl;			 /* RW,  Range:[0, 0xFF]. */
    HI_U8 u8PepperCtrl;			 /* RW,  Range:[0, 0xFF]. */
    HI_U8 u8DetailCtrl;			 /* RW,  Range:[0, 0xFF]. */

    HI_U8 au8LumaThd[ISP_YUV_SHPLUMA_NUM];    	 /* RW,  Range:[0, 0xFF]. */	
    HI_U8 au8LumaWgt[ISP_YUV_SHPLUMA_NUM];   	 /* RW,  Range:[0, 0xFF]. */	
} ISP_YUV_SHARPEN_MANUAL_ATTR_S;

typedef struct hiISP_YUV_SHARPEN_AUTO_ATTR_S
{
    HI_BOOL abEnLowLumaShoot[ISP_AUTO_STENGTH_NUM];
    HI_U8 au8TextureSt[ISP_AUTO_STENGTH_NUM];  		 /* RW,  Sharpen strength of detail, range [0, 0xFF], equal to maxEdgeAmt in fw. */
    HI_U8 au8EdgeSt[ISP_AUTO_STENGTH_NUM];  		 /* RW,  Sharpen strength of Edge, range [0, 0xFF], equal to maxSharpAmt1 in fw */ 
    HI_U8 au8OverShoot[ISP_AUTO_STENGTH_NUM];		 /* RW,  Range:[0, 0xFF]. */	
    HI_U8 au8UnderShoot[ISP_AUTO_STENGTH_NUM];		 /* RW,  Range:[0, 0xFF]. */	
    HI_U8 au8TextureThd[ISP_AUTO_STENGTH_NUM];		 /* RW,  Range:[0, 0xFF]. */	
    HI_U8 au8EdgeThd[ISP_AUTO_STENGTH_NUM];	 		 /* RW,  Range:[0, 0xFF]. */
    HI_U8 au8JagCtrl[ISP_AUTO_STENGTH_NUM];		 	 /* RW,  Range:[0, 0x3F]. set to '0' is off */	
    HI_U8 au8SaltCtrl[ISP_AUTO_STENGTH_NUM];		 /* RW,  Range:[0, 0xFF]. */
    HI_U8 au8PepperCtrl[ISP_AUTO_STENGTH_NUM];		 /* RW,  Range:[0, 0xFF]. */
    HI_U8 au8DetailCtrl[ISP_AUTO_STENGTH_NUM];		 /* RW,  Range:[0, 0xFF]. */

    HI_U8 au8LumaThd[ISP_YUV_SHPLUMA_NUM][ISP_AUTO_STENGTH_NUM];  	 /* RW,  Range:[0, 0xFF]. luma threshold */	
    HI_U8 au8LumaWgt[ISP_YUV_SHPLUMA_NUM][ISP_AUTO_STENGTH_NUM]; 	 /* RW,  Range:[0, 0xFF]. luma weight */		
} ISP_YUV_SHARPEN_AUTO_ATTR_S;

typedef struct hiISP_YUV_SHARPEN_ATTR_S
{
    HI_BOOL bEnable;
    ISP_OP_TYPE_E enOpType;
    ISP_YUV_SHARPEN_MANUAL_ATTR_S stManual;
    ISP_YUV_SHARPEN_AUTO_ATTR_S   stAuto;
} ISP_YUV_SHARPEN_ATTR_S;

/*Crosstalk Removal*/
typedef struct hiISP_CR_ATTR_S
{
    HI_BOOL  bEnable;
    HI_U8    au8Strength[ISP_AUTO_STENGTH_NUM];     /* RW, Range: [0x0, 0xFF]. This register is  not recommended  to change */
    HI_U8    u8Sensitivity;                         /* RW, Range: [0x0, 0xFF]. This register is  not recommended  to change */
    HI_U16   u16Threshold;                          /* RW, Range: [0x0, 0xFFF]. This register is  not recommended  to change */
    HI_U16   u16Slope;                              /* RW, Range: [0x0, 0xFFF]. This register is  not recommended  to change */
}ISP_CR_ATTR_S;

typedef struct hiISP_ANTI_FALSECOLOR_S
{
    HI_U8  u8Strength;      /* RW, Range: [0x0, 0xFF]. The recommended range is [0x0, 0x95], the normal 
                                    color will gradually be eroded when this register is larger than 0x95. */
} ISP_ANTI_FALSECOLOR_S;

typedef enum hiISP_DEMOSAIC_CFG_E
{
    ISP_DEMOSAIC_CFG_DEFAULT = 0,
    ISP_DEMOSAIC_CFG_VH,
    ISP_DEMOSAIC_CFG_AA,
    ISP_DEMOSAIC_CFG_VA,
    ISP_DEMOSAIC_CFG_UU,

    ISP_DEMOSAIC_CFG_BUTT,
} ISP_DEMOSAIC_CFG_E;
    
typedef struct hiISP_DEMOSAIC_ATTR_S
{
    HI_U8   u8VhSlope;              /* RW,Range: [0x0, 0xFF] */
    HI_U8   u8AaSlope;              /* RW,Range: [0x0, 0xFF] */
    HI_U8   u8VaSlope;              /* RW,Range: [0x0, 0xFF] */
    HI_U8   u8UuSlope;              /* RW,Range: [0x0, 0xFF] */
    HI_U16  u16VhThresh;            /* RW,Range: [0x0, 0xFFF] */   
    HI_U16  u16AaThresh;            /* RW,Range: [0x0, 0xFFF] */
    HI_U16  u16VaThresh;            /* RW,Range: [0x0, 0xFFF] */
    HI_U16  u16UuThresh;            /* RW,Range: [0x0, 0xFFF] */
    ISP_DEMOSAIC_CFG_E   enCfgType;             /* RW,Range: [0x0, 0xFF] */    
    HI_U8   au8NpOffset[ISP_AUTO_STENGTH_NUM];  /* RW, Range:[0x0, 0xFF] */
}ISP_DEMOSAIC_ATTR_S;

typedef struct hiISP_BLACK_LEVEL_S 
{ 
    HI_U16 au16BlackLevel[ISP_BAYER_CHN_NUM]; /* RW, Range: [0x0, 0xFFF]*/ 
} ISP_BLACK_LEVEL_S;

/* FPN */
typedef enum hiISP_FPN_TYPE_E
{
    ISP_FPN_TYPE_FRAME = 0, 
    ISP_FPN_TYPE_LINE = 1, 
    ISP_FPN_TYPE_BUTT       
}ISP_FPN_TYPE_E;

typedef struct hiISP_FPN_FRAME_INFO_S
{
    HI_U32              u32Iso;             /* FPN CALIBRATE ISO */
    HI_U32              u32Offset;          /* FPN frame u32Offset (agv pixel value) */    
    HI_U32              u32FrmSize;         /* FPN frame size (exactly frm size or compress len) */
    VIDEO_FRAME_INFO_S  stFpnFrame;         /* FPN frame info, 8bpp,10bpp,12bpp,16bpp. Compression or not */
}ISP_FPN_FRAME_INFO_S;

typedef struct hiISP_FPN_CALIBRATE_ATTR_S
{    
    HI_U32                          u32Threshold;        /* pix value > threshold means defective pixel, [1, 0xFFF] */
    HI_U32                          u32FrameNum;         /* value is 2^N, range: [1, 16] */
    ISP_FPN_TYPE_E                  enFpnType;           /* frame mode or line mode */
    ISP_FPN_FRAME_INFO_S            stFpnCaliFrame;
}ISP_FPN_CALIBRATE_ATTR_S;

typedef struct hiISP_FPN_MANUAL_ATTR_S
{
    HI_U32               u32Strength;         /* gain, RW fp 4.8 */
}ISP_FPN_MANUAL_ATTR_S;

typedef struct hiISP_FPN_AUTO_ATTR_S
{
    HI_U32               u32Strength;         /* gain, RO fp 4.8 */
}ISP_FPN_AUTO_ATTR_S;

typedef struct hiISP_FPN_ATTR_S
{
    HI_BOOL               bEnable;
    ISP_OP_TYPE_E         enOpType;           /* manual or auto mode */   
    ISP_FPN_TYPE_E        enFpnType;          /* frame mode or line mode */
    ISP_FPN_FRAME_INFO_S  stFpnFrmInfo;       /* input in correction mode. */
    ISP_FPN_MANUAL_ATTR_S stManual;
    ISP_FPN_AUTO_ATTR_S   stAuto;
}ISP_FPN_ATTR_S;

/* Shading Calibration */
typedef enum hiISP_BAYER_RAW_E
{
    BAYER_RAW_8BIT  = 8,
    BAYER_RAW_10BIT = 10,
    BAYER_RAW_12BIT = 12,
    BAYER_RAW_14BIT = 14,
    BAYER_RAW_16BIT = 16,
    BAYER_RAW_BUTT    
} ISP_BAYER_RAW_E; 

typedef struct hiISP_LSC_CALIBRATION_CFG_S 
{
    HI_BOOL bOffsetInEn;
    ISP_BAYER_FORMAT_E enBayer;   /*Bayer Pattern RGGB:0 GRBG:1 GBRG:2 BGGR:3*/
    ISP_BAYER_RAW_E    enRawBit;  /*Raw bit bandwidth,Range:[8,10,12,14,16]*/
	HI_U8  u8MeshScale;           /*Scale of Mesh Shading,Range:[0,7]*/ 
	HI_U8  u8EliminatePer;       /*used for eliminate dark and white point,Range:[0,49]*/

    HI_U8  u8GridSizeX;          /* gridSizeX,Range:[3,65]*/
    HI_U8  u8GridSizeY;          /* gridSizeY,Range:[3,65]*/
    HI_U16 u16ImageHeight;       
    HI_U16 u16ImageWidth;           

    HI_U16 u16BLCOffsetR;        /*Black level for R channel of 12bit,Range:[0,0xFFF)*/
    HI_U16 u16BLCOffsetGr;       /*Black level for Gr channel of 12bit,Range:[0,0xFFF)*/
    HI_U16 u16BLCOffsetGb;       /*Black level for Gb channel of 12bit,Range:[0,0xFFF)*/
    HI_U16 u16BLCOffsetB;        /*Black level for B channel of 12bit,Range:[0,0xFFF)*/
}ISP_LSC_CALIBRATION_CFG_S;



typedef struct hiISP_MESH_SHADING_TABLE_S
{
 /* 
    HI_U8 u8Rgain[(GridSizeX-1)*(GridSizeY-1)]
    HI_U8 u8Ggain[(GridSizeX-1)*(GridSizeY-1)]
    HI_U8 u8Bgain[(GridSizeX-1)*(GridSizeY-1)]
 */
	HI_U8 *pu8Rgain;    
	HI_U8 *pu8Ggain;
	HI_U8 *pu8Bgain;
}ISP_MESH_SHADING_TABLE_S;

/* AWB Calibration */
typedef struct hiISP_AWB_Calibration_Gain_S
{
	HI_U16 u16AvgRgain;    
	HI_U16 u16AvgBgain;
}ISP_AWB_Calibration_Gain_S;

/* DeFog */
typedef struct hiISP_DEFOG_MANUAL_ATTR_S
{
    HI_U8               u8strength;
}ISP_DEFOG_MANUAL_ATTR_S;

typedef struct hiISP_DEFOG_AUTO_ATTR_S
{
    HI_U8               u8strength;          
}ISP_DEFOG_AUTO_ATTR_S;
   
typedef struct hiISP_DEFOG_ATTR_S
{
    HI_BOOL            bEnable;
    ISP_OP_TYPE_E      enOpType;
    ISP_DEFOG_MANUAL_ATTR_S stManual;
    ISP_DEFOG_AUTO_ATTR_S   stAuto;
}ISP_DEFOG_ATTR_S;

/********************** ACM **************************************/

typedef struct hiISP_ACM_ATTR_S
{
    HI_BOOL         bEnable         ;
    HI_BOOL         bDemoEnable     ;/*0-debug closed; 1-debug open, the left screen is original video and the right screen is ACM-processed video*/
    HI_U32          u32Stretch      ; /*input data Clip range: 0-Y 64-940, C 64-960; 1-Y 0-1023, C 0-1023*/
    HI_U32          u32ClipRange    ; /*output data Clip range: 0-Y 64-940, C 64-960; 1-Y 0-1023, C 0-1023*/
    HI_U32          u32AcmClipOrWrap; /*0-wrap around; 1-clip*/
    HI_U32          u32CbcrThr      ; /* [0,255]*/
    HI_U32          u32GainLuma     ; /* [0,512]*/
    HI_U32          u32GainHue      ; /* [0,512]*/
    HI_U32          u32GainSat      ; /* [0,512]*/
}ISP_ACM_ATTR_S;

#define ACM_Y_NUM            9
#define ACM_H_NUM            29
#define ACM_S_NUM            13

typedef struct hiISP_ACM_LUT_S
{
    HI_S16 as16Y[ACM_Y_NUM][ACM_S_NUM][ACM_H_NUM];   /**<  */
    HI_S16 as16H[ACM_Y_NUM][ACM_S_NUM][ACM_H_NUM];   /**<  */
    HI_S16 as16S[ACM_Y_NUM][ACM_S_NUM][ACM_H_NUM];   /**<  */
}ISP_ACM_LUT_S;


typedef struct hiISP_INNER_STATE_INFO_S
{
    HI_U32 u32DRCStrengthActual;
    HI_U32 u32NRStrengthActual;
    HI_U32 u32SharpenStrengthDActual;
    HI_U32 u32SharpenStrengthUdActual;
    HI_U32 u32DefogStrengthActual;
    HI_U32 u32LongExpRatioActual;
    HI_U32 u32MedExpRatioActual;
    HI_U32 u32ShortExpRatioActual;

    HI_BOOL bWDRSwitchFinish;           /* HI_TRUE: WDR switch is finished*/
    HI_BOOL bResSwitchFinish;           /* HI_TRUE: Resolution switch is finished*/
} ISP_INNER_STATE_INFO_S;


typedef enum hiISP_AE_SWITCH_E
{
    ISP_AE_AFTER_STATIC_WB           = 0,
    ISP_AE_FROM_SENSOR_CHANNEL_1        ,
    ISP_AE_AFTER_SHADING                ,
    ISP_AE_AFTER_GAMMA_FE               ,  /* not support */
    ISP_AE_AFTER_DRC                    ,  /* not support */
    ISP_AE_FROM_SENSOR_CHANNEL_2        ,  /* not support */  
    ISP_AE_AFTER_WDR_STITCH             ,
    ISP_AE_AFTER_BLC_CHANNEL_2          ,  /* not support */
    ISP_AE_SWITCH_BUTT
} ISP_AE_SWITCH_E;


typedef enum hiISP_AE_HIST_SWITCH_E
{
    ISP_AE_HIST_SAME_AS_AE                = 0,
    ISP_AE_HIST_FROM_SENSOR_CHANNEL_1        ,   
    ISP_AE_HIST_AFTER_SHADING                ,
    ISP_AE_HIST_AFTER_GAMMA_FE               ,  /* not support */
    ISP_AE_HIST_AFTER_DRC                    ,  /* not support */
    ISP_AE_HIST_FROM_SENSOR_CHANNEL_2        ,  /* not support */
    ISP_AE_HIST_AFTER_WDR_STITCH             , 
    ISP_AE_HIST_AFTER_BLC_CHANNEL_2          ,   /* not support */
    ISP_AE_HIST_SWITCH_BUTT
} ISP_AE_HIST_SWITCH_E;

typedef enum hiISP_AE_PRE_HIST_SWITCH_E
{
    ISP_AE_PRE_HIST_ENABLE           = 0,
    ISP_AE_PRE_HIST_DISABLE             ,
    ISP_AE_PRE_HIST_BUTT
} ISP_AE_PRE_HIST_SWITCH_E;


typedef struct hiISP_AE_HIST_CONFIG_S
{
    HI_U8 u8HistSkipX;    /* Histogram decimation in horizontal direction: 0=every 2nd pixel; 1=every 3rd pixel;
                               2=every 4th pixel; 3=every 5th pixel; 4=every 8th pixel ; 5+=every 9th pixel */
    HI_U8 u8HistSkipY;    /* Histogram decimation in vertical direction: 0=every pixel; 1=every 2nd pixel; 2=every 
                               3rd pixel; 3=every 4th pixel; 4=every 5th pixel; 5=every 8th pixel ; 6+=every 9th pixel */
    HI_U8 u8HistOffsetX;  /* 0= start from the first column; 1=start from second column */
    HI_U8 u8HistOffsetY;  /* 0= start from the first row; 1= start from second row */
} ISP_AE_HIST_CONFIG_S;


/* config of statistics structs */
#define HIST_THRESH_NUM     (4)
typedef struct hiISP_AE_STATISTICS_CFG_S
{
    HI_U8  au8HistThresh[HIST_THRESH_NUM];    /*RW, Histogram threshold for bin 0/1 1/2 2/3 3/4 boundary, Range: [0x0, 0xFF]*/
    ISP_AE_SWITCH_E  enAESwitch;          /*RW, The position of Global 5 bins histogram and zone 5bins histogram in ISP pipeline*/ 
                                /* 0 = After static WB;   1 = Immediately from sensor, channel 1 (for WDR modes); */
                                /* 2 = After shading;      */
                                /* 6 = After WDR stitch;  */

    ISP_AE_HIST_SWITCH_E  enHistSwitch;        /*RW, The position of Global 1024 bins histogram in ISP pipeline*/
                                /* 0 = Same as AE;       1 = Immediately from sensor, channel 1 (for WDR modes); */
                                /* 2 = After shading;      */
                                /* 6 = After WDR stitch;  */                                

    ISP_AE_SWITCH_E  enAESumSwitch;       /*RW, The position of Global average and zone average in ISP pipeline*/
                                /* 0 = After static WB;   1 = Immediately from sensor, channel 1 (for WDR modes); */
                                /* 2 = After shading;     */
                                /* 6 = After WDR stitch;  */  

    ISP_AE_PRE_HIST_SWITCH_E  enPreHistSwitch;  /*RW, The position of Global 256 bins histogram in ISP pipeline of channel 2/3/4 for WDR mode */
                                /* 0 = Enabled (Immediately from sensor, channels 2, 3 and 4);   1 = Disabled; */

    ISP_AE_HIST_CONFIG_S  stHistConfig;
} ISP_AE_STATISTICS_CFG_S;

typedef struct hiISP_WB_STATISTICS_CFG_PARA_S
{
    HI_U16 u16WhiteLevel;       /*RW, Upper limit of valid data for white region, Range: [0x0, 0xFFF] for Bayer statistics, [0x0, 0x3FF] for RGB statistics*/
    HI_U16 u16BlackLevel;       /*RW, Lower limit of valid data for white region, Range: [0x0, u16WhiteLevel]. */
    HI_U16 u16CbMax;            /*RW, Maximum value of B/G for white region, Range: [0x0,0xFFF]*/
    HI_U16 u16CbMin;            /*RW, Minimum value of B/G for white region, Range: [0x0, u16CbMax]*/
    HI_U16 u16CrMax;            /*RW, Maximum value of R/G for white region, Range: [0x0, 0xFFF]*/
    HI_U16 u16CrMin;            /*RW, Minimum value of R/G for white region, Range: [0x0, u16CrMax]*/
    HI_U16 u16CbHigh;           /*RW, hexagon limit, Range: [0x0,0xFFF], Axis is [u16CrMax, u16CbHigh]*/  
    HI_U16 u16CbLow;            /*RW, hexagon limit, Range: [0x0,0xFFF], Axis is [u16CrMin, u16CbLow]*/ 
    HI_U16 u16CrHigh;           /*RW, hexagon limit, Range: [0x0,0xFFF], Axis is [u16CrHigh, u16CbMax]*/  
    HI_U16 u16CrLow;            /*RW, hexagon limit, Range: [0x0,0xFFF], Axis is [u16CrLow, u16CbMin]*/  
} ISP_WB_STATISTICS_CFG_PARA_S;
typedef struct hiISP_WB_STATISTICS_CFG_S
{
    ISP_WB_STATISTICS_CFG_PARA_S stBayerCfg;
    ISP_WB_STATISTICS_CFG_PARA_S stRGBCfg;         /*Not support*/
} ISP_WB_STATISTICS_CFG_S;

typedef enum hiISP_AF_PEAK_MODE_E
{
    ISP_AF_STA_NORM         = 0,    /* use every value of the block statistic*/
    ISP_AF_STA_PEAK            ,    /* use the maximum value in one line of the block statistic*/
    ISP_AF_STA_BUTT    
}ISP_AF_PEAK_MODE_E;

typedef enum hiISP_AF_SQU_MODE_E
{
    ISP_AF_STA_SUM_NORM     = 0,    /* statistic value accumlate*/
    ISP_AF_STA_SUM_SQU         ,    /* statistic value square then accumlate*/
    ISP_AF_STA_SUM_BUTT    
}ISP_AF_SQU_MODE_E;

typedef struct hiISP_AF_CROP_S 
{
    HI_BOOL             bEnable;    /* RW, Range: [0,1].  AF crop enable.               */
    HI_U16              u16X;       /* RW, Range: [0x0, 0x1FFF]. AF image crop start x.  */
    HI_U16              u16Y;       /* RW, Range: [0x0, 0x1FFF]. AF image crop start y.  */
    HI_U16              u16W;       /* RW, Range: [0x0, 0x1FFF]. AF image crop width.    */
    HI_U16              u16H;       /* RW, Range: [0x0, 0x1FFF]. AF image crop height.   */       
} ISP_AF_CROP_S;

typedef enum hiISP_AF_STATISTICS_POS_E
{   
    ISP_AF_STATISTICS_YUV    = 0,   
    ISP_AF_STATISTICS_RAW       ,   
    ISP_AF_STATISTICS_BUTT
    
}ISP_AF_STATISTICS_POS_E;

typedef struct hiISP_AF_RAW_CFG_S 
{
    HI_BOOL             bGammaEn;    /* RW, Range: [0,1].  gamma enable  .               */
    HI_BOOL             bOffsetEn;   /* RW, Range: [0,1].  AF black level enable  .      */
    HI_U16              u16GrOffset; /* RW, Range: [0x0, 0x3FFF]. black level of GR.     */
    HI_U16              u16GbOffset; /* RW, Range: [0x0, 0x3FFF]. black level of GB.     */
    ISP_BAYER_FORMAT_E  enPattern;   /* RW, Range: [0x0, 0x3]. raw domain pattern.       */
    
} ISP_AF_RAW_CFG_S;

typedef struct hiISP_AF_PRE_FILTER_CFG_S 
{
    HI_BOOL             bEn;         /* RW, Range: [0,1].  pre filter enable  .          */
    HI_U16              u16strength; /* RW, Range: [0x0, 0xFFFF]. pre filter strength    */
    
} ISP_AF_PRE_FILTER_CFG_S;

typedef struct hiISP_AF_CFG_S
{
    HI_BOOL                 bEnable;        /* RW, Range: [0,1].   AF enable.                             */
    HI_U16                  u16Hwnd;        /* RW, Range: [1, 17]. AF statistic window horizontal block.  */
    HI_U16                  u16Vwnd;        /* RW, Range: [1, 15]. AF statistic window veritical block.   */
    HI_U16                  u16Hsize;       /* RW, Range: [1, RES_WIDTH_MAX]. AF image width.             */
    HI_U16                  u16Vsize;       /* RW, Range: [1, RES_HEIGHT_MAX]. AF image height.           */
    ISP_AF_PEAK_MODE_E      enPeakMode;     /* RW, Range: [0,1]. AF peak value statistic mode.            */
    ISP_AF_SQU_MODE_E       enSquMode;      /* RW, Range: [0,1]. AF statistic square accumulate.          */   
    ISP_AF_CROP_S           stCrop;         /* RW, AF input image crop                                    */
    ISP_AF_STATISTICS_POS_E enStatisticsPos;/* RW, Range: [0,1]. AF statistic position, it can be set to yuv or raw */
    ISP_AF_RAW_CFG_S        stRawCfg;       /* RW, When AF locate at RAW domain, these para should be cfg.*/
    ISP_AF_PRE_FILTER_CFG_S stPreFltCfg;    /* RW, pre filter cfg                                         */
    HI_U16                  u16HighLumaTh;  /* RW, Range: [0,0xFF]. high luma threshold.                  */ 
    
}ISP_AF_CFG_S;

typedef struct hiISP_AF_LD_S 
{
    HI_BOOL     bLdEn;                      /* RW, Range: [0, 1]. FILTER level depend gain enable.        */
    HI_U16      u16ThLow;                   /* RW, range: [0x0, 0xFF]. FILTER level depend th low         */
    HI_U16      u16GainLow;                 /* RW, range: [0x0, 0xFF]. FILTER level depend gain low       */
    HI_U16      u16SlpLow;                  /* RW, range: [0x0, 0xF].  FILTER level depend slope low      */  
    HI_U16      u16ThHigh;                  /* RW, range: [0x0, 0xFF]. FILTER level depend th high        */
    HI_U16      u16GainHigh;                /* RW, range: [0x0, 0xFF]. FILTER level depend gain high      */
    HI_U16      u16SlpHigh;                 /* RW, range: [0x0, 0xF].  FILTER level depend slope high     */
    
} ISP_AF_LD_S;                              
                                            
typedef struct hiISP_AF_CORING_S            
{                                           
    HI_U16      u16Th;                      /* RW, Range: [0x0, 0x7FF].FILTER coring threshold.           */
    HI_U16      u16Slp;                     /* RW, Range: [0x0, 0xF].  FILTER Coring Slope                */
    HI_U16      u16Lmt;                     /* RW, Range: [0x0, 0x7FF].FILTER coring limit                */
    
} ISP_AF_CORING_S ;                         
                                            
                                            
#define IIR_EN_NUM      (3)
#define IIR_GAIN_NUM    (7)
#define IIR_SHIFT_NUM   (4)
typedef struct hiISP_AF_H_PARAM_S           
{
    HI_BOOL         bNarrowBand;            /* RW, Range: [0, 1]. IIR narrow band enable.                 */ 
    HI_BOOL         abIIREn[IIR_EN_NUM];             /* RW, Range: [0, 1]. IIR enable.                             */
    HI_S16          as16IIRGain[IIR_GAIN_NUM];         /* RW, Range: gain0:[0,255]; others:[-511, 511]. IIR gain.    */
    HI_U16          au16IIRShift[IIR_SHIFT_NUM];        /* RW, Range: [0x0, 0x7].  IIR shift.                         */
    ISP_AF_LD_S     stLd;                   /* RW, filter level depend.                                   */
    ISP_AF_CORING_S stCoring;               /* RW, filter coring.                                         */                                                                                                      
                                                                                                          
}ISP_AF_H_PARAM_S;                                                                                        
                                                                                                          
#define FIR_GAIN_NUM    (5)
typedef struct hiISP_AF_V_PARAM_S                                                                         
{                                                                                                         
    HI_S16          as16FIRH[FIR_GAIN_NUM];            /* RW, Range: [-31, 31].   FIR gain.                          */
    ISP_AF_LD_S     stLd;                   /* RW, filter level depend.                                   */
    ISP_AF_CORING_S stCoring;               /* RW, filter coring.                                         */ 
}ISP_AF_V_PARAM_S;                          

#define ACC_SHIFT_H_NUM (2)
#define ACC_SHIFT_V_NUM (2)
typedef struct hiISP_AF_FV_PARAM_S          
{                                           
    HI_U16 u16AccShiftY;                    /* RW, Range: [0x0, 0xF]. luminance Y statistic shift.        */
    HI_U16 au16AccShiftH[ACC_SHIFT_H_NUM];  /* RW, Range: [0x0, 0xF]. IIR statistic shift.                */
    HI_U16 au16AccShiftV[ACC_SHIFT_V_NUM];  /* RW, Range: [0x0, 0xF]. FIR statistic shift.                */
    HI_U16 u16HlCntShift;                   /* RW, Range: [0x0, 0xF]. High luminance counter shift        */
}ISP_AF_FV_PARAM_S;

typedef struct hiISP_FOCUS_STATISTICS_CFG_S
{
    ISP_AF_CFG_S        stConfig;
    ISP_AF_H_PARAM_S    stHParam_IIR0;
    ISP_AF_H_PARAM_S    stHParam_IIR1;
    ISP_AF_V_PARAM_S    stVParam_FIR0;
    ISP_AF_V_PARAM_S    stVParam_FIR1;
    ISP_AF_FV_PARAM_S   stFVParam;
} ISP_FOCUS_STATISTICS_CFG_S;

/* 
   the main purpose of stat key was to access individual statistic info separately...
   ...for achieving performance optimization of CPU, because of we acquire stat...
   ... in ISP_DRV ISR for our firmware reference and USER can also use alternative MPI...
   ... to do this job, so bit1AeStat1~bit1AfStat for MPI behavior control, and bit16IsrAccess...
   ... for ISR access control, they were independent. but they have the same bit order, for example...
   ... bit1AeStat1 for MPI AeStat1 access key, and bit16 of u32Key for ISR AeStat1 access key 
*/
typedef union hiISP_STATISTICS_CTRL_U
{
    HI_U32  u32Key;
    struct
    {
        HI_U32  bit1AeStat1     : 1 ;   /* [0] */
        HI_U32  bit1AeStat2     : 1 ;   /* [1] */
        HI_U32  bit1AeStat3     : 1 ;   /* [2] */
        HI_U32  bit1AeStat4     : 1 ;   /* [3] */
        HI_U32  bit1AeStat5     : 1 ;   /* [4] */
        HI_U32  bit1AeStat6     : 1 ;   /* [5] */
        HI_U32  bit1AwbStat1    : 1 ;   /* [6] */
        HI_U32  bit1AwbStat2    : 1 ;   /* [7] */
        HI_U32  bit1AwbStat3    : 1 ;   /* [8] */
        HI_U32  bit1AwbStat4    : 1 ;   /* [9] */
        HI_U32  bit1AfStat      : 1 ;   /* [10] */
        HI_U32  bit5Rsv         : 5 ;   /* [11:15] */
        HI_U32  bit16IsrAccess  : 16;   /* [16:31] */
    };
}ISP_STATISTICS_CTRL_U;

typedef struct hiISP_STATISTICS_CFG_S
{
    ISP_STATISTICS_CTRL_U       unKey; 
    ISP_AE_STATISTICS_CFG_S     stAECfg;
    ISP_WB_STATISTICS_CFG_S     stWBCfg;
    ISP_FOCUS_STATISTICS_CFG_S  stFocusCfg;
} ISP_STATISTICS_CFG_S;

/* statistics structs */
#define HIST_5_NUM      (5)
#define HIST_1024_NUM   (1024)
#define HIST_256_NUM    (256)
typedef struct hiISP_AE_STATISTICS_S
{   
    HI_U16 au16Hist5Value[HIST_5_NUM];                                   /*RO, Global 5 bins histogram, Range: [0x0, 0xFFFF]*/
    //HI_U16 au16ZoneHist5Value[AE_ZONE_ROW][AE_ZONE_COLUMN][5];  /*RO, Zone 5 bins histogram,Range: [0x0, 0xFFFF]*/
    HI_U32 au32Hist1024Value[HIST_1024_NUM];                             /*RO, Global 1024 bins histogram,Range: [0x0, 0xFFFFFFFF]*/
    HI_U16 au16GlobalAvg[ISP_BAYER_CHN_NUM];                                    /*RO, Global average value, Range: [0x0, 0xFFFF]*/
    HI_U16 au16ZoneAvg[AE_ZONE_ROW][AE_ZONE_COLUMN][ISP_BAYER_CHN_NUM];         /*RO, Zone average value, Range: [0x0, 0xFFFF]*/
    HI_U32 au32PreHistogram1[HIST_256_NUM];                             /*RO, After TPG, channel 1 256 bin histogram,Range: [0x0, 0xFFFFFFFF] */
    HI_U32 au32PreHistogram2[HIST_256_NUM];                             /*RO, After TPG, channel 2 256 bin histogram,Range: [0x0, 0xFFFFFFFF] */
    HI_U32 au32PreHistogram3[HIST_256_NUM];                             /*RO, After TPG, channel 3 256 bin histogram,Range: [0x0, 0xFFFFFFFF] */
    HI_U32 au32PreHistogram4[HIST_256_NUM];                             /*RO, After TPG, channel 4 256 bin histogram,Range: [0x0, 0xFFFFFFFF] */
}ISP_AE_STATISTICS_S;

typedef struct hiISP_WB_BAYER_STATISTICS_S
{
    HI_U16 u16GlobalR;          /*RO, Global WB output Average R, Range: [0x0, 0xFFFF]*/   
    HI_U16 u16GlobalG;          /*RO, Global WB output Average G, Range: [0x0, 0xFFFF]*/  
    HI_U16 u16GlobalB;          /*RO, Global WB output Average B, Range: [0x0, 0xFFFF]*/  
    HI_U16 u16CountAll;         /*RO, normalized number of Gray points, Range: [0x0, 0xFFFF]*/  
    HI_U16 u16CountMin;         /*RO, normalized number of pixels under BlackLevel, Range: [0x0, 0xFFFF]*/  
    HI_U16 u16CountMax;         /*RO, normalized number of pixels above Whitelevel, Range: [0x0, 0xFFFF], u16CountAll + u16CountMin + u16CountMax = 0xFFFF*/  
           
    HI_U16 au16ZoneAvgR[AWB_ZONE_NUM];            /*RO, Zone Average R, Range: [0x0, 0xFFFF]*/   
    HI_U16 au16ZoneAvgG[AWB_ZONE_NUM];            /*RO, Zone Average G, Range: [0x0, 0xFFFF]*/   
    HI_U16 au16ZoneAvgB[AWB_ZONE_NUM];            /*RO, Zone Average B, Range: [0x0, 0xFFFF]*/   
    HI_U16 au16ZoneCountAll[AWB_ZONE_NUM];        /*RO, normalized number of Gray points, Range: [0x0, 0xFFFF]*/   
    HI_U16 au16ZoneCountMin[AWB_ZONE_NUM];        /*RO, normalized number of pixels under BlackLevel, Range: [0x0, 0xFFFF]*/   
    HI_U16 au16ZoneCountMax[AWB_ZONE_NUM];        /*RO, normalized number of pixels above Whitelevel, Range: [0x0, 0xFFFF]*/   
} ISP_WB_BAYER_STATISTICS_INFO_S; 

typedef struct hiISP_WB_RGB_STATISTICS_S
{
    HI_U16 u16GlobalGR;         /*RO, Global WB output G/R, 4.8-bit fix-point, Range: [0x0, 0xFFF]*/
    HI_U16 u16GlobalGB;         /*RO, Global WB output G/B, 4.8-bit fix-point, Range: [0x0, 0xFFF]*/
    HI_U32 u32GlobalSum;        /*RO, Global WB output population*/

    HI_U16 au16ZoneGR[AWB_ZONE_NUM];       /*RO, Zone WB output G/R, 4.8-bit fix-point, Range: [0x0, 0xFFF]*/
    HI_U16 au16ZoneGB[AWB_ZONE_NUM];       /*RO, Zone WB output G/B, 4.8-bit fix-point, Range: [0x0, 0xFFF]*/
    HI_U32 au32ZoneSum[AWB_ZONE_NUM];      /*RO, Zone WB output population*/
} ISP_WB_RGB_STATISTICS_INFO_S; 

typedef struct hiISP_WB_STATISTICS_S
{
    ISP_WB_BAYER_STATISTICS_INFO_S stBayerStatistics;
    ISP_WB_RGB_STATISTICS_INFO_S   stRGBStatistics;
} ISP_WB_STATISTICS_S;

typedef struct hiISP_FOCUS_ZONE_S
{
    HI_U16  u16v1;
    HI_U16  u16h1;
    HI_U16  u16v2;
    HI_U16  u16h2;
    HI_U16  u16y;
    HI_U16  u16HlCnt;
} ISP_FOCUS_ZONE_S;
typedef struct hiISP_FOCUS_STATISTICS_S
{
    ISP_FOCUS_ZONE_S stZoneMetrics[AF_ZONE_ROW][AF_ZONE_COLUMN]; /*RO, The zoned measure of contrast*/
} ISP_FOCUS_STATISTICS_S;

typedef struct hiISP_STATISTICS_S
{
    ISP_AE_STATISTICS_S         stAEStat;   
    ISP_WB_STATISTICS_S         stWBStat;
    ISP_FOCUS_STATISTICS_S      stFocusStat;
} ISP_STATISTICS_S;

typedef struct hiISP_INIT_ATTR_S 
{
    HI_U32 u32ExpTime;
    HI_U32 u32AGain;
    HI_U32 u32DGain;
    HI_U32 u32ISPDGain;
    HI_U32 u32Exposure;
    HI_U32 u32LinesPer500ms;
    HI_U32 u32PirisFNO;
    HI_U16 u16WBRgain;
    HI_U16 u16WBGgain;
    HI_U16 u16WBBgain;
    HI_U16 u16SampleRgain;
    HI_U16 u16SampleBgain;
}ISP_INIT_ATTR_S ;

/*ISP debug information*/
typedef struct hiISP_DEBUG_INFO_S
{
    HI_BOOL bDebugEn;       /*RW, 1:enable debug, 0:disable debug*/
    HI_U32  u32PhyAddr;     /*RW, phy address of debug info */
    HI_U32  u32Depth;       /*RW, depth of debug info */
} ISP_DEBUG_INFO_S;

typedef struct hiISP_DBG_ATTR_S
{
    HI_U32  u32Rsv;         /* need to add member */
} ISP_DBG_ATTR_S;

typedef struct hiISP_DBG_STATUS_S
{
    HI_U32  u32FrmNumBgn;
    HI_U32  u32Rsv;         /* need to add member */
    HI_U32  u32FrmNumEnd;
} ISP_DBG_STATUS_S;

typedef enum hiISP_SNS_TYPE_E
{
    ISP_SNS_I2C_TYPE = 0,
    ISP_SNS_SSP_TYPE,
    
    ISP_SNS_TYPE_BUTT,
} ISP_SNS_TYPE_E;

/* sensor communication bus */
typedef union hiISP_SNS_COMMBUS_U 
{
    HI_S8   s8I2cDev;
    struct 
    {
        HI_S8  bit4SspDev       : 4;
        HI_S8  bit4SspCs        : 4;  
    }s8SspDev;
} ISP_SNS_COMMBUS_U; 

typedef struct hiISP_I2C_DATA_S
{
    HI_BOOL bUpdate;
    HI_U8   u8DelayFrmNum;
    HI_U8   u8IntPos;
    
    HI_U8   u8DevAddr;
    HI_U32  u32RegAddr;
    HI_U32  u32AddrByteNum;
    HI_U32  u32Data;
    HI_U32  u32DataByteNum;
} ISP_I2C_DATA_S;

typedef struct hiISP_SSP_DATA_S
{
    HI_BOOL bUpdate;
    HI_U8   u8DelayFrmNum;
    HI_U8   u8IntPos;
    
    HI_U32  u32DevAddr;
    HI_U32  u32DevAddrByteNum;
    HI_U32  u32RegAddr;
    HI_U32  u32RegAddrByteNum;
    HI_U32  u32Data;
    HI_U32  u32DataByteNum;
} ISP_SSP_DATA_S;

typedef struct hiISP_SNS_REGS_INFO_S
{
    ISP_SNS_TYPE_E enSnsType;
    HI_U32  u32RegNum;
    HI_U8   u8Cfg2ValidDelayMax;
    ISP_SNS_COMMBUS_U  unComBus;
    union
    {
        ISP_I2C_DATA_S astI2cData[ISP_MAX_SNS_REGS];
        ISP_SSP_DATA_S astSspData[ISP_MAX_SNS_REGS];
    };

   struct 
   {  
       HI_BOOL bUpdate;
       HI_U8   u8DelayFrmNum;
       HI_U32  u32SlaveVsTime;      /* time of vsync. Unit: inck clock cycle */             
    } stSlvSync;

   HI_BOOL bConfig;
} ISP_SNS_REGS_INFO_S;

typedef struct hiISP_VD_INFO_S
{
  HI_U32  u32Reserved;                       /*RO, Range: [0x0, 0xFFFFFFFF] */
}ISP_VD_INFO_S;


typedef struct hiISP_REG_ATTR_S
{
    HI_U32 u32IspRegAddr;
    HI_U32 u32IspRegSize;
    HI_U32 u32IspExtRegAddr;
    HI_U32 u32IspExtRegSize;
    HI_U32 u32AeExtRegAddr;
    HI_U32 u32AeExtRegSize;
    HI_U32 u32AwbExtRegAddr;
    HI_U32 u32AwbExtRegSize;
} ISP_REG_ATTR_S;


/******************************************************/
/********************* AI structs ************************/

typedef enum hiISP_IRIS_TYPE_E
{
    ISP_IRIS_DC_TYPE = 0,
    ISP_IRIS_P_TYPE,
    
    ISP_IRIS_TYPE_BUTT,
} ISP_IRIS_TYPE_E;

typedef enum hiISP_IRIS_F_NO_E
{
    ISP_IRIS_F_NO_32_0 = 0,
    ISP_IRIS_F_NO_22_0,
    ISP_IRIS_F_NO_16_0,
    ISP_IRIS_F_NO_11_0,
    ISP_IRIS_F_NO_8_0,
    ISP_IRIS_F_NO_5_6,
    ISP_IRIS_F_NO_4_0,
    ISP_IRIS_F_NO_2_8,
    ISP_IRIS_F_NO_2_0,
    ISP_IRIS_F_NO_1_4,
    ISP_IRIS_F_NO_1_0,    
    
    ISP_IRIS_F_NO_BUTT,
} ISP_IRIS_F_NO_E;

typedef struct hiISP_DCIRIS_ATTR_S
{
    HI_S32 s32Kp;                    /*RW, Range:[0, 100000], the proportional gain of PID algorithm, default value is 7000 */
    HI_S32 s32Ki;                    /*RW, Range:[0, 1000], the integral gain of PID algorithm, default value is 100 */
    HI_S32 s32Kd;                    /*RW, Range:[0, 100000], the derivative gain of PID algorithm, default value is 3000 */
    HI_U32 u32MinPwmDuty;            /*RW, Range:[0, 1000], which is the min pwm duty for dciris control */
    HI_U32 u32MaxPwmDuty;            /*RW, Range:[0, 1000], which is the max pwm duty for dciris control */
    HI_U32 u32OpenPwmDuty;           /*RW, Range:[0, 1000], which is the open pwm duty for dciris control */
} ISP_DCIRIS_ATTR_S;

typedef struct hiISP_PIRIS_ATTR_S
{
    HI_BOOL bStepFNOTableChange;    /*WO, Step-F number mapping table change or not */
    HI_BOOL bZeroIsMax;             /*RW, Step 0 corresponds to max aperture or not, it's related to the specific iris */
    HI_U16 u16TotalStep;            /*RW, Range:[1, 1024], Total steps of  Piris's aperture, it's related to the specific iris */
    HI_U16 u16StepCount;            /*RW, Range:[1, 1024], Used steps of Piris's aperture. when Piris's aperture is too small, the F number precision is not enough, 
                                          so not all the steps can be used. It's related to the specific iris*/
    HI_U16 au16StepFNOTable[AI_MAX_STEP_FNO_NUM];   /*RW, Range:[0, 1024], Step-F number mapping table. F1.0 is expressed as 1024, F32 is expressed as 1, it's related to the specific iris*/
    ISP_IRIS_F_NO_E  enMaxIrisFNOTarget;  /*RW, Range:[F32.0, F1.0], Max F number of Piris's aperture, it's related to the specific iris */
    ISP_IRIS_F_NO_E  enMinIrisFNOTarget;  /*RW, Range:[F32.0, F1.0], Min F number of Piris's aperture, it's related to the specific iris */
    
    HI_BOOL bFNOExValid;            /*RW, use equivalent gain to present FNO or not */
    HI_U32 u32MaxIrisFNOTarget;    /*RW, Range:[1, 1024], Max equivalent gain of F number of Piris's aperture, only used when bFNOExValid is true, it's related to the specific iris */
    HI_U32 u32MinIrisFNOTarget;    /*RW, Range:[1, 1024], Min equivalent gain of F number of Piris's aperture, only used when bFNOExValid is true, it's related to the specific iris */
} ISP_PIRIS_ATTR_S;

typedef struct hiISP_MI_ATTR_S
{
    HI_U32  u32HoldValue;            /*RW, Range: [0x0, 0x3E8], iris hold value for DC-iris*/
    ISP_IRIS_F_NO_E  enIrisFNO;      /*RW, the F number of P-iris*/
} ISP_MI_ATTR_S;

typedef enum hiISP_IRIS_STATUS_E
{
    ISP_IRIS_KEEP  = 0,       /* In this mode can set the MI holdvalue */
    ISP_IRIS_OPEN  = 1,       /* Open Iris to the max */
    ISP_IRIS_CLOSE = 2,       /* Close Iris to the min */
    ISP_IRIS_BUTT
} ISP_IRIS_STATUS_E;

typedef struct hiISP_IRIS_ATTR_S
{
    HI_BOOL bEnable;                 /* iris enable/disable */
    ISP_OP_TYPE_E   enOpType;        /* auto iris or manual iris */
    ISP_IRIS_TYPE_E enIrisType;      /* DC-iris or P-iris */
    ISP_IRIS_STATUS_E enIrisStatus;  /*RW, status of Iris*/
    ISP_MI_ATTR_S   stMIAttr;
} ISP_IRIS_ATTR_S;


/******************************************************/
/********************* AE structs ************************/
typedef enum hiISP_AE_MODE_E
{
    AE_MODE_SLOW_SHUTTER = 0,
    AE_MODE_FIX_FRAME_RATE  = 1,
    AE_MODE_BUTT
} ISP_AE_MODE_E;

typedef enum hiISP_AE_STRATEGY_E
{
    AE_EXP_HIGHLIGHT_PRIOR = 0,
    AE_EXP_LOWLIGHT_PRIOR  = 1,
    AE_STRATEGY_MODE_BUTT
} ISP_AE_STRATEGY_E;

typedef struct hiISP_AE_RANGE_S
{
    HI_U32 u32Max;
    HI_U32 u32Min;
} ISP_AE_RANGE_S;

typedef struct hiISP_AE_DELAY_S
{
    HI_U16 u16BlackDelayFrame;    /*RW, Range: [0x0, 0xFFFF], AE black delay frame count*/
    HI_U16 u16WhiteDelayFrame;    /*RW, Range: [0x0, 0xFFFF], AE white delay frame count*/
} ISP_AE_DELAY_S;

typedef enum hiISP_ANTIFLICKER_MODE_E
{
    /* The epxosure time is fixed to be the multiplies of 1/(2*frequency) sec,
     * it may lead to over exposure in the high-luminance environments. */
    ISP_ANTIFLICKER_NORMAL_MODE = 0x0,

    /* The anti flicker may be closed to avoid over exposure in the high-luminance environments. */
    ISP_ANTIFLICKER_AUTO_MODE = 0x1,

    ISP_ANTIFLICKER_MODE_BUTT
}ISP_ANTIFLICKER_MODE_E;

typedef struct hiISP_ANTIFLICKER_S
{
    HI_BOOL bEnable;
    HI_U8   u8Frequency;          /*RW, Range: usually this value is 50 or 60  which is the frequency of the AC power supply*/
    ISP_ANTIFLICKER_MODE_E  enMode;
} ISP_ANTIFLICKER_S;

typedef struct hiISP_SUBFLICKER_S
{
    HI_BOOL bEnable;

    /* RW, Range: [0x0, 0x64], if subflicker mode enable, current luma is less than AE compensation plus LumaDiff, 
      AE will keep min antiflicker shutter time(for example: 1/100s or 1/120s) to avoid flicker. while current luma is 
      larger than AE compensation plus the LumaDiff, AE will reduce shutter time to avoid over-exposure and introduce 
      flicker in the pircture */
    HI_U8   u8LumaDiff;          
} ISP_SUBFLICKER_S;


typedef enum hiISP_FSWDR_MODE_E
{
    /* Normal FSWDR mode */
    ISP_FSWDR_NORMAL_MODE = 0x0,

    /* Long frame mode, only effective in LINE_WDR, when running in this mode FSWDR module only output the long frame data */
    ISP_FSWDR_LONG_FRAME_MODE = 0x1,

	/*Auto long frame mode, only effective in LINE_WDR, When running in this mode, normal WDR and long frame mode would auto switch*/
	ISP_FSWDR_AUTO_LONG_FRAME_MODE = 0x2,
    ISP_FSWDR_MODE_BUTT
}ISP_FSWDR_MODE_E;

 
typedef struct hiISP_AE_ATTR_S
{
    /* base parameter */
    ISP_AE_RANGE_S stExpTimeRange;   /*RW,  sensor exposure time (unit: us ), Range: [0x0, 0xFFFFFFFF], it's related to the specific sensor */
    ISP_AE_RANGE_S stAGainRange;     /*RW,  sensor analog gain (unit: times, 10bit precision), Range : [0x400, 0xFFFFFFFF], it's related to the specific sensor */
    ISP_AE_RANGE_S stDGainRange;     /*RW,  sensor digital gain (unit: times, 10bit precision), Range : [0x400, 0xFFFFFFFF], it's related to the specific sensor */
    ISP_AE_RANGE_S stISPDGainRange;  /*RW,  ISP digital gain (unit: times, 10bit precision), Range : [0x400, 0xFFFFFFFF], it's related to the ISP digital gain range */
    ISP_AE_RANGE_S stSysGainRange;   /*RW,  system gain (unit: times, 10bit precision), Range : [0x400, 0xFFFFFFFF], it's related to the specific sensor and ISP Dgain range */
    HI_U32 u32GainThreshold;         /*RW,  Gain threshold for slow shutter mode (unit: times, 10bit precision), Range : [0x400, 0xFFFFFFFF] */

    HI_U8  u8Speed;                  /*RW, Range: [0x0, 0xFF], AE adjust step for dark scene to bright scene switch */
    HI_U16  u16BlackSpeedBias;       /*RW, Range: [0x0, 0xFFFF], AE adjust step bias for bright scene to dark scene switch */
    HI_U8  u8Tolerance;              /*RW, Range: [0x0, 0xFF], AE adjust tolerance*/
    HI_U8  u8Compensation;           /*RW, Range: [0x0, 0xFF], AE compensation*/ 
    HI_U16  u16EVBias;               /*RW, Range: [0x0, 0xFFFF], AE EV bias*/
    ISP_AE_STRATEGY_E enAEStrategyMode;  /*RW, Support Highlight prior or Lowlight prior*/
    HI_U16  u16HistRatioSlope;       /*RW, Range: [0x0, 0xFFFF], AE hist ratio slope*/
    HI_U8   u8MaxHistOffset;         /*RW, Range: [0x0, 0xFF], Max hist offset*/
  
    ISP_AE_MODE_E  enAEMode;         /*RW, AE mode(slow shutter/fix framerate)(onvif)*/
    ISP_ANTIFLICKER_S stAntiflicker;
    ISP_SUBFLICKER_S stSubflicker;
    ISP_AE_DELAY_S stAEDelayAttr;       

    HI_BOOL bManualExpValue;         /*RW, manual exposure value or not*/
    HI_U32 u32ExpValue;              /*RW, Range: (0x0, 0xFFFFFFFF], manual exposure value */ 

    ISP_FSWDR_MODE_E enFSWDRMode;    /*RW, FSWDR running mode */
    HI_BOOL bWDRQuick;               /*RW, WDR use delay strategy or not; If is true, WDR AE adjust will be faster */

    HI_U16  u16ISOCalCoef;     /*RW, The coefficient between Standard ISO and origin ISO (unit: times, 8bit precision)*/

    /*AE weighting table*/
    HI_U8 au8Weight[AE_ZONE_ROW][AE_ZONE_COLUMN]; /*Range :  [0, 0xF]*/
} ISP_AE_ATTR_S;

typedef struct hiISP_ME_ATTR_S
{
    ISP_OP_TYPE_E enExpTimeOpType;
    ISP_OP_TYPE_E enAGainOpType;
    ISP_OP_TYPE_E enDGainOpType;
    ISP_OP_TYPE_E enISPDGainOpType;

    HI_U32 u32ExpTime;        /*RW,  sensor exposure time (unit: us ), Range: [0x0, 0xFFFFFFFF], it's related to the specific sensor */
    HI_U32 u32AGain;          /*RW,  sensor analog gain (unit: times, 10bit precision), Range : [0x400, 0xFFFFFFFF], it's related to the specific sensor */
    HI_U32 u32DGain;          /*RW,  sensor digital gain (unit: times, 10bit precision), Range : [0x400, 0xFFFFFFFF], it's related to the specific sensor */
    HI_U32 u32ISPDGain;       /*RW,  ISP digital gain (unit: times, 10bit precision), Range : [0x400, 0xFFFFFFFF], it's related to the ISP digital gain range */
} ISP_ME_ATTR_S;

typedef struct hiISP_EXPOSURE_ATTR_S
{    
    HI_BOOL         bByPass;
    ISP_OP_TYPE_E   enOpType;
    HI_U8     u8AERunInterval;         /*RW, Range: [0x1, 0xFF], set the AE run interval*/
    HI_BOOL   bHistStatAdjust;         /*RW, HI_TRUE: 256 bin histogram statistic config will adjust when large red or blue area detected. 
                                                 HI_FALSE: 256 bin histogram statistic config will not change */
    HI_BOOL   bAERouteExValid;         /*RW, use extend AE route or not */
    ISP_ME_ATTR_S   stManual;
    ISP_AE_ATTR_S   stAuto;    
} ISP_EXPOSURE_ATTR_S;

#define EXP_RATIO_NUM       (3)
typedef struct hiISP_WDR_EXPOSURE_ATTR_S
{
    ISP_OP_TYPE_E enExpRatioType;   /* RW, OP_TYPE_AUTO: The ExpRatio used in ISP is generated by firmware; OP_TYPE_MANUAL: The ExpRatio used in ISP is set by u32ExpRatio */
    HI_U32 au32ExpRatio[EXP_RATIO_NUM];             /* RW, Range: [0x40, 0xFFF]. Format: unsigned 6.6-bit fixed-point. 0x40 means 1 times.
                                            When enExpRatioType is OP_TYPE_AUTO, u32ExpRatio is invalid.
                                            When enExpRatioType is OP_TYPE_MANUAL, u32ExpRatio is quotient of long exposure time / short exposure time. */
    HI_U32 u32ExpRatioMax;          /* RW, Range: [0x40, 0x4000]. 6bit precision, 0x40 means 1 times.
                                            When enExpRatioType is OP_TYPE_AUTO, u32ExpRatioMax is max(upper limit) of ExpRatio generated by firmware.
                                            When enExpRatioType is OP_TYPE_MANUAL, u32ExpRatioMax is invalid. */
    HI_U32 u32ExpRatioMin;          /* RW, Range: [0x40, u32ExpRatioMax]. 6bit precision, 0x40 means 1 times.
                                            When enExpRatioType is OP_TYPE_AUTO, u32ExpRatioMin is min(lower limit) of ExpRatio generated by firmware.
                                            When enExpRatioType is OP_TYPE_MANUAL, u32ExpRatioMin is invalid. */
    HI_U16 u16Tolerance;            /* RW, Range: [0x0, 0xFF], set the dynamic range tolerance. Format: unsigned 6.2-bit fixed-point. 0x4 means 1dB. */
    HI_U16 u16Speed;                /* RW, Range: [0x0, 0xFF], exposure ratio adjust speed */   
    HI_U16 u16RatioBias;            /* RW, Range: [0x0, 0xFFFF], exposure ratio bias */
	HI_U32 u32ProWDRGain;           /* RW, ISP digital gain after WDR, only effective in WDR mode(unit: times, 10bit precision), Range : [0x400, 0xFFFFFFFF], it's related to the ISP digital gain range */
} ISP_WDR_EXPOSURE_ATTR_S;

#define ISP_AE_ROUTE_MAX_NODES (16)
typedef struct hiISP_AE_ROUTE_NODE_S
{
    HI_U32  u32IntTime;     /*RW,  sensor exposure time (unit: us ), Range: (0x0, 0xFFFFFFFF], it's related to the specific sensor */
    HI_U32  u32SysGain;     /*RW,  system gain (unit: times, 10bit precision), Range : [0x400, 0xFFFFFFFF], it's related to the specific sensor and ISP Dgain range */
    ISP_IRIS_F_NO_E  enIrisFNO;  /*RW, the F number of the iris's aperture, Range:[F32.0, F1.0], only support for Piris */
    HI_U32  u32IrisFNOLin;  /*RW, the equivalent gain of F number of the iris's aperture, Range:[1,1024], only support for Piris */
} ISP_AE_ROUTE_NODE_S;

typedef struct hiISP_AE_ROUTE_S
{
    HI_U32 u32TotalNum;     /*RW,  Range: [0x0, 0x10], total node number of AE route */ 
    ISP_AE_ROUTE_NODE_S astRouteNode[ISP_AE_ROUTE_MAX_NODES];
} ISP_AE_ROUTE_S;

#define ISP_AE_ROUTE_EX_MAX_NODES (16)
typedef struct hiISP_AE_ROUTE_EX_NODE_S
{
    HI_U32  u32IntTime;     /*RW,  sensor exposure time (unit: us ), Range: (0x0, 0xFFFFFFFF], it's related to the specific sensor */
    HI_U32  u32Again;       /*RW,  sensor analog gain (unit: times, 10bit precision), Range : [0x400, 0x3FFFFF], it's related to the specific sensor*/
    HI_U32  u32Dgain;       /*RW,  sensor digital gain (unit: times, 10bit precision), Range : [0x400, 0x3FFFFF], it's related to the specific sensor*/
    HI_U32  u32IspDgain;    /*RW,  ISP digital gain (unit: times, 10bit precision), Range : [0x400, 0x40000] */
    ISP_IRIS_F_NO_E  enIrisFNO;  /*RW, the F number of the iris's aperture, Range:[F32.0, F1.0], only support for Piris */
    HI_U32  u32IrisFNOLin;  /*RW, the equivalent gain of F number of the iris's aperture, Range:[1,1024], only support for Piris */
} ISP_AE_ROUTE_EX_NODE_S;

typedef struct hiISP_AE_ROUTE_EX_S
{
    HI_U32 u32TotalNum;       /*RW,  Range: [0x0, 0x10], total node number of extend AE route */ 
    ISP_AE_ROUTE_EX_NODE_S astRouteExNode[ISP_AE_ROUTE_EX_MAX_NODES];
} ISP_AE_ROUTE_EX_S;

typedef struct hiISP_EXP_INFO_S
{
    HI_U32 u32ExpTime;                 /* RO, Range: [0x0, 0xFFFFFFFF] */  
    HI_U32 u32ShortExpTime;            /* RO, Range: [0x0, 0xFFFFFFFF] */
    HI_U32 u32MedianExpTime;           /* RO, Range: [0x0, 0xFFFFFFFF] */
    HI_U32 u32LongExpTime;             /* RO, Range: [0x0, 0xFFFFFFFF] */
    HI_U32 u32AGain;                   /* RO, Range: [0x400, 0xFFFFFFFF] */                
    HI_U32 u32DGain;                   /* RO, Range: [0x400, 0xFFFFFFFF] */            
    HI_U32 u32ISPDGain;                /* RO, Range: [0x400, 0xFFFFFFFF] */            
    HI_U32 u32Exposure;                /* RO, Range: [0x40, 0xFFFFFFFF] */  
    HI_BOOL bExposureIsMAX;            /* RO, Range: [0x0, 0x1]*/
    HI_S16 s16HistError;               /* RO, Range: [-0x8000, 0x7FFF]*/               
    HI_U32 au32AE_Hist1024Value[HIST_1024_NUM]; /* RO, 1024 bins histogram for channel 1 */
    HI_U16 au16AE_Hist5Value[HIST_5_NUM];       /* RO, 5 bins histogram */
    HI_U8  u8AveLum;                   /* RO, Range: [0x0, 0xFF]*/
    HI_U32 u32LinesPer500ms;           /* RO, Range: [0x0, 0xFFFFFFFF], exposure lines per 500ms */ 
    HI_U32 u32PirisFNO;                /* RO, Range: [0x0, 0x400] */  
    HI_U32 u32Fps;                     /* RO, actual fps */  
    HI_U32 u32ISO;                     /* RO, Range: [0x64, 0xFFFFFFFF] */
    HI_U32 u32ISOCalibrate;            /* RO, Range: [0,0xFFFFFFFF]*/
    HI_U32 u32RefExpRatio;             /* RO, Range: [0x40, 0x4000] */
    HI_U32 u32FirstStableTime;         /* RO, AE first stable time for quick start */
    ISP_AE_ROUTE_S stAERoute;          /* RO, Actual AE route */
    ISP_AE_ROUTE_EX_S stAERouteEx;     /* RO, Actual AE route_ex */
}ISP_EXP_INFO_S;


/********************* AWB structs ************************/
typedef enum hiISP_AWB_ALG_TYPE_E
{
    AWB_ALG_LOWCOST = 0,
    AWB_ALG_ADVANCE = 1,
    AWB_ALG_BUTT
} ISP_AWB_ALG_TYPE_E;

typedef enum hiISP_AWB_MULTI_LS_TYPE_E
{
    AWB_MULTI_LS_SAT = 0,
    AWB_MULTI_LS_CCM = 1,
    AWB_MULTI_LS_BUTT
} ISP_AWB_MULTI_LS_TYPE_E;

typedef enum hiISP_AWB_INDOOR_OUTDOOR_STATUS_E
{
    AWB_INDOOR_MODE = 0,
    AWB_OUTDOOR_MODE = 1,
    AWB_INDOOR_OUTDOOR_BUTT
} ISP_AWB_INDOOR_OUTDOOR_STATUS_E;

typedef struct hiISP_AWB_CT_LIMIT_ATTR_S
{
    HI_BOOL bEnable;
    ISP_OP_TYPE_E   enOpType;

    HI_U16 u16HighRgLimit;     /*RW, Range:[0x0, 0xFFF], in Manual Mode, user define the Max Rgain of High Color Temperature*/
    HI_U16 u16HighBgLimit;     /*RW, Range:[0x0, 0xFFF], in Manual Mode, user define the Min Bgain of High Color Temperature*/
    HI_U16 u16LowRgLimit;      /*RW, Range:[0x0, u16HighRgLimit), in Manual Mode, user define the Min Rgain of Low Color Temperature*/
    HI_U16 u16LowBgLimit;      /*RW, Range:(u16HighBgLimit, 0xFFF], in Manual Mode, user define the Max Bgain of Low Color Temperature*/
} ISP_AWB_CT_LIMIT_ATTR_S;

typedef struct hiISP_AWB_IN_OUT_ATTR_S
{
    HI_BOOL bEnable;
    ISP_OP_TYPE_E   enOpType;
    ISP_AWB_INDOOR_OUTDOOR_STATUS_E  enOutdoorStatus;             /*User should config indoor or outdoor status in Manual Mode*/
    HI_U32 u32OutThresh;                                          /*shutter time(in us) to judge indoor or outdoor */
    HI_U16 u16LowStart;                                           /*5000K is recommend*/
    HI_U16 u16LowStop;                                            /*4500K is recommend, should be smaller than u8LowStart*/
    HI_U16 u16HighStart;                                          /*6500K is recommend, shoule be larger than u8LowStart*/
    HI_U16 u16HighStop;                                           /*8000K is recommend, should be larger than u8HighStart*/
    HI_BOOL bGreenEnhanceEn;                                      /*If this is enabled, Green channel will be enhanced based on the ratio of green plant*/
    HI_U8   u8OutShiftLimit;                                      /*Max white point zone distance to Planckian Curve*/	
} ISP_AWB_IN_OUT_ATTR_S;

typedef struct hiISP_AWB_CBCR_TRACK_ATTR_S
{
    HI_BOOL bEnable;                          /*If enabled, statistic parameter cr, cb will change according to iso*/
    
    HI_U16  au16CrMax[ISP_AUTO_STENGTH_NUM];  /*RW, Range:[0x0, 0xFFFF], au16CrMax[i] >= au16CrMin[i]*/
    HI_U16  au16CrMin[ISP_AUTO_STENGTH_NUM];  /*RW, Range:[0x0, 0xFFFF]*/ 
    HI_U16  au16CbMax[ISP_AUTO_STENGTH_NUM];  /*RW, Range:[0x0, 0xFFFF], au16CbMax[i] >= au16CbMin[i]*/
    HI_U16  au16CbMin[ISP_AUTO_STENGTH_NUM];  /*RW, Range:[0x0, 0xFFFF]*/
} ISP_AWB_CBCR_TRACK_ATTR_S;

#define AWB_LUM_HIST_NUM     (6)
typedef struct hiISP_AWB_LUM_HISTGRAM_ATTR_S
{
    HI_BOOL bEnable;                          /*If enabled, zone weight to awb is combined with zone luma*/
    ISP_OP_TYPE_E   enOpType;                 /*In auto mode, the weight distribution follows Gaussian distribution*/
    HI_U8   au8HistThresh[AWB_LUM_HIST_NUM];                 /*In manual mode, user define luma thresh, thresh[0] = 0, thresh[5] = 0xFF,  thresh[i] >=  thresh[i-1]*/
    HI_U16  au16HistWt[AWB_LUM_HIST_NUM];                    /*user can define luma weight in both manual and auto mode. Range:[0x0, 0xFFFF]*/
} ISP_AWB_LUM_HISTGRAM_ATTR_S;

typedef struct hiISP_AWB_LIGHTSOURCE_INFO_S
{
    HI_U16 u16WhiteRgain;         /*G/R of White points at this light source, RW, Range:[0x0, 0xFFF]*/
    HI_U16 u16WhiteBgain;         /*G/B of White points at this light source, RW, Range:[0x0, 0xFFF]*/
    HI_U16 u16ExpQuant;           /*shutter time * again * dgain >> 4, Not support Now*/
    HI_U8  u8LightStatus;         /*RW, 0: idle  1:add light source  2:delete sensitive color */
    HI_U8  u8Radius;              /*Radius of light source, RW, Range:[0x0, 0xFF]*/
} ISP_AWB_EXTRA_LIGHTSOURCE_INFO_S;

#define AWB_LS_NUM          (4)
#define AWB_MULTI_CT_NUM    (8)
typedef struct hiISP_AWB_ATTR_EX_S
{  
    HI_U8  u8Tolerance;                                 /*RW, Range:[0x0, 0xFF], AWB adjust tolerance*/   
    HI_U8  u8ZoneRadius;                                /*RW, Range:[0x0, 0xFF], radius of AWB blocks*/  
    HI_U16 u16CurveLLimit;                              /*RW, Range:[0x0, 0x100],   Left limit of AWB Curve, recomend for indoor 0xE0, outdoor 0xE0*/ 
    HI_U16 u16CurveRLimit;                              /*RW, Range:[0x100, 0xFFF], Right Limit of AWB Curve,recomend for indoor 0x130, outdoor 0x120*/ 
 
    HI_BOOL  bExtraLightEn;                             /*Enable special light source function*/
    ISP_AWB_EXTRA_LIGHTSOURCE_INFO_S    stLightInfo[AWB_LS_NUM]; 
    ISP_AWB_IN_OUT_ATTR_S               stInOrOut;

    HI_BOOL bMultiLightSourceEn;                        /*If enabled, awb will do special process in multi light source enviroment*/ 
    ISP_AWB_MULTI_LS_TYPE_E enMultiLSType;              /*Saturation or CCM Tunning*/
    HI_U16  u16MultiLSScaler;                           /*In saturation type, it means the max saturation it can achieve, in ccm type, it means the strenght of multi process. [0x0, 0x100]*/ 
    HI_U16  au16MultiCTBin[AWB_MULTI_CT_NUM];           /*AWB Support divide the color temperature range by 8 bins*/
    HI_U16  au16MultiCTWt[AWB_MULTI_CT_NUM];            /*Weight for different color temperature, same value of 8 means CT weight does't work, [0x0, 0x400]*/

    HI_BOOL bFineTunEn;                                 /*If enabled, skin color scene will be optimized*/
    HI_U8   u8FineTunStrength;                          /*larger value means better performance of skin color scene, but will increase error probability in low color temperature scene */	
} ISP_AWB_ATTR_EX_S;

#define AWB_CURVE_PARA_NUM  (6)
typedef struct hiISP_AWB_ATTR_S
{
    HI_BOOL bEnable;               /*If AWB is disabled, static wb gain will be used, otherwise auto wb gain will be used*/

    HI_U16 u16RefColorTemp;        /*Calibration Information*/
    HI_U16 au16StaticWB[ISP_BAYER_CHN_NUM];        /*Calibration Information, Range:[0x0, 0xFFF]*/
    HI_S32 as32CurvePara[AWB_CURVE_PARA_NUM];       /*Calibration Information, as32CurvePara[3] != 0, as32CurvePara[4]==128*/

    ISP_AWB_ALG_TYPE_E          enAlgType;

    HI_U8  u8RGStrength;                            /*RW, AWB Strength of R Channel, Range: [0x0, 0xFF]*/
    HI_U8  u8BGStrength;                            /*RW, AWB Strength of B Channel, Range: [0x0, 0xFF]*/
    HI_U16 u16Speed;                                /*RW, Convergence speed of AWB, Range:[0x0, 0xFFF] */
    HI_U16 u16ZoneSel;                              /*RW,  A value of 0 or 0xFF means global AWB, A value between 0 and 0xFF means zoned AWB */
    HI_U16 u16HighColorTemp;                        /*RW, AWB max temperature, Recommended: [8500, 10000] */
    HI_U16 u16LowColorTemp;                         /*RW, AWB min temperature, Range: [0x0, u8HighColorTemp), Recommended: [2000, 2500] */
    ISP_AWB_CT_LIMIT_ATTR_S   stCTLimit;
    HI_BOOL bShiftLimitEn;                          /*RW, If enabled, when the statistic information is out of range, it should be project back*/
    HI_U8  u8ShiftLimit;                            /*RW, planckian curve range, Range: [0x0, 0xFF], Recommended: [0x30, 0x50] */
    HI_BOOL bGainNormEn;                            /*RW, if enabled, the min of RGB gain is fixed. */
    HI_BOOL bNaturalCastEn;                         /*RW, if enabled, the color performance will be natural in lowlight and low color temperature*/	

    ISP_AWB_CBCR_TRACK_ATTR_S stCbCrTrack;
    ISP_AWB_LUM_HISTGRAM_ATTR_S stLumaHist;

    HI_BOOL bAWBZoneWtEn;                           /*RW, if enabled, user can set weight for each zones*/
    HI_U8   au8ZoneWt[AWB_ZONE_NUM];                /*Zone Wt Table*/
} ISP_AWB_ATTR_S;

typedef struct hiISP_MWB_ATTR_S        
{
    HI_U16 u16Rgain;            /*RW, Multiplier for R  color channel, Range: [0x0, 0xFFF]*/
    HI_U16 u16Grgain;           /*RW, Multiplier for Gr color channel, Range: [0x0, 0xFFF]*/
    HI_U16 u16Gbgain;           /*RW, Multiplier for Gb color channel, Range: [0x0, 0xFFF]*/
    HI_U16 u16Bgain;            /*RW, Multiplier for B  color channel, Range: [0x0, 0xFFF]*/
} ISP_MWB_ATTR_S;

typedef struct hiISP_WB_ATTR_S
{
    HI_BOOL bByPass;
    HI_U8   u8AWBRunInterval;         /*RW, Range: [0x1, 0xFF], set the AWB run interval*/
    ISP_OP_TYPE_E   enOpType;       /* manual or auto mode */  
    ISP_MWB_ATTR_S  stManual;
    ISP_AWB_ATTR_S  stAuto;
} ISP_WB_ATTR_S;

typedef struct hiISP_COLORMATRIX_MANUAL_S
{  
    HI_BOOL bSatEn;             /*If bSatEn=1, the active CCM = SatMatrix * ManualMatrix, else tha active CCM =  ManualMatrix*/
    HI_U16 au16CCM[CCM_MATRIX_SIZE];          /*RW,  Range: [0x0,  0xFFFF]*/
} ISP_COLORMATRIX_MANUAL_S;

typedef struct hiISP_COLORMATRIX_AUTO_S
{ 
    HI_BOOL bISOActEn;           /*if enabled, CCM will bypass in low light*/
    HI_BOOL bTempActEn;          /*if enabled, CCM will bypass when color temperate is larger than 10K or less than 2500K*/ 
    
    HI_U16 u16HighColorTemp;    /*RW,  Range: <=10000*/
    HI_U16 au16HighCCM[CCM_MATRIX_SIZE];      /*RW,  Range: [0x0,  0xFFFF]*/
    HI_U16 u16MidColorTemp;     /*RW,  the MidColorTemp should be at least 400 smaller than HighColorTemp*/
    HI_U16 au16MidCCM[CCM_MATRIX_SIZE];       /*RW,  Range: [0x0,  0xFFFF]*/
    HI_U16 u16LowColorTemp;     /*RW,  the LowColorTemp should be at least 400 smaller than u16MidColorTemp, >= 2000*/
    HI_U16 au16LowCCM[CCM_MATRIX_SIZE];       /*RW,  Range: [0x0,  0xFFFF]*/
} ISP_COLORMATRIX_AUTO_S;

typedef struct hiISP_COLORMATRIX_ATTR_S
{
    ISP_OP_TYPE_E enOpType;
    ISP_COLORMATRIX_MANUAL_S stManual;
    ISP_COLORMATRIX_AUTO_S stAuto;   
}ISP_COLORMATRIX_ATTR_S;

typedef struct hiISP_SATURATION_MANUAL_S
{
    HI_U8   u8Saturation;        /*RW,  Range: [0, 0xFF] */
} ISP_SATURATION_MANUAL_S;

typedef struct hiISP_SATURATION_AUTO_S
{
    HI_U8   au8Sat[ISP_AUTO_STENGTH_NUM];           /*RW,  Range: [0, 0xFF], should be decreased based on ISO increase */ 
} ISP_SATURATION_AUTO_S;

typedef struct hiISP_SATURATION_ATTR_S
{
    ISP_OP_TYPE_E enOpType;
    ISP_SATURATION_MANUAL_S stManual;
    ISP_SATURATION_AUTO_S stAuto;   
}ISP_SATURATION_ATTR_S;

typedef struct hiISP_COLOR_TONE_ATTR_S
{
    HI_U16 u16RedCastGain;        /*RW, R channel gain after CCM, Range: [0x0, 0xFFF]*/
    HI_U16 u16GreenCastGain;      /*RW, G channel gain after CCM, Range: [0x0, 0xFFF]*/
    HI_U16 u16BlueCastGain;       /*RW, B channel gain after CCM, Range: [0x0, 0xFFF]*/
} ISP_COLOR_TONE_ATTR_S;

typedef struct hiISP_WB_INFO_S
{
    HI_U16 u16Rgain;                /*RO, AWB result of R color channel, Range: [0x0, 0xFFF]*/
    HI_U16 u16Grgain;               /*RO, AWB result of Gr color channel, Range: [0x0, 0xFFF]*/
    HI_U16 u16Gbgain;               /*RO, AWB result of Gb color channel, Range: [0x0, 0xFFF]*/
    HI_U16 u16Bgain;                /*RO, AWB result of B color channel, Range: [0x0, 0xFFF]*/
    HI_U16 u16Saturation;           /*RO, Current saturation, Range:[0x0, 0xFF]*/
    HI_U16 u16ColorTemp;            /*RO, Detect color temperature, maybe out of color cemeprature range*/
    HI_U16 au16CCM[CCM_MATRIX_SIZE];              /*RO, Current color correction matrix*/

    HI_U16 u16LS0CT;                /*RO, color tempearture of primary light source*/
    HI_U16 u16LS1CT;                /*RO, color tempearture of secondary light source*/
    HI_U16 u16LS0Area;              /*RO, area of primary light source*/
    HI_U16 u16LS1Area;              /*RO, area of secondary light source*/
    HI_U8  u8MultiDegree;           /*RO, 0 means uniform light source, larger value means multi light source*/ 
    HI_U32 u32FirstStableTime;      /*RO, AWB first stable frame number */
    ISP_AWB_INDOOR_OUTDOOR_STATUS_E enInOutStatus; 	 /*RO, indoor or outdoor status*/ 
}ISP_WB_INFO_S;


/********************* AF structs ************************/
typedef struct hiISP_AF_ATTR_S
{
    HI_S32 s32DistanceMax; /* the focuse range */
    HI_S32 s32DistanceMin;

    /* weighting table */
    HI_U8 u8Weight[AF_ZONE_ROW][AF_ZONE_COLUMN];
} ISP_AF_ATTR_S;
typedef struct hiISP_MF_ATTR_S
{
    HI_S32 s32DefaultSpeed;     /* 1,default speed(unit:m/s).(onvif)*/    
} ISP_MF_ATTR_S;
typedef struct hiISP_FOCUS_ATTR_S
{
    ISP_OP_TYPE_E  enOpType;
    ISP_MF_ATTR_S  stManual;
    ISP_AF_ATTR_S  stAuto;
} ISP_FOCUS_ATTR_S;

typedef struct hiISP_PRO_MANUAL_PARAM_S
{
    HI_U32 au32ManExpTime[PRO_MAX_FRAME_NUM];/*RW,  sensor exposure time (unit: us ), Range: [0x0, 0xFFFFFFFF], it's related to the specific sensor */
    HI_U32 au32ManSysgain[PRO_MAX_FRAME_NUM];/*RW,  system gain (unit: times, 10bit precision), Range : [0x400, 0xFFFFFFFF], it's related to the specific sensor and ISP Dgain range */
}ISP_PRO_MANUAL_PARAM_S;

typedef struct hiISP_PRO_AUTO_PARAM_S
{
    HI_U16 au16ProExpStep[PRO_MAX_FRAME_NUM];/*RW, Professional exposure step (unit: times, 8bit precision), Range:[0x0, 0xFFFF]*/
}ISP_PRO_AUTO_PARAM_S;

typedef struct hiISP_PRO_PARAM_S
{
    ISP_OP_TYPE_E enOpType;
    HI_U8 u8ProFrameNum;
    ISP_PRO_AUTO_PARAM_S stAuto;
    ISP_PRO_MANUAL_PARAM_S stManual;
}ISP_PRO_PARAM_S;

typedef enum hiISP_SNAP_PIPE_MODE_E
{
    ISP_SNAP_NONE            = 0, /* default mode:no snap*/
    ISP_SNAP_PREVIEW,             /* only preview*/
    ISP_SNAP_PICTURE,             /* only picture*/
    ISP_SNAP_PREVIEW_PICTURE,     /* preview and picture is the same pipe*/
    ISP_SNAP_BUTT
}ISP_SNAP_PIPE_MODE_E;

typedef struct hiISP_SNAP_ATTR_S
{
    SNAP_TYPE_E enSnapType;
    ISP_SNAP_PIPE_MODE_E enSnapPipeMode;
    ISP_PRO_PARAM_S stProParam;
    HI_BOOL bUseInfoCCM;    /*HI_TRUE:use CCM of SnapIspInfo, HI_FALSE: Algorithm calculate */
}ISP_SNAP_ATTR_S;

typedef struct hiISP_DCF_CONST_INFO_S
{
    HI_U8       au8ImageDescription[DCF_DRSCRIPTION_LENGTH];        /*Describes image*/
    HI_U8       au8Make[DCF_DRSCRIPTION_LENGTH];                    /*Shows manufacturer of digital cameras*/
    HI_U8       au8Model[DCF_DRSCRIPTION_LENGTH];                   /*Shows model number of digital cameras*/
    HI_U8       au8Software[DCF_DRSCRIPTION_LENGTH];                /*Shows firmware (internal software of digital cameras) version number*/

    HI_U8       u8LightSource;                                      /*Light source, actually this means white balance setting. '0' means unknown, '1' daylight, '2'
                                                                               fluorescent, '3' tungsten, '10' flash, '17' standard light A, '18' standard light B, '19' standard light
                                                                               C, '20' D55, '21' D65, '22' D75, '255' other*/
    HI_U32      u32FocalLength;                                     /*Focal length of lens used to take image. Unit is millimeter*/
    HI_U8       u8SceneType;                                        /*Indicates the type of scene. Value '0x01' means that the image was directly photographed.*/
    HI_U8       u8CustomRendered;                                   /*Indicates the use of special processing on image data, such as rendering geared to output.
                                                                               0 = Normal process  1 = Custom process   */
    HI_U8       u8FocalLengthIn35mmFilm;                            /*Indicates the equivalent focal length assuming a 35mm film camera, in mm*/
    HI_U8       u8SceneCaptureType;                                 /*Indicates the type of scene that was shot. 0 = Standard,1 = Landscape,2 = Portrait,3 = Night scene. */
    HI_U8       u8GainControl;                                      /*Indicates the degree of overall image gain adjustment. 0 = None,1 = Low gain up,2 = High gain up,3 = Low gain down,4 = High gain down. */
    HI_U8       u8Contrast;                                         /*Indicates the direction of contrast processing applied by the camera when the image was shot.
                                                                               0 = Normal,1 = Soft,2 = Hard */
    HI_U8       u8Saturation;                                       /*Indicates the direction of saturation processing applied by the camera when the image was shot.
                                                                              0 = Normal,1 = Low saturation,2 = High saturation*/
    HI_U8       u8Sharpness;                                        /*Indicates the direction of sharpness processing applied by the camera when the image was shot.
                                                                              0 = Normal,1 = Soft,2 = Hard .*/
    HI_U8       u8MeteringMode;                                     /*Exposure metering method. '0' means unknown, '1' average, '2' center weighted average, '3'
                                                                              spot, '4' multi-spot, '5' multi-segment, '6' partial, '255' other*/
} ISP_DCF_CONST_INFO_S;

#if 0
typedef struct hiISP_UPDATE_INFO_S
{
    HI_U16      u16ISOSpeedRatings;                                 /*CCD sensitivity equivalent to Ag-Hr film speedrate*/
    HI_U32      u32ExposureTime;                                    /*Exposure time (reciprocal of shutter speed).*/
    HI_U32      u32ExposureBiasValue;                               /*Exposure bias (compensation) value of taking picture*/
    HI_U8       u8ExposureProgram;                                  /*Exposure program that the camera used when image was taken. '1' means manual control, '2'
                                                                              program normal, '3' aperture priority, '4' shutter priority, '5' program creative (slow program),
                                                                              '6' program action(high-speed program), '7' portrait mode, '8' landscape mode*/
    HI_U32      u32FNumber;                                         /*The actual F-number (F-stop) of lens when the image was taken*/
    HI_U32      u32MaxApertureValue;                                /*Maximum aperture value of lens.*/
    HI_U8       u8ExposureMode;                                     /*Indicates the exposure mode set when the image was shot.
                                                                              0 = Auto exposure,1 = Manual exposure, 2 = Auto bracket*/
    HI_U8       u8WhiteBalance;                                     /* Indicates the white balance mode set when the image was shot.
                                                                                0 = Auto white balance ,1 = Manual white balance */
} ISP_DCF_UPDATE_INFO_S;
#endif

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */

#endif /* __HI_COMM_ISP_H__ */

