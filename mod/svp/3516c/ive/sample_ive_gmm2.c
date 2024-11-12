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

#define HI_SAMPLE_IVE_SRC_NUM                           2
#define HI_SAMPLE_IVE_GMM2_QUERY_SLEEP                  100
#define HI_SAMPLE_IVE_GMM2_FIRST_FRMAE_NUM              500
#define HI_SAMPLE_IVE_GMM2_MAX_CCL_REGION               60
#define HI_SAMPLE_IVE_GMM2_UPDATA_FACTOR_VAL1           16
#define HI_SAMPLE_IVE_GMM2_UPDATA_FACTOR_VAL2           200
#define HI_SAMPLE_IVE_GMM2_CHANGE_FACTOR_VAL1           20
#define HI_SAMPLE_IVE_GMM2_CHANGE_FACTOR_VAL2           200
#define HI_SAMPLE_IVE_GMM2_CREATE_FACTOR_VAL1           8
#define HI_SAMPLE_IVE_GMM2_CREATE_FACTOR_VAL2           4
#define HI_SAMPLE_IVE_GMM2_MAX_GRAD_SUM                 10
#define HI_SAMPLE_IVE_GMM2_RATIO_BETWEEN_GRAD_AND_FG    100
#define HI_SAMPLE_IVE_16BIT_TO_8BIT_NUM                 255
#define HI_SAMPLE_IVE_16BIT_TO_8BIT_DEN                 (255 * 4)
#define HI_SAMPLE_IVE_CCL_STEP                          2
#define HI_SAMPLE_IVE_CCL_INIT_AREA_THR                 4
#define HI_SAMPLE_IVE_GMM2_THRESH_LOW_THR               5
#define HI_SAMPLE_IVE_GMM2_NUM_TWO                      2
#define HI_SAMPLE_IVE_GMM2_MODEL_NUM                    3
#define HI_SAMPLE_IVE_GMM2_MAX_VAR                      ((16 * 16)<<7)
#define HI_SAMPLE_IVE_GMM2_MIN_VAR                      ((8 *  8)<<7)
#define HI_SAMPLE_IVE_GMM2_GLB_SNS_FACTOR               8
#define HI_SAMPLE_IVE_GMM2_FREQ_HTR                     12000
#define HI_SAMPLE_IVE_GMM2_FREQ_INIT_VAL                20000
#define HI_SAMPLE_IVE_GMM2_LIFT_THR                     500
#define HI_SAMPLE_IVE_GMM2_NUM_THREE                    3
#define HI_SAMPLE_IVE_GMM2_CHANGE_FACTOR_ABS_THRESHOLD  10
#define HI_SAMPLE_IVE_GMM2_FACTOR_MAX_VAL               8
#define HI_SAMPLE_IVE_GMM2_NUM_TWO                      2
#define HI_SAMPLE_IVE_GMM2_CIF_WIDTH                    352
#define HI_SAMPLE_IVE_GMM2_CIF_HEIGHT                   288
#define HI_SAMPLE_IVE_TOTAL_FRAME                       700

typedef struct {
    hi_svp_src_img src[HI_SAMPLE_IVE_SRC_NUM];
    hi_svp_dst_img fg;
    hi_svp_dst_img bg;
    hi_svp_src_img factor;
    hi_svp_dst_img match_model_info;
    hi_svp_src_img fg_mask;
    hi_svp_src_img last_image;
    hi_svp_dst_img diff_image;
    hi_svp_dst_img mag_image;
    hi_svp_dst_img cur_norm_mag;
    hi_svp_dst_img last_norm_mag;
    hi_svp_dst_mem_info model;
    hi_svp_dst_mem_info blob;
    hi_ive_gmm2_ctrl gmm2_ctrl;

    FILE *fp_src;
    FILE *fp_fg;
    FILE *fp_bg;
} hi_sample_ive_gmm2_info;

static hi_sample_ive_gmm2_info g_gmm2_info;
static hi_bool g_stop_signal = HI_FALSE;

/*
 * gmm2 uninit
 */
static hi_void sample_ive_gmm2_uninit(hi_sample_ive_gmm2_info *gmm2)
{
    hi_u16 i;

    sample_svp_check_exps_return_void(gmm2 == HI_NULL, SAMPLE_SVP_ERR_LEVEL_ERROR, "gmm2 can't be null\n");
    for (i = 0; i < HI_SAMPLE_IVE_SRC_NUM; i++) {
        sample_svp_mmz_free(gmm2->src[i].phys_addr[0], gmm2->src[i].virt_addr[0]);
    }
    sample_svp_mmz_free(gmm2->fg.phys_addr[0], gmm2->fg.virt_addr[0]);
    sample_svp_mmz_free(gmm2->bg.phys_addr[0], gmm2->bg.virt_addr[0]);
    sample_svp_mmz_free(gmm2->factor.phys_addr[0], gmm2->factor.virt_addr[0]);
    sample_svp_mmz_free(gmm2->match_model_info.phys_addr[0], gmm2->match_model_info.virt_addr[0]);
    sample_svp_mmz_free(gmm2->fg_mask.phys_addr[0], gmm2->fg_mask.virt_addr[0]);
    sample_svp_mmz_free(gmm2->diff_image.phys_addr[0], gmm2->diff_image.virt_addr[0]);
    sample_svp_mmz_free(gmm2->mag_image.phys_addr[0], gmm2->mag_image.virt_addr[0]);
    sample_svp_mmz_free(gmm2->cur_norm_mag.phys_addr[0], gmm2->cur_norm_mag.virt_addr[0]);
    sample_svp_mmz_free(gmm2->last_norm_mag.phys_addr[0], gmm2->last_norm_mag.virt_addr[0]);
    sample_svp_mmz_free(gmm2->model.phys_addr, gmm2->model.virt_addr);
    sample_svp_mmz_free(gmm2->blob.phys_addr, gmm2->blob.virt_addr);

    sample_svp_close_file(gmm2->fp_src);
    sample_svp_close_file(gmm2->fp_fg);
    sample_svp_close_file(gmm2->fp_bg);
}

