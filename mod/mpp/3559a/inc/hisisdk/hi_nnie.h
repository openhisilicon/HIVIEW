/******************************************************************************

  Copyright (C), 2017-2018, Hisilicon Tech. Co., Ltd.

 ******************************************************************************
  File Name     : hi_nnie.h
  Version       : Initial Draft
  Author        : Hisilicon multimedia software (SVP) group
  Created       : 2017/09/19
  Description   :
  History       :
  1.Date        : 2017/09/19
    Author      :
    Modification: Created file
******************************************************************************/
#ifndef _HI_NNIE_H_
#define _HI_NNIE_H_

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif

#include "hi_comm_svp.h"
//if sdk environment,include hi_common.h,else typedef HI_S32 SVP_NNIE_HANDLE
#include "hi_common.h"

/**************************************SVP_NNIE Error Code***********************************************************************/
typedef enum hiEN_SVP_NNIE_ERR_CODE_E
{
    ERR_SVP_NNIE_SYS_TIMEOUT    = 0x40,   /* SVP_NNIE process timeout */
    ERR_SVP_NNIE_QUERY_TIMEOUT  = 0x41,   /* SVP_NNIE query timeout */
    ERR_SVP_NNIE_CFG_ERR        = 0x42,   /* SVP_NNIE Configure error */
    ERR_SVP_NNIE_OPEN_FILE      = 0x43,   /* SVP NNIE open file error */
    ERR_SVP_NNIE_READ_FILE      = 0x44,   /* SVP NNIE read file error */
    ERR_SVP_NNIE_WRITE_FILE     = 0x45,   /* SVP NNIE write file error */

    ERR_SVP_NNIE_BUTT
}EN_SVP_NNIE_ERR_CODE_E;
/*Invalid device ID*/
#define HI_ERR_SVP_NNIE_INVALID_DEVID     HI_DEF_ERR(HI_ID_SVP_NNIE, EN_ERR_LEVEL_ERROR, EN_ERR_INVALID_DEVID)
/*Invalid channel ID*/
#define HI_ERR_SVP_NNIE_INVALID_CHNID     HI_DEF_ERR(HI_ID_SVP_NNIE, EN_ERR_LEVEL_ERROR, EN_ERR_INVALID_CHNID)
/*At least one parameter is illegal. For example, an illegal enumeration value exists.*/
#define HI_ERR_SVP_NNIE_ILLEGAL_PARAM     HI_DEF_ERR(HI_ID_SVP_NNIE, EN_ERR_LEVEL_ERROR, EN_ERR_ILLEGAL_PARAM)
/*The channel exists.*/
#define HI_ERR_SVP_NNIE_EXIST             HI_DEF_ERR(HI_ID_SVP_NNIE, EN_ERR_LEVEL_ERROR, EN_ERR_EXIST)
/*The UN exists.*/
#define HI_ERR_SVP_NNIE_UNEXIST           HI_DEF_ERR(HI_ID_SVP_NNIE, EN_ERR_LEVEL_ERROR, EN_ERR_UNEXIST)
/*A null point is used.*/
#define HI_ERR_SVP_NNIE_NULL_PTR          HI_DEF_ERR(HI_ID_SVP_NNIE, EN_ERR_LEVEL_ERROR, EN_ERR_NULL_PTR)
/*Try to enable or initialize the system, device, or channel before configuring attributes.*/
#define HI_ERR_SVP_NNIE_NOT_CONFIG        HI_DEF_ERR(HI_ID_SVP_NNIE, EN_ERR_LEVEL_ERROR, EN_ERR_NOT_CONFIG)
/*The operation is not supported currently.*/
#define HI_ERR_SVP_NNIE_NOT_SURPPORT      HI_DEF_ERR(HI_ID_SVP_NNIE, EN_ERR_LEVEL_ERROR, EN_ERR_NOT_SUPPORT)
/*The operation, changing static attributes for example, is not permitted.*/
#define HI_ERR_SVP_NNIE_NOT_PERM          HI_DEF_ERR(HI_ID_SVP_NNIE, EN_ERR_LEVEL_ERROR, EN_ERR_NOT_PERM)
/*A failure caused by the malloc memory occurs.*/
#define HI_ERR_SVP_NNIE_NOMEM             HI_DEF_ERR(HI_ID_SVP_NNIE, EN_ERR_LEVEL_ERROR, EN_ERR_NOMEM)
/*A failure caused by the malloc buffer occurs.*/
#define HI_ERR_SVP_NNIE_NOBUF             HI_DEF_ERR(HI_ID_SVP_NNIE, EN_ERR_LEVEL_ERROR, EN_ERR_NOBUF)
/*The buffer is empty.*/
#define HI_ERR_SVP_NNIE_BUF_EMPTY         HI_DEF_ERR(HI_ID_SVP_NNIE, EN_ERR_LEVEL_ERROR, EN_ERR_BUF_EMPTY)
/*No buffer is provided for storing new data.*/
#define HI_ERR_SVP_NNIE_BUF_FULL          HI_DEF_ERR(HI_ID_SVP_NNIE, EN_ERR_LEVEL_ERROR, EN_ERR_BUF_FULL)
/*The system is not ready because it may be not initialized or loaded.
 *The error code is returned when a device file fails to be opened. */
#define HI_ERR_SVP_NNIE_NOTREADY          HI_DEF_ERR(HI_ID_SVP_NNIE, EN_ERR_LEVEL_ERROR, EN_ERR_SYS_NOTREADY)
/*The source address or target address is incorrect during the operations such as calling copy_from_user or copy_to_user.*/
#define HI_ERR_SVP_NNIE_BADADDR           HI_DEF_ERR(HI_ID_SVP_NNIE, EN_ERR_LEVEL_ERROR, EN_ERR_BADADDR)
/*The resource is busy during the operations such as destroying a VENC channel without deregistering it.*/
#define HI_ERR_SVP_NNIE_BUSY              HI_DEF_ERR(HI_ID_SVP_NNIE, EN_ERR_LEVEL_ERROR, EN_ERR_BUSY)
/*SVP_NNIE process timeout*/
#define HI_ERR_SVP_NNIE_SYS_TIMEOUT       HI_DEF_ERR(HI_ID_SVP_NNIE, EN_ERR_LEVEL_ERROR, ERR_SVP_NNIE_SYS_TIMEOUT)
/*SVP_NNIE query timeout*/
#define HI_ERR_SVP_NNIE_QUERY_TIMEOUT     HI_DEF_ERR(HI_ID_SVP_NNIE, EN_ERR_LEVEL_ERROR, ERR_SVP_NNIE_QUERY_TIMEOUT)
/*SVP_NNIE configure error*/
#define HI_ERR_SVP_NNIE_CFG_ERR           HI_DEF_ERR(HI_ID_SVP_NNIE, EN_ERR_LEVEL_ERROR, ERR_SVP_NNIE_CFG_ERR)
#define HI_ERR_SVP_NNIE_OPEN_FILE         HI_DEF_ERR(HI_ID_SVP_NNIE, EN_ERR_LEVEL_ERROR, ERR_SVP_NNIE_OPEN_FILE)
#define HI_ERR_SVP_NNIE_READ_FILE         HI_DEF_ERR(HI_ID_SVP_NNIE, EN_ERR_LEVEL_ERROR, ERR_SVP_NNIE_READ_FILE)
#define HI_ERR_SVP_NNIE_WRITE_FILE        HI_DEF_ERR(HI_ID_SVP_NNIE, EN_ERR_LEVEL_ERROR, ERR_SVP_NNIE_WRITE_FILE)

/*macro*/
#define SVP_NNIE_MAX_NET_SEG_NUM            8  /*NNIE max segment num that the net being cut into*/
#define SVP_NNIE_MAX_INPUT_NUM              16 /*NNIE max input num in each seg*/
#define SVP_NNIE_MAX_OUTPUT_NUM             16 /*NNIE max output num in each seg*/
#define SVP_NNIE_MAX_ROI_LAYER_NUM_OF_SEG   2  /*NNIE max roi layer num in each seg*/
#define SVP_NNIE_MAX_ROI_LAYER_NUM          4  /*NNIE max roi layer num*/
#define SVP_NNIE_NODE_NAME_LEN              32 /*NNIE node name length*/

/*NNIE core id*/
typedef enum hiSVP_NNIE_ID_E
{
    SVP_NNIE_ID_0       = 0x0,
    SVP_NNIE_ID_1       = 0x1,

    SVP_NNIE_ID_BUTT
}SVP_NNIE_ID_E;

/*Run Mode*/
typedef enum hiSVP_NNIE_RUN_MODE_E
{
    SVP_NNIE_RUN_MODE_CHIP      = 0x0,  /* on SOC chip running */
    SVP_NNIE_RUN_MODE_FUNC_SIM  = 0x1,  /* functional simultaion */

    SVP_NNIE_RUN_MODE_BUTT
}SVP_NNIE_RUN_MODE_E;

/*Network type*/
typedef enum hiSVP_NNIE_NET_TYPE_E
{
    SVP_NNIE_NET_TYPE_CNN  = 0x0, /* Non-ROI input cnn net */
    SVP_NNIE_NET_TYPE_ROI  = 0x1, /* With ROI input cnn net*/
    SVP_NNIE_NET_TYPE_RECURRENT = 0x2, /* RNN or LSTM net */

    SVP_NNIE_NET_TYPE_BUTT
}SVP_NNIE_NET_TYPE_E;

/*Node information*/
typedef struct hiSVP_NNIE_NODE_S
{
    SVP_BLOB_TYPE_E  enType;
    union
    {
        struct
        {
            HI_U32 u32Width;
            HI_U32 u32Height;
            HI_U32 u32Chn;
        }stWhc;
        HI_U32 u32Dim;
    }unShape;
    HI_U32 u32NodeId;
	HI_CHAR szName[SVP_NNIE_NODE_NAME_LEN];/*Report layer bottom name or data layer bottom name*/
}SVP_NNIE_NODE_S;

/*Roi/Psroi Pooling type*/
typedef enum hiSVP_NNIE_ROIPOOL_TYPE_E
{
    SVP_NNIE_ROIPOOL_TYPE_NORMAL = 0x0,     /* Roipooling*/
    SVP_NNIE_ROIPOOL_TYPE_PS     = 0x1,     /* Position-Sensitive roipooling */

    SVP_NNIE_ROIPOOL_TYPE_BUTT
}SVP_NNIE_ROIPOOL_TYPE_E;

/*RoiPooling information*/
typedef struct hiSVP_NNIE_ROIPOOL_INFO_S
{
    HI_U32 u32SrcHeight;                   /*ROI/PSROI Pooling  src height*/
    HI_U32 u32SrcWidth;                    /*ROI/PSROI Pooling src width*/
    HI_U32 u32SrcMapNum;                   /*Input featrue map channel*/

    HI_U32 u32DstHeight;                   /*ROI/PSROI Pooling dst height*/
    HI_U32 u32DstWidth;                    /*ROI/PSROI Pooling dst width*/
    HI_U32 u32DstMapNum;                   /*Output featrue map channel*/

    HI_U32 u32DinWidthRamOffset;
    HI_U32 u32DinHeightRamOffset;
    HI_U32 u32TotalDinWidthRamOffset;
    HI_U32 u32TotalDinHeightRamOffset;

    HI_U32 u32BlockMaxHeight;              /*Block max height*/
    HI_U32 u32BlockNum;                    /*Block number*/
    HI_U32 u32BboxMaxNum;                  /*Bbox max number*/
    HI_U32 u32SpatialScale;

    HI_BOOL bNormPPMode;                   /*whether use PingPang mode*/
    HI_BOOL bTotalPPMode;                  /*whether use PingPang mode*/
    HI_BOOL bHighPrec;                     /*whether use high precision mode*/
    SVP_NNIE_ROIPOOL_TYPE_E enRoiPoolType; /*ROIPooling or PSROIPooling*/
	HI_CHAR szName[SVP_NNIE_NODE_NAME_LEN];/*Report layer bottom name or data layer bottom name*/
}SVP_NNIE_ROIPOOL_INFO_S;

/***************************************************************/
/*Segment information*/
typedef struct hiSVP_NNIE_SEG_S
{
    SVP_NNIE_NET_TYPE_E enNetType;
    HI_U16              u16SrcNum;
    HI_U16              u16DstNum;
    HI_U16              u16RoiPoolNum;
    HI_U16              u16MaxStep;

    HI_U32              u32InstOffset;
    HI_U32              u32InstLen;

    SVP_NNIE_NODE_S     astSrcNode[SVP_NNIE_MAX_INPUT_NUM];
    SVP_NNIE_NODE_S     astDstNode[SVP_NNIE_MAX_OUTPUT_NUM];
    HI_U32              au32RoiIdx[SVP_NNIE_MAX_ROI_LAYER_NUM_OF_SEG]; /*Roipooling info index*/
}SVP_NNIE_SEG_S;

/*NNIE model*/
typedef struct hiSVP_NNIE_MODEL_S
{
    SVP_NNIE_RUN_MODE_E     enRunMode;

    HI_U32                  u32TmpBufSize; /*temp buffer size*/
    HI_U32                  u32NetSegNum;
    SVP_NNIE_SEG_S          astSeg[SVP_NNIE_MAX_NET_SEG_NUM];
    SVP_NNIE_ROIPOOL_INFO_S astRoiInfo[SVP_NNIE_MAX_ROI_LAYER_NUM]; /*ROIPooling info*/

    SVP_MEM_INFO_S          stBase;
}SVP_NNIE_MODEL_S;


typedef struct hiSVP_NNIE_FORWARD_CTRL_S
{
    HI_U32              u32SrcNum;      /* input node num, [1, 16] */
    HI_U32              u32DstNum;      /* output node num, [1, 16]*/
    HI_U32              u32NetSegId;    /* net segment index running on NNIE */
    SVP_NNIE_ID_E       enNnieId;       /* device target which running the seg*/
    SVP_MEM_INFO_S      stTmpBuf;       /* auxiliary temp mem */
    SVP_MEM_INFO_S      stTskBuf;       /* auxiliary task mem */
}SVP_NNIE_FORWARD_CTRL_S;


typedef struct hiSVP_NNIE_FORWARD_WITHBBOX_CTRL_S
{
    HI_U32              u32SrcNum;      /* input node num, [1, 16] */
    HI_U32              u32DstNum;      /* output node num, [1, 16]*/
    HI_U32              u32ProposalNum; /* elment num of  roi array */
    HI_U32              u32NetSegId;    /* net segment index running on NNIE */
    SVP_NNIE_ID_E       enNnieId;       /* device target which running the seg*/
    SVP_MEM_INFO_S      stTmpBuf;       /* auxiliary temp mem */
    SVP_MEM_INFO_S      stTskBuf;       /* auxiliary task mem */
}SVP_NNIE_FORWARD_WITHBBOX_CTRL_S;


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif
#endif/*_HI_NNIE_H_*/
