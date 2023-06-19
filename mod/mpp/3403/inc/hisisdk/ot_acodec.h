/*
  Copyright (c), 2001-2022, Shenshu Tech. Co., Ltd.
 */

#ifndef OT_ACODEC_H
#define OT_ACODEC_H
#include "ot_common.h"

#define IOC_TYPE_ACODEC 'A'

typedef enum {
    OT_ACODEC_FS_8000  = 0x1,
    OT_ACODEC_FS_11025 = 0x2,
    OT_ACODEC_FS_12000 = 0x3,
    OT_ACODEC_FS_16000 = 0x4,
    OT_ACODEC_FS_22050 = 0x5,
    OT_ACODEC_FS_24000 = 0x6,
    OT_ACODEC_FS_32000 = 0x7,
    OT_ACODEC_FS_44100 = 0x8,
    OT_ACODEC_FS_48000 = 0x9,
    OT_ACODEC_FS_64000 = 0xa,
    OT_ACODEC_FS_96000 = 0xb,
    OT_ACODEC_FS_BUTT = 0xc,
} ot_acodec_fs;

typedef enum {
    OT_ACODEC_MIXER_IN0   = 0x0,
    OT_ACODEC_MIXER_IN1   = 0x1,
    OT_ACODEC_MIXER_IN_D  = 0x2,
    OT_ACODEC_MIXER_BUTT,
} ot_acodec_mixer;

typedef struct {
    /* volume control, 0x00~0x7e, 0x7F:mute */
    td_u32 volume_ctrl;
    /* adc/dac mute control, 1:mute, 0:unmute */
    td_u32 volume_ctrl_mute;
} ot_acodec_volume_ctrl;

typedef enum {
    IOC_NR_SOFT_RESET_CTRL = 0x0,
    IOC_NR_SET_I2S1_FS,
    IOC_NR_SET_MIXER_MIC,

    /* input/output volume */
    IOC_NR_SET_INPUT_VOL,
    IOC_NR_SET_OUTPUT_VOL,
    IOC_NR_GET_INPUT_VOL,
    IOC_NR_GET_OUTPUT_VOL,

    /* analog part input gain */
    IOC_NR_BOOSTL_ENABLE,
    IOC_NR_BOOSTR_ENABLE,
    IOC_NR_SET_GAIN_MICL,
    IOC_NR_SET_GAIN_MICR,
    IOC_NR_GET_GAIN_MICL,
    IOC_NR_GET_GAIN_MICR,

    /* ADC/DAC volume */
    IOC_NR_SET_DACL_VOL,
    IOC_NR_SET_DACR_VOL,
    IOC_NR_SET_ADCL_VOL,
    IOC_NR_SET_ADCR_VOL,
    IOC_NR_GET_DACL_VOL,
    IOC_NR_GET_DACR_VOL,
    IOC_NR_GET_ADCL_VOL,
    IOC_NR_GET_ADCR_VOL,

    /* input/output mute */
    IOC_NR_SET_MICL_MUTE,
    IOC_NR_SET_MICR_MUTE,
    IOC_NR_SET_DACL_MUTE,
    IOC_NR_SET_DACR_MUTE,

    /* ADC/DAC power down */
    IOC_NR_SET_PD_DACL,
    IOC_NR_SET_PD_DACR,
    IOC_NR_SET_PD_ADCL,
    IOC_NR_SET_PD_ADCR,
    IOC_NR_SET_PD_LINEINL,
    IOC_NR_SET_PD_LINEINR,

    /* ADC/DAC clk */
    IOC_NR_SEL_DAC_CLK,
    IOC_NR_SEL_ADC_CLK,
    IOC_NR_SEL_ANA_MCLK,

    /* ADC/DAC track */
    IOC_NR_DACL_SEL_TRACK,
    IOC_NR_DACR_SEL_TRACK,
    IOC_NR_ADCL_SEL_TRACK,
    IOC_NR_ADCR_SEL_TRACK,

    IOC_NR_SET_DAC_DE_EMPHASIS,
    IOC_NR_SET_ADC_HP_FILTER,
    IOC_NR_SET_I2S1_DATAWIDTH,

    /* soft mute */
    IOC_NR_DAC_SOFT_MUTE,
    IOC_NR_DAC_SOFT_UNMUTE,
    IOC_NR_DAC_SOFT_MUTE_RATE,
} acodec_ioc;

/* reset the audio code to the default config */
#define OT_ACODEC_SOFT_RESET_CTRL \
    _IO(IOC_TYPE_ACODEC, IOC_NR_SOFT_RESET_CTRL)

/* set I2S1 sample rate */
#define OT_ACODEC_SET_I2S1_FS \
    _IOWR(IOC_TYPE_ACODEC, IOC_NR_SET_I2S1_FS, td_u32)

