/*
 * Copyright (c) Hisilicon Technologies Co., Ltd. 2016-2019. All rights reserved.
 * Description: common venc
 * Author: Hisilicon multimedia software group
 * Create: 2016/11/15
 * History:
 */
#ifndef __HI_COMMON_VENC_H__
#define __HI_COMMON_VENC_H__

#include "hi_common_rc.h"
#include "hi_common.h"
#include "hi_common_video.h"
#include "hi_common_vb.h"

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif

/* invlalid channel ID */
#define HI_ERR_VENC_INVALID_CHN_ID HI_DEFINE_ERR(HI_ID_VENC, HI_ERR_LEVEL_ERROR, HI_ERR_INVALID_CHN_ID)
/* at lease one parameter is illagal ,eg, an illegal enumeration value  */
#define HI_ERR_VENC_ILLEGAL_PARAM HI_DEFINE_ERR(HI_ID_VENC, HI_ERR_LEVEL_ERROR, HI_ERR_ILLEGAL_PARAM)
/* channel exists */
#define HI_ERR_VENC_EXIST         HI_DEFINE_ERR(HI_ID_VENC, HI_ERR_LEVEL_ERROR, HI_ERR_EXIST)
/* channel exists */
#define HI_ERR_VENC_UNEXIST       HI_DEFINE_ERR(HI_ID_VENC, HI_ERR_LEVEL_ERROR, HI_ERR_UNEXIST)
/* using a NULL point */
#define HI_ERR_VENC_NULL_PTR      HI_DEFINE_ERR(HI_ID_VENC, HI_ERR_LEVEL_ERROR, HI_ERR_NULL_PTR)
/* try to enable or initialize system,device or channel, before configing attribute */
#define HI_ERR_VENC_NOT_CFG    HI_DEFINE_ERR(HI_ID_VENC, HI_ERR_LEVEL_ERROR, HI_ERR_NOT_CFG)
/* operation is not supported by NOW */
#define HI_ERR_VENC_NOT_SUPPORT   HI_DEFINE_ERR(HI_ID_VENC, HI_ERR_LEVEL_ERROR, HI_ERR_NOT_SUPPORT)
/* operation is not permitted ,eg, try to change stati attribute */
#define HI_ERR_VENC_NOT_PERM      HI_DEFINE_ERR(HI_ID_VENC, HI_ERR_LEVEL_ERROR, HI_ERR_NOT_PERM)
/* failure caused by malloc memory */
#define HI_ERR_VENC_NO_MEM         HI_DEFINE_ERR(HI_ID_VENC, HI_ERR_LEVEL_ERROR, HI_ERR_NO_MEM)
/* failure caused by malloc buffer */
#define HI_ERR_VENC_NO_BUF         HI_DEFINE_ERR(HI_ID_VENC, HI_ERR_LEVEL_ERROR, HI_ERR_NO_BUF)
/* no data in buffer */
#define HI_ERR_VENC_BUF_EMPTY     HI_DEFINE_ERR(HI_ID_VENC, HI_ERR_LEVEL_ERROR, HI_ERR_BUF_EMPTY)
/* no buffer for new data */
#define HI_ERR_VENC_BUF_FULL      HI_DEFINE_ERR(HI_ID_VENC, HI_ERR_LEVEL_ERROR, HI_ERR_BUF_FULL)
/* system is not ready,had not initialed or loaded */
#define HI_ERR_VENC_SYS_NOT_READY  HI_DEFINE_ERR(HI_ID_VENC, HI_ERR_LEVEL_ERROR, HI_ERR_NOT_READY)
/* system is busy */
#define HI_ERR_VENC_BUSY          HI_DEFINE_ERR(HI_ID_VENC, HI_ERR_LEVEL_ERROR, HI_ERR_BUSY)
/* buf size not enough */
#define HI_ERR_VENC_SIZE_NOT_ENOUGH HI_DEFINE_ERR(HI_ID_VENC, HI_ERR_LEVEL_ERROR, HI_ERR_SIZE_NOT_ENOUGH)

#define HI_VENC_MAX_PACK_INFO_NUM 8
#define HI_VENC_MAX_MPF_NUM 2
#define HI_VENC_PRORES_MAX_ID_CHAR_NUM 4
#define HI_VENC_MAX_HIERARCHY_NUM 4
#define HI_VENC_SCALING_LIST_SIZE_16 16
#define HI_VENC_SCALING_LIST_SIZE_64 64
#define HI_VENC_JPEG_QT_COEF_NUM 64
#define HI_VENC_MJPEG_QT_COEF_NUM HI_VENC_JPEG_QT_COEF_NUM
#define HI_VENC_QP_HIST_NUM 52
#define HI_VENC_MAX_ROI_NUM 8
#define HI_VENC_MAX_JPEG_ROI_NUM 16
#define HI_VENC_MAX_SSE_NUM 8
#define HI_VENC_MD_THRESHOLD_NUM 3
#define HI_VENC_MD_LEVEL_NUM 4
#define HI_VENC_MAX_SVC_RECT_NUM  32
#define HI_VENC_MAX_MOSAIC_RECT_NUM 200
#define HI_VENC_MAX_FRAME_NUM 2
#define HI_VENC_MB_SIZE 16

