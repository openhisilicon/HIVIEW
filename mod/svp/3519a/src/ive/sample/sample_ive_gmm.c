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
#include <semaphore.h>
#include <pthread.h>
#include <sys/prctl.h>

#include "sample_comm_ive.h"

typedef struct hiSAMPLE_IVE_GMM_S
{
    IVE_SRC_IMAGE_S stSrc;
    IVE_DST_IMAGE_S stFg;
    IVE_DST_IMAGE_S stBg;
    IVE_MEM_INFO_S  stModel;
    IVE_IMAGE_S    stImg1;
    IVE_IMAGE_S    stImg2;
    IVE_DST_MEM_INFO_S stBlob;
    IVE_GMM_CTRL_S  stGmmCtrl;
    IVE_CCL_CTRL_S  stCclCtrl;
    IVE_FILTER_CTRL_S stFltCtrl;
    IVE_DILATE_CTRL_S stDilateCtrl;
    IVE_ERODE_CTRL_S stErodeCtrl;
    SAMPLE_RECT_ARRAY_S stRegion;
} SAMPLE_IVE_GMM_S;

typedef struct hiSAMPLE_IVE_GMM_INFO_S
{
    SAMPLE_IVE_GMM_S stGmm;
    HI_BOOL bEncode;
    HI_BOOL bVo;
} SAMPLE_IVE_GMM_INFO_S;

static HI_BOOL s_bStopSignal = HI_FALSE;
static pthread_t s_hIveThread = 0;
static SAMPLE_IVE_GMM_INFO_S s_stGmmInfo;
static SAMPLE_IVE_SWITCH_S s_stGmmSwitch = {HI_FALSE,HI_FALSE};
static SAMPLE_VI_CONFIG_S s_stViConfig = {0};

static HI_VOID SAMPLE_IVE_Gmm_Uninit(SAMPLE_IVE_GMM_S* pstGmm)
{
    IVE_MMZ_FREE(pstGmm->stSrc.au64PhyAddr[0], pstGmm->stSrc.au64VirAddr[0]);
    IVE_MMZ_FREE(pstGmm->stFg.au64PhyAddr[0], pstGmm->stFg.au64VirAddr[0]);
    IVE_MMZ_FREE(pstGmm->stBg.au64PhyAddr[0], pstGmm->stBg.au64VirAddr[0]);
    IVE_MMZ_FREE(pstGmm->stModel.u64PhyAddr, pstGmm->stModel.u64VirAddr);
    IVE_MMZ_FREE(pstGmm->stImg1.au64PhyAddr[0], pstGmm->stImg1.au64VirAddr[0]);
    IVE_MMZ_FREE(pstGmm->stImg2.au64PhyAddr[0], pstGmm->stImg2.au64VirAddr[0]);
    IVE_MMZ_FREE(pstGmm->stBlob.u64PhyAddr, pstGmm->stBlob.u64VirAddr);
}


static HI_S32 SAMPLE_IVE_Gmm_Init(SAMPLE_IVE_GMM_S* pstGmm, HI_U32 u32Width, HI_U32 u32Height)
{
    HI_S32 s32Ret = HI_SUCCESS;
    HI_U32 u32Size = 0;
    HI_S8 as8Mask[25] = {1, 2, 3, 2, 1,
                         2, 5, 6, 5, 2,
                         3, 6, 8, 6, 3,
                         2, 5, 6, 5, 2,
                         1, 2, 3, 2, 1
                        };

    memset(pstGmm, 0, sizeof(SAMPLE_IVE_GMM_S));

    s32Ret = SAMPLE_COMM_IVE_CreateImage(&(pstGmm->stSrc), IVE_IMAGE_TYPE_U8C1, u32Width, u32Height);
    SAMPLE_CHECK_EXPR_GOTO(HI_SUCCESS != s32Ret, GMM_INIT_FAIL,
        "Error(%#x),Create src image failed!\n", s32Ret);

    s32Ret = SAMPLE_COMM_IVE_CreateImage(&(pstGmm->stFg), IVE_IMAGE_TYPE_U8C1, u32Width, u32Height);
    SAMPLE_CHECK_EXPR_GOTO(HI_SUCCESS != s32Ret, GMM_INIT_FAIL,
        "Error(%#x),Create fg image failed!\n", s32Ret);

    s32Ret = SAMPLE_COMM_IVE_CreateImage(&(pstGmm->stBg), IVE_IMAGE_TYPE_U8C1, u32Width, u32Height);
    SAMPLE_CHECK_EXPR_GOTO(HI_SUCCESS != s32Ret, GMM_INIT_FAIL,
        "Error(%#x),Create bg image failed!\n", s32Ret);

    pstGmm->stGmmCtrl.u0q16InitWeight = 3276; //0.05
    pstGmm->stGmmCtrl.u0q16BgRatio = 52428;   //0.8
    pstGmm->stGmmCtrl.u22q10MaxVar = (2000 << 10);
    pstGmm->stGmmCtrl.u22q10MinVar = (200 << 10);
    pstGmm->stGmmCtrl.u22q10NoiseVar = (225 << 10);
    pstGmm->stGmmCtrl.u8q8VarThr = 1600;
    pstGmm->stGmmCtrl.u8ModelNum = 3;
    pstGmm->stGmmCtrl.u0q16LearnRate = 327;

    u32Size = pstGmm->stSrc.u32Width * pstGmm->stSrc.u32Height * pstGmm->stGmmCtrl.u8ModelNum * 7;
    s32Ret = SAMPLE_COMM_IVE_CreateMemInfo(&pstGmm->stModel, u32Size);
    SAMPLE_CHECK_EXPR_GOTO(HI_SUCCESS != s32Ret, GMM_INIT_FAIL,
        "Error(%#x),Create model mem info failed!\n", s32Ret);
    memset(SAMPLE_COMM_IVE_CONVERT_64BIT_ADDR(HI_VOID,pstGmm->stModel.u64VirAddr), 0, pstGmm->stModel.u32Size);

    s32Ret = SAMPLE_COMM_IVE_CreateImage(&(pstGmm->stImg1), IVE_IMAGE_TYPE_U8C1, u32Width, u32Height);
      SAMPLE_CHECK_EXPR_GOTO(HI_SUCCESS != s32Ret, GMM_INIT_FAIL,
        "Error(%#x),Create img1 image failed!\n", s32Ret);

    s32Ret = SAMPLE_COMM_IVE_CreateImage(&(pstGmm->stImg2), IVE_IMAGE_TYPE_U8C1, u32Width, u32Height);
      SAMPLE_CHECK_EXPR_GOTO(HI_SUCCESS != s32Ret, GMM_INIT_FAIL,
        "Error(%#x),Create img2 image failed!\n", s32Ret);

    u32Size = sizeof(IVE_CCBLOB_S);
    s32Ret = SAMPLE_COMM_IVE_CreateMemInfo(&pstGmm->stBlob, u32Size);
      SAMPLE_CHECK_EXPR_GOTO(HI_SUCCESS != s32Ret, GMM_INIT_FAIL,
        "Error(%#x),Create blob mem info failed!\n", s32Ret);

    memcpy(pstGmm->stFltCtrl.as8Mask, as8Mask, 25);
    pstGmm->stFltCtrl.u8Norm = 7;
    memset(pstGmm->stDilateCtrl.au8Mask, 255, 25);
    memset(pstGmm->stErodeCtrl.au8Mask, 255, 25);
    pstGmm->stCclCtrl.u16InitAreaThr     = 16;
    pstGmm->stCclCtrl.u16Step             = 4;
    pstGmm->stCclCtrl.enMode             = IVE_CCL_MODE_8C;

GMM_INIT_FAIL:

    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_IVE_Gmm_Uninit(pstGmm);
    }
    return s32Ret;
}

static HI_S32 SAMPLE_IVE_GmmProc(SAMPLE_IVE_GMM_INFO_S* pstGmmInfo, VIDEO_FRAME_INFO_S* pstExtFrmInfo,HI_U32 u32BaseWidth,HI_U32 u32BaseHeight)
{
    HI_S32 s32Ret;
    SAMPLE_IVE_GMM_S* pstGmm;
    IVE_HANDLE IveHandle;
    HI_BOOL bFinish = HI_FALSE;
    HI_BOOL bBlock = HI_TRUE;
    HI_BOOL bInstant = HI_TRUE;
    IVE_CCBLOB_S* pstBlob;

    pstGmm = &pstGmmInfo->stGmm;
    pstBlob = SAMPLE_COMM_IVE_CONVERT_64BIT_ADDR(IVE_CCBLOB_S,pstGmm->stBlob.u64VirAddr);

    //1.Get Y
    bInstant = HI_FALSE;
    s32Ret = SAMPLE_COMM_IVE_DmaImage(pstExtFrmInfo,&pstGmm->stSrc,bInstant);
    SAMPLE_CHECK_EXPR_RET(HI_SUCCESS != s32Ret,s32Ret,"Error(%#x),SAMPLE_COMM_IVE_DmaImage failed!\n",s32Ret);

    //2.Filter
    s32Ret = HI_MPI_IVE_Filter(&IveHandle, &pstGmm->stSrc, &pstGmm->stImg1, &pstGmm->stFltCtrl, bInstant);
    SAMPLE_CHECK_EXPR_RET(HI_SUCCESS != s32Ret,s32Ret,"Error(%#x),HI_MPI_IVE_Filter failed!\n",s32Ret);

    //3.Gmm
    s32Ret = HI_MPI_IVE_GMM(&IveHandle, &pstGmm->stImg1, &pstGmm->stFg, &pstGmm->stBg, &pstGmm->stModel, &pstGmm->stGmmCtrl, bInstant);
     SAMPLE_CHECK_EXPR_RET(HI_SUCCESS != s32Ret,s32Ret,"Error(%#x),HI_MPI_IVE_GMM failed!\n",s32Ret);

    //4.Dilate
    s32Ret = HI_MPI_IVE_Dilate(&IveHandle, &pstGmm->stFg, &pstGmm->stImg1, &pstGmm->stDilateCtrl, bInstant);
     SAMPLE_CHECK_EXPR_RET(HI_SUCCESS != s32Ret,s32Ret,"Error(%#x),HI_MPI_IVE_Dilate failed!\n",s32Ret);

    //5.Erode
    s32Ret = HI_MPI_IVE_Erode(&IveHandle, &pstGmm->stImg1, &pstGmm->stImg2, &pstGmm->stErodeCtrl, bInstant);
    SAMPLE_CHECK_EXPR_RET(HI_SUCCESS != s32Ret,s32Ret,"Error(%#x),HI_MPI_IVE_Erode failed!\n",s32Ret);

    //6.CCL
    bInstant = HI_TRUE;
    s32Ret = HI_MPI_IVE_CCL(&IveHandle, &pstGmm->stImg2, &pstGmm->stBlob, &pstGmm->stCclCtrl, bInstant);
    SAMPLE_CHECK_EXPR_RET(HI_SUCCESS != s32Ret,s32Ret,"Error(%#x),HI_MPI_IVE_CCL failed!\n",s32Ret);

    //Wait task finish
    s32Ret = HI_MPI_IVE_Query(IveHandle, &bFinish, bBlock);
    while (HI_ERR_IVE_QUERY_TIMEOUT == s32Ret)
    {
        usleep(100);
        s32Ret = HI_MPI_IVE_Query(IveHandle, &bFinish, bBlock);
    }
    SAMPLE_CHECK_EXPR_RET(HI_SUCCESS != s32Ret,s32Ret,"Error(%#x),HI_MPI_IVE_Query failed!\n",s32Ret);

    SAMPLE_COMM_IVE_BlobToRect(pstBlob, &(pstGmm->stRegion), IVE_RECT_NUM, 8,
        pstExtFrmInfo->stVFrame.u32Width, pstExtFrmInfo->stVFrame.u32Height,
        u32BaseWidth,u32BaseHeight);

    return HI_SUCCESS;
}


static HI_VOID* SAMPLE_IVE_ViToVo(HI_VOID* pArgs)
{
    HI_S32 s32Ret;
    SAMPLE_IVE_GMM_INFO_S *pstGmmInfo;
    SAMPLE_IVE_GMM_S *pstGmm;
    VIDEO_FRAME_INFO_S stBaseFrmInfo;
    VIDEO_FRAME_INFO_S stExtFrmInfo;
    HI_S32 s32MilliSec = 20000;
    VO_LAYER voLayer = 0;
    VO_CHN voChn = 0;
    VENC_CHN vencChn = 0;
    HI_BOOL bEncode;
    HI_BOOL bVo;
    HI_S32 s32VpssGrp = 0;
    HI_S32 as32VpssChn[] = {VPSS_CHN0, VPSS_CHN1};

    pstGmmInfo = (SAMPLE_IVE_GMM_INFO_S*)pArgs;
    pstGmm = &(pstGmmInfo->stGmm);
    bEncode = pstGmmInfo->bEncode;
    bVo = pstGmmInfo->bVo;

    while (HI_FALSE == s_bStopSignal)
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

        s32Ret = SAMPLE_IVE_GmmProc(pstGmmInfo, &stExtFrmInfo,stBaseFrmInfo.stVFrame.u32Width,
            stBaseFrmInfo.stVFrame.u32Height);
        SAMPLE_CHECK_EXPR_GOTO(HI_SUCCESS!=s32Ret, BASE_RELEASE,
            "Error(%#x),SAMPLE_IVE_GmmProc failed!\n", s32Ret);

        //Draw rect
        s32Ret = SAMPLE_COMM_VGS_FillRect(&stBaseFrmInfo, &pstGmm->stRegion, 0x0000FF00);
		SAMPLE_CHECK_EXPR_GOTO(HI_SUCCESS!=s32Ret, BASE_RELEASE,
			"SAMPLE_COMM_VGS_FillRect failed, Error(%#x)!\n", s32Ret);

        //Venc
        if (HI_TRUE == bEncode)
        {
            s32Ret = HI_MPI_VENC_SendFrame(vencChn, &stBaseFrmInfo, s32MilliSec);
			SAMPLE_CHECK_EXPR_GOTO(HI_SUCCESS!=s32Ret, BASE_RELEASE,
				"HI_MPI_VENC_SendFrame failed, Error(%#x)!\n", s32Ret);
        }

        //Vo
        if (HI_TRUE == bVo)
        {
            s32Ret = HI_MPI_VO_SendFrame(voLayer, voChn, &stBaseFrmInfo, s32MilliSec);
			SAMPLE_CHECK_EXPR_GOTO(HI_SUCCESS!=s32Ret, BASE_RELEASE,
				"HI_MPI_VO_SendFrame failed, Error(%#x)!\n", s32Ret);
        }

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

/******************************************************************************
* function : show Gmm sample
******************************************************************************/
HI_VOID SAMPLE_IVE_Gmm(HI_CHAR chEncode, HI_CHAR chVo)
{
    SIZE_S stSize;
    PIC_SIZE_E enSize = PIC_CIF;
    HI_BOOL bEncode = '1' ==chEncode ? HI_TRUE : HI_FALSE;
    HI_BOOL bVo = '1' == chVo ? HI_TRUE : HI_FALSE;
    HI_S32 s32Ret = HI_SUCCESS;
    HI_CHAR acThreadName[16] = {0};

    memset(&s_stGmmInfo,0,sizeof(s_stGmmInfo));
    /******************************************
     step 1: start vi vpss venc vo
     ******************************************/
    s_stGmmSwitch.bVenc = bEncode;
    s_stGmmSwitch.bVo   = bVo;
    s32Ret = SAMPLE_COMM_IVE_StartViVpssVencVo(&s_stViConfig,&s_stGmmSwitch,&enSize);
    SAMPLE_CHECK_EXPR_GOTO(HI_SUCCESS != s32Ret, END_GMM_0,
        "Error(%#x),SAMPLE_COMM_IVE_StartViVpssVencVo failed!\n", s32Ret);

    s32Ret = SAMPLE_COMM_SYS_GetPicSize(enSize, &stSize);
    SAMPLE_CHECK_EXPR_GOTO(HI_SUCCESS != s32Ret, END_GMM_0,
        "Error(%#x),SAMPLE_COMM_SYS_GetPicSize failed!\n", s32Ret);
    /******************************************
      step 2: Init GMM
     ******************************************/
    s32Ret = SAMPLE_IVE_Gmm_Init(&s_stGmmInfo.stGmm, stSize.u32Width, stSize.u32Height);
    SAMPLE_CHECK_EXPR_GOTO(HI_SUCCESS != s32Ret, END_GMM_0,
        "Error(%#x),SAMPLE_IVE_Gmm_Init failed!\n", s32Ret);

    s_stGmmInfo.bEncode = bEncode;
    s_stGmmInfo.bVo = bVo;
    s_bStopSignal = HI_FALSE;
    /******************************************
      step 3: Create work thread
     ******************************************/
    snprintf(acThreadName, 16, "IVE_ViToVo");
    prctl(PR_SET_NAME, (unsigned long)acThreadName, 0,0,0);
    pthread_create(&s_hIveThread, 0, SAMPLE_IVE_ViToVo, (HI_VOID*)&s_stGmmInfo);

    SAMPLE_PAUSE();

    s_bStopSignal = HI_TRUE;
    pthread_join(s_hIveThread, HI_NULL);
    s_hIveThread = 0;
    SAMPLE_IVE_Gmm_Uninit(&s_stGmmInfo.stGmm);
    memset(&s_stGmmInfo,0,sizeof(s_stGmmInfo));

END_GMM_0:
    SAMPLE_COMM_IVE_StopViVpssVencVo(&s_stViConfig,&s_stGmmSwitch);
    return ;
}

/******************************************************************************
* function : Gmm sample signal handle
******************************************************************************/
HI_VOID SAMPLE_IVE_Gmm_HandleSig(HI_VOID)
{
    s_bStopSignal = HI_TRUE;
    if (0 != s_hIveThread)
    {
        pthread_join(s_hIveThread, HI_NULL);
        s_hIveThread = 0;
    }
    SAMPLE_IVE_Gmm_Uninit(&s_stGmmInfo.stGmm);
    memset(&s_stGmmInfo,0,sizeof(s_stGmmInfo));
    SAMPLE_COMM_IVE_StopViVpssVencVo(&s_stViConfig,&s_stGmmSwitch);
}

