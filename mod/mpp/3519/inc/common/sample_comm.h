/******************************************************************************
  Hisilicon Hi35xx sample programs head file.

  Copyright (C), 2010-2018, Hisilicon Tech. Co., Ltd.
 ******************************************************************************
    Modification:  2017-2 Created
******************************************************************************/

#ifndef __SAMPLE_COMM_H__
#define __SAMPLE_COMM_H__

#include "hi_common.h"
#include "hi_comm_sys.h"
#include "hi_comm_vb.h"
#include "hi_comm_isp.h"
#include "hi_comm_vi.h"
#include "hi_comm_vo.h"
#include "hi_comm_venc.h"
#include "hi_comm_vpss.h"
#include "hi_comm_region.h"
#include "hi_comm_adec.h"
#include "hi_comm_aenc.h"
#include "hi_comm_ai.h"
#include "hi_comm_ao.h"
#include "hi_comm_aio.h"
#include "hi_defines.h"

#include "mpi_sys.h"
#include "mpi_vb.h"
#include "mpi_vi.h"
#include "mpi_vo.h"
#include "mpi_venc.h"
#include "mpi_vpss.h"
#include "mpi_region.h"
#include "mpi_adec.h"
#include "mpi_aenc.h"
#include "mpi_ai.h"
#include "mpi_ao.h"
#include "mpi_isp.h"
#include "mpi_ae.h"
#include "mpi_awb.h"
#include "mpi_af.h"
#include "hi_vreg.h"

///maohw 
#include "hi_sns_ctrl.h"

#define ALIGN_UP(x, a)           ( ( ((x) + ((a) - 1) ) / a ) * a )
#define ALIGN_DOWN(x, a)         ( ( (x) / (a)) * (a) )

////////////////////

#include "hi_mipi.h"
#include <sys/prctl.h>


#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* End of #ifdef __cplusplus */

/*******************************************************
    macro define
*******************************************************/
#define FILE_NAME_LEN               128

#define ALIGN_BACK(x, a)              ((a) * (((x) / (a))))
#define SAMPLE_SYS_ALIGN_WIDTH      64
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
#define SAMPLE_PIXEL_FORMAT         PIXEL_FORMAT_YUV_SEMIPLANAR_420

#define TW2865_FILE "/dev/tw2865dev"
#define TW2960_FILE "/dev/tw2960dev"
#define TLV320_FILE "/dev/tlv320aic31"


#if (HICHIP == HI3519_V101  || HICHIP==HI3516A_V200)
#define SAMPLE_VO_DEV_DSD1 0
#define SAMPLE_VO_DEV_DSD0 SAMPLE_VO_DEV_DSD1
#else
#error HICHIP define may be error
#endif

/*** for init global parameter ***/
#define SAMPLE_ENABLE 		    1
#define SAMPLE_DISABLE 		    0
#define SAMPLE_NOUSE 		    -1

#define SAMPLE_AUDIO_AI_DEV 0
#define SAMPLE_AUDIO_AO_DEV 0
#define SAMPLE_AUDIO_PTNUMPERFRM   320

#define VI_MST_NOTPASS_WITH_VALUE_RETURN(s32TempRet) \
    do{\
        NOT_PASS(s32TempRet);\
        VIMST_ExitMpp();\
        return s32TempRet;\
    }while(0)

#define VI_PAUSE()  do {\
        printf("---------------press any key to exit!---------------\n");\
        getchar();\
    } while (0)


#define SAMPLE_PRT(fmt...)   \
    do {\
        printf("[%s]-%d: ", __FUNCTION__, __LINE__);\
        printf(fmt);\
    }while(0)

#define CHECK_NULL_PTR(ptr)\
    do{\
        if(NULL == ptr)\
        {\
            printf("func:%s,line:%d, NULL pointer\n",__FUNCTION__,__LINE__);\
            return HI_FAILURE;\
        }\
    }while(0)


