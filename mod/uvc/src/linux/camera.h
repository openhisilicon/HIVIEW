#ifndef __CAMERA_H__
#define __CAMERA_H__

#ifdef __HuaweiLite__
#define sample_v4l2_fourcc(a, b, c, d) \
    ((unsigned int)(a) | ((unsigned int)(b) << 8) | ((unsigned int)(c) << 16) | ((unsigned int)(d) << 24))

#define SAMPLE_V4L2_PIX_FMT_YUYV sample_v4l2_fourcc('Y', 'U', 'Y', 'V')/* 16  YUV 4:2:2     */
#define SAMPLE_V4L2_PIX_FMT_MJPEG sample_v4l2_fourcc('M', 'J', 'P', 'G')/* Motion-JPEG   */
#define SAMPLE_V4L2_PIX_FMT_H264 sample_v4l2_fourcc('H', '2', '6', '4')/* H264 with start codes */
#endif

typedef struct hicamera
{
    int (*init)(void);
    int (*deinit)(void);
    int (*open)(void);
    int (*close)(void);
    int (*run)(void);
    int (*stop)(void);
} hicamera;

hicamera *get_hicamera(void);

void sample_venc_config(void);
void sample_audio_config(void);

#endif //__CAMERA_H__

