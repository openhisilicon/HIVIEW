/*
  Copyright (c), 2001-2024, Shenshu Tech. Co., Ltd.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <sys/time.h>

#include "sample_comm.h"

#define VPSS_DEFAULT_WIDTH  1920
#define VPSS_DEFAULT_HEIGHT 1080

#define SMALL_STREAM_WIDTH  720
#define SMALL_STREAM_HEIGHT 480

hi_void sample_comm_vpss_get_default_grp_attr(hi_vpss_grp_attr *grp_attr)
{
    grp_attr->ie_en                     = HI_FALSE;
    grp_attr->dci_en                    = HI_FALSE;
    grp_attr->buf_share_en              = HI_FALSE;
    grp_attr->mcf_en                    = HI_FALSE;
    grp_attr->max_width                 = VPSS_DEFAULT_WIDTH;
    grp_attr->max_height                = VPSS_DEFAULT_HEIGHT;
    grp_attr->max_dei_width             = 0;
    grp_attr->max_dei_height            = 0;
    grp_attr->dynamic_range             = HI_DYNAMIC_RANGE_SDR8;
    grp_attr->pixel_format              = HI_PIXEL_FORMAT_YVU_SEMIPLANAR_420;
    grp_attr->dei_mode                  = HI_VPSS_DEI_MODE_OFF;
    grp_attr->buf_share_chn             = HI_VPSS_CHN0;
    grp_attr->frame_rate.src_frame_rate = -1;
    grp_attr->frame_rate.dst_frame_rate = -1;
}

hi_void sample_comm_vpss_get_default_chn_attr(hi_vpss_chn_attr *chn_attr)
{
    chn_attr->mirror_en                 = HI_FALSE;
    chn_attr->flip_en                   = HI_FALSE;
    chn_attr->border_en                 = HI_FALSE;
    chn_attr->width                     = VPSS_DEFAULT_WIDTH;
    chn_attr->height                    = VPSS_DEFAULT_HEIGHT;
    chn_attr->depth                     = 0;
    chn_attr->chn_mode                  = HI_VPSS_CHN_MODE_USER;
    chn_attr->video_format              = HI_VIDEO_FORMAT_LINEAR;
    chn_attr->dynamic_range             = HI_DYNAMIC_RANGE_SDR8;
    chn_attr->pixel_format              = HI_PIXEL_FORMAT_YVU_SEMIPLANAR_420;
    chn_attr->compress_mode             = HI_COMPRESS_MODE_NONE;
    chn_attr->aspect_ratio.mode         = HI_ASPECT_RATIO_NONE;
    chn_attr->frame_rate.src_frame_rate = -1;
    chn_attr->frame_rate.dst_frame_rate = -1;
}

hi_void sample_comm_vpss_get_default_3dnr_attr(hi_3dnr_attr *nr_attr)
{
    nr_attr->enable         = HI_FALSE;
    nr_attr->nr_type        = HI_NR_TYPE_VIDEO_NORM;
    nr_attr->compress_mode  = HI_COMPRESS_MODE_FRAME;
    nr_attr->nr_motion_mode = HI_NR_MOTION_MODE_NORM;
}

hi_s32 sample_comm_vpss_get_wrap_cfg(hi_vpss_chn_attr chn_attr[HI_VPSS_MAX_PHYS_CHN_NUM],
    hi_vi_vpss_mode_type mode, hi_u32 full_lines_std, hi_vpss_chn_buf_wrap_attr *wrap_attr)
{
    hi_vpss_venc_wrap_param wrap_param = { 0 };
    hi_u32 buf_line;
    hi_u32 wrap_buf_size;
    hi_pic_buf_attr buf_attr;

    wrap_attr->enable = HI_FALSE;

#ifdef CONFIG_OT_FPGA
        wrap_param.frame_rate = 10; /* frame_rate 10 */
#else
        wrap_param.frame_rate = 30; /* frame_rate 30 */
#endif
    if (chn_attr[0].frame_rate.src_frame_rate > 0 && chn_attr[0].frame_rate.dst_frame_rate > 0) {
        /* frame_rate 30 */
        wrap_param.frame_rate = chn_attr[0].frame_rate.dst_frame_rate * 30 / chn_attr[0].frame_rate.src_frame_rate;
    }
    if (mode == HI_VI_ONLINE_VPSS_ONLINE) {
        wrap_param.all_online = HI_TRUE;
    }
    wrap_param.full_lines_std = full_lines_std;
    wrap_param.large_stream_size.width = chn_attr[0].width;
    wrap_param.large_stream_size.height = chn_attr[0].height;
    wrap_param.small_stream_size.width = chn_attr[1].width;
    wrap_param.small_stream_size.height = chn_attr[1].height;
    if (hi_mpi_sys_get_vpss_venc_wrap_buf_line(&wrap_param, &buf_line) != HI_SUCCESS) {
        printf("%s: get buf line failed\n", __FUNCTION__);
        return HI_FAILURE;
    }

    buf_attr.width = chn_attr[0].width;
    buf_attr.height = chn_attr[0].height;
    buf_attr.bit_width = HI_DATA_BIT_WIDTH_8;
    buf_attr.pixel_format = chn_attr[0].pixel_format;
    buf_attr.compress_mode = chn_attr[0].compress_mode;
    buf_attr.align = HI_DEFAULT_ALIGN;
    buf_attr.video_format = chn_attr[0].video_format;
    wrap_buf_size = hi_comm_get_vpss_venc_wrap_buf_size(&buf_attr, buf_line);

    /* out */
    wrap_attr->enable = HI_TRUE;
    wrap_attr->buf_line = buf_line;
    wrap_attr->buf_size = wrap_buf_size;
    printf("wrap online is %u, buf line is %u, buf size is %u\n", wrap_param.all_online, buf_line, wrap_buf_size);
    return HI_SUCCESS;
}

hi_void sample_comm_vpss_get_default_vpss_cfg(sample_vpss_cfg *vpss_cfg, td_bool vpss_wrap_en)
{
    hi_vpss_chn chn;
    hi_size in_size;
    sample_sns_type sns_type = SENSOR0_TYPE;

    sample_comm_vi_get_size_by_sns_type(sns_type, &in_size);
    sample_comm_vpss_get_default_grp_attr(&vpss_cfg->grp_attr);
    sample_comm_vpss_get_default_3dnr_attr(&vpss_cfg->nr_attr);
    vpss_cfg->vpss_grp = 0;
    vpss_cfg->grp_attr.max_width  = in_size.width;
    vpss_cfg->grp_attr.max_height = in_size.height;

    for (chn = 0; chn < HI_VPSS_MAX_PHYS_CHN_NUM; chn++) {
        vpss_cfg->chn_en[chn] = HI_FALSE;
        sample_comm_vpss_get_default_chn_attr(&vpss_cfg->chn_attr[chn]);
        if (chn > HI_VPSS_CHN1) {
            vpss_cfg->chn_attr[chn].compress_mode = HI_COMPRESS_MODE_NONE;
        }
        vpss_cfg->chn_attr[chn].width  = in_size.width;
        vpss_cfg->chn_attr[chn].height = in_size.height;
#ifdef OT_FPGA
        vpss_cfg->chn_attr[chn].frame_rate.src_frame_rate = 30; /* 30: fpga src frame rate */
        vpss_cfg->chn_attr[chn].frame_rate.dst_frame_rate = 10; /* 10: fpga dst frame rate */
#endif
    }
    vpss_cfg->chn_en[HI_VPSS_CHN0] = HI_TRUE;
    vpss_cfg->chn_attr[HI_VPSS_CHN0].compress_mode = HI_COMPRESS_MODE_SEG_COMPACT;

    if (vpss_wrap_en) {
        vpss_cfg->chn_en[1] = HI_TRUE; /* vpss_chn0->vnec */
        vpss_cfg->chn_attr[1].width = SMALL_STREAM_WIDTH;
        vpss_cfg->chn_attr[1].height = SMALL_STREAM_HEIGHT;
        vpss_cfg->wrap_attr[0].enable = HI_TRUE;
    }
}

static hi_s32 sample_common_vpss_set_chn_wrap_attr(hi_vpss_grp grp, hi_vpss_chn chn,
    const hi_vpss_chn_buf_wrap_attr *attr)
{
    hi_s32 ret = HI_SUCCESS;
    if (chn == HI_VPSS_CHN0 && attr->enable == HI_TRUE) {
        ret = hi_mpi_vpss_set_chn_buf_wrap(grp, chn, attr);
    }
    return ret;
}

static hi_s32 sample_common_vpss_start_chn(hi_vpss_grp grp, const hi_bool *chn_enable,
    const hi_vpss_chn_attr *chn_attr, const hi_vpss_chn_buf_wrap_attr *wrap_attr, hi_u32 chn_array_size)
{
    hi_vpss_chn vpss_chn;
    hi_s32 ret, i;

    for (i = 0; i < (hi_s32)chn_array_size; ++i) {
        if (chn_enable[i] == HI_TRUE) {
            vpss_chn = i;
            ret = hi_mpi_vpss_set_chn_attr(grp, vpss_chn, &chn_attr[vpss_chn]);
            if (ret != HI_SUCCESS) {
                sample_print("hi_mpi_vpss_set_chn_attr failed with %#x\n", ret);
                goto disable_chn;
            }
            /* set chn0 wrap attr first, then enable chn */
            ret = sample_common_vpss_set_chn_wrap_attr(grp, vpss_chn, &wrap_attr[vpss_chn]);
            if (ret != HI_SUCCESS) {
                sample_print("hi_mpi_vpss_set_chn_buf_wrap failed with %#x\n", ret);
                goto disable_chn;
            }
            ret = hi_mpi_vpss_enable_chn(grp, vpss_chn);
            if (ret != HI_SUCCESS) {
                sample_print("hi_mpi_vpss_enable_chn failed with %#x\n", ret);
                goto disable_chn;
            }
        }
    }
    return HI_SUCCESS;

disable_chn:
    for (i = i - 1; i >= 0; i--) {
        if (chn_enable[i] == HI_TRUE) {
            vpss_chn = i;
            ret = hi_mpi_vpss_disable_chn(grp, vpss_chn);
            if (ret != HI_SUCCESS) {
                sample_print("hi_mpi_vpss_disable_chn failed with %#x!\n", ret);
            }
        }
    }
    return HI_FAILURE;
}

hi_s32 sample_common_vpss_start(hi_vpss_grp grp, const hi_vpss_grp_attr *grp_attr,
    const sample_vpss_chn_attr *vpss_chn_attr)
{
    hi_s32 ret;

    if (vpss_chn_attr->chn_array_size < HI_VPSS_MAX_PHYS_CHN_NUM) {
        sample_print("array size(%u) of chn_enable and chn_attr need >= %u!\n",
            vpss_chn_attr->chn_array_size, HI_VPSS_MAX_PHYS_CHN_NUM);
        return HI_FAILURE;
    }

    ret = hi_mpi_vpss_create_grp(grp, grp_attr);
    if (ret != HI_SUCCESS) {
        sample_print("hi_mpi_vpss_create_grp(grp:%d) failed with %#x!\n", grp, ret);
        return HI_FAILURE;
    }

    ret = hi_mpi_vpss_start_grp(grp);
    if (ret != HI_SUCCESS) {
        sample_print("hi_mpi_vpss_start_grp failed with %#x\n", ret);
        goto destroy_grp;
    }

    ret = sample_common_vpss_start_chn(grp, vpss_chn_attr->chn_enable, &vpss_chn_attr->chn_attr[0],
        &vpss_chn_attr->wrap_attr[0], HI_VPSS_MAX_PHYS_CHN_NUM);
    if (ret != HI_SUCCESS) {
        goto stop_grp;
    }

    return HI_SUCCESS;

stop_grp:
    ret = hi_mpi_vpss_stop_grp(grp);
    if (ret != HI_SUCCESS) {
        sample_print("hi_mpi_vpss_stop_grp failed with %#x!\n", ret);
    }
destroy_grp:
    ret = hi_mpi_vpss_destroy_grp(grp);
    if (ret != HI_SUCCESS) {
        sample_print("hi_mpi_vpss_destroy_grp failed with %#x!\n", ret);
    }
    return HI_FAILURE;
}

hi_s32 sample_common_vpss_stop(hi_vpss_grp grp, const hi_bool *chn_enable, hi_u32 chn_array_size)
{
    hi_s32 i;
    hi_s32 ret;
    hi_vpss_chn vpss_chn;

    if (chn_array_size < HI_VPSS_MAX_PHYS_CHN_NUM) {
        sample_print("array size(%u) of chn_enable need > %u!\n", chn_array_size, HI_VPSS_MAX_PHYS_CHN_NUM);
        return HI_FAILURE;
    }

    for (i = 0; i < HI_VPSS_MAX_PHYS_CHN_NUM; ++i) {
        if (chn_enable[i] == HI_TRUE) {
            vpss_chn = i;
            ret = hi_mpi_vpss_disable_chn(grp, vpss_chn);
            if (ret != HI_SUCCESS) {
                sample_print("hi_mpi_vpss_disable_chn failed with %#x!\n", ret);
            }
        }
    }

    ret = hi_mpi_vpss_stop_grp(grp);
    if (ret != HI_SUCCESS) {
        sample_print("hi_mpi_vpss_stop_grp failed with %#x!\n", ret);
    }

    ret = hi_mpi_vpss_destroy_grp(grp);
    if (ret != HI_SUCCESS) {
        sample_print("hi_mpi_vpss_destroy_grp failed with %#x!\n", ret);
    }

    return HI_SUCCESS;
}
