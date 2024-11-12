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
    OT_VENC_RC_MODE_SVAC3_ABR,
    OT_VENC_RC_MODE_SVAC3_CBR,
    OT_VENC_RC_MODE_SVAC3_VBR,
    OT_VENC_RC_MODE_SVAC3_AVBR,
    OT_VENC_RC_MODE_SVAC3_QVBR,
    OT_VENC_RC_MODE_SVAC3_CVBR,
    OT_VENC_RC_MODE_SVAC3_FIXQP,
    OT_VENC_RC_MODE_SVAC3_QPMAP,
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
    td_u32 gop; /* RW; range:[1, 65536]; */
    td_u32 src_frame_rate; /* RW; range:[1, 240]; */
    td_u32 dst_frame_rate; /* RW; range:[1/64, src_frame_rate]; */
    td_u32 i_qp; /* RW; range:[0, 51]; for svac3: [0, 63]; */
    td_u32 p_qp; /* RW; range:[0, 51]; for svac3: [0, 63]; */
    td_u32 b_qp; /* RW; range:[0, 51]; for svac3: [0, 63]; */
} ot_venc_h264_fixqp;

typedef struct {
    td_u32 gop; /* RW; range:[1, 65536]; */
    td_u32 stats_time; /* RW; range:[1, 60]; */
    td_u32 src_frame_rate; /* RW; range:[1, 240]; */
    td_u32 dst_frame_rate; /* RW; range:[1/64, src_frame_rate]; */
    td_u32 bit_rate; /* RW; range:[OT_VENC_MIN_BITRATE, OT_VENC_MAX_BITRATE]; */
    td_u32 vbv_buf_delay; /* RW; range:[1, 100]; */
} ot_venc_h264_abr;

typedef struct {
    td_u32 gop; /* RW; range:[1, 65536]; */
    td_u32 stats_time; /* RW; range:[1, 60]; */
    td_u32 src_frame_rate; /* RW; range:[1, 240]; */
    td_u32 dst_frame_rate; /* RW; range:[1/64, src_frame_rate]; */
    td_u32 bit_rate; /* RW; range:[OT_VENC_MIN_BITRATE, OT_VENC_MAX_BITRATE]; */
} ot_venc_h264_cbr;

typedef struct {
    td_u32 gop; /* RW; range:[1, 65536]; */
    td_u32 stats_time; /* RW; range:[1, 60]; */
    td_u32 src_frame_rate; /* RW; range:[1, 240]; */
    td_u32 dst_frame_rate; /* RW; range:[1/64, src_frame_rate]; */
    td_u32 max_bit_rate; /* RW; range:[OT_VENC_MIN_BITRATE, OT_VENC_MAX_BITRATE]; */
} ot_venc_h264_vbr;

typedef struct {
    td_u32 gop; /* RW; range:[1, 65536]; */
    td_u32 stats_time; /* RW; range:[1, 60]; */
    td_u32 src_frame_rate; /* RW; range:[1, 240]; */
    td_u32 dst_frame_rate; /* RW; range:[1/64, src_frame_rate]; */
    td_u32 max_bit_rate; /* RW; range:[OT_VENC_MIN_BITRATE, OT_VENC_MAX_BITRATE]; */
    td_u32 short_term_stats_time; /* RW; range:[1, 120]; */
    td_u32 long_term_stats_time; /* RW; range:[1, 1440]; */
    td_u32 long_term_max_bit_rate; /* RW; range:[OT_VENC_MIN_BITRATE, max_bit_rate];
                                        for h265 and svac3: [2, max_bit_rate]; */
    td_u32 long_term_min_bit_rate; /* RW; range:[0, long_term_max_bit_rate]; */
} ot_venc_h264_cvbr;

typedef struct {
    td_u32 gop; /* RW; range:[1, 65536]; */
    td_u32 stats_time; /* RW; range:[1, 60]; */
    td_u32 src_frame_rate; /* RW; range:[1, 240]; */
    td_u32 dst_frame_rate; /* RW; range:[1/64, src_frame_rate]; */
    td_u32 max_bit_rate; /* RW; range:[OT_VENC_MIN_BITRATE, OT_VENC_MAX_BITRATE]; */
} ot_venc_h264_avbr;

