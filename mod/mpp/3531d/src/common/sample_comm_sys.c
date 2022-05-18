/*
 * Copyright (c) Hisilicon Technologies Co., Ltd. 2019-2020. All rights reserved.
 * Description: sys common function for sample.
 * Author: Hisilicon multimedia software group
 * Create: 2019/11/29
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

#include "sample_comm.h"

static hi_s32 s_sample_mem_dev = -1;
#define SAMPLE_MEM_DEV_OPEN if (s_sample_mem_dev <= 0) {      \
        s_sample_mem_dev = open("/dev/mem", O_RDWR | O_SYNC); \
        if (s_sample_mem_dev < 0) {                           \
            perror("open dev/mem error");                     \
            return NULL;                                      \
        }                                                     \
    }

hi_void *sample_sys_io_mmap(hi_u64 phy_addr, hi_u32 size)
{
    hi_u32 diff;
    hi_u64 page_phy;
    hi_u8 *page_addr = HI_NULL;
    hi_ulong ul_page_size;

    SAMPLE_MEM_DEV_OPEN;

    /*
     * page_size will be 0 when u32size is 0 and diff is 0,
     * and then mmap will be error (error: invalid argument)
     */
    if (!size) {
        printf("func: %s size can't be 0.\n", __FUNCTION__);
        return NULL;
    }

    /* the mmap address should align with page */
    page_phy = phy_addr & 0xfffffffffffff000ULL;
    diff = phy_addr - page_phy;

    /* the mmap size shuld be mutliples of 1024 */
    ul_page_size = ((size + diff - 1) & 0xfffff000UL) + 0x1000;

    page_addr = mmap((void *)0, ul_page_size, PROT_READ | PROT_WRITE, MAP_SHARED, s_sample_mem_dev, page_phy);
    if (page_addr == MAP_FAILED) {
        perror("mmap error");
        return NULL;
    }
    return (hi_void *)(page_addr + diff);
}

hi_s32 sample_sys_munmap(hi_void *vir_addr, hi_u32 size)
{
    hi_u64 page_addr;
    hi_u32 page_size;
    hi_u32 diff;

    page_addr = (((hi_uintptr_t)vir_addr) & 0xfffffffffffff000ULL);
    diff = (hi_uintptr_t)vir_addr - page_addr;
    page_size = ((size + diff - 1) & 0xfffff000UL) + 0x1000;

    return munmap((hi_void *)(hi_uintptr_t)page_addr, page_size);
}

hi_s32 sample_sys_set_reg(hi_u64 addr, hi_u32 value)
{
    hi_u32 *reg_addr = NULL;
    hi_u32 map_len = sizeof(value);

    reg_addr = (hi_u32 *)sample_sys_io_mmap(addr, map_len);
    if (reg_addr == NULL) {
        return HI_FAILURE;
    }

    *reg_addr = value;

    return sample_sys_munmap(reg_addr, map_len);
}

hi_s32 sample_sys_get_reg(hi_u64 addr, hi_u32 *value)
{
    hi_u32 *reg_addr = NULL;
    hi_u32 map_len;

    if (value == NULL) {
        return HI_ERR_SYS_NULL_PTR;
    }

    map_len = sizeof(*value);
    reg_addr = (hi_u32 *)sample_sys_io_mmap(addr, map_len);
    if (reg_addr == NULL) {
        return HI_FAILURE;
    }

    *value = *reg_addr;

    return sample_sys_munmap(reg_addr, map_len);
}

/* function : get picture size(w*h), according pic_size */
hi_s32 sample_comm_sys_get_pic_size(hi_pic_size pic_size, hi_size *size)
{
    switch (pic_size) {
        case PIC_CIF: /* 352 * 288 */
            size->width = 352;
            size->height = 288;
            break;

        case PIC_360P: /* 640 * 360 */
            size->width = 640;
            size->height = 360;
            break;

        case PIC_D1_PAL: /* 720 * 576 */
            size->width = 720;
            size->height = 576;
            break;

        case PIC_D1_NTSC: /* 720 * 480 */
            size->width = 720;
            size->height = 480;
            break;

        case PIC_960H: /* 960 * 576 */
            size->width = 960;
            size->height = 576;
            break;

        case PIC_720P: /* 1280 * 720 */
            size->width = 1280;
            size->height = 720;
            break;

        case PIC_1080P: /* 1920 * 1080 */
            size->width = 1920;
            size->height = 1080;
            break;

        case PIC_2592x1520:
            size->width = 2592;
            size->height = 1520;
            break;

        case PIC_2592x1944:
            size->width = 2592;
            size->height = 1944;
            break;

        case PIC_3840x2160:
            size->width = 3840;
            size->height = 2160;
            break;

        case PIC_3000x3000:
            size->width = 3000;
            size->height = 3000;
            break;

        case PIC_4000x3000:
            size->width = 4000;
            size->height = 3000;
            break;

        case PIC_4096x2160:
            size->width = 4096;
            size->height = 2160;
            break;

        case PIC_7680x4320:
            size->width = 7680;
            size->height = 4320;
            break;
        case PIC_3840x8640:
            size->width = 3840;
            size->height = 8640;
            break;

        case PIC_480P:
            size->width = 720;
            size->height = 480;
            break;

        case PIC_576P:
            size->width = 720;
            size->height = 576;
            break;

        case PIC_800x600:
            size->width = 800;
            size->height = 600;
            break;

        case PIC_1024x768:
            size->width = 1024;
            size->height = 768;
            break;

        case PIC_1280x1024:
            size->width = 1280;
            size->height = 1024;
            break;

        case PIC_1366x768:
            size->width = 1366;
            size->height = 768;
            break;

        case PIC_1440x900:
            size->width = 1440;
            size->height = 900;
            break;

        case PIC_1280x800:
            size->width = 1280;
            size->height = 800;
            break;

        case PIC_1600x1200:
            size->width = 1600;
            size->height = 1200;
            break;

        case PIC_1680x1050:
            size->width = 1680;
            size->height = 1050;
            break;

        case PIC_1920x1200:
            size->width = 1920;
            size->height = 1200;
            break;

        case PIC_640x480:
            size->width = 640;
            size->height = 480;
            break;

        case PIC_1920x2160:
            size->width = 1920;
            size->height = 2160;
            break;

        case PIC_2560x1440:
            size->width = 2560;
            size->height = 1440;
            break;

        case PIC_2560x1600:
            size->width = 2560;
            size->height = 1600;
            break;

        default:
            return HI_FAILURE;
    }

    return HI_SUCCESS;
}

/* function : set system memory location */
hi_s32 sample_comm_sys_mem_config(hi_void)
{
    hi_s32 i, j;
    hi_s32 ret;
    hi_char *pc_mmz_name = NULL;
    hi_mpp_chn hi_mpp_chn;

    /* config memory for vi */
    for (i = 0; i < HI_VI_MAX_DEV_NUM; i++) {
        for (j = 0; j < HI_VI_MAX_CHN_NUM; j++) {
            hi_mpp_chn.mod_id = HI_ID_VI;
            hi_mpp_chn.dev_id = i;
            hi_mpp_chn.chn_id = j;
            ret = hi_mpi_sys_set_mem_cfg(&hi_mpp_chn, pc_mmz_name);
            if (ret) {
                SAMPLE_PRT("hi_mpi_sys_set_mem_config ERR !\n");
                return HI_FAILURE;
            }
        }
    }

    /* config memory for vpss */
    for (i = 0; i < HI_VPSS_MAX_GRP_NUM; i++) {
        hi_mpp_chn.mod_id = HI_ID_VPSS;
        hi_mpp_chn.dev_id = i;
        hi_mpp_chn.chn_id = 0;
        ret = hi_mpi_sys_set_mem_cfg(&hi_mpp_chn, pc_mmz_name);
        if (ret) {
            SAMPLE_PRT("hi_mpi_sys_set_mem_config ERR !\n");
            return HI_FAILURE;
        }
    }

    /* config memory for venc */
    for (i = 0; i < HI_VENC_MAX_CHN_NUM; i++) {
        hi_mpp_chn.mod_id = HI_ID_VENC;
        hi_mpp_chn.dev_id = 0;
        hi_mpp_chn.chn_id = i;
        ret = hi_mpi_sys_set_mem_cfg(&hi_mpp_chn, pc_mmz_name);
        if (ret) {
            SAMPLE_PRT("hi_mpi_sys_set_mem_conf ERR !\n");
            return HI_FAILURE;
        }
    }

    /* config memory for vo */
    for (i = 0; i < HI_VO_MAX_LAYER_NUM; i++) {
        for (j = 0; j < HI_VO_MAX_CHN_NUM; j++) {
            hi_mpp_chn.mod_id = HI_ID_VO;
            hi_mpp_chn.dev_id = i;
            hi_mpp_chn.chn_id = j;
            ret = hi_mpi_sys_set_mem_cfg(&hi_mpp_chn, pc_mmz_name);
            if (ret) {
                SAMPLE_PRT("hi_mpi_sys_set_mem_config ERR !\n");
                return HI_FAILURE;
            }
        }
    }

    /* config memory for vdec */
    for (i = 0; i < HI_VDEC_MAX_CHN_NUM; i++) {
        hi_mpp_chn.mod_id = HI_ID_VDEC;
        hi_mpp_chn.dev_id = 0;
        hi_mpp_chn.chn_id = i;
        ret = hi_mpi_sys_set_mem_cfg(&hi_mpp_chn, pc_mmz_name);
        if (ret) {
            SAMPLE_PRT("hi_mpi_sys_set_mem_conf ERR !\n");
            return HI_FAILURE;
        }
    }

    return HI_SUCCESS;
}

/* function : vb init & MPI system init */
hi_s32 sample_comm_sys_init(hi_vb_cfg *vb_cfg)
{
    hi_s32 ret;

    hi_mpi_sys_exit();
    hi_mpi_vb_exit();

    if (vb_cfg == NULL) {
        SAMPLE_PRT("input parameter is null, it is invaild!\n");
        return HI_FAILURE;
    }

    ret = hi_mpi_vb_set_cfg(vb_cfg);
    if (ret != HI_SUCCESS) {
        SAMPLE_PRT("hi_mpi_vb_set_conf failed!\n");
        return HI_FAILURE;
    }

    ret = hi_mpi_vb_init();
    if (ret != HI_SUCCESS) {
        SAMPLE_PRT("hi_mpi_vb_init failed!\n");
        return HI_FAILURE;
    }

    ret = hi_mpi_sys_init();
    if (ret != HI_SUCCESS) {
        SAMPLE_PRT("hi_mpi_sys_init failed!\n");
        hi_mpi_vb_exit();
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

/* function : vb init with vb_supplement & MPI system init */
hi_s32 sample_comm_sys_init_with_vb_supplement(hi_vb_cfg *vb_conf, hi_u32 supplement_config)
{
    hi_vb_supplement_cfg supplement_conf = { 0 };
    hi_s32 ret;

    hi_mpi_sys_exit();
    hi_mpi_vb_exit();

    if (vb_conf == NULL) {
        SAMPLE_PRT("input parameter is null, it is invaild!\n");
        return HI_FAILURE;
    }

    ret = hi_mpi_vb_set_cfg(vb_conf);
    if (ret != HI_SUCCESS) {
        SAMPLE_PRT("hi_mpi_vb_set_conf failed!\n");
        return HI_FAILURE;
    }

    supplement_conf.supplement_cfg = supplement_config;

    ret = hi_mpi_vb_set_supplement_cfg(&supplement_conf);
    if (ret != HI_SUCCESS) {
        SAMPLE_PRT("hi_mpi_vb_set_supplement_conf failed!\n");
        return HI_FAILURE;
    }

    ret = hi_mpi_vb_init();
    if (ret != HI_SUCCESS) {
        SAMPLE_PRT("hi_mpi_vb_init failed!\n");
        return HI_FAILURE;
    }

    ret = hi_mpi_sys_init();
    if (ret != HI_SUCCESS) {
        SAMPLE_PRT("hi_mpi_sys_init failed!\n");
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

/* function : vb exit & MPI system exit */
hi_void sample_comm_sys_exit(void)
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

    CHECK_RET(hi_mpi_sys_bind(&src_chn, &dest_chn), "hi_mpi_sys_bind(VI-VO)");

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

    CHECK_RET(hi_mpi_sys_unbind(&src_chn, &dest_chn), "hi_mpi_sys_unbind(VI-VO)");

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
    
    printf("%s => vi_pipe:%d, vi_chn:%d bind vpss_grp:%d\n", __func__, vi_pipe, vi_chn, vpss_grp);
    CHECK_RET(hi_mpi_sys_bind(&src_chn, &dest_chn), "hi_mpi_sys_bind(VI-VPSS)");

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

    CHECK_RET(hi_mpi_sys_unbind(&src_chn, &dest_chn), "hi_mpi_sys_unbind(VI-VPSS)");

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

    CHECK_RET(hi_mpi_sys_bind(&src_chn, &dest_chn), "hi_mpi_sys_bind(VI-VENC)");

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

    CHECK_RET(hi_mpi_sys_unbind(&src_chn, &dest_chn), "hi_mpi_sys_unbind(VI-VENC)");

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

    CHECK_RET(hi_mpi_sys_bind(&src_chn, &dest_chn), "hi_mpi_sys_bind(VPSS-VO)");

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

    CHECK_RET(hi_mpi_sys_unbind(&src_chn, &dest_chn), "hi_mpi_sys_unbind(VPSS-VO)");

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

    CHECK_RET(hi_mpi_sys_bind(&src_chn, &dest_chn), "hi_mpi_sys_bind(VPSS-VENC)");

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

    CHECK_RET(hi_mpi_sys_unbind(&src_chn, &dest_chn), "hi_mpi_sys_unbind(VPSS-VENC)");

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

    CHECK_RET(hi_mpi_sys_bind(&src_chn, &dest_chn), "hi_mpi_sys_bind(VDEC-VPSS)");

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

    CHECK_RET(hi_mpi_sys_unbind(&src_chn, &dest_chn), "hi_mpi_sys_unbind(VDEC-VPSS)");

    return HI_SUCCESS;
}

hi_s32 sample_comm_vo_bind_vo(hi_vo_layer src_vo_layer, hi_vo_chn src_vo_chn, hi_vo_layer dst_vo_layer,
    hi_vo_chn dst_vo_chn)
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

    return hi_mpi_sys_unbind(NULL, &dest_chn);
}