/*******************************************************
    enum define
*******************************************************/
//maohw
enum // hi_comm_video.h:PIC_SIZE_E
{
    //PIC_CIF = ,
    PIC_D1_PAL = PIC_D1,    /* 720 * 576 */
    PIC_D1_NTSC= PIC_D1,    /* 720 * 480 */
    PIC_720P   = PIC_HD720, /* 1280 * 720  */
    PIC_1080P  = PIC_HD1080,/* 1920 * 1080 */
    //PIC_2592x1520 = ,
    PIC_2592x1536   = PIC_2592x1520,
    PIC_2592x1944 = PIC_5M,
    //--PIC_2716x1524,
    PIC_3840x2160 = PIC_UHD4K,
    //--PIC_4096x2160,
    PIC_3000x3000 = PIC_3Kx3K,
    PIC_4000x3000 = PIC_12M,
    //--PIC_7680x4320,
    //--PIC_3840x8640,
    //PIC_BUTT
};
enum // hi_comm_video.h:PIXEL_FORMAT_E
{
  PIXEL_FORMAT_ARGB_1555 = PIXEL_FORMAT_RGB_1555,
};

/////////////////////


typedef enum sample_ispcfg_opt_e
{
    CFG_OPT_NONE    = 0,
    CFG_OPT_SAVE    = 1,
    CFG_OPT_LOAD    = 2,
    CFG_OPT_BUTT
} SAMPLE_CFG_OPT_E;

typedef enum sample_vi_mode_e
{
    APTINA_AR0130_DC_720P_30FPS = 0,
    APTINA_9M034_DC_720P_30FPS,
    SONY_IMX122_DC_1080P_30FPS,
    APTINA_MT9P006_DC_1080P_30FPS,
    SAMPLE_VI_MODE_1_D1,
    SAMPLE_VI_MODE_BT1120_1080I,
    SAMPLE_VI_MODE_BT1120_720P,
    SAMPLE_VI_MODE_BT1120_1080P,
    PANASONIC_MN34220_SUBLVDS_1080P_30FPS,
    PANASONIC_MN34220_SUBLVDS_720P_120FPS,
    SONY_IMX178_LVDS_1080P_30FPS,
    SONY_IMX185_MIPI_1080P_30FPS,
    SONY_IMX290_MIPI_1080P_30FPS,
    PANASONIC_MN34220_MIPI_1080P_30FPS,
    PANASONIC_MN34220_MIPI_720P_120FPS,
    SONY_IMX178_LVDS_5M_30FPS,
    SONY_IMX226_LVDS_4K_30FPS,
    SONY_IMX226_LVDS_9M_30FPS,
	  SONY_IMX226_LVDS_12M_30FPS,
	  SONY_IMX226_LVDS_4K_60FPS,
    SONY_IMX117_LVDS_4K_30FPS,
    SONY_IMX117_LVDS_12M_28FPS,
    SONY_IMX117_LVDS_1080P_120FPS,
    SONY_IMX117_LVDS_720P_240FPS,
    SONY_IMX377_MIPI_8M_30FPS_10BIT,
    SONY_IMX377_MIPI_8M_60FPS_10BIT,
    SONY_IMX377_MIPI_1080P_120FPS_12BIT,
    SONY_IMX377_MIPI_720P_240FPS_10BIT,
    SONY_IMX377_MIPI_12M_30FPS_12BIT,
    SONY_IMX377_MIPI_8M_30FPS_12BIT,
    SONY_IMX274_LVDS_4K_30FPS,
    PANASONIC_MN34120_LVDS_16M_16P25FPS,
    PANASONIC_MN34120_LVDS_4K_30FPS,
    PANASONIC_MN34120_LVDS_1080P_60FPS,
    APTINA_AR0230_HISPI_1080P_30FPS,
    APTINA_AR0330_MIPI_1080P_30FPS,
    APTINA_AR0330_MIPI_1536P_25FPS,
    APTINA_AR0330_MIPI_1296P_25FPS,
    OMNIVISION_OV4689_MIPI_4M_30FPS,
    OMNIVISION_OV4689_MIPI_1080P_30FPS,
    OMNIVISION_OV5658_MIPI_5M_30FPS,
    SONY_IMX326_MIPI_5M_30FPS,
    OMNIVISION_OS05A_MIPI_5M_30FPS,
    OMNIVISION_OS08A_MIPI_4K_30FPS,
    SONY_IMX334_MIPI_4K_30FPS,
} SAMPLE_VI_MODE_E;

typedef enum
{
    VI_DEV_BT656_D1_1MUX = 0,
    VI_DEV_BT656_D1_4MUX,
    VI_DEV_BT656_960H_1MUX,
    VI_DEV_BT656_960H_4MUX,
    VI_DEV_720P_HD_1MUX,
    VI_DEV_1080P_HD_1MUX,
    VI_DEV_BUTT
} SAMPLE_VI_DEV_TYPE_E;

typedef enum sample_vi_chn_set_e
{
    VI_CHN_SET_NORMAL = 0,      /* mirror, flip close */
    VI_CHN_SET_MIRROR,          /* open MIRROR */
    VI_CHN_SET_FLIP,            /* open filp */
    VI_CHN_SET_FLIP_MIRROR      /* mirror, flip */
} SAMPLE_VI_CHN_SET_E;

typedef enum sample_bind_e
{
    SAMPLE_BIND_VI_VO = 0,      /* VI bind to VO */
    SAMPLE_BIND_VPSS_VO,    /* VPSS bind to VO */
    SAMPLE_BIND_VI_VPSS_VO, /* VI bind to VPSS, VPSS bind to VO */
    SAMPLE_BIND_BUTT
} SAMPLE_BIND_E;

typedef enum
{
    SAMPLE_FRAMERATE_DEFAULT = 0,
    SAMPLE_FRAMERATE_15FPS   = 15,
    SAMPLE_FRAMERATE_20FPS   = 20,
    SAMPLE_FRAMERATE_30FPS   = 30,
    SAMPLE_FRAMERATE_60FPS   = 60,
    SAMPLE_FRAMERATE_120FPS  = 120,
    SAMPLE_FRAMERATE_BUTT,
} SAMPLE_FRAMERATE_E;

typedef enum sample_vo_mode_e
{
    VO_MODE_1MUX = 0,
    VO_MODE_2MUX = 1,
    VO_MODE_BUTT
} SAMPLE_VO_MODE_E;

typedef enum sample_rc_e
{
    SAMPLE_RC_CBR = 0,
    SAMPLE_RC_VBR,
    SAMPLE_RC_AVBR,
    SAMPLE_RC_FIXQP
} SAMPLE_RC_E;

typedef enum sample_rgn_change_type_e
{
    RGN_CHANGE_TYPE_FGALPHA = 0,
    RGN_CHANGE_TYPE_BGALPHA,
    RGN_CHANGE_TYPE_LAYER
} SAMPLE_RGN_CHANGE_TYPE_EN;

typedef enum sample_sensor_num_e
{
    SAMPLE_SENSOR_SINGLE = 0,
    SAMPLE_SENSOR_DOUBLE   = 1,
    SAMPLE_SENSOR_BUT
} SAMPLE_SENSOR_NUM_E;

/*******************************************************
    structure define
*******************************************************/
typedef struct sample_vi_param_s
{
    HI_S32 s32ViDevCnt;         // VI Dev Total Count
    HI_S32 s32ViDevInterval;    // Vi Dev Interval
    HI_S32 s32ViChnCnt;         // Vi Chn Total Count
    HI_S32 s32ViChnInterval;    // VI Chn Interval
} SAMPLE_VI_PARAM_S;

typedef struct sample_video_loss_s
{
    HI_BOOL bStart;
    pthread_t Pid;
    SAMPLE_VI_MODE_E enViMode;
} SAMPLE_VIDEO_LOSS_S;


typedef struct sample_vi_frame_info_s
{
    VB_BLK VbBlk;
    VIDEO_FRAME_INFO_S stVideoFrame;
    HI_U32 u32FrmSize;
} SAMPLE_VI_FRAME_INFO_S;


typedef struct sample_venc_getstream_s
{
    HI_BOOL bThreadStart;
    VENC_CHN VeChn[VENC_MAX_CHN_NUM];
    HI_S32  s32Cnt;
    //maohw
    void *uargs;
    int (*cb)(VENC_CHN VeChn, PAYLOAD_TYPE_E PT, VENC_STREAM_S* pstStream, void* uargs);
} SAMPLE_VENC_GETSTREAM_PARA_S;

typedef struct sample_vi_config_s
{
    SAMPLE_VI_MODE_E    enViMode;
    VIDEO_NORM_E        enNorm;           /*DC: VIDEO_ENCODING_MODE_AUTO */
    ROTATE_E            enRotate;
    SAMPLE_VI_CHN_SET_E enViChnSet;
    WDR_MODE_E          enWDRMode;
    SAMPLE_FRAMERATE_E  enFrmRate;
    SAMPLE_SENSOR_NUM_E	enSnsNum;
} SAMPLE_VI_CONFIG_S;

typedef struct sample_vo_config_s
{
    HI_U32  u32DisBufLen;
} SAMPLE_VO_CONFIG_S;

typedef struct sample_vpss_attr_s
{
    VPSS_GRP VpssGrp;
    VPSS_CHN VpssChn;
    VPSS_GRP_ATTR_S stVpssGrpAttr;
    VPSS_CHN_ATTR_S stVpssChnAttr;
    VPSS_CHN_MODE_S stVpssChnMode;
    VPSS_EXT_CHN_ATTR_S stVpssExtChnAttr;
}SAMPLE_VPSS_ATTR_S;

/*******************************************************
    function announce
*******************************************************/
HI_S32 SAMPLE_COMM_SYS_GetPicSize(VIDEO_NORM_E enNorm, PIC_SIZE_E enPicSize, SIZE_S* pstSize);
HI_U32 SAMPLE_COMM_SYS_CalcPicVbBlkSize(VIDEO_NORM_E enNorm, PIC_SIZE_E enPicSize, PIXEL_FORMAT_E enPixFmt, HI_U32 u32AlignWidth);
HI_U32 SAMPLE_COMM_SYS_CalcPicVbBlkSize2(SIZE_S *pstSize, PIXEL_FORMAT_E enPixFmt, HI_U32 u32AlignWidth);
HI_U32 VI_COMM_SYS_CalcPicVbBlkSize(VIDEO_NORM_E enNorm, HI_U32  u32Width , HI_U32  u32Height, PIXEL_FORMAT_E enPixFmt, HI_U32 u32AlignWidth);
HI_S32 SAMPLE_COMM_SYS_MemConfig(HI_VOID);
HI_VOID SAMPLE_COMM_SYS_Exit(void);
HI_S32 SAMPLE_COMM_SYS_Init(VB_CONF_S* pstVbConf);
HI_S32 SAMPLE_COMM_SYS_Init_With_DCF(VB_CONF_S* pstVbConf, HI_BOOL bIspStat);
HI_S32 SAMPLE_COMM_SYS_Payload2FilePostfix(PAYLOAD_TYPE_E enPayload, HI_CHAR* szFilePostfix);

HI_S32 SAMPLE_COMM_ISP_Init(ISP_DEV IspDev, WDR_MODE_E  enWDRMode, SAMPLE_FRAMERATE_E enFrmRate, SAMPLE_SENSOR_NUM_E enSnsNum);
HI_VOID SAMPLE_COMM_ISP_Stop(void);
HI_S32 SAMPLE_COMM_ISP_Run(ISP_DEV IspDev);
HI_S32 SAMPLE_COMM_ISP_ChangeSensorMode(HI_U8 u8Mode);
HI_S32 SAMPLE_COMM_ISP_BindSns(ISP_DEV IspDev, HI_U32 u32SnsId, SAMPLE_VI_MODE_E enViMode, HI_S8 s8SnsDev);
HI_S32 SAMPLE_COMM_ISP_Sensor_Regiter_callback(ISP_DEV IspDev, HI_U32 u32SnsId);
HI_S32 SAMPLE_COMM_Sensor_UnRegiter_callback(ISP_DEV IspDev);


