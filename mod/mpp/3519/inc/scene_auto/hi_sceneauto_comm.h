#ifndef __HI_SCENEAUTO_COMM_H__
#define __HI_SCENEAUTO_COMM_H__

#include <pthread.h>
#include "hi_type.h"
#include "hi_comm_vpss.h"
#include "hi_comm_vi.h"
#include "hi_comm_isp.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

#define CHECK_NULL_PTR(ptr)\
    do{\
        if(NULL == ptr)\
        {\
            printf("func:%s,line:%d, NULL pointer\n",__FUNCTION__,__LINE__);\
            return HI_FAILURE;\
        }\
    }while(0);

#define FREE_PTR(ptr)\
	do{\
		if(NULL != ptr)\
		{\
			free(ptr);\
			ptr = NULL;\
		}\
	}while(0);


#define CHECK_MALLOC_SIZE(size)\
    do{\
        if(size < 1)\
        {\
            printf("func:%s,line:%d, Error malloc size: %d\n",__FUNCTION__,__LINE__, size);\
            return HI_FAILURE;\
        }\
    }while(0);


#define MAX_ISP_COUNT 2
#define EXPOSURE_LEVEL	16
#define AE_WEIGHT_ROW   15
#define AE_WEIGHT_COLUMN 17

#define CHECK_ISP_DEV_COUNT(IspDevCount)\
    do{\
        if ((IspDevCount < 1) || (IspDevCount > MAX_ISP_COUNT))\
        {\
            printf("func:%s,line:%d, Error DevCount: %d\n",__FUNCTION__,__LINE__, IspDevCount);\
            return HI_FAILURE;\
        }\
    }while(0);

#define ISP_CHECK_DEV(dev)\
    do {\
        if (((dev) < 0) || ((dev) >= ISP_MAX_DEV_NUM))\
        {\
            printf("func:%s,line:%d, Error Dev: %d\n",__FUNCTION__,__LINE__, dev);\
            return HI_ERR_ISP_ILLEGAL_PARAM;\
        }\
    }while(0)  

typedef struct
{
	int  sf0_bright, sf0_dark;   
    int  sf1_bright, sf1_dark, tf1_md_thresh; 
    int  sf2_bright, sf2_dark, tf2_md_thresh;   
    int  sfk_bright, sfk_dark, sf_ed_thresh; 
    
    int  sfk_profile, _reserved_b_; 
    int  sfk_ratio,   sf_bsc_freq; 
    
    int  tf1_md_type, tf1_still; 
    int  tf2_md_type, tf2_still;   
    
    int  tf1_md_profile; 
    int  tf2_md_profile, tf_profile; 
    
    int  csf_strength, ctf_range;
    int  ctf_strength;
    int  tf1_moving;   
    int  tf2_moving; 
}HI_SCENEAUTO_3DNR_S;

typedef struct
{
	HI_U32 u323DnrIsoCount;
    HI_U32 *pu323DnrIsoThresh;
    HI_SCENEAUTO_3DNR_S *pst3dnrParam;
}HI_SCENEAUTO_3DNRPARAM_S;

typedef struct { int  IES, IET, IEB; } tSceAutoV19yIEy;

typedef struct 
{ 
  int     SBF0;
  int     SBF1,     horPro;
  int     SBF2,     verPro, kProDD , SSLP;
  int     VRTO,     SFkRfw, SFkType, SFkBig;

  int    STH[3], SBS[3], SDS[3];  
} tSceAutoV19ySFy;

typedef struct
{
  int    MATH,  MATE,  MATW;
  int    MASW,  MABW,  MAXN;
} tSceAutoV19yMDy;

typedef struct
{
  int     SFR[4], MTFR[4], TFR[4], STR;
  
  int    MDDZ, TFS,  TFT;
  int    SDZ , TSS, _rb_;
} tSceAutoV19yTFy;

typedef struct
{
   int  SFC;
   int  TFC, _rsvd_b1_;
   int  TPC, _rsvd_b2_;
   int  TRC;
}  tSceAutoV19yNRc;

typedef struct
{
    tSceAutoV19yIEy  IEy;
    tSceAutoV19ySFy  SFy[4];
    tSceAutoV19yMDy  MDy[3];
    tSceAutoV19yTFy  TFy[4];
    tSceAutoV19yNRc  NRc;
}HI_SCENEAUTO_3DNR_X_S;