/*
 * function: create gmm2 image memory
 */
static hi_s32 sample_ive_gmm2_create_image(hi_sample_ive_gmm2_info *gmm2, hi_size pic_size)
{
    hi_s32 ret;
    hi_u16 i, j;
    hi_u8 *tmp_ptr = HI_NULL;
    (hi_void)memset_s(gmm2, sizeof(hi_sample_ive_gmm2_info), 0, sizeof(hi_sample_ive_gmm2_info));

    for (i = 0; i < HI_SAMPLE_IVE_SRC_NUM; i++) {
        ret = sample_common_ive_create_image(&(gmm2->src[i]), HI_SVP_IMG_TYPE_U8C1, pic_size.width, pic_size.height);
        sample_svp_check_exps_return(ret != HI_SUCCESS, ret, SAMPLE_SVP_ERR_LEVEL_ERROR,
            "Error(%#x),Create src[%u] image failed!\n", ret, i);
    }

    ret = sample_common_ive_create_image(&(gmm2->fg), HI_SVP_IMG_TYPE_U8C1, pic_size.width, pic_size.height);
    sample_svp_check_exps_return(ret != HI_SUCCESS, ret, SAMPLE_SVP_ERR_LEVEL_ERROR,
        "Error(%#x),Create fg image failed!\n", ret);

    ret = sample_common_ive_create_image(&(gmm2->bg), HI_SVP_IMG_TYPE_U8C1, pic_size.width, pic_size.height);
    sample_svp_check_exps_return(ret != HI_SUCCESS, ret, SAMPLE_SVP_ERR_LEVEL_ERROR,
        "Error(%#x),Create bg image failed!\n", ret);

    ret = sample_common_ive_create_image(&(gmm2->factor), HI_SVP_IMG_TYPE_U16C1, pic_size.width, pic_size.height);
    sample_svp_check_exps_return(ret != HI_SUCCESS, ret, SAMPLE_SVP_ERR_LEVEL_ERROR,
        "Error(%#x),Create factor image failed!\n", ret);

    tmp_ptr = sample_svp_convert_addr_to_ptr(hi_u8, gmm2->factor.virt_addr[0]);
    for (i = 0; i < gmm2->factor.height; i++) {
        for (j = 0; j < gmm2->factor.width; j++) {
            tmp_ptr[HI_SAMPLE_IVE_GMM2_NUM_TWO * j] = HI_SAMPLE_IVE_GMM2_CREATE_FACTOR_VAL1;
            tmp_ptr[HI_SAMPLE_IVE_GMM2_NUM_TWO * j + 1] = HI_SAMPLE_IVE_GMM2_CREATE_FACTOR_VAL2;
        }
        tmp_ptr += gmm2->factor.stride[0] * sizeof(hi_u16);
    }
    ret = sample_common_ive_create_image(&(gmm2->match_model_info), HI_SVP_IMG_TYPE_U8C1,
        pic_size.width, pic_size.height);
    sample_svp_check_exps_return(ret != HI_SUCCESS, ret, SAMPLE_SVP_ERR_LEVEL_ERROR,
        "Error(%#x),Create match_model image failed!\n", ret);

    ret = sample_common_ive_create_image(&(gmm2->fg_mask), HI_SVP_IMG_TYPE_U8C1, pic_size.width, pic_size.height);
    sample_svp_check_exps_return(ret != HI_SUCCESS, ret, SAMPLE_SVP_ERR_LEVEL_ERROR,
        "Error(%#x),Create fg_mask image failed!\n", ret);

    ret = sample_common_ive_create_image(&(gmm2->diff_image), HI_SVP_IMG_TYPE_U8C1, pic_size.width, pic_size.height);
    sample_svp_check_exps_return(ret != HI_SUCCESS, ret, SAMPLE_SVP_ERR_LEVEL_ERROR,
        "Error(%#x),Create diff_image failed!\n", ret);

    ret = sample_common_ive_create_image(&(gmm2->mag_image), HI_SVP_IMG_TYPE_U16C1, pic_size.width, pic_size.height);
    sample_svp_check_exps_return(ret != HI_SUCCESS, ret, SAMPLE_SVP_ERR_LEVEL_ERROR,
        "Error(%#x),Create mag_image failed!\n", ret);

    ret = sample_common_ive_create_image(&(gmm2->cur_norm_mag), HI_SVP_IMG_TYPE_U8C1, pic_size.width, pic_size.height);
    sample_svp_check_exps_return(ret != HI_SUCCESS, ret, SAMPLE_SVP_ERR_LEVEL_ERROR,
        "Error(%#x),Create cur_nor_mag image failed!\n", ret);

    ret = sample_common_ive_create_image(&(gmm2->last_norm_mag), HI_SVP_IMG_TYPE_U8C1, pic_size.width, pic_size.height);
    sample_svp_check_exps_return(ret != HI_SUCCESS, ret, SAMPLE_SVP_ERR_LEVEL_ERROR,
        "Error(%#x),Create last_nor_mag image failed!\n", ret);
    return ret;
}

/*
 * set gmm2 ctrl info
 */
static hi_void sample_ive_gmm2_set_ctrl_info(hi_sample_ive_gmm2_info *gmm2)
{
    gmm2->gmm2_ctrl.var_rate = 1;
    gmm2->gmm2_ctrl.model_num = HI_SAMPLE_IVE_GMM2_MODEL_NUM;
    gmm2->gmm2_ctrl.max_var = HI_SAMPLE_IVE_GMM2_MAX_VAR;
    gmm2->gmm2_ctrl.min_var = HI_SAMPLE_IVE_GMM2_MIN_VAR;
    gmm2->gmm2_ctrl.global_sns_factor = HI_SAMPLE_IVE_GMM2_GLB_SNS_FACTOR;
    gmm2->gmm2_ctrl.sns_factor_mode = HI_IVE_GMM2_SNS_FACTOR_MODE_PIXEL;
    gmm2->gmm2_ctrl.freq_threshold = HI_SAMPLE_IVE_GMM2_FREQ_HTR;
    gmm2->gmm2_ctrl.freq_init_val = HI_SAMPLE_IVE_GMM2_FREQ_INIT_VAL;
    gmm2->gmm2_ctrl.freq_add_factor = 0xEF;
    gmm2->gmm2_ctrl.freq_reduce_factor = 0xFF00;
    gmm2->gmm2_ctrl.life_threshold = HI_SAMPLE_IVE_GMM2_LIFT_THR;
    gmm2->gmm2_ctrl.life_update_factor_mode = HI_IVE_GMM2_LIFE_UPDATE_FACTOR_MODE_GLOBAL;
}
/*
 * function: init gmm2 data
 */
static hi_s32 sample_ive_gmm2_init(hi_sample_ive_gmm2_info *gmm2, hi_size pic_size)
{
    hi_s32 ret = HI_ERR_IVE_NULL_PTR;
    hi_u32 size;
    hi_char path[PATH_MAX + 1] = {0};
    const hi_char *src_file = "./data/input/gmm2/gmm2_352x288_sp400_frm1000.yuv";

    sample_svp_check_exps_return(gmm2 == HI_NULL, ret, SAMPLE_SVP_ERR_LEVEL_ERROR, "gmm2 can't be null\n");
    sample_svp_check_exps_return((strlen(src_file) > PATH_MAX) || (realpath(src_file, path) == HI_NULL),
        HI_ERR_IVE_ILLEGAL_PARAM, SAMPLE_SVP_ERR_LEVEL_ERROR, "invalid file!\n");

    /* create img memory */
    ret = sample_ive_gmm2_create_image(gmm2, pic_size);
    sample_svp_check_exps_goto(ret != HI_SUCCESS, fail, SAMPLE_SVP_ERR_LEVEL_ERROR,
        "sample_ive_gmm2_create_image failed\n");

    /* set gmm2 ctrl info */
    sample_ive_gmm2_set_ctrl_info(gmm2);

    /* create gmm2 model and blob mem_info */
    size = gmm2->gmm2_ctrl.model_num * (hi_u32)sizeof(hi_u64) * gmm2->src[0].width * gmm2->src[0].height;
    ret = sample_common_ive_create_mem_info(&gmm2->model, size);
    sample_svp_check_exps_goto(ret != HI_SUCCESS, fail, SAMPLE_SVP_ERR_LEVEL_ERROR,
        "Error(%#x),Create model mem info failed!\n", ret);

    (hi_void)memset_s(sample_svp_convert_addr_to_ptr(hi_void, gmm2->model.virt_addr),
        gmm2->model.size, 0, gmm2->model.size);
    size = (hi_u32)sizeof(hi_ive_ccblob);
    ret = sample_common_ive_create_mem_info(&gmm2->blob, size);
    sample_svp_check_exps_goto(ret != HI_SUCCESS, fail,  SAMPLE_SVP_ERR_LEVEL_ERROR,
        "Error(%#x),Create blob mem info failed!\n", ret);

    /* open src file */
    ret = HI_FAILURE;
    gmm2->fp_src = fopen(path, "rb");
    sample_svp_check_exps_goto(gmm2->fp_src == HI_NULL, fail, SAMPLE_SVP_ERR_LEVEL_ERROR, "Open file failed!\n");

    /* open fg file */
    sample_svp_check_exps_goto(realpath("./data/output/gmm2", path) == HI_NULL, fail,
        SAMPLE_SVP_ERR_LEVEL_ERROR, "invalid path\n");
    ret = strcat_s(path, PATH_MAX, "/fg_352x288_sp400.yuv");
    sample_svp_check_exps_goto(ret != EOK, fail, SAMPLE_SVP_ERR_LEVEL_ERROR, "strcat_s failed!\n");
    ret = HI_FAILURE;
    gmm2->fp_fg = fopen(path, "wb");
    sample_svp_check_exps_goto(gmm2->fp_fg == HI_NULL, fail, SAMPLE_SVP_ERR_LEVEL_ERROR, "Open file failed!\n");

    /* openc bg file */
    sample_svp_check_exps_goto(realpath("./data/output/gmm2", path) == HI_NULL, fail,
        SAMPLE_SVP_ERR_LEVEL_ERROR, "invalid path\n");
    ret = strcat_s(path, PATH_MAX, "/bg_352x288_sp400.yuv");
    sample_svp_check_exps_goto(ret != EOK, fail, SAMPLE_SVP_ERR_LEVEL_ERROR, "strcat_s failed!\n");
    ret = HI_FAILURE;
    gmm2->fp_bg = fopen(path, "wb");
    sample_svp_check_exps_goto(gmm2->fp_bg == HI_NULL, fail, SAMPLE_SVP_ERR_LEVEL_ERROR,
        "Error,Open file %s failed!\n", path);
    return HI_SUCCESS;
fail:
    sample_ive_gmm2_uninit(gmm2);
    return ret;
}