typedef enum {
    HI_VENC_RECV_SRC0 = 0,
    HI_VENC_RECV_SRC1,
    HI_VENC_RECV_SRC2,
    HI_VENC_RECV_SRC3,
    HI_VENC_RECV_BUTT
} hi_venc_recv_src;

typedef enum {
    HI_VENC_H264_NALU_B_SLICE   = 0,
    HI_VENC_H264_NALU_P_SLICE   = 1,
    HI_VENC_H264_NALU_I_SLICE   = 2,
    HI_VENC_H264_NALU_IDR_SLICE = 5,
    HI_VENC_H264_NALU_SEI       = 6,
    HI_VENC_H264_NALU_SPS       = 7,
    HI_VENC_H264_NALU_PPS       = 8,
    HI_VENC_H264_NALU_BUTT
} hi_venc_h264_nalu_type;

typedef enum {
    HI_VENC_H265_NALU_B_SLICE   = 0,
    HI_VENC_H265_NALU_P_SLICE   = 1,
    HI_VENC_H265_NALU_I_SLICE   = 2,
    HI_VENC_H265_NALU_IDR_SLICE = 19,
    HI_VENC_H265_NALU_VPS       = 32,
    HI_VENC_H265_NALU_SPS       = 33,
    HI_VENC_H265_NALU_PPS       = 34,
    HI_VENC_H265_NALU_SEI       = 39,
    HI_VENC_H265_NALU_BUTT
} hi_venc_h265_nalu_type;

typedef enum {
    HI_VENC_H264_REF_SLICE_FOR_1X = 1,
    HI_VENC_H264_REF_SLICE_FOR_2X = 2,
    HI_VENC_H264_REF_SLICE_FOR_4X = 5,
    HI_VENC_H264_REF_SLICE_FOR_BUTT
} hi_venc_h264_ref_slice_type;

typedef enum {
    HI_VENC_JPEG_PACK_ECS = 5,
    HI_VENC_JPEG_PACK_APP = 6,
    HI_VENC_JPEG_PACK_VDO = 7,
    HI_VENC_JPEG_PACK_PIC = 8,
    HI_VENC_JPEG_PACK_DCF = 9,
    HI_VENC_JPEG_PACK_DCF_PIC = 10,
    HI_VENC_JPEG_PACK_BUTT
} hi_venc_jpege_pack_type;

typedef enum {
    HI_VENC_PRORES_PACK_PIC = 1,
    HI_VENC_PRORES_PACK_BUTT
} hi_venc_prores_pack_type;

typedef enum {
    HI_VENC_MOSAIC_MODE_RECT = 0, /* user input mosaic rect */
    HI_VENC_MOSAIC_MODE_MAP,     /* user input mosaic map */
    HI_VENC_MOSAIC_MODE_BUTT
} hi_venc_mosaic_mode;

typedef union {
    hi_venc_h264_nalu_type h264_type;  /* AUTO:hi_payload_type:HI_PT_H264; */
    hi_venc_jpege_pack_type jpeg_type; /* AUTO:hi_payload_type:HI_PT_JPEG; */
    hi_venc_h265_nalu_type h265_type; /* AUTO:hi_payload_type:HI_PT_H265; */
    hi_venc_prores_pack_type prores_type; /* AUTO:hi_payload_type:HI_PT_PRORES; */
} hi_venc_data_type;

typedef struct {
    hi_venc_data_type pack_type;
    hi_u32 pack_offset;
    hi_u32 pack_len;
} hi_venc_pack_info;

typedef struct {
    hi_phys_addr_t phys_addr;
    hi_u8 ATTRIBUTE *addr;
    hi_u32 ATTRIBUTE len;
    hi_u64 pts;
    hi_bool is_frame_end;
    hi_venc_data_type data_type;
    hi_u32 offset;
    hi_u32 data_num;
    hi_venc_pack_info pack_info[HI_VENC_MAX_PACK_INFO_NUM];
} hi_venc_pack;

/* Defines the frame type and reference attributes of the H.264 frame skipping reference streams */
typedef enum {
    HI_VENC_BASE_IDR_SLICE = 0,                     /* the Idr frame at Base layer */
    HI_VENC_BASE_P_SLICE_REF_TO_IDR,                /* the P frame at Base layer, referenced by other frames
                                                     * at Base layer and reference to Idr frame
                                                     */
    HI_VENC_BASE_P_SLICE_REF_BY_BASE,               /* the P frame at Base layer, referenced by other frames
                                                     * at Base layer
                                                     */
    HI_VENC_BASE_P_SLICE_REF_BY_ENHANCE,            /* the P frame at Base layer, referenced by other frames
                                                     * at Enhance layer
                                                     */
    HI_VENC_ENHANCE_P_SLICE_REF_BY_ENHANCE,         /* the P frame at Enhance layer, referenced by other frames
                                                     * at Enhance layer
                                                     */
    HI_VENC_ENHANCE_P_SLICE_NOT_FOR_REF,            /* the P frame at Enhance layer ,not referenced */
    HI_VENC_P_SLICE_BUTT
} hi_venc_ref_type;