typedef struct
{
	HI_U32 u323DnrIsoCount;
    HI_U32 *pu323DnrIsoThresh;
    HI_SCENEAUTO_3DNR_X_S *pst3dnrxParam;
}HI_SCENEAUTO_3DNRPARAM_X_S;

typedef struct
{
	HI_U32 u32ISOUpTh;
    HI_U32 u32ISODnTh;
    HI_U32 u32RefExpRatioTh;
    
    HI_U32 u32ExpCount;
    HI_U32* pu32ExpThreshLtoH;
    HI_U32* pu32ExpThreshHtoL;
    HI_U8* pu8ExpCompensation;
    HI_U8* pu8MaxHistOffset;

    HI_U32 u32DRCStrength;

    HI_SCENEAUTO_3DNRPARAM_S stFSWDR3dnr;
}HI_SCENEAUTO_FSWDR_S;

typedef struct
{
	HI_U8   u8ExpCompensation;
    HI_U8   u8MaxHistoffset;
    HI_BOOL bDRCEnable;
    HI_BOOL bDRCManulEnable;
    HI_U32 u32DRCStrengthTarget;
    HI_U32 u32DRCAutoStrength;
    HI_U32 u32WhiteLevel;
    HI_U32 u32BlackLevel;
    HI_U32 u32Asymmetry;
    HI_U32 u32BrightEnhance;
    HI_U8 u8FilterMux;
    HI_U32  u32SlopeMax;
    HI_U32  u32SlopeMin;
    HI_U32  u32VarianceSpace;    
    HI_U32  u32VarianceIntensity;
    HI_U32  u32Contrast;	
	HI_U32  u32BrightPr;	
	HI_U32  u32Svariance;	
	HI_U32  u32DarkEnhance;	
	HI_U8  u8ExpRatioType;
	HI_U32 au32ExpRatio[3];
    HI_U32 u32ExpRatioMax; 
    HI_U32 u32ExpRatioMin; 
    HI_U16 u16GammaTable[257];
    HI_U8 au8TextureSt[EXPOSURE_LEVEL];  		 
    HI_U8 au8EdgeSt[EXPOSURE_LEVEL];  		 
    HI_U8 au8OverShoot[EXPOSURE_LEVEL];		 
    HI_U8 au8UnderShoot[EXPOSURE_LEVEL];
    HI_U8 au8TextureThd[EXPOSURE_LEVEL];
    HI_U8 au8EdgeThd[EXPOSURE_LEVEL];
    HI_U8 au8DetailCtrl[EXPOSURE_LEVEL];
    HI_SCENEAUTO_3DNRPARAM_S stTraffic3dnr;
}HI_SCENEAUTO_TRAFFICPARAM_S;

typedef struct
{
    HI_U8 au8TextureSt[EXPOSURE_LEVEL];  		 
    HI_U8 au8EdgeSt[EXPOSURE_LEVEL];  		 
    HI_U8 au8OverShoot[EXPOSURE_LEVEL];		 
    HI_U8 au8UnderShoot[EXPOSURE_LEVEL];
    HI_U8 au8TextureThd[EXPOSURE_LEVEL];
    HI_U8 au8EdgeThd[EXPOSURE_LEVEL];
    HI_U8 au8DetailCtrl[EXPOSURE_LEVEL];
    HI_U16 u16GammaTable[257];
    HI_SCENEAUTO_3DNRPARAM_X_S stFace3dnrx;
}HI_SCENEAUTO_FACEPARAM_S;

typedef struct
{
	HI_U8  u8ExpRatioType;
	HI_U32 au32ExpRatio[3];
    HI_U32 u32ExpRatioMax; 
    HI_U32 u32ExpRatioMin;
	HI_U8 u8ExpCompensation;
	HI_U16 u16BlackDelayFrame;
    HI_U16 u16WhiteDelayFrame;
    HI_U8 u8Speed;
    HI_U16 u16HistRatioSlope;
    HI_U8 u8MaxHistOffset;
    HI_U8 u8Tolerance;    
    HI_BOOL bDCIEnable;
    HI_U32 u32DCIBlackGain;
    HI_U32 u32DCIContrastGain;
    HI_U32 u32DCILightGain;
    HI_BOOL bDRCEnable;
    HI_BOOL bDRCManulEnable;
    HI_U32 u32DRCStrengthTarget;
    HI_U32 u32DRCAutoStrength;
    HI_U32 u32WhiteLevel;
    HI_U32 u32BlackLevel;
    HI_U32 u32Asymmetry;
    HI_U32 u32BrightEnhance;
    HI_U8 u8FilterMux;
    HI_U32  u32SlopeMax;
    HI_U32  u32SlopeMin;
    HI_U32  u32VarianceSpace;    
    HI_U32  u32VarianceIntensity;
    HI_U32  u32Contrast;	
	HI_U32  u32BrightPr;	
	HI_U32  u32Svariance;	
	HI_U32  u32DarkEnhance;	
	HI_U8 u8Saturation[16];  
    HI_U16 u16GammaTable[257];
    HI_SCENEAUTO_3DNRPARAM_S stHlc3dnr;
}HI_SCENEAUTO_HLCPARAM_S;

