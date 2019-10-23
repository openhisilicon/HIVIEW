/******************************************************************************

  Copyright (C), 2013, Hisilicon Tech. Co., Ltd.

 ******************************************************************************
  File Name     : hi_comm_vdec.h
  Version       : Initial Draft
  Author        : Hisilicon multimedia software group
  Created       : 2013/05/08
  Description   : Common Def Of video decode
  History       :
  1.Date        : 2013/05/08
    Author      : l00226816
    Modification: Created file
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

/*vdec channel attr(user interface)*/

#define HI_IO_BLOCK   HI_TRUE
#define HI_IO_NOBLOCK HI_FALSE


typedef struct hiVDEC_CHN_LUM_S
{
    HI_U64 u64LumPixSum;                /* Luma sum of current frame */
    HI_U32 u32LumPixAverage;            /* Luma average of current frame */
    HI_U64 u64Pts;                      /* PTS of current frame ?? */
} VDEC_CHN_LUM_S;


typedef enum hiVIDEO_MODE_E
{
    VIDEO_MODE_STREAM = 0,     /*send by stream*/
    VIDEO_MODE_FRAME     ,     /*send by frame*/
    VIDEO_MODE_BUTT
}VIDEO_MODE_E;

typedef enum hiVDEC_JPG_COLOR_FMT_E
{
    JPG_COLOR_FMT_YCBCR400,        /*YUV400*/
    JPG_COLOR_FMT_YCBCR420,        /*YUV420*/
    JPG_COLOR_FMT_YCBCR422BHP,     /*YUV 422 2x1*/  
    JPG_COLOR_FMT_YCBCR422BVP,     /*YUV 422 1x2*/  
    JPG_COLOR_FMT_YCBCR444,        /*YUV 444*/  
    JPG_COLOR_FMT_BUTT             
} VDEC_JPG_COLOR_FMT_E;

typedef struct hiVDEC_ATTR_JPEG_S
{
    VIDEO_MODE_E     enMode;	    /* video stream mode select */
    VDEC_JPG_COLOR_FMT_E     enJpegFormat;  /* jpeg format select ,may be YUV420,YUV400,YUV422,YUV444*/
}VDEC_ATTR_JPEG_S,*PTR_VDEC_ATTR_JPEG_S;


typedef struct hiVDEC_ATTR_VIDEO_S
{
    HI_U32       u32RefFrameNum;         /*ref pic num [0,16]*/
	VIDEO_MODE_E enMode;				 /*send by stream or by frame*/
    HI_BOOL      bTemporalMvpEnable;     /* specifies whether temporal motion vector predictors can be used for inter prediction*/
}VDEC_ATTR_VIDEO_S,*PTR_VDEC_ATTR_VIDEO_S;


typedef struct hiVDEC_CHN_ATTR_S
{
	PAYLOAD_TYPE_E enType;				   /* video type to be decoded */
	HI_U32   	u32BufSize		;		   /* stream buf size(Byte) */
    HI_U32 		u32Priority		; 		   /* priority */
	HI_U32 		u32PicWidth		;		   /* max pic width */
	HI_U32 		u32PicHeight	;		   /* max pic height */
	union
	{
		VDEC_ATTR_JPEG_S stVdecJpegAttr;	/* structure with jpeg or mjpeg type  */
		VDEC_ATTR_VIDEO_S stVdecVideoAttr;	/* structure with video ( h264/mpeg4) */
	};
}VDEC_CHN_ATTR_S;


typedef struct hiVDEC_STREAM_S
{
    HI_U8* 	pu8Addr;			/* stream address */
    HI_U32  u32Len;				/* stream len */
    HI_U64  u64PTS;             /* time stamp */
    HI_BOOL bEndOfFrame;        /* is the end of a frame */
    HI_BOOL bEndOfStream;       /* is the end of all stream */
}VDEC_STREAM_S;

typedef struct hiVDEC_STREAM_EX_S
{
    VDEC_STREAM_S stStream;
	HI_BOOL bDisplay;           /* whether no display,0:display  1:no display */
}VDEC_STREAM_EX_S;

typedef struct hiVDEC_USERDATA_S{
    HI_U8*    		pu8Addr;   		/* userdata data vir address */
    HI_U32		    u32PhyAddr;		/* userdata data phy address */
    HI_U32  		u32Len;    		/* userdata data len */
    HI_BOOL         bValid;			/* is valid? */
}VDEC_USERDATA_S;


typedef struct hi_VDEC_DECODE_ERROR_S
{
	HI_S32 s32FormatErr;          /* format error. eg: do not support filed */
    HI_S32 s32PicSizeErrSet;      /* picture width or height is larger than chnnel width or height*/
	HI_S32 s32StreamUnsprt;       /* unsupport the stream specification */
	HI_S32 s32PackErr;            /* stream package error */
	HI_S32 s32PrtclNumErrSet;     /* protocol num is not enough. eg: slice, pps, sps */
	HI_S32 s32RefErrSet;          /* refrence num is not enough */
	HI_S32 s32PicBufSizeErrSet;   /* the buffer size of picture is not enough */	
    HI_S32 s32VdecStreamNotRelease; /* the stream not released for too long time */	
}VDEC_DECODE_ERROR_S;


typedef struct hiVDEC_CHN_STAT_S
{
	PAYLOAD_TYPE_E enType;				 /* video type to be decoded */	
	HI_U32  u32LeftStreamBytes;          /* left stream bytes waiting for decode */
	HI_U32  u32LeftStreamFrames;         /* left frames waiting for decode,only valid for H264D_MODE_FRAME */
	HI_U32  u32LeftPics;                 /* pics waiting for output */
	HI_BOOL bStartRecvStream;            /* had started recv stream? */
	HI_U32  u32RecvStreamFrames;         /* how many frames of stream has been received. valid when send by frame. */
	HI_U32  u32DecodeStreamFrames;       /* how many frames of stream has been decoded. valid when send by frame. */
	VDEC_DECODE_ERROR_S stVdecDecErr;    /* information about decode error */	
}VDEC_CHN_STAT_S;

/*
 * static parameter: must set after stop sending stream and all stream is decoded.
 * dynamic parameter: can be set at any time.
 */
typedef struct hiVDEC_CHN_PARAM_S
{
	HI_S32  s32DisplayFrameNum;	 /* display frame num */
    HI_S32  s32ChanErrThr;	     /* threshold for stream error process, 0: discard with any error, 100 : keep data with any error */
    HI_S32  s32ChanStrmOFThr;     /* threshold for stream overflow, 0~ , 0: nothing to do when stream is overflow */
    HI_S32  s32DecMode; 		     /* decode mode , 0: deocde IPB frames, 1: only decode I frame & P frame , 2: only decode I frame */
    HI_S32  s32DecOrderOutput;    /* frames output order ,0: the same with display order , 1: the same width decoder order */ 
    VIDEO_FORMAT_E  enVideoFormat;
    COMPRESS_MODE_E	enCompressMode;
}VDEC_CHN_PARAM_S;


typedef struct hiH264_PRTCL_PARAM_S
{
	HI_S32	s32MaxSliceNum;		      /* max  slice num support */ 
	HI_S32  s32MaxSpsNum;		      /* max sps num support */
	HI_S32  s32MaxPpsNum;		      /* max pps num support */
}H264_PRTCL_PARAM_S;

typedef struct hiH265_PRTCL_PARAM_S
{
	HI_S32	s32MaxSliceSegmentNum;	  /* max slice segmnet num support */ 
	HI_S32  s32MaxVpsNum;		      /* max vps num support */
	HI_S32  s32MaxSpsNum;		      /* max sps num support */
	HI_S32  s32MaxPpsNum;		      /* max pps num support */
}H265_PRTCL_PARAM_S;

typedef struct hiVDEC_PRTCL_PARAM_S
{
	PAYLOAD_TYPE_E enType;				   /* video type to be decoded ,only h264 and HEVC supported*/
	union
	{
		H264_PRTCL_PARAM_S stH264PrtclParam;	/* protocol param structure for h264*/
		H265_PRTCL_PARAM_S stH265PrtclParam;	/* protocol param structure for HEVC*/
	};
}VDEC_PRTCL_PARAM_S;


typedef struct hiVDEC_CHN_POOL_S
{
    VB_POOL hPicVbPool;
    VB_POOL hPmvVbPool;
}VDEC_CHN_POOL_S;

typedef struct hiVDEC_MOD_PARAM_S
{
  HI_U32  u32MiniBufMode;
  HI_U32  u32VBSource;
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
#define HI_ERR_VDEC_NOT_SURPPORT      HI_DEF_ERR(HI_ID_VDEC, EN_ERR_LEVEL_ERROR, EN_ERR_NOT_SUPPORT)
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
#define HI_ERR_VDEC_BUSY			  HI_DEF_ERR(HI_ID_VDEC, EN_ERR_LEVEL_ERROR, EN_ERR_BUSY)

/* bad address,  eg. used for copy_from_user & copy_to_user   */
#define HI_ERR_VDEC_BADADDR			  HI_DEF_ERR(HI_ID_VDEC, EN_ERR_LEVEL_ERROR, EN_ERR_BADADDR)


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */

#endif /* End of #ifndef  __HI_COMM_VDEC_H__ */

