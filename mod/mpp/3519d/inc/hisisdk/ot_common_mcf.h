/*
  Copyright (c), 2001-2024, Shenshu Tech. Co., Ltd.
 */

#ifndef OT_COMMON_MCF_H
#define OT_COMMON_MCF_H

#include "ot_type.h"
#include "ot_common.h"
#include "ot_common_video.h"


#define OT_ERR_NOT_ENCHN  0x43

#define OT_ERR_MCF_NULL_PTR OT_DEFINE_ERR(OT_ID_MCF, OT_ERR_LEVEL_ERROR, OT_ERR_NULL_PTR)
#define OT_ERR_MCF_NOT_READY OT_DEFINE_ERR(OT_ID_MCF, OT_ERR_LEVEL_ERROR, OT_ERR_SYS_NOT_READY)
#define OT_ERR_MCF_INVALID_DEVID OT_DEFINE_ERR(OT_ID_MCF, OT_ERR_LEVEL_ERROR, OT_ERR_INVALID_DEV_ID)
#define OT_ERR_MCF_INVALID_PIPEID OT_DEFINE_ERR(OT_ID_MCF, OT_ERR_LEVEL_ERROR, OT_ERR_INVALID_PIPE_ID)
#define OT_ERR_MCF_INVALID_CHNID OT_DEFINE_ERR(OT_ID_MCF, OT_ERR_LEVEL_ERROR, OT_ERR_INVALID_CHN_ID)
#define OT_ERR_MCF_EXIST OT_DEFINE_ERR(OT_ID_MCF, OT_ERR_LEVEL_ERROR, OT_ERR_EXIST)
#define OT_ERR_MCF_UNEXIST OT_DEFINE_ERR(OT_ID_MCF, OT_ERR_LEVEL_ERROR, OT_ERR_UNEXIST)
#define OT_ERR_MCF_NOT_SUPPORT OT_DEFINE_ERR(OT_ID_MCF, OT_ERR_LEVEL_ERROR, OT_ERR_NOT_SUPPORT)
#define OT_ERR_MCF_NOT_PERM OT_DEFINE_ERR(OT_ID_MCF, OT_ERR_LEVEL_ERROR, OT_ERR_NOT_PERM)
#define OT_ERR_MCF_NO_MEM OT_DEFINE_ERR(OT_ID_MCF, OT_ERR_LEVEL_ERROR, OT_ERR_NO_MEM)
#define OT_ERR_MCF_NO_BUF OT_DEFINE_ERR(OT_ID_MCF, OT_ERR_LEVEL_ERROR, OT_ERR_NO_BUF)
#define OT_ERR_MCF_ILLEGAL_PARAM OT_DEFINE_ERR(OT_ID_MCF, OT_ERR_LEVEL_ERROR, OT_ERR_ILLEGAL_PARAM)
#define OT_ERR_MCF_BUSY OT_DEFINE_ERR(OT_ID_MCF, OT_ERR_LEVEL_ERROR, OT_ERR_BUSY)
#define OT_ERR_MCF_BUF_EMPTY OT_DEFINE_ERR(OT_ID_MCF, OT_ERR_LEVEL_ERROR, OT_ERR_BUF_EMPTY)
#define OT_ERR_MCF_NOT_START OT_DEFINE_ERR(OT_ID_MCF, OT_ERR_LEVEL_ERROR, OT_ERR_NOT_START)
#define OT_ERR_MCF_NOT_STOP OT_DEFINE_ERR(OT_ID_MCF, OT_ERR_LEVEL_ERROR, OT_ERR_NOT_STOP)
#define OT_ERR_MCF_NOT_ENCHN OT_DEFINE_ERR(OT_ID_MCF, OT_ERR_LEVEL_ERROR, OT_ERR_NOT_ENCHN)


#define OT_MCF_BIAS_LUT_NUM 9
#define OT_MCF_WEIGHT_LUT_NUM 33
#define OT_MCF_CC_UV_GAIN_LUT_NUM 256
#define OT_MCF_COEF_NUM 9

