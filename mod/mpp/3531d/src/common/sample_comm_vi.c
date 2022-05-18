/*
 * Copyright (c) Hisilicon Technologies Co., Ltd. 2019-2020. All rights reserved.
 * Description: vi common function for sample.
 * Author: Hisilicon multimedia software group
 * Create: 2019/5/07
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
#include <sys/prctl.h>

#include "hi_common.h"
#include "hi_mipi_rx.h"
#include "sample_comm.h"

#if SAMPLE_AD_TYPE == AD_NVP6158
#include "common.h"
#include "video.h"
#elif SAMPLE_AD_TYPE == AD_TP2854B
#include "tp2854.h"
#elif SAMPLE_AD_TYPE == AD_TP2856
#include "tp2856.h"
#endif

#define MIPI_DEV_NODE "/dev/hi_mipi"
#define MAX_FRAME_WIDTH 8192

#define CHN_NUM_PRE_CHIP 4

hi_vi_dev_attr g_sample_vi_dev_attr = {
    /* interface mode */
    HI_VI_MODE_MIPI_YUV422,
    /* work mode, 1/2/4 multiplex */
    HI_VI_WORK_MODE_MULTIPLEX_1,
    /* r_mask    g_mask    b_mask */
    { 0xFF000000, 0x0 },
    /* for single/double edge, must be set when double edge */
    HI_VI_CLK_EDGE_SINGLE_UP,
    /* enDataSeq, just support yuv */
    HI_VI_IN_DATA_YVYU,
    /* sync info */
    {  /* port_vsync  port_vsync_neg  port_hsync  port_hsync_neg */
        HI_VI_VSYNC_FIELD,
        HI_VI_VSYNC_NEG_HIGH,
        HI_VI_HSYNC_VALID_SIG,
        HI_VI_HSYNC_NEG_HIGH,
        HI_VI_VSYNC_VALID_SIG,
        HI_VI_VSYNC_VALID_NEG_HIGH,
        /*
         * timing info:
         * hsync_hfb  hsync_act  hsync_hhb
         * vsync0_vhb vsync0_act vsync0_hhb
         * vsync1_vhb vsync1_act vsync1_hhb
         */
        { 0, 0, 0,
          0, 0, 0,
          0, 0, 0 }
    },
    /* data_reverse_en */
    HI_FALSE,
    /* mixed_capture_en */
    HI_FALSE,
};

hi_vi_chn_attr g_sample_vi_chn_attr = {
    /* crop_x crop_y, crop_w  crop_h */
    { 0, 0, 1920, 1080 },
    /* dest_w  dest_h */
    { 1920, 1080 },
    /* capture_select */
    HI_VI_CAPTURE_SELECT_BOTH,
    /* scan mode */
    HI_VI_SCAN_PROGRESSIVE,
    /* pixel_format */
    HI_PIXEL_FORMAT_YVU_SEMIPLANAR_420,
    /* mirror_en  flip_en */
    0,
    0,
    /* compress mode */
    HI_COMPRESS_MODE_SEG_COMPACT,
    /* src_frame_rate   dst_frame_rate */
    -1,
    -1
};

hi_s32 sample_common_vi_get_size_by_mode(sample_vi_mode vi_mode, hi_pic_size *pic_size)
{
    if (pic_size == HI_NULL) {
        return HI_FAILURE;
    }

    switch (vi_mode) {
        case SAMPLE_VI_MODE_960H_4X4:
            *pic_size = PIC_960H;
            break;

        case SAMPLE_VI_MODE_720P_1X4:
        case SAMPLE_VI_MODE_720P_2X2:
        case SAMPLE_VI_MODE_720P_4X4:
            *pic_size = PIC_720P;
            break;

        case SAMPLE_VI_MODE_1080P_2X2:
        case SAMPLE_VI_MODE_1080P_1X8:  
            *pic_size = PIC_1080P;
            break;

        case SAMPLE_VI_MODE_4K_1X8:
            *pic_size = PIC_3840x2160;
            break;

        default:
            *pic_size = PIC_3840x2160;
            break;
    }

    return HI_SUCCESS;
}

hi_void sample_common_vi_set_param_4x4(sample_vi_mode vi_mode, sample_vi_param *vi_param)
{
    vi_param->vi_dev_cnt = 4; /* 4 : number of dev */
    vi_param->vi_dev_interval = 1;
    vi_param->vi_chn_cnt = 16; /* 16 : number of chn */
    vi_param->vi_chn_interval = 1;
    vi_param->minor_attr_en = HI_FALSE;
    if (vi_mode == SAMPLE_VI_MODE_960H_4X4) {
        vi_param->scan_mode = HI_VI_SCAN_INTERLACED;
        vi_param->minor_dst_size.width = _960H_WIDTH;
        vi_param->minor_dst_size.height = _960H_HEIGHT_PAL;
    } else if (SAMPLE_VI_MODE_720P_4X4) {
        vi_param->scan_mode = HI_VI_SCAN_PROGRESSIVE;
        vi_param->minor_dst_size.width = HD_WIDTH;
        vi_param->minor_dst_size.height = HD_HEIGHT;
    }
}

hi_void sample_common_vi_set_param_1x4(sample_vi_mode vi_mode, sample_vi_param *vi_param)
{
    vi_param->vi_dev_cnt = 4; /* 4 : number of dev */
    vi_param->vi_dev_interval = 1;
    vi_param->vi_chn_cnt = 4; /* 4 : number of chn */
    vi_param->vi_chn_interval = 4; /* 4 : chn interval */
    vi_param->scan_mode = HI_VI_SCAN_PROGRESSIVE;
    vi_param->minor_attr_en = HI_TRUE;
    vi_param->minor_dst_size.width = D1_WIDTH;
    vi_param->minor_dst_size.height = D1_HEIGHT_NTSC;
}

hi_void sample_common_vi_set_param_2x2(sample_vi_mode vi_mode, sample_vi_param *vi_param)
{
    vi_param->vi_dev_cnt = 2; /* 2 : number of dev */
    vi_param->vi_dev_interval = 1;
    vi_param->vi_chn_cnt = 4; /* 4 : number of chn */
    vi_param->vi_chn_interval = 2; /* 2 : chn interval */
    vi_param->scan_mode = HI_VI_SCAN_PROGRESSIVE;
    vi_param->minor_attr_en = HI_TRUE;
    if (vi_mode == SAMPLE_VI_MODE_720P_2X2) {
        vi_param->minor_dst_size.width = D1_WIDTH;
        vi_param->minor_dst_size.height = D1_HEIGHT_NTSC;
    } else if (SAMPLE_VI_MODE_1080P_2X2) {
        vi_param->minor_dst_size.width = HD_WIDTH;
        vi_param->minor_dst_size.height = HD_HEIGHT;
    }
}

hi_void sample_common_vi_set_param_1x8(sample_vi_mode vi_mode, sample_vi_param *vi_param)
{
    vi_param->vi_dev_cnt = 8; /* 8 : number of dev */
    vi_param->vi_dev_interval = 1;
    vi_param->vi_chn_cnt = 8; /* 8 : number of chn */
    vi_param->vi_chn_interval = 4; /* 4 : chn interval */
    vi_param->scan_mode = HI_VI_SCAN_PROGRESSIVE;
    vi_param->minor_attr_en = HI_TRUE;
    vi_param->minor_dst_size.width = D1_WIDTH;//HD_WIDTH;
    vi_param->minor_dst_size.height = D1_HEIGHT_NTSC;//HD_HEIGHT;
}

hi_s32 sample_common_vi_mode_to_param(sample_vi_mode vi_mode, sample_vi_param *vi_param)
{
    switch (vi_mode) {
        case SAMPLE_VI_MODE_960H_4X4:
        case SAMPLE_VI_MODE_720P_4X4:
            sample_common_vi_set_param_4x4(vi_mode, vi_param);
            break;

        case SAMPLE_VI_MODE_720P_1X4:
            sample_common_vi_set_param_1x4(vi_mode, vi_param);
            break;

        case SAMPLE_VI_MODE_720P_2X2:
        case SAMPLE_VI_MODE_1080P_2X2:
            sample_common_vi_set_param_2x2(vi_mode, vi_param);
            break;

        case SAMPLE_VI_MODE_4K_1X8:
        case SAMPLE_VI_MODE_1080P_1X8:  
            sample_common_vi_set_param_1x8(vi_mode, vi_param);
            break;

        default:
            SAMPLE_PRT("vi_mode invalid !\n");
            return HI_FAILURE;
    }

    return HI_SUCCESS;
}

hi_s32 sample_common_vi_mode_to_size(sample_vi_mode vi_mode, sample_video_encoding video_encoding,
    hi_rect *capture_rect, hi_size *dst_size)
{
    capture_rect->x = 0;
    capture_rect->y = 0;

    switch (vi_mode) {
        case SAMPLE_VI_MODE_960H_4X4:
            capture_rect->width = _960H_WIDTH;
            capture_rect->height = (video_encoding == SAMPLE_VIDEO_ENCODING_PAL) ? _960H_HEIGHT_PAL : _960H_HEIGHT_NTSC;
            dst_size->width = capture_rect->width;
            dst_size->height = capture_rect->height;
            break;

        case SAMPLE_VI_MODE_720P_1X4:
        case SAMPLE_VI_MODE_720P_2X2:
        case SAMPLE_VI_MODE_720P_4X4:
            capture_rect->width = HD_WIDTH;
            capture_rect->height = HD_HEIGHT;
            dst_size->width = capture_rect->width;
            dst_size->height = capture_rect->height;
            break;

        case SAMPLE_VI_MODE_1080P_2X2:
        case SAMPLE_VI_MODE_1080P_1X8:          
            capture_rect->width = FHD_WIDTH;
            capture_rect->height = FHD_HEIGHT;
            dst_size->width = capture_rect->width;
            dst_size->height = capture_rect->height;
            break;

        case SAMPLE_VI_MODE_4K_1X8:
            capture_rect->width = _4K_WIDTH;
            capture_rect->height = _4K_HEIGHT;
            dst_size->width = capture_rect->width;
            dst_size->height = capture_rect->height;
            break;
          
        default:
            SAMPLE_PRT("vi mode invaild!\n");
            return HI_FAILURE;
    }

    return HI_SUCCESS;
}

#if SAMPLE_AD_TYPE == AD_NVP6158
hi_s32 sample_nvp6158_cfg(sample_vi_mode vi_mode, sample_video_encoding video_encoding)
{
    int fd, i, j;
    int video_mode;
    nvp6158_opt_mode optmode;
    nvp6158_chn_mode schnmode;
    int chip_cnt = 1;

    fd = open(NVP6158_FILE, O_RDWR);
    if (fd < 0) {
        printf("open nvp6158 (%s) fail\n", NVP6158_FILE);
        return -1;
    }

    video_mode = (SAMPLE_VIDEO_ENCODING_PAL == video_encoding) ? 1 : 0;

    switch (vi_mode) {
        case SAMPLE_VI_MODE_960H_4X4:
            schnmode.chmode = (video_mode == 1) ? AHD20_SD_H960_NT : AHD20_SD_H960_PAL;
            optmode.portmode = NVP6158_OUTMODE_4MUX_SD;
            break;

        case SAMPLE_VI_MODE_720P_1X4:
            schnmode.chmode = (video_mode == 1) ? AHD20_720P_25P : AHD20_720P_30P;
            optmode.portmode = NVP6158_OUTMODE_1MUX_HD;
            break;

        case SAMPLE_VI_MODE_1080P_2X2:
            schnmode.chmode = (video_mode == 1) ? AHD20_1080P_25P : AHD20_1080P_30P;
            optmode.portmode = NVP6158_OUTMODE_2MUX_FHD;
            break;

        default:
            close(fd);
            SAMPLE_PRT("vi mode invaild!\n");
            return HI_FAILURE;
    }

    for (i = 0; i < chip_cnt * CHN_NUM_PRE_CHIP; i++) {
        schnmode.ch = i;
        schnmode.vformat = video_mode;
        ioctl(fd, IOC_VDEC_SET_CHNMODE, &schnmode);
    }
    for (i = 0; i < chip_cnt; i++) {
        for (j = 0; j < CHN_NUM_PRE_CHIP; j++) {
            optmode.chipsel = i;
            optmode.portsel = j;
            optmode.chid = j;
            ioctl(fd, IOC_VDEC_SET_OUTPORTMODE, &optmode);
        }
    }

    close(fd);

    return HI_SUCCESS;
}
#elif SAMPLE_AD_TYPE == AD_TP2854B
hi_s32 sample_tp2854b_cfg(sample_vi_mode vi_mode, sample_video_encoding video_encoding)
{
    int fd, i, j;
    tp2802_video_mode tp_video_mode;
    int chip_cnt = 8;

    fd = open(TP2854B_FILE, O_RDWR);
    if (fd < 0) {
        printf("open tp2854b (%s) fail\n", TP2854B_FILE);
        return -1;
    }

    for (i = 0; i < chip_cnt; i++) {
        tp_video_mode.chip = i;

        switch (vi_mode) {
            case SAMPLE_VI_MODE_720P_1X4:
                for (j = 0; j < CHANNELS_PER_CHIP; j++) {
                    tp_video_mode.ch = j;
                    tp_video_mode.std = STD_HDA;
                    tp_video_mode.mode =
                        (video_encoding == SAMPLE_VIDEO_ENCODING_PAL) ? TP2802_720P25V2 : TP2802_720P30V2;
                    tp_video_mode.output = MIPI_1CH4LANE_297M;
                    ioctl(fd, TP2802_SET_VIDEO_MODE, &tp_video_mode);
                }
                break;

            case SAMPLE_VI_MODE_720P_2X2:
                for (j = 0; j < CHANNELS_PER_CHIP; j++) {
                    tp_video_mode.ch = j;
                    tp_video_mode.std = STD_HDA;
                    tp_video_mode.mode =
                        (video_encoding == SAMPLE_VIDEO_ENCODING_PAL) ? TP2802_720P25V2 : TP2802_720P30V2;
                    tp_video_mode.output = MIPI_2CH4LANE_297M;
                    ioctl(fd, TP2802_SET_VIDEO_MODE, &tp_video_mode);
                }
                break;

            case SAMPLE_VI_MODE_720P_4X4:
                for (j = 0; j < CHANNELS_PER_CHIP; j++) {
                    tp_video_mode.ch = j;
                    tp_video_mode.std = STD_HDA;
                    tp_video_mode.mode =
                        (video_encoding == SAMPLE_VIDEO_ENCODING_PAL) ? TP2802_720P25V2 : TP2802_720P30V2;
                    tp_video_mode.output = MIPI_4CH4LANE_297M;
                    ioctl(fd, TP2802_SET_VIDEO_MODE, &tp_video_mode);
                }
                break;

            case SAMPLE_VI_MODE_1080P_2X2:
            case SAMPLE_VI_MODE_1080P_2X4:
                for (j = 0; j < CHANNELS_PER_CHIP; j++) {
                    tp_video_mode.ch = j;
                    tp_video_mode.std = STD_HDA;
                    tp_video_mode.mode =
                        (video_encoding == SAMPLE_VIDEO_ENCODING_PAL) ? TP2802_1080P25 : TP2802_1080P30;
                    tp_video_mode.output = MIPI_2CH4LANE_297M;
                    ioctl(fd, TP2802_SET_VIDEO_MODE, &tp_video_mode);
                }
                break;

            case SAMPLE_VI_MODE_4M_1X8:
                for (j = 0; j < CHANNELS_PER_CHIP; j++) {
                    tp_video_mode.ch = j;
                    tp_video_mode.std = STD_HDA;
                    tp_video_mode.mode = TP2802_QHD15;
                    tp_video_mode.output = MIPI_1CH4LANE_297M;
                    ioctl(fd, TP2802_SET_VIDEO_MODE, &tp_video_mode);
                }
                break;

            case SAMPLE_VI_MODE_5M_1X8:
                for (j = 0; j < CHANNELS_PER_CHIP; j++) {
                    tp_video_mode.ch = j;
                    tp_video_mode.std = STD_HDA;
                    tp_video_mode.mode = TP2802_5M12;
                    tp_video_mode.output = MIPI_1CH4LANE_297M;
                    ioctl(fd, TP2802_SET_VIDEO_MODE, &tp_video_mode);
                }
                break;

            case SAMPLE_VI_MODE_4K_1X8:
                for (j = 0; j < CHANNELS_PER_CHIP; j++) {
                    tp_video_mode.ch = j;
                    tp_video_mode.std = STD_HDA;
                    tp_video_mode.mode = TP2802_8M15;
                    tp_video_mode.output = MIPI_1CH4LANE_297M;
                    ioctl(fd, TP2802_SET_VIDEO_MODE, &tp_video_mode);
                }
                break;

            default:
                printf("vi_mode %d not supported\n", vi_mode);
                break;
        }
    }
    close(fd);

    return HI_SUCCESS;
}
#elif SAMPLE_AD_TYPE == AD_TP2856
hi_s32 sample_tp2856_cfg(sample_vi_mode vi_mode, sample_video_encoding video_encoding)
{
    int fd, i, j;
    tp2802_video_mode tp_video_mode;
    int chip_cnt = 8;

    fd = open(TP2856_FILE, O_RDWR);
    if (fd < 0) {
        printf("open tp2854b (%s) fail\n", TP2856_FILE);
        return -1;
    }

    for (i = 0; i < chip_cnt; i++) {
        tp_video_mode.chip = i;

        switch (vi_mode) {
            case SAMPLE_VI_MODE_720P_1X4:
                for (j = 0; j < CHANNELS_PER_CHIP; j++) {
                    tp_video_mode.ch = j;
                    tp_video_mode.std = STD_HDA;
                    tp_video_mode.mode =
                        (video_encoding == SAMPLE_VIDEO_ENCODING_PAL) ? TP2802_720P25V2 : TP2802_720P30V2;
                    tp_video_mode.output = MIPI_1CH4LANE_297M;
                    ioctl(fd, TP2802_SET_VIDEO_MODE, &tp_video_mode);
                }
                break;

            case SAMPLE_VI_MODE_720P_2X2:
                for (j = 0; j < CHANNELS_PER_CHIP; j++) {
                    tp_video_mode.ch = j;
                    tp_video_mode.std = STD_HDA;
                    tp_video_mode.mode =
                        (video_encoding == SAMPLE_VIDEO_ENCODING_PAL) ? TP2802_720P25V2 : TP2802_720P30V2;
                    tp_video_mode.output = MIPI_2CH4LANE_297M;
                    ioctl(fd, TP2802_SET_VIDEO_MODE, &tp_video_mode);
                }
                break;

            case SAMPLE_VI_MODE_720P_4X4:
                for (j = 0; j < CHANNELS_PER_CHIP; j++) {
                    tp_video_mode.ch = j;
                    tp_video_mode.std = STD_HDA;
                    tp_video_mode.mode =
                        (video_encoding == SAMPLE_VIDEO_ENCODING_PAL) ? TP2802_720P25V2 : TP2802_720P30V2;
                    tp_video_mode.output = MIPI_4CH4LANE_297M;
                    ioctl(fd, TP2802_SET_VIDEO_MODE, &tp_video_mode);
                }
                break;

            case SAMPLE_VI_MODE_1080P_2X2:
            case SAMPLE_VI_MODE_1080P_2X4:
                for (j = 0; j < CHANNELS_PER_CHIP; j++) {
                    tp_video_mode.ch = j;
                    tp_video_mode.std = STD_HDA;
                    tp_video_mode.mode =
                        (video_encoding == SAMPLE_VIDEO_ENCODING_PAL) ? TP2802_1080P25 : TP2802_1080P30;
                    tp_video_mode.output = MIPI_2CH4LANE_297M;
                    ioctl(fd, TP2802_SET_VIDEO_MODE, &tp_video_mode);
                }
                break;

            case SAMPLE_VI_MODE_4M_1X8:
                for (j = 0; j < CHANNELS_PER_CHIP; j++) {
                    tp_video_mode.ch = j;
                    tp_video_mode.std = STD_HDA;
                    tp_video_mode.mode = TP2802_QHD15;
                    tp_video_mode.output = MIPI_1CH4LANE_297M;
                    ioctl(fd, TP2802_SET_VIDEO_MODE, &tp_video_mode);
                }
                break;

            case SAMPLE_VI_MODE_5M_1X8:
                for (j = 0; j < CHANNELS_PER_CHIP; j++) {
                    tp_video_mode.ch = j;
                    tp_video_mode.std = STD_HDA;
                    tp_video_mode.mode = TP2802_5M12;
                    tp_video_mode.output = MIPI_1CH4LANE_297M;
                    ioctl(fd, TP2802_SET_VIDEO_MODE, &tp_video_mode);
                }
                break;

            case SAMPLE_VI_MODE_4K_1X8:
                for (j = 0; j < CHANNELS_PER_CHIP; j++) {
                    tp_video_mode.ch = j;
                    tp_video_mode.std = STD_HDA;
                    tp_video_mode.mode = TP2802_8M15;
                    tp_video_mode.output = MIPI_1CH4LANE_297M;
                    ioctl(fd, TP2802_SET_VIDEO_MODE, &tp_video_mode);
                }
                break;

            default:
                printf("vi_mode %d not supported\n", vi_mode);
                break;
        }
    }
    close(fd);

    return HI_SUCCESS;
}
#endif

hi_s32 sample_common_vi_start_ad(sample_vi_mode vi_mode, sample_video_encoding video_encoding)
{
    hi_s32 ret;

#if SAMPLE_AD_TYPE == AD_NVP6158
    ret = sample_nvp6158_cfg(vi_mode, video_encoding);
    if (ret != HI_SUCCESS) {
        SAMPLE_PRT("nvp6158 config failed!\n");
        return ret;
    }
#elif SAMPLE_AD_TYPE == AD_TP2854B
    ret = sample_tp2854b_cfg(vi_mode, video_encoding);
    if (ret != HI_SUCCESS) {
        SAMPLE_PRT("tp2854b config failed!\n");
        return ret;
    }
#elif SAMPLE_AD_TYPE == AD_TP2856
    ret = sample_tp2856_cfg(vi_mode, video_encoding);
    if (ret != HI_SUCCESS) {
        SAMPLE_PRT("tp2854b config failed!\n");
        return ret;
    }
#elif SAMPLE_AD_TYPE == AD_LT8619C
    printf("%s => AD_LT8619C\n", __func__);
#endif

    return HI_SUCCESS;
}

#if SAMPLE_AD_TYPE == AD_TP2854B || SAMPLE_AD_TYPE == AD_TP2856
hi_s32 sample_common_vi_set_mipi_attr(hi_s32 fd, combo_dev_t mipi_dev, hi_rect *capture_rect, sample_vi_param *vi_param)
{
    hi_s32 i;
    hi_s32 ret;
    combo_dev_attr_t mipi_dev_attr;

    mipi_dev_attr.devno = mipi_dev;
    mipi_dev_attr.img_rect.x = 0;
    mipi_dev_attr.img_rect.y = 0;
    mipi_dev_attr.img_rect.width = capture_rect->width;
    mipi_dev_attr.img_rect.height = capture_rect->height;
    mipi_dev_attr.input_mode = INPUT_MODE_MIPI;
    mipi_dev_attr.mipi_attr.input_data_type = DATA_TYPE_YUV422_8BIT;
    mipi_dev_attr.scan_mode = (mipi_scan_mode_t)vi_param->scan_mode;
    for (i = 0; i < MIPI_LANE_NUM_PRE_DEV; i++) {
        mipi_dev_attr.mipi_attr.lane_id[i] = mipi_dev * MIPI_LANE_NUM_PRE_DEV + i;
    }

    ret = ioctl(fd, HI_MIPI_SET_DEV_ATTR, &mipi_dev_attr);
    if (ret != HI_SUCCESS) {
        SAMPLE_PRT("HI_MIPI_SET_DEV_ATTR failed\n");
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

hi_s32 sample_common_vi_start_mipi(hi_rect *capture_rect, sample_vi_param *vi_param)
{
    hi_s32 i;
    hi_s32 ret;
    hi_s32 fd;
    combo_dev_t mipi_dev;

    lane_divide_mode_t hs_mode = LANE_DIVIDE_MODE_0;

    fd = open(MIPI_DEV_NODE, O_RDWR);
    if (fd < 0) {
        SAMPLE_PRT("open hi_mipi dev failed\n");
        return HI_FAILURE;
    }

    ret = ioctl(fd, HI_MIPI_SET_HS_MODE, &hs_mode);
    if (ret != HI_SUCCESS) {
        SAMPLE_PRT("HI_MIPI_SET_HS_MODE failed\n");
        close(fd);
        return ret;
    }

    for (i = 0; i < vi_param->vi_dev_cnt; i++) {
        mipi_dev = i * vi_param->vi_dev_interval;
        ret = ioctl(fd, HI_MIPI_ENABLE_MIPI_CLOCK, &mipi_dev);
        if (ret != HI_SUCCESS) {
            SAMPLE_PRT("HI_MIPI_ENABLE_MIPI_CLOCK failed\n");
            close(fd);
            return ret;
        }

        ret = ioctl(fd, HI_MIPI_RESET_MIPI, &mipi_dev);
        if (ret != HI_SUCCESS) {
            SAMPLE_PRT("HI_MIPI_RESET_MIPI failed\n");
            close(fd);
            return ret;
        }

        ret = sample_common_vi_set_mipi_attr(fd, mipi_dev, capture_rect, vi_param);
        if (ret != HI_SUCCESS) {
            SAMPLE_PRT("vi set mipi attr failed\n");
            close(fd);
            return ret;
        }

        ret = ioctl(fd, HI_MIPI_UNRESET_MIPI, &mipi_dev);
        if (ret != HI_SUCCESS) {
            SAMPLE_PRT("HI_MIPI_UNRESET_MIPI failed\n");
            close(fd);
            return ret;
        }
    }

    close(fd);

    return HI_SUCCESS;
}

hi_s32 sample_common_vi_stop_mipi(sample_vi_param *vi_param)
{
    hi_s32 i;
    hi_s32 ret;
    hi_s32 fd;
    combo_dev_t mipi_dev;

    fd = open(MIPI_DEV_NODE, O_RDWR);
    if (fd < 0) {
        SAMPLE_PRT("open hi_mipi dev failed\n");
        return HI_FAILURE;
    }

    for (i = 0; i < vi_param->vi_dev_cnt; i++) {
        mipi_dev = i * vi_param->vi_dev_interval;
        ret = ioctl(fd, HI_MIPI_RESET_MIPI, &mipi_dev);
        if (ret != HI_SUCCESS) {
            SAMPLE_PRT("HI_MIPI_RESET_MIPI failed\n");
            close(fd);
            return HI_FAILURE;
        }

        ret = ioctl(fd, HI_MIPI_DISABLE_MIPI_CLOCK, &mipi_dev);
        if (ret != HI_SUCCESS) {
            SAMPLE_PRT("HI_MIPI_ENABLE_MIPI_CLOCK failed\n");
            close(fd);
            return HI_FAILURE;
        }
    }

    close(fd);

    return HI_SUCCESS;
}
#endif

hi_void sample_common_vi_set_mask(hi_vi_dev vi_dev, hi_vi_dev_attr *dev_attr)
{
    if (dev_attr->intf_mode == HI_VI_MODE_MIPI_YUV422) {
        dev_attr->component_mask[0] = 0x00ff0;
        dev_attr->component_mask[1] = 0xff000;
        dev_attr->data_seq = HI_VI_IN_DATA_UVUV;
    } else if (dev_attr->intf_mode == HI_VI_MODE_BT656) {
        if (vi_dev % 2 == 0) { /* 2 : odd-even judge */
            dev_attr->component_mask[0] = 0x00ff0;
        } else {
            dev_attr->component_mask[0] = 0xff000;
        }
        dev_attr->data_seq = HI_VI_IN_DATA_UYVY;
    } else if (dev_attr->intf_mode == HI_VI_MODE_BT1120_STANDARD) {
        dev_attr->component_mask[0] = 0x00ff0;
        dev_attr->component_mask[1] = 0xff000;
        dev_attr->data_seq = HI_VI_IN_DATA_UVUV;
    }
}

hi_s32 sample_common_vi_start_dev(hi_vi_dev vi_dev, sample_vi_mode vi_mode)
{
    hi_s32 ret;
    hi_vi_dev_attr vi_dev_attr;
    hi_vi_work_mode vi_work_mode;
    printf("%s => vi_dev:%d, vi_mode:%d\n", __func__, vi_dev, vi_mode);
    (hi_void)memcpy_s(&vi_dev_attr, sizeof(hi_vi_dev_attr), &g_sample_vi_dev_attr, sizeof(hi_vi_dev_attr));

    switch (vi_mode) {
        case SAMPLE_VI_MODE_960H_4X4:
            vi_work_mode = HI_VI_WORK_MODE_MULTIPLEX_4;
            break;

        case SAMPLE_VI_MODE_720P_4X4:
            vi_work_mode = HI_VI_WORK_MODE_MULTIPLEX_4;
            break;

        case SAMPLE_VI_MODE_720P_2X2:
        case SAMPLE_VI_MODE_1080P_2X2:
            vi_work_mode = HI_VI_WORK_MODE_MULTIPLEX_2;
            break;

        case SAMPLE_VI_MODE_720P_1X4:
        case SAMPLE_VI_MODE_4K_1X8:
        case SAMPLE_VI_MODE_1080P_1X8:  
            vi_work_mode = HI_VI_WORK_MODE_MULTIPLEX_1;
            break;

        default:
            SAMPLE_PRT("vi mode invaild!\n");
            return HI_FAILURE;
    }
    vi_dev_attr.work_mode = vi_work_mode;
    
#if SAMPLE_AD_TYPE == AD_NVP6158
    vi_dev_attr.intf_mode = HI_VI_MODE_BT656;
    if (vi_mode == SAMPLE_VI_MODE_720P_4X4 || vi_mode == SAMPLE_VI_MODE_1080P_2X2 || vi_mode == SAMPLE_VI_MODE_4K_1X8) {
        vi_dev_attr.clk_edge = HI_VI_CLK_EDGE_DOUBLE;
    }
#elif SAMPLE_AD_TYPE == AD_TP2854B || SAMPLE_AD_TYPE == AD_TP2856
    vi_dev_attr.intf_mode = HI_VI_MODE_MIPI_YUV422;
#elif SAMPLE_AD_TYPE == AD_LT8619C
    vi_dev_attr.intf_mode = HI_VI_MODE_BT656;
    vi_dev_attr.clk_edge = HI_VI_CLK_EDGE_DOUBLE;
    vi_dev_attr.data_reverse_en = HI_TRUE; //data0-data7 => data7-data0;
    vi_dev_attr.mixed_capture_en = HI_FALSE;
    
    //ret = hi_mpi_vi_disable_dll_slave(vi_dev);
    //printf("hi_mpi_vi_disable_dll_slave(%d) ret:%d\n", vi_dev, ret);
#endif

    sample_common_vi_set_mask(vi_dev, &vi_dev_attr);

    ret = hi_mpi_vi_set_dev_attr(vi_dev, &vi_dev_attr);
    if (ret != HI_SUCCESS) {
        SAMPLE_PRT("hi_mpi_vi_set_dev_attr failed 0x%x\n", ret);
        return ret;
    }

    ret = hi_mpi_vi_enable_dev(vi_dev);
    if (ret != HI_SUCCESS) {
        SAMPLE_PRT("hi_mpi_vi_enable_dev failed 0x%x\n", ret);
        return ret;
    }

    return HI_SUCCESS;
}

hi_s32 sample_common_vi_start_chn(hi_s32 vi_chn, hi_rect *capture_rect, hi_size *dst_size, sample_vi_param *vi_param)
{
    hi_s32 ret;
    hi_vi_chn_attr vi_chn_attr;
    hi_vi_chn_attr minor_chn_attr;

    (hi_void)memcpy_s(&vi_chn_attr, sizeof(hi_vi_chn_attr), &g_sample_vi_chn_attr, sizeof(hi_vi_chn_attr));
    (hi_void)memcpy_s(&vi_chn_attr.capture_rect, sizeof(hi_rect), capture_rect, sizeof(hi_rect));
    (hi_void)memcpy_s(&vi_chn_attr.dst_size, sizeof(hi_size), dst_size, sizeof(hi_size));

    vi_chn_attr.pixel_format = SAMPLE_PIXEL_FORMAT;
    vi_chn_attr.scan_mode = vi_param->scan_mode;
    vi_chn_attr.src_frame_rate = -1;
    vi_chn_attr.dst_frame_rate = -1;

    ret = hi_mpi_vi_set_chn_attr(vi_chn, &vi_chn_attr);
    if (ret != HI_SUCCESS) {
        SAMPLE_PRT("hi_mpi_vi_set_chn_attr failed 0x%x\n", ret);
        return ret;
    }

    ret = hi_mpi_vi_enable_chn(vi_chn);
    if (ret != HI_SUCCESS) {
        SAMPLE_PRT("hi_mpi_vi_enable_chn failed 0x%x\n", ret);
        return ret;
    }

    if (vi_param->minor_attr_en == HI_TRUE) {
        (hi_void)memcpy_s(&minor_chn_attr, sizeof(hi_vi_chn_attr), &vi_chn_attr, sizeof(hi_vi_chn_attr));
        (hi_void)memcpy_s(&minor_chn_attr.dst_size, sizeof(hi_size), &vi_param->minor_dst_size, sizeof(hi_size));
        minor_chn_attr.src_frame_rate = -1;
        minor_chn_attr.dst_frame_rate = -1;
        ret = hi_mpi_vi_set_minor_chn_attr(MINOR_CHN(vi_chn), &minor_chn_attr);
        if (ret != HI_SUCCESS) {
            SAMPLE_PRT("hi_mpi_vi_set_minor_chn_attr failed 0x%x\n", ret);
            return ret;
        }

        ret = hi_mpi_vi_enable_chn(MINOR_CHN(vi_chn));
        if (ret != HI_SUCCESS) {
            SAMPLE_PRT("hi_mpi_vi_enable_chn failed 0x%x\n", ret);
            return ret;
        }
    }

    return HI_SUCCESS;
}

hi_s32 sample_common_vi_set_chn_bind(sample_vi_mode vi_mode, sample_vi_param *vi_param)
{
#if SAMPLE_AD_TYPE == AD_TP2854B || SAMPLE_AD_TYPE == AD_TP2856
    hi_s32 i;
    hi_s32 ret;
    hi_vi_chn vi_chn;
    hi_vi_dev vi_dev;
    hi_vi_chn_bind_attr chn_bind_attr;

    if (vi_mode != SAMPLE_VI_MODE_720P_2X2 && vi_mode != SAMPLE_VI_MODE_1080P_2X2) {
        return HI_SUCCESS;
    }

    for (i = 0; i < vi_param->vi_chn_cnt; i++) {
        vi_chn = i * vi_param->vi_chn_interval;
        vi_dev = vi_chn / CHN_NUM_PRE_DEV;
        /* chn 2,6,10,14,18,22,26,30 */
        if (vi_chn == CHN_NUM_PRE_DEV * vi_dev + SECOND_CHN_OFFSET_2MUX) {
            ret = hi_mpi_vi_unbind_chn(vi_chn);
            if (ret != HI_SUCCESS) {
                return ret;
            }
            chn_bind_attr.vi_dev = vi_dev;
            chn_bind_attr.vi_way = vi_chn;
            ret = hi_mpi_vi_bind_chn(vi_chn, &chn_bind_attr);
            if (ret != HI_SUCCESS) {
                return ret;
            }
        }
    }
#endif
    return HI_SUCCESS;
}

hi_s32 sample_common_vi_start_vi(sample_vi_mode vi_mode, sample_video_encoding video_encoding,
    sample_vi_param *vi_param)
{
    hi_vi_dev vi_dev;
    hi_vi_chn vi_chn;
    hi_s32 i;
    hi_s32 ret;
    hi_size dst_size;
    hi_rect capture_rect;

    ret = sample_common_vi_mode_to_size(vi_mode, video_encoding, &capture_rect, &dst_size);
    if (ret != HI_SUCCESS) {
        SAMPLE_PRT("vi get size failed!\n");
        return ret;
    }

    /* start mipi */
#if SAMPLE_AD_TYPE == AD_TP2854B || SAMPLE_AD_TYPE == AD_TP2856
    ret = sample_common_vi_start_mipi(&capture_rect, vi_param);
    if (ret != HI_SUCCESS) {
        SAMPLE_PRT("vi start mipi failed!\n");
        return ret;
    }
#endif

    /* start ad */
    ret = sample_common_vi_start_ad(vi_mode, video_encoding);
    if (ret != HI_SUCCESS) {
        SAMPLE_PRT("vi start ad failed!\n");
        return ret;
    }

    /* start vi dev */
    for (i = 0; i < vi_param->vi_dev_cnt; i++) {
        vi_dev = i * vi_param->vi_dev_interval;
        ret = sample_common_vi_start_dev(vi_dev, vi_mode);
        if (ret != HI_SUCCESS) {
            SAMPLE_PRT("sample_common_vi_start_dev failed with %#x\n", ret);
            return HI_FAILURE;
        }
    }

    ret = sample_common_vi_set_chn_bind(vi_mode, vi_param);
    if (ret != HI_SUCCESS) {
        SAMPLE_PRT("sample_common_vi_set_chn_bind failed with %#x\n", ret);
        return HI_FAILURE;
    }

    /* start vi chn */
    for (i = 0; i < vi_param->vi_chn_cnt; i++) {
        vi_chn = i * vi_param->vi_chn_interval;
        ret = sample_common_vi_start_chn(vi_chn, &capture_rect, &dst_size, vi_param);
        if (ret != HI_SUCCESS) {
            SAMPLE_PRT("sample_common_vi_start_chn failed with %#x\n", ret);
            return HI_FAILURE;
        }
    }

    return HI_SUCCESS;
}

hi_s32 sample_common_vi_stop(sample_vi_mode vi_mode, sample_vi_param *vi_param)
{
    hi_s32 i;
    hi_s32 ret;
    hi_vi_dev vi_dev;
    hi_vi_chn vi_chn;

    /* stop vi chn */
    for (i = 0; i < vi_param->vi_chn_cnt; i++) {
        vi_chn = i * vi_param->vi_chn_interval;
        ret = hi_mpi_vi_disable_chn(MINOR_CHN(vi_chn));
        if (ret != HI_SUCCESS) {
            SAMPLE_PRT("hi_mpi_vi_disable_chn failed!\n");
            return ret;
        }

        ret = hi_mpi_vi_disable_chn(vi_chn);
        if (ret != HI_SUCCESS) {
            SAMPLE_PRT("hi_mpi_vi_disable_chn failed!\n");
            return ret;
        }
    }

    /* stop vi dev */
    for (i = 0; i < vi_param->vi_dev_cnt; i++) {
        vi_dev = i * vi_param->vi_dev_interval;
        ret = hi_mpi_vi_disable_dev(vi_dev);
        if (ret != HI_SUCCESS) {
            SAMPLE_PRT("hi_mpi_vi_disable_dev failed!\n");
            return ret;
        }
    }

    /* stop mipi */
#if SAMPLE_AD_TYPE == AD_TP2854B || SAMPLE_AD_TYPE == AD_TP2856
    ret = sample_common_vi_stop_mipi(vi_param);
    if (ret != HI_SUCCESS) {
        SAMPLE_PRT("stop mipi failed!\n");
        return ret;
    }
#endif

    return HI_SUCCESS;
}