HI_S32 SAMPLE_COMM_VI_GetSizeBySensor(PIC_SIZE_E* penSize);
HI_S32 SAMPLE_COMM_VI_Mode2Param(SAMPLE_VI_MODE_E enViMode, SAMPLE_VI_PARAM_S* pstViParam);
HI_S32 SAMPLE_COMM_VI_Mode2Size(SAMPLE_VI_MODE_E enViMode, VIDEO_NORM_E enNorm, SIZE_S* pstSize);
VI_DEV SAMPLE_COMM_VI_GetDev(SAMPLE_VI_MODE_E enViMode, VI_CHN ViChn);
HI_S32 SAMPLE_COMM_VI_StartDev(VI_DEV ViDev, SAMPLE_VI_MODE_E enViMode);
HI_S32 SAMPLE_COMM_VI_StartChn(VI_CHN ViChn, RECT_S* pstCapRect, SIZE_S* pstTarSize, SAMPLE_VI_CONFIG_S* pstViConfig);
HI_S32 SAMPLE_COMM_VI_StartBT1120(SAMPLE_VI_CONFIG_S* pstViConfig, int mask[2]);
HI_S32 SAMPLE_COMM_VI_StopBT1120(SAMPLE_VI_MODE_E enViMode);
HI_S32 SAMPLE_COMM_VI_BindVpss(SAMPLE_VI_MODE_E enViMode);
HI_S32 SAMPLE_COMM_VI_BindVpss2(SAMPLE_VI_MODE_E enViMode, HI_S32 VpssGrp, HI_S32 ViPipe);
HI_S32 SAMPLE_COMM_VI_UnBindVpss(SAMPLE_VI_MODE_E enViMode);
HI_S32 SAMPLE_COMM_VI_BindVenc(SAMPLE_VI_MODE_E enViMode);
HI_S32 SAMPLE_COMM_VI_StartMIPI(SAMPLE_VI_CONFIG_S* pstViConfig, int mask[2]);
HI_S32 SAMPLE_COMM_VI_UnBindVenc(SAMPLE_VI_MODE_E enViMode);
HI_S32 SAMPLE_COMM_VI_MemConfig(SAMPLE_VI_MODE_E enViMode);
HI_S32 SAMPLE_COMM_VI_GetVFrameFromYUV(FILE* pYUVFile, HI_U32 u32Width, HI_U32 u32Height, HI_U32 u32Stride, VIDEO_FRAME_INFO_S* pstVFrameInfo);
HI_S32 SAMPLE_COMM_VI_ChangeCapSize(VI_CHN ViChn, HI_U32 u32CapWidth, HI_U32 u32CapHeight, HI_U32 u32Width, HI_U32 u32Height);
HI_S32 SAMPLE_COMM_VI_StartVi(SAMPLE_VI_CONFIG_S* pstViConfig);
HI_S32 SAMPLE_COMM_VI_StartVi2(SAMPLE_VI_CONFIG_S* pstViConfig);
HI_S32 SAMPLE_COMM_VI_StopVi(SAMPLE_VI_CONFIG_S* pstViConfig);
HI_S32 SAMPLE_COMM_VI_SwitchResParam( SAMPLE_VI_CONFIG_S* pstViConfig,
                                      ISP_PUB_ATTR_S* pstPubAttr,
                                      RECT_S* pstCapRect );
HI_S32 SAMPLE_COMM_VI_SetMipiAttr(COMBO_DEV MipiDev, SAMPLE_VI_CONFIG_S* pstViConfig);
HI_S32 SAMPLE_COMM_VI_GetDevAttrBySns(SAMPLE_VI_MODE_E enViMode, VI_DEV_ATTR_S *pstViDevAttr);



HI_BOOL SAMPLE_COMM_IsViVpssOnline(void);
HI_S32 SAMPLE_COMM_VI_StartMIPI_BT1120(SAMPLE_VI_MODE_E enViMode, int mask[2]);