typedef struct {
    hi_u32 pic_bytes;
    hi_u32 inter16x16_mb_num;
    hi_u32 inter8x8_mb_num;
    hi_u32 intra16_mb_num;
    hi_u32 intra8_mb_num;
    hi_u32 intra4_mb_num;
    hi_venc_ref_type ref_type;
    hi_u32 update_attr_cnt;
    hi_u32 start_qp;
    hi_u32 mean_qp;
    hi_bool is_p_skip;
} hi_venc_h264_stream_info;

typedef struct {
    hi_u32 pic_bytes;
    hi_u32 inter64x64_cu_num;
    hi_u32 inter32x32_cu_num;
    hi_u32 inter16x16_cu_num;
    hi_u32 inter8x8_cu_num;
    hi_u32 intra32x32_cu_num;
    hi_u32 intra16x16_cu_num;
    hi_u32 intra8x8_cu_num;
    hi_u32 intra4x4_cu_num;
    hi_venc_ref_type ref_type;
    hi_u32 update_attr_cnt;
    hi_u32 start_qp;
    hi_u32 mean_qp;
    hi_bool is_p_skip;
} hi_venc_h265_stream_info;

typedef struct {
    hi_bool enable;
    hi_u64 sse_val;
} hi_venc_sse_info;

typedef struct {
    hi_u32 residual_bits;
    hi_u32 head_bits;
    hi_u32 madi_val;
    hi_u32 madp_val;
    hi_double psnr_val;
    hi_u32 sse_lcu_cnt;
    hi_u64 sse_sum;
    hi_venc_sse_info sse_info[HI_VENC_MAX_SSE_NUM];
    hi_u32 qp_hist[HI_VENC_QP_HIST_NUM];
    hi_u32 move_scene16x16_num;
    hi_u32 move_scene_bits;
} hi_venc_h264_adv_stream_info;

typedef struct {
    hi_u32             residual_bits;
    hi_u32             head_bits;
    hi_u32             madi_val;
    hi_u32             madp_val;
    hi_double          psnr_val;
    hi_u32             sse_lcu_cnt;
    hi_u64             sse_sum;
    hi_venc_sse_info   sse_info[HI_VENC_MAX_SSE_NUM];
    hi_u32             qp_hist[HI_VENC_QP_HIST_NUM];
    hi_u32             move_scene32x32_num;
    hi_u32             move_scene_bits;
} hi_venc_h265_adv_stream_info;

typedef struct {
    hi_u32 pic_bytes;
    hi_u32 update_attr_cnt;
} hi_venc_prores_stream_info;

typedef struct {
    hi_u32 pic_bytes;
    hi_u32 update_attr_cnt;
    hi_u32 qfactor;
} hi_venc_jpeg_stream_info;

typedef struct {
    hi_venc_pack ATTRIBUTE *pack;
    hi_u32 ATTRIBUTE pack_cnt;
    hi_u32 seq;
    union {
        hi_venc_h264_stream_info   h264_info; /* AUTO:hi_payload_type:HI_PT_H264; */
        hi_venc_jpeg_stream_info   jpeg_info; /* AUTO:hi_payload_type:HI_PT_JPEG; */
        hi_venc_h265_stream_info   h265_info; /* AUTO:hi_payload_type:HI_PT_H265; */
        hi_venc_prores_stream_info prores_info; /* AUTO:hi_payload_type:HI_PT_PRORES; */
    };
    union {
        hi_venc_h264_adv_stream_info h264_adv_info; /* AUTO:hi_payload_type:HI_PT_H264; */
        hi_venc_h265_adv_stream_info h265_adv_info; /* AUTO:hi_payload_type:HI_PT_H265; */
    };
} hi_venc_stream;

typedef struct {
    hi_venc_ref_type ref_type;
    hi_u32  pic_bytes;
    hi_u32  pic_cnt;
    hi_u32  start_qp;
    hi_u32  mean_qp;
    hi_bool is_p_skip;
    hi_u32  residual_bits;
    hi_u32  head_bits;
    hi_u32  madi_val;
    hi_u32  madp_val;
    hi_u64  sse_sum;
    hi_u32  sse_lcu_cnt;
    hi_double psnr_val;
} hi_venc_stream_info;

typedef struct {
    hi_u8 large_thumbnail_num;
    hi_size large_thumbnail_size[HI_VENC_MAX_MPF_NUM];
} hi_venc_mpf_cfg;

