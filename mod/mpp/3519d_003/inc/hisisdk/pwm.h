/*
  Copyright (c), 2001-2022, Shenshu Tech. Co., Ltd.
 */

#ifndef OT_PWM_H
#define OT_PWM_H

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

typedef struct {
    unsigned char pwm_num;  /* 0:PWM0,1:PWM1,2:PWM2,3:PWMII0,4:PWMII1,5:PWMII2 */
    unsigned int  duty;
    unsigned int  period;
    unsigned char enable;
} pwm_data;

#define PWM_CMD_WRITE 0x01
#define PWM_CMD_READ  0x03

int pwm_drv_write(unsigned char pwm_num, unsigned int duty, unsigned int period, unsigned char enable);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif  /* OT_PWM_H */