static hi_s32 sample_ive_gen_fg_mask(hi_svp_src_img *fg, hi_svp_dst_img *mask)
{
    hi_s32 ret;
    hi_ive_handle handle;
    hi_ive_threshold_ctrl ctrl;

    ctrl.mode = HI_IVE_THRESHOLD_MODE_BINARY;
    ctrl.min_val = 0;
    ctrl.max_val = 1;
    ctrl.low_threshold = HI_SAMPLE_IVE_GMM2_THRESH_LOW_THR;
    ret = hi_mpi_ive_threshold(&handle, fg, mask, &ctrl, HI_FALSE);
    sample_svp_check_exps_return(ret != HI_SUCCESS, ret, SAMPLE_SVP_ERR_LEVEL_ERROR,
        "Error(%#x),hi_mpi_ive_thresh failed!\n", ret);

    return ret;
}
/*
 * Reduce the factor gradually to the default value
 */
static hi_void sample_ive_reduce_factor(hi_svp_img *factor)
{
    hi_u16 i, j;
    hi_u8 *tmp_ptr = HI_NULL;

    tmp_ptr = sample_svp_convert_addr_to_ptr(hi_u8, factor->virt_addr[0]);
    for (i = 0; i < factor->height; i++) {
        for (j = 0; j < factor->width; j++) {
            tmp_ptr[j << 1] = hi_sample_ive_max(HI_SAMPLE_IVE_GMM2_FACTOR_MAX_VAL, tmp_ptr[i << 1] - 1);
            tmp_ptr[(i << 1) + 1] = hi_sample_ive_max(HI_SAMPLE_IVE_GMM2_FACTOR_MAX_VAL, tmp_ptr[(i << 1) + 1] -
                HI_SAMPLE_IVE_GMM2_NUM_TWO);
        }
        tmp_ptr += factor->stride[0] * sizeof(hi_u16);
    }
}
/*
 * Change factor by difference frame
 */
static hi_bool sample_ive_change_factor_by_diff_frame(hi_svp_src_img *src_img,
    hi_svp_src_img *last_img, hi_svp_dst_img *diff_img, hi_svp_dst_img *factor)
{
    hi_s32 ret;
    hi_ive_handle handle;
    hi_bool is_finish = HI_FALSE;
    hi_bool is_block = HI_TRUE;
    hi_u16 i, j;
    const hi_s32 abs_thr = HI_SAMPLE_IVE_GMM2_CHANGE_FACTOR_ABS_THRESHOLD;
    hi_u32 point_sum = 0;
    hi_ive_sub_ctrl sub_ctrl;
    hi_u8 *tmp_ptr = HI_NULL;
    hi_u8 *factor_ptr = HI_NULL;

    sub_ctrl.mode = HI_IVE_SUB_MODE_ABS;
    ret = hi_mpi_ive_sub(&handle, src_img, last_img, diff_img, &sub_ctrl, HI_TRUE);
    sample_svp_check_exps_return(ret != HI_SUCCESS, HI_FALSE, SAMPLE_SVP_ERR_LEVEL_ERROR,
        "Error(%#x),hi_mpi_ive_sub failed!\n", ret);

    /* Wait task finish */
    ret = hi_mpi_ive_query(handle, &is_finish, is_block);
    while (ret == HI_ERR_IVE_QUERY_TIMEOUT) {
        usleep(HI_SAMPLE_IVE_GMM2_QUERY_SLEEP);
        ret = hi_mpi_ive_query(handle, &is_finish, is_block);
    }
    sample_svp_check_exps_return(ret != HI_SUCCESS, HI_FALSE, SAMPLE_SVP_ERR_LEVEL_ERROR,
        "Error(%#x),hi_mpi_ive_query failed!\n", ret);

    tmp_ptr = sample_svp_convert_addr_to_ptr(hi_u8, diff_img->virt_addr[0]);
    for (i = 0; i < src_img->height; i++) {
        for (j = 0; j < src_img->width; j++) {
            point_sum += (tmp_ptr[j] > abs_thr);
        }
        tmp_ptr += diff_img->stride[0];
    }
    if ((point_sum * HI_SAMPLE_IVE_GMM2_NUM_THREE) <= (src_img->width * src_img->height * HI_SAMPLE_IVE_GMM2_NUM_TWO)) {
        return HI_FALSE;
    }

    tmp_ptr = sample_svp_convert_addr_to_ptr(hi_u8, diff_img->virt_addr[0]);
    factor_ptr = sample_svp_convert_addr_to_ptr(hi_u8, factor->virt_addr[0]);
    for (i = 0; i < src_img->height; i++) {
        for (j = 0; j < src_img->width; j++) {
            if (tmp_ptr[j] > abs_thr) {
                factor_ptr[j << 1] = HI_SAMPLE_IVE_GMM2_CHANGE_FACTOR_VAL1;
                factor_ptr[(j << 1) + 1] = HI_SAMPLE_IVE_GMM2_CHANGE_FACTOR_VAL2;
            }
        }
        tmp_ptr += diff_img->stride[0];
        factor_ptr += factor->stride[0] * sizeof(hi_u16);
    }
    return HI_TRUE;
}

static hi_s32 sample_ive_set_ctrl_info(hi_ive_ccl_ctrl *ccl_ctrl, hi_ive_mag_and_ang_ctrl *mag_and_ang_ctrl,
    hi_ive_16bit_to_8bit_ctrl *ctrl_16to8bit, hi_ive_sub_ctrl *sub_ctrl)
{
    hi_s32 ret;
    hi_s8 mask[HI_IVE_MASK_NUM] = { 0, 0, 0, 0, 0, 0, -1, 0, 1, 0, 0, -2, 0, 2, 0, 0, -1, 0, 1, 0, 0,  0, 0, 0, 0 };
    /* set ccl ctrl info */
    ccl_ctrl->mode = HI_IVE_CCL_MODE_8C;
    ccl_ctrl->step = HI_SAMPLE_IVE_CCL_STEP;
    ccl_ctrl->init_area_threshold = HI_SAMPLE_IVE_CCL_INIT_AREA_THR;

    /* set mag_and_ang ctrl info */
    ret = memcpy_s(mag_and_ang_ctrl->mask, HI_IVE_MASK_NUM, mask, HI_IVE_MASK_NUM);
    sample_svp_check_exps_return(ret != EOK, HI_ERR_IVE_ILLEGAL_PARAM, SAMPLE_SVP_ERR_LEVEL_ERROR,
        "memcpy_s mask failed!\n");
    mag_and_ang_ctrl->threshold = 0;
    mag_and_ang_ctrl->out_ctrl = HI_IVE_MAG_AND_ANG_OUT_CTRL_MAG;

    /* set 16bit_to_8bit ctrl info */
    ctrl_16to8bit->bias = 0;
    ctrl_16to8bit->mode = HI_IVE_16BIT_TO_8BIT_MODE_U16_TO_U8;
    ctrl_16to8bit->num = HI_SAMPLE_IVE_16BIT_TO_8BIT_NUM;
    ctrl_16to8bit->denominator = HI_SAMPLE_IVE_16BIT_TO_8BIT_DEN;

    /* set sub ctrl info */
    sub_ctrl->mode = HI_IVE_SUB_MODE_ABS;
    return HI_SUCCESS;
}

static hi_s32 sample_ive_calc_grad_diff_bewteen_img(hi_sample_ive_gmm2_info *gmm2, hi_s32 cur_idx)
{
    hi_s32 ret;
    hi_bool is_finish = HI_FALSE;
    hi_bool is_block = HI_TRUE;
    hi_ive_handle handle;

    hi_ive_ccl_ctrl ccl_ctrl;
    hi_ive_ccblob *ccl_blob = HI_NULL;
    hi_ive_mag_and_ang_ctrl mag_and_ang_ctrl;
    hi_ive_16bit_to_8bit_ctrl ctrl_16to8bit;
    hi_ive_sub_ctrl sub_ctrl;

    ret = sample_ive_set_ctrl_info(&ccl_ctrl, &mag_and_ang_ctrl, &ctrl_16to8bit, &sub_ctrl);
    sample_svp_check_exps_return(ret != HI_SUCCESS, ret, SAMPLE_SVP_ERR_LEVEL_ERROR, "ive_set_ctrl_info failed!\n");

    ret = hi_mpi_ive_ccl(&handle, &gmm2->fg_mask, &gmm2->blob, &ccl_ctrl, HI_TRUE);
    sample_svp_check_failed_err_level_return(ret, ret, "Error(%#x),hi_mpi_ive_ccl failed!\n", ret);

    /* Wait task finish */
    ret = hi_mpi_ive_query(handle, &is_finish, is_block);
    while (ret == HI_ERR_IVE_QUERY_TIMEOUT) {
        usleep(HI_SAMPLE_IVE_GMM2_QUERY_SLEEP);
        ret = hi_mpi_ive_query(handle, &is_finish, is_block);
    }
    sample_svp_check_failed_err_level_return(ret, ret, "Error(%#x),hi_mpi_ive_query failed!\n", ret);

    ccl_blob = sample_svp_convert_addr_to_ptr(hi_ive_ccblob, gmm2->blob.virt_addr);
    if (ccl_blob->info.bits.rgn_num > 0) {
        /* Calc the gradient difference of the current image and the last image */
        ret = hi_mpi_ive_mag_and_ang(&handle, &gmm2->src[cur_idx], &gmm2->mag_image,
            HI_NULL, &mag_and_ang_ctrl, HI_FALSE);
        sample_svp_check_failed_err_level_return(ret, ret, "Error(%#x),hi_mpi_ive_mag_and_ang failed!\n", ret);

        ret = hi_mpi_ive_16bit_to_8bit(&handle, &gmm2->mag_image, &gmm2->cur_norm_mag, &ctrl_16to8bit, HI_FALSE);
        sample_svp_check_failed_err_level_return(ret, ret, "Error(%#x),hi_mpi_ive_16bit_to_8bit failed!\n", ret);

        ret = hi_mpi_ive_mag_and_ang(&handle, &gmm2->src[1 - cur_idx], &gmm2->mag_image, HI_NULL,
            &mag_and_ang_ctrl, HI_FALSE);
        sample_svp_check_failed_err_level_return(ret, ret, "Error(%#x),hi_mpi_ive_mag_and_ang failed!\n", ret);

        ret = hi_mpi_ive_16bit_to_8bit(&handle, &gmm2->mag_image, &gmm2->last_norm_mag, &ctrl_16to8bit, HI_FALSE);
        sample_svp_check_failed_err_level_return(ret, ret, "Error(%#x),hi_mpi_ive_16bit_to_8bit failed!\n", ret);

        ret = hi_mpi_ive_sub(&handle, &gmm2->cur_norm_mag, &gmm2->last_norm_mag, &gmm2->diff_image, &sub_ctrl, HI_TRUE);
        sample_svp_check_failed_err_level_return(ret, ret, "Error(%#x),hi_mpi_ive_sub failed!\n", ret);

        ret = hi_mpi_ive_query(handle, &is_finish, is_block);
        while (ret == HI_ERR_IVE_QUERY_TIMEOUT) {
            usleep(HI_SAMPLE_IVE_GMM2_QUERY_SLEEP);
            ret = hi_mpi_ive_query(handle, &is_finish, is_block);
        }
        sample_svp_check_failed_err_level_return(ret, ret, "Error(%#x),hi_mpi_ive_query failed!\n", ret);
    }
    return HI_SUCCESS;
}

static hi_void sample_ive_get_grad_and_fg_sum(hi_svp_src_img *fg_mask, hi_svp_src_img *diff_img,
    hi_svp_rect_u16 rect, hi_s32 *fg_sum, hi_s32 *grad_sum)
{
    hi_u8 *fg_cur_ptr = HI_NULL;
    hi_u8 *grad_diff_ptr = HI_NULL;
    hi_u16 top, left, right, bottom;
    hi_u16 i, j;
    top = rect.x;
    left = rect.y;
    right = rect.width;
    bottom = rect.height;
    *fg_sum = 0;
    *grad_sum = 0;

    fg_cur_ptr = (sample_svp_convert_addr_to_ptr(hi_u8, fg_mask->virt_addr[0])) + top * fg_mask->stride[0];
    grad_diff_ptr = (sample_svp_convert_addr_to_ptr(hi_u8, diff_img->virt_addr[0])) + top * diff_img->stride[0];
    for (i = top; i < bottom; i++) {
        for (j = left; j < right; j++) {
            if (fg_cur_ptr[j] != 0) {
                (*fg_sum)++;
                *grad_sum = grad_diff_ptr[j] ? (*grad_sum) + 1 : (*grad_sum) + 0;
            }
        }

        fg_cur_ptr += fg_mask->stride[0];
        grad_diff_ptr += diff_img->stride[0];
    }
}

static hi_void sample_ive_update_mask_and_factor(hi_svp_rect_u16 rect, hi_s32 fg_sum, hi_s32 grad_sum,
    hi_svp_src_img *factor, hi_svp_src_img *mask)
{
    hi_u8 *fg_cur_ptr = HI_NULL;
    hi_u8 *factor_ptr = HI_NULL;
    hi_u16 top, left, right, bottom;
    hi_u16 i, j;
    top = rect.x;
    left = rect.y;
    right = rect.width;
    bottom = rect.height;

    if ((grad_sum >= HI_SAMPLE_IVE_GMM2_MAX_GRAD_SUM) &&
        (grad_sum * HI_SAMPLE_IVE_GMM2_RATIO_BETWEEN_GRAD_AND_FG >= fg_sum)) {
            return;
    }

    factor_ptr = (sample_svp_convert_addr_to_ptr(hi_u8, factor->virt_addr[0])) +
        sizeof(hi_u16) * top * factor->stride[0];
    fg_cur_ptr = (sample_svp_convert_addr_to_ptr(hi_u8, mask->virt_addr[0])) +
        top * mask->stride[0];
    for (i = top; i < bottom; i++) {
        for (j = left; j < right; j++) {
            if (fg_cur_ptr[j] != 0) {
                factor_ptr[j << 1] = HI_SAMPLE_IVE_GMM2_UPDATA_FACTOR_VAL1;
                factor_ptr[(j << 1) + 1] = HI_SAMPLE_IVE_GMM2_UPDATA_FACTOR_VAL2;
            }
        }
        fg_cur_ptr += mask->stride[0];
        factor_ptr += factor->stride[0] * sizeof(hi_u16);
    }
}
/*
 * Change factor by gradient
 */
static hi_void sample_ive_change_factror_by_grad(hi_sample_ive_gmm2_info *gmm2, hi_s32 cur_idx)
{
    hi_s32 ret;
    hi_u16 k;
    hi_s32 fg_sum, grad_sum;
    hi_ive_ccblob *ccl_blob = HI_NULL;
    hi_svp_rect_u16 rect;
    ret = sample_ive_calc_grad_diff_bewteen_img(gmm2, cur_idx);
    sample_svp_check_exps_return_void(ret != HI_SUCCESS, SAMPLE_SVP_ERR_LEVEL_ERROR,
        "sample_ive_calc_grad_diff_bewteen_cur_and_last_img failed\n");

    ccl_blob = sample_svp_convert_addr_to_ptr(hi_ive_ccblob, gmm2->blob.virt_addr);

    /* for each blob, analyze the gradient change */
    for (k = 0; k < HI_IVE_MAX_RGN_NUM; k++) {
        if (ccl_blob->rgn[k].area == 0) {
            continue;
        }
        rect.x = ccl_blob->rgn[k].top;
        rect.y = ccl_blob->rgn[k].left;
        rect.width = ccl_blob->rgn[k].right;
        rect.height = ccl_blob->rgn[k].bottom;
        if (((ccl_blob->rgn[k].right - ccl_blob->rgn[k].left) *
            (ccl_blob->rgn[k].bottom - ccl_blob->rgn[k].top)) < HI_SAMPLE_IVE_GMM2_MAX_CCL_REGION) {
            continue;
        }
        sample_ive_get_grad_and_fg_sum(&gmm2->fg_mask, &gmm2->diff_image, rect, &fg_sum, &grad_sum);

        sample_ive_update_mask_and_factor(rect, fg_sum, grad_sum, &gmm2->factor, &gmm2->fg_mask);
    }
}

/*
 * Adjustment factor
 */
static hi_void sample_ive_adjust_factor(hi_sample_ive_gmm2_info *gmm2, hi_s32 cur_idx)
{
    hi_bool is_changed;
    /* First, reduce the factor gradually to the default value */
    sample_ive_reduce_factor(&gmm2->factor);

    /*
     * Second, analyze the frame difference
     * When the number of changed pixels is more than the threshold, there maybe a light switch.
     * And then, we should set a big factor to adapt to it quickly.
     */
    is_changed = sample_ive_change_factor_by_diff_frame(&gmm2->src[cur_idx], &gmm2->src[1 - cur_idx],
        &gmm2->diff_image, &gmm2->factor);
    if (is_changed == HI_TRUE) {
        return;
    }
    /*
     * Third, analyze the gradient for foreground blobs
     * When gradient change of a foreground blob is very small, it maybe a local illumination change,
     * a ghost, or a static object.
     * Here we try to reduce the influence by a local illumination change or a ghost only.
     */
    (hi_void)sample_ive_change_factror_by_grad(gmm2, cur_idx);
}

static hi_s32 sample_ive_gmm2_write_fg_bg_file(hi_sample_ive_gmm2_info *gmm2)
{
    hi_s32 ret;
    ret = sample_common_ive_write_file(&(gmm2->fg), gmm2->fp_fg);
    sample_svp_check_exps_return(ret != HI_SUCCESS, ret, SAMPLE_SVP_ERR_LEVEL_ERROR,
        "Error(%#x),Write fg file failed!\n", ret);

    ret = sample_common_ive_write_file(&(gmm2->bg), gmm2->fp_bg);
    sample_svp_check_exps_return(ret != HI_SUCCESS, ret, SAMPLE_SVP_ERR_LEVEL_ERROR,
        "Error(%#x),Write bg file failed!\n", ret);

    return ret;
}

static hi_s32 sample_ive_gmm2_proc(hi_sample_ive_gmm2_info *gmm2)
{
    hi_s32 ret = HI_ERR_IVE_NULL_PTR;
    hi_ive_handle handle;
    hi_bool is_finish = HI_FALSE;
    hi_bool is_block = HI_TRUE;
    hi_bool is_instant = HI_TRUE;
    hi_u32 frame_num;
    hi_s32 cur_idx = 0;
    const hi_u32 total_frame = HI_SAMPLE_IVE_TOTAL_FRAME;

    sample_svp_check_exps_return(gmm2 == HI_NULL, ret, SAMPLE_SVP_ERR_LEVEL_ERROR, "gmm2 can't be null\n");

    for (frame_num = 1; (frame_num < total_frame) && (g_stop_signal == HI_FALSE); frame_num++) {
        sample_svp_trace_info("Proc Frame %u/%u\n", frame_num, total_frame);
        ret = sample_common_ive_read_file(&(gmm2->src[cur_idx]), gmm2->fp_src);
        sample_svp_check_exps_return(ret != HI_SUCCESS, ret, SAMPLE_SVP_ERR_LEVEL_ERROR,
            "Error(%#x),Read src file failed!\n", ret);

        /* To building a stable background model quickly at the begin, some parameters are set specially. */
        if (gmm2->gmm2_ctrl.model_num == 1) {
            /*
             * If the parameter u8ModelNum is set to 1, the parameter u16FreqReduFactor
             * is usually set to a small value at the first N frames. Here, N = 500.
             */
            gmm2->gmm2_ctrl.freq_reduce_factor = (frame_num >= HI_SAMPLE_IVE_GMM2_FIRST_FRMAE_NUM) ? 0xFFA0 : 0xFC00;
        } else {
            /*
             * If the parameter u8ModelNum is more than 1, the global life mode should be used at the first N frames,
             * and the parameter u16GlbLifeUpdateFactor is usually set to a big value. Here, N = 500.
             */
            if (frame_num >= HI_SAMPLE_IVE_GMM2_FIRST_FRMAE_NUM) {
                gmm2->gmm2_ctrl.life_update_factor_mode = HI_IVE_GMM2_LIFE_UPDATE_FACTOR_MODE_PIXEL;
            } else {
                gmm2->gmm2_ctrl.global_life_update_factor = 0xFFFF / frame_num;
            }
        }

        ret = hi_mpi_ive_gmm2(&handle, &gmm2->src[cur_idx], &gmm2->factor, &gmm2->fg, &gmm2->bg,
            &gmm2->match_model_info, &gmm2->model, &gmm2->gmm2_ctrl, is_instant);
        sample_svp_check_exps_return(ret != HI_SUCCESS, ret, SAMPLE_SVP_ERR_LEVEL_ERROR,
            "Error(%#x),hi_mpi_ive_gmm2 failed!\n", ret);

        /* factor adjustment */
        if (frame_num > 1) {
            ret = sample_ive_gen_fg_mask(&(gmm2->fg), &(gmm2->fg_mask));
            sample_svp_check_exps_return(ret != HI_SUCCESS, ret, SAMPLE_SVP_ERR_LEVEL_ERROR,
                "Error(%#x),sample_ive_gen_fg_mask failed!\n", ret);

            sample_ive_adjust_factor(gmm2, cur_idx);
        } else {
            ret = hi_mpi_ive_query(handle, &is_finish, is_block);
            while (ret == HI_ERR_IVE_QUERY_TIMEOUT) {
                usleep(HI_SAMPLE_IVE_GMM2_QUERY_SLEEP);
                ret = hi_mpi_ive_query(handle, &is_finish, is_block);
            }
            sample_svp_check_exps_return(ret != HI_SUCCESS, ret, SAMPLE_SVP_ERR_LEVEL_ERROR,
                "Error(%#x),hi_mpi_ive_query failed!\n", ret);
        }
        ret = sample_ive_gmm2_write_fg_bg_file(gmm2);
        sample_svp_check_exps_return(ret != HI_SUCCESS, ret, SAMPLE_SVP_ERR_LEVEL_ERROR,
            "Error(%#x),Write fg bg file failed!\n", ret);

        cur_idx = 1 - cur_idx;
    }

    return HI_SUCCESS;
}

static hi_void sample_ive_gmm2_stop(hi_void)
{
    sample_ive_gmm2_uninit(&g_gmm2_info);
    (hi_void)memset_s(&g_gmm2_info, sizeof(g_gmm2_info), 0, sizeof(g_gmm2_info));
    sample_common_ive_mpi_exit();
    printf("\033[0;31mprogram termination abnormally!\033[0;39m\n");
}

/*
 * function: gmm2 func
 */
hi_void sample_ive_gmm2(hi_void)
{
    hi_s32 ret;
    hi_size pic_size = { HI_SAMPLE_IVE_GMM2_CIF_WIDTH, HI_SAMPLE_IVE_GMM2_CIF_HEIGHT };

    (hi_void)memset_s(&g_gmm2_info, sizeof(g_gmm2_info), 0, sizeof(g_gmm2_info));
    ret = sample_common_ive_check_mpi_init();
    sample_svp_check_exps_return_void(ret != HI_TRUE, SAMPLE_SVP_ERR_LEVEL_ERROR, "ive_check_mpi_init failed!\n");

    ret = sample_ive_gmm2_init(&g_gmm2_info, pic_size);
    sample_svp_check_exps_goto(ret != HI_SUCCESS, gmm2_fail, SAMPLE_SVP_ERR_LEVEL_ERROR,
        "Error(%#x),sample_ive_gmm2_init failed!\n", ret);

    ret = sample_ive_gmm2_proc(&g_gmm2_info);
    if (g_stop_signal == HI_TRUE) {
        sample_ive_gmm2_stop();
        return;
    }

    if (ret == HI_SUCCESS) {
        sample_svp_trace_info("Process success!\n");
    } else {
        sample_svp_trace_err("gmm2 process failed!\n");
    }

    g_stop_signal = HI_TRUE;
    sample_ive_gmm2_uninit(&g_gmm2_info);
    (hi_void)memset_s(&g_gmm2_info, sizeof(g_gmm2_info), 0, sizeof(g_gmm2_info));

gmm2_fail:
    g_stop_signal = HI_TRUE;
    sample_common_ive_mpi_exit();
}

/*
 * function : Gmm2 sample signal handle
 */
hi_void sample_ive_gmm2_handle_sig(hi_void)
{
    g_stop_signal = HI_TRUE;
}
