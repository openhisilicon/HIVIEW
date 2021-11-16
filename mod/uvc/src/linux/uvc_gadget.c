/*
 * UVC gadget test application
 *
 * Copyright (C) 2010 Ideas on board SPRL <laurent.pinchart@ideasonboard.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 */

#include <sys/time.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/select.h>

#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <errno.h>
#include <linux/usb/ch9.h>

#include "sample_venc.h"
#include "uvc.h"
#include "video.h"
#include "uvc_gadgete.h"
#include "uvc_venc_glue.h"
#include "histream.h"
#include "log.h"
#include "hi_ctrl.h"
#include "config_svc.h"

#if 1
 #define v4l2_fourcc(a, b, c, d) \
    ((__u32)(a) | ((__u32)(b) << 8) | ((__u32)(c) << 16) | ((__u32)(d) << 24))

 #define VIDIOC_S_FMT _IOWR('V', 5, struct v4l2_format)
 #define VIDIOC_REQBUFS _IOWR('V', 8, struct v4l2_requestbuffers)
 #define VIDIOC_QUERYBUF _IOWR('V', 9, struct v4l2_buffer)
 #define VIDIOC_QUERYCAP _IOR('V', 0, struct v4l2_capability)
 #define VIDIOC_STREAMOFF _IOW('V', 19, int)
 #define VIDIOC_QBUF _IOWR('V', 15, struct v4l2_buffer)
 #define VIDIOC_S_CTRL _IOWR('V', 28, struct v4l2_control)
 #define VIDIOC_S_DV_TIMINGS _IOWR('V', 87, struct v4l2_dv_timings)
 #define VIDIOC_G_DV_TIMINGS _IOWR('V', 88, struct v4l2_dv_timings)
 #define VIDIOC_DQEVENT _IOR('V', 89, struct v4l2_event)
 #define VIDIOC_SUBSCRIBE_EVENT _IOW('V', 90, struct v4l2_event_subscription)
 #define VIDIOC_UNSUBSCRIBE_EVENT _IOW('V', 91, struct v4l2_event_subscription)
 #define VIDIOC_STREAMON _IOW('V', 18, int)
 #define VIDIOC_DQBUF _IOWR('V', 17, struct v4l2_buffer)

 #define V4L2_PIX_FMT_YUYV v4l2_fourcc('Y', 'U', 'Y', 'V')/* 16  YUV 4:2:2     */
 #define V4L2_PIX_FMT_YUV420 v4l2_fourcc('Y', 'U', '1', '2')/* 16  YUV 4:2:0     */
 #define V4L2_PIX_FMT_NV12 v4l2_fourcc('N', 'V', '1', '2')/* 16  YUV 4:2:0     */
 #define V4L2_PIX_FMT_MJPEG v4l2_fourcc('M', 'J', 'P', 'G')/* Motion-JPEG   */
 #define V4L2_PIX_FMT_JPEG v4l2_fourcc('J', 'P', 'E', 'G')/* JFIF JPEG     */
 #define V4L2_PIX_FMT_DV v4l2_fourcc('d', 'v', 's', 'd')/* 1394          */
 #define V4L2_PIX_FMT_MPEG v4l2_fourcc('M', 'P', 'E', 'G')/* MPEG-1/2/4 Multiplexed */
 #define V4L2_PIX_FMT_H264 v4l2_fourcc('H', '2', '6', '4')/* H264 with start codes */
 #define V4L2_PIX_FMT_H264_NO_SC v4l2_fourcc('A', 'V', 'C', '1')/* H264 without start codes */
 #define V4L2_PIX_FMT_H264_MVC v4l2_fourcc('M', '2', '6', '4')/* H264 MVC */
 #define V4L2_PIX_FMT_H263 v4l2_fourcc('H', '2', '6', '3')/* H263          */
 #define V4L2_PIX_FMT_MPEG1 v4l2_fourcc('M', 'P', 'G', '1')/* MPEG-1 ES     */
 #define V4L2_PIX_FMT_MPEG2 v4l2_fourcc('M', 'P', 'G', '2')/* MPEG-2 ES     */
 #define V4L2_PIX_FMT_MPEG4 v4l2_fourcc('M', 'P', 'G', '4')/* MPEG-4 ES     */
 #define V4L2_PIX_FMT_XVID v4l2_fourcc('X', 'V', 'I', 'D')/* Xvid           */
 #define V4L2_PIX_FMT_VC1_ANNEX_G v4l2_fourcc('V', 'C', '1', 'G')/* SMPTE 421M Annex G compliant stream */
 #define V4L2_PIX_FMT_VC1_ANNEX_L v4l2_fourcc('V', 'C', '1', 'L')/* SMPTE 421M Annex L compliant stream */
 #define V4L2_PIX_FMT_VP8 v4l2_fourcc('V', 'P', '8', '0')/* VP8 */

 #define V4L2_EVENT_PRIVATE_START 0x08000000
 #define UVC_EVENT_FIRST (V4L2_EVENT_PRIVATE_START + 0)
 #define UVC_EVENT_CONNECT (V4L2_EVENT_PRIVATE_START + 0)
 #define UVC_EVENT_DISCONNECT (V4L2_EVENT_PRIVATE_START + 1)
 #define UVC_EVENT_STREAMON (V4L2_EVENT_PRIVATE_START + 2)
 #define UVC_EVENT_STREAMOFF (V4L2_EVENT_PRIVATE_START + 3)
 #define UVC_EVENT_SETUP (V4L2_EVENT_PRIVATE_START + 4)
 #define UVC_EVENT_DATA (V4L2_EVENT_PRIVATE_START + 5)
 #define UVC_EVENT_LAST (V4L2_EVENT_PRIVATE_START + 5)

/*struct uvc_streaming_control {
    __u16 bmHint;
    __u8  bFormatIndex;
    __u8  bFrameIndex;
    __u32 dwFrameInterval;
    __u16 wKeyFrameRate;
    __u16 wPFrameRate;
    __u16 wCompQuality;
    __u16 wCompWindowSize;
    __u16 wDelay;
    __u32 dwMaxVideoFrameSize;
    __u32 dwMaxPayloadTransferSize;
    __u32 dwClockFrequency;
    __u8  bmFramingInfo;
    __u8  bPreferedVersion;
    __u8  bMinVersion;
    __u8  bMaxVersion;
} __attribute__((__packed__));
 */
struct v4l2_event_subscription
{
    __u32 type;
    __u32 id;
    __u32 flags;
    __u32 reserved[5];
};

/* Payload for V4L2_EVENT_VSYNC */
struct v4l2_event_vsync
{
    /* Can be V4L2_FIELD_ANY, _NONE, _TOP or _BOTTOM */
    __u8 field;
} __attribute__ ((packed));

/* Payload for V4L2_EVENT_CTRL */
 #define V4L2_EVENT_CTRL_CH_VALUE (1 << 0)
 #define V4L2_EVENT_CTRL_CH_FLAGS (1 << 1)
 #define V4L2_EVENT_CTRL_CH_RANGE (1 << 2)

struct v4l2_event_ctrl
{
    __u32 changes;
    __u32 type;
    union
    {
        __s32 value;
        __s64 value64;
    };
    __u32 flags;
    __s32 minimum;
    __s32 maximum;
    __s32 step;
    __s32 default_value;
};

struct v4l2_event_frame_sync
{
    __u32 frame_sequence;
};

struct v4l2_event
{
    __u32 type;
    union
    {
        struct v4l2_event_vsync      vsync;
        struct v4l2_event_ctrl       ctrl;
        struct v4l2_event_frame_sync frame_sync;
        __u8                         data[64];
    }               u;
    __u32           pending;
    __u32           sequence;
    struct timespec timestamp;
    __u32           id;
    __u32           reserved[8];
};
enum v4l2_buf_type
{
    V4L2_BUF_TYPE_VIDEO_CAPTURE = 1,
    V4L2_BUF_TYPE_VIDEO_OUTPUT  = 2,
    V4L2_BUF_TYPE_VIDEO_OVERLAY = 3,
    V4L2_BUF_TYPE_VBI_CAPTURE = 4,
    V4L2_BUF_TYPE_VBI_OUTPUT = 5,
    V4L2_BUF_TYPE_SLICED_VBI_CAPTURE = 6,
    V4L2_BUF_TYPE_SLICED_VBI_OUTPUT = 7,
 #if 1
    /* Experimental */
    V4L2_BUF_TYPE_VIDEO_OUTPUT_OVERLAY = 8,
 #endif
    V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE = 9,
    V4L2_BUF_TYPE_VIDEO_OUTPUT_MPLANE = 10,

    /* Deprecated, do not use */
    V4L2_BUF_TYPE_PRIVATE = 0x80,
};

enum v4l2_memory
{
    V4L2_MEMORY_MMAP = 1,
    V4L2_MEMORY_USERPTR = 2,
    V4L2_MEMORY_OVERLAY = 3,
    V4L2_MEMORY_DMABUF = 4,
};

enum v4l2_field
{
    V4L2_FIELD_ANY  = 0,          /* driver can choose from none,
                                  top, bottom, interlaced
                                  depending on whatever it thinks
                                  is approximate ... */
    V4L2_FIELD_NONE = 1, /* this device has no fields ... */
    V4L2_FIELD_TOP = 2, /* top field only */
    V4L2_FIELD_BOTTOM = 3, /* bottom field only */
    V4L2_FIELD_INTERLACED = 4, /* both fields interlaced */
    V4L2_FIELD_SEQ_TB = 5,        /* both fields sequential into one
                                  buffer, top-bottom order */
    V4L2_FIELD_SEQ_BT = 6, /* same as above + bottom-top order */
    V4L2_FIELD_ALTERNATE = 7,     /* both fields alternating into
                                  separate buffers */
    V4L2_FIELD_INTERLACED_TB = 8, /* both fields interlaced, top field
                                  first and the top field is
                                  transmitted first */
    V4L2_FIELD_INTERLACED_BT = 9, /* both fields interlaced, top field
                                  first and the bottom field is
                                  transmitted first */
};
struct v4l2_capability
{
    __u8  driver[16];
    __u8  card[32];
    __u8  bus_info[32];
    __u32 version;
    __u32 capabilities;
    __u32 device_caps;
    __u32 reserved[3];
};
struct v4l2_timecode
{
    __u32 type;
    __u32 flags;
    __u8  frames;
    __u8  seconds;
    __u8  minutes;
    __u8  hours;
    __u8  userbits[4];
};
struct v4l2_buffer
{
    __u32                index;
    __u32                type;
    __u32                bytesused;
    __u32                flags;
    __u32                field;
    struct timeval       timestamp;
    struct v4l2_timecode timecode;
    __u32                sequence;

    /* memory location */
    __u32 memory;
    union
    {
        __u32              offset;
        unsigned long      userptr;
        struct v4l2_plane* planes;
        __s32              fd;
    }     m;
    __u32 length;
    __u32 reserved2;
    __u32 reserved;
};
struct v4l2_requestbuffers
{
    __u32 count;
    __u32 type;           /* enum v4l2_buf_type */
    __u32 memory;         /* enum v4l2_memory */
    __u32 reserved[2];
};
struct v4l2_pix_format
{
    __u32 width;
    __u32 height;
    __u32 pixelformat;
    __u32 field;          /* enum v4l2_field */
    __u32 bytesperline;   /* for padding, zero if unused */
    __u32 sizeimage;
    __u32 colorspace;     /* enum v4l2_colorspace */
    __u32 priv;           /* private data, depends on pixelformat */
};
 #define VIDEO_MAX_PLANES 8
struct v4l2_plane_pix_format
{
    __u32 sizeimage;
    __u16 bytesperline;
    __u16 reserved[7];
} __attribute__ ((packed));
struct v4l2_pix_format_mplane
{
    __u32 width;
    __u32 height;
    __u32 pixelformat;
    __u32 field;
    __u32 colorspace;

    struct v4l2_plane_pix_format plane_fmt[VIDEO_MAX_PLANES];
    __u8                         num_planes;
    __u8                         reserved[11];
} __attribute__ ((packed));
struct v4l2_rect
{
    __s32 left;
    __s32 top;
    __s32 width;
    __s32 height;
};
 #define __user
struct v4l2_clip
{
    struct v4l2_rect c;
    struct v4l2_clip __user* next;
};
struct v4l2_window
{
    struct v4l2_rect w;
    __u32            field;          /* enum v4l2_field */
    __u32            chromakey;
    struct v4l2_clip __user* clips;
    __u32            clipcount;
    void __user*     bitmap;
    __u8             global_alpha;
};
struct v4l2_vbi_format
{
    __u32 sampling_rate;          /* in 1 Hz */
    __u32 offset;
    __u32 samples_per_line;
    __u32 sample_format;          /* V4L2_PIX_FMT_* */
    __s32 start[2];
    __u32 count[2];
    __u32 flags;                  /* V4L2_VBI_* */
    __u32 reserved[2];            /* must be zero */
};
struct v4l2_sliced_vbi_format
{
    __u16 service_set;

    /* service_lines[0][...] specifies lines 0-23 (1-23 used) of the first field
       service_lines[1][...] specifies lines 0-23 (1-23 used) of the second field
       (equals frame lines 313-336 for 625 line video
       standards, 263-286 for 525 line standards) */
    __u16 service_lines[2][24];
    __u32 io_size;
    __u32 reserved[2];            /* must be zero */
};
struct v4l2_format
{
    __u32 type;
    union
    {
        struct v4l2_pix_format        pix;       /* V4L2_BUF_TYPE_VIDEO_CAPTURE */
        struct v4l2_pix_format_mplane pix_mp;  /* V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE */
        struct v4l2_window            win;       /* V4L2_BUF_TYPE_VIDEO_OVERLAY */
        struct v4l2_vbi_format        vbi;       /* V4L2_BUF_TYPE_VBI_CAPTURE */
        struct v4l2_sliced_vbi_format sliced;  /* V4L2_BUF_TYPE_SLICED_VBI_CAPTURE */
        __u8                          raw_data[200]; /* user-defined */
    } fmt;
};
#endif


#define clamp(val, min, max) ({                 \
                                  typeof(val)__val = (val);              \
                                  typeof(min)__min = (min);              \
                                  typeof(max)__max = (max);              \
                                  (void) (&__val == &__min);              \
                                  (void) (&__val == &__max);              \
                                  __val = __val < __min ? __min : __val;   \
                                  __val > __max ? __max : __val; })

#define ARRAY_SIZE(a) ((sizeof(a) / sizeof(a[0])))

#define CLEAR(x) memset (&(x), 0, sizeof (x))

typedef enum
{
    IO_METHOD_MMAP = 0x1,
    IO_METHOD_USERPTR = 0x2,
} io_method;

#define WAITED_NODE_SIZE (4)
static frame_node_t *__waited_node[WAITED_NODE_SIZE];

extern unsigned int g_bulk;
static void clear_waited_node()
{
    int i = 0;
    uvc_cache_t *uvc_cache = uvc_cache_get();

    for (i = 0; i < WAITED_NODE_SIZE; i++)
    {
        if ((__waited_node[i] != 0) && uvc_cache)
        {
            put_node_to_queue(uvc_cache->free_queue, __waited_node[i]);
            __waited_node[i] = 0;
        }
    }
}

static struct uvc_device*uvc_open(const char* devname)
{
    struct uvc_device* dev;
    struct v4l2_capability cap;
    int ret;
    int fd;

    fd = open(devname, O_RDWR | O_NONBLOCK);

    if (fd == -1)
    {
        LOG("v4l2 open failed(%s): %s (%d)\n",devname, strerror(errno), errno);
        return NULL;
    }

    ret = ioctl(fd, VIDIOC_QUERYCAP, &cap);

    if (ret < 0)
    {
        LOG("unable to query device: %s (%d)\n", strerror(errno),
            errno);
        close(fd);
        return NULL;
    }

    //LOG("open succeeded(%s:caps=0x%04x)\n", devname, cap.capabilities);

    /*
    V4L2_CAP_VIDEO_CAPTURE 0x00000001 support video Capture interface.
    V4L2_CAP_VIDEO_OUTPUT 0x00000002 support video output interface.
    V4L2_CAP_VIDEO_OVERLAY 0x00000004 support video cover interface.
    V4L2_CAP_VBI_CAPTURE 0x00000010 Original VBI Capture interface.
    V4L2_CAP_VBI_OUTPUT 0x00000020 Original VBI Output interface.
    V4L2_CAP_SLICED_VBI_CAPTURE 0x00000040 Sliced VBI Capture interface.
    V4L2_CAP_SLICED_VBI_OUTPUT 0x00000080 Sliced VBI Output interface.
    V4L2_CAP_RDS_CAPTURE 0x00000100 undefined
    */
    if (!(cap.capabilities & 0x00000002)) {
        close(fd);
        return NULL;
    }

    //LOG("device is %s on bus %s\n", cap.card, cap.bus_info);

    dev = (struct uvc_device*)malloc(sizeof * dev);

    if (dev == NULL)
    {
        close(fd);
        return NULL;
    }

    memset(dev, 0, sizeof * dev);
    dev->fd = fd;

    CLEAR(__waited_node);

    return dev;
}

static void uvc_close(struct uvc_device* dev)
{
    close(dev->fd);
    free(dev);
}

/* ---------------------------------------------------------------------------
 * Video streaming
 */

static void uvc_video_fill_buffer_userptr(struct uvc_device* dev, struct v4l2_buffer* buf)
{
    int retry_count = 0;
    buf->bytesused = 0;

    switch (dev->fcc)
    {
    case V4L2_PIX_FMT_MJPEG:
    case V4L2_PIX_FMT_H264:
    case V4L2_PIX_FMT_YUYV:
    case V4L2_PIX_FMT_YUV420:
    {
        uvc_cache_t *uvc_cache = uvc_cache_get();
        frame_node_t *node = 0;
        frame_queue_t *q = 0, *fq = 0;

retry:
        if (uvc_cache)
        {
            q  = uvc_cache->ok_queue;
            fq = uvc_cache->free_queue;
            get_node_from_queue(q, &node);
        }

        if ((__waited_node[buf->index] != 0) && uvc_cache)
        {
            put_node_to_queue(fq, __waited_node[buf->index]);
            __waited_node[buf->index] = 0;
        }

        if (node != 0)
        {
            buf->bytesused = node->used;
            buf->m.userptr = (unsigned long)node->mem;
            buf->length = node->length;
            __waited_node[buf->index] = node;
        }
        else if (retry_count++ < 60)
        {
            // the perfect solution is using locker and waiting queue's notify.
            // but here just only simply used usleep method and try again.
            // it works fine now.
            usleep(5000);
            goto retry;
        }
    }
        break;
    default:
        LOG("what's up....\n");
        break;
    }
}

static int uvc_video_process_userptr(struct uvc_device* dev)
{
    struct v4l2_buffer buf;
    int ret;

    // INFO("#############uvc_video_process_userptr\n");

    memset(&buf, 0, sizeof buf);
    buf.type   = V4L2_BUF_TYPE_VIDEO_OUTPUT;
    buf.memory = V4L2_MEMORY_USERPTR;

    if ((ret = ioctl(dev->fd, VIDIOC_DQBUF, &buf)) < 0)
    {
        return ret;
    }

    uvc_video_fill_buffer_userptr(dev, &buf);

    if ((ret = ioctl(dev->fd, VIDIOC_QBUF, &buf)) < 0)
    {
        LOG("Unable to requeue buffer: %s (%d).\n", strerror(errno), errno);
        return ret;
    }

    return 0;
}

static int uvc_video_reqbufs_userptr(struct uvc_device* dev, int nbufs)
{
    struct v4l2_requestbuffers rb;
    int ret;

    dev->nbufs = 0;

    memset(&rb, 0, sizeof rb);
    rb.count = nbufs;
    rb.type   = V4L2_BUF_TYPE_VIDEO_OUTPUT;
    rb.memory = V4L2_MEMORY_USERPTR;

    ret = ioctl(dev->fd, VIDIOC_REQBUFS, &rb);

    if (ret < 0)
    {
        INFO("Unable to allocate buffers: %s (%d).\n",
             strerror(errno), errno);
        return ret;
    }

    dev->nbufs = rb.count;

    // INFO("%u buffers allocated.\n", rb.count);

    return 0;
}

static int uvc_video_stream_userptr(struct uvc_device* dev, int enable)
{
    struct v4l2_buffer buf;
    unsigned int i;
    int type = V4L2_BUF_TYPE_VIDEO_OUTPUT;
    int ret = 0;

    // INFO("%s:Starting video stream.\n", __func__);

    for (i = 0; i < (dev->nbufs); ++i)
    {
        memset(&buf, 0, sizeof buf);

        buf.index = i;
        buf.type   = V4L2_BUF_TYPE_VIDEO_OUTPUT;
        buf.memory = V4L2_MEMORY_USERPTR;

        uvc_video_fill_buffer_userptr(dev, &buf);

        if ((ret = ioctl(dev->fd, VIDIOC_QBUF, &buf)) < 0)
        {
            INFO("Unable to queue buffer(%d): %s (%d).\n", i,
                 strerror(errno), errno);
            break;
        }
    }

    ioctl(dev->fd, VIDIOC_STREAMON, &type);
    dev->streaming = 1;

    return ret;
}

static int uvc_video_set_format(struct uvc_device* dev)
{
    struct v4l2_format fmt;
    int ret;

    memset(&fmt, 0, sizeof fmt);
    fmt.type = V4L2_BUF_TYPE_VIDEO_OUTPUT;
    fmt.fmt.pix.width  = dev->width;
    fmt.fmt.pix.height = dev->height;
    fmt.fmt.pix.pixelformat = dev->fcc;
    fmt.fmt.pix.field = V4L2_FIELD_NONE;

    if ((dev->fcc == V4L2_PIX_FMT_MJPEG) || (dev->fcc == V4L2_PIX_FMT_H264))
    {
        fmt.fmt.pix.sizeimage = dev->imgsize;
    }

    if ((ret = ioctl(dev->fd, VIDIOC_S_FMT, &fmt)) < 0)
    {
        LOG("Unable to set format: %s (%d).\n",
            strerror(errno), errno);
    }

    return ret;
}

static int uvc_video_init(struct uvc_device* dev __attribute__ ((__unused__)))
{
    return 0;
}

/* ---------------------------------------------------------------------------
 * Request processing
 */

struct uvc_frame_info
{
    unsigned int width;
    unsigned int height;
    unsigned int intervals[8];
};

struct uvc_format_info
{
    unsigned int                 fcc;
    const struct uvc_frame_info* frames;
};

static const struct uvc_frame_info uvc_frames_yuyv[] =
{
    {  640,  360, {333333,       0 }, },
    { 1280,  720, {333333,       0 }, },
    { 1920, 1080, {333333,       0 }, },
    { 3840, 2160, {333333,       0 }, },
    {    0,    0, {     0,         }, },
};

static const struct uvc_frame_info uvc_frames_mjpeg[] =
{
    {  640,  360, {333333,       0 }, },
    { 1280,  720, {333333,       0 }, },
    { 1920, 1080, {333333,       0 }, },
    { 3840, 2160, {333333,       0 }, },
    {    0,    0, {     0,         }, },
};

static const struct uvc_frame_info uvc_frames_h264[] =
{
    {  640,  360, {333333,       0 }, },
    { 1280,  720, {333333,       0 }, },
    { 1920, 1080, {333333,       0 }, },
    { 3840, 2160, {333333,       0 }, },
    {    0,    0, {     0,         }, },
};

static const struct uvc_format_info uvc_formats[] =
{ //yuv first
    { V4L2_PIX_FMT_YUYV,  uvc_frames_yuyv  },
    { V4L2_PIX_FMT_MJPEG, uvc_frames_mjpeg },
    { V4L2_PIX_FMT_H264,  uvc_frames_h264  },
};

static void uvc_streamoff(struct uvc_device *dev)
{
    int type = V4L2_BUF_TYPE_VIDEO_OUTPUT;

    ioctl(dev->fd, VIDIOC_STREAMOFF, &type);
    histream_shutdown();
    dev->streaming = 0;

    // INFO("Stopping video stream.\n");
}

static void disable_uvc_video(struct uvc_device* dev)
{
    uvc_streamoff(dev);
    clear_waited_node();
}

static void enable_uvc_video(struct uvc_device* dev)
{
    encoder_property p;

    clear_ok_queue();
    clear_waited_node();
    disable_uvc_video(dev);

    p.format = dev->fcc;
    p.width    = dev->width;
    p.height   = dev->height;
    p.compsite = 0;

    histream_set_enc_property(&p);
    histream_shutdown();
    histream_startup();

    uvc_video_reqbufs_userptr(dev, WAITED_NODE_SIZE);
    uvc_video_stream_userptr(dev, 1);
}

static void uvc_fill_streaming_control(struct uvc_device* dev,
                                       struct uvc_streaming_control* ctrl,
                                       int iframe, int iformat)
{
    const struct uvc_format_info* format;
    const struct uvc_frame_info* frame;
    unsigned int nframes;

    if (iformat < 0)
    {
        iformat = ARRAY_SIZE(uvc_formats) + iformat;
    }

    if ((iformat < 0) || (iformat >= (int)ARRAY_SIZE(uvc_formats)))
    {
        return;
    }

    // INFO("iformat = %d\n", iformat);
    format = &uvc_formats[iformat];

    nframes = 0;

    while (format->frames[nframes].width != 0)
    {
        ++nframes;
    }

    if (iframe < 0)
    {
        iframe = nframes + iframe;
    }

    if ((iframe < 0) || (iframe >= (int)nframes))
    {
        return;
    }

    frame = &format->frames[iframe];

    memset(ctrl, 0, sizeof * ctrl);

    ctrl->bmHint = 1;
    ctrl->bFormatIndex = iformat + 1; // 1 is yuv ,2 is mjpeg
    ctrl->bFrameIndex = iframe + 1; //360 1 720 2
    ctrl->dwFrameInterval = frame->intervals[0]; //dui ying di ji ge zhenlv
    
	//INFO("bFormatIndex = %d, bFrameIndex = %d\n", ctrl->bFormatIndex, ctrl->bFrameIndex);
    
    switch (format->fcc)
    {
    case V4L2_PIX_FMT_YUYV:
    case V4L2_PIX_FMT_YUV420:
        ctrl->dwMaxVideoFrameSize = frame->width * frame->height * 2;
        break;

    case V4L2_PIX_FMT_MJPEG:
    case V4L2_PIX_FMT_H264:

        ctrl->dwMaxVideoFrameSize = dev->imgsize;
        break;
    }

    if (dev->bulk) {
        ctrl->dwMaxPayloadTransferSize = dev->bulk_size;   /* TODO this should be filled by the driver. */
    } else {
        ctrl->dwMaxPayloadTransferSize = 3072;
    }
    ctrl->bmFramingInfo = 3;
    ctrl->bPreferedVersion = 1;
    ctrl->bMaxVersion = 1;

}

static void uvc_events_process_standard(struct uvc_device* dev, struct usb_ctrlrequest* ctrl,
                                        struct uvc_request_data* resp)
{
    INFO("standard request\n");
    (void)dev;
    (void)ctrl;
    (void)resp;
}

static void uvc_event_undefined_control(struct uvc_device *dev,
                                        uint8_t req,
                                        uint8_t cs,
                                        struct uvc_request_data *resp)
{
    switch (cs)
    {
    case UVC_VC_REQUEST_ERROR_CODE_CONTROL:
        resp->length = dev->request_error_code.length;
        resp->data[0] = dev->request_error_code.data[0];
        //printf("dev->request_error_code.data[0] = %d\n",dev->request_error_code.data[0]);
        break;
    default:
        dev->request_error_code.length = 1;
        dev->request_error_code.data[0] = 0x06;
        break;
    }
}

static void uvc_events_process_control(struct uvc_device*       dev,
                                       uint8_t                  req,
                                       uint8_t                  unit_id,
                                       uint8_t                  cs,
                                       struct uvc_request_data* resp)
{
    switch (unit_id)
    {
        case 0:
            uvc_event_undefined_control(dev, req, cs,  resp);
            break;
        case 1:
            histream_event_it_control(dev, req, unit_id, cs, resp);
            break;
        case 2:
            histream_event_pu_control(dev, req, unit_id, cs, resp);
            break;
        case 10:
            histream_event_eu_h264_control(req, unit_id, cs, resp);
            break;
        default:
            dev->request_error_code.length = 1;
            dev->request_error_code.data[0] = 0x06;
    }
}

static void uvc_events_process_streaming(struct uvc_device* dev, uint8_t req, uint8_t cs,
                                         struct uvc_request_data* resp)
{
    struct uvc_streaming_control* ctrl;

    if ((cs != UVC_VS_PROBE_CONTROL) && (cs != UVC_VS_COMMIT_CONTROL))
    {
        return;
    }

    ctrl = (struct uvc_streaming_control*)&resp->data;
    resp->length = sizeof * ctrl;

    //request
    switch (req)
    {
        //0x01
    case UVC_SET_CUR:
        dev->control = cs;
        resp->length = 34;
        break;

        //0x81
    case UVC_GET_CUR:
        if (cs == UVC_VS_PROBE_CONTROL)
        {
            memcpy(ctrl, &dev->probe, sizeof * ctrl);
        }
        else
        {
            memcpy(ctrl, &dev->commit, sizeof * ctrl);
        }

        break;

        //0x82
    case UVC_GET_MIN:

        //0x83
    case UVC_GET_MAX:
        //uvc_fill_streaming_control(dev, ctrl, 0, 0);
        //break;

        //0x87
    case UVC_GET_DEF:
        uvc_fill_streaming_control(dev, ctrl, req == UVC_GET_MAX ? -1 : 0,
                                   req == UVC_GET_MAX ? -1 : 0);
        break;

        //0x84
    case UVC_GET_RES:
        memset(ctrl, 0, sizeof * ctrl);
        break;

        //0x85
    case UVC_GET_LEN:
        resp->data[0] = 0x00;
        resp->data[1] = 0x22;
        resp->length = 2;
        break;

        //0x86
    case UVC_GET_INFO:
        resp->data[0] = 0x03;
        resp->length = 1;
        break;
    }
}

#if 0
static const char* getcode_s(int c)
{
    if (c == 0x01)
    {
        return "SET_CUR";
    }
    else if (c == 0x81)
    {
        return "GET_CUR	";
    }
    else if (c == 0x82)
    {
        return "GET_MIN";
    }
    else if (c == 0x83)
    {
        return "GET_MAX";
    }
    else if (c == 0x84)
    {
        return "GET_RES";
    }
    else if (c == 0x85)
    {
        return "GET_LEN";
    }
    else if (c == 0x86)
    {
        return "GET_INFO";
    }
    else if (c == 0x87)
    {
        return "GET_DEF";
    }
    else
    {
        return "UNKNOW";
    }
}

static const char *get_interface_cs_s(int c)
{
    if (c == 0x01)
    {
        return "PROB_CONTROL";
    }
    else if (c == 0x02)
    {
        return "COMMIT_CONTROL";
    }
    else
    {
        return "UNKOWN";
    }
}
#endif

static void set_probe_status(struct uvc_device* dev, int cs, int req)
{
    if (cs == 0x01)
    {
        switch (req)
        {
        case 0x01:
        {
            dev->probe_status.set = 1;
        }
            break;
        case 0x81:
        {
            dev->probe_status.get = 1;
        }
            break;
        case 0x82:
        {
            dev->probe_status.min = 1;
        }
            break;
        case 0x83:
        {
            dev->probe_status.max = 1;
        }
            break;
        case 0x84:
        {}
            break;

        case 0x85:
        {}
            break;

        case 0x86:
        {}
            break;
        }
    }
}

static int check_probe_status(struct uvc_device* dev)
{
    if ((dev->probe_status.get == 1)
       && (dev->probe_status.set == 1)
       && (dev->probe_status.min == 1)
       && (dev->probe_status.max == 1))
    {
        return 1;
    }

    RLOG("the probe status is not correct...\n");

    return 0;
}

static void uvc_events_process_class(struct uvc_device* dev, struct usb_ctrlrequest* ctrl,
                                     struct uvc_request_data* resp)
{
    unsigned char probe_status = 1;

    if (probe_status)
    {
        unsigned char type = ctrl->bRequestType & USB_RECIP_MASK;
        switch (type)
        {
        case USB_RECIP_INTERFACE:
#if 0
           // LOG("reqeust type :INTERFACE\n");
           // LOG("interface : %d\n", (ctrl->wIndex &0xff));
            LOG("unit id : %d\n", ((ctrl->wIndex & 0xff00)>>8));
            LOG("cs code : 0x%02x(%s)\n", (ctrl->wValue >> 8), (char*)get_interface_cs_s((ctrl->wValue >> 8)));
            LOG("req code: 0x%02x(%s)\n", ctrl->bRequest, (char*)getcode_s(ctrl->bRequest));
            LOG("################################################\n");
#endif
            set_probe_status(dev, (ctrl->wValue >> 8), ctrl->bRequest);
            break;
        case USB_RECIP_DEVICE:
            LOG("request type :DEVICE\n");
            break;
        case USB_RECIP_ENDPOINT:
            LOG("request type :ENDPOINT\n");
            break;
        case USB_RECIP_OTHER:
            LOG("request type :OTHER\n");
            break;
        }
    }

    if ((ctrl->bRequestType & USB_RECIP_MASK) != USB_RECIP_INTERFACE)
    {
        return;
    }

    //save unit id, interface id and control selector
    dev->control = (ctrl->wValue >> 8);
    dev->unit_id = ((ctrl->wIndex & 0xff00) >> 8);
    dev->interface_id = (ctrl->wIndex & 0xff);

    switch (ctrl->wIndex & 0xff)
    {
        //0x0   0x100
    case UVC_INTF_CONTROL:
        uvc_events_process_control(dev, ctrl->bRequest, ctrl->wIndex >> 8, ctrl->wValue >> 8, resp);
        break;

        //0x1
    case UVC_INTF_STREAMING:
        uvc_events_process_streaming(dev, ctrl->bRequest, ctrl->wValue >> 8, resp);
        break;

    default:
        break;
    }
}

static void uvc_events_process_setup(struct uvc_device* dev, struct usb_ctrlrequest* ctrl,
                                     struct uvc_request_data* resp)
{
    dev->control = 0;
    dev->unit_id = 0;
    dev->interface_id = 0;

    switch (ctrl->bRequestType & USB_TYPE_MASK)
    {
    case USB_TYPE_STANDARD:
        uvc_events_process_standard(dev, ctrl, resp);
        break;

    case USB_TYPE_CLASS:
        uvc_events_process_class(dev, ctrl, resp);
        break;

    default:
        break;
    }
}

static const char* to_string(unsigned int format)
{
    switch (format)
    {
        case V4L2_PIX_FMT_H264:
            return "H264";
            break;

        case V4L2_PIX_FMT_MJPEG:
            return "MJPEG";
            break;

        case V4L2_PIX_FMT_YUYV:
            return "YUYV";
            break;

        case V4L2_PIX_FMT_YUV420:
            return "YUV420";
            break;

        default:
            return "unknown format";
            break;
    }
}

static void handle_control_interface_data(struct uvc_device *dev, struct uvc_request_data *data)
{
    switch (dev->unit_id)
    {
        case 1:
            histream_event_it_data(dev,dev->unit_id, dev->control, data);
            break;
        case 2:
            histream_event_pu_data(dev->unit_id, dev->control, data);
            break;
        case 10:
            histream_event_eu_h264_data(dev->unit_id, dev->control, data);
            break;
        default:
            break;
    }
#if 0
    if (dev->unit_id == UNIT_XU_H264)
    {
        histream_event_eu_h264_data(dev->unit_id, dev->control, data);
    }
    else if (dev->unit_id == UNIT_XU_HICAMERA)
    {
        histream_event_eu_camera_data(dev->unit_id, dev->control, data);
    }
    else if(dev->unit_id == UVC_VC_PROCESSING_UNIT)
    {
        histream_event_pu_data(dev->unit_id, dev->control, data);
    }
    else if(dev->unit_id == UVC_VC_INPUT_TERMINAL)
    {
        histream_event_it_data(dev,dev->unit_id, dev->control, data);
    }
#endif
}

static void uvc_events_process_data(struct uvc_device* dev, struct uvc_request_data* data)
{
    struct uvc_streaming_control* target;
    struct uvc_streaming_control* ctrl;
    const struct uvc_format_info* format;
    const struct uvc_frame_info* frame;
    const unsigned int* interval;
    unsigned int iformat, iframe;
    unsigned int nframes;

    if ((dev->unit_id != 0) && (dev->interface_id == UVC_INTF_CONTROL))
    {
        return handle_control_interface_data(dev, data);
    }

    switch (dev->control)
    {
	    case UVC_VS_PROBE_CONTROL:
	   //     INFO("setting probe control, length = %d\n", data->length);
	        target = &dev->probe;
	        break;

	    case UVC_VS_COMMIT_CONTROL:
	    //    INFO("setting commit control, length = %d\n", data->length);
	        target = &dev->commit;
	        break;

	    default:
	    //    INFO("setting unknown control, length = %d\n", data->length);
	        return;
    }

    ctrl = (struct uvc_streaming_control*)&data->data;

    // INFO("ctrl->bFormatIndex = %d\n", (unsigned int)ctrl->bFormatIndex);

    iformat = clamp((unsigned int)ctrl->bFormatIndex, 1U,
                    (unsigned int)ARRAY_SIZE(uvc_formats));

    //RLOG("set iformat = %d \n", iformat);

    format = &uvc_formats[iformat - 1];

    nframes = 0;

    // INFO("format->frames[nframes].width: %d\n", format->frames[nframes].width);
    // INFO("format->frames[nframes].height: %d\n", format->frames[nframes].height);

    while (format->frames[nframes].width != 0)
    {
        ++nframes;
    }

    iframe = clamp((unsigned int)ctrl->bFrameIndex, 1U, nframes);
    frame = &format->frames[iframe - 1];
    interval = frame->intervals;

    while ((interval[0] < ctrl->dwFrameInterval) && interval[1])
    {
        ++interval;
    }

    target->bFormatIndex = iformat;
    target->bFrameIndex = iframe;

    //RLOG("bFormatIndex = %d, bFrameIndex = %d\n", target->bFormatIndex, target->bFrameIndex);

    switch (format->fcc)
    {
        case V4L2_PIX_FMT_YUYV:
            target->dwMaxVideoFrameSize = frame->width * frame->height * 2;
            break;
        case V4L2_PIX_FMT_YUV420:
            target->dwMaxVideoFrameSize = frame->width * frame->height * 1.5;
            break;

        case V4L2_PIX_FMT_MJPEG:
        case V4L2_PIX_FMT_H264:

            if (dev->imgsize == 0)
            {
                LOG("WARNING: MJPEG requested and no image loaded.\n");
            }

            target->dwMaxVideoFrameSize = dev->imgsize;
            break;
    }

    target->dwFrameInterval = *interval;

    #if 0
    INFO("set interval=%d format=%d frame=%d dwMaxPayloadTransferSize=%d ctrl->dwMaxPayloadTransferSize = %d\n",
         target->dwFrameInterval,
         target->bFormatIndex,
         target->bFrameIndex,
         target->dwMaxPayloadTransferSize,
         ctrl->dwMaxPayloadTransferSize);
    #endif

    if ((dev->control == UVC_VS_COMMIT_CONTROL) && check_probe_status(dev))
    {
        dev->fcc    = format->fcc;
        dev->width  = frame->width;
        dev->height = frame->height;

        uvc_video_set_format(dev);
        INFO("\nset device format=%s width=%d height=%d\n", to_string(dev->fcc), dev->width, dev->height);

        if (dev->bulk != 0)
        {
            disable_uvc_video(dev);
            enable_uvc_video(dev);
        }
    }

    if (dev->control == UVC_VS_COMMIT_CONTROL)
    {
        memset(&dev->probe_status, 0, sizeof (dev->probe_status));
    }
}

static void uvc_events_process(struct uvc_device* dev)
{
    struct v4l2_event v4l2_event;
    struct uvc_event* uvc_event = (struct uvc_event*)(void*)&v4l2_event.u.data;
    struct uvc_request_data resp;
    int ret;

    // INFO("#############uvc_events_process\n");

    ret = ioctl(dev->fd, VIDIOC_DQEVENT, &v4l2_event);

    if (ret < 0)
    {
        LOG("VIDIOC_DQEVENT failed: %s (%d)\n", strerror(errno),
            errno);
        return;
    }

    memset(&resp, 0, sizeof resp);

    resp.length = -EL2HLT;

    switch (v4l2_event.type)
    {
        //0x08000000
    case UVC_EVENT_CONNECT:
        INFO("handle connect event \n");
        return;
        //0x08000001
    case UVC_EVENT_DISCONNECT:
        INFO("handle disconnect event\n");
        return;

        //0x08000004   UVC class
    case UVC_EVENT_SETUP:
        // INFO("handle setup event\n");
        uvc_events_process_setup(dev, &uvc_event->req, &resp);
        break;

        //0x08000005
    case UVC_EVENT_DATA:
        // INFO("handle data event\n");
        uvc_events_process_data(dev, &uvc_event->data);
        return;

        //0x08000002
    case UVC_EVENT_STREAMON:
        INFO("UVC_EVENT_STREAMON\n");
        if (!dev->bulk)
        {
            enable_uvc_video(dev);
        }
        return;

        //0x08000003
    case UVC_EVENT_STREAMOFF:
        INFO("UVC_EVENT_STREAMOFF\n");
        if (!dev->bulk)
        {
            disable_uvc_video(dev);
        }

        return;
    }

    ret = ioctl(dev->fd, UVCIOC_SEND_RESPONSE, &resp);
    if (ret < 0)
    {
        LOG("UVCIOC_S_EVENT failed: %s (%d)\n", strerror(errno),
            errno);
        return;
    }
}

static void uvc_events_init(struct uvc_device* dev)
{
    struct v4l2_event_subscription sub;

    uvc_fill_streaming_control(dev, &dev->probe, 0, 0);
    uvc_fill_streaming_control(dev, &dev->commit, 0, 0);

    if (dev->bulk)
    {
        /* FIXME Crude hack, must be negotiated with the driver. */
        dev->probe.dwMaxPayloadTransferSize  = dev->bulk_size;
        dev->commit.dwMaxPayloadTransferSize = dev->bulk_size;
    }

    memset(&sub, 0, sizeof sub);
    sub.type = UVC_EVENT_SETUP;
    ioctl(dev->fd, VIDIOC_SUBSCRIBE_EVENT, &sub);
    sub.type = UVC_EVENT_DATA;
    ioctl(dev->fd, VIDIOC_SUBSCRIBE_EVENT, &sub);
    sub.type = UVC_EVENT_STREAMON;
    ioctl(dev->fd, VIDIOC_SUBSCRIBE_EVENT, &sub);
    sub.type = UVC_EVENT_STREAMOFF;
    ioctl(dev->fd, VIDIOC_SUBSCRIBE_EVENT, &sub);
}

/* ---------------------------------------------------------------------------
 * main
 */

static struct uvc_device *__uvc_device = 0;

int open_uvc_device()
{
    struct uvc_device *dev;
    const char *devpath = "/dev/video0";

    dev = uvc_open(devpath);
    if (dev == 0)
    {
        return -1;
    }

    dev->imgsize  = get_config_value("uvc:imagesize", 1843200);
    dev->bulk = g_bulk;
    dev->bulk_size = get_config_value("uvc:bulksize", 1843200);

    uvc_events_init(dev);
    uvc_video_init(dev);

    __uvc_device = dev;

    return 0;
}

int close_uvc_device()
{
    if (__uvc_device != 0)
    {
        disable_uvc_video(__uvc_device);
        uvc_close(__uvc_device);
    }

    __uvc_device = 0;

    return 0;
}

int run_uvc_data()
{
    fd_set efds, wfds;
    struct timeval tv;
    int r;

    if (!__uvc_device)
    {
        return -1;
    }

    tv.tv_sec  = 1;
    tv.tv_usec = 0;

    FD_ZERO(&efds);
    FD_ZERO(&wfds);

    if (__uvc_device->streaming == 1)
    {
        FD_SET(__uvc_device->fd, &wfds);
    }

    r = select(__uvc_device->fd + 1, NULL, &wfds, NULL, &tv);
    if (r > 0)
    {
        if (FD_ISSET(__uvc_device->fd, &efds))
        {
        }
        if (FD_ISSET(__uvc_device->fd, &wfds))
        {
            uvc_video_process_userptr(__uvc_device);
        }
    }

    return r;
}

int run_uvc_device()
{
    fd_set efds, wfds;
    struct timeval tv;
    int r;

    if (!__uvc_device)
    {
        return -1;
    }

    tv.tv_sec  = 1;
    tv.tv_usec = 0;

    FD_ZERO(&efds);
    FD_ZERO(&wfds);
    FD_SET(__uvc_device->fd, &efds);

    if (__uvc_device->streaming == 1)
    {
        // FD_SET(__uvc_device->fd, &wfds);
    }

    r = select(__uvc_device->fd + 1, NULL, NULL, &efds, &tv);
    if (r > 0)
    {
        if (FD_ISSET(__uvc_device->fd, &efds))
        {
            uvc_events_process(__uvc_device);
        }

        if (FD_ISSET(__uvc_device->fd, &wfds))
        {
            // uvc_video_process_userptr(__uvc_device);
        }
    }

    return r;
}
