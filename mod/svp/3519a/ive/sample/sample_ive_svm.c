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

#define HI_SVM_PLATE_CLASS_COUNT          2
#define HI_SVM_PLATE_CLASS_DES_NUM        8

#define HI_CLIP(a, maxv, minv)         (((a)>(maxv)) ? (maxv) : (((a) < (minv)) ? (minv) : (a)))

typedef struct hiSAMPLE_IVE_SVM_INFO_S
{
    IVE_SRC_DATA_S stSrc;
    IVE_DST_DATA_S stVote;
    IVE_IMAGE_S stGrayImg;
    IVE_LOOK_UP_TABLE_S stTable;
    IVE_SVM_MODEL_S stSvmModel ;
} SAMPLE_IVE_SVM_INFO_S, *SAMPLE_IVE_SVM_INFO_S_PTR;

static SAMPLE_IVE_SVM_INFO_S s_stSvmInfo;

/******************************************************************************
* function : SVM create table
******************************************************************************/
static HI_S32 SAMPLE_IVE_Svm_CreateTable(IVE_LOOK_UP_TABLE_S* pstTable, HI_FLOAT fGamma)
{
    HI_U32 i;
    HI_DOUBLE tmp;
    HI_S1Q15* ps1q15Tmp;
    HI_FLOAT  f32ExpIn;
    HI_DOUBLE d32ExpOut;

    SAMPLE_CHECK_EXPR_RET(pstTable->stTable.u32Size < pstTable->u16ElemNum * sizeof(HI_S1Q15),HI_FAILURE,
    "Error,Invalid table size!\n");

    ps1q15Tmp = SAMPLE_COMM_IVE_CONVERT_64BIT_ADDR(HI_S1Q15,pstTable->stTable.u64VirAddr);
    for (i = 0; i < pstTable->u16ElemNum; i++)
    {
        f32ExpIn  = pstTable->s32TabInLower + (HI_FLOAT)i / (1 << pstTable->u8TabInPreci);
        d32ExpOut = (exp(-fGamma * f32ExpIn) / (1 << pstTable->u8TabOutNorm)) * (1 << 15);
        tmp = (HI_CLIP(d32ExpOut, (1 << 15) - 1, 0));
        ps1q15Tmp[i] = (HI_S1Q15)(tmp + 0.5f);
    }
    return HI_SUCCESS;
}


/******************************************************************************
* function : SVM uninit
******************************************************************************/
static HI_VOID SAMPLE_IVE_Svm_Uninit(SAMPLE_IVE_SVM_INFO_S* pstSVMInfo)
{
    IVE_MMZ_FREE(pstSVMInfo->stTable.stTable.u64PhyAddr, pstSVMInfo->stTable.stTable.u64VirAddr);
    HI_MPI_IVE_SVM_UnloadModel(&pstSVMInfo->stSvmModel);
    IVE_MMZ_FREE(pstSVMInfo->stSrc.u64PhyAddr, pstSVMInfo->stSrc.u64VirAddr);
    IVE_MMZ_FREE(pstSVMInfo->stVote.u64PhyAddr, pstSVMInfo->stVote.u64VirAddr);
    IVE_MMZ_FREE(pstSVMInfo->stGrayImg.au64PhyAddr[0], pstSVMInfo->stGrayImg.au64VirAddr[0]);
}

/****************************************************************************
* function : SVM init
*****************************************************************************/
static HI_S32 SAMPLE_IVE_Svm_Init(SAMPLE_IVE_SVM_INFO_S* pStSvmInfo, HI_CHAR* pchBinFileName)
{

    HI_S32 s32Ret = HI_SUCCESS;
    HI_U32 u32Size;
    HI_FLOAT fGamma = 0.05f;
    HI_U32 u32Width = 16;
    HI_U32 u32Height = 32;
    HI_U16 u16VecNum = 4;

    memset(pStSvmInfo, 0, sizeof(SAMPLE_IVE_SVM_INFO_S));

    pStSvmInfo->stTable.s32TabInLower = 0;
    pStSvmInfo->stTable.s32TabInUpper = 64;
    pStSvmInfo->stTable.u8TabInPreci  = 3;
    pStSvmInfo->stTable.u8TabOutNorm  = 0;
    pStSvmInfo->stTable.u16ElemNum = (pStSvmInfo->stTable.s32TabInUpper - pStSvmInfo->stTable.s32TabInLower) << pStSvmInfo->stTable.u8TabInPreci;
    pStSvmInfo->stTable.stTable.u32Size = pStSvmInfo->stTable.u16ElemNum * sizeof(HI_U16);
    s32Ret = SAMPLE_COMM_IVE_CreateMemInfo(&(pStSvmInfo->stTable.stTable), pStSvmInfo->stTable.stTable.u32Size);
    SAMPLE_CHECK_EXPR_GOTO(HI_SUCCESS != s32Ret, SVM_INIT_FAIL,
        "Error(%#x),Create table mem info failed!\n", s32Ret);

    s32Ret = SAMPLE_IVE_Svm_CreateTable(&(pStSvmInfo->stTable), fGamma);
    SAMPLE_CHECK_EXPR_GOTO(HI_SUCCESS != s32Ret, SVM_INIT_FAIL,
        "Error(%#x),Create table failed!\n", s32Ret);

    s32Ret = HI_MPI_IVE_SVM_LoadModel(pchBinFileName, &(pStSvmInfo->stSvmModel));
    SAMPLE_CHECK_EXPR_GOTO(HI_SUCCESS != s32Ret, SVM_INIT_FAIL,
        "Error(%#x),HI_MPI_IVE_SVM_LoadModel failed!\n", s32Ret);

    pStSvmInfo ->stSvmModel.enType = IVE_SVM_TYPE_C_SVC;
    pStSvmInfo->stSvmModel.enKernelType = IVE_SVM_KERNEL_TYPE_RBF;
    u32Size = pStSvmInfo->stSvmModel.u16FeatureDim * sizeof(HI_S16Q16);
    s32Ret = SAMPLE_COMM_IVE_CreateData(&(pStSvmInfo->stSrc), u32Size,u16VecNum);
    SAMPLE_CHECK_EXPR_GOTO(HI_SUCCESS != s32Ret, SVM_INIT_FAIL,
        "Error(%#x),Create src data failed!\n", s32Ret);

    u32Size = pStSvmInfo->stSvmModel.u8ClassCount * sizeof(HI_U16);
    s32Ret = SAMPLE_COMM_IVE_CreateData(&(pStSvmInfo->stVote), u32Size,u16VecNum);
    SAMPLE_CHECK_EXPR_GOTO(HI_SUCCESS != s32Ret, SVM_INIT_FAIL,
        "Error(%#x),Create vote data failed!\n", s32Ret);

    s32Ret = SAMPLE_COMM_IVE_CreateImage(&(pStSvmInfo->stGrayImg), IVE_IMAGE_TYPE_U8C1, u32Width, u32Height);
    SAMPLE_CHECK_EXPR_GOTO(HI_SUCCESS != s32Ret, SVM_INIT_FAIL,
        "Error(%#x),Create gray image failed!\n", s32Ret);

SVM_INIT_FAIL:
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_IVE_Svm_Uninit(pStSvmInfo);
    }
    return s32Ret;
}


/******************************************************************************
* function : Svm graysacle feature
******************************************************************************/
static HI_VOID SAMPLE_IVE_Svm_Feature(HI_U8* pu8GrayImg, HI_U32 u32Width, HI_U32 u32Height, HI_S16Q16* ps16q16Feature)
{
    HI_U32  step = 4;
    HI_U16 i, j, m, n;
    HI_U16 u16FeatureNum = 0;
    HI_FLOAT fFeature;
    HI_U32 sum = 0;
    for (i = 0; i < u32Height - step + 1; i += 4)
    {
        for (j = 0; j < u32Width - step + 1; j += 4)
        {
            sum = 0;
            for (m = i; m < i + step; m++)
            {
                for (n = j; n < j + step; n++)
                {
                    sum += pu8GrayImg[m * u32Width + n];
                }
            }
            fFeature = (HI_FLOAT)sum / (step * step * 255);
            ps16q16Feature[u16FeatureNum++] = (HI_S16Q16)(fFeature * (1 << 16) + 0.5f);
        }
    }

}


