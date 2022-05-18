/*
 * Copyright (c) Hisilicon Technologies Co., Ltd. 2019-2020. All rights reserved.
 * Description: vda mpi head file
 * Author: Hisilicon multimedia software group
 * Create: 2019-05-21
 */

#ifndef __HI_MPI_VDA_H__
#define __HI_MPI_VDA_H__

#include "hi_common_vda.h"

#ifdef __cplusplus
extern "C"{
#endif

hi_s32 hi_mpi_vda_create_chn(hi_vda_chn vda_chn, const hi_vda_chn_attr *attr);
hi_s32 hi_mpi_vda_destroy_chn(hi_vda_chn vda_chn);

hi_s32 hi_mpi_vda_get_chn_attr(hi_vda_chn vda_chn, hi_vda_chn_attr *attr);
hi_s32 hi_mpi_vda_set_chn_attr(hi_vda_chn vda_chn, const hi_vda_chn_attr *attr);

hi_s32 hi_mpi_vda_start_recv_pic(hi_vda_chn vda_chn);
hi_s32 hi_mpi_vda_stop_recv_pic(hi_vda_chn vda_chn);

hi_s32 hi_mpi_vda_get_data(hi_vda_chn vda_chn, hi_vda_data *vda_data, hi_s32 milli_sec);
hi_s32 hi_mpi_vda_release_data(hi_vda_chn vda_chn, const hi_vda_data* vda_data);

hi_s32 hi_mpi_vda_reset_od_rgn(hi_vda_chn vda_chn, hi_s32 rgn_idx);

hi_s32 hi_mpi_vda_query(hi_vda_chn vda_chn, hi_vda_chn_status *chn_status);

hi_s32 hi_mpi_vda_get_fd(hi_vda_chn vda_chn);

hi_s32 hi_mpi_vda_update_ref(hi_vda_chn vda_chn, const hi_video_frame_info *ref_frame);

hi_s32 hi_mpi_vda_send_pic(hi_vda_chn vda_chn, const hi_video_frame_info *user_frame, hi_s32 milli_sec);

#ifdef __cplusplus
}
#endif

#endif