typedef struct {
    td_u32 gop; /* RW; range:[1, 65536]; */
    td_u32 stats_time; /* RW; range:[1, 60]; */
    td_u32 src_frame_rate; /* RW; range:[1, 240]; */
    td_u32 dst_frame_rate; /* RW; range:[1/64, src_frame_rate]; */
} ot_venc_h264_qpmap;

typedef struct {
    td_u32 gop; /* RW; range:[1, 65536]; */
    td_u32 stats_time; /* RW; range:[1, 60]; */
    td_u32 src_frame_rate; /* RW; range:[1, 240]; */
    td_u32 dst_frame_rate; /* RW; range:[1/64, src_frame_rate]; */
    td_u32 target_bit_rate; /* RW; range:[OT_VENC_MIN_BITRATE, OT_VENC_MAX_BITRATE]; */
} ot_venc_h264_qvbr;

typedef struct {
    td_u32 gop; /* RW; range:[1, 65536]; */
    td_u32 stats_time; /* RW; range:[1, 60]; */
    td_u32 src_frame_rate; /* RW; range:[1, 240]; */
    td_u32 dst_frame_rate; /* RW; range:[1/64, src_frame_rate]; */
    ot_venc_rc_qpmap_mode qpmap_mode;
} ot_venc_h265_qpmap;

typedef ot_venc_h264_abr   ot_venc_h265_abr;
typedef ot_venc_h264_cbr   ot_venc_h265_cbr;
typedef ot_venc_h264_vbr   ot_venc_h265_vbr;
typedef ot_venc_h264_avbr  ot_venc_h265_avbr;
typedef ot_venc_h264_fixqp ot_venc_h265_fixqp;
typedef ot_venc_h264_qvbr  ot_venc_h265_qvbr;
typedef ot_venc_h264_cvbr  ot_venc_h265_cvbr;

typedef ot_venc_h265_abr   ot_venc_svac3_abr;
typedef ot_venc_h265_cbr   ot_venc_svac3_cbr;
typedef ot_venc_h265_vbr   ot_venc_svac3_vbr;
typedef ot_venc_h265_avbr  ot_venc_svac3_avbr;
typedef ot_venc_h265_fixqp ot_venc_svac3_fixqp;
typedef ot_venc_h265_qvbr  ot_venc_svac3_qvbr;
typedef ot_venc_h265_cvbr  ot_venc_svac3_cvbr;
typedef ot_venc_h265_qpmap ot_venc_svac3_qpmap;

typedef struct {
    td_u32 src_frame_rate; /* RW; range:[1, 240]; */
    td_u32 dst_frame_rate; /* RW; range:[1/64, src_frame_rate]; */
    td_u32 qfactor; /* RW; range:[1, 99]; */
} ot_venc_mjpeg_fixqp;

typedef struct {
    td_u32 stats_time; /* RW; range:[1, 60]; */
    td_u32 src_frame_rate; /* RW; range:[1, 240]; */
    td_u32 dst_frame_rate; /* RW; range:[1/64, src_frame_rate]; */
    td_u32 bit_rate; /* RW; range:[OT_VENC_MIN_BITRATE, OT_VENC_MAX_BITRATE]; */
} ot_venc_mjpeg_cbr;

