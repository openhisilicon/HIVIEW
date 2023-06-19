/*
  Copyright (c), 2001-2022, Shenshu Tech. Co., Ltd.
 */

#ifndef OT_COMMON_MOTIONSENSOR_H
#define OT_COMMON_MOTIONSENSOR_H

#include "ot_type.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

#define OT_MSENSOR_MAX_DATA_NUM        128
#define OT_MSENSOR_GRADIENT            (0x1 << 10)

#define OT_MSENSOR_TEMP_GYRO   0x1
#define OT_MSENSOR_TEMP_ACC    0x2
#define OT_MSENSOR_TEMP_MAGN   0x4
#define OT_MSENSOR_TEMP_ALL    0x7

#define OT_MSENSOR_DEVICE_GYRO 0x1
#define OT_MSENSOR_DEVICE_ACC  0x2
#define OT_MSENSOR_DEVICE_MAGN 0x4
#define OT_MSENSOR_DEVICE_ALL  0x7

typedef struct {
    td_u32  device_mask;
    td_u32  temperature_mask;
} ot_msensor_attr;

typedef struct {
    td_u64 phys_addr;
    td_u64 virt_addr;
    td_u32 buf_len;
} ot_msensor_buf_attr;

typedef struct {
    td_s32 x;
    td_s32 y;
    td_s32 z;
    td_s32 temperature;
    td_u64 pts;
} ot_msensor_sample_data;

typedef struct {
    ot_msensor_sample_data gyro_data[OT_MSENSOR_MAX_DATA_NUM];
    td_u32                 data_num;
} ot_msensor_gyro_buf;

typedef struct {
    ot_msensor_sample_data acc_data[OT_MSENSOR_MAX_DATA_NUM];
    td_u32                 data_num;
} ot_msensor_acc_buf;

typedef struct {
    ot_msensor_sample_data magn_data[OT_MSENSOR_MAX_DATA_NUM];
    td_u32                 data_num;
} ot_msensor_magn_buf;

typedef struct {
    td_u32 odr;
    td_u32 fsr;
    td_u8  data_width;
    td_s32 temperature_max;
    td_s32 temperature_min;
} ot_msensor_gyro_config;

typedef struct {
    td_u32 odr;
    td_u32 fsr;
    td_u8  data_width;
    td_s32 temperature_max;
    td_s32 temperature_min;
} ot_msensor_acc_config;

typedef struct {
    td_u32 odr;
    td_u32 fsr;
    td_u8  data_width;
    td_s32 temperature_max;
    td_s32 temperature_min;
} ot_msensor_magn_config;

typedef struct {
    ot_msensor_gyro_config  gyro_config;
    ot_msensor_acc_config   acc_config;
    ot_msensor_magn_config  magn_config;
} ot_msensor_config;

typedef struct {
    ot_msensor_buf_attr buf_attr;
    ot_msensor_config   config;
    ot_msensor_attr     attr;
} ot_msensor_param;

typedef enum {
    MSENSOR_DATA_GYRO = 0,
    MSENSOR_DATA_ACC,
    MSENSOR_DATA_MAGN,
    MSENSOR_DATA_BUTT
} ot_msensor_data_type;

typedef struct {
    td_s32 *x_phys_addr;
    td_s32 *y_phys_addr;
    td_s32 *z_phys_addr;
    td_s32 *temperature_phys_addr;
    td_u64 *pts_phys_addr;
    td_u32  num; /* number of valid data */
} ot_msensor_data_addr;

typedef struct {
    td_s32                id;
    ot_msensor_data_type  data_type;
    ot_msensor_data_addr  data[2]; /* cyclic buffer 2 */
    td_u64                begin_pts;
    td_u64                end_pts;
    td_s64                addr_offset;
} ot_msensor_data_info;

typedef struct {
    ot_msensor_attr        attr;
    ot_msensor_gyro_buf    gyro_buf;
    ot_msensor_acc_buf     acc_buf;
    ot_msensor_magn_buf    magn_buf;
} ot_msensor_data;

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* end of #ifdef __cplusplus */

#endif