typedef enum {
    HI_VENC_PIC_RECV_SINGLE = 0,
    HI_VENC_PIC_RECV_MULTI,
    HI_VENC_PIC_RECV_BUTT
} hi_venc_pic_recv_mode;

typedef struct {
    hi_bool dcf_en;
    hi_venc_mpf_cfg mpf_cfg;
    hi_venc_pic_recv_mode recv_mode;
} hi_venc_jpeg_attr;

typedef struct {
    hi_bool rcn_ref_share_buf_en;
    hi_u32 frame_buf_ratio;
} hi_venc_h264_attr;

typedef struct {
    hi_bool rcn_ref_share_buf_en;
    hi_u32 frame_buf_ratio;
} hi_venc_h265_attr;

typedef enum {
    HI_VENC_PRORES_FRAME_RATE_UNKNOWN = 0,
    HI_VENC_PRORES_FRAME_RATE_23_976,
    HI_VENC_PRORES_FRAME_RATE_24,
    HI_VENC_PRORES_FRAME_RATE_25,
    HI_VENC_PRORES_FRAME_RATE_29_97,
    HI_VENC_PRORES_FRAME_RATE_30,
    HI_VENC_PRORES_FRAME_RATE_50,
    HI_VENC_PRORES_FRAME_RATE_59_94,
    HI_VENC_PRORES_FRAME_RATE_60,
    HI_VENC_PRORES_FRAME_RATE_100,
    HI_VENC_PRORES_FRAME_RATE_119_88,
    HI_VENC_PRORES_FRAME_RATE_120,
    HI_VENC_PRORES_FRAME_RATE_BUTT
} hi_venc_prores_frame_rate;

typedef enum {
    HI_VENC_PRORES_ASPECT_RATIO_UNKNOWN = 0,
    HI_VENC_PRORES_ASPECT_RATIO_SQUARE,
    HI_VENC_PRORES_ASPECT_RATIO_4_3,
    HI_VENC_PRORES_ASPECT_RATIO_16_9,
    HI_VENC_PRORES_ASPECT_RATIO_BUTT
} hi_venc_prores_aspect_ratio;

typedef struct {
    hi_char identifier[HI_VENC_PRORES_MAX_ID_CHAR_NUM];
    hi_venc_prores_frame_rate frame_rate_code;
    hi_venc_prores_aspect_ratio aspect_ratio;
} hi_venc_prores_attr;

typedef struct {
    hi_payload_type type;
    hi_u32  max_pic_width;
    hi_u32  max_pic_height;
    hi_u32  buf_size;
    hi_u32  profile;
    hi_bool is_by_frame;
    hi_u32  pic_width;
    hi_u32  pic_height;
    union {
        hi_venc_h264_attr h264_attr; /* AUTO:hi_payload_type:HI_PT_H264; */
        hi_venc_h265_attr h265_attr; /* AUTO:hi_payload_type:HI_PT_H265; */
        hi_venc_jpeg_attr  jpeg_attr; /* AUTO:hi_payload_type:HI_PT_JPEG; */
        hi_venc_prores_attr prores_attr; /* AUTO:hi_payload_type:HI_PT_PRORES; */
    };
} hi_venc_attr;

typedef enum {
    HI_VENC_GOP_MODE_NORMAL_P    = 0,
    HI_VENC_GOP_MODE_DUAL_P      = 1,
    HI_VENC_GOP_MODE_SMART_P     = 2,
    HI_VENC_GOP_MODE_ADV_SMART_P = 3,
    HI_VENC_GOP_MODE_BIPRED_B    = 4,
    HI_VENC_GOP_MODE_LOW_DELAY_B = 5,
    HI_VENC_GOP_MODE_BUTT,
} hi_venc_gop_mode;

typedef struct {
    hi_s32 ip_qp_delta;
} hi_venc_gop_normal_p;

typedef struct {
    hi_u32 sp_interval;
    hi_s32 sp_qp_delta;
    hi_s32 ip_qp_delta;
} hi_venc_gop_dual_p;

typedef struct {
    hi_u32 bg_interval;
    hi_s32 bg_qp_delta;
    hi_s32 vi_qp_delta;
} hi_venc_gop_smart_p;

typedef struct {
    hi_u32 bg_interval;
    hi_s32 bg_qp_delta;
    hi_s32 vi_qp_delta;
} hi_venc_gop_adv_smart_p;

typedef struct {
    hi_u32 b_frame_num;
    hi_s32 b_qp_delta;
    hi_s32 ip_qp_delta;
} hi_venc_gop_bipred_b;

typedef struct {
    hi_venc_gop_mode gop_mode;
    union {
        hi_venc_gop_normal_p    normal_p; /* AUTO:hi_venc_gop_mode:HI_VENC_GOP_MODE_NORMAL_P; */
        hi_venc_gop_dual_p      dual_p; /* AUTO:hi_venc_gop_mode:HI_VENC_GOP_MODE_DUAL_P; */
        hi_venc_gop_smart_p     smart_p; /* AUTO:hi_venc_gop_mode:HI_VENC_GOP_MODE_SMART_P; */
        hi_venc_gop_adv_smart_p adv_smart_p; /* AUTO:hi_venc_gop_mode:HI_VENC_GOP_MODE_ADV_SMART_P; */
        hi_venc_gop_bipred_b    bipred_b; /* AUTO:hi_venc_gop_mode:HI_VENC_GOP_MODE_BIPRED_B; */
    };
} hi_venc_gop_attr;

typedef struct {
    hi_venc_attr venc_attr;
    hi_venc_rc_attr rc_attr;
    hi_venc_gop_attr gop_attr;
} hi_venc_chn_attr;

typedef struct {
    hi_s32 recv_pic_num;
} hi_venc_start_param;

typedef struct {
    hi_u32 left_pics;
    hi_u32 left_stream_bytes;
    hi_u32 left_stream_frames;
    hi_u32 cur_packs;
    hi_u32 left_recv_pics;
    hi_u32 left_enc_pics;
    hi_bool is_jpeg_snap_end;
    hi_u64 release_pic_pts;
    hi_venc_stream_info stream_info;
} hi_venc_chn_status;

typedef struct {
    hi_u32 constrained_intra_pred_flag;
} hi_venc_h264_intra_pred;

typedef struct {
    hi_u32  intra_trans_mode;
    hi_u32  inter_trans_mode;
    hi_bool scaling_list_valid;
    hi_u8   inter_scaling_list8x8[HI_VENC_SCALING_LIST_SIZE_64];
    hi_u8   intra_scaling_list8x8[HI_VENC_SCALING_LIST_SIZE_64];
    hi_s32  chroma_qp_index_offset;
} hi_venc_h264_trans;

typedef struct {
    hi_u32 entropy_coding_mode_i;
    hi_u32 entropy_coding_mode_p;
    hi_u32 entropy_coding_mode_b;
    hi_u32 cabac_init_idc;
} hi_venc_h264_entropy;

typedef struct {
    hi_u32 pic_order_cnt_type;
} hi_venc_h264_poc;

typedef struct {
    hi_u32 disable_deblocking_filter_idc;
    hi_s32 slice_alpha_c0_offset_div2;
    hi_s32 slice_beta_offset_div2;
} hi_venc_h264_dblk;

typedef struct {
    hi_u8  timing_info_present_flag;
    hi_u8  fixed_frame_rate_flag;
    hi_u32 num_units_in_tick;
    hi_u32 time_scale;
} hi_venc_h264_vui_time_info;

typedef struct {
    hi_u8  aspect_ratio_info_present_flag;
    hi_u8  aspect_ratio_idc;
    hi_u8  overscan_info_present_flag;
    hi_u8  overscan_appropriate_flag;
    hi_u16 sar_width;
    hi_u16 sar_height;
} hi_venc_vui_aspect_ratio;

typedef struct {
    hi_u8 video_signal_type_present_flag;
    hi_u8 video_format;
    hi_u8 video_full_range_flag;
    hi_u8 colour_description_present_flag;
    hi_u8 colour_primaries;
    hi_u8 transfer_characteristics;
    hi_u8 matrix_coefficients;
} hi_venc_vui_video_signal;

typedef struct {
    hi_u8 bitstream_restriction_flag;
} hi_venc_vui_bitstream_restric;

typedef struct {
    hi_venc_vui_aspect_ratio      vui_aspect_ratio;
    hi_venc_h264_vui_time_info    vui_time_info;
    hi_venc_vui_video_signal      vui_video_signal;
    hi_venc_vui_bitstream_restric vui_bitstream_restric;
} hi_venc_h264_vui;

typedef struct {
    hi_u32 timing_info_present_flag;
    hi_u32 num_units_in_tick;
    hi_u32 time_scale;
    hi_u32 num_ticks_poc_diff_one_minus1;
} hi_venc_h265_vui_time_info;

typedef struct {
    hi_venc_vui_aspect_ratio      vui_aspect_ratio;
    hi_venc_h265_vui_time_info    vui_time_info;
    hi_venc_vui_video_signal      vui_video_signal;
    hi_venc_vui_bitstream_restric vui_bitstream_restric;
} hi_venc_h265_vui;

typedef struct {
    hi_u32 qfactor;
    hi_u8  y_qt[HI_VENC_JPEG_QT_COEF_NUM];
    hi_u8  cb_qt[HI_VENC_JPEG_QT_COEF_NUM];
    hi_u8  cr_qt[HI_VENC_JPEG_QT_COEF_NUM];
    hi_u32 mcu_per_ecs;
    hi_bool ecs_output_en;
} hi_venc_jpeg_param;

typedef struct {
    hi_u8  y_qt[HI_VENC_MJPEG_QT_COEF_NUM];
    hi_u8  cb_qt[HI_VENC_MJPEG_QT_COEF_NUM];
    hi_u8  cr_qt[HI_VENC_MJPEG_QT_COEF_NUM];
    hi_u32 mcu_per_ecs;
    hi_bool ecs_output_en;
} hi_venc_mjpeg_param;