typedef struct {
    td_u32 stats_time; /* RW; range:[1, 60]; */
    td_u32 src_frame_rate; /* RW; range:[1, 240]; */
    td_u32 dst_frame_rate; /* RW; range:[1/64, src_frame_rate]; */
    td_u32 max_bit_rate; /* RW; range:[OT_VENC_MIN_BITRATE, OT_VENC_MAX_BITRATE]; */
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

        ot_venc_svac3_abr   svac3_abr; /* AUTO:ot_venc_rc_mode:OT_VENC_RC_MODE_SVAC3_ABR; */
        ot_venc_svac3_cbr   svac3_cbr; /* AUTO:ot_venc_rc_mode:OT_VENC_RC_MODE_SVAC3_CBR; */
        ot_venc_svac3_vbr   svac3_vbr; /* AUTO:ot_venc_rc_mode:OT_VENC_RC_MODE_SVAC3_VBR; */
        ot_venc_svac3_avbr  svac3_avbr; /* AUTO:ot_venc_rc_mode:OT_VENC_RC_MODE_SVAC3_AVBR; */
        ot_venc_svac3_qvbr  svac3_qvbr; /* AUTO:ot_venc_rc_mode:OT_VENC_RC_MODE_SVAC3_QVBR; */
        ot_venc_svac3_cvbr  svac3_cvbr; /* AUTO:ot_venc_rc_mode:OT_VENC_RC_MODE_SVAC3_CVBR; */
        ot_venc_svac3_fixqp svac3_fixqp; /* AUTO:ot_venc_rc_mode:OT_VENC_RC_MODE_SVAC3_FIXQP; */
        ot_venc_svac3_qpmap svac3_qpmap; /* AUTO:ot_venc_rc_mode:OT_VENC_RC_MODE_SVAC3_QPMAP; */
    };
} ot_venc_rc_attr;

typedef enum {
    OT_VENC_SUPER_FRAME_NONE = 0,                  /* sdk don't care super frame */
    OT_VENC_SUPER_FRAME_DISCARD,                   /* the super frame is discarded */
    OT_VENC_SUPER_FRAME_REENCODE,                  /* the super frame is re-encode */
    OT_VENC_SUPER_FRAME_BUTT
} ot_venc_super_frame_mode;

typedef struct {
    td_u32  max_p_qp; /* RW; range:[0, 51]; */
    td_u32  min_p_qp; /* RW; range:[0, max_p_qp]; */
    td_u32  max_i_qp; /* RW; range:[0, 51]; */
    td_u32  min_i_qp; /* RW; range:[0, max_i_qp]; */
    td_u32  max_vi_qp; /* RW; range:[0, 51], only use for smartp; */
    td_u32  min_vi_qp; /* RW; range:[0, 51], only use for smartp; */
    td_s32  max_reencode_times; /* RW; range:[0, 3]; */
    td_bool qpmap_en; /* RW; range:[0, 1]; */
} ot_venc_h264_abr_param;

typedef struct {
    td_u32  max_i_proportion; /* RW; range:[min_i_proportion, 100]; */
    td_u32  min_i_proportion; /* RW; range:[1, 100]; */
    td_u32  max_qp; /* RW; range:[0, 51]; */
    td_u32  min_qp; /* RW; range:[0, max_qp]; */
    td_u32  max_i_qp; /* RW; range:[0, 51]; */
    td_u32  min_i_qp; /* RW; range:[0, max_i_qp]; */
    td_u32  max_vi_qp; /* RW; range:[0, 51], only use for smartp; */
    td_u32  min_vi_qp; /* RW; range:[0, 51], only use for smartp; */
    td_s32  max_reencode_times; /* RW; range:[0, 3]; */
    td_bool qpmap_en; /* RW; range:[0, 1]; */
} ot_venc_h264_cbr_param;

typedef struct {
    td_s32  chg_pos; /* RW; range:[50, 100]; */
    td_u32  max_i_proportion; /* RW; range:[min_i_proportion, 100]; */
    td_u32  min_i_proportion; /* RW; range:[1, 100]; */
    td_s32  max_reencode_times; /* RW; range:[0, 3]; */
    td_bool qpmap_en; /* RW; range:[0, 1]; */
    td_u32  max_qp; /* RW; range:[0, 51]; */
    td_u32  min_qp; /* RW; range:[0, max_qp]; */
    td_u32  max_i_qp; /* RW; range:[0, 51]; */
    td_u32  min_i_qp; /* RW; range:[0, max_i_qp]; */
    td_u32  max_vi_qp; /* RW; range:[0, 51], only use for smartp; */
    td_u32  min_vi_qp; /* RW; range:[0, 51], only use for smartp; */
} ot_venc_h264_vbr_param;

