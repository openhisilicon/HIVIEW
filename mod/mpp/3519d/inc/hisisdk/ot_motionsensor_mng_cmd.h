/*
  Copyright (c), 2001-2024, Shenshu Tech. Co., Ltd.
 */

#ifndef OT_MOTIONSENSOR_MNG_CMD_H
#define OT_MOTIONSENSOR_MNG_CMD_H

#include "osal_ioctl.h"
#include "ot_common_motionsensor.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* end of #ifdef __cplusplus */

#define MSENSOR_TYPE_MNG    10

typedef enum {
    IOC_NR_MSENSOR_MNG_GET_DATA = 0,
    IOC_NR_MSENSOR_MNG_RELEASE_BUF,
    IOC_NR_MSENSOR_MNG_ADD_USER,
    IOC_NR_MSENSOR_MNG_DELETE_USER,
    IOC_NR_MSENSOR_MNG_SEND_DATA,
    IOC_NR_MSENSOR_MNG_BUTT
} ioc_nr_msensor_mng;

#define MSENSOR_CMD_GET_DATA    _IOWR(MSENSOR_TYPE_MNG, IOC_NR_MSENSOR_MNG_GET_DATA,    ot_msensor_data_info)
#define MSENSOR_CMD_RELEASE_BUF _IOWR(MSENSOR_TYPE_MNG, IOC_NR_MSENSOR_MNG_RELEASE_BUF, ot_msensor_data_info)
#define MSENSOR_CMD_ADD_USER    _IOWR(MSENSOR_TYPE_MNG, IOC_NR_MSENSOR_MNG_ADD_USER,    td_s32)
#define MSENSOR_CMD_DELETE_USER _IOWR(MSENSOR_TYPE_MNG, IOC_NR_MSENSOR_MNG_DELETE_USER, td_s32)
#define MSENSOR_CMD_SEND_DATA   _IOW(MSENSOR_TYPE_MNG,  IOC_NR_MSENSOR_MNG_SEND_DATA,   ot_msensor_data)

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* end of #ifdef __cplusplus */

#endif
