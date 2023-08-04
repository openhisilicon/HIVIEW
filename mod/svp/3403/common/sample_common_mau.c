/*
  Copyright (c), 2001-2022, Shenshu Tech. Co., Ltd.
 */
#include "sample_common_mau.h"
#include "sample_common_svp.h"
#include "hi_mpi_sys.h"

static hi_bool g_mpi_init = HI_FALSE;

static hi_s32 sample_svp_mau_mpi_init(hi_void)
{
    hi_s32 ret;

    ret = hi_mpi_sys_exit();
    sample_svp_check_exps_return(ret != HI_SUCCESS, ret, SAMPLE_SVP_ERR_LEVEL_ERROR,
        "Error(%#x):hi_mpi_sys_exit failed!\n", ret);
    ret = hi_mpi_sys_init();
    sample_svp_check_exps_return(ret != HI_SUCCESS, ret, SAMPLE_SVP_ERR_LEVEL_ERROR,
        "Error(%#x):hi_mpi_sys_init failed!\n", ret);

    return HI_SUCCESS;
}

hi_s32 sample_common_svp_mau_check_mau_mpi_init(hi_void)
{
    if (g_mpi_init == HI_FALSE) {
        if (sample_svp_mau_mpi_init() != HI_SUCCESS) {
            sample_svp_trace_err("mau mpi init failed!\n");
            return HI_FALSE;
        }
        g_mpi_init = HI_TRUE;
    }
    return HI_TRUE;
}
hi_s32 sample_common_svp_mau_mpi_exit(hi_void)
{
    g_mpi_init = HI_FALSE;
    if (hi_mpi_sys_exit() != HI_SUCCESS) {
        sample_svp_trace_err("Sys exit failed!\n");
        return HI_FAILURE;
    }
    return HI_SUCCESS;
}

static hi_s32 sample_svp_mau_check_src_idx_blob_info(const hi_svp_blob *matrix_blob)
{
    hi_u32 stride;
    hi_u32 byte_num = (hi_u32)sizeof(hi_u32);

    sample_svp_check_exps_return(matrix_blob->virt_addr == 0, HI_ERR_SVP_MAU_ILLEGAL_PARAM,
        SAMPLE_SVP_ERR_LEVEL_ERROR, "Error, blob->virt_addr can't be zero!\n");
    sample_svp_check_exps_return(matrix_blob->phys_addr == 0, HI_ERR_SVP_MAU_ILLEGAL_PARAM,
        SAMPLE_SVP_ERR_LEVEL_ERROR, "Error, blob->phys_addr can't be zero!\n");
    /* check phys_addr 4 byte aligned */
    sample_svp_check_exps_return(sample_svp_mau_check_align(matrix_blob->phys_addr, byte_num) != 0,
        HI_ERR_SVP_MAU_ILLEGAL_PARAM, SAMPLE_SVP_ERR_LEVEL_ERROR,
        "Error(%#x),blob->phys_addr(%llu) should be %u bytes aligned\n",
        HI_ERR_SVP_MAU_ILLEGAL_PARAM, matrix_blob->phys_addr, byte_num);
    /* check type */
    sample_svp_check_exps_return(matrix_blob->type != HI_SVP_BLOB_TYPE_U32, HI_ERR_SVP_MAU_ILLEGAL_PARAM,
        SAMPLE_SVP_ERR_LEVEL_ERROR, "Error(%#x),blob->type(%d) must be %d\n", HI_ERR_SVP_MAU_ILLEGAL_PARAM,
        matrix_blob->type, HI_SVP_BLOB_TYPE_U32);
    /* check num */
    sample_svp_check_exps_return(matrix_blob->num != 1, HI_ERR_SVP_MAU_ILLEGAL_PARAM, SAMPLE_SVP_ERR_LEVEL_ERROR,
        "Error(%#x),blob->num(%u) must be 1\n", HI_ERR_SVP_MAU_ILLEGAL_PARAM, matrix_blob->num);
    /* check chn */
    sample_svp_check_exps_return(matrix_blob->shape.whc.chn != 1, HI_ERR_SVP_MAU_ILLEGAL_PARAM,
        SAMPLE_SVP_ERR_LEVEL_ERROR, "Error(%#x),blob->shape.whc.chn(%u) must be 1\n",
        HI_ERR_SVP_MAU_ILLEGAL_PARAM, matrix_blob->shape.whc.chn);

    sample_svp_check_exps_return((matrix_blob->shape.whc.width == 0) ||
        (matrix_blob->shape.whc.width > SAMPLE_SVP_MAU_MATRIX_MAX_HEIGHT), HI_ERR_SVP_MAU_ILLEGAL_PARAM,
        SAMPLE_SVP_ERR_LEVEL_ERROR, "Error(%#x), blob->shape.whc.width(%u) must be (0, %u]\n",
        HI_ERR_SVP_MAU_ILLEGAL_PARAM, matrix_blob->shape.whc.width, SAMPLE_SVP_MAU_MATRIX_MAX_HEIGHT);

    sample_svp_check_exps_return(matrix_blob->shape.whc.height != 1, HI_ERR_SVP_MAU_ILLEGAL_PARAM,
        SAMPLE_SVP_ERR_LEVEL_ERROR, "Error(%#x), blob->shape.whc.height(%u) must be 1\n",
        HI_ERR_SVP_MAU_ILLEGAL_PARAM, matrix_blob->shape.whc.height);

    /* check stride 16 bytes aligned */
    sample_svp_check_exps_return(sample_svp_mau_check_align(matrix_blob->stride, SAMPLE_SVP_MAU_ALIGN_16) != 0,
        HI_ERR_SVP_MAU_ILLEGAL_PARAM, SAMPLE_SVP_ERR_LEVEL_ERROR,
        "Error(%#x),blob->stride(%u) should be %u bytes aligned\n",
        HI_ERR_SVP_MAU_ILLEGAL_PARAM, matrix_blob->stride, SAMPLE_SVP_MAU_ALIGN_16);
    stride = sample_common_svp_align(matrix_blob->shape.whc.width * byte_num, SAMPLE_SVP_MAU_ALIGN_16);
    sample_svp_check_exps_return((matrix_blob->stride == 0) ||
        (matrix_blob->stride < stride), HI_ERR_SVP_MAU_ILLEGAL_PARAM, SAMPLE_SVP_ERR_LEVEL_ERROR,
        "Error(%#x), blob->stride(%u) can't be 0 and should be equal to or greater than %u\n",
        HI_ERR_SVP_MAU_ILLEGAL_PARAM, matrix_blob->stride, stride);

    return HI_SUCCESS;
}