typedef struct {
    hi_u32  idx;
    hi_bool enable;
    hi_bool is_abs_qp;
    hi_s32  qp;
    hi_rect rect;
} hi_venc_roi_attr;

typedef enum {
    HI_VENC_ROI_FOR_I = 0,
    HI_VENC_ROI_FOR_P_B,
    HI_VENC_ROI_FOR_VI,
    HI_VENC_ROI_FOR_BUTT,
} hi_venc_roi_for_frame;

typedef struct {
    hi_u32  idx;
    hi_bool enable[HI_VENC_ROI_FOR_BUTT];
    hi_bool is_abs_qp[HI_VENC_ROI_FOR_BUTT];
    hi_s32  qp[HI_VENC_ROI_FOR_BUTT];
    hi_rect rect[HI_VENC_ROI_FOR_BUTT];
} hi_venc_roi_attr_ex;

typedef struct {
    hi_u32 idx;
    hi_bool enable;
    hi_u32 level;
    hi_rect rect;
} hi_venc_jpeg_roi_attr;

typedef struct {
    hi_s32 src_frame_rate;
    hi_s32 dst_frame_rate;
} hi_venc_roi_bg_frame_rate;

typedef struct {
    hi_u32  base;
    hi_u32  enhance;
    hi_bool pred_en;
} hi_venc_ref_param;

typedef enum {
    HI_VENC_JPEG_ENC_ALL  = 0,                        /* Jpeg channel snap all the pictures when started. */
    HI_VENC_JPEG_ENC_SNAP = 1,                        /* Jpeg channel snap the flashed pictures when started. */
    HI_VENC_JPEG_ENC_BUTT,
} hi_venc_jpeg_enc_mode;

typedef struct {
    hi_phys_addr_t phys_addr[HI_VENC_MAX_TILE_NUM];
    hi_void ATTRIBUTE *user_addr[HI_VENC_MAX_TILE_NUM];
    hi_u64 ATTRIBUTE buf_size[HI_VENC_MAX_TILE_NUM];
} hi_venc_stream_buf_info;

typedef struct {
    hi_u32 constrained_intra_pred_flag;
    hi_u32 strong_intra_smoothing_enabled_flag;
} hi_venc_h265_pu;

typedef struct {
    hi_s32  cb_qp_offset;
    hi_s32  cr_qp_offset;
    hi_bool scaling_list_en;
    hi_bool scaling_list_tu4_valid;
    hi_u8   inter_scaling_list4x4[2][HI_VENC_SCALING_LIST_SIZE_16]; // 2: 0 luma; 1 chroma
    hi_u8   intra_scaling_list4x4[2][HI_VENC_SCALING_LIST_SIZE_16]; // 2: 0 luma; 1 chroma
    hi_bool scaling_list_tu8_valid;
    hi_u8   inter_scaling_list8x8[2][HI_VENC_SCALING_LIST_SIZE_64]; // 2: 0 luma; 1 chroma
    hi_u8   intra_scaling_list8x8[2][HI_VENC_SCALING_LIST_SIZE_64]; // 2: 0 luma; 1 chroma
    hi_bool scaling_list_tu16_valid;
    hi_u8   inter_scaling_list16x16[2][HI_VENC_SCALING_LIST_SIZE_64]; // 2: 0 luma; 1 chroma
    hi_u8   intra_scaling_list16x16[2][HI_VENC_SCALING_LIST_SIZE_64]; // 2: 0 luma; 1 chroma
    hi_bool scaling_list_tu32_valid;
    hi_u8   inter_scaling_list32x32[HI_VENC_SCALING_LIST_SIZE_64];
    hi_u8   intra_scaling_list32x32[HI_VENC_SCALING_LIST_SIZE_64];
} hi_venc_h265_trans;

typedef struct {
    hi_u32 cabac_init_flag;
} hi_venc_h265_entropy;

typedef struct {
    hi_u32 slice_deblocking_filter_disabled_flag;
    hi_s32 slice_beta_offset_div2;
    hi_s32 slice_tc_offset_div2;
    hi_u32 loop_filter_across_tiles_enabled_flag;
    hi_u32 loop_filter_across_slices_enabled_flag;
} hi_venc_h265_dblk;

typedef struct {
    hi_u32 slice_sao_luma_flag;
    hi_u32 slice_sao_chroma_flag;
} hi_venc_h265_sao;

typedef enum {
    HI_VENC_INTRA_REFRESH_ROW = 0,
    HI_VENC_INTRA_REFRESH_COLUMN,
    HI_VENC_INTRA_REFRESH_BUTT
} hi_venc_intra_refresh_mode;

typedef struct {
    hi_bool enable;
    hi_venc_intra_refresh_mode mode;
    hi_u32 refresh_num;
    hi_u32 request_i_qp;
} hi_venc_intra_refresh;

