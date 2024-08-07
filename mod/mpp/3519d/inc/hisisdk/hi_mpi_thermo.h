/*
  Copyright (c), 2001-2024, Shenshu Tech. Co., Ltd.
 */

#ifndef HI_MPI_THERMO_H
#define HI_MPI_THERMO_H

#include "hi_common_isp.h"
#include "hi_common_3a.h"
#include "hi_common_ae.h"
#include "hi_common_sns.h"

#ifdef __cplusplus
extern "C" {
#endif

/* The interface of ae lib register to isp. */
hi_s32 hi_mpi_thermo_register(hi_vi_pipe vi_pipe, const hi_isp_3a_alg_lib *thermo_lib);
hi_s32 hi_mpi_thermo_unregister(hi_vi_pipe vi_pipe, const hi_isp_3a_alg_lib *thermo_lib);
/* The callback function of sensor register to ae lib. */
hi_s32 hi_mpi_thermo_sensor_reg_callback(hi_vi_pipe vi_pipe, hi_isp_3a_alg_lib *thermo_lib,
    const hi_isp_sns_attr_info *sns_attr_info, hi_isp_ae_sensor_register *pregister);
hi_s32 hi_mpi_thermo_sensor_unreg_callback(hi_vi_pipe vi_pipe, const hi_isp_3a_alg_lib *thermo_lib,
    hi_sensor_id sns_id);
hi_s32 hi_mpi_isp_set_thermo_attr(hi_vi_pipe vi_pipe, const hi_isp_thermo_attr *thermo_attr);
hi_s32 hi_mpi_isp_get_thermo_attr(hi_vi_pipe vi_pipe, hi_isp_thermo_attr *thermo_attr);
hi_s32 hi_mpi_thermo_query_exposure_info(hi_vi_pipe vi_pipe, hi_isp_thermo_info *thermo_info);
hi_s32 hi_mpi_isp_set_thermo_curve_attr(hi_vi_pipe vi_pipe, hi_isp_thermo_curve_attr *curve_attr);
hi_s32 hi_mpi_isp_get_thermo_curve_attr(hi_vi_pipe vi_pipe, hi_isp_thermo_curve_attr *curve_attr);
#ifdef __cplusplus
}
#endif
#endif /* HI_MPI_AE_H */
