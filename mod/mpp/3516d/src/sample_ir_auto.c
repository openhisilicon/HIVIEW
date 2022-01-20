/******************************************************************************
  A simple program of Hisilicon Hi35xx video input and output implementation.
  Copyright (C), 2010-2018, Hisilicon Tech. Co., Ltd.
 ******************************************************************************
    Modification:  2017-8 Created
******************************************************************************/

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* End of #ifdef __cplusplus */
#include <sys/time.h>
#include <sys/prctl.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>
//#include "gpio.h"
#include "sample_comm.h"
#include "mpi_isp.h"
#include "hi_comm_isp.h"
#include "mpi_ae.h"
#include "mpi_awb.h"
#include "mpi_sys.h"

#ifdef __HuaweiLite__
#include <asm/io.h>
#endif

#include <fcntl.h>
#include <sys/mman.h>

#include "ir_auto_prev.h"
typedef struct
{
	HI_BOOL   bThreadFlag;
	pthread_t pThread;
}HI_IR_AUTO_THREAD_S;
static HI_IR_AUTO_THREAD_S g_astIrThread[ISP_MAX_PIPE_NUM] = {{0}};
static pthread_mutex_t g_ir_mutex = PTHREAD_MUTEX_INITIALIZER;
static gsf_mpp_ir_t g_ir_switch = {0,};
static ISP_IR_AUTO_ATTR_S g_astIrAttr[ISP_MAX_PIPE_NUM] = {{0}};
static VI_PIPE ViPipe = 0;

#define SAMPLE_IR_CALIBRATION_MODE  (0)
#define SAMPLE_IR_AUTO_MODE         (1)
#define HI_3516EV300_CHIP           (0)
#define HI_3516EV200_CHIP           (1)
#define HI_3518EV300_CHIP           (2)
#define GAIN_MAX_COEF               (280)
#define GAIN_MIN_COEF               (190)

static HI_U32 gu32Chip;

