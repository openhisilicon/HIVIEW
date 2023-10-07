/*
  Copyright (c), 2001-2022, Shenshu Tech. Co., Ltd.
 */

#ifndef HI_MPI_AE_H
#define HI_MPI_AE_H

#include "hi_common_isp.h"
#include "hi_common_3a.h"
#include "hi_common_ae.h"
#include "hi_common_sns.h"

#ifdef __cplusplus
extern "C" {
#endif

/* The interface of ae lib register to isp. */
hi_s32 hi_mpi_ae_register(hi_vi_pipe vi_pipe, const hi_isp_3a_alg_lib *ae_lib);
hi_s32 hi_mpi_ae_unregister(hi_vi_pipe vi_pipe, const hi_isp_3a_alg_lib *ae_lib);

/* The callback function of sensor register to ae lib. */
hi_s32 hi_mpi_ae_sensor_reg_callback(hi_vi_pipe vi_pipe, hi_isp_3a_alg_lib *ae_lib,
    const hi_isp_sns_attr_info *sns_attr_info, hi_isp_ae_sensor_register *pregister);
hi_s32 hi_mpi_ae_sensor_unreg_callback(hi_vi_pipe vi_pipe, const hi_isp_3a_alg_lib *ae_lib, hi_sensor_id sns_id);

hi_s32 hi_mpi_isp_set_exposure_attr(hi_vi_pipe vi_pipe, const hi_isp_exposure_attr *exp_attr);
hi_s32 hi_mpi_isp_get_exposure_attr(hi_vi_pipe vi_pipe, hi_isp_exposure_attr *exp_attr);

hi_s32 hi_mpi_isp_set_wdr_exposure_attr(hi_vi_pipe vi_pipe, const hi_isp_wdr_exposure_attr *wdr_exp_attr);
hi_s32 hi_mpi_isp_get_wdr_exposure_attr(hi_vi_pipe vi_pipe, hi_isp_wdr_exposure_attr *wdr_exp_attr);

hi_s32 hi_mpi_isp_set_hdr_exposure_attr(hi_vi_pipe vi_pipe, const hi_isp_hdr_exposure_attr *hdr_exp_attr);
hi_s32 hi_mpi_isp_get_hdr_exposure_attr(hi_vi_pipe vi_pipe, hi_isp_hdr_exposure_attr *hdr_exp_attr);

hi_s32 hi_mpi_isp_set_ae_route_attr(hi_vi_pipe vi_pipe, const hi_isp_ae_route *ae_route_attr);
hi_s32 hi_mpi_isp_get_ae_route_attr(hi_vi_pipe vi_pipe, hi_isp_ae_route *ae_route_attr);

hi_s32 hi_mpi_isp_set_ae_route_sf_attr(hi_vi_pipe vi_pipe, const hi_isp_ae_route *ae_route_sf_attr);
hi_s32 hi_mpi_isp_get_ae_route_sf_attr(hi_vi_pipe vi_pipe, hi_isp_ae_route *ae_route_sf_attr);

hi_s32 hi_mpi_isp_query_exposure_info(hi_vi_pipe vi_pipe, hi_isp_exp_info *exp_info);

hi_s32 hi_mpi_isp_set_iris_attr(hi_vi_pipe vi_pipe, const hi_isp_iris_attr *iris_attr);
hi_s32 hi_mpi_isp_get_iris_attr(hi_vi_pipe vi_pipe, hi_isp_iris_attr *iris_attr);

hi_s32 hi_mpi_isp_set_dciris_attr(hi_vi_pipe vi_pipe, const hi_isp_dciris_attr *dciris_attr);
hi_s32 hi_mpi_isp_get_dciris_attr(hi_vi_pipe vi_pipe, hi_isp_dciris_attr *dciris_attr);

hi_s32 hi_mpi_isp_set_piris_attr(hi_vi_pipe vi_pipe, const hi_isp_piris_attr *piris_attr);
hi_s32 hi_mpi_isp_get_piris_attr(hi_vi_pipe vi_pipe, hi_isp_piris_attr *piris_attr);

hi_s32 hi_mpi_isp_set_ae_route_attr_ex(hi_vi_pipe vi_pipe, const hi_isp_ae_route_ex *ae_route_attr_ex);
hi_s32 hi_mpi_isp_get_ae_route_attr_ex(hi_vi_pipe vi_pipe, hi_isp_ae_route_ex *ae_route_attr_ex);

hi_s32 hi_mpi_isp_set_ae_route_sf_attr_ex(hi_vi_pipe vi_pipe, const hi_isp_ae_route_ex *ae_route_sf_attr_ex);
hi_s32 hi_mpi_isp_get_ae_route_sf_attr_ex(hi_vi_pipe vi_pipe, hi_isp_ae_route_ex *ae_route_sf_attr_ex);

hi_s32 hi_mpi_isp_set_smart_exposure_attr(hi_vi_pipe vi_pipe,
                                          const hi_isp_smart_exposure_attr *smart_exp_attr);
hi_s32 hi_mpi_isp_get_smart_exposure_attr(hi_vi_pipe vi_pipe, hi_isp_smart_exposure_attr *smart_exp_attr);
hi_s32 hi_mpi_isp_set_exp_convert(hi_vi_pipe vi_pipe, const hi_isp_exp_conv_param *conv_param);
hi_s32 hi_mpi_isp_get_exp_convert(hi_vi_pipe vi_pipe, hi_isp_exp_conv_param *conv_param);

hi_s32 hi_mpi_isp_set_fast_face_ae_attr(hi_vi_pipe vi_pipe, const hi_isp_fast_face_ae_attr *fast_face_attr);
hi_s32 hi_mpi_isp_get_fast_face_ae_attr(hi_vi_pipe vi_pipe, hi_isp_fast_face_ae_attr *fast_face_attr);

#ifdef __cplusplus
}
#endif
#endif /* HI_MPI_AE_H */
