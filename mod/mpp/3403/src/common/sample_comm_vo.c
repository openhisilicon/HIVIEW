/*
  Copyright (c), 2001-2022, Shenshu Tech. Co., Ltd.
 */

#include <sys/ioctl.h>
#include <fcntl.h>
#include <unistd.h>
#include "sample_comm.h"
#if VO_LT8618SX
#include "lt8618sx.h"
#endif

#if VO_MIPI_SUPPORT
#include "hi_mipi_tx.h"
#endif

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* end of #ifdef __cplusplus */

static hi_bool g_vo_hdmi_rgb_mode = HI_FALSE;

static sample_vo_sync_info g_vo_sample_sync_info[HI_VO_OUT_BUTT] = {
    { HI_VO_OUT_PAL,        720, 576, 25 },
    { HI_VO_OUT_NTSC,       720, 480, 30 },
    { HI_VO_OUT_960H_PAL,   960, 576, 25 },
    { HI_VO_OUT_960H_NTSC,  960, 480, 30 },

    { HI_VO_OUT_640x480_60,     640,  480,  60 },
    { HI_VO_OUT_480P60,         720,  480,  60 },
    { HI_VO_OUT_576P50,         720,  576,  50 },
    { HI_VO_OUT_800x600_60,     800,  600,  60 },
    { HI_VO_OUT_1024x768_60,    1024, 768,  60 },
    { HI_VO_OUT_720P50,         1280, 720,  50 },
    { HI_VO_OUT_720P60,         1280, 720,  60 },
    { HI_VO_OUT_1280x800_60,    1280, 800,  60 },
    { HI_VO_OUT_1280x1024_60,   1280, 1024, 60 },
    { HI_VO_OUT_1366x768_60,    1366, 768,  60 },
    { HI_VO_OUT_1400x1050_60,   1400, 1050, 60 },
    { HI_VO_OUT_1440x900_60,    1440, 900,  60 },
    { HI_VO_OUT_1680x1050_60,   1680, 1050, 60 },

    { HI_VO_OUT_1080P24,    1920, 1080, 24 },
    { HI_VO_OUT_1080P25,    1920, 1080, 25 },
    { HI_VO_OUT_1080P30,    1920, 1080, 30 },
    { HI_VO_OUT_1080I50,    1920, 1080, 25 },
    { HI_VO_OUT_1080I60,    1920, 1080, 30 },
    { HI_VO_OUT_1080P50,    1920, 1080, 50 },
    { HI_VO_OUT_1080P60,    1920, 1080, 60 },

    { HI_VO_OUT_1600x1200_60,   1600, 1200, 60 },
    { HI_VO_OUT_1920x1200_60,   1920, 1200, 60 },
    { HI_VO_OUT_1920x2160_30,   1920, 2160, 30 },
    { HI_VO_OUT_2560x1440_30,   2560, 1440, 30 },
    { HI_VO_OUT_2560x1440_60,   2560, 1440, 60 },
    { HI_VO_OUT_2560x1600_60,   2560, 1600, 60 },

    { HI_VO_OUT_3840x2160_24, 3840, 2160, 24 },
    { HI_VO_OUT_3840x2160_25, 3840, 2160, 25 },
    { HI_VO_OUT_3840x2160_30, 3840, 2160, 30 },
    { HI_VO_OUT_3840x2160_50, 3840, 2160, 50 },
    { HI_VO_OUT_3840x2160_60, 3840, 2160, 60 },
    { HI_VO_OUT_4096x2160_24, 4096, 2160, 24 },
    { HI_VO_OUT_4096x2160_25, 4096, 2160, 25 },
    { HI_VO_OUT_4096x2160_30, 4096, 2160, 30 },
    { HI_VO_OUT_4096x2160_50, 4096, 2160, 50 },
    { HI_VO_OUT_4096x2160_60, 4096, 2160, 60 },
    { HI_VO_OUT_7680x4320_30, 7680, 4320, 30 },

    { HI_VO_OUT_240x320_50, 240, 320, 50 },
    { HI_VO_OUT_320x240_50, 320, 240, 50 },
    { HI_VO_OUT_240x320_60, 240, 320, 60 },
    { HI_VO_OUT_320x240_60, 320, 240, 60 },
    { HI_VO_OUT_800x600_50, 800, 600, 50 },

    { HI_VO_OUT_720x1280_60, 720, 1280, 60 },
    { HI_VO_OUT_1080x1920_60, 1080, 1920, 60 },
    { HI_VO_OUT_USER,  0,    0,    0 }
};

hi_s32 sample_comm_vo_get_width_height(hi_vo_intf_sync intf_sync, hi_u32 *width, hi_u32 *height, hi_u32 *frame_rate)
{
    check_null_ptr_return(width);
    check_null_ptr_return(height);
    check_null_ptr_return(frame_rate);

    if (intf_sync > HI_VO_OUT_USER) {
        sample_print("vo intf_sync %d not support!\n", intf_sync);
        return HI_FAILURE;
    }

    *width = g_vo_sample_sync_info[intf_sync].width;
    *height = g_vo_sample_sync_info[intf_sync].height;
    *frame_rate = g_vo_sample_sync_info[intf_sync].frame_rate;
    return HI_SUCCESS;
}

hi_s32 sample_comm_vo_start_dev(hi_vo_dev vo_dev, const hi_vo_pub_attr *pub_attr,
    const hi_vo_user_sync_info *sync_info, hi_u32 dev_frame_rate)
{
    hi_s32 ret;
    check_null_ptr_return(pub_attr);
    if (pub_attr->intf_sync == HI_VO_OUT_USER) {
        check_null_ptr_return(sync_info);
    }

    ret = hi_mpi_vo_set_pub_attr(vo_dev, pub_attr);
    if (ret != HI_SUCCESS) {
        sample_print("failed with %#x!\n", ret);
        return HI_FAILURE;
    }

    if (pub_attr->intf_sync == HI_VO_OUT_USER) {
        ret = hi_mpi_vo_set_user_sync_info(vo_dev, sync_info);
        if (ret != HI_SUCCESS) {
            sample_print("failed with %#x!\n", ret);
            return HI_FAILURE;
        }

        ret = hi_mpi_vo_set_dev_frame_rate(vo_dev, dev_frame_rate);
        if (ret != HI_SUCCESS) {
            sample_print("failed with %#x!\n", ret);
            return HI_FAILURE;
        }
    }

    ret = hi_mpi_vo_enable(vo_dev);
    if (ret != HI_SUCCESS) {
        sample_print("failed with %#x!\n", ret);
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

hi_s32 sample_comm_vo_stop_dev(hi_vo_dev vo_dev)
{
    hi_s32 ret;

    ret = hi_mpi_vo_disable(vo_dev);
    if (ret != HI_SUCCESS) {
        sample_print("failed with %#x!\n", ret);
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

hi_s32 sample_comm_vo_start_layer(hi_vo_layer vo_layer, const hi_vo_video_layer_attr *layer_attr)
{
    hi_s32 ret;

    check_null_ptr_return(layer_attr);

    ret = hi_mpi_vo_set_video_layer_attr(vo_layer, layer_attr);
    if (ret != HI_SUCCESS) {
        sample_print("failed with %#x!\n", ret);
        return HI_FAILURE;
    }

    ret = hi_mpi_vo_enable_video_layer(vo_layer);
    if (ret != HI_SUCCESS) {
        sample_print("failed with %#x!\n", ret);
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

hi_s32 sample_comm_vo_stop_layer(hi_vo_layer vo_layer)
{
    hi_s32 ret;

    ret = hi_mpi_vo_disable_video_layer(vo_layer);
    if (ret != HI_SUCCESS) {
        sample_print("failed with %#x!\n", ret);
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}
//maohw;
static sample_vo_wnd_info g_vo_sample_wnd_info[VO_MODE_BUTT] = {
    [VO_MODE_1MUX ] = { VO_MODE_1MUX,   1,  1, 0, 0 },
    [VO_MODE_2MUX ] = { VO_MODE_2MUX,   2,  2, 0, 0 },
    [VO_MODE_4MUX ] = { VO_MODE_4MUX,   4,  2, 0, 0 },
    [VO_MODE_8MUX ] = { VO_MODE_8MUX,   8,  3, 0, 0 },
    [VO_MODE_9MUX ] = { VO_MODE_9MUX,   9,  3, 0, 0 },
    [VO_MODE_16MUX] = { VO_MODE_16MUX,  16, 4, 0, 0 },
    [VO_MODE_25MUX] = { VO_MODE_25MUX,  25, 5, 0, 0 },
    [VO_MODE_36MUX] = { VO_MODE_36MUX,  36, 6, 0, 0 },
    [VO_MODE_49MUX] = { VO_MODE_49MUX,  49, 7, 0, 0 },
    [VO_MODE_64MUX] = { VO_MODE_64MUX,  64, 8, 0, 0 },
    [VO_MODE_2X4  ] = { VO_MODE_2X4,    8,  4, 4, 2 }
};

hi_s32 sample_comm_vo_get_wnd_info(sample_vo_mode mode, sample_vo_wnd_info *wnd_info)
{
    check_null_ptr_return(wnd_info);

    if (mode >= VO_MODE_BUTT) {
        sample_print("vo mode %d not support!\n", mode);
        return HI_FAILURE;
    }

    wnd_info->mode = mode;
    wnd_info->wnd_num = g_vo_sample_wnd_info[mode].wnd_num;
    wnd_info->square = g_vo_sample_wnd_info[mode].square;
    wnd_info->row = g_vo_sample_wnd_info[mode].row;
    wnd_info->col = g_vo_sample_wnd_info[mode].col;
    return HI_SUCCESS;
}

hi_s32 sample_comm_vo_get_chn_attr(sample_vo_wnd_info *wnd_info, hi_vo_video_layer_attr *layer_attr, hi_s32 chn,
    hi_vo_chn_attr *chn_attr)
{
    hi_u32 width;
    hi_u32 height;

    check_null_ptr_return(wnd_info);
    check_null_ptr_return(layer_attr);
    check_null_ptr_return(chn_attr);

    width = layer_attr->img_size.width;
    height = layer_attr->img_size.height;
    if (wnd_info->mode == VO_MODE_2MUX) {
      chn_attr->rect.x = HI_ALIGN_DOWN((width / wnd_info->square) * (chn % wnd_info->square), 2);  /* 2: 2 align */
      chn_attr->rect.y = HI_ALIGN_DOWN((height / 4), 2);                                           /* 2: 2 align */
      chn_attr->rect.width = HI_ALIGN_DOWN(width / wnd_info->square, 2);                           /* 2: 2 align */
      chn_attr->rect.height = HI_ALIGN_DOWN(height / wnd_info->square, 2);                         /* 2: 2 align */
    }else if (wnd_info->mode != VO_MODE_2X4) {
        chn_attr->rect.x = HI_ALIGN_DOWN((width / wnd_info->square) * (chn % wnd_info->square), 2);  /* 2: 2 align */
        chn_attr->rect.y = HI_ALIGN_DOWN((height / wnd_info->square) * (chn / wnd_info->square), 2); /* 2: 2 align */
        chn_attr->rect.width = HI_ALIGN_DOWN(width / wnd_info->square, 2);                           /* 2: 2 align */
        chn_attr->rect.height = HI_ALIGN_DOWN(height / wnd_info->square, 2);                         /* 2: 2 align */
    }
    else {
        chn_attr->rect.x = HI_ALIGN_DOWN((width / wnd_info->col) * (chn % wnd_info->col), 2);  /* 2: 2 align */
        chn_attr->rect.y = HI_ALIGN_DOWN((height / wnd_info->row) * (chn / wnd_info->col), 2); /* 2: 2 align */
        chn_attr->rect.width = HI_ALIGN_DOWN(width / wnd_info->col, 2);                        /* 2: 2 align */
        chn_attr->rect.height = HI_ALIGN_DOWN(height / wnd_info->row, 2);                      /* 2: 2 align */
    }

    chn_attr->priority = 0;
    chn_attr->deflicker_en = HI_FALSE;
    
    printf("%s => chn:%d, mode:%d, [%d,%d,%d,%d]\n", __func__, chn, wnd_info->mode, chn_attr->rect.x, chn_attr->rect.y, chn_attr->rect.width, chn_attr->rect.height);
    return HI_SUCCESS;
}

hi_s32 sample_comm_vo_start_chn(hi_vo_layer vo_layer, sample_vo_mode mode)
{
    hi_s32 ret;
    hi_s32 i;
    sample_vo_wnd_info wnd_info = { 0 };
    hi_vo_chn_attr chn_attr;
    hi_vo_video_layer_attr layer_attr;

    ret = sample_comm_vo_get_wnd_info(mode, &wnd_info);
    if (ret != HI_SUCCESS) {
        sample_print("failed with %#x!\n", ret);
        return ret;
    }

    ret = hi_mpi_vo_get_video_layer_attr(vo_layer, &layer_attr);
    if (ret != HI_SUCCESS) {
        sample_print("failed with %#x!\n", ret);
        return HI_FAILURE;
    }

    for (i = 0; i < (hi_s32)wnd_info.wnd_num; i++) {
        ret = sample_comm_vo_get_chn_attr(&wnd_info, &layer_attr, i, &chn_attr);
        if (ret != HI_SUCCESS) {
            sample_print("failed with %#x!\n", ret);
            return HI_FAILURE;
        }

        ret = hi_mpi_vo_set_chn_attr(vo_layer, i, &chn_attr);
        if (ret != HI_SUCCESS) {
            sample_print("failed with %#x!\n", ret);
            return HI_FAILURE;
        }

        ret = hi_mpi_vo_enable_chn(vo_layer, i);
        if (ret != HI_SUCCESS) {
            sample_print("failed with %#x!\n", ret);
            return HI_FAILURE;
        }
    }

    return HI_SUCCESS;
}

hi_s32 sample_comm_vo_stop_chn(hi_vo_layer vo_layer, sample_vo_mode mode)
{
    hi_s32 ret;
    hi_s32 i;
    sample_vo_wnd_info wnd_info = { 0 };

    ret = sample_comm_vo_get_wnd_info(mode, &wnd_info);
    if (ret != HI_SUCCESS) {
        sample_print("failed with %#x!\n", ret);
        return ret;
    }

    for (i = 0; i < (hi_s32)wnd_info.wnd_num; i++) {
        ret = hi_mpi_vo_disable_chn(vo_layer, i);
        if (ret != HI_SUCCESS) {
            sample_print("failed with %#x!\n", ret);
            return HI_FAILURE;
        }
    }

    return ret;
}

static hi_void sample_private_vo_get_def_wbc_attr(hi_vo_wbc_attr *wbc_attr, hi_vo_wbc_src *source)
{
    wbc_attr->target_size.width = FHD_WIDTH;
    wbc_attr->target_size.height = FHD_HEIGHT;
    wbc_attr->pixel_format = HI_PIXEL_FORMAT_YVU_SEMIPLANAR_420;
    wbc_attr->frame_rate = 30; /* 30: 30fps */
    wbc_attr->compress_mode = HI_COMPRESS_MODE_NONE;
    wbc_attr->dynamic_range = HI_DYNAMIC_RANGE_SDR8;
    source->src_type = HI_VO_WBC_SRC_DEV;
    source->src_id = SAMPLE_VO_DEV_DHD0;
}

static hi_s32 sample_private_vo_start_wbc(hi_vo_wbc vo_wbc, hi_vo_wbc_attr *wbc_attr, hi_vo_wbc_src *source)
{
    hi_s32 ret;

    ret = hi_mpi_vo_set_wbc_src(vo_wbc, source);
    if (ret != HI_SUCCESS) {
        sample_print("failed with %#x!\n", ret);
        return HI_FAILURE;
    }

    ret = hi_mpi_vo_set_wbc_attr(vo_wbc, wbc_attr);
    if (ret != HI_SUCCESS) {
        sample_print("failed with %#x!\n", ret);
        return HI_FAILURE;
    }

    ret = hi_mpi_vo_enable_wbc(vo_wbc);
    if (ret != HI_SUCCESS) {
        sample_print("failed with %#x!\n", ret);
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

static hi_s32 sample_private_vo_stop_wbc(hi_vo_wbc vo_wbc)
{
    hi_s32 ret;

    ret = hi_mpi_vo_disable_wbc(vo_wbc);
    if (ret != HI_SUCCESS) {
        sample_print("failed with %#x!\n", ret);
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

hi_s32 sample_comm_vo_start_wbc(sample_vo_wbc_cfg *wbc_config)
{
    hi_s32 ret;

    check_null_ptr_return(wbc_config);

    /* set dynamic range if changed */
    if ((hi_s32)wbc_config->dynamic_range != -1) {
        wbc_config->wbc_attr.dynamic_range = wbc_config->dynamic_range;
    }

    /* set compress mode if changed */
    if ((hi_s32)wbc_config->compress_mode != -1) {
        wbc_config->wbc_attr.compress_mode = wbc_config->compress_mode;
    }

    if (wbc_config->depth > -1) {
        ret = hi_mpi_vo_set_wbc_depth(wbc_config->vo_wbc, 5); /* 5: wbc depth */
        if (ret != HI_SUCCESS) {
            sample_print("failed with %#x!\n", ret);
            return ret;
        }
    }

    wbc_config->wbc_source.src_type = wbc_config->source_type;
    ret = sample_private_vo_start_wbc(wbc_config->vo_wbc, &wbc_config->wbc_attr, &wbc_config->wbc_source);
    return ret;
}

hi_s32 sample_comm_vo_stop_wbc(sample_vo_wbc_cfg *wbc_config)
{
    check_null_ptr_return(wbc_config);
    return sample_private_vo_stop_wbc(wbc_config->vo_wbc);
}

hi_s32 sample_comm_vo_get_def_wbc_config(sample_vo_wbc_cfg *wbc_config)
{
    check_null_ptr_return(wbc_config);

    wbc_config->vo_wbc = 0;
    wbc_config->source_type = HI_VO_WBC_SRC_DEV;
    wbc_config->wbc_source.src_type = HI_VO_WBC_SRC_DEV;
    wbc_config->wbc_source.src_id = 0;
    wbc_config->dynamic_range = HI_DYNAMIC_RANGE_BUTT;
    wbc_config->compress_mode = HI_COMPRESS_MODE_BUTT;
    wbc_config->depth = HI_INVALID_VALUE;

    sample_private_vo_get_def_wbc_attr(&wbc_config->wbc_attr, &wbc_config->wbc_source);

    return HI_SUCCESS;
}

#if VO_LT8618SX
static hi_s32 g_vo_lt8618sx_fd = HI_INVALID_VALUE;

hi_s32 sample_comm_vo_bt1120_start(hi_vo_dev vo_dev, hi_vo_pub_attr *pub_attr)
{
    hi_s32 ret;
    hi_u32 lt_mode, clk_edge;
    hi_vo_bt_param bt_param = {0};

    check_null_ptr_return(pub_attr);
    lt_mode = pub_attr->intf_sync;
    if (!(pub_attr->intf_type & HI_VO_INTF_BT1120)) {
        return HI_FAILURE;
    }

    ret = hi_mpi_vo_get_bt_param(vo_dev, &bt_param);
    if (ret != HI_SUCCESS) {
        sample_print("failed with %#x!\n", ret);
        return HI_FAILURE;
    }

    g_vo_lt8618sx_fd = open(LT8618SX_DEV_NAME, O_RDONLY);
    if (g_vo_lt8618sx_fd < 0) {
        sample_print("open file (%s) fail!\n", LT8618SX_DEV_NAME);
        return HI_FAILURE;
    }

    /* 0:single, 1:dual, set the clk edge firstly */
    clk_edge = (hi_u32)bt_param.clk_edge;
    if (ioctl(g_vo_lt8618sx_fd, LT_CMD_SETCLK, &clk_edge) < 0) {
        sample_print("ioctl (%s) set clk fail!\n", LT8618SX_DEV_NAME);
    }

    if (ioctl(g_vo_lt8618sx_fd, LT_CMD_SETMODE, &lt_mode) < 0) {
        sample_print("ioctl (%s) set mode fail!\n", LT8618SX_DEV_NAME);
    }
    close(g_vo_lt8618sx_fd);
    g_vo_lt8618sx_fd = HI_INVALID_VALUE;
    return HI_SUCCESS;
}
#else
hi_s32 sample_comm_vo_bt1120_start(hi_vo_dev vo_dev, hi_vo_pub_attr *pub_attr)
{
    hi_unused(vo_dev);
    hi_unused(pub_attr);
    return HI_SUCCESS;
}
#endif

static const hi_hdmi_video_format g_vo_hdmi_sync[HI_VO_OUT_BUTT] = {
    HI_HDMI_VIDEO_FORMAT_PAL,
    HI_HDMI_VIDEO_FORMAT_NTSC,
    HI_HDMI_VIDEO_FORMAT_VESA_CUSTOMER_DEFINE,
    HI_HDMI_VIDEO_FORMAT_VESA_CUSTOMER_DEFINE,
    HI_HDMI_VIDEO_FORMAT_861D_640X480_60,
    HI_HDMI_VIDEO_FORMAT_480P_60,
    HI_HDMI_VIDEO_FORMAT_576P_50,
    HI_HDMI_VIDEO_FORMAT_VESA_800X600_60,
    HI_HDMI_VIDEO_FORMAT_VESA_1024X768_60,
    HI_HDMI_VIDEO_FORMAT_720P_50,
    HI_HDMI_VIDEO_FORMAT_720P_60,
    HI_HDMI_VIDEO_FORMAT_VESA_1280X800_60,
    HI_HDMI_VIDEO_FORMAT_VESA_1280X1024_60,
    HI_HDMI_VIDEO_FORMAT_VESA_1366X768_60,
    HI_HDMI_VIDEO_FORMAT_VESA_1400X1050_60,
    HI_HDMI_VIDEO_FORMAT_VESA_1440X900_60,
    HI_HDMI_VIDEO_FORMAT_VESA_1680X1050_60,
    HI_HDMI_VIDEO_FORMAT_1080P_24,
    HI_HDMI_VIDEO_FORMAT_1080P_25,
    HI_HDMI_VIDEO_FORMAT_1080P_30,
    HI_HDMI_VIDEO_FORMAT_1080i_50,
    HI_HDMI_VIDEO_FORMAT_1080i_60,
    HI_HDMI_VIDEO_FORMAT_1080P_50,
    HI_HDMI_VIDEO_FORMAT_1080P_60,
    HI_HDMI_VIDEO_FORMAT_VESA_1600X1200_60,
    HI_HDMI_VIDEO_FORMAT_VESA_1920X1200_60,
    HI_HDMI_VIDEO_FORMAT_1920x2160_30,
    HI_HDMI_VIDEO_FORMAT_2560x1440_30,
    HI_HDMI_VIDEO_FORMAT_2560x1440_60,
    HI_HDMI_VIDEO_FORMAT_2560x1600_60,
    HI_HDMI_VIDEO_FORMAT_3840X2160P_24,
    HI_HDMI_VIDEO_FORMAT_3840X2160P_25,
    HI_HDMI_VIDEO_FORMAT_3840X2160P_30,
    HI_HDMI_VIDEO_FORMAT_3840X2160P_50,
    HI_HDMI_VIDEO_FORMAT_3840X2160P_60,
    HI_HDMI_VIDEO_FORMAT_4096X2160P_24,
    HI_HDMI_VIDEO_FORMAT_4096X2160P_25,
    HI_HDMI_VIDEO_FORMAT_4096X2160P_30,
    HI_HDMI_VIDEO_FORMAT_4096X2160P_50,
    HI_HDMI_VIDEO_FORMAT_4096X2160P_60,
    HI_HDMI_VIDEO_FORMAT_7680X4320P_30,
    HI_HDMI_VIDEO_FORMAT_VESA_CUSTOMER_DEFINE,
    HI_HDMI_VIDEO_FORMAT_VESA_CUSTOMER_DEFINE,
    HI_HDMI_VIDEO_FORMAT_VESA_CUSTOMER_DEFINE,
    HI_HDMI_VIDEO_FORMAT_VESA_CUSTOMER_DEFINE,
    HI_HDMI_VIDEO_FORMAT_VESA_CUSTOMER_DEFINE,
    HI_HDMI_VIDEO_FORMAT_VESA_CUSTOMER_DEFINE,
    HI_HDMI_VIDEO_FORMAT_VESA_CUSTOMER_DEFINE,
    HI_HDMI_VIDEO_FORMAT_VESA_CUSTOMER_DEFINE,
};

static hi_void sample_comm_vo_hdmi_convert_sync(hi_vo_intf_sync intf_sync, hi_hdmi_video_format *video_fmt)
{
    *video_fmt = HI_HDMI_VIDEO_FORMAT_1080P_60;
    if ((intf_sync >= HI_VO_OUT_PAL) && (intf_sync < HI_VO_OUT_BUTT)) {
        *video_fmt = g_vo_hdmi_sync[intf_sync];
    }
}

hi_s32 sample_comm_vo_hdmi_start(hi_vo_intf_sync intf_sync)
{
    hi_hdmi_attr attr;
    hi_hdmi_video_format video_fmt;
    hi_hdmi_id hdmi_id = HI_HDMI_ID_0;

    sample_comm_vo_hdmi_convert_sync(intf_sync, &video_fmt);

    check_return(hi_mpi_hdmi_init(), "hi_mpi_hdmi_init");
    check_return(hi_mpi_hdmi_open(hdmi_id), "hi_mpi_hdmi_open");
    check_return(hi_mpi_hdmi_get_attr(hdmi_id, &attr), "hi_mpi_hdmi_get_attr");
    attr.video_format = video_fmt;
       
    printf("%s => attr.video_format:%d[3840X2160P_30:%d,3840X2160P_60:%d]\n", __func__, attr.video_format
        , HI_HDMI_VIDEO_FORMAT_3840X2160P_30, HI_HDMI_VIDEO_FORMAT_3840X2160P_60);
    printf("%s => attr.deep_color_mode:%d[OT_HDMI_DEEP_COLOR_24BIT:%d]\n", __func__, attr.deep_color_mode
        , OT_HDMI_DEEP_COLOR_24BIT);
    printf("%s => attr.auth_mode_en:%d[%s]\n", __func__, attr.auth_mode_en
        , "0: disabled 1: enable");
    printf("%s => attr.bit_depth:%d[OT_HDMI_BIT_DEPTH_16:%d]\n", __func__, attr.bit_depth
        , OT_HDMI_BIT_DEPTH_16);
        
    check_return(hi_mpi_hdmi_set_attr(hdmi_id, &attr), "hi_mpi_hdmi_set_attr");
    check_return(hi_mpi_hdmi_start(hdmi_id), "hi_mpi_hdmi_start");
    return HI_SUCCESS;
}

hi_s32 sample_comm_vo_hdmi_stop(hi_void)
{
    hi_hdmi_id hdmi_id = HI_HDMI_ID_0;

    check_return(hi_mpi_hdmi_stop(hdmi_id), "hi_mpi_hdmi_stop");
    check_return(hi_mpi_hdmi_close(hdmi_id), "hi_mpi_hdmi_close");
    check_return(hi_mpi_hdmi_deinit(), "hi_mpi_hdmi_deinit");
    return HI_SUCCESS;
}

/* dhd0: 1080P60, dhd1: 1080P60 */
static const hi_vo_sync_info g_sample_comm_vo_timing[HI_VO_MAX_PHYS_DEV_NUM] = {
    /*
     * |--INTFACE---||-----TOP-----||----HORIZON--------||----BOTTOM-----||-PULSE-||-INVERSE-|
     * syncm, iop, itf,   vact, vbb,  vfb,  hact,  hbb,  hfb, hmid,bvact,bvbb, bvfb, hpw, vpw,idv, ihs, ivs
     */
    { 0, 1, 1, 1080, 41, 4, 1920, 192, 88,  1,    1,   1,  1,  44, 5, 0, 0, 0 }, /* 1080P@60_hz */
    { 0, 1, 1, 1080, 41, 4, 1920, 192, 88,  1,    1,   1,  1,  44, 5, 0, 0, 0 }, /* 1080P@60_hz */
};

const hi_vo_sync_info *vo_get_dev_timing(hi_vo_dev dev)
{
    if ((dev < 0) || (dev >= HI_VO_MAX_PHYS_DEV_NUM)) {
        return HI_NULL;
    }
    return &(g_sample_comm_vo_timing[dev]);
}

/* dhd0: 1080P60, dhd1: 1080P60 */
static const hi_vo_user_sync_info g_sample_comm_vo_sync_info[HI_VO_MAX_PHYS_DEV_NUM] = {
    {
        .user_sync_attr = {
            .clk_src = HI_VO_CLK_SRC_PLL,
            .vo_pll = { /* if hdmi, set it by pixel clk and div mode */
                .fb_div = 99, /* 99 fb div */
                .frac = 0,
                .ref_div = 2, /* 2 ref div */
                .post_div1 = 4, /* 4 post div1 */
                .post_div2 = 2, /* 2 post div2 */
            },
        },
        .pre_div = 1, /* if hdmi, set it by pixel clk */
        .dev_div = 1, /* if rgb, set it by serial mode */
        .clk_reverse_en = HI_FALSE,
    }, {
        .user_sync_attr = {
            .clk_src = HI_VO_CLK_SRC_PLL,
            .vo_pll = { /* if hdmi, set it by pixel clk and div mode */
                .fb_div = 99, /* 99 fb div */
                .frac = 0,
                .ref_div = 2, /* 2 ref div */
                .post_div1 = 4, /* 4 post div1 */
                .post_div2 = 2, /* 2 post div2 */
            },
        },
        .pre_div = 1, /* if hdmi, set it by pixel clk */
        .dev_div = 1, /* if rgb, set it by serial mode */
        .clk_reverse_en = HI_FALSE,
    }
};

const hi_vo_user_sync_info *vo_get_dev_user_sync_info(hi_vo_dev dev)
{
    if ((dev < 0) || (dev >= HI_VO_MAX_PHYS_DEV_NUM)) {
        return HI_NULL;
    }
    return &g_sample_comm_vo_sync_info[dev];
}

static hi_void sample_comm_vo_get_def_sync_info_config(hi_vo_dev dev, hi_vo_sync_info *sync_info)
{
    (hi_void)memcpy_s(sync_info, sizeof(hi_vo_sync_info), &g_sample_comm_vo_timing[dev], sizeof(hi_vo_sync_info));
}

static hi_void sample_comm_vo_get_def_user_sync_config(hi_vo_dev dev, hi_vo_user_sync_info *user_sync)
{
    (hi_void)memcpy_s(user_sync, sizeof(hi_vo_user_sync_info), &g_sample_comm_vo_sync_info[dev],
        sizeof(hi_vo_user_sync_info));
}

hi_s32 sample_comm_vo_get_def_config(sample_vo_cfg *vo_config)
{
    hi_rect def_disp_rect = { 0, 0, 1920, 1080 };
    hi_size def_img_size = { 1920, 1080 };

    check_null_ptr_return(vo_config);

    vo_config->vo_dev = SAMPLE_VO_DEV_UHD;
    vo_config->vo_intf_type = HI_VO_INTF_HDMI;
    vo_config->intf_sync = HI_VO_OUT_1080P60;
    vo_config->bg_color = COLOR_RGB_BLACK;
    vo_config->pix_format = HI_PIXEL_FORMAT_YVU_SEMIPLANAR_420;
    vo_config->disp_rect = def_disp_rect;
    vo_config->image_size = def_img_size;
    vo_config->vo_part_mode = HI_VO_PARTITION_MODE_SINGLE;
    vo_config->dis_buf_len = 3; /* 3: def buf len for single */
    vo_config->dst_dynamic_range = HI_DYNAMIC_RANGE_SDR8;
    vo_config->vo_mode = VO_MODE_1MUX;
    vo_config->compress_mode = HI_COMPRESS_MODE_NONE;

    sample_comm_vo_get_def_sync_info_config(vo_config->vo_dev, &vo_config->sync_info);
    sample_comm_vo_get_def_user_sync_config(vo_config->vo_dev, &vo_config->user_sync);

    return HI_SUCCESS;
}

static hi_s32 sample_comm_vo_get_vo_layer_attr(const sample_vo_cfg *vo_config, hi_vo_video_layer_attr *layer_attr)
{
    hi_s32 ret;
    hi_rect def_disp_rect = { 0, 0, 1920, 1080 };
    hi_size def_img_size = { 1920, 1080 };

    check_null_ptr_return(vo_config);
    check_null_ptr_return(layer_attr);

    ret = sample_comm_vo_get_width_height(vo_config->intf_sync, &layer_attr->display_rect.width,
        &layer_attr->display_rect.height, &layer_attr->display_frame_rate);
    if (ret != HI_SUCCESS) {
        sample_print("sample_comm_vo_get_width_height failed!\n");
        return ret;
    }
    if (vo_config->intf_sync == HI_VO_OUT_USER) {
        layer_attr->display_frame_rate = vo_config->dev_frame_rate;
    }

    layer_attr->cluster_mode_en = HI_FALSE;
    layer_attr->double_frame_en = HI_FALSE;
    layer_attr->pixel_format = vo_config->pix_format;
    layer_attr->display_rect.x = 0;
    layer_attr->display_rect.y = 0;

    /* set display rectangle if changed. */
    if (memcmp(&vo_config->disp_rect, &def_disp_rect, sizeof(hi_rect)) != 0) {
        layer_attr->display_rect.width = vo_config->disp_rect.width;
        layer_attr->display_rect.height = vo_config->disp_rect.height;
    }

    layer_attr->img_size.width = layer_attr->display_rect.width;
    layer_attr->img_size.height = layer_attr->display_rect.height;

    /* set image size if changed. */
    if (memcmp(&vo_config->image_size, &def_img_size, sizeof(hi_size)) != 0) {
        layer_attr->img_size.width = vo_config->image_size.width;
        layer_attr->img_size.height = vo_config->image_size.height;
    }

    layer_attr->dst_dynamic_range = vo_config->dst_dynamic_range;
    layer_attr->display_buf_len = vo_config->dis_buf_len;
    layer_attr->partition_mode = vo_config->vo_part_mode;
    layer_attr->compress_mode = vo_config->compress_mode;
    return HI_SUCCESS;
}

static hi_void sample_comm_vo_set_intf_param(const sample_vo_cfg *vo_config)
{
    hi_vo_hdmi_param hdmi_param = { 0 };

    /* set interface param */
    if (g_vo_hdmi_rgb_mode == HI_TRUE) {
        hi_mpi_vo_get_hdmi_param(vo_config->vo_dev, &hdmi_param);
        hdmi_param.csc.csc_matrix = HI_VO_CSC_MATRIX_BT709FULL_TO_RGBFULL;
        hi_mpi_vo_set_hdmi_param(vo_config->vo_dev, &hdmi_param);
    }
}

hi_void sample_comm_vo_set_hdmi_rgb_mode(hi_bool enable)
{
    g_vo_hdmi_rgb_mode = !!enable;
}

hi_s32 sample_comm_vo_start_vo(const sample_vo_cfg *vo_config)
{
    /* VO device vo_dev# information declaration. */
    hi_s32 ret;
    hi_vo_dev vo_dev;
    hi_vo_layer vo_layer;
    hi_vo_pub_attr vo_pub_attr = { 0 };
    hi_vo_video_layer_attr layer_attr = { 0 };

    check_null_ptr_return(vo_config);

    vo_dev = vo_config->vo_dev;
    vo_layer = vo_config->vo_dev;

    /* set and start VO device vo_dev#. */
    vo_pub_attr.intf_type = vo_config->vo_intf_type;
    vo_pub_attr.intf_sync = vo_config->intf_sync;
    vo_pub_attr.bg_color = vo_config->bg_color;
    (hi_void)memcpy_s(&vo_pub_attr.sync_info, sizeof(hi_vo_sync_info), &vo_config->sync_info, sizeof(hi_vo_sync_info));
    ret = sample_comm_vo_start_dev(vo_dev, &vo_pub_attr, &vo_config->user_sync, vo_config->dev_frame_rate);
    if (ret != HI_SUCCESS) {
        sample_print("sample_comm_vo_start_dev failed!\n");
        return ret;
    }

    ret = sample_comm_vo_get_vo_layer_attr(vo_config, &layer_attr);
    if (ret != HI_SUCCESS) {
        sample_print("sample_comm_vo_start get video layer failed!\n");
        (hi_void)sample_comm_vo_stop_dev(vo_dev);
        return ret;
    }

    ret = sample_comm_vo_start_layer(vo_layer, &layer_attr);
    if (ret != HI_SUCCESS) {
        sample_print("sample_comm_vo_start set video layer failed!\n");
        (hi_void)sample_comm_vo_stop_dev(vo_dev);
        return ret;
    }

    /* set interface param */
    sample_comm_vo_set_intf_param(vo_config);

    /* start vo channels. */
    ret = sample_comm_vo_start_chn(vo_layer, vo_config->vo_mode);
    if (ret != HI_SUCCESS) {
        sample_print("sample_comm_vo_start_chn failed!\n");
        (hi_void)sample_comm_vo_stop_layer(vo_layer);
        (hi_void)sample_comm_vo_stop_dev(vo_dev);
        return ret;
    }

    /* start hdmi device. note : do this after vo device started. */
    if (vo_config->vo_intf_type & HI_VO_INTF_HDMI) {
        (hi_void)sample_comm_vo_hdmi_start(vo_config->intf_sync);
    }

    /* start bt1120 device. note : do this after vo device started. */
    if (vo_config->vo_intf_type & HI_VO_INTF_BT1120) {
        (hi_void)sample_comm_vo_bt1120_start(vo_dev, &vo_pub_attr);
    }

    return HI_SUCCESS;
}

hi_s32 sample_comm_vo_start_pip(sample_vo_cfg *vo_config)
{
    /* VO device vo_dev# information declaration. */
    hi_vo_dev vo_dev;
    hi_vo_layer vo_layer;
    hi_vo_pub_attr vo_pub_attr = { 0 };
    hi_vo_video_layer_attr layer_attr = { 0 };
    hi_s32 ret;

    check_null_ptr_return(vo_config);

    vo_dev = vo_config->vo_dev;
    vo_layer = SAMPLE_VO_LAYER_PIP;

    vo_pub_attr.intf_sync = vo_config->intf_sync;
    /* set and start layer vo_dev#. */
    ret = sample_comm_vo_get_width_height(vo_pub_attr.intf_sync, &layer_attr.display_rect.width,
        &layer_attr.display_rect.height, &layer_attr.display_frame_rate);
    if (ret != HI_SUCCESS) {
        sample_print("sample_comm_vo_get_width_height failed!\n");
        (hi_void)sample_comm_vo_stop_dev(vo_dev);
        return ret;
    }

    layer_attr.cluster_mode_en = HI_FALSE;
    layer_attr.double_frame_en = HI_FALSE;
    layer_attr.pixel_format = vo_config->pix_format;
    layer_attr.display_rect.x = 0;
    layer_attr.display_rect.y = 0;
    /* set display rectangle if changed. */
    layer_attr.display_rect.width /= 2;  /* 2: 2x */
    layer_attr.display_rect.height /= 2; /* 2: 2x */
    layer_attr.img_size.width = layer_attr.display_rect.width;
    layer_attr.img_size.height = layer_attr.display_rect.height;
    /* set image size if changed. */
    layer_attr.dst_dynamic_range = vo_config->dst_dynamic_range;
    layer_attr.display_buf_len = vo_config->dis_buf_len;
    layer_attr.partition_mode = vo_config->vo_part_mode;

    ret = sample_comm_vo_start_layer(vo_layer, &layer_attr);
    if (ret != HI_SUCCESS) {
        sample_print("sample_comm_vo_start video layer failed!\n");
        (hi_void)sample_comm_vo_stop_dev(vo_dev);
        return ret;
    }

    /* start vo channels. */
    ret = sample_comm_vo_start_chn(vo_layer, vo_config->vo_mode);
    if (ret != HI_SUCCESS) {
        sample_print("sample_comm_vo_start_chn failed!\n");
        (hi_void)sample_comm_vo_stop_layer(vo_layer);
        (hi_void)sample_comm_vo_stop_dev(vo_dev);
        return ret;
    }

    return HI_SUCCESS;
}

hi_s32 sample_comm_vo_stop_vo(const sample_vo_cfg *vo_config)
{
    hi_s32 ret;
    hi_vo_dev vo_dev;
    hi_vo_layer vo_layer;
    sample_vo_mode vo_mode;

    check_null_ptr_return(vo_config);

    vo_dev = vo_config->vo_dev;
    vo_layer = vo_config->vo_dev;
    vo_mode = vo_config->vo_mode;

    if (HI_VO_INTF_HDMI & vo_config->vo_intf_type) {
        sample_comm_vo_hdmi_stop();
    }

    ret = sample_comm_vo_stop_chn(vo_layer, vo_mode);
    if (ret != HI_SUCCESS) {
        sample_print("sample_comm_vo_stop_vo stop chn failed!\n");
        return ret;
    }

    ret = sample_comm_vo_stop_layer(vo_layer);
    if (ret != HI_SUCCESS) {
        sample_print("sample_comm_vo_stop_vo stop layer failed!\n");
        return ret;
    }

    ret = sample_comm_vo_stop_dev(vo_dev);
    if (ret != HI_SUCCESS) {
        sample_print("sample_comm_vo_stop_vo stop dev failed!\n");
        return ret;
    }

    return HI_SUCCESS;
}

hi_s32 sample_comm_vo_stop_pip(const sample_vo_cfg *vo_config)
{
    hi_s32 ret;
    hi_vo_layer vo_layer;

    check_null_ptr_return(vo_config);

    vo_layer = SAMPLE_VO_LAYER_PIP;
    ret = sample_comm_vo_stop_chn(vo_layer, vo_config->vo_mode);
    if (ret != HI_SUCCESS) {
        sample_print("sample_comm_vo_stop_pip stop chn failed!\n");
        return ret;
    }

    ret = sample_comm_vo_stop_layer(vo_layer);
    if (ret != HI_SUCCESS) {
        sample_print("sample_comm_vo_stop_pip stop layer failed!\n");
        return ret;
    }

    return HI_SUCCESS;
}

hi_s32 sample_comm_vo_get_def_layer_config(sample_comm_vo_layer_cfg *vo_layer_config)
{
    hi_rect def_disp_rect = { 0, 0, 1920, 1080 };
    hi_size def_img_size = { 1920, 1080 };

    check_null_ptr_return(vo_layer_config);

    vo_layer_config->vo_layer = SAMPLE_VO_LAYER_VHD2;
    vo_layer_config->intf_sync = HI_VO_OUT_1080P60;
    vo_layer_config->display_rect = def_disp_rect;
    vo_layer_config->image_size = def_img_size;
    vo_layer_config->pix_format = HI_PIXEL_FORMAT_YVU_SEMIPLANAR_420;
    vo_layer_config->dis_buf_len = 3; /* 3: display buf len */
    vo_layer_config->vo_mode = VO_MODE_1MUX;
    return HI_SUCCESS;
}

static hi_s32 sample_comm_vo_get_layer_layer_attr(sample_comm_vo_layer_cfg *vo_layer_config,
    hi_vo_video_layer_attr *layer_attr)
{
    hi_s32 ret;
    hi_u32 frame_rate = 0;
    hi_u32 width = 0;
    hi_u32 height = 0;
    hi_rect def_disp_rect = { 0, 0, 1920, 1080 };
    hi_size def_img_size = { 1920, 1080 };

    check_null_ptr_return(vo_layer_config);
    check_null_ptr_return(layer_attr);

    /* start vo layer. */
    ret = sample_comm_vo_get_width_height(vo_layer_config->intf_sync, &width, &height, &frame_rate);
    if (ret != HI_SUCCESS) {
        sample_print("sample_comm_vo_get_width_height failed!\n");
        return HI_FAILURE;
    }

    layer_attr->display_rect.x = 0;
    layer_attr->display_rect.y = 0;
    layer_attr->display_rect.width = width;
    layer_attr->display_rect.height = height;
    layer_attr->display_frame_rate = frame_rate;
    layer_attr->double_frame_en = HI_FALSE;
    layer_attr->cluster_mode_en = HI_FALSE;
    layer_attr->pixel_format = vo_layer_config->pix_format;

    /* set display rectangle if changed. */
    if (memcmp(&vo_layer_config->display_rect, &def_disp_rect, sizeof(hi_rect)) != 0) {
        layer_attr->display_rect.width = vo_layer_config->display_rect.width;
        layer_attr->display_rect.height = vo_layer_config->display_rect.height;
    }
    layer_attr->img_size.width = layer_attr->display_rect.width;
    layer_attr->img_size.height = layer_attr->display_rect.height;

    /* set image size if changed. */
    if (memcmp(&vo_layer_config->image_size, &def_img_size, sizeof(hi_size)) != 0) {
        layer_attr->img_size.width = vo_layer_config->image_size.width;
        layer_attr->img_size.height = vo_layer_config->image_size.height;
    }

    layer_attr->dst_dynamic_range = vo_layer_config->dst_dynamic_range;
    layer_attr->display_buf_len = vo_layer_config->dis_buf_len;
    return HI_SUCCESS;
}

hi_s32 sample_comm_vo_start_layer_chn(sample_comm_vo_layer_cfg *vo_layer_config)
{
    hi_s32 ret;
    hi_vo_layer vo_layer;
    hi_vo_video_layer_attr layer_attr = { 0 };

    check_null_ptr_return(vo_layer_config);

    vo_layer = vo_layer_config->vo_layer;
    ret = sample_comm_vo_get_layer_layer_attr(vo_layer_config, &layer_attr);
    if (ret != HI_SUCCESS) {
        sample_print("sample_comm_vo_start video layer failed!\n");
        return ret;
    }

    ret = sample_comm_vo_start_layer(vo_layer, &layer_attr);
    if (ret != HI_SUCCESS) {
        sample_print("sample_comm_vo_start video layer failed!\n");
        return ret;
    }

    /* start vo channels. */
    ret = sample_comm_vo_start_chn(vo_layer, vo_layer_config->vo_mode);
    if (ret != HI_SUCCESS) {
        sample_print("sample_comm_vo_start_chn failed!\n");
        (hi_void)sample_comm_vo_stop_layer(vo_layer);
        return ret;
    }
    return ret;
}

hi_s32 sample_comm_vo_stop_layer_chn(sample_comm_vo_layer_cfg *vo_layer_config)
{
    hi_s32 ret;
    hi_vo_layer vo_layer;

    check_null_ptr_return(vo_layer_config);

    vo_layer = vo_layer_config->vo_layer;
    ret = sample_comm_vo_stop_chn(vo_layer, vo_layer_config->vo_mode);
    if (ret != HI_SUCCESS) {
        sample_print("sample_comm_vo_stop_layer_chn stop chn failed!\n");
        return ret;
    }

    ret = sample_comm_vo_stop_layer(vo_layer);
    if (ret != HI_SUCCESS) {
        sample_print("sample_comm_vo_stop_layer_chn stop layer failed!\n");
        return ret;
    }

    return HI_SUCCESS;
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* end of #ifdef __cplusplus */