HI_S32 SAMPLE_COMM_VI_FPN_CALIBRATE_CONFIG(const char* fpn_file,    /* fpn file name */
        ISP_FPN_TYPE_E enFpnType, /* line/frame */
        PIXEL_FORMAT_E enPixelFormat,
        COMPRESS_MODE_E	enCompressMode,
        HI_U32 u32FrmNum,
        HI_U32 u32Threshold);

HI_S32 SAMPLE_COMM_VI_CORRECTION_CONFIG(const char* fpn_file,     /* fpn file_name */
                                        ISP_FPN_TYPE_E enFpnType, /* line/frame */
                                        ISP_OP_TYPE_E  enOpType,  /* auto/manual */
                                        HI_U32 u32Strength,       /* strength */
                                        PIXEL_FORMAT_E enPixelFormat);

HI_S32 SAMPLE_COMM_VPSS_MemConfig();
HI_S32 SAMPLE_COMM_VPSS_Start(HI_S32 s32GrpCnt, SIZE_S* pstSize, HI_S32 s32ChnCnt, VPSS_GRP_ATTR_S* pstVpssGrpAttr);
HI_S32 SAMPLE_COMM_VPSS_Stop(HI_S32 s32GrpCnt, HI_S32 s32ChnCnt) ;
HI_S32 SAMPLE_COMM_VPSS_StartGroup(VPSS_GRP VpssGrp, VPSS_GRP_ATTR_S* pstVpssGrpAttr);
HI_S32 SAMPLE_COMM_VPSS_EnableChn(VPSS_GRP VpssGrp, VPSS_CHN VpssChn,
                                  VPSS_CHN_ATTR_S* pstVpssChnAttr,
                                  VPSS_CHN_MODE_S* pstVpssChnMode,
                                  VPSS_EXT_CHN_ATTR_S* pstVpssExtChnAttr);
HI_S32 SAMPLE_COMM_VPSS_StopGroup(VPSS_GRP VpssGrp);
HI_S32 SAMPLE_COMM_VPSS_DisableChn(VPSS_GRP VpssGrp, VPSS_CHN VpssChn);

HI_S32 SAMPLE_COMM_VO_GetWH(VO_INTF_SYNC_E enIntfSync, HI_U32* pu32W, HI_U32* pu32H, HI_U32* pu32Frm);
HI_S32 SAMPLE_COMM_VO_MemConfig(VO_DEV VoDev, HI_CHAR* pcMmzName);
HI_S32 SAMPLE_COMM_VO_StartDev(VO_DEV VoDev, VO_PUB_ATTR_S* pstPubAttr);
HI_S32 SAMPLE_COMM_VO_StopDev(VO_DEV VoDev);
HI_S32 SAMPLE_COMM_VO_StartLayer(VO_LAYER VoLayer, const VO_VIDEO_LAYER_ATTR_S* pstLayerAttr, HI_BOOL bVgsBypass);
HI_S32 SAMPLE_COMM_VO_StopLayer(VO_LAYER VoLayer);
HI_S32 SAMPLE_COMM_VO_StartChn(VO_LAYER VoLayer, SAMPLE_VO_MODE_E enMode);
HI_S32 SAMPLE_COMM_VO_StopChn(VO_LAYER VoLayer, SAMPLE_VO_MODE_E enMode);
HI_S32 SAMPLE_COMM_VO_BindVpss(VO_LAYER VoLayer, VO_CHN VoChn, VPSS_GRP VpssGrp, VPSS_CHN VpssChn);
HI_S32 SAMPLE_COMM_VO_UnBindVpss(VO_LAYER VoLayer, VO_CHN VoChn, VPSS_GRP VpssGrp, VPSS_CHN VpssChn);
HI_S32 SAMPLE_COMM_VO_BindVi(VO_LAYER VoLayer, VO_CHN VoChn, VI_CHN ViChn);
HI_S32 SAMPLE_COMM_VO_UnBindVi(VO_LAYER VoLayer, VO_CHN VoChn);

