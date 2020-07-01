/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2012-2018. All rights reserved.
 * Description:
 * Author:
 * Create: 2016-09-19
 */

#ifndef __HI_COMM_VIDEO_ADAPT_H__
#define __HI_COMM_VIDEO_ADAPT_H__

#include "hi_type.h"
#include "hi_common.h"
#include "hi_comm_video.h"
#include "hi_common_adapt.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

typedef OPERATION_MODE_E hi_operation_mode;

typedef ROTATION_E hi_rotation;
typedef VB_SOURCE_E hi_vb_source;
typedef DATA_RATE_E hi_data_rate;

typedef struct {
    hi_u32 top_width;
    hi_u32 bottom_width;
    hi_u32 left_width;
    hi_u32 right_width;
    hi_u32 color;
} hi_border;

typedef struct {
    hi_s32 x;
    hi_s32 y;
} hi_point;

typedef struct {
    hi_u32 width;
    hi_u32 height;
} hi_size;

typedef struct {
    hi_s32 x;
    hi_s32 y;
    hi_u32 width;
    hi_u32 height;
} hi_rect;

typedef struct {
    hi_u32           region_num;       /* W; count of the region */
    hi_rect ATTRIBUTE* region;         /* W; region attribute */
} hi_video_region_info;

typedef struct {
    hi_bool enable;
    hi_rect  rect;
} hi_crop_info;

typedef struct {
    hi_s32  src_frame_rate;        /* RW; source frame rate */
    hi_s32  dst_frame_rate;        /* RW; dest frame rate */
} hi_frame_rate_ctrl;

typedef ASPECT_RATIO_E hi_aspect_ratio_type;

typedef struct {
    hi_aspect_ratio_type mode;          /* aspect ratio mode: none/auto/manual */
    hi_u32         bg_color;      /* background color, RGB 888 */
    hi_rect         video_rect;     /* valid in ASPECT_RATIO_MANUAL mode */

} hi_aspect_ratio;

/* we ONLY define picture format used, all unused will be deleted! */
typedef PIXEL_FORMAT_E hi_pixel_format;

typedef VIDEO_FIELD_E hi_video_field;

typedef VIDEO_FORMAT_E hi_video_format;

typedef COMPRESS_MODE_E hi_compress_mode;

typedef VIDEO_DISPLAY_MODE_E hi_video_display_mode;

typedef struct {
    hi_u64 luma_pix_sum;      /* luma sum of current frame */
    hi_u32 luma_pix_average;  /* luma average of current frame */
    hi_u64 pts;             /* PTS of current frame  */
} hi_luma_info;

typedef struct {
    hi_u8       image_description[DCF_DRSCRIPTION_LENGTH];        /*describes image*/
    hi_u8       make[DCF_DRSCRIPTION_LENGTH];                    /*shows manufacturer of digital cameras*/
    hi_u8       model[DCF_DRSCRIPTION_LENGTH];                   /*shows model number of digital cameras*/
    hi_u8       software[DCF_DRSCRIPTION_LENGTH];                /*shows firmware (internal software of digital cameras) version number*/

    hi_u8       light_source;                                      /*light source, actually this means white balance setting. '0' means unknown, '1' daylight, '2'
                                                                               fluorescent, '3' tungsten, '10' flash, '17' standard light A, '18' standard light B, '19' standard light
                                                                               C, '20' D55, '21' D65, '22' D75, '255' other*/
    hi_u32      focal_length;                                     /*focal length of lens used to take image. unit is millimeter*/
    hi_u8       scene_type;                                        /*indicates the type of scene. value '0x01' means that the image was directly photographed.*/
    hi_u8       custom_rendered;                                   /*indicates the use of special processing on image data, such as rendering geared to output.
                                                                               0 = normal process  1 = custom process   */
    hi_u8       focal_length_in35mm_film;                            /*indicates the equivalent focal length assuming a 35mm film camera, in mm*/
    hi_u8       scene_capture_type;                                 /*indicates the type of scene that was shot. 0 = standard,1 = landscape,2 = portrait,3 = night scene. */
    hi_u8       gain_control;                                      /*indicates the degree of overall image gain adjustment. 0 = none,1 = low gain up,2 = high gain up,3 = low gain down,4 = high gain down. */
    hi_u8       contrast;                                         /*indicates the direction of contrast processing applied by the camera when the image was shot.
                                                                               0 = normal,1 = soft,2 = hard */
    hi_u8       saturation;                                       /*indicates the direction of saturation processing applied by the camera when the image was shot.
                                                                              0 = normal,1 = low saturation,2 = high saturation*/
    hi_u8       sharpness;                                        /*indicates the direction of sharpness processing applied by the camera when the image was shot.
                                                                              0 = normal,1 = soft,2 = hard .*/
    hi_u8       metering_mode;                                     /*exposure metering method. '0' means unknown, '1' average, '2' center weighted average, '3'
                                                                              spot, '4' multi-spot, '5' multi-segment, '6' partial, '255' other*/
} hi_isp_dcf_const_info;

typedef struct {
    hi_u32      iso_speed_ratings;                                 /*CCD sensitivity equivalent to ag-hr film speedrate*/
    hi_u32      exposure_time;                                    /*exposure time (reciprocal of shutter speed).*/
    hi_u32      exposure_bias_value;                               /*exposure bias (compensation) value of taking picture*/
    hi_u8       exposure_program;                                  /*exposure program that the camera used when image was taken. '1' means manual control, '2'
                                                                              program normal, '3' aperture priority, '4' shutter priority, '5' program creative (slow program),
                                                                              '6' program action(high-speed program), '7' portrait mode, '8' landscape mode*/
    hi_u32      f_number;                                         /*the actual F-number (F-stop) of lens when the image was taken*/
    hi_u32      max_aperture_value;                                /*maximum aperture value of lens.*/
    hi_u8       exposure_mode;                                     /*indicates the exposure mode set when the image was shot.
                                                                              0 = auto exposure,1 = manual exposure, 2 = auto bracket*/
    hi_u8       white_balance;                                     /* indicates the white balance mode set when the image was shot.
                                                                                0 = auto white balance ,1 = manual white balance */
} hi_isp_dcf_update_info;

typedef struct {
    hi_isp_dcf_const_info  isp_dcf_const_info;
    hi_isp_dcf_update_info isp_dcf_update_info;
} hi_isp_dcf_info;

typedef struct {
    hi_u8           capture_time[DCF_CAPTURE_TIME_LENGTH];            /*the date and time when the picture data was generated*/
    hi_bool         flash;                                             /*whether the picture is captured when a flash lamp is on*/
    hi_u32          digital_zoom_ratio;                                /*indicates the digital zoom ratio when the image was shot.
                                                                                   if the numerator of the recorded value is 0, this indicates that digital zoom was not used.*/
    hi_isp_dcf_info isp_dcf_info;
} hi_jpeg_dcf;

typedef struct {
    hi_u32      iso;                    /* ISP internal ISO : again*dgain*is_pgain */
    hi_u32      exposure_time;           /* exposure time (reciprocal of shutter speed),unit is us */
    hi_u32      isp_dgain;
    hi_u32      again;
    hi_u32      dgain;
    hi_u32      ratio[3];
    hi_u32      isp_nr_strength;
    hi_u32      f_number;                /* the actual F-number (F-stop) of lens when the image was taken */
    hi_u32      sensor_id;               /* which sensor is used */
    hi_u32      sensor_mode;
    hi_u32      hmax_times;              /* sensor hmax_times,unit is ns */
    hi_u32      vmax;                   /* sensor vmax,unit is line */
    hi_u32      vc_num;                  /* when dump wdr frame, which is long or short  exposure frame. */
} hi_isp_frame_info;

typedef struct {
    hi_u32 numerator;/*represents the numerator of a fraction,*/
    hi_u32 denominator;/* the denominator. */
} hi_dng_rational;

/*
defines the structure of DNG image dynamic infomation
*/
typedef struct {
    hi_u32 black_level[ISP_BAYER_CHN];           /* RO;range: [0x0, 0xFFFF]; black level*/
    hi_dng_rational as_shot_neutral[CFACOLORPLANE]; /* specifies the selected white balance at time of capture, encoded as the coordinates of a perfectly neutral color in linear reference space values.*/
    hi_double ad_noise_profile[DNG_NP_SIZE];          /* RO;describes the amount of noise in a raw image*/
} hi_dng_image_dynamic_info;

typedef struct {
    hi_u32 color_temp;
    hi_u16 ccm[9];
    hi_u8  saturation;
} hi_isp_hdr_info;

typedef struct {
    hi_isp_hdr_info     isp_hdr;
    hi_u32 iso;
    hi_u8 sns_wdr_mode;
} hi_isp_attach_info;

typedef FRAME_FLAG_E hi_frame_flag;

typedef struct {
    hi_u32 iso;
    hi_u32 isp_dgain;
    hi_u32 exposure_time;
    hi_u32 white_balance_gain[ISP_WB_GAIN_NUM];
    hi_u32 color_temperature;
    hi_u16 cap_ccm[ISP_CAP_CCM_NUM];
} hi_isp_config_info;

typedef struct {
    hi_u64   jpeg_dcf_phy_addr;
    hi_u64   isp_info_phy_addr;
    hi_u64   low_delay_phy_addr;
    hi_u64   motion_data_phy_addr;
    hi_u64   frame_dng_phy_addr;

    hi_void* ATTRIBUTE jpeg_dcf_vir_addr;        /* jpeg_dcf, used in JPEG DCF */
    hi_void* ATTRIBUTE isp_info_vir_addr;        /* isp_frame_info, used in ISP debug, when get raw and send raw */
    hi_void* ATTRIBUTE low_delay_vir_addr;       /* used in low delay */
    hi_void* ATTRIBUTE motion_data_vir_addr;     /* vpss 3dnr use: gme motion data, filter motion data, gyro data.*/
    hi_void* ATTRIBUTE frame_dng_vir_addr;
} hi_video_supplement;

typedef COLOR_GAMUT_E hi_color_gamut;

typedef struct {
        hi_color_gamut color_gamut;
} hi_isp_colorgammut_info;

typedef DYNAMIC_RANGE_E hi_dynamic_range;

typedef DATA_BITWIDTH_E hi_data_bitwidth;

typedef struct {
    hi_u32              width;
    hi_u32              height;
    hi_video_field       field;
    hi_pixel_format      pixel_format;
    hi_video_format      video_format;
    hi_compress_mode     compress_mode;
    hi_dynamic_range     dynamic_range;
    hi_color_gamut       color_gamut;
    hi_u32              header_stride[3];
    hi_u32              stride[3];
    hi_u32              ext_stride[3];

    hi_u64              header_phy_addr[3];
    hi_u64              header_vir_addr[3];
    hi_u64              phy_addr[3];
    hi_u64              vir_addr[3];
    hi_u64              ext_phy_addr[3];
    hi_u64              ext_vir_addr[3];

    hi_s16              offset_top;        /* top offset of show area */
    hi_s16              offset_bottom;    /* bottom offset of show area */
    hi_s16              offset_left;        /* left offset of show area */
    hi_s16              offset_right;        /* right offset of show area */

    hi_u32              max_luminance;
    hi_u32              min_luminance;

    hi_u32              time_ref;
    hi_u64              pts;

    hi_u64              private_data;
    hi_u32              frame_flag;     /* frame_flag, can be OR operation. */
    hi_video_supplement  supplement;
} hi_video_frame;

typedef struct {
    hi_video_frame v_frame;
    hi_u32        pool_id;
    hi_mod_id      mod_id;
} hi_video_frame_info;

typedef struct {
    hi_u32 vb_size;

    hi_u32 head_stride;
    hi_u32 head_size;
    hi_u32 head_y_size;

    hi_u32 main_stride;
    hi_u32 main_size;
    hi_u32 main_y_size;

    hi_u32 ext_stride;
    hi_u32 ext_y_size;
} hi_vb_cal_config;

typedef struct {
    hi_pixel_format pixel_format;  /* bitmap's pixel format */
    hi_u32 width;               /* bitmap's width */
    hi_u32 height;              /* bitmap's height */
    hi_void* ATTRIBUTE data;      /* address of bitmap's data */
} hi_bitmap;

typedef struct {
    hi_bool    aspect;             /* RW;range: [0, 1];whether aspect ration  is keep */
    hi_s32     x_ratio;           /* RW; range: [0, 100]; field angle ration of  horizontal,valid when aspect=0.*/
    hi_s32     y_ratio;           /* RW; range: [0, 100]; field angle ration of  vertical,valid when aspect=0.*/
    hi_s32     xy_ratio;          /* RW; range: [0, 100]; field angle ration of  all,valid when aspect=1.*/
    hi_s32 center_x_offset;        /* RW; range: [-511, 511]; horizontal offset of the image distortion center relative to image center.*/
    hi_s32 center_y_offset;        /* RW; range: [-511, 511]; vertical offset of the image distortion center relative to image center.*/
    hi_s32 distortion_ratio;      /* RW; range: [-300, 500]; LDC distortion ratio.when spread on,distortion_ratio range should be [0, 500]*/
} hi_ldc_attr;

typedef struct {
    hi_s32 focal_len_x;                                            /* RW; focal length in horizontal direction, with 2 decimal numbers */
    hi_s32 focal_len_y;                                            /* RW; focal length in vertical direction, with 2 decimal numbers */
    hi_s32 coor_shift_x;                                           /* RW; coordinate of image center, with 2 decimal numbers */
    hi_s32 coor_shift_y;                                           /* RW; Y coordinate of image center, with 2 decimal numbers */
    hi_s32 src_cali_ratio[SRC_LENS_COEF_SEG][SRC_LENS_COEF_NUM];  /* RW; lens distortion coefficients of the source image, with 5 decimal numbers */
    hi_s32 src_jun_pt;                                             /* RW; junction point of the two segments */
    hi_s32 dst_cali_ratio[DST_LENS_COEF_SEG][DST_LENS_COEF_NUM];  /* RW; lens distortion coefficients, with 5 decimal numbers */
    hi_s32 dst_jun_pt[DST_LENS_COEF_SEG_POINT];                   /* RW; junction point of the three segments */
    hi_s32 max_du;                                                /* RW; max undistorted distance before 3rd polynomial drop, with 16bits decimal */
} hi_ldc_v2_attr;

typedef LDC_VIEW_TYPE_E hi_ldc_view_type;

typedef struct {
    hi_ldc_view_type view_type; /* RW; range: [0, 1], 0: all mode, 1: crop mode.*/
    hi_s32 center_x_offset;    /* RW; range: ABS(center_x_offset)->[0, min(128, width*0.08)], horizontal offset of the image distortion center relative to image center.*/
    hi_s32 center_y_offset;    /* RW; range: ABS(center_x_offset)->[0, min(128, height*0.08)], vertical offset of the image distortion center relative to image center.*/
    hi_s32 distortion_ratio;  /* RW; range: [-300, 300], LDC distortion ratio.*/
    hi_s32 min_ratio;         /* RW; range: [-300, 300], to remove the black region around when performing pinchusion distortion correction with crop mode.*/
} hi_ldc_v3_attr;

typedef ROTATION_VIEW_TYPE_E hi_rotation_view_type;

typedef struct {
    hi_rotation_view_type view_type;       /*RW;range: [0, 2];rotation mode*/
    hi_u32               angle;         /*RW;range: [0,360];rotation angle:[0,360]*/
    hi_s32               center_x_offset; /*RW;range: [-511, 511];horizontal offset of the image distortion center relative to image center.*/
    hi_s32               center_y_offset; /*RW;range: [-511, 511];vertical offset of the image distortion center relative to image center.*/
    hi_size               dest_size;       /*RW;dest size of any angle rotation*/
} hi_rotation_ex;

typedef WDR_MODE_E hi_wdr_mode;

typedef FRAME_INTERRUPT_TYPE_E hi_frame_interrupt_type;

typedef struct {
    hi_frame_interrupt_type int_type;
    hi_u32 early_line;
} hi_frame_interrupt_attr;

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* __HI_COMM_VIDEO_ADAPT_H__ */

