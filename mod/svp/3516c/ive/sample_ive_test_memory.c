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

#define HI_SAMPLE_IVE_TEST_MEM_QUERY_SLEEP      100
#define HI_SAMPLE_IVE_TEST_MEM_QUERY_D1_WIDTH   720
#define HI_SAMPLE_IVE_TEST_MEM_QUERY_D1_HEIGHT  576

typedef struct {
    hi_svp_src_img src1;
    hi_svp_src_img src2;
    hi_svp_dst_img dst;
    hi_svp_dst_mem_info hist;
    hi_ive_sub_ctrl sub_ctrl;
    FILE *fp_src;
    FILE *fp_dst;
} hi_test_memory_info;

static hi_test_memory_info g_test_mem_info;
static hi_bool g_stop_signal = HI_FALSE;

/*
 * function : test memory uninit
 */
static hi_void sample_ive_test_memory_uninit(hi_test_memory_info *test_mem)
{
    sample_svp_check_exps_return_void(test_mem == HI_NULL, SAMPLE_SVP_ERR_LEVEL_ERROR, "test_mem can't be null\n");
    sample_svp_mmz_free(test_mem->src1.phys_addr[0], test_mem->src1.virt_addr[0]);
    sample_svp_mmz_free(test_mem->src2.phys_addr[0], test_mem->src2.virt_addr[0]);
    sample_svp_mmz_free(test_mem->dst.phys_addr[0], test_mem->dst.virt_addr[0]);
    sample_svp_mmz_free(test_mem->hist.phys_addr, test_mem->hist.virt_addr);

    sample_svp_close_file(test_mem->fp_src);
    sample_svp_close_file(test_mem->fp_dst);
}

/*
 * function : test memory init
 */
static hi_s32 sample_ive_test_memory_init(hi_test_memory_info *test_mem, hi_u32 width, hi_u32 height)
{
    hi_s32 ret = HI_ERR_IVE_NULL_PTR;
    hi_u32 size;
    hi_char path[PATH_MAX + 1] = {0};
    const hi_char *src_file = "./data/input/testmem/test_mem_in.yuv";

    sample_svp_check_exps_return(test_mem == HI_NULL, ret, SAMPLE_SVP_ERR_LEVEL_ERROR, "test_mem can't be null\n");
    sample_svp_check_exps_return((strlen(src_file) > PATH_MAX) || (realpath(src_file, path) == HI_NULL),
        HI_ERR_IVE_ILLEGAL_PARAM, SAMPLE_SVP_ERR_LEVEL_ERROR, "invalid file!\n");

    (hi_void)memset_s(test_mem, sizeof(hi_test_memory_info), 0, sizeof(hi_test_memory_info));

    ret = sample_common_ive_create_image(&(test_mem->src1), HI_SVP_IMG_TYPE_U8C1, width, height);
    sample_svp_check_exps_goto(ret != HI_SUCCESS, fail, SAMPLE_SVP_ERR_LEVEL_ERROR,
        "Error(%#x),Create src1 image failed!\n", ret);

    ret = sample_common_ive_create_image(&(test_mem->src2), HI_SVP_IMG_TYPE_U8C1, width, height);
    sample_svp_check_exps_goto(ret != HI_SUCCESS, fail, SAMPLE_SVP_ERR_LEVEL_ERROR,
        "Error(%#x),Create src2 image failed!\n", ret);

    ret = sample_common_ive_create_image(&(test_mem->dst), HI_SVP_IMG_TYPE_U8C1, width, height);
    sample_svp_check_exps_goto(ret != HI_SUCCESS, fail, SAMPLE_SVP_ERR_LEVEL_ERROR,
        "Error(%#x),Create dst image failed!\n", ret);

    size = HI_IVE_HIST_NUM * sizeof(hi_u32);
    ret = sample_common_ive_create_mem_info(&(test_mem->hist), size);
    sample_svp_check_exps_goto(ret != HI_SUCCESS, fail, SAMPLE_SVP_ERR_LEVEL_ERROR,
        "Error(%#x),Create hist meminfo failed!\n", ret);

    test_mem->sub_ctrl.mode = HI_IVE_SUB_MODE_ABS;

    /* open src file */
    ret = HI_FAILURE;
    test_mem->fp_src = fopen(path, "rb");
    sample_svp_check_exps_goto(test_mem->fp_src == HI_NULL, fail, SAMPLE_SVP_ERR_LEVEL_ERROR,
        "Error,Open file %s failed!\n", path);

    /* open dst file */
    sample_svp_check_exps_goto(realpath("./data/output/testmem", path) == NULL, fail, SAMPLE_SVP_ERR_LEVEL_ERROR,
        "invalid file!\n");
    ret = strcat_s(path, PATH_MAX, "/test_mem_out.yuv");
    sample_svp_check_exps_goto(ret != EOK, fail, SAMPLE_SVP_ERR_LEVEL_ERROR, "strcat_s failed!\n");
    ret = HI_FAILURE;
    test_mem->fp_dst = fopen(path, "wb");
    sample_svp_check_exps_goto(test_mem->fp_dst == HI_NULL, fail, SAMPLE_SVP_ERR_LEVEL_ERROR,
        "Error,Open file %s failed!\n", path);

    return HI_SUCCESS;
fail:
    sample_ive_test_memory_uninit(test_mem);
    return ret;
}

static hi_void sample_ive_test_memory_init_data(const hi_test_memory_info *test_mem, hi_svp_src_data *src_data,
    hi_svp_dst_data *dst_data)
{
    src_data->virt_addr = test_mem->src1.virt_addr[0];
    src_data->phys_addr = test_mem->src1.phys_addr[0];
    src_data->width = test_mem->src1.width;
    src_data->height = test_mem->src1.height;
    src_data->stride = test_mem->src1.stride[0];

    dst_data->virt_addr = test_mem->src2.virt_addr[0];
    dst_data->phys_addr = test_mem->src2.phys_addr[0];
    dst_data->width = test_mem->src2.width;
    dst_data->height = test_mem->src2.height;
    dst_data->stride = test_mem->src2.stride[0];
}
/*
 * function : test memory
 */
static hi_s32 sample_ive_test_memory_proc(hi_test_memory_info *test_mem)
{
    hi_s32 ret;
    hi_u32 *hist_ptr = HI_NULL;
    hi_u32 i, j;
    hi_ive_handle handle;
    hi_bool is_instant = HI_FALSE;
    hi_bool is_finish = HI_FALSE;
    hi_svp_src_data src_data;
    hi_svp_dst_data dst_data;
    hi_ive_dma_ctrl dma_ctrl;

    sample_svp_check_exps_return(test_mem == HI_NULL, HI_ERR_IVE_NULL_PTR,
        SAMPLE_SVP_ERR_LEVEL_ERROR, "test_mem can't be null\n");

    for (j = 0; (j < 1) && (g_stop_signal == HI_FALSE); j++) {
        ret = sample_common_ive_read_file(&(test_mem->src1), test_mem->fp_src);
        sample_svp_check_exps_return(ret != HI_SUCCESS, ret, SAMPLE_SVP_ERR_LEVEL_ERROR, "read src file failed!\n");

        dma_ctrl.mode = HI_IVE_DMA_MODE_DIRECT_COPY;

        sample_ive_test_memory_init_data(test_mem, &src_data, &dst_data);

        ret = hi_mpi_ive_dma(&handle, &src_data, &dst_data, &dma_ctrl, is_instant);
        sample_svp_check_exps_return(ret != HI_SUCCESS, ret, SAMPLE_SVP_ERR_LEVEL_ERROR, "hi_mpi_ive_dma failed!\n");

        ret = hi_mpi_ive_sub(&handle, &test_mem->src1, &test_mem->src2, &test_mem->dst,
            &test_mem->sub_ctrl, is_instant);
        sample_svp_check_exps_return(ret != HI_SUCCESS, ret, SAMPLE_SVP_ERR_LEVEL_ERROR,
            "Error(%#x),hi_mpi_ive_sub failed!\n", ret);

        is_instant = HI_TRUE;
        ret = hi_mpi_ive_hist(&handle, &test_mem->dst, &test_mem->hist, is_instant);
        sample_svp_check_exps_return(ret != HI_SUCCESS, ret, SAMPLE_SVP_ERR_LEVEL_ERROR, "hi_mpi_ive_hist failed!\n");

        ret = hi_mpi_ive_query(handle, &is_finish, HI_TRUE);
        while (ret == HI_ERR_IVE_QUERY_TIMEOUT) {
            usleep(HI_SAMPLE_IVE_TEST_MEM_QUERY_SLEEP);
            ret = hi_mpi_ive_query(handle, &is_finish, HI_TRUE);
        }
        sample_svp_check_exps_return(ret != HI_SUCCESS, ret, SAMPLE_SVP_ERR_LEVEL_ERROR, "hi_mpi_ive_query failed!\n");

        ret = sample_common_ive_write_file(&test_mem->dst, test_mem->fp_dst);
        sample_svp_check_exps_return(ret != HI_SUCCESS, ret, SAMPLE_SVP_ERR_LEVEL_ERROR, "write dst file failed!\n");

        hist_ptr = sample_svp_convert_addr_to_ptr(hi_u32, test_mem->hist.virt_addr);
        if (hist_ptr[0] != (test_mem->src1.width * test_mem->src1.height)) {
            ret = HI_FAILURE;
            sample_svp_trace_err("Test mem error,hist_ptr[0] = %d\n", hist_ptr[0]);
            for (i = 1; i < HI_IVE_HIST_NUM; i++) {
                sample_svp_check_exps_trace(hist_ptr[i] != 0, SAMPLE_SVP_ERR_LEVEL_ERROR,
                    "Test mem error, hist_ptr[%d] = %d\n", i, hist_ptr[i]);
            }
        }
    }
    return HI_SUCCESS;
}

