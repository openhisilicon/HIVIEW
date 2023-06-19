/*
  Copyright (c), 2001-2022, Shenshu Tech. Co., Ltd.
 */

#ifndef OT_COMMON_VI_H
#define OT_COMMON_VI_H

#include "ot_common.h"
#include "ot_errno.h"
#include "ot_common_video.h"
#include "ot_common_sys.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define OT_ERR_VI_INVALID_DEV_ID  OT_DEFINE_ERR(OT_ID_VI, OT_ERR_LEVEL_ERROR, OT_ERR_INVALID_DEV_ID)
#define OT_ERR_VI_INVALID_PIPE_ID OT_DEFINE_ERR(OT_ID_VI, OT_ERR_LEVEL_ERROR, OT_ERR_INVALID_PIPE_ID)
#define OT_ERR_VI_INVALID_CHN_ID  OT_DEFINE_ERR(OT_ID_VI, OT_ERR_LEVEL_ERROR, OT_ERR_INVALID_CHN_ID)
#define OT_ERR_VI_INVALID_GRP_ID  OT_DEFINE_ERR(OT_ID_VI, OT_ERR_LEVEL_ERROR, OT_ERR_INVALID_GRP_ID)
#define OT_ERR_VI_ILLEGAL_PARAM   OT_DEFINE_ERR(OT_ID_VI, OT_ERR_LEVEL_ERROR, OT_ERR_ILLEGAL_PARAM)
#define OT_ERR_VI_NULL_PTR        OT_DEFINE_ERR(OT_ID_VI, OT_ERR_LEVEL_ERROR, OT_ERR_NULL_PTR)
#define OT_ERR_VI_NOT_CFG         OT_DEFINE_ERR(OT_ID_VI, OT_ERR_LEVEL_ERROR, OT_ERR_NOT_CFG)
#define OT_ERR_VI_NOT_SUPPORT     OT_DEFINE_ERR(OT_ID_VI, OT_ERR_LEVEL_ERROR, OT_ERR_NOT_SUPPORT)
#define OT_ERR_VI_NOT_PERM        OT_DEFINE_ERR(OT_ID_VI, OT_ERR_LEVEL_ERROR, OT_ERR_NOT_PERM)
#define OT_ERR_VI_NOT_ENABLE      OT_DEFINE_ERR(OT_ID_VI, OT_ERR_LEVEL_ERROR, OT_ERR_NOT_ENABLE)
#define OT_ERR_VI_NOT_DISABLE     OT_DEFINE_ERR(OT_ID_VI, OT_ERR_LEVEL_ERROR, OT_ERR_NOT_DISABLE)
#define OT_ERR_VI_NO_MEM          OT_DEFINE_ERR(OT_ID_VI, OT_ERR_LEVEL_ERROR, OT_ERR_NO_MEM)
#define OT_ERR_VI_BUF_EMPTY       OT_DEFINE_ERR(OT_ID_VI, OT_ERR_LEVEL_ERROR, OT_ERR_BUF_EMPTY)
#define OT_ERR_VI_BUF_FULL        OT_DEFINE_ERR(OT_ID_VI, OT_ERR_LEVEL_ERROR, OT_ERR_BUF_FULL)
#define OT_ERR_VI_NOT_READY       OT_DEFINE_ERR(OT_ID_VI, OT_ERR_LEVEL_ERROR, OT_ERR_NOT_READY)
#define OT_ERR_VI_TIMEOUT         OT_DEFINE_ERR(OT_ID_VI, OT_ERR_LEVEL_ERROR, OT_ERR_TIMEOUT)
#define OT_ERR_VI_BUSY            OT_DEFINE_ERR(OT_ID_VI, OT_ERR_LEVEL_ERROR, OT_ERR_BUSY)
#define OT_ERR_VI_NOT_BINDED      OT_DEFINE_ERR(OT_ID_VI, OT_ERR_LEVEL_ERROR, OT_ERR_NOT_BINDED)
#define OT_ERR_VI_BINDED          OT_DEFINE_ERR(OT_ID_VI, OT_ERR_LEVEL_ERROR, OT_ERR_BINDED)

#define OT_VI_MAX_AD_CHN_NUM      4UL
#define OT_VI_COMPONENT_MASK_NUM  2UL

#define OT_VI_MAX_SNS_CFG_NUM 64U
#define OT_VI_MAX_SD_MUX_NUM  4U

