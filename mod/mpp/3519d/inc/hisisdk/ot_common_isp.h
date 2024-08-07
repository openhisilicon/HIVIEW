/*
  Copyright (c), 2001-2024, Shenshu Tech. Co., Ltd.
 */

#ifndef OT_COMMON_ISP_H
#define OT_COMMON_ISP_H

#include "ot_type.h"
#include "ot_errno.h"
#include "ot_common.h"
#include "ot_isp_debug.h"
#include "ot_common_video.h"
#include "ot_isp_define.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* End of #ifdef __cplusplus */

/* MACRO DEFINITION */
#define ot_isp_res_width_max(dev)       (isp_get_max_width(dev))
#define ot_isp_res_height_max(dev)      (isp_get_max_height(dev))
#define OT_ISP_WIDTH_MIN                120
#define OT_ISP_HEIGHT_MIN               88
#define OT_ISP_ALIGN_WIDTH              4
#define OT_ISP_ALIGN_HEIGHT             4
#define OT_ISP_FRAME_RATE_MAX           65535.0

#define OT_ISP_BE_BUF_NUM_MIN           2

#define OT_ISP_VREG_MAX_NUM             16

#define OT_ISP_AE_ZONE_ROW              15
#define OT_ISP_AE_ZONE_COLUMN           17
#define OT_ISP_AE_PLANE_NUM             4
#define OT_ISP_MG_ZONE_ROW              15
#define OT_ISP_MG_ZONE_COLUMN           17
#define OT_ISP_AWB_ZONE_NUM             (OT_ISP_AWB_ZONE_ORIG_ROW * OT_ISP_AWB_ZONE_ORIG_COLUMN)
#define OT_ISP_AWB_ZONE_STITCH_MAX      (OT_ISP_AWB_ZONE_NUM * OT_ISP_MAX_STITCH_NUM)
#define OT_ISP_AF_ZONE_ROW              15
#define OT_ISP_AF_ZONE_COLUMN           17
#define OT_ISP_MAX_AWB_LIB_NUM          (OT_ISP_AWB_LIB_NUM)
#define OT_ISP_MAX_AE_LIB_NUM           (OT_ISP_AE_LIB_NUM)

#define OT_ISP_AE_MIN_WIDTH             256
#define OT_ISP_AE_MIN_HEIGHT            120

#define OT_ISP_AWB_MIN_WIDTH            20
#define OT_ISP_AWB_MIN_HEIGHT           15
#define OT_ISP_AWB_ZONE_MAX_WIDTH       1024
#define OT_ISP_AWB_ZONE_MAX_HEIGHT      1024

#define OT_ISP_AF_MIN_WIDTH             256
#define OT_ISP_AF_MIN_HEIGHT            120
#define OT_ISP_AF_ALIGN_WIDTH           8
#define OT_ISP_AF_ALIGN_HEIGHT          2
#define OT_ISP_AF_PLGS_NUM              2

#define OT_ISP_GAMMA_NODE_NUM           1025
#define OT_ISP_PREGAMMA_NODE_NUM        257
#define OT_ISP_SCALE_MIN                204
#define OT_ISP_SCALE_MAX                5120

#define OT_ISP_AUTO_ISO_NUM             16
#define OT_ISP_SHARPEN_LUMA_NUM         32
#define OT_ISP_SHARPEN_GAIN_NUM         32

#define OT_ISP_MAX_SNS_REGS             32

#define OT_ISP_MLSC_X_HALF_GRID_NUM       ((OT_ISP_LSC_GRID_COL - 1) / 2)
#define OT_ISP_MLSC_Y_HALF_GRID_NUM       ((OT_ISP_LSC_GRID_ROW - 1) / 2)
#define OT_ISP_MLSC_GAIN_LUT_NUM          2
#define OT_ISP_ACS_LIGHT_NUM              32
#define OT_ISP_ACS_CHN_NUM                2

#define OT_ISP_WDR_MAX_FRAME_NUM        4

#define OT_ISP_CSC_DC_NUM               3
#define OT_ISP_CSC_COEF_NUM             9

#define OT_ISP_BAYER_CALIBTAION_MAX_NUM 50
#define OT_ISP_AI_MAX_STEP_FNO_NUM      1024
#define OT_ISP_BAYER_GE_LUT_NUM         129

#define OT_ISP_BAYERNR_LUT_LENGTH       33
#define OT_ISP_BAYERNR_LUT_LENGTH1      32
#define OT_ISP_BAYERNR_DENOISE_Y_LUT_NUM      16
#define OT_ISP_PRO_MAX_FRAME_NUM        8
#define OT_ISP_RGB_CHN_NUM              3

#define OT_ISP_DEMOSAIC_LUT_LENGTH      17
#define OT_ISP_DEMOSAIC_AIDM_LUT_NUM    128
#define OT_ISP_BAYER_CHN_NUM            4
#define OT_ISP_CHN_R                    0
#define OT_ISP_CHN_GR                   1
#define OT_ISP_CHN_GB                   2
#define OT_ISP_CHN_B                    3

#define OT_ISP_DRC_LMIX_NODE_NUM        33
#define OT_ISP_DRC_EXP_COMP_SAMPLE_NUM  8
#define OT_ISP_DRC_SHP_LOG_CONFIG_NUM   16

#define OT_ISP_DEHAZE_LUT_SIZE          256
#define OT_ISP_WDR_RATIO_NUM            10

#define OT_ISP_LCAC_EXP_RATIO_NUM       16
#define OT_ISP_LCAC_DET_NUM             3

#define OT_ISP_CAC_EXP_RATIO_NUM        16
#define OT_ISP_CAC_THR_NUM              2
#define OT_ISP_CAC_CURVE_NUM            3

#define OT_ISP_BSHP_LUMA_WGT_NUM        32
#define OT_ISP_BSHP_HALF_CURVE_NUM      32
#define OT_ISP_BSHP_CURVE_NUM           8
#define OT_ISP_BSHP_THD_NUM             2

#define OT_ISP_DO_NOT_NEED_SWITCH_IMAGEMODE (-2)

/*  DEFAULT VALUE OF GLOBAL REGISTERS DEFINED HERE */
/*  AE  */
#define OT_ISP_TOP_RGGB_START_R_GR_GB_B           0
#define OT_ISP_TOP_RGGB_START_GR_R_B_GB           1
#define OT_ISP_TOP_RGGB_START_GB_B_R_GR           2
#define OT_ISP_TOP_RGGB_START_B_GB_GR_R           3

/*  AWB */
#define OT_ISP_AWB_OFFSET_COMP_DEF                0
#define OT_ISP_CCM_COLORTONE_EN_DEFAULT           1
#define OT_ISP_CCM_COLORTONE_RGAIN_DEFAULT        256
#define OT_ISP_CCM_COLORTONE_BGAIN_DEFAULT        256
#define OT_ISP_CCM_COLORTONE_GGAIN_DEFAULT        256

#define OT_ISP_CCM_PROT_EN_DEFAULT                0
#define OT_ISP_CCM_RECOVER_EN_DEFAULT             1
#define OT_ISP_CCM_MATRIX_SIZE                    9
#define OT_ISP_CCM_MATRIX_NUM                     7
#ifdef CONFIG_SD3403V100
#define OT_ISP_DRC_RG_BG_CTR_NODE_NUM   12
#endif
/* GENERAL STRUCTURES */
/*
 * ISP Error Code
 * 0x40 : ISP_NOT_INIT
 * 0x41 : ISP_MEM_NOT_INIT
 * 0x42 : ISP_ATTR_NOT_CFG
 * 0x43 : ISP_SNS_UNREGISTER
 * 0x44 : ISP_INVALID_ADDR
 * 0x45 : ISP_NOMEM
 * 0x46 : ISP_NO_INT
 */
typedef enum {
    OT_ERR_ISP_NOT_INIT                = 0x40, /* ISP not init */
    OT_ERR_ISP_MEM_NOT_INIT            = 0x41, /* ISP memory not init */
    OT_ERR_ISP_ATTR_NOT_CFG            = 0x42, /* ISP attribute not cfg */
    OT_ERR_ISP_SNS_UNREGISTER          = 0x43, /* ISP sensor unregister */
    OT_ERR_ISP_INVALID_ADDR            = 0x44, /* ISP invalid address */
    OT_ERR_ISP_NOMEM                   = 0x45, /* ISP nomem */
    OT_ERR_ISP_NO_INT                  = 0x46, /* ISP */
    OT_ERR_ISP_ALG_NOT_INIT            = 0x47, /* ISP alg not init */
} ot_isp_err_code;

#define OT_ERR_ISP_NULL_PTR         OT_DEFINE_ERR(OT_ID_ISP, OT_ERR_LEVEL_ERROR, OT_ERR_NULL_PTR)
#define OT_ERR_ISP_ILLEGAL_PARAM    OT_DEFINE_ERR(OT_ID_ISP, OT_ERR_LEVEL_ERROR, OT_ERR_ILLEGAL_PARAM)
#define OT_ERR_ISP_NOT_SUPPORT      OT_DEFINE_ERR(OT_ID_ISP, OT_ERR_LEVEL_ERROR, OT_ERR_NOT_SUPPORT)

#define OT_ERR_ISP_NOT_INIT         OT_DEFINE_ERR(OT_ID_ISP, OT_ERR_LEVEL_ERROR, OT_ERR_ISP_NOT_INIT)
#define OT_ERR_ISP_MEM_NOT_INIT     OT_DEFINE_ERR(OT_ID_ISP, OT_ERR_LEVEL_ERROR, OT_ERR_ISP_MEM_NOT_INIT)
#define OT_ERR_ISP_ATTR_NOT_CFG     OT_DEFINE_ERR(OT_ID_ISP, OT_ERR_LEVEL_ERROR, OT_ERR_ISP_ATTR_NOT_CFG)
#define OT_ERR_ISP_SNS_UNREGISTER   OT_DEFINE_ERR(OT_ID_ISP, OT_ERR_LEVEL_ERROR, OT_ERR_ISP_SNS_UNREGISTER)
#define OT_ERR_ISP_INVALID_ADDR     OT_DEFINE_ERR(OT_ID_ISP, OT_ERR_LEVEL_ERROR, OT_ERR_ISP_INVALID_ADDR)
#define OT_ERR_ISP_NOMEM            OT_DEFINE_ERR(OT_ID_ISP, OT_ERR_LEVEL_ERROR, OT_ERR_ISP_NOMEM)
#define OT_ERR_ISP_NO_INT           OT_DEFINE_ERR(OT_ID_ISP, OT_ERR_LEVEL_ERROR, OT_ERR_ISP_NO_INT)
#define OT_ERR_ISP_ALG_NOT_INIT     OT_DEFINE_ERR(OT_ID_ISP, OT_ERR_LEVEL_ERROR, OT_ERR_ISP_ALG_NOT_INIT)
/* Defines the structure of ISP module parameters. */
typedef struct {
    td_u32 interrupt_bottom_half; /* RW;Range:[0, 1]; Format:32.0; Indicate ISP interrupt bottom half,
                                     No distinction vipipe */
} ot_isp_mod_param;

/* Defines the structure of ISP control parameters. */

typedef enum {
    OT_ISP_UPDATE_OB_STATS_FE_FRAME_END   = 0, /* delay 1 frame */
    OT_ISP_UPDATE_OB_STATS_FE_FRAME_START = 1, /* delay 2 frame */
    OT_ISP_UPDATE_OB_STATS_BUTT,
} ot_isp_ob_stats_update_pos;

typedef enum {
    OT_ISP_ALG_RUN_NORM     = 0,
    OT_ISP_ALG_RUN_FE_ONLY  = 1,
    OT_ISP_ALG_RUN_BUTT,
} ot_isp_alg_run_select;

typedef enum {
    OT_ISP_RUN_WAKEUP_FE_START = 0,
    OT_ISP_RUN_WAKEUP_BE_END = 1,
    OT_ISP_RUN_WAKEUP_BUTT,
} ot_isp_run_wakeup_select;

typedef struct {
    td_u8   be_buf_num;      /* RW;Range:[2, 20]; offline be cfg buffer number(Only used for offline mode).
                                Not support changed after isp init */
    td_u32  proc_param;      /* RW;Format:32.0; Indicate the update frequency of ISP_PROC information,
                                No distinction vipipe */
    td_u32  stat_interval;   /* RW;Range:(0, 0xffffffff];Format:32.0;
                                Indicate the time interval of ISP statistic information */
    td_u32  update_pos;      /* RW;Range:[0, 1]; Format:32.0; Indicate the location of the configuration register of
                                ISP interrupt */
    td_u32  interrupt_time_out; /* RW;Format:32.0; Indicate the time(unit:ms) of interrupt timeout */
    td_u32  pwm_num;      /* RW;Format:32.0; Indicate PWM number */
    td_u32  port_interrupt_delay;  /* RW;Format:32.0; Port intertupt delay value, unit:clk */
    td_bool ldci_tpr_flt_en; /* RW;Range:[0, 1]; Indicate Enable LDCI temporal filter. Not support changed after
                                isp init */
    ot_isp_ob_stats_update_pos ob_stats_update_pos; /* RW;Range:[0, 1];  Indicate the location of reading ob orea
                                                       statistics. Not support changed after isp init */
    ot_isp_alg_run_select alg_run_select; /* RW;Range:[0, 1];  Indicate the algorithm of isp register and run.
                                             Not support changed after isp init */
    ot_isp_run_wakeup_select isp_run_wakeup_select; /* RW;Range:[0, 1];  Indicate the isp software is drived by which
                                                       interrupt, FE_START or BE_END */
    td_bool quick_start_en;           /* RW;Range:[0, 1]; Indicate ISP Quick Start. */
    td_bool long_frame_interrupt_en;  /* RW;Range:[0, 1]; when wdr mode enable/disable long frame pipe interrupt. */
} ot_isp_ctrl_param;

/*
 * Defines the prior frame of ISP
 * 0 = long frame
 * 1 = short frame
 */
typedef enum {
    OT_ISP_LONG_FRAME      = 0,
    OT_ISP_SHORT_FRAME     = 1,
    OT_ISP_PRIOR_FRAME_BUTT
} ot_isp_prior_frame;

/*
 * Defines the ISP correction or detection status
 * 0 = initial status, no calibration
 * 1 = The static defect pixel calibration ends normally
 * 2 = The static defect pixel calibration ends due to timeout.
 */
typedef enum {
    OT_ISP_STATE_INIT     = 0,
    OT_ISP_STATE_SUCCESS  = 1,
    OT_ISP_STATE_TIMEOUT  = 2,
    OT_ISP_STATE_BUTT
} ot_isp_status;

typedef struct {
    ot_op_mode   op_type;
    td_u8 overlap;    /* RW; Range:[1,16] */
    td_s32 offset[OT_ISP_BAYER_CHN_NUM];   /* RW;Range:[-0x3FFF, 0x3FFF];Format:15.0;
                                              Black level differences between the offset value */
    td_u32 gain[OT_ISP_BAYER_CHN_NUM];     /* RW;Range:[0x80, 0x400];Format:4.8; The gain difference ratio */
    td_u16 color_matrix[OT_ISP_CCM_MATRIX_SIZE]; /* RW;Range:[0x0, 0xFFFF];Format:8.8; Color correction matrix */
} ot_isp_pipe_diff_attr;

/* Defines the format of the input Bayer image */
typedef enum {
    OT_ISP_BAYER_RGGB    = 0,
    OT_ISP_BAYER_GRBG    = 1,
    OT_ISP_BAYER_GBRG    = 2,
    OT_ISP_BAYER_BGGR    = 3,
    OT_ISP_BAYER_BUTT
} ot_isp_bayer_format;

/* Defines the bitwidth of the input Bayer image, used for lsc online calibration */
typedef enum {
    OT_ISP_BAYER_RAW_BIT_8BIT    = 8,
    OT_ISP_BAYER_RAW_BIT_10BIT   = 10,
    OT_ISP_BAYER_RAW_BIT_12BIT   = 12,
    OT_ISP_BAYER_RAW_BIT_14BIT   = 14,
    OT_ISP_BAYER_RAW_BIT_16BIT   = 16,
    OT_ISP_BAYER_RAW_BIT_BUTT
} ot_isp_bayer_raw_bit;

/* MIPI attribute, contains the mipi crop attribute */
typedef struct {
    td_bool           mipi_crop_en;      /* RW;Range:[0, 0x1];Format:1.0; enable/disable mipi crop */
    ot_rect           mipi_crop_offset;  /* RW; start position of the mipi crop window, image width and height */
} ot_mipi_crop_attr;

/* ISP public attribute, contains the public image attribute */
typedef struct {
    ot_rect             wnd_rect;        /* RW; start position of the cropping window, image width, and image height */
    ot_size             sns_size;          /* RW; width and height of the image output from the sensor */
    td_float            frame_rate;        /* RW; Range: [0, 0xFFFF]; for frame rate */
    ot_isp_bayer_format bayer_format;      /* RW; Range:[0,3] ;Format:2.0;the format of the input bayer image */
    ot_wdr_mode         wdr_mode;          /* RW; WDR mode select */
    td_u8               sns_mode;          /* RW; Range: [0, 0xFF]; Format: 8.0; for special sensor mode switches */
    td_bool             sns_flip_en;    /* RW;Range:[0, 0x1];Format:1.0; enable/disable sensor flip */
    td_bool             sns_mirror_en;  /* RW;Range:[0, 0x1];Format:1.0; enable/disable sensor mirror */
    ot_mipi_crop_attr   mipi_crop_attr;    /* RW; mipi crop window attr, enable/disable mipi crop */
} ot_isp_pub_attr;

/* Slave mode sensor sync signal generate module */
typedef struct {
    union {
        struct {
            td_u32  bit16_reserved      :  16;
            td_u32  bit_h_inv           :  1;
            td_u32  bit_v_inv           :  1;
            td_u32  bit12_reserved      :  12;
            td_u32  bit_h_enable        :  1;
            td_u32  bit_v_enable        :  1;
        } bits;
        td_u32 bytes;
    } cfg;

    td_u32  vs_time;
    td_u32  hs_time;
    td_u32  vs_cyc;
    td_u32  hs_cyc;
    td_u32  hs_dly_cyc;
    td_u32  slave_mode_time;
} ot_isp_slave_sns_sync;

/* Defines the ISP stitch attribute. */
/*
 * Defines the ISP firmware status
 * 0 = Running status
 * 1 = Frozen status
 */
typedef enum {
    OT_ISP_FMW_STATE_RUN = 0,
    OT_ISP_FMW_STATE_FREEZE,
    OT_ISP_FMW_STATE_BUTT
} ot_isp_fmw_state;

/* Defines the WDR mode of the ISP */
typedef struct {
    ot_wdr_mode  wdr_mode;
} ot_isp_wdr_mode;

typedef union {
    td_u64  key;
    struct {
        td_u64  bit_bypass_isp_d_gain        : 1;   /* RW;[0] */
        td_u64  bit_bypass_anti_false_color  : 1;   /* RW;[1] */
        td_u64  bit_bypass_crosstalk_removal : 1;   /* RW;[2] */
        td_u64  bit_bypass_dpc            : 1;   /* RW;[3] */
        td_u64  bit_bypass_nr             : 1;   /* RW;[4] */
        td_u64  bit_bypass_dehaze         : 1;   /* RW;[5] */
        td_u64  bit_bypass_wb_gain        : 1;   /* RW;[6] */
        td_u64  bit_bypass_mesh_shading   : 1;   /* RW;[7] */
        td_u64  bit_bypass_drc            : 1;   /* RW;[8] */
        td_u64  bit_bypass_demosaic       : 1;   /* RW;[9] */
        td_u64  bit_bypass_color_matrix   : 1;   /* RW;[10] */
        td_u64  bit_bypass_gamma          : 1;   /* RW;[11] */
        td_u64  bit_bypass_fswdr          : 1;   /* RW;[12] */
        td_u64  bit_bypass_ca             : 1;   /* RW;[13] */
        td_u64  bit_bypass_csc            : 1;   /* RW;[14] */
        td_u64  bit_bypass_radial_crop    : 1;   /* RW;[15] */
        td_u64  bit_bypass_sharpen        : 1;   /* RW;[16] */
        td_u64  bit_bypass_bayer_sharpen  : 1;   /* RW;[17] */
        td_u64  bit_bypass_cac            : 1;   /* RW;[18]; */
        td_u64  bit2_chn_select           : 2;   /* RW;[19:20] */
        td_u64  bit_bypass_ldci           : 1;   /* RW;[21] */
        td_u64  bit_bypass_pregamma       : 1;   /* RW;[22], not support for Hi3519DV500 */
        td_u64  bit_bypass_ae_stat_fe     : 1;   /* RW;[23] */
        td_u64  bit_bypass_ae_stat_be     : 1;   /* RW;[24] */
        td_u64  bit_bypass_mg_stat        : 1;   /* RW;[25] */
        td_u64  bit_bypass_af_stat_fe     : 1;   /* RW;[26] */
        td_u64  bit_bypass_af_stat_be     : 1;   /* RW;[27] */
        td_u64  bit_bypass_awb_stat       : 1;   /* RW;[28] */
        td_u64  bit_bypass_clut           : 1;   /* RW;[29] */
        td_u64  bit_bypass_rgbir          : 1;   /* RW;[30], not support for Hi3519DV500 */
        td_u64  bit_bypass_lblc           : 1;   /* RW;[31] */
        td_u64  bit_reserved32            : 32;  /* H; [32:63] */
    };
} ot_isp_module_ctrl;

/*
 * Defines the RAW stream Position from the ISP
 * 0 = the data is writing out after all isp module.
 * 1 = the raw data is writing out after the WDR  module.
 */
typedef enum {
    OT_ISP_DUMP_FRAME_POS_NORMAL     = 0,
    OT_ISP_DUMP_FRAME_POS_AFTER_WDR  = 1,
    OT_ISP_DUMP_FRAME_POS_BUTT
} ot_isp_dump_frame_pos;

/*
 * Defines the dump frame Position from the ISP.
 */
typedef struct {
    ot_isp_dump_frame_pos frame_pos;
} ot_isp_be_frame_attr;

/*
 * Defines the ISP WDR merge mode.
 * 0 = WDR mode is used
 * 1 = Fusion mode is used
 */
typedef enum {
    OT_ISP_MERGE_WDR_MODE      = 0,
    OT_ISP_MERGE_FUSION_MODE   = 1,
    OT_ISP_MERGE_BUTT
} ot_isp_wdr_merge_mode;

typedef enum {
    OT_ISP_BNR_EX = 0,
    OT_ISP_BNR_NORM = 1,
} ot_isp_bnr_sfm0_mode;

typedef enum {
    OT_ISP_WDR_WBGAIN_IN_DG1         = 0,
    OT_ISP_WDR_WBGAIN_IN_WB          = 1,
    OT_ISP_WDR_WBGAIN_POS_BUTT
} ot_isp_wdr_wb_gain_pos;

typedef struct {
    td_u8 md_thr_low_gain;    /* RW;Range:[0x0,0xFF];Format:8.0 */
    td_u8 md_thr_hig_gain;    /* RW;Range:[0x0,0xFF];Format:8.0 */
} ot_isp_fswdr_manual_attr;

typedef struct {
    td_u8 md_thr_low_gain[OT_ISP_WDR_RATIO_NUM][OT_ISP_AUTO_ISO_NUM];    /* RW;Range:[0x0,0xFF];Format:8.0 */
    td_u8 md_thr_hig_gain[OT_ISP_WDR_RATIO_NUM][OT_ISP_AUTO_ISO_NUM];    /* RW;Range:[0x0,0xFF];Format:8.0 */
} ot_isp_fswdr_auto_attr;

typedef struct {
    td_bool short_expo_chk;     /* RW;Range:[0x0,0x1];Format:1.0; */
    td_u16  short_check_threshold;   /* RW;Range:[0x0,0xFFF];Format:12.0 */
    td_bool md_ref_flicker;
    td_u8   mdt_still_threshold;     /* RW;Range:[0x0,0xFE];Format:8.0 */
    td_u8   mdt_full_threshold;      /* RW;Range:[0x0,0xFE];Format:8.0 */
    td_u8   mdt_long_blend;    /* RW;Range:[0x0,0xFE] */
    ot_op_mode op_type;
    ot_isp_fswdr_manual_attr manual_attr;
    ot_isp_fswdr_auto_attr   auto_attr;
} ot_isp_fswdr_mdt_attr;

typedef struct {
    td_bool motion_comp;        /* RW;Range:[0, 0x1];Format:1.0; enable/disable motion compensation */
    td_u16  short_threshold;    /* RW;Range:[0, 0xFFF];Format:12.0; data above this threshold will be taken from
                                   short exposure only. */
    td_u16  long_threshold;     /* RW;Range:[0, 0xFFF];Format:12.0; limited range:[0x0, short_threshold],
                                   data below this threshold will be taken from long exposure only. */
    td_bool force_long;         /* RW;Range:[0, 1];Format:1.0; enable/disable force long */
    td_u16  force_long_low_threshold; /* RW;Range:[0, 0xFFF];Format:12.0; data above this threshold will force to choose
                                         long frame only */
    td_u16  force_long_hig_threshold; /* RW;Range:[0, 0xFFF];Format:12.0; data below this threshold will force to choose
                                         long frame only */
    ot_isp_fswdr_mdt_attr wdr_mdt;
} ot_isp_wdr_combine_attr;

