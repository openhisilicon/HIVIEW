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
#include <dirent.h>
#include "hi_common.h"
#include "hi_comm_sys.h"
#include "hi_comm_svp.h"
#include "sample_comm.h"
#include "sample_comm_svp.h"
#include "sample_comm_nnie.h"
#include "nnie_core.h"
#include "Tensor.h"

int load_model(const char *model_path, SAMPLE_SVP_NNIE_MODEL_S *s_stModel)
{

    /*Sys init*/
    SAMPLE_COMM_SVP_CheckSysInit();
    HI_S32 s32Ret = SAMPLE_COMM_SVP_NNIE_LoadModel(model_path, s_stModel);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_SVP_TRACE_INFO("Error,SAMPLE_COMM_SVP_NNIE_LoadModel failed!\n");
        return -1;
    }
}

void nnie_param_init(SAMPLE_SVP_NNIE_MODEL_S *s_stModel, SAMPLE_SVP_NNIE_CFG_S *stNnieCfg, SAMPLE_SVP_NNIE_PARAM_S *s_stNnieParam)
{
    stNnieCfg->u32MaxInputNum = 1; //max input image num in each batch
    stNnieCfg->u32MaxRoiNum = 0;
    stNnieCfg->aenNnieCoreId[0] = SVP_NNIE_ID_0; //set NNIE core
    s_stNnieParam->pstModel = &s_stModel->stModel;

    HI_S32 s32Ret = HI_SUCCESS;
    s32Ret = SAMPLE_COMM_SVP_NNIE_ParamInit(stNnieCfg, s_stNnieParam);

    if (HI_SUCCESS != s32Ret)
    {
        NNIE_Param_Deinit(s_stNnieParam, s_stModel);
        SAMPLE_COMM_SVP_CheckSysExit();
        SAMPLE_SVP_TRACE_INFO("Error,nnie_Param_init failed!\n");
        return;
    }
}

