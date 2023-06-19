/*
  Copyright (c), 2001-2022, Shenshu Tech. Co., Ltd.
 */

#ifndef __HI_MPI_AWB_H__
#define __HI_MPI_AWB_H__

#include "hi_common_isp.h"
#include "hi_common_3a.h"
#include "hi_common_awb.h"
#include "hi_common_sns.h"


#ifdef __cplusplus
extern "C" {
#endif

/* The interface of awb lib register to isp. */
hi_s32 hi_mpi_awb_unregister(hi_vi_pipe vi_pipe, const hi_isp_3a_alg_lib *awb_lib);
hi_s32 hi_mpi_awb_register(hi_vi_pipe vi_pipe, const hi_isp_3a_alg_lib *awb_lib);

/* The callback function of sensor register to awb lib. */
hi_s32 hi_mpi_awb_sensor_reg_callback(hi_vi_pipe vi_pipe, hi_isp_3a_alg_lib *awb_lib,
                                      hi_isp_sns_attr_info *sns_attr_info,
                                      hi_isp_awb_sensor_register *awb_sns_register);
hi_s32 hi_mpi_awb_sensor_unreg_callback(hi_vi_pipe vi_pipe, const hi_isp_3a_alg_lib *awb_lib, hi_sensor_id sensor_id);

hi_s32 hi_mpi_isp_set_wb_attr(hi_vi_pipe vi_pipe, const hi_isp_wb_attr *wb_attr);
hi_s32 hi_mpi_isp_get_wb_attr(hi_vi_pipe vi_pipe, hi_isp_wb_attr *wb_attr);

hi_s32 hi_mpi_isp_set_awb_attr_ex(hi_vi_pipe vi_pipe, const hi_isp_awb_attr_ex *awb_attr_ex);
hi_s32 hi_mpi_isp_get_awb_attr_ex(hi_vi_pipe vi_pipe, hi_isp_awb_attr_ex *awb_attr_ex);

hi_s32 hi_mpi_isp_set_ccm_attr(hi_vi_pipe vi_pipe, const hi_isp_color_matrix_attr *ccm_attr);
hi_s32 hi_mpi_isp_get_ccm_attr(hi_vi_pipe vi_pipe, hi_isp_color_matrix_attr *ccm_attr);

hi_s32 hi_mpi_isp_set_saturation_attr(hi_vi_pipe vi_pipe, const hi_isp_saturation_attr *sat_attr);
hi_s32 hi_mpi_isp_get_saturation_attr(hi_vi_pipe vi_pipe, hi_isp_saturation_attr *sat_attr);

hi_s32 hi_mpi_isp_query_wb_info(hi_vi_pipe vi_pipe, hi_isp_wb_info *wb_info);
hi_s32 hi_mpi_isp_cal_gain_by_temp(hi_vi_pipe vi_pipe, const hi_isp_wb_attr *wb_attr, hi_u16 color_temp,
                                   hi_s16 shift, hi_u16 *awb_gain);

#ifdef __cplusplus
}
#endif
#endif /* __HI_MPI_AWB_H__ */
