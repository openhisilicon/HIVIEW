/*****************************************************************************
  Hisilicon Hi35xx sample programs head file.

  Copyright (C), 2010-2011, Hisilicon Tech. Co., Ltd.
 ******************************************************************************
    Modification:  2011-2 Created
******************************************************************************/

#ifndef __SAMPLE_COMM_H__
#define __SAMPLE_COMM_H__

#include <sys/sem.h>
#include <sys/prctl.h>
#include "hi_common.h"
#include "hi_comm_sys.h"
#include "hi_comm_vb.h"
#include "hi_comm_vi.h"
#include "hi_comm_vo.h"
#include "hi_comm_venc.h"
#include "hi_comm_vpss.h"
#include "hi_comm_vdec.h"
#include "hi_comm_region.h"
#include "hi_comm_adec.h"
#include "hi_comm_aenc.h"
#include "hi_comm_ai.h"
#include "hi_comm_ao.h"
#include "hi_comm_aio.h"
#include "hi_comm_hdmi.h"
#include "hi_defines.h"

#include "mpi_sys.h"
#include "mpi_vb.h"
#include "mpi_vi.h"
#include "mpi_vo.h"
#include "mpi_venc.h"
#include "mpi_vpss.h"
#include "mpi_vdec.h"
#include "mpi_region.h"
#include "mpi_adec.h"
#include "mpi_aenc.h"
#include "mpi_ai.h"
#include "mpi_ao.h"
#include "mpi_hdmi.h"

#include "tlv320aic31.h"
#include "loadbmp.h"

#define FILE_NAME_LEN               64

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* Begin of #ifdef __cplusplus */

/*******************************************************
    macro define 
*******************************************************/
#define CHECK_CHN_RET(express,Chn,name)\
	do{\
		HI_S32 Ret;\
		Ret = express;\
		if (HI_SUCCESS != Ret)\
		{\
			printf("\033[0;31m%s chn %d failed at %s: LINE: %d with %#x!\033[0;39m\n", name, Chn, __FUNCTION__, __LINE__, Ret);\
			fflush(stdout);\
			return Ret;\
		}\
	}while(0)

#define CHECK_RET(express,name)\
    do{\
        HI_S32 Ret;\
        Ret = express;\
        if (HI_SUCCESS != Ret)\
        {\
            printf("\033[0;31m%s failed at %s: LINE: %d with %#x!\033[0;39m\n", name, __FUNCTION__, __LINE__, Ret);\
            return Ret;\
        }\
    }while(0)

//#define SAMPLE_GLOBAL_NORM	    VIDEO_ENCODING_MODE_PAL
#define SAMPLE_PIXEL_FORMAT         PIXEL_FORMAT_YUV_SEMIPLANAR_420

#define TW2865_FILE "/dev/tw2865dev"
#define TP2823_FILE "/dev/tp2823dev"
#define TLV320_FILE "/dev/tlv320aic31"
#define NVP6134_FILE "/dev/nc_vdec"

#define SAMPLE_VO_DEV_DHD0  0
#define SAMPLE_VO_DEV_DHD1  1

#define SAMPLE_VO_LAYER_VHD0 0
#define SAMPLE_VO_LAYER_VPIP 1

#define VO_LAYER_PIP        1
#define VO_LAYER_PIP_STA	1
#define VO_LAYER_PIP_END	2
#define VO_DEV_HD_END	    2


#define SAMPLE_VO_WBC_BASE  0
#define SAMPLE_VO_LAYER_PRIORITY_BASE 0
#define SAMPLE_VO_LAYER_PRIORITY_PIP 1
//#define GRAPHICS_LAYER_HC0 3

#define SAMPLE_AUDIO_PTNUMPERFRM   320
#define SAMPLE_AUDIO_TLV320_AI_DEV 2
#define SAMPLE_AUDIO_TLV320_AO_DEV 0


#define SAMPLE_AUDIO_INNER_HDMI_AO_DEV 1
#define SAMPLE_AUDIO_HDMI_AO_DEV 1
#define SAMPLE_AUDIO_AI_DEV 0//maohw AI_DEV
#define SAMPLE_AUDIO_AO_DEV 0//maohw AO_DEV

#define SAMPLE_CIF_H264_PATH "../common/CIF.h264"
#define SAMPLE_1080P_H264_PATH "../common/1080P.h264"
#define SAMPLE_1080P_H265_PATH "../common/1080P.h265"
#define SAMPLE_4K_H264_PATH "../common/tmp1"
#define SAMPLE_4K_H265_PATH "../common/tmp2"
#define SAMPLE_1080P_MPEG4_PATH "../common/1080P.mpeg4"
#define SAMPLE_FIELD_H264_PATH "../common/D1_field.h264"
#define SAMPLE_1080P_JPEG_PATH "../common/1080P.jpg"
#define SAMPLE_4K_JPEG_PATH "../common/tmp3"
#define SAMPLE_D1_USERPIC_PATH "../common/UserPic_960x576.yuv"


#define SAMPLE_MAX_VDEC_CHN_CNT 8


#define ALIGN_UP(x, a)              ((x+a-1)&(~(a-1)))
#define ALIGN_BACK(x, a)              ((a) * (((x) / (a))))

#define SAMPLE_SYS_ALIGN_WIDTH  16		
#define VO_BKGRD_BLUE           0x0000FF

#define HD_WIDTH                1920
#define HD_HEIGHT               1080
#define _4K_WIDTH                3840
#define _4K_HEIGHT               2160

#define D1_WIDTH                720
#define D1_HEIGHT               576

#define _720P_WIDTH              1280
#define _720P_HEIGHT             720

#define SAMPLE_PRT(fmt...)   \
    do {\
        printf("[%s]-%d: ", __FUNCTION__, __LINE__);\
        printf(fmt);\
       }while(0)


typedef enum sample_vi_mode_e
{


    SAMPLE_VI_MODE_16_D1,
    SAMPLE_VI_MODE_16_960H,
    SAMPLE_VI_MODE_16_1280H,
    SAMPLE_VI_MODE_16_HALF720P,
    
    SAMPLE_VI_MODE_8_720P,
    SAMPLE_VI_MODE_16_720P,

    SAMPLE_VI_MODE_4_1080P,
    SAMPLE_VI_MODE_8_1080P,
    SAMPLE_VI_MODE_16_1080P,

    SAMPLE_VI_MODE_4_3M,
    SAMPLE_VI_MODE_4_4M,
    
    SAMPLE_VI_MODE_32_D1,
    SAMPLE_VI_MODE_32_960H,
    SAMPLE_VI_MODE_32_1280H,
    SAMPLE_VI_MODE_32_HALF720P,
    
    SAMPLE_VI_MODE_1_4Kx2K,
   
    SAMPLE_VI_MODE_4_4Kx2K,
    SAMPLE_VI_MODE_6134_960H,
	SAMPLE_VI_MODE_6134_HDX,   //640x720
	SAMPLE_VI_MODE_6134_HD,    //720p
	SAMPLE_VI_MODE_6134_FHDX,  //960x1080
	SAMPLE_VI_MODE_6134_FHD,   //1080p
    SAMPLE_VI_MODE_960H_720P_2MUX,
    SAMPLE_VI_MODE_6134_2MUX_FHD,   //1080p
    SAMPLE_VI_MODE_6134_4MUX_HD,   //4mux 720p
}SAMPLE_VI_MODE_E;

typedef enum nvp6134_vi_mode
{
		NVP6134_VI_720H 		= 0x00, //720x576i(480)
		NVP6134_VI_960H,				//960x576i(480)
		NVP6134_VI_1280H,				//1280x576i(480)
		NVP6134_VI_1440H,				//1440x576i(480)
		NVP6134_VI_1920H,				//1920x576i(480)
		NVP6134_VI_960H2EX, 			//3840x576i(480)
		NVP6134_VI_720P_2530	= 0x10, //1280x720@25p(30)
		NVP6134_VI_EXC_720P,			//1280x720@25p(30)
		NVP6134_VI_EXT_720PA,			//1280x720@25p(30)	
		NVP6134_VI_EXT_720PB,			//1280x720@25p(30)
		NVP6134_VI_HDEX,				//2560x720@25p(30)
		NVP6134_VI_EXC_HDEX,			//2560x720@25p(30)
		NVP6134_VI_EXT_HDAEX,			//2560x720@25p(30)
		NVP6134_VI_EXT_HDBEX,			//2560x720@25p(30)
		NVP6134_VI_720P_5060,			//1280x720@50p(60)
		NVP6134_VI_EXC_720PRT,			//1280x720@50p(60)
		NVP6134_VI_EXT_720PRT,			//1280x720@50p(60)
		
		NVP6134_VI_1080P_2530	= 0x20, //1920x1080@25p(30)
		NVP6134_VI_EXC_1080P,			//1920x1080@25p(30)
		NVP6134_VI_EXT_1080P,			//1920x1080@25p(30)
		NVP6134_VI_1080P_NRT,			//1920x1080@12.5p(15)
		NVP6134_VI_1080P_NOVIDEO,		//1920x1080@25p(30)
		NVP6134_VI_3M_NRT		= 0x30, //2048x1536@18p
		NVP6134_VI_3M,					//2048x1536@25p(30p)
		NVP6134_VI_EXT_3M_NRT,			//2048x1536@18p
		NVP6134_VI_4M_NRT		= 0x40, //2560x1440@NRT
		NVP6134_VI_4M,					//2560x1440@25p(30)
		NVP6134_VI_5M_NRT		= 0x50, //2592x1944@12.5P
		NVP6134_VI_5M,					//2592x1944@25P 
		NVP6134_VI_EXT_5M_NRT,			//2592x1944@12.5P
		NVP6134_VI_5M_20P,				//2592x1944@20P
		NVP6134_VI_BUTT
}NVP6134_VI_MODE;


typedef enum _nvp6134_outmode_sel
{
	NVP6134_OUTMODE_1MUX_SD = 0,
	NVP6134_OUTMODE_1MUX_HD,
	NVP6134_OUTMODE_1MUX_HD5060,
	NVP6134_OUTMODE_1MUX_FHD,
	NVP6134_OUTMODE_2MUX_SD,
	NVP6134_OUTMODE_2MUX_HD_X,
	NVP6134_OUTMODE_2MUX_HD,
	NVP6134_OUTMODE_2MUX_FHD_X,
	NVP6134_OUTMODE_4MUX_SD,
	NVP6134_OUTMODE_4MUX_HD_X,
	NVP6134_OUTMODE_4MUX_HD,
	NVP6134_OUTMODE_2MUX_FHD,
	NVP6134_OUTMODE_1MUX_HD_X,  
	NVP6134_OUTMODE_1MUX_FHD_X,
	NVP6134_OUTMODE_4MUX_FHD_X,
	NVP6134_OUTMODE_4MUX_MIX,
	NVP6134_OUTMODE_2MUX_MIX,
	NVP6134_OUTMODE_1MUX_BT1120S,
	NVP6134_OUTMODE_1MUX_3M_RT,
	NVP6134_OUTMODE_1MUX_4M_NRT,
	NVP6134_OUTMODE_BUTT
}NVP6134_OUTMODE_SEL;

typedef struct sample_vi_param_s
{
    HI_S32 s32ViDevCnt;        // VI Dev Total Count    
    HI_S32 s32ViDevInterval;   // Vi Dev Interval    
    HI_S32 s32ViChnCnt;        // Vi Chn Total Count    
    HI_S32 s32ViChnInterval;   // VI Chn Interval
}SAMPLE_VI_PARAM_S;

typedef enum sample_vi_chn_set_e
{
    VI_CHN_SET_NORMAL = 0, /* mirror, filp close */ 
    VI_CHN_SET_MIRROR,      /* open MIRROR */   
    VI_CHN_SET_FILP        /* open filp */
}SAMPLE_VI_CHN_SET_E;

typedef enum sample_vo_mode_e
{
    VO_MODE_1MUX  = 0,
    VO_MODE_4MUX = 1,
    VO_MODE_9MUX = 2,
    VO_MODE_16MUX = 3,    
    VO_MODE_BUTT
}SAMPLE_VO_MODE_E;

typedef struct hisample_MEMBUF_S
{
    VB_BLK  hBlock;
    VB_POOL hPool;
    HI_U32  u32PoolId;
    
    HI_U32  u32PhyAddr;
    HI_U8   *pVirAddr;
    HI_S32  s32Mdev;
} SAMPLE_MEMBUF_S;

typedef enum sample_rc_e
{
    SAMPLE_RC_CBR = 0,
    SAMPLE_RC_VBR,
    SAMPLE_RC_AVBR,
    SAMPLE_RC_FIXQP
}SAMPLE_RC_E;

typedef enum sample_rgn_change_type_e
{
    RGN_CHANGE_TYPE_FGALPHA = 0,
    RGN_CHANGE_TYPE_BGALPHA,
    RGN_CHANGE_TYPE_LAYER
}SAMPLE_RGN_CHANGE_TYPE_EN;

typedef struct sample_vo_param_s
{
    VO_DEV VoDev;
    HI_CHAR acMmzName[20];
    HI_U32 u32WndNum;
    SAMPLE_VO_MODE_E enVoMode;
    VO_PUB_ATTR_S stVoPubAttr;
    HI_BOOL bVpssBind;
}SAMPLE_VO_PARAM_S;

typedef struct sample_venc_getstream_s
{
     HI_BOOL bThreadStart;
     HI_S32  s32Cnt;
}SAMPLE_VENC_GETSTREAM_PARA_S;

typedef enum 
{
    HIFB_LAYER_0 = 0x0,
    HIFB_LAYER_1,
    HIFB_LAYER_2,
    HIFB_LAYER_CURSOR_0,
    HIFB_LAYER_ID_BUTT
} HIFB_LAYER_ID_E;

typedef enum hiVdecThreadCtrlSignal_E
{
	VDEC_CTRL_START,
	VDEC_CTRL_PAUSE,
	VDEC_CTRL_STOP,	
}VdecThreadCtrlSignal_E;
	
typedef struct hiVdecThreadParam
{
	HI_S32 s32ChnId;	
	PAYLOAD_TYPE_E enType;
	HI_CHAR cFileName[100];
	HI_S32 s32StreamMode;
	HI_S32 s32MilliSec;
	HI_S32 s32MinBufSize;
	HI_S32 s32IntervalTime;
	VdecThreadCtrlSignal_E eCtrlSinal;
	HI_U64	u64PtsInit;
	HI_U64	u64PtsIncrease;
    HI_BOOL bLoopSend;
    HI_BOOL bManuSend;
    HI_CHAR cUserCmd;
}VdecThreadParam;

typedef enum hiAudioCodecType
{
    AUDIO_CODEC_INNER = 0,
    AUDIO_CODEC_TLV320,
    AUDIO_CODEC_HDMI,
    AUDIO_CODEC_TW2865,
    AUDIO_CODEC_BUTT
}AudioCodecType;



/*******************************************************
    function announce  
*******************************************************/
HI_S32 SAMPLE_COMM_SYS_GetPicSize(VIDEO_NORM_E enNorm, PIC_SIZE_E enPicSize, SIZE_S *pstSize);
HI_U32 SAMPLE_COMM_SYS_CalcPicVbBlkSize(VIDEO_NORM_E enNorm, PIC_SIZE_E enPicSize, PIXEL_FORMAT_E enPixFmt, HI_U32 u32AlignWidth,COMPRESS_MODE_E enCompFmt);
HI_S32 SAMPLE_COMM_SYS_MemConfig(HI_VOID);
HI_VOID SAMPLE_COMM_SYS_Exit(void);
HI_S32 SAMPLE_COMM_SYS_Init(VB_CONF_S *pstVbConf);
HI_S32 SAMPLE_COMM_SYS_Payload2FilePostfix(PAYLOAD_TYPE_E enPayload, HI_CHAR* szFilePostfix);

HI_S32 SAMPLE_COMM_VI_Mode2Param(SAMPLE_VI_MODE_E enViMode, SAMPLE_VI_PARAM_S *pstViParam);
HI_S32 SAMPLE_COMM_VI_Mode2Size(SAMPLE_VI_MODE_E enViMode, VIDEO_NORM_E enNorm, RECT_S *pstCapRect, SIZE_S *pstDestSize);
VI_DEV SAMPLE_COMM_VI_GetDev(SAMPLE_VI_MODE_E enViMode, VI_CHN ViChn);
HI_S32 SAMPLE_COMM_VI_StartDev(VI_DEV ViDev, SAMPLE_VI_MODE_E enViMode);
HI_S32 SAMPLE_COMM_VI_ChnBindDev(VI_CHN ViChn, SAMPLE_VI_MODE_E enViMode);
HI_S32 SAMPLE_COMM_VI_StartChn(VI_CHN ViChn, RECT_S *pstCapRect, SIZE_S *pstTarSize,     SAMPLE_VI_MODE_E enViMode, SAMPLE_VI_CHN_SET_E enViChnSet);
HI_S32 SAMPLE_COMM_VI_Start(SAMPLE_VI_MODE_E enViMode, VIDEO_NORM_E enNorm);
HI_S32 SAMPLE_COMM_VI_Stop(SAMPLE_VI_MODE_E enViMode);
HI_S32 SAMPLE_COMM_VI_BindVpss(SAMPLE_VI_MODE_E enViMode);
HI_S32 SAMPLE_COMM_VI_UnBindVpss(SAMPLE_VI_MODE_E enViMode);
HI_S32 SAMPLE_COMM_VI_BindVo(VI_CHN ViChn, VO_LAYER VoLayer, VO_CHN VoChn);
HI_S32 SAMPLE_COMM_VI_UnBindVo(VI_CHN ViChn, VO_LAYER VoLayer, VO_CHN VoChn);

