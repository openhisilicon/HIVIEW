
/******************************************************************************
Copyright (C), 2016, Hisilicon Tech. Co., Ltd.
******************************************************************************
File Name     : hi_comm_vdec.h
Version       : Initial Draft
Author        : Hisilicon multimedia software group
Created       : 2016/07/15
Last Modified :
Description   : Common defination Of video decode
Function List :
******************************************************************************/


#ifndef  __HI_COMM_VDEC_H__
#define  __HI_COMM_VDEC_H__
#include "hi_type.h"
#include "hi_common.h"
#include "hi_errno.h"
#include "hi_comm_video.h"
#include "hi_comm_vb.h"
#include "hi_defines.h"

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* End of #ifdef __cplusplus */



#define HI_IO_BLOCK   HI_TRUE
#define HI_IO_NOBLOCK HI_FALSE


typedef enum hiVIDEO_MODE_E
{
    VIDEO_MODE_STREAM = 0, /* send by stream */
    VIDEO_MODE_FRAME     , /* send by frame  */
    VIDEO_MODE_COMPAT    , /* One frame supports multiple packets sending. The current frame is considered to end when bEndOfFrame is equal to HI_TRUE */
    VIDEO_MODE_BUTT
}VIDEO_MODE_E;


typedef struct hiVDEC_ATTR_VIDEO_S
{
    HI_U32       u32RefFrameNum;       /* RW, Range: [0, 16]; reference frame num. */
    HI_BOOL      bTemporalMvpEnable;   /* RW; specifies whether temporal motion vector predictors can be used for inter prediction */
    HI_U32       u32TmvBufSize;        /* RW; tmv buffer size(Byte) */
}VDEC_ATTR_VIDEO_S;


typedef struct hiVDEC_CHN_ATTR_S
{
    PAYLOAD_TYPE_E enType;              /* RW; video type to be decoded   */
    VIDEO_MODE_E   enMode;              /* RW; send by stream or by frame */
    HI_U32         u32PicWidth;         /* RW; max pic width */
    HI_U32         u32PicHeight;        /* RW; max pic height */
    HI_U32         u32StreamBufSize;    /* RW; stream buffer size(Byte) */
    HI_U32         u32FrameBufSize;     /* RW; frame buffer size(Byte) */
    HI_U32         u32FrameBufCnt;
    union
    {
        VDEC_ATTR_VIDEO_S stVdecVideoAttr;    /* structure with video ( h264/h265) */
    };
}VDEC_CHN_ATTR_S;


typedef struct hiVDEC_STREAM_S
{
    HI_U32  u32Len;              /* W; stream len */
    HI_U64  u64PTS;              /* W; time stamp */
    HI_BOOL bEndOfFrame;         /* W; is the end of a frame */
    HI_BOOL bEndOfStream;        /* W; is the end of all stream */
    HI_BOOL bDisplay;            /* W; is the current frame displayed. only valid by VIDEO_MODE_FRAME */
    HI_U8* ATTRIBUTE pu8Addr;    /* W; stream address */
}VDEC_STREAM_S;

typedef struct hiVDEC_USERDATA_S
{
    HI_U64           u64PhyAddr;  /* R; userdata data phy address */
    HI_U32           u32Len;        /* R; userdata data len */
    HI_BOOL          bValid;        /* R; is valid? */
    HI_U8* ATTRIBUTE pu8Addr;       /* R; userdata data vir address */
}VDEC_USERDATA_S;


typedef struct hi_VDEC_DECODE_ERROR_S
{
    HI_S32 s32FormatErr;              /* R; format error. eg: do not support filed */
    HI_S32 s32PicSizeErrSet;          /* R; picture width or height is larger than chnnel width or height*/
    HI_S32 s32StreamUnsprt;           /* R; unsupport the stream specification */
    HI_S32 s32PackErr;                /* R; stream package error */
    HI_S32 s32PrtclNumErrSet;         /* R; protocol num is not enough. eg: slice, pps, sps */
    HI_S32 s32RefErrSet;              /* R; refrence num is not enough */
    HI_S32 s32PicBufSizeErrSet;       /* R; the buffer size of picture is not enough */
    HI_S32 s32StreamSizeOver;         /* R; the stream size is too big and and force discard stream */
    HI_S32 s32VdecStreamNotRelease;   /* R; the stream not released for too long time */
}VDEC_DECODE_ERROR_S;


typedef struct hiVDEC_CHN_STATUS_S
{
    PAYLOAD_TYPE_E enType;               /* R; video type to be decoded */
    HI_U32  u32LeftStreamBytes;          /* R; left stream bytes waiting for decode */
    HI_U32  u32LeftStreamFrames;         /* R; left frames waiting for decode,only valid for VIDEO_MODE_FRAME*/
    HI_U32  u32LeftPics;                 /* R; pics waiting for output */
    HI_BOOL bStartRecvStream;            /* R; had started recv stream? */
    HI_U32  u32RecvStreamFrames;         /* R; how many frames of stream has been received. valid when send by frame. */
    HI_U32  u32DecodeStreamFrames;       /* R; how many frames of stream has been decoded. valid when send by frame. */
    VDEC_DECODE_ERROR_S stVdecDecErr;    /* R; information about decode error */
}VDEC_CHN_STATUS_S;



typedef enum hiVIDEO_DEC_MODE_E
{
    VIDEO_DEC_MODE_IPB = 0,
    VIDEO_DEC_MODE_IP,
    VIDEO_DEC_MODE_I,
    VIDEO_DEC_MODE_BUTT
}VIDEO_DEC_MODE_E;


typedef enum hiVIDEO_OUTPUT_ORDER_E
{
    VIDEO_OUTPUT_ORDER_DISP = 0,
    VIDEO_OUTPUT_ORDER_DEC,
    VIDEO_OUTPUT_ORDER_BUTT
}VIDEO_OUTPUT_ORDER_E;



typedef struct hiVDEC_PARAM_VIDEO_S
{
    HI_S32               s32ErrThreshold;      /* RW, Range: [0, 100]; threshold for stream error process, 0: discard with any error, 100 : keep data with any error */
    VIDEO_DEC_MODE_E     enDecMode;            /* RW; decode mode , 0: deocde IPB frames, 1: only decode I frame & P frame , 2: only decode I frame */
    VIDEO_OUTPUT_ORDER_E enOutputOrder;        /* RW; frames output order ,0: the same with display order , 1: the same width decoder order */
    COMPRESS_MODE_E      enCompressMode;       /* RW; compress mode */
    VIDEO_FORMAT_E       enVideoFormat;        /* RW; video format */
}VDEC_PARAM_VIDEO_S;


typedef struct hiVDEC_PARAM_PICTURE_S
{
    PIXEL_FORMAT_E    enPixelFormat;    /* RW; out put pixel format */
    HI_U32            u32Alpha;         /* RW, Range: [0, 255]; value 0 is transparent.
                                                      [0 ,127]   is deemed to transparent when enPixelFormat is ARGB1555 or ABGR1555
                                                      [128 ,256] is deemed to non-transparent when enPixelFormat is ARGB1555 or ABGR1555 */
}VDEC_PARAM_PICTURE_S;


typedef struct hiVDEC_CHN_PARAM_S
{
    PAYLOAD_TYPE_E enType;                       /* RW; video type to be decoded   */
    HI_U32  u32DisplayFrameNum;                  /* RW, Range: [0, 16]; display frame num */
    union
    {
        VDEC_PARAM_VIDEO_S stVdecVideoParam;     /* structure with video ( h265/h264) */
        VDEC_PARAM_PICTURE_S stVdecPictureParam; /* structure with picture (jpeg/mjpeg )*/
    };

}VDEC_CHN_PARAM_S;



typedef struct hiH264_PRTCL_PARAM_S
{
    HI_S32  s32MaxSliceNum;            /* RW; max slice num support */
    HI_S32  s32MaxSpsNum;              /* RW; max sps num support */
    HI_S32  s32MaxPpsNum;              /* RW; max pps num support */
}H264_PRTCL_PARAM_S;

typedef struct hiH265_PRTCL_PARAM_S
{
    HI_S32  s32MaxSliceSegmentNum;     /* RW; max slice segmnet num support */
    HI_S32  s32MaxVpsNum;              /* RW; max vps num support */
    HI_S32  s32MaxSpsNum;              /* RW; max sps num support */
    HI_S32  s32MaxPpsNum;              /* RW; max pps num support */
}H265_PRTCL_PARAM_S;

typedef struct hiVDEC_PRTCL_PARAM_S
{
    PAYLOAD_TYPE_E enType;                      /* RW; video type to be decoded, only h264 and h265 supported*/
    union
    {
        H264_PRTCL_PARAM_S stH264PrtclParam;    /* protocol param structure for h264 */
        H265_PRTCL_PARAM_S stH265PrtclParam;    /* protocol param structure for h265 */
    };
}VDEC_PRTCL_PARAM_S;


typedef struct hiVDEC_CHN_POOL_S
{
    VB_POOL hPicVbPool;         /* RW;  vb pool id for pic buffer */
    VB_POOL hTmvVbPool;         /* RW;  vb pool id for tmv buffer */
}VDEC_CHN_POOL_S;


typedef enum hiVDEC_EVNT_E
{
    VDEC_EVNT_STREAM_ERR = 1,
    VDEC_EVNT_UNSUPPORT,
    VDEC_EVNT_OVER_REFTHR,
    VDEC_EVNT_REF_NUM_OVER,
    VDEC_EVNT_SLICE_NUM_OVER,
    VDEC_EVNT_SPS_NUM_OVER,
    VDEC_EVNT_PPS_NUM_OVER,
    VDEC_EVNT_PICBUF_SIZE_ERR,
    VDEC_EVNT_SIZE_OVER,
    VDEC_EVNT_IMG_SIZE_CHANGE,
    VDEC_EVNT_VPS_NUM_OVER,
    VDEC_EVNT_BUTT
} VDEC_EVNT_E;

typedef struct hiVDEC_VIDEO_MOD_PARAM_S
{
    HI_U32  u32MaxPicWidth;
    HI_U32  u32MaxPicHeight;
    HI_U32  u32MaxSliceNum;
    HI_U32  u32VdhMsgNum;
    HI_U32  u32VdhBinSize;
    HI_U32  u32VdhExtMemLevel;
} VDEC_VIDEO_MOD_PARAM_S;


typedef struct hiVDEC_MOD_PARAM_S
{
    VB_SOURCE_E             enVdecVBSource;    /* RW, Range: [1, 3];  frame buffer mode  */
    HI_U32                  u32MiniBufMode;    /* RW, Range: [0, 1];  stream buffer mode */
    HI_U32                  u32ParallelMode;   /* RW, Range: [0, 1];  VDH working mode   */
    VDEC_VIDEO_MOD_PARAM_S  stVideoModParam;
} VDEC_MOD_PARAM_S;

/************************************************************************************************************************/

/* invlalid channel ID */
#define HI_ERR_VDEC_INVALID_CHNID     HI_DEF_ERR(HI_ID_VDEC, EN_ERR_LEVEL_ERROR, EN_ERR_INVALID_CHNID)
/* at lease one parameter is illagal ,eg, an illegal enumeration value  */
#define HI_ERR_VDEC_ILLEGAL_PARAM     HI_DEF_ERR(HI_ID_VDEC, EN_ERR_LEVEL_ERROR, EN_ERR_ILLEGAL_PARAM)
/* channel exists */
#define HI_ERR_VDEC_EXIST             HI_DEF_ERR(HI_ID_VDEC, EN_ERR_LEVEL_ERROR, EN_ERR_EXIST)
/* using a NULL point */
#define HI_ERR_VDEC_NULL_PTR          HI_DEF_ERR(HI_ID_VDEC, EN_ERR_LEVEL_ERROR, EN_ERR_NULL_PTR)
/* try to enable or initialize system,device or channel, before configing attribute */
#define HI_ERR_VDEC_NOT_CONFIG        HI_DEF_ERR(HI_ID_VDEC, EN_ERR_LEVEL_ERROR, EN_ERR_NOT_CONFIG)
/* operation is not supported by NOW */
#define HI_ERR_VDEC_NOT_SUPPORT      HI_DEF_ERR(HI_ID_VDEC, EN_ERR_LEVEL_ERROR, EN_ERR_NOT_SUPPORT)
/* operation is not permitted ,eg, try to change stati attribute */
#define HI_ERR_VDEC_NOT_PERM          HI_DEF_ERR(HI_ID_VDEC, EN_ERR_LEVEL_ERROR, EN_ERR_NOT_PERM)
/* the channle is not existed  */
#define HI_ERR_VDEC_UNEXIST           HI_DEF_ERR(HI_ID_VDEC, EN_ERR_LEVEL_ERROR, EN_ERR_UNEXIST)
/* failure caused by malloc memory */
#define HI_ERR_VDEC_NOMEM             HI_DEF_ERR(HI_ID_VDEC, EN_ERR_LEVEL_ERROR, EN_ERR_NOMEM)
/* failure caused by malloc buffer */
#define HI_ERR_VDEC_NOBUF             HI_DEF_ERR(HI_ID_VDEC, EN_ERR_LEVEL_ERROR, EN_ERR_NOBUF)
/* no data in buffer */
#define HI_ERR_VDEC_BUF_EMPTY         HI_DEF_ERR(HI_ID_VDEC, EN_ERR_LEVEL_ERROR, EN_ERR_BUF_EMPTY)
/* no buffer for new data */
#define HI_ERR_VDEC_BUF_FULL          HI_DEF_ERR(HI_ID_VDEC, EN_ERR_LEVEL_ERROR, EN_ERR_BUF_FULL)
/* system is not ready,had not initialed or loaded*/
#define HI_ERR_VDEC_SYS_NOTREADY      HI_DEF_ERR(HI_ID_VDEC, EN_ERR_LEVEL_ERROR, EN_ERR_SYS_NOTREADY)
/*system busy*/
#define HI_ERR_VDEC_BUSY              HI_DEF_ERR(HI_ID_VDEC, EN_ERR_LEVEL_ERROR, EN_ERR_BUSY)

/* bad address,  eg. used for copy_from_user & copy_to_user   */
#define HI_ERR_VDEC_BADADDR              HI_DEF_ERR(HI_ID_VDEC, EN_ERR_LEVEL_ERROR, EN_ERR_BADADDR)


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */

#endif /* End of #ifndef  __HI_COMM_VDEC_H__ */

