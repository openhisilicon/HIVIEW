#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>
#include <pthread.h>
#include <sys/prctl.h>
#include <math.h>

#include "hi_common.h"
#include "hi_comm_video.h"
#include "hi_comm_sys.h"
#include "hi_comm_svp.h"

#include "sample_comm_ive.h"
#include "sample_comm_svp.h"
#include "sample_dsp_main.h"
#include "sample_dsp_enca.h"
#include "mpi_dsp.h"


typedef struct hiSAMPLE_SVP_DSP_DILATE_S
{
    SVP_SRC_IMAGE_S stSrc;
    SVP_DST_IMAGE_S stDst;
    SVP_MEM_INFO_S  stAssistBuf;
    SVP_DSP_PRI_E   enPri;
    SVP_DSP_ID_E    enDspId;
}SAMPLE_SVP_DSP_DILATE_S;

static SAMPLE_SVP_DSP_DILATE_S s_stDilate = {0};
static SAMPLE_VI_CONFIG_S s_stViConfig = {0};
static SAMPLE_IVE_SWITCH_S s_stDspSwitch = {HI_FALSE,HI_FALSE};
static HI_BOOL s_bDspStopSignal = HI_FALSE;
static pthread_t s_hDspThread = 0;

/*
*Uninit Dilate
*/
HI_VOID SAMPLE_SVP_DSP_DilateUninit(SAMPLE_SVP_DSP_DILATE_S *pstDilate)
{
    SAMPLE_COMM_SVP_DestroyMemInfo(&(pstDilate->stAssistBuf),0);
    SAMPLE_COMM_SVP_DestroyImage(&(pstDilate->stDst),0);
}
/*
*Init Dilate
*/
HI_S32 SAMPLE_SVP_DSP_DilateInit(SAMPLE_SVP_DSP_DILATE_S *pstDilate,
    HI_U32 u32Width,HI_U32 u32Height,SVP_DSP_ID_E enDspId,SVP_DSP_PRI_E enPri)
{
    HI_S32 s32Ret;
    HI_U32 u32Size = sizeof(SVP_IMAGE_S) * 2;
    memset(pstDilate,0,sizeof(*pstDilate));

    /*Do not malloc src address ,it get from vpss*/
    pstDilate->stSrc.u32Width  = u32Width;
    pstDilate->stSrc.u32Height = u32Height;
    pstDilate->stSrc.enType    = SVP_IMAGE_TYPE_U8C1;

    s32Ret = SAMPLE_COMM_SVP_CreateImage(&(pstDilate->stDst),SVP_IMAGE_TYPE_U8C1,u32Width,u32Height,0);
    SAMPLE_SVP_CHECK_EXPR_RET(HI_SUCCESS != s32Ret, s32Ret, SAMPLE_SVP_ERR_LEVEL_ERROR, "Error(%#x):SAMPLE_COMM_SVP_CreateImage failed!\n", s32Ret);

    s32Ret = SAMPLE_COMM_SVP_CreateMemInfo(&(pstDilate->stAssistBuf),u32Size,0);
    SAMPLE_SVP_CHECK_EXPR_GOTO(HI_SUCCESS != s32Ret, FAIL_0, SAMPLE_SVP_ERR_LEVEL_ERROR, "Error(%#x):SAMPLE_COMM_SVP_CreateMemInfo failed!\n", s32Ret);

    pstDilate->enDspId = enDspId;
    pstDilate->enPri   = enPri;
    return s32Ret;
FAIL_0:
    SAMPLE_COMM_SVP_DestroyImage(&(pstDilate->stDst),0);
    memset(pstDilate,0,sizeof(*pstDilate));
    return s32Ret;
}
/*
*Process Dilate
*/
static HI_S32 SAMPLE_SVP_DSP_DilateProc(SAMPLE_SVP_DSP_DILATE_S* pstDilate, VIDEO_FRAME_INFO_S* pstExtFrmInfo)
{
    SVP_DSP_HANDLE hHandle;
    HI_BOOL bFinish;
    HI_BOOL bBlock = HI_TRUE;
    HI_S32 s32Ret;
    /*Ony get YVU400*/
    pstDilate->stSrc.au64PhyAddr[0] = pstExtFrmInfo->stVFrame.u64PhyAddr[0];
    pstDilate->stSrc.au64VirAddr[0] = pstExtFrmInfo->stVFrame.u64VirAddr[0];
    pstDilate->stSrc.au32Stride[0]  = pstExtFrmInfo->stVFrame.u32Stride[0];
    /*Call enca mpi*/
    s32Ret = SAMPLE_SVP_DSP_ENCA_Dilate3x3(&hHandle, pstDilate->enDspId,pstDilate->enPri, &pstDilate->stSrc, &pstDilate->stDst, &(pstDilate->stAssistBuf));
    SAMPLE_SVP_CHECK_EXPR_RET(HI_SUCCESS != s32Ret, s32Ret, SAMPLE_SVP_ERR_LEVEL_ERROR, "Error(%#x):HI_MPI_SVP_DSP_ENCA_Dilate3x3 failed!\n", s32Ret);
    /*Wait dsp finish*/
    while(HI_ERR_SVP_DSP_QUERY_TIMEOUT == (s32Ret = HI_MPI_SVP_DSP_Query(pstDilate->enDspId, hHandle, &bFinish, bBlock)))
    {
        usleep(100);
    }

    return s32Ret;
}

/*
*Process thread function
*/
static HI_VOID* SAMPLE_SVP_DSP_ViToVo(HI_VOID* pArgs)
{
    HI_S32 s32Ret;
    SAMPLE_SVP_DSP_DILATE_S *pstDilate;
    VIDEO_FRAME_INFO_S stBaseFrmInfo;
    VIDEO_FRAME_INFO_S stExtFrmInfo;
    HI_S32 s32MilliSec = 20000;
    VO_LAYER voLayer = 0;
    VO_CHN voChn = 0;
    HI_S32 s32VpssGrp = 0;
    HI_S32 as32VpssChn[] = {VPSS_CHN0, VPSS_CHN1};

    pstDilate = (SAMPLE_SVP_DSP_DILATE_S*)pArgs;

    while (HI_FALSE == s_bDspStopSignal)
    {
        s32Ret = HI_MPI_VPSS_GetChnFrame(s32VpssGrp, as32VpssChn[1], &stExtFrmInfo, s32MilliSec);
        if(HI_SUCCESS != s32Ret)
        {
            SAMPLE_PRT("Error(%#x),HI_MPI_VPSS_GetChnFrame failed, VPSS_GRP(%d), VPSS_CHN(%d)!\n",
                s32Ret,s32VpssGrp, as32VpssChn[1]);
            continue;
        }

        s32Ret = HI_MPI_VPSS_GetChnFrame(s32VpssGrp, as32VpssChn[0], &stBaseFrmInfo, s32MilliSec);
        SAMPLE_CHECK_EXPR_GOTO(HI_SUCCESS!=s32Ret, EXT_RELEASE,
            "Error(%#x),HI_MPI_VPSS_GetChnFrame failed, VPSS_GRP(%d), VPSS_CHN(%d)!\n",
            s32Ret,s32VpssGrp, as32VpssChn[0]);

        s32Ret = SAMPLE_SVP_DSP_DilateProc(pstDilate,&stExtFrmInfo);
        SAMPLE_CHECK_EXPR_GOTO(HI_SUCCESS!=s32Ret, BASE_RELEASE,
            "Error(%#x),SAMPLE_SVP_DSP_DilateProc failed!\n", s32Ret);

        s32Ret = HI_MPI_VO_SendFrame(voLayer, voChn, &stBaseFrmInfo, s32MilliSec);
		SAMPLE_CHECK_EXPR_GOTO(HI_SUCCESS!=s32Ret, BASE_RELEASE,
			"HI_MPI_VO_SendFrame failed, Error(%#x)!\n", s32Ret);

        BASE_RELEASE:
            s32Ret = HI_MPI_VPSS_ReleaseChnFrame(s32VpssGrp,as32VpssChn[0], &stBaseFrmInfo);
            if (HI_SUCCESS != s32Ret)
            {
                SAMPLE_PRT("Error(%#x),HI_MPI_VPSS_ReleaseChnFrame failed,Grp(%d) chn(%d)!\n",
                    s32Ret,s32VpssGrp,as32VpssChn[0]);
            }

        EXT_RELEASE:
            s32Ret = HI_MPI_VPSS_ReleaseChnFrame(s32VpssGrp,as32VpssChn[1], &stExtFrmInfo);
            if (HI_SUCCESS != s32Ret)
            {
                SAMPLE_PRT("Error(%#x),HI_MPI_VPSS_ReleaseChnFrame failed,Grp(%d) chn(%d)!\n",
                    s32Ret,s32VpssGrp,as32VpssChn[1]);
            }

    }

    return HI_NULL;
}

static HI_VOID SAMPLE_SVP_DSP_DilateCore(SVP_DSP_ID_E enDspId,SVP_DSP_PRI_E enPri)
{
    HI_S32 s32Ret;
    PIC_SIZE_E enSize = PIC_CIF;
    SIZE_S stSize;
    HI_CHAR acThreadName[16] = {0};

    s_stDspSwitch.bVo   = HI_TRUE;
    s_stDspSwitch.bVenc = HI_FALSE;

    s32Ret = SAMPLE_COMM_IVE_StartViVpssVencVo(&s_stViConfig,&s_stDspSwitch,&enSize);
    SAMPLE_CHECK_EXPR_GOTO(HI_SUCCESS != s32Ret, END_DSP_0,
        "Error(%#x),SAMPLE_COMM_IVE_StartViVpssVencVo failed!\n", s32Ret);

    s32Ret = SAMPLE_COMM_SYS_GetPicSize(enSize, &stSize);
    SAMPLE_CHECK_EXPR_GOTO(HI_SUCCESS != s32Ret, END_DSP_0,
        "Error(%#x),SAMPLE_COMM_SYS_GetPicSize failed!\n", s32Ret);

    /*Load bin*/
    s32Ret = SAMPLE_COMM_SVP_LoadCoreBinary(enDspId);
    SAMPLE_SVP_CHECK_EXPR_GOTO(HI_SUCCESS != s32Ret,END_DSP_0, SAMPLE_SVP_ERR_LEVEL_ERROR, "Error(%#x):SAMPLE_COMM_SVP_LoadCoreBinary failed!\n", s32Ret);
    /*Init*/
    s32Ret = SAMPLE_SVP_DSP_DilateInit(&s_stDilate,stSize.u32Width,stSize.u32Height,enDspId,enPri);
    SAMPLE_SVP_CHECK_EXPR_GOTO(HI_SUCCESS != s32Ret, END_DSP_1, SAMPLE_SVP_ERR_LEVEL_ERROR, "Error(%#x):SAMPLE_SVP_DSP_DilateInit failed!\n", s32Ret);

    snprintf(acThreadName, 16, "DSP_ViToVo");
    prctl(PR_SET_NAME, (unsigned long)acThreadName, 0,0,0);
    pthread_create(&s_hDspThread, 0, SAMPLE_SVP_DSP_ViToVo, (HI_VOID*)&s_stDilate);

    SAMPLE_PAUSE();

    s_bDspStopSignal = HI_TRUE;
    pthread_join(s_hDspThread, HI_NULL);
    s_hDspThread = 0;

  SAMPLE_SVP_DSP_DilateUninit(&s_stDilate);
END_DSP_1:
  SAMPLE_COMM_SVP_UnLoadCoreBinary(enDspId);
END_DSP_0:
  SAMPLE_COMM_IVE_StopViVpssVencVo(&s_stViConfig,&s_stDspSwitch);

}

/*
*Dilate sample
*/
HI_VOID SAMPLE_SVP_DSP_Dilate(HI_VOID)
{
    SVP_DSP_PRI_E enPri = SVP_DSP_PRI_0;
    SVP_DSP_ID_E enDspId = SVP_DSP_ID_0;
    SAMPLE_SVP_DSP_DilateCore(enDspId,enPri);
}

/*
*Dilate single handle
*/
HI_VOID SAMPLE_SVP_DSP_DilateHandleSig(HI_VOID)
{
    s_bDspStopSignal = HI_TRUE;
    if (0 != s_hDspThread)
    {
        pthread_join(s_hDspThread, HI_NULL);
        s_hDspThread = 0;
    }
    SAMPLE_SVP_DSP_DilateUninit(&s_stDilate);
    SAMPLE_COMM_SVP_UnLoadCoreBinary(s_stDilate.enDspId);
    memset(&s_stDilate,0,sizeof(s_stDilate));
    SAMPLE_COMM_IVE_StopViVpssVencVo(&s_stViConfig,&s_stDspSwitch);
}