/* select the micpga's input, micin linein, or differential input(ot_acodec_mixer) */
#define OT_ACODEC_SET_MIXER_MIC \
    _IOWR(IOC_TYPE_ACODEC, IOC_NR_SET_MIXER_MIC, td_u32)

/* input/output volume control(both channel) */
#define OT_ACODEC_SET_INPUT_VOLUME \
    _IOWR(IOC_TYPE_ACODEC, IOC_NR_SET_INPUT_VOL, td_u32)
#define OT_ACODEC_SET_OUTPUT_VOLUME \
    _IOWR(IOC_TYPE_ACODEC, IOC_NR_SET_OUTPUT_VOL, td_u32)
#define OT_ACODEC_GET_INPUT_VOLUME \
    _IOWR(IOC_TYPE_ACODEC, IOC_NR_GET_INPUT_VOL, td_u32)
#define OT_ACODEC_GET_OUTPUT_VOLUME \
    _IOWR(IOC_TYPE_ACODEC, IOC_NR_GET_OUTPUT_VOL, td_u32)

/* analog part input boost control(left channel), 1:on, 0:off */
#define OT_ACODEC_ENABLE_BOOSTL \
    _IOWR(IOC_TYPE_ACODEC, IOC_NR_BOOSTL_ENABLE, td_u32)
/* analog part input boost control(right channel), 1:on, 0:off */
#define OT_ACODEC_ENABLE_BOOSTR \
    _IOWR(IOC_TYPE_ACODEC, IOC_NR_BOOSTR_ENABLE, td_u32)
/* analog part input gain control(left channel 0~0x1f) */
#define OT_ACODEC_SET_GAIN_MICL \
    _IOWR(IOC_TYPE_ACODEC, IOC_NR_SET_GAIN_MICL, td_u32)
/* analog part input gain control(right channel 0~0x1f) */
#define OT_ACODEC_SET_GAIN_MICR \
    _IOWR(IOC_TYPE_ACODEC, IOC_NR_SET_GAIN_MICR, td_u32)
/* get analog part input gain(left channel) */
#define OT_ACODEC_GET_GAIN_MICL \
    _IOWR(IOC_TYPE_ACODEC, IOC_NR_GET_GAIN_MICL, td_u32)
/* get analog part input gain(right channel) */
#define OT_ACODEC_GET_GAIN_MICR \
    _IOWR(IOC_TYPE_ACODEC, IOC_NR_GET_GAIN_MICR, td_u32)

/* DAC volume control(left channel) ot_acodec_volume_ctrl */
#define OT_ACODEC_SET_DACL_VOLUME \
    _IOWR(IOC_TYPE_ACODEC, IOC_NR_SET_DACL_VOL, ot_acodec_volume_ctrl)
/* DAC volume control(right channel) ot_acodec_volume_ctrl */
#define OT_ACODEC_SET_DACR_VOLUME \
    _IOWR(IOC_TYPE_ACODEC, IOC_NR_SET_DACR_VOL, ot_acodec_volume_ctrl)
/* ADC volume control(left channel) ot_acodec_volume_ctrl */
#define OT_ACODEC_SET_ADCL_VOLUME \
    _IOWR(IOC_TYPE_ACODEC, IOC_NR_SET_ADCL_VOL, ot_acodec_volume_ctrl)
/* ADC volume control(right channel) ot_acodec_volume_ctrl */
#define OT_ACODEC_SET_ADCR_VOLUME \
    _IOWR(IOC_TYPE_ACODEC, IOC_NR_SET_ADCR_VOL, ot_acodec_volume_ctrl)

/* get DAC volume(left channel) */
#define OT_ACODEC_GET_DACL_VOLUME \
    _IOWR(IOC_TYPE_ACODEC, IOC_NR_GET_DACL_VOL, ot_acodec_volume_ctrl)
/* get DAC volume(right channel) */
#define OT_ACODEC_GET_DACR_VOLUME \
    _IOWR(IOC_TYPE_ACODEC, IOC_NR_GET_DACR_VOL, ot_acodec_volume_ctrl)
/* get ADC volume(left channel) */
#define OT_ACODEC_GET_ADCL_VOLUME \
    _IOWR(IOC_TYPE_ACODEC, IOC_NR_GET_ADCL_VOL, ot_acodec_volume_ctrl)
/* get ADC volume(right channel) */
#define OT_ACODEC_GET_ADCR_VOLUME \
    _IOWR(IOC_TYPE_ACODEC, IOC_NR_GET_ADCR_VOL, ot_acodec_volume_ctrl)

/* Input mute control(left channel), 1:mute, 0:unmute */
#define OT_ACODEC_SET_MICL_MUTE \
    _IOWR(IOC_TYPE_ACODEC, IOC_NR_SET_MICL_MUTE, td_u32)
/* Input mute control(right channel), 1:mute, 0:unmute */
#define OT_ACODEC_SET_MICR_MUTE \
    _IOWR(IOC_TYPE_ACODEC, IOC_NR_SET_MICR_MUTE, td_u32)