HI_S32 SAMPLE_COMM_VENC_MemConfig(HI_VOID);
HI_S32 SAMPLE_COMM_VENC_Start(VENC_CHN VencChn, PAYLOAD_TYPE_E enType, VIDEO_NORM_E enNorm, PIC_SIZE_E enSize, SAMPLE_RC_E enRcMode, HI_U32 u32Profile, VENC_GOP_ATTR_S *pstGopAttr);
HI_S32 SAMPLE_COMM_VENC_Stop(VENC_CHN VencChn);
//HI_S32 SAMPLE_COMM_VENC_SnapStart(VENC_CHN VencChn, SIZE_S* pstSize);
HI_S32 SAMPLE_COMM_VENC_SnapStart(VENC_CHN VencChn, SIZE_S* pstSize, HI_BOOL bSupportDCF);

//HI_S32 SAMPLE_COMM_VENC_SnapProcess(VENC_CHN VencChn);
HI_S32 SAMPLE_COMM_VENC_SnapProcess(VENC_CHN VencChn, HI_BOOL bSaveJpg, HI_BOOL bSaveThm);

HI_S32 SAMPLE_COMM_VENC_SnapStop(VENC_CHN VencChn);
HI_S32 SAMPLE_COMM_VENC_StartGetStream(VENC_CHN VeChn[],HI_S32 s32Cnt);
//maohw
HI_S32 SAMPLE_COMM_VENC_StartGetStreamCb(VENC_CHN VeChn[],HI_S32 s32Cnt, int (*cb)(VENC_CHN VeChn, PAYLOAD_TYPE_E PT, VENC_STREAM_S* pstStream, void* uargs), void *uargs);
HI_S32 SAMPLE_COMM_VENC_StopGetStream();
HI_S32 SAMPLE_COMM_VENC_BindVpss(VENC_CHN VencChn, VPSS_GRP VpssGrp, VPSS_CHN VpssChn);
HI_S32 SAMPLE_COMM_VENC_UnBindVpss(VENC_CHN VencChn, VPSS_GRP VpssGrp, VPSS_CHN VpssChn);
HI_S32 SAMPLE_COMM_VENC_StartGetStream_Svc_t(HI_S32 s32Cnt);
HI_S32 SAMPLE_COMM_VENC_GetGopAttr(VENC_GOP_MODE_E enGopMode,VENC_GOP_ATTR_S *pstGopAttr,VIDEO_NORM_E enNorm);


HI_S32 SAMPLE_COMM_VDA_MdStart(VDA_CHN VdaChn, HI_U32 u32Chn, SIZE_S* pstSize);
HI_S32 SAMPLE_COMM_VDA_OdStart(VDA_CHN VdaChn, HI_U32 u32Chn, SIZE_S* pstSize);
HI_VOID SAMPLE_COMM_VDA_MdStop(VDA_CHN VdaChn, HI_U32 u32Chn);
HI_VOID SAMPLE_COMM_VDA_OdStop(VDA_CHN VdaChn, HI_U32 u32Chn);