HI_VOID	SAMPLE_COMM_VDEC_Sysconf(VB_CONF_S *pstVbConf, SIZE_S *pstSize);
HI_VOID	SAMPLE_COMM_VDEC_ModCommPoolConf(VB_CONF_S *pstModVbConf, 
    PAYLOAD_TYPE_E enType, SIZE_S *pstSize, HI_S32 s32ChnNum, HI_BOOL bCompress);
HI_S32	SAMPLE_COMM_VDEC_InitModCommVb(VB_CONF_S *pstModVbConf);
HI_VOID	SAMPLE_COMM_VDEC_ChnAttr(HI_S32 s32ChnNum, VDEC_CHN_ATTR_S *pstVdecChnAttr, PAYLOAD_TYPE_E enType, SIZE_S *pstSize);
HI_VOID	SAMPLE_COMM_VDEC_VpssGrpAttr(HI_S32 s32ChnNum, VPSS_GRP_ATTR_S *pstVpssGrpAttr, SIZE_S *pstSize);
HI_VOID	SAMPLE_COMM_VDEC_VoAttr(HI_S32 s32ChnNum, VO_DEV VoDev ,VO_PUB_ATTR_S *pstVoPubAttr, VO_VIDEO_LAYER_ATTR_S *pstVoLayerAttr);
HI_VOID SAMPLE_COMM_VDEC_ThreadParam(HI_S32 s32ChnNum, VdecThreadParam *pstVdecSend, VDEC_CHN_ATTR_S *pstVdecChnAttr, char *pStreamFileName);
HI_S32 Sample_COMM_VPSS_StartCover(HI_S32 VpssGrp);
HI_VOID Sample_COMM_SetCrop(HI_S32 VpssGrp);
HI_VOID Sample_MST_GetDefVoAttr(VO_DEV VoDev, VO_PUB_ATTR_S *pstPubAttr, VO_VIDEO_LAYER_ATTR_S *pstLayerAttr,
        HI_S32 s32SquareSort, VO_CHN_ATTR_S *astChnAttr);
HI_VOID Sample_MST_StartPIPVideoLayer(VO_DEV VoDev, VO_VIDEO_LAYER_ATTR_S *pstLayerAttr,
    VO_CHN_ATTR_S *astChnAttr, HI_S32 s32ChnNum);
HI_VOID SAMPLE_COMM_VDEC_CmdCtrl(HI_S32 s32ChnNum,VdecThreadParam *pstVdecSend);
HI_VOID SAMPLE_COMM_VDEC_StartSendStream(HI_S32 s32ChnNum, VdecThreadParam *pstVdecSend, pthread_t *pVdecThread);
HI_VOID SAMPLE_COMM_VDEC_StopSendStream(HI_S32 s32ChnNum, VdecThreadParam *pstVdecSend, pthread_t *pVdecThread);
HI_VOID* SAMPLE_COMM_VDEC_SendStream(HI_VOID *pArgs);
HI_VOID SAMPLE_COMM_VDEC_StartGetLuma(HI_S32 s32ChnNum, VdecThreadParam *pstVdecSend, pthread_t *pVdecThread);
HI_VOID SAMPLE_COMM_VDEC_StopGetLuma(HI_S32 s32ChnNum, VdecThreadParam *pstVdecSend, pthread_t *pVdecThread);
HI_VOID* SAMPLE_COMM_VDEC_GetChnLuma(HI_VOID *pArgs);
HI_S32 SAMPLE_COMM_VDEC_Start(HI_S32 s32ChnNum, VDEC_CHN_ATTR_S *pstAttr);
HI_S32 SAMPLE_COMM_VDEC_Stop(HI_S32 s32ChnNum);
HI_S32 SAMPLE_COMM_VDEC_BindVpss(VDEC_CHN VdChn, VPSS_GRP VpssGrp);
HI_S32 SAMPLE_COMM_VDEC_UnBindVpss(VDEC_CHN VdChn, VPSS_GRP VpssGrp);
HI_S32 SAMPLE_COMM_VDEC_BindVo(VDEC_CHN VdChn, VO_LAYER VoLayer, VO_CHN VoChn);
HI_S32 SAMPLE_COMM_VDEC_UnBindVo(VDEC_CHN VdChn, VO_LAYER VoLayer, VO_CHN VoChn);
HI_S32 SAMPLE_COMM_VDEC_BindVenc(VDEC_CHN VdChn,VENC_CHN VeChn);
HI_S32 SAMPLE_COMM_VDEC_UnBindVenc(VDEC_CHN VdChn,VENC_CHN VeChn);
HI_S32 SAMPLE_COMM_VDEC_MemConfig(HI_VOID);

HI_S32 SAMPLE_COMM_VPSS_MemConfig();
HI_S32 SAMPLE_COMM_VPSS_Start(HI_S32 s32GrpCnt, SIZE_S *pstSize, HI_S32 s32ChnCnt,VPSS_GRP_ATTR_S *pstVpssGrpAttr);
HI_S32 SAMPLE_COMM_VPSS_Stop(HI_S32 s32GrpCnt, HI_S32 s32ChnCnt) ;
HI_S32 SAMPLE_COMM_DisableVpssPreScale(VPSS_GRP VpssGrp,SIZE_S stSize);
HI_S32 SAMPLE_COMM_EnableVpssPreScale(VPSS_GRP VpssGrp,SIZE_S stSize);

HI_S32 SAMPLE_COMM_VO_MemConfig(VO_DEV VoDev, HI_CHAR *pcMmzName);
HI_S32 SAMPLE_COMM_VO_StartDev(VO_DEV VoDev, VO_PUB_ATTR_S *pstPubAttr);
HI_S32 SAMPLE_COMM_VO_StopDev(VO_DEV VoDev);
HI_S32 SAMPLE_COMM_VO_StartLayer(VO_LAYER VoLayer,const VO_VIDEO_LAYER_ATTR_S *pstLayerAttr);
HI_S32 SAMPLE_COMM_VO_StopLayer(VO_LAYER VoLayer);
HI_S32 SAMPLE_COMM_VO_StartChn(VO_LAYER VoLayer, SAMPLE_VO_MODE_E enMode);
HI_S32 SAMPLE_COMM_VO_StopChn(VO_LAYER VoLayer, SAMPLE_VO_MODE_E enMode);
HI_S32 SAMPLE_COMM_VO_StartWbc(VO_WBC VoWbc,const VO_WBC_ATTR_S *pstWbcAttr);
HI_S32 SAMPLE_COMM_VO_StopWbc(VO_WBC VoWbc);
HI_S32 SAMPLE_COMM_WBC_BindVo(VO_WBC VoWbc,VO_WBC_SOURCE_S *pstWbcSource);
HI_S32 SAMPLE_COMM_VO_BindVoWbc(VO_DEV VoWbcDev, VO_LAYER VoLayer, VO_CHN VoChn);
HI_S32 SAMPLE_COMM_VO_UnBindVoWbc(VO_LAYER VoLayer, VO_CHN VoChn);
HI_S32 SAMPLE_COMM_VO_BindVpss(VO_LAYER VoLayer,VO_CHN VoChn,VPSS_GRP VpssGrp,VPSS_CHN VpssChn);
HI_S32 SAMPLE_COMM_Vpss_BindVpss(VO_LAYER VoLayer,VO_CHN VoChn,VPSS_GRP VpssGrp,VPSS_CHN VpssChn);
HI_S32 SAMPLE_COMM_VO_UnBindVpss(VO_LAYER VoLayer,VO_CHN VoChn,VPSS_GRP VpssGrp,VPSS_CHN VpssChn);
HI_S32 SAMPLE_COMM_VO_BindVi(VO_LAYER VoLayer, VO_CHN VoChn, VI_CHN ViChn);
HI_S32 SAMPLE_COMM_VO_UnBindVi(VO_LAYER VoLayer, VO_CHN VoChn);
HI_S32 SAMPLE_COMM_VO_GetWH(VO_INTF_SYNC_E enIntfSync,HI_U32 *pu32W,HI_U32 *pu32H,HI_U32 *pu32Frm);
HI_S32 SAMPLE_COMM_VO_HdmiStart(VO_INTF_SYNC_E enIntfSync);
HI_S32 SAMPLE_COMM_VO_HdmiStop(HI_VOID);
HI_S32 SAMPLE_COMM_VO_SnapStart(VENC_CHN VencChn, SIZE_S *pstSize);
HI_S32 SAMPLE_COMM_VO_SnapProcess(VENC_CHN VencChn);

HI_S32 SAMPLE_COMM_VO_SaveSnap(VENC_STREAM_S *pstStream);
HI_S32 SAMPLE_COMM_VO_SnapStop(VENC_CHN VencChn);

HI_S32 SAMPLE_COMM_VENC_MemConfig(HI_VOID);
HI_S32 SAMPLE_COMM_VENC_Start(VENC_CHN VencChn, PAYLOAD_TYPE_E enType, VIDEO_NORM_E enNorm, PIC_SIZE_E enSize, SAMPLE_RC_E enRcMode,HI_U32 u32Profile);
HI_S32 SAMPLE_COMM_VENC_Stop(VENC_CHN VencChn);
HI_S32 SAMPLE_COMM_VENC_SnapStart(VENC_CHN VencChn, SIZE_S *pstSize);
HI_S32 SAMPLE_COMM_VENC_SnapProcess(VENC_CHN VencChn);
HI_S32 SAMPLE_COMM_VENC_SnapStop(VENC_CHN VencChn);
HI_S32 SAMPLE_COMM_VENC_StartGetStream(HI_S32 s32Cnt);
HI_S32 SAMPLE_COMM_VENC_StopGetStream();
HI_S32 SAMPLE_COMM_VENC_BindVpss(VENC_CHN VencChn,VPSS_GRP VpssGrp,VPSS_CHN VpssChn);
HI_S32 SAMPLE_COMM_VENC_UnBindVpss(VENC_CHN VencChn,VPSS_GRP VpssGrp,VPSS_CHN VpssChn);
HI_S32 SAMPLE_COMM_VENC_BindVo(VO_DEV VoDev,VO_CHN VoChn,VENC_CHN VeChn);
HI_S32 SAMPLE_COMM_VENC_UnBindVo(VENC_CHN GrpChn,VO_DEV VoDev,VO_CHN VoChn);
HI_VOID SAMPLE_COMM_VENC_ReadOneFrame( FILE * fp, HI_U8 * pY, HI_U8 * pU, HI_U8 * pV,
                                              HI_U32 width, HI_U32 height, HI_U32 stride, HI_U32 stride2);
HI_S32 SAMPLE_COMM_VENC_PlanToSemi(HI_U8 *pY, HI_S32 yStride, 
                       HI_U8 *pU, HI_S32 uStride,
					   HI_U8 *pV, HI_S32 vStride, 
					   HI_S32 picWidth, HI_S32 picHeight);
HI_S32 SAMPLE_COMM_VENC_SaveJPEG(FILE *fpJpegFile, VENC_STREAM_S *pstStream);
HI_S32 SAMPLE_COMM_VENC_StartEx(VENC_CHN VencChn, PAYLOAD_TYPE_E enType, VIDEO_NORM_E enNorm, PIC_SIZE_E enSize, SAMPLE_RC_E enRcMode, HI_U32  u32Profile, VENC_GOP_ATTR_S *pstGopAttr);

HI_S32 SAMPLE_COMM_VENC_GetGopAttr(VENC_GOP_MODE_E enGopMode,VENC_GOP_ATTR_S *pstGopAttr,VIDEO_NORM_E enNorm);
HI_S32 SAMPLE_COMM_VENC_StartGetStream_Svc_t(HI_S32 s32Cnt);

