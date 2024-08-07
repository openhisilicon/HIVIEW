/*
  Copyright (c), 2001-2024, Shenshu Tech. Co., Ltd.
 */

#ifndef OT_COMMON_VDEC_H
#define OT_COMMON_VDEC_H
#include "ot_type.h"
#include "ot_common.h"
#include "ot_errno.h"
#include "ot_common_video.h"
#include "ot_common_vb.h"
#include "ot_defines.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    OT_VDEC_SEND_MODE_STREAM = 0,  /* Send by stream. */
    OT_VDEC_SEND_MODE_FRAME,  /* Send by frame. */
    /**
     * One frame supports multiple packets sending.
     * The current frame is considered to end when end_of_frame is equal to TD_TRUE.
     */
    OT_VDEC_SEND_MODE_COMPAT,
    OT_VDEC_SEND_MODE_BUTT
} ot_vdec_send_mode;

typedef enum {
    OT_VIDEO_DEC_MODE_IPB = 0,
    OT_VIDEO_DEC_MODE_IP,
    OT_VIDEO_DEC_MODE_I,
    OT_VIDEO_DEC_MODE_BUTT
} ot_video_dec_mode;

typedef enum {
    OT_VIDEO_OUT_ORDER_DISPLAY = 0,
    OT_VIDEO_OUT_ORDER_DEC,
    OT_VIDEO_OUT_ORDER_BUTT
} ot_video_out_order;

typedef enum {
    OT_VDEC_EVENT_STREAM_ERR = 1,
    OT_VDEC_EVENT_UNSUPPORT,
    OT_VDEC_EVENT_ERR_THRESHOLD_OVER,
    OT_VDEC_EVENT_REF_NUM_OVER,
    OT_VDEC_EVENT_SLICE_NUM_OVER,
    OT_VDEC_EVENT_SPS_NUM_OVER,
    OT_VDEC_EVENT_PPS_NUM_OVER,
    OT_VDEC_EVENT_PIC_BUF_SIZE_ERR,
    OT_VDEC_EVENT_SIZE_OVER,
    OT_VDEC_EVENT_FRAME_SIZE_CHG,
    OT_VDEC_EVENT_VPS_NUM_OVER,
    OT_VDEC_EVENT_BUTT
} ot_vdec_event;

typedef enum {
    OT_VDEC_CAPACITY_STRATEGY_BY_MOD = 0,
    OT_VDEC_CAPACITY_STRATEGY_BY_CHN = 1,
    OT_VDEC_CAPACITY_STRATEGY_BUTT
} ot_vdec_capacity_strategy;

typedef enum {
    OT_VDEC_FRAME_TYPE_I = 0,
    OT_VDEC_FRAME_TYPE_P = 1,
    OT_VDEC_FRAME_TYPE_B = 2,
    OT_VDEC_FRAME_TYPE_BUTT
} ot_vdec_frame_type;


typedef struct {
    td_u32 ref_frame_num;  /* RW, Range: [0, 16]; reference frame num. */
    /**
     * RW; Specifies whether temporal motion vector predictors
     * can be used for inter prediction.
     */
    td_bool temporal_mvp_en;
    td_u32 tmv_buf_size;  /* RW; The size of tmv buffer (byte). */
} ot_vdec_video_attr;

typedef struct {
    ot_payload_type type;  /* RW; Video type to be decoded.  */
    ot_vdec_send_mode mode;  /* RW; Send by stream or by frame. */
    td_u32 pic_width;  /* RW; Max width of pic. */
    td_u32 pic_height;  /* RW; Max height of pic. */
    td_u32 stream_buf_size;  /* RW; The size of stream buffer (byte). */
    td_u32 frame_buf_size;  /* RW; The size of frame buffer (byte). */
    td_u32 frame_buf_cnt;
    union {
        /**
         * RW; Structure with video (h264/h265/mpeg4).
         * AUTO:ot_payload_type:OT_PT_H264, OT_PT_H265, OT_PT_MP4VIDEO;
         */
        ot_vdec_video_attr video_attr;
    };
} ot_vdec_chn_attr;

typedef struct {
    td_bool end_of_frame;  /* W; Is the end of a frame. */
    td_bool end_of_stream;  /* W; Is the end of all stream. */
    td_bool need_display;  /* W; Is the current frame displayed. only valid by OT_VDEC_SEND_MODE_FRAME. */
    td_u64 pts;  /* W; Time stamp */
    td_u64 private_data;  /* W; Private data, only valid by OT_VDEC_SEND_MODE_FRAME or OT_VDEC_SEND_MODE_COMPAT. */
    td_u32 len;  /* W; The len of stream */
    td_u8 *ATTRIBUTE addr;  /* W; The address of stream. */
} ot_vdec_stream;

typedef struct {
    td_phys_addr_t phys_addr;  /* R; The physical address of user data. */
    td_u32 len;  /* R; The len of user data. */
    td_bool is_valid;  /* R; Is valid? */
    td_u8 *ATTRIBUTE virt_addr;  /* R; The virtual address of user data. */
} ot_vdec_user_data;

typedef struct {
    ot_vdec_frame_type frame_type;
    td_u32 err_rate;
    td_u32 poc;
} ot_vdec_video_supplement_info;

typedef struct {
    ot_payload_type type;  /* RW; Video type to be decoded. */
    union {
        /**
         * Structure with video (h265/h264/mpeg4).
         * AUTO:ot_payload_type:OT_PT_H264, OT_PT_H265, OT_PT_MP4VIDEO;
         */
        ot_vdec_video_supplement_info video_supplement_info;
    };
} ot_vdec_supplement_info;

typedef struct {
    td_s32 set_pic_size_err;  /* R; Picture width or height is larger than channel width or height. */
    td_s32 set_protocol_num_err;  /* R; Protocol num is not enough. eg: slice, pps, sps. */
    td_s32 set_ref_num_err;  /* R; Reference num is not enough. */
    td_s32 set_pic_buf_size_err;  /* R; The buffer size of picture is not enough. */
    td_s32 format_err;  /* R; Format error. eg: do not support filed. */
    td_s32 stream_unsupport;  /* R; Unsupported stream specification. */
    td_s32 pack_err;  /* R; Stream package error. */
    td_s32 stream_size_over;  /* R; The stream size is too big and force discard stream. */
    td_s32 stream_not_release;  /* R; The stream not released for too long time. */
} ot_vdec_dec_err;

typedef struct {
    ot_payload_type type;  /* R; Video type to be decoded. */
    td_u32 left_stream_bytes;  /* R; Left stream bytes waiting for decode. */
    td_u32 left_stream_frames;  /* R; Left frames waiting for decode,only valid for OT_VDEC_SEND_MODE_FRAME. */
    td_u32 left_decoded_frames;  /* R; Pics waiting for output. */
    td_bool is_started;  /* R; Had started recv stream? */
    td_u32 recv_stream_frames;  /* R; How many frames of stream has been received. valid when send by frame. */
    td_u32 dec_stream_frames;  /* R; How many frames of stream has been decoded. valid when send by frame. */
    ot_vdec_dec_err dec_err;  /* R; Information about decode error. */
    td_u32 width;  /* R; The width of the currently decoded stream. */
    td_u32 height;  /* R; The height of the currently decoded stream. */
    td_u64 latest_frame_pts;  /* R; PTS of the latest decoded frame. */
} ot_vdec_chn_status;

typedef enum {
    OT_QUICK_MARK_ADAPT = 0,
    OT_QUICK_MARK_FORCE,
    OT_QUICK_MARK_NONE,
    OT_QUICK_MARK_BUTT
} ot_quick_mark_mode;

typedef struct {
    /* RW; TD_FALSE: output base layer; TD_TRUE: output enhance layer; default: TD_FALSE */
    td_bool composite_dec_en;
    /* RW; TD_FALSE: don't support slice low latency; TD_TRUE: support slice low latency; default: TD_FALSE */
    td_bool slice_input_en;
    /**
     * RW, Range: [0, 100]; threshold for stream error process,
     * 0: discard with any error, 100 : keep data with any error.
     */
    td_s32 err_threshold;
    /**
     * RW; Decode mode , 0: decode IPB frames,
     * 1: only decode I frame & P frame , 2: only decode I frame.
     */
    ot_video_dec_mode dec_mode;
    /**
     * RW; Frames output order ,
     * 0: the same with display order , 1: the same width decoder order.
     */
    ot_video_out_order out_order;
    ot_compress_mode compress_mode;  /* RW; Compress mode. */
    ot_video_format video_format;  /* RW; Video format. */
    ot_quick_mark_mode quick_mark_mode;
} ot_vdec_video_param;

typedef struct {
    ot_pixel_format pixel_format;  /* RW; Out put pixel format. */
    /**
     * RW, range: [0, 255]; Value 0 is transparent.
     * [0 ,127]   is deemed to transparent when pixel_format is ARGB1555 or ABGR1555
     * [128 ,256] is deemed to non-transparent when pixel_format is ARGB1555 or ABGR1555.
     */
    td_u32 alpha;
} ot_vdec_pic_param;

typedef struct {
    ot_payload_type type;  /* RW; Video type to be decoded. */
    td_u32 display_frame_num;  /* RW, Range: [0, 16]; display frame num. */
    union {
        /**
         * Structure with video (h265/h264/mpeg4).
         * AUTO:ot_payload_type:OT_PT_H264, OT_PT_H265, OT_PT_MP4VIDEO;
         */
        ot_vdec_video_param video_param;
        /**
         * Structure with picture (jpeg/mjpeg).
         * AUTO:ot_payload_type:OT_PT_JPEG, OT_PT_MJPEG;
         */
        ot_vdec_pic_param pic_param;
    };
} ot_vdec_chn_param;

typedef struct {
    td_s32 max_slice_num;  /* RW; Max slice num support. */
    td_s32 max_sps_num;  /* RW; Max sps num support. */
    td_s32 max_pps_num;  /* RW; Max pps num support. */
} ot_h264_protocol_param;

typedef struct {
    td_s32 max_slice_segment_num;  /* RW; Max slice segment num support. */
    td_s32 max_vps_num;  /* RW; Max vps num support. */
    td_s32 max_sps_num;  /* RW; Max sps num support. */
    td_s32 max_pps_num;  /* RW; Max pps num support. */
} ot_h265_protocol_param;

typedef struct {
    ot_payload_type type;  /* RW; Video type to be decoded, only h264 and h265 supported. */
    union {
        /**
         * Protocol param structure for h264.
         * AUTO:ot_payload_type:OT_PT_H264;
         */
        ot_h264_protocol_param h264_param;
        /**
         * Protocol param structure for h265.
         * AUTO:ot_payload_type:OT_PT_H265;
         */
        ot_h265_protocol_param h265_param;
    };
} ot_vdec_protocol_param;

typedef struct {
    ot_vb_pool pic_vb_pool;  /* RW;  Vb pool id for pic buffer. */
    ot_vb_pool tmv_vb_pool;  /* RW;  Vb pool id for tmv buffer. */
} ot_vdec_chn_pool;

typedef struct {
    td_u32 max_pic_width;
    td_u32 max_pic_height;
    td_u32 max_slice_num;
    td_u32 vdh_msg_num;
    td_u32 compat_no_ref;
} ot_vdec_video_mod_param;

typedef struct {
    td_u32 max_pic_width;
    td_u32 max_pic_height;
    td_bool progressive_en;
    td_bool dynamic_alloc_en;
    ot_vdec_capacity_strategy capacity_strategy;
} ot_vdec_pic_mod_param;

typedef struct {
    ot_vb_src vb_src;  /* RW, Range: [1, 3];  frame buffer mode. */
    td_u32 mini_buf_mode;  /* RW, Range: [0, 1];  stream buffer mode. */
    ot_vdec_video_mod_param video_mod_param;
    ot_vdec_pic_mod_param pic_mod_param;
} ot_vdec_mod_param;

typedef struct {
    td_bool enable;
    td_u32  max_user_data_len;
} ot_vdec_user_data_attr;

typedef struct {
    ot_vdec_user_data_attr user_data_attr;
} ot_vdec_chn_config;

/*********************************************************************************************/
/* Invalid channel ID. */
#define OT_ERR_VDEC_INVALID_CHN_ID OT_DEFINE_ERR(OT_ID_VDEC, OT_ERR_LEVEL_ERROR, OT_ERR_INVALID_CHN_ID)
/* At least one parameter is illegal ,eg, an illegal enumeration value. */
#define OT_ERR_VDEC_ILLEGAL_PARAM OT_DEFINE_ERR(OT_ID_VDEC, OT_ERR_LEVEL_ERROR, OT_ERR_ILLEGAL_PARAM)
/* Channel exists. */
#define OT_ERR_VDEC_EXIST         OT_DEFINE_ERR(OT_ID_VDEC, OT_ERR_LEVEL_ERROR, OT_ERR_EXIST)
/* Using a NULL pointer. */
#define OT_ERR_VDEC_NULL_PTR      OT_DEFINE_ERR(OT_ID_VDEC, OT_ERR_LEVEL_ERROR, OT_ERR_NULL_PTR)
/* Try to enable or initialize system,device or channel, before configure attribute. */
#define OT_ERR_VDEC_NOT_CFG    OT_DEFINE_ERR(OT_ID_VDEC, OT_ERR_LEVEL_ERROR, OT_ERR_NOT_CFG)
/* Operation is not supported by NOW. */
#define OT_ERR_VDEC_NOT_SUPPORT   OT_DEFINE_ERR(OT_ID_VDEC, OT_ERR_LEVEL_ERROR, OT_ERR_NOT_SUPPORT)
/* Operation is not permitted ,eg, try to change statuses attribute. */
#define OT_ERR_VDEC_NOT_PERM      OT_DEFINE_ERR(OT_ID_VDEC, OT_ERR_LEVEL_ERROR, OT_ERR_NOT_PERM)
/* The channel is not existed. */
#define OT_ERR_VDEC_UNEXIST       OT_DEFINE_ERR(OT_ID_VDEC, OT_ERR_LEVEL_ERROR, OT_ERR_UNEXIST)
/* Failure caused by malloc memory. */
#define OT_ERR_VDEC_NO_MEM         OT_DEFINE_ERR(OT_ID_VDEC, OT_ERR_LEVEL_ERROR, OT_ERR_NO_MEM)
/* Failure caused by malloc buffer. */
#define OT_ERR_VDEC_NO_BUF         OT_DEFINE_ERR(OT_ID_VDEC, OT_ERR_LEVEL_ERROR, OT_ERR_NO_BUF)
/* No data in buffer. */
#define OT_ERR_VDEC_BUF_EMPTY     OT_DEFINE_ERR(OT_ID_VDEC, OT_ERR_LEVEL_ERROR, OT_ERR_BUF_EMPTY)
/* No buffer for new data. */
#define OT_ERR_VDEC_BUF_FULL      OT_DEFINE_ERR(OT_ID_VDEC, OT_ERR_LEVEL_ERROR, OT_ERR_BUF_FULL)
/* System is not ready,had not initialed or loaded. */
#define OT_ERR_VDEC_SYS_NOT_READY  OT_DEFINE_ERR(OT_ID_VDEC, OT_ERR_LEVEL_ERROR, OT_ERR_NOT_READY)
/* System busy */
#define OT_ERR_VDEC_BUSY          OT_DEFINE_ERR(OT_ID_VDEC, OT_ERR_LEVEL_ERROR, OT_ERR_BUSY)

/* Bad address,  eg. used for copy_from_user & copy_to_user. */
#define OT_ERR_VDEC_BAD_ADDR       OT_DEFINE_ERR(OT_ID_VDEC, OT_ERR_LEVEL_ERROR, OT_ERR_BAD_ADDR)

#ifdef __cplusplus
}
#endif

#endif /* end of #ifndef  OT_COMMON_VDEC_H */