typedef struct
{
	HI_U32 u32ExpCount;
    HI_U32* pu32ExpThreshLtoH;
    HI_U32* pu32ExpThreshHtoL;
    HI_U8* pu8ExpCompensation;
    HI_U8* pu8MaxHistOffset;
    HI_U16 u16HistRatioSlope;
    HI_U16 u16BlackDelayFrame;
    HI_U16 u16WhiteDelayFrame;
    HI_U8 u8Tolerance;
    HI_U8 u8Speed;    
    HI_BOOL bDCIEnable;
    HI_U32 u32DCIBlackGain;
    HI_U32 u32DCIContrastGain;
    HI_U32 u32DCILightGain;
    HI_U16 u16Slope;
    HI_U16 au16LumThresh[EXPOSURE_LEVEL];
    HI_U8 au8SharpenD[EXPOSURE_LEVEL];
    HI_U8 au8SharpenUd[EXPOSURE_LEVEL];
    HI_U8 au8TextureSt[EXPOSURE_LEVEL];
    HI_U8 au8EdgeSt[EXPOSURE_LEVEL];
    HI_U8 au8OverShoot[EXPOSURE_LEVEL]; 
    HI_U8 au8UnderShoot[EXPOSURE_LEVEL];
    HI_U16 u16GammaTable[257];
    HI_U8 au8Weight[AE_WEIGHT_ROW][AE_WEIGHT_COLUMN];
    HI_SCENEAUTO_3DNRPARAM_S stIr3dnr;
}HI_SCENEAUTO_IRPARAM_S;


typedef struct
{
	VPSS_GRP VpssGrp;
    HI_U32 u32NrSel;    /* 0: 3DNR S; 1: 3DNR X */
	HI_SCENEAUTO_3DNRPARAM_S st3dnrParam;
    HI_SCENEAUTO_3DNRPARAM_X_S st3dnrParamX;
}HI_SCENEAUTO_VPSSPARAM_S;

typedef struct
{
	HI_BOOL bDCIEnable;
    HI_U32 u32DCIBlackGain;
    HI_U32 u32DCIContrastGain;
    HI_U32 u32DCILightGain;
}HI_SCENAUTO_DCIPARAM_S;

typedef struct
{
	VI_DEV ViDev;
	HI_SCENAUTO_DCIPARAM_S stDciParam;
}HI_SCENEAUTO_VIPARAM_S;

typedef struct
{
	HI_U8 u8CurveType;
    HI_U16 u16Table[257];
}HI_SCENEAUTO_GAMMA_S;

typedef struct
{
	HI_U32 u32ExpCount;
    HI_U32 u32DelayCount;
    HI_U32 u32Interval;
    HI_U32* pu32ExpThreshLtoH;
    HI_U32* pu32ExpThreshHtoL;
    HI_SCENEAUTO_GAMMA_S* pstGamma;
}HI_SCENEAUTO_GAMMAPARAM_S;

typedef struct
{
	HI_U8 u8Strength;
}HI_SCENEAUTO_FALSECOLOR_S;

typedef struct
{
	HI_U32 u32ExpCount;
    HI_U32* pu32ExpThresh;
    HI_SCENEAUTO_FALSECOLOR_S* pstFalseColor;
}HI_SCENEAUTO_FALSECOLORPARAM_S;

typedef struct
{
	HI_U16 u16Slope;
    HI_U16 u16Thresh;
}HI_SCENEAUTO_DPATTR_S;

typedef struct
{
	HI_U8 u8AwbType;
    HI_U8 u8CcmType;
}HI_SCENEAUTO_COLOR_S;

typedef struct
{
	HI_U32 u32ExpCount;
    HI_U32* pu32ExpThresh;
    HI_SCENEAUTO_DPATTR_S* pstDPAttr;
}HI_SCENEAUTO_DPPARAM_S;