typedef enum {
    HI_VENC_MOD_VENC = 1,
    HI_VENC_MOD_H264,
    HI_VENC_MOD_H265,
    HI_VENC_MOD_JPEG,
    HI_VENC_MOD_RC,
    HI_VENC_MOD_BUTT
} hi_venc_mod_type;

typedef struct {
    hi_u32 buf_cache;
    hi_u32 frame_buf_recycle;
} hi_venc_venc_mod_param;

typedef struct {
    hi_u32    one_stream_buf;
    hi_u32    mini_buf_mode;
    hi_u32    low_power_mode;
    hi_vb_src vb_src;
    hi_bool   qp_hist_en;
    hi_u32    max_user_data_len; /* RW; Range:[0, 65536]; one user data buffer len */
} hi_venc_h264_mod_param;

typedef struct {
    hi_u32    one_stream_buf;
    hi_u32    mini_buf_mode;
    hi_u32    low_power_mode;
    hi_vb_src vb_src;
    hi_bool   qp_hist_en;
    hi_u32    max_user_data_len; /* RW; Range:[0, 65536]; one user data buffer len */
} hi_venc_h265_mod_param;

typedef struct {
    hi_u32 one_stream_buf;
    hi_u32 mini_buf_mode;
    hi_u32 clear_stream_buf;
    hi_u32 dering_mode;
} hi_venc_jpeg_mod_param;

typedef struct {
    hi_venc_mod_type mod_type;
    union {
        hi_venc_venc_mod_param venc_mod_param; /* AUTO:hi_venc_mod_type:HI_VENC_MOD_VENC; */
        hi_venc_h264_mod_param h264_mod_param; /* AUTO:hi_venc_mod_type:HI_VENC_MOD_H264; */
        hi_venc_h265_mod_param h265_mod_param; /* AUTO:hi_venc_mod_type:HI_VENC_MOD_H265; */
        hi_venc_jpeg_mod_param jpeg_mod_param; /* AUTO:hi_venc_mod_type:HI_VENC_MOD_JPEG; */
    };
} hi_venc_mod_param;

typedef enum {
    HI_VENC_FRAME_TYPE_NONE = 1,
    HI_VENC_FRAME_TYPE_IDR,
    HI_VENC_FRAME_TYPE_P_SKIP,
    HI_VENC_FRAME_TYPE_BUTT
} hi_venc_frame_type;

typedef struct {
    hi_bool qpmap_valid;
    hi_bool skip_weight_valid;
    hi_u32  blk_start_qp;
    hi_phys_addr_t qpmap_phys_addr;
    hi_phys_addr_t skip_weight_phys_addr;
    hi_venc_frame_type frame_type;
} hi_venc_user_rc_info;

typedef struct {
    hi_bool valid;
    hi_phys_addr_t phys_addr;
} hi_venc_user_roimap;

typedef struct {
    hi_video_frame_info user_frame;
    hi_venc_user_rc_info user_rc_info;
    hi_venc_user_roimap user_roimap;
} hi_venc_user_frame_info;

typedef struct {
    hi_u8 data_y;
    hi_u8 data_u;
    hi_u8 data_v;
} hi_venc_mosaic_pixel_yuv;

typedef struct {
    hi_u32 rect_num;
    hi_rect rect[HI_VENC_MAX_MOSAIC_RECT_NUM];
} hi_venc_mosaic_rect;

typedef struct {
    hi_bool valid;
    hi_phys_addr_t phys_addr;
    hi_bool specified_yuv_en;
    hi_venc_mosaic_pixel_yuv pixel_yuv;
} hi_venc_mosaic_map;

/* the information of the mosaic */
typedef struct {
    hi_venc_mosaic_mode mode;
    hi_mosaic_blk_size blk_size;

    union {
        hi_venc_mosaic_rect rect_param; /* AUTO:hi_venc_mosaic_mode:HI_VENC_MOSAIC_MODE_RECT; */
        hi_venc_mosaic_map map_param; /* AUTO:hi_venc_mosaic_mode:HI_VENC_MOSAIC_MODE_MAP; */
    };
} hi_venc_mosaic_info;

/* the information of the multi frame */
typedef struct {
    hi_u32 frame_num;
    hi_video_frame_info frame[HI_VENC_MAX_FRAME_NUM];
    hi_venc_mosaic_info mosaic_info;
} hi_venc_multi_frame_info;

typedef struct {
    hi_bool mosaic_en;
    hi_bool composite_enc_en;
    hi_u32 quality_level; /* 0: lower quality with less ddr consumption; 1: higher quality with more ddr consumption. */
} hi_venc_chn_config;

typedef struct {
    hi_u32  idx;
    hi_bool enable;
    hi_rect rect;
} hi_venc_sse_rgn;