#define VI_INVALID_FRAME_RATE     (-1)

#define OT_VI_CORRECTION_COEF_NUM 9

/* Interface mode of video input */
typedef enum {
    OT_VI_INTF_MODE_BT656 = 0,          /* BT.656 mode */
    OT_VI_INTF_MODE_BT601,              /* BT.601 mode */
    OT_VI_INTF_MODE_DC,                 /* Digital Camera mode */
    OT_VI_INTF_MODE_BT1120,             /* BT.1120 mode */
    OT_VI_INTF_MODE_MIPI,               /* MIPI RAW mode */
    OT_VI_INTF_MODE_MIPI_YUV420_NORM,   /* MIPI YUV420 mode */
    OT_VI_INTF_MODE_MIPI_YUV420_LEGACY, /* MIPI YUV420 legacy mode */
    OT_VI_INTF_MODE_MIPI_YUV422,        /* MIPI YUV422 mode */
    OT_VI_INTF_MODE_THERMO,             /* Thermo mode */

    OT_VI_INTF_MODE_BUTT
} ot_vi_intf_mode;

/* Work mode */
typedef enum {
    OT_VI_WORK_MODE_MULTIPLEX_1 = 0, /* 1 Multiplex mode */
    OT_VI_WORK_MODE_MULTIPLEX_2,     /* 2 Multiplex mode */
    OT_VI_WORK_MODE_MULTIPLEX_4,     /* 4 Multiplex mode */

    OT_VI_WORK_MODE_BUTT
} ot_vi_work_mode;

/* Whether an input picture is interlaced or progressive */
typedef enum {
    OT_VI_SCAN_PROGRESSIVE = 0, /* progressive mode */
    OT_VI_SCAN_INTERLACED,      /* interlaced mode */

    OT_VI_SCAN_BUTT
} ot_vi_scan_mode;

/* Sequence of YUV data */
typedef enum {
    OT_VI_DATA_SEQ_VUVU = 0, /*
                              * The input sequence of the second component(only contains u and v)
                              * in BT.1120 mode is VUVU
                              */

    OT_VI_DATA_SEQ_UVUV,     /*
                              * The input sequence of the second component(only contains u and v)
                              * in BT.1120 mode is UVUV
                              */

    OT_VI_DATA_SEQ_UYVY,     /* The input sequence of YUV is UYVY */
    OT_VI_DATA_SEQ_VYUY,     /* The input sequence of YUV is VYUY */
    OT_VI_DATA_SEQ_YUYV,     /* The input sequence of YUV is YUYV */
    OT_VI_DATA_SEQ_YVYU,     /* The input sequence of YUV is YVYU */

    OT_VI_DATA_SEQ_BUTT
} ot_vi_data_seq;

/* Attribute of the vertical synchronization signal */
typedef enum {
    OT_VI_VSYNC_FIELD = 0, /* Field/toggle mode: a signal reversal means a new frame or a field */
    OT_VI_VSYNC_PULSE,     /* Pulse/effective mode: a pulse or an effective signal means a new frame or a field */

    OT_VI_VSYNC_BUTT
} ot_vi_vsync;

/* Polarity of the vertical synchronization signal */
typedef enum {
    OT_VI_VSYNC_NEG_HIGH = 0, /*
                               * if VIU_VSYNC_E = VIU_VSYNC_FIELD,
                               * then the vertical synchronization signal of even field is high-level,
                               * if VIU_VSYNC_E = VIU_VSYNC_PULSE,
                               * then the vertical synchronization pulse is positive pulse.
                               */

    OT_VI_VSYNC_NEG_LOW,      /*
                               * if VIU_VSYNC_E = VIU_VSYNC_FIELD,
                               * then the vertical synchronization signal of even field is low-level,
                               * if VIU_VSYNC_E = VIU_VSYNC_PULSE,
                               * then the vertical synchronization pulse is negative pulse.
                               */

    OT_VI_VSYNC_NEG_BUTT
} ot_vi_vsync_neg;

/* Attribute of the horizontal synchronization signal */
typedef enum {
    OT_VI_HSYNC_VALID_SIG = 0, /* the horizontal synchronization is valid signal mode */
    OT_VI_HSYNC_PULSE,         /*
                                * the horizontal synchronization is pulse mode,
                                * a new pulse means the beginning of a new line.
                                */

    OT_VI_HSYNC_BUTT
} ot_vi_hsync;