typedef struct
{
    HI_U16 u16LumThresh;
    HI_U8 u8SharpenD;
    HI_U8 u8SharpenUd;
    HI_U8 u8TextureSt;
    HI_U8 u8EdgeSt; 
    HI_U8 u8OverShoot;
    HI_U8 u8UnderShoot;
    HI_U8 u8EdgeThd;
    HI_U8 u8TextureThd;
    HI_U8 u8DetailCtrl;	 		        		       
}HI_SCENEAUTO_SHARPEN_S;

typedef struct
{
	HI_U32 u32ExpCount;
    HI_U32* pu32ExpThresh;
    HI_SCENEAUTO_SHARPEN_S *pstDynamicSharpen;
    HI_SCENEAUTO_SHARPEN_S astDefaultSharpen[EXPOSURE_LEVEL];    
}HI_SCENEAUTO_SHARPENPARAM_S;

typedef struct
{
	HI_U8   u8UuSlope;
    HI_U8   u8VaSlope;
    HI_U8   u8AaSlope;
    HI_U8   u8VhSlope; 
}HI_SCENEAUTO_DEMOSAIC_S;

typedef struct
{
	HI_U32 u32ExpCount;
    HI_U32* pu32ExpThresh;
    HI_SCENEAUTO_DEMOSAIC_S* pstDemosaic;
}HI_SCENEAUTO_DEMOSAICPARAM_S;

typedef struct
{
	HI_BOOL bEnable;
    HI_U8  u8OpType;
    HI_U32 u32Strength;
    HI_U32 u32DRCAutoStrength;
    HI_U32 u32WhiteLevel;
    HI_U32 u32BlackLevel;
    HI_U32 u32Asymmetry;
    HI_U32 u32BrightEnhance;
    HI_U8  u8FilterMux;
    HI_U32 u32SlopeMax;
    HI_U32 u32SlopeMin;
    HI_U32 u32VarianceSpace;    
    HI_U32 u32VarianceIntensity;
    HI_U32 u32Contrast;	
	HI_U32 u32BrightPr;	
	HI_U32 u32Svariance;	
	HI_U32 u32DarkEnhance;
}HI_SCENEAUTO_DRC_S;

typedef struct
{
	HI_U32 u32DRCount;
    HI_U32* pu32DRThresh;
    HI_SCENEAUTO_DRC_S* pstDRCAttr;
}HI_SCENEAUTO_DRCPARAM_S;

typedef struct
{
	HI_U8 u8AECompesation;
    HI_U8 u8AEHistOffset;
}HI_SCENEAUTO_AERELATEDEXP_S;

typedef struct
{
	HI_U8  u8Speed;
    HI_U8  u8Tolerance;
    HI_U16 u16BlackDelayFrame;
    HI_U16 u16WhiteDelayFrame;
    HI_U32 u32SysGainMax;
}HI_SCENEAUTO_AERELATEDBIT_S;

typedef struct
{
	HI_U32  u32IntTime;
    HI_U32  u32SysGain;  
}HI_SCENEAUTO_ROUTENODE_S;

typedef struct
{
	HI_U32 u32TotalNum;
	HI_SCENEAUTO_ROUTENODE_S *pstRuteNode;
}HI_SCENEAUTO_AEROUTE_S;

typedef struct
{
	HI_U8 u8AERunInterval;
	HI_SCENEAUTO_AEROUTE_S stAeRoute;
    HI_U32 u32BitrateCount;
    HI_U32* pu32BitrateThresh;
    HI_SCENEAUTO_AERELATEDBIT_S* pstAERelatedBit;
    HI_U32 u32ExpCount;
    HI_U32* pu32AEExpHtoLThresh;
    HI_U32* pu32AEExpLtoHThresh;
    HI_SCENEAUTO_AERELATEDEXP_S* pstAERelatedExp;
}HI_SCENEAUTO_AEPARAM_S;

typedef struct
{
	ISP_DEV IspDev;
	HI_SCENEAUTO_AEPARAM_S stAeParam;
	HI_SCENEAUTO_DRCPARAM_S stDrcParam;
	HI_SCENEAUTO_DEMOSAICPARAM_S stDemosaicParam;
	HI_SCENEAUTO_SHARPENPARAM_S stSharpenParam;
	HI_SCENEAUTO_DPPARAM_S stDpParam;
	HI_SCENEAUTO_FALSECOLORPARAM_S stFalseColorParam;
	HI_SCENEAUTO_GAMMAPARAM_S stGammaParam;
}HI_SCENEAUTO_ISPPARAM_S;

