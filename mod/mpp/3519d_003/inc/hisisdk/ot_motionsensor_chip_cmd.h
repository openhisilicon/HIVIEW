/*
  Copyright (c), 2001-2022, Shenshu Tech. Co., Ltd.
 */

#ifndef MOTIONSENSOR_CHIP_CMD_H
#define MOTIONSENSOR_CHIP_CMD_H

#include "osal_ioctl.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* end of #ifdef __cplusplus */

#define MSENSOR_TYPE_CHIP   12

typedef enum {
    IOC_NR_MSENSOR_CHIP_START = 0,
    IOC_NR_MSENSOR_CHIP_STOP,
    IOC_NR_MSENSOR_CHIP_INIT,
    IOC_NR_MSENSOR_CHIP_DEINIT,
    IOC_NR_MSENSOR_CHIP_GET_PARAM,
    IOC_NR_MSENSOR_CHIP_MNG_INIT,
    IOC_NR_MSENSOR_CHIP_BUTT
} ioc_nr_msensor_chip;

#define MSENSOR_CMD_START     _IO(MSENSOR_TYPE_CHIP, IOC_NR_MSENSOR_CHIP_START)
#define MSENSOR_CMD_STOP      _IO(MSENSOR_TYPE_CHIP, IOC_NR_MSENSOR_CHIP_STOP)
#define MSENSOR_CMD_INIT      _IOW(MSENSOR_TYPE_CHIP, IOC_NR_MSENSOR_CHIP_INIT, ot_msensor_param)
#define MSENSOR_CMD_DEINIT    _IO(MSENSOR_TYPE_CHIP, IOC_NR_MSENSOR_CHIP_DEINIT)
#define MSENSOR_CMD_GET_PARAM _IOR(MSENSOR_TYPE_CHIP, IOC_NR_MSENSOR_CHIP_GET_PARAM, ot_msensor_param)
#define MSENSOR_CMD_MNG_INIT  _IOW(MSENSOR_TYPE_CHIP, IOC_NR_MSENSOR_CHIP_MNG_INIT, ot_msensor_param)

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* end of #ifdef __cplusplus */

#endif