HI_S32 SAMPLE_IR_AUTO_START_PREV(IR_AUTO_PREV_S *pstIr_AutoPrev)
{

    HI_S32             s32Ret = HI_SUCCESS;
    HI_S32             s32ViCnt       = 1;
    VI_DEV             ViDev          = 0;
    VI_PIPE            ViPipe         = 0;
    VI_CHN             ViChn          = 0;
    HI_S32             s32WorkSnsId   = 0;

    SIZE_S             stSize;
    VB_CONFIG_S        stVbConf;
    PIC_SIZE_E         enPicSize;
    HI_U32             u32BlkSize;

    WDR_MODE_E         enWDRMode      = WDR_MODE_NONE;
    //WDR_MODE_E         enWDRMode      = WDR_MODE_2To1_LINE;
    DYNAMIC_RANGE_E    enDynamicRange = DYNAMIC_RANGE_SDR8;
    PIXEL_FORMAT_E     enPixFormat    = PIXEL_FORMAT_YVU_SEMIPLANAR_420;
    VIDEO_FORMAT_E     enVideoFormat  = VIDEO_FORMAT_LINEAR;
    COMPRESS_MODE_E    enCompressMode = COMPRESS_MODE_NONE;
    VI_VPSS_MODE_E     enMastPipeMode = VI_ONLINE_VPSS_ONLINE;
    VPSS_GRP_ATTR_S    stVpssGrpAttr;

    VPSS_CHN_ATTR_S    astVpssChnAttr[VPSS_MAX_PHY_CHN_NUM];
    HI_U8 i = 0;

    PAYLOAD_TYPE_E     enType      = PT_H265;
    SAMPLE_RC_E        enRcMode    = SAMPLE_RC_CBR;
    HI_U32             u32Profile  = 0;
    HI_BOOL            bRcnRefShareBuf = HI_TRUE;
    VENC_GOP_ATTR_S    stGopAttr;

    pstIr_AutoPrev->VencChn[0] = 0;
    pstIr_AutoPrev->VpssGrp    = 0;
    pstIr_AutoPrev->VpssChn    = VPSS_CHN0;
    pstIr_AutoPrev->VoChn      = 0;

    for(i=0;i<VPSS_MAX_PHY_CHN_NUM;i++)
    {
        pstIr_AutoPrev->abChnEnable[i] = 0;
    }

    /*config vi*/
    SAMPLE_COMM_VI_GetSensorInfo(&pstIr_AutoPrev->stViConfig);

    pstIr_AutoPrev->stViConfig.s32WorkingViNum                                   = s32ViCnt;
    pstIr_AutoPrev->stViConfig.as32WorkingViId[0]                                = 0;
    pstIr_AutoPrev->stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.MipiDev         = ViDev;
    pstIr_AutoPrev->stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.s32BusId        = 0;
    pstIr_AutoPrev->stViConfig.astViInfo[s32WorkSnsId].stDevInfo.ViDev           = ViDev;
    pstIr_AutoPrev->stViConfig.astViInfo[s32WorkSnsId].stDevInfo.enWDRMode       = enWDRMode;
    pstIr_AutoPrev->stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.enMastPipeMode = enMastPipeMode;
    pstIr_AutoPrev->stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.aPipe[0]       = ViPipe;
    pstIr_AutoPrev->stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.aPipe[1]       = -1;
    pstIr_AutoPrev->stViConfig.astViInfo[s32WorkSnsId].stChnInfo.ViChn           = ViChn;
    pstIr_AutoPrev->stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enPixFormat     = enPixFormat;
    pstIr_AutoPrev->stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enDynamicRange  = enDynamicRange;
    pstIr_AutoPrev->stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enVideoFormat   = enVideoFormat;
    pstIr_AutoPrev->stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enCompressMode  = enCompressMode;

    /*get picture size*/
    s32Ret = SAMPLE_COMM_VI_GetSizeBySensor(pstIr_AutoPrev->stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.enSnsType, &enPicSize);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("get picture size by sensor failed!\n");
        return s32Ret;
    }

    s32Ret = SAMPLE_COMM_SYS_GetPicSize(enPicSize, &stSize);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("get picture size failed!\n");
        return s32Ret;
    }

    /*config vb*/
    hi_memset(&stVbConf, sizeof(VB_CONFIG_S), 0, sizeof(VB_CONFIG_S));
    stVbConf.u32MaxPoolCnt              = 1;

    u32BlkSize = COMMON_GetPicBufferSize(stSize.u32Width, stSize.u32Height, SAMPLE_PIXEL_FORMAT, DATA_BITWIDTH_8, COMPRESS_MODE_SEG, DEFAULT_ALIGN);
    stVbConf.astCommPool[0].u64BlkSize  = u32BlkSize;
    stVbConf.astCommPool[0].u32BlkCnt   = 3;

    u32BlkSize = COMMON_GetPicBufferSize(720, 576, SAMPLE_PIXEL_FORMAT, DATA_BITWIDTH_8, COMPRESS_MODE_NONE, DEFAULT_ALIGN);
    stVbConf.astCommPool[1].u64BlkSize  = u32BlkSize;
    stVbConf.astCommPool[1].u32BlkCnt   = 5;


    s32Ret = SAMPLE_COMM_SYS_Init(&stVbConf);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("system init failed with %d!\n", s32Ret);
        return s32Ret;
    }

    /*start vi*/
    s32Ret = SAMPLE_COMM_VI_StartVi(&pstIr_AutoPrev->stViConfig);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("start vi failed.s32Ret:0x%x !\n", s32Ret);
        goto EXIT;
    }

    /*config vpss*/
    hi_memset(&stVpssGrpAttr, sizeof(VPSS_GRP_ATTR_S), 0, sizeof(VPSS_GRP_ATTR_S));
    stVpssGrpAttr.stFrameRate.s32SrcFrameRate    = -1;
    stVpssGrpAttr.stFrameRate.s32DstFrameRate    = -1;
    stVpssGrpAttr.enDynamicRange                 = DYNAMIC_RANGE_SDR8;
    stVpssGrpAttr.enPixelFormat                  = enPixFormat;
    stVpssGrpAttr.u32MaxW                        = stSize.u32Width;
    stVpssGrpAttr.u32MaxH                        = stSize.u32Height;
    stVpssGrpAttr.bNrEn                          = HI_TRUE;
    stVpssGrpAttr.stNrAttr.enCompressMode        = COMPRESS_MODE_FRAME;
    stVpssGrpAttr.stNrAttr.enNrMotionMode        = NR_MOTION_MODE_NORMAL;

    astVpssChnAttr[pstIr_AutoPrev->VpssChn].u32Width                    = stSize.u32Width;
    astVpssChnAttr[pstIr_AutoPrev->VpssChn].u32Height                   = stSize.u32Height;
    astVpssChnAttr[pstIr_AutoPrev->VpssChn].enChnMode                   = VPSS_CHN_MODE_USER;
    astVpssChnAttr[pstIr_AutoPrev->VpssChn].enCompressMode              = COMPRESS_MODE_SEG;
    astVpssChnAttr[pstIr_AutoPrev->VpssChn].enDynamicRange              = enDynamicRange;
    astVpssChnAttr[pstIr_AutoPrev->VpssChn].enVideoFormat               = enVideoFormat;
    astVpssChnAttr[pstIr_AutoPrev->VpssChn].enPixelFormat               = enPixFormat;
    astVpssChnAttr[pstIr_AutoPrev->VpssChn].stFrameRate.s32SrcFrameRate = -1;
    astVpssChnAttr[pstIr_AutoPrev->VpssChn].stFrameRate.s32DstFrameRate = -1;
    astVpssChnAttr[pstIr_AutoPrev->VpssChn].u32Depth                    = 0;
    astVpssChnAttr[pstIr_AutoPrev->VpssChn].bMirror                     = HI_FALSE;
    astVpssChnAttr[pstIr_AutoPrev->VpssChn].bFlip                       = HI_FALSE;
    astVpssChnAttr[pstIr_AutoPrev->VpssChn].stAspectRatio.enMode        = ASPECT_RATIO_NONE;

    astVpssChnAttr[VPSS_CHN1].u32Width                    = 720;
    astVpssChnAttr[VPSS_CHN1].u32Height                   = 576;
    astVpssChnAttr[VPSS_CHN1].enChnMode                   = VPSS_CHN_MODE_USER;
    astVpssChnAttr[VPSS_CHN1].enCompressMode              = enCompressMode;
    astVpssChnAttr[VPSS_CHN1].enDynamicRange              = enDynamicRange;
    astVpssChnAttr[VPSS_CHN1].enVideoFormat               = enVideoFormat;
    astVpssChnAttr[VPSS_CHN1].enPixelFormat               = enPixFormat;
    astVpssChnAttr[VPSS_CHN1].stFrameRate.s32SrcFrameRate = -1;
    astVpssChnAttr[VPSS_CHN1].stFrameRate.s32DstFrameRate = -1;
    astVpssChnAttr[VPSS_CHN1].u32Depth                    = 0;
    astVpssChnAttr[VPSS_CHN1].bMirror                     = HI_FALSE;
    astVpssChnAttr[VPSS_CHN1].bFlip                       = HI_FALSE;
    astVpssChnAttr[VPSS_CHN1].stAspectRatio.enMode        = ASPECT_RATIO_NONE;
    /*start vpss*/
    pstIr_AutoPrev->abChnEnable[0] = HI_TRUE;
    pstIr_AutoPrev->abChnEnable[1] = HI_TRUE;
    s32Ret = SAMPLE_COMM_VPSS_Start(pstIr_AutoPrev->VpssGrp, pstIr_AutoPrev->abChnEnable, &stVpssGrpAttr, astVpssChnAttr);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("start vpss group failed. s32Ret: 0x%x !\n", s32Ret);
        goto EXIT1;
    }

    /*config venc */
    stGopAttr.enGopMode  = VENC_GOPMODE_SMARTP;
    stGopAttr.stSmartP.s32BgQpDelta  = 7;
    stGopAttr.stSmartP.s32ViQpDelta  = 2;
    stGopAttr.stSmartP.u32BgInterval = 1200;
    s32Ret = SAMPLE_COMM_VENC_Start(pstIr_AutoPrev->VencChn[0], enType, enPicSize, enRcMode, u32Profile,bRcnRefShareBuf, &stGopAttr);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("start venc failed. s32Ret: 0x%x !\n", s32Ret);
        goto EXIT2;
    }

    s32Ret = SAMPLE_COMM_VPSS_Bind_VENC(pstIr_AutoPrev->VpssGrp, pstIr_AutoPrev->VpssChn, pstIr_AutoPrev->VencChn[0]);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("Venc bind Vpss failed. s32Ret: 0x%x !n", s32Ret);
        goto EXIT3;
    }

    /*config vo*/
    SAMPLE_COMM_VO_GetDefConfig(&pstIr_AutoPrev->stVoConfig);
    pstIr_AutoPrev->stVoConfig.enDstDynamicRange = enDynamicRange;
    pstIr_AutoPrev->stVoConfig.enVoIntfType = VO_INTF_BT1120;
    pstIr_AutoPrev->stVoConfig.enIntfSync   = VO_OUTPUT_576P50;
    pstIr_AutoPrev->stVoConfig.u32DisBufLen = 3;
    pstIr_AutoPrev->stVoConfig.enPicSize = enPicSize;

    /*start vo*/
    s32Ret = SAMPLE_COMM_VO_StartVO(&pstIr_AutoPrev->stVoConfig);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("start vo failed. s32Ret: 0x%x !\n", s32Ret);
        goto EXIT4;
    }

    /*vpss bind vo*/
    s32Ret = SAMPLE_COMM_VPSS_Bind_VO(pstIr_AutoPrev->VpssGrp, VPSS_CHN1, pstIr_AutoPrev->stVoConfig.VoDev, pstIr_AutoPrev->VoChn);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("vo bind vpss failed. s32Ret: 0x%x !\n", s32Ret);
        goto EXIT5;
    }

    s32Ret = SAMPLE_COMM_VENC_StartGetStream(pstIr_AutoPrev->VencChn, sizeof(pstIr_AutoPrev->VencChn)/sizeof(VENC_CHN));
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("Get venc stream failed!\n");
        goto EXIT6;
    }

    return HI_SUCCESS;

