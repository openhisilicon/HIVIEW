/*
  Copyright (c), 2001-2024, Shenshu Tech. Co., Ltd.
 */
#ifndef OT_COMMON_VENC_H
#define OT_COMMON_VENC_H

#include "ot_common_rc.h"
#include "ot_common.h"
#include "ot_common_video.h"
#include "ot_common_vb.h"
#include "ot_errno.h"

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif

/* invalid channel ID */
#define OT_ERR_VENC_INVALID_CHN_ID OT_DEFINE_ERR(OT_ID_VENC, OT_ERR_LEVEL_ERROR, OT_ERR_INVALID_CHN_ID)
/* at least one parameter is illagal ,eg, an illegal enumeration value  */
#define OT_ERR_VENC_ILLEGAL_PARAM OT_DEFINE_ERR(OT_ID_VENC, OT_ERR_LEVEL_ERROR, OT_ERR_ILLEGAL_PARAM)
/* channel exists */
#define OT_ERR_VENC_EXIST         OT_DEFINE_ERR(OT_ID_VENC, OT_ERR_LEVEL_ERROR, OT_ERR_EXIST)
/* channel exists */
#define OT_ERR_VENC_UNEXIST       OT_DEFINE_ERR(OT_ID_VENC, OT_ERR_LEVEL_ERROR, OT_ERR_UNEXIST)
/* using a NULL point */
#define OT_ERR_VENC_NULL_PTR      OT_DEFINE_ERR(OT_ID_VENC, OT_ERR_LEVEL_ERROR, OT_ERR_NULL_PTR)
/* try to enable or initialize system,device or channel, before configing attribute */
#define OT_ERR_VENC_NOT_CFG    OT_DEFINE_ERR(OT_ID_VENC, OT_ERR_LEVEL_ERROR, OT_ERR_NOT_CFG)
/* operation is not supported by NOW */
#define OT_ERR_VENC_NOT_SUPPORT   OT_DEFINE_ERR(OT_ID_VENC, OT_ERR_LEVEL_ERROR, OT_ERR_NOT_SUPPORT)
/* operation is not permitted ,eg, try to change stati attribute */
#define OT_ERR_VENC_NOT_PERM      OT_DEFINE_ERR(OT_ID_VENC, OT_ERR_LEVEL_ERROR, OT_ERR_NOT_PERM)
/* failure caused by malloc memory */
#define OT_ERR_VENC_NO_MEM         OT_DEFINE_ERR(OT_ID_VENC, OT_ERR_LEVEL_ERROR, OT_ERR_NO_MEM)
/* failure caused by malloc buffer */
#define OT_ERR_VENC_NO_BUF         OT_DEFINE_ERR(OT_ID_VENC, OT_ERR_LEVEL_ERROR, OT_ERR_NO_BUF)
/* no data in buffer */
#define OT_ERR_VENC_BUF_EMPTY     OT_DEFINE_ERR(OT_ID_VENC, OT_ERR_LEVEL_ERROR, OT_ERR_BUF_EMPTY)
/* no buffer for new data */
#define OT_ERR_VENC_BUF_FULL      OT_DEFINE_ERR(OT_ID_VENC, OT_ERR_LEVEL_ERROR, OT_ERR_BUF_FULL)
/* system is not ready,had not initialed or loaded */
#define OT_ERR_VENC_SYS_NOT_READY  OT_DEFINE_ERR(OT_ID_VENC, OT_ERR_LEVEL_ERROR, OT_ERR_NOT_READY)
/* system is busy */
#define OT_ERR_VENC_BUSY          OT_DEFINE_ERR(OT_ID_VENC, OT_ERR_LEVEL_ERROR, OT_ERR_BUSY)
/* buf size not enough */
#define OT_ERR_VENC_SIZE_NOT_ENOUGH OT_DEFINE_ERR(OT_ID_VENC, OT_ERR_LEVEL_ERROR, OT_ERR_SIZE_NOT_ENOUGH)

#define OT_VENC_MAX_PACK_INFO_NUM 8
#define OT_VENC_MAX_MPF_NUM 2
#define OT_VENC_PRORES_MAX_ID_CHAR_NUM 4
#define OT_VENC_SCALING_LIST_SIZE_16 16
#define OT_VENC_SCALING_LIST_SIZE_64 64
#define OT_VENC_JPEG_QT_COEF_NUM 64
#define OT_VENC_MJPEG_QT_COEF_NUM OT_VENC_JPEG_QT_COEF_NUM
#define OT_VENC_QP_HIST_NUM 52
#define OT_VENC_H264_QP_HIST_NUM OT_VENC_QP_HIST_NUM
#define OT_VENC_H265_QP_HIST_NUM OT_VENC_QP_HIST_NUM
#define OT_VENC_SVAC3_QP_HIST_NUM 64
#define OT_VENC_MAX_ROI_NUM 8
#define OT_VENC_MAX_JPEG_ROI_NUM 16
#define OT_VENC_MAX_SSE_NUM 8
#define OT_VENC_MD_THRESHOLD_NUM 3
#define OT_VENC_MD_LEVEL_NUM 4
#define OT_VENC_MAX_QUALITY_BALANCE_NUM 8
#define OT_VENC_MAX_WATERMARK_INFO_NUM 64
#define OT_VENC_MAX_WATERMARK_SEED_NUM 4
#define OT_VENC_MAX_SVC_RECT_NUM  32
#define OT_VENC_MAX_MOSAIC_RECT_NUM 200
#define OT_VENC_MAX_FRAME_NUM 2
#define OT_VENC_MAX_SVC_REF_NUM 4
#define OT_VENC_MIN_SVC_REF_NUM 2
#define OT_VENC_MAX_SVC_V1_RECT_TYPE SVC_RECT_TYPE5
#define OT_VENC_MB_SIZE 16
#define OT_VENC_MAX_BATCH_CROP_RECT_NUM 60

typedef enum {
    OT_VENC_RECV_SRC0 = 0,
    OT_VENC_RECV_SRC1,
    OT_VENC_RECV_SRC2,
    OT_VENC_RECV_SRC3,
    OT_VENC_RECV_BUTT
} ot_venc_recv_src;

typedef enum {
    OT_VENC_H264_NALU_B_SLICE   = 0,
    OT_VENC_H264_NALU_P_SLICE   = 1,
    OT_VENC_H264_NALU_I_SLICE   = 2,
    OT_VENC_H264_NALU_IDR_SLICE = 5,
    OT_VENC_H264_NALU_SEI       = 6,
    OT_VENC_H264_NALU_SPS       = 7,
    OT_VENC_H264_NALU_PPS       = 8,
    OT_VENC_H264_NALU_BUTT
} ot_venc_h264_nalu_type;

