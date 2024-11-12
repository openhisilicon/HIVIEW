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

#define HI_SAMPLE_IVE_RESIZE_D1_WIDTH           720
#define HI_SAMPLE_IVE_RESIZE_D1_HEIGHT          576
#define HI_SAMPLE_IVE_RESIZE_NUM                8
#define HI_SAMPLE_IVE_RESIZE_SLEEP              100
#define HI_SAMPLE_RESIZE_TASK_INFO_SIZE         12
#define HI_SAMPLE_RESIZE_ONE_BATCH_SIZE         64
#define HI_SAMPLE_RESIZE_CBCR_NUM_1             1
#define HI_SAMPLE_RESIZE_CBCR_NUM_2             2
#define HI_SAMPLE_RESIZE_CBCR_NUM_3             3
 
typedef struct {
    hi_svp_src_img src[HI_SAMPLE_IVE_RESIZE_NUM];
    hi_svp_dst_img dst[HI_SAMPLE_IVE_RESIZE_NUM];
    hi_ive_resize_ctrl ctrl;
    FILE *fp_src;
    FILE *fp_dst;
} hi_sample_ive_resize_info;
 
static hi_sample_ive_resize_info g_resize_info;
static hi_bool g_stop_signal = HI_FALSE;
 
/*
 * function : resize uninit
 */
static hi_void sample_ive_resize_uninit(hi_sample_ive_resize_info *resize_info)
{
    hi_u16 i;
 
    for (i = 0; i < resize_info->ctrl.num; i++) {
        sample_svp_mmz_free(resize_info->src[i].phys_addr[0], resize_info->src[i].virt_addr[0]);
        sample_svp_mmz_free(resize_info->dst[i].phys_addr[0], resize_info->dst[i].virt_addr[0]);
    }
    sample_svp_mmz_free(resize_info->ctrl.mem.phys_addr, resize_info->ctrl.mem.virt_addr);
    sample_svp_close_file(resize_info->fp_src);
    sample_svp_close_file(resize_info->fp_dst);
}
 
static td_u32 sample_ive_get_resize_cbcr_num(hi_svp_img_type src_type)
{
    td_u32 cbcr_num;
    if ((src_type == HI_SVP_IMG_TYPE_YUV420SP) || (src_type == HI_SVP_IMG_TYPE_YUV422SP)) {
            cbcr_num = HI_SAMPLE_RESIZE_CBCR_NUM_2;
        } else if (src_type == HI_SVP_IMG_TYPE_U8C3_PLANAR) {
            cbcr_num = HI_SAMPLE_RESIZE_CBCR_NUM_3;
        } else {
            cbcr_num = HI_SAMPLE_RESIZE_CBCR_NUM_1;
        }
    return cbcr_num;
}
/*
 * function : resize init
 */
