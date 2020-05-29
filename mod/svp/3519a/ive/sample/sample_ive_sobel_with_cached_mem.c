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

typedef struct hiSAMPLE_IVE_SOBEL_S
{
    IVE_SRC_IMAGE_S stSrc1;
    IVE_SRC_IMAGE_S stSrc2;
    IVE_DST_IMAGE_S stDstH1;
    IVE_DST_IMAGE_S stDstH2;
    IVE_DST_IMAGE_S stDstV1;
    IVE_DST_IMAGE_S stDstV2;
    IVE_SOBEL_CTRL_S stSobelCtrl;
    FILE* pFpSrc;
    FILE* pFpDstH1;
    FILE* pFpDstH2;
    FILE* pFpDstV1;
    FILE* pFpDstV2;
} SAMPLE_IVE_SOBEL_S;

static HI_BOOL s_bFlushCached = HI_TRUE;
static SAMPLE_IVE_SOBEL_S s_stSobel;
/******************************************************************************
* function : show Sobel uninit
******************************************************************************/
static HI_VOID SAMPLE_IVE_Sobel_Uninit(SAMPLE_IVE_SOBEL_S* pstSobel)
{
    IVE_MMZ_FREE(pstSobel->stSrc1.au64PhyAddr[0], pstSobel->stSrc1.au64VirAddr[0]);
    IVE_MMZ_FREE(pstSobel->stSrc2.au64PhyAddr[0], pstSobel->stSrc2.au64VirAddr[0]);
    IVE_MMZ_FREE(pstSobel->stDstH1.au64PhyAddr[0], pstSobel->stDstH1.au64VirAddr[0]);
    IVE_MMZ_FREE(pstSobel->stDstH2.au64PhyAddr[0], pstSobel->stDstH2.au64VirAddr[0]);
    IVE_MMZ_FREE(pstSobel->stDstV1.au64PhyAddr[0], pstSobel->stDstV1.au64VirAddr[0]);
    IVE_MMZ_FREE(pstSobel->stDstV2.au64PhyAddr[0], pstSobel->stDstV2.au64VirAddr[0]);

    IVE_CLOSE_FILE(pstSobel->pFpSrc);
    IVE_CLOSE_FILE(pstSobel->pFpDstH1);
    IVE_CLOSE_FILE(pstSobel->pFpDstH2);
    IVE_CLOSE_FILE(pstSobel->pFpDstV1);
    IVE_CLOSE_FILE(pstSobel->pFpDstV2);
}
/******************************************************************************
* function : show Sobel init
******************************************************************************/
static HI_S32 SAMPLE_IVE_Sobel_Init(SAMPLE_IVE_SOBEL_S* pstSobel,
                                    HI_CHAR* pchSrcFileName, HI_U32 u32Width, HI_U32 u32Height)
{
    HI_S32 s32Ret = HI_SUCCESS;
    HI_CHAR achFileName[IVE_FILE_NAME_LEN];
    HI_S8 as8Mask[25] = {0, 0, 0, 0, 0,
                         0, -1, 0, 1, 0,
                         0, -2, 0, 2, 0,
                         0, -1, 0, 1, 0,
                         0, 0, 0, 0, 0
                        };

    memset(pstSobel, 0, sizeof(SAMPLE_IVE_SOBEL_S));

    s32Ret = SAMPLE_COMM_IVE_CreateImageByCached(&(pstSobel->stSrc1), IVE_IMAGE_TYPE_U8C1, u32Width, u32Height);
    SAMPLE_CHECK_EXPR_GOTO(HI_SUCCESS != s32Ret, SOBEL_INIT_FAIL,
        "Error(%#x),Create src1 image failed!\n", s32Ret);

    s32Ret = SAMPLE_COMM_IVE_CreateImageByCached(&(pstSobel->stSrc2), IVE_IMAGE_TYPE_U8C1, u32Width, u32Height);
    SAMPLE_CHECK_EXPR_GOTO(HI_SUCCESS != s32Ret, SOBEL_INIT_FAIL,
        "Error(%#x),Create src2 image failed!\n", s32Ret);

    s32Ret = SAMPLE_COMM_IVE_CreateImage(&(pstSobel->stDstH1), IVE_IMAGE_TYPE_S16C1, u32Width, u32Height);
    SAMPLE_CHECK_EXPR_GOTO(HI_SUCCESS != s32Ret, SOBEL_INIT_FAIL,
        "Error(%#x),Create dstH1 image failed!\n", s32Ret);

    s32Ret = SAMPLE_COMM_IVE_CreateImage(&(pstSobel->stDstH2), IVE_IMAGE_TYPE_S16C1, u32Width, u32Height);
    SAMPLE_CHECK_EXPR_GOTO(HI_SUCCESS != s32Ret, SOBEL_INIT_FAIL,
        "Error(%#x),Create dstH2 image failed!\n", s32Ret);

    s32Ret = SAMPLE_COMM_IVE_CreateImage(&(pstSobel->stDstV1), IVE_IMAGE_TYPE_S16C1, u32Width, u32Height);
    SAMPLE_CHECK_EXPR_GOTO(HI_SUCCESS != s32Ret, SOBEL_INIT_FAIL,
        "Error(%#x),Create dstV1 image failed!\n", s32Ret);

    s32Ret = SAMPLE_COMM_IVE_CreateImage(&(pstSobel->stDstV2), IVE_IMAGE_TYPE_S16C1, u32Width, u32Height);
    SAMPLE_CHECK_EXPR_GOTO(HI_SUCCESS != s32Ret, SOBEL_INIT_FAIL,
        "Error(%#x),Create dstV2 image failed!\n", s32Ret);

    pstSobel->stSobelCtrl.enOutCtrl = IVE_SOBEL_OUT_CTRL_BOTH;
    memcpy(pstSobel->stSobelCtrl.as8Mask, as8Mask, 25);
    s32Ret = HI_FAILURE;
    pstSobel->pFpSrc = fopen(pchSrcFileName, "rb");
    SAMPLE_CHECK_EXPR_GOTO(HI_NULL == pstSobel->pFpSrc, SOBEL_INIT_FAIL,
        "Error,Open file %s failed!\n", pchSrcFileName);

    snprintf(achFileName, sizeof(achFileName), "./data/output/sobel/sobelh1.dat");
    pstSobel->pFpDstH1 = fopen(achFileName, "wb");
    SAMPLE_CHECK_EXPR_GOTO(HI_NULL == pstSobel->pFpDstH1, SOBEL_INIT_FAIL,
        "Error,Open file %s failed!\n", achFileName);

    snprintf(achFileName, sizeof(achFileName), "./data/output/sobel/sobelh2.dat");
    pstSobel->pFpDstH2 = fopen(achFileName, "wb");
    SAMPLE_CHECK_EXPR_GOTO(HI_NULL == pstSobel->pFpDstH2, SOBEL_INIT_FAIL,
        "Error,Open file %s failed!\n", achFileName);

    snprintf(achFileName, sizeof(achFileName), "./data/output/sobel/sobelv1.dat");
    pstSobel->pFpDstV1 = fopen(achFileName, "wb");
    SAMPLE_CHECK_EXPR_GOTO(HI_NULL == pstSobel->pFpDstV1, SOBEL_INIT_FAIL,
        "Error,Open file %s failed!\n", achFileName);

    snprintf(achFileName, sizeof(achFileName), "./data/output/sobel/sobelv2.dat");
    pstSobel->pFpDstV2 = fopen(achFileName, "wb");
    SAMPLE_CHECK_EXPR_GOTO(HI_NULL == pstSobel->pFpDstV2, SOBEL_INIT_FAIL,
        "Error,Open file %s failed!\n", achFileName);

    s32Ret = HI_SUCCESS;

SOBEL_INIT_FAIL:

    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_IVE_Sobel_Uninit(pstSobel);
    }
    return s32Ret;
}
/******************************************************************************
* function : show Sobel proc
******************************************************************************/
static HI_S32 SAMPLE_IVE_SobelProc(SAMPLE_IVE_SOBEL_S* pstSobel)
{
    HI_S32 s32Ret = HI_SUCCESS;
    HI_BOOL bInstant = HI_TRUE;
    IVE_HANDLE IveHandle;
    HI_BOOL bBlock = HI_TRUE;
    HI_BOOL bFinish = HI_FALSE;

    s32Ret = SAMPLE_COMM_IVE_ReadFile(&(pstSobel->stSrc1), pstSobel->pFpSrc);
    SAMPLE_CHECK_EXPR_RET(HI_SUCCESS != s32Ret,s32Ret,"Error(%#x),Read src file failed!\n",s32Ret);

    memcpy(SAMPLE_COMM_IVE_CONVERT_64BIT_ADDR(HI_VOID,pstSobel->stSrc2.au64VirAddr[0]),
        SAMPLE_COMM_IVE_CONVERT_64BIT_ADDR(HI_VOID,pstSobel->stSrc1.au64VirAddr[0]), pstSobel->stSrc2.au32Stride[0] * pstSobel->stSrc2.u32Height);
    s32Ret = HI_MPI_SYS_MmzFlushCache(pstSobel->stSrc1.au64PhyAddr[0], SAMPLE_COMM_IVE_CONVERT_64BIT_ADDR(HI_VOID,pstSobel->stSrc1.au64VirAddr[0]),
                                      pstSobel->stSrc1.au32Stride[0] * pstSobel->stSrc1.u32Height);
    SAMPLE_CHECK_EXPR_RET(HI_SUCCESS != s32Ret,s32Ret,"Error(%#x),HI_MPI_SYS_MmzFlushCache failed!\n",s32Ret);

    s32Ret = HI_MPI_IVE_Sobel(&IveHandle, &pstSobel->stSrc1, &pstSobel->stDstH1, &pstSobel->stDstV1, &pstSobel->stSobelCtrl, bInstant);
    SAMPLE_CHECK_EXPR_RET(HI_SUCCESS != s32Ret,s32Ret,"Error(%#x),HI_MPI_IVE_Sobel failed!\n",s32Ret);

    s32Ret = HI_MPI_IVE_Query(IveHandle, &bFinish, bBlock);
    while (HI_ERR_IVE_QUERY_TIMEOUT == s32Ret)
    {
        usleep(100);
        s32Ret = HI_MPI_IVE_Query(IveHandle, &bFinish, bBlock);
    }

    SAMPLE_CHECK_EXPR_RET(HI_SUCCESS != s32Ret,s32Ret,"Error(%#x),HI_MPI_IVE_Query failed!\n",s32Ret);

    s32Ret = SAMPLE_COMM_IVE_WriteFile(&pstSobel->stDstH1, pstSobel->pFpDstH1);
    SAMPLE_CHECK_EXPR_RET(HI_SUCCESS != s32Ret,s32Ret,"Error(%#x),Write dstH1 failed!\n",s32Ret);

    s32Ret = SAMPLE_COMM_IVE_WriteFile(&pstSobel->stDstV1, pstSobel->pFpDstV1);
    SAMPLE_CHECK_EXPR_RET(HI_SUCCESS != s32Ret,s32Ret,"Error(%#x),Write dstV1 failed!\n",s32Ret);

    //Second sobel
    // The result of sobel my be error,if you do not call HI_MPI_SYS_MmzFlushCache to flush cache
    if (s_bFlushCached == HI_TRUE)
    {
        s32Ret = HI_MPI_SYS_MmzFlushCache(pstSobel->stSrc2.au64PhyAddr[0], SAMPLE_COMM_IVE_CONVERT_64BIT_ADDR(HI_VOID,pstSobel->stSrc2.au64VirAddr[0]),
                                          pstSobel->stSrc2.au32Stride[0] * pstSobel->stSrc2.u32Height);
        SAMPLE_CHECK_EXPR_RET(HI_SUCCESS != s32Ret,s32Ret,"Error(%#x),HI_MPI_SYS_MmzFlushCache failed!\n",s32Ret);
    }

    s32Ret = HI_MPI_IVE_Sobel(&IveHandle, &pstSobel->stSrc2, &pstSobel->stDstH2, &pstSobel->stDstV2, &pstSobel->stSobelCtrl, bInstant);
      SAMPLE_CHECK_EXPR_RET(HI_SUCCESS != s32Ret,s32Ret,"Error(%#x),HI_MPI_IVE_Sobel failed!\n",s32Ret);

    s32Ret = HI_MPI_IVE_Query(IveHandle, &bFinish, bBlock);
    while (HI_ERR_IVE_QUERY_TIMEOUT == s32Ret)
    {
        usleep(100);
        s32Ret = HI_MPI_IVE_Query(IveHandle, &bFinish, bBlock);
    }
      SAMPLE_CHECK_EXPR_RET(HI_SUCCESS != s32Ret,s32Ret,"Error(%#x),HI_MPI_IVE_Query failed!\n",s32Ret);

    s32Ret = SAMPLE_COMM_IVE_WriteFile(&pstSobel->stDstH2, pstSobel->pFpDstH2);
      SAMPLE_CHECK_EXPR_RET(HI_SUCCESS != s32Ret,s32Ret,"Error(%#x),Write dstH2 failed!\n",s32Ret);

    s32Ret = SAMPLE_COMM_IVE_WriteFile(&pstSobel->stDstV2, pstSobel->pFpDstV2);
      SAMPLE_CHECK_EXPR_RET(HI_SUCCESS != s32Ret,s32Ret,"Error(%#x),Write dstV2 failed!\n",s32Ret);

    return s32Ret;
}
/******************************************************************************
* function : show Sobel sample
******************************************************************************/
HI_VOID SAMPLE_IVE_Sobel(HI_VOID)
{
    HI_S32 s32Ret;
    HI_U32 u32Width = 720;
    HI_U32 u32Height = 576;
    HI_CHAR* pchSrcFileName = "./data/input/sobel/sobel.yuv";

    memset(&s_stSobel,0,sizeof(s_stSobel));
    SAMPLE_COMM_IVE_CheckIveMpiInit();

    s32Ret = SAMPLE_IVE_Sobel_Init(&s_stSobel, pchSrcFileName, u32Width, u32Height);
    SAMPLE_CHECK_EXPR_GOTO(HI_SUCCESS != s32Ret, SOBEL_FAIL,
        "Error(%#x),SAMPLE_IVE_Sobel_Init failed!\n", s32Ret);

    s32Ret =  SAMPLE_IVE_SobelProc(&s_stSobel);
    if (HI_SUCCESS == s32Ret)
    {
      SAMPLE_PRT("Process success!\n");
    }

    SAMPLE_IVE_Sobel_Uninit(&s_stSobel);
    memset(&s_stSobel,0,sizeof(s_stSobel));

SOBEL_FAIL:
    SAMPLE_COMM_IVE_IveMpiExit();
}
/******************************************************************************
* function : Sobel sample signal handle
******************************************************************************/
HI_VOID SAMPLE_IVE_Sobel_HandleSig(HI_VOID)
{
    SAMPLE_IVE_Sobel_Uninit(&s_stSobel);
    memset(&s_stSobel,0,sizeof(s_stSobel));
    SAMPLE_COMM_IVE_IveMpiExit();
}