static hi_s32 sample_svp_mau_set_idx_info_check_param(const hi_svp_mau_src_double_matrix *src_idx,
    const hi_svp_mau_ctrl *ctrl, hi_u32 left_matrix_height, hi_u32 right_matrix_height)
{
    hi_s32 ret;

    sample_svp_check_exps_return(ctrl == HI_NULL, HI_ERR_SVP_MAU_NULL_PTR,
        SAMPLE_SVP_ERR_LEVEL_ERROR, "Error, ctrl is HI_NULL!\n");
    sample_svp_check_exps_return(src_idx == HI_NULL, HI_ERR_SVP_MAU_NULL_PTR,
        SAMPLE_SVP_ERR_LEVEL_ERROR, "Error, src_idx is HI_NULL!\n");

    sample_svp_check_exps_return((ctrl->has_left_idx != HI_FALSE) && (ctrl->has_left_idx != HI_TRUE),
        HI_ERR_SVP_MAU_ILLEGAL_PARAM, SAMPLE_SVP_ERR_LEVEL_ERROR,
        "Error(%#x),ctrl->has_left_idx(%d) must be [%d, %d]\n",
        HI_ERR_SVP_MAU_ILLEGAL_PARAM, ctrl->has_left_idx, HI_FALSE, HI_TRUE);
    sample_svp_check_exps_return((ctrl->has_right_idx != HI_FALSE) && (ctrl->has_right_idx != HI_TRUE),
        HI_ERR_SVP_MAU_ILLEGAL_PARAM, SAMPLE_SVP_ERR_LEVEL_ERROR,
        "Error(%#x),ctrl->has_right_idx(%d) must be [%d, %d]\n",
        HI_ERR_SVP_MAU_ILLEGAL_PARAM, ctrl->has_right_idx, HI_FALSE, HI_TRUE);

    sample_svp_check_exps_return((left_matrix_height == 0) ||
        (left_matrix_height > SAMPLE_SVP_MAU_MATRIX_MAX_HEIGHT), HI_ERR_SVP_MAU_ILLEGAL_PARAM,
        SAMPLE_SVP_ERR_LEVEL_ERROR, "left_matrix_height(%u) must be (0, %u]\n",
        left_matrix_height, SAMPLE_SVP_MAU_MATRIX_MAX_HEIGHT);
    sample_svp_check_exps_return((right_matrix_height == 0) ||
        (right_matrix_height > SAMPLE_SVP_MAU_MATRIX_MAX_HEIGHT), HI_ERR_SVP_MAU_ILLEGAL_PARAM,
        SAMPLE_SVP_ERR_LEVEL_ERROR, "right_matrix_height(%u) must be (0, %u]\n",
        right_matrix_height, SAMPLE_SVP_MAU_MATRIX_MAX_HEIGHT);

    if (ctrl->has_left_idx == HI_TRUE) {
        ret = sample_svp_mau_check_src_idx_blob_info(&src_idx->left_matrix);
        sample_svp_check_exps_return(ret != HI_SUCCESS, ret, SAMPLE_SVP_ERR_LEVEL_ERROR,
            "Error(%#x), sample_svp_mau_check_src_idx_left_matrix fail\n", ret);
    }
    if (ctrl->has_right_idx == HI_TRUE) {
        ret = sample_svp_mau_check_src_idx_blob_info(&src_idx->right_matrix);
        sample_svp_check_exps_return(ret != HI_SUCCESS, ret, SAMPLE_SVP_ERR_LEVEL_ERROR,
            "Error(%#x), sample_svp_mau_check_src_idx_right_matrix fail\n", ret);
    }

    return HI_SUCCESS;
}
hi_s32 sample_svp_mau_set_idx_info(hi_svp_mau_src_double_matrix *src_idx,
    const hi_svp_mau_ctrl *ctrl, const hi_svp_mau_src_double_matrix *src)
{
    hi_u32 i;
    hi_u32 *idx = HI_NULL;
    hi_s32 ret;
    hi_u32 tmp;
    hi_u32 left_matrix_height, right_matrix_height;

    sample_svp_check_exps_return(src == HI_NULL, HI_ERR_SVP_MAU_NULL_PTR,
        SAMPLE_SVP_ERR_LEVEL_ERROR, "Error, src is HI_NULL!\n");

    left_matrix_height = src->left_matrix.shape.whc.height;
    right_matrix_height = src->right_matrix.shape.whc.height;
    ret = sample_svp_mau_set_idx_info_check_param(src_idx, ctrl, left_matrix_height, right_matrix_height);
    sample_svp_check_exps_return(ret != HI_SUCCESS, ret, SAMPLE_SVP_ERR_LEVEL_ERROR,
        "Error(%#x), sample_svp_mau_set_idx_info_check_param fail\n", ret);

    /* set left matrix idx */
    if (ctrl->has_left_idx == HI_TRUE) {
        idx = sample_svp_convert_addr_to_ptr(hi_u32, src_idx->left_matrix.virt_addr);
        for (i = 0; i < src_idx->left_matrix.shape.whc.width; i++) {
            tmp = i * SAMPLE_SVP_MAU_GENERATE_IDX_INTERVAL;
            if (tmp < left_matrix_height) {
                *(idx + i) = tmp;
            } else {
                *(idx + i) = left_matrix_height - 1;
            }
        }
        ret = sample_common_svp_flush_cache(src_idx->left_matrix.phys_addr,
            sample_svp_convert_addr_to_ptr(hi_void, src_idx->left_matrix.virt_addr),
            src_idx->left_matrix.shape.whc.height * src_idx->left_matrix.stride);
        sample_svp_check_exps_return(ret != HI_SUCCESS, ret,
            SAMPLE_SVP_ERR_LEVEL_ERROR, "Error,flush cache failed!\n");
    }

    /* set right matrix idx */
    if (ctrl->has_right_idx == HI_TRUE) {
        idx = sample_svp_convert_addr_to_ptr(hi_u32, src_idx->right_matrix.virt_addr);
        for (i = 0; i < src_idx->right_matrix.shape.whc.width; i++) {
            tmp = i * SAMPLE_SVP_MAU_GENERATE_IDX_INTERVAL;
            if (tmp < right_matrix_height) {
                *(idx + i) = tmp;
            } else {
                *(idx + i) = right_matrix_height - 1;
            }
        }
        ret = sample_common_svp_flush_cache(src_idx->right_matrix.phys_addr,
            sample_svp_convert_addr_to_ptr(hi_void, src_idx->right_matrix.virt_addr),
            src_idx->right_matrix.shape.whc.height * src_idx->right_matrix.stride);
        sample_svp_check_exps_return(ret != HI_SUCCESS, ret,
            SAMPLE_SVP_ERR_LEVEL_ERROR, "Error,flush cache failed!\n");
    }
    return HI_SUCCESS;
}

static hi_s32 sample_svp_mau_check_src_blob_info(const hi_svp_blob *matrix_blob)
{
    hi_u32 stride;
    hi_u32 byte_num = (hi_u32)sizeof(hi_u32);

    sample_svp_check_exps_return(matrix_blob->virt_addr == 0, HI_ERR_SVP_MAU_ILLEGAL_PARAM,
        SAMPLE_SVP_ERR_LEVEL_ERROR, "Error, blob->virt_addr can't be zero!\n");
    sample_svp_check_exps_return(matrix_blob->phys_addr == 0, HI_ERR_SVP_MAU_ILLEGAL_PARAM,
        SAMPLE_SVP_ERR_LEVEL_ERROR, "Error, blob->phys_addr can't be zero!\n");
    /* check phys_addr 4 byte aligned */
    sample_svp_check_exps_return(sample_svp_mau_check_align(matrix_blob->phys_addr, byte_num) != 0,
        HI_ERR_SVP_MAU_ILLEGAL_PARAM, SAMPLE_SVP_ERR_LEVEL_ERROR,
        "Error(%#x),blob->phys_addr(%llu) should be %u bytes aligned\n",
        HI_ERR_SVP_MAU_ILLEGAL_PARAM, matrix_blob->phys_addr, byte_num);
    /* check type */
    sample_svp_check_exps_return(matrix_blob->type != HI_SVP_BLOB_TYPE_FP32, HI_ERR_SVP_MAU_ILLEGAL_PARAM,
        SAMPLE_SVP_ERR_LEVEL_ERROR, "Error(%#x),blob->type(%d) must be %d\n", HI_ERR_SVP_MAU_ILLEGAL_PARAM,
        matrix_blob->type, HI_SVP_BLOB_TYPE_FP32);
    /* check num */
    sample_svp_check_exps_return(matrix_blob->num != 1, HI_ERR_SVP_MAU_ILLEGAL_PARAM, SAMPLE_SVP_ERR_LEVEL_ERROR,
        "Error(%#x),blob->num(%u) must be 1\n", HI_ERR_SVP_MAU_ILLEGAL_PARAM, matrix_blob->num);
    /* check chn */
    sample_svp_check_exps_return(matrix_blob->shape.whc.chn != 1, HI_ERR_SVP_MAU_ILLEGAL_PARAM,
        SAMPLE_SVP_ERR_LEVEL_ERROR, "Error(%#x),blob->shape.whc.chn(%u) must be 1\n",
        HI_ERR_SVP_MAU_ILLEGAL_PARAM, matrix_blob->shape.whc.chn);

    sample_svp_check_exps_return((matrix_blob->shape.whc.width == 0) ||
        (matrix_blob->shape.whc.width > SAMPLE_SVP_MAU_MATRIX_MAX_FP32_WIDTH), HI_ERR_SVP_MAU_ILLEGAL_PARAM,
        SAMPLE_SVP_ERR_LEVEL_ERROR, "Error(%#x),matrix_blob->shape.whc.width(%u) must be (0, %u]\n",
        HI_ERR_SVP_MAU_ILLEGAL_PARAM, matrix_blob->shape.whc.width, SAMPLE_SVP_MAU_MATRIX_MAX_FP32_WIDTH);
    sample_svp_check_exps_return((matrix_blob->shape.whc.height == 0) ||
        (matrix_blob->shape.whc.height > SAMPLE_SVP_MAU_MATRIX_MAX_HEIGHT), HI_ERR_SVP_MAU_ILLEGAL_PARAM,
        SAMPLE_SVP_ERR_LEVEL_ERROR, "Error,matrix_blob->shape.whcheight(%u) must be (0, %u]\n",
        matrix_blob->shape.whc.height, SAMPLE_SVP_MAU_MATRIX_MAX_HEIGHT);

    /* check stride 16 bytes aligned */
    sample_svp_check_exps_return(sample_svp_mau_check_align(matrix_blob->stride, SAMPLE_SVP_MAU_ALIGN_16) != 0,
        HI_ERR_SVP_MAU_ILLEGAL_PARAM, SAMPLE_SVP_ERR_LEVEL_ERROR,
        "Error(%#x),blob->stride(%u) should be %u bytes aligned\n",
        HI_ERR_SVP_MAU_ILLEGAL_PARAM, matrix_blob->stride, SAMPLE_SVP_MAU_ALIGN_16);
    stride = sample_common_svp_align(matrix_blob->shape.whc.width * byte_num, SAMPLE_SVP_MAU_ALIGN_16);
    sample_svp_check_exps_return((matrix_blob->stride == 0) ||
        (matrix_blob->stride < stride), HI_ERR_SVP_MAU_ILLEGAL_PARAM, SAMPLE_SVP_ERR_LEVEL_ERROR,
        "Error(%#x), blob->stride(%u) can't be 0 and should be equal to or greater than %u\n",
        HI_ERR_SVP_MAU_ILLEGAL_PARAM, matrix_blob->stride, stride);

    return HI_SUCCESS;
}

hi_s32 sample_svp_mau_generate_matrix_data(const hi_svp_mau_src_double_matrix *src)
{
    hi_s32 ret;
    hi_u32 i, j;
    hi_float *matrix_data = HI_NULL;
    hi_u32 stride, size;
    hi_float tmp_data;

    /* check */
    sample_svp_check_exps_return(src == HI_NULL, HI_ERR_SVP_MAU_NULL_PTR,
        SAMPLE_SVP_ERR_LEVEL_ERROR, "Error, src is HI_NULL!\n");
    ret = sample_svp_mau_check_src_blob_info(&src->left_matrix);
    sample_svp_check_exps_return(ret != HI_SUCCESS, ret, SAMPLE_SVP_ERR_LEVEL_ERROR,
        "Error(%#x), check param of src->left_matrix fail\n", ret);
    ret = sample_svp_mau_check_src_blob_info(&src->right_matrix);
    sample_svp_check_exps_return(ret != HI_SUCCESS, ret, SAMPLE_SVP_ERR_LEVEL_ERROR,
        "Error(%#x), check param of src->right_matrix fail\n", ret);

    /* left matrix */
    matrix_data = sample_svp_convert_addr_to_ptr(hi_float, src->left_matrix.virt_addr);
    stride = src->left_matrix.stride / sizeof(hi_u32); /* fp32 */
    size = src->left_matrix.stride * src->left_matrix.shape.whc.height;
    for (i = 0; i < src->left_matrix.shape.whc.height; i++) {
        tmp_data = (hi_float)(i + 1);
        for (j = 0; j < src->left_matrix.shape.whc.width; j++) {
            matrix_data[j] = tmp_data;
            tmp_data++;
        }
        matrix_data += stride;
    }
    ret = sample_common_svp_flush_cache(src->left_matrix.phys_addr,
        sample_svp_convert_addr_to_ptr(hi_void, src->left_matrix.virt_addr), size);
    sample_svp_check_exps_return(ret != HI_SUCCESS, ret,
        SAMPLE_SVP_ERR_LEVEL_ERROR, "Error, flush cache failed!\n");

    /* right matrix */
    matrix_data = sample_svp_convert_addr_to_ptr(hi_float, src->right_matrix.virt_addr);
    stride = src->right_matrix.stride / sizeof(hi_u32); /* fp32 */
    size = src->right_matrix.stride * src->right_matrix.shape.whc.height;
    for (i = 0; i < src->right_matrix.shape.whc.height; i++) {
        tmp_data = (hi_float)(i + 1);
        for (j = 0; j < src->right_matrix.shape.whc.width; j++) {
            matrix_data[j] = tmp_data;
            tmp_data++;
        }
        matrix_data += stride;
    }
    ret = sample_common_svp_flush_cache(src->right_matrix.phys_addr,
        sample_svp_convert_addr_to_ptr(hi_void, src->right_matrix.virt_addr), size);
    sample_svp_check_exps_return(ret != HI_SUCCESS, ret,
        SAMPLE_SVP_ERR_LEVEL_ERROR, "Error, flush cache failed!\n");

    return HI_SUCCESS;
}