typedef struct {
    hi_bool color_to_grey_en;
    hi_u32  priority;
    hi_u32  max_stream_cnt;
    hi_u32  poll_wake_up_frame_cnt;
    hi_crop_info crop_info;
    hi_frame_rate_ctrl frame_rate;
} hi_venc_chn_param;

typedef struct {
    hi_bool enable;
    hi_u32  direction;
    hi_u32  gain;
    hi_u32  offset;
    hi_u32  threshold_p[HI_VENC_TEXTURE_THRESHOLD_SIZE];
    hi_u32  threshold_b[HI_VENC_TEXTURE_THRESHOLD_SIZE];
} hi_venc_fg_protect;

typedef enum {
    HI_VENC_SCENE_0 = 0,        /* RW; A scene in which the camera does not move or periodically moves continuously */
    HI_VENC_SCENE_1 = 1,        /* RW; Motion scene at high bit rate */
    HI_VENC_SCENE_2 = 2,        /* RW; It has regular continuous motion at medium bit rate and
                                   the encoding pressure is relatively large */
    HI_VENC_SCENE_BUTT
} hi_venc_scene_mode;

typedef struct {
    hi_bool enable;
    hi_s32 strength0;
    hi_s32 strength1;
} hi_venc_debreath_effect;

typedef struct {
    hi_op_mode pred_mode;
    hi_u32 intra32_cost;
    hi_u32 intra16_cost;
    hi_u32 intra8_cost;
    hi_u32 intra4_cost;
    hi_u32 inter64_cost;
    hi_u32 inter32_cost;
    hi_u32 inter16_cost;
    hi_u32 inter8_cost;
} hi_venc_cu_pred;

typedef struct {
    hi_bool enable;
    hi_u32  gain;
    hi_u32  offset;
    hi_u32  bg_cost;
    hi_u32  fg_cost;
} hi_venc_skip_bias;

typedef struct {
    hi_bool enable;
    hi_s32  qp_delta[HI_VENC_MAX_HIERARCHY_NUM];
    hi_s32  frame_num[HI_VENC_MAX_HIERARCHY_NUM];
} hi_venc_hierarchical_qp;

typedef struct {
    hi_vb_pool pic_vb_pool;
    hi_vb_pool pic_info_vb_pool;
} hi_venc_chn_pool;

typedef struct {
    hi_u32 clear_stats_after_set_attr;
} hi_venc_rc_adv_param;

typedef struct {
    hi_bool enable;
    hi_u32 split_mode;
    hi_u32 split_size;
    hi_bool slice_output_en;
} hi_venc_slice_split;

typedef struct {
    hi_op_mode mode;
    hi_u32 hor;
    hi_u32 ver;
} hi_venc_search_window;

typedef enum {
    SVC_RECT_TYPE0 = 0,
    SVC_RECT_TYPE1,
    SVC_RECT_TYPE2,
    SVC_RECT_TYPE3,
    SVC_RECT_TYPE4,
    SVC_RECT_TYPE_BUTT
} hi_venc_svc_rect_type;

typedef struct {
    hi_u8 qpmap_value_i;   /* RW;Range: [0, 255] */
    hi_u8 qpmap_value_p;   /* RW;Range: [0, 255] */
    hi_u8 skipmap_value;  /* RW;Range: [0, 255] */
} hi_venc_svc_map_param;

typedef struct {
    hi_u32 rect_num;
    hi_size base_resolution;
    hi_rect rect_attr[HI_VENC_MAX_SVC_RECT_NUM];
    hi_venc_svc_rect_type detect_type[HI_VENC_MAX_SVC_RECT_NUM];
    hi_u64 pts;
} hi_venc_svc_rect_info;

typedef struct {
    hi_venc_svc_map_param  fg_region[SVC_RECT_TYPE_BUTT];
    hi_venc_svc_map_param  activity_region;
    hi_venc_svc_map_param  bg_region;
    hi_bool fg_protect_adaptive_en; /* RW;Range: [0, 1] */
} hi_venc_svc_param;

typedef struct {
    hi_bool deblur_en;
    hi_bool deblur_adaptive_en;
} hi_venc_deblur_param;

typedef enum {
    HI_VENC_MD_SAD_MODE_ZERO = 0,
    HI_VENC_MD_SAD_MODE_SEARCH = 1,
    HI_VENC_MD_SAD_MODE_BUTT
} hi_venc_md_sad_mode;

typedef struct {
    hi_bool online_en;
    hi_bool sad_stats_en;
    hi_bool level_stats_en;
    hi_venc_md_sad_mode sad_mode;
    hi_s32 threshold_gain[HI_VENC_MD_THRESHOLD_NUM];
    hi_s32 threshold_offset[HI_VENC_MD_THRESHOLD_NUM];
    hi_s32 qp_delta[HI_VENC_MD_LEVEL_NUM];
    hi_s32 md_skip_weight[HI_VENC_MD_LEVEL_NUM];
} hi_venc_md_param;

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* __HI_COMMON_VENC_H__ */
