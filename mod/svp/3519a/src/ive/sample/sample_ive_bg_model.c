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


typedef struct hiSAMPLE_IVE_BG_MODEL_S
{
    IVE_SRC_IMAGE_S stCurImg;
    IVE_IMAGE_S stFgFlag;
    IVE_IMAGE_S  stImg1;
    IVE_IMAGE_S  stImg2;
    IVE_DST_IMAGE_S stBgDiffFg;
    IVE_DST_IMAGE_S stFrmDiffFg;
    IVE_DST_IMAGE_S stBgImg;
    IVE_DST_IMAGE_S stChgStaImg;
    IVE_DST_IMAGE_S stChgStaFg;
    IVE_DST_IMAGE_S stChgStaLife;
    IVE_DST_MEM_INFO_S  stStatData;
    IVE_DST_MEM_INFO_S stBlob;
    IVE_DATA_S stBgModel;
    IVE_CCL_CTRL_S  stCclCtrl;
    IVE_FILTER_CTRL_S stFltCtrl;
    IVE_DILATE_CTRL_S stDilateCtrl;
    IVE_ERODE_CTRL_S stErodeCtrl;
    IVE_MATCH_BG_MODEL_CTRL_S stMatchBgModelCtrl;
    IVE_UPDATE_BG_MODEL_CTRL_S stUpdateBgModelCtrl;
    SAMPLE_RECT_ARRAY_S stRegion;
    HI_U32 u32FrameCnt;
    HI_U32 u32UpdCnt;
    HI_U32 u32PreUpdTime;
    HI_U32 u32PreChkTime;
} SAMPLE_IVE_BG_MODEL_S;

typedef struct hiSAMPLE_IVE_BG_MODEL_INFO_S
{
    SAMPLE_IVE_BG_MODEL_S stBgModel;
    HI_BOOL bEncode;
    HI_BOOL bVo;
} SAMPLE_IVE_BG_MODEL_INFO_S;

static HI_BOOL s_bStopSignal = HI_FALSE;
static pthread_t s_hIveThread = 0;
static SAMPLE_IVE_BG_MODEL_INFO_S s_stBgModelInfo;
static SAMPLE_VI_CONFIG_S s_stViConfig = {0};
static SAMPLE_IVE_SWITCH_S s_stBgModelSwitch = {HI_FALSE,HI_FALSE};

static HI_VOID SAMPLE_IVE_BgModel_Uninit(SAMPLE_IVE_BG_MODEL_S* pstBgModel)
{
    IVE_MMZ_FREE(pstBgModel->stCurImg.au64PhyAddr[0], pstBgModel->stCurImg.au64VirAddr[0]);
    IVE_MMZ_FREE(pstBgModel->stFgFlag.au64PhyAddr[0], pstBgModel->stFgFlag.au64VirAddr[0]);
    IVE_MMZ_FREE(pstBgModel->stImg1.au64PhyAddr[0], pstBgModel->stImg1.au64VirAddr[0]);
    IVE_MMZ_FREE(pstBgModel->stImg2.au64PhyAddr[0], pstBgModel->stImg2.au64VirAddr[0]);
    IVE_MMZ_FREE(pstBgModel->stBgDiffFg.au64PhyAddr[0], pstBgModel->stBgDiffFg.au64VirAddr[0]);
    IVE_MMZ_FREE(pstBgModel->stFrmDiffFg.au64PhyAddr[0], pstBgModel->stFrmDiffFg.au64VirAddr[0]);
    IVE_MMZ_FREE(pstBgModel->stBgImg.au64PhyAddr[0], pstBgModel->stBgImg.au64VirAddr[0]);
    IVE_MMZ_FREE(pstBgModel->stChgStaImg.au64PhyAddr[0], pstBgModel->stChgStaImg.au64VirAddr[0]);
    IVE_MMZ_FREE(pstBgModel->stChgStaFg.au64PhyAddr[0], pstBgModel->stChgStaFg.au64VirAddr[0]);
    IVE_MMZ_FREE(pstBgModel->stChgStaLife.au64PhyAddr[0], pstBgModel->stChgStaLife.au64VirAddr[0]);
    IVE_MMZ_FREE(pstBgModel->stBgModel.u64PhyAddr, pstBgModel->stBgModel.u64VirAddr);
    IVE_MMZ_FREE(pstBgModel->stStatData.u64PhyAddr, pstBgModel->stStatData.u64VirAddr);
    IVE_MMZ_FREE(pstBgModel->stBlob.u64PhyAddr, pstBgModel->stBlob.u64VirAddr);
}

static HI_S32 SAMPLE_IVE_BgModel_Init(SAMPLE_IVE_BG_MODEL_S* pstBgModel, \
    HI_U32 u32Width, HI_U32 u32Height)
{
    HI_S32 s32Ret = HI_SUCCESS;
    HI_U32 u32Size = 0;
    HI_S8 as8Mask[25] = {1, 2, 3, 2, 1,
                         2, 5, 6, 5, 2,
                         3, 6, 8, 6, 3,
                         2, 5, 6, 5, 2,
                         1, 2, 3, 2, 1
                        };
    memset(pstBgModel, 0, sizeof(SAMPLE_IVE_BG_MODEL_S));

    s32Ret = SAMPLE_COMM_IVE_CreateImage(&pstBgModel->stCurImg, IVE_IMAGE_TYPE_U8C1, \
                    u32Width, u32Height);
    SAMPLE_CHECK_EXPR_GOTO(HI_SUCCESS != s32Ret, BGMODEL_INIT_FAIL,
        "Error(%#x),Create curImg image failed!\n", s32Ret);

    s32Ret = SAMPLE_COMM_IVE_CreateImage(&pstBgModel->stFgFlag, IVE_IMAGE_TYPE_U8C1,\
                    u32Width, u32Height);
    SAMPLE_CHECK_EXPR_GOTO(HI_SUCCESS != s32Ret, BGMODEL_INIT_FAIL,
        "Error(%#x),Create fgFlag image failed!\n", s32Ret);

    memset((HI_VOID *)(HI_UL)pstBgModel->stFgFlag.au64VirAddr[0], 0, \
        pstBgModel->stFgFlag.au32Stride[0] * pstBgModel->stFgFlag.u32Height);
    s32Ret = SAMPLE_COMM_IVE_CreateImage(&pstBgModel->stBgDiffFg, IVE_IMAGE_TYPE_S8C1,\
                    u32Width, u32Height);
    SAMPLE_CHECK_EXPR_GOTO(HI_SUCCESS != s32Ret, BGMODEL_INIT_FAIL,
        "Error(%#x),Create bgDifffg image failed!\n", s32Ret);

    memset((HI_VOID *)(HI_UL)pstBgModel->stBgDiffFg.au64VirAddr[0], 0, \
        pstBgModel->stBgDiffFg.au32Stride[0] * pstBgModel->stBgDiffFg.u32Height);
    s32Ret = SAMPLE_COMM_IVE_CreateImage(&pstBgModel->stFrmDiffFg, IVE_IMAGE_TYPE_S8C1,\
                    u32Width, u32Height);
    SAMPLE_CHECK_EXPR_GOTO(HI_SUCCESS != s32Ret, BGMODEL_INIT_FAIL,
        "Error(%#x),Create frmDifffg image failed!\n", s32Ret);
    memset((HI_VOID *)(HI_UL)pstBgModel->stFrmDiffFg.au64VirAddr[0], 0, \
        pstBgModel->stFrmDiffFg.au32Stride[0] * pstBgModel->stFrmDiffFg.u32Height);

    pstBgModel->stBgModel.u32Width = u32Width * sizeof(IVE_BG_MODEL_PIX_S);
    pstBgModel->stBgModel.u32Height = u32Height;
    pstBgModel->stBgModel.u32Stride = SAMPLE_COMM_IVE_CalcStride(pstBgModel->stBgModel.u32Width, IVE_ALIGN);
    u32Size = pstBgModel->stBgModel.u32Stride * pstBgModel->stBgModel.u32Height;
    s32Ret = HI_MPI_SYS_MmzAlloc(&pstBgModel->stBgModel.u64PhyAddr,
                                 (HI_VOID**)&pstBgModel->stBgModel.u64VirAddr, \
                                 NULL, NULL, u32Size);
    SAMPLE_CHECK_EXPR_GOTO(HI_SUCCESS != s32Ret, BGMODEL_INIT_FAIL,
        "Error(%#x),Create bgModel failed!\n", s32Ret);
    memset((HI_VOID *)(HI_UL)pstBgModel->stBgModel.u64VirAddr, 0, u32Size);

    s32Ret = SAMPLE_COMM_IVE_CreateImage(&pstBgModel->stBgImg, IVE_IMAGE_TYPE_U8C1,\
                    u32Width, u32Height);
    SAMPLE_CHECK_EXPR_GOTO(HI_SUCCESS != s32Ret, BGMODEL_INIT_FAIL,
        "Error(%#x),Create bgImg image failed!\n", s32Ret);
    memset((HI_VOID *)(HI_UL)pstBgModel->stBgImg.au64VirAddr[0], 0, \
        pstBgModel->stBgImg.au32Stride[0] * pstBgModel->stBgImg.u32Height);

    s32Ret = SAMPLE_COMM_IVE_CreateImage(&pstBgModel->stChgStaImg, IVE_IMAGE_TYPE_U8C1,\
                    u32Width, u32Height);
    SAMPLE_CHECK_EXPR_GOTO(HI_SUCCESS != s32Ret, BGMODEL_INIT_FAIL,
        "Error(%#x),Create chgStaImg image failed!\n", s32Ret);
    memset((HI_VOID *)(HI_UL)pstBgModel->stChgStaImg.au64VirAddr[0], 0, \
        pstBgModel->stChgStaImg.au32Stride[0] * pstBgModel->stChgStaImg.u32Height);

    s32Ret = SAMPLE_COMM_IVE_CreateImage(&pstBgModel->stChgStaFg, IVE_IMAGE_TYPE_S8C1,\
                    u32Width, u32Height);
    SAMPLE_CHECK_EXPR_GOTO(HI_SUCCESS != s32Ret, BGMODEL_INIT_FAIL,
        "Error(%#x),Create chgStaFg image failed!\n", s32Ret);
    memset((HI_VOID *)(HI_UL)pstBgModel->stChgStaFg.au64VirAddr[0], 0, \
        pstBgModel->stChgStaFg.au32Stride[0] * pstBgModel->stChgStaFg.u32Height);

    s32Ret = SAMPLE_COMM_IVE_CreateImage(&pstBgModel->stChgStaLife, IVE_IMAGE_TYPE_U16C1,\
                    u32Width, u32Height);
    SAMPLE_CHECK_EXPR_GOTO(HI_SUCCESS != s32Ret, BGMODEL_INIT_FAIL,
        "Error(%#x),Create chgStaLife image failed!\n", s32Ret);
    memset((HI_VOID *)(HI_UL)pstBgModel->stChgStaLife.au64VirAddr[0], 0,\
        pstBgModel->stChgStaLife.au32Stride[0] * pstBgModel->stChgStaLife.u32Height * sizeof(HI_U16));

    u32Size = sizeof(IVE_BG_STAT_DATA_S);
    s32Ret = SAMPLE_COMM_IVE_CreateMemInfo(&pstBgModel->stStatData, u32Size);
    SAMPLE_CHECK_EXPR_GOTO(HI_SUCCESS != s32Ret, BGMODEL_INIT_FAIL,
        "Error(%#x),Create statData image failed!\n", s32Ret);

    s32Ret = SAMPLE_COMM_IVE_CreateImage(&pstBgModel->stImg1, IVE_IMAGE_TYPE_U8C1,\
        u32Width, u32Height);
    SAMPLE_CHECK_EXPR_GOTO(HI_SUCCESS != s32Ret, BGMODEL_INIT_FAIL,
        "Error(%#x),Create img1 image failed!\n", s32Ret);

    s32Ret = SAMPLE_COMM_IVE_CreateImage(&pstBgModel->stImg2, IVE_IMAGE_TYPE_U8C1,\
        u32Width, u32Height);
    SAMPLE_CHECK_EXPR_GOTO(HI_SUCCESS != s32Ret, BGMODEL_INIT_FAIL,
        "Error(%#x),Create img2 image failed!\n", s32Ret);

    u32Size = sizeof(IVE_CCBLOB_S);
    s32Ret = SAMPLE_COMM_IVE_CreateMemInfo(&pstBgModel->stBlob, u32Size);
    SAMPLE_CHECK_EXPR_GOTO(HI_SUCCESS != s32Ret, BGMODEL_INIT_FAIL,
        "Error(%#x),Create blob mem info failed!\n", s32Ret);


    pstBgModel->stMatchBgModelCtrl.u32CurFrmNum = 0;
    pstBgModel->stMatchBgModelCtrl.u32PreFrmNum = 0;
    pstBgModel->stMatchBgModelCtrl.u16TimeThr = 20;
    pstBgModel->stMatchBgModelCtrl.u8DiffThrCrlCoef = 0;
    pstBgModel->stMatchBgModelCtrl.u8DiffMaxThr = 10;
    pstBgModel->stMatchBgModelCtrl.u8DiffMinThr = 10;
    pstBgModel->stMatchBgModelCtrl.u8DiffThrInc = 0;
    pstBgModel->stMatchBgModelCtrl.u8FastLearnRate = 4;
    pstBgModel->stMatchBgModelCtrl.u8DetChgRegion = 0;

    pstBgModel->stUpdateBgModelCtrl.u32CurFrmNum = 0;
    pstBgModel->stUpdateBgModelCtrl.u32PreChkTime = 0;
    pstBgModel->stUpdateBgModelCtrl.u32FrmChkPeriod = 50;
    pstBgModel->stUpdateBgModelCtrl.u32InitMinTime = 25;
    pstBgModel->stUpdateBgModelCtrl.u32StyBgMinBlendTime = 100;
    pstBgModel->stUpdateBgModelCtrl.u32StyBgMaxBlendTime = 1500;
    pstBgModel->stUpdateBgModelCtrl.u32DynBgMinBlendTime = 0;
    pstBgModel->stUpdateBgModelCtrl.u32StaticDetMinTime = 80;
    pstBgModel->stUpdateBgModelCtrl.u16FgMaxFadeTime = 15;
    pstBgModel->stUpdateBgModelCtrl.u16BgMaxFadeTime = 60;
    pstBgModel->stUpdateBgModelCtrl.u8StyBgAccTimeRateThr = 80;
    pstBgModel->stUpdateBgModelCtrl.u8ChgBgAccTimeRateThr = 60;
    pstBgModel->stUpdateBgModelCtrl.u8DynBgAccTimeThr = 0;
    pstBgModel->stUpdateBgModelCtrl.u8DynBgDepth = 3;
    pstBgModel->stUpdateBgModelCtrl.u8BgEffStaRateThr = 90;
    pstBgModel->stUpdateBgModelCtrl.u8AcceBgLearn = 0;
    pstBgModel->stUpdateBgModelCtrl.u8DetChgRegion = 0;

    memcpy(pstBgModel->stFltCtrl.as8Mask, as8Mask, 25);
    pstBgModel->stFltCtrl.u8Norm = 7;
    memset(pstBgModel->stDilateCtrl.au8Mask, 255, 25);
    memset(pstBgModel->stErodeCtrl.au8Mask, 255, 25);
    pstBgModel->stCclCtrl.u16InitAreaThr = 16;
    pstBgModel->stCclCtrl.u16Step = 4;
    pstBgModel->stCclCtrl.enMode  = IVE_CCL_MODE_8C;
    pstBgModel->stRegion.u16Num = 0;

BGMODEL_INIT_FAIL:

    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_IVE_BgModel_Uninit(pstBgModel);
    }
    return s32Ret;
}

static HI_S32 SAMPLE_IVE_BgModelProc(SAMPLE_IVE_BG_MODEL_INFO_S* pstBgModelInfo,
    VIDEO_FRAME_INFO_S* pstExtFrmInfo,HI_U32 u32BaseWidth,HI_U32 u32BaseHeight)
{
    HI_S32 s32Ret;
    SAMPLE_IVE_BG_MODEL_S* pstBgModel;
    IVE_CCBLOB_S* pstBlob;
    IVE_HANDLE IveHandle;
    HI_BOOL bFinish = HI_FALSE;
    HI_BOOL bBlock = HI_TRUE;
    HI_BOOL bInstant = HI_TRUE;
    IVE_IMAGE_S stFg;
    IVE_THRESH_CTRL_S stThrCtrl = {IVE_THRESH_MODE_BINARY, 0, 0, 0, 0, 255};

    HI_U32 u32FrmUpdPeriod = 15;
    HI_U32 u32FrmChkPeriod = 50;

    pstBgModel = &(pstBgModelInfo->stBgModel);

    stFg.enType = IVE_IMAGE_TYPE_U8C1;
    stFg.au64VirAddr[0] = pstBgModel->stBgDiffFg.au64VirAddr[0];
    stFg.au64PhyAddr[0] = pstBgModel->stBgDiffFg.au64PhyAddr[0];
    stFg.u32Width = pstBgModel->stBgDiffFg.u32Width;
    stFg.u32Height = pstBgModel->stBgDiffFg.u32Height;
    stFg.au32Stride[0] = pstBgModel->stBgDiffFg.au32Stride[0];
    pstBlob = SAMPLE_COMM_IVE_CONVERT_64BIT_ADDR(IVE_CCBLOB_S,pstBgModel->stBlob.u64VirAddr);

    pstBgModel->u32FrameCnt++;

    bInstant = HI_FALSE;
    s32Ret = SAMPLE_COMM_IVE_DmaImage(pstExtFrmInfo,&pstBgModel->stCurImg,bInstant);
    SAMPLE_CHECK_EXPR_RET(HI_SUCCESS != s32Ret,s32Ret,"Error(%#x),SAMPLE_COMM_IVE_DmaImage failed!\n",s32Ret);

    s32Ret = HI_MPI_IVE_Filter(&IveHandle, &pstBgModel->stCurImg, &pstBgModel->stImg1,
                    &pstBgModel->stFltCtrl, bInstant);
    SAMPLE_CHECK_EXPR_RET(HI_SUCCESS != s32Ret,s32Ret,"Error(%#x),HI_MPI_IVE_Filter failed!\n",s32Ret);

    pstBgModel->stMatchBgModelCtrl.u32PreFrmNum = pstBgModel->stMatchBgModelCtrl.u32CurFrmNum;
    pstBgModel->stMatchBgModelCtrl.u32CurFrmNum = pstBgModel->u32FrameCnt;

    s32Ret = HI_MPI_IVE_MatchBgModel(&IveHandle, &pstBgModel->stImg1, &pstBgModel->stBgModel,
                    &pstBgModel->stFgFlag, &pstBgModel->stBgDiffFg, &pstBgModel->stFrmDiffFg,
                    &pstBgModel->stStatData, &pstBgModel->stMatchBgModelCtrl, bInstant);
    SAMPLE_CHECK_EXPR_RET(HI_SUCCESS != s32Ret,s32Ret,"Error(%#x),HI_MPI_IVE_MatchBgModel failed!\n",s32Ret);

    if ( (0 == pstBgModel->u32UpdCnt) || (pstBgModel->u32FrameCnt >= pstBgModel->u32PreUpdTime + u32FrmUpdPeriod) )
    {
        pstBgModel->u32UpdCnt++;
        pstBgModel->u32PreUpdTime = pstBgModel->u32FrameCnt;
        pstBgModel->stUpdateBgModelCtrl.u32CurFrmNum = pstBgModel->u32FrameCnt;
        pstBgModel->stUpdateBgModelCtrl.u32PreChkTime = pstBgModel->u32PreChkTime;
        pstBgModel->stUpdateBgModelCtrl.u32FrmChkPeriod = 0;
        if ( pstBgModel->u32FrameCnt >= pstBgModel->u32PreChkTime + u32FrmChkPeriod )
        {
            pstBgModel->stUpdateBgModelCtrl.u32FrmChkPeriod = u32FrmChkPeriod;
            pstBgModel->u32PreChkTime = pstBgModel->u32FrameCnt;
        }

        s32Ret = HI_MPI_IVE_UpdateBgModel(&IveHandle, &pstBgModel->stBgModel,
                        &pstBgModel->stFgFlag,&pstBgModel->stBgImg,
                        &pstBgModel->stChgStaImg, &pstBgModel->stChgStaFg,
                        &pstBgModel->stChgStaLife,&pstBgModel->stStatData,
                        &pstBgModel->stUpdateBgModelCtrl, bInstant);
        SAMPLE_CHECK_EXPR_RET(HI_SUCCESS != s32Ret,s32Ret,"Error(%#x),HI_MPI_IVE_UpdateBgModel failed!\n",s32Ret);
    }

    s32Ret = HI_MPI_IVE_Thresh(&IveHandle, &stFg, &stFg, &stThrCtrl, bInstant);
    SAMPLE_CHECK_EXPR_RET(HI_SUCCESS != s32Ret,s32Ret,"Error(%#x),HI_MPI_IVE_Thresh failed!\n",s32Ret);

    s32Ret = HI_MPI_IVE_Dilate(&IveHandle, &stFg, &pstBgModel->stImg2,
                    &pstBgModel->stDilateCtrl, bInstant);
    SAMPLE_CHECK_EXPR_RET(HI_SUCCESS != s32Ret,s32Ret,"Error(%#x),HI_MPI_IVE_Dilate failed!\n",s32Ret);

    s32Ret = HI_MPI_IVE_Erode(&IveHandle, &pstBgModel->stImg2, &pstBgModel->stImg1,
                    &pstBgModel->stErodeCtrl, bInstant);
    SAMPLE_CHECK_EXPR_RET(HI_SUCCESS != s32Ret,s32Ret,"Error(%#x),HI_MPI_IVE_Erode failed!\n",s32Ret);

    bInstant = HI_TRUE;
    s32Ret = HI_MPI_IVE_CCL(&IveHandle, &pstBgModel->stImg1, &pstBgModel->stBlob, &pstBgModel->stCclCtrl, bInstant);
    SAMPLE_CHECK_EXPR_RET(HI_SUCCESS != s32Ret,s32Ret,"Error(%#x),HI_MPI_IVE_CCL failed!\n",s32Ret);

    s32Ret = HI_MPI_IVE_Query(IveHandle, &bFinish, bBlock);
    while (HI_ERR_IVE_QUERY_TIMEOUT == s32Ret)
    {
        usleep(100);
        s32Ret = HI_MPI_IVE_Query(IveHandle, &bFinish, bBlock);
    }
    SAMPLE_CHECK_EXPR_RET(HI_SUCCESS != s32Ret,s32Ret,"Error(%#x),HI_MPI_IVE_Query failed!\n",s32Ret);

    SAMPLE_COMM_IVE_BlobToRect(pstBlob, &(pstBgModel->stRegion),
        IVE_RECT_NUM, 8, pstExtFrmInfo->stVFrame.u32Width, pstExtFrmInfo->stVFrame.u32Height,
        u32BaseWidth,u32BaseHeight);

    return HI_SUCCESS;
}

static HI_VOID* SAMPLE_IVE_ViToVo(HI_VOID* pArgs)
{
    HI_S32 s32Ret;
    SAMPLE_IVE_BG_MODEL_INFO_S* pstBgModelInfo;
    SAMPLE_IVE_BG_MODEL_S* pstBgModel;
    VIDEO_FRAME_INFO_S stBaseFrmInfo;
    VIDEO_FRAME_INFO_S stExtFrmInfo;
    HI_S32 s32MilliSec = 20000;
    VO_CHN voChn = 0;
    VO_LAYER voLayer = 0;
    VENC_CHN vencChn = 0;
    HI_BOOL bEncode;
    HI_BOOL bVo;
    HI_S32 s32VpssGrp = 0;
    HI_S32 as32VpssChn[] = {VPSS_CHN0, VPSS_CHN1};

    pstBgModelInfo = (SAMPLE_IVE_BG_MODEL_INFO_S*)pArgs;
    pstBgModel = &(pstBgModelInfo->stBgModel);
    bEncode = pstBgModelInfo->bEncode;
    bVo = pstBgModelInfo->bVo;

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

        s32Ret = SAMPLE_IVE_BgModelProc(pstBgModelInfo, &stExtFrmInfo,stBaseFrmInfo.stVFrame.u32Width,
            stBaseFrmInfo.stVFrame.u32Height);
        SAMPLE_CHECK_EXPR_GOTO(HI_SUCCESS!=s32Ret, BASE_RELEASE,
            "Error(%#x),SAMPLE_IVE_BgModelProc failed!\n", s32Ret);

        //Draw rect
        s32Ret = SAMPLE_COMM_VGS_FillRect(&stBaseFrmInfo, &pstBgModel->stRegion, 0x0000FF00);
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

    return NULL;
}

/******************************************************************************
* function : show BgModel sample
******************************************************************************/
HI_VOID SAMPLE_IVE_BgModel(HI_CHAR chEncode, HI_CHAR chVo)
{
    SIZE_S stSize;
    PIC_SIZE_E enSize = PIC_CIF;
    HI_BOOL bEncode = '1' == chEncode ? HI_TRUE : HI_FALSE;
    HI_BOOL bVo = '1' == chVo ? HI_TRUE : HI_FALSE;
    HI_S32 s32Ret = HI_SUCCESS;
    HI_CHAR acThreadName[16] = {0};

    memset(&s_stBgModelInfo,0,sizeof(s_stBgModelInfo));
    /******************************************
      step 1: start vi vpss venc vo
     ******************************************/
    s_stBgModelSwitch.bVenc = bEncode;
    s_stBgModelSwitch.bVo   = bVo;
    s32Ret = SAMPLE_COMM_IVE_StartViVpssVencVo(&s_stViConfig,&s_stBgModelSwitch,&enSize);
    SAMPLE_CHECK_EXPR_GOTO(HI_SUCCESS != s32Ret, END_BG_MODEL_0,
        "Error(%#x),SAMPLE_COMM_SYS_GetPicSize failed!\n", s32Ret);

    s32Ret = SAMPLE_COMM_SYS_GetPicSize(enSize, &stSize);
    SAMPLE_CHECK_EXPR_GOTO(HI_SUCCESS != s32Ret, END_BG_MODEL_0,
        "Error(%#x),SAMPLE_COMM_SYS_GetPicSize failed!\n", s32Ret);
    /******************************************
      step 2: Init BgModel
     ******************************************/
    s32Ret = SAMPLE_IVE_BgModel_Init(&(s_stBgModelInfo.stBgModel), stSize.u32Width, stSize.u32Height);
    SAMPLE_CHECK_EXPR_GOTO(HI_SUCCESS != s32Ret, END_BG_MODEL_0,
        "Error(%#x),SAMPLE_IVE_BgModel_Init failed!\n", s32Ret);

    s_stBgModelInfo.bEncode = bEncode;
    s_stBgModelInfo.bVo = bVo;
    s_bStopSignal = HI_FALSE;
    /******************************************
      step 3: Create work thread
     ******************************************/
    snprintf(acThreadName, 16, "IVE_ViToVo");
    prctl(PR_SET_NAME, (unsigned long)acThreadName, 0,0,0);
    pthread_create(&s_hIveThread, 0, SAMPLE_IVE_ViToVo, (HI_VOID*)&s_stBgModelInfo);

    SAMPLE_PAUSE();

    s_bStopSignal = HI_TRUE;
    pthread_join(s_hIveThread, HI_NULL);
    s_hIveThread = 0;
    SAMPLE_IVE_BgModel_Uninit(&(s_stBgModelInfo.stBgModel));
    memset(&s_stBgModelInfo,0,sizeof(s_stBgModelInfo));

END_BG_MODEL_0:    //system exit
    SAMPLE_COMM_IVE_StopViVpssVencVo(&s_stViConfig,&s_stBgModelSwitch);

    return ;
}

/******************************************************************************
* function : BgModel sample signal handle
******************************************************************************/
HI_VOID SAMPLE_IVE_BgModel_HandleSig(HI_VOID)
{
    s_bStopSignal = HI_TRUE;
    if (0 != s_hIveThread)
    {
        pthread_join(s_hIveThread, HI_NULL);
        s_hIveThread = 0;
    }
    SAMPLE_IVE_BgModel_Uninit(&(s_stBgModelInfo.stBgModel));
    memset(&s_stBgModelInfo,0,sizeof(s_stBgModelInfo));

    SAMPLE_COMM_IVE_StopViVpssVencVo(&s_stViConfig,&s_stBgModelSwitch);
}