typedef struct {
    td_s32  chg_pos; /* RW; range:[50, 100]; */
    td_u32  max_i_proportion; /* RW; range:[min_i_proportion, 100]; */
    td_u32  min_i_proportion; /* RW; range:[1, 100]; */
    td_s32  max_reencode_times; /* RW; range:[0, 3]; */
    td_bool qpmap_en; /* RW; range:[0, 1]; */
    td_s32  min_still_percent; /* RW; range:[5, 100]; */
    td_u32  max_still_qp; /* RW; range:[min_i_qp, max_i_qp]; */
    td_u32  min_still_psnr; /* R; invalid, can only be set to 0; */
    td_u32  max_qp; /* RW; range:[0, 51]; */
    td_u32  min_qp; /* RW; range:[0, max_qp]; */
    td_u32  max_i_qp; /* RW; range:[0, 51]; */
    td_u32  min_i_qp; /* RW; range:[0, max_i_qp]; */
    td_u32  max_vi_qp; /* RW; range:[0, 51], only use for smartp; */
    td_u32  min_vi_qp; /* RW; range:[0, 51], only use for smartp; */
    td_u32  min_qp_delta; /* RW; range:[0, 4]; */
    td_u32  motion_sensitivity; /* RW; range:[0, 100]; */
    td_bool save_bitrate_en; /* RW; range:[0, 1]; */
} ot_venc_h264_avbr_param;

typedef struct {
    td_u32  max_i_proportion; /* RW; range:[min_i_proportion, 100]; */
    td_u32  min_i_proportion; /* RW; range:[1, 100]; */
    td_s32  max_reencode_times; /* RW; range:[0, 3]; */
    td_bool qpmap_en; /* RW; range:[0, 1]; */
    td_u32  max_qp; /* RW; range:[min_qp, 51]; */
    td_u32  min_qp; /* RW; range:[0, 51]; */
    td_u32  max_i_qp; /* RW; range:[min_i_qp, 51]; */
    td_u32  min_i_qp; /* RW; range:[0, 51]; */
    td_u32  max_vi_qp; /* RW; range:[0, 51], only use for smartp; */
    td_u32  min_vi_qp; /* RW; range:[0, 51], only use for smartp; */
    td_s32  max_bit_percent; /* RW; range:[min_bit_percent, 180]; */
    td_s32  min_bit_percent; /* RW; range:[30, 180]; */
    td_s32  max_psnr_fluctuate; /* RW; range:[min_psnr_fluctuate, 40]; */
    td_s32  min_psnr_fluctuate; /* RW; range:[18, 40]; */
} ot_venc_h264_qvbr_param;

typedef struct {
    td_u32  max_i_proportion; /* RW; range:[min_i_proportion, 100]; */
    td_u32  min_i_proportion; /* RW; range:[1, 100]; */
    td_s32  max_reencode_times; /* RW; range:[0, 3]; */
    td_bool qpmap_en; /* RW; range:[0, 1]; */
    td_u32  max_qp; /* RW; range:[min_qp, 51]; */
    td_u32  min_qp; /* RW; range:[0, 51]; */
    td_u32  max_i_qp; /* RW; range:[min_i_qp, 51]; */
    td_u32  min_i_qp; /* RW; range:[0, 51]; */
    td_u32  max_vi_qp; /* RW; range:[0, 51], only use for smartp; */
    td_u32  min_vi_qp; /* RW; range:[0, 51], only use for smartp; */
    td_u32  min_qp_delta; /* RW; range:[0, 4]; */
    td_u32  max_qp_delta; /* RW; range:[0, 4]; */
    td_u32  extra_bit_percent; /* RW; range:[0, 1000]; */
    td_u32  long_term_stats_time_unit; /* RW; range:[1, 1800]; */
    td_bool save_bitrate_en; /* RW; range:[0, 1]; */
} ot_venc_h264_cvbr_param;

typedef struct {
    td_u32 max_qfactor; /* RW; range:[1, 99]; */
    td_u32 min_qfactor; /* RW; range:[1, max_qfactor]; */
} ot_venc_mjpeg_cbr_param;

typedef struct {
    td_s32 chg_pos; /* RW; range:[50, 100]; */
    td_u32 max_qfactor; /* RW; range:[1, 99]; */
    td_u32 min_qfactor; /* RW; range:[1, max_qfactor]; */
} ot_venc_mjpeg_vbr_param;

typedef struct {
    td_u32  max_p_qp; /* RW; range:[0, 51]; for svac3: [0, 63]; */
    td_u32  min_p_qp; /* RW; range:[0, max_p_qp]; */
    td_u32  max_i_qp; /* RW; range:[0, 51]; for svac3: [0, 63]; */
    td_u32  min_i_qp; /* RW; range:[0, max_i_qp]; */
    td_u32  max_vi_qp; /* RW; range:[0, 51], for svac3: [0, 63]; only use for smartp; */
    td_u32  min_vi_qp; /* RW; range:[0, 51], for svac3: [0, 63]; only use for smartp; */
    td_s32  max_reencode_times; /* RW; range:[0, 3]; */
    td_bool qpmap_en; /* RW; range:[0, 1]; */
    ot_venc_rc_qpmap_mode qpmap_mode;
} ot_venc_h265_abr_param;

typedef struct {
    td_u32  max_i_proportion; /* RW; range:[min_i_proportion, 100]; */
    td_u32  min_i_proportion; /* RW; range:[1, 100]; */
    td_u32  max_qp; /* RW; range:[0, 51]; for svac3: [0, 63]; */
    td_u32  min_qp; /* RW; range:[0, max_qp]; */
    td_u32  max_i_qp; /* RW; range:[0, 51]; for svac3: [0, 63]; */
    td_u32  min_i_qp; /* RW; range:[0, max_i_qp]; */
    td_u32  max_vi_qp; /* RW; range:[0, 51], for svac3: [0, 63]; only use for smartp; */
    td_u32  min_vi_qp; /* RW; range:[0, 51], for svac3: [0, 63]; only use for smartp; */
    td_s32  max_reencode_times;  /* RW; range:[0, 3]; */
    td_bool qpmap_en; /* RW; range:[0, 1]; */
    ot_venc_rc_qpmap_mode qpmap_mode;
} ot_venc_h265_cbr_param;

typedef struct {
    td_s32  chg_pos; /* RW; range:[50, 100]; */
    td_u32  max_i_proportion; /* RW; range:[min_i_proportion, 100]; */
    td_u32  min_i_proportion; /* RW; range:[1, 100]; */
    td_s32  max_reencode_times; /* RW; range:[0, 3]; */
    td_u32  max_qp; /* RW; range:[0, 51]; for svac3: [0, 63]; */
    td_u32  min_qp; /* RW; range:[0, max_qp]; */
    td_u32  max_i_qp; /* RW; range:[0, 51]; for svac3: [0, 63]; */
    td_u32  min_i_qp; /* RW; range:[0, max_i_qp]; */
    td_u32  max_vi_qp; /* RW; range:[0, 51]; for svac3: [0, 63]; only use for smartp; */
    td_u32  min_vi_qp; /* RW; range:[0, 51], for svac3: [0, 63]; only use for smartp; */
    td_bool qpmap_en; /* RW; range:[0, 1]; */
    ot_venc_rc_qpmap_mode qpmap_mode;
} ot_venc_h265_vbr_param;

typedef struct {
    td_s32  chg_pos; /* RW; range:[50, 100]; */
    td_u32  max_i_proportion; /* RW; range:[min_i_proportion, 100]; */
    td_u32  min_i_proportion; /* RW; range:[1, 100]; */
    td_s32  max_reencode_times; /* RW; range:[0, 3]; */
    td_s32  min_still_percent; /* RW; range:[5, 100]; */
    td_u32  max_still_qp; /* RW; range:[min_i_qp, max_i_qp]; */
    td_u32  min_still_psnr; /* R; invalid, can only be set to 0 */
    td_u32  max_qp; /* RW; range:[0, 51]; for svac3: [0, 63]; */
    td_u32  min_qp; /* RW; range:[0, max_qp]; */
    td_u32  max_i_qp; /* RW; range:[0, 51]; for svac3: [0, 63]; */
    td_u32  min_i_qp; /* RW; range:[0, max_i_qp]; */
    td_u32  max_vi_qp; /* RW; range:[0, 51], for svac3: [0, 63]; only use for smartp; */
    td_u32  min_vi_qp; /* RW; range:[0, 51], for svac3: [0, 63]; only use for smartp; */
    td_u32  min_qp_delta; /* RW; range:[0, 4]; */
    td_u32  motion_sensitivity; /* RW; range:[0, 100]; */
    td_bool qpmap_en; /* RW; range:[0, 1]; */
    ot_venc_rc_qpmap_mode qpmap_mode;
} ot_venc_h265_avbr_param;

typedef struct {
    td_u32  max_i_proportion; /* RW; range:[min_i_proportion, 100]; */
    td_u32  min_i_proportion; /* RW; range:[1, 100]; */
    td_s32  max_reencode_times; /* RW; range:[0, 3]; */
    td_bool qpmap_en; /* RW; range:[0, 1]; */
    ot_venc_rc_qpmap_mode qpmap_mode;
    td_u32  max_qp; /* RW; range:[min_qp, 51]; for svac3: [min_qp, 63]; */
    td_u32  min_qp; /* RW; range:[0, 51]; for svac3: [0, 63]; */
    td_u32  max_i_qp; /* RW; range:[min_i_qp, 51]; for svac3: [min_i_qp, 63]; */
    td_u32  min_i_qp; /* RW; range:[0, 51]; for svac3: [0, 63]; */
    td_u32  max_vi_qp; /* RW; range:[0, 51], for svac3: [0, 63]; only use for smartp; */
    td_u32  min_vi_qp; /* RW; range:[0, 51], for svac3: [0, 63]; only use for smartp; */
    td_s32  max_bit_percent; /* RW; range:[min_bit_percent, 180]; */
    td_s32  min_bit_percent; /* RW; range:[30, 180]; */
    td_s32  max_psnr_fluctuate; /* RW; range:[min_psnr_fluctuate, 40]; */
    td_s32  min_psnr_fluctuate; /* RW; range:[18, 40]; */
} ot_venc_h265_qvbr_param;

typedef struct {
    td_u32  max_i_proportion; /* RW; range:[min_i_proportion, 100]; */
    td_u32  min_i_proportion; /* RW; range:[1, 100]; */
    td_s32  max_reencode_times; /* RW; range:[0, 3]; */
    td_bool qpmap_en; /* RW; range:[0, 1]; */
    ot_venc_rc_qpmap_mode qpmap_mode;
    td_u32  max_qp; /* RW; range:[min_qp, 51]; for svac3: [min_qp, 63]; */
    td_u32  min_qp; /* RW; range:[0, 51]; for svac3: [0, 63]; */
    td_u32  max_i_qp; /* RW; range:[min_i_qp, 51]; for svac3: [min_i_qp, 63]; */
    td_u32  min_i_qp; /* RW; range:[0, 51]; for svac3: [0, 63]; */
    td_u32  max_vi_qp; /* RW; range:[0, 51], for svac3: [0, 63]; only use for smartp; */
    td_u32  min_vi_qp; /* RW; range:[0, 51], for svac3: [0, 63]; only use for smartp; */
    td_u32  min_qp_delta; /* RW; range:[0, 4]; */
    td_u32  max_qp_delta; /* RW; range:[0, 4]; */
    td_u32  extra_bit_percent; /* RW; range:[0, 1000]; */
    td_u32  long_term_stats_time_unit; /* RW; range:[1, 1800]; */
} ot_venc_h265_cvbr_param;

typedef ot_venc_h265_abr_param   ot_venc_svac3_abr_param;
typedef ot_venc_h265_cbr_param   ot_venc_svac3_cbr_param;
typedef ot_venc_h265_vbr_param   ot_venc_svac3_vbr_param;
typedef ot_venc_h265_avbr_param  ot_venc_svac3_avbr_param;
typedef ot_venc_h265_qvbr_param  ot_venc_svac3_qvbr_param;
typedef ot_venc_h265_cvbr_param  ot_venc_svac3_cvbr_param;

typedef struct {
    td_bool detect_scene_chg_en; /* RW; range:[0, 1]; */
    td_bool adapt_insert_idr_frame_en; /* RW; range:[0, 1]; */
}  ot_venc_scene_chg_detect;

