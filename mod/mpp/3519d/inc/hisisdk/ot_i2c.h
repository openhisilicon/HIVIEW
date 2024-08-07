/*
  Copyright (c), 2001-2024, Shenshu Tech. Co., Ltd.
 */

#ifndef OT_I2C_H
#define OT_I2C_H

#include "ot_type.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* End of #ifdef __cplusplus */

/* /dev/i2c-X ioctl commands.  The ioctl's parameter is always an
 * unsigned long, except for:
 *      - I2C_FUNCS, takes pointer to an unsigned long
 *      - I2C_RDWR, takes pointer to struct i2c_rdwr_ioctl_data
 *      - I2C_SMBUS, takes pointer to struct i2c_smbus_ioctl_data
 * */
#define OT_I2C_RETRIES     0x0701  /* number of times a device address should be polled when not acknowledging */
#define OT_I2C_TIMEOUT     0x0702  /* set timeout in units of 10 ms */

/* NOTE: Slave address is 7 or 10 bits, but 10-bit addresses
 * are NOT supported! (due to code brokenness)
 * */
#define OT_I2C_SLAVE       0x0703  /* Use this slave address */
#define OT_I2C_SLAVE_FORCE 0x0706  /* Use this slave address, even if it is already in use by a driver! */
#define OT_I2C_TENBIT      0x0704  /* 0 for 7 bit addrs, != 0 for 10 bit */

#define OT_I2C_FUNCS       0x0705  /* Get the adapter functionality mask */

#define OT_I2C_RDWR        0x0707  /* Combined R/W transfer (one STOP only) */

#define OT_I2C_PEC         0x0708  /* != 0 to use PEC with SMBus */
#define OT_I2C_SMBUS       0x0720  /* SMBus transfer */
#define OT_I2C_16BIT_REG   0x0709  /* 16BIT REG WIDTH */
#define OT_I2C_16BIT_DATA  0x070a  /* 16BIT DATA WIDTH */

typedef struct {
    td_u8  dev_addr;
    td_u32 reg_addr;
    td_u32 addr_byte_num;
    td_u32 data;
    td_u32 data_byte_num;
} ot_i2c_data;

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */

#endif /* OT_I2C_H */
