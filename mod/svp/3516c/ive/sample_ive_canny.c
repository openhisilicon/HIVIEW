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
#include <math.h>
#include <limits.h>

#define HI_SAMPLE_IVE_CANNY_QUERY_SLEEP  100
#define HI_SAMPLE_IVE_CANNY_HIGH_THR     150
#define HI_SAMPLE_IVE_CANNY_LOW_THR      50
#define HI_SAMPLE_IVE_THR_U16_HIGH_THR   100
#define HI_SAMPLE_IVE_THR_U16_LOW_THR    100
#define HI_SAMPLE_IVE_THR_U16_MAX_VAL    255
#define HI_SAMPLE_IVE_CANNY_EDGE_NUM     3
#define HI_SAMPLE_IVE_D1_WIDTH           720
#define HI_SAMPLE_IVE_D1_HEIGHT          576

typedef struct {
    hi_svp_src_img src;
    hi_svp_dst_img edge;
    hi_svp_dst_img mag;
    hi_svp_mem_info stack;
    hi_ive_canny_hys_edge_ctrl canny_hys_edge_ctrl;
    hi_ive_mag_and_ang_ctrl mag_and_ang_ctrl;
    hi_ive_threshold_u16_ctrl thr_u16_ctrl;
    FILE *fp_src;
    FILE *fp_dst;
} hi_sample_ive_canny_info;

static hi_sample_ive_canny_info g_canny_info;
static hi_bool g_stop_signal = HI_FALSE;

/*
 * function : Canny uninit
 */
static hi_void sample_ive_canny_uninit(hi_sample_ive_canny_info *canny_info)
{
    sample_svp_mmz_free(canny_info->src.phys_addr[0], canny_info->src.virt_addr[0]);
    sample_svp_mmz_free(canny_info->edge.phys_addr[0], canny_info->edge.virt_addr[0]);
    sample_svp_mmz_free(canny_info->mag.phys_addr[0], canny_info->mag.virt_addr[0]);
    sample_svp_mmz_free(canny_info->stack.phys_addr, canny_info->stack.virt_addr);
    sample_svp_mmz_free(canny_info->canny_hys_edge_ctrl.mem.phys_addr,
        canny_info->canny_hys_edge_ctrl.mem.virt_addr);

    sample_svp_close_file(canny_info->fp_src);
    sample_svp_close_file(canny_info->fp_dst);
}

static hi_void sample_ive_canny_ctrl_init(hi_sample_ive_canny_info *canny_info)
{
    canny_info->canny_hys_edge_ctrl.high_threshold = HI_SAMPLE_IVE_CANNY_HIGH_THR;
    canny_info->canny_hys_edge_ctrl.low_threshold = HI_SAMPLE_IVE_CANNY_LOW_THR;
    canny_info->mag_and_ang_ctrl.out_ctrl = HI_IVE_MAG_AND_ANG_OUT_CTRL_MAG;
    canny_info->mag_and_ang_ctrl.threshold = 0;
    canny_info->thr_u16_ctrl.mode = HI_IVE_THRESHOLD_U16_MODE_U16_TO_U8_MIN_MID_MAX;
    canny_info->thr_u16_ctrl.high_threshold = HI_SAMPLE_IVE_THR_U16_HIGH_THR;
    canny_info->thr_u16_ctrl.low_threshold = HI_SAMPLE_IVE_THR_U16_LOW_THR;
    canny_info->thr_u16_ctrl.max_val = HI_SAMPLE_IVE_THR_U16_MAX_VAL;
    canny_info->thr_u16_ctrl.mid_val = 0;
    canny_info->thr_u16_ctrl.min_val = 0;
}

/*
 * function : Canny init
 */