/* Output mute control(left channel), 1:mute, 0:unmute */
#define OT_ACODEC_SET_DACL_MUTE \
    _IOWR(IOC_TYPE_ACODEC, IOC_NR_SET_DACL_MUTE, td_u32)
/* Output mute control(right channel), 1:mute, 0:unmute */
#define OT_ACODEC_SET_DACR_MUTE \
    _IOWR(IOC_TYPE_ACODEC, IOC_NR_SET_DACR_MUTE, td_u32)

/* set dacl power, 0: power up, 1: power down */
#define OT_ACODEC_SET_PD_DACL \
    _IOWR(IOC_TYPE_ACODEC, IOC_NR_SET_PD_DACL, td_u32)
/* set dacr power, 0: power up, 1: power down */
#define OT_ACODEC_SET_PD_DACR \
    _IOWR(IOC_TYPE_ACODEC, IOC_NR_SET_PD_DACR, td_u32)
/* set adcl power, 0: power up, 1: power down */
#define OT_ACODEC_SET_PD_ADCL \
    _IOWR(IOC_TYPE_ACODEC, IOC_NR_SET_PD_ADCL, td_u32)
/* set adcr power, 0: power up, 1: power down */
#define OT_ACODEC_SET_PD_ADCR \
    _IOWR(IOC_TYPE_ACODEC, IOC_NR_SET_PD_ADCR, td_u32)
/* set analog part input power, 0: power up, 1: power down */
#define OT_ACODEC_SET_PD_LINEINL \
    _IOWR(IOC_TYPE_ACODEC, IOC_NR_SET_PD_LINEINL, td_u32)
/* set analog part input power, 0: power up, 1: power down */
#define OT_ACODEC_SET_PD_LINEINR \
    _IOWR(IOC_TYPE_ACODEC, IOC_NR_SET_PD_LINEINR, td_u32)

/* clock of dac and adc is reverse or obverse */
#define OT_ACODEC_SELECT_DAC_CLK \
    _IOWR(IOC_TYPE_ACODEC, IOC_NR_SEL_DAC_CLK, td_u32)
#define OT_ACODEC_SELECT_ADC_CLK \
    _IOWR(IOC_TYPE_ACODEC, IOC_NR_SEL_ADC_CLK, td_u32)

/* clock of analog part and digital part is reverse or obverse */
#define OT_ACODEC_SELECT_ANA_MCLK \
    _IOWR(IOC_TYPE_ACODEC, IOC_NR_SEL_ANA_MCLK, td_u32)

/* ADC/DAC track control */
#define OT_ACODEC_DACL_SELECT_TRACK \
    _IOWR(IOC_TYPE_ACODEC, IOC_NR_DACL_SEL_TRACK, td_u32)
#define OT_ACODEC_DACR_SELECT_TRACK \
    _IOWR(IOC_TYPE_ACODEC, IOC_NR_DACR_SEL_TRACK, td_u32)
#define OT_ACODEC_ADCL_SELECT_TRACK \
    _IOWR(IOC_TYPE_ACODEC, IOC_NR_ADCL_SEL_TRACK, td_u32)
#define OT_ACODEC_ADCR_SELECT_TRACK \
    _IOWR(IOC_TYPE_ACODEC, IOC_NR_ADCR_SEL_TRACK, td_u32)

#define OT_ACODEC_SET_DAC_DE_EMPHASIS \
    _IOWR(IOC_TYPE_ACODEC, IOC_NR_SET_DAC_DE_EMPHASIS, td_u32)
#define OT_ACODEC_SET_ADC_HP_FILTER \
    _IOWR(IOC_TYPE_ACODEC, IOC_NR_SET_ADC_HP_FILTER, td_u32)
#define OT_ACODEC_SET_I2S1_DATAWIDTH \
    _IOWR(IOC_TYPE_ACODEC, IOC_NR_SET_I2S1_DATAWIDTH, td_u32)

/* DAC soft mute control, 1:enable, 0:disable */
#define OT_ACODEC_DAC_SOFT_MUTE \
    _IOWR(IOC_TYPE_ACODEC, IOC_NR_DAC_SOFT_MUTE, td_u32)
/* DAC soft unmute control, 1:enable, 0:disable */
#define OT_ACODEC_DAC_SOFT_UNMUTE \
    _IOWR(IOC_TYPE_ACODEC, IOC_NR_DAC_SOFT_UNMUTE, td_u32)
/* set DAC soft mute rate, 0:fs/2, 1:fs/8, 2:fs/32, 3:fs/64 */
#define OT_ACODEC_DAC_SOFT_MUTE_RATE \
    _IOWR(IOC_TYPE_ACODEC, IOC_NR_DAC_SOFT_MUTE_RATE, td_u32)

#endif
