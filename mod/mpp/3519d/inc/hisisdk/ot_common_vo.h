/*
  Copyright (c), 2001-2024, Shenshu Tech. Co., Ltd.
 */

#ifndef OT_COMMON_VO_H
#define OT_COMMON_VO_H

#include "ot_errno.h"
#include "ot_common_video.h"
#include "ot_common_vo_dev.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* end of #ifdef __cplusplus */

typedef enum {
    OT_VO_PARTITION_MODE_SINGLE = 0, /* Single partition, use software to make multi-picture in one hardware cell */
    OT_VO_PARTITION_MODE_MULTI = 1,  /* Multi partition, each partition is a hardware cell */
    OT_VO_PARTITION_MODE_BUTT,
} ot_vo_partition_mode;

typedef struct {
    ot_rect display_rect; /* RW; display resolution */
    ot_size img_size; /* RW; canvas size of the video layer */
    td_u32 display_frame_rate; /* RW; display frame rate */
    ot_pixel_format pixel_format; /* RW; pixel format of the video layer */
    td_bool double_frame_en; /* RW; whether to double frames */
    td_bool cluster_mode_en; /* RW; whether to take cluster way to use memory */
    ot_dynamic_range dst_dynamic_range; /* RW; video layer output dynamic range type */
    td_u32 display_buf_len;            /* RW; Video Layer display buffer length */
    ot_vo_partition_mode partition_mode;  /* RW; Video Layer partition mode */
    ot_compress_mode compress_mode; /* RW; Compressing mode */
} ot_vo_video_layer_attr;

typedef struct {
    td_bool enable; /* RW; hsharpen enable */
    td_u32 peak_ratio; /* RW; range: [0, 255];can change sharpen strength */
} ot_vo_layer_hsharpen_param;

typedef struct {
    td_bool hor_split_en; /* RW; horizontal two picture middle edge optimize */
} ot_vo_layer_param;

typedef struct {
    td_u32 priority; /* RW; video out overlay priority sd */
    ot_rect rect; /* RW; rectangle of video output channel */
    td_bool deflicker_en; /* RW; deflicker or not sd */
} ot_vo_chn_attr;

typedef struct {
    ot_aspect_ratio aspect_ratio; /* RW; aspect ratio */
} ot_vo_chn_param;

typedef enum {
    OT_VO_ZOOM_IN_RECT = 0, /* Zoom in by rect */
    OT_VO_ZOOM_IN_RATIO = 1, /* Zoom in by ratio */
    OT_VO_ZOOM_IN_BUTT,
} ot_vo_zoom_in_type;

typedef struct {
    /* RW; range: [0, 1000]; x_ratio = x * 1000 / W, x means start point to be zoomed, W means channel's width. */
    td_u32 x_ratio;
    /* RW; range: [0, 1000]; y_ratio = y * 1000 / H, y means start point to be zoomed, H means channel's height. */
    td_u32 y_ratio;
    /* RW; range: [0, 1000]; width_ratio = w * 1000 / W, w means width to be zoomed, W means channel's width. */
    td_u32 width_ratio;
    /* RW; range: [0, 1000]; height_ratio = h * 1000 / H, h means height to be zoomed, H means channel's height. */
    td_u32 height_ratio;
} ot_vo_zoom_ratio;

typedef struct {
    ot_vo_zoom_in_type zoom_type; /* RW; choose the type of zoom in */
    union {
        ot_rect zoom_rect; /* RW; zoom in by rect. AUTO:ot_vo_zoom_in_type:OT_VO_ZOOM_IN_RECT; */
        ot_vo_zoom_ratio zoom_ratio; /* RW; zoom in by ratio. AUTO:ot_vo_zoom_in_type:OT_VO_ZOOM_IN_RATIO; */
    };
} ot_vo_zoom_attr;

typedef struct {
    td_bool enable; /* RW; do frame or not */
    ot_border border; /* RW; frame's top, bottom, left, right width and color */
} ot_vo_border;

typedef enum  {
    OT_VO_MIRROR_NONE = 0, /* Mirror mode is none */
    OT_VO_MIRROR_HOR  = 1, /* Mirror mode is horizontal mirror */
    OT_VO_MIRROR_VER  = 2, /* Mirror mode is vertical mirror */
    OT_VO_MIRROR_BOTH = 3, /* Mirror mode is horizontal and vertical mirror */
    OT_VO_MIRROR_BUTT
} ot_vo_mirror_mode;

typedef struct {
    td_u32 chn_buf_used; /* R; channel buffer that been used */
    td_u32 chn_busy_num; /* R; busy channel buffer number */
} ot_vo_chn_status;

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* end of #ifdef __cplusplus */

#endif /* end of #ifndef OT_COMMON_VO_H */
