/*
  Copyright (c), 2001-2022, Shenshu Tech. Co., Ltd.
 */
#ifndef SAMPLE_COMMON_MAU_H
#define SAMPLE_COMMON_MAU_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "hi_type.h"
#include "sample_common_svp.h"
#include "hi_common_mau.h"
#include "hi_mpi_sys.h"
#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */

#define SAMPLE_SVP_MAU_LEFT_MATRIX_WIDTH         5
#define SAMPLE_SVP_MAU_LEFT_MATRIX_HEIGHT        4
#define SAMPLE_SVP_MAU_RIGHT_MATRIX_WIDTH         5
#define SAMPLE_SVP_MAU_RIGHT_MATRIX_HEIGHT        6

#define SAMPLE_SVP_MAU_MATRIX_IDX_WIDTH     2
#define SAMPLE_SVP_MAU_MAX_TOP_N            32
#define SAMPLE_SVP_MAU_ALIGN_16             16
#define SAMPLE_SVP_MAU_QUERY_SLEEP_TIME     100
#define SAMPLE_SVP_MAU_GENERATE_IDX_INTERVAL     2
#define SAMPLE_SVP_MAU_DIST_TOP_N           5

#define SAMPLE_SVP_MAU_MATRIX_MAX_FP32_WIDTH           8192
#define SAMPLE_SVP_MAU_MATRIX_MAX_HEIGHT               100000

/* max and min */
#define sample_svp_mau_max(a, b) (((a) > (b)) ? (a) : (b))
#define sample_svp_mau_min(a, b) (((a) < (b)) ? (a) : (b))
#define sample_svp_mau_check_align(num, align_byte) ((num) & ((align_byte) - 1))

typedef struct {
    hi_bool is_record_mem_info;
    hi_svp_mau_src_double_matrix src;
    hi_svp_mau_src_double_matrix src_idx;
    hi_svp_mau_ctrl ctrl;
    hi_svp_dst_blob chip_dst;
} sample_svp_mau_matrix_mul_info;

typedef struct {
    hi_bool is_record_mem_info;
    hi_svp_mau_src_double_matrix src;
    hi_svp_mau_src_double_matrix src_idx;
    hi_svp_mau_ctrl ctrl;
    hi_svp_mau_dist_result dst;
} sample_svp_mau_dist;

typedef struct {
    hi_u32 left_matrix_width;
    hi_u32 left_matrix_height;
    hi_u32 right_matrix_width;
    hi_u32 right_matrix_height;
    hi_u32 left_idx_width;
    hi_u32 right_idx_width;
    hi_u32 top_n;
} sample_svp_mau_dim;

hi_s32 sample_common_svp_mau_check_mau_mpi_init(hi_void);

hi_s32 sample_common_svp_mau_mpi_exit(hi_void);

hi_s32 sample_svp_mau_set_idx_info(hi_svp_mau_src_double_matrix *src_idx,
    const hi_svp_mau_ctrl *ctrl, const hi_svp_mau_src_double_matrix *src);

hi_s32 sample_svp_mau_generate_matrix_data(const hi_svp_mau_src_double_matrix *src);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */
#endif /* SAMPLE_COMMON_MAU_H */
