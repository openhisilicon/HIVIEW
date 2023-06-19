/*
  Copyright (c), 2001-2022, Shenshu Tech. Co., Ltd.
 */
#ifndef OT_COMMON_MOTIONFUSION_H
#define OT_COMMON_MOTIONFUSION_H

#include "ot_type.h"
#include "ot_errno.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

#define OT_ERR_GYRO_NOTWORK 0x30
#define OT_ERR_ACC_NOTWORK  0x31
#define OT_ERR_MODE         0x32
#define OT_ERR_USECASE      0x33

#define OT_ERR_MOTIONFUSION_NOT_CONFIG    OT_DEFINE_ERR(OT_ID_MOTIONFUSION, OT_ERR_LEVEL_ERROR, OT_ERR_NOT_CFG)
#define OT_ERR_MOTIONFUSION_NOBUF         OT_DEFINE_ERR(OT_ID_MOTIONFUSION, OT_ERR_LEVEL_ERROR, OT_ERR_NO_BUF)
#define OT_ERR_MOTIONFUSION_BUF_EMPTY     OT_DEFINE_ERR(OT_ID_MOTIONFUSION, OT_ERR_LEVEL_ERROR, OT_ERR_BUF_EMPTY)
#define OT_ERR_MOTIONFUSION_NULL_PTR      OT_DEFINE_ERR(OT_ID_MOTIONFUSION, OT_ERR_LEVEL_ERROR, OT_ERR_NULL_PTR)
#define OT_ERR_MOTIONFUSION_ILLEGAL_PARAM OT_DEFINE_ERR(OT_ID_MOTIONFUSION, OT_ERR_LEVEL_ERROR, OT_ERR_ILLEGAL_PARAM)
#define OT_ERR_MOTIONFUSION_BUF_FULL      OT_DEFINE_ERR(OT_ID_MOTIONFUSION, OT_ERR_LEVEL_ERROR, OT_ERR_BUF_FULL)
#define OT_ERR_MOTIONFUSION_SYS_NOTREADY  OT_DEFINE_ERR(OT_ID_MOTIONFUSION, OT_ERR_LEVEL_ERROR, OT_ERR_NOT_READY)
#define OT_ERR_MOTIONFUSION_NOT_SUPPORT   OT_DEFINE_ERR(OT_ID_MOTIONFUSION, OT_ERR_LEVEL_ERROR, OT_ERR_NOT_SUPPORT)
#define OT_ERR_MOTIONFUSION_NOT_PERMITTED OT_DEFINE_ERR(OT_ID_MOTIONFUSION, OT_ERR_LEVEL_ERROR, OT_ERR_NOT_PERM)
#define OT_ERR_MOTIONFUSION_BUSY          OT_DEFINE_ERR(OT_ID_MOTIONFUSION, OT_ERR_LEVEL_ERROR, OT_ERR_BUSY)
#define OT_ERR_MOTIONFUSION_INVALID_CHNID OT_DEFINE_ERR(OT_ID_MOTIONFUSION, OT_ERR_LEVEL_ERROR, OT_ERR_INVALID_CHN_ID)
#define OT_ERR_MOTIONFUSION_CHN_UNEXIST     OT_DEFINE_ERR(OT_ID_MOTIONFUSION, OT_ERR_LEVEL_ERROR, OT_ERR_UNEXIST)
#define OT_ERR_MOTIONFUSION_GYRO_NOTWORK    OT_DEFINE_ERR(OT_ID_MOTIONFUSION, OT_ERR_LEVEL_ERROR, OT_ERR_GYRO_NOTWORK)
#define OT_ERR_MOTIONFUSION_ACC_NOTWORK     OT_DEFINE_ERR(OT_ID_MOTIONFUSION, OT_ERR_LEVEL_ERROR, OT_ERR_ACC_NOTWORK)
#define OT_ERR_MOTIONFUSION_INVALID_MODE    OT_DEFINE_ERR(OT_ID_MOTIONFUSION, OT_ERR_LEVEL_ERROR, OT_ERR_MODE)
#define OT_ERR_MOTIONFUSION_INVALID_USECASE OT_DEFINE_ERR(OT_ID_MOTIONFUSION, OT_ERR_LEVEL_ERROR, OT_ERR_USECASE)

#define OT_MFUSION_MAX_CHN_NUM 1

#define OT_MFUSION_AXIS_NUM         3
#define OT_MFUSION_MATRIX_NUM       9
#define OT_MFUSION_MATRIX_TEMPERATURE_NUM  9
#define OT_MFUSION_TEMPERATURE_LUT_SAMPLES 30

#define OT_MFUSION_TEMPERATURE_GYRO 0x1
#define OT_MFUSION_TEMPERATURE_ACC  0x2
#define OT_MFUSION_TEMPERATURE_MAGN 0x4
#define OT_MFUSION_TEMPERATURE_ALL  0x7

#define OT_MFUSION_DEVICE_GYRO 0x1
#define OT_MFUSION_DEVICE_ACC  0x2
#define OT_MFUSION_DEVICE_MAGN 0x4
#define OT_MFUSION_DEVICE_ALL  0x7

#define OT_MFUSION_SIXSIDE_MATRIX_GRADINT 15
#define OT_MFUSION_ZERO_OFFSET_GRADINT    15
#define OT_MFUSION_TEMPERATURE_OFFSET_GRADINT    15

#define OT_MFUSION_COMMON_BUF_SIZE 128
#define OT_MFUSION_LUT_STATUS_NUM   2

typedef struct {
    /*
     * RW; continues steady time (in sec)
     * threshold for steady detection
     * range: [0, (1<<16-1]
     */
    td_u32 steady_time_thr;
    /*
     * RW; max gyro ZRO tolerance presented in datasheet,
     * with (ADC word length - 1) decimal bits
     * range: [0, 100 * (1<<15)]
     */
    td_s32 gyro_offset;
    /*
     * RW; max acc ZRO tolerance presented in datasheet,
     * with (ADC word length - 1) decimal bits
     * range: [0, 0.5 * (1<<15)]
     */
    td_s32 acc_offset;
    /*
     * RW; gyro rms noise of under the current filter BW,
     * with (ADC Word Length - 1) decimal bits
     * range: [0, 0.5 * (1<<15)]
     */
    td_s32 gyro_rms;
    /*
     * RW; acc rms noise of under the current filter BW
     * with (acc word length - 1) decimal bits
     * range: [0, 0.005 * (1<<15)]
     */
    td_s32 acc_rms;
    /*
     * RW; scale factor of gyro offset for steady detection,
     * larger -> higher recall, but less the precision
     * range: [0, 1000 * (1<<4)]
     */
    td_s32 gyro_offset_factor;
    /*
     * RW; scale factor of acc offset for steady detection,
     * larger -> higher recall, but less the precision
     * range: [0, 1000 * (1<<4)]
     */
    td_s32 acc_offset_factor;
    /*
     * RW; scale factor of gyro rms for steady detection,
     * larger -> higher recall, but less the precision
     * range: [0, 1000 * (1<<4)]
     */
    td_s32 gyro_rms_factor;
    /*
     * RW; scale factor of acc rms for steady detection,
     * larger -> higher recall, but less the precision
     * range: [0, 1000 * (1<<4)]
     */
    td_s32 acc_rms_factor;
} ot_mfusion_steady_detect_attr;

typedef struct {
    td_u32 device_mask;      /* device mask: gyro,acc or magn */
    td_u32 temperature_mask; /* temperature mask: gyro temperature ,acc temperatureor magn temperature */
    ot_mfusion_steady_detect_attr steady_detect_attr;
} ot_mfusion_attr;

/* angle data per sample */
typedef struct {
    td_s32 x;
    td_s32 y;
    td_s32 z;
    td_s32 temperature;
    td_u64 pts;
} ot_mfusion_sample_data;

typedef struct {
    ot_mfusion_sample_data gyro_data[OT_MFUSION_COMMON_BUF_SIZE];
    td_u32 buf_data_num; /* RW; data length of the occupied buffer */
    td_u32 buf_rep_num;  /* RW; data start position of invalid, in case of pts overlap */
} ot_mfusion_gyro_buf;

typedef struct {
    ot_mfusion_sample_data acc_data[OT_MFUSION_COMMON_BUF_SIZE];
    td_u32 buf_data_num; /* RW; data length of the occupied buffer */
    td_u32 buf_rep_num;  /* RW; data start position of invalid, in case of pts overlap */
} ot_mfusion_acc_buf;

typedef enum {
    OT_IMU_TEMPERATURE_DRIFT_CURV = 0, /* polynomial mode */
    OT_IMU_TEMPERATURE_DRIFT_LUT,      /* lookup table mode */
    OT_IMU_TEMPERATURE_DRIFT_BUTT
} ot_mfusion_temperature_drift_mode;

typedef struct {
    /*
     * RW;temperature drift lookup table
     * the 1st col is the time (in sec) for which the sample has not been updated.
     * the 2nd col is the nearest temperature sample during update
     */
    td_s32 imu_lut[OT_MFUSION_TEMPERATURE_LUT_SAMPLES][OT_MFUSION_AXIS_NUM];
    td_s32 gyro_lut_status[OT_MFUSION_TEMPERATURE_LUT_SAMPLES][OT_MFUSION_LUT_STATUS_NUM];
    td_s32 range_min; /* RW;temperature range minimum  */
    td_s32 range_max; /* RW;temperature range maximum  */
    td_u32 step;
} ot_mfusion_temperature_drift_lut;

typedef struct {
    td_bool enable;
    ot_mfusion_temperature_drift_mode mode; /* RW;temperature drift mode  */
    union {
        /*
         * RW;temperature drift polynomial matrix data
         * AUTO:ot_mfusion_temperature_drift_mode:OT_IMU_TEMPERATURE_DRIFT_CURV
         */
        td_s32 temperature_matrix[OT_MFUSION_MATRIX_TEMPERATURE_NUM];
        /*
         * RW;temperature drift lookup table data
         * AUTO:ot_mfusion_temperature_drift_mode:OT_IMU_TEMPERATURE_DRIFT_LUT
         */
        ot_mfusion_temperature_drift_lut temperature_lut;
    };
} ot_mfusion_temperature_drift;

typedef struct {
    td_bool enable;
    td_s32 drift[OT_MFUSION_AXIS_NUM];
} ot_mfusion_drift;

typedef struct {
    td_bool enable;
    td_s32 matrix[OT_MFUSION_MATRIX_NUM];
} ot_mfusion_six_side_calibration;

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif

