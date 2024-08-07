/*
  Copyright (c), 2001-2024, Shenshu Tech. Co., Ltd.
 */

#ifndef OT_COMMON_VIDEO_H
#define OT_COMMON_VIDEO_H

#include "ot_type.h"
#include "ot_common.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define OT_ISP_BAYER_CHN           4
#define OT_DCF_DESCRIPTION_LENGTH  32
#define OT_DCF_CAPTURE_TIME_LENGTH 20
#define OT_CFACOLORPLANE           3
#define OT_DNG_NP_SIZE             6
#define OT_ISP_WB_GAIN_NUM         4
/* 3*3=9 matrix */
#define OT_ISP_CAP_CCM_NUM         9
#define OT_SRC_LENS_COEF_NUM       9
#define OT_DST_LENS_COEF_NUM       14
#define OT_ISP_WDR_MAX_FRAME_NUM   4

#define OT_GDC_COMMON_COEF_NUM     9
#define OT_GDC_MAX_STITCH_NUM      4

#define OT_NR_S_IDX_LEN            17

#define OT_FMU_MIN_PAGE_NUM        4
#define OT_FMU_MAX_Y_PAGE_NUM      512
#define OT_FMU_MAX_C_PAGE_NUM      256

#define OT_DIS_PRIVATE_NUM 9
#define OT_DIS_MOTIONDATA_NUM 9
#define OT_DIS_POINT_NUM 10

typedef enum  {
    OT_OP_MODE_AUTO   = 0,
    OT_OP_MODE_MANUAL = 1,
    OT_OP_MODE_BUTT
} ot_op_mode;

typedef enum  {
    OT_VB_SRC_COMMON  = 0,
    OT_VB_SRC_MOD  = 1,
    OT_VB_SRC_PRIVATE = 2,
    OT_VB_SRC_USER    = 3,
    OT_VB_SRC_BUTT
} ot_vb_src;

typedef enum  {
    OT_ASPECT_RATIO_NONE   = 0,        /* full screen */
    OT_ASPECT_RATIO_AUTO   = 1,        /* ratio no change, 1:1 */
    OT_ASPECT_RATIO_MANUAL = 2,        /* ratio manual set */
    OT_ASPECT_RATIO_BUTT
} ot_aspect_ratio_type;

typedef enum  {
    OT_VIDEO_FIELD_TOP         = 1,    /* even field */
    OT_VIDEO_FIELD_BOTTOM      = 2,    /* odd field */
    OT_VIDEO_FIELD_INTERLACED  = 3,    /* two interlaced fields */
    OT_VIDEO_FIELD_FRAME       = 4,    /* frame */

    OT_VIDEO_FIELD_BUTT
} ot_video_field;

typedef enum  {
    OT_VIDEO_FORMAT_LINEAR = 0,       /* nature video line */
    OT_VIDEO_FORMAT_TILE_64x16,       /* tile cell: 64pixel x 16line */
    OT_VIDEO_FORMAT_TILE_16x8,        /* tile cell: 16pixel x 8line */
    OT_VIDEO_FORMAT_TILE_32x4,        /* tile cell: 32pixel x 4ine */

    OT_VIDEO_FORMAT_BUTT
} ot_video_format;

typedef enum  {
    OT_COMPRESS_MODE_NONE = 0,      /* no compress */
    OT_COMPRESS_MODE_SEG,           /* compress unit is 256x1 bytes as a segment. */
    OT_COMPRESS_MODE_SEG_COMPACT,   /* compact compress unit is 256x1 bytes as a segment. */
    OT_COMPRESS_MODE_TILE,          /* compress unit is a tile. */
    OT_COMPRESS_MODE_LINE,          /* compress unit is the whole line. */
    OT_COMPRESS_MODE_FRAME,         /* compress unit is the whole frame. YUV for 3DNR */
    OT_COMPRESS_MODE_FMAP,          /* compress unit is the whole line, based on mapping transformation. */

    OT_COMPRESS_MODE_BUTT
} ot_compress_mode;

typedef enum  {
    OT_VIDEO_DISPLAY_MODE_PREVIEW  = 0,
    OT_VIDEO_DISPLAY_MODE_PLAYBACK = 1,

    OT_VIDEO_DISPLAY_MODE_BUTT
} ot_video_display_mode;

typedef enum  {
    OT_FRAME_FLAG_SNAP_FLASH  = 0x1 << 0,
    OT_FRAME_FLAG_SNAP_CUR    = 0x1 << 1,
    OT_FRAME_FLAG_SNAP_REF    = 0x1 << 2,
    OT_FRAME_FLAG_SNAP_END    = 0x1 << 3,
    OT_FRAME_FLAG_MIRROR      = 0x1 << 4,
    OT_FRAME_FLAG_FLIP        = 0x1 << 5,
    OT_FRAME_FLAG_DGAIN_BYPASS = 0x1 << 6,
    OT_FRAME_FLAG_OUTPUT_NONE = 0x1 << 7,
    OT_FRAME_FLAG_USER_SEND_DISCARD     = 0x1 << 8,
    OT_FRAME_FLAG_BUTT
} ot_frame_flag;

typedef enum  {
    OT_COLOR_GAMUT_BT601 = 0,
    OT_COLOR_GAMUT_BT709,
    OT_COLOR_GAMUT_BT2020,
    OT_COLOR_GAMUT_USER,
    OT_COLOR_GAMUT_BUTT
} ot_color_gamut;

/* we ONLY define picture format used, all unused will be deleted! */
typedef enum {
    OT_PIXEL_FORMAT_RGB_444 = 0,
    OT_PIXEL_FORMAT_RGB_555,
    OT_PIXEL_FORMAT_RGB_565,
    OT_PIXEL_FORMAT_RGB_888,

    OT_PIXEL_FORMAT_BGR_444,
    OT_PIXEL_FORMAT_BGR_555,
    OT_PIXEL_FORMAT_BGR_565,
    OT_PIXEL_FORMAT_BGR_888,

    OT_PIXEL_FORMAT_ARGB_1555,
    OT_PIXEL_FORMAT_ARGB_4444,
    OT_PIXEL_FORMAT_ARGB_8565,
    OT_PIXEL_FORMAT_ARGB_8888,
    OT_PIXEL_FORMAT_ARGB_2BPP,
    OT_PIXEL_FORMAT_ARGB_CLUT2,
    OT_PIXEL_FORMAT_ARGB_CLUT4,

    OT_PIXEL_FORMAT_ABGR_1555,
    OT_PIXEL_FORMAT_ABGR_4444,
    OT_PIXEL_FORMAT_ABGR_8565,
    OT_PIXEL_FORMAT_ABGR_8888,

    OT_PIXEL_FORMAT_RGB_BAYER_8BPP,
    OT_PIXEL_FORMAT_RGB_BAYER_10BPP,
    OT_PIXEL_FORMAT_RGB_BAYER_12BPP,
    OT_PIXEL_FORMAT_RGB_BAYER_14BPP,
    OT_PIXEL_FORMAT_RGB_BAYER_16BPP,

    OT_PIXEL_FORMAT_RGB_BAYER_16BPP_H10,
    OT_PIXEL_FORMAT_RGB_BAYER_16BPP_H12,
    OT_PIXEL_FORMAT_RGB_BAYER_16BPP_H14,

    OT_PIXEL_FORMAT_RGB_BAYER_16BPP_L10,
    OT_PIXEL_FORMAT_RGB_BAYER_16BPP_L12,
    OT_PIXEL_FORMAT_RGB_BAYER_16BPP_L14,

    OT_PIXEL_FORMAT_YVU_PLANAR_422,
    OT_PIXEL_FORMAT_YVU_PLANAR_420,
    OT_PIXEL_FORMAT_YVU_PLANAR_444,

    OT_PIXEL_FORMAT_YVU_SEMIPLANAR_422,
    OT_PIXEL_FORMAT_YVU_SEMIPLANAR_420,
    OT_PIXEL_FORMAT_YVU_SEMIPLANAR_444,

    OT_PIXEL_FORMAT_YUV_SEMIPLANAR_422,
    OT_PIXEL_FORMAT_YUV_SEMIPLANAR_420,
    OT_PIXEL_FORMAT_YUV_SEMIPLANAR_444,

    OT_PIXEL_FORMAT_YUYV_PACKAGE_422,
    OT_PIXEL_FORMAT_YVYU_PACKAGE_422,
    OT_PIXEL_FORMAT_UYVY_PACKAGE_422,
    OT_PIXEL_FORMAT_VYUY_PACKAGE_422,
    OT_PIXEL_FORMAT_YYUV_PACKAGE_422,
    OT_PIXEL_FORMAT_YYVU_PACKAGE_422,
    OT_PIXEL_FORMAT_UVYY_PACKAGE_422,
    OT_PIXEL_FORMAT_VUYY_PACKAGE_422,
    OT_PIXEL_FORMAT_VY1UY0_PACKAGE_422,

    OT_PIXEL_FORMAT_YUV_400,
    OT_PIXEL_FORMAT_UV_420,

    /* SVP data format */
    OT_PIXEL_FORMAT_BGR_888_PLANAR,
    OT_PIXEL_FORMAT_HSV_888_PACKAGE,
    OT_PIXEL_FORMAT_HSV_888_PLANAR,
    OT_PIXEL_FORMAT_LAB_888_PACKAGE,
    OT_PIXEL_FORMAT_LAB_888_PLANAR,
    OT_PIXEL_FORMAT_S8C1,
    OT_PIXEL_FORMAT_S8C2_PACKAGE,
    OT_PIXEL_FORMAT_S8C2_PLANAR,
    OT_PIXEL_FORMAT_S8C3_PLANAR,
    OT_PIXEL_FORMAT_S16C1,
    OT_PIXEL_FORMAT_U8C1,
    OT_PIXEL_FORMAT_U16C1,
    OT_PIXEL_FORMAT_S32C1,
    OT_PIXEL_FORMAT_U32C1,
    OT_PIXEL_FORMAT_U64C1,
    OT_PIXEL_FORMAT_S64C1,

    OT_PIXEL_FORMAT_BUTT
} ot_pixel_format;