/******************************************************************************
* function : SVM predict
******************************************************************************/
static HI_S32 SAMPLE_IVE_Svm_Predict(SAMPLE_IVE_SVM_INFO_S* pstSvmInfo)
{

    HI_S32 i, j;
    HI_S32 s32Ret;
    HI_U16*        pu16Vote;
    HI_U8        u8MaxVoteIdx;
    HI_BOOL bInstant = HI_TRUE;
    HI_BOOL bFinish;
    HI_BOOL bBlock = HI_TRUE;
    HI_CHAR achFileName[IVE_FILE_NAME_LEN];
    FILE* pFp = HI_NULL;
    IVE_HANDLE iveHandle;
    HI_S16Q16 *ps16q16Src;
    HI_BOOL bIsSuccess = HI_TRUE;

    for ( i = 0; i < pstSvmInfo->stSrc.u32Height/*4*/; i++)
    {
        ps16q16Src = SAMPLE_COMM_IVE_CONVERT_64BIT_ADDR(HI_S16Q16 ,(pstSvmInfo->stSrc.u64VirAddr  + i * pstSvmInfo->stSrc.u32Stride));

        snprintf(achFileName, sizeof(achFileName), "./data/input/svm/%d.yuv", i);
        pFp = fopen(achFileName, "rb");
        if (NULL == pFp)
        {
            SAMPLE_PRT("Open file %s fail\n", achFileName);
            bIsSuccess = HI_FALSE;
            break;
        }
        s32Ret = SAMPLE_COMM_IVE_ReadFile(&(pstSvmInfo->stGrayImg), pFp);
        if (HI_SUCCESS != s32Ret)
        {
            SAMPLE_PRT("SAMPLE_COMM_IVE_ReadFile fail, file %s\n", achFileName);
            IVE_CLOSE_FILE(pFp);
            bIsSuccess = HI_FALSE;
            break;
        }
        IVE_CLOSE_FILE(pFp);

        SAMPLE_IVE_Svm_Feature(SAMPLE_COMM_IVE_CONVERT_64BIT_ADDR(HI_U8,pstSvmInfo->stGrayImg.au64VirAddr[0]), pstSvmInfo->stGrayImg.u32Width, \
                               pstSvmInfo->stGrayImg.u32Height, ps16q16Src);
    }
    SAMPLE_CHECK_EXPR_RET(HI_TRUE != bIsSuccess,HI_FAILURE,"Error,Open or read file failed!\n");

    s32Ret = HI_MPI_IVE_SVM_Predict(&iveHandle, &(pstSvmInfo->stSrc), &(pstSvmInfo->stTable), \
                                    & (pstSvmInfo->stSvmModel), &(pstSvmInfo->stVote), bInstant);
    SAMPLE_CHECK_EXPR_RET(HI_SUCCESS != s32Ret,s32Ret,"Error(%#x),HI_MPI_IVE_SVM_Predict failed!\n",s32Ret);

    s32Ret = HI_MPI_IVE_Query(iveHandle, &bFinish, bBlock);
    while (HI_ERR_IVE_QUERY_TIMEOUT == s32Ret)
    {
        usleep(100);
        s32Ret = HI_MPI_IVE_Query(iveHandle, &bFinish, bBlock);
    }
    SAMPLE_CHECK_EXPR_RET(HI_SUCCESS != s32Ret,s32Ret,"Error(%#x),HI_MPI_IVE_Query failed!\n",s32Ret);

    for (i = 0; i < pstSvmInfo->stVote.u32Height; i++)
    {
        pu16Vote = SAMPLE_COMM_IVE_CONVERT_64BIT_ADDR(HI_U16,(pstSvmInfo->stVote.u64VirAddr + i * pstSvmInfo->stVote.u32Stride));
        u8MaxVoteIdx = 0;

        for ( j = 1; j < pstSvmInfo->stSvmModel.u8ClassCount; j++)
        {
            if (pu16Vote[u8MaxVoteIdx] < pu16Vote[j])
            {
                u8MaxVoteIdx = j;
            }
        }

        SAMPLE_PRT("%d.yuv belong to class %d.\n", i, u8MaxVoteIdx);
    }

   return s32Ret;
}


/******************************************************************************
* function : show svm sample
******************************************************************************/

HI_VOID SAMPLE_IVE_Svm(HI_VOID)
{
    HI_S32 s32Ret;
    HI_CHAR*    pchBinFileName = "./data/input/svm/svm_char_rec.bin";

    memset(&s_stSvmInfo,0,sizeof(s_stSvmInfo));
    SAMPLE_COMM_IVE_CheckIveMpiInit();

    //Init
    s32Ret = SAMPLE_IVE_Svm_Init(&s_stSvmInfo, pchBinFileName);
    SAMPLE_CHECK_EXPR_GOTO(HI_SUCCESS != s32Ret, SVM_FAIL,
        "Error(%#x),SAMPLE_IVE_Svm_Init failed!\n", s32Ret);

    //Predict
    s32Ret = SAMPLE_IVE_Svm_Predict(&s_stSvmInfo);
    if (HI_SUCCESS == s32Ret)
    {
       SAMPLE_PRT("Process success!\n");
    }
    //Uninit
    SAMPLE_IVE_Svm_Uninit(&s_stSvmInfo);
    memset(&s_stSvmInfo,0,sizeof(s_stSvmInfo));
SVM_FAIL:
    SAMPLE_COMM_IVE_IveMpiExit();
}

/******************************************************************************
* function : Svm sample signal handle
******************************************************************************/
HI_VOID SAMPLE_IVE_Svm_HandleSig(HI_VOID)
{
    SAMPLE_IVE_Svm_Uninit(&s_stSvmInfo);
    memset(&s_stSvmInfo,0,sizeof(s_stSvmInfo));
    SAMPLE_COMM_IVE_IveMpiExit();
}


