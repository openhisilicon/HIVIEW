/*
  Copyright (c), 2001-2024, Shenshu Tech. Co., Ltd.
 */
#include "sample_common_ive.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>
#include <semaphore.h>
#include <pthread.h>
#include <limits.h>

#define HI_SAMPLE_IVE_LK_MAX_POINT_NUM      500
#define HI_SAMPLE_IVE_LK_MIN_DIST           5
#define HI_SAMPLE_IVE_LK_PYR_NUM            4
#define HI_SAMPLE_IVE_LK_QUERY_SLEEP        100
#define HI_SAMPLE_IVE_LK_MIN_EIG_VALUE      100
#define HI_SAMPLE_IVE_LK_ITER_CNT           10
#define HI_SAMPLE_IVE_LK_EPS                2
#define HI_SAMPLE_IVE_LEFT_SHIFT_SEVEN      7
#define HI_SAMPLE_IVE_ST_QUALITY_LEVEL      25
#define HI_SAMPLE_IVE_LK_D1_WIDTH           720
#define HI_SAMPLE_IVE_LK_D1_HEIGHT          576
#define HI_SAMPLE_IVE_LK_MAX_LEVEL          3
#define HI_SAMPLE_IVE_LK_FRAME_NUM          10
#define HI_SAMPLE_IVE_HOR_SEG_SIZE          2
#define HI_SAMPLE_IVE_ELEM_SIZE             1
#define HI_SAMPLE_IVE_VER_SEG_ROWS          2

typedef struct {
    hi_svp_src_img prev_pyr[HI_SAMPLE_IVE_LK_PYR_NUM];
    hi_svp_src_img next_pyr[HI_SAMPLE_IVE_LK_PYR_NUM];
    hi_svp_src_mem_info prev_points;
    hi_svp_mem_info next_points;
    hi_svp_dst_mem_info status;
    hi_svp_dst_mem_info err;
    hi_ive_lk_optical_flow_pyr_ctrl lk_pyr_ctrl;

    hi_svp_src_img src;
    hi_svp_img dst;
    hi_svp_dst_mem_info corner;
    hi_ive_st_cand_corner_ctrl cand_corner_ctrl;
    hi_ive_st_corner_ctrl corner_ctrl;

    hi_svp_img pyr_tmp;
    hi_svp_img src_yuv;
    FILE *fp_src;
    FILE *fp_dst_status;
    FILE *fp_dst_err;
    FILE *fp_dst_next_pts;
} hi_sample_ive_st_lk_inf;

static hi_sample_ive_st_lk_inf g_lk_info;
static hi_bool g_stop_signal = HI_FALSE;

static hi_s32 sample_ive_st_lk_dma(hi_ive_handle *ive_handle, hi_svp_src_img *src,
    hi_svp_dst_img *dst, hi_ive_dma_ctrl *dma_ctrl, hi_bool is_instant)
{
    hi_s32 ret;
    hi_svp_src_data data_src;
    hi_svp_dst_data data_dst;

    data_src.virt_addr = src->virt_addr[0];
    data_src.phys_addr = src->phys_addr[0];
    data_src.width = src->width;
    data_src.height = src->height;
    data_src.stride = src->stride[0];

    data_dst.virt_addr = dst->virt_addr[0];
    data_dst.phys_addr = dst->phys_addr[0];
    data_dst.width = dst->width;
    data_dst.height = dst->height;
    data_dst.stride = dst->stride[0];
    ret = hi_mpi_ive_dma(ive_handle, &data_src, &data_dst, dma_ctrl, is_instant);
    sample_svp_check_exps_return(ret != HI_SUCCESS, ret, SAMPLE_SVP_ERR_LEVEL_ERROR,
        "Error(%#x),hi_mpi_ive_dma failed!\n", ret);

    return ret;
}


/*
 * function : Copy pyr
 */
static hi_void sample_ive_st_lk_copy_pyr(hi_svp_src_img pyr_src[], hi_svp_dst_img pyr_dst[],
    hi_u8 max_level)
{
    hi_u8 i;
    hi_s32 ret;
    hi_ive_handle handle;

    hi_ive_dma_ctrl dma_ctrl;
    (hi_void)memset_s(&dma_ctrl, sizeof(dma_ctrl), 0, sizeof(dma_ctrl));
    dma_ctrl.mode = HI_IVE_DMA_MODE_DIRECT_COPY;

    for (i = 0; i <= max_level; i++) {
        ret = sample_ive_st_lk_dma(&handle, &pyr_src[i], &pyr_dst[i], &dma_ctrl, HI_FALSE);
        if (ret != HI_SUCCESS) {
            sample_svp_trace_err("sample_ive_st_lk_dma fail,Error(%d)\n", ret);
            break;
        }
    }
}