static hi_s32 sample_ive_canny_init(hi_sample_ive_canny_info *canny_info, hi_u32 width,
    hi_u32 height, hi_char canny_complete)
{
    hi_s32 ret;
    hi_u32 size;
    hi_char path[PATH_MAX + 1] = {0};
    hi_char tmp_file[PATH_MAX + 1] = {0};
    hi_s8 mask[HI_IVE_MASK_NUM] =   { 0,  0, 0, 0, 0, 0, -1, 0, 1, 0, 0, -2, 0, 2, 0, 0, -1, 0, 1, 0, 0,  0, 0, 0, 0 };
    const hi_char *src_file = "./data/input/canny/canny.yuv";

    sample_svp_check_exps_return((strlen(src_file) > PATH_MAX) || (realpath(src_file, path) == HI_NULL),
        HI_ERR_IVE_ILLEGAL_PARAM, SAMPLE_SVP_ERR_LEVEL_ERROR, "invalid file!\n");

    (hi_void)memset_s(canny_info, sizeof(hi_sample_ive_canny_info), 0, sizeof(hi_sample_ive_canny_info));
    ret = memcpy_s(canny_info->canny_hys_edge_ctrl.mask, HI_IVE_MASK_NUM, mask, HI_IVE_MASK_NUM);
    sample_svp_check_exps_return(ret != EOK, ret, SAMPLE_SVP_ERR_LEVEL_ERROR, "memcpy_s mask failed!\n");
    ret = memcpy_s(canny_info->mag_and_ang_ctrl.mask, HI_IVE_MASK_NUM, mask, HI_IVE_MASK_NUM);
    sample_svp_check_exps_return(ret != EOK, ret, SAMPLE_SVP_ERR_LEVEL_ERROR, "memcpy_s mask failed!\n");

    sample_ive_canny_ctrl_init(canny_info);

    ret = sample_common_ive_create_image(&canny_info->src, HI_SVP_IMG_TYPE_U8C1, width, height);
    sample_svp_check_exps_goto(ret != HI_SUCCESS, fail, SAMPLE_SVP_ERR_LEVEL_ERROR, "Error,create src img failed!\n");

    ret = sample_common_ive_create_image(&canny_info->edge, HI_SVP_IMG_TYPE_U8C1, width, height);
    sample_svp_check_exps_goto(ret != HI_SUCCESS, fail, SAMPLE_SVP_ERR_LEVEL_ERROR, "Error,create edge img failed!\n");

    ret = sample_common_ive_create_image(&canny_info->mag, HI_SVP_IMG_TYPE_U16C1, width, height);
    sample_svp_check_exps_goto(ret != HI_SUCCESS, fail, SAMPLE_SVP_ERR_LEVEL_ERROR, "Error,create mag img failed!\n");

    size = canny_info->src.stride[0] * canny_info->src.height * (hi_u32)sizeof(hi_svp_point_u16) +
        (hi_u32)sizeof(hi_ive_canny_stack_size);
    ret = sample_common_ive_create_mem_info(&canny_info->stack, size);
    sample_svp_check_exps_goto(ret != HI_SUCCESS, fail, SAMPLE_SVP_ERR_LEVEL_ERROR,
        "Error,create stack mem_info failed!\n");

    size = canny_info->src.stride[0] * canny_info->src.height * HI_SAMPLE_IVE_CANNY_EDGE_NUM;
    ret = sample_common_ive_create_mem_info(&canny_info->canny_hys_edge_ctrl.mem, size);
    sample_svp_check_exps_goto(ret != HI_SUCCESS, fail, SAMPLE_SVP_ERR_LEVEL_ERROR,
        "Error,create canny_hys_edge_ctrl.mem failed!\n");

    /* src file */
    ret = HI_FAILURE;
    canny_info->fp_src = fopen(path, "rb");
    sample_svp_check_exps_goto(canny_info->fp_src == HI_NULL, fail, SAMPLE_SVP_ERR_LEVEL_ERROR, "Open file failed!\n");

    /* dst file */
    sample_svp_check_exps_goto(realpath("./data/output/canny", path) == HI_NULL, fail,
        SAMPLE_SVP_ERR_LEVEL_ERROR, "invalid file!\n");
    ret = snprintf_s(tmp_file, sizeof(tmp_file) - 1, sizeof(tmp_file) - 1, "/cannyout_complete_%c.yuv", canny_complete);
    sample_svp_check_exps_goto((ret < 0) || (ret > (hi_s32)(sizeof(tmp_file) - 1)), fail,
        SAMPLE_SVP_ERR_LEVEL_ERROR, "Error,snprintf_s src file name failed!\n");

    ret = strcat_s(path, PATH_MAX, tmp_file);
    sample_svp_check_exps_goto(ret != EOK, fail, SAMPLE_SVP_ERR_LEVEL_ERROR, "strcat_s failed!\n");
    ret = HI_FAILURE;
    canny_info->fp_dst = fopen(path, "wb");
    sample_svp_check_exps_goto(canny_info->fp_dst == HI_NULL, fail, SAMPLE_SVP_ERR_LEVEL_ERROR, "Open file failed!\n");

    return HI_SUCCESS;
fail:
    sample_ive_canny_uninit(canny_info);
    return ret;
}