/******************************************************************************
* function : Fill Src Data
******************************************************************************/
static HI_S32 NNIE_FillData(const unsigned char *data,
                            SAMPLE_SVP_NNIE_PARAM_S *pstNnieParam, SAMPLE_SVP_NNIE_INPUT_DATA_INDEX_S *pstInputDataIdx)
{
    HI_U32 i = 0, j = 0, n = 0;
    HI_U32 u32Height = 0, u32Width = 0, u32Chn = 0, u32Stride = 0, u32Dim = 0;
    HI_U32 u32VarSize = 0;
    HI_U8 *pu8PicAddr = NULL;
    HI_U32 *pu32StepAddr = NULL;
    HI_U32 u32SegIdx = pstInputDataIdx->u32SegIdx;
    HI_U32 u32NodeIdx = pstInputDataIdx->u32NodeIdx;
    HI_U32 u32TotalStepNum = 0;

    /*get data size*/
    if (SVP_BLOB_TYPE_U8 <= pstNnieParam->astSegData[u32SegIdx].astSrc[u32NodeIdx].enType &&
        SVP_BLOB_TYPE_YVU422SP >= pstNnieParam->astSegData[u32SegIdx].astSrc[u32NodeIdx].enType)
    {
        u32VarSize = sizeof(HI_U8);
    }
    else
    {
        u32VarSize = sizeof(HI_U32);
    }

    /*fill src data*/
    if (SVP_BLOB_TYPE_SEQ_S32 == pstNnieParam->astSegData[u32SegIdx].astSrc[u32NodeIdx].enType)
    {
        u32Dim = pstNnieParam->astSegData[u32SegIdx].astSrc[u32NodeIdx].unShape.stSeq.u32Dim;
        u32Stride = pstNnieParam->astSegData[u32SegIdx].astSrc[u32NodeIdx].u32Stride;
        pu32StepAddr = (HI_U32 *)(pstNnieParam->astSegData[u32SegIdx].astSrc[u32NodeIdx].unShape.stSeq.u64VirAddrStep);
        pu8PicAddr = (HI_U8 *)(pstNnieParam->astSegData[u32SegIdx].astSrc[u32NodeIdx].u64VirAddr);
        for (n = 0; n < pstNnieParam->astSegData[u32SegIdx].astSrc[u32NodeIdx].u32Num; n++)
        {
            for (i = 0; i < *(pu32StepAddr + n); i++)
            {
                memcpy(pu8PicAddr, data, u32Dim * u32VarSize);
                data += u32Dim * u32VarSize;
                pu8PicAddr += u32Stride;
            }
            u32TotalStepNum += *(pu32StepAddr + n);
        }
        SAMPLE_COMM_SVP_FlushCache(pstNnieParam->astSegData[u32SegIdx].astSrc[u32NodeIdx].u64PhyAddr,
                                   (HI_VOID *)pstNnieParam->astSegData[u32SegIdx].astSrc[u32NodeIdx].u64VirAddr,
                                   u32TotalStepNum * u32Stride);
    }
    else
    {
        u32Height = pstNnieParam->astSegData[u32SegIdx].astSrc[u32NodeIdx].unShape.stWhc.u32Height;
        u32Width = pstNnieParam->astSegData[u32SegIdx].astSrc[u32NodeIdx].unShape.stWhc.u32Width;
        u32Chn = pstNnieParam->astSegData[u32SegIdx].astSrc[u32NodeIdx].unShape.stWhc.u32Chn;
        u32Stride = pstNnieParam->astSegData[u32SegIdx].astSrc[u32NodeIdx].u32Stride;
        pu8PicAddr = (HI_U8 *)(pstNnieParam->astSegData[u32SegIdx].astSrc[u32NodeIdx].u64VirAddr);
        if (SVP_BLOB_TYPE_YVU420SP == pstNnieParam->astSegData[u32SegIdx].astSrc[u32NodeIdx].enType)
        {
            for (n = 0; n < pstNnieParam->astSegData[u32SegIdx].astSrc[u32NodeIdx].u32Num; n++)
            {
                for (i = 0; i < u32Chn * u32Height / 2; i++)
                {

                    memcpy(pu8PicAddr, data, u32Width * u32VarSize);
                    data += u32Width * u32VarSize;
                    pu8PicAddr += u32Stride;
                }
            }
        }
        else if (SVP_BLOB_TYPE_YVU422SP == pstNnieParam->astSegData[u32SegIdx].astSrc[u32NodeIdx].enType)
        {
            for (n = 0; n < pstNnieParam->astSegData[u32SegIdx].astSrc[u32NodeIdx].u32Num; n++)
            {
                for (i = 0; i < u32Height * 2; i++)
                {
                    memcpy(pu8PicAddr, data, u32Width * u32VarSize);
                    data += u32Width * u32VarSize;
                    pu8PicAddr += u32Stride;
                }
            }
        }
        else
        {
            for (n = 0; n < pstNnieParam->astSegData[u32SegIdx].astSrc[u32NodeIdx].u32Num; n++)
            {
                for (i = 0; i < u32Chn; i++)
                {
                    for (j = 0; j < u32Height; j++)
                    {
                        memcpy(pu8PicAddr, data, u32Width * u32VarSize);
                        data += u32Width * u32VarSize;
                        pu8PicAddr += u32Stride;
                    }
                }
            }
        }
        SAMPLE_COMM_SVP_FlushCache(pstNnieParam->astSegData[u32SegIdx].astSrc[u32NodeIdx].u64PhyAddr,
                                   (HI_VOID *)pstNnieParam->astSegData[u32SegIdx].astSrc[u32NodeIdx].u64VirAddr,
                                   pstNnieParam->astSegData[u32SegIdx].astSrc[u32NodeIdx].u32Num * u32Chn * u32Height * u32Stride);
    }

    return HI_SUCCESS;
}

static HI_S32 NNIE_Forward(SAMPLE_SVP_NNIE_PARAM_S *pstNnieParam,
                           SAMPLE_SVP_NNIE_INPUT_DATA_INDEX_S *pstInputDataIdx,
                           SAMPLE_SVP_NNIE_PROCESS_SEG_INDEX_S *pstProcSegIdx, HI_BOOL bInstant)
{
    HI_S32 s32Ret = HI_SUCCESS;
    HI_U32 i = 0, j = 0;
    HI_BOOL bFinish = HI_FALSE;
    SVP_NNIE_HANDLE hSvpNnieHandle = 0;
    HI_U32 u32TotalStepNum = 0;

    SAMPLE_COMM_SVP_FlushCache(pstNnieParam->astForwardCtrl[pstProcSegIdx->u32SegIdx].stTskBuf.u64PhyAddr,
                               (HI_VOID *)pstNnieParam->astForwardCtrl[pstProcSegIdx->u32SegIdx].stTskBuf.u64VirAddr,
                               pstNnieParam->astForwardCtrl[pstProcSegIdx->u32SegIdx].stTskBuf.u32Size);

    /*set input blob according to node name*/
    if (pstInputDataIdx->u32SegIdx != pstProcSegIdx->u32SegIdx)
    {
        for (i = 0; i < pstNnieParam->pstModel->astSeg[pstProcSegIdx->u32SegIdx].u16SrcNum; i++)
        {
            for (j = 0; j < pstNnieParam->pstModel->astSeg[pstInputDataIdx->u32SegIdx].u16DstNum; j++)
            {
                if (0 == strncmp(pstNnieParam->pstModel->astSeg[pstInputDataIdx->u32SegIdx].astDstNode[j].szName,
                                 pstNnieParam->pstModel->astSeg[pstProcSegIdx->u32SegIdx].astSrcNode[i].szName,
                                 SVP_NNIE_NODE_NAME_LEN))
                {
                    pstNnieParam->astSegData[pstProcSegIdx->u32SegIdx].astSrc[i] =
                        pstNnieParam->astSegData[pstInputDataIdx->u32SegIdx].astDst[j];
                    break;
                }
            }
            SAMPLE_SVP_CHECK_EXPR_RET((j == pstNnieParam->pstModel->astSeg[pstInputDataIdx->u32SegIdx].u16DstNum),
                                      HI_FAILURE, SAMPLE_SVP_ERR_LEVEL_ERROR, "Error,can't find %d-th seg's %d-th src blob!\n",
                                      pstProcSegIdx->u32SegIdx, i);
        }
    }

    /*NNIE_Forward*/
    s32Ret = HI_MPI_SVP_NNIE_Forward(&hSvpNnieHandle,
                                     pstNnieParam->astSegData[pstProcSegIdx->u32SegIdx].astSrc,
                                     pstNnieParam->pstModel, pstNnieParam->astSegData[pstProcSegIdx->u32SegIdx].astDst,
                                     &pstNnieParam->astForwardCtrl[pstProcSegIdx->u32SegIdx], bInstant);
    SAMPLE_SVP_CHECK_EXPR_RET(HI_SUCCESS != s32Ret, s32Ret, SAMPLE_SVP_ERR_LEVEL_ERROR,
                              "Error,HI_MPI_SVP_NNIE_Forward failed!\n");

    if (bInstant)
    {
        /*Wait NNIE finish*/
        while (HI_ERR_SVP_NNIE_QUERY_TIMEOUT == (s32Ret = HI_MPI_SVP_NNIE_Query(pstNnieParam->astForwardCtrl[pstProcSegIdx->u32SegIdx].enNnieId,
                                                                                hSvpNnieHandle, &bFinish, HI_TRUE)))
        {
            usleep(100);
            SAMPLE_SVP_TRACE(SAMPLE_SVP_ERR_LEVEL_INFO,
                             "HI_MPI_SVP_NNIE_Query Query timeout!\n");
        }
    }

    bFinish = HI_FALSE;
    for (i = 0; i < pstNnieParam->astForwardCtrl[pstProcSegIdx->u32SegIdx].u32DstNum; i++)
    {
        if (SVP_BLOB_TYPE_SEQ_S32 == pstNnieParam->astSegData[pstProcSegIdx->u32SegIdx].astDst[i].enType)
        {
            for (j = 0; j < pstNnieParam->astSegData[pstProcSegIdx->u32SegIdx].astDst[i].u32Num; j++)
            {
                u32TotalStepNum += *((HI_U32 *)(pstNnieParam->astSegData[pstProcSegIdx->u32SegIdx].astDst[i].unShape.stSeq.u64VirAddrStep) + j);
            }
            SAMPLE_COMM_SVP_FlushCache(pstNnieParam->astSegData[pstProcSegIdx->u32SegIdx].astDst[i].u64PhyAddr,
                                       (HI_VOID *)pstNnieParam->astSegData[pstProcSegIdx->u32SegIdx].astDst[i].u64VirAddr,
                                       u32TotalStepNum * pstNnieParam->astSegData[pstProcSegIdx->u32SegIdx].astDst[i].u32Stride);
        }
        else
        {

            SAMPLE_COMM_SVP_FlushCache(pstNnieParam->astSegData[pstProcSegIdx->u32SegIdx].astDst[i].u64PhyAddr,
                                       (HI_VOID *)pstNnieParam->astSegData[pstProcSegIdx->u32SegIdx].astDst[i].u64VirAddr,
                                       pstNnieParam->astSegData[pstProcSegIdx->u32SegIdx].astDst[i].u32Num *
                                           pstNnieParam->astSegData[pstProcSegIdx->u32SegIdx].astDst[i].unShape.stWhc.u32Chn *
                                           pstNnieParam->astSegData[pstProcSegIdx->u32SegIdx].astDst[i].unShape.stWhc.u32Height *
                                           pstNnieParam->astSegData[pstProcSegIdx->u32SegIdx].astDst[i].u32Stride);
        }
    }

    return s32Ret;
}

static HI_S32 NNIE_Get_Result(SAMPLE_SVP_NNIE_PARAM_S *pstNnieParam, Tensor output_tensors[5])
{
    HI_U32 u32SegNum = pstNnieParam->pstModel->u32NetSegNum;
    HI_U32 i = 0, j = 0, k = 0, n = 0;
    HI_U32 u32SegIdx = 0, u32NodeIdx = 0;
    HI_U32 *pu32StepAddr = NULL;
    HI_S32 *ps32ResultAddr = NULL;
    HI_U32 u32Height = 0, u32Width = 0, u32Chn = 0, u32Stride = 0, u32Dim = 0;

    for (u32SegIdx = 0; u32SegIdx < u32SegNum; u32SegIdx++)
    {
        for (u32NodeIdx = 0; u32NodeIdx < pstNnieParam->pstModel->astSeg[u32SegIdx].u16DstNum; u32NodeIdx++)
        {

            Tensor t;
            if (SVP_BLOB_TYPE_SEQ_S32 == pstNnieParam->astSegData[u32SegIdx].astDst[u32NodeIdx].enType)
            {
                u32Dim = pstNnieParam->astSegData[u32SegIdx].astDst[u32NodeIdx].unShape.stSeq.u32Dim;
                u32Stride = pstNnieParam->astSegData[u32SegIdx].astDst[u32NodeIdx].u32Stride;
                pu32StepAddr = (HI_U32 *)(pstNnieParam->astSegData[u32SegIdx].astDst[u32NodeIdx].unShape.stSeq.u64VirAddrStep);
                ps32ResultAddr = (HI_S32 *)(pstNnieParam->astSegData[u32SegIdx].astDst[u32NodeIdx].u64VirAddr);

                for (n = 0; n < pstNnieParam->astSegData[u32SegIdx].astDst[u32NodeIdx].u32Num; n++)
                {
                    for (i = 0; i < *(pu32StepAddr + n); i++)
                    {
                        for (j = 0; j < u32Dim; j++)
                        {
                        }
                        ps32ResultAddr += u32Stride / sizeof(HI_U32);
                    }
                }
            }
            else
            {
                u32Height = pstNnieParam->astSegData[u32SegIdx].astDst[u32NodeIdx].unShape.stWhc.u32Height;
                u32Width = pstNnieParam->astSegData[u32SegIdx].astDst[u32NodeIdx].unShape.stWhc.u32Width;
                u32Chn = pstNnieParam->astSegData[u32SegIdx].astDst[u32NodeIdx].unShape.stWhc.u32Chn;
                u32Stride = pstNnieParam->astSegData[u32SegIdx].astDst[u32NodeIdx].u32Stride;
                ps32ResultAddr = (HI_S32 *)(pstNnieParam->astSegData[u32SegIdx].astDst[u32NodeIdx].u64VirAddr);
                t.n = pstNnieParam->astSegData[u32SegIdx].astDst[u32NodeIdx].u32Num;
                t.channel = u32Chn;
                t.height = u32Height;
                t.width = u32Width;
                t.data = (float *)ps32ResultAddr;
                float *tmp = t.data;

                for (n = 0; n < pstNnieParam->astSegData[u32SegIdx].astDst[u32NodeIdx].u32Num; n++)
                {
                    for (i = 0; i < u32Chn; i++)
                    {
                        for (j = 0; j < u32Height; j++)
                        {
                            for (k = 0; k < u32Width; k++)
                            {
                                *(tmp + k) = (*(ps32ResultAddr + k)) * 1.0f / SAMPLE_SVP_NNIE_QUANT_BASE;
                            }
                            ps32ResultAddr += u32Stride / sizeof(HI_U32);
                            tmp += t.width;
                        }
                    }
                }
                output_tensors[u32NodeIdx] = t;
                ;
            }
        }
    }
    return HI_SUCCESS;
}

