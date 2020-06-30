/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2018-2019. All rights reserved.
 * Description:
 * Author:
 * Create: 2018-05-19
 */

#ifndef __HI_RUNTIME_COMM_H__
#define __HI_RUNTIME_COMM_H__
#include "hi_type.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

#define MAX_NAME_LEN 64                 /*Model max name */
#define FULL_NAME_LEN 130

typedef HI_VOID* HI_MW_PTR;                /*Handle define for model and tasks */

#ifndef IN
#define IN
#endif

#ifndef OUT
#define OUT
#endif

#ifndef INOUT
#define INOUT
#endif

/*Blob type */
typedef enum hiRUNTIME_BLOB_TYPE_E
{
    HI_RUNTIME_BLOB_TYPE_S32       =  0x0,
    HI_RUNTIME_BLOB_TYPE_U8        =  0x1,
    HI_RUNTIME_BLOB_TYPE_YVU420SP  =  0x2,
    HI_RUNTIME_BLOB_TYPE_YVU422SP  =  0x3,
    HI_RUNTIME_BLOB_TYPE_VEC_S32   =  0x4,
    HI_RUNTIME_BLOB_TYPE_SEQ_S32   =  0x5,
    HI_RUNTIME_BLOB_TYPE_BUTT
} HI_RUNTIME_BLOB_TYPE_E;

typedef enum hiRUNTIME_GROUP_PRIORITY_E
{
    HI_RUNTIME_GROUP_PRIORITY_HIGHEST = 0x0,
    HI_RUNTIME_GROUP_PRIORITY_HIGH,
    HI_RUNTIME_GROUP_PRIORITY_MEDIUM,
    HI_RUNTIME_GROUP_PRIORITY_LOW,
    HI_RUNTIME_GROUP_PRIORITY_LOWEST,
    HI_RUNTIME_GROUP_PRIORITY_BUTT
} HI_RUNTIME_GROUP_PRIORITY_E;

/****************************** Blob struct ******************************
In Caffe, the blob contain shape info as the following order:
Image\FeatureMap:               N       C       H       W
RNN\LSTM(Recurrent) vector:     T       N       D

The relationship of the following blob struct with Caffe blob is as follows:
Image\FeatureMap:               Num    Chn    Height   With
RNN\LSTM(SEQ_S32) vector:       Step   Num     Dim
The stride, which measuring unit is byte, is always algined by the width or
dim direction.
**************************************************************************/
typedef struct hiRUNTIME_BLOB_S
{
    HI_RUNTIME_BLOB_TYPE_E enBlobType;  /*Blob type*/
    HI_U32 u32Stride;                /*Stride, a line bytes num*/
    HI_U64 u64VirAddr;               /*virtual addr*/
    HI_U64 u64PhyAddr;               /*physical addr*/

    HI_U32 u32Num;                   /*N: frame num or sequence num,correspond to caffe blob's n*/
    union
    {
        struct
        {
            HI_U32 u32Width;         /*W: frame width, correspond to caffe blob's w*/
            HI_U32 u32Height;        /*H: frame height, correspond to caffe blob's h*/
            HI_U32 u32Chn;           /*C: frame channel,correspond to caffe blob's c*/
        } stWhc;
        struct
        {
            HI_U32 u32Dim;           /*D: vecotr dimension*/
            HI_U64 u64VirAddrStep;   /*T: virtual adress of time steps array in each sequence*/
        } stSeq;
    } unShape;
} HI_RUNTIME_BLOB_S, *HI_RUNTIME_BLOB_PTR;

typedef struct hiRUNTIME_SRC_BLOB_ARRAY_S
{
    HI_U32 u32BlobNum;
    HI_RUNTIME_BLOB_S *pstBlobs;
} HI_RUNTIME_BLOB_ARRAY_S, *HI_RUNTIME_BLOB_ARRAY_PTR;

typedef HI_RUNTIME_BLOB_ARRAY_S HI_RUNTIME_SRC_BLOB_ARRAY_S;
typedef HI_RUNTIME_BLOB_ARRAY_S HI_RUNTIME_DST_BLOB_ARRAY_S;
typedef HI_RUNTIME_BLOB_ARRAY_PTR HI_RUNTIME_SRC_BLOB_ARRAY_PTR;
typedef HI_RUNTIME_BLOB_ARRAY_PTR HI_RUNTIME_DST_BLOB_ARRAY_PTR;

typedef struct hiRUNTIME_GROUP_BLOB_S
{
    HI_RUNTIME_BLOB_PTR pstBlob;
    HI_CHAR acOwnerName[MAX_NAME_LEN+1];
    HI_CHAR acBlobName[MAX_NAME_LEN+1];
} HI_RUNTIME_GROUP_BLOB_S, *HI_RUNTIME_GROUP_BLOB_PTR;

typedef HI_RUNTIME_GROUP_BLOB_S HI_RUNTIME_GROUP_SRC_BLOB_S;
typedef HI_RUNTIME_GROUP_BLOB_S HI_RUNTIME_GROUP_DST_BLOB_S;

/*Mem information*/
typedef struct hiRUNTIME_MEM_S
{
    HI_U64  u64PhyAddr; /* RW;The physical address of the memory */
    HI_U64  u64VirAddr; /* RW;The virtual address of the memory */
    HI_U32  u32Size;    /* RW;The size of memory */
} HI_RUNTIME_MEM_S, *HI_RUNTIME_MEM_PTR;

typedef HI_S32 (*HI_RUNTIME_AllocMem)(INOUT HI_RUNTIME_MEM_S *pstMemInfo);
typedef HI_S32 (*HI_RUNTIME_FlushCache)(HI_RUNTIME_MEM_S *pstMemInfo);
typedef HI_S32 (*HI_RUNTIME_FreeMem)(HI_RUNTIME_MEM_S *pstMemInfo);

typedef struct hiRUNTIME_MEM_CTRL_S
{
    HI_RUNTIME_AllocMem allocMem;
    HI_RUNTIME_FlushCache flushMem;
    HI_RUNTIME_FreeMem freeMem;
} HI_RUNTIME_MEM_CTRL_S;

typedef struct hiRUNTIME_WK_MEM_S
{
    HI_CHAR acModelName[MAX_NAME_LEN+1];
    HI_RUNTIME_MEM_S stWKMemory;
} HI_RUNTIME_WK_INFO_S;

typedef struct hiRUNTIME_WK_MEM_ARRAY_S
{
    HI_U32 u32WKNum;
    HI_RUNTIME_WK_INFO_S *pstAttrs;
} HI_RUNTIME_WK_INFO_ARRAY_S, *HI_RUNTIME_WK_INFO_ARRAY_PTR;

typedef struct hiRUNTIME_COP_ATTR_S
{
    HI_CHAR acModelName[MAX_NAME_LEN+1];
    HI_CHAR acCopName[MAX_NAME_LEN+1];
    HI_U32 u32ConstParamSize;
    HI_VOID* pConstParam;
} HI_RUNTIME_COP_ATTR_S, *HI_RUNTIME_COP_ATTR_PTR;

typedef struct hiRUNTIME_COP_ATTR_ARRAY_S
{
    HI_U32 u32CopNum;
    HI_RUNTIME_COP_ATTR_S *pstAttrs;
} HI_RUNTIME_COP_ATTR_ARRAY_S, *HI_RUNTIME_COP_ATTR_ARRAY_PTR;

/* Forward callback event*/
typedef enum hiRUNTIME_FORWARD_STATUS_CALLBACK_E
{
    HI_RUNTIME_FORWARD_STATUS_SUCC = 0x0,
    HI_RUNTIME_FORWARD_STATUS_FAIL,
    HI_RUNTIME_FORWARD_STATUS_ABORT,
    HI_RUNTIME_FORWARD_STATUS_BUTT
} HI_RUNTIME_FORWARD_STATUS_CALLBACK_E;

typedef HI_S32 (*HI_RUNTIME_Connector_Compute)(HI_RUNTIME_SRC_BLOB_ARRAY_S* pstConnectorSrc, HI_RUNTIME_DST_BLOB_ARRAY_S* pstConnectorDst, HI_U64 u64SrcId, HI_VOID* pParam);

typedef struct hiRUNTIME_CONNECTOR_ATTR_S
{
    HI_CHAR acName[MAX_NAME_LEN+1];
    HI_RUNTIME_Connector_Compute pConnectorFun;
    HI_VOID *pParam;
} HI_RUNTIME_CONNECTOR_ATTR_S;

typedef struct hiRUNTIME_CONNECTOR_ATTR_ARRAY_S
{
    HI_U32 u32ConnectorNum;
    HI_RUNTIME_CONNECTOR_ATTR_S *pstAttrs;
} HI_RUNTIME_CONNECTOR_ATTR_ARRAY_S, *HI_RUNTIME_CONNECTOR_ATTR_ARRAY_PTR;

typedef struct hiRUNTIME_GROUP_INFO_S
{
    HI_RUNTIME_WK_INFO_ARRAY_S stWKsInfo;
    HI_RUNTIME_COP_ATTR_ARRAY_S stCopsAttr;
    HI_RUNTIME_CONNECTOR_ATTR_ARRAY_S stConnectorsAttr;
} HI_RUNTIME_GROUP_INFO_S, *HI_RUNTIME_GROUP_INFO_PTR;

/*handle info after LoadModelGroup*/
typedef HI_VOID* HI_RUNTIME_GROUP_HANDLE;

/*the src blobs of group, all model inputs are inside*/
typedef struct hiRUNTIME_GROUP_BLOB_ARRAY_S
{
    HI_U32 u32BlobNum;
    HI_RUNTIME_GROUP_BLOB_S* pstBlobs;
} HI_RUNTIME_GROUP_BLOB_ARRAY_S, *HI_RUNTIME_GROUP_BLOB_ARRAY_PTR;

typedef HI_RUNTIME_GROUP_BLOB_ARRAY_S HI_RUNTIME_GROUP_SRC_BLOB_ARRAY_S;
typedef HI_RUNTIME_GROUP_BLOB_ARRAY_S HI_RUNTIME_GROUP_DST_BLOB_ARRAY_S;
typedef HI_RUNTIME_GROUP_BLOB_ARRAY_S* HI_RUNTIME_GROUP_SRC_BLOB_ARRAY_PTR;
typedef HI_RUNTIME_GROUP_BLOB_ARRAY_S* HI_RUNTIME_GROUP_DST_BLOB_ARRAY_PTR;

/**
\brief forward result calback function.
\attention \n
none.
\param[out] enEvent:  Result message of forward.
\param[out] pstGroup: Group Handle
\param[out] u64SrcId: Frame ID
\param[out] pstDst:   Result blob
\see \n
none.
*/
typedef HI_S32 (*HI_RUNTIME_Forward_Callback)(HI_RUNTIME_FORWARD_STATUS_CALLBACK_E enEvent, HI_RUNTIME_GROUP_HANDLE hGroupHandle, HI_U64 u64SrcId, HI_RUNTIME_GROUP_DST_BLOB_ARRAY_PTR pstDst);

//errno message
enum {
    HI_ERR_SVP_RUNTIME_BASE = (HI_S32)(0xFF000F00),
    HI_ERR_SVP_RUNTIME_ILLEGAL_STATE,
    HI_ERR_SVP_RUNTIME_MODEL_NOLOAD,
    HI_ERR_SVP_RUNTIME_NULL_PTR,
    HI_ERR_SVP_RUNTIME_INVALID_PARAM,
    HI_ERR_SVP_RUNTIME_SDK_ERROR,
    HI_ERR_SVP_RUNTIME_SDK_NOMEM,
};

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* __HI_RUNTIME_COMM_H__ */