EXIT6:
    SAMPLE_COMM_VPSS_UnBind_VO(pstIr_AutoPrev->VpssGrp, VPSS_CHN1, pstIr_AutoPrev->stVoConfig.VoDev, pstIr_AutoPrev->VoChn);
EXIT5:
    SAMPLE_COMM_VO_StopVO(&pstIr_AutoPrev->stVoConfig);
EXIT4:
    SAMPLE_COMM_VPSS_UnBind_VENC(pstIr_AutoPrev->VpssGrp, pstIr_AutoPrev->VpssChn, pstIr_AutoPrev->VencChn[0]);
EXIT3:
    SAMPLE_COMM_VENC_Stop(pstIr_AutoPrev->VencChn[0]);
EXIT2:
    SAMPLE_COMM_VPSS_Stop(pstIr_AutoPrev->VpssGrp, pstIr_AutoPrev->abChnEnable);
EXIT1:
    SAMPLE_COMM_VI_StopVi(&pstIr_AutoPrev->stViConfig);
EXIT:
    SAMPLE_COMM_SYS_Exit();
    return s32Ret;

}

HI_S32 SAMPLE_IR_AUTO_STOP_PREV(IR_AUTO_PREV_S *pstIr_AutoPrev)
{
    //printf("%s,%d\n",__FUNCTION__,__LINE__);
    if(pstIr_AutoPrev == HI_NULL)
    {
        SAMPLE_PRT("Err: pstIr_AutoPrev is NULL \n");
        return HI_FAILURE;
    }

    SAMPLE_COMM_VENC_StopGetStream();
    SAMPLE_COMM_VPSS_UnBind_VO(pstIr_AutoPrev->VpssGrp, VPSS_CHN1, pstIr_AutoPrev->stVoConfig.VoDev, pstIr_AutoPrev->VoChn);
    SAMPLE_COMM_VO_StopVO(&pstIr_AutoPrev->stVoConfig);
    SAMPLE_COMM_VPSS_UnBind_VENC(pstIr_AutoPrev->VpssGrp, pstIr_AutoPrev->VpssChn, pstIr_AutoPrev->VencChn[0]);
    SAMPLE_COMM_VENC_Stop(pstIr_AutoPrev->VencChn[0]);
    SAMPLE_COMM_VPSS_Stop(pstIr_AutoPrev->VpssGrp, pstIr_AutoPrev->abChnEnable);
    SAMPLE_COMM_VI_StopVi(&pstIr_AutoPrev->stViConfig);
    SAMPLE_COMM_SYS_Exit();
    return HI_SUCCESS;
}