typedef struct
{
	HI_S32 s32IspDevCount;
	HI_S32 s32ViDevCount;
	HI_S32 s32VpssGrpCount;
	HI_U32 u32AveLumThresh;
	HI_U32 u32DeltaDisExpThresh;
	HI_U32 u32DRCStrengthThresh;
}HI_SCENEAUTO_COMMPARAM_S;

typedef struct
{
	HI_SCENEAUTO_COMMPARAM_S stCommParam;
	HI_SCENEAUTO_ISPPARAM_S *pstIspParam;
	HI_SCENEAUTO_VIPARAM_S *pstViParam;
	HI_SCENEAUTO_VPSSPARAM_S *pstVpssParam;
	HI_SCENEAUTO_IRPARAM_S stIrParam;
	HI_SCENEAUTO_HLCPARAM_S stHlcParam;
	HI_SCENEAUTO_TRAFFICPARAM_S stTrafficParam;
	HI_SCENEAUTO_FSWDR_S stFswdrParam;
    HI_SCENEAUTO_FACEPARAM_S stFaceParam;
}HI_SCENEAUTO_PARAM_S;

typedef struct
{
	HI_BOOL bThreadFlag;
	pthread_t pThread;
}HI_SCENEAUTO_THREAD_S;

typedef struct
{
	HI_U8  u8ExpRatioType;
	HI_U32 au32ExpRatio[3];
    HI_U32 u32ExpRatioMax; 
    HI_U32 u32ExpRatioMin; 
}HI_SCENEAUTO_WDREXPOSURE_S;

typedef struct
{
	HI_U8 u8OpType;
    HI_U8 u8Saturation[16];
}HI_SCENEAUTO_SATURATION_S;

typedef struct
{
	HI_U8 u8AERunInterval;
    HI_U8  u8Speed;
    HI_U8  u8Tolerance;
    HI_U16 u16BlackDelayFrame;
    HI_U16 u16WhiteDelayFrame;
    HI_U32 u32SysGainMax;
    HI_U8 u8AECompesation;
    HI_U8 u8AEHistOffset;
    HI_U16 u16HistRatioSlope;
    HI_U8 au8Weight[AE_WEIGHT_ROW][AE_WEIGHT_COLUMN];
    HI_S32 s32TotalNum;
    HI_U8 u8FSWDRLFMode;
    HI_SCENEAUTO_ROUTENODE_S astRouteNode[16];
}HI_SCENEAUTO_EXPOSURE_S;

typedef struct
{
	HI_SCENAUTO_DCIPARAM_S stDci;
}HI_SCENEAUTO_PREVIPARAM_S;

typedef struct
{
	HI_SCENEAUTO_DEMOSAIC_S stDemosaic;
	HI_SCENEAUTO_SHARPEN_S stSharpen[EXPOSURE_LEVEL];
	HI_SCENEAUTO_GAMMA_S stGamma;
	HI_SCENEAUTO_EXPOSURE_S stExposure;
	HI_SCENEAUTO_SATURATION_S stSaturation;
	HI_SCENEAUTO_DRC_S stDrc;
	HI_SCENEAUTO_WDREXPOSURE_S stWdrExposureAttr;
	HI_SCENEAUTO_DPATTR_S stDp;
    HI_SCENEAUTO_COLOR_S stColor;
}HI_SCENEAUTO_PREISPPARAM_S;

typedef struct
{
	HI_BOOL bSceneautoInit;
	HI_BOOL bSceneautoStart;
	HI_SCENEAUTO_THREAD_S stThreadNormal;
	HI_SCENEAUTO_THREAD_S stThreadSpecial;
	HI_SCENEAUTO_THREAD_S stThreadDrc;
   	HI_SCENEAUTO_THREAD_S stThread3Dnr;
	HI_SCENEAUTO_PREISPPARAM_S *pstPreviousIspParam;
	HI_SCENEAUTO_PREVIPARAM_S *pstPrevisouViParam;
}HI_SCENEAUTO_STATE_S;



HI_S32 Sceneauto_LoadFile(const HI_CHAR* pszFILENAME);
HI_S32 Sceneauto_LoadINIPara();
HI_VOID Sceneauto_FreeDict();
HI_VOID Sceneauto_FreeMem();



#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif
