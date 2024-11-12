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
#include <sys/prctl.h>
#include <math.h>

#define HI_SAMPLE_IVE_OD_MILLI_SEC              20000
#define HI_SAMPLE_IVE_OD_QUERY_SLEEP            100
#define HI_SAMPLE_IVE_OD_LINEAR_NUM             2
#define HI_SAMPLE_IVE_OD_LINEAR_POINT0_X        80
#define HI_SAMPLE_IVE_OD_LINEAR_POINT0_Y        0
#define HI_SAMPLE_IVE_OD_LINEAR_POINT1_X        80
#define HI_SAMPLE_IVE_OD_LINEAR_POINT1_Y        20
#define HI_SAMPLE_IVE_OD_THREAD_NAME_LEN        16
#define HI_SAMPLE_IVE_OD_NUM_TWO                2
#define HI_SAMPLE_IVE_OD_POINT_NUM              10
#define HI_SAMPLE_IVE_RIGHT_SHIFT_TWENTY_EIGHT  28

typedef struct {
    hi_svp_src_img src;
    hi_svp_dst_img integ;
    hi_ive_integ_ctrl integ_ctrl;
    hi_u32 width;
    hi_u32 height;
} hi_sample_ive_od_info;

static hi_bool g_stop_signal = HI_FALSE;
static pthread_t g_ive_thread = 0;
static hi_sample_ive_od_info g_od_info;
static hi_sample_svp_switch g_od_switch = { HI_TRUE };
static sample_vi_cfg g_vi_config;

static hi_void sample_ive_od_uninit(hi_sample_ive_od_info *od_info)
{
    sample_svp_check_exps_return_void(od_info == HI_NULL, SAMPLE_SVP_ERR_LEVEL_ERROR, "od_info can't be null\n");
    sample_svp_mmz_free(od_info->src.phys_addr[0], od_info->src.virt_addr[0]);
    sample_svp_mmz_free(od_info->integ.phys_addr[0], od_info->integ.virt_addr[0]);
}

static hi_s32 sample_ive_od_init(hi_sample_ive_od_info *od_info, hi_u32 width, hi_u32 height)
{
    hi_s32 ret = HI_ERR_IVE_NULL_PTR;

    sample_svp_check_exps_return(od_info == HI_NULL, ret, SAMPLE_SVP_ERR_LEVEL_ERROR, "od_info can't be null\n");

    (hi_void)memset_s(od_info, sizeof(hi_sample_ive_od_info), 0, sizeof(hi_sample_ive_od_info));

    ret = sample_common_ive_create_image(&od_info->src, HI_SVP_IMG_TYPE_U8C1, width, height);
    sample_svp_check_exps_goto(ret != HI_SUCCESS, od_init_fail, SAMPLE_SVP_ERR_LEVEL_ERROR,
        "Error(%#x),Create src image failed!\n", ret);

    ret = sample_common_ive_create_image(&od_info->integ, HI_SVP_IMG_TYPE_U64C1, width, height);
    sample_svp_check_exps_goto(ret != HI_SUCCESS, od_init_fail, SAMPLE_SVP_ERR_LEVEL_ERROR,
        "Error(%#x),Create integ image failed!\n", ret);

    od_info->integ_ctrl.out_ctrl = HI_IVE_INTEG_OUT_CTRL_COMBINE;

    od_info->width = width / HI_IVE_CHAR_CALW;
    od_info->height = height / HI_IVE_CHAR_CALH;

od_init_fail:
    if (ret != HI_SUCCESS) {
        sample_ive_od_uninit(od_info);
    }
    return ret;
}

