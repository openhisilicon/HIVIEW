/*
  Copyright (c), 2001-2024, Shenshu Tech. Co., Ltd.
 */
#ifndef OT_COMMON_RC_H
#define OT_COMMON_RC_H

#include "ot_type.h"
#include "ot_defines.h"

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif

#define OT_VENC_TEXTURE_THRESHOLD_SIZE 16
#define OT_VENC_MAX_HIERARCHY_NUM 4

/* rc mode */
typedef enum {
    OT_VENC_RC_MODE_H264_ABR = 1,
    OT_VENC_RC_MODE_H264_CBR,
    OT_VENC_RC_MODE_H264_VBR,
    OT_VENC_RC_MODE_H264_AVBR,
    OT_VENC_RC_MODE_H264_QVBR,
    OT_VENC_RC_MODE_H264_CVBR,
    OT_VENC_RC_MODE_H264_FIXQP,
    OT_VENC_RC_MODE_H264_QPMAP,
    OT_VENC_RC_MODE_MJPEG_CBR,
    OT_VENC_RC_MODE_MJPEG_VBR,
    OT_VENC_RC_MODE_MJPEG_FIXQP,
    OT_VENC_RC_MODE_H265_ABR,
    OT_VENC_RC_MODE_H265_CBR,
    OT_VENC_RC_MODE_H265_VBR,
    OT_VENC_RC_MODE_H265_AVBR,
    OT_VENC_RC_MODE_H265_QVBR,
    OT_VENC_RC_MODE_H265_CVBR,
    OT_VENC_RC_MODE_H265_FIXQP,
    OT_VENC_RC_MODE_H265_QPMAP,
    OT_VENC_RC_MODE_BUTT,
} ot_venc_rc_mode;

/* qpmap mode */
typedef enum {
    OT_VENC_RC_QPMAP_MODE_MEAN_QP = 0,
    OT_VENC_RC_QPMAP_MODE_MIN_QP,
    OT_VENC_RC_QPMAP_MODE_MAX_QP,
    OT_VENC_RC_QPMAP_MODE_BUTT,
} ot_venc_rc_qpmap_mode;

typedef struct {
    td_u32 gop;
    td_u32 src_frame_rate;
    td_u32 dst_frame_rate;
    td_u32 i_qp;
    td_u32 p_qp;
    td_u32 b_qp;
} ot_venc_h264_fixqp;

typedef struct {
    td_u32 gop;
    td_u32 stats_time;
    td_u32 src_frame_rate;
    td_u32 dst_frame_rate ;
    td_u32 bit_rate;
    td_u32 vbv_buf_delay;
} ot_venc_h264_abr;

typedef struct {
    td_u32 gop;
    td_u32 stats_time;
    td_u32 src_frame_rate;
    td_u32 dst_frame_rate;
    td_u32 bit_rate;
} ot_venc_h264_cbr;

typedef struct {
    td_u32 gop;
    td_u32 stats_time;
    td_u32 src_frame_rate;
    td_u32 dst_frame_rate;
    td_u32 max_bit_rate;
} ot_venc_h264_vbr;

typedef struct {
    td_u32 gop;
    td_u32 stats_time;
    td_u32 src_frame_rate;
    td_u32 dst_frame_rate;
    td_u32 max_bit_rate;
    td_u32 short_term_stats_time;
    td_u32 long_term_stats_time;
    td_u32 long_term_max_bit_rate;
    td_u32 long_term_min_bit_rate;
} ot_venc_h264_cvbr;

typedef struct {
    td_u32 gop;
    td_u32 stats_time;
    td_u32 src_frame_rate;
    td_u32 dst_frame_rate;
    td_u32 max_bit_rate;
} ot_venc_h264_avbr;

typedef struct {
    td_u32 gop;
    td_u32 stats_time;
    td_u32 src_frame_rate;
    td_u32 dst_frame_rate;
} ot_venc_h264_qpmap;

typedef struct {
    td_u32 gop;
    td_u32 stats_time;
    td_u32 src_frame_rate;
    td_u32 dst_frame_rate;
    td_u32 target_bit_rate;
} ot_venc_h264_qvbr;

