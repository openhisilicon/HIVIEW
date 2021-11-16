#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "camera.h"
#include "frame_cache.h"

#ifndef __HuaweiLite__
#include "config_svc.h"
#endif

unsigned int g_uvc = 1;
unsigned int g_uac = 1;
unsigned int g_bulk = 0;

#ifdef __HuaweiLite__
struct fuvc_frame_info
{
    unsigned int width;
    unsigned int height;
    unsigned int intervals[8];
};

struct fuvc_format_info
{
    unsigned int fcc;
    const struct fuvc_frame_info *frames;
};

static const struct fuvc_frame_info uvc_frames_yuyv[] =
{
    {  640,  480, {333333,       0 }, },
    { 1280,  720, {333333,       0 }, },
    {    0,    0, {     0,         }, },
};

static const struct fuvc_frame_info uvc_frames_mjpeg[] =
{
    {  640,  480, {333333,       0 }, },
    { 1280,  720, {333333,       0 }, },
    { 1920, 1080, {333333,       0 }, },
    {    0,    0, {     0,         }, },
};

static const struct fuvc_frame_info uvc_frames_h264[] =
{
    {  640,  480, {333333,       0 }, },
    { 1280,  720, {333333,       0 }, },
    { 1920, 1080, {333333,       0 }, },
    {    0,    0, {     0,         }, },
};

static const struct fuvc_format_info uvc_formats[] =
{ //yuv first
    { SAMPLE_V4L2_PIX_FMT_YUYV, uvc_frames_yuyv },
    { SAMPLE_V4L2_PIX_FMT_MJPEG, uvc_frames_mjpeg },
    { SAMPLE_V4L2_PIX_FMT_H264, uvc_frames_h264 },
};

extern void fuvc_frame_descriptors_get(struct fuvc_format_info *format_info);
void fuvc_frame_descriptors_set(void)
{
    unsigned int i = 0;

    for (; i < sizeof(uvc_formats) / sizeof(uvc_formats[0]); ++i)
    {
        fuvc_frame_descriptors_get((struct fuvc_format_info *)&uvc_formats[i]);
    }

    return;
}
#endif /* end of #ifdef __HuaweiLite__ */

static void SAMPLE_UVC_Usage(char *sPrgNm)
{
    printf("Usage : %s <param>\n", sPrgNm);
    printf("param:\n");
    printf("\t -h        --for help.\n");
    printf("\t -bulkmode --use bulkmode.\n");

    return;
}

int create_cache(void)
{
    if (create_uvc_cache() != 0)
    {
        return -1;
    }

    if (create_uac_cache() != 0)
    {
        destroy_uvc_cache();
        return -1;
    }

    return 0;
}

void destroy_cache(void)
{
    destroy_uvc_cache();
    destroy_uac_cache();
    return;
}

int run_camera(void)
{
    if (get_hicamera()->init() != 0)
    {
        return -1;
    }

    if (get_hicamera()->open() != 0)
    {
        get_hicamera()->deinit();
        return -1;
    }

    if (get_hicamera()->run() != 0)
    {
        get_hicamera()->close();
        get_hicamera()->deinit();
        return -1;
    }

    return 0;
}

void stop_camera(void)
{
    get_hicamera()->stop();
    get_hicamera()->close();
    get_hicamera()->deinit();
    return;
}

#ifndef __HuaweiLite__
int sample_uvc_main(int argc, char *argv[])
#else
int app_main(int argc, char *argv[])
#endif
{
    int i = argc;

    printf("\n@@@@@ HiUVC App Sample @@@@@\n\n");

    while (i > 1)
    {
        if (strcmp(argv[i-1], "-bulkmode") == 0)
        {
            g_bulk = 1;
        }

        if (strcmp(argv[i-1], "-h") == 0)
        {
            SAMPLE_UVC_Usage(argv[0]);
        }

        i--;
    }

#ifndef __HuaweiLite__
    if (create_config_svc("./uvc_app.conf") != 0)
    {
        goto EXIT0;
    }
#endif

    if (create_cache() != 0)
    {
        goto EXIT1;
    }

    if (run_camera() != 0)
    {
        goto EXIT2;
    }

    while (1)
    {
        sleep(1);
    }

EXIT2:
    destroy_cache();
EXIT1:
#ifndef __HuaweiLite__
    release_cofnig_svc();
EXIT0:
#endif
    printf("uvc_app exit!\n");
    return 0;
}