typedef struct {
    td_bool fusion_blend_en;
    td_u8   fusion_blend_wgt; /* RW; Range: [0, 16] */
    td_u16  fusion_threshold[OT_ISP_WDR_MAX_FRAME_NUM]; /* RW; Range: [0x0, 0xFFF] */
    td_bool  fusion_force_gray_en;
    td_u8   fusion_force_blend_threshold; /* RW; Range: [0, 15] */
} ot_isp_fusion_attr;

typedef struct {
    ot_isp_wdr_merge_mode   wdr_merge_mode;
    ot_isp_wdr_combine_attr wdr_combine;
    ot_isp_fusion_attr      fusion_attr;
} ot_isp_wdr_fs_attr;

typedef struct {
    td_u8 asymmetry;    /* RW; Range:[  1,  30]; Parameter0 of DRC asymmetry tone mapping curve */
    td_u8 second_pole;  /* RW; Range:[150, 210]; Parameter1 of DRC asymmetry tone mapping curve */
    td_u8 stretch;      /* RW; Range:[ 30,  60]; Parameter2 of DRC asymmetry tone mapping curve */
    td_u8 compress;     /* RW; Range:[100, 200]; Parameter3 of DRC asymmetry tone mapping curve */
} ot_isp_drc_asymmetry_curve_attr;

typedef struct {
    td_u16  strength;  /* RW; Range:[0x0, 0x3FF]; DRC manual strength */
} ot_isp_drc_manual_attr;

typedef struct {
    td_u16  strength;      /* RW; Range:[0x0, 0x3FF]; Base strength for strength adaptation in auto mode.
                              The actual DRC strength is calculated automatically by firmware:
                              1) linear mode, auto strength = f1(strength, histogram)
                              2) sensor built-in WDR mode: auto strength = f2(strength, histogram)
                              3) FS WDR mode: auto strength = f3(strength, exp_ratio) */
    td_u16  strength_max;  /* RW; Range:[0x0, 0x3FF]; uppper limit of firmware calculated strength */
    td_u16  strength_min;  /* RW; Range:[0x0, 0x3FF]; lower limit of firmware calculated strength */
} ot_isp_drc_auto_attr;

/* DRC curve type: 0 = Asymmetry curve, 1 = User-defined curve */
typedef enum {
    OT_ISP_DRC_CURVE_ASYMMETRY = 0x0,
    OT_ISP_DRC_CURVE_USER,
    OT_ISP_DRC_CURVE_BUTT
} ot_isp_drc_curve_select;

typedef struct {
    td_bool enable;
    ot_isp_drc_curve_select curve_select; /* RW; Range:[0x0, 0x1]; Tone mapping curve selection */
    td_u8  purple_reduction_strength; /* RW; Range:[0x0, 0x80]; Purple detection and reduction strength */
    td_u8  bright_gain_limit; /* RW; Range:[0x0, 0xF]; Bright area gain limit */
    td_u8  bright_gain_limit_step; /* RW; Range:[0x0, 0xF]; Bright area gain high limit step */
    td_u8  dark_gain_limit_luma; /* RW; Range:[0x0, 0x85]; Dark area luma gain limit */
    td_u8  dark_gain_limit_chroma; /* RW; Range:[0x0, 0x85]; Dark area chroma gain limit */
    td_u8  contrast_ctrl; /* RW; Range:[0x0, 0xF]; Format:4.0; Contrast control, smaller value yields better contrast */
    td_u8  rim_reduction_strength;  /* RW; Range:[0x0, 0x40]; Gradient reversal (rim) reduction strength */
    td_u8  rim_reduction_threshold; /* RW; Range:[0x0, 0x80]; Gradient reversal (rim) reduction threshold */
    td_u16 color_correction_lut[OT_ISP_DRC_CC_NODE_NUM]; /* RW; Range:[0x0, 0x400];
                                                           LUT of color correction coefficients */
    td_u16 tone_mapping_value[OT_ISP_DRC_TM_NODE_NUM]; /* RW; Range:[0x0, 0xffff]; LUT of user-defined curve */
    td_u8  spatial_filter_coef; /* RW; Range:[0x0, 0x5]; Spatial filter coefficients */
    td_u8  range_filter_coef; /* RW; Range:[0x0, 0xA]; Range filter coefficients */
    td_u8  detail_adjust_coef; /* RW; Range:[0x0, 0xF]; Detail adjust coefficient */
    td_u8  local_mixing_bright[OT_ISP_DRC_LMIX_NODE_NUM]; /* RW; Range:[0x0, 0x80];
                                                             LUT of enhancement coefficients for positive details */
    td_u8  local_mixing_dark[OT_ISP_DRC_LMIX_NODE_NUM]; /* RW; Range:[0x0, 0x80];
                                                           LUT of enhancement coefficients for negative details */
    td_u8  high_saturation_color_ctrl; /* RW; Range:[0x0, 0xF]; High saturation color control */
    td_u8  global_color_ctrl; /* RW; Range:[0x0, 0xF]; global color control */

    td_bool shoot_reduction_en; /* RW; Range:[0x0, 0x1]; shoot reduction enable */

    ot_op_mode op_type;
    ot_isp_drc_manual_attr manual_attr;
    ot_isp_drc_auto_attr   auto_attr;
    ot_isp_drc_asymmetry_curve_attr asymmetry_curve;
} ot_isp_drc_attr;

#define OT_ISP_EXPANDER_POINT_NUM_MAX (OT_ISP_EXPANDER_NODE_NUM - 1)

typedef struct {
    td_bool  enable;
    td_u8    bit_depth_in;        /* RW;Range:[0xC, 0x14];Format:5.0;The Bit depth of input.
                                     bit_depth_in must be an even number */
    td_u8    bit_depth_out;       /* RW;Range:[0xC, 0x14];Format:5.0;The Bit depth of output.
                                     bit_depth_out must be an even number */
    td_u16   knee_point_num;      /* Range:[1, 256]; Format:9.0; the number of knee point */
    ot_point knee_point_coord[OT_ISP_EXPANDER_POINT_NUM_MAX]; /* RW; Range: x:[0x0, 0x100], y:[0x0, 0x100000];
                                                                 The coordinate of knee point.
                                                                 The coordinate must be monotonically increasing */
} ot_isp_expander_attr;

typedef struct {
    td_u8 wgt;   /* RW;Range: [0x0, 0xFF];
                    Format:1.7;Weight of Gaussian distribution */
    td_u8 sigma; /* RW;Range: [0x1, 0xFF];Format:0.8;Sigma of Gaussian distribution */
    td_u8 mean;  /* RW;Range: [0x0, 0xFF];Format:0.8;Mean of Gaussian distribution */
} ot_isp_ldci_gauss_coef_attr;

typedef struct {
    ot_isp_ldci_gauss_coef_attr  he_pos_wgt;
    ot_isp_ldci_gauss_coef_attr  he_neg_wgt;
} ot_isp_ldci_he_wgt_attr;

typedef struct {
    ot_isp_ldci_he_wgt_attr  he_wgt;
    td_u16                   blc_ctrl;  /* RW;Range: [0x0, 0x1FF];Format:9.0;Restrain dark region */
} ot_isp_ldci_manual_attr;

typedef struct {
    ot_isp_ldci_he_wgt_attr  he_wgt[OT_ISP_AUTO_ISO_NUM];
    td_u16                   blc_ctrl[OT_ISP_AUTO_ISO_NUM]; /* RW;Range: [0x0, 0x1FF];Format:9.0;auto mode,
                                                               Restrain dark region */
} ot_isp_ldci_auto_attr;

typedef struct {
    td_bool                 enable;          /* RW;Range:[0x0,0x1];Format:1.0; */
    td_u8                   gauss_lpf_sigma; /* RW;Range: [0x1, 0xFF];Format:0.8;
                                                Coefficient of Gaussian low-pass filter */
    ot_op_mode              op_type;
    ot_isp_ldci_manual_attr manual_attr;
    ot_isp_ldci_auto_attr   auto_attr;
    td_u16                  tpr_incr_coef;   /* RW;Range: [0x0, 0x100];Format:0.9;
                                                Increase Coefficient of temporal filter */
    td_u16                  tpr_decr_coef;   /* RW;Range: [0x0, 0x100];Format:0.9;
                                                Decrease Coefficient of temporal filter */
} ot_isp_ldci_attr;

/*
  defines CA type
  0 = enable Ca module
  1 = enable Cp module
 */
typedef enum {
    OT_ISP_CA_ENABLE = 0x0,
    OT_ISP_CP_ENABLE,
    OT_ISP_CA_BUTT
} ot_isp_ca_type;

typedef struct {
    td_u32  y_ratio_lut[OT_ISP_CA_YRATIO_LUT_LENGTH];  /* RW;Range:[0,2047];Format:1.11 */
    td_s32  iso_ratio[OT_ISP_AUTO_ISO_NUM];            /* RW;Range:[0,2047];Format:1.10 */
    td_u32  y_sat_lut[OT_ISP_CA_YRATIO_LUT_LENGTH];  /* RW;Range:[0,2047];Format:1.11 */
} ot_isp_ca_lut;

typedef struct {
    td_u8   cp_lut_y[OT_ISP_CA_YRATIO_LUT_LENGTH]; /* RW;Range:[0,255];Format:8.0; */
    td_u8   cp_lut_u[OT_ISP_CA_YRATIO_LUT_LENGTH]; /* RW;Range:[0,255];Format:8.0; */
    td_u8   cp_lut_v[OT_ISP_CA_YRATIO_LUT_LENGTH]; /* RW;Range:[0,255];Format:8.0; */
} ot_isp_cp_lut;

typedef struct {
    td_bool enable;             /* RW;Range:[0x0,0x1];Format:1.0 */
    ot_isp_ca_type ca_cp_en;
    ot_isp_ca_lut  ca;
    ot_isp_cp_lut  cp;
} ot_isp_ca_attr;

/* CSC */
typedef struct {
    td_s16 csc_in_dc[OT_ISP_CSC_DC_NUM]; /* RW; Range:[-1024, 1023]; Format: 11.0;
                                            Input dc component for csc matrix */
    td_s16 csc_out_dc[OT_ISP_CSC_DC_NUM]; /* RW; Range:[-1024, 1023]; Format: 11.0;
                                             Output dc component for csc matrix */
    td_s16 csc_coef[OT_ISP_CSC_COEF_NUM];  /* RW; Range:[-4096, 4095]; Format: 5.10; 3x3 coefficients for csc matrix */
} ot_isp_csc_matrx;

typedef struct {
    td_bool enable;                 /* RW; Range:[0, 1];Format:1.0; Enable/Disable CSC Function */
    ot_color_gamut color_gamut; /* RW; Range: [0, 3]; Color gamut type; Not Support COLOR_GAMUT_BT2020 */
    td_u8   hue;              /* RW; Range:[0, 100];Format:8.0; Csc Hue Value */
    td_u8   luma;             /* RW; Range:[0, 100];Format:8.0; Csc Luma Value */
    td_u8   contr;            /* RW; Range:[0, 100];Format:8.0; Csc Contrast Value */
    td_u8   satu;             /* RW; Range:[0, 100];Format:8.0; Csc Saturation Value */
    td_bool limited_range_en; /* RW; Range: [0x0, 0x1];
                                 Enable/Disable: Enable Limited range output mode(default full range output) */
    td_bool ext_csc_en;       /* RW; Range: [0x0, 0x1]; Enable/Disable: Enable extended luma range */
    td_bool ct_mode_en;       /* RW; Range: [0x0, 0x1]; Enable/Disable: Enable ct mode */
    ot_isp_csc_matrx csc_magtrx;     /* RW; Color Space Conversion matrix */
} ot_isp_csc_attr;

/* CLUT */
typedef struct {
    td_bool enable; /* RW; Range:[0, 1];Format:1.0; Enable/Disable CLUT Function */
    td_u32  gain_r; /* RW; Range:[0, 4095];Format:12.0 */
    td_u32  gain_g; /* RW; Range:[0, 4095];Format:12.0 */
    td_u32  gain_b; /* RW; Range:[0, 4095];Format:12.0 */
} ot_isp_clut_attr;

/* CLUT LUT */
typedef struct {
    td_u32 lut[OT_ISP_CLUT_LUT_LENGTH];  /* RW; Range: [0, 1073741823] */
} ot_isp_clut_lut;

/*
 * Defines the type of static defect pixel calibration
 * 0 = bright defect pixel calibration
 * 1 = dark defect pixel calibration
 */
typedef enum {
    OT_ISP_STATIC_DP_BRIGHT = 0x0,
    OT_ISP_STATIC_DP_DARK,
    OT_ISP_STATIC_DP_BUTT
} ot_isp_static_dp_type;

typedef struct {
    td_bool enable_detect;                /* RW; Range: [0, 1];Format 1.0;Set 'TD_TRUE'to start static defect-pixel
                                             calibration, and firmware will set 'TD_FALSE' when finished */
    ot_isp_static_dp_type static_dp_type; /* RW; Range: [0, 1];Format 1.0;Select static bright/dark defect-pixel
                                             calibration */
    td_u8  start_thresh;  /* RW; Range: [1, 255]; Format 8.0;Start threshold for static defect-pixel calibraiton */
    td_u16 count_max;     /* RW; Range:Hi3519DV500 = (0, 6144];
                           Format 14.0; limited Range: (0, OT_ISP_STATIC_DP_COUNT_NORMAL*BlkNum],
                           Limit of max number of static defect-pixel calibraiton */
    td_u16 count_min;     /* RW; Range:Hi3519DV500 = [0, 6144);Format 14.0; limited Range: [0, count_max),
                             Limit of min number of static defect-pixel calibraiton */
    td_u16 time_limit;    /* RW; Range: [0x0, 1600];Format 11.0;Time limit for static defect-pixel calibraiton */

    td_u32 table[OT_ISP_STATIC_DP_COUNT_MAX];  /* R; Range: [0, 0x1FFF1FFF];Format 29.0;
                                           Static defect-pixel calibraiton table,
                                           0~12 bits represents the X coordinate of the defect pixel,
                                           16~28 bits represent the Y coordinate of the defect pixel */
    td_u8  finish_thresh;   /* R; Range: [0, 255];Format 8.0; Finish threshold for static defect-pixel calibraiton */
    td_u16 count;          /* R; Range:Hi3519DV500 = [0, 6144];Format 14.0;
                              Finish number for static defect-pixel calibraiton */
    ot_isp_status status;  /* R; Range: [0, 2];Format 2.0;Status of static defect-pixel calibraiton */
} ot_isp_dp_static_calibrate;

typedef struct {
    td_bool enable;       /* RW; Range: [0, 1];Format 1.0;Enable/disable the static defect-pixel module */
    td_u16  bright_count; /* RW; Range:Hi3519DV500 = [0, 6144];Format 14.0;
                             limited Range: [0, OT_ISP_STATIC_DP_COUNT_NORMAL*BlkNum],
                             When used as input(W), indicate the number of static bright defect pixels;
                             As output(R),indicate the number of static bright and dark defect pixels */
    td_u16  dark_count;   /* RW; Range:Hi3519DV500 = [0, 6144];Format 14.0;
                             limited Range: [0, OT_ISP_STATIC_DP_COUNT_NORMAL*BlkNum],
                             When used as input(W), indicate the number of static dark defect pixels;
                             As output(R), invalid value 0. */
    td_u32  bright_table[OT_ISP_STATIC_DP_COUNT_MAX]; /* RW; Range: [0x0, 0x1FFF1FFF];Format 29.0;
                                                  0~12 bits represents the X coordinate of the defect pixel,
                                                  16~28 bits represent the Y coordinate of the defect pixel
                                                  Notice: When used as input(W), indicate static bright defect pixels
                                                  table; As output(R), indicate static bright and dark defect pixels
                                                  table. */

    td_u32  dark_table[OT_ISP_STATIC_DP_COUNT_MAX]; /* RW; Range: [0x0, 0x1FFF1FFF];Format 29.0;
                                                0~12 bits represents the X coordinate of the defect pixel,
                                                16~28 bits represent the Y coordinate of the defect pixel
                                                Notice: When used as input(W), indicate static dark defect pixels table;
                                                As output(R), invalid value */
    td_bool show;                  /* RW; Range: [0, 1];Format 1.0;RW;highlight static defect pixel */
} ot_isp_dp_static_attr;

typedef struct {
    td_u8  strength;           /* RW; Range: [0, 255];Format:8.0;Dynamic DPC strength. */
    td_u8  blend_ratio;        /* RW; Range: [0, 128];Format:1.7;Blending ratio required for DPC */
} ot_isp_dp_dynamic_manual_attr;

typedef struct {
    td_u8  strength[OT_ISP_AUTO_ISO_NUM];    /* RW; Range: [0, 255];Format:8.0;Dynamic DPC strength. */
    td_u8  blend_ratio[OT_ISP_AUTO_ISO_NUM]; /* RW; Range: [0, 128];Format:1.7;Blending ratio required for DPC */
} ot_isp_dp_dynamic_auto_attr;

typedef struct {
    td_bool sup_twinkle_en;     /* RW; Range: [0, 1];Format 1.0;Enable/disable the twinkle suppression  module */
    td_s8   soft_thr;           /* RW; Range: [0, 127];Format s8.0;twinkle suppression threshold */
    td_u8   soft_slope;         /* RW; Range: [0, 255];Format 8.0;Correction controlling parameter of the pixels whose
                                   deviation is smaller than soft_thr. */
    ot_op_mode op_type;         /* RW; Range: [0, 1];Format 1.0;Working mode of dynamic DPC */
    ot_isp_dp_dynamic_manual_attr manual_attr;
    ot_isp_dp_dynamic_auto_attr   auto_attr;
    td_u8 bright_strength;      /* RW; Range: [0, 127];Format 8.0, local DPC strength for bright area */
    td_u8 dark_strength;        /* RW; Range: [0, 127];Format 8.0, local DPC strength for dark area */
} ot_isp_dp_frame_dynamic_attr;

typedef struct {
    td_bool enable;             /* RW; Range: [0, 1];Format 1.0;Enable/disable the dynamic defect-pixel module */
    ot_isp_dp_frame_dynamic_attr frame_dynamic[OT_ISP_WDR_MAX_FRAME_NUM]; /* dynamic attr for short/long frames */
} ot_isp_dp_dynamic_attr;

typedef struct {
    td_bool enable;    /* RW; Range:[0, 1];Format:1.0; TD_TRUE: enable lblc; TD_FALSE: disable lblc */
    td_u16  strength;  /* RW; Range:[0, 1023]; The strength of the lblc correction */
} ot_isp_lblc_attr;

typedef struct {
    td_u16  offset_r;  /* RW; Range:[0, 16383]; average value of the R channel Mesh BLC. */
    td_u16  offset_gr; /* RW; Range:[0, 16383]; average value of the Gr channel Mesh BLC. */
    td_u16  offset_gb; /* RW; Range:[0, 16383]; average value of the Gb channel Mesh BLC. */
    td_u16  offset_b;  /* RW; Range:[0, 16383]; average value of the B channel Mesh BLC. */
    td_u16  mesh_blc_r[OT_ISP_LBLC_GRID_POINTS];  /* RW; Range:[0, 16383]; Member used to store
                                                    the calibration data of the R channel Mesh BLC. */
    td_u16  mesh_blc_gr[OT_ISP_LBLC_GRID_POINTS]; /* RW; Range:[0, 16383]; Member used to store
                                                    the calibration data of the Gr channel Mesh BLC. */
    td_u16  mesh_blc_gb[OT_ISP_LBLC_GRID_POINTS]; /* RW; Range:[0, 16383]; Member used to store
                                                    the calibration data of the Gb channel Mesh BLC. */
    td_u16  mesh_blc_b[OT_ISP_LBLC_GRID_POINTS];  /* RW; Range:[0, 16383]; Member used to store
                                                    the calibration data of the B channel Mesh BLC. */
} ot_isp_lblc_lut_attr;

typedef struct {
    td_bool enable;        /* RW; Range:[0, 1];Format:1.0; TD_TRUE: enable lsc; TD_FALSE: disable lsc */
    td_u16  mesh_strength; /* RW; Range:[0, 16383]; The strength of the mesh shading correction */
    td_u16  blend_ratio; /* RW; Range:[0, 256];Format:9.0; the blendratio of the two mesh gain lookup-table */
} ot_isp_shading_attr;

typedef struct {
    td_u16  r_gain[OT_ISP_LSC_GRID_POINTS];  /* RW; Range:[0, 1023];Member used to store the calibration data of the
                                                R channel required for LSC. */
    td_u16  gr_gain[OT_ISP_LSC_GRID_POINTS]; /* RW; Range:[0, 1023];Member used to store the calibration data of the
                                                Gr channel required for LSC. */
    td_u16  gb_gain[OT_ISP_LSC_GRID_POINTS]; /* RW; Range:[0, 1023];Member used to store the calibration data of the
                                                Gb channel required for LSC. */
    td_u16  b_gain[OT_ISP_LSC_GRID_POINTS];  /* RW; Range:[0, 1023];Member used to store the calibration data of the
                                                B channel required for LSC. */
} ot_isp_shading_gain_lut;

typedef struct {
    td_u8   mesh_scale;        /* RW; Range:[0, 7];Format:3.0;Three bit value that selects the scale and precision for
                                  the 10 bit gain coefficients stored in mesh tables */
    td_u16  x_grid_width[OT_ISP_MLSC_X_HALF_GRID_NUM]; /* RW; Range:[4, 255],
                                                          Member used to store the width data of each GRID partition */
    td_u16  y_grid_width[OT_ISP_MLSC_Y_HALF_GRID_NUM]; /* RW; Range:[4, 255];
                                                        Member used to store the height data of each GRID partition */
    ot_isp_shading_gain_lut lsc_gain_lut[OT_ISP_MLSC_GAIN_LUT_NUM];
} ot_isp_shading_lut_attr;

typedef struct {
    ot_isp_bayer_format bayer;    /* RW; Range: [0, 3];Format ENUM;Shows bayer pattern */
    ot_isp_bayer_raw_bit raw_bit;  /* RW; Range: {8,10,12,14,16};Format ENUM;Shows input raw bitwidth */

    td_u16  img_height; /* RW; Range: [0, 65535];Format 16.0;Input raw image height */
    td_u16  img_width;  /* RW; Range: [0, 65535];Format 16.0;Input raw image width */

    td_u16  dst_img_height; /* RW; Range: [0, 65535];Format 16.0;limited Range:[0, img_height], Image height that
                               crop from input raw image, set to ImgHeight if don't need to crop */
    td_u16  dst_img_width;  /* RW; Range: [0, 65535];Format 16.0;limited Range:[0, img_width], Image width that
                               crop from input raw image, set to ImgWidth if don't need to crop */
    td_u16  offset_x;       /* RW; Range: [0, 65535];Format 16.0;limited Range:[0, img_width - dst_img_width],
                               Horizontal offset that crop from input raw image, set to 0 if don't need to crop */
    td_u16  offset_y;       /* RW; Range: [0, 65535];Format 16.0;limited Range:[0, img_height - dst_img_height],
                               Vertical offset that crop from input raw image, set to 0 if don't need to crop */

    td_u32  mesh_scale; /* RW; Range: [0, 7];Format 3.0; Shows Mesh Scale value */

    td_u16  blc_offset_r; /* RW; Range: [0, 4095];Format 12.0;BLC value for R channel */
    td_u16  blc_offset_gr; /* RW; Range: [0, 4095];Format 12.0;BLC value for Gr channel */
    td_u16  blc_offset_gb; /* RW; Range: [0, 4095];Format 12.0;BLC value for Gb channel */
    td_u16  blc_offset_b; /* RW; Range: [0, 4095];Format 12.0;BLC value for B channel */
} ot_isp_mlsc_calibration_cfg;

typedef struct {
    td_u8   mesh_scale;       /* RW; Range:[0, 7];Format:3.0;Three bit value that selects the scale and precision for
                                 the 10 bit gain coefficients stored in mesh tables */
    td_u16  x_grid_width[OT_ISP_MLSC_X_HALF_GRID_NUM]; /* RW; Range:[4, 255],
                                                     Member used to store the width data of each GRID partition */
    td_u16  y_grid_width[OT_ISP_MLSC_Y_HALF_GRID_NUM]; /* RW; Range:[4, 255],
                                                     Member used to store the height data of each GRID partition */
    ot_isp_shading_gain_lut lsc_gain_lut;
} ot_isp_mesh_shading_table;

typedef struct {
    td_bool enable;       /* RW; Range:[0,1];Format:1.0;Acs Enable */
    td_u16  y_strength;   /* RW; Range:[0,256];Format:8.0;Acs correction strength for y shading */
    td_u16  run_interval; /* RW; Range:[1,255];Format:16.0;Acs Run Interval */
    td_bool lock_enable;  /* RW; Range:[0,1];Format:1.0;Lock Gain Lut Enable */
} ot_isp_acs_attr;

