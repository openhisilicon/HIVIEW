/*
 * CopyRightight (C) Hisilicon Technologies Co., Ltd. 2019. All rights reserved.
 * Description:
 * Author:
 * Create: 2019-05-19
 */

#ifndef __HI_RUNTIME_COMM_H__
#define __HI_RUNTIME_COMM_H__
#include "hi_type.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

#define MAX_NAME_LEN 64                 /* Model max name */
#define FULL_NAME_LEN 130
typedef hi_void* HiMwPtr;               /* Handle define for model and tasks */

#ifndef IN
#define IN
#endif

#ifndef OUT
#define OUT
#endif

#ifndef INOUT
#define INOUT
#endif

/* Blob type */
typedef enum {
    HI_RUNTIME_BLOB_TYPE_S20Q12            =  0x0,
    HI_RUNTIME_BLOB_TYPE_U8                =  0x1,
    HI_RUNTIME_BLOB_TYPE_YVU420SP          =  0x2,
    HI_RUNTIME_BLOB_TYPE_YVU422SP          =  0x3,
    HI_RUNTIME_BLOB_TYPE_VEC_S20Q12        =  0x4,
    HI_RUNTIME_BLOB_TYPE_SEQ_S20Q12        =  0x5,
    HI_RUNTIME_BLOB_TYPE_BBOX_S20Q12       =  0x6,
    HI_RUNTIME_BLOB_TYPE_BSI_SQ32          =  0x7,
    HI_RUNTIME_BLOB_TYPE_S12Q20            =  0x8,
    HI_RUNTIME_BLOB_TYPE_VEC_S12Q20        =  0x9,
    HI_RUNTIME_BLOB_TYPE_S32               =  0xa,
    HI_RUNTIME_BLOB_TYPE_U32               =  0xb, // not support
    HI_RUNTIME_BLOB_TYPE_FP32              =  0xc, // not support
    HI_RUNTIME_BLOB_TYPE_FP16              =  0xd, // not support
    HI_RUNTIME_BLOB_TYPE_U16               =  0xe,
    HI_RUNTIME_BLOB_TYPE_BUTT
} HiRTBlobType;

typedef enum {
    HI_RUNTIME_GROUP_PRIORITY_HIGHEST = 0x0,
    HI_RUNTIME_GROUP_PRIORITY_HIGH,
    HI_RUNTIME_GROUP_PRIORITY_MEDIUM,
    HI_RUNTIME_GROUP_PRIORITY_LOW,
    HI_RUNTIME_GROUP_PRIORITY_LOWEST,
    HI_RUNTIME_GROUP_PRIORITY_BUTT
} HiRTGroupPriority;

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
typedef struct {
    HiRTBlobType blobType;        /* Blob type */
    hi_u32 stride;                /* Stride, a line bytes num */
    hi_u64 virtAddr;               /* virtual addr */
    hi_u64 physAddr;               /* physical addr */
    hi_u32 num;                   /* N: frame num or sequence num, correspond to caffe blob's n */
    union {
        struct {
            hi_u32 width;         /* W: frame width, correspond to caffe blob's w */
            hi_u32 height;        /* H: frame height, correspond to caffe blob's h */
            hi_u32 chn;           /* C: frame channel, correspond to caffe blob's c */
        } whc;                    /* AUTO:HiRTBlobType:other; */
        struct {
            hi_u32 dim;           /* D: vecotr dimension */
            hi_u64 virtAddrStep;   /* T: virtual adress of time steps array in each sequence */
        } seq;                    /* AUTO:HiRTBlobType:HI_RUNTIME_BLOB_TYPE_SEQ_S20Q12; */
    } shape;
} HiRTBlob, *HiRTBlobPtr;

typedef struct {
    hi_u32 blobNum;
    HiRTBlob *blobs;
} HiRTBlobArray, *HiRTBlobArrayPtr;

typedef HiRTBlobArray HiRTSrcBlobArray;
typedef HiRTBlobArray HiRTDstBlobArray;
typedef HiRTBlobArrayPtr HiRTSrcBlobArrayPtr;
typedef HiRTBlobArrayPtr HiRTDstBlobArrayPtr;

typedef struct {
    HiRTBlobPtr blob;
    hi_char ownerName[MAX_NAME_LEN + 1];
    hi_char blobName[MAX_NAME_LEN + 1];
} HiRTGroupBlob, *HiRTGroupBlobPtr;

typedef HiRTGroupBlob HiRTGroupSrcBlob;
typedef HiRTGroupBlob HiRTGroupDstBlob;

/* Mem information */
typedef struct {
    hi_u64  physAddr; /* RW;The physical address of the memory */
    hi_u64  virtAddr; /* RW;The virtual address of the memory */
    hi_u32  size;    /* RW;The size of memory */
} HiRTMem, *HiRTMemPtr;