typedef enum  {
    OT_DYNAMIC_RANGE_SDR8 = 0,
    OT_DYNAMIC_RANGE_SDR10,
    OT_DYNAMIC_RANGE_HDR10,
    OT_DYNAMIC_RANGE_HLG,
    OT_DYNAMIC_RANGE_SLF,
    OT_DYNAMIC_RANGE_XDR,
    OT_DYNAMIC_RANGE_BUTT
} ot_dynamic_range;

typedef enum  {
    OT_DATA_BIT_WIDTH_8 = 0,
    OT_DATA_BIT_WIDTH_10,
    OT_DATA_BIT_WIDTH_12,
    OT_DATA_BIT_WIDTH_14,
    OT_DATA_BIT_WIDTH_16,
    OT_DATA_BIT_WIDTH_32,
    OT_DATA_BIT_WIDTH_64,
    OT_DATA_BIT_WIDTH_BUTT
} ot_data_bit_width;

typedef struct {
    td_u32 top_width;
    td_u32 bottom_width;
    td_u32 left_width;
    td_u32 right_width;
    td_u32 color;
} ot_border;

typedef struct {
    td_s32 x;
    td_s32 y;
} ot_point;

typedef struct {
    td_u32 width;
    td_u32 height;
} ot_size;

typedef struct {
    td_s32 x;
    td_s32 y;
    td_u32 width;
    td_u32 height;
} ot_rect;

typedef enum {
    OT_COORD_ABS = 0,                          /* Absolute coordinate. */
    OT_COORD_RATIO,                            /* Ratio coordinate. */
    OT_COORD_BUTT
} ot_coord;

typedef enum {
    OT_COVER_RECT = 0,                         /* Rectangle cover. */
    OT_COVER_QUAD,                             /* Quadrangle cover. */
    OT_COVER_BUTT
} ot_cover_type;

typedef struct {
    td_bool        is_solid;                   /* RW; Solid or hollow cover. */
    td_u32         thick;                      /* RW; Thick of the hollow quadrangle. */
    ot_point       point[OT_QUAD_POINT_NUM];   /* RW; Four points of the quadrangle. */
} ot_quad_cover;

typedef struct {
    td_bool        is_solid;  /* RW; Solid or hollow cover. */
    td_u32         thick;     /* RW; Thick of the hollow rectangle. */
    ot_rect        rect;      /* RW; The area of rectangle. */
} ot_rect_cover;

typedef struct {
    ot_cover_type      type;   /* RW; Cover type. */
    union {
        ot_rect_cover  rect_attr;   /* RW; The rectangle area. AUTO:ot_cover_type:OT_COVER_RECT; */
        ot_quad_cover  quad_attr;   /* RW; The quadrangle area.  AUTO:ot_cover_type:OT_COVER_QUAD; */
    };
    td_u32             color;  /* RW; Range: [0,0xFFFFFF]; Color of cover. */
} ot_cover;

typedef enum  {
    OT_MOSAIC_BLK_SIZE_4 = 0, /* block size 4 * 4 of MOSAIC */
    OT_MOSAIC_BLK_SIZE_8,     /* block size 8 * 8 of MOSAIC */
    OT_MOSAIC_BLK_SIZE_16,    /* block size 16 * 16 of MOSAIC */
    OT_MOSAIC_BLK_SIZE_32,    /* block size 32 * 32 of MOSAIC */
    OT_MOSAIC_BLK_SIZE_64,    /* block size 64 * 64 of MOSAIC */
    OT_MOSAIC_BLK_SIZE_128,   /* block size 128 * 128 of MOSAIC */
    OT_MOSAIC_BLK_SIZE_BUTT
} ot_mosaic_blk_size;

typedef struct {
    ot_mosaic_blk_size blk_size;
    ot_rect            rect;   /* RW; The rectangle area. */
} ot_mosaic;

typedef struct {
    td_bool enable;
    ot_rect  rect;
} ot_crop_info;

typedef struct {
    td_s32  src_frame_rate;        /* RW; source frame rate */
    td_s32  dst_frame_rate;        /* RW; dest frame rate */
} ot_frame_rate_ctrl;

typedef struct {
    ot_aspect_ratio_type mode;          /* aspect ratio mode: none/auto/manual */
    td_u32               bg_color;      /* background color, RGB 888 */
    ot_rect              video_rect;    /* valid in ASPECT_RATIO_MANUAL mode */
} ot_aspect_ratio;

typedef struct {
    td_s16              offset_top;           /* top offset of show area */
    td_s16              offset_left;          /* left offset of show area */
    td_s16              offset_bottom;        /* bottom offset of show area */
    td_s16              offset_right;         /* right offset of show area */

    td_u32              max_luma;             /* for HDR */
    td_u32              min_luma;             /* for HDR */
    td_u64              private_data[OT_MAX_PRIVATE_DATA_NUM];
} ot_video_supplement_misc;

typedef struct {
    td_phys_addr_t   misc_info_phys_addr;        /* default allocated buffer */
    td_phys_addr_t   jpeg_dcf_phys_addr;
    td_phys_addr_t   isp_info_phys_addr;
    td_phys_addr_t   low_delay_phys_addr;
    td_phys_addr_t   bnr_mot_phys_addr;
    td_phys_addr_t   motion_data_phys_addr;
    td_phys_addr_t   frame_dng_phys_addr;
    td_phys_addr_t   aiisp_phys_addr;

    td_void* ATTRIBUTE misc_info_virt_addr;       /* misc info */
    td_void* ATTRIBUTE jpeg_dcf_virt_addr;        /* jpeg_dcf, used in JPEG DCF */
    td_void* ATTRIBUTE isp_info_virt_addr;        /* isp_frame_info, used in ISP debug, when get raw and send raw */
    td_void* ATTRIBUTE low_delay_virt_addr;       /* used in low delay */
    td_void* ATTRIBUTE bnr_mot_virt_addr;         /* used for 3dnr from bnr mot */
    td_void* ATTRIBUTE motion_data_virt_addr;     /* gme motion data, filter motion data, gyro data. */
    td_void* ATTRIBUTE frame_dng_virt_addr;
    td_void* ATTRIBUTE aiisp_virt_addr;           /* aiisp info */
} ot_video_supplement;

typedef struct {
    td_u32              width;
    td_u32              height;
    ot_video_field      field;
    ot_pixel_format     pixel_format;
    ot_video_format     video_format;
    ot_compress_mode    compress_mode;
    ot_dynamic_range    dynamic_range;
    ot_color_gamut      color_gamut;

    td_u32              header_stride[OT_MAX_COLOR_COMPONENT];
    td_u32              stride[OT_MAX_COLOR_COMPONENT];

    td_phys_addr_t      header_phys_addr[OT_MAX_COLOR_COMPONENT];
    td_phys_addr_t      phys_addr[OT_MAX_COLOR_COMPONENT];
    td_void* ATTRIBUTE  header_virt_addr[OT_MAX_COLOR_COMPONENT];
    td_void* ATTRIBUTE  virt_addr[OT_MAX_COLOR_COMPONENT];

    td_u32              time_ref;
    td_u64              pts;

    td_u64              user_data[OT_MAX_USER_DATA_NUM];
    td_u32              frame_flag;     /* frame_flag, can be OR operation. */
    ot_video_supplement supplement;
} ot_video_frame;

typedef struct {
    ot_video_frame video_frame;
    td_u32        pool_id;
    ot_mod_id      mod_id;
} ot_video_frame_info;

typedef struct {
    td_u32 vb_size;

    td_u32 head_stride;
    td_u32 head_size;
    td_u32 head_y_size;

    td_u32 main_stride;
    td_u32 main_size;
    td_u32 main_y_size;
} ot_vb_calc_cfg;

typedef struct {
    ot_pixel_format pixel_format;  /* bitmap's pixel format */
    td_u32 width;               /* bitmap's width */
    td_u32 height;              /* bitmap's height */
    td_void* ATTRIBUTE data;      /* address of bitmap's data */
} ot_bmp;

/* Angle of rotation */
typedef enum  {
    OT_ROTATION_0   = 0,
    OT_ROTATION_90  = 1,
    OT_ROTATION_180 = 2,
    OT_ROTATION_270 = 3,
    OT_ROTATION_BUTT
} ot_rotation;

typedef enum {
    OT_ROTATION_VIEW_TYPE_ALL      = 0, /* View all source Image,no lose */
    OT_ROTATION_VIEW_TYPE_TYPICAL  = 1, /* View from rotation Image with source size,same lose */
    OT_ROTATION_VIEW_TYPE_INSIDE   = 2, /* View with no black section,all  in dest Image */
    OT_ROTATION_VIEW_TYPE_BUTT,
} ot_rotation_view_type;

typedef struct {
    ot_rotation_view_type view_type; /* RW;range: [0, 2];rotation mode */
    td_u32    angle; /* RW;range: [0,360];rotation angle:[0,360] */
    /* RW;range: [-511, 511];horizontal offset of the image distortion center relative to image center. */
    td_s32    center_x_offset;
    /* RW;range: [-511, 511];vertical offset of the image distortion center relative to image center. */
    td_s32    center_y_offset;
    ot_size   dst_size; /* RW; range: width:[480, 8192],height:[360, 8192]; dest size of any angle rotation */
} ot_free_rotation_attr;

typedef enum {
    OT_ROTATION_ANG_FIXED = 0,  /* Fixed angle. */
    OT_ROTATION_ANG_FREE,       /* Free angle. */
    OT_ROTATION_ANG_FREE_HP,    /* Free high-precision angle. */
    OT_ROTATION_ANG_BUTT
} ot_rotation_type;

typedef struct {
    td_bool             enable;         /* RW; range: [0, 1]; Rotate enable. */
    ot_rotation_type    rotation_type;  /* RW; Rotate type. */
    union {
        /*
         * RW; Fixed rotation attribute.
         * AUTO:ot_rotation_type:OT_ROTATION_ANG_FIXED;
         */
        ot_rotation rotation_fixed;
        /*
         * RW; Free rotation attribute.
         * AUTO:ot_rotation_type:OT_ROTATION_ANG_FREE, OT_ROTATION_ANG_FREE_HP;
         */
        ot_free_rotation_attr rotation_free;
    };
} ot_rotation_attr;

typedef enum  {
    OT_FRAME_INTERRUPT_START,
    OT_FRAME_INTERRUPT_EARLY,
    OT_FRAME_INTERRUPT_EARLY_END,
    OT_FRAME_INTERRUPT_EARLY_EARLY,
    OT_FRAME_INTERRUPT_BUTT,
} ot_frame_interrupt_type;

typedef struct {
    ot_frame_interrupt_type interrupt_type;
    td_u32 early_line;
} ot_frame_interrupt_attr;

typedef enum {
    OT_DATA_RATE_X1 = 0, /* RW; output 1 pixel per clock */
    OT_DATA_RATE_X2,     /* RW; output 2 pixel per clock */

    OT_DATA_RATE_BUTT
} ot_data_rate;

typedef enum {
    OT_WDR_MODE_NONE = 0,
    OT_WDR_MODE_BUILT_IN,
    OT_WDR_MODE_QUADRA,

    OT_WDR_MODE_2To1_LINE,
    OT_WDR_MODE_2To1_FRAME,

    OT_WDR_MODE_3To1_LINE,
    OT_WDR_MODE_3To1_FRAME,

    OT_WDR_MODE_4To1_LINE,
    OT_WDR_MODE_4To1_FRAME,

    OT_WDR_MODE_BUTT,
} ot_wdr_mode;

typedef enum {
    OT_CORNER_RECT_TYPE_CORNER = 0,
    OT_CORNER_RECT_TYPE_FULL_LINE,
    OT_CORNER_RECT_TYPE_BUTT
}ot_corner_rect_type;

typedef struct {
    ot_rect      rect;
    td_u32       hor_len;
    td_u32       ver_len;
    td_u32       thick;
} ot_corner_rect;

typedef struct {
    ot_corner_rect_type    corner_rect_type;
    td_u32                 color;
} ot_corner_rect_attr;

typedef struct {
    td_u8 image_description[OT_DCF_DESCRIPTION_LENGTH];
    /* shows manufacturer of digital cameras */
    td_u8 make[OT_DCF_DESCRIPTION_LENGTH];
    /* shows model number of digital cameras */
    td_u8 model[OT_DCF_DESCRIPTION_LENGTH];
    /* shows firmware (internal software of digital cameras) version number */
    td_u8 software[OT_DCF_DESCRIPTION_LENGTH];
    /*
     * light source, actually this means white balance setting. '0' means unknown, '1' daylight,
     * '2' fluorescent, '3' tungsten, '10' flash, '17' standard light A, '18' standard light B,
     * '19' standard light C, '20' D55, '21' D65, '22' D75, '255' other.
     */
    td_u8 light_source;
    /* focal length of lens used to take image. unit is millimeter */
    td_u32 focal_length;
    /* indicates the type of scene. value '0x01' means that the image was directly photographed. */
    td_u8 scene_type;
    /*
     * indicates the use of special processing on image data, such as rendering geared to output.
     * 0 = normal process, 1 = custom process.
     */
    td_u8 custom_rendered;
    /* indicates the equivalent focal length assuming a 35mm film camera, in mm */
    td_u8 focal_length_in35mm_film;
    /* indicates the type of scene that was shot. 0 = standard, 1 = landscape, 2 = portrait, 3 = night scene. */
    td_u8 scene_capture_type;
    /*
     * indicates the degree of overall image gain adjustment. 0 = none, 1 = low gain up, 2 = high gain up,
     * 3 = low gain down, 4 = high gain down.
     */
    td_u8 gain_control;
    /*
     * indicates the direction of contrast processing applied by the camera when the image was shot.
     * 0 = normal, 1 = soft, 2 = hard.
     */
    td_u8 contrast;
    /*
     * indicates the direction of saturation processing applied by the camera when the image was shot.
     * 0 = normal, 1 = low saturation, 2 = high saturation.
     */
    td_u8 saturation;
    /*
     * indicates the direction of sharpness processing applied by the camera when the image was shot.
     * 0 = normal, 1 = soft, 2 = hard.
     */
    td_u8 sharpness;
    /*
     * exposure metering method. '0' means unknown, '1' average, '2' center weighted average, '3' spot,
     * '4' multi-spot, '5' multi-segment, '6' partial, '255' other.
     */
    td_u8 metering_mode;
} ot_isp_dcf_const_info;

typedef struct {
    /* CCD sensitivity equivalent to ag-hr film speedrate */
    td_u32 iso_speed_ratings;
    /* exposure time (reciprocal of shutter speed). */
    td_u32 exposure_time;
    /* exposure bias (compensation) value of taking picture */
    td_u32 exposure_bias_value;
    /*
     * exposure program that the camera used when image was taken. '1' means manual control,
     * '2' program normal, '3' aperture priority, '4' shutter priority, '5' program creative (slow program),
     * '6' program action(high-speed program), '7' portrait mode, '8' landscape mode.
     */
    td_u8 exposure_program;
    /* the actual F-number (F-stop) of lens when the image was taken */
    td_u32 f_number;
    /* maximum aperture value of lens. */
    td_u32 max_aperture_value;
    /*
     * indicates the exposure mode set when the image was shot.
     * 0 = auto exposure, 1 = manual exposure, 2 = auto bracket
     */
    td_u8 exposure_mode;
    /*
     * indicates the white balance mode set when the image was shot.
     * 0 = auto white balance, 1 = manual white balance.
     */
    td_u8 white_balance;
} ot_isp_dcf_update_info;

typedef struct {
    ot_isp_dcf_const_info  isp_dcf_const_info;
    ot_isp_dcf_update_info isp_dcf_update_info;
} ot_isp_dcf_info;

typedef struct {
    /* the date and time when the picture data was generated */
    td_u8           capture_time[OT_DCF_CAPTURE_TIME_LENGTH];
    /* whether the picture is captured when a flash lamp is on */
    td_bool         flash;
    /*
     * indicates the digital zoom ratio when the image was shot.
     * if the numerator of the recorded value is 0, this indicates that digital zoom was not used.
     */
    td_u32          digital_zoom_ratio;
    ot_isp_dcf_info isp_dcf_info;
} ot_jpeg_dcf;

/*
 * Defines the ISP FSWDR operating mode
 * 0 = Normal FSWDR mode
 * 1 = Long frame mode, only effective in LINE_WDR,
 * when running in this mode FSWDR module only output the long frame data
 */
typedef enum {
    OT_ISP_FSWDR_NORMAL_MODE          = 0x0,
    OT_ISP_FSWDR_LONG_FRAME_MODE      = 0x1,
    OT_ISP_FSWDR_AUTO_LONG_FRAME_MODE = 0x2, /* Auto long frame mode, only effective in LINE_WDR,  When running in this
                                             mode, normal WDR and long frame mode would auto switch */
    OT_ISP_FSWDR_MODE_BUTT
} ot_isp_fswdr_mode;

typedef struct {
    td_u32      iso;                    /* ISP internal ISO : again*dgain*is_pgain */
    td_u32      exposure_time[OT_ISP_WDR_MAX_FRAME_NUM]; /* exposure time (reciprocal of shutter speed), unit is us */
    td_u32      isp_dgain[OT_ISP_WDR_MAX_FRAME_NUM];
    td_u32      again[OT_ISP_WDR_MAX_FRAME_NUM];
    td_u32      dgain[OT_ISP_WDR_MAX_FRAME_NUM];
    ot_isp_fswdr_mode fs_wdr_mode;

    td_s32      fe_id[OT_ISP_WDR_MAX_FRAME_NUM];
    td_u32      ratio[OT_ISP_WDR_MAX_FRAME_NUM - 1];
    td_u32      isp_nr_strength;
    td_u32      f_number;               /* the actual F-number (F-stop) of lens when the image was taken */
    td_u32      sensor_id;              /* which sensor is used */
    td_u32      sensor_mode;
    td_u32      hmax_times;             /* sensor hmax_times,unit is ns */
    td_u32      vmax;                   /* sensor vmax,unit is line */
    td_u32      vc_num;                 /* when dump wdr frame, which is long or short  exposure frame. */
    td_u32      wb_gain[OT_ISP_WB_GAIN_NUM]; /* white balance gain info */
    td_u16      ccm[OT_ISP_CAP_CCM_NUM];     /* RW; Range: [0x0, 0xFFFF]; Format:8.8; Manual CCM matrix. */
    td_u32      exposure_distance[OT_ISP_WDR_MAX_FRAME_NUM - 1]; /* wdr sensor exposure distance line number. */
    td_s32      sync_id;
    td_u32      blc_sync_id;
} ot_isp_frame_info;

typedef struct {
    td_u32 color_temp;
    td_u16 ccm[OT_ISP_CAP_CCM_NUM];
    td_u8  saturation;
} ot_isp_hdr_info;

typedef enum {
    OT_NR_V1 = 1,

    OT_NR_BUTT
} ot_nr_version;

typedef struct {
    /* ies0~3 for different frequency response. */
    td_u16 ies0;     /* RW; Range: [0, 255]; The gains of edge and texture enhancement. */
    td_u16 ies1;     /* RW; Range: [0, 255]; The gains of edge and texture enhancement. */
    td_u16 ies2;     /* RW; Range: [0, 255]; The gains of edge and texture enhancement. */
    td_u16 ies3;     /* RW; Range: [0, 255]; The gains of edge and texture enhancement. */
    td_u16 aux_ies;  /* RW; Range: [0, 255]; The gains of edge enhancement for NO.2 filter. */
    struct {
        td_u16 iedz : 10;    /* RW; Range: [0, 999]; The threshold to control the generated artifacts. */
        td_u16 _rb_ : 6;
    };
    td_u8 o_sht;       /* RW; Range: [0, 255]; The gains to control the overshoot. */
    td_u8 u_sht;       /* RW; Range: [0, 255]; The gains to control the undershoot. */
    td_u8 aux_o_sht;   /* RW; Range: [0, 255]; The gains to control the overshoot. */
    td_u8 aux_u_sht;   /* RW; Range: [0, 255]; The gains to control the undershoot. */
} ot_nr_v1_adv_iey;

typedef struct {
    struct {
        td_u16 spn6    : 4;  /* RW; Range: [0,   5]; The selection of filters to be mixed for NO.6 filter. */
        td_u16 sbn6    : 4;  /* RW; Range: [0,   5]; The selection of filters to be mixed for NO.6 filter. */
        td_u16 pbr6    : 5;  /* RW; Range: [0,  16]; The mix ratio between spn and sbr. */
        td_u16 j_mode6 : 2;  /* RW; Range: [0,   2]; The selection modes for the blending of spatial filters. */
        td_u16 _rb_   : 1;  /* RW; Reserved */
    };
    td_u8 sfr6[4];     /* RW; Range: [0, 31]; The relative NR strength for NO.6 filter. (Effective when jmode = 2). */
    td_u8 sbr6[2];     /* RW; Range: [0, 15]; The control of overshoot and undershoot. */
    td_u8 sfs1;        /* RW; Range: [0, 255]; The NR strength parameters for NO.1 filter. */
    td_u8 sfs2;        /* RW; Range: [0, 255]; Currently no use, reserved. */
    td_u16 bld1;       /* RW; Range: [0, 999];  The NR strength parameters for NO.1 filter. */
    struct {
        td_u16 spn3  : 3;  /* RW; Range: [0,   5]; The selection of filters to be mixed for NO.3 filter. */
        td_u16 sbn3  : 3;  /* RW; Range: [0,   5]; The selection of filters to be mixed for NO.3 filter. */
        td_u16 pbr3  : 5;  /* RW; Range: [0,  16]; The mix ratio between spn and sbr. */
        td_u16 _rb0_ : 5;
    };
    struct {
        td_u16 spn4  : 3;  /* RW; Range: [0,   5]; The selection of filters to be mixed for NO.4 filter. */
        td_u16 sbn4  : 3;  /* RW; Range: [0,   5]; The selection of filters to be mixed for NO.4 filter. */
        td_u16 pbr4  : 5;  /* RW; Range: [0,  16]; The mix ratio between spn and sbr. */
        td_u16 _rb1_ : 5;
    };

    td_u8 sfs5;           /* RW; Range: [0, 255]; The NR strength parameters for NO.5 filters. */
    td_u8 sft5;           /* RW; Range: [0, 255]; The NR strength parameters for NO.5 filters. */
    td_u8 sbr5;           /* RW; Range: [0, 255]; The NR strength parameters for NO.5 filters. */
    td_u8 reserved2;

    td_u16  sth1;        /* RW; Range: [0, 511]; The thresholds for protection of edges from blurring. */
    td_u16  sth2;        /* RW; Range: [0, 511]; The thresholds for protection of edges from blurring. */
    td_u16  sth3;        /* RW; Range: [0, 511]; The thresholds for protection of edges from blurring. */
    struct {
        td_u16 sfn0 : 4; /* RW; Range: [0, 6]; Filter selection for different foreground image areas based on sth1. */
        td_u16 sfn1 : 4; /* RW; Range: [0, 6]; Filter selection for different foreground image areas based on sth1. */
        td_u16 sfn2 : 4; /* RW; Range: [0, 6]; Filter selection for different foreground image areas based on sth2. */
        td_u16 sfn3 : 4; /* RW; Range: [0, 6]; Filter selection for different foreground image areas based on sth3. */
    };
    td_u8 bri_str[OT_NR_S_IDX_LEN];   /* RW; Range: [0, 255];  Spatial NR strength based on brightness. */
    td_u8 bri_idx;                    /* RW; Range: [0, 6]; The filter selection based on brightness */
    td_u8 k_mode;               /* RW; Range: [0, 1]; mode selection of k filter */
    td_u8 sbs_k[33];            /* RW; Range: [0, 255]; the filter strength of k filter depending on brightness */
    td_u8 sds_k[33];            /* RW; Range: [0, 255]; the filter strength of k filter depending on brightness */
    td_u8 reserved3;
} ot_nr_v1_adv_sfy;

typedef struct {
    td_u8 adv_mode; /* RW; Range: [0, 3]; */
    ot_nr_v1_adv_iey adv_ie_y[2];
    ot_nr_v1_adv_sfy adv_sf_y[2];
} ot_ai3dnr_adv_v1;

typedef struct {
    ot_ai3dnr_adv_v1 adv_param;
} ot_ai3dnr_manual_adv_param;

typedef struct {
    td_u32 param_num;
    ot_ai3dnr_adv_v1 ATTRIBUTE *adv_param;
} ot_ai3dnr_auto_adv_param;

typedef struct {
    ot_op_mode op_mode;
    ot_ai3dnr_manual_adv_param manual_adv_param;
    ot_ai3dnr_auto_adv_param auto_adv_param;
} ot_ai3dnr_adv_param_v1;

typedef struct {
    ot_nr_version           nr_version;          /* RW; Version of 3DNR parameters. */
    union {
        ot_ai3dnr_adv_param_v1 nr_adv_param_v1;    /* AUTO: ot_nr_version:OT_NR_V1. */
    };
} ot_ai3dnr_adv_param;

typedef enum {
    OT_NR_TYPE_VIDEO_NORM = 0,
    OT_NR_TYPE_SNAP_NORM,
    OT_NR_TYPE_VIDEO_SPATIAL,
    OT_NR_TYPE_VIDEO_ENHANCED,
    OT_NR_TYPE_BUTT
} ot_nr_type;

typedef enum {
    OT_NR_MOTION_MODE_NORM = 0,              /* Normal. */
    OT_NR_MOTION_MODE_COMPENSATION,          /* Motion compensate. */
    OT_NR_MOTION_MODE_BUTT
} ot_nr_motion_mode;

typedef struct {
    /* ies0~3 for different frequency response. */
    td_u8 ies0;     /* RW; Range: [0, 255]; The gains of edge and texture enhancement. */
    td_u8 ies1;     /* RW; Range: [0, 255]; The gains of edge and texture enhancement. */
    td_u8 ies2;     /* RW; Range: [0, 255]; The gains of edge and texture enhancement. */
    td_u8 ies3;     /* RW; Range: [0, 255]; The gains of edge and texture enhancement. */
    td_u16 iedz;    /* RW; Range: [0, 999]; The threshold to control the generated artifacts. */
    td_u8 o_sht;    /* RW; Range: [0, 255]; The gains to control the overshoot. */
    td_u8 u_sht;    /* RW; Range: [0, 255]; The gains to control the undershoot. */
} ot_nr_v1_iey;

typedef struct {
    struct {
        td_u16 spn    : 4;  /* RW; Range: [0,   5]; The selection of filters to be mixed for NO.6 filter. */
        td_u16 sbn    : 4;  /* RW; Range: [0,   5]; The selection of filters to be mixed for NO.6 filter. */
        td_u16 pbr    : 5;  /* RW; Range: [0,  16]; The mix ratio between spn and sbr. */
        td_u16 j_mode : 2;  /* RW; Range: [0,   2]; The selection modes for the blending of spatial filters. */
        td_u16 _rb_   : 1;  /* RW; Reserved */
    };

    td_u8 sfr6[4];     /* RW; Range: [0, 31]; The relative NR strength for NO.6 filter. (Effective when jmode = 2). */
    td_u8 sbr6[2];     /* RW; Range: [0, 15]; The control of overshoot and undershoot. */

    td_u8 sfs1;        /* RW; Range: [0, 255]; The NR strength parameters for NO.1 filter. */
    td_u8 sbr1;        /* RW; Range: [0, 255]; The NR strength parameters for NO.1 filter. */
    td_u8 sfs2;        /* RW; Range: [0, 255]; The NR strength parameters for NO.2 filter. */
    td_u8 sft2;        /* RW; Range: [0, 255]; The NR strength parameters for NO.2 filter. */
    td_u8 sbr2;        /* RW; Range: [0, 255]; The NR strength parameters for NO.2 filter. */
    td_u8 sfs4;        /* RW; Range: [0, 255]; The NR strength parameters for NO.3 and NO.4 filters. */
    td_u8 sft4;        /* RW; Range: [0, 255]; The NR strength parameters for NO.3 and NO.4 filters. */
    td_u8 sbr4;        /* RW; Range: [0, 255]; The NR strength parameters for NO.3 and NO.4 filters. */
    /*
     * sth1_0, sth2_0, sth3_0; Range: [0, 511]; The thresholds for protection of edges on foreground from blurring.
     * sth1_1, sth2_1, sth3_1; Range: [0, 511]; The thresholds for protection of edges on background from blurring.
     * bri_idx0, bri_idx1; Range: [0, 7]; The filter selection based on brightness
     * for foreground and background, respectively.
     */
    td_u16 sth1_0;
    td_u16 sth2_0;
    td_u16 sth3_0;
    td_u16 sth1_1;
    td_u16 sth2_1;
    td_u16 sth3_1;

    /*
     * sfn0_0~sfn3_0; Range: [0, 6]; Filter selection for different foreground image areas based on sth1_0~sth3_0.
     * sfn0_1~sfn3_1; Range: [0, 6]; Filter selection for different background image areas based on sth1_1~sth3_1.
     */
    struct {
        td_u16 sfn0_0 : 4;
        td_u16 sfn1_0 : 4;
        td_u16 sfn2_0 : 4;
        td_u16 sfn3_0 : 4;
    };
    struct {
        td_u16 sfn0_1 : 4;
        td_u16 sfn1_1 : 4;
        td_u16 sfn2_1 : 4;
        td_u16 sfn3_1 : 4;
    };
    td_u8 bri_str[OT_NR_S_IDX_LEN]; /* RW ; Range: [0, 255];  Spatial NR strength based on brightness. */
    struct {
        td_u8 bri_idx0 : 3;     /* RW; Range: [0, 6]; The filter selection based on brightness */
        td_u8 bri_idx1 : 3;     /* RW; Range: [0, 6]; The filter selection based on brightness */
        td_u8 reserved1 : 2;
    };
    td_u8 k_mode;               /* RW; Range: [0, 2]; mode selection of k filter */
    td_u8 sbs_k[33];            /* RW; Range: [0, 255]; the filter strength of k filter depending on brightness */
    td_u8 sds_k[33];            /* RW; Range: [0, 255]; the filter strength of k filter depending on brightness */
    td_u8 reserved2;
} ot_nr_v1_sfy;

typedef struct {
    struct {
        td_u16 tfs0   : 4;   /* RW; Range: [0,  15]; The NR strength for temporal filtering. */
        td_u16 tdz0   : 10;  /* RW; Range: [0, 999]; Protection of the weak texture area from temporal filtering. */
        td_u16 ref_en : 1;   /* RW; Range: [0,  1];  The switch the reference frame. */
        td_u16 _rb0_  : 1;
    };
    struct {
        td_u16 tfs1      : 4;   /* RW; Range: [0,  15]; The NR strength for temporal filtering. */
        td_u16 tdz1      : 10;  /* RW; Range: [0, 999]; Protection of the weak texture area from temporal filtering. */
        td_u16 math_mode : 1;   /* RW; Range: [0,   1]; The motion detection mode. */
        td_u16 _rb1_     : 1;
    };
    td_u8   tss0;       /* RW; Range: [0,  15]; The ratio for blending spatial NR with the temporal NR results. */
    td_u8   tss1;       /* RW; Range: [0,  15]; The ratio for blending spatial NR with the temporal NR results. */

    td_u16 auto_math;   /* RW; Range: [0, 999]; The motion threshold for the Level 0 denoise. */
    td_u8 tfr0[6];      /* RW; Range: [0,  31]; The temporal NR strength control for background (static) area. */
    td_u8 tfr1[6];      /* RW; Range: [0,  31]; The temporal NR strength control for background (static) area. */
} ot_nr_v1_tfy;

typedef struct {
    struct {
        td_u16 math0 : 10;   /* RW; Range: [0, 999]; The threshold for motion detection. */
        td_u16 mate0 : 4;    /* RW; Range: [0,   8]; The motion index for smooth image area. */
        td_u16 adv_math : 2; /* RW; Range: [0,   2]; The mode selection for motion detection. */
    };
    struct {
        td_u16 math1 : 10;  /* RW; Range: [0, 999]; The threshold for motion detection. */
        td_u16 mate1 : 4;   /* RW; Range: [0,   8]; The motion index for smooth image area. */
        td_u16 _rb1_ : 2;
    };
    td_u8   mabw0;          /* RW; Range: [5,   9]; The window size for motion detection. */
    td_u8   mabw1;          /* RW; Range: [5,   9]; The window size for motion detection. */

    td_u16 artc;           /* RW; Range: [0, 255]; The artifact control for motion */
    td_u16 mcth;           /* RW; Range: [0, 999]; The sensitivity for motion */
    td_u16 dci_w;          /* RW; Range: [0, 999]; The temporal gain for dci gain */
} ot_nr_v1_mdy;

typedef struct {
    td_u8 sfc_enhance;     /* RW; Range: [0, 255];  The chroma NR strength for sfc. */
    td_u8 sfc_ext;         /* RW; Range: [0, 255];  The extra chroma NR strength for sfc. */
    td_u8 trc;             /* RW; Range: [0, 255];  The temporal chroma NR strength. */
    td_u8 reserved;
    struct {
        td_u16 sfc   : 8;   /* RW; Range: [0, 255];  The motion threshold for chroma. */
        td_u16 tfc   : 6;   /* RW; Range: [0, 63];   The control of temporal chroma NR strength. */
        td_u16 _rb0_ : 2;
    };
    td_u8 tpc;              /* RW; Range: [0, 1];  Chroma denoise mode. */
    td_u8 pre_sfc;          /* RW; Range: [0, 31];  pre-chroma denoise. */
} ot_nr_v1_nrc0;

typedef struct {
    td_u8 sfs2;      /* RW; Range: [0, 255];  The NR strength parameters for NO.2 filter. */
    td_u8 sft2;      /* RW; Range: [0, 255];  The NR strength parameters for NO.2 filter. */
    td_u8 sbr2;      /* RW; Range: [0, 255];  The NR strength parameters for NO.2 filter. */
    struct {
        td_u16 pbr0  : 4;    /* RW; Range: [0, 15]; The mix ratio. */
        td_u16 pbr1  : 4;    /* RW; Range: [0, 15]; The mix ratio. */
        td_u16 _rb0_ : 8;
    };
} ot_nr_v1_nrc1;

typedef struct {
    ot_nr_v1_iey  iey[5];
    ot_nr_v1_sfy  sfy[5];
    ot_nr_v1_mdy  mdy[2];
    ot_nr_v1_tfy  tfy[3];
    ot_nr_v1_nrc0 nrc0;
    ot_nr_v1_nrc1 nrc1;

    struct {
        td_u8 limit_range_en : 1;  /* RW; Range: [0, 1]; The switch for limit range mode. */
        td_u8 nry0_en        : 1;  /* RW; Range: [0, 1]; The switch for luma denoise. */
        td_u8 nry1_en        : 1;  /* RW; Range: [0, 1]; The switch for luma denoise. */
        td_u8 nry2_en        : 1;  /* RW; Range: [0, 1]; The switch for luma denoise. */
        td_u8 nry3_en        : 1;  /* RW; Range: [0, 1]; The switch for luma denoise. */
        td_u8 nrc0_en        : 1;  /* RW; Range: [0, 1]; The switch for chroma denoise. */
        td_u8 nrc1_en        : 1;  /* RW; Range: [0, 1]; The switch for chroma denoise. */
        td_u8 _rb_           : 1;
    };
} ot_nr_v1;

typedef struct {
    ot_nr_v1 nr_param;
} ot_nr_param_manual_v1;

typedef struct {
    td_u32 param_num;
    td_u32 ATTRIBUTE *iso;
    ot_nr_v1 ATTRIBUTE *nr_param;
} ot_nr_param_auto_v1;

typedef struct {
    ot_op_mode op_mode;
    ot_nr_param_manual_v1 nr_manual;
    ot_nr_param_auto_v1 nr_auto;
} ot_nr_norm_param_v1;

typedef struct {
    td_u32                   mdz;                 /* RW; Range: [0, 63]; */
    td_u32                   hard_tf_strength;    /* RW; Range: [0, 96]; Relation strength of hard NR channel. */
    /* Only for weak nr chn */
    td_u32                   edz;                 /* RW; Range: [0, 63]; */
    td_u32                   weak_tf_strength;    /* RW; Range: [0, 63]; Relation strength of weak NR channel. */
} ot_nr_adv_param;

typedef struct {
    ot_nr_version           nr_version;          /* RW; Version of 3DNR parameters. */
    union {
        ot_nr_norm_param_v1 nr_norm_param_v1;    /* AUTO: ot_nr_version:OT_NR_V1. */
    };
} ot_3dnr_param;

typedef struct {
    td_bool            enable;
    ot_nr_type         nr_type;        /* RW; 3DNR type. */
    ot_compress_mode   compress_mode;  /* RW; Reference frame compression mode. */
    ot_nr_motion_mode  nr_motion_mode; /* RW; 3DNR motion compensate mode. */
} ot_3dnr_attr;

typedef struct {
    ot_isp_hdr_info isp_hdr;
    td_u32 iso;
    td_u32 init_iso;
    td_u8 sns_wdr_mode;
} ot_isp_attach_info;

typedef struct {
    ot_color_gamut color_gamut;
} ot_isp_colorgammut_info;

typedef struct {
    td_u32 numerator;   /* represents the numerator of a fraction. */
    td_u32 denominator; /* the denominator. */
} ot_dng_rational;

/* defines the structure of DNG image dynamic information */
typedef struct {
    /* RO; range: [0x0, 0xFFFF]; black level. */
    td_u32 black_level[OT_ISP_BAYER_CHN];
    /*
     * specifies the selected white balance at time of capture,
     * encoded as the coordinates of a perfectly neutral color in linear reference space values.
     */
    ot_dng_rational as_shot_neutral[OT_CFACOLORPLANE];
    /* RO; describes the amount of noise in a raw image */
    td_double ad_noise_profile[OT_DNG_NP_SIZE];
} ot_dng_image_dynamic_info;

typedef struct {
    td_u32 iso;
    td_u32 isp_dgain;
    td_u32 exposure_time;
    td_u32 white_balance_gain[OT_ISP_WB_GAIN_NUM];
    td_u32 color_temperature;
    td_u16 cap_ccm[OT_ISP_CAP_CCM_NUM];
} ot_isp_config_info;

typedef struct {
    td_bool enable;         /* RW; low delay enable. */
    td_u32  line_cnt;       /* RW; range: [16, 8192]; low delay shoreline. */
    td_bool one_buf_en;     /* RW; one buffer for low delay enable. */
} ot_low_delay_info;

typedef enum {
    OT_SCHEDULE_NORMAL = 0,
    OT_SCHEDULE_QUICK,
    OT_SCHEDULE_BUTT
} ot_schedule_mode;

/* spread correction attribute */
typedef struct {
    /* RW; range: [0, 1];whether enable spread or not, when spread on,ldc distortion_ratio range should be [0, 500] */
    td_bool enable;
    td_u32  spread_coef; /* RW; range: [0, 18];strength coefficient of spread correction */
} ot_spread_attr;

/* dest_point */
typedef struct {
    td_s64 motion_data[OT_DIS_MOTIONDATA_NUM];
} ot_dis_motion_data;

typedef struct {
    ot_dis_motion_data data;
    td_bool valid;
} ot_dis_motion_info;

typedef struct {
    td_s32 point_num;
    ot_point point[OT_DIS_POINT_NUM];
} ot_dst_point_info;

typedef struct {
    ot_dis_motion_info gme_motion_data;
    ot_dis_motion_info fliter_motion_data;
    ot_dst_point_info dst_point_info;
    td_s32 private_data[OT_DIS_PRIVATE_NUM];
} ot_dis_motion_data_info;

typedef struct {
    td_bool    aspect;      /* RW;range: [0, 1];whether aspect ration  is keep */
    td_s32     x_ratio;     /* RW; range: [0, 100]; field angle ration of  horizontal,valid when aspect=0. */
    td_s32     y_ratio;     /* RW; range: [0, 100]; field angle ration of  vertical,valid when aspect=0. */
    td_s32     xy_ratio;    /* RW; range: [0, 100]; field angle ration of  all,valid when aspect=1. */
     /* RW; range: [-511, 511]; horizontal offset of the image distortion center relative to image center. */
    td_s32 center_x_offset;
     /* RW; range: [-511, 511]; vertical offset of the image distortion center relative to image center. */
    td_s32 center_y_offset;
    /* RW; range: [-300, 500]; LDC distortion ratio.when spread on,distortion_ratio range should be [0, 500] */
    td_s32 distortion_ratio;
} ot_ldc_v1_attr;

typedef struct {
    td_bool    aspect;      /* RW;range: [0, 1];whether aspect ration  is keep */
    td_s32     x_ratio;     /* RW; range: [0, 100]; field angle ration of  horizontal,valid when aspect=0. */
    td_s32     y_ratio;     /* RW; range: [0, 100]; field angle ration of  vertical,valid when aspect=0. */
    td_s32     xy_ratio;    /* RW; range: [0, 100]; field angle ration of  all,valid when aspect=1. */
    /* RW; range: [6400, 3276700]; focal length in horizontal direction, with 2 decimal numbers */
    td_s32 focal_len_x;
    /* RW; range: [6400, 3276700]; focal length in vertical direction, with 2 decimal numbers */
    td_s32 focal_len_y;
    /* RW; range: [35*width, 65*width]; coordinate of image center, with 2 decimal numbers */
    td_s32 coord_shift_x;
    /* RW; range: [35*height, 65*height]; Y coordinate of image center, with 2 decimal numbers */
    td_s32 coord_shift_y;
    /* RW; lens distortion coefficients of the source image, with 5 decimal numbers */
    td_s32 src_calibration_ratio[OT_SRC_LENS_COEF_NUM];
    /* RW; lens distortion coefficients of the source image (for interpolation), with 5 decimal numbers */
    td_s32 src_calibration_ratio_next[OT_SRC_LENS_COEF_NUM];
    td_u32 coef_intp_ratio; /* RW; range: [0, 32768]; interpolation ratio between two coefficient sets, 15bits */
} ot_ldc_v2_attr;

typedef struct {
    /* RW; range: [6400, 117341700]; focal length in horizontal direction, with 2 decimal numbers */
    td_s32 focal_len_x;
    /* RW; range: [6400, 117341700]; focal length in vertical direction, with 2 decimal numbers */
    td_s32 focal_len_y;
    /* RW; range: [35*width, 65*width]; coordinate of image center, with 2 decimal numbers */
    td_s32 coord_shift_x;
    /* RW; range: [35*height, 65*height]; Y coordinate of image center, with 2 decimal numbers */
    td_s32 coord_shift_y;
    /* RW; range: [-1600000, 1600000]; lens distortion coefficients of the source image, with 5 decimal numbers
        src_calibration_ratio[0]: 100000; src_calibration_ratio[8]: [0, 3200000] */
    td_s32 src_calibration_ratio[OT_SRC_LENS_COEF_NUM];
    /* RW; range: [-1600000, 1600000]; lens distortion coefficients, with 5 decimal numbers
        dst_calibration_ratio[12]: [0, 1600000]; dst_calibration_ratio[13]: [0, 1600000] */
    td_s32 dst_calibration_ratio[OT_DST_LENS_COEF_NUM];
    /* RW; range: [0, 1048576]; max undistorted distance before 3rd polynomial drop, with 16bits decimal */
    td_s32 max_du;
} ot_dis_ldc_attr;

typedef enum {
    OT_LDC_V1 = 0,
    OT_LDC_V2 = 1,

    OT_LDC_VERSION_BUTT
} ot_ldc_version;

