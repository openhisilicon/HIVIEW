/*
  Copyright (c), 2001-2022, Shenshu Tech. Co., Ltd.
 */

#ifndef HI_I2C_H
#define HI_I2C_H

#include "hi_type.h"
#include "ot_i2c.h"

#ifdef __cplusplus
extern "C" {
#endif

#define HI_I2C_RETRIES OT_I2C_RETRIES
#define HI_I2C_TIMEOUT OT_I2C_TIMEOUT
#define HI_I2C_SLAVE OT_I2C_SLAVE
#define HI_I2C_SLAVE_FORCE OT_I2C_SLAVE_FORCE
#define HI_I2C_TENBIT OT_I2C_TENBIT
#define HI_I2C_FUNCS OT_I2C_FUNCS
#define HI_I2C_RDWR OT_I2C_RDWR
#define HI_I2C_PEC OT_I2C_PEC
#define HI_I2C_SMBUS OT_I2C_SMBUS
#define HI_I2C_16BIT_REG OT_I2C_16BIT_REG
#define HI_I2C_16BIT_DATA OT_I2C_16BIT_DATA
typedef ot_i2c_data hi_i2c_data;

#ifdef __cplusplus
}
#endif
#endif /* HI_I2C_H */