/*
 * function : St lk uninit
 */
static hi_void sample_ive_st_lk_uninit(hi_sample_ive_st_lk_inf *lk_info)
{
    hi_u16 i;

    sample_svp_check_exps_return_void(lk_info == HI_NULL, SAMPLE_SVP_ERR_LEVEL_ERROR, "test_mem can't be null\n");

    for (i = 0; i <= lk_info->lk_pyr_ctrl.max_level; i++) {
        sample_svp_mmz_free(lk_info->prev_pyr[i].phys_addr[0], lk_info->prev_pyr[i].virt_addr[0]);
        sample_svp_mmz_free(lk_info->next_pyr[i].phys_addr[0], lk_info->next_pyr[i].virt_addr[0]);
    }

    sample_svp_mmz_free(lk_info->prev_points.phys_addr, lk_info->prev_points.virt_addr);
    sample_svp_mmz_free(lk_info->next_points.phys_addr, lk_info->next_points.virt_addr);
    sample_svp_mmz_free(lk_info->status.phys_addr, lk_info->status.virt_addr);
    sample_svp_mmz_free(lk_info->err.phys_addr, lk_info->err.virt_addr);

    sample_svp_mmz_free(lk_info->src.phys_addr[0], lk_info->src.virt_addr[0]);
    sample_svp_mmz_free(lk_info->dst.phys_addr[0], lk_info->dst.virt_addr[0]);
    sample_svp_mmz_free(lk_info->corner.phys_addr, lk_info->corner.virt_addr);

    sample_svp_mmz_free(lk_info->cand_corner_ctrl.mem.phys_addr, lk_info->cand_corner_ctrl.mem.virt_addr);

    sample_svp_mmz_free(lk_info->pyr_tmp.phys_addr[0], lk_info->pyr_tmp.virt_addr[0]);
    sample_svp_mmz_free(lk_info->src_yuv.phys_addr[0], lk_info->src_yuv.virt_addr[0]);

    sample_svp_close_file(lk_info->fp_src);
    sample_svp_close_file(lk_info->fp_dst_status);
    sample_svp_close_file(lk_info->fp_dst_err);
    sample_svp_close_file(lk_info->fp_dst_next_pts);
}

