/*
 * Copyright (c) Hisilicon Technologies Co., Ltd. 2019-2020. All rights reserved.
 * Description: es8388 driver.
 * Author: Hisilicon multimedia software group
 * Create: 2019-10-10
 * History:
 *  1. 2019-10-10 Created file
 */

#ifndef __ES8388_H__
#define __ES8388_H__

/* ES8388 Register Definition */
#define ES8388_REG0_CHIP_CONTROL1           0
#define ES8388_REG1_CHIP_CONTROL2           1
#define ES8388_REG2_CHIP_POWER_MANAGEMENT   2
#define ES8388_REG3_ADC_POWER_MANAGEMENT    3
#define ES8388_REG4_DAC_POWER_MANAGEMENT    4
#define ES8388_REG5_CHIP_LOW_POWER1         5
#define ES8388_REG6_CHIP_LOW_POWER2         6
#define ES8388_REG7_ANALOG_VOLTAGE_MANAGEMENT   7
#define ES8388_REG8_MASTER_MODE_CONTROL     8

#define ES8388_REG9_ADC_CONTROL1            9
#define ES8388_REG10_ADC_CONTROL2           10
#define ES8388_REG11_ADC_CONTROL3           11
#define ES8388_REG12_ADC_CONTROL4           12
#define ES8388_REG13_ADC_CONTROL5           13
#define ES8388_REG14_ADC_CONTROL6           14
#define ES8388_REG15_ADC_CONTROL7           15
#define ES8388_REG16_ADC_CONTROL8           16
#define ES8388_REG17_ADC_CONTROL9           17
#define ES8388_REG18_ADC_CONTROL10          18
#define ES8388_REG19_ADC_CONTROL11          19
#define ES8388_REG20_ADC_CONTROL12          20
#define ES8388_REG21_ADC_CONTROL13          21
#define ES8388_REG22_ADC_CONTROL14          22

#define ES8388_REG23_DAC_CONTROL1           23
#define ES8388_REG24_DAC_CONTROL2           24
#define ES8388_REG25_DAC_CONTROL3           25
#define ES8388_REG26_DAC_CONTROL4           26
#define ES8388_REG27_DAC_CONTROL5           27
#define ES8388_REG28_DAC_CONTROL6           28
#define ES8388_REG29_DAC_CONTROL7           29
#define ES8388_REG30_DAC_CONTROL8           30
#define ES8388_REG31_DAC_CONTROL9           31
#define ES8388_REG32_DAC_CONTROL10          32
#define ES8388_REG33_DAC_CONTROL11          33
#define ES8388_REG34_DAC_CONTROL12          34
#define ES8388_REG35_DAC_CONTROL13          35
#define ES8388_REG36_DAC_CONTROL14          36
#define ES8388_REG37_DAC_CONTROL15          37
#define ES8388_REG38_DAC_CONTROL16          38
#define ES8388_REG39_DAC_CONTROL17          39
#define ES8388_REG40_DAC_CONTROL18          40
#define ES8388_REG41_DAC_CONTROL19          41
#define ES8388_REG42_DAC_CONTROL20          42
#define ES8388_REG43_DAC_CONTROL21          43
#define ES8388_REG44_DAC_CONTROL22          44
#define ES8388_REG45_DAC_CONTROL23          45
#define ES8388_REG46_DAC_CONTROL24          46
#define ES8388_REG47_DAC_CONTROL25          47
#define ES8388_REG48_DAC_CONTROL26          48
#define ES8388_REG49_DAC_CONTROL27          49
#define ES8388_REG50_DAC_CONTROL28          50
#define ES8388_REG51_DAC_CONTROL29          51
#define ES8388_REG52_DAC_CONTROL30          52

/* register number */
#define ES8388_REG_NUM      53

/* master/slave mode */
#define ES8388_MASTER_MODE  0x1
#define ES8388_SLAVE_MODE   0x0

/* I2S clk polarity */
#define ES8388_I2S_POLARITY_NORMAL  0x0
#define ES8388_I2S_POLARITY_INVERT  0x1

/* PCM offset */
#define ES8388_PCM_OFFSET_2ND   0x0
#define ES8388_PCM_OFFSET_1ST   0x1

/* BCLK direction */
#define ES8388_BCLK_DIR_NORMAL  0x0
#define ES8388_BCLK_DIR_INVERT  0x1

/* BCLK clock dividers */
#define ES8388_BCLK_DIV_AUTO    0x00
#define ES8388_BCLK_DIV_1       0x01
#define ES8388_BCLK_DIV_2       0x02
#define ES8388_BCLK_DIV_4       0x04
#define ES8388_BCLK_DIV_6       0x05
#define ES8388_BCLK_DIV_8       0x06

/* FSCLK clock dividers */
#define ES8388_FSCLK_DIV_1536   0x0A
#define ES8388_FSCLK_DIV_1024   0x07
#define ES8388_FSCLK_DIV_768    0x06
#define ES8388_FSCLK_DIV_512    0x04
#define ES8388_FSCLK_DIV_384    0x03
#define ES8388_FSCLK_DIV_256    0x02
#define ES8388_FSCLK_DIV_128    0x00

/* data format */
#define ES8388_DATA_FORMAT_I2S              0x0
#define ES8388_DATA_FORMAT_LEFT_JUSTIFY     0x1
#define ES8388_DATA_FORMAT_RIGHT_JUSTIFY    0x2
#define ES8388_DATA_FORMAT_PCM              0x3

/* data length */
#define ES8388_DATA_LENGTH_24BIT        0x0
#define ES8388_DATA_LENGTH_20BIT        0x1
#define ES8388_DATA_LENGTH_18BIT        0x2
#define ES8388_DATA_LENGTH_16BIT        0x3
#define ES8388_DATA_LENGTH_32BIT        0x4

/* input select */
#define ES8388_INPUT1       0x0
#define ES8388_INPUT2       0x1
#define ES8388_INPUT_DIFF   0x3

/* power up/down */
#define ES8388_POWER_UP     0x0
#define ES8388_POWER_DOWN   0x1

/* out enable/disable */
#define ES8388_OUT_DISABLE  0x0
#define ES8388_OUT_ENABLE   0x1

/* PGA gain */
#define ES8388_PGA_GAIN_0DB     0x0
#define ES8388_PGA_GAIN_3DB     0x1
#define ES8388_PGA_GAIN_6DB     0x2
#define ES8388_PGA_GAIN_9DB     0x3
#define ES8388_PGA_GAIN_12DB    0x4
#define ES8388_PGA_GAIN_15DB    0x5
#define ES8388_PGA_GAIN_18DB    0x6
#define ES8388_PGA_GAIN_21DB    0x7
#define ES8388_PGA_GAIN_24DB    0x8

/* ADc/DAC mute */
#define ES8388_MUTE_ENABLE      0x1
#define ES8388_MUTE_DISABLE     0x0

/* ADC volume */
#define ES8388_ADC_VOLUME_0DB   0x00

/* DAC volume */
#define ES8388_DAC_VOLUME_0DB   0x00

/* output volume */
#define ES8388_OUTPUT_VOLUME_0DB    0x1E

typedef enum {
    /* clk */
    ES8388_SET_ADC_MCLK_LRCLK_RATIO = 0x0,
    ES8388_SET_DAC_MCLK_LRCLK_RATIO,
    ES8388_SET_MCLK_BCLK_RATIO,
    ES8388_SET_ADC_POLARITY_AND_OFFSET,
    ES8388_SET_DAC_POLARITY_AND_OFFSET,
    ES8388_SET_MASTER_MODE,
    ES8388_SET_BCLK_DIR,
    /* data */
    ES8388_SET_ADC_DATA_WIDTH,
    ES8388_SET_DAC_DATA_WIDTH,
    ES8388_SET_ADC_DATA_FORMAT,
    ES8388_SET_DAC_DATA_FORMAT,
    /* input select */
    ES8388_SET_LEFT_INPUT_SELECT,
    ES8388_SET_RIGHT_INPUT_SELECT,
    /* input volume */
    ES8388_SET_LFET_INPUT_GAIN,
    ES8388_SET_RIGHT_INPUT_GAIN,
    ES8388_SET_LFET_ADC_VOLUME,
    ES8388_SET_RIGHT_ADC_VOLUME,
    ES8388_SET_ADC_MUTE,
    /* output volume */
    ES8388_SET_LFET_DAC_VOLUME,
    ES8388_SET_RIGHT_DAC_VOLUME,
    ES8388_SET_LFET_OUTPUT1_VOLUME,
    ES8388_SET_LFET_OUTPUT2_VOLUME,
    ES8388_SET_RIGHT_OUTPUT1_VOLUME,
    ES8388_SET_RIGHT_OUTPUT2_VOLUME,
    ES8388_SET_DAC_MUTE,
    /* input power */
    ES8388_SET_LFET_INPUT_POWER,
    ES8388_SET_RIGHT_INPUT_POWER,
    ES8388_SET_LFET_ADC_POWER,
    ES8388_SET_RIGHT_ADC_POWER,
    /* output power */
    ES8388_SET_LFET_DAC_POWER,
    ES8388_SET_RIGHT_DAC_POWER,
    ES8388_SET_LFET_OUTPUT1_ENABLE,
    ES8388_SET_RIGHT_OUTPUT1_ENABLE,
    ES8388_SET_LFET_OUTPUT2_ENABLE,
    ES8388_SET_RIGHT_OUTPUT2_ENABLE,
    /* control */
    ES8388_SOFT_RESET,
    ES8388_REG_DUMP,
} hi_es8388_codec_ioctl;

typedef union {
    struct {
        unsigned char int1_lp: 1;
        unsigned char flash_lp: 1;
        unsigned char pdn_adc_biasgen: 1;
        unsigned char pdn_micb: 1;
        unsigned char pdn_adcr: 1;
        unsigned char pdn_adcl: 1;
        unsigned char pdn_ainr: 1;
        unsigned char pdn_ainl: 1;
    } bit;
    unsigned char b8;
} es8388_reg3_ctrl;

typedef union {
    struct {
        unsigned char reserved: 2;
        unsigned char rout2: 1;
        unsigned char lout2: 1;
        unsigned char rout1: 1;
        unsigned char lout1: 1;
        unsigned char pdn_dacr: 1;
        unsigned char pdn_dacl: 1;
    } bit;
    unsigned char b8;
} es8388_reg4_ctrl;

typedef union {
    struct {
        unsigned char bclk_div: 5;
        unsigned char bclk_inv: 1;
        unsigned char mclk_div2: 1;
        unsigned char msc: 1;
    } bit;
    unsigned char b8;
} es8388_reg8_ctrl;

typedef union {
    struct {
        unsigned char mic_amp_r: 4;
        unsigned char mic_amp_l: 4;
    } bit;
    unsigned char b8;
} es8388_reg9_ctrl;

typedef union {
    struct {
        unsigned char reserved: 2;
        unsigned char dsr: 1;
        unsigned char ds_sel: 1;
        unsigned char rin_sel: 2;
        unsigned char lin_sel: 2;
    } bit;
    unsigned char b8;
} es8388_reg10_ctrl;

typedef union {
    struct {
        unsigned char adc_format: 2;
        unsigned char adc_wl: 3;
        unsigned char adc_lr_p: 1;
        unsigned char dat_sel: 2;
    } bit;
    unsigned char b8;
} es8388_reg12_ctrl;

typedef union {
    struct {
        unsigned char adc_fs_ratio: 5;
        unsigned char adc_fs_mode: 1;
        unsigned char reserved: 2;
    } bit;
    unsigned char b8;
} es8388_reg13_ctrl;

typedef union {
    struct {
        unsigned char reserved_0: 2;
        unsigned char adc_mute: 1;
        unsigned char adc_le_r: 1;
        unsigned char reserved_1: 1;
        unsigned char adc_soft_ramp: 1;
        unsigned char adc_ramp_rate: 2;
    } bit;
    unsigned char b8;
} es8388_reg15_ctrl;

typedef union {
    struct {
        unsigned char ladc_vol: 8;
    } bit;
    unsigned char b8;
} es8388_reg16_ctrl;

typedef union {
    struct {
        unsigned char radc_vol: 8;
    } bit;
    unsigned char b8;
} es8388_reg17_ctrl;

typedef union {
    struct {
        unsigned char reserved: 1;
        unsigned char dac_format: 2;
        unsigned char dac_wl: 3;
        unsigned char dac_lr_p: 1;
        unsigned char dac_lr_swap: 1;
    } bit;
    unsigned char b8;
} es8388_reg23_ctrl;

typedef union {
    struct {
        unsigned char dac_fs_ratio: 5;
        unsigned char dac_fs_mode: 1;
        unsigned char reserved: 2;
    } bit;
    unsigned char b8;
} es8388_reg24_ctrl;

typedef union {
    struct {
        unsigned char reserved_0: 2;
        unsigned char dac_mute: 1;
        unsigned char dac_le_r: 1;
        unsigned char reserved_1: 1;
        unsigned char dac_soft_ramp: 1;
        unsigned char dac_ramp_rate: 2;
    } bit;
    unsigned char b8;
} es8388_reg25_ctrl;

typedef union {
    struct {
        unsigned char ldac_vol: 8;
    } bit;
    unsigned char b8;
} es8388_reg26_ctrl;

typedef union {
    struct {
        unsigned char rdac_vol: 8;
    } bit;
    unsigned char b8;
} es8388_reg27_ctrl;

typedef union {
    struct {
        unsigned char lout1_vol: 6;
        unsigned char reserved: 2;
    } bit;
    unsigned char b8;
} es8388_reg46_ctrl;

typedef union {
    struct {
        unsigned char rout1_vol: 6;
        unsigned char reserved: 2;
    } bit;
    unsigned char b8;
} es8388_reg47_ctrl;

typedef union {
    struct {
        unsigned char lout2_vol: 6;
        unsigned char reserved: 2;
    } bit;
    unsigned char b8;
} es8388_reg48_ctrl;

typedef union {
    struct {
        unsigned char rout2_vol: 6;
        unsigned char reserved: 2;
    } bit;
    unsigned char b8;
} es8388_reg49_ctrl;

typedef struct {
    unsigned int chip_num: 3;
    unsigned int audio_in_select: 2;
    unsigned int clk_div: 5;
    unsigned int clk_polarity: 1;
    unsigned int data_format: 2;
    unsigned int data_length: 3;
    unsigned int ctrl_mode: 1;
    unsigned int volume: 8;
    unsigned int if_mute:1;
    unsigned int if_powerup: 1;
    unsigned int reserved : 5;
} hi_es8388_audio_ctrl;

#define ES8388_IOC_MAGIC 'E'

/* clk */
#define HI_ES8388_SET_ADC_MCLK_LRCLK_RATIO \
    _IOW(ES8388_IOC_MAGIC, ES8388_SET_ADC_MCLK_LRCLK_RATIO, hi_es8388_audio_ctrl)
#define HI_ES8388_SET_DAC_MCLK_LRCLK_RATIO \
    _IOW(ES8388_IOC_MAGIC, ES8388_SET_DAC_MCLK_LRCLK_RATIO, hi_es8388_audio_ctrl)
#define HI_ES8388_SET_MCLK_BCLK_RATIO \
    _IOW(ES8388_IOC_MAGIC, ES8388_SET_MCLK_BCLK_RATIO, hi_es8388_audio_ctrl)
#define HI_ES8388_SET_ADC_POLARITY_AND_OFFSET \
    _IOW(ES8388_IOC_MAGIC, ES8388_SET_ADC_POLARITY_AND_OFFSET, hi_es8388_audio_ctrl)
#define HI_ES8388_SET_DAC_POLARITY_AND_OFFSET \
    _IOW(ES8388_IOC_MAGIC, ES8388_SET_DAC_POLARITY_AND_OFFSET, hi_es8388_audio_ctrl)
#define HI_ES8388_SET_MASTER_MODE \
    _IOW(ES8388_IOC_MAGIC, ES8388_SET_MASTER_MODE, hi_es8388_audio_ctrl)
#define HI_ES8388_SET_BCLK_DIR \
    _IOW(ES8388_IOC_MAGIC, ES8388_SET_BCLK_DIR, hi_es8388_audio_ctrl)

/* data */
#define HI_ES8388_SET_ADC_DATA_WIDTH \
    _IOW(ES8388_IOC_MAGIC, ES8388_SET_ADC_DATA_WIDTH, hi_es8388_audio_ctrl)
#define HI_ES8388_SET_DAC_DATA_WIDTH \
    _IOW(ES8388_IOC_MAGIC, ES8388_SET_DAC_DATA_WIDTH, hi_es8388_audio_ctrl)
#define HI_ES8388_SET_ADC_DATA_FORMAT \
    _IOW(ES8388_IOC_MAGIC, ES8388_SET_ADC_DATA_FORMAT, hi_es8388_audio_ctrl)
#define HI_ES8388_SET_DAC_DATA_FORMAT \
    _IOW(ES8388_IOC_MAGIC, ES8388_SET_DAC_DATA_FORMAT, hi_es8388_audio_ctrl)

/* input select */
#define HI_ES8388_SET_LEFT_INPUT_SELECT \
    _IOW(ES8388_IOC_MAGIC, ES8388_SET_LEFT_INPUT_SELECT, hi_es8388_audio_ctrl)
#define HI_ES8388_SET_RIGHT_INPUT_SELECT \
    _IOW(ES8388_IOC_MAGIC, ES8388_SET_RIGHT_INPUT_SELECT, hi_es8388_audio_ctrl)

/* input volume */
#define HI_ES8388_SET_LFET_INPUT_GAIN \
    _IOW(ES8388_IOC_MAGIC, ES8388_SET_LFET_INPUT_GAIN, hi_es8388_audio_ctrl)
#define HI_ES8388_SET_RIGHT_INPUT_GAIN \
    _IOW(ES8388_IOC_MAGIC, ES8388_SET_RIGHT_INPUT_GAIN, hi_es8388_audio_ctrl)
#define HI_ES8388_SET_LFET_ADC_VOLUME \
    _IOW(ES8388_IOC_MAGIC, ES8388_SET_LFET_ADC_VOLUME, hi_es8388_audio_ctrl)
#define HI_ES8388_SET_RIGHT_ADC_VOLUME \
    _IOW(ES8388_IOC_MAGIC, ES8388_SET_RIGHT_ADC_VOLUME, hi_es8388_audio_ctrl)
#define HI_ES8388_SET_ADC_MUTE \
    _IOW(ES8388_IOC_MAGIC, ES8388_SET_ADC_MUTE, hi_es8388_audio_ctrl)

/* output volume */
#define HI_ES8388_SET_LFET_DAC_VOLUME \
    _IOW(ES8388_IOC_MAGIC, ES8388_SET_LFET_DAC_VOLUME, hi_es8388_audio_ctrl)
#define HI_ES8388_SET_RIGHT_DAC_VOLUME \
    _IOW(ES8388_IOC_MAGIC, ES8388_SET_RIGHT_DAC_VOLUME, hi_es8388_audio_ctrl)
#define HI_ES8388_SET_LFET_OUTPUT1_VOLUME \
    _IOW(ES8388_IOC_MAGIC, ES8388_SET_LFET_OUTPUT1_VOLUME, hi_es8388_audio_ctrl)
#define HI_ES8388_SET_LFET_OUTPUT2_VOLUME \
    _IOW(ES8388_IOC_MAGIC, ES8388_SET_LFET_OUTPUT2_VOLUME, hi_es8388_audio_ctrl)
#define HI_ES8388_SET_RIGHT_OUTPUT1_VOLUME \
    _IOW(ES8388_IOC_MAGIC, ES8388_SET_RIGHT_OUTPUT1_VOLUME, hi_es8388_audio_ctrl)
#define HI_ES8388_SET_RIGHT_OUTPUT2_VOLUME \
    _IOW(ES8388_IOC_MAGIC, ES8388_SET_RIGHT_OUTPUT2_VOLUME, hi_es8388_audio_ctrl)
#define HI_ES8388_SET_DAC_MUTE \
    _IOW(ES8388_IOC_MAGIC, ES8388_SET_DAC_MUTE, hi_es8388_audio_ctrl)

/* input power */
#define HI_ES8388_SET_LFET_INPUT_POWER \
    _IOW(ES8388_IOC_MAGIC, ES8388_SET_LFET_INPUT_POWER, hi_es8388_audio_ctrl)
#define HI_ES8388_SET_RIGHT_INPUT_POWER \
    _IOW(ES8388_IOC_MAGIC, ES8388_SET_RIGHT_INPUT_POWER, hi_es8388_audio_ctrl)
#define HI_ES8388_SET_LFET_ADC_POWER \
    _IOW(ES8388_IOC_MAGIC, ES8388_SET_LFET_ADC_POWER, hi_es8388_audio_ctrl)
#define HI_ES8388_SET_RIGHT_ADC_POWER \
    _IOW(ES8388_IOC_MAGIC, ES8388_SET_RIGHT_ADC_POWER, hi_es8388_audio_ctrl)

/* output power */
#define HI_ES8388_SET_LFET_DAC_POWER \
    _IOW(ES8388_IOC_MAGIC, ES8388_SET_LFET_DAC_POWER, hi_es8388_audio_ctrl)
#define HI_ES8388_SET_RIGHT_DAC_POWER \
    _IOW(ES8388_IOC_MAGIC, ES8388_SET_RIGHT_DAC_POWER, hi_es8388_audio_ctrl)
#define HI_ES8388_SET_LFET_OUTPUT1_ENABLE \
    _IOW(ES8388_IOC_MAGIC, ES8388_SET_LFET_OUTPUT1_ENABLE, hi_es8388_audio_ctrl)
#define HI_ES8388_SET_RIGHT_OUTPUT1_ENABLE \
    _IOW(ES8388_IOC_MAGIC, ES8388_SET_RIGHT_OUTPUT1_ENABLE, hi_es8388_audio_ctrl)
#define HI_ES8388_SET_LFET_OUTPUT2_ENABLE \
    _IOW(ES8388_IOC_MAGIC, ES8388_SET_LFET_OUTPUT2_ENABLE, hi_es8388_audio_ctrl)
#define HI_ES8388_SET_RIGHT_OUTPUT2_ENABLE \
    _IOW(ES8388_IOC_MAGIC, ES8388_SET_RIGHT_OUTPUT2_ENABLE, hi_es8388_audio_ctrl)

/* control */
#define HI_ES8388_SOFT_RESET \
    _IOW(ES8388_IOC_MAGIC, ES8388_SOFT_RESET, hi_es8388_audio_ctrl)
#define HI_ES8388_REG_DUMP \
    _IOW(ES8388_IOC_MAGIC, ES8388_REG_DUMP, hi_es8388_audio_ctrl)

#endif
