/*
 * Copyright (c) Hisilicon Technologies Co., Ltd. 2019-2020. All rights reserved.
 * Description: vpss common function for sample.
 * Author: Hisilicon multimedia software group
 * Create: 2019/11/29
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

#define SAMPLE_VPSS_DEFAULT_BORDER_THICK 2
#define SAMPLE_VPSS_DEFAULT_CHN_WIDTH 960
#define SAMPLE_VPSS_DEFAULT_CHN_HEIGHT 576

/* start vpss */
hi_s32 sample_common_vpss_start(hi_vpss_grp grp, const hi_bool *chn_enable,
    const hi_vpss_grp_attr *grp_attr, const hi_vpss_chn_attr *chn_attr, hi_u32 chn_array_size)
{
    hi_vpss_chn vpss_chn;
    hi_s32 ret;
    hi_s32 i;

    if (chn_array_size < HI_VPSS_MAX_PHYS_CHN_NUM) {
        SAMPLE_PRT("array size(%u) of chn_enable and chn_attr need > %u!\n", chn_array_size, HI_VPSS_MAX_PHYS_CHN_NUM);
        return HI_FAILURE;
    }

    ret = hi_mpi_vpss_create_grp(grp, grp_attr);
    if (ret != HI_SUCCESS) {
        SAMPLE_PRT("hi_mpi_vpss_create_grp(grp:%d) failed with %#x!\n", grp, ret);
        return HI_FAILURE;
    }

    ret = hi_mpi_vpss_start_grp(grp);
    if (ret != HI_SUCCESS) {
        SAMPLE_PRT("hi_mpi_vpss_start_grp failed with %#x\n", ret);
        return HI_FAILURE;
    }

    for (i = 0; i < HI_VPSS_MAX_PHYS_CHN_NUM; ++i) {
        if (chn_enable[i] == HI_TRUE) {
            vpss_chn = i;
            ret = hi_mpi_vpss_set_chn_attr(grp, vpss_chn, &chn_attr[vpss_chn]);
            if (ret != HI_SUCCESS) {
                SAMPLE_PRT("hi_mpi_vpss_set_chn_attr failed with %#x\n", ret);
                return HI_FAILURE;
            }

            ret = hi_mpi_vpss_enable_chn(grp, vpss_chn);
            if (ret != HI_SUCCESS) {
                SAMPLE_PRT("hi_mpi_vpss_enable_chn failed with %#x\n", ret);
                return HI_FAILURE;
            }
        }
    }
    return HI_SUCCESS;
}

/* stop vpss */
hi_s32 sample_common_vpss_stop(hi_vpss_grp grp, const hi_bool *chn_enable, hi_u32 chn_array_size)
{
    hi_s32 i;
    hi_s32 ret;
    hi_vpss_chn vpss_chn;

    if (chn_array_size < HI_VPSS_MAX_PHYS_CHN_NUM) {
        SAMPLE_PRT("array size(%u) of chn_enable need > %u!\n", chn_array_size, HI_VPSS_MAX_PHYS_CHN_NUM);
        return HI_FAILURE;
    }

    for (i = 0; i < HI_VPSS_MAX_PHYS_CHN_NUM; ++i) {
        if (chn_enable[i] == HI_TRUE) {
            vpss_chn = i;
            ret = hi_mpi_vpss_disable_chn(grp, vpss_chn);
            if (ret != HI_SUCCESS) {
                SAMPLE_PRT("failed with %#x!\n", ret);
                return HI_FAILURE;
            }
        }
    }

    ret = hi_mpi_vpss_stop_grp(grp);
    if (ret != HI_SUCCESS) {
        SAMPLE_PRT("failed with %#x!\n", ret);
        return HI_FAILURE;
    }

    ret = hi_mpi_vpss_destroy_grp(grp);
    if (ret != HI_SUCCESS) {
        SAMPLE_PRT("failed with %#x!\n", ret);
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

hi_s32 sample_common_vpss_start_vpss_user(hi_s32 grp_cnt, hi_s32 chn_cnt, const hi_size *size)
{
    hi_s32 ret;
    hi_vpss_grp vpss_grp;
    hi_vpss_chn vpss_chn;
    hi_vpss_grp_attr grp_attr = {0};
    hi_vpss_chn_attr chn_attr[HI_VPSS_MAX_PHYS_CHN_NUM] = {0};
    hi_bool chn_enable[HI_VPSS_MAX_PHYS_CHN_NUM] = {0};

    grp_attr.max_width = size->width;
    grp_attr.max_height = size->height;
    grp_attr.nr_en = HI_FALSE;
    grp_attr.dei_mode = HI_VPSS_DEI_MODE_OFF;
    grp_attr.pixel_format = SAMPLE_PIXEL_FORMAT;
    grp_attr.frame_rate.src_frame_rate = -1;
    grp_attr.frame_rate.dst_frame_rate = -1;

    for (vpss_chn = 0; vpss_chn < chn_cnt; ++vpss_chn) {
        chn_enable[vpss_chn] = HI_TRUE;
        chn_attr[vpss_chn].border_en = HI_TRUE;
        chn_attr[vpss_chn].chn_mode = HI_VPSS_CHN_MODE_USER;
        chn_attr[vpss_chn].pixel_format = HI_PIXEL_FORMAT_YVU_SEMIPLANAR_420;
        chn_attr[vpss_chn].width = SAMPLE_VPSS_DEFAULT_CHN_WIDTH;
        chn_attr[vpss_chn].height = SAMPLE_VPSS_DEFAULT_CHN_HEIGHT;
        chn_attr[vpss_chn].frame_rate.src_frame_rate = -1;
        chn_attr[vpss_chn].frame_rate.dst_frame_rate = -1;
        chn_attr[vpss_chn].compress_mode = HI_COMPRESS_MODE_NONE;

        chn_attr[vpss_chn].border_attr.color = COLOR_RGB_WHITE;
        chn_attr[vpss_chn].border_attr.top_width = SAMPLE_VPSS_DEFAULT_BORDER_THICK;
        chn_attr[vpss_chn].border_attr.bottom_width = SAMPLE_VPSS_DEFAULT_BORDER_THICK;
        chn_attr[vpss_chn].border_attr.left_width = SAMPLE_VPSS_DEFAULT_BORDER_THICK;
        chn_attr[vpss_chn].border_attr.right_width = SAMPLE_VPSS_DEFAULT_BORDER_THICK;
    }

    for (vpss_grp = 0; vpss_grp < grp_cnt; ++vpss_grp) {
        ret = sample_common_vpss_start(vpss_grp, chn_enable, &grp_attr, chn_attr, HI_VPSS_MAX_PHYS_CHN_NUM);
        if (ret != HI_SUCCESS) {
            SAMPLE_PRT("failed with %#x!\n", ret);
            return HI_FAILURE;
        }
    }

    return HI_SUCCESS;
}