static hi_s32 sample_ive_lk_param_init(hi_sample_ive_st_lk_inf *lk_info, hi_size src_size, hi_u8 max_level)
{
    hi_s32 ret;
    hi_u32 size;
    hi_u32 i;
    lk_info->lk_pyr_ctrl.out_mode = HI_IVE_LK_OPTICAL_FLOW_PYR_OUT_MODE_BOTH;
    lk_info->lk_pyr_ctrl.use_init_flow = HI_TRUE;
    lk_info->lk_pyr_ctrl.points_num = HI_SAMPLE_IVE_LK_MAX_POINT_NUM;
    lk_info->lk_pyr_ctrl.max_level = max_level;
    lk_info->lk_pyr_ctrl.min_eig_val_threshold = HI_SAMPLE_IVE_LK_MIN_EIG_VALUE;
    lk_info->lk_pyr_ctrl.iter_cnt = HI_SAMPLE_IVE_LK_ITER_CNT;
    lk_info->lk_pyr_ctrl.eps = HI_SAMPLE_IVE_LK_EPS;
    /* Init Pyr */
    for (i = 0; i <= max_level; i++) {
        ret = sample_common_ive_create_image(&lk_info->prev_pyr[i], HI_SVP_IMG_TYPE_U8C1,
            src_size.width >> i, src_size.height >> i);
        sample_svp_check_exps_goto(ret != HI_SUCCESS, lk_init_fail, SAMPLE_SVP_ERR_LEVEL_ERROR,
            "Error(%#x),Create prevPyr[%u] image failed!\n", ret, i);

        ret = sample_common_ive_create_image(&lk_info->next_pyr[i], HI_SVP_IMG_TYPE_U8C1,
            lk_info->prev_pyr[i].width, lk_info->prev_pyr[i].height);
        sample_svp_check_exps_goto(ret != HI_SUCCESS, lk_init_fail, SAMPLE_SVP_ERR_LEVEL_ERROR,
            "Error(%#x),Create nextPyr[%u] image failed!\n", ret, i);
    }
    /* Init prev pts */
    size = (hi_u32)sizeof(hi_svp_point_s25q7) * HI_SAMPLE_IVE_LK_MAX_POINT_NUM;
    size = sample_common_ive_calc_stride(size, HI_IVE_ALIGN);
    ret = sample_common_ive_create_mem_info(&(lk_info->prev_points), size);
    sample_svp_check_exps_goto(ret != HI_SUCCESS, lk_init_fail, SAMPLE_SVP_ERR_LEVEL_ERROR,
        "Error(%#x),Create prevPts mem info failed!\n", ret);
    /* Init next pts */
    ret = sample_common_ive_create_mem_info(&(lk_info->next_points), size);
    sample_svp_check_exps_goto(ret != HI_SUCCESS, lk_init_fail, SAMPLE_SVP_ERR_LEVEL_ERROR,
        "Error(%#x),Create nextPts mem info failed!\n", ret);

    /* Init status */
    size = (hi_u32)sizeof(hi_u8) * HI_SAMPLE_IVE_LK_MAX_POINT_NUM;
    size = sample_common_ive_calc_stride(size, HI_IVE_ALIGN);
    ret = sample_common_ive_create_mem_info(&(lk_info->status), size);
    sample_svp_check_exps_goto(ret != HI_SUCCESS, lk_init_fail, SAMPLE_SVP_ERR_LEVEL_ERROR,
        "Error(%#x),Create status mem info failed!\n", ret);

    /* Init err */
    size = (hi_u32)sizeof(hi_u9q7) * HI_SAMPLE_IVE_LK_MAX_POINT_NUM;
    size = sample_common_ive_calc_stride(size, HI_IVE_ALIGN);
    ret = sample_common_ive_create_mem_info(&(lk_info->err), size);
    sample_svp_check_exps_goto(ret != HI_SUCCESS, lk_init_fail, SAMPLE_SVP_ERR_LEVEL_ERROR,
        "Error(%#x),Create err mem info failed!\n", ret);

lk_init_fail:
    if (ret != HI_SUCCESS) {
        sample_ive_st_lk_uninit(lk_info);
    }
    return ret;
}

static hi_s32 sample_ive_st_param_init(hi_sample_ive_st_lk_inf *lk_info, hi_size src_size, hi_size pyr_size)
{
    hi_s32 ret;
    hi_u32 size;

    hi_unused(pyr_size);
    /* Init St */
    ret = sample_common_ive_create_image(&lk_info->src, HI_SVP_IMG_TYPE_U8C1, src_size.width, src_size.height);
    sample_svp_check_exps_goto(ret != HI_SUCCESS, st_init_fail, SAMPLE_SVP_ERR_LEVEL_ERROR,
        "Error(%#x),Create src image failed!\n", ret);

    ret = sample_common_ive_create_image(&lk_info->dst, HI_SVP_IMG_TYPE_U8C1, src_size.width, src_size.height);
    sample_svp_check_exps_goto(ret != HI_SUCCESS, st_init_fail, SAMPLE_SVP_ERR_LEVEL_ERROR,
        "Error(%#x),Create dst image failed!\n", ret);

    lk_info->cand_corner_ctrl.quality_level = HI_SAMPLE_IVE_ST_QUALITY_LEVEL;
    size = (hi_u32)sizeof(hi_u32) * sample_common_ive_calc_stride(src_size.width, HI_IVE_ALIGN) * src_size.height +
        (hi_u32)sizeof(hi_ive_st_max_eig_val);
    ret = sample_common_ive_create_mem_info(&(lk_info->cand_corner_ctrl.mem), size);
    sample_svp_check_exps_goto(ret != HI_SUCCESS, st_init_fail, SAMPLE_SVP_ERR_LEVEL_ERROR,
        "Error(%#x),Create CandiCornerCtrl.stMem mem info failed!\n", ret);

    size = (hi_u32)sizeof(hi_ive_st_corner_info);
    ret = sample_common_ive_create_mem_info(&(lk_info->corner), size);
    sample_svp_check_exps_goto(ret != HI_SUCCESS, st_init_fail, SAMPLE_SVP_ERR_LEVEL_ERROR,
        "Error(%#x),Create corner mem info failed!\n", ret);

    lk_info->corner_ctrl.max_corner_num = HI_SAMPLE_IVE_LK_MAX_POINT_NUM;
    lk_info->corner_ctrl.min_dist = HI_SAMPLE_IVE_LK_MIN_DIST;
st_init_fail:
    if (ret != HI_SUCCESS) {
        sample_ive_st_lk_uninit(lk_info);
    }
    return ret;
}
static hi_s32 sample_ive_st_open_dst_file(hi_sample_ive_st_lk_inf *lk_info)
{
    hi_s32 ret = HI_FAILURE;
    hi_char path[PATH_MAX + 1] = {0};
    /* open dst file */
    sample_svp_check_exps_return(realpath("./data/output/stlk", path) == HI_NULL, ret, SAMPLE_SVP_ERR_LEVEL_ERROR,
        "invalid file!\n");
    ret = strcat_s(path, PATH_MAX, "/stlk_out_status");
    sample_svp_check_exps_return(ret != EOK, ret, SAMPLE_SVP_ERR_LEVEL_ERROR, "strcat_s failed!\n");
    ret = HI_FAILURE;
    lk_info->fp_dst_status = fopen(path, "wb");
    sample_svp_check_exps_return(lk_info->fp_dst_status == HI_NULL, ret, SAMPLE_SVP_ERR_LEVEL_ERROR,
        "Open file failed!\n");

    sample_svp_check_exps_return(realpath("./data/output/stlk", path) == HI_NULL, ret, SAMPLE_SVP_ERR_LEVEL_ERROR,
        "invalid file!\n");
    ret = strcat_s(path, PATH_MAX, "/stlk_out_err");
    sample_svp_check_exps_return(ret != EOK, ret, SAMPLE_SVP_ERR_LEVEL_ERROR, "strcat_s failed!\n");
    ret = HI_FAILURE;
    lk_info->fp_dst_err = fopen(path, "wb");
    sample_svp_check_exps_return(lk_info->fp_dst_err == HI_NULL, ret, SAMPLE_SVP_ERR_LEVEL_ERROR,
        "Open file failed!\n");

    sample_svp_check_exps_return(realpath("./data/output/stlk", path) == HI_NULL, ret, SAMPLE_SVP_ERR_LEVEL_ERROR,
        "invalid file!\n");
    ret = strcat_s(path, PATH_MAX, "/stlk_out_next_pts");
    sample_svp_check_exps_return(ret != EOK, ret, SAMPLE_SVP_ERR_LEVEL_ERROR, "strcat_s failed!\n");
    ret = HI_FAILURE;
    lk_info->fp_dst_next_pts = fopen(path, "wb");
    sample_svp_check_exps_return(lk_info->fp_dst_next_pts == HI_NULL, ret, SAMPLE_SVP_ERR_LEVEL_ERROR,
        "Open file failed!\n");
    return HI_SUCCESS;
}
/*
 * function : St lk init
 */
