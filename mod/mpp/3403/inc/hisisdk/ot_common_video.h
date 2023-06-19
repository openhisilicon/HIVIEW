/*
  Copyright (c), 2001-2022, Shenshu Tech. Co., Ltd.
 */

#ifndef __OT_COMMON_VIDEO_H__
#define __OT_COMMON_VIDEO_H__

#include "ot_type.h"
#include "ot_common.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

#define OT_ISP_BAYER_CHN           4
#define OT_DCF_DRSCRIPTION_LENGTH  32
#define OT_DCF_CAPTURE_TIME_LENGTH 20
#define OT_CFACOLORPLANE           3
#define OT_DNG_NP_SIZE             6
#define OT_ISP_WB_GAIN_NUM         4
/* 3*3=9 matrix */
#define OT_ISP_CAP_CCM_NUM         9
#define OT_SRC_LENS_COEF_NUM       9
#define OT_DST_LENS_COEF_NUM       14
#define OT_ISP_WDR_MAX_FRAME_NUM   4

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
    OT_VIDEO_FORMAT_BUTT
} ot_video_format;

typedef enum  {
    OT_COMPRESS_MODE_NONE = 0,      /* no compress */
    OT_COMPRESS_MODE_SEG,           /* compress unit is 256x1 bytes as a segment. */
    OT_COMPRESS_MODE_SEG_COMPACT,   /* compact compress unit is 256x1 bytes as a segment. */
    OT_COMPRESS_MODE_TILE,          /* compress unit is a tile. */
    OT_COMPRESS_MODE_LINE,          /* compress unit is the whole line. */
    OT_COMPRESS_MODE_FRAME,         /* compress unit is the whole frame. YUV for VPSS(3DNR) */

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
    OT_FRAME_FLAG_BUTT
} ot_frame_flag;

typedef enum  {
    OT_ISP_INNER_FLAG_DGAIN_FORWARD = 0x1 << 0,
    OT_ISP_INNER_FLAG_BUTT
} ot_isp_inner_flag;

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
    ot_cover_type      type;   /* RW; Cover type. */
    union {
        ot_rect        rect;   /* RW; The rectangle area. AUTO:ot_cover_type:OT_COVER_RECT; */
        ot_quad_cover  quad;   /* RW; The quadrangle area.  AUTO:ot_cover_type:OT_COVER_QUAD; */
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
    td_u32         bg_color;      /* background color, RGB 888 */
    ot_rect         video_rect;     /* valid in ASPECT_RATIO_MANUAL mode */
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
    td_phys_addr_t   bnr_rnt_phys_addr;
    td_phys_addr_t   motion_data_phys_addr;
    td_phys_addr_t   frame_dng_phys_addr;

    td_void* ATTRIBUTE misc_info_virt_addr;       /* misc info */
    td_void* ATTRIBUTE jpeg_dcf_virt_addr;        /* jpeg_dcf, used in JPEG DCF */
    td_void* ATTRIBUTE isp_info_virt_addr;        /* isp_frame_info, used in ISP debug, when get raw and send raw */
    td_void* ATTRIBUTE low_delay_virt_addr;       /* used in low delay */
    td_void* ATTRIBUTE bnr_mot_virt_addr;         /* used for 3dnr from bnr mot */
    td_void* ATTRIBUTE motion_data_virt_addr;     /* vpss 3dnr use: gme motion data, filter motion data, gyro data. */
    td_void* ATTRIBUTE frame_dng_virt_addr;
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
    /* RW; range: [6400, 117341700]; focal length in horizontal direction, with 2 decimal numbers */
    td_s32 focal_len_x;
    /* RW; range: [6400, 117341700]; focal length in vertical direction, with 2 decimal numbers */
    td_s32 focal_len_y;
    /* RW; range: [35*width, 65*width]; coordinate of image center, with 2 decimal numbers */
    td_s32 coord_shift_x;
    /* RW; range: [35*height, 65*height]; Y coordinate of image center, with 2 decimal numbers */
    td_s32 coord_shift_y;
    /* RW; lens distortion coefficients of the source image, with 5 decimal numbers */
    td_s32 src_calibration_ratio[OT_SRC_LENS_COEF_NUM];
    /* RW; lens distortion coefficients, with 5 decimal numbers */
    td_s32 dst_calibration_ratio[OT_DST_LENS_COEF_NUM];
    /* RW; range: [0, 1048576]; max undistorted distance before 3rd polynomial drop, with 16bits decimal */
    td_s32 max_du;
} ot_ldc_v2_attr;

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
} ot_ldc_v3_attr;

typedef enum {
    OT_LDC_V1 = 0,
    OT_LDC_V2 = 1,
    OT_LDC_V3 = 2,

    OT_LDC_VERSION_BUTT
} ot_ldc_version;

typedef struct {
    td_bool enable;                 /* RW; Range [0,1];Whether LDC is enable */
    ot_ldc_version ldc_version;     /* RW; Version of LDC */
    union {
        ot_ldc_v1_attr ldc_v1_attr; /* RW; Attribute of LDC v1. AUTO:ot_ldc_version:OT_LDC_V1; */
        ot_ldc_v2_attr ldc_v2_attr; /* RW; Attribute of LDC v2. AUTO:ot_ldc_version:OT_LDC_V2; */
        ot_ldc_v3_attr ldc_v3_attr; /* RW; Attribute of LDC v3. AUTO:ot_ldc_version:OT_LDC_V3; */
    };
} ot_ldc_attr;

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
         * AUTO:ot_rotation_type:OT_ROTATION_ANG_FREE;
         */
        ot_free_rotation_attr rotation_free;
    };
} ot_rotation_attr;

/* spread correction attribute */
typedef struct {
    /* RW; range: [0, 1];whether enable spread or not, when spread on,ldc distortion_ratio range should be [0, 500] */
    td_bool enable;
    td_u32  spread_coef; /* RW; range: [0, 18];strength coefficient of spread correction */
} ot_spread_attr;

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
    OT_WDR_MODE_QUDRA,

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
    td_u8 image_description[OT_DCF_DRSCRIPTION_LENGTH];
    /* shows manufacturer of digital cameras */
    td_u8 make[OT_DCF_DRSCRIPTION_LENGTH];
    /* shows model number of digital cameras */
    td_u8 model[OT_DCF_DRSCRIPTION_LENGTH];
    /* shows firmware (internal software of digital cameras) version number */
    td_u8 software[OT_DCF_DRSCRIPTION_LENGTH];
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
    td_u32      ratio[3];
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
    td_u32      inner_flag; /* isp inner flag, can be OR operation. */
} ot_isp_frame_info;

typedef struct {
    td_u32 color_temp;
    td_u16 ccm[9];
    td_u8  saturation;
} ot_isp_hdr_info;

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

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* __OT_COMMON_VIDEO_H__ */
