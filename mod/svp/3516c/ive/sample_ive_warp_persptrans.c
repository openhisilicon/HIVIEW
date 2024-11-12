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

#define HI_SAMPLE_IVE_WARP_PSP_MAX_ROI_NUM      6
#define HI_SAMPLE_IVE_WARP_PSP_QUERY_SLEEP      100
#define HI_SAMPLE_IVE_WARP_PSP_POINT_NUM        4
#define HI_PSP_NUM_TWO                          2
#define HI_SAMPLE_IVE_WARP_PSP_SRC_WIDTH        640
#define HI_SAMPLE_IVE_WARP_PSP_SRC_HEIGHT       480
#define HI_SAMPLE_IVE_WARP_PSP_MATRIX_STRIDE    48

typedef struct {
    hi_svp_src_img src;
    hi_svp_src_mem_info inverse_matrix;
    hi_svp_rect_u32 roi[HI_SAMPLE_IVE_WARP_PSP_MAX_ROI_NUM];
    hi_svp_dst_img dst[HI_SAMPLE_IVE_WARP_PSP_MAX_ROI_NUM];
    hi_ive_warp_perspective_points points[HI_SAMPLE_IVE_WARP_PSP_MAX_ROI_NUM];
    hi_ive_warp_perspective_ctrl warp_persp_ctrl;
    hi_ive_warp_perspective_transform_ctrl get_warp_persp_trans_ctrl;
    FILE *fp_src;
    FILE *fp_dst[HI_SAMPLE_IVE_WARP_PSP_MAX_ROI_NUM];
} hi_sample_ive_warp_persp_trans_info;

static hi_sample_ive_warp_persp_trans_info g_warp_persp_trans;
static hi_bool g_stop_signal = HI_FALSE;

static hi_void sample_ive_warp_persp_trans_uninit(hi_sample_ive_warp_persp_trans_info *psp_info)
{
    hi_u16 i;
    sample_svp_check_exps_return_void(psp_info == HI_NULL, SAMPLE_SVP_ERR_LEVEL_ERROR, "psp_info can't be null\n");

    sample_svp_mmz_free(psp_info->src.phys_addr[0], psp_info->src.virt_addr[0]);
    sample_svp_mmz_free(psp_info->inverse_matrix.phys_addr, psp_info->inverse_matrix.virt_addr);
    for (i = 0; i < psp_info->warp_persp_ctrl.roi_num; i++) {
        sample_svp_mmz_free(psp_info->dst[i].phys_addr[0], psp_info->dst[i].virt_addr[0]);
        sample_svp_close_file(psp_info->fp_dst[i]);
    }

    sample_svp_close_file(psp_info->fp_src);
}

static hi_s32 sample_ive_warp_persp_trans_proc(hi_sample_ive_warp_persp_trans_info *persp_trans)
{
    hi_s32 ret = HI_ERR_IVE_NULL_PTR;
    hi_ive_handle handle;
    hi_bool is_finish = HI_FALSE;
    hi_bool is_block = HI_TRUE;
    hi_bool is_instant = HI_TRUE;
    hi_u32 i, j;

    sample_svp_check_exps_return(persp_trans == HI_NULL, ret, SAMPLE_SVP_ERR_LEVEL_ERROR, "psp_trans can't be null\n");
    for (i = 0; (i < 1) && (g_stop_signal == HI_FALSE); i++) {
        ret = sample_common_ive_read_file(&(persp_trans->src), persp_trans->fp_src);
        sample_svp_check_exps_return(ret != HI_SUCCESS, ret, SAMPLE_SVP_ERR_LEVEL_ERROR,
            "Error(%#x),Read src file failed!\n", ret);
        ret = hi_mpi_ive_get_perspective_transform(&persp_trans->src, persp_trans->roi, persp_trans->points,
            &persp_trans->inverse_matrix, &persp_trans->get_warp_persp_trans_ctrl);
        sample_svp_check_exps_return(ret != HI_SUCCESS, ret, SAMPLE_SVP_ERR_LEVEL_ERROR,
            "Error(%#x),hi_mpi_ive_get_perspective_transform failed!\n", ret);
        ret = hi_mpi_ive_warp_perspective(&handle, &persp_trans->src, persp_trans->roi, &persp_trans->inverse_matrix,
            persp_trans->dst, &persp_trans->warp_persp_ctrl, is_instant);
        sample_svp_check_exps_return(ret != HI_SUCCESS, ret, SAMPLE_SVP_ERR_LEVEL_ERROR,
            "Error(%#x),hi_mpi_ive_warp_perspective failed!\n", ret);

        ret = hi_mpi_ive_query(handle, &is_finish, is_block);
        while (ret == HI_ERR_IVE_QUERY_TIMEOUT) {
            usleep(HI_SAMPLE_IVE_WARP_PSP_QUERY_SLEEP);
            ret = hi_mpi_ive_query(handle, &is_finish, is_block);
        }
        sample_svp_check_exps_return(ret != HI_SUCCESS, ret, SAMPLE_SVP_ERR_LEVEL_ERROR,
            "Error(%#x),hi_mpi_ive_query failed!\n", ret);
        for (j = 0; j < persp_trans->warp_persp_ctrl.roi_num; j++) {
            ret = sample_common_ive_write_file(&persp_trans->dst[j], persp_trans->fp_dst[j]);
            sample_svp_check_exps_return(ret != HI_SUCCESS, ret, SAMPLE_SVP_ERR_LEVEL_ERROR,
                "Error(%#x),Read src file failed!\n", ret);
        }
    }
    return HI_SUCCESS;
}

static hi_void sample_ive_warp_persp_trans_ctrl_init(hi_sample_ive_warp_persp_trans_info *psp_info)
{
    psp_info->warp_persp_ctrl.roi_num = HI_PSP_NUM_TWO;
    psp_info->warp_persp_ctrl.padding_mode = HI_IVE_PADDING_MODE_CONSTANT;
    psp_info->warp_persp_ctrl.padding_val[0] = 0;
    psp_info->warp_persp_ctrl.padding_val[1] = 0;
    psp_info->warp_persp_ctrl.padding_val[HI_PSP_NUM_TWO] = 0;
    psp_info->get_warp_persp_trans_ctrl.roi_num = HI_PSP_NUM_TWO;
    psp_info->get_warp_persp_trans_ctrl.norm = 1;
}

static hi_s32 sample_ive_open_file(hi_sample_ive_warp_persp_trans_info *psp)
{
    hi_s32 ret = HI_ERR_IVE_ILLEGAL_PARAM;
    hi_u32 i;
    hi_char path[PATH_MAX + 1] = {0};
    hi_char tmp_file[PATH_MAX + 1] = {0};
    hi_u32 roi_num = psp->warp_persp_ctrl.roi_num;
    const hi_char *src_file = "./data/input/psp/src/box_640x480_420sp_nv21.yuv";

    sample_svp_check_exps_return((roi_num > HI_SAMPLE_IVE_WARP_PSP_MAX_ROI_NUM) || (roi_num == 0),
        ret, SAMPLE_SVP_ERR_LEVEL_ERROR, "Error, roi_num(%u) must be (%u, %u]!\n", roi_num, 0,
        HI_SAMPLE_IVE_WARP_PSP_MAX_ROI_NUM);

    sample_svp_check_exps_return((strlen(src_file) > PATH_MAX) || (realpath(src_file, path) == HI_NULL),
        ret, SAMPLE_SVP_ERR_LEVEL_ERROR, "invalid file!\n");
    psp->fp_src = fopen(path, "rb");
    sample_svp_check_exps_return(psp->fp_src == HI_NULL, HI_FAILURE, SAMPLE_SVP_ERR_LEVEL_ERROR, "Open file failed!\n");
    ret = HI_ERR_IVE_ILLEGAL_PARAM;
    sample_svp_check_exps_goto(realpath("./data/output/psp", path) == HI_NULL, fail,
        SAMPLE_SVP_ERR_LEVEL_ERROR, "invalid file!\n");

    for (i = 0; i < roi_num; i++) {
        ret = snprintf_s(tmp_file, PATH_MAX, PATH_MAX, "%s/box_420sp_nv21_out_%u.yuv", path, i);
        sample_svp_check_exps_goto((ret < 0), fail, SAMPLE_SVP_ERR_LEVEL_ERROR,
            "Error,snprintf_s file name failed!\n");
        ret = HI_FAILURE;
        psp->fp_dst[i] = fopen(tmp_file, "wb");
        sample_svp_check_exps_goto(psp->fp_dst[i] == HI_NULL,
            fail, SAMPLE_SVP_ERR_LEVEL_ERROR, "fopen file %s failed!\n", tmp_file);
    }
    return HI_SUCCESS;
fail:
    for (i = 0; i < roi_num; i++) {
        sample_svp_close_file(psp->fp_dst[i]);
    }
    sample_svp_close_file(psp->fp_src);
    return ret;
}

static hi_s32 sample_ive_warp_persp_trans_init(hi_sample_ive_warp_persp_trans_info *psp,
    const hi_svp_point_float src_corners_array[][HI_SAMPLE_IVE_WARP_PSP_POINT_NUM],
    const hi_svp_point_float dst_corners_array[][HI_SAMPLE_IVE_WARP_PSP_POINT_NUM])
{
    hi_s32 ret = HI_ERR_IVE_ILLEGAL_PARAM;
    size_t size;
    hi_u32 i, j;
    hi_svp_rect_u32 roi[HI_SAMPLE_IVE_WARP_PSP_MAX_ROI_NUM] = {
        { 30, 196, 284, 214},
        { 310, 90, 300, 256}
    };

    sample_svp_check_exps_return(psp == HI_NULL, HI_ERR_IVE_NULL_PTR, SAMPLE_SVP_ERR_LEVEL_ERROR, "psp is null\n");
    sample_ive_warp_persp_trans_ctrl_init(psp);

    size = sizeof(hi_svp_rect_u32) * psp->warp_persp_ctrl.roi_num;
    ret = memcpy_s(psp->roi, sizeof(hi_svp_rect_u32) * HI_SAMPLE_IVE_WARP_PSP_MAX_ROI_NUM, roi, size);
    sample_svp_check_exps_return(ret != EOK, HI_ERR_IVE_ILLEGAL_PARAM, SAMPLE_SVP_ERR_LEVEL_ERROR, "copy failed!\n");

    ret = sample_common_ive_create_image(&(psp->src), HI_SVP_IMG_TYPE_YUV420SP, HI_SAMPLE_IVE_WARP_PSP_SRC_WIDTH,
        HI_SAMPLE_IVE_WARP_PSP_SRC_HEIGHT);
    sample_svp_check_failed_err_level_goto(ret, fail, "Error(%#x),Create src image failed!\n", ret);

    for (i = 0; i < psp->warp_persp_ctrl.roi_num; i++) {
        ret = sample_common_ive_create_image(&(psp->dst[i]), HI_SVP_IMG_TYPE_YUV420SP,
            psp->roi[i].width, psp->roi[i].height);
        sample_svp_check_failed_err_level_goto(ret, fail, "Error(%#x),Create dst image failed!\n", ret);
    }

    size = psp->warp_persp_ctrl.roi_num * HI_SAMPLE_IVE_WARP_PSP_MATRIX_STRIDE;
    ret = sample_common_ive_create_mem_info(&(psp->inverse_matrix), size);
    sample_svp_check_failed_err_level_goto(ret, fail, "Error(%#x),Create inverse_matrix failed!\n", ret);

    /* set four pairs of the the corresponding points in the roi image and destination image */
    for (i = 0; i < psp->warp_persp_ctrl.roi_num; i++) {
        for (j = 0; j < HI_SAMPLE_IVE_WARP_PSP_POINT_NUM; j++) {
            psp->points[i].point[j].src_point.x = dst_corners_array[i][j].x;
            psp->points[i].point[j].src_point.y = dst_corners_array[i][j].y;
            psp->points[i].point[j].dst_point.x = src_corners_array[i][j].x;
            psp->points[i].point[j].dst_point.y = src_corners_array[i][j].y;
        }
    }

    /* open file */
    ret = sample_ive_open_file(psp);
    sample_svp_check_exps_goto(ret != HI_SUCCESS, fail, SAMPLE_SVP_ERR_LEVEL_ERROR, "fopen file failed!\n");

    return HI_SUCCESS;
fail:
    sample_ive_warp_persp_trans_uninit(psp);
    return ret;
}

static hi_void sample_ive_warp_persp_trans_stop(hi_void)
{
    sample_ive_warp_persp_trans_uninit(&g_warp_persp_trans);
    (hi_void)memset_s(&g_warp_persp_trans, sizeof(g_warp_persp_trans), 0, sizeof(g_warp_persp_trans));
    sample_common_ive_mpi_exit();
    printf("\033[0;31mprogram termination abnormally!\033[0;39m\n");
}
hi_void sample_ive_warp_persp_trans(hi_void)
{
    hi_s32 ret;
    hi_svp_point_float src_array[][HI_SAMPLE_IVE_WARP_PSP_POINT_NUM] = {
        { {17.0, 47.0}, {274.0, 30.0}, {270.0, 144.0}, {29.0, 195.0} },
        { {40.0, 22.0}, {289.0, 34.0}, {281.0, 218.0}, {39.0, 244.0} }
    };
    hi_svp_point_float dst_array[][HI_SAMPLE_IVE_WARP_PSP_POINT_NUM] = {
        { {23.0, 38.5}, {272.0, 38.5}, {272.0, 169.5}, {23.0, 169.5} },
        { {39.5, 28.0}, {285.0, 28.0}, {285.0, 231.0}, {39.5, 231.0} }
    };

    (hi_void)memset_s(&g_warp_persp_trans, sizeof(g_warp_persp_trans), 0, sizeof(g_warp_persp_trans));
    ret = sample_common_ive_check_mpi_init();
    sample_svp_check_exps_return_void(ret != HI_TRUE, SAMPLE_SVP_ERR_LEVEL_ERROR, "ive_check_mpi_init failed!\n");

    ret = sample_ive_warp_persp_trans_init(&g_warp_persp_trans, src_array, dst_array);
    sample_svp_check_exps_goto(ret != HI_SUCCESS, persp_trans_fail, SAMPLE_SVP_ERR_LEVEL_ERROR,
        "Error(%#x),sample_ive_warp_persp_trans_init failed!\n", ret);

    ret = sample_ive_warp_persp_trans_proc(&g_warp_persp_trans);
    if (g_stop_signal == HI_TRUE) {
        sample_ive_warp_persp_trans_stop();
        return;
    }
    if (ret == HI_SUCCESS) {
        sample_svp_trace_info("Process success!\n");
    }

    g_stop_signal = HI_TRUE;
    sample_ive_warp_persp_trans_uninit(&g_warp_persp_trans);
    (hi_void)memset_s(&g_warp_persp_trans, sizeof(g_warp_persp_trans), 0, sizeof(g_warp_persp_trans));

persp_trans_fail:
    g_stop_signal = HI_TRUE;
    sample_common_ive_mpi_exit();
}

/*
 * function : PerspTrans sample signal handle
 */
hi_void sample_ive_warp_persp_trans_handle_sig(hi_void)
{
    g_stop_signal = HI_TRUE;
}
