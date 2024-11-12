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

#define HI_SAMPLE_IVE_SOBEL_QUERY_SLEEP         100
#define HI_SAMPLE_IVE_SOBEL_SRC_WIDTH           720
#define HI_SAMPLE_IVE_SOBEL_SRC_HEIGHT          576

typedef struct {
    hi_svp_src_img src1;
    hi_svp_src_img src2;
    hi_svp_dst_img dst_h1;
    hi_svp_dst_img dst_h2;
    hi_svp_dst_img dst_v1;
    hi_svp_dst_img dst_v2;
    hi_ive_sobel_ctrl sobel_ctrl;
    FILE *fp_src;
    FILE *fp_dst_h1;
    FILE *fp_dst_h2;
    FILE *fp_dst_v1;
    FILE *fp_dst_v2;
} hi_sample_ive_sobel_info;

static hi_bool g_is_flush_cached = HI_TRUE;
static hi_sample_ive_sobel_info g_sobel_info;
static hi_bool g_stop_signal = HI_FALSE;

/*
 * function : show Sobel uninit
 */
static hi_void sample_ive_sobel_uninit(hi_sample_ive_sobel_info *sobel)
{
    sample_svp_check_exps_return_void(sobel == HI_NULL, SAMPLE_SVP_ERR_LEVEL_ERROR, "test_mem can't be null\n");

    sample_svp_mmz_free(sobel->src1.phys_addr[0], sobel->src1.virt_addr[0]);
    sample_svp_mmz_free(sobel->src2.phys_addr[0], sobel->src2.virt_addr[0]);
    sample_svp_mmz_free(sobel->dst_h1.phys_addr[0], sobel->dst_h1.virt_addr[0]);
    sample_svp_mmz_free(sobel->dst_h2.phys_addr[0], sobel->dst_h2.virt_addr[0]);
    sample_svp_mmz_free(sobel->dst_v1.phys_addr[0], sobel->dst_v1.virt_addr[0]);
    sample_svp_mmz_free(sobel->dst_v2.phys_addr[0], sobel->dst_v2.virt_addr[0]);

    sample_svp_close_file(sobel->fp_src);
    sample_svp_close_file(sobel->fp_dst_h1);
    sample_svp_close_file(sobel->fp_dst_h2);
    sample_svp_close_file(sobel->fp_dst_v1);
    sample_svp_close_file(sobel->fp_dst_v2);
}

static hi_s32 sample_ive_sobel_create_img(hi_sample_ive_sobel_info *sobel, hi_u32 width, hi_u32 height)
{
    hi_s32 ret;
    ret = sample_common_ive_create_image_by_cached(&(sobel->src1), HI_SVP_IMG_TYPE_U8C1, width, height);
    sample_svp_check_exps_return(ret != HI_SUCCESS, ret, SAMPLE_SVP_ERR_LEVEL_ERROR,
        "Error(%#x),Create src1 image failed!\n", ret);

    ret = sample_common_ive_create_image_by_cached(&(sobel->src2), HI_SVP_IMG_TYPE_U8C1, width, height);
    sample_svp_check_exps_return(ret != HI_SUCCESS, ret, SAMPLE_SVP_ERR_LEVEL_ERROR,
        "Error(%#x),Create src2 image failed!\n", ret);

    ret = sample_common_ive_create_image(&(sobel->dst_h1), HI_SVP_IMG_TYPE_S16C1, width, height);
    sample_svp_check_exps_return(ret != HI_SUCCESS, ret, SAMPLE_SVP_ERR_LEVEL_ERROR,
        "Error(%#x),Create dstH1 image failed!\n", ret);

    ret = sample_common_ive_create_image(&(sobel->dst_h2), HI_SVP_IMG_TYPE_S16C1, width, height);
    sample_svp_check_exps_return(ret != HI_SUCCESS, ret, SAMPLE_SVP_ERR_LEVEL_ERROR,
        "Error(%#x),Create dstH2 image failed!\n", ret);

    ret = sample_common_ive_create_image(&(sobel->dst_v1), HI_SVP_IMG_TYPE_S16C1, width, height);
    sample_svp_check_exps_return(ret != HI_SUCCESS, ret, SAMPLE_SVP_ERR_LEVEL_ERROR,
        "Error(%#x),Create dstV1 image failed!\n", ret);

    ret = sample_common_ive_create_image(&(sobel->dst_v2), HI_SVP_IMG_TYPE_S16C1, width, height);
    sample_svp_check_exps_return(ret != HI_SUCCESS, ret, SAMPLE_SVP_ERR_LEVEL_ERROR,
        "Error(%#x),Create dstV2 image failed!\n", ret);
    return HI_SUCCESS;
}

/*
 * function : show Sobel init
 */
static hi_s32 sample_ive_sobel_init(hi_sample_ive_sobel_info *sobel, hi_u32 width, hi_u32 height)
{
    hi_s32 ret = HI_ERR_IVE_NULL_PTR;
    hi_char path[PATH_MAX + 1] = {0};
    hi_s8 mask[HI_IVE_MASK_NUM] = { 0, 0, 0, 0, 0, 0, -1, 0, 1, 0, 0, -2, 0, 2, 0, 0, -1, 0, 1, 0, 0, 0, 0, 0, 0 };
    const hi_char *src_sobel = "./data/input/sobel/sobel.yuv";
    const hi_char *file = "./data/output/sobel";

    sample_svp_check_exps_return(sobel == HI_NULL, ret, SAMPLE_SVP_ERR_LEVEL_ERROR, "test_mem can't be null\n");
    (hi_void)memset_s(sobel, sizeof(hi_sample_ive_sobel_info), 0, sizeof(hi_sample_ive_sobel_info));

    sobel->sobel_ctrl.out_ctrl = HI_IVE_SOBEL_OUT_CTRL_BOTH;
    ret = memcpy_s(sobel->sobel_ctrl.mask, HI_IVE_MASK_NUM, mask, HI_IVE_MASK_NUM);
    sample_svp_check_exps_return(ret != EOK, HI_ERR_IVE_ILLEGAL_PARAM,
        SAMPLE_SVP_ERR_LEVEL_ERROR, "memcpy_s mask failed!\n");

    ret = sample_ive_sobel_create_img(sobel, width, height);
    sample_svp_check_exps_goto(ret != HI_SUCCESS, fail, SAMPLE_SVP_ERR_LEVEL_ERROR,
        "Error(%#x),Sobel create image failed!\n", ret);

    ret = HI_FAILURE;
    sample_svp_check_exps_goto((realpath(src_sobel, path) == NULL) || (strlen(src_sobel) > PATH_MAX),
        fail, SAMPLE_SVP_ERR_LEVEL_ERROR, "invalid file!\n");
    sobel->fp_src = fopen(path, "rb");
    sample_svp_check_exps_goto(sobel->fp_src == HI_NULL, fail, SAMPLE_SVP_ERR_LEVEL_ERROR, "Open file failed!\n");

    /* sobelh1 */
    sample_svp_check_exps_goto(realpath(file, path) == NULL, fail, SAMPLE_SVP_ERR_LEVEL_ERROR, "invalid file!\n");
    ret = strcat_s(path, PATH_MAX, "/sobelh1.dat");
    sample_svp_check_exps_goto(ret != EOK, fail, SAMPLE_SVP_ERR_LEVEL_ERROR, "strcat_s failed!\n");
    ret = HI_FAILURE;
    sobel->fp_dst_h1 = fopen(path, "wb");
    sample_svp_check_exps_goto(sobel->fp_dst_h1 == HI_NULL, fail, SAMPLE_SVP_ERR_LEVEL_ERROR, "Open file failed!\n");

    /* sobelh2 */
    sample_svp_check_exps_goto(realpath(file, path) == NULL, fail, SAMPLE_SVP_ERR_LEVEL_ERROR, "invalid file!\n");
    ret = strcat_s(path, PATH_MAX, "/sobelh2.dat");
    sample_svp_check_exps_goto(ret != EOK, fail, SAMPLE_SVP_ERR_LEVEL_ERROR, "strcat_s failed!\n");
    ret = HI_FAILURE;
    sobel->fp_dst_h2 = fopen(path, "wb");
    sample_svp_check_exps_goto(sobel->fp_dst_h2 == HI_NULL, fail, SAMPLE_SVP_ERR_LEVEL_ERROR, "Open file failed!\n");

    /* sobelv1 */
    sample_svp_check_exps_goto(realpath(file, path) == NULL, fail, SAMPLE_SVP_ERR_LEVEL_ERROR, "invalid file!\n");
    ret = strcat_s(path, PATH_MAX, "/sobelv1.dat");
    sample_svp_check_exps_goto(ret != EOK, fail, SAMPLE_SVP_ERR_LEVEL_ERROR, "strcat_s failed!\n");
    ret = HI_FAILURE;
    sobel->fp_dst_v1 = fopen(path, "wb");
    sample_svp_check_exps_goto(sobel->fp_dst_v1 == HI_NULL, fail,  SAMPLE_SVP_ERR_LEVEL_ERROR, "Open file failed!\n");

    /* sobelv2 */
    sample_svp_check_exps_goto(realpath(file, path) == NULL, fail, SAMPLE_SVP_ERR_LEVEL_ERROR, "invalid file!\n");
    ret = strcat_s(path, PATH_MAX, "/sobelv2.dat");
    sample_svp_check_exps_goto(ret != EOK, fail, SAMPLE_SVP_ERR_LEVEL_ERROR, "strcat_s failed!\n");
    ret = HI_FAILURE;
    sobel->fp_dst_v2 = fopen(path, "wb");
    sample_svp_check_exps_goto(sobel->fp_dst_v2 == HI_NULL, fail,  SAMPLE_SVP_ERR_LEVEL_ERROR, "Open file failed!\n");

    return HI_SUCCESS;
fail:
    sample_ive_sobel_uninit(sobel);
    return ret;
}

static hi_s32 sample_ive_sobel_query_task(hi_ive_handle handle)
{
    hi_s32 ret;
    hi_bool is_block = HI_TRUE;
    hi_bool is_finish = HI_FALSE;

    ret = hi_mpi_ive_query(handle, &is_finish, is_block);
    while (ret == HI_ERR_IVE_QUERY_TIMEOUT) {
        usleep(HI_SAMPLE_IVE_SOBEL_QUERY_SLEEP);
        ret = hi_mpi_ive_query(handle, &is_finish, is_block);
    }
    sample_svp_check_exps_return(ret != HI_SUCCESS, ret, SAMPLE_SVP_ERR_LEVEL_ERROR,
        "Error(%#x),hi_mpi_ive_query failed!\n", ret);

    return HI_SUCCESS;
}
/*
 * function : show Sobel proc
 */
static hi_s32 sample_ive_sobel_proc(hi_sample_ive_sobel_info *sobel)
{
    hi_s32 ret = HI_ERR_IVE_NULL_PTR;
    hi_bool is_instant = HI_TRUE;
    hi_ive_handle handle;
    hi_u32 i;

    sample_svp_check_exps_return(sobel == HI_NULL, ret, SAMPLE_SVP_ERR_LEVEL_ERROR, "test_mem can't be null\n");

    for (i = 0; (i < 1) && (g_stop_signal == HI_FALSE); i++) {
        ret = sample_common_ive_read_file(&(sobel->src1), sobel->fp_src);
        sample_svp_check_exps_return(ret != HI_SUCCESS, ret, SAMPLE_SVP_ERR_LEVEL_ERROR, "read src file failed!\n");

        ret = memcpy_s(sample_svp_convert_addr_to_ptr(hi_void, sobel->src2.virt_addr[0]),
            sobel->src2.stride[0] * sobel->src2.height,
            sample_svp_convert_addr_to_ptr(hi_void, sobel->src1.virt_addr[0]),
            sobel->src2.stride[0] * sobel->src2.height);
        sample_svp_check_exps_return(ret != EOK, HI_ERR_IVE_ILLEGAL_PARAM,
            SAMPLE_SVP_ERR_LEVEL_ERROR, "memcpy_s src failed!\n");

        ret = hi_mpi_sys_flush_cache(sobel->src1.phys_addr[0], sample_svp_convert_addr_to_ptr(hi_void,
            sobel->src1.virt_addr[0]), sobel->src1.stride[0] * sobel->src1.height);
        sample_svp_check_exps_return(ret != HI_SUCCESS, ret, SAMPLE_SVP_ERR_LEVEL_ERROR,
            "Error(%#x),hi_mpi_sys_flush_cache failed!\n", ret);

        ret = hi_mpi_ive_sobel(&handle, &sobel->src1, &sobel->dst_h1, &sobel->dst_v1, &sobel->sobel_ctrl, is_instant);
        sample_svp_check_exps_return(ret != HI_SUCCESS, ret, SAMPLE_SVP_ERR_LEVEL_ERROR, "hi_mpi_ive_sobel failed!\n");

        ret = sample_ive_sobel_query_task(handle);
        sample_svp_check_exps_return(ret != HI_SUCCESS, ret, SAMPLE_SVP_ERR_LEVEL_ERROR, "sobel_query_task failed!\n");

        ret = sample_common_ive_write_file(&sobel->dst_h1, sobel->fp_dst_h1);
        sample_svp_check_exps_return(ret != HI_SUCCESS, ret, SAMPLE_SVP_ERR_LEVEL_ERROR, "write dstH1 failed!\n");

        ret = sample_common_ive_write_file(&sobel->dst_v1, sobel->fp_dst_v1);
        sample_svp_check_exps_return(ret != HI_SUCCESS, ret, SAMPLE_SVP_ERR_LEVEL_ERROR, "write dstV1 failed!\n");

        /* The result of sobel my be error,if you do not call hi_mpi_sys_flush_cache to flush cache */
        if (g_is_flush_cached == HI_TRUE) {
            ret = hi_mpi_sys_flush_cache(sobel->src2.phys_addr[0], sample_svp_convert_addr_to_ptr(hi_void,
                sobel->src2.virt_addr[0]), sobel->src2.stride[0] * sobel->src2.height);
            sample_svp_check_exps_return(ret != HI_SUCCESS, ret, SAMPLE_SVP_ERR_LEVEL_ERROR,
                "Error(%#x),hi_mpi_sys_flush_cache failed!\n", ret);
        }

        ret = hi_mpi_ive_sobel(&handle, &sobel->src2, &sobel->dst_h2, &sobel->dst_v2, &sobel->sobel_ctrl, is_instant);
        sample_svp_check_exps_return(ret != HI_SUCCESS, ret, SAMPLE_SVP_ERR_LEVEL_ERROR, "hi_mpi_ive_sobel failed!\n");

        ret = sample_ive_sobel_query_task(handle);
        sample_svp_check_exps_return(ret != HI_SUCCESS, ret, SAMPLE_SVP_ERR_LEVEL_ERROR, "sobel_query_task failed!\n");

        ret = sample_common_ive_write_file(&sobel->dst_h2, sobel->fp_dst_h2);
        sample_svp_check_exps_return(ret != HI_SUCCESS, ret, SAMPLE_SVP_ERR_LEVEL_ERROR, "write dstH2 failed!\n");

        ret = sample_common_ive_write_file(&sobel->dst_v2, sobel->fp_dst_v2);
        sample_svp_check_exps_return(ret != HI_SUCCESS, ret, SAMPLE_SVP_ERR_LEVEL_ERROR, "write dstV2 failed!\n");
    }
    return HI_SUCCESS;
}

static hi_void sample_ive_sobel_stop(hi_void)
{
    sample_ive_sobel_uninit(&g_sobel_info);
    (hi_void)memset_s(&g_sobel_info, sizeof(g_sobel_info), 0, sizeof(g_sobel_info));
    sample_common_ive_mpi_exit();
    printf("\033[0;31mprogram termination abnormally!\033[0;39m\n");
}
/*
 * function : show Sobel sample
 */
hi_void sample_ive_sobel(hi_void)
{
    hi_s32 ret;
    const hi_u32 width = HI_SAMPLE_IVE_SOBEL_SRC_WIDTH;
    const hi_u32 height = HI_SAMPLE_IVE_SOBEL_SRC_HEIGHT;

    (hi_void)memset_s(&g_sobel_info, sizeof(g_sobel_info), 0, sizeof(g_sobel_info));
    ret = sample_common_ive_check_mpi_init();
    sample_svp_check_exps_return_void(ret != HI_TRUE, SAMPLE_SVP_ERR_LEVEL_ERROR, "ive_check_mpi_init failed!\n");

    ret = sample_ive_sobel_init(&g_sobel_info, width, height);
    sample_svp_check_exps_goto(ret != HI_SUCCESS, sobel_fail, SAMPLE_SVP_ERR_LEVEL_ERROR,
        "Error(%#x),sample_ive_sobel_init failed!\n", ret);

    ret = sample_ive_sobel_proc(&g_sobel_info);
    if (g_stop_signal == HI_TRUE) {
        sample_ive_sobel_stop();
        return;
    }
    if (ret == HI_SUCCESS) {
        sample_svp_trace_info("Process success!\n");
    } else {
        sample_svp_trace_err("sobel process failed!\n");
    }

    g_stop_signal = HI_TRUE;
    sample_ive_sobel_uninit(&g_sobel_info);
    (hi_void)memset_s(&g_sobel_info, sizeof(g_sobel_info), 0, sizeof(g_sobel_info));

sobel_fail:
    g_stop_signal = HI_TRUE;
    sample_common_ive_mpi_exit();
}


/*
 * function : Sobel sample signal handle
 */
hi_void sample_ive_sobel_handle_sig(hi_void)
{
    g_stop_signal = HI_TRUE;
}