typedef struct {
    td_bool enable;                 /* RW; Range [0,1]; Whether LDC is enable */
    ot_ldc_version ldc_version;     /* RW; Version of LDC */
    union {
        ot_ldc_v1_attr ldc_v1_attr; /* RW; Attribute of LDC v1. AUTO:ot_ldc_version:OT_LDC_V1; */
        ot_ldc_v2_attr ldc_v2_attr; /* RW; Attribute of LDC v2. AUTO:ot_ldc_version:OT_LDC_V2; */
    };
} ot_ldc_attr;

/* gdc PMF attr */
typedef struct {
    td_bool enable;                          /* RW; Range [0,1]; Whether or not pmf is enable */
    ot_size dst_size;
    td_s64  pmf_coef[OT_GDC_COMMON_COEF_NUM]; /* RW; PMF coefficient of gdc */
} ot_pmf_attr;


/* gdc FOV attr */
typedef struct {
    td_bool enable;                          /* RW; Range [0,1]; Whether or not fov is enable */
    td_s64 fov_coef[OT_GDC_COMMON_COEF_NUM]; /* RW; matrix for match the two */
} ot_fov_attr;

typedef enum {
    OT_LUT_CELL_SIZE_16,
    OT_LUT_CELL_SIZE_32,
    OT_LUT_CELL_SIZE_64,
    OT_LUT_CELL_SIZE_128,
    OT_LUT_CELL_SIZE_256,

    OT_LUT_CELL_SIZE_BUTT
} ot_lut_cell_size;

typedef struct {
    ot_size          in_size;   /* gdc input size, default: grp_attr.max_width, grp_attr.max_height */
    ot_lut_cell_size cell_size; /* gdc function cell size, default:OT_LUT_CELL_SIZE_16 */
} ot_gdc_param;

typedef struct {
    ot_size             in_size;        /* lut in size */
    ot_size             out_size;       /* lut out size */
    ot_lut_cell_size    cell_size;      /* lut cell size */
    td_phys_addr_t      lut_phys_addr;  /* phys addr of lut */
    td_u32              lut_len;        /* length of lut */
} ot_lut;

typedef enum {
    OT_LUT_CONNECT_MODE,
    OT_LUT_FUSION_MODE,
    OT_LUT_MODE_BUTT
} ot_lut_mode;

typedef struct {
    td_bool     pre_lut_en;     /* RW; Range [0,1]; Whether or not the pre lut is enable */
    ot_lut      pre_lut;        /* RW; pre lut */
    td_bool     post_lut_en;    /* RW; Range [0,1]; Whether or not the post lut is enable */
    ot_lut      post_lut;       /* RW; post lut */
    ot_lut_mode lut_mode;       /* RW; lut mode */
    td_u32      ratio;          /* RW; range: [0, 32768]; fusion ratio of two lut */
} ot_lut_attr;

typedef struct {
    td_u32  blend_position; /* RW; blend position, the beginning of the blend range */
    td_u32  blend_range; /* RW; blend range, within blend area */
} ot_blend_param;

typedef struct {
    td_u32         buf_num; /* RW; Range [0, 5]; buffer number of blend area */
    ot_blend_param blend_param[OT_GDC_MAX_STITCH_NUM - 1]; /* RW; param of stitch blend */
} ot_stitch_blend_param;

typedef struct {
    td_s32 isp_pipe[OT_GDC_MAX_STITCH_NUM]; /* W; isp pipe of bind stitch in pipe */
    td_u32 milli_sec; /* W; Range [100, 10000]; sync time interval, millisecond */
    td_u8 smooth_strength; /* W; Range [1, 16]; vertical filter window size */
    td_u8 shading_adjust_threshold; /* W; Range [0, 255]; shading gain range */
} ot_stitch_luma_sync_param;

typedef enum {
    OT_STITCH_BLEND, /* Stitching in blend mode. */
    OT_STITCH_NOBLEND_VER, /* Vertical stitching, no blending. */
    OT_STITCH_NOBLEND_HOR, /* Horizontal stitching, no blending. */
    OT_STITCH_NOBLEND_QR, /* Quarter stitching, no blending. */

    OT_STITCH_BUTT
} ot_stitch_mode;

typedef enum {
    OT_STITCH_CORRECTION_CYLINDRICAL,
    OT_STITCH_CORRECTION_LUT,
    OT_STITCH_CORRECTION_BUTT,
} ot_stitch_correction_mode;

typedef struct {
    td_bool pmf_enable;      /* RW; Range [0, 1]; Whether or not pmf is enable */
    td_s64  pmf_coef[OT_GDC_COMMON_COEF_NUM]; /* RW; PMF coefficient of gdc */
    td_s32  center_x_offset; /* RW; offset of x; Range [-511, 511] */
    td_s32  center_y_offset; /* RW; offset of y; Range [-511, 511] */
    td_u32  ratio;           /* RW: ratio of cylindrical projection; Range [0, 500]  */
    ot_size dst_size;        /* RW: destination size of stitch pipe */
} ot_cylindrical_attr;

typedef union {
    /* attribute of cylindrical mode. AUTO:ot_stitch_correction_mode: OT_STITCH_CORRECTION_CYLINDRICAL; */
    ot_cylindrical_attr cylindrical_attr;
    /* attribute of lut mode. AUTO:ot_stitch_correction_mode: OT_STITCH_CORRECTION_LUT; */
    ot_lut              lut;
} ot_stitch_correction_attr;

typedef struct {
    td_bool         enable; /* RW; Range [0, 1]; Whether or not stitch is enable */
    td_u32          pipe_num; /* RW; Range [2, 4]; stitch pipe number */
    td_bool         sync_pipe_en; /* RW; Range [0, 1]; sync pipe with pts or not */
    ot_stitch_mode  stitch_mode; /* RW; stitch mode: blend/noblend_ver/noblend_hor/noblend_qr */
    ot_point        overlap_point[OT_GDC_MAX_STITCH_NUM][2]; /* RW; start and end position of overlap area */
    ot_stitch_correction_mode correction_mode; /* RW; correction mode: cylindrical/lut */
    ot_stitch_correction_attr correction_attr[OT_GDC_MAX_STITCH_NUM]; /* RW; attribute of correction */
} ot_stitch_attr;

typedef struct {
    td_bool enable; /* RW; Range: [0, 1]; Zoom enable. */
    ot_coord mode;  /* RW; Coordinate mode of zoom. */
    ot_rect rect;   /* RW; Zoom rectangular. */
} ot_zoom_attr;

typedef enum {
    OT_3DNR_POS_VI,
    OT_3DNR_POS_VPSS,

    OT_3DNR_POS_BUTT
} ot_3dnr_pos_type;

typedef enum {
    OT_FMU_MODE_OFF,
    OT_FMU_MODE_WRAP,
    OT_FMU_MODE_DIRECT,

    OT_FMU_MODE_BUTT
} ot_fmu_mode;

typedef enum {
    OT_FMU_ID_VI = 0,
    OT_FMU_ID_VPSS = 1,

    OT_FMU_ID_BUTT
} ot_fmu_id;

typedef struct {
    td_bool wrap_en;
    td_u32  page_num;
} ot_fmu_attr;

typedef struct {
    td_u32            width;
    td_u32            height;
    td_u32            align;
    ot_data_bit_width bit_width;
    ot_pixel_format   pixel_format;
    ot_compress_mode  compress_mode;
    ot_video_format   video_format;
} ot_pic_buf_attr;

typedef struct {
    td_bool           share_buf_en;
    td_u32            frame_buf_ratio;
    ot_pic_buf_attr   pic_buf_attr;
} ot_venc_buf_attr;

typedef enum {
    OT_SCALE_COEF_TYPE_NORMAL = 0,
    OT_SCALE_COEF_TYPE_BILINEAR,
    OT_SCALE_COEF_TYPE_BUTT
} ot_scale_coef_type;

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* OT_COMMON_VIDEO_H */
