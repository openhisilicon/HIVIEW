/*
  Copyright (c), 2001-2022, Shenshu Tech. Co., Ltd.
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

#define VPSS_DEFAULT_WIDTH  3840
#define VPSS_DEFAULT_HEIGHT 2160

hi_void sample_comm_vpss_get_default_grp_attr(hi_vpss_grp_attr *grp_attr)
{
    grp_attr->nr_en                     = HI_TRUE;
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
    grp_attr->nr_attr.nr_type           = HI_VPSS_NR_TYPE_VIDEO_NORM;
    grp_attr->nr_attr.compress_mode     = HI_COMPRESS_MODE_FRAME;
    grp_attr->nr_attr.nr_motion_mode    = HI_VPSS_NR_MOTION_MODE_NORM;
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
    chn_attr->compress_mode             = HI_COMPRESS_MODE_SEG;
    chn_attr->aspect_ratio.mode         = HI_ASPECT_RATIO_NONE;
    chn_attr->frame_rate.src_frame_rate = -1;
    chn_attr->frame_rate.dst_frame_rate = -1;
}

static hi_s32 sample_common_vpss_start_chn(hi_vpss_grp grp, const hi_bool *chn_enable,
                                           const hi_vpss_chn_attr *chn_attr, hi_u32 chn_array_size)
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

hi_s32 sample_common_vpss_start(hi_vpss_grp grp, const hi_bool *chn_enable,
    const hi_vpss_grp_attr *grp_attr, const hi_vpss_chn_attr *chn_attr, hi_u32 chn_array_size)
{
    hi_s32 ret;
    if (chn_array_size < HI_VPSS_MAX_PHYS_CHN_NUM) {
        sample_print("array size(%u) of chn_enable and chn_attr need > %u!\n",
            chn_array_size, HI_VPSS_MAX_PHYS_CHN_NUM);
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

    ret = sample_common_vpss_start_chn(grp, chn_enable, chn_attr, HI_VPSS_MAX_PHYS_CHN_NUM);
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