typedef hi_s32 (*HiRTAllocMem)(INOUT HiRTMem *memInfo);
typedef hi_s32 (*HiRTFlushCache)(HiRTMem *memInfo);
typedef hi_s32 (*HiRTFreeMem)(HiRTMem *memInfo);

typedef struct {
    HiRTAllocMem allocMem;
    HiRTFlushCache flushMem;
    HiRTFreeMem freeMem;
} HiRTMemCtrl;

typedef struct {
    hi_s32 xLeft;
    hi_s32 yLeft;
    hi_s32 xRight;
    hi_s32 yRight;
} HiRTPriorboxCoord;

typedef struct {
    HiRTPriorboxCoord priorBox;
    HiRTPriorboxCoord var;
} HiRTPriorBoxInfo, *HiRTPriorBoxInfoPtr;

typedef struct {
    hi_u32 priorBoxNum;
    hi_u32 varNum;
} HiRTPriorBoxNum, *HiRTPriorBoxNumPtr;

typedef hi_s32(*HiRTGenerateBaseAnchorCallback)(HiRTPriorBoxNum priorBoxNumAddr[], hi_u32 priorBoxNum,
    HiRTPriorBoxInfo priorBoxInfo[], hi_u32 *priorBoxBlobNum);

typedef struct {
    hi_u32 minWidth;  /* min width */
    hi_u32 minHeight; /* min height */
    hi_u32 nmsThreshold;    /* nms thresh */
    hi_u32 scoreThreshold;  /* score thresh */
} HiRTThreshold;

typedef struct {
    hi_char modelName[MAX_NAME_LEN + 1];
    HiRTMem wkMemory;
    HiRTThreshold proposalThreshold;
    HiRTGenerateBaseAnchorCallback generateBaseAnchorCallback;
} HiRTWkInfo;

typedef struct {
    hi_u32 wkNum;
    HiRTWkInfo *attrs;
} HiRTWkInfoArray, *HiRTWkInfoArrayPtr;

typedef struct {
    hi_char modelName[MAX_NAME_LEN + 1];
    hi_char copName[MAX_NAME_LEN + 1];
    hi_u32 constParamSize;
    hi_void* constParam;
} HiRTCopAttr, *HiRTCopAttrPtr;

typedef struct {
    hi_u32 copNum;
    HiRTCopAttr *attrs;
} HiRTCopAttrArray, *HiRTCopAttrArrayPtr;

/* Forward callback event */
typedef enum {
    HI_RUNTIME_FORWARD_STATUS_SUCC = 0x0,
    HI_RUNTIME_FORWARD_STATUS_FAIL,
    HI_RUNTIME_FORWARD_STATUS_ABORT,
    HI_RUNTIME_FORWARD_STATUS_BUTT
} HiRTForwardStatusCallback;

typedef hi_s32 (*HiRTConnectorCompute)(HiRTSrcBlobArray* connectorSrc, HiRTDstBlobArray* connectorDst,
    hi_u64 srcId, hi_void* param);

typedef struct {
    hi_char name[MAX_NAME_LEN + 1];
    HiRTConnectorCompute connectorFunc;
    hi_void *param;
} HiRTConnectorAttr;

typedef struct {
    hi_u32 connectorNum;
    HiRTConnectorAttr *attrs;
} HiRTConnectorAttrArray, *HiRTConnectorAttrArrayPtr;

typedef struct {
    HiRTWkInfoArray wksInfo;
    HiRTCopAttrArray copsAttr;
    HiRTConnectorAttrArray connectorsAttr;
} HiRTGroupInfo, *HiRTGroupInfoPtr;

/* handle info after LoadModelGroup */
typedef hi_void* HiRTGroupHandle;

/* the src blobs of group, all model inputs are inside */
typedef struct {
    hi_u32 blobNum;
    HiRTGroupBlob* blobs;
} HiRTGroupBlobArray, *HiRTGroupBlobArrayPtr;

typedef HiRTGroupBlobArray HiRTGroupSrcBlobArray;
typedef HiRTGroupBlobArray HiRTGroupDstBlobArray;
typedef HiRTGroupBlobArray* HiRTGroupSrcBlobArrayPtr;
typedef HiRTGroupBlobArray* HiRTGroupDstBlobArrayPtr;

/**
\brief forward result calback function.
\attention
none.
\param[out] enEvent:  Result message of forward.
\param[out] pstGroup: Group Handle
\param[out] u64SrcId: Frame ID
\param[out] pstDst:   Result blob
\see \n
none.
*/
typedef hi_s32 (*HiRTForwardCallback)(HiRTForwardStatusCallback event, HiRTGroupHandle groupHandle, hi_u64 srcId,
    HiRTGroupDstBlobArrayPtr dst);

// errno message
enum {
    HI_ERR_SVP_RUNTIME_BASE = (hi_s32)(0xFF000F00),
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
