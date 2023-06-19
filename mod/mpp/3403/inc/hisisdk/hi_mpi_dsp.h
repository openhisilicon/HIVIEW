/*
  Copyright (c), 2001-2022, Shenshu Tech. Co., Ltd.
 */

#ifndef __HI_MPI_DSP_H__
#define __HI_MPI_DSP_H__

#include "hi_debug.h"
#include "hi_common_dsp.h"
#include "hi_common.h"


#ifdef __cplusplus
extern "C" {
#endif

hi_s32 hi_mpi_svp_dsp_load_bin(const hi_char *bin_file_name, hi_svp_dsp_mem_type mem_type);

hi_s32 hi_mpi_svp_dsp_enable_core(hi_svp_dsp_id dsp_id);

hi_s32 hi_mpi_svp_dsp_disable_core(hi_svp_dsp_id dsp_id);

hi_s32 hi_mpi_svp_dsp_rpc(hi_svp_dsp_handle *handle, const hi_svp_dsp_msg *msg,
    hi_svp_dsp_id dsp_id, hi_svp_dsp_pri pri);

hi_s32 hi_mpi_svp_dsp_query(hi_svp_dsp_id dsp_id, hi_svp_dsp_handle handle,
    hi_bool *is_finish, hi_bool is_block);

hi_s32 hi_mpi_svp_dsp_power_on(hi_svp_dsp_id dsp_id);

hi_s32 hi_mpi_svp_dsp_power_off(hi_svp_dsp_id dsp_id);

#ifdef __cplusplus
}
#endif
#endif /* __HI_MPI_DSP_H__ */