typedef struct {
    td_u16  dering_strength[OT_ISP_AUTO_ISO_NUM];        /* RW; Range:[0, 1023];Format:10.0; dering strength. */
    td_u8   dering_thresh[OT_ISP_AUTO_ISO_NUM];          /* RW; Range:[0, 255];Format:8.0; dering threshold. */
    td_u8   dering_static_strength[OT_ISP_AUTO_ISO_NUM]; /* RW; Range:[0, 32];Format:6.0; dering static strength */
    td_u8   dering_motion_strength[OT_ISP_AUTO_ISO_NUM]; /* RW; Range:[0, 32];Format:6.0; dering moving strength */
} ot_isp_nr_dering_auto_attr;

typedef struct {
    td_u16  dering_strength;        /* RW; Range:[0, 1023];Format:10.0; dering strength. */
    td_u8   dering_thresh;          /* RW; Range:[0, 255];Format:8.0; dering threshold. */
    td_u8   dering_static_strength; /* RW; Range:[0, 32];Format:6.0; dering static strength. */
    td_u8   dering_motion_strength; /* RW; Range:[0, 32];Format:6.0; dering moving strength. */
} ot_isp_nr_dering_manual_attr;

typedef struct {
    ot_isp_nr_dering_auto_attr   dering_auto;
    ot_isp_nr_dering_manual_attr dering_manual;
} ot_isp_nr_dering_attr;

typedef struct {
    td_u16 sfm0_coarse_strength[OT_ISP_BAYER_CHN_NUM][OT_ISP_AUTO_ISO_NUM]; /* RW; Range:[0, 864];Format:10.0;
                                                                                sfm0 filter coarse strength */
    td_u8  sfm0_detail_prot[OT_ISP_AUTO_ISO_NUM];  /* RW; Range:[0, 31];Format:5.0; sfm0 filter detail protect rate */
    td_u16 sfm1_strength[OT_ISP_AUTO_ISO_NUM];     /* RW; Range:[0, 1023];Format:16.0; sfm1 filter strength. */
    td_u8  sfm1_adp_strength[OT_ISP_AUTO_ISO_NUM]; /* RW; Range:[0, 16];Format:5.0; sfm1 filter adaptive strength. */
    td_u8  sfm6_strength[OT_ISP_AUTO_ISO_NUM];     /* RW; Range:[0, 64];Format:7.0; sfm6 hybrid filter strength. */
    td_u8  sfm7_strength[OT_ISP_AUTO_ISO_NUM];     /* RW; Range:[0, 64];Format:7.0; sfm7 hybrid filter strength. */
    td_u8  sth[OT_ISP_AUTO_ISO_NUM];               /* RW; Range:[0, 255];Format:8.0; hybrid filter threshold. */
    td_u8  fine_strength[OT_ISP_AUTO_ISO_NUM];     /* RW; Range:[0, 128];Format:8.0; denoise rate. */
    td_u16 coring_wgt[OT_ISP_AUTO_ISO_NUM]; /* RW;Range:[0, 3200];Format:12.0;Strength of reserving the random noise. */
    td_u8  coring_mot_ratio[OT_ISP_AUTO_ISO_NUM];  /* RW; Range:[0, 63];Format:6.0; coring on moving area. */
} ot_isp_nr_snr_auto_attr;

typedef struct {
    td_u16    sfm0_coarse_strength[OT_ISP_BAYER_CHN_NUM]; /* RW; Range:[0,864];Format:10.0; sfm0 coarse strength */
    td_u8     sfm0_detail_prot;  /* RW; Range:[0, 31];Format:5.0; sfm0 filter detail protect rate */
    td_u16    sfm1_strength;     /* RW; Range:[0, 1023];Format:16.0; sfm1 filter strength. */
    td_u8     sfm1_adp_strength; /* RW; Range:[0, 16];Format:5.0; sfm1 filter adaptive strength. */
    td_u8     sfm6_strength;     /* RW; Range:[0, 64];Format:7.0; sfm6 hybrid filter strength. */
    td_u8     sfm7_strength;     /* RW; Range:[0, 64];Format:7.0; sfm7 hybrid filter strength. */
    td_u8     sth;               /* RW; Range:[0, 255];Format:8.0; hybrid filter threshold. */
    td_u8     fine_strength;     /* RW; Range:[0, 128];Format:8.0; denoise rate. */
    td_u16    coring_wgt;        /* RW;Range:[0, 3200];Format:12.0;Strength of reserving the random noise. */
    td_u8     coring_mot_ratio;  /* RW; Range:[0, 63];Format:6.0; coring on moving area. */
} ot_isp_nr_snr_manual_attr;

typedef struct {
    ot_isp_nr_snr_auto_attr   snr_auto;
    ot_isp_nr_snr_manual_attr snr_manual;
} ot_isp_nr_snr_attr_v0;

typedef enum {
    OT_NR_SNR_V0,
} ot_nr_snr_mode;

typedef struct {
    ot_nr_snr_mode snr_version;
    union {
        ot_isp_nr_snr_attr_v0 snr_attr; /* AUTO:ot_nr_snr_mode:OT_NR_SNR_V0 */
    };
} ot_isp_nr_snr_attr;

typedef struct {
    td_u8   md_mode[OT_ISP_AUTO_ISO_NUM];       /* RW; Range:[0, 1];Format:1.0; motion detection win size. */
    td_u8   md_size_ratio[OT_ISP_AUTO_ISO_NUM]; /* RW; Range:[0, 32];Format:6.0; motion detection size blend ratio. */
    td_u8   md_anti_flicker_strength[OT_ISP_AUTO_ISO_NUM]; /* RW; Range:[0, 64];Format:7.0;
                                                               motion detection anti flicker strength. */
    td_u8   md_static_ratio[OT_ISP_AUTO_ISO_NUM]; /* RW; Range:[0, 64];Format:7.0; motion detection static area ratio */
    td_u8   md_motion_ratio[OT_ISP_AUTO_ISO_NUM]; /* RW; Range:[0, 64];Format:7.0; motion detection motion area ratio */
    td_u8   md_static_fine_strength[OT_ISP_AUTO_ISO_NUM];  /* RW; Range:[0, 255];Format:8.0;
                                                              motion detection background tnr strength  */
    td_u8   tfs[OT_ISP_AUTO_ISO_NUM]; /* RW; Range:[0, 255];Format:8.0; tnr absolute strength. */
    td_bool user_define_md[OT_ISP_AUTO_ISO_NUM];  /* RW; Range:[0, 1];Format:1.0; user define motion detection mode. */
    td_s16  user_define_slope[OT_ISP_AUTO_ISO_NUM];        /* RW; Range:[-32768, 32767];Format:8.8; user define motion
                                                              detection strength change with luma. */
    td_u16  user_define_dark_thresh[OT_ISP_AUTO_ISO_NUM];  /* RW; Range:[0, 65535];Format:8.8; user define motion
                                                              detection strength in dark area. */
    td_u8   user_define_color_thresh[OT_ISP_AUTO_ISO_NUM]; /* RW; Range:[0, 64];Format:7.0; user define motion
                                                              detection strength in color area. */
    td_u8   sfr_r[OT_ISP_AUTO_ISO_NUM]; /* RW; Range:[0, 128];Format:8.0; sfr rate r */
    td_u8   sfr_g[OT_ISP_AUTO_ISO_NUM]; /* RW; Range:[0, 128];Format:8.0; sfr rate g */
    td_u8   sfr_b[OT_ISP_AUTO_ISO_NUM]; /* RW; Range:[0, 128];Format:8.0; sfr rate b */
} ot_isp_nr_tnr_auto_attr;

typedef struct {
    td_u8   md_mode;                  /* RW; Range:[0, 1];Format:1.0; motion detection win size. */
    td_u8   md_size_ratio;            /* RW; Range:[0, 32];Format:6.0; motion detection size blend ratio. */
    td_u8   md_anti_flicker_strength; /* RW; Range:[0, 64];Format:7.0; motion detection anti flicker strength. */
    td_u8   md_static_ratio;          /* RW; Range:[0, 64];Format:7.0; motion detection static area ratio */
    td_u8   md_motion_ratio;          /* RW; Range:[0, 64];Format:7.0; motion detection motion area ratio */
    td_u8   md_static_fine_strength;  /* RW; Range:[0, 255];Format:8.0; motion detection background tnr strength  */
    td_u8   tfs;                      /* RW; Range:[0, 255];Format:8.0; tnr absolute strength. */
    td_bool user_define_md;           /* RW; Range:[0, 1];Format:1.0; user define motion detection mode. */
    td_s16  user_define_slope;        /* RW; Range:[-32768, 32767];Format:8.8; user define motion detection strength
                                         change with luma. */
    td_u16  user_define_dark_thresh;  /* RW; Range:[0, 65535];Format:8.8; user define motion detection strength in
                                         dark area. */
    td_u8   user_define_color_thresh; /* RW; Range:[0, 64];Format:7.0; user define motion detection strength in
                                         color area. */
    td_u8   sfr_r;                    /* RW; Range:[0, 128];Format:8.0; sfr rate r */
    td_u8   sfr_g;                    /* RW; Range:[0, 128];Format:8.0; sfr rate g */
    td_u8   sfr_b;                    /* RW; Range:[0, 128];Format:8.0; sfr rate b */
} ot_isp_nr_tnr_manual_attr;

typedef struct {
    ot_isp_nr_tnr_auto_attr   tnr_auto;
    ot_isp_nr_tnr_manual_attr tnr_manual;
} ot_isp_nr_tnr_attr;

typedef struct {
    td_u8 snr_sfm0_wdr_strength[OT_ISP_WDR_MAX_FRAME_NUM];    /* RW; Range:[0, 64];Format:7.0; wdr sfm0 strength */
    td_u8 snr_sfm0_fusion_strength[OT_ISP_WDR_MAX_FRAME_NUM]; /* RW; Range:[0, 64];Format:7.0; fusion sfm0 strength */
    td_u8 snr_wdr_sfm6_strength[OT_ISP_WDR_MAX_FRAME_NUM];    /* RW; Range:[0, 64];Format:7.0; wdr sfm6 strength */
    td_u8 snr_wdr_sfm7_strength[OT_ISP_WDR_MAX_FRAME_NUM];    /* RW; Range:[0, 64];Format:7.0; wdr sfm7 strength */
    td_u8 snr_fusion_sfm6_strength[OT_ISP_WDR_MAX_FRAME_NUM]; /* RW; Range:[0, 64];Format:7.0; fusion sfm6 strength */
    td_u8 snr_fusion_sfm7_strength[OT_ISP_WDR_MAX_FRAME_NUM]; /* RW; Range:[0, 64];Format:7.0; fusion sfm7 strength */
    td_u8 md_wdr_strength[OT_ISP_WDR_MAX_FRAME_NUM];          /* RW; Range:[0, 64];Format:7.0; wdr md tnr strength */
    td_u8 md_fusion_strength[OT_ISP_WDR_MAX_FRAME_NUM];       /* RW; Range:[0, 64];Format:7.0; fusion md tnr strength */
} ot_isp_nr_wdr_attr;

typedef struct {
    td_u8 denoise_y_alpha;                                        /* RW; Range:[0, 255];Format:8.0; */
    td_u8 denoise_y_fg_str_lut[OT_ISP_BAYERNR_DENOISE_Y_LUT_NUM]; /* RW; Range:[0, 255];Format:8.0; */
    td_u8 denoise_y_bg_str_lut[OT_ISP_BAYERNR_DENOISE_Y_LUT_NUM]; /* RW; Range:[0, 255];Format:8.0; */
} ot_isp_nr_ynet_attr;

typedef struct {
    td_bool    enable;     /* RW; Range:[0, 1];Format:1.0; bayernr enable  */
    ot_op_mode op_type;    /* RW; Range:[0, 1];Format:1.0; bayernr operation mode  */
    td_bool    tnr_en;     /* RW; Range:[0, 1];Format:1.0; bayernr tnr enable */
    td_bool    lsc_nr_en;  /* RW; Range:[0, 1];Format:1.0; lsc nr enable. */
    td_u8      lsc_ratio1; /* RW; Range:[0, 15];Format:4.0; lsc nr ratio1. */
    td_u16     coring_ratio[OT_ISP_BAYERNR_LUT_LENGTH]; /* RW;Range:[0, 1023];Format:10.0;
                                                            Strength of reserving the random noise according to luma. */
    td_u16     mix_gain[OT_ISP_BAYERNR_LUT_LENGTH1];    /* RW;Range:[0, 128];Format:8.0; mixing rate based on luma. */

    ot_isp_nr_snr_attr   snr_cfg;
    ot_isp_nr_tnr_attr   tnr_cfg;
    ot_isp_nr_wdr_attr   wdr_cfg;
    ot_isp_nr_dering_attr   dering_cfg;
} ot_isp_nr_attr;

typedef enum {
    OT_ISP_IRBAYER_GRGBI = 0,
    OT_ISP_IRBAYER_RGBGI = 1,
    OT_ISP_IRBAYER_GBGRI = 2,
    OT_ISP_IRBAYER_BGRGI = 3,
    OT_ISP_IRBAYER_IGRGB = 4,
    OT_ISP_IRBAYER_IRGBG = 5,
    OT_ISP_IRBAYER_IBGRG = 6,
    OT_ISP_IRBAYER_IGBGR = 7,
    OT_ISP_IRBAYER_BUTT
} ot_isp_ir_bayer_format;

typedef enum {
    OT_ISP_IR_CVTMAT_MODE_NORMAL = 0,
    OT_ISP_IR_CVTMAT_MODE_MONO,
    OT_ISP_IR_CVTMAT_MODE_USER,
    OT_ISP_IR_CVTMAT_MODE_BUTT
} ot_isp_ir_cvtmat_mode;

typedef enum {
    OT_ISP_RGBIR_MODE_RGBIR4X4 = 0,
    OT_ISP_RGBIR_MODE_NORMAL,
    OT_ISP_RGBIR_MODE_BUTT
} ot_isp_rgbir_mode;

typedef struct {
    ot_isp_rgbir_mode mode; /* RW; Range:[0x0, 0x1]; 0: rgbir mode; 1: ir remove mode */
    ot_isp_bayer_format   out_pattern; /* RW;Range:[0x0, 0x3];Format:2.0; Bayer pattern of the output */
    ot_isp_ir_bayer_format in_rgbir_pattern; /* RW;Range:[0x0, 0x7];Format:3.0; IR pattern of the input */
    ot_isp_bayer_format in_bayer_pattern; /* RW;Range:[0x0, 0x3]; Format:2.0;
                                                 Bayer pattern of the input signal for ir remove mode */
    td_bool is_ir_upscale; /* RW;Range:[0x0, 0x1]; Format:1.0;
                                is upscale for ir image input of IR remove mode */
} ot_isp_rgbir_cfg;

typedef struct {
    td_bool rgbir_en; /* RW;Range:[0x0, 0x1];Format:1.0; Enable/Disable RGBIR module */
    ot_isp_rgbir_cfg rgbir_cfg;
    td_u32  ir_sum_info; /* R;Range:[0, 0xffffffff]; Format:32.0; ir_sum_info value, only read */
    td_bool auto_gain_en; /* RW;Range:[0x0,0x1];Format:1.0;  Enable/Disable auto gain */
    td_u16  auto_gain; /* RW;Range:[0, 1023]; Format:10.0; auto gain value, 256 for 1x */
    td_bool smooth_en; /* RW;Range:[0x0, 0x1];Format:1.0; Enable/Disable Smooth Function */
    td_u16  exp_ctrl[OT_ISP_RGBIR_CTRL_NUM]; /* RW;Range:[0, 2047];Format:11.0; Over expose control */
    td_u16  exp_gain[OT_ISP_RGBIR_CTRL_NUM]; /* RW;Range:[0, 511];Format:9.0; Over expose control */
    td_bool ir_rm_en; /* RW;Range:[0x0,0x1]; Format:1.0; Enable/Disable ir remove Function before interplattion */
    td_u16  ir_rm_ratio[OT_ISP_RGBIR_CROSSTALK_NUM]; /* RW;Range:[0, 1000]; Format:10.0; crosstalk, 1000 for 1x */
    ot_isp_ir_cvtmat_mode ir_cvtmat_mode;
    td_s16  cvt_matrix[OT_ISP_RGBIR_CVTMAT_NUM]; /* RW;Range:[-16384, 16383]; Format:s14.0;
                                                      RGBIR to Bayer image Convert matrix (need calibration) */
    td_u16  wb_ctrl_strength[OT_ISP_RGBIR_CTRL_NUM]; /* RW;Range:[0,1024]; Format:11.0; wb control of R/B */
} ot_isp_rgbir_attr;

/*
  Defines the type of the ISP gamma curve
  0 = Default curve
  1 = Default BT.709 curve (Gamma Curve)
  2 = Default SMPTE.2084 curve (PQ Curve), Not Support
  3 = User defined Gamma curve, LUT must be correct
*/
typedef enum {
    OT_ISP_GAMMA_CURVE_DEFAULT = 0x0,
    OT_ISP_GAMMA_CURVE_SRGB,
    OT_ISP_GAMMA_CURVE_HDR,    /* Not Support */
    OT_ISP_GAMMA_CURVE_USER_DEFINE,
    OT_ISP_GAMMA_CURVE_BUTT
} ot_isp_gamma_curve_type;
typedef struct {
    td_bool   enable;                     /* RW; Range:[0, 1]; Format:1.0;Enable/Disable Gamma Function */
    td_u16    table[OT_ISP_GAMMA_NODE_NUM];    /* RW; Range:[0, 4095]; Format:12.0;Gamma LUT nodes value */

    ot_isp_gamma_curve_type curve_type;    /* RW; Range:[0, 3]; Format:2.0;Gamma curve type */
} ot_isp_gamma_attr;

typedef struct {
    td_bool enable;                   /* RW; Range:[0, 1]; Format:1.0;Enable/Disable PreGamma Function */
    td_u32  table[OT_ISP_PREGAMMA_NODE_NUM]; /* RW; Range:SD3403V100 = [0, 0xFFFFF]; Format:20.0;
                                         Not support for Hi3519DV500;
                                         PreGamma LUT nodes value */
} ot_isp_pregamma_attr;

typedef struct {
    td_u8  luma_wgt[OT_ISP_SHARPEN_LUMA_NUM]; /* RW; Range: [0, 31];
                                               Format:0.5;Adjust the sharpen strength according to luma.
                                               Sharpen strength will be weaker when it decrease. */
    td_u16 texture_strength[OT_ISP_SHARPEN_GAIN_NUM]; /* RW; Range: [0, 4095]; Format:7.5;Undirectional sharpen
                                                         strength for texture and detail enhancement */
    td_u16 edge_strength[OT_ISP_SHARPEN_GAIN_NUM];    /* RW; Range: [0, 4095]; Format:7.5;
                                           Directional sharpen strength for edge enhancement */
    td_u16 texture_freq;       /* RW; Range: [0, 4095];Format:6.6; Texture frequency adjustment.
                                  Texture and detail will be finer when it increase */
    td_u16 edge_freq;          /* RW; Range: [0, 4095];Format:6.6; Edge frequency adjustment.
                                  Edge will be narrower and thiner when it increase */
    td_u8  over_shoot;         /* RW; Range: [0, 127]; Format:7.0;u8OvershootAmt */
    td_u8  under_shoot;        /* RW; Range: [0, 127]; Format:7.0;u8UndershootAmt */

    td_u16 motion_texture_strength[OT_ISP_SHARPEN_GAIN_NUM]; /* RW; Range: [0, 4095];Format:7.5.
                                                                Not support for Hi3519DV500 */
    td_u16 motion_edge_strength[OT_ISP_SHARPEN_GAIN_NUM]; /* RW; Range: [0, 4095]; Format:7.5.
                                                             Not support for Hi3519DV500 */
    td_u16 motion_texture_freq; /* RW; Range: [0, 4095]; Format:6.6. Not support for Hi3519DV500 */
    td_u16 motion_edge_freq;   /* RW; Range: [0, 4095]; Format:6.6. Not support for Hi3519DV500 */
    td_u8  motion_over_shoot;  /* RW; Range: [0, 127];  Format:7.0. Not support for Hi3519DV500 */
    td_u8  motion_under_shoot; /* RW; Range: [0, 127];  Format:7.0. Not support for Hi3519DV500 */

    td_u8  shoot_sup_strength; /* RW; Range: [0, 255]; Format:8.0;overshoot and undershoot suppression strength, the
                                  amplitude and width of shoot will be decrease when shootSupSt increase */
    td_u8  shoot_sup_adj;      /* RW; Range: [0, 15]; Format:4.0;overshoot and undershoot suppression adjusting,
                                  adjust the edge shoot suppression strength */
    td_u8  detail_ctrl;        /* RW; Range: [0, 255]; Format:8.0;Different sharpen strength for detail and edge.
                                  When it is bigger than 128, detail sharpen strength will be stronger than edge */
    td_u8  detail_ctrl_threshold; /* RW; Range: [0, 255]; Format:8.0; The threshold of DetailCtrl, it is used to
                                  distinguish detail and edge. */
    td_u8  edge_filt_strength;      /* RW; Range: [0, 63]; Format:6.0;The strength of edge filtering. */
    td_u8  edge_filt_max_cap;  /* RW; Range: [0, 47]; Format:6.0;The max capacity of edge filtering. */
    td_u8  r_gain;             /* RW; Range: [0, 31];   Format:5.0;Sharpen Gain for Red Area */
    td_u8  g_gain;             /* RW; Range: [0, 255]; Format:8.0; Sharpen Gain for Green Area */
    td_u8  b_gain;             /* RW; Range: [0, 31];   Format:5.0;Sharpen Gain for Blue Area */
    td_u8  skin_gain;          /* RW; Range: [0, 31]; Format:5.0;Sharpen Gain for Skin Area */
    td_u16 max_sharp_gain;     /* RW; Range: [0, 0x7FF]; Format:8.3; Maximum sharpen gain */
} ot_isp_sharpen_manual_attr;

typedef struct {
    td_u8  luma_wgt[OT_ISP_SHARPEN_LUMA_NUM][OT_ISP_AUTO_ISO_NUM];  /* RW; Range:[0, 31]; Format:0.5;
                                                                       Adjust the sharpen strength according to luma.
                                                                    Sharpen strength will be weaker when it decrease */
    td_u16 texture_strength[OT_ISP_SHARPEN_GAIN_NUM][OT_ISP_AUTO_ISO_NUM];  /* RW; Range: [0, 4095]; Format:7.5;
                                                                          Undirectional sharpen strength for texture
                                                                          and detail enhancement */
    td_u16 edge_strength[OT_ISP_SHARPEN_GAIN_NUM][OT_ISP_AUTO_ISO_NUM]; /* RW; Range: [0, 4095]; Format:7.5;
                                                                   Directional sharpen strength for edge enhancement */
    td_u16 texture_freq[OT_ISP_AUTO_ISO_NUM];   /* RW; Range: [0, 4095]; Format:6.6;Texture frequency adjustment.
                                                   Texture and detail will be finer when it increase */
    td_u16 edge_freq[OT_ISP_AUTO_ISO_NUM];      /* RW; Range: [0, 4095]; Format:6.6;Edge frequency adjustment.
                                                   Edge will be narrower and thiner when it increase */
    td_u8  over_shoot[OT_ISP_AUTO_ISO_NUM];     /* RW; Range: [0, 127];  Format:7.0 */
    td_u8  under_shoot[OT_ISP_AUTO_ISO_NUM];    /* RW; Range: [0, 127];  Format:7.0 */

    td_u16 motion_texture_strength[OT_ISP_SHARPEN_GAIN_NUM][OT_ISP_AUTO_ISO_NUM]; /* RW; Range: [0, 4095];
                                                                             Format:7.5. Not support for Hi3519DV500 */
    td_u16 motion_edge_strength[OT_ISP_SHARPEN_GAIN_NUM][OT_ISP_AUTO_ISO_NUM]; /* RW; Range: [0, 4095]; Format:7.5.
                                                                                  Not support for Hi3519DV500 */
    td_u16 motion_texture_freq[OT_ISP_AUTO_ISO_NUM]; /* RW; Range: [0, 4095]; Format:6.6. Not support for Hi3519DV500 */
    td_u16 motion_edge_freq[OT_ISP_AUTO_ISO_NUM];   /* RW; Range: [0, 4095]; Format:6.6. Not support for Hi3519DV500 */
    td_u8  motion_over_shoot[OT_ISP_AUTO_ISO_NUM];  /* RW; Range: [0, 127];  Format:7.0. Not support for Hi3519DV500 */
    td_u8  motion_under_shoot[OT_ISP_AUTO_ISO_NUM]; /* RW; Range: [0, 127];  Format:7.0. Not support for Hi3519DV500 */

    td_u8  shoot_sup_strength[OT_ISP_AUTO_ISO_NUM];  /* RW; Range: [0, 255]; Format:8.0;overshoot and undershoot
                                                   suppression strength, the amplitude and width of shoot will be
                                                   decrease when shootSupSt increase */
    td_u8  shoot_sup_adj[OT_ISP_AUTO_ISO_NUM];  /* RW; Range: [0, 15]; Format:4.0;overshoot and undershoot
                                                   suppression adjusting, adjust the edge shoot suppression strength */
    td_u8  detail_ctrl[OT_ISP_AUTO_ISO_NUM];    /* RW; Range: [0, 255]; Format:8.0;Different sharpen strength for
                                                   detail and edge. When it is bigger than 128, detail sharpen
                                                   strength will be stronger than edge. */
    td_u8  detail_ctrl_threshold[OT_ISP_AUTO_ISO_NUM]; /* RW; Range: [0, 255]; Format:8.0; The threshold of DetailCtrl,
                                                    it is used to distinguish detail and edge. */
    td_u8  edge_filt_strength[OT_ISP_AUTO_ISO_NUM];   /* RW; Range: [0, 63]; Format:6.0;
                                                    The strength of edge filtering. */
    td_u8  edge_filt_max_cap[OT_ISP_AUTO_ISO_NUM]; /* RW; Range: [0, 47]; Format:6.0;
                                                      The max capacity of edge filtering. */
    td_u8  r_gain[OT_ISP_AUTO_ISO_NUM];        /* RW; Range: [0, 31]; Format:5.0; Sharpen Gain for Red Area */
    td_u8  g_gain[OT_ISP_AUTO_ISO_NUM];        /* RW; Range: [0, 255]; Format:8.0; Sharpen Gain for Green Area */
    td_u8  b_gain[OT_ISP_AUTO_ISO_NUM];        /* RW; Range: [0, 31]; Format:5.0; Sharpen Gain for Blue Area */
    td_u8  skin_gain[OT_ISP_AUTO_ISO_NUM];      /* RW; Range: [0, 31]; Format:5.0;Sharpen Gain for Skin Area */
    td_u16 max_sharp_gain[OT_ISP_AUTO_ISO_NUM]; /* RW; Range: [0, 0x7FF]; Format:8.3; Maximum sharpen gain */
} ot_isp_sharpen_auto_attr;

typedef enum {
    OT_ISP_SHARPEN_NORMAL = 0,
    OT_ISP_SHARPEN_OFF,
    OT_ISP_SHARPEN_DIFF_NORM,
    OT_ISP_SHARPEN_DIFF,
    OT_ISP_SHARPEN_TOTAL,
    OT_ISP_SHARPEN_TEXTURE,
    OT_ISP_SHARPEN_EDGE,
    OT_ISP_SHARPEN_TOTAL_FREQ,
    OT_ISP_SHARPEN_TEXTURE_FREQ,
    OT_ISP_SHARPEN_EDGE_FREQ,
    OT_ISP_SHARPEN_BUTT
} ot_isp_sharpen_detail_map;

typedef struct {
    td_bool enable;    /* RW; Range:[0, 1]; Format:1.0;Enable/Disable sharpen module */
    td_bool motion_en;  /* RW; Range:[0, 1]; Format:1.0;Enable/Disable motion sharpen module.
                           Not support for Hi3519DV500 */
    td_u8   motion_threshold0; /* RW; Range: [0, 15]; Format:8.0. Not support for Hi3519DV500 */
    td_u8   motion_threshold1; /* RW; Range: [0, 15]; Format:8.0. Not support for Hi3519DV500 */
    td_u16  motion_gain0;      /* RW; Range: [0, 256]; Format:16.0. Not support for Hi3519DV500 */
    td_u16  motion_gain1;      /* RW; Range: [0, 256]; Format:16.0. Not support for Hi3519DV500 */
    td_u8   skin_umin;   /* RW; Range: [0, 255];  Format:8.0; U min value of the range of skin area */
    td_u8   skin_vmin;   /* RW; Range: [0, 255];  Format:8.0; V min value of the range of skin area */
    td_u8   skin_umax;   /* RW; Range: [0, 255];  Format:8.0; U max value of the range of skin area */
    td_u8   skin_vmax;   /* RW; Range: [0, 255];  Format:8.0; V max value of the range of skin area */
    ot_op_mode op_type;
    ot_isp_sharpen_detail_map  detail_map;
    ot_isp_sharpen_manual_attr manual_attr;
    ot_isp_sharpen_auto_attr   auto_attr;
} ot_isp_sharpen_attr;

/* Crosstalk Removal */
typedef struct {
    td_bool  enable;      /* RW; Range: [0, 1];Format:1.0;Enable/disable the crosstalk removal module */
    td_u8    slope;       /* RW; Range: [0, 12]; Crosstalk slope value. */
    td_u8    sensi_slope; /* RW; Range: [0, 12]; Crosstalk sensitivity. */
    td_u16   sensi_threshold; /* RW; Range: [0, 4095]; Crosstalk sensitivity threshold. */
    td_u16   strength[OT_ISP_AUTO_ISO_NUM];    /* RW; Range: [0,256];Crosstalk strength value. */
    td_u16   threshold[OT_ISP_AUTO_ISO_NUM];   /* RW; Range: [0, 4095]; Crosstalk threshold. */
    td_u8    filter_mode[OT_ISP_AUTO_ISO_NUM]; /* RW; Range: [0, 31];Format:5.0; Crosstalk filter mode. */
    td_u16   np_offset[OT_ISP_AUTO_ISO_NUM];   /* RW; Range: [512, 4095]; Not support for Hi3519DV500 */
} ot_isp_cr_attr;

typedef struct {
    td_u8    threshold;     /* RW;Range:[0x0,0x20];Format:6.0;Threshold for antifalsecolor */
    td_u8    strength;      /* RW;Range:[0x0,0x1F];Format:5.0;Strength of antifalsecolor */
} ot_isp_anti_false_color_manual_attr;

typedef struct {
    td_u8  threshold[OT_ISP_AUTO_ISO_NUM];    /* RW;Range:[0x0,0x20];Format:6.0;Threshold for antifalsecolor */
    td_u8  strength[OT_ISP_AUTO_ISO_NUM];     /* RW;Range:[0x0,0x1F];Format:5.0;Strength of antifalsecolor */
} ot_isp_anti_false_color_auto_attr;

typedef struct {
    td_bool  enable;                               /* RW;Range:[0x0,0x1];Format:1.0; AntiFalseColor Enable */
    ot_op_mode op_type;
    ot_isp_anti_false_color_manual_attr manual_attr;
    ot_isp_anti_false_color_auto_attr   auto_attr;
} ot_isp_anti_false_color_attr;

typedef struct {
    td_u8   nddm_strength;           /* RW; Range:[0x0, 0xFF]; Format:4.4; Non-direction strength */
    td_u8   nddm_mf_detail_strength; /* RW; Range:[0x0, 0x7f];
                                          Format:3.4; Non-direction medium frequent detail enhance  strength */
    td_u8   nddm_hf_detail_strength; /* RW; Range:[0x0, 0x10]; Format:2.2;
                                          Non-direction high frequent detail enhance strength */
    td_u8   detail_smooth_range;     /* RW; Range:[0x1, 0x7]; Format:4.0; Detail smooth range */
    td_u8   color_noise_f_threshold; /* RW; Range:[0x0,0xff]; Format:8.0; Threshold of color noise cancel */
    td_u8   color_noise_f_strength;  /* RW; Range:[0x0, 0x8]; Format:4.0; Strength of color noise cancel */
    td_u8   color_noise_y_threshold; /* RW; Range:[0x0, 0xF];
                                      Format:8.0; Range of color denoise luma, related to luminance and saturation */
    td_u8   color_noise_y_strength;  /* RW; Range:[0x0, 0x3F]; Format:8.0;Format:8.0; Strength of color denoise luma */
} ot_isp_demosaic_manual_attr;

typedef struct {
    td_u8   nddm_strength[OT_ISP_AUTO_ISO_NUM];     /* RW; Range:[0x0, 0xFF]; Format:4.4; Non-direction strength */
    td_u8   nddm_mf_detail_strength[OT_ISP_AUTO_ISO_NUM]; /* RW; Range:[0x0, 0x7f]; Format:3.4;
                                                               Non-direction medium frequent detail enhance strength */
    td_u8   nddm_hf_detail_strength[OT_ISP_AUTO_ISO_NUM]; /* RW; Range:[0x0, 0x10]; Format:2.2; Non-direction
                                                               high frequent detail enhance strength */
    td_u8   detail_smooth_range[OT_ISP_AUTO_ISO_NUM];     /* RW; Range:[0x1, 0x7]; Format:4.0; Detail smooth range */
    td_u8   color_noise_f_threshold[OT_ISP_AUTO_ISO_NUM]; /* RW; Range:[0x0, 0xff]; Format:8.0;
                                                               Threshold of color noise cancel */
    td_u8   color_noise_f_strength[OT_ISP_AUTO_ISO_NUM];  /* RW; Range:[0x0, 0x8]; Format:4.0; Strength of color
                                                               noise cancel */
    td_u8   color_noise_y_threshold[OT_ISP_AUTO_ISO_NUM];  /* RW; Range:[0x0, 0xF]; Format:8.0;
                                                    Range of color denoise luma, related to luminance and saturation */
    td_u8   color_noise_y_strength[OT_ISP_AUTO_ISO_NUM];  /* RW; Range:[0x0, 0x3F]; Format:8.0;Format:8.0;
                                                               Strength of color denoise luma */
} ot_isp_demosaic_auto_attr;

typedef struct {
    td_bool enable;        /* RW; Range:[0, 1]; Format:1.0;Enable/Disable demosaic module */
    ot_op_mode op_type;
    td_u16     ai_detail_strength;    /* RW; Range:[0x0, 0x1FF]; Format:9.0; ai detail strength */
    ot_isp_demosaic_manual_attr manual_attr;
    ot_isp_demosaic_auto_attr   auto_attr;
} ot_isp_demosaic_attr;

/* Defines the attributes of the ISP black level */
typedef enum {
    OT_ISP_BLACK_LEVEL_MODE_AUTO    = 0,
    OT_ISP_BLACK_LEVEL_MODE_MANUAL  = 1,
    OT_ISP_BLACK_LEVEL_MODE_DYNAMIC = 2,
    OT_ISP_BLACK_LEVEL_MODE_BUTT
} ot_isp_black_level_mode;

typedef struct {
    td_u16 black_level[OT_ISP_WDR_MAX_FRAME_NUM][OT_ISP_BAYER_CHN_NUM]; /* RW; Range: [0x0, 0x3FFF];Format:14.0;
                                                 sensor Black level values that correspond to the black levels of the
                                                 R,Gr, Gb, and B components respectively. */
} ot_isp_black_level_manual_attr;

typedef enum {
    OT_ISP_DYNAMIC_PATTERN_RGB  = 0,
    OT_ISP_DYNAMIC_PATTERN_RGBIR,
    OT_ISP_DYNAMIC_PATTERN_BUTT
} ot_isp_black_level_dynamic_pattern;

typedef struct {
    ot_isp_black_level_dynamic_pattern pattern;  /* RW; Range: [0, 1]; The pattern of raw. */
    ot_rect ob_area;        /* RW; xRange: [0, width of the ob_area]; yRange: [0, height of the ob_area];
                            obWidthRange: [0, width of the ob_area - x]; obHeightRange: [0, height of ob_area - y];
                            The size of OB area. */
    td_u16  low_threshold;   /* RW; Range: [0x0, 0xFFF]; Format:14.0;Lower limit of normal pixel value; */
    td_u16  high_threshold;  /* RW; Range: [0x0, 0x3FFF]; Format:14.0;Higher limit of normal pixel value; */
    td_s16  offset[OT_ISP_AUTO_ISO_NUM]; /* RW; Range: [-0x3FFF, 0x3FFF]; Format:15.0;Offset limit of dynamic blc; */
    td_u16  tolerance;       /* RW; Range: [0x0, 0x3FFF]; Format:14.0;Tolerance limit of dynamic blc; */
    td_u8   filter_strength; /* RW; Range: [0, 64];Format:6.0; ob black level minus light black level */
    td_bool separate_en;     /* RW; Range: [0, 1]; set rggb black level separately */
    td_u16  calibration_black_level[OT_ISP_AUTO_ISO_NUM]; /* RW; Range: [0x0, 0x3FFF]; Format:14.0;
                                                             dynamic blc of again; */
    td_u16  filter_thr;      /* RW; Range: [0, 0x3FFF]; Format:14.0; black level IIR filter threshold; */
} ot_isp_black_level_dynamic_attr;

typedef struct {
    td_bool  user_black_level_en;
    td_u16   user_black_level[OT_ISP_WDR_MAX_FRAME_NUM][OT_ISP_BAYER_CHN_NUM];  /* RW; Range: [0x0, 0x3FFF];Format:14.0;
                                                 user define Black level values that correspond to the black levels of
                                                 the R,Gr, Gb, and B components respectively. */
    ot_isp_black_level_mode         black_level_mode;
    ot_isp_black_level_manual_attr  manual_attr;
    ot_isp_black_level_dynamic_attr dynamic_attr;
} ot_isp_black_level_attr;

/*
  Defines the FPN removal type
  0 = Frame mode correction
  1 = Line mode correction
 */
typedef enum {
    OT_ISP_FPN_TYPE_FRAME = 0,
    OT_ISP_FPN_TYPE_LINE = 1,
    OT_ISP_FPN_TYPE_BUTT
} ot_isp_fpn_type;

/*
  Defines the FPN frame output mode
  0: the pixel value algin with the least significant bit (right-most bit)
  1: the pixel value algin with the most significant bit (left-most bit)
 */
typedef enum {
    OT_ISP_FPN_OUT_MODE_NORM = 0,
    OT_ISP_FPN_OUT_MODE_HIGH,
    OT_ISP_FPN_OUT_MODE_BUTT
} ot_isp_fpn_out_mode;

/* Defines the information about calibrated black frames */
typedef struct {
    td_u32              iso;             /* RW;Range:[0x64,0xFFFFFFFF];Format:32.0;FPN CALIBRATE ISO */
    td_u32              offset[OT_ISP_STRIPING_MAX_NUM]; /* RW;Range:[0, 0x3FFF];Format:14.0;
                                                          FPN frame u32Offset (agv pixel value),
                                                          only higher 12bits valid  */
    td_u32              frm_size;         /* RW;FPN frame size (exactly frm size or compress len) */
    ot_video_frame_info fpn_frame;         /* FPN frame info, 8bpp,10bpp,12bpp,16bpp. Compression or not */
} ot_isp_fpn_frame_info;

/* Defines the calibration attribute of the FPN removal module */
typedef struct {
    td_u32                 threshold;  /* RW;Range:[1,0xFFF];Format:12.0;pix value > threshold means defective pixel */
    td_u32                 frame_num;  /* RW;Range:[1, 16];Format:5.0;Number of frames to be calibrated.
                                        The value range is {1, 2, 4, 8, 16},that is, the integer exponentiation of 2 */
    ot_isp_fpn_type        fpn_type;   /* frame mode or line mode */
    ot_isp_fpn_frame_info  fpn_cali_frame;
    ot_isp_fpn_out_mode    fpn_mode;   /* FPN frame output mode, for normal use, set to 0 */
} ot_isp_fpn_calibrate_attr;

/* Defines the manual correction attribute of the FPN removal module */
typedef struct {
    td_u32  strength;         /* RW;Range:[0,1023];Format:10.0;Manual correction strength */
} ot_isp_fpn_manual_attr;

/* Defines the automatic correction attribute of the FPN removal module */
typedef struct {
    td_u32 strength;          /* RW;Range:[0,1023];Format:10.0;Auto correction strength */
} ot_isp_fpn_auto_attr;

/* Defines the correction attribute of the FPN removal module */
typedef struct {
    td_bool                enable;   /* RW;Range:[0,1];Format:1.0; */
    td_bool                aibnr_mode; /* RW;Range:[0,1];Format:1.0; */
    ot_op_mode             op_type;  /* manual or auto mode */
    ot_isp_fpn_type        fpn_type;
    ot_isp_fpn_frame_info  fpn_frm_info;
    ot_isp_fpn_manual_attr manual_attr;
    ot_isp_fpn_auto_attr   auto_attr;
} ot_isp_fpn_attr;

/* Defines the manual dehaze attribute */
typedef struct {
    td_u8 strength;  /* RW;Range:[0,0xFF];Format:8.0;Manual dehaze strength */
} ot_isp_dehaze_manual_attr;

/* Defines the automatic dehaze attribute */
typedef struct {
    td_u8 strength; /* RW;Range:[0,0xFF];Format:8.0;Weighted coefficient for automatic dehaze strength. */
} ot_isp_dehaze_auto_attr;

/* Defines the ISP dehaze attribute */
typedef struct {
    td_bool  enable;           /* RW;Range:[0,1];Format:1.0; */
    td_bool  user_lut_en;    /* RW;Range:[0,1];0:Auto Lut 1:User Lut */
    td_u8    dehaze_lut[OT_ISP_DEHAZE_LUT_SIZE];
    ot_op_mode                op_type;
    ot_isp_dehaze_manual_attr manual_attr;
    ot_isp_dehaze_auto_attr   auto_attr;
    td_u16       tmprflt_incr_coef; /* RW, Range: [0x0, 0x80].filter increase coefficient. */
    td_u16       tmprflt_decr_coef; /* RW, Range: [0x0, 0x80].filter decrease coefficient. */
} ot_isp_dehaze_attr;

/* Defines purple fringing correction attribute */
typedef struct {
    td_u16  edge_threshold[OT_ISP_CAC_THR_NUM][OT_ISP_AUTO_ISO_NUM]; /* RW; range: [0, 4095];
                                                                            format:12.0; cac edge threshold */
    td_u16  edge_gain[OT_ISP_AUTO_ISO_NUM];           /* RW; range: [0, 1023]; format:10.0; cac edge gain */
    td_u16  cac_rb_strength[OT_ISP_AUTO_ISO_NUM];     /* RW; range: [0, 31]; format:5.0; cac strength on rb */
    td_u16  purple_alpha[OT_ISP_AUTO_ISO_NUM];        /* RW; range: [0, 63]; format:6.0; cac purple alpha */
    td_u16  edge_alpha[OT_ISP_AUTO_ISO_NUM];          /* RW; range: [0, 63]; format:6.0; cac edge  alpha */
    td_u16  satu_low_threshold[OT_ISP_AUTO_ISO_NUM];  /* RW; range: [0, 4095]; format:12.0; cac purple saturation
                                                         low threshold */
    td_u16  satu_high_threshold[OT_ISP_AUTO_ISO_NUM]; /* RW; range: [0, 16383]; format:14.0; cac purple saturation
                                                         high threshold */
} ot_isp_cac_acac_auto_attr;

typedef struct {
    td_u16  edge_threshold[OT_ISP_CAC_THR_NUM];  /* RW; range: [0, 4095]; format:12.0; cac edge threshold */
    td_u16  edge_gain;              /* RW; range: [0, 1023]; format:10.0; cac edge gain */
    td_u16  cac_rb_strength;        /* RW; range: [0, 31]; format:5.0; cac strength on rb */
    td_u16  purple_alpha;           /* RW; range: [0, 63]; format:6.0; cac purple alpha */
    td_u16  edge_alpha;             /* RW; range: [0, 63]; format:6.0; cac edge alpha */
    td_u16  satu_low_threshold;     /* RW; range: [0, 4095]; format:12.0; cac purple saturation low threshold */
    td_u16  satu_high_threshold;    /* RW; range: [0, 16383]; format:14.0; cac purple saturation high threshold */
} ot_isp_cac_acac_manual_attr;

typedef struct {
    ot_isp_cac_acac_manual_attr     acac_manual;
    ot_isp_cac_acac_auto_attr       acac_auto;
} ot_isp_cac_acac_attr;

typedef struct {
    td_u8   de_purple_cr_strength[OT_ISP_CAC_EXP_RATIO_NUM]; /* RW; range: [0, 8]; format:4.0;
                                                         correction strength of the R channel */
    td_u8   de_purple_cb_strength[OT_ISP_CAC_EXP_RATIO_NUM]; /* RW; range: [0, 8]; format:4.0;
                                                         correction strength of the B channel */
} ot_isp_cac_lcac_auto_attr;

typedef struct {
    td_u8   de_purple_cr_strength;  /* RW; range: [0, 8]; format:4.0; correction strength of the R channel */
    td_u8   de_purple_cb_strength;  /* RW; range: [0, 8]; format:4.0; correction strength of the B channel */
} ot_isp_cac_lcac_manual_attr;

typedef struct {
    td_u16  purple_detect_range;        /* RW; range: [0, 410]; format:9.0; purple fringing detection scope */
    td_u16  var_threshold;              /* RW; range: [0, 4095]; format:12.0; edge detection threshold */
    td_u16  r_detect_threshold[OT_ISP_CAC_CURVE_NUM]; /* RW; range: [0,4095]; format:12.0; component R detection thd */
    td_u16  g_detect_threshold[OT_ISP_CAC_CURVE_NUM]; /* RW; range: [0,4095]; format:12.0; component G detection thd */
    td_u16  b_detect_threshold[OT_ISP_CAC_CURVE_NUM]; /* RW; range: [0,4095]; format:12.0; component B detection thd */
    ot_isp_cac_lcac_manual_attr     lcac_manual;
    ot_isp_cac_lcac_auto_attr       lcac_auto;
} ot_isp_cac_lcac_attr;

typedef struct {
    td_bool enable;                     /* RW; range: [0, 1]; format:1.0; cac enable */
    ot_op_mode op_type;                 /* RW; range: [0, 1]; format:1.0; purple fringing correction working mode */
    td_u8   detect_mode;                /* RW; range: [0, 2]; format:2.0; cac edge detect mode */
    td_s16  purple_upper_limit;         /* RW; range: [-2048,2047]; format:s12.0; cac purple high limitation */
    td_s16  purple_lower_limit;         /* RW; range: [-2048,2047]; format:s12.0; cac purple low limitation */
    ot_isp_cac_acac_attr     acac_cfg;
    ot_isp_cac_lcac_attr     lcac_cfg;
} ot_isp_cac_attr;


typedef struct {
    td_u8  mf_strength[OT_ISP_BSHP_CURVE_NUM];   /* RW; range:[0, 63];mf_strength. */
    td_u8  hf_strength[OT_ISP_BSHP_CURVE_NUM];   /* RW; range:[0, 63];hf_strength. */
    td_u8  dark_strength[OT_ISP_BSHP_CURVE_NUM]; /* RW; range:[0, 63];dark_strength. */
    td_u8  mf_gain;                /* RW; range:[0, 15];format:4.0; mf gain */
    td_u8  hf_gain;                /* RW; range:[0, 15];format:4.0; hf gain */
    td_u8  dark_gain;              /* RW; range:[0, 15];format:4.0; dark gain */
    td_u16 overshoot;              /* RW; range:[0, 255];format:8.0; over shoot */
    td_u16 undershoot;             /* RW; range:[0, 255];format:8.0; under shoot */
} ot_isp_bayershp_manual_attr;

typedef struct {
    td_u8  mf_strength[OT_ISP_BSHP_CURVE_NUM][OT_ISP_AUTO_ISO_NUM];   /* RW; range:[0, 63];mf_strength. */
    td_u8  hf_strength[OT_ISP_BSHP_CURVE_NUM][OT_ISP_AUTO_ISO_NUM];   /* RW; range:[0, 63];hf_strength. */
    td_u8  dark_strength[OT_ISP_BSHP_CURVE_NUM][OT_ISP_AUTO_ISO_NUM]; /* RW; range:[0, 63];dark_strength. */
    td_u8  mf_gain[OT_ISP_AUTO_ISO_NUM];    /* RW; range:[0, 15];format:4.0; mf gain */
    td_u8  hf_gain[OT_ISP_AUTO_ISO_NUM];    /* RW; range:[0, 15];format:4.0; hf gain */
    td_u8  dark_gain[OT_ISP_AUTO_ISO_NUM];  /* RW; range:[0, 15];format:4.0; dark gain */
    td_u16 overshoot[OT_ISP_AUTO_ISO_NUM];  /* RW; range:[0, 255];format:8.0; over shoot */
    td_u16 undershoot[OT_ISP_AUTO_ISO_NUM]; /* RW; range:[0, 255];format:8.0; under shoot */
} ot_isp_bayershp_auto_attr;

typedef struct {
    td_bool enable;                                 /* RW; range:[0, 1]; format:1.0;en/disable bayershrpen module */
    td_u16  dark_threshold[OT_ISP_BSHP_THD_NUM];    /* RW; range:[0, 4095];format:12.0; dark threshold */
    td_u16  texture_threshold[OT_ISP_BSHP_THD_NUM]; /* RW; range:[0, 4095];format:12.0; texture threshold */
    ot_op_mode op_type;
    ot_isp_bayershp_manual_attr manual_attr;
    ot_isp_bayershp_auto_attr   auto_attr;
} ot_isp_bayershp_attr;

/* Defines the Radial Crop attribute */
typedef struct {
    td_bool  enable;       /* RW;Range:[0, 1];Format:1.0; enable/disable ridial crop */
    ot_point center_coord; /* RW;the coordinate of central pixel */
    td_u32   radius;       /* RW;Range:[0, 11586];Format:14.0; when the distance to central pixel is greater than
                              u32Radius, the pixel value becomes 0. */
} ot_isp_rc_attr;

typedef struct {
    td_u16 texture_strength[OT_ISP_SHARPEN_GAIN_NUM]; /* R; range: [0, 4095]; Format:7.5;Undirectional sharpen
                                                         strength for texture and detail enhancement */
    td_u16 edge_strength[OT_ISP_SHARPEN_GAIN_NUM];    /* R; range: [0, 4095]; Format:7.5;Directional sharpen strength
                                                         for edge enhancement */
    td_u16 texture_freq;         /* R; range: [0, 4095];Format:6.6; Texture frequency adjustment.
                                    Texture and detail will be finer when it increase */
    td_u16 edge_freq;            /* R; range: [0, 4095];Format:6.6; Edge frequency adjustment.
                                    Edge will be narrower and thiner when it increase */
    td_u8  over_shoot;           /* R; range: [0, 127]; Format:7.0;u8OvershootAmt */
    td_u8  under_shoot;          /* R; range: [0, 127]; Format:7.0;u8UndershootAmt */
    td_u8  shoot_sup_strength;   /* R; range: [0, 255]; Format:8.0;overshoot and undershoot suppression strength,
                                    the amplitude and width of shoot will be decrease when shootSupSt increase */

    td_u8   nr_lsc_ratio;                     /* R;Range:[0x0, 0xf];Format:4.0; lsc nr ratio */
    td_u16  coarse_strength[OT_ISP_BAYER_CHN_NUM];    /* R;Range:[0x0, 0x360];Format:10.0;
                                                 Coarse Strength of noise reduction */
    td_u8   wdr_frame_strength[OT_ISP_WDR_MAX_FRAME_NUM]; /* R;Range:[0x0, 0x40];Format:7.0;
                                                 Coarse strength of each frame in wdr mode */
    td_u8   fine_strength;          /* R;Range:[0x0,0x80];Format:8.0;Strength of Luma noise reduction */
    td_u16  coring_wgt;             /* R;Range:[0x0,0xC80];Format:12.0;Strength of reserving the random noise */

    td_u16 dehaze_strength_actual; /* R;Range:[0,0xFF];Format:8.0;actual dehaze strength */
    td_u16 drc_strength_actual;    /* R;Range: Hi3519DV500 = [0x0, 0x3FF];
                                    Strength of dynamic range compression.
                                    Higher values lead to higher differential gain between shadows and highlights. */
    td_u32 wdr_exp_ratio_actual[OT_ISP_WDR_MAX_FRAME_NUM - 1]; /* R; Range:[0x40, 0x4000]; Format:26.6;
                                                         0x40 means 1 times.
                                                         When enExpRatioType is OT_OP_MODE_AUTO, u32ExpRatio is invalid.
                                                         When enExpRatioType is OT_OP_MODE_MANUAL, u32ExpRatio is
                                                         quotient of long exposure time / short exposure time. */
    td_bool wdr_switch_finish;       /* R; Range:[0, 1];Format:1.0;TD_TRUE: WDR switch is finished */
    td_bool res_switch_finish;       /* R; Range:[0, 1];Format:1.0;TD_TRUE: Resolution switch is finished */
    td_u16  black_level_actual[OT_ISP_WDR_MAX_FRAME_NUM][OT_ISP_BAYER_CHN_NUM]; /* R; Range: [0x0, 0x3FFF];Format:14.0;
                                             Actual Black level values that correspond to the black levels of
                                             the R,Gr, Gb, and B components respectively. */

    td_u16  sns_black_level[OT_ISP_WDR_MAX_FRAME_NUM][OT_ISP_BAYER_CHN_NUM]; /* R; Range: [0x0, 0x3FFF];Format:14.0;
                                                      sesnor Black level values that correspond to the black levels of
                                                      the R,Gr, Gb, and B components respectively. */
} ot_isp_inner_state_info;

typedef enum {
    OT_ISP_AI_NONE = 0,
    OT_ISP_AI_BNR,
    OT_ISP_AI_DRC,
    OT_ISP_AI_DEMOSAIC,
    OT_ISP_AI_BUTT
} ot_isp_ai_mode;

/*
  The position of AE histogram in ISP BE pipeline
  0 = After DG;
  1 = After static WB;
  2 = After DRC;
 */
typedef enum {
    OT_ISP_AE_AFTER_DG = 0,
    OT_ISP_AE_AFTER_WB,
    OT_ISP_AE_AFTER_DRC,
    OT_ISP_AE_SWITCH_BUTT
} ot_isp_ae_switch;

/*
  FourPlaneMode enable
  0 : Disabled;
  1 : Enabled
*/
typedef enum {
    OT_ISP_AE_FOUR_PLANE_MODE_DISABLE = 0,
    OT_ISP_AE_FOUR_PLANE_MODE_ENABLE,
    OT_ISP_AE_FOUR_PLANE_MODE_BUTT
} ot_isp_ae_four_plane_mode;

typedef enum {
    OT_ISP_AE_HIST_SKIP_EVERY_PIXEL = 0,
    OT_ISP_AE_HIST_SKIP_EVERY_2ND_PIXEL,
    OT_ISP_AE_HIST_SKIP_EVERY_3RD_PIXEL,
    OT_ISP_AE_HIST_SKIP_EVERY_4TH_PIXEL,
    OT_ISP_AE_HIST_SKIP_EVERY_5TH_PIXEL,
    OT_ISP_AE_HIST_SKIP_EVERY_8TH_PIXEL,
    OT_ISP_AE_HIST_SKIP_EVERY_9TH_PIXEL,
    OT_ISP_AE_HIST_SKIP_BUTT
} ot_isp_ae_hist_skip;

typedef enum {
    OT_ISP_AE_HIST_START_FORM_FIRST_COLUMN = 0,
    OT_ISP_AE_HIST_START_FORM_SECOND_COLUMN,
    OT_ISP_AE_HIST_OFFSET_X_BUTT
} ot_isp_ae_hist_offset_x;

typedef enum {
    OT_ISP_AE_HIST_START_FORM_FIRST_ROW = 0,
    OT_ISP_AE_HIST_START_FORM_SECOND_ROW,
    OT_ISP_AE_HIST_OFFSET_Y_BUTT
} ot_isp_ae_hist_offset_y;

/* Defines the mode configuration for the sampling points during global histogram statistics. */
typedef struct {
    ot_isp_ae_hist_skip hist_skip_x;  /* RW; Range:[0, 6]; Format:4.0;
                                         Histogram decimation in horizontal direction: 0=every pixel;
                                         1=every 2nd pixel; 2=every 3rd pixel; 3=every 4th pixel; 4=every 5th pixel;
                                         5=every 8th pixel ; 6+=every 9th pixel */
    ot_isp_ae_hist_skip hist_skip_y;  /* RW; Range:[0, 6]; Format:4.0;
                                         Histogram decimation in vertical direction: 0=every pixel; 1=every 2nd pixel;
                                         2=every 3rd pixel; 3=every 4th pixel; 4=every 5th pixel; 5=every 8th pixel;
                                         6+=every 9th pixel */
    ot_isp_ae_hist_offset_x hist_offset_x;  /* RW; Range:[0, 1]; Format:1.0;
                                               0= start from the first column; 1=start from second column */
    ot_isp_ae_hist_offset_y hist_offset_y;  /* RW; Range:[0, 1]; Format:1.0;
                                               0= start from the first row; 1= start from second row */
} ot_isp_ae_hist_config;

typedef enum {
    OT_ISP_AE_NORMAL = 0,
    OT_ISP_AE_ROOT,
    OT_ISP_AE_STAT_MODE_BUTT
} ot_isp_ae_stat_mode;

/* Crops the input image of the AE module */
typedef struct {
    td_bool enable;  /* RW; Range: [0,1];  Format:1.0;AE crop enable. */
    td_u16  x;   /* RW; Range: Hi3519DV500 = [0, 8192 - 256];
                    Format:13.0;AE image crop start x, limited range:[0, ImageWidth - 256] */
    td_u16  y;   /* RW; Range: Hi3519DV500 = [0, 8192 - 120];
                    Format:13.0;AEimage crop start y, limited range:[0, ImageHeight - 120]  */
    td_u16  width;   /* RW; Range: Hi3519DV500 = [256, 8192];
                    Format:14.0;AE image crop width,  limited range:[256, ImageWidth] */
    td_u16  height;   /* RW; Range: Hi3519DV500 = [120, 8192];
                    Format:14.0;AE image crop height  limited range:[120, ImageHeight] */
} ot_isp_ae_crop;

/* config of statistics structs */
typedef struct {
    ot_isp_ae_switch          ae_switch;          /* RW; Range:[0, 2]; Format:2.0;
                                                     The position of AE histogram in ISP BE pipeline */
    ot_isp_ae_hist_config     hist_config;
    ot_isp_ae_four_plane_mode four_plane_mode;    /* RW; Range:[0, 1]; Format:2.0;Four Plane Mode Enable */
    ot_isp_ae_stat_mode       hist_mode;          /* RW; Range:[0, 1]; Format:2.0;AE Hist Rooting Mode */
    ot_isp_ae_stat_mode       aver_mode;          /* RW; Range:[0, 1]; Format:2.0;AE Aver Rooting Mode */
    ot_isp_ae_stat_mode       max_gain_mode;      /* RW; Range:[0, 1]; Format:2.0;Max Gain Rooting Mode */
    ot_isp_ae_crop            crop;               /* RW; AE input image crop for  module */
    ot_isp_ae_crop            fe_crop;            /* RW; AE input image crop for FE module */
    td_u8 weight[OT_ISP_AE_ZONE_ROW][OT_ISP_AE_ZONE_COLUMN]; /* RW; Range:[0x0, 0xF]; Format:4.0; AE weighting table */
} ot_isp_ae_stats_cfg;

/* Smart Info */
#define OT_ISP_PEOPLE_CLASS_MAX   2
typedef struct {
    td_bool  enable;                /* R; Range:[0, 1]; enable smart roi info */
    td_bool  available;             /* R; Range:[0, 1]; whether roi area is available */
    td_u8    luma;                  /* R; Range:[0x00, 0xFF]; luma of roi area */
} ot_isp_people_roi;

typedef enum {
    OT_ISP_FACE_INDEX = 0,
    OT_ISP_PEOPLE_INDEX = 1,
    OT_ISP_PEOPLE_BUTT
} ot_isp_people_type;

#define OT_ISP_TUNNEL_CLASS_MAX   2
typedef struct {
    td_bool  enable;                /* R; Range:[0, 1]; enable tunnel roi info */
    td_bool  available;             /* R; Range:[0, 1]; whether roi area is available */
    td_u32   tunnel_area_ratio;     /* R; Range:[0, 10000]; the proportion of tunnels in the whole picture */
    td_u32   tunnel_exp_perf;       /* R; Range:[0, 10000]; the performance of tunnels exposure result */
} ot_isp_tunnel_roi;

typedef enum {
    OT_ISP_TUNNEL_IN_INDEX = 0,
    OT_ISP_TUNNEL_OUT_INDEX = 1,
    OT_ISP_TUNNEL_BUTT
} ot_isp_tunnel_type;

#define OT_ISP_FACE_NUM 5
typedef struct {
    td_bool  enable;                     /* R; Range:[0, 1]; enable fast face roi info */
    td_bool  available;                  /* R; Range:[0, 1]; whether roi area is available */
    td_u64   frame_pts;                  /* pts of face info */
    ot_rect face_rect[OT_ISP_FACE_NUM];  /* face roi rect */
} ot_isp_face_roi;

typedef struct {
    ot_isp_people_roi  people_roi[OT_ISP_PEOPLE_CLASS_MAX]; /* people ae info */
    ot_isp_tunnel_roi  tunnel_roi[OT_ISP_TUNNEL_CLASS_MAX]; /* tunnels ae info */
    ot_isp_face_roi face_roi;                               /* fast face ae info */
} ot_isp_smart_info;

/* fines whether the peak value of the zoned IIR statistics is calculated.  */
typedef enum {
    OT_ISP_AF_STA_NORM         = 0,    /* use every value of the block statistic */
    OT_ISP_AF_STA_PEAK,                /* use the maximum value in one line of the block statistic */
    OT_ISP_AF_STA_BUTT
} ot_isp_af_peak_mode;

/* Defines whether the zoned statistics are squared in square mode.  */
typedef enum {
    OT_ISP_AF_STA_SUM_NORM     = 0,    /* statistic value accumulate */
    OT_ISP_AF_STA_SUM_SQUARE,          /* statistic value square then accumulate */
    OT_ISP_AF_STA_SUM_BUTT
} ot_isp_af_square_mode;
/* Crops the input image of the AF module.  */
typedef struct {
    td_bool enable;   /* RW; Range: [0,1];  Format:1.0; AF crop enable */
    td_u16  x;    /* RW; Range: Hi3519DV500 = [0, 7936];
                     Format:13.0;AF image crop start x, limited range:[0, ImageWidth-256] */
    td_u16  y;    /* RW; Range: Hi3519DV500 = [0, 8072];
                     Format:13.0;AF image crop start y, limited range:[0, ImageHeight-120] */
    td_u16  width;    /* RW; Range: Hi3519DV500 = [256, 8192];
                     Format:14.0;AF image crop width, limited range:[256, ImageWidth] */
    td_u16  height;    /* RW; Range: Hi3519DV500 = [120, 8192];
                     Format:14.0;AF image crop height, limited range:[120, ImageHeight] */
} ot_isp_af_crop;

/* Defines the position of AF module statistics.  */
typedef enum {
    OT_ISP_AF_STATS_AFTER_DGAIN = 0, /* The AF module is placed in the raw field for statistics,AF after DGain */
    OT_ISP_AF_STATS_AFTER_DRC,       /* The AF module is placed in the raw field for statistics,AF after DRC */
    OT_ISP_AF_STATS_AFTER_CSC,       /* The AF module is placed in the YUV field for statistics,AF after CSC */
    OT_ISP_AF_STATS_BUTT
} ot_isp_af_stats_pos;

/* Configures the Bayer field of the AF module.  */
typedef struct {
    td_u8               gamma_gain_limit; /* RW; Range: [0x0, 0x5]; Format:3.0; Not support for Hi3519DV500 */
    td_u8               gamma_value;      /* RW; Range: SD3403V100 = [0x0, 0x6]| Hi3519DV500 = [0x0, 0x1]; */
    ot_isp_bayer_format bayer_format;     /* RW; Range: [0x0, 0x3]; Format:2.0;raw domain pattern */
} ot_isp_af_raw_cfg;

/* Configures the pre-filter of the AF module.  */
typedef struct {
    td_bool             enable;       /* RW; Range: [0,1]; Format:1.0; pre filter enable  .          */
    td_u16              strength; /* RW; Range: [0x0, 0xFFFF]; Format:16.0;pre filter strength    */
} ot_isp_af_pre_filter_cfg;

/* Defines AF statistics configuration parameters.  */
typedef struct {
    td_bool                  af_en;         /* RW; Range: [0,1];   AF enable. */
    td_u16                   zone_col;      /* RW; Range: [1, 17]; AF statistic window horizontal block.  */
    td_u16                   zone_row;      /* RW; Range: [1, 15]; AF statistic window vertical block.  */
    ot_isp_af_peak_mode      peak_mode;     /* RW; Range: [0,1]; AF peak value statistic mode. */
    ot_isp_af_square_mode    square_mode;   /* RW; Range: [0,1]; AF statistic square accumulate. */
    ot_isp_af_crop           crop;          /* RW; AF input image crop */
    ot_isp_af_crop           fe_crop;       /* RW; AF input image crop for FE module */
    ot_isp_af_stats_pos      stats_pos;     /* RW; Range: [0,2]; AF statistic position, it can be set to yuv or raw */
    ot_isp_af_raw_cfg        raw_cfg;       /* RW; When AF locate at RAW domain, these para should be cfg. */
    ot_isp_af_pre_filter_cfg pre_flt_cfg;   /* RW; pre filter cfg */
    td_u16                   high_luma_threshold;   /* RW; Range: [0,0xFF]; high luma threshold. */
} ot_isp_af_cfg;

/* Configures the AF level depend gain module.  */
typedef struct {
    td_bool     enable;                       /* RW; Range: [0, 1]; FILTER level depend gain enable. */
    td_u16      threshold_low;            /* RW; range: [0x0, 0xFF]; FILTER level depend th low */
    td_u16      gain_low;                 /* RW; range: [0x0, 0xFF]; FILTER level depend gain low */
    td_u16      slope_low;                /* RW; range: [0x0, 0xF];  FILTER level depend slope low */
    td_u16      threshold_high;           /* RW; range: [0x0, 0xFF]; FILTER level depend th high */
    td_u16      gain_high;                /* RW; range: [0x0, 0xFF]; FILTER level depend gain high */
    td_u16      slope_high;               /* RW; range: [0x0, 0xF];  FILTER level depend slope high */
} ot_isp_af_level_depend;

/* Configures the AF coring module.  */
typedef struct {
    td_u16      threshold;                 /* RW; Range: [0x0, 0x7FF];FILTER coring threshold. */
    td_u16      slope;                     /* RW; Range: [0x0, 0xF];  FILTER Coring Slope */
    td_u16      limit;                     /* RW; Range: [0x0, 0x7FF];FILTER coring limit */
} ot_isp_af_coring;

#define OT_ISP_IIR_EN_NUM                                3
#define OT_ISP_IIR_GAIN_NUM                              7
#define OT_ISP_IIR_SHIFT_NUM                             4
/* Defines the IIR parameter configuration of horizontal filters for AF statistics.  */
typedef struct {
    td_bool          narrow_band_en;             /* RW; Range: [0, 1]; IIR narrow band enable. */
    td_bool          iir_en[OT_ISP_IIR_EN_NUM];  /* RW; Range: [0, 1]; IIR enable. */
    td_u8            iir_shift;                  /* RW; Range: [0, 63]; IIR Shift */
    td_s16           iir_gain[OT_ISP_IIR_GAIN_NUM];  /* RW; Range: [-511, 511]. IIR gain,gain0 range:[0,255]. */
    td_u16           iir_shift_lut[OT_ISP_IIR_SHIFT_NUM]; /* RW; Range: [0x0, 0x7];  IIR shift. */
    ot_isp_af_level_depend  level_depend;                 /* RW; filter level depend. */
    ot_isp_af_coring        coring;                       /* RW; filter coring. */
} ot_isp_af_h_param;

#define OT_ISP_FIR_GAIN_NUM                              5

typedef struct {
    td_s16                 fir_gain[OT_ISP_FIR_GAIN_NUM]; /* RW; Range: [-31, 31];   FIR gain. */
    ot_isp_af_level_depend level_depend;                  /* RW; filter level depend. */
    ot_isp_af_coring       coring;                        /* RW; filter coring.  */
} ot_isp_af_v_param;

#define OT_ISP_ACC_SHIFT_H_NUM                           2
#define OT_ISP_ACC_SHIFT_V_NUM                           2
typedef struct {
    td_u16 acc_shift_y;                          /* RW; Range: [0x0, 0xF]; luminance Y statistic shift. */
    td_u16 acc_shift_h[OT_ISP_ACC_SHIFT_H_NUM];  /* RW; Range: [0x0, 0xF]; IIR statistic shift. */
    td_u16 acc_shift_v[OT_ISP_ACC_SHIFT_V_NUM];  /* RW; Range: [0x0, 0xF]; FIR statistic shift. */
    td_u16 hl_cnt_shift;                         /* RW; Range: [0x0, 0xF]; High luminance counter shift */
} ot_isp_af_fv_param;

typedef struct {
    ot_isp_af_cfg        config;
    ot_isp_af_h_param    h_param_iir0;
    ot_isp_af_h_param    h_param_iir1;
    ot_isp_af_v_param    v_param_fir0;
    ot_isp_af_v_param    v_param_fir1;
    ot_isp_af_fv_param   fv_param;
} ot_isp_focus_stats_cfg;

/*
   the main purpose of stat key was to access individual statistic info separately...
   ...for achieving performance optimization of CPU, because of we acquire stat...
   ... in ISP_DRV ISR for our firmware reference and USER can also use alternative MPI...
   ... to do this job, so bit1AeStat1~bit1AfStat for MPI behavior control, and bit16IsrAccess...
   ... for ISR access control, they were independent. but they have the same bit order, for example...
   ... bit1AeStat1 for MPI AeStat1 access key, and bit16 of u32Key for ISR AeStat1 access key
 */
typedef union {
    td_u32  key;
    struct {
        td_u32  bit1_fe_ae_global_stat         : 1;   /* [0] */
        td_u32  bit1_fe_ae_local_stat          : 1;   /* [1] */
        td_u32  bit1_fe_ae_stitch_global_stat  : 1;   /* [2] */
        td_u32  bit1_fe_ae_stitch_local_stat   : 1;   /* [3] */
        td_u32  bit1_be_ae_global_stat         : 1;   /* [4] */
        td_u32  bit1_be_ae_local_stat          : 1;   /* [5] */
        td_u32  bit1_be_ae_stitch_global_stat  : 1;   /* [6] */
        td_u32  bit1_be_ae_stitch_local_stat   : 1;   /* [7] */
        td_u32  bit1_awb_stat1                 : 1;   /* [8] */
        td_u32  bit1_awb_stat2                 : 1;   /* [9] */
        td_u32  bit2_reserved0                 : 2;   /* [10:11] */
        td_u32  bit1_fe_af_stat                : 1;   /* [12] */
        td_u32  bit1_be_af_stat                : 1;   /* [13] */
        td_u32  bit2_reserved1                 : 2;   /* [14:15] */
        td_u32  bit1_dehaze                    : 1;   /* [16] */
        td_u32  bit1_mg_stat                   : 1;   /* [17] */
        td_u32  bit1_extend_stats              : 1;   /* [18] */
        td_u32  bit13_reserved                 : 13;   /* [19:31] */
    };
} ot_isp_stats_ctrl;

/* statistics structs */
#define OT_ISP_HIST_NUM 1024

typedef struct {
    td_u16 grid_y_pos[OT_ISP_AE_ZONE_ROW + 1];   /* R */
    td_u16 grid_x_pos[OT_ISP_AE_ZONE_COLUMN + 1]; /* R */
    td_u8  status;                        /* R;0:not update, 1: update,others:reserved */
} ot_isp_ae_grid_info;

typedef struct {
    td_u16 grid_y_pos[OT_ISP_MG_ZONE_ROW + 1];    /* R */
    td_u16 grid_x_pos[OT_ISP_MG_ZONE_COLUMN + 1]; /* R */
    td_u8  status;                         /* R;0:not update, 1: update,others:reserved */
} ot_isp_mg_grid_info;

typedef struct {
    td_u16 grid_y_pos[OT_ISP_AWB_ZONE_ORIG_ROW  + 1];   /* R */
    td_u16 grid_x_pos[OT_ISP_AWB_ZONE_ORIG_COLUMN + 1]; /* R */
    td_u8  status;                               /* R;0:not update, 1: update,others:reserved */
} ot_isp_awb_grid_info;

typedef struct {
    td_u16 grid_y_pos[OT_ISP_AF_ZONE_ROW + 1];    /* R */
    td_u16 grid_x_pos[OT_ISP_AF_ZONE_COLUMN + 1]; /* R */
    td_u8  status;                         /* R;0:not update, 1: update,others:reserved */
} ot_isp_focus_grid_info;

typedef struct {
    td_u32 fe_hist1024_value[OT_ISP_WDR_MAX_FRAME_NUM][OT_ISP_HIST_NUM];  /* R; Range: [0x0, 0xFFFFFFFF]; Format:32.0;
                                                             Global 1024 bins histogram of FE */
    td_u16 fe_global_avg[OT_ISP_WDR_MAX_FRAME_NUM][OT_ISP_BAYER_CHN_NUM]; /* R; Range: [0x0, 0xFFFF]; Format:16.0;
                                                                 Global average value of FE */
    td_u16 fe_zone_avg[OT_ISP_WDR_MAX_FRAME_NUM][OT_ISP_AE_ZONE_ROW][OT_ISP_AE_ZONE_COLUMN][OT_ISP_BAYER_CHN_NUM]; /* R;
                              Range: [0x0, 0xFFFF]; Format:16.0; Zone average value of FE */
    td_u32 be_estimate_hist1024_value[OT_ISP_HIST_NUM]; /* R; Range: [0x0, 0xFFFFFFFF]; Format:32.0;
                                                          Estimated Global 1024 bins histogram of BE */
    td_u32 be_hist1024_value[OT_ISP_HIST_NUM];         /* R; Range: [0x0, 0xFFFFFFFF]; Format:32.0;
                                                          Global 1024 bins histogram of BE */
    td_u16 be_global_avg[OT_ISP_BAYER_CHN_NUM];        /* R; Range: [0x0, 0xFFFF]; Format:16.0;
                                                          Global average value of BE */
    td_u16 be_zone_avg[OT_ISP_AE_ZONE_ROW][OT_ISP_AE_ZONE_COLUMN][OT_ISP_BAYER_CHN_NUM]; /* R; Range: [0x0, 0xFFFF];
                                                                               Format:16.0; Zone average value of BE */
    ot_isp_ae_grid_info fe_grid_info;
    ot_isp_ae_grid_info be_grid_info;
    td_u64 pts;
} ot_isp_ae_stats;

typedef struct {
    td_u32 fe_hist1024_value[OT_ISP_WDR_MAX_FRAME_NUM][OT_ISP_HIST_NUM];   /* R; Range: [0x0, 0xFFFFFFFF]; Format:32.0;
                                                                 Global 1024 bins histogram of FE */
    td_u16 fe_global_avg[OT_ISP_WDR_MAX_FRAME_NUM][OT_ISP_BAYER_CHN_NUM]; /* R; Range: [0x0, 0xFFFF]; Format:16.0;
                                                                 Global average value of FE. */
    td_u16 fe_zone_avg[OT_ISP_MAX_STITCH_NUM][OT_ISP_WDR_MAX_FRAME_NUM][OT_ISP_AE_ZONE_ROW][OT_ISP_AE_ZONE_COLUMN]\
    [OT_ISP_BAYER_CHN_NUM]; /* R; Range: [0x0, 0xFFFF]; Format:16.0; Zone average value of FE. */
    td_u32 be_hist1024_value[OT_ISP_HIST_NUM];          /* R; Range: [0x0, 0xFFFFFFFF]; Format:32.0;
                                                    Global 1024 bins histogram of BE. */
    td_u16 be_global_avg[OT_ISP_BAYER_CHN_NUM];  /* R; Range: [0x0, 0xFFFF]; Format:16.0; Global average value of BE */
    td_u16 be_zone_avg[OT_ISP_MAX_STITCH_NUM][OT_ISP_AE_ZONE_ROW][OT_ISP_AE_ZONE_COLUMN][OT_ISP_BAYER_CHN_NUM]; /* R;
                                                        Range: [0x0, 0xFFFF]; Format:16.0; Zone average value of BE */
    td_u64 pts;
} ot_isp_ae_stitch_stats;

typedef struct {
    td_u16 zone_avg[OT_ISP_MG_ZONE_ROW][OT_ISP_MG_ZONE_COLUMN][OT_ISP_BAYER_CHN_NUM]; /* R; Range: [0x0, 0xFF];
                                                                                      Format:8.0; Zone average value */
    ot_isp_mg_grid_info grid_info;
} ot_isp_mg_stats;

typedef enum {
    OT_ISP_AWB_AFTER_DG = 0,
    OT_ISP_AWB_AFTER_EXPANDER,
    OT_ISP_AWB_AFTER_DRC,
    OT_ISP_AWB_SWITCH_BUTT
} ot_isp_awb_switch;

/* Crops the input image of the AWB module */
typedef struct {
    td_bool enable;  /* RW; Range: [0,1];  Format:1.0;AWB crop enable */
    td_u16  x;    /* RW; Range: [0, 8192 - 20];
                     Format:13.0; AWB image crop start x, limited range:[0, image_width - zone_col * 20] */
    td_u16  y;    /* RW; Range: [0, 8192 - 15];
                     Format:13.0; AWB image crop start y,limited range:[0, image_height - zone_row * 15] */
    td_u16  width;    /* RW; Range: [20, 8192]; Format:14.0;
                     AWB image crop width, limited range:[zone_col * 20, image_width] */
    td_u16  height;    /* RW; Range: [15, 8192]; Format:14.0;
                     AWB image crop height, limited range:[zone_row * 15, image_height] */
} ot_isp_awb_crop;

/* Defines the AWB statistics configuration */
typedef struct {
    ot_isp_awb_switch awb_switch; /* RW; Range: [0x0, 0x2]; Position of AWB statistics in pipeline */
    td_u16 zone_row;           /* RW; Range: [0x1, 0x20]; Vertical Blocks,
                                  limited range:[ceil(image_height / 1024), min(32, image_height / 15)] */
    td_u16 zone_col;           /* RW; Range: [0x1, 0x20]; Horizontal Blocks,
                                  limited range:[max(BlkNum, ceil(width / 1024)), min(32, image_width / 20)] */
    td_u16 white_level;        /* RW; Range: [0x0, 0xFFFF];Upper limit of valid data for white region,
                                  for Bayer statistics, [0x0, 0x3FF] for RGB statistics */
    td_u16 black_level;        /* RW; Range: [0x0, 0xFFFF];limited range: [0x0, u16WhiteLevel],
                                  Lower limit of valid data for white region .
                                  for Bayer statistics, bitwidth is 12, for RGB statistics, bitwidth is 10 */
    td_u16 cb_max;             /* RW; Range: [0x0, 0xFFF];Maximum value of B/G for white region */
    td_u16 cb_min;             /* RW; Range: [0x0, 0xFFF];
                                  limited range: [0x0, u16CbMax]Minimum value of B/G for white region */
    td_u16 cr_max;             /* RW; Range: [0x0, 0xFFF];Maximum value of R/G for white region */
    td_u16 cr_min;             /* RW; Range: [0x0, 0xFFF];
                                  limited range: [0x0, u16CrMax],Minimum value of R/G for white region */
    ot_isp_awb_crop crop;
} ot_isp_wb_stats_cfg;

typedef struct {
    td_u16  zone_row;                            /* R; Range: [0x0, 0x20];
                                                    effective horizontal block number for AWB statistic stitch window */
    td_u16  zone_col;                            /* R; Range: [0x0, 0x80];
                                                    effective vetical block number for AWB statistic stitch window */
    td_u16  zone_avg_r[OT_ISP_AWB_ZONE_STITCH_MAX];     /* R; Range: [0x0, 0xFFFF];Zone Average R  for Stitich mode */
    td_u16  zone_avg_g[OT_ISP_AWB_ZONE_STITCH_MAX];     /* R; Range: [0x0, 0xFFFF];Zone Average G  for Stitich mode */
    td_u16  zone_avg_b[OT_ISP_AWB_ZONE_STITCH_MAX];     /* R; Range: [0x0, 0xFFFF];Zone Average B  for Stitich mode */
    td_u16  zone_count_all[OT_ISP_AWB_ZONE_STITCH_MAX]; /* R; Range: [0x0, 0xFFFF];
                                                    normalized number of Gray points  for Stitich mode */
    td_u64 pts;
} ot_isp_wb_stitch_stats;

typedef struct {
    td_u16 global_r;          /* R; Range: [0x0, 0xFFFF];Global WB output Average R */
    td_u16 global_g;          /* R; Range: [0x0, 0xFFFF];Global WB output Average G */
    td_u16 global_b;          /* R; Range: [0x0, 0xFFFF];Global WB output Average B */
    td_u16 count_all;         /* R; Range: [0x0, 0xFFFF];normalized number of Gray points */

    td_u16 zone_avg_r[OT_ISP_AWB_ZONE_NUM];            /* R; Range: [0x0, 0xFFFF];Zone Average R */
    td_u16 zone_avg_g[OT_ISP_AWB_ZONE_NUM];            /* R; Range: [0x0, 0xFFFF];Zone Average G */
    td_u16 zone_avg_b[OT_ISP_AWB_ZONE_NUM];            /* R; Range: [0x0, 0xFFFF];Zone Average B */
    td_u16 zone_count_all[OT_ISP_AWB_ZONE_NUM];        /* R; Range: [0x0, 0xFFFF];normalized number of Gray points */
    ot_isp_awb_grid_info grid_info;
    td_u64 pts;
} ot_isp_wb_stats;

typedef struct {
    td_u16  v1;            /* R; Range: [0x0, 0xFFFF];vertical fir block1 Statistics */
    td_u16  h1;            /* R; Range: [0x0, 0xFFFF];horizontal iir block1 Statistics */
    td_u16  v2;            /* R; Range: [0x0, 0xFFFF];vertical fir block2 Statistics */
    td_u16  h2;            /* R; Range: [0x0, 0xFFFF];horizontal iir block2 Statistics */
    td_u16  y;             /* R; Range: [0x0, 0xFFFF];Y Statistics */
    td_u16  hl_cnt;        /* R; Range: [0x0, 0xFFFF];HlCnt Statistics */
} ot_isp_focus_zone;

typedef struct {
    ot_isp_focus_zone zone_metrics[OT_ISP_WDR_MAX_FRAME_NUM][OT_ISP_AF_ZONE_ROW][OT_ISP_AF_ZONE_COLUMN]; /* R;
                                                                                       The zoned measure of contrast */
} ot_isp_fe_focus_stats;

typedef struct {
    ot_isp_focus_zone zone_metrics[OT_ISP_AF_ZONE_ROW][OT_ISP_AF_ZONE_COLUMN]; /* R; The zoned measure of contrast */
} ot_isp_be_focus_stats;

typedef struct {
    ot_isp_fe_focus_stats  fe_af_stat;
    ot_isp_be_focus_stats  be_af_stat;
    ot_isp_focus_grid_info fe_af_grid_info;
    ot_isp_focus_grid_info be_af_grid_info;
    td_u64 pts;
} ot_isp_af_stats;

typedef struct {
    td_bool enable;
    ot_rect roi_rect;
} ot_isp_fe_roi_cfg;

typedef struct {
    ot_isp_stats_ctrl      ctrl;
    ot_isp_stats_ctrl      update;
    ot_isp_ae_stats_cfg    ae_cfg;
    ot_isp_wb_stats_cfg    wb_cfg;
    ot_isp_focus_stats_cfg focus_cfg;
} ot_isp_stats_cfg;

typedef enum {
    OT_ISP_DEBUG_AE = 0,
    OT_ISP_DEBUG_AWB,
    OT_ISP_DEBUG_BLC,
    OT_ISP_DEBUG_DEHAZE,
    OT_ISP_DEBUG_BUTT,
} ot_isp_debug_type;

/* ISP debug information */
typedef struct {
    ot_isp_debug_type debug_type; /* RW; type of debug info */
    td_bool debug_en;     /* RW; 1:enable debug, 0:disable debug */
    td_u64  phys_addr;     /* RW; physics address of debug info */
    td_u32  depth;        /* RW; depth of debug info */
} ot_isp_debug_info;

typedef struct {
    td_u32  reserved;         /* H;need to add member */
} ot_isp_debug_attr;

typedef struct {
    td_u32  frame_num_begain;
    td_u16  black_level_actual[OT_ISP_WDR_MAX_FRAME_NUM][OT_ISP_BAYER_CHN_NUM];    /* R; BLC actual value  */
    td_u16  black_level_original[OT_ISP_WDR_MAX_FRAME_NUM][OT_ISP_BAYER_CHN_NUM];  /* R; BLC logic value  */
    td_u32  frame_num_end;
} ot_isp_debug_status;

/*
0 = Communication between the sensor and the ISP over the I2C interface
1 = Communication between the sensor and the ISP over the SSP interface
*/
typedef enum {
    OT_ISP_SNS_TYPE_I2C = 0,
    OT_ISP_SNS_TYPE_SSP,

    OT_ISP_SNS_TYPE_BUTT,
} ot_isp_sns_type;

/* sensor communication bus */
typedef union {
    td_s8   i2c_dev;       /* AUTO:ot_isp_sns_type:OT_ISP_SNS_TYPE_I2C */
    struct {
        td_s8  bit4_ssp_dev       : 4;
        td_s8  bit4_ssp_cs        : 4;
    } ssp_dev;             /* AUTO:ot_isp_sns_type:OT_ISP_SNS_TYPE_SSP */
} ot_isp_sns_commbus;

typedef struct {
    td_bool update;        /* RW; Range: [0x0, 0x1]; Format:1.0;
                              TD_TRUE: The sensor registers are written,
                              TD_FALSE: The sensor registers are not written */
    td_u8   delay_frame_num; /* RW; Number of delayed frames for the sensor register */
    td_u8   interrupt_pos;       /* RW;Position where the configuration of the sensor register takes effect */
                                /* 0x0,very short frame start interrupt, 0x1:very short frame end interrupt,
                                   0x10,short frame start interrupt, 0x11:short frame end interrupt,
                                   0x20,middle frame start interrupt, 0x21:middle frame end interrupt,
                                   0x30,long frame start interrupt, 0x31:long frame end interrupt            */
    td_u8   dev_addr;      /* RW;Sensor device address */
    td_u32  reg_addr;      /* RW;Sensor register address */
    td_u32  addr_byte_num; /* RW;Bit width of the sensor register address */
    td_u32  data;          /* RW;Sensor register data */
    td_u32  data_byte_num; /* RW;Bit width of sensor register data */
} ot_isp_i2c_data;

typedef struct {
    td_bool update;             /* RW; Range: [0x0, 0x1]; Format:1.0;
                                   TD_TRUE: The sensor registers are written,
                                   TD_FALSE: The sensor registers are not written */
    td_u8   delay_frame_num;      /* RW; Number of delayed frames for the sensor register */
    td_u8   interrupt_pos;      /* RW;Position where the configuration of the sensor register takes effect
                                 0x0,very short frame start interrupt, 0x1:very short frame end interrupt
                                 0x10,short frame start interrupt, 0x11:short frame end interrupt
                                 0x20,middle frame start interrupt, 0x21:middle frame end interrupt
                                 0x30,long frame start interrupt, 0x31:long frame end interrupt            */

    td_u32  dev_addr;           /* RW;Sensor device address */
    td_u32  dev_addr_byte_num;  /* RW;Bit width of the sensor device address */
    td_u32  reg_addr;           /* RW;Sensor register address */
    td_u32  reg_addr_byte_num;  /* RW;Bit width of the sensor register address */
    td_u32  data;               /* RW;Sensor register data */
    td_u32  data_byte_num;      /* RW;Bit width of sensor register data */
} ot_isp_ssp_data;

typedef struct {
    ot_isp_sns_type sns_type;      /* RW; Range:[0, 1]; Format: 1.0;
                                      OT_ISP_SNS_TYPE_I2C: I2C interface type between the sensor and ISP,
                                      OT_ISP_SNS_TYPE_SSP: SPI interface type between the sensor and ISP */
    td_u32  reg_num;               /* RW;Number of registers required when exposure results are written to the sensor.
                                      The member value cannot be dynamically changed */
    td_u8   cfg2_valid_delay_max;  /* RW;Maximum number of delayed frames from the time when all sensor registers are
                                      configured to the time when configurations take effect, which is used to ensure
                                      the synchronization between sensor registers and ISP registers */

    td_u32  exp_distance[OT_ISP_WDR_MAX_FRAME_NUM - 1]; /* RW; wdr sensor exposure distance,
                                                            not support for Hi3519DV500 */
    ot_isp_sns_commbus  com_bus;
    union {
        ot_isp_i2c_data i2c_data[OT_ISP_MAX_SNS_REGS];  /* AUTO: ot_isp_sns_type:OT_ISP_SNS_TYPE_I2C */
        ot_isp_ssp_data ssp_data[OT_ISP_MAX_SNS_REGS];  /* AUTO: ot_isp_sns_type:OT_ISP_SNS_TYPE_SSP */
    };

    struct {
        td_bool update;
        td_u8   delay_frame_num;
        td_u32  slave_vs_time;      /* RW;time of vsync. Unit: inck clock cycle */
        td_u32  slave_bind_dev;
    } slv_sync;

    td_bool config;
} ot_isp_sns_regs_info;

typedef enum {
    OT_ISP_VD_FE_START   = 0,
    OT_ISP_VD_FE_END,
    OT_ISP_VD_BE_END,

    OT_ISP_VD_BUTT
} ot_isp_vd_type;

/* Defines the attributes of the virtual addresses for the registers of ISP submodules */
typedef struct {
    td_void *isp_ext_reg_addr;        /* R;Start virtual address for the ISP external virtual registers */
    td_u32  isp_ext_reg_size;         /* R;Size of the ISP external virtual registers */
    td_void *ae_ext_reg_addr;         /* R;Start virtual address for the AE library module */
    td_u32  ae_ext_reg_size;          /* R;Size of the AE library module */
    td_void *awb_ext_reg_addr;        /* R;Start virtual address for the AWB library module */
    td_u32  awb_ext_reg_size;         /* R;Size of the AWB library module */
} ot_isp_reg_attr;

/* AI structs */
/*
  Defines the ISP iris type
  0 = DC iris
  1 = P iris
 */
typedef enum {
    OT_ISP_IRIS_TYPE_DC = 0,
    OT_ISP_IRIS_TYPE_P,

    OT_ISP_IRIS_TYPE_BUTT,
} ot_isp_iris_type;

/* Defines the F value of the ISP iris */
typedef enum {
    OT_ISP_IRIS_F_NO_32_0 = 0,
    OT_ISP_IRIS_F_NO_22_0,
    OT_ISP_IRIS_F_NO_16_0,
    OT_ISP_IRIS_F_NO_11_0,
    OT_ISP_IRIS_F_NO_8_0,
    OT_ISP_IRIS_F_NO_5_6,
    OT_ISP_IRIS_F_NO_4_0,
    OT_ISP_IRIS_F_NO_2_8,
    OT_ISP_IRIS_F_NO_2_0,
    OT_ISP_IRIS_F_NO_1_4,
    OT_ISP_IRIS_F_NO_1_0,

    OT_ISP_IRIS_F_NO_BUTT,
} ot_isp_iris_f_no;

typedef struct {
    td_s32 kp;             /* RW; Range:[0, 100000]; Format:32.0; the proportional gain of PID algorithm,
                              default value is 7000 */
    td_s32 ki;             /* RW; Range:[0, 1000]; Format:32.0; the integral gain of PID algorithm,
                              default value is 100 */
    td_s32 kd;             /* RW; Range:[0, 100000]; Format:32.0; the derivative gain of PID algorithm,
                              default value is 3000 */
    td_u32 min_pwm_duty;   /* RW; Range:[0, 1000]; Format:32.0; which is the min pwm duty for dciris control */
    td_u32 max_pwm_duty;   /* RW; Range:[0, 1000]; Format:32.0; which is the max pwm duty for dciris control */
    td_u32 open_pwm_duty;  /* RW; Range:[0, 1000]; Format:32.0; which is the open pwm duty for dciris control */
} ot_isp_dciris_attr;

typedef struct {
    td_bool step_fno_table_change;    /* W; Range:[0, 1]; Format:1.0; Step-F number mapping table change or not */
    td_bool zero_is_max;              /* RW; Range:[0, 1]; Format:1.0; Step 0 corresponds to max aperture or not,
                                         it's related to the specific iris */
    td_u16  total_step;               /* RW; Range:[1, 1024]; Format:16.0; Total steps of  Piris's aperture,
                                         it's related to the specific iris */
    td_u16  step_count;               /* RW; Range:[1, 1024]; Format:16.0; Used steps of Piris's aperture.
                                         when Piris's aperture is too small, the F number precision is not enough,
                                         so not all the steps can be used. It's related to the specific iris */
    td_u16 step_fno_table[OT_ISP_AI_MAX_STEP_FNO_NUM]; /* RW; Range:[1, 1024]; Format:16.0; Step-F number mapping table.
                                                    F1.0 is expressed as 1024, F32 is expressed as 1,
                                                    it's related to the specific iris */
    ot_isp_iris_f_no  max_iris_fno_target;  /* RW; Range:[0, 10]; Format:4.0;
                                               Max F number of Piris's aperture, it's related to the specific iris */
    ot_isp_iris_f_no  min_iris_fno_target;  /* RW; Range:[0, 10]; Format:4.0;
                                               Min F number of Piris's aperture, it's related to the specific iris */

    td_bool fno_ex_valid;           /* RW; Range:[0, 1]; Format:1.0; use equivalent gain to present FNO or not */
    td_u32  max_iris_fno_target_linear;  /* RW; Range:[1, 1024]; Format:16.0;
                                            Max equivalent gain of F number of Piris's aperture,
                                            only used when bFNOExValid is true, it's related to the specific iris */
    td_u32  min_iris_fno_target_linear;  /* RW; Range:[1, 1024]; Format:16.0;
                                            Min equivalent gain of F number of Piris's aperture,
                                            only used when bFNOExValid is true, it's related to the specific iris */
} ot_isp_piris_attr;

/* Defines the MI attribute */
typedef struct {
    td_u32  hold_value;           /* RW; Range:[0, 1000]; Format:32.0; iris hold value for DC-iris */
    ot_isp_iris_f_no  iris_fno;   /* RW; Range:[0, 10]; Format:4.0; the F number of P-iris */
} ot_isp_mi_attr;

/*
  Defines the ISP iris status
  0 = In this mode can set the MI holdvalue
  1 = Open Iris to the max
  2 = Close Iris to the min
 */
typedef enum {
    OT_ISP_IRIS_KEEP  = 0,
    OT_ISP_IRIS_OPEN  = 1,
    OT_ISP_IRIS_CLOSE = 2,
    OT_ISP_IRIS_BUTT
} ot_isp_iris_status;

/* Defines the iris attribute */
typedef struct {
    td_bool enable;                    /* RW;iris enable/disable */
    ot_op_mode         op_type;        /* auto iris or manual iris */
    ot_isp_iris_type   iris_type;      /* DC-iris or P-iris */
    ot_isp_iris_status iris_status;    /* RW; status of Iris */
    ot_isp_mi_attr     mi_attr;
} ot_isp_iris_attr;

/* AE structs */
/*
  Defines the AE mode
  0 = Automatic frame rate reduction mode (slow shutter mode)
  1 = Constant frame rate mode
 */
typedef enum {
    OT_ISP_AE_MODE_SLOW_SHUTTER = 0,
    OT_ISP_AE_MODE_FIX_FRAME_RATE  = 1,
    OT_ISP_AE_MODE_BUTT
} ot_isp_ae_mode;
/*
  Defines the AE exposure policy mode
  0 = Highlight first exposure mode
  1 = Lowlight first exposure mode
 */
typedef enum {
    OT_ISP_AE_EXP_HIGHLIGHT_PRIOR = 0,
    OT_ISP_AE_EXP_LOWLIGHT_PRIOR  = 1,
    OT_ISP_AE_STRATEGY_MODE_BUTT
} ot_isp_ae_strategy;
/* Defines the maximum exposure time or gain and minimum exposure time or gain */
typedef struct {
    td_u32 max;  /* RW;Range:[0,0xFFFFFFFF];Format:32.0;Maximum value */
    td_u32 min;  /* RW;Range:[0,0xFFFFFFFF];Format:32.0;limited Range:[0,u32Max],Minimum value */
} ot_isp_ae_range;

/* Defines the ISP exposure delay attribute */
typedef struct {
    td_u16 black_delay_frame;    /* RW; Range:[0, 65535]; Format:16.0; AE black delay frame count */
    td_u16 white_delay_frame;    /* RW; Range:[0, 65535]; Format:16.0; AE white delay frame count */
} ot_isp_ae_delay;

/*
  Defines the anti-flicker mode.
  0 = The epxosure time is fixed to be the multiplies of 1/(2*frequency) sec,
    it may lead to over exposure in the high-luminance environments.
  1 = The anti flicker may be closed to avoid over exposure in the high-luminance environments.
 */
typedef enum {
    OT_ISP_ANTIFLICKER_NORMAL_MODE = 0x0,
    OT_ISP_ANTIFLICKER_AUTO_MODE   = 0x1,
    OT_ISP_ANTIFLICKER_MODE_BUTT
} ot_isp_antiflicker_mode;

/* Defines the anti-flicker attribute */
typedef struct {
    td_bool enable;      /* RW; Range:[0, 1]; Format:1.0; */
    td_u8   frequency;   /* RW; Range:[0, 255]; Format:8.0;
                            usually this value is 50 or 60  which is the frequency of the AC power supply */
    ot_isp_antiflicker_mode  mode;
} ot_isp_antiflicker;

typedef struct {
    td_bool enable; /* RW; Range:[0, 1]; Format:1.0; */

    td_u8   luma_diff; /* RW; Range:[0, 100]; Format:8.0; if subflicker mode enable, current luma is less than AE
                          compensation plus LumaDiff, AE will keep min antiflicker shutter time
                          (for example: 1/100s or 1/120s) to avoid flicker. while current luma is larger than AE
                          compensation plus the LumaDiff, AE will reduce shutter time to avoid over-exposure and
                          introduce flicker in the pircture */
} ot_isp_subflicker;

typedef struct {
    /* base parameter */
    ot_isp_ae_range exp_time_range;   /* RW; Range:[0x0, 0xFFFFFFFF]; Format:32.0; sensor exposure time (unit: us),
                                         it's related to the specific sensor */
    ot_isp_ae_range a_gain_range;     /* RW; Range:[0x400, 0xFFFFFFFF]; Format:22.10;
                                         sensor analog gain (unit: times, 10bit precision), it's related to the
                                         specific sensor */
    ot_isp_ae_range d_gain_range;     /* RW; Range:[0x400, 0xFFFFFFFF]; Format:22.10;
                                         sensor digital gain (unit: times, 10bit precision), it's related to the
                                         specific sensor */
    ot_isp_ae_range ispd_gain_range;  /* RW; Range:[0x400, 0x40000]; Format:22.10;
                                         ISP digital gain (unit: times, 10bit precision), it's related to the
                                         ISP digital gain range */
    ot_isp_ae_range sys_gain_range;   /* RW; Range:[0x400, 0xFFFFFFFF]; Format:22.10;
                                         system gain (unit: times, 10bit precision), it's related to the specific sensor
                                         and ISP Dgain range */
    td_u32 gain_threshold;            /* RW; Range:[0x400, 0xFFFFFFFF]; Format:22.10;
                                         Gain threshold for slow shutter mode (unit: times, 10bit precision) */

    td_u8   speed;                   /* RW; Range:[0x0, 0xFF]; Format:8.0;
                                        AE adjust step for dark scene to bright scene switch */
    td_u16  black_speed_bias;        /* RW; Range:[0x0, 0xFFFF]; Format:16.0;
                                        AE adjust step bias for bright scene to dark scene switch */
    td_u8   tolerance;               /* RW; Range:[0x0, 0xFF]; Format:8.0; AE adjust tolerance */
    td_u8   compensation;            /* RW; Range:[0x0, 0xFF]; Format:8.0; AE compensation */
    td_u16  ev_bias;                 /* RW; Range:[0x0, 0xFFFF]; Format:16.0; AE EV bias */
    ot_isp_ae_strategy ae_strategy_mode;  /* RW; Range:[0, 1]; Format:1.0; Support Highlight prior or Lowlight prior */
    td_u16  hist_ratio_slope;       /* RW; Range:[0x0, 0xFFFF]; Format:16.0; AE hist ratio slope */
    td_u8   max_hist_offset;        /* RW; Range:[0x0, 0xFF]; Format:8.0; Max hist offset */

    ot_isp_ae_mode     ae_mode;     /* RW; Range:[0, 1]; Format:1.0; AE mode(slow shutter/fix framerate)(onvif) */
    ot_isp_antiflicker antiflicker;
    ot_isp_subflicker  subflicker;
    ot_isp_ae_delay    ae_delay_attr;

    td_bool manual_exp_value;     /* RW; Range:[0, 1]; Format:1.0; manual exposure value or not */
    td_u32  exp_value;            /* RW; Range:(0x0, 0xFFFFFFFF]; Format:32.0; manual exposure value */

    ot_isp_fswdr_mode fswdr_mode; /* RW; Range:[0, 2]; Format:2.0; FSWDR running mode */
    td_bool wdr_quick;            /* RW; Range:[0, 1]; Format:1.0; WDR use delay strategy or not;
                                     If is true, WDR AE adjust will be faster */

    td_u16  iso_cal_coef;     /* RW; Range:[0x0, 0xFFFF]; Format:8.8;
                                 The coefficient between Standard ISO and origin ISO (unit: times, 8bit precision) */
} ot_isp_ae_attr;

typedef struct {
    ot_op_mode exp_time_op_type;
    ot_op_mode a_gain_op_type;
    ot_op_mode d_gain_op_type;
    ot_op_mode ispd_gain_op_type;

    td_u32 exp_time;   /* RW; Range:[0x0, 0xFFFFFFFF]; Format:32.0;
                          sensor exposure time (unit: us), it's related to the specific sensor */
    td_u32 a_gain;     /* RW; Range:[0x400, 0xFFFFFFFF]; Format:22.10;
                          sensor analog gain (unit: times, 10bit precision), it's related to the specific sensor */
    td_u32 d_gain;     /* RW; Range:[0x400, 0xFFFFFFFF]; Format:22.10;
                          sensor digital gain (unit: times, 10bit precision), it's related to the specific sensor */
    td_u32 isp_d_gain; /* RW; Range:[0x400, 0x40000]; Format:22.10;
                          ISP digital gain(unit: times, 10bit precision), it's related to the ISP digital gain range */
} ot_isp_me_attr;

typedef struct {
    td_bool    bypass;      /* RW; Range:[0, 1]; Format:1.0; */
    ot_op_mode op_type;
    td_u8     ae_run_interval;   /* RW; Range:[0x1, 0xFF]; Format:8.0; set the AE run interval */
    td_bool   hist_stat_adjust;  /* RW; Range:[0, 1]; Format:1.0;
                                    TD_TRUE: 256 bin histogram statistic config will adjust when large red or blue
                                    area detected.
                                    TD_FALSE: 256 bin histogram statistic config will not change */
    td_bool   ae_route_ex_valid;   /* RW; Range:[0, 1]; Format:1.0; use extend AE route or not */
    ot_isp_me_attr   manual_attr;
    ot_isp_ae_attr   auto_attr;
    ot_isp_prior_frame prior_frame; /* RW; Range:[0, 1]; Format:1.0; AE prior frame */
    td_bool ae_gain_sep_cfg; /* RW; Range:[0, 1]; Format:1.0; long and short frame gain separately configure or not */
    td_bool advance_ae; /* RW; Range:[0, 1]; Format:1.0;  open advance AE or not */
} ot_isp_exposure_attr;

#define OT_ISP_AE_ROUTE_MAX_NODES                    16
typedef struct {
    td_u32  int_time;         /* RW; Range:(0x0, 0xFFFFFFFF]; Format:32.0;
                                 sensor exposure time (unit: us), it's related to the specific sensor */
    td_u32  sys_gain;         /* RW; Range:[0x400, 0xFFFFFFFF]; Format:22.10;
                                 system gain (unit: times, 10bit precision), it's related to the specific sensor and
                                 ISP Dgain range */
    ot_isp_iris_f_no iris_fno;  /* RW; Range:[0, 10]; Format:4.0;
                                   the F number of the iris's aperture, only support for Piris */
    td_u32  iris_fno_lin;       /* RW; Range:[0x1, 0x400]; Format:32.0;
                                   the equivalent gain of F number of the iris's aperture, only support for Piris */
} ot_isp_ae_route_node;

typedef struct {
    td_u32 total_num;     /* RW; Range:[0, 0x10]; Format:8.0; total node number of AE route */
    ot_isp_ae_route_node route_node[OT_ISP_AE_ROUTE_MAX_NODES];
} ot_isp_ae_route;

#define OT_ISP_AE_ROUTE_EX_MAX_NODES                 16
typedef struct {
    td_u32  int_time;     /* RW; Range:(0x0, 0xFFFFFFFF]; Format:32.0;
                             sensor exposure time (unit: us), it's related to the specific sensor */
    td_u32  a_gain;       /* RW; Range:[0x400, 0x3FFFFF]; Format:22.10;
                             sensor analog gain (unit: times, 10bit precision), it's related to the specific sensor */
    td_u32  d_gain;       /* RW; Range:[0x400, 0x3FFFFF]; Format:22.10;
                             sensor digital gain (unit: times, 10bit precision), it's related to the specific sensor */
    td_u32  isp_d_gain;   /* RW; Range:[0x400, 0x40000]; Format:22.10;
                             ISP digital gain (unit: times, 10bit precision) */
    ot_isp_iris_f_no  iris_fno;  /* RW; Range:[0, 10]; Format:4.0;
                                    the F number of the iris's aperture, only support for Piris */
    td_u32  iris_fno_lin;  /* RW; Range:[0x1, 0x400]; Format:32.0;
                              the equivalent gain of F number of the iris's aperture, only support for Piris */
} ot_isp_ae_route_ex_node;

typedef struct {
    td_u32 total_num;       /* RW; Range:[0, 0x10]; Format:8.0; total node number of extend AE route */
    ot_isp_ae_route_ex_node route_ex_node[OT_ISP_AE_ROUTE_EX_MAX_NODES];
} ot_isp_ae_route_ex;

typedef struct {
    td_u32  exp_time;                   /* R; Range:[0x0, 0xFFFFFFFF]; Format:32.0; Linear long_exp_time */
    td_u32  short_exp_time;             /* R; Range:[0x0, 0xFFFFFFFF]; Format:32.0; 2To1 WDR long_exp_time */
    td_u32  median_exp_time;            /* R; Range:[0x0, 0xFFFFFFFF]; Format:32.0; 3To1 WDR long_exp_time */
    td_u32  long_exp_time;              /* R; Range:[0x0, 0xFFFFFFFF]; Format:32.0; 4To1 WDR long_exp_time */
    td_u32  a_gain;                     /* R; Range:[0x400, 0xFFFFFFFF]; Format:22.10; */
    td_u32  d_gain;                     /* R; Range:[0x400, 0xFFFFFFFF]; Format:22.10; */
    td_u32  a_gain_sf;                  /* R; Range:[0x400, 0xFFFFFFFF]; Format:22.10; */
    td_u32  d_gain_sf;                  /* R; Range:[0x400, 0xFFFFFFFF]; Format:22.10; */
    td_u32  isp_d_gain;                 /* R; Range:[0x400, 0xFFFFFFFF]; Format:22.10; */
    td_u32  exposure;                   /* R; Range:[0x0, 0xFFFFFFFF]; Format:32.0; */
    td_bool exposure_is_max;            /* R; Range:[0, 1]; Format:1.0; */
    td_s16  hist_error;                 /* R; Range:[-32768, 32767]; Format:16.0; */
    td_u32  ae_hist1024_value[OT_ISP_HIST_NUM]; /* R; Range:[0x0, 0xFFFFFFFF]; Format:32.0;
                                            1024 bins histogram for channel 1 */

    td_u8  ave_lum;                 /* R; Range:[0x0, 0xFF]; Format:8.0; */
    td_u32 lines_per500ms;          /* R; Range:[0x0, 0xFFFFFFFF]; Format:32.0; exposure lines per 500ms */
    td_u32 piris_fno;               /* R; Range:[0x0, 0x400]; Format:32.0; */
    td_u32 fps;                     /* R; Range:[0x0, 0xFFFFFFFF]; Format:32.0; actual fps */
    td_u32 iso;                     /* R; Range:[0x64, 0xFFFFFFFF]; Format:32.0; */
    td_u32 isosf;                   /* R; Range:[0x64, 0xFFFFFFFF]; Format:32.0; */
    td_u32 iso_calibrate;           /* R; Range:[0x0, 0xFFFFFFFF]; Format:32.0; */
    td_u32 ref_exp_ratio;           /* R; Range:[0x40, 0x4000]; Format:26.6; */
    td_u16 wdr_exp_coef;            /* R; Range:[0x0, 0x400]; Format:6.10; 0x400 means 1 times */
    td_u32 first_stable_time;       /* R; Range:[0x0, 0xFFFFFFFF]; Format:32.0; AE first stable time for quick start */
    td_u32 quick_star_iso;          /* R; Range:[0x64, 0xFFFFFFFF]; Format:32.0; */
    ot_isp_ae_route ae_route;       /* R; Actual AE route */
    ot_isp_ae_route_ex ae_route_ex; /* R; Actual AE route_ex */
    ot_isp_ae_route ae_route_sf;    /* R; Actual AE route_sf */
    ot_isp_ae_route_ex ae_route_sf_ex; /* R; Actual AE route_sf_ex */
} ot_isp_exp_info;

typedef struct {
    td_u32 reg_addr;
    td_u32 reg_value;
} ot_isp_exp_param_reg;

typedef struct {
    td_u32 tar_fps;
    td_u32 tar_isp_dgain; /* 10 bit */
    ot_isp_exp_param_reg time_reg[10]; /* default 10 */
    ot_isp_exp_param_reg again_reg[10]; /* default 10 */
    ot_isp_exp_param_reg dgain_reg[10]; /*  default 10 */
} ot_isp_exp_conv_param;

typedef struct {
    td_bool enable;           /* RW; Range:[0, 1]; Format:1.0; */
    td_u8 face_tolerance;     /* RW; Range:[0, 0xFF];  Format:8.0   */
    td_u8 face_comp;          /* RW; Range:[0, 0xFF];  Format:8.0   */
    td_u8 stat_delay_num;    /* RW; Range:[0, 10];    Format:4.0   */
    td_u8 speed;              /* RW; Range:[0, 0x64];  Format:7.0   */
    td_u16 face_delay_num;    /* RW; Range:[0, 0x400]; Format:11.0  */
} ot_isp_fast_face_ae_attr;

#define OT_ISP_EXP_RATIO_NUM                             3
typedef struct {
    ot_op_mode     exp_ratio_type;   /* RW; Range:[0, 1]; Format:1.0;
                                        OT_OP_MODE_AUTO: The exp_ratio used in ISP is generated by firmware;
                                        OT_OP_MODE_MANUAL: The exp_ratio used in ISP is set by exp_ratio */
    td_u32 exp_ratio[OT_ISP_EXP_RATIO_NUM]; /* RW; Range:[0x40, 0x4000]; Format:26.6; 0x40 means 1 times.
                                        When exp_ratio_type is OT_OP_MODE_AUTO, exp_ratio is invalid.
                                        When exp_ratio_type is OT_OP_MODE_MANUAL, exp_ratio is quotient of
                                        long exposure time / short exposure time. */
    td_u32 exp_ratio_max;           /* RW; Range:[0x40, 0x4000]; Format:26.6; 0x40 means 1 times.
                                       When exp_ratio_type is OT_OP_MODE_AUTO, exp_ratio_max is max(upper limit) of
                                       exp_ratio generated by firmware.
                                       When exp_ratio_type is OT_OP_MODE_MANUAL, exp_ratio_max is invalid. */
    td_u32 exp_ratio_min;          /* RW; Range:[0x40, 0x4000]; Format:26.6; limited Range:[0x40, exp_ratio_max],
                                      0x40 means 1 times.
                                      When exp_ratio_type is OT_OP_MODE_AUTO, exp_ratio_min is min(lower limit) of
                                      exp_ratio generated by firmware.
                                      When exp_ratio_type is OT_OP_MODE_MANUAL, exp_ratio_min is invalid. */
    td_u16 tolerance;              /* RW; Range:[0x0, 0xFF]; Format:8.0;et the dynamic range tolerance.
                                      Format: unsigned 6.2-bit fixed-point. 0x4 means 1dB. */
    td_u16 speed;                  /* RW; Range:[0x0, 0xFF]; Format:8.0; exposure ratio adjust speed */
    td_u16 ratio_bias;             /* RW; Range:[0x0, 0xFFFF]; Format:16.0; exposure ratio bias */
    td_u16 high_light_target;      /* RW; Range:[0x0, 0x400]; Format:10.0; the high light target. */
    td_u16 exp_coef_min;       /* RW; Range:[0x0, 0x400]; Format:6.10; 0x400 means 1 times; min exposure coefficient */
} ot_isp_wdr_exposure_attr;

typedef struct {
    ot_op_mode  exp_hdr_lv_type;   /* RW; Range:[0, 1]; Format:1.0;
                                         OT_OP_MODE_AUTO: The ExpHDRLv used in ISP is generated by firmware;
                                         OT_OP_MODE_MANUAL: The ExpHDRLv used in ISP is set by exp_hdr_lv */
    td_u32 exp_hdr_lv;             /* RW; Range:[0x40, 0x400]; Format:26.6; 0x40 means 1 times.
                                      When exp_hdr_lv_type is OT_OP_MODE_AUTO, exp_hdr_lv is invalid.
                                      When exp_hdr_lv_type is OT_OP_MODE_MANUAL, exp_hdr_lv is the quotient of
                                      exposure / linear exposure. */
    td_u32 exp_hdr_lv_max;         /* RW; Range:[0x40, 0x400]; Format:26.6; 0x40 means 1 times.
                                      When exp_hdr_lv_type is OT_OP_MODE_AUTO, exp_hdr_lv_max is max(upper limit) of
                                      ExpHDRLv generated by firmware.
                                      When exp_hdr_lv_type is OT_OP_MODE_MANUAL, exp_hdr_lv_max is invalid. */
    td_u32 exp_hdr_lv_min;         /* RW; Range:[0x40, 0x400]; Format:26.6; limited range:[0x40, exp_hdr_lv_max],
                                      0x40 means 1 times.
                                      When exp_hdr_lv_type is OT_OP_MODE_AUTO, exp_hdr_lv_min is min(lower limit) of
                                      ExpHDRLv generated by firmware.
                                      When exp_hdr_lv_type is OT_OP_MODE_MANUAL, exp_hdr_lv_min is invalid. */
    td_u32 exp_hdr_lv_weight;      /* RW; Range:[0x0, 0x400]; Format:16.0; exposure HDR level weight. */
} ot_isp_hdr_exposure_attr;

typedef struct {
    td_bool enable;               /* RW; Range:[0, 1]; Format:1.0; smart ae enable or not */
    td_bool ir_mode;              /* RW; Range:[0, 1]; Format:1.0; smart ae IR mode or not */
    ot_op_mode   smart_exp_type;  /* RW; Range:[0, 1]; Format:1.0;
                                       OT_OP_MODE_AUTO: The ExpCoef used in ISP is  generated by firmware;
                                       OT_OP_MODE_MANUAL: The ExpCoef used in ISP is set by u32ExpCoef */
    td_u16  exp_coef;          /* RW; Range:[0x0, 0xFFFF]; Format:6.10; 0x400 means 1 times.
                                  When enExpHDRLvType is OT_OP_MODE_AUTO, u32ExpCoef is invalid.
                                  When enExpHDRLvType is OT_OP_MODE_MANUAL, u32ExpCoef is the quotient of exposure. */
    td_u8   luma_target;       /* RW; Range:[0x0, 0xFF]; Format:8.0; luma target of smart ae. */
    td_u16  exp_coef_max;      /* RW; Range:[0x0, 0xFFFF]; Format:6.10; 0x400 means 1 times.
                                  When enExpHDRLvType is OT_OP_MODE_AUTO, u32ExpCoefvMax is max(upper limit) of ExpCoef
                                  generated by firmware.
                                  When enExpHDRLvType is OT_OP_MODE_MANUAL, u32ExpCoefMax is invalid. */
    td_u16  exp_coef_min;      /* RW; Range:[0x0, 0xFFFF]; Format:6.10; 0x400 means 1 times.
                                  When enExpHDRLvType is OT_OP_MODE_AUTO, u32ExpCoefMax is min(lower limit) of ExpCoef
                                  generated by firmware.
                                  When enExpHDRLvType is OT_OP_MODE_MANUAL, u32ExpCoefMin is invalid. */
    td_u8   smart_interval;       /* RW; Range:[0x1, 0xFF]; Format:8.0; smart ae run interval */
    td_u8   smart_speed;          /* RW; Range:[0x0, 0xFF]; Format:8.0; smart ae adjust step */
    td_u16  smart_delay_num;      /* RW; Range:[0x0, 0x400]; Format:16.0; smart ae adjust delay frame num */
} ot_isp_smart_exposure_attr;

/* AWB structs */
/*
 * Defines the AWB online calibration type
 * avg_r_gain: the avg value of Rgain after online calibration
 * avg_b_gain: the avg value of Bgain after online calibration
 */
typedef struct {
    td_u16 avg_r_gain;
    td_u16 avg_b_gain;
} ot_isp_awb_calibration_gain;

/*
  Defines the AWB algorithm type
  0 = Improved gray world algorithm.
  1 = AWB algorithm that classifies the statistics and re-filters the white blocks
  2 = AWB algorithm that opt outdoor performance for dv
 */
typedef enum {
    OT_ISP_AWB_ALG_LOWCOST = 0,
    OT_ISP_AWB_ALG_ADVANCE = 1,
    OT_ISP_AWB_ALG_NATURA  = 2,
    OT_ISP_AWB_ALG_BUTT
} ot_isp_awb_alg_type;

/*
  Defines the AWB policy in the multi-illuminant scenario
  0 = Automatic saturation adjustment in the multi-illuminant scenario
  1 = Automatic ccm adjustment in the multi-illuminant scenario
 */
typedef enum {
    OT_ISP_AWB_MULTI_LS_SAT = 0,
    OT_ISP_AWB_MULTI_LS_CCM = 1,
    OT_ISP_AWB_MULTI_LS_BUTT
} ot_isp_awb_multi_ls_type;

typedef enum {
    OT_ISP_AWB_SCENE_MODE_INDOOR  = 0,
    OT_ISP_AWB_SCENE_MODE_OUTDOOR = 1,
    OT_ISP_AWB_SCENE_MODE_BUTT
} ot_isp_awb_scene_mode_status;

/* Defines the AWB gain range */
typedef struct {
    td_bool enable;
    ot_op_mode   op_type;

    td_u16 high_rg_limit; /* RW; Range:[0x0, 0xFFF]; Format:4.8;
                             in Manual Mode, user define the Max Rgain of High Color Temperature */
    td_u16 high_bg_limit; /* RW; Range:[0x0, 0xFFF]; Format:4.8;
                             in Manual Mode, user define the Min Bgain of High Color Temperature */
    td_u16 low_rg_limit;  /* RW; Range:[0x0, 0xFFF]; Format:4.8; limited range:[0x0, high_rg_limit),
                             in Manual Mode, user define the Min Rgain of Low Color Temperature */
    td_u16 low_bg_limit;  /* RW; Range:[0, 0xFFF]; Format:4.8; limited Range:(high_bg_limit, 0xFFF],
                             in Manual Mode, user define the Max Bgain of Low Color Temperature */
} ot_isp_awb_ct_limit_attr;

typedef struct {
    td_bool enable;                 /* RW; Range:[0, 1]; Format:1.0; Outdoor/Indoor scenario determination enable */
    ot_op_mode   op_type;
    ot_isp_awb_scene_mode_status scene_status; /* RW; User should config indoor or outdoor status in Manual Mode */
    td_u32 out_thresh;         /* RW;Format:32.0;shutter time(in us) to judge indoor or outdoor */
    td_u16 low_start;          /* RW;Format:16.0;5000K is recommend */
    td_u16 low_stop;           /* RW;Format:16.0;limited range:(0, low_start),
                                  4500K is recommend, should be smaller than low_start */
    td_u16 high_start;         /* RW;Format:16.0;limited range:(low_start, 0xFFFF],
                                  6500K is recommend, should be larger than low_start */
    td_u16 high_stop;          /* RW;Format:16.0;limited range:(high_start, 0xFFFF],
                                  8000K is recommend, should be larger than high_start */
    td_bool green_enhance_en;  /* RW; Range:[0, 1]; Format:1.0;If this is enabled, Green channel will be enhanced
                                  based on the ratio of green plant */
    td_u8   out_shift_limit;   /* RW; Range:[0, 0xFF]; Format:8.0;Max white point zone distance to Planckian Curve */
} ot_isp_awb_in_out_attr;

typedef struct {
    td_bool enable;                         /* RW; Range:[0, 1]; Format:1.0;
                                               If enabled, statistic parameter cr, cb will change according to iso */

    td_u16  cr_max[OT_ISP_AUTO_ISO_NUM]; /* RW; Range:[0x0, 0xFFF]; cr_max[i] >= cr_min[i] */
    td_u16  cr_min[OT_ISP_AUTO_ISO_NUM]; /* RW; Range:[0x0, 0xFFF] */
    td_u16  cb_max[OT_ISP_AUTO_ISO_NUM]; /* RW; Range:[0x0, 0xFFF]; cb_max[i] >= cb_min[i] */
    td_u16  cb_min[OT_ISP_AUTO_ISO_NUM]; /* RW; Range:[0x0, 0xFFF] */
} ot_isp_awb_cbcr_track_attr;

#define OT_ISP_AWB_LUM_HIST_NUM                          6
/* Defines the parameters of the luminance histogram statistics for white balance */
typedef struct {
    td_bool enable;                        /* RW; Range:[0, 1]; Format:1.0;
                                              If enabled, zone weight to awb is combined with zone luma */
    ot_op_mode   op_type;              /* In auto mode, the weight distribution follows Gaussian distribution */
    td_u8   hist_thresh[OT_ISP_AWB_LUM_HIST_NUM]; /* RW; Range:[0x0, 0xFF]; Format:8.0;
                                              In manual mode, user define luma thresh, thresh[0] is 0,
                                              thresh[5] is 0xFF,  thresh[i] greater or equal to  thresh[i-1] */
    td_u16  hist_wt[OT_ISP_AWB_LUM_HIST_NUM];     /* RW; Range:[0x0, 0xFFFF]; Format:16.0;
                                              user can define luma weight in both manual and auto mode. */
} ot_isp_awb_lum_histgram_attr;

/* Defines the information about a separate illuminant */
typedef struct {
    td_u16 white_r_gain;  /* RW;Range:[0x0, 0xFFF]; Format:4.8; G/R of White points at this light source */
    td_u16 white_b_gain;  /* RW;Range:[0x0, 0xFFF]; Format:4.8; G/B of White points at this light source */
    td_u16 exp_quant;     /* RW;shutter time * again * dgain >> 4, Not support Now */
    td_u8  light_status;  /* RW;Range:[0,2]; Format:2.0; idle  1:add light source  2:delete sensitive color */
    td_u8  radius;        /* RW;Range:[0x0, 0xFF];  Format:8.0; Radius of light source, */
} ot_isp_awb_extra_light_source_info;

#define OT_ISP_AWB_LS_NUM                                4
#define OT_ISP_AWB_MULTI_CT_NUM                          8
/* extended AWB attributes */
typedef struct {
    td_u8  tolerance;        /* RW; Range:[0x0, 0xFF]; Format:8.0; AWB adjust tolerance */
    td_u8  zone_radius;      /* RW; Range:[0x0, 0xFF]; Format:8.0; radius of AWB blocks */
    td_u16 curve_l_limit;    /* RW; Range:[0x0, 0x100]; Format:9.0; Left limit of AWB Curve,
                                recommend for indoor 0xE0, outdoor 0xE0 */
    td_u16 curve_r_limit;    /* RW; Range:[0x100, 0xFFF]; Format:12.0; Right Limit of AWB Curve,
                                recommend for indoor 0x130, outdoor 0x120 */

    td_bool  extra_light_en; /* RW; Range:[0, 1]; Format:1.0; Enable special light source function */
    ot_isp_awb_extra_light_source_info  light_info[OT_ISP_AWB_LS_NUM];
    ot_isp_awb_in_out_attr              in_or_out;

    td_bool multi_light_source_en;           /* RW; Range:[0, 1]; Format:1.0; If enabled, awb will do special process
                                                in multi light source environment */
    ot_isp_awb_multi_ls_type multi_ls_type;  /* Saturation or CCM Tuning */
    td_u16  multi_ls_scaler;                 /* RW; Range:[0x0, 0x100]; Format:12.0;
                                                In saturation type, it means the max saturation it can achieve,
                                                in ccm type, it means the strength of multi process. */
    td_u16  multi_ct_bin[OT_ISP_AWB_MULTI_CT_NUM];  /* RW; Range:[0, 0XFFFF]; Format:16.0;
                                                AWB Support divide the color temperature range by 8 bins */
    td_u16  multi_ct_wt[OT_ISP_AWB_MULTI_CT_NUM];   /* RW; Range:[0x0, 0x400];Weight for different color temperature,
                                                same value of 8 means CT weight doesn't work, */

    td_bool fine_tun_en;       /* RW; Range:[0x0, 0x1]; Format:1.0;If enabled, skin color scene will be optimized */
    td_u8   fine_tun_strength; /* RW; Range:[0x0, 0xFF]; Format:8.0;
                                  larger value means better performance of skin color scene,
                                  but will increase error probability in low color temperature scene */
} ot_isp_awb_attr_ex;

#define OT_ISP_AWB_CURVE_PARA_NUM                        6
typedef struct {
    td_bool enable;               /* RW; Range:[0x0, 0x1]; Format:1.0;If AWB is disabled, static wb gain will be used,
                                     otherwise auto wb gain will be used */

    td_u16 ref_color_temp;        /* RW; Range:[0x0, 0xFFFF]; Format:16.0; Calibration Information */
    td_u16 static_wb[OT_ISP_BAYER_CHN_NUM];   /* RW; Range:[0x0, 0xFFF]; Format:12.0;Calibration Information */
    td_s32 curve_para[OT_ISP_AWB_CURVE_PARA_NUM]; /* RW; Format:32.0;Calibration Information,
                                              limited Range:as32CurvePara[3] != 0, as32CurvePara[4]==128 */

    ot_isp_awb_alg_type       alg_type;

    td_u8  rg_strength;     /* RW; Range: [0x0, 0xFF]; Format:8.0; AWB Strength of R Channel */
    td_u8  bg_strength;     /* RW; Range: [0x0, 0xFF]; Format:8.0; AWB Strength of B Channel */
    td_u16 speed;           /* RW; Range: [0x0, 0xFFF]; Format:12.0; Convergence speed of AWB  */
    td_u16 zone_sel;        /* RW; Range: [0, 255]; Format:8.0; A value of 0 or 0xFF means global AWB,
                               A value between 0 and 0xFF means zoned AWB */
    td_u16 high_color_temp; /* RW; Range: [1, 65535]; Format:16.0; AWB max temperature, Recommended: [8500, 10000] */
    td_u16 low_color_temp;  /* RW; Range: [0, 65535]; Format:16.0; Limited Range:[0, high_color_temp),
                               AWB min temperature, Recommended: [2000, 2500] */
    ot_isp_awb_ct_limit_attr ct_limit;
    td_bool shift_limit_en;  /* RW; Range: [0, 1]; Format:1.0; If enabled, when the statistic information is out of
                                range, it should be project back */
    td_u8  shift_limit;      /* RW; Range: [0x0, 0xFF];  Format:8.0; planckian curve range, Recommended: [0x30, 0x50] */
    td_bool gain_norm_en;    /* RW; Range: [0, 1]; Format:1.0; if enabled, the min of RGB gain is fixed. */
    td_bool natural_cast_en; /* RW, Range: [0, 1]; Format:1.0; if enabled, the color performance will be natural in
                                lowlight and low color temperature */

    ot_isp_awb_cbcr_track_attr cb_cr_track;
    ot_isp_awb_lum_histgram_attr luma_hist;
    td_bool awb_zone_wt_en;      /* RW, Range: [0, 1]; Format:1.0; if enabled, user can set weight for each zones */
    td_u8   zone_wt[OT_ISP_AWB_ZONE_NUM];   /* RW; Range: [0, 255]; Format:8.0;Zone Wt Table */
} ot_isp_awb_attr;

typedef struct {
    td_u16 r_gain;            /* RW; Range: [0x0, 0xFFF]; Format:4.8; Multiplier for R  color channel  */
    td_u16 gr_gain;           /* RW; Range: [0x0, 0xFFF]; Format:4.8; Multiplier for Gr color channel */
    td_u16 gb_gain;           /* RW; Range: [0x0, 0xFFF]; Format:4.8; Multiplier for Gb color channel */
    td_u16 b_gain;            /* RW; Range: [0x0, 0xFFF]; Format:4.8; Multiplier for B  color channel */
} ot_isp_mwb_attr;

typedef enum {
    OT_ISP_ALG_AWB_GW      = 0,
    OT_ISP_ALG_AWB_SPEC    = 1,
    OT_ISP_ALG_BUTT
} ot_isp_awb_alg;

typedef struct {
    td_bool bypass;              /* RW; Range: [0, 1];  Format:1.0; If enabled, awb will be bypassed */
    td_u8   awb_run_interval;    /* RW; Range: [0x1, 0xFF];  Format:8.0; set the AWB run interval */
    ot_op_mode   op_type;
    ot_isp_mwb_attr  manual_attr;
    ot_isp_awb_attr  auto_attr;
    ot_isp_awb_alg   alg_type;
} ot_isp_wb_attr;

typedef struct {
    td_bool sat_en;               /* RW; Range: [0, 1]; Format:1.0;
                                     If sat_en=1, the active CCM = SatMatrix * ManualMatrix,
                                     else the active CCM =  ManualMatrix */
    td_u16  ccm[OT_ISP_CCM_MATRIX_SIZE]; /* RW; Range: [0x0, 0xFFFF]; Format:8.8; Manul CCM matrix, */
} ot_isp_color_matrix_manual;

typedef struct {
    td_u16 color_temp;           /* RW; Range: [500, 30000]; Format:16.0; the current color temperature */
    td_u16 ccm[OT_ISP_CCM_MATRIX_SIZE]; /* RW; Range: [0x0, 0xFFFF]; Format:8.8;CCM matrixes for different
                                           color temperature */
} ot_isp_color_matrix_param;

typedef struct {
    td_bool iso_act_en;    /* RW; Range: [0, 1]; Format:1.0; if enabled, CCM will bypass in low light */
    td_bool temp_act_en;   /* RW; Range: [0, 1]; Format:1.0; if enabled, CCM will bypass when color temperature is
                              larger than 8000K or less than 2500K */
    td_u16  ccm_tab_num;   /* RW; Range: [0x3, 0x7]; Format:16.0; The number of CCM matrixes */
    ot_isp_color_matrix_param ccm_tab[OT_ISP_CCM_MATRIX_NUM];
} ot_isp_color_matrix_auto;

typedef struct {
    ot_op_mode op_type;
    ot_isp_color_matrix_manual manual_attr;
    ot_isp_color_matrix_auto   auto_attr;
} ot_isp_color_matrix_attr;

typedef struct {
    td_u8 saturation;           /* RW; Range: [0, 0xFF]; Format:8.0; set the manual saturation of CCM */
} ot_isp_saturation_manual;

typedef struct {
    td_u8 sat[OT_ISP_AUTO_ISO_NUM];           /* RW; Range: [0, 0xFF]; should be decreased with increasing ISO */
} ot_isp_saturation_auto;

typedef struct {
    ot_op_mode op_type;
    ot_isp_saturation_manual manual_attr;
    ot_isp_saturation_auto   auto_attr;
} ot_isp_saturation_attr;

typedef struct {
    td_u16 red_cast_gain;        /* RW; Range: [0x100, 0x180]; Format:4.8; R channel gain after CCM */
    td_u16 green_cast_gain;      /* RW; Range: [0x100, 0x180]; Format:4.8; G channel gain after CCM */
    td_u16 blue_cast_gain;       /* RW; Range: [0x100, 0x180]; Format:4.8; B channel gain after CCM */
} ot_isp_color_tone_attr;

typedef struct {
    td_u16 r_gain;               /* R; Range: [0x0, 0xFFF]; Format:8.8;AWB result of R color channel */
    td_u16 gr_gain;              /* R; Range: [0x0, 0xFFF]; Format:8.8; AWB result of Gr color channel */
    td_u16 gb_gain;              /* R; Range: [0x0, 0xFFF]; Format:8.8; AWB result of Gb color channel */
    td_u16 b_gain;               /* R; Range: [0x0, 0xFFF]; Format:8.8; AWB result of B color channel */
    td_u16 saturation;           /* R; Range: [0x0, 0xFF];Format:8.0;Current saturation */
    td_u16 color_temp;           /* R; Range: [0x0, 0xFFFF];Format:16.0;Detect color temperature, maybe out of color
                                    cemeprature range */
    td_u16 ccm[OT_ISP_CCM_MATRIX_SIZE]; /* R; Range: [0x0, 0xFFFF];Format:16.0;Current color correction matrix */

    td_u16 ls0_ct;               /* R; Range: [0x0, 0xFFFF];Format:16.0;color tempearture of primary light source */
    td_u16 ls1_ct;               /* R; Range: [0x0, 0xFFFF];Format:16.0;color tempearture of secondary light source */
    td_u16 ls0_area;             /* R; Range: [0x0, 0xFF];Format:8.0;area of primary light source */
    td_u16 ls1_area;             /* R; Range: [0x0, 0xFF];Format:8.0;area of secondary light source */
    td_u8  multi_degree;         /* R; Range: [0x0, 0xFF];0 means uniform light source, larger value means multi
                                    light source */
    td_u16 active_shift;         /* R; Range;[0x0,0xFF] */
    td_u32 first_stable_time;    /* R, Range: [0x0, 0xFFFFFFFF];Format:32.0;AWB first stable frame number */
    ot_isp_awb_scene_mode_status scene_status;  /* R; indoor or outdoor status */
    td_s16 bv;                                   /* R; Range;[-32768, 32767]; Bv value */
} ot_isp_wb_info;

typedef struct {
    td_bool is_ir_mode;
    td_u32 ae_comp;
    td_u32 exp_time;
    td_float int_time_accu;
    td_u32 a_gain;
    td_float again_accu;
    td_u32 d_gain;
    td_float dgain_accu;
    td_u32 ispd_gain;
    td_u32 exposure;
    td_u32 init_iso;
    td_u32 lines_per500ms;
    td_u32 piris_fno;
    td_u16 wb_r_gain;
    td_u16 wb_g_gain;
    td_u16 wb_b_gain;
    td_u16 sample_r_gain;
    td_u16 sample_b_gain;
    td_u16 init_ccm[OT_ISP_CCM_MATRIX_SIZE];
    td_bool ae_route_ex_valid;
    td_bool quick_start_en;
    ot_isp_ae_route ae_route;
    ot_isp_ae_route_ex ae_route_ex;
    ot_isp_ae_route ae_route_sf;
    ot_isp_ae_route_ex ae_route_sf_ex;
    td_u32 ae_stat_pos;
} ot_isp_init_attr;

typedef struct {
    td_s32 distance_max; /* RW;the focus range */
    td_s32 distance_min;
    td_u8  wgt[OT_ISP_AF_ZONE_ROW][OT_ISP_AF_ZONE_COLUMN]; /* RW;weighting table */
} ot_isp_af_attr;

typedef struct {
    td_s32 default_speed;     /* RW;1,default speed(unit:m/s).(onvif) */
} ot_isp_mf_attr;

typedef struct {
    ot_op_mode  op_type;
    ot_isp_mf_attr  manual_attr;
    ot_isp_af_attr  auto_attr;
} ot_isp_focus_attr;

/*
  DNG cfalayout type
  1 = Rectangular (or square) layout
  2 = Staggered layout A: even columns are offset down by 1/2 row
  3 = Staggered layout B: even columns are offset up by 1/2 row
  4 = Staggered layout C: even rows are offset right by 1/2 column
  5 = Staggered layout D: even rows are offset left by 1/2 column
  6 = Staggered layout E: even rows are offset up by 1/2 row, even columns are offset left by 1/2 column
  7 = Staggered layout F: even rows are offset up by 1/2 row, even columns are offset right by 1/2 column
  8 = Staggered layout G: even rows are offset down by 1/2 row, even columns are offset left by 1/2 column
  9 = Staggered layout H: even rows are offset down by 1/2 row, even columns are offset right by 1/2 column
 */
typedef enum {
    OT_ISP_CFALAYOUT_TYPE_RECTANGULAR = 1,
    OT_ISP_CFALAYOUT_TYPE_A,        /* a,b,c... not support */
    OT_ISP_CFALAYOUT_TYPE_B,
    OT_ISP_CFALAYOUT_TYPE_C,
    OT_ISP_CFALAYOUT_TYPE_D,
    OT_ISP_CFALAYOUT_TYPE_E,
    OT_ISP_CFALAYOUT_TYPE_F,
    OT_ISP_CFALAYOUT_TYPE_G,
    OT_ISP_CFALAYOUT_TYPE_H,
    OT_ISP_CFALAYOUT_TYPE_BUTT
} ot_isp_dng_cfa_layout_type;

typedef struct {
    td_s32 numerator;   /* represents the numerator of a fraction, */
    td_s32 denominator; /* the denominator. */
} ot_isp_dng_srational;

typedef struct {
    td_u16 repeat_row;
    td_u16 repeat_col;
} ot_isp_dng_black_level_repeat_dim;

typedef struct {
    ot_isp_dng_srational default_scale_hor;
    ot_isp_dng_srational default_scale_ver;
} ot_isp_dng_default_scale;

typedef struct {
    td_u16 repeat_pattern_dim_row;
    td_u16 repeat_pattern_dim_col;
} ot_isp_dng_repeat_pattern_dim;

/* Defines the structure of dng raw format. */
typedef struct {
    td_u8 bits_per_sample;                      /* RO;Format:8.0; Indicate the bit numbers of raw data */
    td_u8 cfa_plane_color[OT_CFACOLORPLANE];       /* RO;Format:8.0; Indicate the planer numbers of raw data;
                                                   0:red 1:green 2: blue */
    ot_isp_dng_cfa_layout_type cfa_layout;      /* RO;Range:[1,9]; Describes the spatial layout of the CFA */
    ot_isp_dng_black_level_repeat_dim black_level_repeat_dim;   /* Specifies repeat pattern size for the BlackLevel */
    td_u32 white_level;                         /* RO;Format:32.0; Indicate the WhiteLevel of the raw data */
    ot_isp_dng_default_scale default_scale;     /* Specifies the default scale factors for each direction to convert
                                                   the image to square pixels */
    ot_isp_dng_repeat_pattern_dim cfa_repeat_pattern_dim; /* Specifies the pixel number of repeat color planer in
                                                             each direction */
    td_u8 cfa_pattern[OT_ISP_BAYER_CHN_NUM];        /* RO;Format:8.0; Indicate the bayer start order;
                                                       0:red 1:green 2: blue */
} ot_isp_dng_raw_format;

/* Defines the structure of dng image static information. read only */
typedef struct {
    ot_isp_dng_raw_format dng_raw_format;
    ot_isp_dng_srational color_matrix1[OT_ISP_CCM_MATRIX_SIZE];  /* defines a transformation matrix that converts XYZ
                                                                  values to reference camera native color space values,
                                                                  under the first calibration illuminant. */
    ot_isp_dng_srational color_matrix2[OT_ISP_CCM_MATRIX_SIZE];  /* defines a transformation matrix that converts XYZ
                                                                  values to reference camera native color space values,
                                                                  under the second calibration illuminant. */
    ot_isp_dng_srational camera_calibration1[OT_ISP_CCM_MATRIX_SIZE]; /* defines a calibration matrix that transforms
                                                                         reference camera native space values to
                                                                         individual camera native space values under
                                                                         the first calibration illuminant */
    ot_isp_dng_srational camera_calibration2[OT_ISP_CCM_MATRIX_SIZE]; /* defines a calibration matrix that transforms
                                                                         reference camera native space values to
                                                                         individual camera native space values under
                                                                         the second calibration illuminant */
    ot_isp_dng_srational forwad_matrix1[OT_ISP_CCM_MATRIX_SIZE];  /* defines a matrix that maps white balanced camera
                                                                  colors to XYZ D50 colors */
    ot_isp_dng_srational forwad_matrix2[OT_ISP_CCM_MATRIX_SIZE];  /* defines a matrix that maps white balanced camera
                                                                  colors to XYZ D50 colors */

    td_u8  calibration_illuminant1;      /* RO;Format:8.0;Light source, actually this means white balance setting.
                                            '0' means unknown, '1' daylight, '2' fluorescent, '3' tungsten, '10' flash,
                                            '17' standard light A, '18' standard light B, '19' standard light C,
                                            '20' D55, '21' D65, '22' D75, '255' other */
    td_u8  calibration_illuminant2;      /* RO;Format:8.0;Light source, actually this means white balance setting.
                                            '0' means unknown, '1' daylight, '2' fluorescent, '3' tungsten, '10' flash,
                                            '17' standard light A, '18' standard light B, '19' standard light C,
                                            '20' D55, '21' D65, '22' D75, '255' other */
} ot_isp_dng_image_static_info;

/* Defines the structure of DNG WB gain used for calculate DNG colormatrix. */
typedef struct {
    td_u16 r_gain;            /* RW;Range: [0x0, 0xFFF]; Multiplier for R  color channel */
    td_u16 g_gain;            /* RW;Range: [0x0, 0xFFF]; Multiplier for G  color channel */
    td_u16 b_gain;            /* RW;Range: [0x0, 0xFFF]; Multiplier for B  color channel */
} ot_isp_dng_wb_gain;

typedef struct {
    td_u16 color_temp;              /* RW;  range:[2000,10000]; format:16.0; the current color temperature */
    td_u16 ccm[OT_ISP_CCM_MATRIX_SIZE];    /* RW;  range: [0x0, 0xFFFF]; format:8.8;
                                       CCM matrixes for different color temperature */
} ot_isp_dng_ccm_tab;

/* Defines the structure of DNG color parameters. */
typedef struct {
    ot_isp_dng_wb_gain wb_gain1; /* the calibration White balance gain of colorcheker in low colortemper light source */
    ot_isp_dng_wb_gain wb_gain2; /* the calibration White balance gain of colorcheker in high colortemper
                                    light source */
    ot_isp_dng_ccm_tab ccm_tab1; /* the calibration CCM in low colortemper light source */
    ot_isp_dng_ccm_tab ccm_tab2; /* the calibration CCM in high colortemper light source */
} ot_isp_dng_color_param;

typedef enum {
    OT_ISP_IR_STATUS_NORMAL = 0,
    OT_ISP_IR_STATUS_IR     = 1,
    OT_ISP_IR_BUTT
} ot_isp_ir_status;

typedef enum {
    OT_ISP_IR_SWITCH_NONE      = 0,
    OT_ISP_IR_SWITCH_TO_NORMAL = 1,
    OT_ISP_IR_SWITCH_TO_IR     = 2,
    OT_ISP_IR_SWITCH_BUTT
} ot_isp_ir_switch_status;

typedef struct {
    td_bool enable;            /* RW, TD_TRUE: enable IR_auto function;  TD_TRUE: disable IR_auto function. */
    td_u32  normal_to_ir_iso_threshold; /* RW, Range: [0, 0xFFFFFFFF].
                                           ISO threshold of switching from normal to IR mode. */
    td_u32  ir_to_normal_iso_threshold; /* RW, Range: [0, 0xFFFFFFFF].
                                           ISO threshold of switching from IR to normal mode. */
    td_u32  rg_max;           /* RW, Range: [0x0, 0xFFF].    Maximum value of R/G in IR scene, 4.8-bit fix-point. */
    td_u32  rg_min;           /* RW, Range: [0x0, u32RGMax]. Minimum value of R/G in IR scene, 4.8-bit fix-point. */
    td_u32  bg_max;           /* RW, Range: [0x0, 0xFFF].    Maximum value of B/G in IR scene, 4.8-bit fix-point. */
    td_u32  bg_min;           /* RW, Range: [0x0, u32BGMax]. Minimum value of B/G in IR scene, 4.8-bit fix-point. */

    ot_isp_ir_status ir_status; /* RW. Current IR status. */
    ot_isp_ir_switch_status ir_switch; /* RO, IR switch status. */
} ot_isp_ir_auto_attr;

typedef enum {
    OT_ISP_FLICKER_TYPE_NONE = 0,
    OT_ISP_FLICKER_TYPE_UNKNOW,
    OT_ISP_FLICKER_TYPE_50HZ,
    OT_ISP_FLICKER_TYPE_60HZ,
    OT_ISP_FLICKER_TYPE_BUTT,
} ot_isp_calc_flicker_type;

typedef struct  {
    td_u32 lines_per_second;      /* The total line number of 1 second */
} ot_isp_calc_flicker_input;

typedef struct {
    ot_isp_calc_flicker_type flicker_type;  /* The calculate result of flicker type */
} ot_isp_calc_flicker_output;

#define OT_BAYER_CALIBRATION_PARA_NUM_NEW 16
typedef struct {
    td_double calibration_coef[OT_BAYER_CALIBRATION_PARA_NUM_NEW];
} ot_isp_noise_calibration;

typedef union {
    td_u32  key;
    struct {
        td_u32  bit1_ae         : 1;   /* [0] */
        td_u32  bit1_awb        : 1;   /* [1] */
        td_u32  bit30_reserved  : 30;   /* [2:31] */
    };
} ot_isp_detail_stats_ctrl;

typedef struct {
    td_bool enable;
    ot_isp_detail_stats_ctrl ctrl;
    td_u8 row; /* RW; Range: [1, 4], when col = 1, raw can't = 1, (1, 4]; Vertical Blocks */
    td_u8 col; /* RW; Range: [1, 6]; when raw = 1, col can't = 1, (1, 6]; Horizontal Blocks */
    td_u16 interval;
} ot_isp_detail_stats_cfg;

#define OT_ISP_DETAIL_STATS_MAX_ROW  4
#define OT_ISP_DETAIL_STATS_MAX_COLUMN  6
#define OT_ISP_DETAIL_STATS_BLK_NUM_MAX (OT_ISP_DETAIL_STATS_MAX_COLUMN * OT_ISP_DETAIL_STATS_MAX_ROW)

#define OT_ISP_DETAIL_AWB_ZONE_NUM (OT_ISP_AWB_ZONE_NUM * OT_ISP_DETAIL_STATS_BLK_NUM_MAX)
#define OT_ISP_DETAIL_AE_ZONE_ROW  (OT_ISP_AE_ZONE_ROW * OT_ISP_DETAIL_STATS_MAX_ROW)
#define OT_ISP_DETAIL_AE_ZONE_COLUMN  (OT_ISP_AE_ZONE_COLUMN * OT_ISP_DETAIL_STATS_MAX_COLUMN)

typedef struct {
    td_u16 grid_y_pos[OT_ISP_DETAIL_AE_ZONE_ROW + 1]; /* R */
    td_u16 grid_x_pos[OT_ISP_DETAIL_AE_ZONE_COLUMN + 1]; /* R */
    td_u8 status; /* R;0:not update, 1: update,others:reserved */
} ot_isp_detail_ae_grid_info;

typedef struct {
    td_u16 grid_y_pos[OT_ISP_AWB_ZONE_ORIG_ROW * OT_ISP_DETAIL_STATS_MAX_ROW + 1]; /* R */
    td_u16 grid_x_pos[OT_ISP_AWB_ZONE_ORIG_COLUMN * OT_ISP_DETAIL_STATS_MAX_COLUMN + 1]; /* R */
    td_u8 status; /* R;0:not update, 1: update,others:reserved */
} ot_isp_detail_awb_grid_info;

typedef struct {
    td_u16 be_zone_avg[OT_ISP_DETAIL_AE_ZONE_ROW][OT_ISP_DETAIL_AE_ZONE_COLUMN][OT_ISP_BAYER_CHN_NUM]; /* R;
        Range: [0x0, 0xFFFF]; Format:16.0; Zone average value of BE detail stats */
    ot_isp_detail_ae_grid_info ae_grid_info;
} ot_isp_detail_ae_stats;

typedef struct {
    td_u16 zone_avg_r[OT_ISP_DETAIL_AWB_ZONE_NUM]; /* R; Range: [0x0, 0xFFFF];Zone Average R */
    td_u16 zone_avg_g[OT_ISP_DETAIL_AWB_ZONE_NUM]; /* R; Range: [0x0, 0xFFFF];Zone Average G */
    td_u16 zone_avg_b[OT_ISP_DETAIL_AWB_ZONE_NUM]; /* R; Range: [0x0, 0xFFFF];Zone Average B */
    td_u16 zone_count_all[OT_ISP_DETAIL_AWB_ZONE_NUM]; /* R; Range: [0x0, 0xFFFF];normalized number of Gray points */
    ot_isp_detail_awb_grid_info awb_grid_info;
} ot_isp_detail_awb_stats;

typedef struct {
    ot_isp_detail_ae_stats ae_stats;
    ot_isp_detail_awb_stats awb_stats;
    td_u64 pts;
} ot_isp_detail_stats;

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */

#endif /* OT_COMM_ISP_H */