typedef enum {
    OT_VENC_H265_NALU_B_SLICE   = 0,
    OT_VENC_H265_NALU_P_SLICE   = 1,
    OT_VENC_H265_NALU_I_SLICE   = 2,
    OT_VENC_H265_NALU_IDR_SLICE = 19,
    OT_VENC_H265_NALU_VPS       = 32,
    OT_VENC_H265_NALU_SPS       = 33,
    OT_VENC_H265_NALU_PPS       = 34,
    OT_VENC_H265_NALU_SEI       = 39,
    OT_VENC_H265_NALU_ENHANCE   = 64,
    OT_VENC_H265_NALU_BUTT
} ot_venc_h265_nalu_type;

typedef enum {
    OT_VENC_SVAC3_NALU_FORBIDDEN   = 0,
    OT_VENC_SVAC3_NALU_NRAP_PATCH  = 1,
    OT_VENC_SVAC3_NALU_RAPI_PATCH  = 2,
    OT_VENC_SVAC3_NALU_PIC_HEAD    = 3,
    OT_VENC_SVAC3_NALU_IDR_PATCH   = 4,
    OT_VENC_SVAC3_NALU_EXTENSION   = 5,
    OT_VENC_SVAC3_NALU_SEI         = 6,
    OT_VENC_SVAC3_NALU_SPS         = 7,
    OT_VENC_SVAC3_NALU_PPS         = 8,
    OT_VENC_SVAC3_NALU_SEC_PS      = 9,
    OT_VENC_SVAC3_NALU_AUTH_DATA   = 10,
    OT_VENC_SVAC3_NALU_EOS         = 11,
    OT_VENC_SVAC3_NALU_CRR_NDP_PATCH = 12,
    /* RESERVED 13 */
    OT_VENC_SVAC3_NALU_RL_PATCH   = 14,
    OT_VENC_SVAC3_NALU_PIC_DL     = 15,
    OT_VENC_SVAC3_NALU_EOCVS      = 16,
    OT_VENC_SVAC3_NALU_CRR_DP_PATCH = 17,
    OT_VENC_SVAC3_NALU_CRR_DL_PATCH = 18,
    OT_VENC_SVAC3_NALU_CRR_PRIVACY = 19,
    OT_VENC_SVAC3_NALU_BUTT
} ot_venc_svac3_nalu_type;

typedef enum {
    OT_VENC_H264_REF_SLICE_FOR_1X = 1,
    OT_VENC_H264_REF_SLICE_FOR_2X = 2,
    OT_VENC_H264_REF_SLICE_FOR_4X = 5,
    OT_VENC_H264_REF_SLICE_FOR_BUTT
} ot_venc_h264_ref_slice_type;

typedef enum {
    OT_VENC_JPEG_PACK_ECS = 5,
    OT_VENC_JPEG_PACK_APP = 6,
    OT_VENC_JPEG_PACK_VDO = 7,
    OT_VENC_JPEG_PACK_PIC = 8,
    OT_VENC_JPEG_PACK_DCF = 9,
    OT_VENC_JPEG_PACK_DCF_PIC = 10,
    OT_VENC_JPEG_PACK_BUTT
} ot_venc_jpege_pack_type;

typedef enum {
    OT_VENC_PRORES_PACK_PIC = 1,
    OT_VENC_PRORES_PACK_BUTT
} ot_venc_prores_pack_type;

typedef enum {
    OT_VENC_MOSAIC_MODE_RECT = 0, /* user input mosaic rect */
    OT_VENC_MOSAIC_MODE_MAP,     /* user input mosaic map */
    OT_VENC_MOSAIC_MODE_BUTT
} ot_venc_mosaic_mode;

typedef union {
    ot_venc_h264_nalu_type h264_type;  /* AUTO:ot_payload_type:OT_PT_H264; */
    ot_venc_jpege_pack_type jpeg_type; /* AUTO:ot_payload_type:OT_PT_JPEG; */
    ot_venc_h265_nalu_type h265_type; /* AUTO:ot_payload_type:OT_PT_H265; */
    ot_venc_svac3_nalu_type svac3_type; /* AUTO:ot_payload_type:OT_PT_SVAC3; */
    ot_venc_prores_pack_type prores_type; /* AUTO:ot_payload_type:OT_PT_PRORES; */
} ot_venc_data_type;

typedef struct {
    ot_venc_data_type pack_type;
    td_u32 pack_offset;
    td_u32 pack_len;
} ot_venc_pack_info;

typedef struct {
    td_phys_addr_t phys_addr;
    td_u8 ATTRIBUTE *addr;
    td_u32 ATTRIBUTE len;
    td_u64 pts;
    td_bool is_frame_end;
    td_bool stuff03_disable;
    td_u32 layer_id;
    td_bool is_lu_begin; /* a layer's first nal  */
    ot_venc_data_type data_type;
    td_u32 offset;
    td_u32 data_num;
    ot_venc_pack_info pack_info[OT_VENC_MAX_PACK_INFO_NUM];
} ot_venc_pack;

/* Defines the frame type and reference attributes of the H.264 frame skipping reference streams */
typedef enum {
    OT_VENC_BASE_IDR_SLICE = 0,                     /* the Idr frame at Base layer */
    OT_VENC_BASE_P_SLICE_REF_TO_IDR,                /* the P frame at Base layer, referenced by other frames
                                                     * at Base layer and reference to Idr frame
                                                     */
    OT_VENC_BASE_P_SLICE_REF_BY_BASE,               /* the P frame at Base layer, referenced by other frames
                                                     * at Base layer
                                                     */
    OT_VENC_BASE_P_SLICE_REF_BY_ENHANCE,            /* the P frame at Base layer, referenced by other frames
                                                     * at Enhance layer
                                                     */
    OT_VENC_ENHANCE_P_SLICE_REF_BY_ENHANCE,         /* the P frame at Enhance layer, referenced by other frames
                                                     * at Enhance layer
                                                     */
    OT_VENC_ENHANCE_P_SLICE_NOT_FOR_REF,            /* the P frame at Enhance layer ,not referenced */
    OT_VENC_P_SLICE_BUTT
} ot_venc_ref_type;

typedef struct {
    td_u32 pic_bytes;
    td_u32 inter16x16_mb_num;
    td_u32 inter8x8_mb_num;
    td_u32 intra16_mb_num;
    td_u32 intra8_mb_num;
    td_u32 intra4_mb_num;
    ot_venc_ref_type ref_type;
    td_u32 update_attr_cnt;
    td_u32 start_qp;
    td_u32 mean_qp;
    td_bool is_p_skip;
} ot_venc_h264_stream_info;

typedef struct {
    td_u32 pic_bytes;
    td_u32 inter64x64_cu_num;
    td_u32 inter32x32_cu_num;
    td_u32 inter16x16_cu_num;
    td_u32 inter8x8_cu_num;
    td_u32 intra32x32_cu_num;
    td_u32 intra16x16_cu_num;
    td_u32 intra8x8_cu_num;
    td_u32 intra4x4_cu_num;
    ot_venc_ref_type ref_type;
    td_u32 update_attr_cnt;
    td_u32 start_qp;
    td_u32 mean_qp;
    td_bool is_p_skip;
} ot_venc_h265_stream_info;

typedef struct {
    td_u32 pic_bytes;
    td_u32 inter64x64_cu_num;
    td_u32 inter32x32_cu_num;
    td_u32 inter16x16_cu_num;
    td_u32 inter8x8_cu_num;
    td_u32 intra32x32_cu_num;
    td_u32 intra16x16_cu_num;
    td_u32 intra8x8_cu_num;
    td_u32 intra4x4_cu_num;
    ot_venc_ref_type ref_type;
    td_u32 update_attr_cnt;
    td_u32 start_qp;
    td_u32 mean_qp;
    td_bool is_p_skip;
    td_bool is_library;
    td_u32 library_picture_index;
    td_bool is_ras_begin; /* random access begin  */
} ot_venc_svac3_stream_info;

typedef struct {
    td_bool enable;
    td_u64 sse_val;
} ot_venc_sse_info;

typedef struct {
    td_u32 residual_bits;
    td_u32 head_bits;
    td_u32 madi_val;
    td_u32 madp_val;
    td_double psnr_val;
    td_u32 sse_lcu_cnt;
    td_u64 sse_sum;
    ot_venc_sse_info sse_info[OT_VENC_MAX_SSE_NUM];
    td_u32 qp_hist[OT_VENC_H264_QP_HIST_NUM];
    td_u32 move_scene16x16_num;
    td_u32 move_scene_bits;
    td_u32 deblur_near_bg_num;
    td_u32 deblur_far_bg_num;
} ot_venc_h264_adv_stream_info;

typedef struct {
    td_u32             residual_bits;
    td_u32             head_bits;
    td_u32             madi_val;
    td_u32             madp_val;
    td_double          psnr_val;
    td_u32             sse_lcu_cnt;
    td_u64             sse_sum;
    ot_venc_sse_info   sse_info[OT_VENC_MAX_SSE_NUM];
    td_u32             qp_hist[OT_VENC_H265_QP_HIST_NUM];
    td_u32             move_scene32x32_num;
    td_u32             move_scene_bits;
    td_u32             deblur_near_bg_num;
    td_u32             deblur_far_bg_num;
} ot_venc_h265_adv_stream_info;

typedef struct {
    td_u32             residual_bits;
    td_u32             head_bits;
    td_u32             madi_val;
    td_u32             madp_val;
    td_double          psnr_val;
    td_u32             sse_lcu_cnt;
    td_u64             sse_sum;
    ot_venc_sse_info   sse_info[OT_VENC_MAX_SSE_NUM];
    td_u32             qp_hist[OT_VENC_SVAC3_QP_HIST_NUM];
    td_u32             move_scene32x32_num;
    td_u32             move_scene_bits;
    td_u32             deblur_near_bg_num;
    td_u32             deblur_far_bg_num;
} ot_venc_svac3_adv_stream_info;

typedef struct {
    td_u32 pic_bytes;
    td_u32 update_attr_cnt;
} ot_venc_prores_stream_info;

typedef struct {
    td_u32 pic_bytes;
    td_u32 update_attr_cnt;
    td_u32 qfactor;
} ot_venc_jpeg_stream_info;

typedef struct {
    ot_venc_pack ATTRIBUTE *pack;
    td_u32 ATTRIBUTE pack_cnt;
    td_u32 seq;
    union {
        ot_venc_h264_stream_info   h264_info; /* AUTO:ot_payload_type:OT_PT_H264; */
        ot_venc_jpeg_stream_info   jpeg_info; /* AUTO:ot_payload_type:OT_PT_JPEG; */
        ot_venc_h265_stream_info   h265_info; /* AUTO:ot_payload_type:OT_PT_H265; */
        ot_venc_svac3_stream_info   svac3_info; /* AUTO:ot_payload_type:OT_PT_SVAC3; */
        ot_venc_prores_stream_info prores_info; /* AUTO:ot_payload_type:OT_PT_PRORES; */
    };
    union {
        ot_venc_h264_adv_stream_info h264_adv_info; /* AUTO:ot_payload_type:OT_PT_H264; */
        ot_venc_h265_adv_stream_info h265_adv_info; /* AUTO:ot_payload_type:OT_PT_H265; */
        ot_venc_svac3_adv_stream_info svac3_adv_info; /* AUTO:ot_payload_type:OT_PT_SVAC3; */
    };
} ot_venc_stream;

typedef struct {
    ot_venc_ref_type ref_type;
    td_u32  pic_bytes;
    td_u32  pic_cnt;
    td_u32  start_qp;
    td_u32  mean_qp;
    td_bool is_p_skip;
    td_u32  residual_bits;
    td_u32  head_bits;
    td_u32  madi_val;
    td_u32  madp_val;
    td_u64  sse_sum;
    td_u32  sse_lcu_cnt;
    td_double psnr_val;
} ot_venc_stream_info;

typedef struct {
    td_u8 large_thumbnail_num;
    ot_size large_thumbnail_size[OT_VENC_MAX_MPF_NUM];
} ot_venc_mpf_cfg;

typedef enum {
    OT_VENC_PIC_RECV_SINGLE = 0,
    OT_VENC_PIC_RECV_MULTI,
    OT_VENC_PIC_RECV_BUTT
} ot_venc_pic_recv_mode;

typedef struct {
    td_bool dcf_en;
    ot_venc_mpf_cfg mpf_cfg;
    ot_venc_pic_recv_mode recv_mode;
} ot_venc_jpeg_attr;

typedef struct {
    td_bool rcn_ref_share_buf_en;
    td_u32 frame_buf_ratio;
} ot_venc_h264_attr;

typedef struct {
    td_bool rcn_ref_share_buf_en;
    td_u32 frame_buf_ratio;
} ot_venc_h265_attr;

typedef struct {
    td_bool rcn_ref_share_buf_en;
    td_u32 frame_buf_ratio;
} ot_venc_svac3_attr;

typedef enum {
    OT_VENC_PRORES_FRAME_RATE_UNKNOWN = 0,
    OT_VENC_PRORES_FRAME_RATE_23_976,
    OT_VENC_PRORES_FRAME_RATE_24,
    OT_VENC_PRORES_FRAME_RATE_25,
    OT_VENC_PRORES_FRAME_RATE_29_97,
    OT_VENC_PRORES_FRAME_RATE_30,
    OT_VENC_PRORES_FRAME_RATE_50,
    OT_VENC_PRORES_FRAME_RATE_59_94,
    OT_VENC_PRORES_FRAME_RATE_60,
    OT_VENC_PRORES_FRAME_RATE_100,
    OT_VENC_PRORES_FRAME_RATE_119_88,
    OT_VENC_PRORES_FRAME_RATE_120,
    OT_VENC_PRORES_FRAME_RATE_BUTT
} ot_venc_prores_frame_rate;

typedef enum {
    OT_VENC_PRORES_ASPECT_RATIO_UNKNOWN = 0,
    OT_VENC_PRORES_ASPECT_RATIO_SQUARE,
    OT_VENC_PRORES_ASPECT_RATIO_4_3,
    OT_VENC_PRORES_ASPECT_RATIO_16_9,
    OT_VENC_PRORES_ASPECT_RATIO_BUTT
} ot_venc_prores_aspect_ratio;

typedef struct {
    td_char identifier[OT_VENC_PRORES_MAX_ID_CHAR_NUM];
    ot_venc_prores_frame_rate frame_rate_code;
    ot_venc_prores_aspect_ratio aspect_ratio;
} ot_venc_prores_attr;

typedef struct {
    ot_payload_type type;
    td_u32  max_pic_width;
    td_u32  max_pic_height;
    td_u32  buf_size;
    td_u32  profile;
    td_bool is_by_frame;
    td_u32  pic_width;
    td_u32  pic_height;
    union {
        ot_venc_h264_attr h264_attr; /* AUTO:ot_payload_type:OT_PT_H264; */
        ot_venc_h265_attr h265_attr; /* AUTO:ot_payload_type:OT_PT_H265; */
        ot_venc_jpeg_attr  jpeg_attr; /* AUTO:ot_payload_type:OT_PT_JPEG; */
        ot_venc_prores_attr prores_attr; /* AUTO:ot_payload_type:OT_PT_PRORES; */
        ot_venc_svac3_attr svac3_attr; /* AUTO:ot_payload_type:OT_PT_SVAC3; */
    };
} ot_venc_attr;

typedef enum {
    OT_VENC_GOP_MODE_NORMAL_P    = 0,
    OT_VENC_GOP_MODE_DUAL_P      = 1,
    OT_VENC_GOP_MODE_SMART_P     = 2,
    OT_VENC_GOP_MODE_ADV_SMART_P = 3,
    OT_VENC_GOP_MODE_BIPRED_B    = 4,
    OT_VENC_GOP_MODE_LOW_DELAY_B = 5,
    OT_VENC_GOP_MODE_SMART_CRR   = 6,
    OT_VENC_GOP_MODE_BUTT,
} ot_venc_gop_mode;

typedef struct {
    td_s32 ip_qp_delta; /* RW; range:[-10, 30]; */
} ot_venc_gop_normal_p;

typedef struct {
    td_u32 sp_interval; /* RW; range:[0, 1) or (1, gop-1], gop is the I frame interval; */
    td_s32 sp_qp_delta; /* RW; range:[-10, 30]; */
    td_s32 ip_qp_delta; /* RW; range:[-10, 30]; */
} ot_venc_gop_dual_p;

typedef struct {
    td_u32 bg_interval; /* RW; range:[gop, 65536] and must be an integer multiple of gop; */
    td_s32 bg_qp_delta; /* RW; range:[-10, 30]; */
    td_s32 vi_qp_delta; /* RW; range:[-10, 30]; */
} ot_venc_gop_smart_p;

typedef struct {
    td_u32 bg_interval; /* RW; range:[gop, 65536] and must be an integer multiple of gop; */
    td_s32 bg_qp_delta; /* RW; range:[-10, 30]; */
    td_s32 vi_qp_delta; /* RW; range:[-10, 30]; */
} ot_venc_gop_adv_smart_p;

typedef struct {
    td_u32 b_frame_num; /* RW; range:[1, 3]; */
    td_s32 b_qp_delta; /* RW; range:[-10, 30]; */
    td_s32 ip_qp_delta; /* RW; range:[-10, 30]; */
} ot_venc_gop_bipred_b;

typedef enum {
    OT_VENC_CRR_RECODE_DISABLE  = 0,   /* not recode  frame */
    OT_VENC_CRR_RECODE_NORMAL = 1,     /* recode frame for display */
    OT_VENC_CRR_RECODE_COPY   = 2,     /* copy next frame stream to replace splited frame */
    OT_VENC_CRR_RECODE_WRITE_BACK = 3,     /* multi chn use one wirte back buf to recode */
    OT_VENC_CRR_RECODE_BUTT,
} ot_venc_crr_recode_strategy;

typedef struct {
    td_u32 bg_interval; /* RW; range:[gop, 65536] and must be an integer multiple of gop; */
    td_s32 bg_qp_delta; /* RW; range:[-10, 30]; */
    td_s32 vi_qp_delta; /* RW; range:[-10, 30]; */
    td_s32 crr_split_num; /* RW; range:[1, gop); */
    td_s32 crr_delay_num; /* RW; formula: (crr_split_num-1) * (crr_delay_num+1) + 2 < gop; */
    ot_venc_crr_recode_strategy strategy;
} ot_venc_gop_smart_crr;

typedef struct {
    ot_venc_gop_mode gop_mode;
    union {
        ot_venc_gop_normal_p    normal_p; /* AUTO:ot_venc_gop_mode:OT_VENC_GOP_MODE_NORMAL_P; */
        ot_venc_gop_dual_p      dual_p; /* AUTO:ot_venc_gop_mode:OT_VENC_GOP_MODE_DUAL_P; */
        ot_venc_gop_smart_p     smart_p; /* AUTO:ot_venc_gop_mode:OT_VENC_GOP_MODE_SMART_P; */
        ot_venc_gop_adv_smart_p adv_smart_p; /* AUTO:ot_venc_gop_mode:OT_VENC_GOP_MODE_ADV_SMART_P; */
        ot_venc_gop_bipred_b    bipred_b; /* AUTO:ot_venc_gop_mode:OT_VENC_GOP_MODE_BIPRED_B; */
        ot_venc_gop_smart_crr   smart_crr; /* AUTO:ot_venc_gop_mode:OT_VENC_GOP_MODE_SMART_CRR; */
    };
} ot_venc_gop_attr;

typedef struct {
    ot_venc_attr venc_attr;
    ot_venc_rc_attr rc_attr;
    ot_venc_gop_attr gop_attr;
} ot_venc_chn_attr;

typedef struct {
    td_s32 recv_pic_num;
} ot_venc_start_param;

typedef struct {
    td_u32 left_pics;
    td_u32 left_stream_bytes;
    td_u32 left_stream_frames;
    td_u32 cur_packs;
    td_u32 left_recv_pics;
    td_u32 left_enc_pics;
    td_bool is_jpeg_snap_end;
    td_u64 release_pic_pts;
    ot_venc_stream_info stream_info;
    td_u32 discard_pics;
    td_u32 stream_buf_full_cnt;
} ot_venc_chn_status;

typedef struct {
    td_u32 constrained_intra_pred_flag;
} ot_venc_h264_intra_pred;

typedef struct {
    td_u32  intra_trans_mode; /* R; 0: 4x4, 8x8 conversion, high profile, svc-t are all supported;
                                    1: 4x4 conversion, baseline, main, high profile, svc-t are all supported;
                                    2: 8x8 conversion, high profile supports it; */
    td_u32  inter_trans_mode; /* R; 0: 4x4, 8x8 conversion, high profile, svc-t are all supported;
                                    1: 4x4 conversion, baseline, main, high profile, svc-t are all supported;
                                    2: 8x8 conversion, high profile supports it; */
    td_bool scaling_list_valid; /* R; range:[0, 1]; only supports configuration 0; */
    td_u8   inter_scaling_list8x8[OT_VENC_SCALING_LIST_SIZE_64]; /* R; range:[1, 255]; */
    td_u8   intra_scaling_list8x8[OT_VENC_SCALING_LIST_SIZE_64]; /* R; range:[1, 255]; */
    td_s32  chroma_qp_index_offset; /* RW; range:[-12, 12]; */
} ot_venc_h264_trans;

typedef struct {
    td_u32 entropy_coding_mode_i;
    td_u32 entropy_coding_mode_p;
    td_u32 entropy_coding_mode_b;
    td_u32 cabac_init_idc;
} ot_venc_h264_entropy;

typedef struct {
    td_u32 pic_order_cnt_type;
} ot_venc_h264_poc;

typedef struct {
    td_u32 disable_deblocking_filter_idc;
    td_s32 slice_alpha_c0_offset_div2;
    td_s32 slice_beta_offset_div2;
} ot_venc_h264_dblk;

typedef struct {
    td_u8  timing_info_present_flag;
    td_u8  fixed_frame_rate_flag;
    td_u32 num_units_in_tick;
    td_u32 time_scale;
} ot_venc_h264_vui_time_info;

typedef struct {
    td_u8  aspect_ratio_info_present_flag;
    td_u8  aspect_ratio_idc;
    td_u8  overscan_info_present_flag;
    td_u8  overscan_appropriate_flag;
    td_u16 sar_width;
    td_u16 sar_height;
} ot_venc_vui_aspect_ratio;

typedef struct {
    td_u8 video_signal_type_present_flag;
    td_u8 video_format;
    td_u8 video_full_range_flag;
    td_u8 colour_description_present_flag;
    td_u8 colour_primaries;
    td_u8 transfer_characteristics;
    td_u8 matrix_coefficients;
} ot_venc_vui_video_signal;

typedef struct {
    td_u8 bitstream_restriction_flag;
} ot_venc_vui_bitstream_restric;

typedef struct {
    ot_venc_vui_aspect_ratio      vui_aspect_ratio;
    ot_venc_h264_vui_time_info    vui_time_info;
    ot_venc_vui_video_signal      vui_video_signal;
    ot_venc_vui_bitstream_restric vui_bitstream_restric;
} ot_venc_h264_vui;

typedef struct {
    td_u32 timing_info_present_flag;
    td_u32 num_units_in_tick;
    td_u32 time_scale;
    td_u32 num_ticks_poc_diff_one_minus1;
} ot_venc_h265_vui_time_info;

typedef struct {
    ot_venc_vui_aspect_ratio      vui_aspect_ratio;
    ot_venc_h265_vui_time_info    vui_time_info;
    ot_venc_vui_video_signal      vui_video_signal;
    ot_venc_vui_bitstream_restric vui_bitstream_restric;
} ot_venc_h265_vui;

typedef struct {
    td_u32 qfactor; /* RW; range:[1, 99]; */
    td_u8  y_qt[OT_VENC_JPEG_QT_COEF_NUM];
    td_u8  cb_qt[OT_VENC_JPEG_QT_COEF_NUM];
    td_u8  cr_qt[OT_VENC_JPEG_QT_COEF_NUM];
    td_u32 mcu_per_ecs;
    td_bool ecs_output_en;
} ot_venc_jpeg_param;

typedef struct {
    td_u8  y_qt[OT_VENC_MJPEG_QT_COEF_NUM];
    td_u8  cb_qt[OT_VENC_MJPEG_QT_COEF_NUM];
    td_u8  cr_qt[OT_VENC_MJPEG_QT_COEF_NUM];
    td_u32 mcu_per_ecs;
    td_bool ecs_output_en;
} ot_venc_mjpeg_param;

typedef struct {
    td_u32  idx; /* RW; range:[0, 7]; */
    td_bool enable; /* RW; range:[0, 1]; */
    td_bool is_abs_qp; /* RW; range:[0, 1]; */
    td_s32  qp; /* RW; when is_abs_qp is TD_FALSE, Range: for H264 and H265: [-51, 51], for SVAC3: [-63, 63];
                 * when is_abs_qp is TD_TRUE, Range: for H264 and H265: [0, 51], for SVAC3: [0, 63];
                 */
    ot_rect rect;
} ot_venc_roi_attr;

typedef enum {
    OT_VENC_ROI_FOR_I = 0,
    OT_VENC_ROI_FOR_P_B,
    OT_VENC_ROI_FOR_VI,
    OT_VENC_ROI_FOR_BUTT,
} ot_venc_roi_for_frame;

typedef struct {
    td_u32  idx;
    td_bool enable[OT_VENC_ROI_FOR_BUTT];
    td_bool is_abs_qp[OT_VENC_ROI_FOR_BUTT];
    td_s32  qp[OT_VENC_ROI_FOR_BUTT];
    ot_rect rect[OT_VENC_ROI_FOR_BUTT];
} ot_venc_roi_attr_ex;

typedef struct {
    td_u32 idx;
    td_bool enable;
    td_u32 level;
    ot_rect rect;
} ot_venc_jpeg_roi_attr;

typedef struct {
    td_u32 idx;
    td_bool enable;
    td_u32 fg_level;
    td_u32 bg_level;
    ot_rect rect;
} ot_venc_jpeg_roi_adv_attr;

typedef struct {
    td_s32 src_frame_rate; /* RW; range:-1 or [1, 65536); */
    td_s32 dst_frame_rate; /* RW; range:-1 or [0, src_frame_rate]; */
} ot_venc_roi_bg_frame_rate;

typedef struct {
    td_u32  base; /* RW; range:greater than 0; */
    td_u32  enhance; /* RW; range:[0, 255]; */
    td_bool pred_en; /* RW; range:[0, 1]; */
    td_bool base_qp_delta_en; /* RW; range:[0, 1]; */
    td_s32  base_qp_delta; /* RW; range:[-10, 10]; */
} ot_venc_ref_param;

typedef enum {
    OT_VENC_JPEG_DERING_LEVEL_0 = 0,                  /* Jpeg channel dering mode is 1 */
    OT_VENC_JPEG_DERING_LEVEL_1 = 1,                  /* Jpeg channel dering mode is adaptive  */
    OT_VENC_JPEG_DERING_LEVEL_2 = 2,                  /* Jpeg channel dering mode is adaptive  */
    OT_VENC_JPEG_DERING_LEVEL_3 = 3,                  /* Jpeg channel dering mode is adaptive  */
    OT_VENC_JPEG_DERING_LEVEL_BUTT,
} ot_venc_jpeg_dering_level;

typedef struct {
    td_u32  dering_en; /* RW; range:[0, 1]; */
    ot_venc_jpeg_dering_level dering_level;
} ot_venc_jpeg_dering_attr;

typedef enum {
    OT_VENC_JPEG_ENC_ALL  = 0,                        /* Jpeg channel snap all the pictures when started. */
    OT_VENC_JPEG_ENC_SNAP = 1,                        /* Jpeg channel snap the flashed pictures when started. */
    OT_VENC_JPEG_ENC_BUTT,
} ot_venc_jpeg_enc_mode;

