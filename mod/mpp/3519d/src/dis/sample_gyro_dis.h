/*
  Copyright (c), 2001-2024, Shenshu Tech. Co., Ltd.
 */

#ifndef OT_SAMPLE_GYRO_DIS_H
#define OT_SAMPLE_GYRO_DIS_H

#include "hi_type.h"
#include "hi_common_vo.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

hi_s32 sample_dis_ipc_gyro(hi_vo_intf_type vo_intf_type);

hi_s32 sample_dis_dv_gyro(hi_vo_intf_type vo_intf_type);

hi_s32 sample_dis_gyro_demo(hi_vo_intf_type vo_intf_type);

hi_s32 sample_dis_gyro_ldc_switch(hi_vo_intf_type vo_intf_type);

hi_s32 sample_dis_gyro_ldcv2_switch(hi_vo_intf_type vo_intf_type);

hi_s32 sample_dis_ipc_gyro_two_sensor(hi_size *size);

hi_void sample_dis_stop_gyro(hi_void);

hi_s32 sample_dis_gyro_rotation_compensation(hi_vo_intf_type vo_intf_type);

hi_s32 sample_dis_send(hi_vo_intf_type vo_intf_type, hi_size* input_size);

hi_s32 sample_dis_query_center_point(hi_vo_intf_type vo_intf_type);


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of __cplusplus */

#endif /* OT_SAMPLE_GYRO_DIS_H */

