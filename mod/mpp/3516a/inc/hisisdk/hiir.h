/*
 * Copyright (c) Hisilicon Technologies Co., Ltd. 2006-2019. All rights reserved.
 * Description: Head of hiir.
 * Author: Hisilicon multimedia software group
 * Create: 2006-12-16
 */

#ifndef __HI_IR_H__
#define __HI_IR_H__

#include <linux/types.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* define device name */
#define HIIR_DEVICE_NAME "Hi_IR"

/* Ioctl definitions */
#define IR_IOC_SET_BUF                  0x01
#define IR_IOC_SET_ENABLE_KEYUP         0x02  /* 1:check keyup */
#define IR_IOC_SET_ENABLE_REPKEY        0x03  /* 1:check repkey, 0:hardware behave */
#define IR_IOC_SET_REPKEY_TIMEOUTVAL    0x04

#define IR_IOC_SET_FORMAT               0x10
#define IR_IOC_SET_CODELEN              0x11
#define IR_IOC_SET_FREQ                 0x12
#define IR_IOC_SET_LEADS                0x13
#define IR_IOC_SET_LEADE                0x14
#define IR_IOC_SET_SLEADE               0x15
#define IR_IOC_SET_CNT0_B               0x16
#define IR_IOC_SET_CNT1_B               0x17

#define IR_IOC_GET_CONFIG               0x20

#define IR_IOC_ENDBG                    0x31
#define IR_IOC_DISDBG                   0x32

#define HIIR_DEFAULT_FREQ               24  /* 24M */

/* DEFINE KEY STATE */
#define HIIR_KEY_DOWN                   0x00
#define HIIR_KEY_UP                     0x01

#define IR_CLOCK_REG_LENGTH             0x4

/* device parameter */
typedef struct {
    unsigned short int leads_min;
    unsigned short int leads_max;

    unsigned short int leade_min;
    unsigned short int leade_max;

    unsigned short int cnt0_b_min;
    unsigned short int cnt0_b_max;

    unsigned short int cnt1_b_min;
    unsigned short int cnt1_b_max;

    unsigned short int sleade_min;
    unsigned short int sleade_max;

    unsigned short int code_len;
    unsigned short int codetype;
    unsigned short int frequence;
} hiir_dev_param;

typedef struct {
    unsigned long irkey_datah;
    unsigned long irkey_datal;
    unsigned long irkey_state_code;
} irkey_info_s;

int hiir_init(void);
void hiir_exit(void);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* End of #ifndef __HI_IR_H__*/