#ifndef __HuaweiLite__
HI_S32  s_s32MemDev = 0;
HI_S32 memopen( void )
{
    if (s_s32MemDev <= 0)
    {
        s_s32MemDev = open ("/dev/mem", O_CREAT|O_RDWR|O_SYNC);
        if (s_s32MemDev <= 0)
        {
            return -1;
        }
    }
    return 0;
}
HI_VOID memclose(void)
{
    close(s_s32MemDev);
}

void * memmap(HI_U32 u32PhyAddr, HI_U32 u32Size )
{
    HI_U32 u32Diff;
    HI_U32 u32PagePhy;
    HI_U32 u32PageSize;
    HI_U8 * pPageAddr;

    u32PagePhy = u32PhyAddr & 0xfffff000;
    u32Diff    = u32PhyAddr - u32PagePhy;

    u32PageSize = ((u32Size + u32Diff - 1) & 0xfffff000) + 0x1000;
    pPageAddr   = mmap ((void *)0, u32PageSize, PROT_READ|PROT_WRITE,
                                    MAP_SHARED, s_s32MemDev, u32PagePhy);
    if (MAP_FAILED == pPageAddr )
    {
        perror("mmap error\n");
        return NULL;
    }
    //printf("%08x, %08x\n", (HI_U32)pPageAddr, u32PageSize);
    return (void *) (pPageAddr + u32Diff);
}

HI_S32 memunmap(HI_VOID* pVirAddr, HI_U32 u32Size )
{
    HI_U32 u32PageAddr;
    HI_U32 u32PageSize;
    HI_U32 u32Diff;

    u32PageAddr = (((HI_U32)pVirAddr) & 0xfffff000);
    u32Diff     = (HI_U32)pVirAddr - u32PageAddr;
    u32PageSize = ((u32Size + u32Diff - 1) & 0xfffff000) + 0x1000;

    return munmap((HI_VOID*)u32PageAddr, u32PageSize);
}
#endif


static HI_S32 SAMPLE_IR_Set_Reg(HI_U32 u32Addr,HI_U32 u32Value)
{
#ifndef __HuaweiLite__
    HI_U32 *pPortVirAddr;
    pPortVirAddr =   memmap(u32Addr,sizeof(u32Value) );
    *pPortVirAddr = u32Value;
    memunmap(pPortVirAddr,sizeof(u32Value));
#else
    writel(u32Value, u32Addr);
#endif
    return 0;

}

//Normal Mode
void SAMPLE_IR_CUT_ENBLE(VI_PIPE ViPipe)
{   
    pthread_mutex_lock(&g_ir_mutex);
    if(g_ir_switch.cb)
    {
      g_ir_switch.cb(ViPipe, ISP_IR_STATUS_NORMAL, g_ir_switch.uargs);
    }
    pthread_mutex_unlock(&g_ir_mutex);
    
    g_astIrAttr[ViPipe].enIrStatus = ISP_IR_STATUS_NORMAL;
    return;
}
//IR MODE
void SAMPLE_IR_CUT_DISABLE(VI_PIPE ViPipe)
{
    pthread_mutex_lock(&g_ir_mutex);
    if(g_ir_switch.cb)
    {
      g_ir_switch.cb(ViPipe, ISP_IR_STATUS_IR, g_ir_switch.uargs);
    }
    pthread_mutex_unlock(&g_ir_mutex);
    g_astIrAttr[ViPipe].enIrStatus = ISP_IR_STATUS_IR;
    return;
}

void SAMPLE_IR_AUTO_Usage(char* sPrgNm)
{
    printf("Usage : %s <chip> <mode> <Normal2IrExpThr> <Ir2NormalExpThr> <RGMax> <RGMin> <BGMax> <BGMin> <IrStatus>\n", sPrgNm);

    printf("chip:\n");
    printf("\t 0) HI3516EV300 Demo board.\n");

    printf("mode:\n");
    printf("\t 0) SAMPLE_IR_CALIBRATION_MODE.\n");
    printf("\t 1) SAMPLE_IR_AUTO_MODE.\n");

    printf("Normal2IrExpThr:\n");
    printf("\t ISO threshold of switching from normal to IR mode.\n");

    printf("Ir2NormalExpThr:\n");
    printf("\t ISO threshold of switching from IR to normal mode.\n");

    printf("RGMax/RGMin/BGMax/BGMin:\n");
    printf("\t Maximum(Minimum) value of R/G(B/G) in IR scene.\n");

    printf("IrStatus:\n");
    printf("\t Current IR status. 0: Normal mode; 1: IR mode.\n");

    printf("e.g : %s 0 0 (SAMPLE_IR_CALIBRATION_MODE)\n", sPrgNm);
    printf("e.g : %s 0 1 (SAMPLE_IR_AUTO_MODE, default parameters)\n", sPrgNm);
    printf("e.g : %s 0 1 16000 400 280 190 280 190 0 (SAMPLE_IR_AUTO_MODE, user_define parameters)\n", sPrgNm);

    return;
}

HI_S32 ISP_IrSwitchToNormal(VI_PIPE ViPipe)
{
    HI_S32 s32Ret;
    ISP_SATURATION_ATTR_S stIspSaturationAttr;
    ISP_WB_ATTR_S stIspWbAttr;
    ISP_COLORMATRIX_ATTR_S stIspCCMAttr;

    /* switch ir-cut to normal */
    SAMPLE_IR_CUT_ENBLE(ViPipe);


    /* switch isp config to normal */
    s32Ret = HI_MPI_ISP_GetSaturationAttr(ViPipe, &stIspSaturationAttr);
    if (HI_SUCCESS != s32Ret)
    {
        printf("HI_MPI_ISP_GetSaturationAttr failed\n");
        return s32Ret;
    }
    stIspSaturationAttr.enOpType = OP_TYPE_AUTO;
    s32Ret = HI_MPI_ISP_SetSaturationAttr(ViPipe, &stIspSaturationAttr);
    if (HI_SUCCESS != s32Ret)
    {
        printf("HI_MPI_ISP_SetSaturationAttr failed\n");
        return s32Ret;
    }

    s32Ret = HI_MPI_ISP_GetWBAttr(ViPipe, &stIspWbAttr);
    if (HI_SUCCESS != s32Ret)
    {
        printf("HI_MPI_ISP_GetWBAttr failed\n");
        return s32Ret;
    }
    stIspWbAttr.enOpType = OP_TYPE_AUTO;
    s32Ret = HI_MPI_ISP_SetWBAttr(ViPipe, &stIspWbAttr);
    if (HI_SUCCESS != s32Ret)
    {
        printf("HI_MPI_ISP_SetWBAttr failed\n");
        return s32Ret;
    }

    s32Ret = HI_MPI_ISP_GetCCMAttr(ViPipe, &stIspCCMAttr);
    if (HI_SUCCESS != s32Ret)
    {
        printf("HI_MPI_ISP_GetCCMAttr failed\n");
        return s32Ret;
    }
    stIspCCMAttr.enOpType = OP_TYPE_AUTO;
    s32Ret = HI_MPI_ISP_SetCCMAttr(ViPipe, &stIspCCMAttr);
    if (HI_SUCCESS != s32Ret)
    {
        printf("HI_MPI_ISP_SetCCMAttr failed\n");
        return s32Ret;
    }

    return HI_SUCCESS;
}

HI_S32 ISP_IrSwitchToIr(VI_PIPE ViPipe)
{
    HI_S32 s32Ret;
    ISP_SATURATION_ATTR_S stIspSaturationAttr;
    ISP_WB_ATTR_S stIspWbAttr;
    ISP_COLORMATRIX_ATTR_S stIspCCMAttr;

    /* switch isp config to ir */
    s32Ret = HI_MPI_ISP_GetSaturationAttr(ViPipe, &stIspSaturationAttr);
    if (HI_SUCCESS != s32Ret)
    {
        printf("HI_MPI_ISP_GetSaturationAttr failed\n");
        return s32Ret;
    }
    stIspSaturationAttr.enOpType = OP_TYPE_MANUAL;
    stIspSaturationAttr.stManual.u8Saturation = 0;
    s32Ret = HI_MPI_ISP_SetSaturationAttr(ViPipe, &stIspSaturationAttr);
    if (HI_SUCCESS != s32Ret)
    {
        printf("HI_MPI_ISP_SetSaturationAttr failed\n");
        return s32Ret;
    }

    s32Ret = HI_MPI_ISP_GetWBAttr(ViPipe, &stIspWbAttr);
    if (HI_SUCCESS != s32Ret)
    {
        printf("HI_MPI_ISP_GetWBAttr failed\n");
        return s32Ret;
    }
    stIspWbAttr.enOpType = OP_TYPE_MANUAL;
    stIspWbAttr.stManual.u16Bgain  = 0x100;
    stIspWbAttr.stManual.u16Gbgain = 0x100;
    stIspWbAttr.stManual.u16Grgain = 0x100;
    stIspWbAttr.stManual.u16Rgain  = 0x100;
    s32Ret = HI_MPI_ISP_SetWBAttr(ViPipe, &stIspWbAttr);
    if (HI_SUCCESS != s32Ret)
    {
        printf("HI_MPI_ISP_SetWBAttr failed\n");
        return s32Ret;
    }

    s32Ret = HI_MPI_ISP_GetCCMAttr(ViPipe, &stIspCCMAttr);
    if (HI_SUCCESS != s32Ret)
    {
        printf("HI_MPI_ISP_GetCCMAttr failed\n");
        return s32Ret;
    }
    stIspCCMAttr.enOpType = OP_TYPE_MANUAL;
    stIspCCMAttr.stManual.au16CCM[0] = 0x100;
    stIspCCMAttr.stManual.au16CCM[1] = 0x0;
    stIspCCMAttr.stManual.au16CCM[2] = 0x0;
    stIspCCMAttr.stManual.au16CCM[3] = 0x0;
    stIspCCMAttr.stManual.au16CCM[4] = 0x100;
    stIspCCMAttr.stManual.au16CCM[5] = 0x0;
    stIspCCMAttr.stManual.au16CCM[6] = 0x0;
    stIspCCMAttr.stManual.au16CCM[7] = 0x0;
    stIspCCMAttr.stManual.au16CCM[8] = 0x100;
    s32Ret = HI_MPI_ISP_SetCCMAttr(ViPipe, &stIspCCMAttr);
    if (HI_SUCCESS != s32Ret)
    {
        printf("HI_MPI_ISP_SetCCMAttr failed\n");
        return s32Ret;
    }
    usleep(1000000);

    /* switch ir-cut to ir */
    SAMPLE_IR_CUT_DISABLE(ViPipe);


    return HI_SUCCESS;
}

HI_S32 SAMPLE_ISP_IrAutoRun(VI_PIPE ViPipe)
{
    HI_S32 s32Ret = HI_SUCCESS;
    ISP_IR_AUTO_ATTR_S stIrAttr;

    stIrAttr.bEnable            = g_astIrAttr[ViPipe].bEnable;
    stIrAttr.u32Normal2IrIsoThr = g_astIrAttr[ViPipe].u32Normal2IrIsoThr;
    stIrAttr.u32Ir2NormalIsoThr = g_astIrAttr[ViPipe].u32Ir2NormalIsoThr;
    stIrAttr.u32RGMax           = g_astIrAttr[ViPipe].u32RGMax;
    stIrAttr.u32RGMin           = g_astIrAttr[ViPipe].u32RGMin;
    stIrAttr.u32BGMax           = g_astIrAttr[ViPipe].u32BGMax;
    stIrAttr.u32BGMin           = g_astIrAttr[ViPipe].u32BGMin;
    stIrAttr.enIrStatus         = g_astIrAttr[ViPipe].enIrStatus;

    while (HI_TRUE == g_astIrThread[ViPipe].bThreadFlag)
    {
        /* run_interval: 40 ms */
        //usleep(40000);
        usleep(40000*4);
        
        if(!g_ir_switch.cb)
          continue;
        else
          stIrAttr.enIrStatus = g_astIrAttr[ViPipe].enIrStatus;
        
        if(g_ir_switch.cds)
        {
          int night = 0; 
          pthread_mutex_lock(&g_ir_mutex);
          if(g_ir_switch.cds)
          {
            night = g_ir_switch.cds(ViPipe, g_ir_switch.uargs);
          }
          pthread_mutex_unlock(&g_ir_mutex);
          if(stIrAttr.enIrStatus != night)
          {
            stIrAttr.enIrSwitch = (night)?ISP_IR_SWITCH_TO_IR:ISP_IR_SWITCH_TO_NORMAL;
          }
          else 
          {
            stIrAttr.enIrSwitch = ISP_IR_SWITCH_NONE;
          }
        }
        else 
        {
          s32Ret = HI_MPI_ISP_IrAutoRunOnce(ViPipe, &stIrAttr);
          if (HI_SUCCESS != s32Ret)
          {
              printf("HI_MPI_ISP_IrAutoRunOnce failed\n");
              return s32Ret;
          }
        }

        if (ISP_IR_SWITCH_TO_IR == stIrAttr.enIrSwitch) /* Normal to IR */
        {
            printf("\n[Normal -> IR]\n");

            s32Ret = ISP_IrSwitchToIr(ViPipe);
            if (HI_SUCCESS != s32Ret)
            {
                printf("ISP_IrSwitchToIr failed\n");
                return s32Ret;
            }

            g_astIrAttr[ViPipe].enIrStatus = stIrAttr.enIrStatus = ISP_IR_STATUS_IR;
        }
        else if (ISP_IR_SWITCH_TO_NORMAL == stIrAttr.enIrSwitch) /* IR to Normal */
        {
            printf("\n[IR -> Normal]\n");

            s32Ret = ISP_IrSwitchToNormal(ViPipe);
            if (HI_SUCCESS != s32Ret)
            {
                printf("ISP_IrSwitchToNormal failed\n");
                return s32Ret;
            }

            g_astIrAttr[ViPipe].enIrStatus = stIrAttr.enIrStatus = ISP_IR_STATUS_NORMAL;
        }
        else
        {}
    }

    return HI_SUCCESS;
}

static void* SAMPLE_ISP_IrAutoThread(void* param)
{
    VI_PIPE ViPipe;

    ViPipe = (VI_PIPE)param;

    prctl(PR_SET_NAME, (unsigned long)"isp_IrAuto", 0, 0, 0);

    SAMPLE_ISP_IrAutoRun(ViPipe);

	return NULL;
}

HI_S32 SAMPLE_ISP_IrAutoExit(VI_PIPE ViPipe)
{
    if (g_astIrThread[ViPipe].pThread)
    {
        g_astIrThread[ViPipe].bThreadFlag = HI_FALSE;
        pthread_join(g_astIrThread[ViPipe].pThread, NULL);
        g_astIrThread[ViPipe].pThread = 0;
    }

    //SAMPLE_IR_CUT_RELEASE();


    return HI_SUCCESS;
}
void SAMPLE_IR_AUTO_HandleSig(HI_S32 signo)
{
    int ret;

    if (SIGINT == signo || SIGTERM == signo)
    {
        ret = SAMPLE_ISP_IrAutoExit(ViPipe);
        if (ret != 0)
        {
            printf("SAMPLE_ISP_IrAutoExit failed\n");
            //exit(-1);
            return;
        }
    }
    //exit(-1);
    return;
}


int SAMPLE_IR_AUTO_Thread(int argc,char* argv[])
{
    HI_S32 s32Ret = HI_SUCCESS;
    HI_U32 u32Mode = SAMPLE_IR_AUTO_MODE;

    ViPipe = 0;
    u32Mode = atoi(argv[2]);
    //SAMPLE_IR_CUT_INIT();

    /* SAMPLE_IR_CALIBRATION_MODE */
    if (SAMPLE_IR_CALIBRATION_MODE == u32Mode)
    {
        ISP_WB_STATISTICS_S stStat;
        HI_U32 u32RG, u32BG;

        /* 1. Infrared scene without visible light */

        /* 2. Switch to IR */
        s32Ret = ISP_IrSwitchToIr(ViPipe);
        if (HI_SUCCESS != s32Ret)
        {
            printf("ISP_IrSwitchToIr failed\n");
            //SAMPLE_IR_CUT_RELEASE();
            return s32Ret;
        }

        /* 3. waiting for AE to stabilize */
        usleep(1000000);

        /* 4. calculate RGMax/RGMin/BGMax/BGMin */

        s32Ret = HI_MPI_ISP_GetWBStatistics(ViPipe, &stStat);
        if (HI_SUCCESS != s32Ret)
        {
            printf("HI_MPI_ISP_GetStatistics failed\n");
            //SAMPLE_IR_CUT_RELEASE();
            return s32Ret;
        }

        u32RG = ((HI_U32)stStat.u16GlobalR << 8) / DIV_0_TO_1(stStat.u16GlobalG);
        u32BG = ((HI_U32)stStat.u16GlobalB << 8) / DIV_0_TO_1(stStat.u16GlobalG);
        printf("RGMax:%d, RGMin:%d, BGMax:%d, BGMin:%d\n",
            u32RG * GAIN_MAX_COEF >> 8, u32RG * GAIN_MIN_COEF >> 8,
            u32BG * GAIN_MAX_COEF >> 8, u32BG * GAIN_MIN_COEF >> 8);
        //SAMPLE_IR_CUT_RELEASE();
        return HI_SUCCESS;
    }
    /* SAMPLE_IR_AUTO_MODE */
    if (argc > 8)
    {
        g_astIrAttr[ViPipe].u32Normal2IrIsoThr = atoi(argv[3]);
        g_astIrAttr[ViPipe].u32Ir2NormalIsoThr = atoi(argv[4]);
        g_astIrAttr[ViPipe].u32RGMax = atoi(argv[5]);
        g_astIrAttr[ViPipe].u32RGMin = atoi(argv[6]);
        g_astIrAttr[ViPipe].u32BGMax = atoi(argv[7]);
        g_astIrAttr[ViPipe].u32BGMin = atoi(argv[8]);
        g_astIrAttr[ViPipe].enIrStatus = atoi(argv[9]);
        if ((g_astIrAttr[ViPipe].enIrStatus != ISP_IR_STATUS_NORMAL) && (g_astIrAttr[ViPipe].enIrStatus != ISP_IR_STATUS_IR))
        {
            printf("the mode is invaild!\n");
            SAMPLE_IR_AUTO_Usage(argv[0]);
            return HI_SUCCESS;
        }
        printf("[UserConfig] Normal2IrIsoThr:%u, Ir2NormalIsoThr:%u, RG:[%u,%u], BG:[%u,%u], IrStatus:%d\n",
            g_astIrAttr[ViPipe].u32Normal2IrIsoThr, g_astIrAttr[ViPipe].u32Ir2NormalIsoThr,
            g_astIrAttr[ViPipe].u32RGMin, g_astIrAttr[ViPipe].u32RGMax,
            g_astIrAttr[ViPipe].u32BGMin, g_astIrAttr[ViPipe].u32BGMax,
            g_astIrAttr[ViPipe].enIrStatus);
    }

    g_astIrThread[ViPipe].bThreadFlag = HI_TRUE;

    s32Ret = pthread_create(&g_astIrThread[ViPipe].pThread, HI_NULL, SAMPLE_ISP_IrAutoThread, (HI_VOID *)ViPipe);
    if (0 != s32Ret)
    {
        printf("%s: create isp ir_auto thread failed!, error: %d, %s\r\n", __FUNCTION__, s32Ret, strerror(s32Ret));
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

HI_S32 ISP_IrMode(gsf_mpp_ir_t *ir)
{
  pthread_mutex_lock(&g_ir_mutex);
  if(ir)
    g_ir_switch = *ir;
  else
    g_ir_switch.cb = NULL;
  pthread_mutex_unlock(&g_ir_mutex);
  return 0;
}



HI_S32 ISP_IrSwitchAuto(VI_PIPE ViPipe)
{
    if(g_astIrThread[ViPipe].pThread)
      return 0;
  
    /* need to modified when sensor/lens/IR-cut/Infrared light changed */
    g_astIrAttr[ViPipe].bEnable = HI_TRUE;
    g_astIrAttr[ViPipe].u32Normal2IrIsoThr = 16000;
    g_astIrAttr[ViPipe].u32Ir2NormalIsoThr = 400;
    g_astIrAttr[ViPipe].u32RGMax = 280 *1.5;
    g_astIrAttr[ViPipe].u32RGMin = 190 /1.5;
    g_astIrAttr[ViPipe].u32BGMax = 280 *1.5;
    g_astIrAttr[ViPipe].u32BGMin = 190 /1.5;
    g_astIrAttr[ViPipe].enIrStatus = ISP_IR_STATUS_NORMAL;
    
    g_astIrThread[ViPipe].bThreadFlag = HI_TRUE;

    int s32Ret = pthread_create(&g_astIrThread[ViPipe].pThread, HI_NULL, SAMPLE_ISP_IrAutoThread, (HI_VOID *)ViPipe);
    if (0 != s32Ret)
    {
        printf("%s: create isp ir_auto thread failed!, error: %d, %s\r\n", __FUNCTION__, s32Ret, strerror(s32Ret));
        return HI_FAILURE;
    }    
}


/******************************************************************************
* function : to process abnormal case
******************************************************************************/
#ifndef __HuaweiLite__
void SAMPLE_VIO_HandleSig(HI_S32 signo)
{
    signal(SIGINT, SIG_IGN);
	signal(SIGTERM, SIG_IGN);

    exit(-1);
}
#endif

/******************************************************************************
* function    : main()
* Description : video preview sample
******************************************************************************/
#ifdef __HuaweiLite__
int app_main(int argc, char *argv[])
#else
int sample_ir_auto_main(int argc, char* argv[])
#endif
{
    HI_S32 s32Ret = HI_FAILURE;
    HI_U32 u32Mode = SAMPLE_IR_AUTO_MODE;
    ViPipe = 0;
    /* need to modified when sensor/lens/IR-cut/Infrared light changed */
    g_astIrAttr[ViPipe].bEnable = HI_TRUE;
    g_astIrAttr[ViPipe].u32Normal2IrIsoThr = 16000;
    g_astIrAttr[ViPipe].u32Ir2NormalIsoThr = 400;
    g_astIrAttr[ViPipe].u32RGMax = 280;
    g_astIrAttr[ViPipe].u32RGMin = 190;
    g_astIrAttr[ViPipe].u32BGMax = 280;
    g_astIrAttr[ViPipe].u32BGMin = 190;
    g_astIrAttr[ViPipe].enIrStatus = ISP_IR_STATUS_NORMAL;
#ifndef __HuaweiLite__
    signal(SIGINT, SAMPLE_VIO_HandleSig);
    signal(SIGTERM, SAMPLE_VIO_HandleSig);
#endif
    if (argc < 3)
    {
        SAMPLE_IR_AUTO_Usage(argv[0]);
    return HI_SUCCESS;
    }
    if (!strncmp(argv[1], "-h", 2))
    {
        SAMPLE_IR_AUTO_Usage(argv[0]);
        return HI_SUCCESS;
    }
    else
    {
        gu32Chip = atoi(argv[1]);
        u32Mode = atoi(argv[2]);
    }
    if ((u32Mode != SAMPLE_IR_AUTO_MODE) && (u32Mode != SAMPLE_IR_CALIBRATION_MODE))
    {
        printf("the mode is invaild!\n");
        SAMPLE_IR_AUTO_Usage(argv[0]);
        return HI_SUCCESS;
    }

    //if((gu32Chip != HI_3516EV300_CHIP) && (gu32Chip != HI_3516EV200_CHIP) && (gu32Chip != HI_3518EV300_CHIP))
    if((gu32Chip != HI_3516EV300_CHIP))
    {
        printf("only Support Hi3516EV300 Demo pulse Right Now!\n");
        SAMPLE_IR_AUTO_Usage(argv[0]);
        return HI_SUCCESS;
    }
#ifdef __HuaweiLite__
    IR_AUTO_PREV_S stIr_AutoPrev;
    HI_S32 s32ret;
    s32ret = SAMPLE_IR_AUTO_START_PREV(&stIr_AutoPrev);

    if (HI_SUCCESS == s32ret)
    {
        SAMPLE_PRT("ISP is now running normally\n");
    }
    else
    {
        SAMPLE_PRT("ISP is not running normally!Please check it\n");
        return -1;
    }
    printf("input anything to continue....\n");
    getchar();
#else
    memopen();
#endif
	sleep(2);
    SAMPLE_IR_AUTO_Thread(argc,argv);

    PAUSE();
    SAMPLE_ISP_IrAutoExit(ViPipe);
#ifdef __HuaweiLite__
    printf("input anything to continue....\n");
    getchar();
    SAMPLE_IR_AUTO_STOP_PREV(&stIr_AutoPrev);
#else
    memclose();
#endif
    return s32Ret;
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */
