/*
  Copyright (c), 2001-2022, Shenshu Tech. Co., Ltd.
 */
#include "sample_common_svp.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <signal.h>

#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "hi_common.h"
#include "hi_common_video.h"
#include "hi_common_sys.h"
#include "hi_common_svp.h"
#include "hi_mpi_vb.h"
#include "sample_comm.h"
#include "sample_common_ive.h"

#define SAMPLE_SVP_BLK_CNT           16
#define SMAPLE_SVP_DISPLAY_BUF_LEN   3
#define SAMPLE_SVP_VI_CHN_INTERVAL   4
#define SAMPLE_SVP_VDEC_CHN_0        0
#define SAMPLE_SVP_VDEC_CHN_NUM      1
#define SAMPLE_SVP_VPSS_BORDER_WIDTH 2
#define SAMPLE_SVP_VO_DIS_BUF_LEN    3
#define SAMPLE_SVP_MAX_WIDTH         32768
#define SAMPLE_SVP_NUM_TWO          2
#define SAMPLE_SVP_DSP_BIN_NUM_PER  4
#define SAMPLE_SVP_DSP_MEM_TYPE_SYS_DDR 0
#define SAMPLE_SVP_DSP_MEM_TYPE_IRAM    1
#define SAMPLE_SVP_DSP_MEM_TYPE_DRAM_0  2
#define SAMPLE_SVP_DSP_MEM_TYPE_DRAM_1  3

static hi_bool g_sample_svp_init_flag = HI_FALSE;

/* System init */
static hi_s32 sample_comm_svp_sys_init(hi_void)
{
    hi_s32 ret;
    hi_vb_cfg vb_cfg;

    ret = hi_mpi_sys_exit();
    sample_svp_check_exps_return(ret != HI_SUCCESS, ret, SAMPLE_SVP_ERR_LEVEL_ERROR,
        "Error(%#x):hi_mpi_sys_exit failed!\n", ret);
    ret = hi_mpi_vb_exit();
    sample_svp_check_exps_return(ret != HI_SUCCESS, ret, SAMPLE_SVP_ERR_LEVEL_ERROR,
        "Error(%#x):hi_mpi_vb_exit failed!\n", ret);

    (hi_void)memset_s(&vb_cfg, sizeof(hi_vb_cfg), 0, sizeof(hi_vb_cfg));

    vb_cfg.max_pool_cnt = SAMPLE_SVP_VB_POOL_NUM;
    vb_cfg.common_pool[1].blk_size = SAMPLE_SVP_D1_PAL_WIDTH * SAMPLE_SVP_D1_PAL_HEIGHT * SAMPLE_SVP_VB_POOL_NUM;
    vb_cfg.common_pool[1].blk_cnt  = 1;

    ret = hi_mpi_vb_set_cfg((const hi_vb_cfg *)&vb_cfg);
    sample_svp_check_exps_return(ret != HI_SUCCESS, ret, SAMPLE_SVP_ERR_LEVEL_ERROR,
        "Error(%#x):hi_mpi_vb_set_config failed!\n", ret);

    ret = hi_mpi_vb_init();
    sample_svp_check_exps_return(ret != HI_SUCCESS, ret, SAMPLE_SVP_ERR_LEVEL_ERROR,
        "Error(%#x):hi_mpi_vb_init failed!\n", ret);

    ret = hi_mpi_sys_init();
    sample_svp_check_exps_return(ret != HI_SUCCESS, ret, SAMPLE_SVP_ERR_LEVEL_ERROR,
        "Error(%#x):hi_mpi_sys_init failed!\n", ret);

    return ret;
}

/* System exit */
static hi_s32 sample_comm_svp_sys_exit(hi_void)
{
    hi_s32 ret;

    ret = hi_mpi_sys_exit();
    sample_svp_check_exps_return(ret != HI_SUCCESS, ret, SAMPLE_SVP_ERR_LEVEL_ERROR,
        "Error(%#x):hi_mpi_sys_exit failed!\n", ret);

    ret = hi_mpi_vb_exit();
    sample_svp_check_exps_return(ret != HI_SUCCESS, ret, SAMPLE_SVP_ERR_LEVEL_ERROR,
        "Error(%#x):hi_mpi_vb_exit failed!\n", ret);

    return HI_SUCCESS;
}

/* System init */
hi_s32 sample_common_svp_check_sys_init(hi_void)
{
    if(0)//maohw if (g_sample_svp_init_flag == HI_FALSE) 
    {
        if (sample_comm_svp_sys_init() != HI_SUCCESS) {
            sample_svp_trace_err("Svp mpi init failed!\n");
            return HI_FALSE;
        }
        g_sample_svp_init_flag = HI_TRUE;
    }

    sample_svp_trace_info("Svp mpi init ok!\n");
    return HI_TRUE;
}

/* System exit */
hi_void sample_common_svp_check_sys_exit(hi_void)
{
    hi_s32 ret;

    if(0)//maohw if (g_sample_svp_init_flag == HI_TRUE) 
    {
        ret = sample_comm_svp_sys_exit();
        if (ret != HI_SUCCESS) {
            sample_svp_trace_err("svp mpi exit failed!\n");
        }
    }
    g_sample_svp_init_flag = HI_FALSE;
    sample_svp_trace_info("Svp mpi exit ok!\n");
}

/* Align */
hi_u32 sample_common_svp_align(hi_u32 size, hi_u16 align)
{
    hi_u32 stride;

    sample_svp_check_exps_return(align == 0, 0, SAMPLE_SVP_ERR_LEVEL_ERROR, "align can't be zero!\n");
    sample_svp_check_exps_return((size < 1) || (size > SAMPLE_SVP_MAX_WIDTH), 0, SAMPLE_SVP_ERR_LEVEL_ERROR,
        "size(%u) must be [1, %u]\n", size, SAMPLE_SVP_MAX_WIDTH);
    stride = size + (align - size % align) % align;
    return stride;
}

/* Create mem info */
hi_s32 sample_common_svp_create_mem_info(hi_svp_mem_info *mem_info, hi_u32 size, hi_u32 addr_offset)
{
    hi_s32 ret = HI_FAILURE;
    hi_u32 size_tmp;

    sample_svp_check_exps_return(mem_info == HI_NULL, ret, SAMPLE_SVP_ERR_LEVEL_ERROR, "mem_info can't be zero\n");

    size_tmp = size + addr_offset;
    mem_info->size = size;
    ret = hi_mpi_sys_mmz_alloc((hi_phys_addr_t *)(&mem_info->phys_addr),
        (void **)&mem_info->virt_addr, HI_NULL, HI_NULL, size_tmp);
    sample_svp_check_exps_return(ret != HI_SUCCESS, ret, SAMPLE_SVP_ERR_LEVEL_ERROR,
        "Error(%#x):hi_mpi_sys_alloc failed!\n", ret);

    mem_info->phys_addr += addr_offset;
    mem_info->virt_addr += addr_offset;

    return ret;
}

/* Destory mem info */
hi_void sample_common_svp_destroy_mem_info(hi_svp_mem_info *mem_info, hi_u32 addr_offset)
{
    sample_svp_check_exps_return_void(mem_info == HI_NULL, SAMPLE_SVP_ERR_LEVEL_ERROR, "mem_info can't be zero\n");

    if ((mem_info->virt_addr != 0) && (mem_info->phys_addr != 0)) {
        (hi_void)hi_mpi_sys_mmz_free(mem_info->phys_addr - addr_offset,
            sample_svp_convert_addr_to_ptr(void, (mem_info->virt_addr - addr_offset)));
    }
    (hi_void)memset_s(mem_info, sizeof(*mem_info), 0, sizeof(*mem_info));
}
/* Malloc memory */
hi_s32 sample_common_svp_malloc_mem(hi_char *mmb, hi_char *zone, hi_phys_addr_t *phys_addr,
    hi_void **virt_addr, hi_u32 size)
{
    hi_s32 ret = HI_FAILURE;

    sample_svp_check_exps_return(phys_addr == HI_NULL, ret, SAMPLE_SVP_ERR_LEVEL_ERROR, "phys_addr can't be null\n");
    sample_svp_check_exps_return(virt_addr == HI_NULL, ret, SAMPLE_SVP_ERR_LEVEL_ERROR, "virt_addr can't be null\n");
    ret = hi_mpi_sys_mmz_alloc((hi_phys_addr_t *)phys_addr, virt_addr, mmb, zone, size);

    return ret;
}

/* Malloc memory with cached */
hi_s32 sample_common_svp_malloc_cached(hi_char *mmb, hi_char *zone, hi_phys_addr_t *phys_addr,
    hi_void **virt_addr, hi_u32 size)
{
    hi_s32 ret = HI_FAILURE;

    sample_svp_check_exps_return(phys_addr == HI_NULL, ret, SAMPLE_SVP_ERR_LEVEL_ERROR, "phys_addr can't be null\n");
    sample_svp_check_exps_return(virt_addr == HI_NULL, ret, SAMPLE_SVP_ERR_LEVEL_ERROR, "virt_addr can't be null\n");
    ret = hi_mpi_sys_mmz_alloc_cached((hi_phys_addr_t *)phys_addr, virt_addr, mmb, zone, size);

    return ret;
}

/* Fulsh cached */
hi_s32 sample_common_svp_flush_cache(hi_phys_addr_t phys_addr, hi_void *virt_addr, hi_u32 size)
{
    hi_s32 ret = HI_FAILURE;

    sample_svp_check_exps_return(virt_addr == HI_NULL, ret, SAMPLE_SVP_ERR_LEVEL_ERROR, "virt_addr can't be null\n");
    ret = hi_mpi_sys_flush_cache((hi_phys_addr_t)phys_addr, virt_addr, size);
    return ret;
}

/*
 * function : Init Vb
 */
static hi_s32 sample_common_svp_vb_init(hi_pic_size *pic_type, hi_size *pic_size,
    hi_u32 vpss_chn_num)
{
    hi_s32 ret;
    hi_u32 i;
    hi_vb_cfg vb_cfg = {0};
    hi_pic_buf_attr pic_buf_attr;
    hi_vb_calc_cfg calc_cfg;
    hi_vi_vpss_mode_type mode_type = HI_VI_ONLINE_VPSS_OFFLINE;
    hi_vi_aiisp_mode aiisp_mode = HI_VI_AIISP_MODE_DEFAULT;

    vb_cfg.max_pool_cnt = HI_SAMPLE_IVE_MAX_POOL_CNT;

    ret = sample_comm_sys_get_pic_size(pic_type[0], &pic_size[0]);
    sample_svp_check_exps_goto(ret != HI_SUCCESS, vb_fail_0, SAMPLE_SVP_ERR_LEVEL_ERROR,
        "sample_comm_sys_get_pic_size failed,Error(%#x)!\n", ret);
    pic_buf_attr.width = pic_size[0].width;
    pic_buf_attr.height = pic_size[0].height;
    pic_buf_attr.align = HI_DEFAULT_ALIGN;
    pic_buf_attr.bit_width = HI_DATA_BIT_WIDTH_8;
    pic_buf_attr.pixel_format = HI_PIXEL_FORMAT_YVU_SEMIPLANAR_422;
    pic_buf_attr.compress_mode = HI_COMPRESS_MODE_NONE;
    pic_buf_attr.video_format = HI_VIDEO_FORMAT_LINEAR;
    hi_common_get_pic_buf_cfg(&pic_buf_attr, &calc_cfg);

    vb_cfg.common_pool[0].blk_size = calc_cfg.vb_size;
    vb_cfg.common_pool[0].blk_cnt = SAMPLE_SVP_BLK_CNT;

    for (i = 1; (i < vpss_chn_num) && (i < HI_VB_MAX_COMMON_POOLS); i++) {
        ret = sample_comm_sys_get_pic_size(pic_type[i], &pic_size[i]);
        sample_svp_check_exps_goto(ret != HI_SUCCESS, vb_fail_0, SAMPLE_SVP_ERR_LEVEL_ERROR,
            "sample_comm_sys_get_pic_size failed,Error(%#x)!\n", ret);
        pic_buf_attr.width = pic_size[i].width;
        pic_buf_attr.height = pic_size[i].height;
        pic_buf_attr.compress_mode = HI_COMPRESS_MODE_NONE;
        pic_buf_attr.align = HI_DEFAULT_ALIGN;

        hi_common_get_pic_buf_cfg(&pic_buf_attr, &calc_cfg);

        /* comm video buffer */
        vb_cfg.common_pool[i].blk_size = calc_cfg.vb_size;
        vb_cfg.common_pool[i].blk_cnt = SAMPLE_SVP_BLK_CNT;
    }

    ret = sample_comm_sys_init_with_vb_supplement(&vb_cfg, HI_VB_SUPPLEMENT_BNR_MOT_MASK);
    sample_svp_check_exps_goto(ret != HI_SUCCESS, vb_fail_1, SAMPLE_SVP_ERR_LEVEL_ERROR,
        "sample_comm_sys_vb_init failed,Error(%#x)!\n", ret);

    ret = sample_comm_vi_set_vi_vpss_mode(mode_type, aiisp_mode);
    sample_svp_check_exps_goto(ret != HI_SUCCESS, vb_fail_1, SAMPLE_SVP_ERR_LEVEL_ERROR,
        "sample_comm_vi_set_vi_vpss_mode failed!\n");
    return ret;
vb_fail_1:
    sample_comm_sys_exit();
vb_fail_0:
    return ret;
}

hi_s32 sample_common_svp_vgs_fill_rect(const hi_video_frame_info *frame_info,
    hi_sample_svp_rect_info *rect, hi_u32 color)
{
    hi_vgs_handle vgs_handle = -1;
    hi_s32 ret = HI_FAILURE;
    hi_u16 i;
    hi_vgs_task_attr vgs_task;
    hi_cover vgs_add_cover;

    sample_svp_check_exps_return(frame_info == HI_NULL, ret, SAMPLE_SVP_ERR_LEVEL_ERROR, "frame_info can't be null\n");
    sample_svp_check_exps_return(rect == HI_NULL, ret, SAMPLE_SVP_ERR_LEVEL_ERROR, "rect can't be null\n");
    sample_svp_check_exps_return(rect->num > HI_SVP_RECT_NUM, ret,
        SAMPLE_SVP_ERR_LEVEL_ERROR, "rect->num can't lager than %u\n", HI_SVP_RECT_NUM);
    if (rect->num == 0) {
        return HI_SUCCESS;
    }

    ret = hi_mpi_vgs_begin_job(&vgs_handle);
    sample_svp_check_exps_return(ret != HI_SUCCESS, ret, SAMPLE_SVP_ERR_LEVEL_ERROR,
        "Vgs begin job fail,Error(%#x)\n", ret);

    ret = memcpy_s(&vgs_task.img_in, sizeof(hi_video_frame_info), frame_info, sizeof(hi_video_frame_info));
    sample_svp_check_exps_goto(ret != EOK, fail, SAMPLE_SVP_ERR_LEVEL_ERROR, "get img_in failed\n");
    ret = memcpy_s(&vgs_task.img_out, sizeof(hi_video_frame_info), frame_info, sizeof(hi_video_frame_info));
    sample_svp_check_exps_goto(ret != EOK, fail, SAMPLE_SVP_ERR_LEVEL_ERROR, "get img_out failed\n");

    vgs_add_cover.type = HI_COVER_QUAD;
    vgs_add_cover.color = color;
    for (i = 0; i < rect->num; i++) {
        vgs_add_cover.quad_attr.is_solid = HI_FALSE;
        vgs_add_cover.quad_attr.thick = HI_SAMPLE_IVE_DRAW_THICK;
        ret = memcpy_s(vgs_add_cover.quad_attr.point, sizeof(rect->rect[i].point),
            rect->rect[i].point, sizeof(rect->rect[i].point));
        sample_svp_check_exps_goto(ret != EOK, fail, SAMPLE_SVP_ERR_LEVEL_ERROR, "get point failed\n");
        ret = hi_mpi_vgs_add_cover_task(vgs_handle, &vgs_task, &vgs_add_cover, 1);
        sample_svp_check_exps_goto(ret != HI_SUCCESS, fail, SAMPLE_SVP_ERR_LEVEL_ERROR,
            "hi_mpi_vgs_add_cover_task fail,Error(%#x)\n", ret);
    }

    ret = hi_mpi_vgs_end_job(vgs_handle);
    sample_svp_check_exps_goto(ret != HI_SUCCESS, fail, SAMPLE_SVP_ERR_LEVEL_ERROR,
        "hi_mpi_vgs_end_job fail,Error(%#x)\n", ret);

    return ret;
fail:
    hi_mpi_vgs_cancel_job(vgs_handle);
    return ret;
}

/* function : Start Vpss */
static hi_s32 sample_common_svp_start_vpss(hi_s32 vpss_grp_cnt, hi_size *pic_size, hi_u32 vpss_chn_num)
{
    hi_u32 i;
    sample_vpss_chn_attr vpss_chn_attr = {0};
    hi_vpss_grp_attr vpss_grp_attr;
    hi_vpss_grp vpss_grp;
    hi_s32 ret;

    (hi_void)memset_s(&vpss_grp_attr, sizeof(hi_vpss_grp_attr), 0, sizeof(hi_vpss_grp_attr));
    sample_comm_vpss_get_default_grp_attr(&vpss_grp_attr);
    vpss_grp_attr.max_width = pic_size[0].width;
    vpss_grp_attr.max_height = pic_size[0].height;
    /* VPSS only onle channel0 support compress seg mode */
    sample_comm_vpss_get_default_chn_attr(&vpss_chn_attr.chn_attr[0]);
    vpss_chn_attr.chn_attr[0].width = pic_size[0].width;
    vpss_chn_attr.chn_attr[0].height = pic_size[0].height;
    vpss_chn_attr.chn_attr[0].depth = 1;
    vpss_chn_attr.chn_attr[0].compress_mode = HI_COMPRESS_MODE_NONE;

    for (i = 1; i < vpss_chn_num; i++) {
        (hi_void)memset_s(&vpss_chn_attr.chn_attr[i], sizeof(hi_vpss_chn_attr), 0, sizeof(hi_vpss_chn_attr));
        sample_comm_vpss_get_default_chn_attr(&vpss_chn_attr.chn_attr[i]);
        vpss_chn_attr.chn_attr[i].width = pic_size[i].width;
        vpss_chn_attr.chn_attr[i].height = pic_size[i].height;
        vpss_chn_attr.chn_attr[i].compress_mode = HI_COMPRESS_MODE_NONE;
        vpss_chn_attr.chn_attr[i].depth = 1;
    }

    vpss_chn_attr.chn_enable[0] = HI_TRUE;
    vpss_chn_attr.chn_enable[1] = HI_TRUE;
    vpss_chn_attr.chn_array_size = HI_VPSS_MAX_CHN_NUM;

    for (vpss_grp = 0; vpss_grp < vpss_grp_cnt; vpss_grp++) {
        ret = sample_common_vpss_start(vpss_grp, &vpss_grp_attr, &vpss_chn_attr);
        if (ret != HI_SUCCESS) {
            sample_svp_trace_err("failed with %#x!\n", ret);
            return HI_FAILURE;
        }
    }
    return HI_SUCCESS;
}

/* function : Stop Vpss */
static hi_void sample_common_svp_stop_vpss(hi_s32 vpss_grp_cnt)
{
    hi_vpss_grp vpss_grp = 0;
    hi_bool chn_enable[HI_VPSS_MAX_CHN_NUM] = { HI_TRUE, HI_TRUE, HI_FALSE, HI_FALSE };
    hi_s32 i;

    for (i = 0; (i < vpss_grp_cnt) && (i < HI_VPSS_MAX_CHN_NUM); i++) {
        sample_common_vpss_stop(vpss_grp, chn_enable, HI_VPSS_MAX_CHN_NUM);
        vpss_grp++;
    }
}

/*
 * function : Start Vo
 */
hi_s32 sample_common_svp_start_vo(sample_vo_cfg *vo_cfg)
{
    sample_svp_check_exps_return(vo_cfg == HI_NULL, HI_FAILURE, SAMPLE_SVP_ERR_LEVEL_ERROR, "vo_cfg can't be null\n");
    return sample_comm_vo_start_vo(vo_cfg);
}

/*
 * function : Stop Vo
 */
hi_void sample_common_svp_stop_vo(sample_vo_cfg *vo_cfg)
{
    sample_svp_check_exps_return_void(vo_cfg == HI_NULL, SAMPLE_SVP_ERR_LEVEL_ERROR, "vo_cfg can't be null\n");
    (hi_void)sample_comm_vo_stop_vo(vo_cfg);
}

static hi_s32 sample_common_svp_vi_bind_multi_vpss(hi_s32 vpss_grp_cnt, hi_s32 vi_chn_cnt,
    hi_s32 vi_chn_interval)
{
    hi_s32 ret;
    hi_s32 loop;
    hi_vi_chn vi_chn;
    hi_vpss_grp vpss_grp = 0;

    for (loop = 0; loop < vi_chn_cnt  && vpss_grp < vpss_grp_cnt; loop++) {
        vi_chn = loop * vi_chn_interval;
        ret = sample_comm_vi_bind_vpss(0, vi_chn, vpss_grp, 0);
        if (ret != HI_SUCCESS) {
            sample_svp_trace_err("vi bind vpss failed!\n");
            return ret;
        }
        vpss_grp++;
    }

    return HI_SUCCESS;
}

static hi_s32 sample_common_svp_vi_unbind_multi_vpss(hi_s32 vpss_grp_cnt, hi_s32 vi_chn_cnt,
    hi_s32 vi_chn_interval)
{
    hi_s32 ret;
    hi_s32 loop;
    hi_vi_chn vi_chn;
    hi_vpss_grp vpss_grp = 0;

    for (loop = 0; loop < vi_chn_cnt && vpss_grp < vpss_grp_cnt; loop++) {
        vi_chn = loop * vi_chn_interval;
        ret = sample_comm_vi_un_bind_vpss(0, vi_chn, vpss_grp, 0);
        if (ret != HI_SUCCESS) {
            sample_svp_trace_err("vi bind vpss failed!\n");
            return ret;
        }

        vpss_grp++;
    }

    return HI_SUCCESS;
}

static hi_s32 sample_common_svp_set_vi_cfg(sample_vi_cfg *vi_cfg, hi_pic_size *pic_type,
    hi_u32 pic_type_len, hi_pic_size *ext_pic_size_type, sample_sns_type sns_type)
{
    sample_comm_vi_get_default_vi_cfg(sns_type, vi_cfg);
    sample_svp_check_exps_return(pic_type_len < HI_VPSS_CHN_NUM,
        HI_FAILURE, SAMPLE_SVP_ERR_LEVEL_ERROR, "pic_type_len is illegal!\n");
    pic_type[1] = *ext_pic_size_type;

    return HI_SUCCESS;
}

static hi_s32 sample_common_svp_get_def_vo_cfg(sample_vo_cfg *vo_cfg)
{
    hi_rect  def_disp_rect  = {0, 0, 1920, 1080};
    hi_size  def_img_size = {1920, 1080};

    if (vo_cfg == HI_NULL) {
        sample_svp_trace_err("error:argument can not be HI_NULL\n");
        return HI_FAILURE;
    }

    vo_cfg->vo_dev  = SAMPLE_VO_DEV_UHD;
    vo_cfg->vo_layer = SAMPLE_VO_LAYER_VHD0;

    vo_cfg->vo_intf_type      = HI_VO_INTF_BT1120;
    vo_cfg->intf_sync         = HI_VO_OUT_1080P60;
    vo_cfg->bg_color          = COLOR_RGB_BLACK;
    vo_cfg->pix_format        = HI_PIXEL_FORMAT_YVU_SEMIPLANAR_420;
    vo_cfg->disp_rect         = def_disp_rect;
    vo_cfg->image_size        = def_img_size;
    vo_cfg->vo_part_mode      = HI_VO_PARTITION_MODE_SINGLE;
    vo_cfg->dis_buf_len       = 3; /* 3: def buf len for single */
    vo_cfg->dst_dynamic_range = HI_DYNAMIC_RANGE_SDR8;
    vo_cfg->vo_mode           = VO_MODE_1MUX;
    vo_cfg->compress_mode     = HI_COMPRESS_MODE_NONE;

    return HI_SUCCESS;
}

static hi_s32 sample_common_svp_set_and_start_vo(const hi_sample_svp_switch *switch_ptr,
    sample_vo_cfg *vo_cfg)
{
    hi_s32 ret;

    /*  Start Vo */
    if (switch_ptr->is_vo_open == HI_TRUE) {
        sample_common_svp_get_def_vo_cfg(vo_cfg);
        ret = sample_common_svp_start_vo(vo_cfg);
        sample_svp_check_exps_return(ret != HI_SUCCESS, ret, SAMPLE_SVP_ERR_LEVEL_ERROR,
            "Error(%#x),sample_common_svp_start_vo failed!\n", ret);
    }
    return HI_SUCCESS;
}

static hi_s32 sample_common_svp_start_venc(const hi_sample_svp_switch *switch_ptr,
    sample_vo_cfg *vo_cfg)
{
    hi_s32 ret = HI_SUCCESS;
    hi_venc_chn h264_chn = 0;
    sample_comm_venc_chn_param chn_param;
    hi_size ven_size = {1920, 1080};

    chn_param.frame_rate = 30; /* 30 is a number */
    chn_param.stats_time = 2; /* 2 is a number */
    chn_param.gop = 60; /* 60 is a number */
    chn_param.venc_size = ven_size;
    chn_param.size = PIC_1080P;
    chn_param.profile = 0;
    chn_param.is_rcn_ref_share_buf = HI_TRUE;

    chn_param.type = HI_PT_H264;
    chn_param.rc_mode = SAMPLE_RC_CBR;

    if (switch_ptr->is_venc_open  == HI_TRUE) {
        ret = sample_comm_venc_get_gop_attr(HI_VENC_GOP_MODE_NORMAL_P, &chn_param.gop_attr);
        sample_svp_check_exps_goto(ret != HI_SUCCESS, end_0, SAMPLE_SVP_ERR_LEVEL_ERROR,
            "Error(%#x),sample_comm_venc_get_gop_attr failed!\n", ret);

        ret = sample_comm_venc_start(h264_chn, &chn_param);
        sample_svp_check_exps_goto(ret != HI_SUCCESS, end_0, SAMPLE_SVP_ERR_LEVEL_ERROR,
            "Error(%#x),sample_comm_venc_start failed!\n", ret);

        ret = sample_comm_venc_start_get_stream(&h264_chn, 1);
        sample_svp_check_exps_goto(ret != HI_SUCCESS, end_1, SAMPLE_SVP_ERR_LEVEL_ERROR,
            "Error(%#x),sample_comm_venc_start_get_stream failed!\n", ret);
    }
    return ret;

end_1:
    if (switch_ptr->is_venc_open == HI_TRUE) {
        sample_comm_venc_stop(h264_chn);
    }
end_0:
    if (switch_ptr->is_vo_open == HI_TRUE) {
        sample_common_svp_stop_vo(vo_cfg);
    }
    return ret;
}

static hi_s32 sample_common_svp_get_pic_type_by_sns_type(sample_sns_type sns_type, hi_pic_size size[], hi_u32 num)
{
    sample_svp_check_exps_return(num > HI_VPSS_CHN_NUM, HI_FAILURE, SAMPLE_SVP_ERR_LEVEL_ERROR,
        "num(%u) can't be larger than (%u)\n", num, HI_VPSS_CHN_NUM);
    switch (sns_type) {
        case OV_OS08A20_MIPI_8M_30FPS_12BIT:
        case OV_OS08A20_MIPI_8M_30FPS_10BIT_WDR2TO1:
            size[0] = PIC_3840X2160;
            break;
        default:
            size[0] = PIC_3840X2160;
            break;
    }
    return HI_SUCCESS;
}

/*
 * function : Start Vi/Vpss/Venc/Vo
 */
hi_s32 sample_common_svp_start_vi_vpss_venc_vo(sample_vi_cfg *vi_cfg,
    hi_sample_svp_switch *switch_ptr, hi_pic_size *ext_pic_size_type)
{
    hi_size pic_size[HI_VPSS_CHN_NUM];
    hi_pic_size pic_type[HI_VPSS_CHN_NUM];
    sample_vo_cfg vo_cfg;

    const hi_s32 vpss_grp_cnt = 1;
    hi_s32 ret = HI_FAILURE;
    sample_sns_type sns_type = SENSOR0_TYPE;

    sample_svp_check_exps_return(vi_cfg == HI_NULL, ret, SAMPLE_SVP_ERR_LEVEL_ERROR, "vi_cfg can't be null\n");
    sample_svp_check_exps_return(switch_ptr == HI_NULL, ret, SAMPLE_SVP_ERR_LEVEL_ERROR, "switch_ptr can't be null\n");
    sample_svp_check_exps_return(ext_pic_size_type == HI_NULL, ret,
        SAMPLE_SVP_ERR_LEVEL_ERROR, "ext_pic_size_type can't be null\n");

    ret = sample_common_svp_get_pic_type_by_sns_type(sns_type, pic_type, HI_VPSS_CHN_NUM);
    sample_svp_check_exps_return(ret != HI_SUCCESS, ret, SAMPLE_SVP_ERR_LEVEL_ERROR,
        "sample_common_svp_get_pic_type_by_sns_type failed!\n");
    ret = sample_common_svp_set_vi_cfg(vi_cfg, pic_type, HI_VPSS_CHN_NUM, ext_pic_size_type, sns_type);
    sample_svp_check_exps_return(ret != HI_SUCCESS, ret, SAMPLE_SVP_ERR_LEVEL_ERROR,
        "sample_common_svp_set_vi_cfg failed,Error:%#x\n", ret);

    /* step  1: Init vb */
    ret = sample_common_svp_vb_init(pic_type, pic_size, HI_VPSS_CHN_NUM);
    sample_svp_check_exps_return(ret != HI_SUCCESS, ret, SAMPLE_SVP_ERR_LEVEL_ERROR,
        "Error(%#x),sample_common_svp_vb_init failed!\n", ret);

    /* step 2: Start vi */
    ret = sample_comm_vi_start_vi(vi_cfg);
    sample_svp_check_exps_goto(ret != HI_SUCCESS, end_init_1, SAMPLE_SVP_ERR_LEVEL_ERROR,
        "Error(%#x),sample_comm_vi_start_vi failed!\n", ret);

    /* step 3: Bind vpss to vi */
    ret = sample_common_svp_vi_bind_multi_vpss(vpss_grp_cnt, 1, 1);
    sample_svp_check_exps_goto(ret != HI_SUCCESS, end_init_2, SAMPLE_SVP_ERR_LEVEL_ERROR,
        "Error(%#x),sample_common_vi_bind_multi_vpss failed!\n", ret);

    /* step 4: Start vpss */
    ret = sample_common_svp_start_vpss(vpss_grp_cnt, pic_size, HI_VPSS_CHN_NUM);
    sample_svp_check_exps_goto(ret != HI_SUCCESS, end_init_3, SAMPLE_SVP_ERR_LEVEL_ERROR,
        "Error(%#x),sample_common_svp_start_vpss failed!\n", ret);

    /* step 5: Set vi frame, Start Vo */
    ret = sample_common_svp_set_and_start_vo(switch_ptr, &vo_cfg);
    sample_svp_check_exps_goto(ret != HI_SUCCESS, end_init_4, SAMPLE_SVP_ERR_LEVEL_ERROR,
        "Error(%#x),sample_common_svp_set_vi_frame failed!\n", ret);

    /* step 6: Start Venc */
    ret = sample_common_svp_start_venc(switch_ptr, &vo_cfg);
    sample_svp_check_exps_goto(ret != HI_SUCCESS, end_init_4, SAMPLE_SVP_ERR_LEVEL_ERROR,
        "Error(%#x),sample_common_svp_start_vencb failed!\n", ret);

    return HI_SUCCESS;
end_init_4:
    sample_common_svp_stop_vpss(vpss_grp_cnt);
end_init_3:
    ret = sample_common_svp_vi_unbind_multi_vpss(vpss_grp_cnt, 1, 1);
    sample_svp_check_exps_trace(ret != HI_SUCCESS, SAMPLE_SVP_ERR_LEVEL_ERROR, "svp_vi_unbind_multi_vpss failed!\n");
end_init_2:
    sample_comm_vi_stop_vi(vi_cfg);
end_init_1:  /*  system exit */
    sample_comm_sys_exit();
    (hi_void)memset_s(vi_cfg, sizeof(sample_vi_cfg), 0, sizeof(sample_vi_cfg));
    return ret;
}
static hi_void sample_common_svp_stop_venc(const hi_sample_svp_switch *vo_venc_switch)
{
    if (vo_venc_switch->is_venc_open == HI_TRUE) {
        sample_comm_venc_stop_get_stream(1);
        sample_comm_venc_stop(0);
    }
}
/*
 * function : Stop Vi/Vpss/Venc/Vo
 */
hi_void sample_common_svp_stop_vi_vpss_venc_vo(sample_vi_cfg *vi_cfg,
    hi_sample_svp_switch *switch_ptr)
{
    sample_vo_cfg vo_cfg = {0};
    hi_s32 ret;
    const hi_s32 vpss_grp_cnt = 1;

    sample_svp_check_exps_return_void(vi_cfg == HI_NULL, SAMPLE_SVP_ERR_LEVEL_ERROR, "vi_cfg can't be null\n");
    sample_svp_check_exps_return_void(switch_ptr == HI_NULL, SAMPLE_SVP_ERR_LEVEL_ERROR, "switch_ptr can't be null\n");

    if (switch_ptr->is_venc_open == HI_TRUE) {
        sample_comm_venc_stop_get_stream(1);
        sample_comm_venc_stop(0);
    }

    if (switch_ptr->is_vo_open == HI_TRUE) {
        (hi_void)sample_common_svp_get_def_vo_cfg(&vo_cfg);
        sample_common_svp_stop_vo(&vo_cfg);
    }

    ret = sample_common_svp_vi_unbind_multi_vpss(vpss_grp_cnt, 1, 1);
    sample_svp_check_exps_trace(ret != HI_SUCCESS, SAMPLE_SVP_ERR_LEVEL_ERROR,
        "sample_common_svp_vi_unbind_multi_vpss failed\n");
    sample_common_svp_stop_vpss(vpss_grp_cnt);
    sample_comm_vi_stop_vi(vi_cfg);
    sample_comm_sys_exit();

    (hi_void)memset_s(vi_cfg, sizeof(sample_vi_cfg), 0, sizeof(sample_vi_cfg));
}
static hi_s32 sample_common_svp_start_vdec(hi_u32 chn_num, sample_vdec_attr *vdec_attr, hi_u32 arr_len)
{
    hi_s32 ret;
    ret = sample_comm_vdec_init_vb_pool(chn_num, vdec_attr, arr_len);
    sample_svp_check_exps_return(ret != HI_SUCCESS, ret, SAMPLE_SVP_ERR_LEVEL_ERROR,
        "Error(%#x),sample_comm_vdec_init_vb_pool failed!\n", ret);

    ret = sample_comm_vdec_start(chn_num, vdec_attr, arr_len);
    sample_svp_check_exps_goto(ret != HI_SUCCESS, end_1, SAMPLE_SVP_ERR_LEVEL_ERROR,
        "Error(%#x), sample_comm_vdec_start failed!\n", ret);

    ret = hi_mpi_vdec_set_display_mode(SAMPLE_SVP_VDEC_CHN_0, HI_VIDEO_DISPLAY_MODE_PREVIEW);
    sample_svp_check_exps_goto(ret != HI_SUCCESS, end_1, SAMPLE_SVP_ERR_LEVEL_ERROR,
        "Error(%#x), sample_comm_vdec_start failed!\n", ret);

    return ret;
end_1:
    sample_comm_vdec_exit_vb_pool();
    return ret;
}

static hi_void sample_common_svp_stop_vdec(hi_u32 chn_num)
{
    hi_s32 ret;
    ret = sample_comm_vdec_stop(chn_num);
    sample_svp_check_exps_trace(ret != HI_SUCCESS, SAMPLE_SVP_ERR_LEVEL_ERROR, "sample_comm_vdec_stop failed\n");
    sample_comm_vdec_exit_vb_pool();
}

static hi_s32 sample_common_svp_vpss_bind_vdec(hi_s32 vpss_grp_cnt, hi_s32 vdec_chn)
{
    hi_s32 i;
    hi_s32 j;
    hi_s32 ret;
    for (i = 0; i < vpss_grp_cnt; i++) {
        ret = sample_comm_vdec_bind_vpss(vdec_chn, i);
        sample_svp_check_exps_goto(ret != HI_SUCCESS, end_1, SAMPLE_SVP_ERR_LEVEL_ERROR,
            "Error(%#x), sample_comm_vdec_bind_vpss failed!\n", ret);
    }
    return ret;
end_1:
    for (j = 0; j < i; j++) {
        sample_comm_vdec_un_bind_vpss(vdec_chn, j);
    }
    return ret;
}

static hi_void sample_common_svp_vpss_unbind_vdec(hi_s32 vpss_grp_cnt, hi_s32 vdec_chn)
{
    hi_s32 i;
    for (i = 0; i < vpss_grp_cnt; i++) {
        sample_comm_vdec_un_bind_vpss(vdec_chn, i);
    }
}

static hi_s32 sample_common_svp_create_vb(hi_sample_svp_media_cfg *media_cfg)
{
    hi_s32 ret = HI_INVALID_VALUE;
    hi_u32 i, j;
    hi_u64 blk_size;
    hi_vb_pool_cfg vb_pool_cfg = { 0 };
    hi_pic_buf_attr pic_buf_attr = {0};

    sample_svp_check_exps_return(media_cfg == HI_NULL, ret, SAMPLE_SVP_ERR_LEVEL_ERROR, "media_cfg is NULL!\n");

    sample_svp_check_exps_return((media_cfg->chn_num == 0) || (media_cfg->chn_num > HI_SVP_MAX_VPSS_CHN_NUM), ret,
        SAMPLE_SVP_ERR_LEVEL_ERROR, "media_cfg->chn_num must be [1, %d],Error(%#x)!\n", HI_SVP_MAX_VPSS_CHN_NUM, ret);

    for (i = 0; i < media_cfg->chn_num; i++) {
        if (media_cfg->pic_type[i] != PIC_BUTT) {
            ret = sample_comm_sys_get_pic_size(media_cfg->pic_type[i], &media_cfg->pic_size[i]);
            sample_svp_check_exps_goto(ret != HI_SUCCESS, vb_fail_0, SAMPLE_SVP_ERR_LEVEL_ERROR,
                "sample_comm_sys_get_pic_size failed,Error(%#x)!\n", ret);
        }
        pic_buf_attr.width = media_cfg->pic_size[i].width;
        pic_buf_attr.height = media_cfg->pic_size[i].height;
        pic_buf_attr.compress_mode = HI_COMPRESS_MODE_NONE;
        pic_buf_attr.align = HI_DEFAULT_ALIGN;
        pic_buf_attr.bit_width = HI_DATA_BIT_WIDTH_8;
        pic_buf_attr.pixel_format = HI_PIXEL_FORMAT_YVU_SEMIPLANAR_422;
        pic_buf_attr.video_format = HI_VIDEO_FORMAT_LINEAR;
        blk_size = hi_common_get_pic_buf_size(&pic_buf_attr);
        vb_pool_cfg.blk_size = blk_size;
        vb_pool_cfg.blk_cnt = SAMPLE_SVP_BLK_CNT;
        media_cfg->vb_pool[i] = hi_mpi_vb_create_pool(&vb_pool_cfg);

        (hi_void)hi_mpi_vb_pool_share_all(media_cfg->vb_pool[i]);
        sample_svp_check_exps_goto(media_cfg->vb_pool[i] == HI_VB_INVALID_POOL_ID, vb_fail_0,
            SAMPLE_SVP_ERR_LEVEL_ERROR, "create %u-th vb pool failed!\n", i);
    }
    return HI_SUCCESS;
vb_fail_0:
    for (j = 0; j < i; j++) {
        (hi_void)hi_mpi_vb_destroy_pool(media_cfg->vb_pool[j]);
    }
    return HI_FAILURE;
}

static hi_void sample_common_svp_destroy_vb(hi_sample_svp_media_cfg *media_cfg)
{
    hi_u32 i;
    for (i = 0; i < media_cfg->chn_num; i++) {
        (hi_void)hi_mpi_vb_destroy_pool(media_cfg->vb_pool[i]);
    }
}
static hi_s32 sample_common_svp_vpss_attach_vb(hi_sample_svp_media_cfg *media_cfg, hi_s32 vpss_grp_cnt)
{
    hi_vpss_grp grp, grp_tmp;
    hi_vpss_chn chn, chn_tmp;
    hi_s32 ret;

    for (grp = 0; grp < vpss_grp_cnt; grp++) {
        for (chn = 0; chn < media_cfg->chn_num; chn++) {
            (hi_void)hi_mpi_vpss_set_chn_vb_src(grp, chn, HI_VB_SRC_USER);
            ret = hi_mpi_vpss_attach_chn_vb_pool(grp, chn, media_cfg->vb_pool[chn]);
            if (ret == HI_SUCCESS) {
                continue;
            }
            for (chn_tmp = 0; chn_tmp < chn; chn_tmp++) {
                (hi_void)hi_mpi_vpss_detach_chn_vb_pool(grp, chn_tmp);
            }
            sample_svp_check_exps_goto(ret != HI_SUCCESS, end_0, SAMPLE_SVP_ERR_LEVEL_ERROR,
                "Error(%#x), hi_mpi_vpss_attach_chn_vb_pool failed!\n", ret);
        }
    }
    return ret;
end_0:
    for (grp_tmp = 0; grp_tmp < grp; grp_tmp++) {
        for (chn_tmp = 0; chn_tmp < media_cfg->chn_num; chn_tmp++) {
            (hi_void)hi_mpi_vpss_detach_chn_vb_pool(grp_tmp, chn_tmp);
        }
    }
    return ret;
}

static hi_void sample_common_svp_vpss_detach_vb(hi_sample_svp_media_cfg *media_cfg, hi_s32 vpss_grp_cnt)
{
    hi_vpss_grp grp;
    hi_vpss_chn chn;

    for (grp = 0; grp < vpss_grp_cnt; grp++) {
        for (chn = 0; chn < media_cfg->chn_num; chn++) {
            (hi_void)hi_mpi_vpss_detach_chn_vb_pool(grp, chn);
        }
    }
}

hi_s32 sample_common_svp_create_vb_start_vdec_vpss_vo(sample_vdec_attr *vdec_attr, vdec_thread_param *vdec_send,
    pthread_t *vdec_thread, hi_sample_svp_media_cfg *media_cfg, sample_vo_cfg *vo_cfg)
{
    const hi_s32 vpss_grp_cnt = 1;
    hi_s32 ret;
    /* step  1: Init vb */
    ret = sample_common_svp_create_vb(media_cfg);
    sample_svp_check_exps_return(ret != HI_SUCCESS, ret, SAMPLE_SVP_ERR_LEVEL_ERROR,
        "Error(%#x),sample_common_svp_create_vb failed!\n", ret);
    /* step  2: start vdec */
    ret = sample_common_svp_start_vdec(SAMPLE_SVP_VDEC_CHN_NUM, vdec_attr, SAMPLE_SVP_VDEC_CHN_NUM);
    sample_svp_check_exps_goto(ret != HI_SUCCESS, end_init_1, SAMPLE_SVP_ERR_LEVEL_ERROR,
        "Error(%#x), sample_common_svp_start_vdec failed!\n", ret);
    /* step 3: Start vpss */
    ret = sample_common_svp_start_vpss(vpss_grp_cnt, media_cfg->pic_size, media_cfg->chn_num);
    sample_svp_check_exps_goto(ret != HI_SUCCESS, end_init_2, SAMPLE_SVP_ERR_LEVEL_ERROR,
        "Error(%#x),sample_common_svp_start_vpss failed!\n", ret);

    ret = sample_common_svp_vpss_attach_vb(media_cfg, vpss_grp_cnt);
    sample_svp_check_exps_goto(ret != HI_SUCCESS, end_init_3, SAMPLE_SVP_ERR_LEVEL_ERROR,
        "Error(%#x),sample_common_svp_start_vpss failed!\n", ret);

    /* step 4: Bind vpss to vdec */
    ret = sample_common_svp_vpss_bind_vdec(vpss_grp_cnt, SAMPLE_SVP_VDEC_CHN_0);
    sample_svp_check_exps_goto(ret != HI_SUCCESS, end_init_4, SAMPLE_SVP_ERR_LEVEL_ERROR,
        "Error(%#x),sample_comm_vdec_bind_vpss failed!\n", ret);

    /* step 5: Start vo */
    ret = sample_common_svp_start_vo(vo_cfg);
    sample_svp_check_exps_goto(ret != HI_SUCCESS, end_init_5, SAMPLE_SVP_ERR_LEVEL_ERROR,
        "Error(%#x),sample_common_svp_start_vo failed!\n", ret);

    /* step 5: Start Send thread */
    sample_comm_vdec_start_send_stream(SAMPLE_SVP_VDEC_CHN_NUM, vdec_send, vdec_thread, SAMPLE_SVP_VDEC_CHN_NUM,
        SAMPLE_SVP_VDEC_CHN_NUM);
    /* step 6: Start Venc */
    ret = sample_common_svp_start_venc(&(media_cfg->svp_switch), vo_cfg);
    sample_svp_check_exps_goto(ret != HI_SUCCESS, end_init_6, SAMPLE_SVP_ERR_LEVEL_ERROR,
        "Error(%#x),sample_common_svp_start_venc failed!\n", ret);

    return ret;
end_init_6:
    sample_comm_vdec_stop_send_stream(SAMPLE_SVP_VDEC_CHN_NUM, vdec_send, vdec_thread, SAMPLE_SVP_VDEC_CHN_NUM,
        SAMPLE_SVP_VDEC_CHN_NUM);
    sample_common_svp_stop_vo(vo_cfg);
end_init_5:
    sample_common_svp_vpss_unbind_vdec(vpss_grp_cnt, SAMPLE_SVP_VDEC_CHN_0);
end_init_4:
    sample_common_svp_vpss_detach_vb(media_cfg, vpss_grp_cnt);
end_init_3:
    sample_common_svp_stop_vpss(vpss_grp_cnt);
end_init_2:
    sample_common_svp_stop_vdec(SAMPLE_SVP_VDEC_CHN_NUM);
end_init_1:
    sample_common_svp_destroy_vb(media_cfg);
    return ret;
}

hi_void sample_common_svp_destroy_vb_stop_vdec_vpss_vo(vdec_thread_param *vdec_send, pthread_t *vdec_thread,
    hi_sample_svp_media_cfg *media_cfg, sample_vo_cfg *vo_cfg)
{
    const hi_s32 vpss_grp_cnt = 1;

    sample_svp_check_exps_return_void(media_cfg == HI_NULL, SAMPLE_SVP_ERR_LEVEL_ERROR, "media_cfg is NULL!\n");

    sample_svp_check_exps_return_void((media_cfg->chn_num == 0) || (media_cfg->chn_num > HI_SVP_MAX_VPSS_CHN_NUM),
        SAMPLE_SVP_ERR_LEVEL_ERROR, "media_cfg->chn_num must be [1, %u]!\n", HI_SVP_MAX_VPSS_CHN_NUM);

    sample_common_svp_stop_venc(&(media_cfg->svp_switch));

    sample_comm_vdec_stop_send_stream(SAMPLE_SVP_VDEC_CHN_NUM, vdec_send, vdec_thread, SAMPLE_SVP_VDEC_CHN_NUM,
        SAMPLE_SVP_VDEC_CHN_NUM);
    sample_common_svp_stop_vo(vo_cfg);
    sample_common_svp_vpss_unbind_vdec(vpss_grp_cnt, SAMPLE_SVP_VDEC_CHN_0);
    sample_common_svp_vpss_detach_vb(media_cfg, vpss_grp_cnt);
    sample_common_svp_stop_vpss(vpss_grp_cnt);
    sample_common_svp_stop_vdec(SAMPLE_SVP_VDEC_CHN_NUM);
    sample_common_svp_destroy_vb(media_cfg);
}

hi_s32 sample_common_svp_venc_vo_send_stream(const hi_sample_svp_switch *vo_venc_switch, hi_venc_chn venc_chn,
    hi_vo_layer vo_layer, hi_vo_chn vo_chn, const hi_video_frame_info *frame)
{
    hi_s32 ret = HI_INVALID_VALUE;

    sample_svp_check_exps_return((vo_venc_switch == HI_NULL || frame == HI_NULL), ret, SAMPLE_SVP_ERR_LEVEL_ERROR,
        "vo_venc_switch is NULL!\n");
    if (vo_venc_switch->is_venc_open) {
        ret = hi_mpi_venc_send_frame(venc_chn, frame, HI_SVP_TIMEOUT);
        sample_svp_check_exps_return(ret != HI_SUCCESS, ret, SAMPLE_SVP_ERR_LEVEL_ERROR,
            "Error(%#x),hi_mpi_venc_send_frame failed!\n", ret);
    }
    if (vo_venc_switch->is_vo_open) {
        ret = hi_mpi_vo_send_frame(vo_layer, vo_chn, frame, HI_SVP_TIMEOUT);
        sample_svp_check_exps_return(ret != HI_SUCCESS, ret, SAMPLE_SVP_ERR_LEVEL_ERROR,
            "Error(%#x),hi_mpi_vo_send_frame failed!\n", ret);
    }
    return HI_SUCCESS;
}

static hi_s32 sample_common_svp_npu_get_pic_size(hi_sample_svp_media_cfg *media_cfg, hi_u64 *blk_size)
{
    hi_s32 ret;
    hi_pic_buf_attr pic_buf_attr = {0};

    ret = sample_comm_sys_get_pic_size(media_cfg->pic_type[0], &media_cfg->pic_size[0]);
    sample_svp_check_exps_return(ret != HI_SUCCESS, ret, SAMPLE_SVP_ERR_LEVEL_ERROR,
        "sample_comm_sys_get_pic_size failed,Error(%#x)!\n", ret);
    pic_buf_attr.width = media_cfg->pic_size[0].width;
    pic_buf_attr.height = media_cfg->pic_size[0].height;
    pic_buf_attr.compress_mode = HI_COMPRESS_MODE_NONE;
    pic_buf_attr.align = HI_DEFAULT_ALIGN;
    pic_buf_attr.bit_width = HI_DATA_BIT_WIDTH_8;
    pic_buf_attr.pixel_format = HI_PIXEL_FORMAT_YVU_SEMIPLANAR_422;
    pic_buf_attr.video_format = HI_VIDEO_FORMAT_LINEAR;
    *blk_size = hi_common_get_pic_buf_size(&pic_buf_attr);
    return HI_SUCCESS;
}

static hi_s32 sample_common_svp_npu_reset_vb(hi_sample_svp_media_cfg *media_cfg, hi_u64 *blk_size,
    hi_vb_cfg *vb_default_cfg)
{
    hi_s32 ret;
    ret = sample_comm_svp_sys_exit();
    sample_svp_check_exps_return(ret != HI_SUCCESS, ret, SAMPLE_SVP_ERR_LEVEL_ERROR,
        "Error(%#x):sample_comm_svp_sys_exit failed!\n", ret);
    ret = sample_common_svp_npu_get_pic_size(media_cfg, blk_size);
    sample_svp_check_exps_return(ret != HI_SUCCESS, ret, SAMPLE_SVP_ERR_LEVEL_ERROR,
        "Error(%#x):sample_common_svp_npu_get_pic_size failed!\n", ret);
    vb_default_cfg->max_pool_cnt = SAMPLE_SVP_VB_POOL_NUM;
    vb_default_cfg->common_pool[1].blk_cnt = SAMPLE_SVP_BLK_CNT;
    vb_default_cfg->common_pool[1].blk_size = *blk_size;
    ret = hi_mpi_vb_set_cfg(vb_default_cfg);
    sample_svp_check_exps_return(ret != HI_SUCCESS, ret, SAMPLE_SVP_ERR_LEVEL_ERROR,
        "Error(%#x):hi_mpi_vb_set_cfg failed!\n", ret);
    ret = hi_mpi_vb_init();
    sample_svp_check_exps_return(ret != HI_SUCCESS, ret, SAMPLE_SVP_ERR_LEVEL_ERROR,
        "Error(%#x):hi_mpi_vb_init failed!\n", ret);
    ret = hi_mpi_sys_init();
    sample_svp_check_exps_return(ret != HI_SUCCESS, ret, SAMPLE_SVP_ERR_LEVEL_ERROR,
        "Error(%#x):hi_mpi_sys_init failed!\n", ret);
    return HI_SUCCESS;
}

/*
 * function : Start Vi/Vpss/Vo
 */
hi_s32 sample_common_svp_npu_start_vi_vpss_vo(sample_vi_cfg *vi_cfg, hi_sample_svp_media_cfg *media_cfg,
    sample_vo_cfg *vo_cfg)
{
    hi_s32 ret;
    const hi_s32 vpss_grp_cnt = 1;
    sample_sns_type sns_type = SENSOR0_TYPE;
    hi_vb_cfg vb_default_cfg;
    hi_u64 blk_size;

    ret = sample_common_svp_get_pic_type_by_sns_type(sns_type, media_cfg->pic_type, HI_SVP_MAX_VPSS_CHN_NUM);
    sample_svp_check_exps_return(ret != HI_SUCCESS, ret, SAMPLE_SVP_ERR_LEVEL_ERROR,
        "sample_common_svp_get_pic_type_by_sns_type failed!\n");
    sample_comm_vi_get_default_vi_cfg(sns_type, vi_cfg);

    ret = sample_common_svp_npu_reset_vb(media_cfg, &blk_size, &vb_default_cfg);
    sample_svp_check_exps_return(ret != HI_SUCCESS, ret, SAMPLE_SVP_ERR_LEVEL_ERROR,
        "sample_common_svp_npu_reset_vb failed!\n");

    /* step  1: Init vb */
    ret = sample_common_svp_create_vb(media_cfg);
    sample_svp_check_exps_return(ret != HI_SUCCESS, ret, SAMPLE_SVP_ERR_LEVEL_ERROR,
        "Error(%#x),sample_common_svp_create_vb failed!\n", ret);

    /* step 2: Start vi */
    ret = sample_comm_vi_start_vi(vi_cfg);
    sample_svp_check_exps_goto(ret != HI_SUCCESS, end_init_1, SAMPLE_SVP_ERR_LEVEL_ERROR,
        "Error(%#x),sample_comm_vi_start_vi failed!\n", ret);

    /* step 3: Bind vpss to vi */
    ret = sample_common_svp_vi_bind_multi_vpss(vpss_grp_cnt, 1, 1);
    sample_svp_check_exps_goto(ret != HI_SUCCESS, end_init_2, SAMPLE_SVP_ERR_LEVEL_ERROR,
        "Error(%#x),sample_common_vi_bind_multi_vpss failed!\n", ret);

    /* step 4: Start vpss */
    ret = sample_common_svp_start_vpss(vpss_grp_cnt, media_cfg->pic_size, media_cfg->chn_num);
    sample_svp_check_exps_goto(ret != HI_SUCCESS, end_init_3, SAMPLE_SVP_ERR_LEVEL_ERROR,
        "Error(%#x),sample_common_svp_start_vpss failed!\n", ret);

    /* step 5: attach vb */
    ret = sample_common_svp_vpss_attach_vb(media_cfg, vpss_grp_cnt);
    sample_svp_check_exps_goto(ret != HI_SUCCESS, end_init_4, SAMPLE_SVP_ERR_LEVEL_ERROR,
        "Error(%#x),sample_common_svp_start_vpss failed!\n", ret);

    /* step 6: Start vo */
    ret = sample_common_svp_start_vo(vo_cfg);
    sample_svp_check_exps_goto(ret != HI_SUCCESS, end_init_5, SAMPLE_SVP_ERR_LEVEL_ERROR,
        "Error(%#x),sample_common_svp_start_vo failed!\n", ret);

    return ret;

end_init_5:
    sample_common_svp_vpss_detach_vb(media_cfg, vpss_grp_cnt);
end_init_4:
    sample_common_svp_stop_vpss(vpss_grp_cnt);
end_init_3:
    sample_common_svp_vi_unbind_multi_vpss(vpss_grp_cnt, 1, 1);
end_init_2:
    sample_comm_vi_stop_vi(vi_cfg);
end_init_1:
    sample_common_svp_destroy_vb(media_cfg);
    return ret;
}

hi_void sample_common_svp_destroy_vb_stop_vi_vpss_vo(sample_vi_cfg *vi_cfg, hi_sample_svp_media_cfg *media_cfg,
    sample_vo_cfg *vo_cfg)
{
    const hi_s32 vpss_grp_cnt = 1;

    sample_svp_check_exps_return_void(media_cfg == HI_NULL, SAMPLE_SVP_ERR_LEVEL_ERROR, "media_cfg is NULL!\n");

    sample_svp_check_exps_return_void((media_cfg->chn_num == 0) || (media_cfg->chn_num > HI_SVP_MAX_VPSS_CHN_NUM),
        SAMPLE_SVP_ERR_LEVEL_ERROR, "media_cfg->chn_num must be [1, %u]!\n", HI_SVP_MAX_VPSS_CHN_NUM);

    sample_common_svp_stop_vo(vo_cfg);
    sample_common_svp_vpss_detach_vb(media_cfg, vpss_grp_cnt);
    sample_common_svp_stop_vpss(vpss_grp_cnt);
    sample_common_svp_vi_unbind_multi_vpss(vpss_grp_cnt, 1, 1);
    sample_comm_vi_stop_vi(vi_cfg);
    sample_common_svp_destroy_vb(media_cfg);
}
