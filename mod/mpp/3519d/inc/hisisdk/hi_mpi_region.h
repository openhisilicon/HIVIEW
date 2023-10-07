/*
  Copyright (c), 2001-2022, Shenshu Tech. Co., Ltd.
 */

#ifndef HI_MPI_REGION_H
#define HI_MPI_REGION_H

#include "hi_debug.h"
#include "hi_common_region.h"

#ifdef __cplusplus
extern "C" {
#endif

hi_s32 hi_mpi_rgn_create(hi_rgn_handle handle, const hi_rgn_attr *rgn_attr);
hi_s32 hi_mpi_rgn_destroy(hi_rgn_handle handle);

hi_s32 hi_mpi_rgn_get_attr(hi_rgn_handle handle, hi_rgn_attr *rgn_attr);
hi_s32 hi_mpi_rgn_set_attr(hi_rgn_handle handle, const hi_rgn_attr *rgn_attr);

hi_s32 hi_mpi_rgn_set_bmp(hi_rgn_handle handle, const hi_bmp *bmp);

hi_s32 hi_mpi_rgn_attach_to_chn(hi_rgn_handle handle, const hi_mpp_chn *chn, const hi_rgn_chn_attr *chn_attr);
hi_s32 hi_mpi_rgn_detach_from_chn(hi_rgn_handle handle, const hi_mpp_chn *chn);

hi_s32 hi_mpi_rgn_attach_to_dev(hi_rgn_handle handle, const hi_mpp_chn *chn, const hi_rgn_chn_attr *chn_attr);
hi_s32 hi_mpi_rgn_detach_from_dev(hi_rgn_handle handle, const hi_mpp_chn *chn);

hi_s32 hi_mpi_rgn_set_chn_display_attr(hi_rgn_handle handle, const hi_mpp_chn *chn, const hi_rgn_chn_attr *chn_attr);
hi_s32 hi_mpi_rgn_get_chn_display_attr(hi_rgn_handle handle, const hi_mpp_chn *chn, hi_rgn_chn_attr *chn_attr);

hi_s32 hi_mpi_rgn_set_dev_display_attr(hi_rgn_handle handle, const hi_mpp_chn *chn, const hi_rgn_chn_attr *chn_attr);
hi_s32 hi_mpi_rgn_get_dev_display_attr(hi_rgn_handle handle, const hi_mpp_chn *chn, hi_rgn_chn_attr *chn_attr);

hi_s32 hi_mpi_rgn_get_canvas_info(hi_rgn_handle handle, hi_rgn_canvas_info *canvas_info);
hi_s32 hi_mpi_rgn_update_canvas(hi_rgn_handle handle);

hi_s32 hi_mpi_rgn_batch_begin(hi_rgn_handle_grp *grp, hi_u32 handle_num, const hi_rgn_handle handle[]);
hi_s32 hi_mpi_rgn_batch_end(hi_rgn_handle_grp grp);

#ifdef __cplusplus
}
#endif
#endif /* HI_MPI_REGION_H */