HI_S32 SAMPLE_COMM_AUDIO_CreatTrdAiAo(AUDIO_DEV AiDev, AI_CHN AiChn, AUDIO_DEV AoDev, AO_CHN AoChn);
HI_S32 SAMPLE_COMM_AUDIO_CreatTrdAiAenc(AUDIO_DEV AiDev, AI_CHN AiChn, AENC_CHN AeChn);
HI_S32 SAMPLE_COMM_AUDIO_CreatTrdAencAdec(AENC_CHN AeChn, ADEC_CHN AdChn, FILE* pAecFd);
HI_S32 SAMPLE_COMM_AUDIO_CreatTrdFileAdec(ADEC_CHN AdChn, FILE* pAdcFd);
HI_S32 SAMPLE_COMM_AUDIO_CreatTrdAoVolCtrl(AUDIO_DEV AoDev);
HI_S32 SAMPLE_COMM_AUDIO_DestoryTrdAi(AUDIO_DEV AiDev, AI_CHN AiChn);
HI_S32 SAMPLE_COMM_AUDIO_DestoryTrdAencAdec(AENC_CHN AeChn);
HI_S32 SAMPLE_COMM_AUDIO_DestoryTrdFileAdec(ADEC_CHN AdChn);
HI_S32 SAMPLE_COMM_AUDIO_DestoryTrdAoVolCtrl(AUDIO_DEV AoDev);
HI_S32 SAMPLE_COMM_AUDIO_DestoryAllTrd();
HI_S32 SAMPLE_COMM_AUDIO_AoBindAdec(AUDIO_DEV AoDev, AO_CHN AoChn, ADEC_CHN AdChn);
HI_S32 SAMPLE_COMM_AUDIO_AoUnbindAdec(AUDIO_DEV AoDev, AO_CHN AoChn, ADEC_CHN AdChn);
HI_S32 SAMPLE_COMM_AUDIO_AoBindAi(AUDIO_DEV AiDev, AI_CHN AiChn, AUDIO_DEV AoDev, AO_CHN AoChn);
HI_S32 SAMPLE_COMM_AUDIO_AoUnbindAi(AUDIO_DEV AiDev, AI_CHN AiChn, AUDIO_DEV AoDev, AO_CHN AoChn);
HI_S32 SAMPLE_COMM_AUDIO_AencBindAi(AUDIO_DEV AiDev, AI_CHN AiChn, AENC_CHN AeChn);
HI_S32 SAMPLE_COMM_AUDIO_AencUnbindAi(AUDIO_DEV AiDev, AI_CHN AiChn, AENC_CHN AeChn);
HI_S32 SAMPLE_COMM_AUDIO_CfgAcodec(AIO_ATTR_S* pstAioAttr);
HI_S32 SAMPLE_COMM_AUDIO_DisableAcodec();
HI_S32 SAMPLE_COMM_AUDIO_StartAi(AUDIO_DEV AiDevId, HI_S32 s32AiChnCnt,
                                 AIO_ATTR_S* pstAioAttr, AUDIO_SAMPLE_RATE_E enOutSampleRate, HI_BOOL bResampleEn, HI_VOID* pstAiVqeAttr, HI_U32 u32AiVqeType);
HI_S32 SAMPLE_COMM_AUDIO_StopAi(AUDIO_DEV AiDevId, HI_S32 s32AiChnCnt, HI_BOOL bResampleEn, HI_BOOL bVqeEn);
HI_S32 SAMPLE_COMM_AUDIO_StartAo(AUDIO_DEV AoDevId, HI_S32 s32AoChnCnt,
                                 AIO_ATTR_S* pstAioAttr, AUDIO_SAMPLE_RATE_E enInSampleRate, HI_BOOL bResampleEn, HI_VOID* pstAoVqeAttr, HI_U32 u32AoVqeType);
HI_S32 SAMPLE_COMM_AUDIO_StopAo(AUDIO_DEV AoDevId, HI_S32 s32AoChnCnt, HI_BOOL bResampleEn, HI_BOOL bVqeEn);
HI_S32 SAMPLE_COMM_AUDIO_StartAenc(HI_S32 s32AencChnCnt, HI_U32 u32AencPtNumPerFrm, PAYLOAD_TYPE_E enType);
HI_S32 SAMPLE_COMM_AUDIO_StopAenc(HI_S32 s32AencChnCnt);
HI_S32 SAMPLE_COMM_AUDIO_StartAdec(ADEC_CHN AdChn, PAYLOAD_TYPE_E enType);
HI_S32 SAMPLE_COMM_AUDIO_StopAdec(ADEC_CHN AdChn);
HI_VOID SAMPLE_COMM_SYS_Exit(void);
HI_S32 SAMPLE_SYS_SetReg(HI_U32 u32Addr, HI_U32 u32Value);
HI_S32 SAMPLE_SYS_GetReg(HI_U32 u32Addr, HI_U32 *pu32Value);


//maohw
#ifndef MAX_SENSOR_NUM
#define MAX_SENSOR_NUM      2
#endif
#ifndef ISP_MAX_DEV_NUM
#define ISP_MAX_DEV_NUM     2
#endif
#include "mpp.h"
////////////


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */


#endif /* End of #ifndef __SAMPLE_COMMON_H__ */