typedef struct {
    td_phys_addr_t phys_addr[OT_VENC_MAX_TILE_NUM];
    td_void ATTRIBUTE *user_addr[OT_VENC_MAX_TILE_NUM];
    td_u64 ATTRIBUTE buf_size[OT_VENC_MAX_TILE_NUM];
} ot_venc_stream_buf_info;

typedef struct {
    td_u32 constrained_intra_pred_flag;
    td_u32 strong_intra_smoothing_enabled_flag;
} ot_venc_h265_pu;

typedef struct {
    td_s32  cb_qp_offset; /* RW; range:[-12, 12]; */
    td_s32  cr_qp_offset; /* RW; range:[-12, 12]; */
    td_bool scaling_list_en; /* R; range:[0, 1]; */
    td_bool scaling_list_tu4_valid; /* R; range:[0, 1]; */
    td_u8   inter_scaling_list4x4[2][OT_VENC_SCALING_LIST_SIZE_16]; /* R; 2: 0 luma; 1 chroma  range:16; */
    td_u8   intra_scaling_list4x4[2][OT_VENC_SCALING_LIST_SIZE_16]; /* R; 2: 0 luma; 1 chroma  range:16; */
    td_bool scaling_list_tu8_valid; /* R; range:[0, 1]; */
    td_u8   inter_scaling_list8x8[2][OT_VENC_SCALING_LIST_SIZE_64]; /* R; 2: 0 luma; 1 chroma  range:[1, 255]; */
    td_u8   intra_scaling_list8x8[2][OT_VENC_SCALING_LIST_SIZE_64]; /* R; 2: 0 luma; 1 chroma  range:[1, 255]; */
    td_bool scaling_list_tu16_valid; /* R; range:[0, 1]; */
    td_u8   inter_scaling_list16x16[2][OT_VENC_SCALING_LIST_SIZE_64]; /* R; 2: 0 luma; 1 chroma  range:[1, 255]; */
    td_u8   intra_scaling_list16x16[2][OT_VENC_SCALING_LIST_SIZE_64]; /* R; 2: 0 luma; 1 chroma  range:[1, 255]; */
    td_bool scaling_list_tu32_valid; /* R; range:[0, 1]; */
    td_u8   inter_scaling_list32x32[OT_VENC_SCALING_LIST_SIZE_64]; /* R; range:[1, 255]; */
    td_u8   intra_scaling_list32x32[OT_VENC_SCALING_LIST_SIZE_64]; /* R; range:[1, 255]; */
} ot_venc_h265_trans;

typedef struct {
    td_u32 cabac_init_flag;
} ot_venc_h265_entropy;

typedef struct {
    td_u32 slice_deblocking_filter_disabled_flag;
    td_s32 slice_beta_offset_div2;
    td_s32 slice_tc_offset_div2;
    td_u32 loop_filter_across_tiles_enabled_flag;
    td_u32 loop_filter_across_slices_enabled_flag;
} ot_venc_h265_dblk;

typedef struct {
    td_u32 slice_sao_luma_flag;
    td_u32 slice_sao_chroma_flag;
} ot_venc_h265_sao;

typedef enum {
    OT_VENC_INTRA_REFRESH_ROW = 0,
    OT_VENC_INTRA_REFRESH_COLUMN,
    OT_VENC_INTRA_REFRESH_BUTT
} ot_venc_intra_refresh_mode;

typedef struct {
    td_bool enable; /* RW; range:[0, 1]; */
    ot_venc_intra_refresh_mode mode;
    td_u32 refresh_num; /* RW; indicates the number of rows or columns of I macroblock refresh;
                                H264: row: refresh_num*max_refresh_frame_in_gop >= (pic_height + 15) / 16;
                                column: refresh_num*max_refresh_frame_in_gop >= (pic_width + 15) / 16;
                        H265/SVAC3: row: refresh_num*max_refresh_frame_in_gop >= (pic_height + lcu_size - 1) / lcu_size
                                column: refresh_num*Max_refresh_frame_in_gop >= (pic_width + lcu_size - 1) / lcu_size
                                                */
    td_u32 request_i_qp; /* RW; range: [0, 51]; for svac3: [0, 63]; */
} ot_venc_intra_refresh;

typedef enum {
    OT_VENC_MOD_VENC = 1,
    OT_VENC_MOD_H264,
    OT_VENC_MOD_H265,
    OT_VENC_MOD_JPEG,
    OT_VENC_MOD_RC,
    OT_VENC_MOD_SVAC3,
    OT_VENC_MOD_BUTT
} ot_venc_mod_type;

typedef struct {
    td_u32 buf_cache;
    td_u32 frame_buf_recycle;
} ot_venc_venc_mod_param;

typedef struct {
    td_u32    one_stream_buf;
    td_u32    mini_buf_mode;
    td_u32    low_power_mode;
    ot_vb_src vb_src;
    td_bool   qp_hist_en;
    td_u32    max_user_data_len; /* RW; Range:[0, 65536]; one user data buffer len */
} ot_venc_h264_mod_param;

typedef struct {
    td_u32    one_stream_buf;
    td_u32    mini_buf_mode;
    td_u32    low_power_mode;
    ot_vb_src vb_src;
    td_bool   qp_hist_en;
    td_u32    max_user_data_len; /* RW; Range:[0, 65536]; one user data buffer len */
} ot_venc_h265_mod_param;

typedef struct {
    td_u32    one_stream_buf;
    td_u32    mini_buf_mode;
    td_u32    low_power_mode;
    ot_vb_src vb_src;
    td_bool   qp_hist_en;
    td_u32    max_user_data_len; /* RW; Range:[0, 65536]; one user data buffer len */
} ot_venc_svac3_mod_param;

typedef struct {
    td_u32 one_stream_buf;
    td_u32 mini_buf_mode;
    td_u32 clear_stream_buf;
} ot_venc_jpeg_mod_param;

typedef struct {
    ot_venc_mod_type mod_type;
    union {
        ot_venc_venc_mod_param venc_mod_param; /* AUTO:ot_venc_mod_type:OT_VENC_MOD_VENC; */
        ot_venc_h264_mod_param h264_mod_param; /* AUTO:ot_venc_mod_type:OT_VENC_MOD_H264; */
        ot_venc_h265_mod_param h265_mod_param; /* AUTO:ot_venc_mod_type:OT_VENC_MOD_H265; */
        ot_venc_svac3_mod_param svac3_mod_param; /* AUTO:ot_venc_mod_type:OT_VENC_MOD_SVAC3; */
        ot_venc_jpeg_mod_param jpeg_mod_param; /* AUTO:ot_venc_mod_type:OT_VENC_MOD_JPEG; */
    };
} ot_venc_mod_param;

typedef enum {
    OT_VENC_FRAME_TYPE_NONE = 1,
    OT_VENC_FRAME_TYPE_IDR,
    OT_VENC_FRAME_TYPE_P_SKIP,
    OT_VENC_FRAME_TYPE_BUTT
} ot_venc_frame_type;

typedef struct {
    td_bool qpmap_valid;
    td_bool skip_weight_valid;
    td_u32  blk_start_qp;
    td_phys_addr_t qpmap_phys_addr;
    td_phys_addr_t skip_weight_phys_addr;
    ot_venc_frame_type frame_type; /* no use when use mpi_venc_send_qpmap */
} ot_venc_user_rc_info;

typedef struct {
    td_u32 width;  /* encode width */
    td_u32 height; /* encode height */
    td_u64 pts;  /* used to debug */
    ot_venc_user_rc_info user_rc_info;
} ot_venc_user_qpmap;

typedef struct {
    td_bool valid;
    td_phys_addr_t phys_addr;
} ot_venc_user_roimap;

typedef struct {
    td_u8 data_y;
    td_u8 data_u;
    td_u8 data_v;
} ot_venc_mosaic_pixel_yuv;

typedef struct {
    td_u32 rect_num;
    ot_rect rect[OT_VENC_MAX_MOSAIC_RECT_NUM];
} ot_venc_mosaic_rect;

typedef struct {
    td_bool valid;
    td_phys_addr_t phys_addr;
    td_bool specified_yuv_en;
    ot_venc_mosaic_pixel_yuv pixel_yuv;
} ot_venc_mosaic_map;

/* the information of the mosaic */
typedef struct {
    ot_venc_mosaic_mode mode;
    ot_mosaic_blk_size blk_size;

    union {
        ot_venc_mosaic_rect rect_param; /* AUTO:ot_venc_mosaic_mode:OT_VENC_MOSAIC_MODE_RECT; */
        ot_venc_mosaic_map map_param; /* AUTO:ot_venc_mosaic_mode:OT_VENC_MOSAIC_MODE_MAP; */
    };
} ot_venc_mosaic_info;

typedef struct {
    ot_video_frame_info user_frame;
    ot_venc_user_rc_info user_rc_info;
    ot_venc_user_roimap user_roimap;
    ot_venc_mosaic_info user_mosaic;
} ot_venc_user_frame_info;

/* the information of the multi frame */
typedef struct {
    td_u32 frame_num;
    ot_video_frame_info frame[OT_VENC_MAX_FRAME_NUM];
    ot_venc_mosaic_info mosaic_info;
} ot_venc_multi_frame_info;

typedef struct {
    td_bool valid;
    td_u32 crop_num;
    ot_rect crop_rect[OT_VENC_MAX_BATCH_CROP_RECT_NUM];
} ot_venc_batch_crop_info;

typedef struct {
    ot_video_frame_info frame;
    ot_venc_batch_crop_info batch_crop_info;
} ot_venc_frame_batch_crop_info;

typedef enum {
    OT_VENC_DUPLICATE_FRAME_STRATEGE_RECODE = 0,
    OT_VENC_DUPLICATE_FRAME_STRATEGE_COPY,
    OT_VENC_DUPLICATE_FRAME_STRATEGE_BUTT
} ot_venc_duplicate_frame_strategy;

typedef struct {
    td_bool mosaic_en;
    td_bool composite_enc_en;
    td_u32 quality_level; /* 0: lower quality with less ddr consumption; 1: higher quality with more ddr consumption. */
    ot_venc_svc_version svc_version;
    ot_venc_duplicate_frame_strategy duplicate_frame_strategy;
} ot_venc_chn_config;

typedef struct {
    td_u32  idx;
    td_bool enable;
    ot_rect rect;
} ot_venc_sse_rgn;

typedef struct {
    td_bool color_to_grey_en; /* RW; range:[0, 1]; */
    td_u32  priority;
    td_u32  max_stream_cnt;
    td_u32  poll_wake_up_frame_cnt;
    ot_crop_info crop_info;
    ot_frame_rate_ctrl frame_rate;
    td_u32 in_depth; /* RW; Range: [1, OT_VENC_MAX_IN_DEPTH]; Depth of chn image list. */
} ot_venc_chn_param;

typedef struct {
    td_bool enable; /* RW; range:[0, 1]; */
    td_u32  direction; /* RW; range:[0, 16]; */
    td_u32  gain; /* RW; range:[0, 15]; */
    td_u32  offset; /* RW; range:[0, 255]; */
    td_u32  threshold_p[OT_VENC_TEXTURE_THRESHOLD_SIZE]; /* RW; range:[0, 255]; */
    td_u32  threshold_b[OT_VENC_TEXTURE_THRESHOLD_SIZE]; /* RW; range:[0, 255]; */
} ot_venc_fg_protect;

typedef enum {
    OT_VENC_SCENE_0 = 0,        /* RW; A scene in which the camera does not move or periodically moves continuously */
    OT_VENC_SCENE_1 = 1,        /* RW; Motion scene at high bit rate */
    OT_VENC_SCENE_2 = 2,        /* RW; It has regular continuous motion at medium bit rate and
                                   the encoding pressure is relatively large */
    OT_VENC_SCENE_BUTT
} ot_venc_scene_mode;

typedef struct {
    ot_op_mode pred_mode;
    td_u32 intra32_cost;
    td_u32 intra16_cost;
    td_u32 intra8_cost;
    td_u32 intra4_cost;
    td_u32 inter64_cost;
    td_u32 inter32_cost;
    td_u32 inter16_cost;
    td_u32 inter8_cost;
} ot_venc_cu_pred;

typedef struct {
    td_bool enable;
    td_u32  gain;
    td_u32  offset;
    td_u32  bg_cost;
    td_u32  fg_cost;
} ot_venc_skip_bias;

typedef struct {
    ot_vb_pool pic_vb_pool;
    ot_vb_pool pic_info_vb_pool;
} ot_venc_chn_pool;

typedef struct {
    td_bool enable;
    td_u32 split_mode;
    td_u32 split_size;
    td_bool slice_output_en;
} ot_venc_slice_split;

typedef struct {
    td_u32 param_set_id; /* RW; range: for H264: [0, 30]; for H265: [0, 15]; */
} ot_venc_param_set_id;

typedef struct {
    ot_op_mode mode;
    td_u32 hor;
    td_u32 ver;
} ot_venc_search_window;

typedef enum {
    SVC_RECT_TYPE0 = 0,
    SVC_RECT_TYPE1,
    SVC_RECT_TYPE2,
    SVC_RECT_TYPE3,
    SVC_RECT_TYPE4,
    SVC_RECT_TYPE5,
    SVC_RECT_TYPE6,
    SVC_RECT_TYPE7,
    SVC_RECT_TYPE8,
    SVC_RECT_TYPE9,
    SVC_RECT_TYPE10,
    SVC_RECT_TYPE11,
    SVC_RECT_TYPE12,
    SVC_RECT_TYPE13,
    SVC_RECT_TYPE14,
    SVC_RECT_TYPE15,
    SVC_RECT_TYPE_BUTT
} ot_venc_svc_rect_type;

typedef struct {
    td_u8 qpmap_value_i;   /* RW;Range: [0, 255] */
    td_u8 qpmap_value_p;   /* RW;Range: [0, 255] */
    td_u8 skipmap_value;  /* RW;Range: [0, 255] */
} ot_venc_svc_map_param;

typedef enum {
    OT_VENC_SVC_ROI_TYPE_RECT = 0,
    OT_VENC_SVC_ROI_TYPE_MASK,
    OT_VENC_SVC_ROI_TYPE_BUTT
} ot_venc_svc_roi_type;

typedef struct {
    td_u32 rect_num;
    ot_size base_resolution;
    ot_rect rect_attr[OT_VENC_MAX_SVC_RECT_NUM];
    ot_venc_svc_rect_type detect_type[OT_VENC_MAX_SVC_RECT_NUM];
    td_u64 pts;
} ot_venc_svc_rect_info;

typedef struct {
    ot_size base_resolution;
    ot_vb_blk mask_blk;
    td_u64 pts;
} ot_venc_svc_mask_info;

typedef struct {
    ot_venc_svc_map_param  fg_region[SVC_RECT_TYPE_BUTT];
    ot_venc_svc_map_param  activity_region;
    ot_venc_svc_map_param  bg_region;
    td_bool fg_protect_adaptive_en; /* RW;Range: [0, 1] */
} ot_venc_svc_param;

typedef ot_venc_svc_param ot_venc_svc_param_v1;

typedef struct {
    td_u32 max_ref_num; /* RW; range:[2, 4]; */
    td_u32 refresh_interval; /* RW; range:[1, 21]; */
    td_s32 qp_delta[SVC_RECT_TYPE_BUTT]; /* RW; range:[-16, 15]; */
    /* RW; Range: Hi3516CV608 = [OT_VENC_SVC_ROI_TYPE_RECT];
    Hi3516CV610 = [OT_VENC_SVC_ROI_TYPE_RECT, OT_VENC_SVC_ROI_TYPE_MASK]. */
    ot_venc_svc_roi_type roi_type;
} ot_venc_svc_param_v2;

typedef struct {
    /* RW; Range: Hi3519DV500 = [OT_VENC_SVC_V1]; Hi3516CV610/Hi3516CV608 = [OT_VENC_SVC_V1, OT_VENC_SVC_V2]. */
    ot_venc_svc_version svc_version;
    union {
        ot_venc_svc_param_v1 svc_param_v1; /* AUTO:ot_venc_svc_version:OT_VENC_SVC_V1; */
        ot_venc_svc_param_v2 svc_param_v2; /* AUTO:ot_venc_svc_version:OT_VENC_SVC_V2; */
    };
} ot_venc_svc_param_ex;

typedef struct {
    td_bool deblur_en; /* RW; range:[0, 1]; */
    td_bool deblur_adaptive_en; /* RW; range:[0, 1]; */
} ot_venc_deblur_param;

typedef struct {
    td_u8 gain; /* RW; range:[0, 15]; */
    td_u8 offset; /* RW; range:[0, 255]; */
    td_u8 qp_delta; /* RW; range:[0, 15]; */
    td_u8 min_qp; /* RW; range:[0, 51]; for svac3: [0, 63]; */
    td_u8 max_qp; /* RW; range:[0, 51]; for svac3: [0, 63]; */
} ot_venc_deblur_near_bg;

typedef struct {
    td_u8 qp_delta; /* RW; range:[0, 15]; */
    td_u8 min_qp; /* RW; range:[0, 51]; for svac3: [0, 63]; */
    td_u8 max_qp; /* RW; range:[0, 51]; for svac3: [0, 63]; */
} ot_venc_deblur_far_bg;

typedef struct {
    td_bool near_bg_en; /* RW; range:[0, 1]; */
    td_bool far_bg_en; /* RW; range:[0, 1]; */
    ot_venc_deblur_near_bg near_bg;
    ot_venc_deblur_far_bg far_bg;
} ot_venc_adv_deblur;

typedef enum {
    OT_VENC_MD_SAD_MODE_ZERO = 0,
    OT_VENC_MD_SAD_MODE_SEARCH = 1,
    OT_VENC_MD_SAD_MODE_BUTT
} ot_venc_md_sad_mode;

typedef struct {
    td_bool online_en;
    td_bool sad_stats_en;
    td_bool level_stats_en;
    ot_venc_md_sad_mode sad_mode;
    td_s32 threshold_gain[OT_VENC_MD_THRESHOLD_NUM];
    td_s32 threshold_offset[OT_VENC_MD_THRESHOLD_NUM];
    td_s32 qp_delta[OT_VENC_MD_LEVEL_NUM];
    td_s32 md_skip_weight[OT_VENC_MD_LEVEL_NUM];
} ot_venc_md_param;

/* the information of the user frame */
typedef struct {
    td_bool rc_info_valid;
    td_u32 target_bits;
    td_u32 max_qp;
    td_u32 min_qp;
    td_u32 start_qp;

    ot_venc_frame_type frame_type;
} ot_omx_user_rc_info;

typedef struct {
    ot_video_frame_info user_frame;
    ot_omx_user_rc_info user_rc_info;
} ot_omx_user_frame_info;

typedef struct {
    td_bool enable;
    td_bool level_adaptive_en;
    td_u32 interval;
    td_u32 target_ratio;
    td_s32 ratio_offset_i;
    td_s32 ratio_offset_p;
    td_u32 quality_switch;
    td_u8 threshold[OT_VENC_MAX_QUALITY_BALANCE_NUM];
    td_u8 level[OT_VENC_MAX_QUALITY_BALANCE_NUM];
} ot_venc_quality_balance;

typedef struct {
    td_bool enable;
    td_u8 strength;
    td_u8 component;
    td_u8 template_period;
    td_u8 info[OT_VENC_MAX_WATERMARK_INFO_NUM];
    td_u32 seed[OT_VENC_MAX_WATERMARK_SEED_NUM];
} ot_venc_watermark;

typedef struct {
    td_bool enable;
    td_u8 strength;
    td_u32 top;
    td_u32 bottom;
    td_u32 left;
    td_u32 right;
} ot_venc_watermark_roi;

typedef struct {
    td_u32 chroma_quant_param_disable_flag;
    td_s32 chroma_quant_param_delta_cb;
    td_s32 chroma_quant_param_delta_cr;
} ot_venc_svac3_trans;

typedef struct {
    td_u32 deblocking_filter_disable_flag;
    td_u32 deblocking_filter_parameter_flag;
    td_s32 alpha_c_offset;
    td_s32 beta_offset;
    td_u32 cross_patch_loop_filter_enable_flag;
} ot_venc_svac3_dblk;

typedef struct {
    td_u32 patch_sao_luma_flag;
    td_u32 patch_sao_chroma_flag;
} ot_venc_svac3_sao;

typedef struct {
    td_bool stuff03_disable;
} ot_venc_svac3_emulation;

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* OT_COMMON_VENC_H */
