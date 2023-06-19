/*
  Copyright (c), 2001-2022, Shenshu Tech. Co., Ltd.
 */
#include "sample_hnr_mcf_common.h"
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

hi_bool g_vpss_chn_enable[HI_VPSS_MAX_PHYS_CHN_NUM] = {HI_TRUE, HI_TRUE, HI_FALSE};
#define MCF_VO_WIDHT_1920 1920
#define MCF_VO_HEIGHT_1080 1080

hi_vb_pool g_pool_id_color;
hi_vb_pool g_pool_id_mono;

hi_s32 sample_hnr_common_vi_bind_mcf(hi_vi_pipe vi_pipe, hi_vi_chn vi_chn, hi_mcf_grp mcf_grp, hi_mcf_pipe mcf_pipe)
{
    hi_mpp_chn src_chn;
    hi_mpp_chn dest_chn;

    src_chn.mod_id = HI_ID_VI;
    src_chn.dev_id = vi_pipe;
    src_chn.chn_id = vi_chn;

    dest_chn.mod_id = HI_ID_MCF;
    dest_chn.dev_id = mcf_grp;
    dest_chn.chn_id = mcf_pipe;

    check_return(hi_mpi_sys_bind(&src_chn, &dest_chn), "hi_mpi_sys_bind(VI-MCF)");

    return HI_SUCCESS;
}

hi_s32 sample_hnr_common_vi_unbind_mcf(hi_vi_pipe vi_pipe, hi_vi_chn vi_chn, hi_mcf_grp mcf_grp, hi_mcf_pipe mcf_pipe)
{
    hi_mpp_chn src_chn;
    hi_mpp_chn dest_chn;

    src_chn.mod_id = HI_ID_VI;
    src_chn.dev_id = vi_pipe;
    src_chn.chn_id = vi_chn;

    dest_chn.mod_id = HI_ID_MCF;
    dest_chn.dev_id = mcf_grp;
    dest_chn.chn_id = mcf_pipe;

    check_return(hi_mpi_sys_unbind(&src_chn, &dest_chn), "hi_mpi_sys_unbind(VI-MCF)");

    return HI_SUCCESS;
}

hi_s32 sample_hnr_common_mcf_bind_vpss(hi_mcf_grp mcf_grp, hi_mcf_chn mcf_chn, hi_vpss_grp vpss_grp,
    hi_vpss_chn vpss_chn)
{
    hi_mpp_chn src_chn;
    hi_mpp_chn dest_chn;

    src_chn.mod_id = HI_ID_MCF;
    src_chn.dev_id = mcf_grp;
    src_chn.chn_id = mcf_chn;

    dest_chn.mod_id = HI_ID_VPSS;
    dest_chn.dev_id = vpss_grp;
    dest_chn.chn_id = vpss_chn;

    check_return(hi_mpi_sys_bind(&src_chn, &dest_chn), "hi_mpi_sys_bind(VI-VPSS)");

    return HI_SUCCESS;
}

hi_s32 sample_hnr_common_mcf_unbind_vpss(hi_mcf_grp mcf_grp, hi_mcf_chn mcf_chn, hi_vpss_grp vpss_grp,
    hi_vpss_chn vpss_chn)
{
    hi_mpp_chn src_chn;
    hi_mpp_chn dest_chn;

    src_chn.mod_id = HI_ID_MCF;
    src_chn.dev_id = mcf_grp;
    src_chn.chn_id = mcf_chn;

    dest_chn.mod_id = HI_ID_VPSS;
    dest_chn.chn_id = vpss_chn;
    dest_chn.dev_id = vpss_grp;

    check_return(hi_mpi_sys_unbind(&src_chn, &dest_chn), "hi_mpi_sys_unbind(VI-VPSS)");

    return HI_SUCCESS;
}

hi_void sample_hnr_common_mcf_get_default_grp_attr(hi_mcf_grp_attr *grp_attr, hi_size *mono_size, hi_size *color_size)
{
    grp_attr->sync_pipe                 = HI_TRUE;
    grp_attr->mono_pipe_attr.pipe_id    = 0;
    if ((mono_size->width >= 3840) && (mono_size->height >= 2160)) { /* 3840:width, 2160: height */
        grp_attr->mono_pipe_attr.vpss_grp   = 100; /* vgs group num 100 */
    } else {
        grp_attr->mono_pipe_attr.vpss_grp   = 0;
    }
    grp_attr->mono_pipe_attr.width      = mono_size->width;
    grp_attr->mono_pipe_attr.height     = mono_size->height;

    grp_attr->color_pipe_attr.pipe_id    = 1;
    grp_attr->color_pipe_attr.vpss_grp   = 1;
    grp_attr->color_pipe_attr.width      = color_size->width;
    grp_attr->color_pipe_attr.height     = color_size->height;

    grp_attr->frame_rate.src_frame_rate = -1;
    grp_attr->frame_rate.dst_frame_rate = -1;

    grp_attr->depth = 0;
    grp_attr->mcf_path = OT_MCF_PATH_FUSION;
}

hi_void sample_common_mcf_get_default_chn_attr(hi_mcf_chn_attr *chn_attr, hi_size *out_size)
{
    chn_attr->mirror_en                 = HI_FALSE;
    chn_attr->flip_en                   = HI_FALSE;
    chn_attr->width                     = out_size->width;
    chn_attr->height                    = out_size->height;
    chn_attr->depth                     = 0;
    chn_attr->pixel_format              = HI_PIXEL_FORMAT_YVU_SEMIPLANAR_420;
    chn_attr->compress_mode             = HI_COMPRESS_MODE_NONE;
    chn_attr->frame_rate.src_frame_rate = -1;
    chn_attr->frame_rate.dst_frame_rate = -1;
}

hi_void sample_common_mcf_set_default_vpss_attr(hi_vpss_grp_attr *vpss_grp_attr,
                                                const hi_mcf_grp_attr *mcf_grp_attr, hi_bool is_mono_pipe)
{
    hi_u32 mono_pipe_width, mono_pipe_height, color_pipe_width, color_pipe_height;

    mono_pipe_width = mcf_grp_attr->mono_pipe_attr.width;
    mono_pipe_height = mcf_grp_attr->mono_pipe_attr.height;
    color_pipe_width = mcf_grp_attr->color_pipe_attr.width;
    color_pipe_height = mcf_grp_attr->color_pipe_attr.height;

    (hi_void)memset_s(vpss_grp_attr, sizeof(hi_vpss_grp_attr), 0, sizeof(hi_vpss_grp_attr));
    vpss_grp_attr->ie_en                     = HI_FALSE;
    vpss_grp_attr->dci_en                    = HI_FALSE;
    vpss_grp_attr->buf_share_en              = HI_FALSE;
    vpss_grp_attr->mcf_en                    = HI_TRUE;
    vpss_grp_attr->max_dei_width             = 0;
    vpss_grp_attr->max_dei_height            = 0;
    vpss_grp_attr->dynamic_range             = HI_DYNAMIC_RANGE_SDR8;
    vpss_grp_attr->dei_mode                  = HI_VPSS_DEI_MODE_OFF;
    vpss_grp_attr->buf_share_chn             = HI_VPSS_CHN0;
    vpss_grp_attr->nr_attr.nr_type           = HI_VPSS_NR_TYPE_VIDEO_NORM;
    vpss_grp_attr->nr_attr.compress_mode     = HI_COMPRESS_MODE_FRAME;
    vpss_grp_attr->nr_attr.nr_motion_mode    = HI_VPSS_NR_MOTION_MODE_NORM;
    vpss_grp_attr->frame_rate.src_frame_rate = -1;
    vpss_grp_attr->frame_rate.dst_frame_rate = -1;
    if ((is_mono_pipe == HI_TRUE) &&
        (mono_pipe_width >= 3840) && (mono_pipe_height >= 2160)) { /* width 3840, height:2160 */
        vpss_grp_attr->nr_en        = HI_FALSE;
    } else {
        vpss_grp_attr->nr_en       = HI_TRUE;
    }
    if (is_mono_pipe == HI_TRUE) {
        vpss_grp_attr->max_width    = mono_pipe_width;
        vpss_grp_attr->max_height   = mono_pipe_height;
        vpss_grp_attr->pixel_format = HI_PIXEL_FORMAT_YUV_400;
    } else {
        vpss_grp_attr->max_width    = color_pipe_width;
        vpss_grp_attr->max_height   = color_pipe_height;
        vpss_grp_attr->pixel_format = HI_PIXEL_FORMAT_YVU_SEMIPLANAR_420;
        vpss_grp_attr->nr_en        = HI_TRUE;
    }
}

hi_s32 sample_hnr_common_mcf_start_vpss(const hi_mcf_grp_attr *mcf_grp_attr)
{
    hi_vpss_grp vpss_grp[HI_MCF_PIPE_NUM];
    hi_vpss_grp_attr vpss_grp_attr[HI_MCF_PIPE_NUM];
    hi_s32 i, j, k;
    hi_s32 ret;
    hi_vpss_grp_attr *grp_attr = HI_NULL;
    grp_attr = &vpss_grp_attr[0];
    sample_common_mcf_set_default_vpss_attr(grp_attr, mcf_grp_attr, HI_TRUE);
    grp_attr = &vpss_grp_attr[1];
    sample_common_mcf_set_default_vpss_attr(grp_attr, mcf_grp_attr, HI_FALSE);

    vpss_grp[0] = mcf_grp_attr->mono_pipe_attr.vpss_grp;
    vpss_grp[1] = mcf_grp_attr->color_pipe_attr.vpss_grp;

    for (i = 0; i < HI_MCF_PIPE_NUM; i++) {
        ret = hi_mpi_vpss_create_grp(vpss_grp[i], &vpss_grp_attr[i]);
        if (ret != HI_SUCCESS) {
            sample_print("VPSS Create Grp %d,fail! 0x%x\n", vpss_grp[i], ret);
            goto destroy_grp;
        }
    }

    for (k = 0; k < HI_MCF_PIPE_NUM; k++) {
        ret =  hi_mpi_vpss_start_grp(vpss_grp[k]);
        if (ret != HI_SUCCESS) {
            sample_print("VPSS Set Start Grp %d fail! 0x%x\n", vpss_grp[k], ret);
            goto stop_grp;
        }
    }

    return HI_SUCCESS;
stop_grp:
    for (j = k; j > 0; j--) {
        ret = hi_mpi_vpss_stop_grp(j);
        if (ret != HI_SUCCESS) {
            sample_print("hi_mpi_mcf_destroy_grp failed with %#x!\n", ret);
        }
    }

destroy_grp:
    for (j = i; j > 0; j--) {
        ret = hi_mpi_vpss_destroy_grp(j);
        if (ret != HI_SUCCESS) {
            sample_print("hi_mpi_mcf_destroy_grp failed with %#x!\n", ret);
        }
    }
    return HI_FAILURE;
}

hi_s32 sample_hnr_common_mcf_stop_vpss(const hi_mcf_grp_attr *grp_attr)
{
    hi_vpss_grp grp[HI_MCF_PIPE_NUM];
    hi_s32 i;
    hi_s32 ret;
    grp[0] = grp_attr->mono_pipe_attr.vpss_grp;
    grp[1] = grp_attr->color_pipe_attr.vpss_grp;

    for (i = 0; i < HI_MCF_PIPE_NUM; i++) {
        ret =  hi_mpi_vpss_stop_grp(grp[i]);
        if (ret != HI_SUCCESS) {
            sample_print("VPSS Set stop Grp %d fail! 0x%x\n", grp[i], ret);
        }
    }

    for (i = 0; i < HI_MCF_PIPE_NUM; i++) {
        ret = hi_mpi_vpss_destroy_grp(grp[i]);
        if (ret != HI_SUCCESS) {
            sample_print("VPSS Destroy Grp %d fail! 0x%x\n", grp[i], ret);
        }
    }

    return HI_SUCCESS;
}

static hi_s32 sample_common_mcf_enable_chn(hi_mcf_grp grp, hi_bool *mcf_chn_enable,
    hi_size *chn_out_size)
{
    hi_s32 i, ret;
    hi_mcf_chn mcf_chn;
    hi_mcf_chn_attr chn_attr;
    for (i = 0; i < HI_MCF_MAX_PHYS_CHN_NUM; ++i) {
        if (mcf_chn_enable[i] == HI_TRUE) {
            mcf_chn = i;
            sample_common_mcf_get_default_chn_attr(&chn_attr, &chn_out_size[mcf_chn]);
            ret = hi_mpi_mcf_set_chn_attr(grp, mcf_chn, &chn_attr);
            if (ret != HI_SUCCESS) {
                sample_print("hi_mpi_mcf_set_chn_attr failed with %#x\n", ret);
                goto disable_chn;
            }

            ret = hi_mpi_mcf_enable_chn(grp, mcf_chn);
            if (ret != HI_SUCCESS) {
                sample_print("hi_mpi_mcf_enable_chn failed with %#x\n", ret);
                goto disable_chn;
            }
        }
    }
    return HI_SUCCESS;

disable_chn:
    i--;
    for (; i >= 0; i--) {
        if (mcf_chn_enable[i] == HI_TRUE) {
            mcf_chn = i;
            ret = hi_mpi_mcf_disable_chn(grp, mcf_chn);
            if (ret != HI_SUCCESS) {
                sample_print("hi_mpi_mcf_disable_chn failed with %#x!\n", ret);
            }
        }
    }
    return HI_FAILURE;
}

hi_s32 sample_hnr_common_mcf_start(hi_mcf_grp grp, const hi_mcf_grp_attr *grp_attr, hi_crop_info *grp_crop_info,
    hi_bool *mcf_chn_en, hi_size *chn_out_size)
{
    hi_s32 ret;

    ret = hi_mpi_mcf_create_grp(grp, grp_attr);
    if (ret != HI_SUCCESS) {
        sample_print("hi_mpi_mcf_create_grp(grp:%d) failed with %#x!\n", grp, ret);
        return HI_FAILURE;
    }
    if (grp_crop_info != HI_NULL) {
        ret = hi_mpi_mcf_set_grp_crop(grp, grp_crop_info);
        if (ret != HI_SUCCESS) {
            sample_print("hi_mpi_mcf_set_grp_crop failed with %#x\n", ret);
            goto destroy_grp;
        }
    }
    ret = hi_mpi_mcf_start_grp(grp);
    if (ret != HI_SUCCESS) {
        sample_print("hi_mpi_mcf_start_grp failed with %#x\n", ret);
        goto destroy_grp;
    }

    ret = sample_common_mcf_enable_chn(grp, mcf_chn_en, chn_out_size);
    if (ret != HI_SUCCESS) {
        sample_print("sample_common_mcf_enable_chn failed with %#x\n", ret);
        goto stop_grp;
    }
    return HI_SUCCESS;

stop_grp:
    ret = hi_mpi_mcf_stop_grp(grp);
    if (ret != HI_SUCCESS) {
        sample_print("hi_mpi_mcf_stop_grp failed with %#x\n", ret);
    }

destroy_grp:
    ret = hi_mpi_mcf_destroy_grp(grp);
    if (ret != HI_SUCCESS) {
        sample_print("hi_mpi_mcf_destroy_grp failed with %#x!\n", ret);
    }

    return HI_FAILURE;
}

hi_s32 sample_hnr_common_mcf_stop(hi_mcf_grp grp, const hi_bool *chn_enable, hi_u32 chn_array_size)
{
    hi_s32 i;
    hi_s32 ret;
    hi_mcf_chn mcf_chn;

    if (chn_array_size < HI_MCF_MAX_PHYS_CHN_NUM) {
        sample_print("array size(%u) of chn_enable need > %u!\n", chn_array_size, HI_MCF_MAX_PHYS_CHN_NUM);
        return HI_FAILURE;
    }

    for (i = 0; i < HI_MCF_MAX_PHYS_CHN_NUM; ++i) {
        if (chn_enable[i] == HI_TRUE) {
            mcf_chn = i;
            ret = hi_mpi_mcf_disable_chn(grp, mcf_chn);
            if (ret != HI_SUCCESS) {
                sample_print("hi_mpi_mcf_disable_chn failed with %#x!\n", ret);
            }
        }
    }
    ret = hi_mpi_mcf_stop_grp(grp);
    if (ret != HI_SUCCESS) {
        sample_print("hi_mpi_mcf_stop_grp failed with %#x!\n", ret);
    }
    ret = hi_mpi_mcf_destroy_grp(grp);
    if (ret != HI_SUCCESS) {
        sample_print("hi_mpi_mcf_destroy_grp failed with %#x!\n", ret);
    }
    return HI_SUCCESS;
}

/* function for hnr */
hi_s32 sample_hnr_mcf_check_size(hi_size mono_size, hi_size color_size)
{
    if (mono_size.width != color_size.width || mono_size.height != color_size.height) {
        sample_print(" mono_size and color_size is not same!\n");
        return HI_FAILURE;
    }
    return HI_SUCCESS;
}

hi_void sample_hnr_mcf_start_mcf_vi_cfg(hi_mcf_grp mcf_grp, const hi_mcf_grp_attr *mcf_grp_attr)
{
    hi_mcf_vi_attr mcf_vi_attr;
    hi_s32 ret;
    mcf_vi_attr.enable     = HI_TRUE;
    mcf_vi_attr.mono_pipe  = mcf_grp_attr->mono_pipe_attr.pipe_id;
    mcf_vi_attr.color_pipe = mcf_grp_attr->color_pipe_attr.pipe_id;
    ret = hi_mpi_mcf_set_vi_attr(mcf_grp, &mcf_vi_attr);
    if (ret != HI_SUCCESS) {
        printf("hi_mpi_mcf_set_vi_attr error\n");
    }
}

hi_void sample_hnr_mcf_stop_mcf_vi_cfg(hi_mcf_grp mcf_grp)
{
    hi_mcf_vi_attr mcf_vi_attr;
    hi_s32 ret;
    ret = hi_mpi_mcf_get_vi_attr(mcf_grp, &mcf_vi_attr);
    if (ret != HI_SUCCESS) {
        printf("hi_mpi_mcf_get_vi_attr err\n");
        return;
    }
    mcf_vi_attr.enable = HI_FALSE;
    ret = hi_mpi_mcf_set_vi_attr(mcf_grp, &mcf_vi_attr);
    if (ret != HI_SUCCESS) {
        printf("hi_mpi_mcf_set_vi_attr error\n");
    }
}

hi_s32 sample_hnr_mcf_start_vpss(hi_vpss_grp grp, hi_size *in_size)
{
    hi_vpss_grp_attr grp_attr = {0};
    hi_vpss_chn_attr chn_attr[HI_VPSS_MAX_PHYS_CHN_NUM] = {0};
    hi_vpss_chn_attr *attr = HI_NULL;
    sample_comm_vpss_get_default_grp_attr(&grp_attr);
    grp_attr.max_width  = in_size->width;
    grp_attr.max_height = in_size->height;
    grp_attr.mcf_en     = HI_FALSE;
    attr = &chn_attr[0];
    sample_comm_vpss_get_default_chn_attr(attr);
    chn_attr[0].width  = in_size->width;
    chn_attr[0].height = in_size->height;
    attr = &chn_attr[1];
    sample_comm_vpss_get_default_chn_attr(attr);
    chn_attr[1].width  = MCF_VO_WIDHT_1920;
    chn_attr[1].height = MCF_VO_HEIGHT_1080;
    chn_attr[1].compress_mode = HI_COMPRESS_MODE_NONE;
    return sample_common_vpss_start(grp, g_vpss_chn_enable, &grp_attr, &chn_attr[0], HI_VPSS_MAX_PHYS_CHN_NUM);
}

hi_void sample_hnr_mcf_stop_vpss(hi_vpss_grp grp)
{
    sample_common_vpss_stop(grp, g_vpss_chn_enable, HI_VPSS_MAX_PHYS_CHN_NUM);
}

/* bind */
hi_void sample_hnr_mcf_bind_vi(hi_mcf_grp mcf_grp, const hi_mcf_grp_attr *mcf_grp_attr, hi_vi_chn vi_chn)
{
    hi_mcf_pipe mcf_color_pipe, mcf_mono_pipe;
    hi_vi_pipe vi_mono_pipe, vi_color_pipe;

    vi_mono_pipe = mcf_grp_attr->mono_pipe_attr.pipe_id;
    vi_color_pipe = mcf_grp_attr->color_pipe_attr.pipe_id;

    mcf_mono_pipe = mcf_grp_attr->mono_pipe_attr.pipe_id;
    mcf_color_pipe = mcf_grp_attr->color_pipe_attr.pipe_id;

    sample_hnr_common_vi_bind_mcf(vi_mono_pipe, vi_chn, mcf_grp, mcf_mono_pipe);
    sample_hnr_common_vi_bind_mcf(vi_color_pipe, vi_chn, mcf_grp, mcf_color_pipe);
}

hi_void sample_hnr_mcf_unbind_vi(hi_mcf_grp mcf_grp, const hi_mcf_grp_attr *mcf_grp_attr, hi_vi_chn vi_chn)
{
    hi_mcf_pipe mcf_color_pipe, mcf_mono_pipe;
    hi_vi_pipe vi_mono_pipe, vi_color_pipe;

    vi_mono_pipe = mcf_grp_attr->mono_pipe_attr.pipe_id;
    vi_color_pipe = mcf_grp_attr->color_pipe_attr.pipe_id;

    mcf_mono_pipe = mcf_grp_attr->mono_pipe_attr.pipe_id;
    mcf_color_pipe = mcf_grp_attr->color_pipe_attr.pipe_id;

    sample_hnr_common_vi_unbind_mcf(vi_mono_pipe, vi_chn, mcf_grp, mcf_mono_pipe);
    sample_hnr_common_vi_unbind_mcf(vi_color_pipe, vi_chn, mcf_grp, mcf_color_pipe);
}
