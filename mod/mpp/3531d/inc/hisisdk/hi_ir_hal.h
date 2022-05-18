/*
 * Copyright (c) Hisilicon Technologies Co., Ltd. 2006-2019. All rights reserved.
 * Description: Hal of hiir.
 * Author: Hisilicon multimedia software group
 * Create: 2006-12-16
 */

#ifndef __HIIR_HAL_H__
#define __HIIR_HAL_H__

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define HIIR_DEVICE_IRQ_NO 34 /* 34 is IR Interrupt source */
#define IR_REG_BASE 0x110f0000 /* 0x110f0000 is IR reg base Address */

#define IR_CRG_ADDR 0x11014640 /* 0x11014640 is IR CRG Address */
#define IR_BIT 4 /* 4bit is IR CRG Clock enable */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif