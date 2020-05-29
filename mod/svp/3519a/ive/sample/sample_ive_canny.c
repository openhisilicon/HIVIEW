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
#include <math.h>
#include "sample_comm_ive.h"

typedef struct hiSAMPLE_IVE_CANNY_INFO_S
{
    IVE_SRC_IMAGE_S stSrc;
    IVE_DST_IMAGE_S stEdge;
    IVE_DST_IMAGE_S stMag;
    IVE_MEM_INFO_S  stStack;
    IVE_CANNY_HYS_EDGE_CTRL_S  stCannyHysEdgeCtrl;
    IVE_MAG_AND_ANG_CTRL_S stMagAndAngCtrl;
    IVE_THRESH_U16_CTRL_S stThrU16Ctrl;
    FILE* pFpSrc;
    FILE* pFpDst;
} SAMPLE_IVE_CANNY_INFO_S;

static SAMPLE_IVE_CANNY_INFO_S s_stCannyInfo;

/******************************************************************************
* function : Canny uninit
******************************************************************************/
static HI_VOID SAMPLE_IVE_Canny_Uninit(SAMPLE_IVE_CANNY_INFO_S* pstCannyInfo)
{
    IVE_MMZ_FREE(pstCannyInfo->stSrc.au64PhyAddr[0], pstCannyInfo->stSrc.au64VirAddr[0]);
    IVE_MMZ_FREE(pstCannyInfo->stEdge.au64PhyAddr[0], pstCannyInfo->stEdge.au64VirAddr[0]);
    IVE_MMZ_FREE(pstCannyInfo->stMag.au64PhyAddr[0], pstCannyInfo->stMag.au64VirAddr[0]);
    IVE_MMZ_FREE(pstCannyInfo->stStack.u64PhyAddr, pstCannyInfo->stStack.u64VirAddr);
    IVE_MMZ_FREE(pstCannyInfo->stCannyHysEdgeCtrl.stMem.u64PhyAddr, \
                 pstCannyInfo->stCannyHysEdgeCtrl.stMem.u64VirAddr);

    IVE_CLOSE_FILE(pstCannyInfo->pFpSrc);
    IVE_CLOSE_FILE(pstCannyInfo->pFpDst);
}
/******************************************************************************
* function : Canny init
******************************************************************************/
static HI_S32 SAMPLE_IVE_Canny_Init(SAMPLE_IVE_CANNY_INFO_S* pstCannyInfo,
                                    HI_CHAR* pchSrcFileName, HI_CHAR* pchDstFileName, HI_U32 u32Width, HI_U32 u32Height)
{
    HI_S32 s32Ret = HI_SUCCESS;
    HI_U32 u32Size = 0;
    HI_S8 as8Mask[25] = {0, 0, 0, 0, 0,
                         0, -1, 0, 1, 0,
                         0, -2, 0, 2, 0,
                         0, -1, 0, 1, 0,
                         0, 0, 0, 0, 0
                        };

    memset(pstCannyInfo, 0, sizeof(SAMPLE_IVE_CANNY_INFO_S));
    memcpy(pstCannyInfo->stCannyHysEdgeCtrl.as8Mask, as8Mask, 25);
    memcpy(pstCannyInfo->stMagAndAngCtrl.as8Mask, as8Mask, 25);
    pstCannyInfo->stCannyHysEdgeCtrl.u16HighThr = 150;
    pstCannyInfo->stCannyHysEdgeCtrl.u16LowThr = 50;
    pstCannyInfo->stMagAndAngCtrl.enOutCtrl = IVE_MAG_AND_ANG_OUT_CTRL_MAG;
    pstCannyInfo->stMagAndAngCtrl.u16Thr = 0;
    pstCannyInfo->stThrU16Ctrl.enMode = IVE_THRESH_U16_MODE_U16_TO_U8_MIN_MID_MAX;
    pstCannyInfo->stThrU16Ctrl.u16HighThr = 100;
    pstCannyInfo->stThrU16Ctrl.u16LowThr = 100;
    pstCannyInfo->stThrU16Ctrl.u8MaxVal = 255;
    pstCannyInfo->stThrU16Ctrl.u8MidVal = 0;
    pstCannyInfo->stThrU16Ctrl.u8MinVal = 0;

    s32Ret = SAMPLE_COMM_IVE_CreateImage(&pstCannyInfo->stSrc, IVE_IMAGE_TYPE_U8C1, u32Width, u32Height);
    SAMPLE_CHECK_EXPR_GOTO(HI_SUCCESS != s32Ret, CANNY_INIT_FAIL,
        "Error(%#x),Create Src Image failed!\n", s32Ret);

    s32Ret = SAMPLE_COMM_IVE_CreateImage(&pstCannyInfo->stEdge, IVE_IMAGE_TYPE_U8C1, u32Width, u32Height);
    SAMPLE_CHECK_EXPR_GOTO(HI_SUCCESS != s32Ret, CANNY_INIT_FAIL,
        "Error(%#x),Create edge Image failed!\n", s32Ret);

    s32Ret = SAMPLE_COMM_IVE_CreateImage(&pstCannyInfo->stMag, IVE_IMAGE_TYPE_U16C1, u32Width, u32Height);
    SAMPLE_CHECK_EXPR_GOTO(HI_SUCCESS != s32Ret, CANNY_INIT_FAIL,
        "Error(%#x),Create Mag Image failed!\n", s32Ret);

    u32Size = pstCannyInfo->stSrc.au32Stride [0] * pstCannyInfo->stSrc.u32Height * 4 + sizeof(IVE_CANNY_STACK_SIZE_S);
    s32Ret = SAMPLE_COMM_IVE_CreateMemInfo(&pstCannyInfo->stStack, u32Size);
    SAMPLE_CHECK_EXPR_GOTO(HI_SUCCESS != s32Ret, CANNY_INIT_FAIL,
        "Error(%#x),Create Stack Mem info failed!\n", s32Ret);

    u32Size = pstCannyInfo->stSrc.au32Stride [0] * pstCannyInfo->stSrc.u32Height * 3;
    s32Ret = SAMPLE_COMM_IVE_CreateMemInfo(&pstCannyInfo->stCannyHysEdgeCtrl.stMem, u32Size);
    SAMPLE_CHECK_EXPR_GOTO(HI_SUCCESS != s32Ret, CANNY_INIT_FAIL,
        "Error(%#x),Create CannyHysEdgeCtrl.stMem Mem info failed!\n", s32Ret);

    s32Ret = HI_FAILURE;
    pstCannyInfo->pFpSrc = fopen(pchSrcFileName, "rb");
    SAMPLE_CHECK_EXPR_GOTO(HI_NULL == pstCannyInfo->pFpSrc, CANNY_INIT_FAIL,
        "Error,Open file %s failed!\n", pchSrcFileName);

    pstCannyInfo->pFpDst = fopen(pchDstFileName, "wb");
    SAMPLE_CHECK_EXPR_GOTO(HI_NULL == pstCannyInfo->pFpDst, CANNY_INIT_FAIL,
        "Error,Open file %s failed!\n", pchDstFileName);

    s32Ret = HI_SUCCESS;

CANNY_INIT_FAIL:

    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_IVE_Canny_Uninit(pstCannyInfo);
    }
    return s32Ret;
}
/******************************************************************************
* function : show complate canny sample
******************************************************************************/
static HI_S32 SAMPLE_IVE_Complate_Canny(SAMPLE_IVE_CANNY_INFO_S* pstCannyInfo)
{
    HI_S32 s32Ret = HI_SUCCESS;
    HI_BOOL bInstant = HI_TRUE;
    HI_BOOL bBlock = HI_TRUE;
    HI_BOOL bFinish = HI_FALSE;
    IVE_HANDLE IveHandle;

    s32Ret = SAMPLE_COMM_IVE_ReadFile(&(pstCannyInfo->stSrc), pstCannyInfo->pFpSrc);
    SAMPLE_CHECK_EXPR_RET(HI_SUCCESS != s32Ret,s32Ret,"Error(%#x),Read src file failed!\n",s32Ret);

    s32Ret = HI_MPI_IVE_CannyHysEdge(&IveHandle, &pstCannyInfo->stSrc, \
                                     &pstCannyInfo->stEdge, &pstCannyInfo->stStack, \
                                     &pstCannyInfo->stCannyHysEdgeCtrl, bInstant);
    SAMPLE_CHECK_EXPR_RET(HI_SUCCESS != s32Ret,s32Ret,"Error(%#x),HI_MPI_IVE_CannyHysEdge failed!\n",s32Ret);


    s32Ret = HI_MPI_IVE_Query(IveHandle, &bFinish, bBlock);
    while (HI_ERR_IVE_QUERY_TIMEOUT == s32Ret)
    {
        usleep(100);
        s32Ret = HI_MPI_IVE_Query(IveHandle, &bFinish, bBlock);
    }
    SAMPLE_CHECK_EXPR_RET(HI_SUCCESS != s32Ret,s32Ret,"Error(%#x),HI_MPI_IVE_Query failed!\n",s32Ret);

    s32Ret = HI_MPI_IVE_CannyEdge(&pstCannyInfo->stEdge, &pstCannyInfo->stStack);
    SAMPLE_CHECK_EXPR_RET(HI_SUCCESS != s32Ret,s32Ret,"Error(%#x),HI_MPI_IVE_CannyEdge failed!\n",s32Ret);

    s32Ret = SAMPLE_COMM_IVE_WriteFile(&pstCannyInfo->stEdge, pstCannyInfo->pFpDst);
    SAMPLE_CHECK_EXPR_RET(HI_SUCCESS != s32Ret,s32Ret,"Error(%#x),Write edge file failed!\n",s32Ret);

    return s32Ret;
}
/******************************************************************************
* function : show part canny sample
******************************************************************************/
static HI_S32 SAMPLE_IVE_Part_Canny(SAMPLE_IVE_CANNY_INFO_S* pstCannyInfo)
{
    HI_S32 s32Ret = HI_SUCCESS;
    HI_BOOL bInstant = HI_TRUE;
    HI_BOOL bBlock = HI_TRUE;
    HI_BOOL bFinish = HI_FALSE;
    IVE_HANDLE IveHandle;

    s32Ret = SAMPLE_COMM_IVE_ReadFile(&pstCannyInfo->stSrc, pstCannyInfo->pFpSrc);
    SAMPLE_CHECK_EXPR_RET(HI_SUCCESS != s32Ret,s32Ret,"Error(%#x),Read src file failed!\n",s32Ret);

    bInstant = HI_FALSE;
    s32Ret = HI_MPI_IVE_MagAndAng(&IveHandle, &pstCannyInfo->stSrc, &pstCannyInfo->stMag, \
                                  HI_NULL, &pstCannyInfo->stMagAndAngCtrl, bInstant);
    SAMPLE_CHECK_EXPR_RET(HI_SUCCESS != s32Ret,s32Ret,"Error(%#x),HI_MPI_IVE_MagAndAng failed!\n",s32Ret);

    bInstant = HI_TRUE;
    s32Ret = HI_MPI_IVE_Thresh_U16(&IveHandle, &pstCannyInfo->stMag, &pstCannyInfo->stEdge, \
                                   &pstCannyInfo->stThrU16Ctrl, bInstant);
    SAMPLE_CHECK_EXPR_RET(HI_SUCCESS != s32Ret,s32Ret,"Error(%#x),HI_MPI_IVE_Thresh_U16 failed!\n",s32Ret);

    s32Ret = HI_MPI_IVE_Query(IveHandle, &bFinish, bBlock);
    while (HI_ERR_IVE_QUERY_TIMEOUT == s32Ret)
    {
        usleep(100);
        s32Ret = HI_MPI_IVE_Query(IveHandle, &bFinish, bBlock);
    }
    SAMPLE_CHECK_EXPR_RET(HI_SUCCESS != s32Ret,s32Ret,"Error(%#x),HI_MPI_IVE_Query failed!\n",s32Ret);

    s32Ret = SAMPLE_COMM_IVE_WriteFile(&pstCannyInfo->stEdge, pstCannyInfo->pFpDst);
    SAMPLE_CHECK_EXPR_RET(HI_SUCCESS != s32Ret,s32Ret,"Error(%#x),Write edge file failed!\n",s32Ret);

    return s32Ret;
}
/******************************************************************************
* function : show canny sample
******************************************************************************/
HI_VOID SAMPLE_IVE_Canny(HI_CHAR chComplete)
{
    HI_U16 u32Width = 720;
    HI_U16 u32Height = 576;
    HI_CHAR* pchSrcFileName = "./data/input/canny/canny.yuv";
    HI_CHAR achDstFileName[IVE_FILE_NAME_LEN];
    HI_S32 s32Ret;

    memset(&s_stCannyInfo,0,sizeof(s_stCannyInfo));
    SAMPLE_COMM_IVE_CheckIveMpiInit();

    snprintf(achDstFileName, sizeof(achDstFileName), "./data/output/canny/cannyout_complete_%c.yuv", chComplete);
    s32Ret = SAMPLE_IVE_Canny_Init(&s_stCannyInfo, pchSrcFileName, achDstFileName, u32Width, u32Height);
    SAMPLE_CHECK_EXPR_GOTO(HI_SUCCESS != s32Ret, CANNY_FAIL,
        "Error(%#x),SAMPLE_IVE_Canny_Init failed!\n", s32Ret);

    if ('0' == chComplete)
    {
       s32Ret =  SAMPLE_IVE_Part_Canny(&s_stCannyInfo);
    }
    else
    {
       s32Ret =  SAMPLE_IVE_Complate_Canny(&s_stCannyInfo);
    }

    if (HI_SUCCESS == s32Ret)
    {
        SAMPLE_PRT("Process success!\n");
    }

    SAMPLE_IVE_Canny_Uninit(&s_stCannyInfo);
    memset(&s_stCannyInfo,0,sizeof(s_stCannyInfo));

CANNY_FAIL:
    SAMPLE_COMM_IVE_IveMpiExit();
}
/******************************************************************************
* function :Canny sample signal handle
******************************************************************************/
HI_VOID SAMPLE_IVE_Canny_HandleSig(HI_VOID)
{
    SAMPLE_IVE_Canny_Uninit(&s_stCannyInfo);
    memset(&s_stCannyInfo,0,sizeof(s_stCannyInfo));
    SAMPLE_COMM_IVE_IveMpiExit();
}