static hi_s32 sample_ive_resize_init(hi_sample_ive_resize_info *resize_info, hi_u32 src_width,
    hi_u32 src_height, hi_u32 dst_width, hi_u32 dst_height)
{
    hi_s32 ret;
    hi_u32 i, cbcr_num;
    td_u32 total_batch_num = 0;
    hi_char path[PATH_MAX + 1] = {0};
    hi_char tmp_file[PATH_MAX + 1] = {0};
    const hi_char *src_file = "./data/input/resize/resize_720x576_420sp.yuv";

    sample_svp_check_exps_return((strlen(src_file) > PATH_MAX) || (realpath(src_file, path) == HI_NULL),
        HI_ERR_IVE_ILLEGAL_PARAM, SAMPLE_SVP_ERR_LEVEL_ERROR, "invalid file!\n");

    (hi_void)memset_s(resize_info, sizeof(hi_sample_ive_resize_info), 0, sizeof(hi_sample_ive_resize_info));
    resize_info->ctrl.mode = HI_IVE_RESIZE_MODE_LINEAR;
    resize_info->ctrl.num = HI_SAMPLE_IVE_RESIZE_NUM;
    for (i = 0; i < resize_info->ctrl.num; i++) {
        ret = sample_common_ive_create_image(&resize_info->src[i], HI_SVP_IMG_TYPE_YUV420SP, src_width, src_height);
        sample_svp_check_exps_goto(ret != HI_SUCCESS, fail, SAMPLE_SVP_ERR_LEVEL_ERROR, "Create src img failed!\n");

        ret = sample_common_ive_create_image(&resize_info->dst[i], HI_SVP_IMG_TYPE_YUV420SP, dst_width, dst_height);
        sample_svp_check_exps_goto(ret != HI_SUCCESS, fail, SAMPLE_SVP_ERR_LEVEL_ERROR, "Create dst img failed!\n");
    }
    for (i = 0; i < resize_info->ctrl.num; i++) {
        cbcr_num = sample_ive_get_resize_cbcr_num(resize_info->src[i].type);
        total_batch_num += cbcr_num;
    }
    resize_info->ctrl.mem.size = HI_SAMPLE_RESIZE_TASK_INFO_SIZE + HI_SAMPLE_RESIZE_ONE_BATCH_SIZE * total_batch_num;
    sample_svp_check_exps_goto(resize_info->ctrl.mem.size == 0, fail, SAMPLE_SVP_ERR_LEVEL_ERROR,
        "Error,mem size can't be 0!\n");
    ret = sample_common_ive_create_mem_info(&resize_info->ctrl.mem, resize_info->ctrl.mem.size);
    sample_svp_check_exps_goto(ret != HI_SUCCESS, fail, SAMPLE_SVP_ERR_LEVEL_ERROR,
        "Error,create stack mem_info failed!\n");

    /* src file */
    ret = HI_FAILURE;
    resize_info->fp_src = fopen(path, "rb");
    sample_svp_check_exps_goto(resize_info->fp_src == HI_NULL, fail, SAMPLE_SVP_ERR_LEVEL_ERROR, "Open file failed!\n");

    /* dst file */
    sample_svp_check_exps_goto(realpath("./data/output/resize", path) == HI_NULL, fail,
        SAMPLE_SVP_ERR_LEVEL_ERROR, "invalid file!\n");
    ret = snprintf_s(tmp_file, sizeof(tmp_file) - 1, sizeof(tmp_file) - 1, "/resize_out_360x288_420sp.yuv");
    sample_svp_check_exps_goto((ret < 0) || (ret > (hi_s32)(sizeof(tmp_file) - 1)), fail,
        SAMPLE_SVP_ERR_LEVEL_ERROR, "Error,snprintf_s src file name failed!\n");

    ret = strcat_s(path, PATH_MAX, tmp_file);
    sample_svp_check_exps_goto(ret != EOK, fail, SAMPLE_SVP_ERR_LEVEL_ERROR, "strcat_s failed!\n");
    ret = HI_FAILURE;
    resize_info->fp_dst = fopen(path, "wb");
    sample_svp_check_exps_goto(resize_info->fp_dst == HI_NULL, fail, SAMPLE_SVP_ERR_LEVEL_ERROR, "Open file failed!\n");
 
    return HI_SUCCESS;
fail:
    sample_ive_resize_uninit(resize_info);
    return ret;
}

/*
 * function : show resize sample
 */
static hi_s32 sample_ive_resize_proc(hi_sample_ive_resize_info *resize_info)
{
    hi_s32 ret, i, j;
    hi_bool is_instant = HI_TRUE;
    hi_bool is_block = HI_TRUE;
    hi_bool is_finish = HI_FALSE;
    hi_ive_handle handle;
 
    for (i = 0; (i < 1) && (g_stop_signal == HI_FALSE); i++) {
        for (j = 0; j < resize_info->ctrl.num; j++) {
            ret = sample_common_ive_read_file(&(resize_info->src[j]), resize_info->fp_src);
            sample_svp_check_exps_return(ret != HI_SUCCESS, ret, SAMPLE_SVP_ERR_LEVEL_ERROR,
                "Error(%#x),Read src file failed, %d!\n", ret, j);
        }
            ret = hi_mpi_ive_resize(&handle, resize_info->src, resize_info->dst, &resize_info->ctrl,
                is_instant);
            sample_svp_check_exps_return(ret != HI_SUCCESS, ret, SAMPLE_SVP_ERR_LEVEL_ERROR,
                "Error(%#x),hi_mpi_ive_resize failed!\n", ret);
 
            ret = hi_mpi_ive_query(handle, &is_finish, is_block);
            while (ret == HI_ERR_IVE_QUERY_TIMEOUT) {
                usleep(HI_SAMPLE_IVE_RESIZE_SLEEP);
                ret = hi_mpi_ive_query(handle, &is_finish, is_block);
            }
            sample_svp_check_exps_return(ret != HI_SUCCESS, ret, SAMPLE_SVP_ERR_LEVEL_ERROR,
                "Error(%#x),hi_mpi_ive_query failed!\n", ret);
        for (j = 0; j < resize_info->ctrl.num; j++) {
            ret = sample_common_ive_write_file(&resize_info->dst[j], resize_info->fp_dst);
            sample_svp_check_exps_return(ret != HI_SUCCESS, ret, SAMPLE_SVP_ERR_LEVEL_ERROR,
                "Error(%#x),Write dst file failed!\n", ret);
        }
    }
    return HI_SUCCESS;
}
static hi_void sample_ive_resize_stop(hi_void)
{
    sample_ive_resize_uninit(&g_resize_info);
    (hi_void)memset_s(&g_resize_info, sizeof(g_resize_info), 0, sizeof(g_resize_info));
    sample_common_ive_mpi_exit();
    printf("\033[0;31mprogram termination abnormally!\033[0;39m\n");
}

/*
 * function : show resize sample
 */
hi_void sample_ive_resize(hi_void)
{
    const hi_u16 src_width = HI_SAMPLE_IVE_RESIZE_D1_WIDTH;
    const hi_u16 src_height = HI_SAMPLE_IVE_RESIZE_D1_HEIGHT;
    const hi_u16 dst_width = HI_SAMPLE_IVE_RESIZE_D1_WIDTH >> 1;
    const hi_u16 dst_height = HI_SAMPLE_IVE_RESIZE_D1_HEIGHT >> 1;
    hi_s32 ret;
 
    (hi_void)memset_s(&g_resize_info, sizeof(g_resize_info), 0, sizeof(g_resize_info));
 
    ret = sample_common_ive_check_mpi_init();
    sample_svp_check_exps_return_void(ret != HI_TRUE, SAMPLE_SVP_ERR_LEVEL_ERROR, "ive_check_mpi_init failed!\n");

    ret = sample_ive_resize_init(&g_resize_info, src_width, src_height, dst_width, dst_height);
    sample_svp_check_exps_goto(ret != HI_SUCCESS, resize_fail, SAMPLE_SVP_ERR_LEVEL_ERROR,
        "Error(%#x),sample_ive_resize_init failed!\n", ret);
 
    ret = sample_ive_resize_proc(&g_resize_info);
    if (g_stop_signal == HI_TRUE) {
        sample_ive_resize_stop();
        return;
    }
    if (ret == HI_SUCCESS) {
        sample_svp_trace_info("Process success!\n");
    } else {
        sample_svp_trace_err("resize process failed!\n");
    }

    g_stop_signal = HI_TRUE;
    sample_ive_resize_uninit(&g_resize_info);
    (hi_void)memset_s(&g_resize_info, sizeof(g_resize_info), 0, sizeof(g_resize_info));
 
resize_fail:
    g_stop_signal = HI_TRUE;
    sample_common_ive_mpi_exit();
}

/*
 * function :resize sample signal handle
 */
hi_void sample_ive_resize_handle_sig(hi_void)
{
    g_stop_signal = HI_TRUE;
}