static hi_s32 sample_ive_st_lk_init(hi_sample_ive_st_lk_inf *lk_info, hi_size src_size,
    hi_size pyr_size, hi_u8 max_level)
{
    hi_s32 ret = HI_ERR_IVE_NULL_PTR;
    hi_char path[PATH_MAX + 1] = {0};
    const hi_char *src_file = "./data/input/stlk/st_lk_720x576_420sp.yuv";

    sample_svp_check_exps_return(lk_info == HI_NULL, ret, SAMPLE_SVP_ERR_LEVEL_ERROR, "test_mem can't be null\n");
    sample_svp_check_exps_return((strlen(src_file) > PATH_MAX) || (realpath(src_file, path) == HI_NULL),
        HI_ERR_IVE_ILLEGAL_PARAM, SAMPLE_SVP_ERR_LEVEL_ERROR, "invalid file!\n");

    /* max_level can't be large than HI_SAMPLE_IVE_LK_PYR_NUM */
    sample_svp_check_exps_return(max_level > (HI_SAMPLE_IVE_LK_PYR_NUM - 1), HI_FAILURE,
        SAMPLE_SVP_ERR_LEVEL_ERROR, "max_level can't be larger than %u\n", (HI_SAMPLE_IVE_LK_PYR_NUM - 1));
    (hi_void)memset_s(lk_info, sizeof(hi_sample_ive_st_lk_inf), 0, sizeof(hi_sample_ive_st_lk_inf));
    /* lk param init */
    ret = sample_ive_lk_param_init(lk_info, src_size, max_level);
    sample_svp_check_exps_return(ret != HI_SUCCESS, ret, SAMPLE_SVP_ERR_LEVEL_ERROR,
        "sample_ive_lk_param_init failed\n");

    /* st param init */
    ret = sample_ive_st_param_init(lk_info, src_size, pyr_size);
    sample_svp_check_exps_return(ret != HI_SUCCESS, ret, SAMPLE_SVP_ERR_LEVEL_ERROR,
        "sample_ive_st_param_init failed\n");

    /* init pyr assist buff */
    ret = sample_common_ive_create_image(&lk_info->pyr_tmp, HI_SVP_IMG_TYPE_U8C1, pyr_size.width, pyr_size.height);
    sample_svp_check_exps_goto(ret != HI_SUCCESS, st_lk_init_fail, SAMPLE_SVP_ERR_LEVEL_ERROR,
        "Error(%#x),Create pyrTmp image failed!\n", ret);

    ret = sample_common_ive_create_image(&lk_info->src_yuv, HI_SVP_IMG_TYPE_YUV420SP, src_size.width, src_size.height);
    sample_svp_check_exps_goto(ret != HI_SUCCESS, st_lk_init_fail, SAMPLE_SVP_ERR_LEVEL_ERROR,
        "Error(%#x),Create srcYuv image failed!\n", ret);

    /* open src file */
    lk_info->fp_src = fopen(path, "rb");
    sample_svp_check_exps_goto(lk_info->fp_src == HI_NULL, st_lk_init_fail, SAMPLE_SVP_ERR_LEVEL_ERROR,
        "Error,Open file %s failed!\n", path);
    
    ret = sample_ive_st_open_dst_file(lk_info);
    sample_svp_check_exps_goto(ret != HI_SUCCESS, st_lk_init_fail, SAMPLE_SVP_ERR_LEVEL_ERROR,
        "Error(%#x),sample_ive_st_open_dst_file failed!\n", ret);
    ret = HI_SUCCESS;

st_lk_init_fail:
    if (ret != HI_SUCCESS) {
        sample_ive_st_lk_uninit(lk_info);
    }
    return ret;
}


/*
 * function : Pyr down
 */
static hi_s32 sample_ive_st_lk_pyr_down(hi_sample_ive_st_lk_inf *lk_info, hi_svp_src_img *src,
    hi_svp_dst_img *dst)
{
    hi_s32 ret;
    hi_ive_handle handle;
    hi_ive_dma_ctrl dma_ctrl;
    hi_ive_filter_ctrl filter_ctrl = { { 1, 2, 3, 2, 1, 2, 5, 6, 5, 2, 3, 6, 8, 6, 3, 2, 5, 6, 5, 2, 1, 2, 3, 2, 1,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
        7, 5, 5, HI_IVE_PADDING_MODE_EDGE, {0} };
    (hi_void)memset_s(&dma_ctrl, sizeof(dma_ctrl), 0, sizeof(dma_ctrl));
    dma_ctrl.mode = HI_IVE_DMA_MODE_INTERVAL_COPY;
    dma_ctrl.hor_seg_size = HI_SAMPLE_IVE_HOR_SEG_SIZE;
    dma_ctrl.elem_size = HI_SAMPLE_IVE_ELEM_SIZE;
    dma_ctrl.ver_seg_rows = HI_SAMPLE_IVE_VER_SEG_ROWS;
    lk_info->pyr_tmp.width = src->width;
    lk_info->pyr_tmp.height = src->height;

    ret = hi_mpi_ive_filter(&handle, src, &lk_info->pyr_tmp, &filter_ctrl, HI_FALSE);
    sample_svp_check_exps_return(ret != HI_SUCCESS, ret, SAMPLE_SVP_ERR_LEVEL_ERROR,
        "Error(%#x),hi_mpi_ive_filter failed!\n", ret);

    ret = sample_ive_st_lk_dma(&handle, &lk_info->pyr_tmp, dst, &dma_ctrl, HI_FALSE);
    sample_svp_check_exps_return(ret != HI_SUCCESS, ret, SAMPLE_SVP_ERR_LEVEL_ERROR,
        "Error(%#x),sample_ive_st_lk_dma failed!\n", ret);

    return ret;
}


static hi_s32 sample_ive_query_task(hi_ive_handle handle)
{
    hi_s32 ret;
    hi_bool is_block = HI_TRUE;
    hi_bool is_finish = HI_FALSE;
    ret = hi_mpi_ive_query(handle, &is_finish, is_block);
    while (ret == HI_ERR_IVE_QUERY_TIMEOUT) {
        usleep(HI_SAMPLE_IVE_LK_QUERY_SLEEP);
        ret = hi_mpi_ive_query(handle, &is_finish, is_block);
    }
    sample_svp_check_exps_return(ret != HI_SUCCESS, ret, SAMPLE_SVP_ERR_LEVEL_ERROR,
        "Error(%#x),hi_mpi_ive_query failed!\n", ret);
    return HI_SUCCESS;
}

static hi_s32 sample_ive_proc_frame(hi_u32 i, hi_ive_handle handle, hi_sample_ive_st_lk_inf *lk)
{
    hi_s32 ret;
    hi_u32 k;
    hi_u32 rect_num;

    hi_ive_st_corner_info *corner_info =
        sample_svp_convert_addr_to_ptr(hi_ive_st_corner_info, lk->corner.virt_addr);
    hi_svp_point_s25q7 *next_points =
        sample_svp_convert_addr_to_ptr(hi_svp_point_s25q7, lk->next_points.virt_addr);

    if (i == 0) {
        ret = hi_mpi_ive_st_cand_corner(&handle, &lk->next_pyr[0], &lk->dst, &lk->cand_corner_ctrl, HI_TRUE);
        sample_svp_check_exps_return(ret != HI_SUCCESS, ret, SAMPLE_SVP_ERR_LEVEL_ERROR,
            "Error(%#x),hi_mpi_ive_st_cand_corner failed!\n", ret);

        ret = sample_ive_query_task(handle);
        sample_svp_check_exps_return(ret != HI_SUCCESS, ret, SAMPLE_SVP_ERR_LEVEL_ERROR,
            "Error(%#x),sample_ive_query_task failed!\n", ret);

        ret = hi_mpi_ive_st_corner(&lk->dst, &lk->corner, &lk->corner_ctrl);
        sample_svp_check_exps_return(ret != HI_SUCCESS, ret, SAMPLE_SVP_ERR_LEVEL_ERROR,
            "Error(%#x),hi_mpi_ive_st_corner failed!\n", ret);

        lk->lk_pyr_ctrl.points_num = corner_info->corner_num;
        for (k = 0; k < lk->lk_pyr_ctrl.points_num; k++) {
            next_points[k].x = (hi_s32)(corner_info->corner[k].x << HI_SAMPLE_IVE_LEFT_SHIFT_SEVEN);
            next_points[k].y = (hi_s32)(corner_info->corner[k].y << HI_SAMPLE_IVE_LEFT_SHIFT_SEVEN);
        }
    } else {
        ret = hi_mpi_ive_lk_optical_flow_pyr(&handle, lk->prev_pyr, lk->next_pyr, &lk->prev_points,
            &lk->next_points, &lk->status, &lk->err, &lk->lk_pyr_ctrl, HI_TRUE);
        sample_svp_check_exps_return(ret != HI_SUCCESS, ret, SAMPLE_SVP_ERR_LEVEL_ERROR,
            "Error(%#x),hi_mpi_ive_lk_optical_flow_pyr failed!\n", ret);

        ret = sample_ive_query_task(handle);
        sample_svp_check_exps_return(ret != HI_SUCCESS, ret, SAMPLE_SVP_ERR_LEVEL_ERROR,
            "Error(%#x),sample_ive_query_task failed!\n", ret);

        rect_num = 0;
        for (k = 0; k < lk->lk_pyr_ctrl.points_num; k++) {
            if ((sample_svp_convert_addr_to_ptr(hi_u8, lk->status.virt_addr))[k] == 0) {
                continue;
            }
            next_points[rect_num].x = next_points[k].x;
            next_points[rect_num].y = next_points[k].y;
            rect_num++;
        }

        lk->lk_pyr_ctrl.points_num = rect_num;
    }
    return HI_SUCCESS;
}
/*
 * function :Write st_and_lk file
 */
static hi_s32 sample_common_ive_write_stlk_file(hi_svp_mem_info *lk_out, FILE *fp, td_u16 points_num)
{
    hi_u8 *ptr_tmp = HI_NULL;
    hi_s32 ret = HI_ERR_IVE_NULL_PTR;

    sample_svp_check_exps_return(lk_out == HI_NULL, ret, SAMPLE_SVP_ERR_LEVEL_ERROR,
        "lk_out can't be null\n");
    sample_svp_check_exps_return(fp == HI_NULL, ret, SAMPLE_SVP_ERR_LEVEL_ERROR, "fp can't be null\n");
    ret = HI_ERR_IVE_ILLEGAL_PARAM;
    sample_svp_check_exps_return(lk_out->phys_addr == 0, ret, SAMPLE_SVP_ERR_LEVEL_ERROR,
        "lk_out->phys_addr can't be 0\n");
    sample_svp_check_exps_return(lk_out->virt_addr == 0, ret, SAMPLE_SVP_ERR_LEVEL_ERROR,
        "lk_out->virt_addr can't be 0\n");
    ptr_tmp = sample_svp_convert_addr_to_ptr(hi_u8, lk_out->virt_addr);
    sample_svp_check_exps_return(ptr_tmp == 0, ret, SAMPLE_SVP_ERR_LEVEL_ERROR, "ptr_tmp can't be 0\n");
    if (fwrite(ptr_tmp, points_num, 1, fp) != 1) {
        sample_svp_trace_err("Write file fail\n");
        return ret;
    }
    return HI_SUCCESS;
}
/*
 * function : St lk proc
 */
static hi_s32 sample_ive_st_lk_proc(hi_sample_ive_st_lk_inf *lk)
{
    const hi_u32 frame_num = HI_SAMPLE_IVE_LK_FRAME_NUM;
    hi_u32 i, k;
    hi_s32 ret = HI_ERR_IVE_NULL_PTR;
    hi_ive_handle handle;
    hi_ive_dma_ctrl dma_ctrl;

    sample_svp_check_exps_return(lk == HI_NULL, ret, SAMPLE_SVP_ERR_LEVEL_ERROR, "test_mem can't be null\n");

    (hi_void)memset_s(&dma_ctrl, sizeof(dma_ctrl), 0, sizeof(dma_ctrl));
    dma_ctrl.mode = HI_IVE_DMA_MODE_DIRECT_COPY;

    for (i = 0; (i < frame_num) && (g_stop_signal == HI_FALSE); i++) {
        sample_svp_trace_info("Proc frame %u\n", i);
        ret = sample_common_ive_read_file(&lk->src_yuv, lk->fp_src);
        sample_svp_check_exps_return(ret != HI_SUCCESS, ret, SAMPLE_SVP_ERR_LEVEL_ERROR,
            "Error(%#x),Read src file failed!\n", ret);

        ret = sample_ive_st_lk_dma(&handle, &lk->src_yuv, &lk->next_pyr[0], &dma_ctrl, HI_FALSE);
        sample_svp_check_exps_return(ret != HI_SUCCESS, ret, SAMPLE_SVP_ERR_LEVEL_ERROR,
            "Error(%#x),sample_ive_st_lk_dma failed!\n", ret);

        /* buid pyr */
        for (k = 1; k <= lk->lk_pyr_ctrl.max_level; k++) {
            ret = sample_ive_st_lk_pyr_down(lk, &lk->next_pyr[k - 1], &lk->next_pyr[k]);
            sample_svp_check_exps_return(ret != HI_SUCCESS, ret, SAMPLE_SVP_ERR_LEVEL_ERROR,
                "Error(%#x),sample_ive_st_lk_pyr_down %u failed!\n", ret, k);
        }
        /* process frame */
        ret = sample_ive_proc_frame(i, handle, lk);
        sample_svp_check_exps_return(ret != HI_SUCCESS, ret, SAMPLE_SVP_ERR_LEVEL_ERROR,
            "Error(%#x),sample_ive_proc_frame failed!\n", ret);

        ret = memcpy_s(sample_svp_convert_addr_to_ptr(hi_void, lk->prev_points.virt_addr),
            sizeof(hi_svp_point_s25q7) * lk->lk_pyr_ctrl.points_num,
            sample_svp_convert_addr_to_ptr(hi_void, lk->next_points.virt_addr),
            sizeof(hi_svp_point_s25q7) * lk->lk_pyr_ctrl.points_num);
        sample_svp_check_exps_return(ret != EOK, HI_ERR_IVE_ILLEGAL_PARAM,
            SAMPLE_SVP_ERR_LEVEL_ERROR, "Error,memcpy_s lk points failed!\n");

        sample_ive_st_lk_copy_pyr(lk->next_pyr, lk->prev_pyr, lk->lk_pyr_ctrl.max_level);
        if (i != 0) {
            ret = sample_common_ive_write_stlk_file(&lk->status, lk->fp_dst_status, lk->lk_pyr_ctrl.points_num);
            sample_svp_check_exps_return(ret != HI_SUCCESS, ret, SAMPLE_SVP_ERR_LEVEL_ERROR,
                "Error(%#x),Write stlk status file failed!\n", ret);
            ret = sample_common_ive_write_stlk_file(&lk->err, lk->fp_dst_err, lk->lk_pyr_ctrl.points_num);
            sample_svp_check_exps_return(ret != HI_SUCCESS, ret, SAMPLE_SVP_ERR_LEVEL_ERROR,
                "Error(%#x),Write stlk err file failed!\n", ret);
            ret = sample_common_ive_write_stlk_file(&lk->next_points, lk->fp_dst_next_pts, lk->lk_pyr_ctrl.points_num);
            sample_svp_check_exps_return(ret != HI_SUCCESS, ret, SAMPLE_SVP_ERR_LEVEL_ERROR,
                "Error(%#x),Write stlk next_pts file failed!\n", ret);
        }
    }
    return HI_SUCCESS;
}

static hi_void sample_ive_lk_stop(hi_void)
{
    sample_ive_st_lk_uninit(&g_lk_info);
    (hi_void)memset_s(&g_lk_info, sizeof(g_lk_info), 0, sizeof(g_lk_info));
    sample_common_ive_mpi_exit();
    printf("\033[0;31mprogram termination abnormally!\033[0;39m\n");
}
/*
 * function : show St Lk sample
 */
hi_void sample_ive_st_lk(hi_void)
{
    hi_size src_size = { HI_SAMPLE_IVE_LK_D1_WIDTH, HI_SAMPLE_IVE_LK_D1_HEIGHT };
    hi_size pyr_size = { HI_SAMPLE_IVE_LK_D1_WIDTH, HI_SAMPLE_IVE_LK_D1_HEIGHT };

    hi_s32 ret;
    const hi_u8 max_level = HI_SAMPLE_IVE_LK_MAX_LEVEL;

    (hi_void)memset_s(&g_lk_info, sizeof(g_lk_info), 0, sizeof(g_lk_info));
    ret = sample_common_ive_check_mpi_init();
    sample_svp_check_exps_return_void(ret != HI_TRUE, SAMPLE_SVP_ERR_LEVEL_ERROR, "ive_check_mpi_init failed!\n");

    ret = sample_ive_st_lk_init(&g_lk_info, src_size, pyr_size, max_level);
    sample_svp_check_exps_goto(ret != HI_SUCCESS, st_lk_fail, SAMPLE_SVP_ERR_LEVEL_ERROR,
        "Error(%#x),sample_ive_st_lk_init failed!\n", ret);

    ret = sample_ive_st_lk_proc(&g_lk_info);
    if (g_stop_signal == HI_TRUE) {
        sample_ive_lk_stop();
        return;
    }
    if (ret == HI_SUCCESS) {
        sample_svp_trace_info("Process success!\n");
    } else {
        sample_svp_trace_err("st_lk process failed\n");
    }

    g_stop_signal = HI_TRUE;
    sample_ive_st_lk_uninit(&g_lk_info);
    (hi_void)memset_s(&g_lk_info, sizeof(g_lk_info), 0, sizeof(g_lk_info));

st_lk_fail:
    g_stop_signal = HI_TRUE;
    sample_common_ive_mpi_exit();
}

/*
 * function : St_Lk sample signal handle
 */
hi_void sample_ive_st_lk_handle_sig(hi_void)
{
    g_stop_signal = HI_TRUE;
}