/* Polarity of the horizontal synchronization signal */
typedef enum {
    OT_VI_HSYNC_NEG_HIGH = 0, /*
                               * if VI_HSYNC_E = VI_HSYNC_VALID_SIG,
                               * then the valid horizontal synchronization signal is high-level;
                               * if VI_HSYNC_E = VI_HSYNC_PULSE,
                               * then the horizontal synchronization pulse is positive pulse.
                               */

    OT_VI_HSYNC_NEG_LOW,      /*
                               * if VI_HSYNC_E = VI_HSYNC_VALID_SIG,
                               * then the valid horizontal synchronization signal is low-level;
                               * if VI_HSYNC_E = VI_HSYNC_PULSE,
                               * then the horizontal synchronization pulse is negative pulse.
                               */

    OT_VI_HSYNC_NEG_BUTT
} ot_vi_hsync_neg;

/* Attribute of the valid vertical synchronization signal */
typedef enum {
    OT_VI_VSYNC_NORM_PULSE = 0, /* the vertical synchronization is pulse mode, a pulse means a new frame or field  */
    OT_VI_VSYNC_VALID_SIG,      /*
                                 * the vertical synchronization is effective mode,
                                 * a effective signal means a new frame or field.
                                 */

    OT_VI_VSYNC_VALID_BUTT
} ot_vi_vsync_valid;

/* Polarity of the valid vertical synchronization signal */
typedef enum {
    OT_VI_VSYNC_VALID_NEG_HIGH = 0, /*
                                     * if VI_VSYNC_VALID_E = VI_VSYNC_NORM_PULSE,
                                     * a positive pulse means vertical synchronization pulse;
                                     * if VI_VSYNC_VALID_E = VI_VSYNC_VALID_SINGAL,
                                     * the valid vertical synchronization signal is high-level.
                                     */

    OT_VI_VSYNC_VALID_NEG_LOW,      /*
                                     * if VI_VSYNC_VALID_E = VI_VSYNC_NORM_PULSE,
                                     * a negative pulse means vertical synchronization pulse;
                                     * if VI_VSYNC_VALID_E = VI_VSYNC_VALID_SINGAL,
                                     * the valid vertical synchronization signal is low-level.
                                     */

    OT_VI_VSYNC_VALID_NEG_BUTT
} ot_vi_vsync_valid_neg;

/* Blank information of the input timing */
typedef struct {
    td_u32 hsync_hfb;   /* RW; Horizontal front blanking width */
    td_u32 hsync_act;   /* RW; Horizontal effective width */
    td_u32 hsync_hbb;   /* RW; Horizontal back blanking width */
    td_u32 vsync_vfb;   /* RW; Vertical front blanking height of one frame or odd-field frame picture */
    td_u32 vsync_vact;  /* RW; Vertical effective width of one frame or odd-field frame picture */
    td_u32 vsync_vbb;   /* RW; Vertical back blanking height of one frame or odd-field frame picture */

    td_u32 vsync_vbfb;  /*
                         * RW; Even-field vertical front blanking height
                         * when input mode is interlace(invalid when progressive input mode).
                         */

    td_u32 vsync_vbact; /*
                         * RW; Even-field vertical effective width
                         * when input mode is interlace(invalid when progressive input mode).
                         */

    td_u32 vsync_vbbb;  /*
                         * RW; Even-field vertical back blanking height
                         * when input mode is interlace(invalid when progressive input mode).
                         */
} ot_vi_timing_blank;

/* synchronization information about the BT.601 or DC timing */
typedef struct {
    ot_vi_vsync           vsync;
    ot_vi_vsync_neg       vsync_neg;
    ot_vi_hsync           hsync;
    ot_vi_hsync_neg       hsync_neg;
    ot_vi_vsync_valid     vsync_valid;
    ot_vi_vsync_valid_neg vsync_valid_neg;
    ot_vi_timing_blank    timing_blank;
} ot_vi_sync_cfg;

/* Input data type */
typedef enum {
    OT_VI_DATA_TYPE_RAW = 0, /* input format is raw */
    OT_VI_DATA_TYPE_YUV,     /* input format is yuv */

    OT_VI_DATA_TYPE_BUTT
} ot_vi_data_type;

