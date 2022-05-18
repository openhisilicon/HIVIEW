/*
 * Copyright (c) Hisilicon Technologies Co., Ltd. 2019-2019. All rights reserved.
 * Description: NNIE MPI header file
 * Author: Hisilicon multimedia software (SVP) group
 * Create: 2019-05-05
 */

#ifndef _HI_MPI_NNIE_H_
#define _HI_MPI_NNIE_H_

#include "hi_common_nnie.h"

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* End of #ifdef __cplusplus */

hi_s32 hi_mpi_svp_nnie_load_model(const hi_svp_src_mem_info *model_buf, hi_svp_nnie_model *model);

hi_s32 hi_mpi_svp_nnie_get_task_buf_size(hi_u32 max_batch_num, hi_u32 max_bbox_num,
    const hi_svp_nnie_model *model, hi_svp_nnie_task_buf_info task_buf_info[], hi_u32 net_seg_num);

hi_s32 hi_mpi_svp_nnie_forward(hi_svp_nnie_handle *handle,
    const hi_svp_src_blob src[], const hi_svp_nnie_model *model, const hi_svp_dst_blob dst[],
    const hi_svp_nnie_forward_ctrl *forward_ctrl, hi_bool is_instant);

hi_s32 hi_mpi_svp_nnie_unload_model(hi_svp_nnie_model *model);

hi_s32 hi_mpi_svp_nnie_set_prior_box(const hi_svp_src_blob prior_box[],
    hi_u32 prior_box_num, hi_svp_nnie_model *model);

hi_s32 hi_mpi_svp_nnie_query(hi_svp_nnie_id nnie_id, hi_svp_nnie_handle handle,
    hi_bool *is_finish, hi_bool is_block);

hi_s32 hi_mpi_svp_nnie_add_mem_info(const hi_svp_mem_info* mem_info);

hi_s32 hi_mpi_svp_nnie_rm_mem_info(const hi_svp_mem_info* mem_info);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif
#endif /* _HI_MPI_NNIE_H_ */
