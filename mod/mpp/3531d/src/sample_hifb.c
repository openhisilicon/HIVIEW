/*
 * Copyright (c) Hisilicon Technologies Co., Ltd. 2019-2020. All rights reserved.
 * Description: hifb sample for demo.
 * Author: Hisilicon multimedia software group
 * Create: 2019/12/04
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <sys/poll.h>
#include <sys/time.h>
#include <fcntl.h>
#include <errno.h>
#include <pthread.h>
#include <math.h>
#include <unistd.h>
#include <signal.h>
#include <sys/mman.h>
#include <sys/prctl.h>
#include <linux/fb.h>
#include <limits.h>

#include "securec.h"
#include "hifb.h"
#include "loadbmp.h"
#include "hi_common_tde.h"
#include "hi_mpi_tde.h"
#include "sample_comm.h"

#define FILE_LENGTH_MAX 12
#define CMAP_LENGTH_MAX 256
#define WIDTH_1080P 1920
#define HEIGHT_1080P 1080
#define WIDTH_800 800
#define HEIGHT_600 600

#define SAMPLE_IMAGE_WIDTH     300
#define SAMPLE_IMAGE_HEIGHT    150
#define SAMPLE_IMAGE_NUM       20
#define HIFB_RED_1555          0xFC00
#define HIFB_RED_8888          0xFFff0000
#define WIDTH_1920             1920
#define HEIGHT_1080            1080

#define GRAPHICS_LAYER_G0      0
#define GRAPHICS_LAYER_G1      1
#define GRAPHICS_LAYER_G2      2
#define GRAPHICS_LAYER_G3      3

#define SAMPLE_IMAGE1_PATH        "./res/%d.bmp"
#define SAMPLE_IMAGE2_PATH        "./res/1280_720.bits"
#define SAMPLE_CURSOR_PATH        "./res/cursor.bmp"

#if (defined(CONFIG_HI_HIFB_SUPPORT) && defined(CONFIG_HI_VO_SUPPORT))
#define HIFB_BE_WITH_VO    1
#else
#define HIFB_BE_WITH_VO    0
#endif

static hi_char g_exit_flag = 0;
hi_vo_intf_type g_vo_intf_type = HI_VO_INTF_BT1120;
hi_vo_dev g_vo_dev = 0;
osd_color_format g_osd_color_fmt = OSD_COLOR_FORMAT_RGB1555;

static struct fb_bitfield g_r16 = {10, 5, 0};
static struct fb_bitfield g_g16 = {5, 5, 0};
static struct fb_bitfield g_b16 = {0, 5, 0};
static struct fb_bitfield g_a16 = {15, 1, 0};

static struct fb_bitfield g_a32 = {24, 8, 0};
static struct fb_bitfield g_r32 = {16, 8, 0};
static struct fb_bitfield g_g32 = {8,  8, 0};
static struct fb_bitfield g_b32 = {0,  8, 0};

static struct fb_bitfield g_a4 = {0, 0, 0};
static struct fb_bitfield g_r4 = {0, 4, 0};
static struct fb_bitfield g_g4 = {0, 4, 0};
static struct fb_bitfield g_b4 = {0, 4, 0};

hi_u16 g_cmap_red[CMAP_LENGTH_MAX] = {0xff, 0, 0, 0xff};
hi_u16 g_cmap_green[CMAP_LENGTH_MAX] = {0, 0xff, 0, 0xff};
hi_u16 g_cmap_blue[CMAP_LENGTH_MAX] = {0, 0, 0xff, 0xff};
hi_u16 g_cmap_alpha[CMAP_LENGTH_MAX] = {0xff, 0xff, 0xff, 0xff};

pthread_t g_hifb_thread = 0;
pthread_t g_hifb_thread1 = 0;

hi_phys_addr_t g_phyaddr = 0;
hi_phys_addr_t g_canvas_addr = 0;

#ifdef CONFIG_SUPPORT_SAMPLE_ROTATION
static hi_phys_addr_t g_canvas_phy = 0;
static hi_void *g_canvas_vir = NULL;
static hi_phys_addr_t g_picture_phy = 0;
static hi_void *g_picture_vir = NULL;
#endif

typedef struct {
    hi_s32 fd; /* fb's file describ */
    hi_s32 layer; /* which graphic layer */
    hi_s32 ctrlkey; /* {0,1,2,3}={1buffer, 2buffer, 0buffer pan display, 0buffer refresh} */
    hi_bool compress; /* image compressed or not */
    hi_fb_color_format color_format; /* color format. */
} pthread_hifb_sample_info;

typedef struct {
    hi_vo_dev vo_dev;
    hi_vo_intf_type hi_vo_intf_type;
}vo_device_info;

#ifdef CONFIG_SUPPORT_SAMPLE_ROTATION
static hi_s32 hifb_put_layer_info(pthread_hifb_sample_info *info);
static hi_s32 hifb_get_canvas(pthread_hifb_sample_info* info);
static hi_void hifb_put_canvas(hi_void);
static hi_void hifb_init_surface(hi_tde_surface *, hi_tde_surface *, hi_tde_rect *, hi_tde_rect *, hi_u32);
static hi_s32 hifb_draw(pthread_hifb_sample_info *info);
static hi_s32 hifb_refresh(pthread_hifb_sample_info *info);
static hi_void hifb_get_var_by_format(pthread_hifb_sample_info *info, struct fb_var_screeninfo *var_info);
static hi_void hifb_put_rotation_degree(struct fb_var_screeninfo *var_info, hi_fb_rotate_mode *rotate_mode);
static hi_s32 hifb_put_rotation(pthread_hifb_sample_info *info);
static hi_void hifb_rotate(hi_vo_dev vo_dev);
#endif

hi_s32 sample_hifb_load_bmp(const char *filename, hi_u8 *addr)
{
    osd_surface surface;
    osd_bit_map_file_header bmp_file_header;
    osd_bit_map_info bmp_info;

    if (get_bmp_info(filename, &bmp_file_header, &bmp_info) < 0) {
        SAMPLE_PRT("get_bmp_info err!\n");
        return HI_FAILURE;
    }
    surface.color_format = g_osd_color_fmt;
    create_surface_by_bit_map(filename, &surface, addr);
    return HI_SUCCESS;
}

hi_void sample_hifb_to_exit(hi_void)
{
    hi_char ch;
    while (1) {
        printf("\npress 'q' to exit this sample.\n");
        while ((ch = (char)getchar()) == '\n');
        getchar();
        if (ch == 'q') {
            g_exit_flag = ch;
            break;
        } else {
            printf("input invaild! please try again.\n");
        }
    }
    if (g_hifb_thread != 0) {
        pthread_join(g_hifb_thread, 0);
        g_hifb_thread = 0;
    }

    if (g_hifb_thread1 != 0) {
        pthread_join(g_hifb_thread1, 0);
        g_hifb_thread1 = 0;
    }
    return;
}

static hi_s32 sample_get_file_name(pthread_hifb_sample_info *info, hi_char *file, hi_u32 file_length)
{
    switch (info->layer) {
        case GRAPHICS_LAYER_G0:
            if (strncpy_s(file, file_length, "/dev/fb0", strlen("/dev/fb0")) != EOK) {
                printf("%s:%d:strncpy_s failed.\n", __FUNCTION__, __LINE__);
                return HI_FAILURE;
            }
            break;
        case GRAPHICS_LAYER_G1:
            if (strncpy_s(file, file_length, "/dev/fb1", strlen("/dev/fb1")) != EOK) {
                printf("%s:%d:strncpy_s failed.\n", __FUNCTION__, __LINE__);
                return HI_FAILURE;
            }
            break;
        case GRAPHICS_LAYER_G2:
            if (strncpy_s(file, file_length, "/dev/fb2", strlen("/dev/fb2")) != EOK) {
                printf("%s:%d:strncpy_s failed.\n", __FUNCTION__, __LINE__);
                return HI_FAILURE;
            }
            break;
        case GRAPHICS_LAYER_G3:
            if (strncpy_s(file, file_length, "/dev/fb3", strlen("/dev/fb3")) != EOK) {
                printf("%s:%d:strncpy_s failed.\n", __FUNCTION__, __LINE__);
                return HI_FAILURE;
            }
            break;
        default:
            if (strncpy_s(file, file_length, "/dev/fb0", strlen("/dev/fb0")) != EOK) {
                printf("%s:%d:strncpy_s failed.\n", __FUNCTION__, __LINE__);
                return HI_FAILURE;
            }
            break;
    }
    return HI_SUCCESS;
}

static hi_s32 sample_init_frame_buffer(pthread_hifb_sample_info *info, const char *input_file)
{
    hi_bool show = HI_FALSE;
    hi_fb_point point = {0, 0};
    hi_char file[PATH_MAX + 1] = {0};

    if (strlen(input_file) > PATH_MAX || realpath(input_file, file) == HI_NULL) {
        return HI_FAILURE;
    }
    /* step 1. open framebuffer device overlay 0 */
    info->fd = open(file, O_RDWR, 0);
    if (info->fd < 0) {
        SAMPLE_PRT("open %s failed!\n", file);
        return HI_FAILURE;
    }

    show = HI_FALSE;
    if (ioctl(info->fd, FBIOPUT_SHOW_HIFB, &show) < 0) {
        SAMPLE_PRT("FBIOPUT_SHOW_HIFB failed!\n");
        close(info->fd);
        info->fd = -1;
        return HI_FAILURE;
    }

    /* step 2. set the screen original position */
    switch (info->ctrlkey) {
        case 3: /* 3 mouse case */
            point.x_pos = 150; /* 150 x pos */
            point.y_pos = 150; /* 150 y pos */
            break;
        default:
            point.x_pos = 0;
            point.y_pos = 0;
    }

    if (ioctl(info->fd, FBIOPUT_SCREEN_ORIGIN_HIFB, &point) < 0) {
        SAMPLE_PRT("set screen original show position failed!\n");
        close(info->fd);
        info->fd = -1;
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

static hi_s32 sample_init_var(pthread_hifb_sample_info* info)
{
    struct fb_var_screeninfo var;

    if (ioctl(info->fd, FBIOGET_VSCREENINFO, &var) < 0) {
        SAMPLE_PRT("get variable screen info failed!\n");
        return HI_FAILURE;
    }

    switch (info->color_format) {
        case HI_FB_FORMAT_ARGB8888:
            var.transp = g_a32;
            var.red = g_r32;
            var.green = g_g32;
            var.blue = g_b32;
            var.bits_per_pixel = 32; /* 32 for 4 byte */
            g_osd_color_fmt = OSD_COLOR_FORMAT_RGB8888;
            break;
        default:
            var.transp = g_a16;
            var.red = g_r16;
            var.green = g_g16;
            var.blue = g_b16;
            var.bits_per_pixel = 16; /* 16 for 2 byte */
            break;
    }

    switch (info->ctrlkey) {
        case 3: /* 3 mouse case */
            var.xres_virtual = 48; /* 48 for alg data */
            var.yres_virtual = 48; /* 48 for alg data */
            var.xres = 48; /* 48 for alg data */
            var.yres = 48; /* 48 for alg data */
        break;
        default:
            var.xres_virtual = WIDTH_1080P;
            var.yres_virtual = HEIGHT_1080P * 2; /* 2 for 2buf */
            var.xres = WIDTH_1080P;
            var.yres = HEIGHT_1080P;
    }
    var.activate       = FB_ACTIVATE_NOW;

    if (ioctl(info->fd, FBIOPUT_VSCREENINFO, &var) < 0) {
        SAMPLE_PRT("put variable screen info failed!\n");
        return HI_FAILURE;
    }
    return HI_SUCCESS;
}

static hi_s32 sample_put_alpha_and_colorkey(pthread_hifb_sample_info *info, hi_fb_color_format clr_fmt)
{
    hi_s32 ret;
    hi_fb_alpha alpha = {0};
    hi_fb_colorkey color_key;
    SAMPLE_PRT("expected: the red box will appear!\n");
    sleep(2); /* 2 second */
    alpha.alpha_en = HI_TRUE;
    alpha.alpha0 = 0x0;
    alpha.alpha1 = 0x0;
    if (ioctl(info->fd, FBIOPUT_ALPHA_HIFB,  &alpha) < 0) {
        SAMPLE_PRT("set alpha failed!\n");
        return HI_FAILURE;
    }
    SAMPLE_PRT("expected: after set alpha = 0, the red box will disappear!\n");
    sleep(2); /* 2 second */

    alpha.alpha0 = 0;
    alpha.alpha1 = 0xFF;
    if (ioctl(info->fd, FBIOPUT_ALPHA_HIFB,  &alpha) < 0) {
        SAMPLE_PRT("set alpha failed!\n");
        return HI_FAILURE;
    }
    SAMPLE_PRT("expected:after set set alpha = 0xFF, the red box will appear again!\n");
    sleep(2); /* 2 second */

    SAMPLE_PRT("expected: the red box will erased by colorkey!\n");
    color_key.enable = HI_TRUE;
    color_key.value = (clr_fmt == HI_FB_FORMAT_ARGB8888 ? HIFB_RED_8888 : HIFB_RED_1555);
    ret = ioctl(info->fd, FBIOPUT_COLORKEY_HIFB, &color_key);
    if (ret < 0) {
        SAMPLE_PRT("FBIOPUT_COLORKEY_HIFB failed!\n");
        return HI_FAILURE;
    }
    sleep(2); /* 2 second */
    SAMPLE_PRT("expected: the red box will appear again!\n");
    color_key.enable = HI_FALSE;
    ret = ioctl(info->fd, FBIOPUT_COLORKEY_HIFB, &color_key);
    if (ret < 0) {
        SAMPLE_PRT("FBIOPUT_COLORKEY_HIFB failed!\n");
        return HI_FAILURE;
    }
    return HI_SUCCESS;
}

static hi_void sample_draw_rect_by_cpu(hi_void *ptemp, hi_fb_color_format clr_fmt, struct fb_var_screeninfo *var)
{
    hi_s32 x, y;
    for (y = 100; y < 300; y++) { /* 100 300 for y */
        for (x = 0; x < 300; x++) { /* 300 for x */
            if (clr_fmt == HI_FB_FORMAT_ARGB8888) {
                *((hi_u32*)ptemp + y * var->xres + x) = HIFB_RED_8888;
            } else {
                *((hi_u16*)ptemp + y * var->xres + x) = HIFB_RED_1555;
            }
        }
    }
    return;
}

static hi_s32 sample_time_to_play(pthread_hifb_sample_info* info, hi_u8 *show_screen, hi_u32 fix_screen_stride)
{
    hi_bool show = HI_FALSE;
    hi_void *ptemp = HI_NULL;
    hi_s32 i;
    struct fb_var_screeninfo var;
    hi_fb_color_format clr_fmt;

    if (ioctl(info->fd, FBIOGET_VSCREENINFO, &var) < 0) {
        SAMPLE_PRT("get variable screen info failed!\n");
        return HI_FAILURE;
    }

    show = HI_TRUE;
    if (ioctl(info->fd, FBIOPUT_SHOW_HIFB, &show) < 0) {
        SAMPLE_PRT("FBIOPUT_SHOW_HIFB failed!\n");
        return HI_FAILURE;
    }

    switch (info->color_format) {
        case HI_FB_FORMAT_ARGB8888:
            clr_fmt = HI_FB_FORMAT_ARGB8888;
            break;
        default:
            clr_fmt = HI_FB_FORMAT_ARGB1555;
            break;
    }
    /* only for G0 or G1 */
    if ((info->layer != GRAPHICS_LAYER_G0) && (info->layer != GRAPHICS_LAYER_G1)) {
        return HI_SUCCESS;
    }
    for (i = 0; i < 1; i++) {
        if (i % 2) { /* 2 for 0 or 1 */
            var.yoffset = var.yres;
        } else {
            var.yoffset = 0;
        }
        ptemp = (show_screen + var.yres * fix_screen_stride * (i % 2)); /* 2 for 0 or 1 */
        sample_draw_rect_by_cpu(ptemp, clr_fmt, &var);
        /*
         * note : not acting on ARGB8888, for ARGB8888 format image's alpha, you can change ptemp[x][y]'s value
         * HIFB_RED_8888 = 0xffff00000 means alpha=255(show),red.0x00ff0000 means alpha=0,red(hide).
         */
        if (sample_put_alpha_and_colorkey(info, clr_fmt) != HI_SUCCESS) {
            return HI_FAILURE;
        }
        sleep(2); /* 2 second */
    }
    return HI_SUCCESS;
}

static hi_s32 sample_quick_copy_by_tde(hi_u32 byte_per_pixel, hi_tde_color_format tde_clr_fmt,
    hi_u32 fix_screen_stride, hi_u64 hide_screen_phy)
{
    hi_s32 handle;
    hi_tde_rect src_rect = {0};
    hi_tde_rect dst_rect = {0};
    hi_tde_surface src_surface = {0};
    hi_tde_surface dst_surface = {0};
    hi_tde_single_src single_src = {0};

    /* TDE job step 0. open tde */
    src_rect.pos_x = dst_rect.pos_x = 0;
    src_rect.pos_y = dst_rect.pos_y = 0;
    src_rect.height = SAMPLE_IMAGE_HEIGHT;
    src_rect.width = SAMPLE_IMAGE_WIDTH;
    dst_rect.height = src_rect.height;
    dst_rect.width = src_rect.width;

    dst_surface.color_format = tde_clr_fmt;
    dst_surface.width = WIDTH_1080P;
    dst_surface.height = HEIGHT_1080P;
    dst_surface.stride = fix_screen_stride;
    dst_surface.phys_addr = hide_screen_phy;

    src_surface.color_format = tde_clr_fmt;
    src_surface.width = SAMPLE_IMAGE_WIDTH;
    src_surface.height = SAMPLE_IMAGE_HEIGHT;
    src_surface.stride = byte_per_pixel * SAMPLE_IMAGE_WIDTH;
    src_surface.phys_addr = g_phyaddr;
    src_surface.support_alpha_ex_1555 = HI_TRUE;
    src_surface.alpha_max_is_255  = HI_TRUE;
    src_surface.alpha0 = 0XFF;
    src_surface.alpha1 = 0XFF;

    /* TDE job step 1. start job */
    handle = hi_tde_begin_job();
    if (handle == HI_ERR_TDE_INVALID_HANDLE) {
        return HI_FAILURE;
    }
    single_src.src_surface = &src_surface;
    single_src.dst_surface = &dst_surface;
    single_src.src_rect = &src_rect;
    single_src.dst_rect = &dst_rect;
    if (hi_tde_quick_copy(handle, &single_src) < 0) {
        SAMPLE_PRT("hi_tde_quick_copy:%d failed!\n", __LINE__);
        hi_tde_cancel_job(handle);
        return HI_FAILURE;
    }

    /* TDE job step 2. submit job */
    if (hi_tde_end_job(handle, HI_FALSE, HI_TRUE, 10) < 0) { /* 10 timeout */
        SAMPLE_PRT("line:%d,hi_tde_end_job failed!\n", __LINE__);
        hi_tde_cancel_job(handle);
        return HI_FAILURE;
    }
    return HI_SUCCESS;
}

static hi_void sample_get_param(pthread_hifb_sample_info* info, hi_fb_color_format *clr_fmt,
    hi_tde_color_format *tde_clr_fmt, hi_u32 *color)
{
    switch (info->color_format) {
        case HI_FB_FORMAT_ARGB8888:
            *clr_fmt = HI_FB_FORMAT_ARGB8888;
            *color = HIFB_RED_8888;
            *tde_clr_fmt = HI_TDE_COLOR_FORMAT_ARGB8888;
            break;
        default:
            *clr_fmt = HI_FB_FORMAT_ARGB1555;
            *color = HIFB_RED_1555;
            *tde_clr_fmt = HI_TDE_COLOR_FORMAT_ARGB1555;
            break;
    }
    return;
}

static hi_s32 sample_draw_line_by_cpu(pthread_hifb_sample_info* info, hi_u8 *show_screen, hi_u32 fix_screen_stride,
    hi_u64 *hide_screen_phy, hi_s32 i)
{
    hi_s32 x, y;
    hi_u8 *hide_screen = HI_NULL;
    hi_void *show_line = HI_NULL;
    struct fb_var_screeninfo var;
    struct fb_fix_screeninfo fix;
    hi_fb_color_format clr_fmt;
    hi_tde_color_format tde_clr_fmt = HI_TDE_COLOR_FORMAT_ARGB1555;
    hi_u32 color = HIFB_RED_1555;

    if (ioctl(info->fd, FBIOGET_VSCREENINFO, &var) < 0) {
        SAMPLE_PRT("get variable screen info failed!\n");
        return HI_FAILURE;
    }

    if (ioctl(info->fd, FBIOGET_FSCREENINFO, &fix) < 0) {
        return HI_FAILURE;
    }

    sample_get_param(info, &clr_fmt, &tde_clr_fmt, &color);
    /* TDE step1: draw two red line */
    var.yoffset = (i % 2 == 1) ? var.yres : 0; /* 2 for 0 1 */

    hide_screen = show_screen + (fix_screen_stride * var.yres) * (i % 2); /* 2 for 0 1 */
    if  (memset_s(hide_screen, fix_screen_stride * var.yres, 0x00, fix_screen_stride * var.yres) != EOK) {
        return HI_FAILURE;
    }
    *hide_screen_phy = fix.smem_start + (i % 2) * fix_screen_stride * var.yres; /* 2 for 0 1 */
    show_line = hide_screen;
    for (y = (HEIGHT_1080P / 2 - 2); y < (HEIGHT_1080P / 2 + 2); y++) { /* 2 for alg data */
        for (x = 0; x < WIDTH_1080P; x++) {
            if (clr_fmt == HI_FB_FORMAT_ARGB8888) {
                *((hi_u32*)show_line + y * var.xres + x) = color;
            } else {
                *((hi_u16*)show_line + y * var.xres + x) = color;
            }
        }
    }
    for (y = 0; y < HEIGHT_1080P; y++) {
        for (x = (WIDTH_1080P / 2 - 2); x < (WIDTH_1080P / 2 + 2); x++) { /* 2 for alg data */
            if (clr_fmt == HI_FB_FORMAT_ARGB8888) {
                *((hi_u32*)show_line + y * var.xres + x) = color;
            } else {
                *((hi_u16*)show_line + y * var.xres + x) = color;
            }
        }
    }
    if (ioctl(info->fd, FBIOPAN_DISPLAY, &var) < 0) {
            SAMPLE_PRT("FBIOPAN_DISPLAY failed!\n");
            return HI_FAILURE;
    }
    return HI_SUCCESS;
}

static hi_s32 sample_draw_line_and_picture(pthread_hifb_sample_info* info, hi_void *viraddr, hi_u8 *show_screen,
    hi_u32 fix_screen_stride)
{
    hi_s32 i;
    struct fb_var_screeninfo var;
    struct fb_fix_screeninfo fix;
    hi_u64 hide_screen_phy = 0;
    hi_u32 byte_per_pixel;
    hi_fb_color_format clr_fmt;
    hi_tde_color_format tde_clr_fmt = HI_TDE_COLOR_FORMAT_ARGB1555;
    hi_u32 color = HIFB_RED_1555;
    hi_char image_name[128]; /* 128 for char length */
    hi_u8 *dst = HI_NULL;

    sample_get_param(info, &clr_fmt, &tde_clr_fmt, &color);

    if (ioctl(info->fd, FBIOGET_VSCREENINFO, &var) < 0) {
        return HI_FAILURE;
    }
    byte_per_pixel = var.bits_per_pixel / 8; /* 8 for 1 byte */

    if (ioctl(info->fd, FBIOGET_FSCREENINFO, &fix) < 0) {
        return HI_FAILURE;
    }

    SAMPLE_PRT("expected:two red line!\n");
    for (i = 0; i < SAMPLE_IMAGE_NUM; i++) {
        if (g_exit_flag == 'q') {
            printf("process exit...\n");
            break;
        }

        if (sample_draw_line_by_cpu(info, show_screen, fix_screen_stride, &hide_screen_phy, i) != HI_SUCCESS) {
            return HI_FAILURE;
        }

        /* TDE step2: draw gui picture */
        if (snprintf_s(image_name, sizeof(image_name), 12, SAMPLE_IMAGE1_PATH, i % 2) == -1) { /* 12 2 length */
            return HI_FAILURE;
        }
        dst = (hi_u8*)viraddr;
        if (sample_hifb_load_bmp(image_name, dst) != HI_SUCCESS) {
            return HI_FAILURE;
        }
        if (sample_quick_copy_by_tde(byte_per_pixel, tde_clr_fmt, fix_screen_stride, hide_screen_phy) !=
            HI_SUCCESS) {
            return HI_FAILURE;
        }

        var.yoffset = (i % 2 == 1) ? var.yres : 0; /* 2 for 0 1 */
        if (ioctl(info->fd, FBIOPAN_DISPLAY, &var) < 0) {
            SAMPLE_PRT("FBIOPAN_DISPLAY failed!\n");
            return HI_FAILURE;
        }
        sleep(1);
    }
    return HI_SUCCESS;
}

static hi_s32 sample_move_cursor(pthread_hifb_sample_info* info, struct fb_var_screeninfo *var, hi_u8 *show_screen)
{
    hi_s32 i;
    hi_fb_point point = {0, 0};

    point.x_pos = (info->ctrlkey == 3) ? 150 : 0; /* 3 150:for case;alg data */
    point.y_pos = (info->ctrlkey == 3) ? 150 : 0; /* 3 150:for case;alg data */

    if (sample_hifb_load_bmp(SAMPLE_CURSOR_PATH, show_screen) != HI_SUCCESS) {
        return HI_FAILURE;
    }
    if (ioctl(info->fd, FBIOPAN_DISPLAY, var) < 0) {
        SAMPLE_PRT("FBIOPAN_DISPLAY failed!\n");
        return HI_FAILURE;
    }
    SAMPLE_PRT("show cursor\n");
    sleep(2); /* 2 second */
    for (i = 0; i < 100; i++) { /* 100 for times */
        if (g_exit_flag == 'q') {
            printf("process exit...\n");
            break;
        }
        point.x_pos += 2; /* 2 pos */
        point.y_pos += 2; /* 2 pos */
        if (ioctl(info->fd, FBIOPUT_SCREEN_ORIGIN_HIFB, &point) < 0) {
            SAMPLE_PRT("set screen original show position failed!\n");
            return HI_FAILURE;
        }
        usleep(70 * 1000); /* 70 1000 for sleep */
    }
    for (i = 0; i < 100; i++) { /* 100 for times */
        if (g_exit_flag == 'q') {
            printf("process exit...\n");
            break;
        }
        point.x_pos += 2; /* 2 pos */
        point.y_pos -= 2; /* 2 pos */
        if (ioctl(info->fd, FBIOPUT_SCREEN_ORIGIN_HIFB, &point) < 0) {
            SAMPLE_PRT("set screen original show position failed!\n");
            return HI_FAILURE;
        }
        usleep(70 * 1000); /* 70 1000 for sleep */
    }
    SAMPLE_PRT("move the cursor\n");
    sleep(1);
    return HI_SUCCESS;
}

static hi_s32 sample_show_bitmap(pthread_hifb_sample_info* info, hi_u8 *show_screen, hi_u32 fix_screen_stride,
    hi_u32 byte_per_pixel)
{
    hi_s32 ret;
    struct fb_var_screeninfo var;
    struct fb_fix_screeninfo fix;
    hi_void *viraddr = HI_NULL;

    if (ioctl(info->fd, FBIOGET_VSCREENINFO, &var) < 0) {
        SAMPLE_PRT("get variable screen info failed!\n");
        return HI_FAILURE;
    }

    if (ioctl(info->fd, FBIOGET_FSCREENINFO, &fix) < 0) {
        SAMPLE_PRT("get fix screen info failed!\n");
        return HI_FAILURE;
    }

    switch (info->ctrlkey) {
        /* 2 means none buffer and just for pan display. */
        case 2:
            if (hi_mpi_sys_mmz_alloc(&g_phyaddr, ((void**)&viraddr), HI_NULL, HI_NULL, SAMPLE_IMAGE_WIDTH *
                SAMPLE_IMAGE_HEIGHT * byte_per_pixel) == HI_FAILURE) {
                SAMPLE_PRT("allocate memory (max_w*max_h*%d bytes) failed\n", byte_per_pixel);
                return HI_FAILURE;
            }
            ret = hi_tde_open();
            if (ret < 0) {
                SAMPLE_PRT("hi_tde_open failed :%d!\n", ret);
                hi_mpi_sys_mmz_free(g_phyaddr, viraddr);
                g_phyaddr = 0;
                return HI_FAILURE;
            }
            if (sample_draw_line_and_picture(info, viraddr, show_screen, fix_screen_stride) != HI_SUCCESS) {
                hi_tde_close();
                hi_mpi_sys_mmz_free(g_phyaddr, viraddr);
                g_phyaddr = 0;
                return HI_FAILURE;
            }
            hi_mpi_sys_mmz_free(g_phyaddr, viraddr);
            g_phyaddr = 0;
            hi_tde_close();
            break;
        case 3: /* 3 mouse case */
            /* move cursor */
            if (sample_move_cursor(info, &var, show_screen) != HI_SUCCESS) {
                return HI_FAILURE;
            }
            break;
    }

    return HI_SUCCESS;
}

static hi_s32 sample_show_process(pthread_hifb_sample_info* info)
{
    hi_u8 *show_screen = HI_NULL;
    struct fb_var_screeninfo var;
    struct fb_fix_screeninfo fix;
    hi_u32 fix_screen_stride;
    hi_bool show;

    if (ioctl(info->fd, FBIOGET_VSCREENINFO, &var) < 0) {
        SAMPLE_PRT("get variable screen info failed!\n");
        goto ERR1;
    }

    if (ioctl(info->fd, FBIOGET_FSCREENINFO, &fix) < 0) {
        SAMPLE_PRT("get fix screen info failed!\n");
        goto ERR1;
    }
    fix_screen_stride = fix.line_length;
    show_screen = mmap(HI_NULL, fix.smem_len, PROT_READ | PROT_WRITE, MAP_SHARED, info->fd, 0);
    if (show_screen == MAP_FAILED) {
        SAMPLE_PRT("mmap framebuffer failed!\n");
        goto ERR1;
    }

    if (memset_s(show_screen, fix.smem_len, 0x0, fix.smem_len) != EOK) {
        goto ERR2;
    }

    if (sample_time_to_play(info, show_screen, fix_screen_stride) != HI_SUCCESS) {
        goto ERR2;
    }
    /* 8 for 1 byte */
    if (sample_show_bitmap(info, show_screen, fix_screen_stride, var.bits_per_pixel / 8) != HI_SUCCESS) {
        goto ERR2;
    }
    munmap(show_screen, fix.smem_len);
    show = HI_FALSE;
    if (ioctl(info->fd, FBIOPUT_SHOW_HIFB, &show) < 0) {
        SAMPLE_PRT("FBIOPUT_SHOW_HIFB failed!\n");
        close(info->fd);
        return HI_FAILURE;
    }
    close(info->fd);
    info->fd = -1;
    return HI_SUCCESS;

ERR2:
    munmap(show_screen, fix.smem_len);
    show_screen = HI_NULL;
ERR1:
    close(info->fd);
    info->fd = -1;
    return HI_FAILURE;
}

hi_void* sample_hifb_pandisplay(hi_void* data)
{
    pthread_hifb_sample_info* info = HI_NULL;
    hi_char thdname[64]; /* 64 for char length */
    hi_char file[FILE_LENGTH_MAX] = {0};

    if (data == HI_NULL) {
        return HI_NULL;
    }
    info = (pthread_hifb_sample_info*)data;
    if (snprintf_s(thdname, sizeof(thdname), 17, "HIFB%d_pandisplay", info->layer) == -1) { /* 17 for char length */
        printf("%s:%d:snprintf_s failed.\n", __FUNCTION__, __LINE__);
        return HI_NULL;
    }
    prctl(PR_SET_NAME, thdname, 0, 0, 0);

    if (sample_get_file_name(info, file, FILE_LENGTH_MAX) != HI_SUCCESS) {
        return HI_NULL;
    }

    if (sample_init_frame_buffer(info, file) != HI_SUCCESS) {
        return HI_NULL;
    }

    if (sample_init_var(info) != HI_SUCCESS) {
        close(info->fd);
        info->fd = -1;
        return HI_NULL;
    }

    /* map the physical video memory for user use */
    if (sample_show_process(info) != HI_SUCCESS) {
        return HI_NULL;
    }
    SAMPLE_PRT("[end]\n");
    return HI_NULL;
}

static hi_s32 sample_init_frame_buffer_ex(pthread_hifb_sample_info* info, const char *input_file)
{
    hi_fb_colorkey color_key;
    hi_fb_point point = {0, 0};
    hi_char file[PATH_MAX + 1] = {0};

    if (strlen(input_file) > PATH_MAX || realpath(input_file, file) == HI_NULL) {
        return HI_FAILURE;
    }
    /* step 1. open framebuffer device overlay 0 */
    info->fd = open(file, O_RDWR, 0);
    if (info->fd < 0) {
        SAMPLE_PRT("open %s failed!\n", file);
        return HI_FAILURE;
    }

    /* all layer surport colorkey */
    color_key.enable = HI_TRUE;
    color_key.value = 0x0;
    if (ioctl(info->fd, FBIOPUT_COLORKEY_HIFB, &color_key) < 0) {
        SAMPLE_PRT("FBIOPUT_COLORKEY_HIFB!\n");
        close(info->fd);
        info->fd = -1;
        return HI_FAILURE;
    }

    if (ioctl(info->fd, FBIOPUT_SCREEN_ORIGIN_HIFB, &point) < 0) {
        SAMPLE_PRT("set screen original show position failed!\n");
        close(info->fd);
        info->fd = -1;
        return HI_FAILURE;
    }
    return HI_SUCCESS;
}

static hi_s32 sample_init_var_ex(pthread_hifb_sample_info* info)
{
    struct fb_var_screeninfo var;

    /* step 3. get the variable screen information */
    if (ioctl(info->fd, FBIOGET_VSCREENINFO, &var) < 0) {
        SAMPLE_PRT("get variable screen info failed!\n");
        return HI_FAILURE;
    }

    switch (info->color_format) {
        case HI_FB_FORMAT_ARGB8888:
            var.transp = g_a32;
            var.red = g_r32;
            var.green = g_g32;
            var.blue = g_b32;
            var.bits_per_pixel = 32; /* 32 for 4 byte */
            g_osd_color_fmt = OSD_COLOR_FORMAT_RGB8888;
            break;
        default:
            var.transp = g_a16;
            var.red = g_r16;
            var.green = g_g16;
            var.blue = g_b16;
            var.bits_per_pixel = 16; /* 16 for 2 byte */
            break;
    }

    var.xres_virtual = WIDTH_1080P;
    var.yres_virtual = HEIGHT_1080P;
    var.xres = WIDTH_1080P;
    var.yres = HEIGHT_1080P;
    var.activate = FB_ACTIVATE_NOW;

    /* step 5. set the variable screen information */
    if (ioctl(info->fd, FBIOPUT_VSCREENINFO, &var) < 0) {
        SAMPLE_PRT("put variable screen info failed!\n");
        return HI_FAILURE;
    }
    return HI_SUCCESS;
}

static hi_s32 sample_init_layer_info(pthread_hifb_sample_info* info)
{
    hi_fb_layer_info layer_info = {0};
    switch (info->ctrlkey) {
        case 0: /* 0 case */
            layer_info.buf_mode = HI_FB_LAYER_BUF_ONE;
            layer_info.mask = HI_FB_LAYER_MASK_BUF_MODE;
            break;
        case 1: /* 1 case */
            layer_info.buf_mode = HI_FB_LAYER_BUF_DOUBLE;
            layer_info.mask = HI_FB_LAYER_MASK_BUF_MODE;
            break;
        default:
            layer_info.buf_mode = HI_FB_LAYER_BUF_NONE;
            layer_info.mask = HI_FB_LAYER_MASK_BUF_MODE;
    }
    if (ioctl(info->fd, FBIOPUT_LAYER_INFO, &layer_info) < 0) {
        SAMPLE_PRT("PUT_LAYER_INFO failed!\n");
        close(info->fd);
        info->fd = -1;
        return HI_FAILURE;
    }
    return HI_SUCCESS;
}

static hi_s32 sample_init_compress(pthread_hifb_sample_info* info)
{
    hi_bool show = HI_TRUE;
    if (ioctl(info->fd, FBIOPUT_SHOW_HIFB, &show) < 0) {
        SAMPLE_PRT("FBIOPUT_SHOW_HIFB failed!\n");
        close(info->fd);
        info->fd = -1;
        return HI_FAILURE;
    }

    if (info->compress == HI_TRUE) {
        if (ioctl(info->fd, FBIOPUT_COMPRESSION_HIFB, &info->compress) < 0) {
            SAMPLE_PRT("FBIOPUT_COMPRESSION_HIFB failed!\n");
            close(info->fd);
            info->fd = -1;
            return HI_FAILURE;
        }
    }
    return HI_SUCCESS;
}

static hi_s32 sample_init_canvas(pthread_hifb_sample_info* info, hi_fb_buf *canvas_buf, hi_void **buf,
    hi_void **viraddr)
{
    hi_u32 byte_per_pixel = 2; /* 2 bytes */
    hi_fb_color_format clr_fmt;

    switch (info->color_format) {
        case HI_FB_FORMAT_ARGB8888:
            byte_per_pixel = 4; /* 4 bytes */
            clr_fmt = HI_FB_FORMAT_ARGB8888;
            break;
        default:
            byte_per_pixel = 2; /* 2 bytes */
            clr_fmt = HI_FB_FORMAT_ARGB1555;
            break;
    }

    if (hi_mpi_sys_mmz_alloc(&g_canvas_addr, buf, HI_NULL, HI_NULL, WIDTH_1080P * HEIGHT_1080P *
        (byte_per_pixel)) == HI_FAILURE) {
        SAMPLE_PRT("allocate memory (max_w*max_h*%d bytes) failed\n", byte_per_pixel);
        close(info->fd);
        info->fd = -1;
        return HI_FAILURE;
    }
    canvas_buf->canvas.phys_addr = g_canvas_addr;
    canvas_buf->canvas.height = HEIGHT_1080P;
    canvas_buf->canvas.width = WIDTH_1080P;
    canvas_buf->canvas.pitch = WIDTH_1080P * (byte_per_pixel);
    canvas_buf->canvas.format = clr_fmt;
    if (memset_s(*buf, WIDTH_1080P * HEIGHT_1080P * (byte_per_pixel), 0x00, canvas_buf->canvas.pitch *
        canvas_buf->canvas.height) != EOK) {
        SAMPLE_PRT("%s:%d:memset_s failed\n", __FUNCTION__, __LINE__);
        hi_mpi_sys_mmz_free(g_canvas_addr, *buf);
        g_canvas_addr = 0;
        close(info->fd);
        return HI_FAILURE;
    }

    /* change bmp */
    if (hi_mpi_sys_mmz_alloc(&g_phyaddr, viraddr, HI_NULL, HI_NULL, SAMPLE_IMAGE_WIDTH * SAMPLE_IMAGE_HEIGHT *
        byte_per_pixel) == HI_FAILURE) {
        SAMPLE_PRT("allocate memory (max_w*max_h*%d bytes) failed\n", byte_per_pixel);
        hi_mpi_sys_mmz_free(g_canvas_addr, *buf);
        g_canvas_addr = 0;
        close(info->fd);
        return HI_FAILURE;
    }
    return HI_SUCCESS;
}

static hi_s32 sample_draw_line_by_cpu_ex(pthread_hifb_sample_info* info, hi_fb_buf *canvas_buf, hi_void *buf)
{
    hi_u32 x, y, color;
    hi_s32 ret;
    hi_fb_color_format clr_fmt;

    switch (info->color_format) {
        case HI_FB_FORMAT_ARGB8888:
            clr_fmt = HI_FB_FORMAT_ARGB8888;
            color = HIFB_RED_8888;
            break;
        default:
            clr_fmt = HI_FB_FORMAT_ARGB1555;
            color = HIFB_RED_1555;
            break;
    }
    for (y = (HEIGHT_1080P / 2 - 2); y < (HEIGHT_1080P / 2 + 2); y++) { /* 2 alg data */
        for (x = 0; x < WIDTH_1080P; x++) {
            if (clr_fmt == HI_FB_FORMAT_ARGB8888) {
                *((hi_u32*)buf + y * WIDTH_1080P + x) = color;
            }else {
                *((hi_u16*)buf + y * WIDTH_1080P + x) = color;
            }
        }
    }
    for (y = 0; y < HEIGHT_1080P; y++) {
        for (x = (WIDTH_1080P / 2 - 2); x < (WIDTH_1080P / 2 + 2); x++) { /* 2 alg data */
            if (clr_fmt == HI_FB_FORMAT_ARGB8888) {
                *((hi_u32*)buf + y * WIDTH_1080P + x) = color;
            } else {
                *((hi_u16*)buf + y * WIDTH_1080P + x) = color;
            }
        }
    }
    canvas_buf->update_rect.x = 0;
    canvas_buf->update_rect.y = 0;
    canvas_buf->update_rect.width = WIDTH_1080P;
    canvas_buf->update_rect.height = HEIGHT_1080P;
    ret = ioctl(info->fd, FBIO_REFRESH, canvas_buf);
    if (ret < 0) {
        SAMPLE_PRT("REFRESH failed!\n");
        return HI_FAILURE;
    }
    return HI_SUCCESS;
}

static hi_void sample_source_cfg_tde(pthread_hifb_sample_info* info, hi_tde_surface *src_surface, hi_tde_rect *src_rect,
    hi_tde_surface *dst_surface, hi_tde_rect *dst_rect)
{
    hi_u32 byte_per_pixel;
    hi_tde_color_format tde_clr_fmt;

    switch (info->color_format) {
        case HI_FB_FORMAT_ARGB8888:
            tde_clr_fmt = HI_TDE_COLOR_FORMAT_ARGB8888;
            byte_per_pixel = 4; /* 4 bytes */
            break;
        default:
            tde_clr_fmt = HI_TDE_COLOR_FORMAT_ARGB1555;
            byte_per_pixel = 2; /* 2 bytes */
            break;
    }
    src_rect->pos_x   = 0;
    src_rect->pos_y   = 0;
    src_rect->height = SAMPLE_IMAGE_HEIGHT;
    src_rect->width  = SAMPLE_IMAGE_WIDTH;
    dst_rect->pos_x   = 0;
    dst_rect->pos_y   = 0;
    dst_rect->height = src_rect->height;
    dst_rect->width  = src_rect->width;

    dst_surface->color_format = tde_clr_fmt;
    dst_surface->width = WIDTH_1080P;
    dst_surface->height = HEIGHT_1080P;
    dst_surface->stride  = WIDTH_1080P * byte_per_pixel;

    src_surface->color_format = tde_clr_fmt;
    src_surface->width = SAMPLE_IMAGE_WIDTH;
    src_surface->height = SAMPLE_IMAGE_HEIGHT;
    src_surface->stride = byte_per_pixel * SAMPLE_IMAGE_WIDTH;
    src_surface->phys_addr = g_phyaddr;
    src_surface->support_alpha_ex_1555 = HI_TRUE;
    src_surface->alpha_max_is_255 = HI_TRUE;
    src_surface->alpha0 = 0XFF;
    src_surface->alpha1 = 0XFF;
    return ;
}

static hi_s32 sample_draw_picture_by_tde(pthread_hifb_sample_info* info, hi_fb_buf *canvas_buf, hi_u32 i,
    hi_void *viraddr)
{
    hi_char image_name[128]; /* 128 for char length */
    hi_u8 *dst = HI_NULL;
    hi_tde_rect src_rect = {0};
    hi_tde_rect dst_rect = {0};
    hi_tde_surface src_surface = {0};
    hi_tde_surface dst_surface = {0};
    hi_s32 handle;
    hi_tde_single_src single_src = {0};
    hi_s32 ret;

    if (snprintf_s(image_name, sizeof(image_name), 12, SAMPLE_IMAGE1_PATH, i % 2) == -1) { /* 12 2 char lenght */
        SAMPLE_PRT("%s:%d:snprintf_s failed.\n", __FUNCTION__, __LINE__);
        return HI_FAILURE;
    }
    dst = (hi_u8*)viraddr;
    if (sample_hifb_load_bmp(image_name, dst) != HI_SUCCESS) {
        SAMPLE_PRT("sample_hifb_load_bmp failed!\n");
        return HI_FAILURE;
    }

    sample_source_cfg_tde(info, &src_surface, &src_rect, &dst_surface, &dst_rect);
    dst_surface.phys_addr = canvas_buf->canvas.phys_addr;

    /* 1. start job */
    handle = hi_tde_begin_job();
    if (handle == HI_ERR_TDE_INVALID_HANDLE) {
        SAMPLE_PRT("start job failed!\n");
        return HI_FAILURE;
    }
    single_src.src_surface = &src_surface;
    single_src.dst_surface = &dst_surface;
    single_src.src_rect = &src_rect;
    single_src.dst_rect = &dst_rect;
    ret = hi_tde_quick_copy(handle, &single_src);
    if (ret < 0) {
        SAMPLE_PRT("hi_tde_quick_copy:%d failed,ret=0x%x!\n", __LINE__, ret);
        hi_tde_cancel_job(handle);
        return HI_FAILURE;
    }

    /* 3. submit job */
    ret = hi_tde_end_job(handle, HI_FALSE, HI_TRUE, 10); /* timeout 10s */
    if (ret < 0) {
        SAMPLE_PRT("line:%d,hi_tde_end_job failed,ret=0x%x!\n", __LINE__, ret);
        hi_tde_cancel_job(handle);
        return HI_FAILURE;
    }
    return HI_SUCCESS;
}

static hi_s32 sample_time_to_play_ex(pthread_hifb_sample_info* info, hi_fb_buf *canvas_buf, hi_void *buf,
    hi_void *viraddr)
{
    hi_s32 ret;
    hi_u32 i;

    ret = hi_tde_open();
    if (ret < 0) {
        SAMPLE_PRT("hi_tde_open failed :%d!\n", ret);
        return HI_FAILURE;
    }

    SAMPLE_PRT("[begin]\n");
    SAMPLE_PRT("expected:two red line!\n");
    /* time to play */
    for (i = 0; i < SAMPLE_IMAGE_NUM; i++) {
        if (g_exit_flag == 'q') {
            printf("process exit...\n");
            break;
        }
        /* draw two lines by cpu */
        if (sample_draw_line_by_cpu_ex(info, canvas_buf, buf) != HI_SUCCESS) {
            return HI_FAILURE;
        }
        sleep(2); /* 2 second */

        if (sample_draw_picture_by_tde(info, canvas_buf, i, viraddr) != HI_SUCCESS) {
            return HI_FAILURE;
        }

        canvas_buf->update_rect.x = 0;
        canvas_buf->update_rect.y = 0;
        canvas_buf->update_rect.width = WIDTH_1080P;
        canvas_buf->update_rect.height = HEIGHT_1080P;
        ret = ioctl(info->fd, FBIO_REFRESH, canvas_buf);
        if (ret < 0) {
            SAMPLE_PRT("REFRESH failed!\n");
            return HI_FAILURE;
        }
        sleep(2); /* 2 second */
    }
    return HI_SUCCESS;
}

hi_void* sample_hifb_refresh(hi_void* data)
{
    hi_char file[FILE_LENGTH_MAX] = {0};
    hi_fb_buf canvas_buf;
    hi_void *buf = HI_NULL;
    hi_void *viraddr = HI_NULL;
    pthread_hifb_sample_info *info = HI_NULL;

    prctl(PR_SET_NAME, "HIFB_REFRESH", 0, 0, 0);
    if (data == HI_NULL) {
        return HI_NULL;
    }
    info = (pthread_hifb_sample_info*)data;

    if (sample_get_file_name(info, file, FILE_LENGTH_MAX) != HI_SUCCESS) {
        return HI_NULL;
    }

    if (sample_init_frame_buffer_ex(info, file) != HI_SUCCESS) {
        return HI_NULL;
    }

    if (sample_init_var_ex(info) != HI_SUCCESS) {
        close(info->fd);
        info->fd = -1;
        return HI_NULL;
    }

    if (sample_init_layer_info(info) != HI_SUCCESS) {
        return HI_NULL;
    }

    if (sample_init_compress(info) != HI_SUCCESS) {
        return HI_NULL;
    }

    if (sample_init_canvas(info, &canvas_buf, &buf, &viraddr) != HI_SUCCESS) {
        return HI_NULL;
    }

    if (sample_time_to_play_ex(info, &canvas_buf, buf, viraddr) != HI_SUCCESS) {
        goto ERR;
    }
    SAMPLE_PRT("[end]\n");
ERR:
    hi_mpi_sys_mmz_free(g_phyaddr, viraddr);
    g_phyaddr = 0;
    hi_mpi_sys_mmz_free(g_canvas_addr, buf);
    g_canvas_addr = 0;
    close(info->fd);
    return HI_NULL;
}

hi_s32 sample_bind_layer_to_vo(hi_vo_layer layer, hi_vo_dev dev)
{
    hi_s32 ret;
    ret = hi_mpi_vo_unbind_layer(layer, dev);
    if (ret != HI_SUCCESS) {
        SAMPLE_PRT("unbind failed!\n");
        return ret;
    }
    ret = hi_mpi_vo_bind_layer(layer, dev);
    if (ret != HI_SUCCESS) {
        SAMPLE_PRT("bind failed!\n");
        return ret;
    }
    return ret;
}

hi_s32 sample_var_init(pthread_hifb_sample_info* info, struct fb_var_screeninfo *var)
{
    if (info == HI_NULL) {
        return HI_FAILURE;
    }
    /* step 1. open framebuffer */
    info->fd = open("/dev/fb3", O_RDWR, 0);
    if (info->fd < 0) {
        SAMPLE_PRT("open /def/fb3 failed!\n");
        return HI_FAILURE;
    }
    /* step 2. get the variable screen information */
    if (ioctl(info->fd, FBIOGET_VSCREENINFO, var) < 0) {
        SAMPLE_PRT("get variable screen info failed!\n");
        close(info->fd);
        return HI_FAILURE;
    }
    /* step 3. modify the variable screen info */
    switch (info->color_format) {
        case HI_FB_FORMAT_4BPP:
            var->transp = g_a4;
            var->red    = g_r4;
            var->green  = g_g4;
            var->blue   = g_b4;
            var->bits_per_pixel = 4; /* 4 bits per pixel */
            var->xres_virtual = WIDTH_1920;
            var->yres_virtual = HEIGHT_1080 * 2; /* alloc 2 buf */
            var->xres = WIDTH_1920;
            var->yres = HEIGHT_1080;
            var->activate = 0;
            var->xoffset = 0;
            var->yoffset = 0;
            break;
        default:
            return HI_FAILURE;
    }

    if (ioctl(info->fd, FBIOPUT_VSCREENINFO, var) < 0) {
        SAMPLE_PRT("put variable screen info failed!\n");
        close(info->fd);
        return HI_FAILURE;
    }
    return HI_SUCCESS;
}

hi_s32 sample_cmap_init(pthread_hifb_sample_info* info)
{
    struct fb_cmap cmap;
    cmap.start = 0;
    cmap.len = CMAP_LENGTH_MAX;
    cmap.red = g_cmap_red;
    cmap.green = g_cmap_green;
    cmap.blue = g_cmap_blue;
    cmap.transp = g_cmap_alpha;

    if (ioctl(info->fd, FBIOPUTCMAP, &cmap) < 0) {
        SAMPLE_PRT("put cmap info failed!\n");
        close(info->fd);
        return HI_FAILURE;
    }
    return HI_SUCCESS;
}

hi_s32 sample_get_fix_and_mmap(pthread_hifb_sample_info* info, struct fb_fix_screeninfo *fix,
                               hi_void **viraddr)
{
    if (info == HI_NULL || fix == HI_NULL || viraddr == HI_NULL) {
        return HI_FAILURE;
    }
    if (ioctl(info->fd, FBIOGET_FSCREENINFO, fix) < 0) {
        SAMPLE_PRT("get fix screen info failed!\n");
        close(info->fd);
        return HI_FAILURE;
    }

    *viraddr = mmap(HI_NULL, WIDTH_1920 * HEIGHT_1080,
                    PROT_READ | PROT_WRITE, MAP_SHARED, info->fd, 0);
    if (*viraddr == HI_NULL) {
        SAMPLE_PRT("mmap /def/fb3 failed!\n");
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

static hi_s32 sample_start_draw_rect(hi_tde_none_src *none_src, hi_tde_corner_rect_info *conner_rect)
{
    hi_s32 handle;
    hi_u32 i;

    handle = hi_tde_begin_job();
    if (handle == HI_ERR_TDE_INVALID_HANDLE) {
        return HI_FAILURE;
    }

    /* times draw rect,once commit */
    for (i = 0; i < 5; i++) { /* 5 draw 5 rect */
        if (hi_tde_quick_draw_rect(handle, none_src, conner_rect) < 0) {
            hi_tde_cancel_job(handle);
            return HI_FAILURE;
        }
        none_src->dst_rect->pos_x += 200; /* 200 alg data */
        none_src->dst_rect->pos_y += 200; /* 200 alg data */
    }

    if (hi_tde_end_job(handle, HI_FALSE, HI_TRUE, 10) < 0) { /* 10 timeout */
        hi_tde_cancel_job(handle);
        return HI_FAILURE;
    }
    return HI_SUCCESS;
}

hi_s32 sample_draw_rect(hi_void* viraddr, hi_u32 index,
                        struct fb_var_screeninfo var, struct fb_fix_screeninfo fix)
{
    hi_tde_surface dst_surface = {0};
    hi_tde_rect dst_rect = {0};
    hi_tde_none_src none_src = {0};
    hi_tde_corner_rect_info conner_rect = {0};

    if (viraddr == HI_NULL) {
        return HI_FAILURE;
    }
    /* clear buffer,fill white */
    if (memset_s(viraddr, WIDTH_1920 * HEIGHT_1080, 0x33, WIDTH_1920 * HEIGHT_1080) != EOK) {
        SAMPLE_PRT("%s:%d:memset_s failed\n", __FUNCTION__, __LINE__);
        return HI_FAILURE;
    }
    if (hi_tde_open() < 0) {
        return HI_FAILURE;
    }

    dst_surface.color_format = HI_TDE_COLOR_FORMAT_CLUT4;
    dst_surface.width = WIDTH_1920;
    dst_surface.height = HEIGHT_1080;
    dst_surface.stride = fix.line_length;
    dst_surface.phys_addr = (index % 2) ? (fix.smem_start + fix.line_length * var.yres) : /* 2 alg data */
                            (fix.smem_start);
    dst_rect.pos_x = 0;
    dst_rect.pos_y = 0;
    dst_rect.height = 100; /* 100 alg data */
    dst_rect.width = 100; /* 100 alg data */
    none_src.dst_surface = &dst_surface;
    none_src.dst_rect = &dst_rect;
    /* conner rect */
    conner_rect.width = 10; /* 10 alg data */
    conner_rect.height = 20; /* 20 alg data */
    conner_rect.inner_color = 0x1;
    conner_rect.outer_color = (index % 2) ? 0x0 : 0x2; /* 2 alg data,0x2 for cmap index */

    if (sample_start_draw_rect(&none_src, &conner_rect) != HI_SUCCESS) {
        return HI_FAILURE;
    }
    return HI_SUCCESS;
}

hi_s32 sample_pandisplay(pthread_hifb_sample_info* info, hi_u32 index,
                         struct fb_var_screeninfo *var)
{
    if (info == HI_NULL || var == HI_NULL) {
        return HI_FAILURE;
    }
    if ((index % 2) == 0) { /* 2 alg data */
        var->yoffset = 0;
    } else {
        var->yoffset = var->yres;
    }

    if (ioctl(info->fd, FBIOPAN_DISPLAY, var) < 0) {
        SAMPLE_PRT("FBIOPAN_DISPLAY failed!\n");
        return HI_FAILURE;
    }

    SAMPLE_PRT("expected: buffer%d!\n", ((index % 2 == 0) ? 0 : 1)); /* 2 alg data */
    SAMPLE_PRT("wait 1 seconde.\n");
    sleep(1);
    hi_tde_close();
    return HI_SUCCESS;
}

hi_void* sample_hifb_g3_clut(void *data)
{
    pthread_hifb_sample_info* info = HI_NULL;
    struct fb_var_screeninfo var;
    struct fb_fix_screeninfo fix;
    hi_s32 ret;
    hi_void* viraddr = HI_NULL;
    hi_u32 j = 0;

    if (data == HI_NULL) {
        return HI_NULL;
    }

    info = (pthread_hifb_sample_info*)data;
    if (info->layer != GRAPHICS_LAYER_G3) {
        SAMPLE_PRT("++++%s:%d:only G3 support clut\n", __FUNCTION__, __LINE__);
        return HI_NULL;
    }

    ret = sample_bind_layer_to_vo(HI_VO_LAYER_G3, g_vo_dev);
    if (ret != HI_SUCCESS) {
        return HI_NULL;
    }

    ret = sample_var_init(info, &var);
    if (ret != HI_SUCCESS) {
        return HI_NULL;
    }

    ret = sample_cmap_init(info);
    if (ret != HI_SUCCESS) {
        return HI_NULL;
    }

    ret = sample_get_fix_and_mmap(info, &fix, &viraddr);
    if (ret != HI_SUCCESS) {
        return HI_NULL;
    }

    while (j < 20) { /* 20 times */
        if (sample_draw_rect(viraddr, j, var, fix) != HI_SUCCESS) {
            munmap(viraddr, WIDTH_1920 * HEIGHT_1080);
            close(info->fd);
            return HI_NULL;
        }
        if (sample_pandisplay(info, j, &var) != HI_SUCCESS) {
            munmap(viraddr, WIDTH_1920 * HEIGHT_1080);
            close(info->fd);
            return HI_NULL;
        }
        j++;
    }
    munmap(viraddr, WIDTH_1920 * HEIGHT_1080);
    close(info->fd);
    SAMPLE_PRT("[end]\n");
    return HI_NULL;
}


hi_void sample_hifb_handle_sig(hi_s32 signo)
{
    static int sig_handled = 0;

#ifndef __huawei_lite__
    signal(SIGINT, SIG_IGN);
    signal(SIGTERM, SIG_IGN);
#endif

    if (!sig_handled && (signo == SIGINT || signo == SIGTERM)) {
        sig_handled = 1;
        g_exit_flag = 'q';

        if (g_hifb_thread1) {
            pthread_join(g_hifb_thread1, 0);
            g_hifb_thread1 = 0;
        }

        if (g_hifb_thread) {
            pthread_join(g_hifb_thread, 0);
            g_hifb_thread = 0;
        }

        if (g_phyaddr) {
            hi_mpi_sys_mmz_free(g_phyaddr, HI_NULL);
            g_phyaddr = 0;
        }

        if (g_canvas_addr) {
            hi_mpi_sys_mmz_free(g_canvas_addr, HI_NULL);
            g_canvas_addr = 0;
        }

        sample_comm_sys_exit();

        printf("\033[0;31mprogram exit abnormally!\033[0;39m\n");
    }

    exit(-1);
}

hi_void sample_hifb_usage2(hi_void)
{
    printf("\n\n/****************index******************/\n");
    printf("please choose the case which you want to run:\n");
    printf("\t0:  ARGB8888 standard mode\n");
    printf("\t1:  ARGB1555 BUF_DOUBLE mode\n");
    printf("\t2:  ARGB1555 BUF_ONE mode\n");
    printf("\t3:  ARGB1555 BUF_NONE mode\n");
    printf("\t4:  ARGB1555 BUF_DOUBLE mode with compress\n");
    printf("\t5:  ARGB8888 BUF_DOUBLE mode with compress\n");
    printf("\t6:  CLUT4 BUF_NONE mode\n");
    printf("\t7:  rotate mode\n");
    return;
}

hi_void sample_hifb_usage1(hi_char* s_prg_nm)
{
    printf("usage : %s <index> <device> <intf>\n", s_prg_nm);
    sample_hifb_usage2();
    printf("\n/****************device******************/\n");
    printf("\t 0) VO device 0#, default.\n");
    printf("\t 1) VO device 1#.\n");
    printf("\n/****************intf******************/\n");
    printf("\t 0) VO HDMI output, default.\n");
    printf("\t 1) VO BT1120 output.\n");

    return;
}

hi_s32 sample_hifb_start_vo(vo_device_info *vo_dev_info)
{
#if HIFB_BE_WITH_VO
    hi_vo_intf_type hi_vo_intf_type = vo_dev_info->hi_vo_intf_type;
    g_vo_intf_type = vo_dev_info->hi_vo_intf_type;
    hi_vo_dev vo_dev = vo_dev_info->vo_dev;
    hi_vo_pub_attr pub_attr;
    hi_u32  vo_frm_rate;
    hi_size size;
    hi_s32 ret;

    /* step 1(start vo):  start vo device. */
    pub_attr.intf_type = hi_vo_intf_type;
    pub_attr.intf_sync = HI_VO_OUT_1080P60;
    pub_attr.bg_color = COLOR_RGB_BLUE;
    ret = sample_comm_vo_get_width_height(pub_attr.intf_sync, &size.width, &size.height, &vo_frm_rate);
    if (ret != HI_SUCCESS) {
        SAMPLE_PRT("get vo width and height failed with %d!\n", ret);
        return ret;
    }
    ret = sample_comm_vo_start_dev(vo_dev, &pub_attr);
    if (ret != HI_SUCCESS) {
        SAMPLE_PRT("start vo device failed with %d!\n", ret);
        return ret;
    }
    /*
     * step 2(start vo): bind G3 to VO device.
     * do this after VO device started.
     * set bind relationship.
     */
    ret = hi_mpi_vo_unbind_layer(HI_VO_LAYER_G3, vo_dev);
    if (ret != HI_SUCCESS) {
        SAMPLE_PRT("un_bind_graphic_layer failed with %d!\n", ret);
        return ret;
    }

    ret = hi_mpi_vo_bind_layer(HI_VO_LAYER_G3, vo_dev);
    if (ret != HI_SUCCESS) {
        SAMPLE_PRT("bind_graphic_layer failed with %d!\n", ret);
        return ret;
    }
    /* step 3(start vo): start hdmi device. */
    if (hi_vo_intf_type == HI_VO_INTF_HDMI) {
        sample_comm_vo_hdmi_start(pub_attr.intf_sync);
    }
    /* if it's displayed on bt1120, we should start bt1120 */
    if (hi_vo_intf_type == HI_VO_INTF_BT1120) {
        sample_comm_vo_bt1120_start(&pub_attr);
    }
    return HI_SUCCESS;
#else
    return HI_SUCCESS;
#endif
}

hi_void sample_hifb_stop_vo(vo_device_info *vo_dev_info)
{
#if HIFB_BE_WITH_VO
    hi_vo_intf_type hi_vo_intf_type = vo_dev_info->hi_vo_intf_type;
    hi_vo_dev vo_dev = vo_dev_info->vo_dev;

    if (hi_vo_intf_type == HI_VO_INTF_HDMI) {
        sample_comm_vo_hdmi_stop();
    }
    sample_comm_vo_stop_dev(vo_dev);
    return ;
#else
    return ;
#endif
}

hi_s32 sample_hifb_standard_mode(vo_device_info *vo_dev_info)
{
    hi_s32 ret;
    pthread_hifb_sample_info info0;
    pthread_hifb_sample_info info1;
    hi_vo_dev vo_dev = vo_dev_info->vo_dev;
    hi_vb_cfg vb_conf;

    /* step  1: init variable */
    if (memset_s(&vb_conf, sizeof(hi_vb_cfg), 0, sizeof(hi_vb_cfg)) != EOK) {
        SAMPLE_PRT("%s:%d:memset_s failed\n", __FUNCTION__, __LINE__);
        return HI_FAILURE;
    }
    /* step 2: mpp system init. */
    ret = sample_comm_sys_init(&vb_conf);
    if (ret != HI_SUCCESS) {
        SAMPLE_PRT("system init failed with %d!\n", ret);
        return ret;
    }
    /*
     * step 3: start VO device.
     * NOTE: step 3 is optional when VO is running on other system.
     */
    ret = sample_hifb_start_vo(vo_dev_info);
    if (ret != HI_SUCCESS) {
        SAMPLE_PRT("VO device %d start failed\n", vo_dev_info->vo_dev);
        goto sample_hifb_standard_mode_0;
    }
    /* step 4:  start hifb. */
    info0.layer = vo_dev;    /* VO device number */
    info0.fd = -1;
    info0.ctrlkey = 2; /* 2 none buffer */
    info0.compress = HI_FALSE; /* compress opened or not */
    info0.color_format = HI_FB_FORMAT_ARGB8888;
    if (pthread_create(&g_hifb_thread, 0, sample_hifb_pandisplay, (hi_void *)(&info0)) != 0) {
        SAMPLE_PRT("start hifb thread0 failed!\n");
        goto sample_hifb_standard_mode_1;
    }

    info1.layer = GRAPHICS_LAYER_G3;
    info1.fd = -1;
    info1.ctrlkey = 3; /* 3 point 150,150 */
    info1.compress = HI_FALSE; /* compress opened or not */
    info1.color_format = HI_FB_FORMAT_ARGB8888;
    if (pthread_create(&g_hifb_thread1, 0, sample_hifb_pandisplay, (hi_void *)(&info1)) != 0) {
        SAMPLE_PRT("start hifb thread1 failed!\n");
        goto sample_hifb_standard_mode_1;
    }
    sample_hifb_to_exit();
sample_hifb_standard_mode_1:
    sample_hifb_stop_vo(vo_dev_info);
sample_hifb_standard_mode_0:
    sample_comm_sys_exit();
    return ret;
}

hi_s32 sample_hifb_double_buf_mode(vo_device_info *vo_dev_info)
{
    hi_s32 ret;
    pthread_hifb_sample_info info0;
    hi_vo_dev vo_dev = vo_dev_info->vo_dev;
    g_vo_intf_type = vo_dev_info->hi_vo_intf_type;
    hi_vb_cfg vb_conf;

    /* step  1: init variable */
    if (memset_s(&vb_conf, sizeof(hi_vb_cfg), 0, sizeof(hi_vb_cfg)) != EOK) {
        SAMPLE_PRT("%s:%d:memset_s failed\n", __FUNCTION__, __LINE__);
        return HI_FAILURE;
    }

    /* step 2: mpp system init. */
    ret = sample_comm_sys_init(&vb_conf);
    if (ret != HI_SUCCESS) {
        SAMPLE_PRT("system init failed with %d!\n", ret);
        return ret;
    }
    /*
     * step 3: start VO device.
     * NOTE: step 3 is optional when VO is running on other system.
     */
    ret = sample_hifb_start_vo(vo_dev_info);
    if (ret != HI_SUCCESS) {
        SAMPLE_PRT("VO device %d start failed\n", vo_dev_info->vo_dev);
        goto sample_hifb_double_buf_mode_0;
    }
    /* step 4:  start hifb. */
    info0.layer = vo_dev;
    info0.fd = -1;
    info0.ctrlkey = 1;   /* double buffer */
    info0.compress = HI_FALSE;
    info0.color_format = HI_FB_FORMAT_ABGR1555;
    if (pthread_create(&g_hifb_thread, 0, sample_hifb_refresh, (hi_void*)(&info0)) != 0) {
        SAMPLE_PRT("start hifb thread failed!\n");
        goto sample_hifb_double_buf_mode_1;
    }
    sample_hifb_to_exit();
sample_hifb_double_buf_mode_1:
    sample_hifb_stop_vo(vo_dev_info);
sample_hifb_double_buf_mode_0:
    sample_comm_sys_exit();
    return ret;
}

hi_s32 sample_hifb_one_buf_mode(vo_device_info *vo_dev_info)
{
    hi_s32 ret;
    pthread_hifb_sample_info info0;
    hi_vo_dev vo_dev = vo_dev_info->vo_dev;
    g_vo_intf_type = vo_dev_info->hi_vo_intf_type;
    hi_vb_cfg vb_conf;

    /* step  1: init variable */
    if (memset_s(&vb_conf, sizeof(hi_vb_cfg), 0, sizeof(hi_vb_cfg)) != EOK) {
        SAMPLE_PRT("%s:%d:memset_s failed\n", __FUNCTION__, __LINE__);
        return HI_FAILURE;
    }
    /* step 2: mpp system init. */
    ret = sample_comm_sys_init(&vb_conf);
    if (ret != HI_SUCCESS) {
        SAMPLE_PRT("system init failed with %d!\n", ret);
        return ret;
    }
    /*
     * step 3: start VO device.
     * NOTE: step 3 is optional when VO is running on other system.
     */
    ret = sample_hifb_start_vo(vo_dev_info);
    if (ret != HI_SUCCESS) {
        SAMPLE_PRT("VO device %d start failed\n", vo_dev_info->vo_dev);
        goto sample_hifb_one_buf_mode_0;
    }
    /* step 4:  start hifb. */
    info0.layer =  vo_dev;
    info0.fd = -1;
    info0.ctrlkey = 0;   /* one buffer */
    info0.compress = HI_FALSE;
    info0.color_format = HI_FB_FORMAT_ABGR1555;
    if (pthread_create(&g_hifb_thread, 0, sample_hifb_refresh, (hi_void*)(&info0)) != 0) {
        SAMPLE_PRT("start hifb thread failed!\n");
        goto sample_hifb_one_buf_mode_1;
    }
    sample_hifb_to_exit();
sample_hifb_one_buf_mode_1:
    sample_hifb_stop_vo(vo_dev_info);
sample_hifb_one_buf_mode_0:
    sample_comm_sys_exit();
    return ret;
}

hi_s32 sample_hifb_none_buf_mode(vo_device_info *vo_dev_info)
{
    hi_s32 ret;
    pthread_hifb_sample_info info0;
    hi_vo_dev vo_dev = vo_dev_info->vo_dev;
    g_vo_intf_type = vo_dev_info->hi_vo_intf_type;
    hi_vb_cfg vb_conf;

    /* step  1: init variable */
    if (memset_s(&vb_conf, sizeof(hi_vb_cfg), 0, sizeof(hi_vb_cfg)) != EOK) {
        SAMPLE_PRT("%s:%d:memset_s failed\n", __FUNCTION__, __LINE__);
        return HI_FAILURE;
    }
    /* step 2: mpp system init. */
    ret = sample_comm_sys_init(&vb_conf);
    if (ret != HI_SUCCESS) {
        SAMPLE_PRT("system init failed with %d!\n", ret);
        return ret;
    }
    /*
     * step 3: start VO device.
     * NOTE: step 3 is optional when VO is running on other system.
     */
    ret = sample_hifb_start_vo(vo_dev_info);
    if (ret != HI_SUCCESS) {
        SAMPLE_PRT("VO device %d start failed\n", vo_dev_info->vo_dev);
        goto sample_hifb_none_buf_mode_0;
    }
    /* step 4:  start hifb. */
    info0.layer = vo_dev;
    info0.fd = -1;
    info0.ctrlkey = 3; /* 3: none buffer */
    info0.compress = HI_FALSE;
    info0.color_format = HI_FB_FORMAT_ABGR1555;
    if (pthread_create(&g_hifb_thread, 0, sample_hifb_refresh, (hi_void*)(&info0)) != 0) {
        SAMPLE_PRT("start hifb thread failed!\n");
        goto sample_hifb_none_buf_mode_1;
    }
    sample_hifb_to_exit();
sample_hifb_none_buf_mode_1:
    sample_hifb_stop_vo(vo_dev_info);
sample_hifb_none_buf_mode_0:
    sample_comm_sys_exit();
    return ret;
}

hi_s32 sample_hifb_dcmp(vo_device_info *vo_dev_info)
{
    hi_s32 ret;
    pthread_hifb_sample_info info0;
    hi_vo_dev vo_dev = vo_dev_info->vo_dev;
    g_vo_intf_type = vo_dev_info->hi_vo_intf_type;
    hi_vb_cfg vb_conf;

    /* step  1: init variable */
    if (memset_s(&vb_conf, sizeof(hi_vb_cfg), 0, sizeof(hi_vb_cfg)) != EOK) {
        SAMPLE_PRT("%s:%d:memset_s failed\n", __FUNCTION__, __LINE__);
        return HI_FAILURE;
    }

    /* step 2: mpp system init. */
    ret = sample_comm_sys_init(&vb_conf);
    if (ret != HI_SUCCESS) {
        SAMPLE_PRT("system init failed with %d!\n", ret);
        return HI_FAILURE;
    }
    /*
     * step 3: start VO device.
     * NOTE: step 3 is optional when VO is running on other system.
     */
    ret = sample_hifb_start_vo(vo_dev_info);
    if (ret != HI_SUCCESS) {
        SAMPLE_PRT("VO device %d start failed\n", vo_dev_info->vo_dev);
        goto sample_hifb_double_buf_mode_0;
    }

    /* step 4:  start hifb. */
    info0.layer =  vo_dev;
    info0.fd = -1;
    info0.ctrlkey =  1;
    info0.compress = HI_TRUE;
    info0.color_format = HI_FB_FORMAT_ABGR1555;
    if (pthread_create(&g_hifb_thread, 0, sample_hifb_refresh, (hi_void*)(&info0)) != 0) {
        SAMPLE_PRT("start hifb thread failed!\n");
        goto sample_hifb_double_buf_mode_1;
    }

    sample_hifb_to_exit();

sample_hifb_double_buf_mode_1:
    sample_hifb_stop_vo(vo_dev_info);
sample_hifb_double_buf_mode_0:
    sample_comm_sys_exit();

    return ret;
}

hi_s32 sample_hifb_dcmp_argb8888(vo_device_info *vo_dev_info)
{
    hi_s32 ret;
    pthread_hifb_sample_info info0;
    hi_vo_dev vo_dev = vo_dev_info->vo_dev;
    g_vo_intf_type = vo_dev_info->hi_vo_intf_type;
    hi_vb_cfg vb_conf;

    /* step  1: init variable */
    if (memset_s(&vb_conf, sizeof(hi_vb_cfg), 0, sizeof(hi_vb_cfg)) != EOK) {
        SAMPLE_PRT("%s:%d:memset_s failed\n", __FUNCTION__, __LINE__);
        return HI_FAILURE;
    }

    /* step 2: mpp system init. */
    ret = sample_comm_sys_init(&vb_conf);
    if (ret != HI_SUCCESS) {
        SAMPLE_PRT("system init failed with %d!\n", ret);
        return HI_FAILURE;
    }

    /*
     * step 3: start VO device.
     * NOTE: step 3 is optional when VO is running on other system.
     */
    ret = sample_hifb_start_vo(vo_dev_info);
    if (ret != HI_SUCCESS) {
        SAMPLE_PRT("VO device %d start failed\n", vo_dev_info->vo_dev);
        goto sample_hifb_double_buf_mode_0;
    }
    /* step 4:  start hifb. */
    info0.layer = vo_dev;
    info0.fd = -1;
    info0.ctrlkey = 1;
    info0.compress = HI_TRUE;
    info0.color_format = HI_FB_FORMAT_ARGB8888;
    if (pthread_create(&g_hifb_thread, 0, sample_hifb_refresh, (hi_void*)(&info0)) != 0) {
        SAMPLE_PRT("start hifb thread failed!\n");
        goto sample_hifb_double_buf_mode_1;
    }

    sample_hifb_to_exit();

sample_hifb_double_buf_mode_1:
    sample_hifb_stop_vo(vo_dev_info);
sample_hifb_double_buf_mode_0:
    sample_comm_sys_exit();

    return ret;
}

hi_s32 sample_hifb_clut_mode(vo_device_info *vo_dev_info)
{
    hi_s32  ret;
    pthread_hifb_sample_info info0;
    g_vo_intf_type  = vo_dev_info->hi_vo_intf_type;
    g_vo_dev = vo_dev_info->vo_dev;
    hi_vb_cfg  vb_conf;

    /* step  1: init variable */
    if (memset_s(&vb_conf, sizeof(hi_vb_cfg), 0, sizeof(hi_vb_cfg)) != EOK) {
        SAMPLE_PRT("%s:%d:memset_s failed\n", __FUNCTION__, __LINE__);
        return HI_FAILURE;
    }

    /* step 2: mpp system init. */
    ret = sample_comm_sys_init(&vb_conf);
    if (ret != HI_SUCCESS) {
        SAMPLE_PRT("system init failed with %d!\n", ret);
        goto sample_hifb_clut_mode_0;
    }

    /* step 3: start VO device.
     NOTE: step 3 is optional when VO is running on other system. */
    ret = sample_hifb_start_vo(vo_dev_info);
    if (ret != HI_SUCCESS) {
        SAMPLE_PRT("VO device %d start failed\n", vo_dev_info->vo_dev);
        goto sample_hifb_clut_mode_0;
    }

    /* step 4:  start hifb. */
    info0.layer =  GRAPHICS_LAYER_G3;
    info0.fd  = -1;
    info0.ctrlkey = 2; /* 2 0buffer pan display */
    info0.compress = HI_FALSE;
    info0.color_format = HI_FB_FORMAT_4BPP;
    if (pthread_create(&g_hifb_thread, 0, sample_hifb_g3_clut, (void *)(&info0)) != 0) {
        SAMPLE_PRT("start hifb thread failed!\n");
        goto sample_hifb_clut_mode_1;
    }

    sample_hifb_to_exit();

sample_hifb_clut_mode_1:
    sample_hifb_stop_vo(vo_dev_info);
sample_hifb_clut_mode_0:
    sample_comm_sys_exit();

    return ret;
}

#ifdef CONFIG_SUPPORT_SAMPLE_ROTATION
static hi_void hifb_rotate(hi_vo_dev vo_dev)
{
    hi_s32 ret;
    pthread_hifb_sample_info info;

    info.layer = vo_dev;
    info.fd = -1;
    info.ctrlkey = 1; /* 0 : one buf 1: double buf */
    info.compress = HI_FALSE;
    info.color_format = HI_FB_FORMAT_ARGB1555;

    info.fd = open("/dev/fb0", O_RDWR, 0);
    if (info.fd < 0) {
        SAMPLE_PRT("open /dev/fb0 failed!\n");
        return;
    }

    ret = hifb_get_canvas(&info);
    if (ret != HI_SUCCESS) {
        close(info.fd);
        return;
    }

    ret = hifb_draw(&info);
    if (ret != HI_SUCCESS) {
        hifb_put_canvas();
        close(info.fd);
        return;
    }

    ret = hifb_put_rotation(&info);
    if (ret != HI_SUCCESS) {
        hifb_put_canvas();
        close(info.fd);
        return;
    }

    ret = hifb_refresh(&info);
    if (ret != HI_SUCCESS) {
        hifb_put_canvas();
        close(info.fd);
        return;
    }

    hifb_put_canvas();
    close(info.fd);

    return;
}

static hi_s32 hifb_put_layer_info(pthread_hifb_sample_info *info)
{
    hi_s32 ret;
    hi_fb_layer_info layer_info = {0};

    ret = ioctl(info->fd, FBIOGET_LAYER_INFO, &layer_info);
    if (ret < 0) {
        SAMPLE_PRT("GET_LAYER_INFO failed!\n");
        return HI_FAILURE;
    }
    layer_info.mask = 0;
    layer_info.antiflicker_level = HI_FB_LAYER_ANTIFLICKER_AUTO;
    layer_info.mask |= HI_FB_LAYER_MASK_BUF_MODE;
    layer_info.mask |= HI_FB_LAYER_MASK_ANTIFLICKER_MODE;
    switch (info->ctrlkey) {
        case 0: /* 0 one buf */
            layer_info.buf_mode = HI_FB_LAYER_BUF_ONE;
            break;
        case 1: /* 1 double buf */
            layer_info.buf_mode = HI_FB_LAYER_BUF_DOUBLE;
            break;
        default:
            layer_info.buf_mode = HI_FB_LAYER_BUF_NONE;
    }

    ret = ioctl(info->fd, FBIOPUT_LAYER_INFO, &layer_info);
    if (ret < 0) {
        SAMPLE_PRT("PUT_LAYER_INFO failed!\n");
        return HI_FAILURE;
    }
    return HI_SUCCESS;
}

static hi_s32 hifb_get_canvas(pthread_hifb_sample_info* info)
{
    hi_s32 ret;
    hi_u32 byte_per_pixel = (info->color_format == HI_FB_FORMAT_ARGB8888) ? 4 : 2; /* 4 2 bytes per pixel */
    ret = hi_mpi_sys_mmz_alloc(&g_canvas_phy, ((hi_void**)&g_canvas_vir), HI_NULL, HI_NULL, WIDTH_800 *
        HEIGHT_600 * byte_per_pixel * 2); /* 2 double canvas buf */
    if (ret != HI_SUCCESS) {
        SAMPLE_PRT("allocate memory (maxW*maxH*%d bytes) failed\n", byte_per_pixel);
        return HI_FAILURE;
    }

    if ((g_canvas_phy == 0) || (g_canvas_vir == NULL)) {
        return HI_FAILURE;
    }
    (hi_void)memset_s(g_canvas_vir, WIDTH_800 * HEIGHT_600 * byte_per_pixel * 2, 0xff, /* 2 double canvas buf */
        WIDTH_800 * HEIGHT_600 * byte_per_pixel * 2); /* 2 double canvas buf */
    ret = hi_mpi_sys_mmz_alloc(&g_picture_phy, ((hi_void**)&g_picture_vir), NULL, NULL, SAMPLE_IMAGE_WIDTH *
        SAMPLE_IMAGE_HEIGHT * byte_per_pixel);
    if (ret != HI_SUCCESS) {
        SAMPLE_PRT("allocate memory (maxW*maxH*%d bytes) failed\n", byte_per_pixel);
        return HI_FAILURE;
    }
    if ((g_picture_phy == 0) || (g_picture_vir == NULL)) {
        return HI_FAILURE;
    }
    (hi_void)memset_s(g_picture_vir, SAMPLE_IMAGE_WIDTH * SAMPLE_IMAGE_HEIGHT * byte_per_pixel, 0xff,
        SAMPLE_IMAGE_WIDTH * SAMPLE_IMAGE_HEIGHT * byte_per_pixel);
    return HI_SUCCESS;
}

static hi_void hifb_put_canvas(hi_void)
{
    if ((g_canvas_phy == 0) || (g_canvas_vir == NULL) || (g_picture_phy == 0) || (g_picture_vir == NULL)) {
        return;
    }

    hi_mpi_sys_mmz_free(g_canvas_phy, g_canvas_vir);
    g_canvas_phy = 0;
    g_canvas_vir = NULL;
    hi_mpi_sys_mmz_free(g_picture_phy, g_picture_vir);
    g_picture_phy = 0;
    g_picture_vir = NULL;

    return;
}

static hi_void hifb_init_surface(hi_tde_surface *src_surface, hi_tde_surface *dst_surface,
    hi_tde_rect *src_rect, hi_tde_rect *dst_rect, hi_u32 byte_per_pixel)
{
    src_rect->pos_x = dst_rect->pos_x = 0;
    src_rect->pos_y = dst_rect->pos_y = 0;
    src_rect->height = dst_rect->height = SAMPLE_IMAGE_HEIGHT;
    src_rect->width = dst_rect->width = SAMPLE_IMAGE_WIDTH;
    src_surface->color_format = dst_surface->color_format = HI_TDE_COLOR_FORMAT_ARGB1555;
    src_surface->width = SAMPLE_IMAGE_WIDTH;
    src_surface->height = SAMPLE_IMAGE_HEIGHT;
    src_surface->stride = byte_per_pixel * SAMPLE_IMAGE_WIDTH;
    src_surface->phys_addr = g_picture_phy;
    src_surface->support_alpha_ex_1555 = dst_surface->support_alpha_ex_1555 = HI_TRUE;
    src_surface->alpha_max_is_255 = dst_surface->alpha_max_is_255 = HI_TRUE;
    src_surface->alpha0 = dst_surface->alpha0 = 0XFF;
    src_surface->alpha1 = dst_surface->alpha1 = 0XFF;
    dst_surface->width = WIDTH_800;
    dst_surface->height = HEIGHT_600;
    dst_surface->stride = WIDTH_800 * byte_per_pixel;
    dst_surface->phys_addr = g_canvas_phy;
    dst_surface->is_ycbcr_clut = HI_FALSE;
    return;
}

static hi_s32 hifb_draw_by_tde(hi_tde_surface *src_surface, hi_tde_surface *dst_surface,
    hi_tde_rect *src_rect, hi_tde_rect *dst_rect, hi_s32 index)
{
    hi_s32 ret, handle;
    hi_tde_single_src single_src = {0};
    if (index == 1) {
        sample_hifb_load_bmp("./res/1.bmp", g_picture_vir);
    } else {
        sample_hifb_load_bmp("./res/0.bmp", g_picture_vir);
        dst_surface->phys_addr = g_canvas_phy + dst_surface->stride * dst_surface->height;
    }

    /* 1. start job */
    handle = hi_tde_begin_job();
    if (handle == HI_ERR_TDE_INVALID_HANDLE) {
        SAMPLE_PRT("start job failed!\n");
        return HI_FAILURE;
    }
    single_src.src_surface = src_surface;
    single_src.src_rect = src_rect;
    single_src.dst_surface = dst_surface;
    single_src.dst_rect = dst_rect;
    ret = hi_tde_quick_copy(handle, &single_src);
    if (ret < 0) {
        SAMPLE_PRT("hi_tde_quick_copy:%d failed,ret=0x%x!\n", __LINE__, ret);
        hi_tde_cancel_job(handle);
        return HI_FAILURE;
    }

    /* 3. submit job */
    ret = hi_tde_end_job(handle, HI_FALSE, HI_TRUE, 10000); /* 10000 timeout */
    if (ret < 0) {
        SAMPLE_PRT("Line:%d,hi_tde_end_job failed,ret=0x%x!\n", __LINE__, ret);
        hi_tde_cancel_job(handle);
        return HI_FAILURE;
    }
    return HI_SUCCESS;
}

static hi_s32 hifb_draw(pthread_hifb_sample_info *info)
{
    hi_s32 ret;
    hi_tde_rect dst_rect, src_rect;
    hi_tde_surface dst_surface = {0};
    hi_tde_surface src_surface = {0};
    hi_u32 byte_per_pixel = (info->color_format == HI_FB_FORMAT_ARGB8888) ? 4 : 2; /* 4 2 bytes per pixel */

    ret = hi_tde_open();
    if (ret < 0) {
        SAMPLE_PRT("hi_tde_open failed :%d!\n", ret);
        return HI_FAILURE;
    }

    hifb_init_surface(&src_surface, &dst_surface, &src_rect, &dst_rect, byte_per_pixel);

    ret = hifb_draw_by_tde(&src_surface, &dst_surface, &src_rect, &dst_rect, 1);
    if (ret != HI_SUCCESS) {
        return HI_FAILURE;
    }
    ret = hifb_draw_by_tde(&src_surface, &dst_surface, &src_rect, &dst_rect, 0);
    if (ret != HI_SUCCESS) {
        return HI_FAILURE;
    }

    hi_tde_close();
    return HI_SUCCESS;
}

static hi_s32 hifb_refresh(pthread_hifb_sample_info *info)
{
    hi_s32 ret;
    hi_fb_buf canvas_buf;
    hi_u32 byte_per_pixel;
    hi_s32 i;

    switch (info->color_format) {
        case HI_FB_FORMAT_ARGB8888:
            byte_per_pixel = 4; /* 4 bytes */
            break;
        default:
            byte_per_pixel = 2; /* 2 bytes */
            break;
    }
    canvas_buf.canvas.phys_addr = g_canvas_phy;
    canvas_buf.canvas.height = HEIGHT_600;
    canvas_buf.canvas.width = WIDTH_800;
    canvas_buf.canvas.pitch = WIDTH_800 * (byte_per_pixel);
    canvas_buf.canvas.format = info->color_format;
    canvas_buf.update_rect.x = 0;
    canvas_buf.update_rect.y = 0;
    canvas_buf.update_rect.width = WIDTH_800;
    canvas_buf.update_rect.height = HEIGHT_600;

    for (i = 0; i < 10; i++) { /* 10 times */
        if (i % 2 != 0) { /* 2 change 0.bmp or 1.bmp */
            canvas_buf.canvas.phys_addr = g_canvas_phy + canvas_buf.canvas.pitch * canvas_buf.canvas.height;
        } else {
            canvas_buf.canvas.phys_addr = g_canvas_phy;
        }
        ret = ioctl(info->fd, FBIO_REFRESH, &canvas_buf);
        if (ret < 0) {
            SAMPLE_PRT("REFRESH failed!\n");
            return HI_FAILURE;
        }

        SAMPLE_PRT("wait 1 seconds\n");
        usleep(1 * 1000 * 1000); /* 1000 1seconds */
    }

    return HI_SUCCESS;
}

static hi_void hifb_get_var_by_format(pthread_hifb_sample_info *info, struct fb_var_screeninfo *var_info)
{
    switch (info->color_format) {
        case HI_FB_FORMAT_ARGB8888:
            var_info->transp = g_a32;
            var_info->red    = g_r32;
            var_info->green  = g_g32;
            var_info->blue   = g_b32;
            var_info->bits_per_pixel = 32; /* 32 bits */
            break;
        default:
            var_info->transp = g_a16;
            var_info->red    = g_r16;
            var_info->green  = g_g16;
            var_info->blue   = g_b16;
            var_info->bits_per_pixel = 16; /* 16 bits */
            break;
    }
}

static hi_void hifb_put_rotation_degree(struct fb_var_screeninfo *var_info, hi_fb_rotate_mode *rotate_mode)
{
    hi_char input;
    printf("\n\n/****************index******************/\n");
    printf("please input 1 or 2 or 3 to choose the case which you want to run:\n");
    printf("\t1:  rotate 90\n");
    printf("\t2:  rotate 180\n");
    printf("\t3:  rotate 270\n");

    input = getchar();
    if (input == '1') {
        var_info->xres = var_info->xres_virtual = HEIGHT_600;
        var_info->yres = var_info->yres_virtual = WIDTH_800;
        *rotate_mode = HI_FB_ROTATE_90;
    } else if (input == '2') {
        var_info->xres = var_info->xres_virtual = WIDTH_800;
        var_info->yres = var_info->yres_virtual = HEIGHT_600;
        *rotate_mode = HI_FB_ROTATE_180;
    } else if (input == '3') {
        var_info->xres = var_info->xres_virtual = HEIGHT_600;
        var_info->yres = var_info->yres_virtual = WIDTH_800;
        *rotate_mode = HI_FB_ROTATE_270;
    } else {
        var_info->xres = var_info->xres_virtual = WIDTH_800;
        var_info->yres = var_info->yres_virtual = HEIGHT_600;
        *rotate_mode = HI_FB_ROTATE_NONE;
        SAMPLE_PRT("input param invalid, no rotate!\n");
    }
}

static hi_s32 hifb_put_rotation(pthread_hifb_sample_info *info)
{
    hi_s32 ret;
    struct fb_var_screeninfo var_info = {0};
    hi_fb_rotate_mode rotate_mode = HI_FB_ROTATE_BUTT;

    ret = ioctl(info->fd, FBIOGET_VSCREENINFO, &var_info);
    if (ret < 0) {
        SAMPLE_PRT("FBIOGET_VSCREENINFO failed!\n");
        return HI_FAILURE;
    }

    hifb_get_var_by_format(info, &var_info);

    hifb_put_rotation_degree(&var_info, &rotate_mode);

    ret = ioctl(info->fd, FBIOPUT_VSCREENINFO, &var_info);
    if (ret < 0) {
        SAMPLE_PRT("PUT_VSCREENINFO failed!\n");
        return HI_FAILURE;
    }

    ret = hifb_put_layer_info(info);
    if (ret != HI_SUCCESS) {
        SAMPLE_PRT("PUT_LAYER_INFO failed!\n");
        return HI_FAILURE;
    }

    ret = ioctl (info->fd, FBIOPUT_ROTATE_MODE, &rotate_mode);
    if (ret < 0) {
        SAMPLE_PRT("rotate failed!\n");
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

hi_s32 sample_hifb_rotation(vo_device_info *vo_dev_info)
{
    hi_s32 ret;
    hi_vb_cfg vb_conf;

    if (memset_s(&vb_conf, sizeof(hi_vb_cfg), 0, sizeof(hi_vb_cfg)) != EOK) {
        SAMPLE_PRT("%s:%d:memset_s failed\n", __FUNCTION__, __LINE__);
        return HI_FAILURE;
    }
    ret = sample_comm_sys_init(&vb_conf);
    if (ret != HI_SUCCESS) {
        SAMPLE_PRT("system init failed with %d!\n", ret);
        goto sample_hifb_rotation_0;
    }

    /* open display */
    ret = sample_hifb_start_vo(vo_dev_info);
    if (ret != HI_SUCCESS) {
        SAMPLE_PRT("VO device %d start failed\n", vo_dev_info->vo_dev);
        goto sample_hifb_rotation_0;
    }

    hifb_rotate(vo_dev_info->vo_dev);

    sample_hifb_stop_vo(vo_dev_info);
sample_hifb_rotation_0:
    sample_comm_sys_exit();
    return (ret);
}
#endif

static hi_s32 sample_choose_the_case(char **argv, vo_device_info *vo_dev_info)
{
    hi_s32 ret = HI_FAILURE;
    hi_char ch;
    ch = *(argv[1]);
    g_exit_flag = 0;
    if (ch == '0') {
        SAMPLE_PRT("\nindex 0 selected.\n");
        ret = sample_hifb_standard_mode(vo_dev_info);
    } else if (ch == '1') {
        SAMPLE_PRT("\nindex 1 selected.\n");
        ret = sample_hifb_double_buf_mode(vo_dev_info);
    } else if (ch == '2') {
        SAMPLE_PRT("\nindex 2 selected.\n");
        ret = sample_hifb_one_buf_mode(vo_dev_info);
    } else if (ch == '3') {
        SAMPLE_PRT("\nindex 3 selected.\n");
        ret = sample_hifb_none_buf_mode(vo_dev_info);
    } else if (ch == '4') {
        SAMPLE_PRT("\nindex 4 selected.\n");
        ret = sample_hifb_dcmp(vo_dev_info);
    } else if (ch == '5') {
        SAMPLE_PRT("\nindex 5 selected.\n");
        ret = sample_hifb_dcmp_argb8888(vo_dev_info);
    } else if (ch == '6') {
        SAMPLE_PRT("\nindex 6 selected.\n");
        ret = sample_hifb_clut_mode(vo_dev_info);
#ifdef CONFIG_SUPPORT_SAMPLE_ROTATION
    } else if (ch == '7') {
        SAMPLE_PRT("\nindex 7 selected.\n");
        ret = sample_hifb_rotation(vo_dev_info);
#endif
    } else {
        printf("index invaild! please try again.\n");
        sample_hifb_usage1(argv[0]);
        return HI_FAILURE;
    }
    if (ret == HI_SUCCESS) {
        SAMPLE_PRT("program exit normally!\n");
    } else {
        SAMPLE_PRT("program exit abnormally!\n");
    }
    return ret;
}

#ifdef __huawei_lite__
#define SAMPLE_HIFB_NAME "sample"
void sample_vo_sel_usage(hi_void)
{
    printf("usage : %s <index> <device> <intf>\n", SAMPLE_HIFB_NAME);
    sample_hifb_usage2();
    printf("\n/****************device******************/\n");
    printf("\t 0) VO device 0#, default.\n");
    printf("\t 1) VO device 1#.\n");
    printf("\n/****************intf******************/\n");
    printf("\t 0) VO HDMI output, default.\n");
    printf("\t 1) VO BT1120 output.\n");
    return;
}
#endif

#ifdef __huawei_lite__
int app_main(int argc, char *argv[])
{
    hi_s32 ret;
    vo_device_info vo_dev_info;
    vo_dev_info.vo_dev = SAMPLE_VO_DEV_DHD0;
    vo_dev_info.hi_vo_intf_type = HI_VO_INTF_HDMI; /* default:HDMI or BT1120 */
    hi_char sel_dev; /* '0': VO_INTF_CVBS, else: BT1120 */
    /* 4 2 for alg */
    if ((argc < 4) || (1 != strlen(argv[1])) || (1 != strlen(argv[2]))) {
        sample_vo_sel_usage();
        return HI_FAILURE;
    }
    if ((argc > 2) && *argv[2] == '1') { /* 2 for alg */
        vo_dev_info.vo_dev = SAMPLE_VO_DEV_DHD1;
    }
    /* '0': HI_VO_INTF_HDMI, else: HI_VO_INTF_BT1120 */
    if ((argc > 2) && *argv[3] == '1') { /* 2 3 for alg */
        vo_dev_info.hi_vo_intf_type = HI_VO_INTF_BT1120;
    }

    ret = sample_choose_the_case(argv, &vo_dev_info);
    if (ret != HI_SUCCESS) {
        return ret;
    }
    return ret;
}
#else
int sample_hifb_main(int argc, char* argv[])
{
    hi_s32 ret;
    vo_device_info vo_dev_info;
    vo_dev_info.vo_dev = SAMPLE_VO_DEV_DHD0;
    vo_dev_info.hi_vo_intf_type = HI_VO_INTF_HDMI; /* default:HDMI or BT1120 */
    /* 4 2 3 for alg */
    if ((argc != 4) || (strlen(argv[1]) != 1) || (strlen(argv[2]) != 1) || (strlen(argv[3]) != 1)) {
        printf("index invaild! please try again.\n");
        sample_hifb_usage1(argv[0]);
        return HI_FAILURE;
    }
    if (*argv[2] != '0' && *argv[2] != '1') { /* 2 for alg */
        printf("index invaild! please try again.\n");
        sample_hifb_usage1(argv[0]);
        return HI_FAILURE;
    }
    if (*argv[3] != '0' && *argv[3] != '1') { /* 3 for alg */
        printf("index invaild! please try again.\n");
        sample_hifb_usage1(argv[0]);
        return HI_FAILURE;
    }

    signal(SIGINT, sample_hifb_handle_sig);
    signal(SIGTERM, sample_hifb_handle_sig);
    /* '0': DHD0, else: DHD1 */
    if ((argc > 2) && *argv[2] == '1') { /* 2 for alg */
        vo_dev_info.vo_dev = SAMPLE_VO_DEV_DHD1;
    }
    /* '0': HI_VO_INTF_HDMI, else: HI_VO_INTF_BT1120 */
    if ((argc > 2) && *argv[3] == '1') { /* 2 3 for alg */
        vo_dev_info.hi_vo_intf_type = HI_VO_INTF_BT1120;
    }

    ret = sample_choose_the_case(argv, &vo_dev_info);
    if (ret != HI_SUCCESS) {
        return ret;
    }
    return ret;
}
#endif
