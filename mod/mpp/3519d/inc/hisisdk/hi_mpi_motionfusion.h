/*
  Copyright (c), 2001-2022, Shenshu Tech. Co., Ltd.
 */

#ifndef HI_MPI_MOTIONFUSION_H
#define HI_MPI_MOTIONFUSION_H

#include "hi_type.h"
#include "hi_common_motionfusion.h"

#ifdef __cplusplus
extern "C" {
#endif

hi_s32 hi_mpi_mfusion_set_attr(const hi_u32 fusion_id, const hi_mfusion_attr *mfusion_attr);
hi_s32 hi_mpi_mfusion_get_attr(const hi_u32 fusion_id, hi_mfusion_attr *mfusion_attr);

hi_s32 hi_mpi_mfusion_set_gyro_drift(const hi_u32 fusion_id,
                                     const hi_mfusion_drift *drift);
hi_s32 hi_mpi_mfusion_get_gyro_drift(const hi_u32 fusion_id,
                                     hi_mfusion_drift *drift);

hi_s32 hi_mpi_mfusion_set_gyro_six_side_calibration(const hi_u32 fusion_id,
                                                    const hi_mfusion_six_side_calibration *six_side_calibration);
hi_s32 hi_mpi_mfusion_get_gyro_six_side_calibration(const hi_u32 fusion_id,
                                                    hi_mfusion_six_side_calibration *six_side_calibration);

hi_s32 hi_mpi_mfusion_set_gyro_temperature_drift(const hi_u32 fusion_id,
                                                 const hi_mfusion_temperature_drift *temperature_drift);
hi_s32 hi_mpi_mfusion_get_gyro_temperature_drift(const hi_u32 fusion_id,
                                                 hi_mfusion_temperature_drift *temperature_drift);

hi_s32 hi_mpi_mfusion_set_gyro_online_temperature_drift(const hi_u32 fusion_id,
                                                        const hi_mfusion_temperature_drift *temperature_drift);
hi_s32 hi_mpi_mfusion_get_gyro_online_temperature_drift(const hi_u32 fusion_id,
                                                        hi_mfusion_temperature_drift *temperature_drift);

hi_s32 hi_mpi_mfusion_set_gyro_online_drift(const hi_u32 fusion_id,
                                            const hi_mfusion_drift *drift);
hi_s32 hi_mpi_mfusion_get_gyro_online_drift(const hi_u32 fusion_id,
                                            hi_mfusion_drift *drift);

hi_s32 hi_mpi_mfusion_init_rotation_compensation(const hi_u32 fusion_id,
    const hi_mfusion_rotation_cfg *cfg);
hi_s32 hi_mpi_mfusion_deinit_rotation_compensation(const hi_u32 fusion_id);
hi_s32 hi_mpi_mfusion_get_rotation_compensation(const hi_u32 fusion_id,
    hi_mfusion_rotation_compensation *compensation);
hi_s32 hi_mpi_mfusion_set_rotation_compensation(const hi_u32 fusion_id,
    const hi_mfusion_rotation_compensation *compensation);
hi_s32 hi_mpi_mfusion_send_quaternion(const hi_u32 fusion_id, hi_mfusion_quaternion_buf *quaternion_buf);
hi_s32 hi_mpi_mfusion_get_quaternion(const hi_u32 fusion_id, hi_u64 begin_pts, hi_u64 end_pts,
    hi_mfusion_quaternion_buf *quaternion_buf);

#ifdef __cplusplus
}
#endif
#endif /* HI_MPI_MOTIONFUSION_H */