static hi_void sample_ive_test_mem_stop(hi_void)
{
    sample_ive_test_memory_uninit(&g_test_mem_info);
    (hi_void)memset_s(&g_test_mem_info, sizeof(g_test_mem_info), 0, sizeof(g_test_mem_info));
    sample_common_ive_mpi_exit();
    printf("\033[0;31mprogram termination abnormally!\033[0;39m\n");
}
/*
 * function : Show test memory sample
 */
hi_void sample_ive_test_memory(hi_void)
{
    hi_s32 ret;
    const hi_u32 width = HI_SAMPLE_IVE_TEST_MEM_QUERY_D1_WIDTH;
    const hi_u32 height = HI_SAMPLE_IVE_TEST_MEM_QUERY_D1_HEIGHT;

    (hi_void)memset_s(&g_test_mem_info, sizeof(g_test_mem_info), 0, sizeof(g_test_mem_info));
    ret = sample_common_ive_check_mpi_init();
    sample_svp_check_exps_return_void(ret != HI_TRUE, SAMPLE_SVP_ERR_LEVEL_ERROR, "ive_check_mpi_init failed!\n");

    ret = sample_ive_test_memory_init(&g_test_mem_info, width, height);
    sample_svp_check_exps_goto(ret != HI_SUCCESS, test_memory_fail, SAMPLE_SVP_ERR_LEVEL_ERROR,
        "Error(%#x),sample_ive_test_memory_init failed!\n", ret);

    ret = sample_ive_test_memory_proc(&g_test_mem_info);
    if (g_stop_signal == HI_TRUE) {
        sample_ive_test_mem_stop();
        return;
    }
    if (ret == HI_SUCCESS) {
        sample_svp_trace_info("Process success!\n");
    } else {
        sample_svp_trace_err("test_memory process failed\n");
    }

    g_stop_signal = HI_TRUE;
    sample_ive_test_memory_uninit(&g_test_mem_info);
    (hi_void)memset_s(&g_test_mem_info, sizeof(g_test_mem_info), 0, sizeof(g_test_mem_info));

test_memory_fail:
    g_stop_signal = HI_TRUE;
    sample_common_ive_mpi_exit();
}

/*
 * function : TestMemory sample signal handle
 */
hi_void sample_ive_test_memory_handle_sig(hi_void)
{
    g_stop_signal = HI_TRUE;
}