/*
 * function : show complate canny sample
 */
static hi_s32 sample_ive_complete_canny(hi_sample_ive_canny_info *canny_info)
{
    hi_s32 ret, i;
    hi_bool is_instant = HI_TRUE;
    hi_bool is_block = HI_TRUE;
    hi_bool is_finish = HI_FALSE;
    hi_ive_handle handle;

    for (i = 0; (i < 1) && (g_stop_signal == HI_FALSE); i++) {
        ret = sample_common_ive_read_file(&(canny_info->src), canny_info->fp_src);
        sample_svp_check_exps_return(ret != HI_SUCCESS, ret, SAMPLE_SVP_ERR_LEVEL_ERROR,
            "Error(%#x),Read src file failed!\n", ret);

        ret = hi_mpi_ive_canny_hys_edge(&handle, &canny_info->src, &canny_info->edge, &canny_info->stack,
            &canny_info->canny_hys_edge_ctrl, is_instant);
        sample_svp_check_exps_return(ret != HI_SUCCESS, ret, SAMPLE_SVP_ERR_LEVEL_ERROR,
            "Error(%#x),hi_mpi_ive_canny_hys_edge failed!\n", ret);

        ret = hi_mpi_ive_query(handle, &is_finish, is_block);
        while (ret == HI_ERR_IVE_QUERY_TIMEOUT) {
            usleep(HI_SAMPLE_IVE_CANNY_QUERY_SLEEP);
            ret = hi_mpi_ive_query(handle, &is_finish, is_block);
        }
        sample_svp_check_exps_return(ret != HI_SUCCESS, ret, SAMPLE_SVP_ERR_LEVEL_ERROR,
            "Error(%#x),hi_mpi_ive_query failed!\n", ret);

        ret = hi_mpi_ive_canny_edge(&canny_info->edge, &canny_info->stack);
        sample_svp_check_exps_return(ret != HI_SUCCESS, ret, SAMPLE_SVP_ERR_LEVEL_ERROR,
            "Error(%#x),hi_mpi_ive_canny_edge failed!\n", ret);

        ret = sample_common_ive_write_file(&canny_info->edge, canny_info->fp_dst);
        sample_svp_check_exps_return(ret != HI_SUCCESS, ret, SAMPLE_SVP_ERR_LEVEL_ERROR,
            "Error(%#x),Write edge file failed!\n", ret);
    }
    return HI_SUCCESS;
}


/*
 * function : show part canny sample
 */
static hi_s32 sample_ive_part_canny(hi_sample_ive_canny_info *canny_info)
{
    hi_s32 ret, i;
    hi_bool is_instant = HI_FALSE;
    hi_bool is_block = HI_TRUE;
    hi_bool is_finish = HI_FALSE;
    hi_ive_handle handle;

    for (i = 0; (i < 1) && (g_stop_signal == HI_FALSE); i++) {
        ret = sample_common_ive_read_file(&canny_info->src, canny_info->fp_src);
        sample_svp_check_exps_return(ret != HI_SUCCESS, ret, SAMPLE_SVP_ERR_LEVEL_ERROR,
            "Error(%#x),Read src file failed!\n", ret);

        ret = hi_mpi_ive_mag_and_ang(&handle, &canny_info->src, &canny_info->mag, HI_NULL,
            &canny_info->mag_and_ang_ctrl, is_instant);
        sample_svp_check_exps_return(ret != HI_SUCCESS, ret, SAMPLE_SVP_ERR_LEVEL_ERROR,
            "Error(%#x),hi_mpi_ive_mag_and_ang failed!\n", ret);

        is_instant = HI_TRUE;
        ret = hi_mpi_ive_threshold_u16(&handle, &canny_info->mag, &canny_info->edge,
            &canny_info->thr_u16_ctrl, is_instant);
        sample_svp_check_exps_return(ret != HI_SUCCESS, ret, SAMPLE_SVP_ERR_LEVEL_ERROR,
            "Error(%#x),hi_mpi_ive_threshold_u16 failed!\n", ret);

        ret = hi_mpi_ive_query(handle, &is_finish, is_block);
        while (ret == HI_ERR_IVE_QUERY_TIMEOUT) {
            usleep(HI_SAMPLE_IVE_CANNY_QUERY_SLEEP);
            ret = hi_mpi_ive_query(handle, &is_finish, is_block);
        }
        sample_svp_check_exps_return(ret != HI_SUCCESS, ret, SAMPLE_SVP_ERR_LEVEL_ERROR,
            "Error(%#x),hi_mpi_ive_query failed!\n", ret);

        ret = sample_common_ive_write_file(&canny_info->edge, canny_info->fp_dst);
        sample_svp_check_exps_return(ret != HI_SUCCESS, ret, SAMPLE_SVP_ERR_LEVEL_ERROR,
            "Error(%#x),Write edge file failed!\n", ret);
    }
    return HI_SUCCESS;
}

static hi_void sample_ive_canny_stop(hi_void)
{
    sample_ive_canny_uninit(&g_canny_info);
    (hi_void)memset_s(&g_canny_info, sizeof(g_canny_info), 0, sizeof(g_canny_info));
    sample_common_ive_mpi_exit();
    printf("\033[0;31mprogram termination abnormally!\033[0;39m\n");
}

/*
 * function : show canny sample
 */
hi_void sample_ive_canny(hi_char canny_complete)
{
    const hi_u16 width = HI_SAMPLE_IVE_D1_WIDTH;
    const hi_u16 height = HI_SAMPLE_IVE_D1_HEIGHT;
    hi_s32 ret;

    (hi_void)memset_s(&g_canny_info, sizeof(g_canny_info), 0, sizeof(g_canny_info));

    ret = sample_common_ive_check_mpi_init();
    sample_svp_check_exps_return_void(ret != HI_TRUE, SAMPLE_SVP_ERR_LEVEL_ERROR, "ive_check_mpi_init failed!\n");

    ret = sample_ive_canny_init(&g_canny_info, width, height, canny_complete);
    sample_svp_check_exps_goto(ret != HI_SUCCESS, canny_fail, SAMPLE_SVP_ERR_LEVEL_ERROR,
        "Error(%#x),sample_ive_canny_init failed!\n", ret);

    if (canny_complete == '0') {
        ret = sample_ive_part_canny(&g_canny_info);
    } else {
        ret = sample_ive_complete_canny(&g_canny_info);
    }

    if (g_stop_signal == HI_TRUE) {
        sample_ive_canny_stop();
        return;
    }

    if (ret == HI_SUCCESS) {
        sample_svp_trace_info("Process success!\n");
    }

    g_stop_signal = HI_TRUE;
    sample_ive_canny_uninit(&g_canny_info);
    (hi_void)memset_s(&g_canny_info, sizeof(g_canny_info), 0, sizeof(g_canny_info));

canny_fail:
    g_stop_signal = HI_TRUE;
    sample_common_ive_mpi_exit();
}

/*
 * function :Canny sample signal handle
 */
hi_void sample_ive_canny_handle_sig(hi_void)
{
    g_stop_signal = HI_TRUE;
}