/* The attributes of a VI device */
typedef struct {
    ot_vi_intf_mode      intf_mode;                                /* RW; Interface mode */
    ot_vi_work_mode      work_mode;                                /* RW; Work mode */
    td_u32               component_mask[OT_VI_COMPONENT_MASK_NUM]; /* RW; Component mask */
    ot_vi_scan_mode      scan_mode;                                /* RW; Input scanning mode */

    td_s32               ad_chn_id[OT_VI_MAX_AD_CHN_NUM];          /*
                                                                    * RW; AD channel ID. Typically,
                                                                    * the default value -1 is recommended
                                                                    */

    ot_vi_data_seq       data_seq;                                 /* RW; Input data sequence(only YUV supported) */

    ot_vi_sync_cfg       sync_cfg;                                 /*
                                                                    * RW; Sync timing,
                                                                    * it must be configured in BT.601 mode or DC mode.
                                                                    */

    ot_vi_data_type      data_type;                                /* RW; Raw or YUV input format */
    td_bool              data_reverse;                             /* RW; Data reverse */
    ot_size              in_size;                                  /* RW; Input size */
    ot_data_rate         data_rate;                                /* RW; Data rate of device */
} ot_vi_dev_attr;

typedef enum {
    OT_VI_REPHASE_MODE_NONE = 0,
    OT_VI_REPHASE_MODE_SKIP_1_2,    /* skip 1/2 */
    OT_VI_REPHASE_MODE_SKIP_1_3,    /* skip 1/3 */
    OT_VI_REPHASE_MODE_BINNING_1_2, /* binning 1/2 */
    OT_VI_REPHASE_MODE_BINNING_1_3, /* binning 1/3 */

    OT_VI_REPHASE_MODE_BUTT
} ot_vi_rephase_mode;

typedef struct {
    ot_vi_rephase_mode hor_rephase_mode;
    ot_vi_rephase_mode ver_rephase_mode;
} ot_vi_rephase_attr;

/* Attribute of bas scale */
typedef struct {
    ot_size out_size; /* RW; bayer scale size. */
} ot_vi_scale_attr;

/* Attribute of bayer scale */
typedef struct {
    td_bool            enable;
    ot_vi_scale_attr   scale_attr;
    ot_vi_rephase_attr rephase_attr;
} ot_vi_bas_attr;

typedef enum {
    OT_VI_THERMO_WORK_MODE_T0 = 0,
    OT_VI_THERMO_WORK_MODE_T1,
    OT_VI_THERMO_WORK_MODE_T2,
    OT_VI_THERMO_WORK_MODE_T3,

    OT_VI_THERMO_WORK_MODE_BUTT,
} ot_vi_thermo_work_mode;

typedef enum {
    OT_VI_SD_MUX_T0_SD0 = 0,
    OT_VI_SD_MUX_T0_SD1,
    OT_VI_SD_MUX_T1_SDA,
    OT_VI_SD_MUX_T1_FS,
    OT_VI_SD_MUX_T2_SDA0,
    OT_VI_SD_MUX_T2_SDA1,
    OT_VI_SD_MUX_T2_SDA2,
    OT_VI_SD_MUX_T2_FS,

    OT_VI_SD_MUX_BUTT
} ot_vi_sd_mux;

typedef struct {
    ot_vi_thermo_work_mode work_mode;                       /* RW; work mode. */
    ot_video_frame_info    ooc_frame_info;                  /* RW; ooc data. */
    td_u32                 cfg_num;                         /* RW; sns cfg num. */
    td_u8                  sns_cfg[OT_VI_MAX_SNS_CFG_NUM];  /* RW; sns cfg data. */
    td_u32                 frame_rate;                      /* RW; output frame rate. */
    ot_vi_sd_mux           sd_mux[OT_VI_MAX_SD_MUX_NUM];    /* RW; sd0~sd3 mux. */
} ot_vi_thermo_sns_attr;

/* Attribute of vi generate timing */
typedef struct {
    td_bool enable;     /* RW; range:[0,1];whether vi enable generate timing */
    td_u32  frame_rate; /* RW; range:(0, 240]; generate timing frame rate */
} ot_vi_dev_timing_attr;

/* Attribute of vi generate data */
typedef struct {
    td_bool enable;     /* RW; range:[0,1];whether vi enable generate data */
} ot_vi_dev_data_attr;

/* Information of pipe binded to device */
typedef struct {
    td_u32     pipe_num;                         /* RW; Range [1,OT_VI_MAX_PHYS_PIPE_NUM] */
    ot_vi_pipe pipe_id[OT_VI_MAX_PHYS_PIPE_NUM]; /* RW; Array of pipe ID */
} ot_vi_bind_pipe;

/* Attribute of WDR fusion group */
typedef struct {
    ot_wdr_mode wdr_mode;                         /* RW; WDR mode. */
    td_u32      cache_line;                       /* RW; WDR cache line. */
    ot_vi_pipe  pipe_id[OT_VI_MAX_WDR_FRAME_NUM]; /* RW; WDR fusion pipe */
    td_bool     pipe_reverse;                       /* RW; WDR pipe reverse */
} ot_vi_wdr_fusion_grp_attr;

typedef enum {
    OT_VI_PIPE_BYPASS_NONE = 0,
    OT_VI_PIPE_BYPASS_FE,
    OT_VI_PIPE_BYPASS_BE,

    OT_VI_PIPE_BYPASS_BUTT
} ot_vi_pipe_bypass_mode;

typedef enum {
    OT_VI_BIT_ALIGN_MODE_HIGH = 0,
    OT_VI_BIT_ALIGN_MODE_LOW,

    OT_VI_BIT_ALIGN_MODE_BUTT
} ot_vi_bit_align_mode;

/* The attributes of pipe */
typedef struct {
    ot_vi_pipe_bypass_mode pipe_bypass_mode; /* RW; pipe bypass mode */
    td_bool                isp_bypass;       /* RW;Range:[0, 1];ISP bypass enable */
    ot_size                size;             /* RW; pipe BE input size */
    ot_pixel_format        pixel_format;     /* RW; Pixel format */
    ot_compress_mode       compress_mode;    /* RW; Range:[0, 4];Compress mode. */
    ot_data_bit_width      bit_width;        /* RW; Range:[0, 4];Bit width */

    ot_vi_bit_align_mode   bit_align_mode;   /*
                                              * RW; pipe FE output bit align.
                                              * eg. 14bit high bit align: 0xfffc, low bit align: 0x3fff.
                                              */

    ot_frame_rate_ctrl     frame_rate_ctrl;  /* RW; Frame rate ctrl */
} ot_vi_pipe_attr;

typedef struct {
    td_bool enable; /* RW; whether dump is enable */
    td_u32  depth;  /* RW; range [0,8]; user frame queue depth */
} ot_vi_frame_dump_attr;

typedef enum {
    OT_VI_PRIVATE_DATA_MODE_BACK = 0,
    OT_VI_PRIVATE_DATA_MODE_FRONT,

    OT_VI_PRIVATE_DATA_MODE_BUTT
} ot_vi_private_data_mode;

typedef struct {
    td_bool                 enable;    /* RW; whether dump is enable */
    ot_vi_private_data_mode data_mode; /* RW; private data mode */
    td_u32                  depth;     /* RW; range [0,8]; depth */
    td_u32                  data_size; /* RW; data size, 2byte align, unit: bytes */
} ot_vi_private_data_dump_attr;

typedef struct {
    td_u32         pool_id;
    td_phys_addr_t phys_addr;
    td_u32         data_size;
    td_u64         pts;
} ot_vi_private_data_info;

typedef enum {
    OT_VI_OUT_MODE_NORM = 0,
    OT_VI_OUT_MODE_2F1_STAGGER,
    OT_VI_OUT_MODE_3F1_STAGGER,
    OT_VI_OUT_MODE_4F1_STAGGER,

    OT_VI_OUT_MODE_BUTT
} ot_vi_out_mode;

typedef struct {
    td_bool discard_pro_pic_en; /*
                                 * RW; Range:[0, 1]; When pro mode snap, whether to discard
                                 * long exposure picture in the video pipe.
                                 */

    ot_vi_out_mode out_mode;    /* RW; Support Stagger Auto mode output timing */
    ot_data_rate data_rate;     /* RW; Support pipe fe output data rate */
} ot_vi_pipe_param;

typedef enum {
    OT_VI_PIPE_FRAME_SOURCE_FE = 0, /* RW; Frame source from pipe FE */
    OT_VI_PIPE_FRAME_SOURCE_USER,   /* RW; User send frame to pipe BE */

    OT_VI_PIPE_FRAME_SOURCE_BUTT
} ot_vi_pipe_frame_source;

/* the status of pipe */
typedef struct {
    td_bool enable;         /* RO; Whether this pipe is enabled */
    td_u32  frame_rate;     /* RO; Current frame rate */
    td_u32  interrupt_cnt;  /* RO; The video frame interrupt count */
    td_u32  lost_frame_cnt; /* RO; lost frame count */
    td_u32  vb_fail_cnt;    /* RO; Video buffer malloc failure */
    ot_size size;           /* RO; Current pipe FE output size */
} ot_vi_pipe_status;

/* Information of raw data compresss param */
typedef struct {
    td_u8 compress_param[OT_VI_COMPRESS_PARAM_SIZE];
} ot_vi_compress_param;

/* the attributes of channel */
typedef struct {
    ot_size              size;            /* RW; channel out put size */
    ot_pixel_format      pixel_format;    /* RW; pixel format */
    ot_dynamic_range     dynamic_range;   /* RW; dynamic range */
    ot_video_format      video_format;    /* RW; video format */
    ot_compress_mode     compress_mode;   /* RW; 256B segment compress or no compress. */
    td_bool              mirror_en;       /* RW; mirror enable */
    td_bool              flip_en;         /* RW; flip enable */
    td_u32               depth;           /* RW; range [0,8];depth */
    ot_frame_rate_ctrl   frame_rate_ctrl; /* RW; frame rate ctrl */
} ot_vi_chn_attr;

typedef struct {
    ot_vi_chn           bind_chn;        /*
                                          * RW; range [VI_CHN0, VI_MAX_PHY_CHN_NUM);
                                          * the channel num which extend channel will bind to.
                                          */
    ot_ext_chn_src_type src_type;        /* RW; channel input source */
    ot_size             size;            /* RW; channel out put size */
    ot_pixel_format     pixel_format;    /* RW; pixel format */
    ot_video_format     video_format;    /* RW; video format */
    ot_dynamic_range    dynamic_range;   /* RW; dynamic range */
    ot_compress_mode    compress_mode;   /* RW; 256B segment compress or no compress. */
    td_u32              depth;           /* RW; range [0,8];depth */
    ot_frame_rate_ctrl  frame_rate_ctrl; /* RW; frame rate ctrl */
} ot_vi_ext_chn_attr;

/* information of chn crop */
typedef struct {
    td_bool  enable;    /* RW; Crop enable */
    ot_coord crop_mode; /* RW; Coordinate mode of the crop start point */
    ot_rect  rect;      /* RW; Crop rectangular */
} ot_vi_crop_info;

/* the status of chn */
typedef struct {
    td_bool enable;         /* RO; whether this channel is enabled */
    td_u32  frame_rate;     /* RO; current frame rate */
    td_u32  lost_frame_cnt; /* RO; lost frame count */
    td_u32  vb_fail_cnt;    /* RO; video buffer malloc failure */
    ot_size size;           /* RO; chn output size */
} ot_vi_chn_status;

typedef enum {
    OT_VI_STITCH_CFG_MODE_NORM = 0,
    OT_VI_STITCH_CFG_MODE_SYNC,
    OT_VI_STITCH_CFG_MODE_BUTT
} ot_vi_stitch_cfg_mode;

/* The attributes of stitch group */
typedef struct {
    td_bool               stitch_en;                        /* RW; Stitch enable */
    ot_vi_stitch_cfg_mode cfg_mode;                         /* RW; Stitch cfg mode */
    td_u32                max_pts_gap;                      /* RW; MAX PTS gap between frame of pipe, unit:us */
    td_u32                pipe_num;                         /* RW; Range [2, OT_VI_MAX_PHYS_PIPE_NUM] */
    ot_vi_pipe            pipe_id[OT_VI_MAX_PHYS_PIPE_NUM]; /* RW; Array of pipe ID */
} ot_vi_stitch_grp_attr;

/* module params */
typedef struct {
    td_s32 detect_err_frame;
    td_u32 drop_err_frame;
} ot_vi_mod_param;

/* gdc correction attr */
typedef struct {
    td_bool enable;
    td_s64 correction_coef[OT_VI_CORRECTION_COEF_NUM]; /* matrix for match the two */
} ot_vi_fov_correction_attr;

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* OT_COMMON_VI_H */