static hi_s32 sample_ive_linear_2d_classifer(hi_point *char_point, hi_s32 char_num, hi_point *linear_point,
    hi_s32 linear_num)
{
    hi_s32 result_num;
    hi_s32 i, j;
    hi_bool test_flag;
    hi_point *next_linear_point = HI_NULL;

    result_num = 0;
    next_linear_point = &linear_point[1];
    for (i = 0; i < char_num; i++) {
        test_flag = HI_TRUE;
        for (j = 0; j < (linear_num - 1); j++) {
            if (((char_point[i].y - linear_point[j].y) * (next_linear_point[j].x - linear_point[j].x) >
                 (char_point[i].x - linear_point[j].x) * (next_linear_point[j].y - linear_point[j].y) &&
                 (next_linear_point[j].x != linear_point[j].x)) ||
                 ((char_point[i].x > linear_point[j].x) && (next_linear_point[j].x == linear_point[j].x))) {
                test_flag = HI_FALSE;
                break;
            }
        }
        if (test_flag == HI_TRUE) {
            result_num++;
        }
    }
    return result_num;
}

static hi_void sample_ive_prepare_dma_data(hi_sample_ive_od_info *od_ptr, hi_video_frame_info *ext_frm_info,
    hi_svp_data *src_data, hi_svp_data *dst_data)
{
    src_data->virt_addr = sample_svp_convert_ptr_to_addr(hi_u64, ext_frm_info->video_frame.virt_addr[0]);
    src_data->phys_addr = ext_frm_info->video_frame.phys_addr[0];
    src_data->stride = ext_frm_info->video_frame.stride[0];
    src_data->width = ext_frm_info->video_frame.width;
    src_data->height = ext_frm_info->video_frame.height;

    dst_data->virt_addr = od_ptr->src.virt_addr[0];
    dst_data->phys_addr = od_ptr->src.phys_addr[0];
    dst_data->stride = ext_frm_info->video_frame.stride[0];
    dst_data->width = ext_frm_info->video_frame.width;
    dst_data->height = ext_frm_info->video_frame.height;
}

static hi_s32 sample_ive_query_task(hi_ive_handle handle)
{
    hi_s32 ret;
    hi_bool is_block = HI_TRUE;
    hi_bool is_finish = HI_FALSE;
    ret = hi_mpi_ive_query(handle, &is_finish, is_block);
    while (ret == HI_ERR_IVE_QUERY_TIMEOUT) {
        usleep(HI_SAMPLE_IVE_OD_QUERY_SLEEP);
        ret = hi_mpi_ive_query(handle, &is_finish, is_block);
    }
    sample_svp_check_exps_return(ret != HI_SUCCESS, ret, SAMPLE_SVP_ERR_LEVEL_ERROR,
        "Error(%#x),hi_mpi_ive_query failed!\n", ret);
    return HI_SUCCESS;
}

static hi_s32 sample_ive_get_char_point(hi_sample_ive_od_info *od_ptr, hi_point char_point[], hi_u32 length)
{
    hi_u64 *vir_data = HI_NULL;
    hi_u32 i, j;
    hi_u64 top_left, top_right, btm_left, btm_right;
    hi_u64 *top_row_ptr = HI_NULL;
    hi_u64 *btm_row_ptr = HI_NULL;
    hi_u64 block_sum, block_sqrt;
    hi_float sqrt_val;
    vir_data = sample_svp_convert_addr_to_ptr(hi_u64, od_ptr->integ.virt_addr[0]);
    sample_svp_check_exps_return(length > HI_IVE_CHAR_CALW * HI_IVE_CHAR_CALH, HI_ERR_IVE_ILLEGAL_PARAM,
        SAMPLE_SVP_ERR_LEVEL_ERROR, "length(%u) is larger than %u\n", length, HI_IVE_CHAR_CALW * HI_IVE_CHAR_CALH);

    for (j = 0; (j < HI_IVE_CHAR_CALH) && (j < length); j++) {
        top_row_ptr = (0 == j) ? (vir_data) : (vir_data + (j * od_ptr->height - 1) * od_ptr->integ.stride[0]);
        btm_row_ptr = vir_data + ((j + 1) * od_ptr->height - 1) * od_ptr->integ.stride[0];

        for (i = 0; i < HI_IVE_CHAR_CALW; i++) {
            top_left = (0 == j) ? (0) : ((0 == i) ? (0) : (top_row_ptr[i * od_ptr->width - 1]));
            top_right = (0 == j) ? (0) : (top_row_ptr[(i + 1) * od_ptr->width - 1]);
            btm_left = (0 == i) ? (0) : (btm_row_ptr[i * od_ptr->width - 1]);
            btm_right = btm_row_ptr[(i + 1) * od_ptr->width - 1];

            block_sum = (top_left & 0xfffffffLL) + (btm_right & 0xfffffffLL) -
                (btm_left & 0xfffffffLL) - (top_right & 0xfffffffLL);

            block_sqrt = (top_left >> HI_SAMPLE_IVE_RIGHT_SHIFT_TWENTY_EIGHT) +
                        (btm_right >> HI_SAMPLE_IVE_RIGHT_SHIFT_TWENTY_EIGHT) -
                        (btm_left >> HI_SAMPLE_IVE_RIGHT_SHIFT_TWENTY_EIGHT) -
                        (top_right >> HI_SAMPLE_IVE_RIGHT_SHIFT_TWENTY_EIGHT);

            /* mean */
            char_point[j * HI_IVE_CHAR_CALW + i].x = (hi_s32)(block_sum / (od_ptr->width * od_ptr->width));
            /* sigma=sqrt(1/(w*h)*sum((x(i,j)-mean)^2)= sqrt(sum(x(i,j)^2)/(w*h)-mean^2) */
            sqrt_val = (hi_s64)(block_sqrt / (od_ptr->width * od_ptr->height)) -
                char_point[j * HI_IVE_CHAR_CALW + i].x * char_point[j * HI_IVE_CHAR_CALW + i].x;
            char_point[j * HI_IVE_CHAR_CALW + i].y = (hi_s32)sqrt(sqrt_val);
        }
    }
    return HI_SUCCESS;
}

static hi_void sample_ive_set_linear_data(hi_sample_ive_linear_data *data)
{
    data->linear_num = HI_SAMPLE_IVE_OD_LINEAR_NUM;
    data->thresh_num = HI_IVE_CHAR_NUM / HI_SAMPLE_IVE_OD_NUM_TWO;
    data->linear_point[0].x = HI_SAMPLE_IVE_OD_LINEAR_POINT0_X;
    data->linear_point[0].y = HI_SAMPLE_IVE_OD_LINEAR_POINT0_Y;
    data->linear_point[1].x = HI_SAMPLE_IVE_OD_LINEAR_POINT1_X;
    data->linear_point[1].y = HI_SAMPLE_IVE_OD_LINEAR_POINT1_Y;
}

static hi_void *sample_ive_od_proc(hi_void *arg)
{
    hi_s32 ret;
    hi_sample_ive_od_info *od_ptr = HI_NULL;
    hi_video_frame_info base_frm_info, ext_frm_info;
    const hi_s32 vpss_grp = 0;
    hi_s32 vpss_chn[] = { HI_VPSS_CHN0, HI_VPSS_CHN1 };
    hi_svp_data src_data, dst_data;
    hi_ive_handle handle;
    hi_point points[HI_SAMPLE_IVE_OD_POINT_NUM] = {{ 0, 0 }};

    hi_point char_point[HI_IVE_CHAR_NUM];
    hi_sample_ive_linear_data data;
    hi_ive_dma_ctrl dma_ctrl = { HI_IVE_DMA_MODE_DIRECT_COPY, 0, 0, 0, 0 };
    data.linear_point = &points[0];

    od_ptr = (hi_sample_ive_od_info *)(arg);

    sample_svp_check_exps_return(od_ptr == HI_NULL, HI_NULL, SAMPLE_SVP_ERR_LEVEL_ERROR, "od_ptr can't be null\n");
    /* init data */
    sample_ive_set_linear_data(&data);

    while (g_stop_signal == HI_FALSE) {
        /* get frame */
        ret = hi_mpi_vpss_get_chn_frame(vpss_grp, vpss_chn[1], &ext_frm_info, HI_SAMPLE_IVE_OD_MILLI_SEC);
        sample_svp_check_exps_continue(ret != HI_SUCCESS, SAMPLE_SVP_ERR_LEVEL_DEBUG,
            "Error(%#x),hi_mpi_vpss_get_chn_frame failed, VPSS_GRP(%d), VPSS_CHN(%d)!\n", ret, vpss_grp, vpss_chn[1]);

        ret = hi_mpi_vpss_get_chn_frame(vpss_grp, vpss_chn[0], &base_frm_info, HI_SAMPLE_IVE_OD_MILLI_SEC);
        sample_svp_check_failed_goto(ret, ext_free, SAMPLE_SVP_ERR_LEVEL_ERROR,
            "Error(%#x),hi_mpi_vpss_get_chn_frame failed, VPSS_GRP(%d), VPSS_CHN(%d)!\n", ret, vpss_grp, vpss_chn[0]);

        /* prepare dma data */
        sample_ive_prepare_dma_data(od_ptr, &ext_frm_info, &src_data, &dst_data);

        /* dma */
        ret = hi_mpi_ive_dma(&handle, &src_data, &dst_data, &dma_ctrl, HI_FALSE);
        sample_svp_check_failed_err_level_goto(ret, base_free, "Error(%#x),hi_mpi_ive_dma failed!\n", ret);

        /* integ */
        ret = hi_mpi_ive_integ(&handle, &od_ptr->src, &od_ptr->integ, &od_ptr->integ_ctrl, HI_TRUE);
        sample_svp_check_failed_err_level_goto(ret, base_free, "Error(%#x),hi_mpi_ive_integ failed!\n", ret);

        /* query task */
        ret = sample_ive_query_task(handle);
        sample_svp_check_failed_err_level_goto(ret, base_free, "ive_query_task failed!\n");

        /* get result */
        ret = sample_ive_get_char_point(od_ptr, char_point, HI_IVE_CHAR_NUM);
        sample_svp_check_failed_err_level_goto(ret, base_free, "sample_ive_get_char_point failed!\n");

        /* classify */
        ret = sample_ive_linear_2d_classifer(char_point, HI_IVE_CHAR_NUM, data.linear_point, data.linear_num);
        if (ret > data.thresh_num) {
            sample_svp_trace_info("\033[0;31m Occlusion detected!\033[0;39m\n");
        }
        /* send venc frame */
        ret = hi_mpi_venc_send_frame(0, &base_frm_info, HI_SAMPLE_IVE_OD_MILLI_SEC);
        sample_svp_check_failed_err_level_goto(ret, base_free, "Error(%#x),sample_venc_send_frame failed!\n", ret);

base_free:
        ret = hi_mpi_vpss_release_chn_frame(vpss_grp, vpss_chn[0], &base_frm_info);
        sample_svp_check_exps_trace(ret != HI_SUCCESS, SAMPLE_SVP_ERR_LEVEL_DEBUG,
            "Error(%#x),release_frame failed,Grp(%d) chn(%d)!\n", ret, vpss_grp, vpss_chn[0]);

ext_free:
        ret = hi_mpi_vpss_release_chn_frame(vpss_grp, vpss_chn[1], &ext_frm_info);
        sample_svp_check_exps_trace(ret != HI_SUCCESS, SAMPLE_SVP_ERR_LEVEL_DEBUG,
            "Error(%#x),release_frame failed,Grp(%d) chn(%d)!\n", ret, vpss_grp, vpss_chn[1]);
    }
    return HI_NULL;
}

static hi_s32 sample_ive_od_pause(hi_void)
{
    printf("---------------press Enter key to exit!---------------\n");
    if (g_stop_signal == HI_TRUE) {
        if (g_ive_thread != 0) {
            pthread_join(g_ive_thread, HI_NULL);
            g_ive_thread = 0;
        }
        sample_ive_od_uninit(&(g_od_info));
        (hi_void)memset_s(&g_od_info, sizeof(g_od_info), 0, sizeof(g_od_info));
        sample_common_svp_stop_vi_vpss_venc(&g_vi_config, &g_od_switch);
        printf("\033[0;31mprogram termination abnormally!\033[0;39m\n");
        return HI_TRUE;
    }
    (void)getchar();
    if (g_stop_signal == HI_TRUE) {
        if (g_ive_thread != 0) {
            pthread_join(g_ive_thread, HI_NULL);
            g_ive_thread = 0;
        }
        sample_ive_od_uninit(&(g_od_info));
        (hi_void)memset_s(&g_od_info, sizeof(g_od_info), 0, sizeof(g_od_info));
        sample_common_svp_stop_vi_vpss_venc(&g_vi_config, &g_od_switch);
        printf("\033[0;31mprogram termination abnormally!\033[0;39m\n");
        return HI_TRUE;
    }
    return HI_FALSE;
}

hi_void sample_ive_od(hi_void)
{
    hi_s32 ret;
    hi_size pic_size;
    hi_pic_size pic_type = PIC_720P;

    (hi_void)memset_s(&g_od_info, sizeof(g_od_info), 0, sizeof(g_od_info));

    /*
     * step 1: start vi vpss venc
     */
    ret = sample_common_svp_start_vi_vpss_venc(&g_vi_config, &g_od_switch, &pic_type);
    sample_svp_check_exps_goto(ret != HI_SUCCESS, end_od_0, SAMPLE_SVP_ERR_LEVEL_DEBUG,
        "Error(%#x),sample_common_svp_start_vi_vpss_venc_vo failed!\n", ret);

    ret = sample_comm_sys_get_pic_size(pic_type, &pic_size);
    sample_svp_check_exps_goto(ret != HI_SUCCESS, end_od_0, SAMPLE_SVP_ERR_LEVEL_DEBUG,
        "Error(%#x),sample_comm_sys_get_pic_size failed!\n", ret);
    /*
     * step 2: Init OD
     */
    ret = sample_ive_od_init(&g_od_info, pic_size.width, pic_size.height);
    sample_svp_check_exps_goto(ret != HI_SUCCESS, end_od_0, SAMPLE_SVP_ERR_LEVEL_DEBUG,
        "sample_ive_od_init failed, Error(%#x)!\n", ret);
    g_stop_signal = HI_FALSE;

    /*
     * step 3: Create work thread
     */
    ret = prctl(PR_SET_NAME, "ive_od_proc", 0, 0, 0);
    sample_svp_check_exps_goto(ret != HI_SUCCESS, end_od_0, SAMPLE_SVP_ERR_LEVEL_DEBUG, "set thread name failed!\n");
    ret = pthread_create(&g_ive_thread, 0, sample_ive_od_proc, (hi_void *)&g_od_info);
    sample_svp_check_exps_goto(ret != HI_SUCCESS, end_od_0, SAMPLE_SVP_ERR_LEVEL_DEBUG, "pthread_create failed!\n");

    ret = sample_ive_od_pause();
    sample_svp_check_exps_return_void(ret == HI_TRUE, SAMPLE_SVP_ERR_LEVEL_DEBUG, "od exit!\n");

    g_stop_signal = HI_TRUE;
    pthread_join(g_ive_thread, HI_NULL);
    g_ive_thread = 0;
    sample_ive_od_uninit(&(g_od_info));
    (hi_void)memset_s(&g_od_info, sizeof(g_od_info), 0, sizeof(g_od_info));

end_od_0:
    g_ive_thread = 0;
    g_stop_signal = HI_TRUE;
    sample_common_svp_stop_vi_vpss_venc(&g_vi_config, &g_od_switch);
    return;
}

/*
 * function : Od sample signal handle
 */
hi_void sample_ive_od_handle_sig(hi_void)
{
    g_stop_signal = HI_TRUE;
}