HI_S32 SAMPLE_COMM_AUDIO_CreatTrdAiAo(AUDIO_DEV AiDev, AI_CHN AiChn, AUDIO_DEV AoDev, AO_CHN AoChn);
HI_S32 SAMPLE_COMM_AUDIO_CreatTrdAiAenc(AUDIO_DEV AiDev, AI_CHN AiChn, AENC_CHN AeChn);
HI_S32 SAMPLE_COMM_AUDIO_CreatTrdAencAdec(AENC_CHN AeChn, ADEC_CHN AdChn, FILE *pAecFd);
HI_S32 SAMPLE_COMM_AUDIO_CreatTrdFileAdec(ADEC_CHN AdChn, FILE *pAdcFd);
HI_S32 SAMPLE_COMM_AUDIO_CreatTrdAoVolCtrl(AUDIO_DEV AiDev);
HI_S32 SAMPLE_COMM_AUDIO_DestoryTrdAi(AUDIO_DEV AiDev, AI_CHN AiChn);
HI_S32 SAMPLE_COMM_AUDIO_DestoryTrdAencAdec(AENC_CHN AeChn);
HI_S32 SAMPLE_COMM_AUDIO_DestoryTrdFileAdec(ADEC_CHN AdChn);
HI_S32 SAMPLE_COMM_AUDIO_DestoryTrdAoVolCtrl(AUDIO_DEV AiDev);
HI_S32 SAMPLE_COMM_AUDIO_DestoryAllTrd();
HI_S32 SAMPLE_COMM_AUDIO_AoBindAdec(AUDIO_DEV AoDev, AO_CHN AoChn, ADEC_CHN AdChn);
HI_S32 SAMPLE_COMM_AUDIO_AoUnbindAdec(AUDIO_DEV AoDev, AO_CHN AoChn, ADEC_CHN AdChn);
HI_S32 SAMPLE_COMM_AUDIO_AoBindAi(AUDIO_DEV AiDev, AI_CHN AiChn, AUDIO_DEV AoDev, AO_CHN AoChn);
HI_S32 SAMPLE_COMM_AUDIO_AoUnbindAi(AUDIO_DEV AiDev, AI_CHN AiChn, AUDIO_DEV AoDev, AO_CHN AoChn);
HI_S32 SAMPLE_COMM_AUDIO_AencBindAi(AUDIO_DEV AiDev, AI_CHN AiChn, AENC_CHN AeChn);
HI_S32 SAMPLE_COMM_AUDIO_AencUnbindAi(AUDIO_DEV AiDev, AI_CHN AiChn, AENC_CHN AeChn);
HI_S32 SAMPLE_COMM_AUDIO_CfgAcodec(AIO_ATTR_S *pstAioAttr);
HI_S32 SAMPLE_COMM_AUDIO_DisableAcodec();
HI_S32 SAMPLE_COMM_AUDIO_StartAi(AUDIO_DEV AiDevId, HI_S32 s32AiChnCnt,
                                 AIO_ATTR_S* pstAioAttr, AUDIO_SAMPLE_RATE_E enOutSampleRate, HI_BOOL bResampleEn, HI_VOID* pstAiVqeAttr, HI_U32 u32AiVqeType);
HI_S32 SAMPLE_COMM_AUDIO_StopAi(AUDIO_DEV AiDevId, HI_S32 s32AiChnCnt, HI_BOOL bResampleEn, HI_BOOL bVqeEn);
HI_S32 SAMPLE_COMM_AUDIO_StartAo(AUDIO_DEV AoDevId, HI_S32 s32AoChnCnt,
                                 AIO_ATTR_S* pstAioAttr, AUDIO_SAMPLE_RATE_E enInSampleRate, HI_BOOL bResampleEn, HI_VOID* pstAoVqeAttr, HI_U32 u32AoVqeType);
HI_S32 SAMPLE_COMM_AUDIO_StopAo(AUDIO_DEV AoDevId, HI_S32 s32AoChnCnt, HI_BOOL bResampleEn, HI_BOOL bVqeEn);
HI_S32 SAMPLE_COMM_AUDIO_StartAenc(HI_S32 s32AencChnCnt, AIO_ATTR_S *pstAioAttr, PAYLOAD_TYPE_E enType);
HI_S32 SAMPLE_COMM_AUDIO_StopAenc(HI_S32 s32AencChnCnt);
HI_S32 SAMPLE_COMM_AUDIO_StartAdec(ADEC_CHN AdChn, PAYLOAD_TYPE_E enType);
HI_S32 SAMPLE_COMM_AUDIO_StopAdec(ADEC_CHN AdChn);
HI_S32 SAMPLE_COMM_AUDIO_CfgTlv320(AIO_ATTR_S *pstAioAttr);
HI_S32 SAMPLE_COMM_AUDIO_RESET(HI_VOID);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */


#endif /* End of #ifndef __SAMPLE_COMMON_H__ */