typedef struct {
    td_u32 threshold_i[OT_VENC_TEXTURE_THRESHOLD_SIZE]; /* RW; range:[0, 255]; */
    td_u32 threshold_p[OT_VENC_TEXTURE_THRESHOLD_SIZE]; /* RW; range:[0, 255]; */
    td_u32 threshold_b[OT_VENC_TEXTURE_THRESHOLD_SIZE]; /* RW; range:[0, 255]; */
    td_u32 direction; /* RW; range:[0, 16]; */
    td_u32 row_qp_delta; /* RW; range:[0, 10]; */
    td_s32 first_frame_start_qp; /* RW; range:[min_i_qp, max_i_qp] and -1; if first_frame_start_qp is -1,
                                    the starting QP of the first frame is calculated internally by the
                                    encoder.if it is another legal value, the value is specified
                                    by the user as the starting QP of the first frame; */
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

        ot_venc_svac3_abr_param svac3_abr_param; /* AUTO:ot_venc_rc_mode:OT_VENC_RC_MODE_SVAC3_ABR; */
        ot_venc_svac3_cbr_param svac3_cbr_param; /* AUTO:ot_venc_rc_mode:OT_VENC_RC_MODE_SVAC3_CBR; */
        ot_venc_svac3_vbr_param svac3_vbr_param; /* AUTO:ot_venc_rc_mode:OT_VENC_RC_MODE_SVAC3_VBR; */
        ot_venc_svac3_avbr_param svac3_avbr_param; /* AUTO:ot_venc_rc_mode:OT_VENC_RC_MODE_SVAC3_AVBR; */
        ot_venc_svac3_qvbr_param svac3_qvbr_param; /* AUTO:ot_venc_rc_mode:OT_VENC_RC_MODE_SVAC3_QVBR; */
        ot_venc_svac3_cvbr_param svac3_cvbr_param; /* AUTO:ot_venc_rc_mode:OT_VENC_RC_MODE_SVAC3_CVBR; */
    };
}  ot_venc_rc_param;

typedef enum {
    OT_VENC_FRAME_LOST_NORMAL = 0,
    OT_VENC_FRAME_LOST_P_SKIP,
    OT_VENC_FRAME_LOST_BUTT,
} ot_venc_frame_lost_mode;

typedef struct {
    td_bool enable; /* RW; range:[0, 1]; */
    td_u32  bit_rate_threshold; /* RW; range:greater than or equal to 65536 (64k) ; */
    ot_venc_frame_lost_mode mode;
    td_u32 frame_gap; /* RW; range:[0, 65535]; */
}  ot_venc_frame_lost_strategy;

typedef enum {
    OT_VENC_REENCODE_BIT_RATE_FIRST = 1,
    OT_VENC_REENCODE_FRAME_BITS_FIRST,
    OT_VENC_REENCODE_BUTT,
} ot_venc_reencode_priority;

typedef struct {
    ot_venc_super_frame_mode super_frame_mode;
    td_u32 i_frame_bits_threshold; /* RW; range:[greater than or equal to 0]; */
    td_u32 p_frame_bits_threshold; /* RW; range:[greater than or equal to 0]; */
    td_u32 b_frame_bits_threshold; /* RW; range:[greater than or equal to 0]; */
    ot_venc_reencode_priority reencode_priority;
} ot_venc_super_frame_strategy;

typedef struct {
    td_u32 clear_stats_after_set_attr; /* RW; range:[0, 1];
                                          0: After setting the channel code rate, the frame rate and
                                          code rate statistics of RC are not cleared;
                                          1: Clears the statistics of the RC frame rate and bit rate
                                          after the channel bit rate is set; */
} ot_venc_rc_adv_param;

typedef struct {
    td_bool enable;
    td_s32  qp_delta[OT_VENC_MAX_HIERARCHY_NUM];
    td_s32  frame_num[OT_VENC_MAX_HIERARCHY_NUM];
} ot_venc_hierarchical_qp;

typedef struct {
    td_bool enable; /* RW; range:[0, 1]; */
    td_s32 strength0; /* RW; range:[0, 35]; */
    td_s32 strength1; /* RW; range:[0, 35]; */
} ot_venc_debreath_effect;

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif
