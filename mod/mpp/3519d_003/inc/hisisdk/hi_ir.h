/*
  Copyright (c), 2001-2022, Shenshu Tech. Co., Ltd.
 */

#ifndef HI_IR_H
#define HI_IR_H

#include "ot_ir.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* define device name */
#define HI_IR_DEVICE_NAME OT_IR_DEVICE_NAME

#define HI_IR_DEFAULT_FREQ OT_IR_DEFAULT_FREQ

/* DEFINE KEY STATE */
#define HI_IR_KEY_DOWN   OT_IR_KEY_DOWN
#define HI_IR_KEY_UP     OT_IR_KEY_UP

/* device parameter */
typedef ot_ir_dev_param hi_ir_dev_param;

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* End of HI_IR_H */
