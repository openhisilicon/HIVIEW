/*
  Copyright (c), 2001-2024, Shenshu Tech. Co., Ltd.
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

#define sample_mem_check_open_return() \
    do { \
        if (g_sample_mem_dev <= 0) { \
            g_sample_mem_dev = open("/dev/mem", O_RDWR | O_SYNC); \
            if (g_sample_mem_dev < 0) { \
                perror("open dev/mem error"); \
                return HI_NULL; \
            } \
        } \
    } while (0)

static hi_s32 g_sample_mem_dev = -1;

static hi_mpp_chn g_sample_mpp_chn[] = {
    {HI_ID_VI, HI_VI_MAX_DEV_NUM, HI_VI_MAX_CHN_NUM},
    {HI_ID_VPSS, HI_VPSS_MAX_GRP_NUM, 1},
    {HI_ID_VENC, 1, HI_VENC_MAX_CHN_NUM}
};

/* The order of g_sample_pic_size's element must be consistent with the enum value defined in "hi_pic_size". */
static hi_size g_sample_pic_size[PIC_BUTT] = {
    { 352,  288  },  /* PIC_CIF */
    { 640,  360  },  /* PIC_360P */
    { 640,  512  },  /* maohw PIC_512P */
    { 640,  640  },  /* maohw PIC_640P */
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
    { 2304, 1296 },  /* PIC_2304X1296 */
    { 2560, 1440 },  /* PIC_2560X1440 */
    { 2560, 1600 },  /* PIC_2560X1600 */
    { 2592, 1520 },  /* PIC_2592X1520 */
    { 2688, 1520 },  /* PIC_2688X1520 */
    { 2592, 1944 },  /* PIC_2592X1944 */
    { 3840, 2160 },  /* PIC_3840X2160 */
    { 4096, 2160 },  /* PIC_4096X2160 */
    { 3000, 3000 },  /* PIC_3000X3000 */
    { 4000, 3000 },  /* PIC_4000X3000 */
    { 6080, 2800 },  /* PIC_6080X2800 */
    { 7680, 4320 },  /* PIC_7680X4320 */
    { 3840, 8640 }   /* PIC_3840X8640 */
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

hi_void *sample_sys_io_mmap(hi_u64 phy_addr, hi_u32 size)
{
    hi_u32 diff;
    hi_u64 page_phy;
    hi_u8 *page_addr = HI_NULL;
    hi_ulong page_size;

    sample_mem_check_open_return();

    /* page_size will be 0 when size is 0 and diff is 0, and then mmap will be error(invalid argument) */
    if (!size) {
        printf("func: %s size can't be 0.\n", __FUNCTION__);
        return HI_NULL;
    }

    /* the mmap address should align with page */
    page_phy = phy_addr & 0xfffffffffffff000ULL;
    diff = phy_addr - page_phy;

    /* the mmap size should be multiples of 1024 */
    page_size = ((size + diff - 1) & 0xfffff000UL) + 0x1000;

    page_addr = mmap((void *)0, page_size, PROT_READ | PROT_WRITE, MAP_SHARED, g_sample_mem_dev, page_phy);
    if (page_addr == MAP_FAILED) {
        perror("mmap error");
        return HI_NULL;
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
    hi_u32 *reg_addr = HI_NULL;
    hi_u32 map_len = sizeof(value);

    reg_addr = (hi_u32 *)sample_sys_io_mmap(addr, map_len);
    if (reg_addr == HI_NULL) {
        return HI_FAILURE;
    }

    *reg_addr = value;

    return sample_sys_munmap(reg_addr, map_len);
}

hi_s32 sample_sys_get_reg(hi_u64 addr, hi_u32 *value)
{
    hi_u32 *reg_addr = HI_NULL;
    hi_u32 map_len;

    if (value == HI_NULL) {
        return HI_ERR_SYS_NULL_PTR;
    }

    map_len = sizeof(*value);
    reg_addr = (hi_u32 *)sample_sys_io_mmap(addr, map_len);
    if (reg_addr == HI_NULL) {
        return HI_FAILURE;
    }

    *value = *reg_addr;

    return sample_sys_munmap(reg_addr, map_len);
}

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
hi_s32 sample_comm_sys_vb_init(const hi_vb_cfg *vb_cfg)
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

hi_void sample_comm_sys_get_default_vb_cfg(sample_vb_param *input_cfg, hi_vb_cfg *vb_cfg)
{
    hi_vb_calc_cfg calc_cfg;
    hi_pic_buf_attr buf_attr;
    hi_s32 i;

    (hi_void)memset_s(vb_cfg, sizeof(hi_vb_cfg), 0, sizeof(hi_vb_cfg));
    vb_cfg->max_pool_cnt = 128; /* 128 blks */

    for (i = 0; i < SAMPLE_VIE_POOL_NUM; i++) {
        buf_attr.width         = input_cfg->vb_size[i].width;
        buf_attr.height        = input_cfg->vb_size[i].height;
        buf_attr.align         = HI_DEFAULT_ALIGN;
        buf_attr.bit_width     = HI_DATA_BIT_WIDTH_8;
        buf_attr.pixel_format  = input_cfg->pixel_format[i];
        buf_attr.compress_mode = input_cfg->compress_mode[i];
        buf_attr.video_format  = input_cfg->video_format[i];
        hi_common_get_pic_buf_cfg(&buf_attr, &calc_cfg);

        vb_cfg->common_pool[i].blk_size = calc_cfg.vb_size;
        vb_cfg->common_pool[i].blk_cnt  = input_cfg->blk_num[i];
    }
}

hi_void sample_comm_sys_get_default_cfg(hi_u32 pipe_num, sample_comm_cfg *comm_cfg)
{
    hi_u32 i;
    hi_vpss_chn chn;
    sample_sns_type sns_type;

    comm_cfg->mode_type = HI_VI_ONLINE_VPSS_OFFLINE;
    comm_cfg->aiisp_mode = HI_VI_AIISP_MODE_DEFAULT;
    comm_cfg->nr_pos = HI_3DNR_POS_VI;
    comm_cfg->supplement_cfg = HI_VB_SUPPLEMENT_BNR_MOT_MASK;
    comm_cfg->vi_pipe = 0;
    comm_cfg->vi_chn = 0;
    sns_type = SENSOR0_TYPE;
    sample_comm_vi_get_size_by_sns_type(sns_type, &comm_cfg->in_size);
    sample_comm_vi_get_default_vi_cfg(sns_type, &comm_cfg->vi_cfg);

    comm_cfg->pipe_num = pipe_num > SAMPLE_VIE_MAX_ROUTE_NUM ? SAMPLE_VIE_MAX_ROUTE_NUM : pipe_num;
    for (i = 0; i < comm_cfg->pipe_num; i++) {
        comm_cfg->vpss_grp[i] = i;
        comm_cfg->vi_fmu[i] = HI_FMU_MODE_OFF;
        comm_cfg->vpss_fmu[i] = HI_FMU_MODE_OFF;
        comm_cfg->is_direct[i] = HI_FALSE;
    }

    sample_comm_vpss_get_default_grp_attr(&comm_cfg->grp_attr);
    comm_cfg->grp_attr.max_width  = comm_cfg->in_size.width;
    comm_cfg->grp_attr.max_height = comm_cfg->in_size.height;

    for (chn = 0; chn < HI_VPSS_MAX_PHYS_CHN_NUM; chn++) {
        comm_cfg->chn_en[chn] = HI_FALSE;
        sample_comm_vpss_get_default_chn_attr(&comm_cfg->chn_attr[chn]);
        if (chn > HI_VPSS_CHN1) {
            comm_cfg->chn_attr[chn].compress_mode = HI_COMPRESS_MODE_NONE;
        }
        comm_cfg->chn_attr[chn].width  = comm_cfg->in_size.width;
        comm_cfg->chn_attr[chn].height = comm_cfg->in_size.height;
#ifdef OT_FPGA
        comm_cfg->chn_attr[chn].frame_rate.src_frame_rate = 30; /* 30: fpga src frame rate */
        comm_cfg->chn_attr[chn].frame_rate.dst_frame_rate = 10; /* 10: fpga dst frame rate */
#endif
    }
    comm_cfg->chn_en[HI_VPSS_CHN0] = HI_TRUE;
}

static hi_u32 sample_comm_sys_get_fmu_wrap_num(hi_fmu_mode fmu_mode[], hi_u32 len)
{
    hi_u32 i;
    hi_u32 cnt = 0;

    for (i = 0; i < len; i++) {
        if (fmu_mode[i] == HI_FMU_MODE_WRAP) {
            cnt++;
        }
    }
    return cnt;
}

static hi_s32 sample_comm_sys_init_fmu_wrap(sample_comm_cfg *comm_cfg)
{
    hi_u32 cnt;
    hi_fmu_attr fmu_attr;

    cnt = sample_comm_sys_get_fmu_wrap_num(comm_cfg->vi_fmu, comm_cfg->pipe_num);
    if (cnt > 0) {
        fmu_attr.wrap_en = HI_TRUE;
        fmu_attr.page_num = MIN2(hi_common_get_fmu_wrap_page_num(HI_FMU_ID_VI,
            comm_cfg->in_size.width, comm_cfg->in_size.height) + (cnt - 1) * 3, /* 3: for multi pipe */
            HI_FMU_MAX_Y_PAGE_NUM);
    } else {
        fmu_attr.wrap_en = HI_FALSE;
    }
    if (hi_mpi_sys_set_fmu_attr(HI_FMU_ID_VI, &fmu_attr) != HI_SUCCESS) {
        return HI_FAILURE;
    }

    cnt = sample_comm_sys_get_fmu_wrap_num(comm_cfg->vpss_fmu, comm_cfg->pipe_num);
    if (cnt > 0) {
        fmu_attr.wrap_en = HI_TRUE;
        fmu_attr.page_num = MIN2(hi_common_get_fmu_wrap_page_num(HI_FMU_ID_VPSS,
            comm_cfg->in_size.width, comm_cfg->in_size.height) + (cnt - 1) * 3, /* 3: for multi pipe */
            HI_FMU_MAX_Y_PAGE_NUM + HI_FMU_MAX_C_PAGE_NUM);
    } else {
        fmu_attr.wrap_en = HI_FALSE;
    }
    if (hi_mpi_sys_set_fmu_attr(HI_FMU_ID_VPSS, &fmu_attr) != HI_SUCCESS) {
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

hi_s32 sample_comm_sys_init(sample_comm_cfg *comm_cfg)
{
    if (sample_comm_sys_init_with_vb_supplement(&comm_cfg->vb_cfg, comm_cfg->supplement_cfg) != HI_SUCCESS) {
        goto fail;
    }

    if (sample_comm_vi_set_vi_vpss_mode(comm_cfg->mode_type, comm_cfg->aiisp_mode) != HI_SUCCESS) {
        goto fail;
    }

    if (hi_mpi_sys_set_3dnr_pos(comm_cfg->nr_pos) != HI_SUCCESS) {
        goto fail;
    }

    if (sample_comm_sys_init_fmu_wrap(comm_cfg) != HI_SUCCESS) {
        goto fail;
    }

    return HI_SUCCESS;
fail:
    sample_comm_sys_exit();
    return HI_FAILURE;
}

/* vb exit & MPI system exit */
hi_void sample_comm_sys_exit(hi_void)
{
    hi_mpi_sys_exit();
    hi_mpi_vb_exit();
    return;
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
