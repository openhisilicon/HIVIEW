/*
  Copyright (c), 2001-2022, Shenshu Tech. Co., Ltd.
 */

#ifndef __HI_MPI_MAU_H__
#define __HI_MPI_MAU_H__

#include "hi_common_mau.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
 *   Prototype    : hi_mpi_svp_mau_matrix_mul
 *   Description  : matrix product operator
 *   Parameters   : hi_svp_mau_handle            *handle        Returned handle ID of a task
 *                  const hi_svp_mau_src_double_matrix  *src           Input left and right matrix
 *                  const hi_svp_mau_src_double_matrix  *src_idx       Input left or right matrix's row vector idx
 *                  const hi_svp_mau_ctrl        *ctrl          Ctrl prameters
 *                  const hi_svp_mau_op_result   *dst           Output matrix product operator result
 *   Return Value : HI_SUCCESS: Success; Error codes: Failure.
 *   History:
 *       1.  Date         : 2019-05-28
 *          Modification  : Created function
 */
hi_s32 hi_mpi_svp_mau_matrix_mul(hi_svp_mau_handle *handle, const hi_svp_mau_src_double_matrix *src,
    const hi_svp_mau_src_double_matrix *src_idx, const hi_svp_mau_ctrl *ctrl, const hi_svp_dst_blob *dst);

/*
 *   Prototype    : hi_mpi_svp_mau_cos_dist
 *   Description  : cosine distance operator
 *   Parameters   : hi_svp_mau_handle            *handle        Returned handle ID of a task
 *                  const hi_svp_mau_src_double_matrix  *src           Input left and right matrix,
 *                                                                the data in each row vector must be normalized;
 *                  const hi_svp_mau_src_double_matrix  *src_idx       Input left or right matrix's row vector idx
 *                  const hi_svp_mau_ctrl        *ctrl          Ctrl prameters
 *                  const hi_svp_mau_dist_result *dst           Output cosine distance operator result
 *   Return Value : HI_SUCCESS: Success; Error codes: Failure.
 *   History:
 *       1.  Date         : 2019-05-28
 *          Modification  : Created function
 */
hi_s32 hi_mpi_svp_mau_cos_dist(hi_svp_mau_handle *handle, const hi_svp_mau_src_double_matrix *src,
    const hi_svp_mau_src_double_matrix *src_idx, const hi_svp_mau_ctrl *ctrl, const hi_svp_mau_dist_result *dst);

/*
 *   Prototype    : hi_mpi_svp_mau_euclid_dist
 *   Description  : euclidean distance operator
 *   Parameters   : hi_svp_mau_handle            *handle        Returned handle ID of a task
 *                  const hi_svp_mau_src_double_matrix  *src           Input left and right matrix
 *                  const hi_svp_mau_src_double_matrix  *src_idx       Input left or right matrix's row vector idx
 *                  const hi_svp_mau_ctrl        *ctrl          Ctrl prameters
 *                  const hi_svp_mau_op_result   *dst           Output euclidean distance operator result
 *   Return Value : HI_SUCCESS: Success; Error codes: Failure.
 *   History:
 *       1.  Date         : 2019-05-28
 *          Modification  : Created function
 */
hi_s32 hi_mpi_svp_mau_euclid_dist(hi_svp_mau_handle *handle, const hi_svp_mau_src_double_matrix *src,
    const hi_svp_mau_src_double_matrix *src_idx, const hi_svp_mau_ctrl *ctrl, const hi_svp_mau_dist_result *dst);

/*
 *   Prototype    : hi_mpi_svp_mau_query
 *   Description  : This API is used to query the status of a function run on mau.
 *                In block mode, the system waits until the function that is being queried is called.
 *                In non-block mode, the current status is queried and no action is taken.
 *   Parameters   : hi_svp_mau_id        mau_id       MAU core id
 *                  hi_svp_mau_handle    handle       Handle of a called function. It is entered by users.
 *                  hi_bool              *is_finish      Returned status
 *                  hi_bool               is_block       Flag indicating the block mode or non-block mode
 *   Return Value : HI_SUCCESS: Success;Error codes: Failure.
 *   History:
 *       1.  Date         : 2019-05-28
 *          Modification  : Created function
 */
hi_s32 hi_mpi_svp_mau_query(hi_svp_mau_id mau_id, hi_svp_mau_handle handle, hi_bool *is_finish, hi_bool is_block);

/*
 *   Prototype    : hi_mpi_svp_mau_add_mem_info
 *   Description  : This API is used to record mem info in mem info array.
 *                  If the mem info has been recorded, when user send the mem the data in which
 *                   will be checked in kernel    (e.g. the mem which stores row vector idx),
 *                   the phyaddr of this mem will not be mapped again,
 *                  this will improve the execution efficiency
 *   Parameters   :  const hi_svp_mau_mem_info *mem_info mem info
 *   Return Value : HI_SUCCESS: Success;Error codes: Failure.
 *   History:
 *       1.  Date         : 2019-05-28
 *          Modification  : Created function
 */
hi_s32 hi_mpi_svp_mau_add_mem_info(const hi_svp_mem_info *mem_info);

/*
 *   Prototype    : hi_mpi_svp_mau_rm_mem_info
 *   Description  : This API is used to remove mem info from mem info array and umap viraddr.
 *                  If user no longer uses the recorded mem info, the recorded mem info must be removed
 *   Parameters   : const hi_svp_mau_mem_info *mem_info mem info
 *   Return Value : HI_SUCCESS: Success;Error codes: Failure.
 *   History:
 *       1.  Date         : 2019-05-28
 *          Modification  : Created function
 */
hi_s32 hi_mpi_svp_mau_rm_mem_info(const hi_svp_mem_info *mem_info);

#ifdef __cplusplus
}
#endif
#endif /* __HI_MPI_MAU_H__ */
