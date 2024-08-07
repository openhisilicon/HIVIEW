/*
  Copyright (c), 2001-2024, Shenshu Tech. Co., Ltd.
 */

#ifndef OT_ADC_H
#define OT_ADC_H

#include "ot_type.h"
#include "osal_ioctl.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define ot_adc_unused(x) (void)(x)

int lsadc_init(void);
void lsadc_exit(void);
uintptr_t *get_adc_irq(void);
volatile void **get_adc_reg_base(void);

#define LSADC_IOCTL_BASE 'A'

#define LSADC_CLOCK_REG_LENGTH  0x4
#define LSADC_REG_LENGTH        0x100

typedef enum {
    IOC_NR_LSADC_MODEL_SEL = 0,
    IOC_NR_LSADC_CHN_ENABLE,
    IOC_NR_LSADC_CHN_DISABLE,
    IOC_NR_LSADC_START,
    IOC_NR_LSADC_STOP,
    IOC_NR_LSADC_GET_CHNVAL,
    IOC_NR_LSADC_BUTT
} ioc_nr_lsadc;

#define LSADC_IOC_MODEL_SEL     _IOWR(LSADC_IOCTL_BASE, IOC_NR_LSADC_MODEL_SEL, int)
#define LSADC_IOC_CHN_ENABLE    _IOW(LSADC_IOCTL_BASE, IOC_NR_LSADC_CHN_ENABLE, int)
#define LSADC_IOC_CHN_DISABLE   _IOW(LSADC_IOCTL_BASE, IOC_NR_LSADC_CHN_DISABLE, int)
#define LSADC_IOC_START         _IO(LSADC_IOCTL_BASE, IOC_NR_LSADC_START)
#define LSADC_IOC_STOP          _IO(LSADC_IOCTL_BASE, IOC_NR_LSADC_STOP)
#define LSADC_IOC_GET_CHNVAL    _IOWR(LSADC_IOCTL_BASE, IOC_NR_LSADC_GET_CHNVAL, int)

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __OT_ADC_H__ */
