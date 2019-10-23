#ifndef _ACODEC_H_
#define _ACODEC_H_

#define IOC_TYPE_ACODEC 'A'

typedef enum hiACODEC_FS_E
{
    ACODEC_FS_48000 =   0x2,
    ACODEC_FS_24000 =   0x1,
    ACODEC_FS_12000 =   0x0,

    ACODEC_FS_44100 =   0x2,
    ACODEC_FS_22050 =   0x1,
    ACODEC_FS_11025 =   0x0,

    ACODEC_FS_32000 =   0x2,
    ACODEC_FS_16000 =   0x1,
    ACODEC_FS_8000  =   0x0,

    ACODEC_FS_64000 =   0x3,

    ACODEC_FS_96000 =   0x3,

    ACODEC_FS_BUTT = 0x5,
} ACODEC_FS_E;

typedef struct
{
    int vol_ctrl;
    unsigned int vol_ctrl_mute;
} ACODEC_VOL_CTRL;

typedef enum hiACODEC_IOCTL_E
{
    IOC_NR_SOFT_RESET_CTRL = 0x0,
    IOC_NR_SET_OUTPUT_VOL,
    IOC_NR_GET_OUTPUT_VOL,
    /***********************/
    IOC_NR_SET_I2S1_FS,

    IOC_NR_SET_DACL_VOL,
    IOC_NR_SET_DACR_VOL,

    IOC_NR_SET_DACL_MUTE,
    IOC_NR_SET_DACR_MUTE,

    IOC_NR_GET_DACL_VOL,
    IOC_NR_GET_DACR_VOL,

    IOC_NR_SET_PD_DACL,
    IOC_NR_SET_PD_DACR,
} ACODEC_IOCTL_E;

/*reset the audio code to the default config*/
#define ACODEC_SOFT_RESET_CTRL \
    _IO(IOC_TYPE_ACODEC, IOC_NR_SOFT_RESET_CTRL)
/*ACODEC_FS_E*/
#define ACODEC_SET_I2S1_FS \
    _IOWR(IOC_TYPE_ACODEC, IOC_NR_SET_I2S1_FS, unsigned int)

/*Output volume control(left channel) ACODEC_VOL_CTRL*/
#define ACODEC_SET_DACL_VOL \
    _IOWR(IOC_TYPE_ACODEC, IOC_NR_SET_DACL_VOL, ACODEC_VOL_CTRL)
/*Output volume control(right channel) ACODEC_VOL_CTRL*/
#define ACODEC_SET_DACR_VOL \
    _IOWR(IOC_TYPE_ACODEC, IOC_NR_SET_DACR_VOL, ACODEC_VOL_CTRL)

/*Output mute control(left channel), 1:mute, 0:unmute*/
#define ACODEC_SET_DACL_MUTE \
    _IOWR(IOC_TYPE_ACODEC, IOC_NR_SET_DACL_MUTE, unsigned int)
/*Output mute control(right channel), 1:mute, 0:unmute*/
#define ACODEC_SET_DACR_MUTE \
    _IOWR(IOC_TYPE_ACODEC, IOC_NR_SET_DACR_MUTE, unsigned int)

#define ACODEC_GET_DACL_VOL \
    _IOWR(IOC_TYPE_ACODEC, IOC_NR_GET_DACL_VOL, ACODEC_VOL_CTRL)
#define ACODEC_GET_DACR_VOL \
    _IOWR(IOC_TYPE_ACODEC, IOC_NR_GET_DACR_VOL, ACODEC_VOL_CTRL)

/*set adcl power, 0: power up, 1: power down*/
#define  ACODEC_SET_PD_DACL \
    _IOWR(IOC_TYPE_ACODEC, IOC_NR_SET_PD_DACL, unsigned int)
/*set adcr power, 0: power up, 1: power down*/
#define  ACODEC_SET_PD_DACR \
    _IOWR(IOC_TYPE_ACODEC, IOC_NR_SET_PD_DACR, unsigned int)

/* Don't need to set, the driver will set a default value */
/*clock of dac and adc is reverse or obverse*/
#define ACODEC_SET_OUTPUT_VOL \
    _IOWR(IOC_TYPE_ACODEC, IOC_NR_SET_OUTPUT_VOL, unsigned int)
#define ACODEC_GET_OUTPUT_VOL \
    _IOWR(IOC_TYPE_ACODEC, IOC_NR_GET_OUTPUT_VOL, unsigned int)


#endif /* End of #ifndef _ACODEC_H_ */
