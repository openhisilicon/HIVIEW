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

#define HI_CLIP(a, maxv, minv)         (((a)>(maxv)) ? (maxv) : (((a) < (minv)) ? (minv) : (a)))

typedef struct hiSAMPLE_IVE_ANN_INFO_S
{
    IVE_SRC_DATA_S stSrc;
    IVE_DST_DATA_S stDst;
    IVE_IMAGE_S stGray;
    IVE_LOOK_UP_TABLE_S stTable;
    IVE_ANN_MLP_MODEL_S stAnnModel;

} SAMPLE_IVE_ANN_INFO_S;

static SAMPLE_IVE_ANN_INFO_S s_stAnnInfo;

static HI_S32 SAMPLE_IVE_Round(HI_FLOAT fValue)
{
    if (fValue >= 0)
    {
        return (HI_S32)(fValue + 0.5);
    }
    else
    {
        return (HI_S32)(fValue - 0.5);
    }
}
/******************************************************************************
* function : Ann  mlp create table
******************************************************************************/
static HI_S32 SAMPLE_IVE_Ann_Mlp_CreateTable(IVE_LOOK_UP_TABLE_S* pstTable, HI_FLOAT fAlpha, HI_FLOAT fBeta)
{
    HI_U32 i;
    HI_S1Q15* ps1q15Tmp;
    HI_FLOAT  fExpIn;
    HI_DOUBLE dExpOut;
    //Check table size

    SAMPLE_CHECK_EXPR_RET(pstTable->stTable.u32Size < pstTable->u16ElemNum * sizeof(HI_S1Q15),HI_FAILURE,
    "Error,Invalid table size!\n");

    ps1q15Tmp = SAMPLE_COMM_IVE_CONVERT_64BIT_ADDR(HI_S1Q15,pstTable->stTable.u64VirAddr);
    for (i = 0; i < pstTable->u16ElemNum; i++)
    {
        fExpIn  = (HI_FLOAT)i / (1 << pstTable->u8TabInPreci);
        dExpOut = (2 / (1 + exp(-fAlpha * fExpIn)) - 1) * fBeta * (1 << 15) / (1 << pstTable->u8TabOutNorm);
        ps1q15Tmp[i] = (HI_CLIP(SAMPLE_IVE_Round(dExpOut), (1 << 15) - 1, -(1 << 15)));
    }

    return HI_SUCCESS;
}
/******************************************************************************
* function : Ann  mlp binary feature
******************************************************************************/
static HI_VOID SAMPLE_IVE_Ann_Mlp_BinFeature(HI_U8* pu8GrayImg, HI_U32 u32Width, HI_U32 u32Height, HI_S16Q16* ps16q16BinFeature)
{
    HI_U32 u32Step = 4;
    HI_U32 u16Sum = 0;
    HI_U32 i, j;
    HI_U32 m, n;
    HI_U32 u32FeatureNum = 0;

    for (i = 0; i < u32Height - u32Step + 1; i += 4)
    {
        for (j = 0; j < u32Width -  u32Step + 1; j += 4)
        {
            u16Sum = 0;
            for (m = i; m < i + u32Step; m++)
            {
                for (n = j; n < j + u32Step; n++)
                {
                    u16Sum += pu8GrayImg[m * u32Width + n];
                }
            }
            ps16q16BinFeature[u32FeatureNum++] = u16Sum * 65536 / (u32Step * u32Step * 255);
        }
    }
}
/******************************************************************************
* function : Ann mlp uninit
******************************************************************************/
static HI_VOID SAMPLE_IVE_Ann_Mlp_Uninit(SAMPLE_IVE_ANN_INFO_S* pstAnnInfo)
{
    IVE_MMZ_FREE(pstAnnInfo->stTable.stTable.u64PhyAddr, pstAnnInfo->stTable.stTable.u64VirAddr);
    HI_MPI_IVE_ANN_MLP_UnloadModel(&pstAnnInfo->stAnnModel);
    IVE_MMZ_FREE(pstAnnInfo->stSrc.u64PhyAddr, pstAnnInfo->stSrc.u64VirAddr);
    IVE_MMZ_FREE(pstAnnInfo->stDst.u64PhyAddr, pstAnnInfo->stDst.u64VirAddr);
    IVE_MMZ_FREE(pstAnnInfo->stGray.au64PhyAddr[0], pstAnnInfo->stGray.au64VirAddr[0]);

}
/******************************************************************************
* function : Ann mlp init
******************************************************************************/
static HI_S32 SAMPLE_IVE_Ann_Mlp_Init(SAMPLE_IVE_ANN_INFO_S* pstAnnInfo, HI_CHAR* pchBinFileName)
{
    HI_S32 s32Ret = HI_SUCCESS;
    HI_U32 u32Size;
    HI_U16 u16VecNum = 3;
    memset(pstAnnInfo, 0, sizeof(SAMPLE_IVE_ANN_INFO_S));

    pstAnnInfo->stTable.s32TabInLower = 0;
    pstAnnInfo->stTable.s32TabInUpper = 1;//1;
    pstAnnInfo->stTable.u8TabInPreci  = 8;//12;
    pstAnnInfo->stTable.u8TabOutNorm  = 2;
    pstAnnInfo->stTable.u16ElemNum = pstAnnInfo->stTable.s32TabInUpper << pstAnnInfo->stTable.u8TabInPreci;
    u32Size = pstAnnInfo->stTable.u16ElemNum * sizeof(HI_U16);
    s32Ret = SAMPLE_COMM_IVE_CreateMemInfo(&(pstAnnInfo->stTable.stTable), u32Size);
    SAMPLE_CHECK_EXPR_GOTO(HI_SUCCESS != s32Ret, ANN_INIT_FAIL,
        "Error(%#x),Create table mem info failed!\n", s32Ret);

    s32Ret = SAMPLE_IVE_Ann_Mlp_CreateTable(&(pstAnnInfo->stTable), 0.6667f, 1.7159f);
    SAMPLE_CHECK_EXPR_GOTO(HI_SUCCESS != s32Ret, ANN_INIT_FAIL,
        "Error(%#x),Create table failed!\n", s32Ret);

    s32Ret = HI_MPI_IVE_ANN_MLP_LoadModel(pchBinFileName, &(pstAnnInfo->stAnnModel));
    SAMPLE_CHECK_EXPR_GOTO(HI_SUCCESS != s32Ret, ANN_INIT_FAIL,
        "Error(%#x),HI_MPI_IVE_ANN_MLP_LoadModel failed!\n", s32Ret);

    u32Size = pstAnnInfo->stAnnModel.au16LayerCount[0] * sizeof(HI_S16Q16);
    s32Ret = SAMPLE_COMM_IVE_CreateData(&(pstAnnInfo->stSrc),u32Size,u16VecNum);
    SAMPLE_CHECK_EXPR_GOTO(HI_SUCCESS != s32Ret, ANN_INIT_FAIL,
        "Error(%#x),Create src data failed!\n", s32Ret);

    u32Size = pstAnnInfo->stAnnModel.au16LayerCount[pstAnnInfo->stAnnModel.u8LayerNum - 1] * sizeof(HI_S16Q16);
    s32Ret = SAMPLE_COMM_IVE_CreateData(&(pstAnnInfo->stDst), u32Size,u16VecNum);
    SAMPLE_CHECK_EXPR_GOTO(HI_SUCCESS != s32Ret, ANN_INIT_FAIL,
        "Error(%#x),Create dst data failed!\n", s32Ret);

    s32Ret = SAMPLE_COMM_IVE_CreateImage(&(pstAnnInfo->stGray), IVE_IMAGE_TYPE_U8C1, 16, 32);
    SAMPLE_CHECK_EXPR_GOTO(HI_SUCCESS != s32Ret, ANN_INIT_FAIL,
       "Error(%#x),Create gray image failed!\n", s32Ret);

ANN_INIT_FAIL:

    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_IVE_Ann_Mlp_Uninit(pstAnnInfo);
    }

    return s32Ret;

}
/******************************************************************************
* function : Ann mlp predict
******************************************************************************/
static HI_S32 SAMPLE_IVE_Ann_Mlp_Predict(SAMPLE_IVE_ANN_INFO_S* pstAnnInfo)
{
    HI_S32 i, k;
    HI_S32 s32Ret = HI_FAILURE;
    HI_S32 s32ResponseCls;
    HI_U16 u16LayerCount;
    HI_S16Q16 *ps16q16Src,*ps16q16Dst;
    HI_S16Q16 s16q16Response;
    HI_BOOL bInstant = HI_TRUE;
    HI_BOOL bFinish;
    HI_BOOL bBlock = HI_TRUE;
    HI_BOOL bIsSuccess = HI_TRUE;
    HI_CHAR achFileName[IVE_FILE_NAME_LEN];
    FILE* pFp = HI_NULL;
    IVE_HANDLE iveHandle;

    for (i = 0; i < pstAnnInfo->stSrc.u32Height; ++i)
    {

        ps16q16Src = SAMPLE_COMM_IVE_CONVERT_64BIT_ADDR(HI_S16Q16,(pstAnnInfo->stSrc.u64VirAddr + pstAnnInfo->stSrc.u32Stride * i));

        snprintf(achFileName, sizeof(achFileName), "./data/input/ann/%d.yuv", i);
        pFp = fopen(achFileName, "rb");
        if (NULL == pFp)
        {
            SAMPLE_PRT("Open file fail,file %s\n", achFileName);
            bIsSuccess = HI_FALSE;
            break;
        }

        s32Ret = SAMPLE_COMM_IVE_ReadFile(&(pstAnnInfo->stGray), pFp);
        if (s32Ret != HI_SUCCESS)
        {
            SAMPLE_PRT("SAMPLE_COMM_IVE_ReadFile fail,file %s\n", achFileName);
            IVE_CLOSE_FILE(pFp);
            bIsSuccess = HI_FALSE;
            break;
        }
        IVE_CLOSE_FILE(pFp);

        SAMPLE_IVE_Ann_Mlp_BinFeature(SAMPLE_COMM_IVE_CONVERT_64BIT_ADDR(HI_U8,pstAnnInfo->stGray.au64VirAddr[0]),\
                                        pstAnnInfo->stGray.u32Width, \
                                      pstAnnInfo->stGray.u32Height, \
                                      (HI_S16Q16*)ps16q16Src);

    }
    SAMPLE_CHECK_EXPR_RET(HI_TRUE != bIsSuccess,HI_FAILURE,"Error,Open or read file failed!\n");

    s32Ret = HI_MPI_IVE_ANN_MLP_Predict(&iveHandle, &(pstAnnInfo->stSrc), \
                                                &(pstAnnInfo->stTable), &(pstAnnInfo->stAnnModel), &(pstAnnInfo->stDst), bInstant);
    SAMPLE_CHECK_EXPR_RET(HI_SUCCESS != s32Ret,s32Ret,"Error(%#x),HI_MPI_IVE_ANN_MLP_Predict failed!\n",s32Ret);

    s32Ret = HI_MPI_IVE_Query(iveHandle, &bFinish, bBlock);
    while (HI_ERR_IVE_QUERY_TIMEOUT == s32Ret)
    {
        usleep(100);
        s32Ret = HI_MPI_IVE_Query(iveHandle, &bFinish, bBlock);
    }
    SAMPLE_CHECK_EXPR_RET(HI_SUCCESS != s32Ret,s32Ret,"Error(%#x),HI_MPI_IVE_Query failed!\n",s32Ret);

    u16LayerCount = pstAnnInfo->stAnnModel.au16LayerCount[pstAnnInfo->stAnnModel.u8LayerNum - 1];
    for (i = 0; i < pstAnnInfo->stDst.u32Height; i++)
    {
        ps16q16Dst = SAMPLE_COMM_IVE_CONVERT_64BIT_ADDR(HI_S16Q16,(pstAnnInfo->stDst.u64VirAddr + pstAnnInfo->stDst.u32Stride * i));
        s16q16Response = 0;
        s32ResponseCls = -1;

        for (k = 0; k < u16LayerCount; k++)
        {
            if (s16q16Response < ps16q16Dst[k])
            {
                s16q16Response = ps16q16Dst[k];
                s32ResponseCls = k;
            }
        }

        SAMPLE_PRT("%d.yuv belong to class %d.\n", i, s32ResponseCls);
    }

    return s32Ret;

}
/******************************************************************************
* function : show ann sample
******************************************************************************/
HI_VOID SAMPLE_IVE_Ann(HI_VOID)
{
    HI_S32 s32Ret;
    HI_CHAR* pchBinFileName = "./data/input/ann/ann_char_rec_ex.bin";

    memset(&s_stAnnInfo,0,sizeof(s_stAnnInfo));
    SAMPLE_COMM_IVE_CheckIveMpiInit();

    //init
    s32Ret = SAMPLE_IVE_Ann_Mlp_Init(&s_stAnnInfo, pchBinFileName);
    SAMPLE_CHECK_EXPR_GOTO(HI_SUCCESS != s32Ret, ANN_FAIL,
        "Error(%#x),SAMPLE_IVE_Ann_Mlp_Init failed!\n", s32Ret);

    //predict
    s32Ret = SAMPLE_IVE_Ann_Mlp_Predict(&s_stAnnInfo);
    if (HI_SUCCESS == s32Ret)
    {
       SAMPLE_PRT("Process success!\n");
    }
    //uninit
    SAMPLE_IVE_Ann_Mlp_Uninit(&s_stAnnInfo);
    memset(&s_stAnnInfo,0,sizeof(s_stAnnInfo));

ANN_FAIL:
    SAMPLE_COMM_IVE_IveMpiExit();
}

/******************************************************************************
* function : Ann sample signal handle
******************************************************************************/
HI_VOID SAMPLE_IVE_Ann_HandleSig(HI_VOID)
{
    SAMPLE_IVE_Ann_Mlp_Uninit(&s_stAnnInfo);
    memset(&s_stAnnInfo,0,sizeof(s_stAnnInfo));
    SAMPLE_COMM_IVE_IveMpiExit();
}


