/*
  Copyright (c), 2001-2022, Shenshu Tech. Co., Ltd.
 */

#include "sample_comm.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <signal.h>


static hi_mpp_chn g_sample_mpp_chn[] = {
    {HI_ID_VI, HI_VI_MAX_DEV_NUM, HI_VI_MAX_CHN_NUM},
    {HI_ID_VPSS, HI_VPSS_MAX_GRP_NUM, 1},
    {HI_ID_VENC, 1, HI_VENC_MAX_CHN_NUM},
    {HI_ID_VO, HI_VO_MAX_LAYER_NUM, HI_VO_MAX_CHN_NUM},
    {HI_ID_VDEC, 1, HI_VDEC_MAX_CHN_NUM}
};

/* The order of g_sample_pic_size's element must be consistent with the enum value defined in "hi_pic_size". */
static hi_size g_sample_pic_size[PIC_BUTT] = {
    { 352,  288  },  /* PIC_CIF */
    { 640,  360  },  /* PIC_360P */
    { 640,  512  },  /* PIC_512P maohw */
    { 640,  640  },  /* PIC_640P maohw */
    { 720,  576  },  /* PIC_D1_PAL */
    { 720,  480  },  /* PIC_D1_NTSC */
    { 960,  576  },  /* PIC_960H */
    { 1280, 720  },  /* PIC_720P */
    { 1920, 1080 },  /* PIC_1080P */
    { 720,  480  },  /* PIC_480P */
    { 720,  576  },  /* PIC_576P */
    { 800,  600  },  /* PIC_800X600 */
    { 1024, 768  },  /* PIC_1024X768 */
    { 1280, 1024 },  /* PIC_1280X1024 */
    { 1366, 768  },  /* PIC_1366X768 */
    { 1440, 900  },  /* PIC_1440X900 */
    { 1280, 800  },  /* PIC_1280X800 */
    { 1600, 1200 },  /* PIC_1600X1200 */
    { 1680, 1050 },  /* PIC_1680X1050 */
    { 1920, 1200 },  /* PIC_1920X1200 */
    { 640,  480  },  /* PIC_640X480 */
    { 1920, 2160 },  /* PIC_1920X2160 */
    { 2560, 1440 },  /* PIC_2560X1440 */
    { 2560, 1600 },  /* PIC_2560X1600 */
    { 2592, 1520 },  /* PIC_2592X1520 */
    { 2592, 1944 },  /* PIC_2592X1944 */
    { 2688, 1520 },  /* PIC_2688X1520 */
    { 3840, 2160 },  /* PIC_3840X2160 */
    { 4096, 2160 },  /* PIC_4096X2160 */
    { 3000, 3000 },  /* PIC_3000X3000 */
    { 4000, 3000 },  /* PIC_4000X3000 */
    { 6080, 2800 },  /* PIC_6080X2800 */
    { 7680, 4320 },  /* PIC_7680X4320 */
    { 3840, 8640 },  /* PIC_3840X8640 */
    { 8000, 6000 }   /* PIC_8000X6000 */
};

#ifndef __LITEOS__
hi_void sample_sys_signal(void (*func)(int))
{
    struct sigaction sa = { 0 };

    sa.sa_handler = func;
    sa.sa_flags = 0;
    sigaction(SIGINT, &sa, HI_NULL);
    sigaction(SIGTERM, &sa, HI_NULL);
}
#endif

/* get picture size(w*h), according pic_size */
hi_s32 sample_comm_sys_get_pic_size(hi_pic_size pic_size, hi_size *size)
{
    if (size == HI_NULL) {
        sample_print("null ptr arg!\n");
        return HI_FAILURE;
    }

    if (pic_size >= PIC_BUTT) {
        sample_print("illegal pic_size!\n");
        return HI_FAILURE;
    }

    size->width = g_sample_pic_size[pic_size].width;
    size->height = g_sample_pic_size[pic_size].height;

    return HI_SUCCESS;
}

hi_pic_size sample_comm_sys_get_pic_enum(const hi_size *size)
{
    hi_pic_size i;

    for (i = PIC_CIF; i < PIC_BUTT; i++) {
        if ((g_sample_pic_size[i].width == size->width) &&
            (g_sample_pic_size[i].height == size->height)) {
            return i;
        }
    }

    return PIC_1080P;
}

static hi_s32 sample_comm_sys_set_module_mem_config(hi_u32 mod)
{
    hi_char *mmz_name = HI_NULL;
    hi_mpp_chn mpp_chn;
    hi_u32 max_dev_num = g_sample_mpp_chn[mod].dev_id;
    hi_u32 max_chn_num = g_sample_mpp_chn[mod].chn_id;
    hi_u32 i, j;

    mpp_chn.mod_id = g_sample_mpp_chn[mod].mod_id;

    for (i = 0; i < max_dev_num; ++i) {
        mpp_chn.dev_id = i;

        for (j = 0; j < max_chn_num; ++j) {
            mpp_chn.chn_id = j;
            if (hi_mpi_sys_set_mem_cfg(&mpp_chn, mmz_name) != HI_SUCCESS) {
                sample_print("hi_mpi_sys_set_mem_cfg ERR!\n");
                return HI_FAILURE;
            }
        }
    }

    return HI_SUCCESS;
}

/* set system memory location */
hi_s32 sample_comm_sys_mem_config(hi_void)
{
    hi_u32 i;

    /* config memory */
    for (i = 0; i < sizeof(g_sample_mpp_chn) / sizeof(g_sample_mpp_chn[0]); ++i) {
        if (sample_comm_sys_set_module_mem_config(i) != HI_SUCCESS) {
            return HI_FAILURE;
        }
    }

    return HI_SUCCESS;
}

/* vb init & MPI system init */
hi_s32 sample_comm_sys_init(const hi_vb_cfg *vb_cfg)
{
    hi_s32 ret;

    hi_mpi_sys_exit();
    hi_mpi_vb_exit();

    if (vb_cfg == HI_NULL) {
        sample_print("input parameter is null, it is invalid!\n");
        return HI_FAILURE;
    }

    ret = hi_mpi_vb_set_cfg(vb_cfg);
    if (ret != HI_SUCCESS) {
        sample_print("hi_mpi_vb_set_conf failed!\n");
        return HI_FAILURE;
    }

    ret = hi_mpi_vb_init();
    if (ret != HI_SUCCESS) {
        sample_print("hi_mpi_vb_init failed!\n");
        return HI_FAILURE;
    }

    ret = hi_mpi_sys_init();
    if (ret != HI_SUCCESS) {
        sample_print("hi_mpi_sys_init failed!\n");
        hi_mpi_vb_exit();
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

/* vb init with vb_supplement & MPI system init */
hi_s32 sample_comm_sys_init_with_vb_supplement(const hi_vb_cfg *vb_conf, hi_u32 supplement_config)
{
    hi_s32 ret;
    hi_vb_supplement_cfg supplement_conf = {0};

    hi_mpi_sys_exit();
    hi_mpi_vb_exit();

    if (vb_conf == HI_NULL) {
        sample_print("input parameter is null, it is invalid!\n");
        return HI_FAILURE;
    }

    ret = hi_mpi_vb_set_cfg(vb_conf);
    if (ret != HI_SUCCESS) {
        sample_print("hi_mpi_vb_set_conf failed!\n");
        return HI_FAILURE;
    }

    supplement_conf.supplement_cfg = supplement_config;
    ret = hi_mpi_vb_set_supplement_cfg(&supplement_conf);
    if (ret != HI_SUCCESS) {
        sample_print("hi_mpi_vb_set_supplement_conf failed!\n");
        return HI_FAILURE;
    }

    ret = hi_mpi_vb_init();
    if (ret != HI_SUCCESS) {
        sample_print("hi_mpi_vb_init failed!\n");
        return HI_FAILURE;
    }

    ret = hi_mpi_sys_init();
    if (ret != HI_SUCCESS) {
        sample_print("hi_mpi_sys_init failed!\n");
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

/* vb exit & MPI system exit */
hi_void sample_comm_sys_exit(hi_void)
{
    hi_mpi_sys_exit();
    hi_mpi_vb_exit_mod_common_pool(HI_VB_UID_VDEC);
    hi_mpi_vb_exit();
    return;
}

hi_s32 sample_comm_vi_bind_vo(hi_vi_pipe vi_pipe, hi_vi_chn vi_chn, hi_vo_layer vo_layer, hi_vo_chn vo_chn)
{
    hi_mpp_chn src_chn;
    hi_mpp_chn dest_chn;

    src_chn.mod_id = HI_ID_VI;
    src_chn.dev_id = vi_pipe;
    src_chn.chn_id = vi_chn;

    dest_chn.mod_id = HI_ID_VO;
    dest_chn.dev_id = vo_layer;
    dest_chn.chn_id = vo_chn;

    check_return(hi_mpi_sys_bind(&src_chn, &dest_chn), "hi_mpi_sys_bind(VI-VO)");

    return HI_SUCCESS;
}

hi_s32 sample_comm_vi_un_bind_vo(hi_vi_pipe vi_pipe, hi_vi_chn vi_chn, hi_vo_layer vo_layer, hi_vo_chn vo_chn)
{
    hi_mpp_chn src_chn;
    hi_mpp_chn dest_chn;

    src_chn.mod_id = HI_ID_VI;
    src_chn.dev_id = vi_pipe;
    src_chn.chn_id = vi_chn;

    dest_chn.mod_id = HI_ID_VO;
    dest_chn.dev_id = vo_layer;
    dest_chn.chn_id = vo_chn;

    check_return(hi_mpi_sys_unbind(&src_chn, &dest_chn), "hi_mpi_sys_unbind(VI-VO)");

    return HI_SUCCESS;
}

hi_s32 sample_comm_vi_bind_vpss(hi_vi_pipe vi_pipe, hi_vi_chn vi_chn, hi_vpss_grp vpss_grp, hi_vpss_chn vpss_chn)
{
    hi_mpp_chn src_chn;
    hi_mpp_chn dest_chn;

    src_chn.mod_id = HI_ID_VI;
    src_chn.dev_id = vi_pipe;
    src_chn.chn_id = vi_chn;

    dest_chn.mod_id = HI_ID_VPSS;
    dest_chn.dev_id = vpss_grp;
    dest_chn.chn_id = vpss_chn;

    check_return(hi_mpi_sys_bind(&src_chn, &dest_chn), "hi_mpi_sys_bind(VI-VPSS)");

    return HI_SUCCESS;
}

hi_s32 sample_comm_vi_un_bind_vpss(hi_vi_pipe vi_pipe, hi_vi_chn vi_chn, hi_vpss_grp vpss_grp, hi_vpss_chn vpss_chn)
{
    hi_mpp_chn src_chn;
    hi_mpp_chn dest_chn;

    src_chn.mod_id = HI_ID_VI;
    src_chn.dev_id = vi_pipe;
    src_chn.chn_id = vi_chn;

    dest_chn.mod_id = HI_ID_VPSS;
    dest_chn.dev_id = vpss_grp;
    dest_chn.chn_id = vpss_chn;

    check_return(hi_mpi_sys_unbind(&src_chn, &dest_chn), "hi_mpi_sys_unbind(VI-VPSS)");

    return HI_SUCCESS;
}

hi_s32 sample_comm_vi_bind_venc(hi_vi_pipe vi_pipe, hi_vi_chn vi_chn, hi_venc_chn venc_chn)
{
    hi_mpp_chn src_chn;
    hi_mpp_chn dest_chn;

    src_chn.mod_id = HI_ID_VI;
    src_chn.dev_id = vi_pipe;
    src_chn.chn_id = vi_chn;

    dest_chn.mod_id = HI_ID_VENC;
    dest_chn.dev_id = 0;
    dest_chn.chn_id = venc_chn;

    check_return(hi_mpi_sys_bind(&src_chn, &dest_chn), "hi_mpi_sys_bind(VI-VENC)");

    return HI_SUCCESS;
}

hi_s32 sample_comm_vi_un_bind_venc(hi_vi_pipe vi_pipe, hi_vi_chn vi_chn, hi_venc_chn venc_chn)
{
    hi_mpp_chn src_chn;
    hi_mpp_chn dest_chn;

    src_chn.mod_id = HI_ID_VI;
    src_chn.dev_id = vi_pipe;
    src_chn.chn_id = vi_chn;

    dest_chn.mod_id = HI_ID_VENC;
    dest_chn.dev_id = 0;
    dest_chn.chn_id = venc_chn;

    check_return(hi_mpi_sys_unbind(&src_chn, &dest_chn), "hi_mpi_sys_unbind(VI-VENC)");

    return HI_SUCCESS;
}

hi_s32 sample_comm_avs_bind_venc(hi_avs_grp avs_grp, hi_avs_chn avs_chn, hi_venc_chn venc_chn)
{
    hi_mpp_chn src_chn;
    hi_mpp_chn dest_chn;

    src_chn.mod_id = HI_ID_AVS;
    src_chn.dev_id = avs_grp;
    src_chn.chn_id = avs_chn;

    dest_chn.mod_id = HI_ID_VENC;
    dest_chn.dev_id = 0;
    dest_chn.chn_id = venc_chn;

    check_return(hi_mpi_sys_bind(&src_chn, &dest_chn), "hi_mpi_sys_bind(AVS-VENC)");

    return HI_SUCCESS;
}

hi_s32 sample_comm_avs_un_bind_venc(hi_avs_grp avs_grp, hi_avs_chn avs_chn, hi_venc_chn venc_chn)
{
    hi_mpp_chn src_chn;
    hi_mpp_chn dest_chn;

    src_chn.mod_id = HI_ID_AVS;
    src_chn.dev_id = avs_grp;
    src_chn.chn_id = avs_chn;

    dest_chn.mod_id = HI_ID_VENC;
    dest_chn.dev_id = 0;
    dest_chn.chn_id = venc_chn;

    check_return(hi_mpi_sys_unbind(&src_chn, &dest_chn), "hi_mpi_sys_unbind(AVS-VENC)");

    return HI_SUCCESS;
}

hi_s32 sample_comm_avs_bind_vo(hi_avs_grp avs_grp, hi_avs_chn avs_chn, hi_vo_layer vo_layer, hi_vo_chn vo_chn)
{
    hi_mpp_chn src_chn;
    hi_mpp_chn dest_chn;

    src_chn.mod_id = HI_ID_AVS;
    src_chn.dev_id = avs_grp;
    src_chn.chn_id = avs_chn;

    dest_chn.mod_id = HI_ID_VO;
    dest_chn.dev_id = vo_layer;
    dest_chn.chn_id = vo_chn;

    check_return(hi_mpi_sys_bind(&src_chn, &dest_chn), "hi_mpi_sys_bind(AVS-VO)");

    return HI_SUCCESS;
}

hi_s32 sample_comm_avs_un_bind_vo(hi_avs_grp avs_grp, hi_avs_chn avs_chn, hi_vo_layer vo_layer, hi_vo_chn vo_chn)
{
    hi_mpp_chn src_chn;
    hi_mpp_chn dest_chn;

    src_chn.mod_id = HI_ID_AVS;
    src_chn.dev_id = avs_grp;
    src_chn.chn_id = avs_chn;

    dest_chn.mod_id = HI_ID_VO;
    dest_chn.dev_id = vo_layer;
    dest_chn.chn_id = vo_chn;

    check_return(hi_mpi_sys_unbind(&src_chn, &dest_chn), "hi_mpi_sys_unbind(AVS-VO)");

    return HI_SUCCESS;
}


hi_s32 sample_comm_vpss_bind_vo(hi_vpss_grp vpss_grp, hi_vpss_chn vpss_chn, hi_vo_layer vo_layer, hi_vo_chn vo_chn)
{
    hi_mpp_chn src_chn;
    hi_mpp_chn dest_chn;

    src_chn.mod_id = HI_ID_VPSS;
    src_chn.dev_id = vpss_grp;
    src_chn.chn_id = vpss_chn;

    dest_chn.mod_id = HI_ID_VO;
    dest_chn.dev_id = vo_layer;
    dest_chn.chn_id = vo_chn;

    check_return(hi_mpi_sys_bind(&src_chn, &dest_chn), "hi_mpi_sys_bind(VPSS-VO)");

    return HI_SUCCESS;
}

hi_s32 sample_comm_vpss_un_bind_vo(hi_vpss_grp vpss_grp, hi_vpss_chn vpss_chn, hi_vo_layer vo_layer, hi_vo_chn vo_chn)
{
    hi_mpp_chn src_chn;
    hi_mpp_chn dest_chn;

    src_chn.mod_id = HI_ID_VPSS;
    src_chn.dev_id = vpss_grp;
    src_chn.chn_id = vpss_chn;

    dest_chn.mod_id = HI_ID_VO;
    dest_chn.dev_id = vo_layer;
    dest_chn.chn_id = vo_chn;

    check_return(hi_mpi_sys_unbind(&src_chn, &dest_chn), "hi_mpi_sys_unbind(VPSS-VO)");

    return HI_SUCCESS;
}

hi_s32 sample_comm_vpss_bind_avs(hi_vpss_grp vpss_grp, hi_vpss_chn vpss_chn, hi_avs_grp avs_grp, hi_avs_pipe avs_pipe)
{
    hi_mpp_chn src_chn;
    hi_mpp_chn dest_chn;

    src_chn.mod_id = HI_ID_VPSS;
    src_chn.dev_id = vpss_grp;
    src_chn.chn_id = vpss_chn;

    dest_chn.mod_id = HI_ID_AVS;
    dest_chn.dev_id = avs_grp;
    dest_chn.chn_id = avs_pipe;

    check_return(hi_mpi_sys_bind(&src_chn, &dest_chn), "hi_mpi_sys_bind(VPSS-AVS)");

    return HI_SUCCESS;
}

hi_s32 sample_comm_vpss_un_bind_avs(hi_vpss_grp vpss_grp, hi_vpss_chn vpss_chn,
                                    hi_avs_grp avs_grp, hi_avs_pipe avs_pipe)
{
    hi_mpp_chn src_chn;
    hi_mpp_chn dest_chn;

    src_chn.mod_id = HI_ID_VPSS;
    src_chn.dev_id = vpss_grp;
    src_chn.chn_id = vpss_chn;

    dest_chn.mod_id = HI_ID_AVS;
    dest_chn.dev_id = avs_grp;
    dest_chn.chn_id = avs_pipe;

    check_return(hi_mpi_sys_unbind(&src_chn, &dest_chn), "hi_mpi_sys_unbind(VPSS-AVS)");

    return HI_SUCCESS;
}


hi_s32 sample_comm_vpss_bind_venc(hi_vpss_grp vpss_grp, hi_vpss_chn vpss_chn, hi_venc_chn venc_chn)
{
    hi_mpp_chn src_chn;
    hi_mpp_chn dest_chn;

    src_chn.mod_id = HI_ID_VPSS;
    src_chn.dev_id = vpss_grp;
    src_chn.chn_id = vpss_chn;

    dest_chn.mod_id = HI_ID_VENC;
    dest_chn.dev_id = 0;
    dest_chn.chn_id = venc_chn;

    check_return(hi_mpi_sys_bind(&src_chn, &dest_chn), "hi_mpi_sys_bind(VPSS-VENC)");

    return HI_SUCCESS;
}

hi_s32 sample_comm_vpss_un_bind_venc(hi_vpss_grp vpss_grp, hi_vpss_chn vpss_chn, hi_venc_chn venc_chn)
{
    hi_mpp_chn src_chn;
    hi_mpp_chn dest_chn;

    src_chn.mod_id = HI_ID_VPSS;
    src_chn.dev_id = vpss_grp;
    src_chn.chn_id = vpss_chn;

    dest_chn.mod_id = HI_ID_VENC;
    dest_chn.dev_id = 0;
    dest_chn.chn_id = venc_chn;

    check_return(hi_mpi_sys_unbind(&src_chn, &dest_chn), "hi_mpi_sys_unbind(VPSS-VENC)");

    return HI_SUCCESS;
}

hi_s32 sample_comm_vdec_bind_vpss(hi_vdec_chn vdec_chn, hi_vpss_grp vpss_grp)
{
    hi_mpp_chn src_chn;
    hi_mpp_chn dest_chn;

    src_chn.mod_id = HI_ID_VDEC;
    src_chn.dev_id = 0;
    src_chn.chn_id = vdec_chn;

    dest_chn.mod_id = HI_ID_VPSS;
    dest_chn.dev_id = vpss_grp;
    dest_chn.chn_id = 0;

    check_return(hi_mpi_sys_bind(&src_chn, &dest_chn), "hi_mpi_sys_bind(VDEC-VPSS)");

    return HI_SUCCESS;
}

hi_s32 sample_comm_vdec_un_bind_vpss(hi_vdec_chn vdec_chn, hi_vpss_grp vpss_grp)
{
    hi_mpp_chn src_chn;
    hi_mpp_chn dest_chn;

    src_chn.mod_id = HI_ID_VDEC;
    src_chn.dev_id = 0;
    src_chn.chn_id = vdec_chn;

    dest_chn.mod_id = HI_ID_VPSS;
    dest_chn.dev_id = vpss_grp;
    dest_chn.chn_id = 0;

    check_return(hi_mpi_sys_unbind(&src_chn, &dest_chn), "hi_mpi_sys_unbind(VDEC-VPSS)");

    return HI_SUCCESS;
}

hi_s32 sample_comm_vo_bind_vo(hi_vo_layer src_vo_layer, hi_vo_chn src_vo_chn,
                              hi_vo_layer dst_vo_layer, hi_vo_chn dst_vo_chn)
{
    hi_mpp_chn src_chn, dest_chn;

    src_chn.mod_id = HI_ID_VO;
    src_chn.dev_id = src_vo_layer;
    src_chn.chn_id = src_vo_chn;

    dest_chn.mod_id = HI_ID_VO;
    dest_chn.dev_id = dst_vo_layer;
    dest_chn.chn_id = dst_vo_chn;

    return hi_mpi_sys_bind(&src_chn, &dest_chn);
}

hi_s32 sample_comm_vo_un_bind_vo(hi_vo_layer dst_vo_layer, hi_vo_chn dst_vo_chn)
{
    hi_mpp_chn dest_chn;

    dest_chn.mod_id = HI_ID_VO;
    dest_chn.dev_id = dst_vo_layer;
    dest_chn.chn_id = dst_vo_chn;

    return hi_mpi_sys_unbind(HI_NULL, &dest_chn);
}
