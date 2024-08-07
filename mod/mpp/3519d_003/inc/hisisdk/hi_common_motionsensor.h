/*
  Copyright (c), 2001-2022, Shenshu Tech. Co., Ltd.
 */

#ifndef _HI_COMMON_MOTIONSENSOR_H__
#define _HI_COMMON_MOTIONSENSOR_H__

#include "ot_common_motionsensor.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define   HI_MSENSOR_MAX_DATA_NUM OT_MSENSOR_MAX_DATA_NUM
#define   HI_MSENSOR_GRADIENT     OT_MSENSOR_GRADIENT
#define   HI_MSENSOR_TEMP_GYRO    OT_MSENSOR_TEMP_GYRO
#define   HI_MSENSOR_TEMP_ACC     OT_MSENSOR_TEMP_ACC
#define   HI_MSENSOR_TEMP_MAGN    OT_MSENSOR_TEMP_MAGN
#define   HI_MSENSOR_TEMP_ALL     OT_MSENSOR_TEMP_ALL
#define   HI_MSENSOR_DEVICE_GYRO  OT_MSENSOR_DEVICE_GYRO
#define   HI_MSENSOR_DEVICE_ACC   OT_MSENSOR_DEVICE_ACC
#define   HI_MSENSOR_DEVICE_MAGN  OT_MSENSOR_DEVICE_MAGN
#define   HI_MSENSOR_DEVICE_ALL   OT_MSENSOR_DEVICE_ALL

typedef ot_msensor_attr hi_msensor_attr;
typedef ot_msensor_buf_attr hi_msensor_buf_attr;
typedef ot_msensor_sample_data hi_msensor_sample_data;
typedef ot_msensor_gyro_buf hi_msensor_gyro_buf;
typedef ot_msensor_acc_buf hi_msensor_acc_buf;
typedef ot_msensor_magn_buf hi_msensor_magn_buf;
typedef ot_msensor_gyro_config hi_msensor_gyro_config;
typedef ot_msensor_acc_config hi_msensor_acc_config;
typedef ot_msensor_magn_config hi_msensor_magn_config;
typedef ot_msensor_config hi_msensor_config;
typedef ot_msensor_param hi_msensor_param;
typedef ot_msensor_data_type hi_msensor_data_type;
typedef ot_msensor_data_addr hi_msensor_data_addr;
typedef ot_msensor_data_info hi_msensor_data_info;
typedef ot_msensor_data hi_msensor_data;

#ifdef __cplusplus
}
#endif /* end of #ifdef __cplusplus */

#endif
