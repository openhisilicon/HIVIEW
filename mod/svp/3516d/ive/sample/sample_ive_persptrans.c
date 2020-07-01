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

#include "sample_comm_ive.h"

typedef struct hiSAMPLE_IVE_PERSP_TRANS_S
{
    IVE_SRC_IMAGE_S stSrc;
    IVE_RECT_U32_S astRoi[64];
    HI_U16 u16RoiNum;
    IVE_DST_IMAGE_S astDst[64];
    IVE_SRC_MEM_INFO_S astPointPair[64];
    IVE_PERSP_TRANS_CTRL_S stPerspTransCtrl;

    FILE *pFpSrc;
    FILE *pFpDst;
} SAMPLE_IVE_PERSP_TRANS_S;

static SAMPLE_IVE_PERSP_TRANS_S     s_stPerspTrans;

static HI_VOID SAMPLE_IVE_PerspTrans_Uninit(SAMPLE_IVE_PERSP_TRANS_S *pstPerspTrans)
{
    HI_U16 i;

    IVE_MMZ_FREE(pstPerspTrans->stSrc.au64PhyAddr[0], pstPerspTrans->stSrc.au64VirAddr[0]);

    for (i = 0; i < pstPerspTrans->u16RoiNum; i++)
    {
        IVE_MMZ_FREE(pstPerspTrans->astDst[i].au64PhyAddr[0], pstPerspTrans->astDst[i].au64VirAddr[0]);
        IVE_MMZ_FREE(pstPerspTrans->astPointPair[i].u64PhyAddr, pstPerspTrans->astPointPair[i].u64VirAddr);
    }

    IVE_CLOSE_FILE(pstPerspTrans->pFpSrc);
    IVE_CLOSE_FILE(pstPerspTrans->pFpDst);
}

static HI_S32 SAMPLE_IVE_PerspTrans_Init(SAMPLE_IVE_PERSP_TRANS_S *pstPerspTrans,
    HI_U32 u32SrcWidth,HI_U32 u32SrcHeight, HI_U32 u32DstWidth,HI_U32 u32DstHeight,
    IVE_RECT_U32_S astRoi[], HI_U16 u16RoiNum,  HI_U16 u16MaxPointPairNum, HI_CHAR *pchSrcFileName,
    HI_CHAR *pchDstFileName)
{
    HI_S32 s32Ret = HI_SUCCESS;
    HI_U32 u32Size;
    HI_U16 i, j;
    HI_U16 au16LandMark[] = {107, 109, 30, 52,
                             149, 117, 66, 52,
                             123, 135, 48, 72,
                             99,  157, 34, 92,
                             144, 157, 63, 92
                            };
    IVE_PERSP_TRANS_POINT_PAIR_S *pstTmp;

    s32Ret = SAMPLE_COMM_IVE_CreateImage(&(pstPerspTrans->stSrc), IVE_IMAGE_TYPE_YUV420SP, u32SrcWidth, u32SrcHeight);
        SAMPLE_CHECK_EXPR_GOTO(HI_SUCCESS != s32Ret, PERSP_TRANS_INIT_FAIL,
            "Error(%#x),Create src image failed!\n", s32Ret);

    memcpy_s(pstPerspTrans->astRoi, sizeof(IVE_RECT_U32_S) * u16RoiNum, astRoi, sizeof(IVE_RECT_U32_S) * u16RoiNum);

    for (i = 0; i < u16RoiNum; i++)
    {
        s32Ret = SAMPLE_COMM_IVE_CreateImage(&(pstPerspTrans->astDst[i]), IVE_IMAGE_TYPE_YUV420SP, u32DstWidth, u32DstHeight);
            SAMPLE_CHECK_EXPR_GOTO(HI_SUCCESS != s32Ret, PERSP_TRANS_INIT_FAIL,
                "Error(%#x),Create src image failed!\n", s32Ret);
    }

    u32Size = sizeof(IVE_PERSP_TRANS_POINT_PAIR_S) * u16MaxPointPairNum;
    for (i = 0; i < u16RoiNum; i++)
    {
        s32Ret = SAMPLE_COMM_IVE_CreateMemInfo(&(pstPerspTrans->astPointPair[i]), u32Size);
            SAMPLE_CHECK_EXPR_GOTO(HI_SUCCESS != s32Ret, PERSP_TRANS_INIT_FAIL,
                "Error(%#x),Create src image failed!\n", s32Ret);
    }

    pstPerspTrans->stPerspTransCtrl.enAlgMode = IVE_PERSP_TRANS_ALG_MODE_AFFINE;
    pstPerspTrans->stPerspTransCtrl.enCscMode = IVE_PERSP_TRANS_CSC_MODE_NONE;
    pstPerspTrans->stPerspTransCtrl.u16RoiNum = u16RoiNum;
    pstPerspTrans->stPerspTransCtrl.u16PointPairNum = 5;
    pstPerspTrans->u16RoiNum = u16RoiNum;

    for (i = 0; i < u16RoiNum; i++)
    {
        pstTmp = (IVE_PERSP_TRANS_POINT_PAIR_S *)(HI_UL)pstPerspTrans->astPointPair[i].u64VirAddr;
        for (j = 0; j < pstPerspTrans->stPerspTransCtrl.u16PointPairNum; j++)
        {
            pstTmp->stSrcPoint.u14q2X = au16LandMark[j * 4] << 2;
            pstTmp->stSrcPoint.u14q2Y = au16LandMark[j * 4 + 1] << 2;
            pstTmp->stDstPoint.u14q2X = au16LandMark[j * 4 + 2] << 2;
            pstTmp->stDstPoint.u14q2Y = au16LandMark[j * 4 + 3] << 2;

            pstTmp++;
        }
    }

    s32Ret = HI_FAILURE;
    pstPerspTrans->pFpSrc = fopen(pchSrcFileName, "rb");
    SAMPLE_CHECK_EXPR_GOTO(NULL == pstPerspTrans->pFpSrc, PERSP_TRANS_INIT_FAIL,
        "Error,Open file %s failed!\n", pchSrcFileName);

    pstPerspTrans->pFpDst = fopen(pchDstFileName, "wb");
    SAMPLE_CHECK_EXPR_GOTO(NULL == pstPerspTrans->pFpDst, PERSP_TRANS_INIT_FAIL,
        "Error,Open file %s failed!\n", pchDstFileName);
    s32Ret = HI_SUCCESS;

    return s32Ret;

PERSP_TRANS_INIT_FAIL:
    SAMPLE_IVE_PerspTrans_Uninit(pstPerspTrans);

    return s32Ret;
}

static HI_S32 SAMPLE_IVE_PerspTransProc(SAMPLE_IVE_PERSP_TRANS_S *pstPerspTrans)
{
    HI_S32 s32Ret = HI_FAILURE;
    IVE_HANDLE IveHandle;
    HI_BOOL bFinish = HI_FALSE;
    HI_BOOL bBlock = HI_TRUE;
    HI_BOOL bInstant = HI_TRUE;

    s32Ret = SAMPLE_COMM_IVE_ReadFile(&(pstPerspTrans->stSrc), pstPerspTrans->pFpSrc);
    SAMPLE_CHECK_EXPR_RET(HI_SUCCESS != s32Ret,s32Ret,"Error(%#x),Read src file failed!\n",s32Ret);

    s32Ret = HI_MPI_IVE_PerspTrans(&IveHandle, &pstPerspTrans->stSrc, pstPerspTrans->astRoi, pstPerspTrans->astPointPair,
	    pstPerspTrans->astDst, &pstPerspTrans->stPerspTransCtrl, bInstant);

    s32Ret = HI_MPI_IVE_Query(IveHandle, &bFinish, bBlock);
    while (HI_ERR_IVE_QUERY_TIMEOUT == s32Ret)
    {
        usleep(100);
        s32Ret = HI_MPI_IVE_Query(IveHandle, &bFinish, bBlock);
    }
    SAMPLE_CHECK_EXPR_RET(HI_SUCCESS != s32Ret,s32Ret,"Error(%#x),HI_MPI_IVE_Query failed!\n",s32Ret);

    s32Ret = SAMPLE_COMM_IVE_WriteFile(&pstPerspTrans->astDst[0], pstPerspTrans->pFpDst);
    SAMPLE_CHECK_EXPR_RET(HI_SUCCESS != s32Ret,s32Ret,"Error(%#x),Read src file failed!\n",s32Ret);

    return s32Ret;
}

HI_VOID SAMPLE_IVE_PerspTrans(HI_VOID)
{
    HI_S32                 s32Ret;
    HI_U32                 u32SrcWidth  = 250;
    HI_U32                 u32SrcHeight = 250;
    HI_U32                 u32DstWidth  = 96;
    HI_U32                 u32DstHeight = 112;
    HI_CHAR                *pchSrcFileName = "./data/input/psp/src/Amelia_Vega_250x250_420sp.yuv";
    HI_CHAR                *pchDstFileName = "./data/output/psp/Amelia_Vega_Affine_96x112_420sp.yuv";
    IVE_RECT_U32_S astRoi[2] = {0};
    HI_U16 u16RoiNum = 1;
    HI_U16 u16MaxPointPairNum = 68;

    memset(&s_stPerspTrans,0,sizeof(s_stPerspTrans));
    SAMPLE_COMM_IVE_CheckIveMpiInit();

    astRoi[0].u32X = 0;
    astRoi[0].u32Y = 0;
    astRoi[0].u32Width = 250;
    astRoi[0].u32Height = 250;
    s32Ret = SAMPLE_IVE_PerspTrans_Init(&s_stPerspTrans, u32SrcWidth,u32SrcHeight,
        u32DstWidth,u32DstHeight,astRoi, u16RoiNum, u16MaxPointPairNum, pchSrcFileName, pchDstFileName);
    SAMPLE_CHECK_EXPR_GOTO(HI_SUCCESS != s32Ret, PERSP_TRANS_FAIL,
        "Error(%#x),SAMPLE_IVE_PerspTrans_Init failed!\n", s32Ret);

    s32Ret = SAMPLE_IVE_PerspTransProc(&s_stPerspTrans);
    if (HI_SUCCESS == s32Ret)
    {
        SAMPLE_PRT("Process success!\n");
    }

    SAMPLE_IVE_PerspTrans_Uninit(&s_stPerspTrans);
    memset(&s_stPerspTrans,0,sizeof(s_stPerspTrans));

PERSP_TRANS_FAIL:
    SAMPLE_COMM_IVE_IveMpiExit();
}
/******************************************************************************
* function : PerspTrans sample signal handle
******************************************************************************/
HI_VOID SAMPLE_IVE_PerspTrans_HandleSig(HI_VOID)
{
    SAMPLE_IVE_PerspTrans_Uninit(&s_stPerspTrans);
    memset(&s_stPerspTrans,0,sizeof(s_stPerspTrans));
    SAMPLE_COMM_IVE_IveMpiExit();
}