typedef struct {
    td_bool enable;
    ot_vi_pipe mono_pipe;
    ot_vi_pipe color_pipe;
} ot_mcf_vi_attr;

typedef struct {
    td_bool enable;     /* RW Range: [0, 1] Crop enable. */
    ot_coord crop_mode; /* RW Coordinate mode of crop. */
    ot_rect crop_rect;  /* RW Crop rectangular. */
} ot_mcf_crop_info;

typedef struct {
    ot_mcf_pipe pipe_id; /* MCF Pipe Id, Range: [0:1] */
    ot_vpss_grp vpss_grp;
    td_u32 width;  /* input image width, Range: [256, 8192] */
    td_u32 height; /* input image height, Range: [256, 4096] */
} ot_mcf_pipe_attr;

typedef enum {
    OT_MCF_PATH_FUSION = 0,
    OT_MCF_PATH_COLOR,
    OT_MCF_PATH_MONO,
    OT_MCF_PATH_BUTT,
} ot_mcf_path;

typedef struct {
    td_bool sync_pipe_en; /* whether sync pipe image. */
    ot_mcf_pipe_attr mono_pipe_attr;
    ot_mcf_pipe_attr color_pipe_attr;
    ot_frame_rate_ctrl frame_rate; /* RW; Group frame rate control. */
    td_u32 depth;                  /* RW; range: [0, 8] grp user list depth. */
    ot_mcf_path mcf_path; /* RW; output fusion, color, or mono */
} ot_mcf_grp_attr;

typedef struct {
    td_bool mirror_en;              /* RW; Range: [0, 1] Mirror enable. */
    td_bool flip_en;                /* RW; Range: [0, 1] Flip enable. */
    td_u32 width;                   /* RW; range: [128, 8192] width of target image. */
    td_u32 height;                  /* RW; range: [64, 8192] height of target image. */
    td_u32 depth;                   /* RW; range: [0, 8] chn user list depth. */
    ot_pixel_format pixel_format;   /* RW; Pixel format of target image. */
    ot_video_format video_format;
    ot_compress_mode compress_mode; /* RW; Compression mode of the output. */
    ot_frame_rate_ctrl frame_rate;  /* RW; Frame rate control info. */
} ot_mcf_chn_attr;

typedef struct {
    ot_mcf_chn bind_chn;
    td_u32 width;                   /* RW; range: [64, 8192] width of target image. */
    td_u32 height;                  /* RW; range: [64, 8192] height of target image. */
    ot_pixel_format pixel_format;
    ot_video_format video_format;
    ot_compress_mode compress_mode;
    td_u32 depth;
    ot_frame_rate_ctrl frame_rate;
} ot_mcf_ext_chn_attr;

typedef enum {
    OT_MCF_FUSION_GLOBAL_MODE = 0,
    OT_MCF_FUSION_ADAPTIVE_MODE,
    OT_MCF_FUSION_ALPHA_MODE_BUTT,
} ot_mcf_fusion_alpha_mode;

typedef enum {
    OT_MCF_NO_HIST_ADJ = 0,
    OT_MCF_COLOR_HIST_ADJ,
    OT_MCF_MONO_HIST_ADJ,
    OT_MCF_HIST_ADJ_MODE_BUTT,
} ot_mcf_hist_adj_mode;

typedef struct {
    td_bool color_correct_en;                         /* range: [0, 1] enable signal for color correction */
    td_u8 cc_thd_y;                                   /* range:[0,127] cc_bias_slope = (cc_max_y_bias * 4) / cc_thd_y */
    td_u16 cc_uv_gain_lut[OT_MCF_CC_UV_GAIN_LUT_NUM]; /* range: [0, 511] color correction lut */
} ot_mcf_color_correct_cfg;

typedef struct {
    td_u8 fusion_global_alpha; /* range: [0, 255] global alpha for color base */
} ot_mcf_fusion_global_alpha_mode_cfg;