typedef struct {
    td_u32 gop;
    td_u32 stats_time;
    td_u32 src_frame_rate;
    td_u32 dst_frame_rate;
    ot_venc_rc_qpmap_mode qpmap_mode;
} ot_venc_h265_qpmap;

typedef ot_venc_h264_abr   ot_venc_h265_abr;
typedef ot_venc_h264_cbr   ot_venc_h265_cbr;
typedef ot_venc_h264_vbr   ot_venc_h265_vbr;
typedef ot_venc_h264_avbr  ot_venc_h265_avbr;
typedef ot_venc_h264_fixqp ot_venc_h265_fixqp;
typedef ot_venc_h264_qvbr  ot_venc_h265_qvbr;
typedef ot_venc_h264_cvbr  ot_venc_h265_cvbr;

typedef struct {
    td_u32 src_frame_rate;
    td_u32 dst_frame_rate;
    td_u32 qfactor;
} ot_venc_mjpeg_fixqp;

typedef struct {
    td_u32 stats_time;
    td_u32 src_frame_rate;
    td_u32 dst_frame_rate;
    td_u32 bit_rate;
} ot_venc_mjpeg_cbr;

typedef struct {
    td_u32 stats_time;
    td_u32 src_frame_rate;
    td_u32 dst_frame_rate;
    td_u32 max_bit_rate;
} ot_venc_mjpeg_vbr;

typedef struct {
    ot_venc_rc_mode rc_mode;
    union {
        ot_venc_h264_abr   h264_abr; /* AUTO:ot_venc_rc_mode:OT_VENC_RC_MODE_H264_ABR; */
        ot_venc_h264_cbr   h264_cbr; /* AUTO:ot_venc_rc_mode:OT_VENC_RC_MODE_H264_CBR; */
        ot_venc_h264_vbr   h264_vbr; /* AUTO:ot_venc_rc_mode:OT_VENC_RC_MODE_H264_VBR; */
        ot_venc_h264_avbr  h264_avbr; /* AUTO:ot_venc_rc_mode:OT_VENC_RC_MODE_H264_AVBR; */
        ot_venc_h264_qvbr  h264_qvbr; /* AUTO:ot_venc_rc_mode:OT_VENC_RC_MODE_H264_QVBR; */
        ot_venc_h264_cvbr  h264_cvbr; /* AUTO:ot_venc_rc_mode:OT_VENC_RC_MODE_H264_CVBR; */
        ot_venc_h264_fixqp h264_fixqp; /* AUTO:ot_venc_rc_mode:OT_VENC_RC_MODE_H264_FIXQP; */
        ot_venc_h264_qpmap h264_qpmap; /* AUTO:ot_venc_rc_mode:OT_VENC_RC_MODE_H264_QPMAP; */

        ot_venc_mjpeg_cbr   mjpeg_cbr; /* AUTO:ot_venc_rc_mode:OT_VENC_RC_MODE_MJPEG_CBR; */
        ot_venc_mjpeg_vbr   mjpeg_vbr; /* AUTO:ot_venc_rc_mode:OT_VENC_RC_MODE_MJPEG_VBR; */
        ot_venc_mjpeg_fixqp mjpeg_fixqp; /* AUTO:ot_venc_rc_mode:OT_VENC_RC_MODE_MJPEG_FIXQP; */

        ot_venc_h265_abr   h265_abr; /* AUTO:ot_venc_rc_mode:OT_VENC_RC_MODE_H265_ABR; */
        ot_venc_h265_cbr   h265_cbr; /* AUTO:ot_venc_rc_mode:OT_VENC_RC_MODE_H265_CBR; */
        ot_venc_h265_vbr   h265_vbr; /* AUTO:ot_venc_rc_mode:OT_VENC_RC_MODE_H265_VBR; */
        ot_venc_h265_avbr  h265_avbr; /* AUTO:ot_venc_rc_mode:OT_VENC_RC_MODE_H265_AVBR; */
        ot_venc_h265_qvbr  h265_qvbr; /* AUTO:ot_venc_rc_mode:OT_VENC_RC_MODE_H265_QVBR; */
        ot_venc_h265_cvbr  h265_cvbr; /* AUTO:ot_venc_rc_mode:OT_VENC_RC_MODE_H265_CVBR; */
        ot_venc_h265_fixqp h265_fixqp; /* AUTO:ot_venc_rc_mode:OT_VENC_RC_MODE_H265_FIXQP; */
        ot_venc_h265_qpmap h265_qpmap; /* AUTO:ot_venc_rc_mode:OT_VENC_RC_MODE_H265_QPMAP; */
    };
} ot_venc_rc_attr;

typedef enum {
    OT_VENC_SUPER_FRAME_NONE = 0,                  /* sdk don't care super frame */
    OT_VENC_SUPER_FRAME_DISCARD,                   /* the super frame is discarded */
    OT_VENC_SUPER_FRAME_REENCODE,                  /* the super frame is re-encode */
    OT_VENC_SUPER_FRAME_BUTT
} ot_venc_super_frame_mode;

typedef struct {
    td_u32  max_p_qp;
    td_u32  min_p_qp;
    td_u32  max_i_qp;
    td_u32  min_i_qp;
    td_u32  max_vi_qp;
    td_u32  min_vi_qp;
    td_s32  max_reencode_times;
    td_bool qpmap_en;
} ot_venc_h264_abr_param;

typedef struct {
    td_u32  max_i_proportion;
    td_u32  min_i_proportion;
    td_u32  max_qp;
    td_u32  min_qp;
    td_u32  max_i_qp;
    td_u32  min_i_qp;
    td_u32  max_vi_qp;
    td_u32  min_vi_qp;
    td_s32  max_reencode_times;
    td_bool qpmap_en;
} ot_venc_h264_cbr_param;

typedef struct {
    td_s32  chg_pos;
    td_u32  max_i_proportion;
    td_u32  min_i_proportion;
    td_s32  max_reencode_times;
    td_bool qpmap_en;
    td_u32  max_qp;
    td_u32  min_qp;
    td_u32  max_i_qp;
    td_u32  min_i_qp;
    td_u32  max_vi_qp;
    td_u32  min_vi_qp;
} ot_venc_h264_vbr_param;

typedef struct {
    td_s32  chg_pos;
    td_u32  max_i_proportion;
    td_u32  min_i_proportion;
    td_s32  max_reencode_times;
    td_bool qpmap_en;
    td_s32  min_still_percent;
    td_u32  max_still_qp;
    td_u32  min_still_psnr;
    td_u32  max_qp;
    td_u32  min_qp;
    td_u32  max_i_qp;
    td_u32  min_i_qp;
    td_u32  max_vi_qp;
    td_u32  min_vi_qp;
    td_u32  min_qp_delta;
    td_u32  motion_sensitivity;
    td_bool save_bitrate_en;
} ot_venc_h264_avbr_param;

typedef struct {
    td_u32  max_i_proportion;
    td_u32  min_i_proportion;
    td_s32  max_reencode_times;
    td_bool qpmap_en;
    td_u32  max_qp;
    td_u32  min_qp;
    td_u32  max_i_qp;
    td_u32  min_i_qp;
    td_u32  max_vi_qp;
    td_u32  min_vi_qp;
    td_s32  max_bit_percent;
    td_s32  min_bit_percent;
    td_s32  max_psnr_fluctuate;
    td_s32  min_psnr_fluctuate;
} ot_venc_h264_qvbr_param;

typedef struct {
    td_u32  max_i_proportion;
    td_u32  min_i_proportion;
    td_s32  max_reencode_times;
    td_bool qpmap_en;
    td_u32  max_qp;
    td_u32  min_qp;
    td_u32  max_i_qp;
    td_u32  min_i_qp;
    td_u32  max_vi_qp;
    td_u32  min_vi_qp;
    td_u32  min_qp_delta;
    td_u32  max_qp_delta;
    td_u32  extra_bit_percent;
    td_u32  long_term_stats_time_unit;
    td_bool save_bitrate_en;
} ot_venc_h264_cvbr_param;

typedef struct {
    td_u32 max_qfactor;
    td_u32 min_qfactor;
} ot_venc_mjpeg_cbr_param;

typedef struct {
    td_s32 chg_pos;
    td_u32 max_qfactor;
    td_u32 min_qfactor;
} ot_venc_mjpeg_vbr_param;

typedef struct {
    td_u32  max_p_qp;
    td_u32  min_p_qp;
    td_u32  max_i_qp;
    td_u32  min_i_qp;
    td_u32  max_vi_qp;
    td_u32  min_vi_qp;
    td_s32  max_reencode_times;
    td_bool qpmap_en;
    ot_venc_rc_qpmap_mode qpmap_mode;
} ot_venc_h265_abr_param;

typedef struct {
    td_u32  max_i_proportion;
    td_u32  min_i_proportion;
    td_u32  max_qp;
    td_u32  min_qp;
    td_u32  max_i_qp;
    td_u32  min_i_qp;
    td_u32  max_vi_qp;
    td_u32  min_vi_qp;
    td_s32  max_reencode_times;
    td_bool qpmap_en;
    ot_venc_rc_qpmap_mode qpmap_mode;
} ot_venc_h265_cbr_param;

typedef struct {
    td_s32  chg_pos;
    td_u32  max_i_proportion;
    td_u32  min_i_proportion;
    td_s32  max_reencode_times;
    td_u32  max_qp;
    td_u32  min_qp;
    td_u32  max_i_qp;
    td_u32  min_i_qp;
    td_u32  max_vi_qp;
    td_u32  min_vi_qp;
    td_bool qpmap_en;
    ot_venc_rc_qpmap_mode qpmap_mode;
} ot_venc_h265_vbr_param;

typedef struct {
    td_s32  chg_pos;
    td_u32  max_i_proportion;
    td_u32  min_i_proportion;
    td_s32  max_reencode_times;
    td_s32  min_still_percent;
    td_u32  max_still_qp;
    td_u32  min_still_psnr;
    td_u32  max_qp;
    td_u32  min_qp;
    td_u32  max_i_qp;
    td_u32  min_i_qp;
    td_u32  max_vi_qp;
    td_u32  min_vi_qp;
    td_u32  min_qp_delta;
    td_u32  motion_sensitivity;
    td_bool qpmap_en;
    ot_venc_rc_qpmap_mode qpmap_mode;
} ot_venc_h265_avbr_param;

typedef struct {
    td_u32  max_i_proportion;
    td_u32  min_i_proportion;
    td_s32  max_reencode_times;
    td_bool qpmap_en;
    ot_venc_rc_qpmap_mode qpmap_mode;
    td_u32  max_qp;
    td_u32  min_qp;
    td_u32  max_i_qp;
    td_u32  min_i_qp;
    td_u32  max_vi_qp;
    td_u32  min_vi_qp;
    td_s32  max_bit_percent;
    td_s32  min_bit_percent;
    td_s32  max_psnr_fluctuate;
    td_s32  min_psnr_fluctuate;
} ot_venc_h265_qvbr_param;

typedef struct {
    td_u32  max_i_proportion;
    td_u32  min_i_proportion;
    td_s32  max_reencode_times;
    td_bool qpmap_en;
    ot_venc_rc_qpmap_mode qpmap_mode;
    td_u32  max_qp;
    td_u32  min_qp;
    td_u32  max_i_qp;
    td_u32  min_i_qp;
    td_u32  max_vi_qp;
    td_u32  min_vi_qp;
    td_u32  min_qp_delta;
    td_u32  max_qp_delta;
    td_u32  extra_bit_percent;
    td_u32  long_term_stats_time_unit;
} ot_venc_h265_cvbr_param;

typedef struct {
    td_bool detect_scene_chg_en;
    td_bool adapt_insert_idr_frame_en;
}  ot_venc_scene_chg_detect;

typedef struct {
    td_u32 threshold_i[OT_VENC_TEXTURE_THRESHOLD_SIZE];
    td_u32 threshold_p[OT_VENC_TEXTURE_THRESHOLD_SIZE];
    td_u32 threshold_b[OT_VENC_TEXTURE_THRESHOLD_SIZE];
    td_u32 direction;
    td_u32 row_qp_delta;
    td_s32 first_frame_start_qp;
    ot_venc_scene_chg_detect scene_chg_detect;
    union {
        ot_venc_h264_abr_param  h264_abr_param; /* AUTO:ot_venc_rc_mode:OT_VENC_RC_MODE_H264_ABR; */
        ot_venc_h264_cbr_param  h264_cbr_param; /* AUTO:ot_venc_rc_mode:OT_VENC_RC_MODE_H264_CBR; */
        ot_venc_h264_vbr_param  h264_vbr_param; /* AUTO:ot_venc_rc_mode:OT_VENC_RC_MODE_H264_VBR; */
        ot_venc_h264_avbr_param h264_avbr_param; /* AUTO:ot_venc_rc_mode:OT_VENC_RC_MODE_H264_AVBR; */
        ot_venc_h264_qvbr_param h264_qvbr_param; /* AUTO:ot_venc_rc_mode:OT_VENC_RC_MODE_H264_QVBR; */
        ot_venc_h264_cvbr_param h264_cvbr_param; /* AUTO:ot_venc_rc_mode:OT_VENC_RC_MODE_H264_CVBR; */
        ot_venc_h265_abr_param  h265_abr_param; /* AUTO:ot_venc_rc_mode:OT_VENC_RC_MODE_H265_ABR; */
        ot_venc_h265_cbr_param  h265_cbr_param; /* AUTO:ot_venc_rc_mode:OT_VENC_RC_MODE_H265_CBR; */
        ot_venc_h265_vbr_param  h265_vbr_param; /* AUTO:ot_venc_rc_mode:OT_VENC_RC_MODE_H265_VBR; */
        ot_venc_h265_avbr_param h265_avbr_param; /* AUTO:ot_venc_rc_mode:OT_VENC_RC_MODE_H265_AVBR; */
        ot_venc_h265_qvbr_param h265_qvbr_param; /* AUTO:ot_venc_rc_mode:OT_VENC_RC_MODE_H265_QVBR; */
        ot_venc_h265_cvbr_param h265_cvbr_param; /* AUTO:ot_venc_rc_mode:OT_VENC_RC_MODE_H265_CVBR; */
        ot_venc_mjpeg_cbr_param mjpeg_cbr_param; /* AUTO:ot_venc_rc_mode:OT_VENC_RC_MODE_MJPEG_CBR; */
        ot_venc_mjpeg_vbr_param mjpeg_vbr_param; /* AUTO:ot_venc_rc_mode:OT_VENC_RC_MODE_MJPEG_VBR; */
    };
}  ot_venc_rc_param;

typedef enum {
    OT_VENC_FRAME_LOST_NORMAL = 0,
    OT_VENC_FRAME_LOST_P_SKIP,
    OT_VENC_FRAME_LOST_BUTT,
} ot_venc_frame_lost_mode;

typedef struct {
    td_bool enable;
    td_u32  bit_rate_threshold;
    ot_venc_frame_lost_mode mode;
    td_u32 frame_gap;
}  ot_venc_frame_lost_strategy;

typedef enum {
    OT_VENC_REENCODE_BIT_RATE_FIRST = 1,
    OT_VENC_REENCODE_FRAME_BITS_FIRST,
    OT_VENC_REENCODE_BUTT,
} ot_venc_reencode_priority;

typedef struct {
    ot_venc_super_frame_mode super_frame_mode;
    td_u32 i_frame_bits_threshold;
    td_u32 p_frame_bits_threshold;
    td_u32 b_frame_bits_threshold;
    ot_venc_reencode_priority reencode_priority;
} ot_venc_super_frame_strategy;

typedef struct {
    td_u32 clear_stats_after_set_attr;
} ot_venc_rc_adv_param;

typedef struct {
    td_bool enable;
    td_s32  qp_delta[OT_VENC_MAX_HIERARCHY_NUM];
    td_s32  frame_num[OT_VENC_MAX_HIERARCHY_NUM];
} ot_venc_hierarchical_qp;

typedef struct {
    td_bool enable;
    td_s32 strength0;
    td_s32 strength1;
} ot_venc_debreath_effect;

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif
