/*
* Copyright (c) Hisilicon Technologies Co., Ltd. 2019. All rights reserved.
* Description:Common defination Of video decode
* Author: Hisilicon multimedia software group
* Create: 2019/06/15
*/

#ifndef __HI_COMMON_VDEC_H__
#define __HI_COMMON_VDEC_H__
#include "hi_type.h"
#include "hi_common.h"
#include "hi_errno.h"
#include "hi_common_video.h"
#include "hi_common_vb.h"
#include "hi_defines.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* end of #ifdef __cplusplus */

typedef enum {
    HI_VDEC_SEND_MODE_STREAM = 0,  /* Send by stream. */
    HI_VDEC_SEND_MODE_FRAME,  /* Send by frame. */
    /**
     * One frame supports multiple packets sending.
     * The current frame is considered to end when end_of_frame is equal to HI_TRUE.
     */
    HI_VDEC_SEND_MODE_COMPAT,
    HI_VDEC_SEND_MODE_BUTT
} hi_vdec_send_mode;

typedef enum {
    HI_VIDEO_DEC_MODE_IPB = 0,
    HI_VIDEO_DEC_MODE_IP,
    HI_VIDEO_DEC_MODE_I,
    HI_VIDEO_DEC_MODE_BUTT
} hi_video_dec_mode;

typedef enum {
    HI_VIDEO_OUT_ORDER_DISPLAY = 0,
    HI_VIDEO_OUT_ORDER_DEC,
    HI_VIDEO_OUT_ORDER_BUTT
} hi_video_out_order;

typedef enum {
    HI_VDEC_EVENT_STREAM_ERR = 1,
    HI_VDEC_EVENT_UNSUPPORT,
    HI_VDEC_EVENT_ERR_THRESHOLD_OVER,
    HI_VDEC_EVENT_REF_NUM_OVER,
    HI_VDEC_EVENT_SLICE_NUM_OVER,
    HI_VDEC_EVENT_SPS_NUM_OVER,
    HI_VDEC_EVENT_PPS_NUM_OVER,
    HI_VDEC_EVENT_PIC_BUF_SIZE_ERR,
    HI_VDEC_EVENT_SIZE_OVER,
    HI_VDEC_EVENT_FRAME_SIZE_CHG,
    HI_VDEC_EVENT_VPS_NUM_OVER,
    HI_VDEC_EVENT_BUTT
} hi_vdec_event;

typedef enum {
    HI_VDEC_CAPACITY_STRATEGY_BY_MOD = 0,
    HI_VDEC_CAPACITY_STRATEGY_BY_CHN = 1,
    HI_VDEC_CAPACITY_STRATEGY_BUTT
} hi_vdec_capacity_strategy;

typedef enum {
    HI_VDEC_FRAME_TYPE_I = 0,
    HI_VDEC_FRAME_TYPE_P = 1,
    HI_VDEC_FRAME_TYPE_B = 2,
    HI_VDEC_FRAME_TYPE_BUTT
} hi_vdec_frame_type;


typedef struct {
    hi_u32 ref_frame_num;  /* RW, Range: [0, 16]; reference frame num. */
    /**
     * RW; Specifies whether temporal motion vector predictors
     * can be used for inter prediction.
     */
    hi_bool temporal_mvp_en;
    hi_u32 tmv_buf_size;  /* RW; The size of tmv buffer (byte). */
} hi_vdec_video_attr;

typedef struct {
    hi_payload_type type;  /* RW; Video type to be decoded.  */
    hi_vdec_send_mode mode;  /* RW; Send by stream or by frame. */
    hi_u32 pic_width;  /* RW; Max width of pic. */
    hi_u32 pic_height;  /* RW; Max height of pic. */
    hi_u32 stream_buf_size;  /* RW; The size of stream buffer (byte). */
    hi_u32 frame_buf_size;  /* RW; The size of frame buffer (byte). */
    hi_u32 frame_buf_cnt;
    union {
        /**
         * RW; Structure with video ( h264/h265).
         * AUTO:hi_payload_type:HI_PT_H264,HI_PT_H265;
         */
        hi_vdec_video_attr video_attr;
    };
} hi_vdec_chn_attr;

typedef struct {
    hi_bool end_of_frame;  /* W; Is the end of a frame. */
    hi_bool end_of_stream;  /* W; Is the end of all stream. */
    hi_bool need_display;  /* W; Is the current frame displayed. only valid by HI_VDEC_SEND_MODE_FRAME. */
    hi_u64 pts;  /* W; Time stamp */
    hi_u64 private_data;  /* W; Private data, only valid by HI_VDEC_SEND_MODE_FRAME or HI_VDEC_SEND_MODE_COMPAT. */
    hi_u32 len;  /* W; The len of stream */
    hi_u8 *ATTRIBUTE addr;  /* W; The address of stream. */
} hi_vdec_stream;

typedef struct {
    hi_phys_addr_t phys_addr;  /* R; The physical address of user data. */
    hi_u32 len;  /* R; The len of user data. */
    hi_bool is_valid;  /* R; Is valid? */
    hi_u8 *ATTRIBUTE virt_addr;  /* R; The virtual address of user data. */
} hi_vdec_user_data;

typedef struct {
    hi_vdec_frame_type frame_type;
    hi_u32 err_rate;
    hi_u32 poc;
} hi_vdec_video_supplement_info;

typedef struct {
    hi_payload_type type;  /* RW; Video type to be decoded. */
    union {
        /**
         * Structure with video ( h265/h264).
         * AUTO:hi_payload_type:HI_PT_H264,HI_PT_H265;
         */
        hi_vdec_video_supplement_info video_supplement_info;
    };
} hi_vdec_supplement_info;

typedef struct {
    hi_s32 set_pic_size_err;  /* R; Picture width or height is larger than channel width or height. */
    hi_s32 set_protocol_num_err;  /* R; Protocol num is not enough. eg: slice, pps, sps. */
    hi_s32 set_ref_num_err;  /* R; Reference num is not enough. */
    hi_s32 set_pic_buf_size_err;  /* R; The buffer size of picture is not enough. */
    hi_s32 format_err;  /* R; Format error. eg: do not support filed. */
    hi_s32 stream_unsupport;  /* R; Unsupport the stream specification. */
    hi_s32 pack_err;  /* R; Stream package error. */
    hi_s32 stream_size_over;  /* R; The stream size is too big and force discard stream. */
    hi_s32 stream_not_release;  /* R; The stream not released for too long time. */
} hi_vdec_dec_err;

typedef struct {
    hi_payload_type type;  /* R; Video type to be decoded. */
    hi_u32 left_stream_bytes;  /* R; Left stream bytes waiting for decode. */
    hi_u32 left_stream_frames;  /* R; Left frames waiting for decode,only valid for HI_VDEC_SEND_MODE_FRAME. */
    hi_u32 left_decoded_frames;  /* R; Pics waiting for output. */
    hi_bool is_started;  /* R; Had started recv stream? */
    hi_u32 recv_stream_frames;  /* R; How many frames of stream has been received. valid when send by frame. */
    hi_u32 dec_stream_frames;  /* R; How many frames of stream has been decoded. valid when send by frame. */
    hi_vdec_dec_err dec_err;  /* R; Information about decode error. */
    hi_u32 width;  /* R; The width of the currently decoded stream. */
    hi_u32 height;  /* R; The height of the currently decoded stream. */
    hi_u64 latest_frame_pts;  /* R; PTS of the latest decoded frame. */
} hi_vdec_chn_status;

typedef enum {
    HI_QUICK_MARK_ADAPT = 0,
    HI_QUICK_MARK_FORCE,
    HI_QUICK_MARK_NONE,
    HI_QUICK_MARK_BUTT
} hi_quick_mark_mode;

typedef struct {
    /* RW; HI_FALSE: output base layer; HI_TRUE: output enhance layer; default: HI_FALSE */
    hi_bool composite_dec_en;
    /**
     * RW, Range: [0, 100]; threshold for stream error process,
     * 0: discard with any error, 100 : keep data with any error.
     */
    hi_s32 err_threshold;
    /**
     * RW; Decode mode , 0: decode IPB frames,
     * 1: only decode I frame & P frame , 2: only decode I frame.
     */
    hi_video_dec_mode dec_mode;
    /**
     * RW; Frames output order ,
     * 0: the same with display order , 1: the same width decoder order.
     */
    hi_video_out_order out_order;
    hi_compress_mode compress_mode;  /* RW; Compress mode. */
    hi_video_format video_format;  /* RW; Video format. */
    hi_quick_mark_mode quick_mark_mode;
} hi_vdec_video_param;

typedef struct {
    hi_pixel_format pixel_format;  /* RW; Out put pixel format. */
    /**
     * RW, range: [0, 255]; Value 0 is transparent.
     * [0 ,127]   is deemed to transparent when pixel_format is ARGB1555 or ABGR1555
     * [128 ,256] is deemed to non-transparent when pixel_format is ARGB1555 or ABGR1555.
     */
    hi_u32 alpha;
} hi_vdec_pic_param;

typedef struct {
    hi_payload_type type;  /* RW; Video type to be decoded. */
    hi_u32 display_frame_num;  /* RW, Range: [0, 16]; display frame num. */
    union {
        /**
         * Structure with video ( h265/h264).
         * AUTO:hi_payload_type:HI_PT_H264,HI_PT_H265;
         */
        hi_vdec_video_param video_param;
        /**
         * Structure with picture (jpeg/mjpeg ).
         * AUTO:hi_payload_type:HI_PT_JPEG,HI_PT_MJPEG;
         */
        hi_vdec_pic_param pic_param;
    };
} hi_vdec_chn_param;

typedef struct {
    hi_s32 max_slice_num;  /* RW; Max slice num support. */
    hi_s32 max_sps_num;  /* RW; Max sps num support. */
    hi_s32 max_pps_num;  /* RW; Max pps num support. */
} hi_h264_protocol_param;

typedef struct {
    hi_s32 max_slice_segment_num;  /* RW; Max slice segment num support. */
    hi_s32 max_vps_num;  /* RW; Max vps num support. */
    hi_s32 max_sps_num;  /* RW; Max sps num support. */
    hi_s32 max_pps_num;  /* RW; Max pps num support. */
} hi_h265_protocol_param;

typedef struct {
    hi_payload_type type;  /* RW; Video type to be decoded, only h264 and h265 supported. */
    union {
        /**
         * Protocol param structure for h264.
         * AUTO:hi_payload_type:HI_PT_H264;
         */
        hi_h264_protocol_param h264_param;
        /**
         * Protocol param structure for h265.
         * AUTO:hi_payload_type:HI_PT_H265;
         */
        hi_h265_protocol_param h265_param;
    };
} hi_vdec_protocol_param;

typedef struct {
    hi_vb_pool pic_vb_pool;  /* RW;  Vb pool id for pic buffer. */
    hi_vb_pool tmv_vb_pool;  /* RW;  Vb pool id for tmv buffer. */
} hi_vdec_chn_pool;

typedef struct {
    hi_u32 max_pic_width;
    hi_u32 max_pic_height;
    hi_u32 max_slice_num;
    hi_u32 vdh_msg_num;
} hi_vdec_video_mod_param;

typedef struct {
    hi_u32 max_pic_width;
    hi_u32 max_pic_height;
    hi_bool progressive_en;
    hi_bool dynamic_alloc_en;
    hi_vdec_capacity_strategy capacity_strategy;
} hi_vdec_pic_mod_param;

typedef struct {
    hi_vb_src vb_src;  /* RW, Range: [1, 3];  frame buffer mode. */
    hi_u32 mini_buf_mode;  /* RW, Range: [0, 1];  stream buffer mode. */
    hi_vdec_video_mod_param video_mod_param;
    hi_vdec_pic_mod_param pic_mod_param;
} hi_vdec_mod_param;

typedef struct {
    hi_bool enable;
    hi_u32  max_user_data_len;
} hi_vdec_user_data_attr;

/*********************************************************************************************/
/* Invalid channel ID. */
#define HI_ERR_VDEC_INVALID_CHN_ID HI_DEFINE_ERR(HI_ID_VDEC, HI_ERR_LEVEL_ERROR, HI_ERR_INVALID_CHN_ID)
/* At least one parameter is illegal ,eg, an illegal enumeration value. */
#define HI_ERR_VDEC_ILLEGAL_PARAM HI_DEFINE_ERR(HI_ID_VDEC, HI_ERR_LEVEL_ERROR, HI_ERR_ILLEGAL_PARAM)
/* Channel exists. */
#define HI_ERR_VDEC_EXIST         HI_DEFINE_ERR(HI_ID_VDEC, HI_ERR_LEVEL_ERROR, HI_ERR_EXIST)
/* Using a NULL point. */
#define HI_ERR_VDEC_NULL_PTR      HI_DEFINE_ERR(HI_ID_VDEC, HI_ERR_LEVEL_ERROR, HI_ERR_NULL_PTR)
/* Try to enable or initialize system,device or channel, before configure attribute. */
#define HI_ERR_VDEC_NOT_CFG    HI_DEFINE_ERR(HI_ID_VDEC, HI_ERR_LEVEL_ERROR, HI_ERR_NOT_CFG)
/* Operation is not supported by NOW. */
#define HI_ERR_VDEC_NOT_SUPPORT   HI_DEFINE_ERR(HI_ID_VDEC, HI_ERR_LEVEL_ERROR, HI_ERR_NOT_SUPPORT)
/* Operation is not permitted ,eg, try to change stati attribute. */
#define HI_ERR_VDEC_NOT_PERM      HI_DEFINE_ERR(HI_ID_VDEC, HI_ERR_LEVEL_ERROR, HI_ERR_NOT_PERM)
/* The channel is not existed. */
#define HI_ERR_VDEC_UNEXIST       HI_DEFINE_ERR(HI_ID_VDEC, HI_ERR_LEVEL_ERROR, HI_ERR_UNEXIST)
/* Failure caused by malloc memory. */
#define HI_ERR_VDEC_NO_MEM         HI_DEFINE_ERR(HI_ID_VDEC, HI_ERR_LEVEL_ERROR, HI_ERR_NO_MEM)
/* Failure caused by malloc buffer. */
#define HI_ERR_VDEC_NO_BUF         HI_DEFINE_ERR(HI_ID_VDEC, HI_ERR_LEVEL_ERROR, HI_ERR_NO_BUF)
/* No data in buffer. */
#define HI_ERR_VDEC_BUF_EMPTY     HI_DEFINE_ERR(HI_ID_VDEC, HI_ERR_LEVEL_ERROR, HI_ERR_BUF_EMPTY)
/* No buffer for new data. */
#define HI_ERR_VDEC_BUF_FULL      HI_DEFINE_ERR(HI_ID_VDEC, HI_ERR_LEVEL_ERROR, HI_ERR_BUF_FULL)
/* System is not ready,had not initialed or loaded. */
#define HI_ERR_VDEC_SYS_NOT_READY  HI_DEFINE_ERR(HI_ID_VDEC, HI_ERR_LEVEL_ERROR, HI_ERR_NOT_READY)
/* System busy */
#define HI_ERR_VDEC_BUSY          HI_DEFINE_ERR(HI_ID_VDEC, HI_ERR_LEVEL_ERROR, HI_ERR_BUSY)

/* Bad address,  eg. used for copy_from_user & copy_to_user. */
#define HI_ERR_VDEC_BAD_ADDR       HI_DEFINE_ERR(HI_ID_VDEC, HI_ERR_LEVEL_ERROR, HI_ERR_BAD_ADDR)


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* end of #ifdef __cplusplus */

#endif /* end of #ifndef  __HI_COMMON_VDEC_H__ */