typedef struct {
    /* calc ratio */
    td_u8 fusion_ratio_scale;                             /* range: [0, 255] control the range of ratio; */
    td_u8 fusion_ratio_bias_lut[OT_MCF_BIAS_LUT_NUM];     /* range: [1, 127] regularization parameter for */
                                                          /* calculating ratio between mono base and color base */
    td_u8 fusion_mono_flat_bias_lut[OT_MCF_BIAS_LUT_NUM]; /* range: [1, 255] */
                                                          /* bias for adjusting ratio based on mono flat area */
    /* adjust ratio  */
    td_bool fusion_mono_ratio_en; /* range: [0, 1] enable signal for adjusting ratio based on mono luma */
    td_u8 fusion_mono_ratio_lut[OT_MCF_WEIGHT_LUT_NUM]; /* range: [0, 255] */
                                                        /* adaptive gain for adjusting ratio based on mono luma */
    td_bool fusion_mono_flat_en; /* range: [0, 1] enable signal for adjusting ratio based on mono flat area */
    td_u8 fusion_mono_flat_lut[OT_MCF_WEIGHT_LUT_NUM]; /* range: [0, 255] */
                                                       /*  gain for adjusting ratio based on mono flat area */
    td_bool fusion_color_ratio_en; /* range: [0, 1] enable signal for adjusting ratio based on color luma */
    td_u8 fusion_color_ratio_lut[OT_MCF_WEIGHT_LUT_NUM]; /* range: [0, 255] */
                                                         /* adaptive gain for adjusting ratio based on color luma */
    /* calc alpha */
    td_u8 fusion_alpha_lut[OT_MCF_WEIGHT_LUT_NUM]; /* range: [0, 255] adaptive alpha for blending color base */
                                                   /* and mono base based on ratio */
} ot_mcf_fusion_adaptive_alpha_mode_cfg;

typedef struct {
    /* fusion config */
    td_u8 mono_flt_radius;                        /* range: [1, 2] filter radius for mono image */
    td_u8 mono_flt_bias_lut[OT_MCF_BIAS_LUT_NUM]; /* range: [1, 128] mono detail strength based on */
                                                  /* luma value of mono image */
    td_u8 color_flt_sgms;                         /* range: [1, 50] spatial range for generating color filter, */
                                                  /* the actual value is color_flt_sgms/10.0 */
    td_u8 color_flt_sgmr;                         /* range: [1, 255] value range for generating color filter */
} ot_mcf_filter_proc_cfg;

typedef struct {
    td_bool fusion_mono_det_adap_en;                  /* range: [0, 1] enable adaptive mono detail control;  */
    td_u8 fusion_mono_det_lut[OT_MCF_WEIGHT_LUT_NUM]; /* range: [0, 255] adaptive gain for mono detail */
    td_u8 fusion_det_gain; /* range: [0, 255] gain for mono detail that be added to fusion result */
} ot_mcf_detail_proc_cfg;

typedef struct {
    ot_mcf_fusion_alpha_mode fusion_alpha_mode;
    ot_mcf_fusion_global_alpha_mode_cfg fusion_global_alpha_mode_cfg;
    ot_mcf_fusion_adaptive_alpha_mode_cfg fusion_adaptive_alpha_mode_cfg;
} ot_mcf_base_proc_cfg;

/* common parameters */
typedef struct {
    ot_mcf_filter_proc_cfg filter_proc_cfg;
    ot_mcf_detail_proc_cfg detail_proc_cfg;
    ot_mcf_base_proc_cfg base_prco_cfg;
} ot_mcf_each_freq_proc_cfg;

typedef struct {
    /* color proc */
    ot_mcf_color_correct_cfg mcf_color_correct_cfg;

    /* each freq proc */
    ot_mcf_each_freq_proc_cfg mcf_hf_cfg; /* mcf parameters for high-freq of image */
    ot_mcf_each_freq_proc_cfg mcf_mf_cfg; /* mcf parameters for medium-freq of image */
    ot_mcf_each_freq_proc_cfg mcf_lf_cfg; /* mcf parameters for low-freq of image */
} ot_mcf_alg_param;

#endif