void NNIE_Forward_From_Data(const unsigned char *data, SAMPLE_SVP_NNIE_MODEL_S *s_stModel, SAMPLE_SVP_NNIE_PARAM_S *s_stNnieParam, Tensor output_tensors[5])
{
    HI_S32 s32Ret = HI_SUCCESS;

    SAMPLE_SVP_NNIE_INPUT_DATA_INDEX_S stInputDataIdx = {0};
    SAMPLE_SVP_NNIE_PROCESS_SEG_INDEX_S stProcSegIdx = {0};

    stInputDataIdx.u32SegIdx = 0;
    stInputDataIdx.u32NodeIdx = 0;
    s32Ret = NNIE_FillData(data, s_stNnieParam, &stInputDataIdx);
    SAMPLE_SVP_CHECK_EXPR_GOTO(HI_SUCCESS != s32Ret, NNIE_FORWARD_FAIL_0, SAMPLE_SVP_ERR_LEVEL_ERROR,
                               "Error,SAMPLE_SVP_NNIE_FillSrcData failed!\n");

    /*NNIE process(process the 0-th segment)*/
    stProcSegIdx.u32SegIdx = 0;
    s32Ret = NNIE_Forward(s_stNnieParam, &stInputDataIdx, &stProcSegIdx, HI_TRUE);
    SAMPLE_SVP_CHECK_EXPR_GOTO(HI_SUCCESS != s32Ret, NNIE_FORWARD_FAIL_0, SAMPLE_SVP_ERR_LEVEL_ERROR,
                               "Error,SAMPLE_SVP_NNIE_Forward failed!\n");

    s32Ret = NNIE_Get_Result(s_stNnieParam, output_tensors);

    SAMPLE_SVP_CHECK_EXPR_GOTO(HI_SUCCESS != s32Ret, NNIE_FORWARD_FAIL_0, SAMPLE_SVP_ERR_LEVEL_ERROR,
                               "Error,SAMPLE_SVP_NNIE_PrintReportResult failed!\n");

    return;

NNIE_FORWARD_FAIL_0:
    NNIE_Param_Deinit(s_stNnieParam, s_stModel);
    SAMPLE_COMM_SVP_CheckSysExit();
}

void NNIE_Param_Deinit(SAMPLE_SVP_NNIE_PARAM_S *pstNnieParam, SAMPLE_SVP_NNIE_MODEL_S *pstNnieModel)
{

    HI_S32 s32Ret = HI_SUCCESS;
    /*hardware para deinit*/
    if (pstNnieParam != NULL)
    {
        s32Ret = SAMPLE_COMM_SVP_NNIE_ParamDeinit(pstNnieParam);
        SAMPLE_SVP_CHECK_EXPR_TRACE(HI_SUCCESS != s32Ret, SAMPLE_SVP_ERR_LEVEL_ERROR,
                                    "Error,SAMPLE_COMM_SVP_NNIE_ParamDeinit failed!\n");
    }
    /*model deinit*/
    if (pstNnieModel != NULL)
    {
        s32Ret = SAMPLE_COMM_SVP_NNIE_UnloadModel(pstNnieModel);
        SAMPLE_SVP_CHECK_EXPR_TRACE(HI_SUCCESS != s32Ret, SAMPLE_SVP_ERR_LEVEL_ERROR,
                                    "Error,SAMPLE_COMM_SVP_NNIE_UnloadModel failed!\n");
    }
}
