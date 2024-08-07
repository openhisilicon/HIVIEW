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
    chn_attr->compress_mode             = HI_COMPRESS_MODE_SEG;
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

hi_void sample_comm_vpss_get_default_vpss_cfg(sample_vpss_cfg *vpss_cfg, hi_fmu_mode vpss_fmu)
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

    if (vpss_fmu == HI_FMU_MODE_WRAP) {
        vpss_cfg->chn_en[1] = HI_TRUE; /* vpss_chn0->vnec & vpss_chn1->vo */
    }
}

static hi_s32 sample_common_vpss_set_chn_fmu_mode(hi_vpss_grp grp, hi_vpss_chn chn, hi_bool chn0_wrap)
{
    hi_s32 ret = HI_SUCCESS;
    if (chn == HI_VPSS_CHN0 && chn0_wrap == HI_TRUE) {
        ret = hi_mpi_vpss_set_chn_fmu_mode(grp, HI_VPSS_CHN0, HI_FMU_MODE_WRAP);
    }
    return ret;
}

static hi_s32 sample_common_vpss_start_chn(hi_vpss_grp grp, const hi_bool *chn_enable,
    const hi_vpss_chn_attr *chn_attr, hi_bool chn0_wrap, hi_u32 chn_array_size)
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
            /* set chn0 wrap fmu mode first, then enable chn */
            ret = sample_common_vpss_set_chn_fmu_mode(grp, vpss_chn, chn0_wrap);
            if (ret != HI_SUCCESS) {
                sample_print("hi_mpi_vpss_set_chn_fmu_mode failed with %#x\n", ret);
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
        vpss_chn_attr->chn0_wrap, HI_VPSS_MAX_PHYS_CHN_NUM);
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
