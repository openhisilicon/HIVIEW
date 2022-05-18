/*
 * Copyright (c) Hisilicon Technologies Co., Ltd. 2016-2019. All rights reserved.
 * Description: common rc
 * Author: Hisilicon multimedia software group
 * Create: 2016/11/15
 * History:
 */
#ifndef __HI_COMMON_RC_H__
#define __HI_COMMON_RC_H__

#include "hi_type.h"
#include "hi_defines.h"

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif

#define HI_VENC_TEXTURE_THRESHOLD_SIZE 16

/* rc mode */
typedef enum {
    HI_VENC_RC_MODE_H264_CBR = 1,
    HI_VENC_RC_MODE_H264_VBR,
    HI_VENC_RC_MODE_H264_AVBR,
    HI_VENC_RC_MODE_H264_QVBR,
    HI_VENC_RC_MODE_H264_CVBR,
    HI_VENC_RC_MODE_H264_FIXQP,
    HI_VENC_RC_MODE_H264_QPMAP,
    HI_VENC_RC_MODE_MJPEG_CBR,
    HI_VENC_RC_MODE_MJPEG_VBR,
    HI_VENC_RC_MODE_MJPEG_FIXQP,
    HI_VENC_RC_MODE_H265_CBR,
    HI_VENC_RC_MODE_H265_VBR,
    HI_VENC_RC_MODE_H265_AVBR,
    HI_VENC_RC_MODE_H265_QVBR,
    HI_VENC_RC_MODE_H265_CVBR,
    HI_VENC_RC_MODE_H265_FIXQP,
    HI_VENC_RC_MODE_H265_QPMAP,
    HI_VENC_RC_MODE_BUTT,
} hi_venc_rc_mode;

/* qpmap mode */
typedef enum {
    HI_VENC_RC_QPMAP_MODE_MEAN_QP = 0,
    HI_VENC_RC_QPMAP_MODE_MIN_QP,
    HI_VENC_RC_QPMAP_MODE_MAX_QP,
    HI_VENC_RC_QPMAP_MODE_BUTT,
} hi_venc_rc_qpmap_mode;

typedef struct {
    hi_u32 gop;
    hi_u32 src_frame_rate;
    hi_u32 dst_frame_rate;
    hi_u32 i_qp;
    hi_u32 p_qp;
    hi_u32 b_qp;
} hi_venc_h264_fixqp;

typedef struct {
    hi_u32 gop;
    hi_u32 stats_time;
    hi_u32 src_frame_rate;
    hi_u32 dst_frame_rate;
    hi_u32 bit_rate;
} hi_venc_h264_cbr;

typedef struct {
    hi_u32 gop;
    hi_u32 stats_time;
    hi_u32 src_frame_rate;
    hi_u32 dst_frame_rate;
    hi_u32 max_bit_rate;
} hi_venc_h264_vbr;

typedef struct {
    hi_u32 gop;
    hi_u32 stats_time;
    hi_u32 src_frame_rate;
    hi_u32 dst_frame_rate;
    hi_u32 max_bit_rate;
    hi_u32 short_term_stats_time;
    hi_u32 long_term_stats_time;
    hi_u32 long_term_max_bit_rate;
    hi_u32 long_term_min_bit_rate;
} hi_venc_h264_cvbr;

typedef struct {
    hi_u32 gop;
    hi_u32 stats_time;
    hi_u32 src_frame_rate;
    hi_u32 dst_frame_rate;
    hi_u32 max_bit_rate;
} hi_venc_h264_avbr;

typedef struct {
    hi_u32 gop;
    hi_u32 stats_time;
    hi_u32 src_frame_rate;
    hi_u32 dst_frame_rate;
} hi_venc_h264_qpmap;

typedef struct {
    hi_u32 gop;
    hi_u32 stats_time;
    hi_u32 src_frame_rate;
    hi_u32 dst_frame_rate;
    hi_u32 target_bit_rate;
} hi_venc_h264_qvbr;

typedef struct {
    hi_u32 gop;
    hi_u32 stats_time;
    hi_u32 src_frame_rate;
    hi_u32 dst_frame_rate;
    hi_venc_rc_qpmap_mode qpmap_mode;
} hi_venc_h265_qpmap;

typedef hi_venc_h264_cbr   hi_venc_h265_cbr;
typedef hi_venc_h264_vbr   hi_venc_h265_vbr;
typedef hi_venc_h264_avbr  hi_venc_h265_avbr;
typedef hi_venc_h264_fixqp hi_venc_h265_fixqp;
typedef hi_venc_h264_qvbr  hi_venc_h265_qvbr;
typedef hi_venc_h264_cvbr  hi_venc_h265_cvbr;

typedef struct {
    hi_u32 src_frame_rate;
    hi_u32 dst_frame_rate;
    hi_u32 qfactor;
} hi_venc_mjpeg_fixqp;

typedef struct {
    hi_u32 stats_time;
    hi_u32 src_frame_rate;
    hi_u32 dst_frame_rate;
    hi_u32 bit_rate;
} hi_venc_mjpeg_cbr;

typedef struct {
    hi_u32 stats_time;
    hi_u32 src_frame_rate;
    hi_u32 dst_frame_rate;
    hi_u32 max_bit_rate;
} hi_venc_mjpeg_vbr;

typedef struct {
    hi_venc_rc_mode rc_mode;
    union {
        hi_venc_h264_cbr   h264_cbr; /* AUTO:hi_venc_rc_mode:HI_VENC_RC_MODE_H264_CBR; */
        hi_venc_h264_vbr   h264_vbr; /* AUTO:hi_venc_rc_mode:HI_VENC_RC_MODE_H264_VBR; */
        hi_venc_h264_avbr  h264_avbr; /* AUTO:hi_venc_rc_mode:HI_VENC_RC_MODE_H264_AVBR; */
        hi_venc_h264_qvbr  h264_qvbr; /* AUTO:hi_venc_rc_mode:HI_VENC_RC_MODE_H264_QVBR; */
        hi_venc_h264_cvbr  h264_cvbr; /* AUTO:hi_venc_rc_mode:HI_VENC_RC_MODE_H264_CVBR; */
        hi_venc_h264_fixqp h264_fixqp; /* AUTO:hi_venc_rc_mode:HI_VENC_RC_MODE_H264_FIXQP; */
        hi_venc_h264_qpmap h264_qpmap; /* AUTO:hi_venc_rc_mode:HI_VENC_RC_MODE_H264_QPMAP; */

        hi_venc_mjpeg_cbr   mjpeg_cbr; /* AUTO:hi_venc_rc_mode:HI_VENC_RC_MODE_MJPEG_CBR; */
        hi_venc_mjpeg_vbr   mjpeg_vbr; /* AUTO:hi_venc_rc_mode:HI_VENC_RC_MODE_MJPEG_VBR; */
        hi_venc_mjpeg_fixqp mjpeg_fixqp; /* AUTO:hi_venc_rc_mode:HI_VENC_RC_MODE_MJPEG_FIXQP; */

        hi_venc_h265_cbr   h265_cbr; /* AUTO:hi_venc_rc_mode:HI_VENC_RC_MODE_H265_CBR; */
        hi_venc_h265_vbr   h265_vbr; /* AUTO:hi_venc_rc_mode:HI_VENC_RC_MODE_H265_VBR; */
        hi_venc_h265_avbr  h265_avbr; /* AUTO:hi_venc_rc_mode:HI_VENC_RC_MODE_H265_AVBR; */
        hi_venc_h265_qvbr  h265_qvbr; /* AUTO:hi_venc_rc_mode:HI_VENC_RC_MODE_H265_QVBR; */
        hi_venc_h265_cvbr  h265_cvbr; /* AUTO:hi_venc_rc_mode:HI_VENC_RC_MODE_H265_CVBR; */
        hi_venc_h265_fixqp h265_fixqp; /* AUTO:hi_venc_rc_mode:HI_VENC_RC_MODE_H265_FIXQP; */
        hi_venc_h265_qpmap h265_qpmap; /* AUTO:hi_venc_rc_mode:HI_VENC_RC_MODE_H265_QPMAP; */
    };
} hi_venc_rc_attr;

typedef enum {
    HI_VENC_SUPER_FRAME_NONE = 0,                  /* sdk don't care super frame */
    HI_VENC_SUPER_FRAME_DISCARD,                   /* the super frame is discarded */
    HI_VENC_SUPER_FRAME_REENCODE,                  /* the super frame is re-encode */
    HI_VENC_SUPER_FRAME_BUTT
} hi_venc_super_frame_mode;

typedef struct {
    hi_u32  max_i_proportion;
    hi_u32  min_i_proportion;
    hi_u32  max_qp;
    hi_u32  min_qp;
    hi_u32  max_i_qp;
    hi_u32  min_i_qp;
    hi_s32  max_reencode_times;
    hi_bool qpmap_en;
} hi_venc_h264_cbr_param;

typedef struct {
    hi_s32  chg_pos;
    hi_u32  max_i_proportion;
    hi_u32  min_i_proportion;
    hi_s32  max_reencode_times;
    hi_bool qpmap_en;
    hi_u32  max_qp;
    hi_u32  min_qp;
    hi_u32  max_i_qp;
    hi_u32  min_i_qp;
} hi_venc_h264_vbr_param;

typedef struct {
    hi_s32  chg_pos;
    hi_u32  max_i_proportion;
    hi_u32  min_i_proportion;
    hi_s32  max_reencode_times;
    hi_bool qpmap_en;
    hi_s32  min_still_percent;
    hi_u32  max_still_qp;
    hi_u32  min_still_psnr;
    hi_u32  max_qp;
    hi_u32  min_qp;
    hi_u32  max_i_qp;
    hi_u32  min_i_qp;
    hi_u32  min_qp_delta;
    hi_u32  motion_sensitivity;
    hi_bool save_bitrate_en;
} hi_venc_h264_avbr_param;

typedef struct {
    hi_u32  max_i_proportion;
    hi_u32  min_i_proportion;
    hi_s32  max_reencode_times;
    hi_bool qpmap_en;
    hi_u32  max_qp;
    hi_u32  min_qp;
    hi_u32  max_i_qp;
    hi_u32  min_i_qp;
    hi_s32  max_bit_percent;
    hi_s32  min_bit_percent;
    hi_s32  max_psnr_fluctuate;
    hi_s32  min_psnr_fluctuate;
} hi_venc_h264_qvbr_param;

typedef struct {
    hi_u32  max_i_proportion;
    hi_u32  min_i_proportion;
    hi_s32  max_reencode_times;
    hi_bool qpmap_en;
    hi_u32  max_qp;
    hi_u32  min_qp;
    hi_u32  max_i_qp;
    hi_u32  min_i_qp;
    hi_u32  min_qp_delta;
    hi_u32  max_qp_delta;
    hi_u32  extra_bit_percent;
    hi_u32  long_term_stats_time_unit;
    hi_bool save_bitrate_en;
} hi_venc_h264_cvbr_param;

typedef struct {
    hi_u32 max_qfactor;
    hi_u32 min_qfactor;
} hi_venc_mjpeg_cbr_param;

typedef struct {
    hi_s32 chg_pos;
    hi_u32 max_qfactor;
    hi_u32 min_qfactor;
} hi_venc_mjpeg_vbr_param;

typedef struct {
    hi_u32  max_i_proportion;
    hi_u32  min_i_proportion;
    hi_u32  max_qp;
    hi_u32  min_qp;
    hi_u32  max_i_qp;
    hi_u32  min_i_qp;
    hi_s32  max_reencode_times;
    hi_bool qpmap_en;
    hi_venc_rc_qpmap_mode qpmap_mode;
} hi_venc_h265_cbr_param;

typedef struct {
    hi_s32  chg_pos;
    hi_u32  max_i_proportion;
    hi_u32  min_i_proportion;
    hi_s32  max_reencode_times;
    hi_u32  max_qp;
    hi_u32  min_qp;
    hi_u32  max_i_qp;
    hi_u32  min_i_qp;
    hi_bool qpmap_en;
    hi_venc_rc_qpmap_mode qpmap_mode;
} hi_venc_h265_vbr_param;

typedef struct {
    hi_s32  chg_pos;
    hi_u32  max_i_proportion;
    hi_u32  min_i_proportion;
    hi_s32  max_reencode_times;
    hi_s32  min_still_percent;
    hi_u32  max_still_qp;
    hi_u32  min_still_psnr;
    hi_u32  max_qp;
    hi_u32  min_qp;
    hi_u32  max_i_qp;
    hi_u32  min_i_qp;
    hi_u32  min_qp_delta;
    hi_u32  motion_sensitivity;
    hi_bool qpmap_en;
    hi_venc_rc_qpmap_mode qpmap_mode;
} hi_venc_h265_avbr_param;

typedef struct {
    hi_u32  max_i_proportion;
    hi_u32  min_i_proportion;
    hi_s32  max_reencode_times;
    hi_bool qpmap_en;
    hi_venc_rc_qpmap_mode qpmap_mode;
    hi_u32  max_qp;
    hi_u32  min_qp;
    hi_u32  max_i_qp;
    hi_u32  min_i_qp;
    hi_s32  max_bit_percent;
    hi_s32  min_bit_percent;
    hi_s32  max_psnr_fluctuate;
    hi_s32  min_psnr_fluctuate;
} hi_venc_h265_qvbr_param;

typedef struct {
    hi_u32  max_i_proportion;
    hi_u32  min_i_proportion;
    hi_s32  max_reencode_times;
    hi_bool qpmap_en;
    hi_venc_rc_qpmap_mode qpmap_mode;
    hi_u32  max_qp;
    hi_u32  min_qp;
    hi_u32  max_i_qp;
    hi_u32  min_i_qp;
    hi_u32  min_qp_delta;
    hi_u32  max_qp_delta;
    hi_u32  extra_bit_percent;
    hi_u32  long_term_stats_time_unit;
} hi_venc_h265_cvbr_param;

typedef struct {
    hi_bool detect_scene_chg_en;
    hi_bool adapt_insert_idr_frame_en;
}  hi_venc_scene_chg_detect;

typedef struct {
    hi_u32 threshold_i[HI_VENC_TEXTURE_THRESHOLD_SIZE];
    hi_u32 threshold_p[HI_VENC_TEXTURE_THRESHOLD_SIZE];
    hi_u32 threshold_b[HI_VENC_TEXTURE_THRESHOLD_SIZE];
    hi_u32 direction;
    hi_u32 row_qp_delta;
    hi_s32 first_frame_start_qp;
    hi_venc_scene_chg_detect scene_chg_detect;
    union {
        hi_venc_h264_cbr_param  h264_cbr_param; /* AUTO:hi_venc_rc_mode:HI_VENC_RC_MODE_H264_CBR; */
        hi_venc_h264_vbr_param  h264_vbr_param; /* AUTO:hi_venc_rc_mode:HI_VENC_RC_MODE_H264_VBR; */
        hi_venc_h264_avbr_param h264_avbr_param; /* AUTO:hi_venc_rc_mode:HI_VENC_RC_MODE_H264_AVBR; */
        hi_venc_h264_qvbr_param h264_qvbr_param; /* AUTO:hi_venc_rc_mode:HI_VENC_RC_MODE_H264_QVBR; */
        hi_venc_h264_cvbr_param h264_cvbr_param; /* AUTO:hi_venc_rc_mode:HI_VENC_RC_MODE_H264_CVBR; */
        hi_venc_h265_cbr_param  h265_cbr_param; /* AUTO:hi_venc_rc_mode:HI_VENC_RC_MODE_H265_CBR; */
        hi_venc_h265_vbr_param  h265_vbr_param; /* AUTO:hi_venc_rc_mode:HI_VENC_RC_MODE_H265_VBR; */
        hi_venc_h265_avbr_param h265_avbr_param; /* AUTO:hi_venc_rc_mode:HI_VENC_RC_MODE_H265_AVBR; */
        hi_venc_h265_qvbr_param h265_qvbr_param; /* AUTO:hi_venc_rc_mode:HI_VENC_RC_MODE_H265_QVBR; */
        hi_venc_h265_cvbr_param h265_cvbr_param; /* AUTO:hi_venc_rc_mode:HI_VENC_RC_MODE_H265_CVBR; */
        hi_venc_mjpeg_cbr_param mjpeg_cbr_param; /* AUTO:hi_venc_rc_mode:HI_VENC_RC_MODE_MJPEG_CBR; */
        hi_venc_mjpeg_vbr_param mjpeg_vbr_param; /* AUTO:hi_venc_rc_mode:HI_VENC_RC_MODE_MJPEG_VBR; */
    };
}  hi_venc_rc_param;

typedef enum {
    HI_VENC_FRAME_LOST_NORMAL = 0,
    HI_VENC_FRAME_LOST_P_SKIP,
    HI_VENC_FRAME_LOST_BUTT,
} hi_venc_frame_lost_mode;

typedef struct {
    hi_bool enable;
    hi_u32  bit_rate_threshold;
    hi_venc_frame_lost_mode mode;
    hi_u32 frame_gap;
}  hi_venc_frame_lost_strategy;

typedef enum {
    HI_VENC_REENCODE_BIT_RATE_FIRST = 1,
    HI_VENC_REENCODE_FRAME_BITS_FIRST,
    HI_VENC_REENCODE_BUTT,
} hi_venc_reencode_priority;

typedef struct {
    hi_venc_super_frame_mode super_frame_mode;
    hi_u32 i_frame_bits_threshold;
    hi_u32 p_frame_bits_threshold;
    hi_u32 b_frame_bits_threshold;
    hi_venc_reencode_priority reencode_priority;
} hi_venc_super_frame_strategy;

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif
