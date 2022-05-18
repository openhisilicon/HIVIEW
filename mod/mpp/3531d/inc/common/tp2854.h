#ifndef __TP2854B_H__
#define __TP2854B_H__

//#include <linux/ioctl.h>

#define TP2802_VERSION_CODE KERNEL_VERSION(0, 7, 8)

enum {
    TP2802_1080P25 = 0x03,
    TP2802_1080P30 = 0x02,
    TP2802_720P25 = 0x05,
    TP2802_720P30 = 0x04,
    TP2802_720P50 = 0x01,
    TP2802_720P60 = 0x00,
    TP2802_SD = 0x06,
    INVALID_FORMAT = 0x07,
    TP2802_720P25V2 = 0x0D,
    TP2802_720P30V2 = 0x0C,
    TP2802_PAL = 0x08,
    TP2802_NTSC = 0x09,
    TP2802_HALF1080P25 = 0x43,
    TP2802_HALF1080P30 = 0x42,
    TP2802_HALF720P25 = 0x45,
    TP2802_HALF720P30 = 0x44,
    TP2802_HALF720P50 = 0x41,
    TP2802_HALF720P60 = 0x40,
    TP2802_3M18 = 0x20,      // 2048x1536@18.75 for TVI
    TP2802_5M12 = 0x21,      // 2592x1944@12.5 for TVI
    TP2802_4M15 = 0x22,      // 2688x1520@15 for TVI
    TP2802_3M20 = 0x23,      // 2048x1536@20 for TVI
    TP2802_4M12 = 0x24,      // 2688x1520@12.5 for TVI
    TP2802_6M10 = 0x25,      // 3200x1800@10 for TVI
    TP2802_QHD30 = 0x26,     // 2560x1440@30 for TVI/HDA/HDC
    TP2802_QHD25 = 0x27,     // 2560x1440@25 for TVI/HDA/HDC
    TP2802_QHD15 = 0x28,     // 2560x1440@15 for HDA
    TP2802_QXGA18 = 0x29,    // 2048x1536@18 for HDA/TVI
    TP2802_QXGA30 = 0x2A,    // 2048x1536@30 for HDA
    TP2802_QXGA25 = 0x2B,    // 2048x1536@25 for HDA
    TP2802_4M30 = 0x2C,      // 2688x1520@30 for TVI(for future)
    TP2802_4M25 = 0x2D,      // 2688x1520@25 for TVI(for future)
    TP2802_5M20 = 0x2E,      // 2592x1944@20 for TVI/HDA
    TP2802_8M15 = 0x2f,      // 3840x2160@15 for TVI
    TP2802_8M12 = 0x30,      // 3840x2160@12.5 for TVI
    TP2802_1080P15 = 0x31,   // 1920x1080@15 for TVI
    TP2802_1080P60 = 0x32,   // 1920x1080@60 for TVI
    TP2802_960P30 = 0x33,    // 1280x960@30 for TVI
    TP2802_1080P20 = 0x34,   // 1920x1080@20 for TVI
    TP2802_1080P50 = 0x35,   // 1920x1080@50 for TVI
    TP2802_720P14 = 0x36,    // 1280x720@14 for TVI
    TP2802_720P30HDR = 0x37, // 1280x720@30 for TVI
    TP2802_6M20 = 0x38       // 2960x1920@20 for CVI
};
enum {
    VIDEO_UNPLUG,
    VIDEO_IN,
    VIDEO_LOCKED,
    VIDEO_UNLOCK
};
 enum{
    SDR_1CH,    //148.5M mode
    SDR_2CH,    //148.5M mode
    DDR_2CH,    //297M mode, support from TP2822/23
    DDR_4CH,    //297M mode, support from TP2824/33
    DDR_1CH,    //297M mode, support from TP2827
    MIPI_4CH4LANE_297M, //clock frquecny
    MIPI_4CH4LANE_594M,
    MIPI_2CH4LANE_297M, //clock frquecny
    MIPI_2CH4LANE_594M,
    MIPI_1CH4LANE_297M,
};
enum{
    CH_1=0,   //
    CH_2=1,    //
    CH_3=2,    //
    CH_4=3,     //
    CH_ALL=4,
    DATA_PAGE=5,
    AUDIO_PAGE=9,
    MIPI_PAGE=8,
    //VIDEO_PAGE=4,
};
enum {
    SCAN_DISABLE = 0,
    SCAN_AUTO,
    SCAN_TVI,
    SCAN_HDA,
    SCAN_HDC,
    SCAN_MANUAL,
    SCAN_TEST
};
enum {
    STD_TVI,
    STD_HDA,
    STD_HDC,
    STD_HDA_DEFAULT,
    STD_HDC_DEFAULT
};
enum {
    PTZ_TVI = 0,
    PTZ_HDA_1080P = 1,
    PTZ_HDA_720P = 2,
    PTZ_HDA_CVBS = 3,
    PTZ_HDC = 4,
    PTZ_HDA_3M18 = 5,  // HDA QXGA18
    PTZ_HDA_3M25 = 6,  // HDA QXGA25,QXGA30
    PTZ_HDA_4M25 = 7,  // HDA QHD25,QHD30,5M20
    PTZ_HDA_4M15 = 8,  // HDA QHD15,5M12.5
    PTZ_HDC_QHD = 9,   // HDC QHD25,QHD30
    PTZ_HDC_FIFO = 10, // HDC 1080p,720p FIFO
    PTZ_HDC_8M12 = 11, // HDC 8M12.5
    PTZ_HDC_8M15 = 12, // HDC 8M15
    PTZ_HDC_6M20 = 13  // HDC 6M20
};
enum {
    PTZ_RX_TVI_CMD,
    PTZ_RX_TVI_BURST,
    PTZ_RX_ACP1,
    PTZ_RX_ACP2,
    PTZ_RX_ACP3,
    PTZ_RX_TEST,
    PTZ_RX_HDC1,
    PTZ_RX_HDC2
};
enum { // Added AUDIO detection enum value - 10/16/2018
    AUDIO_NO_DETECTED,
    AUDIO_DETECTED
};
#define FLAG_LOSS 0x80
#define FLAG_H_LOCKED 0x20
#define FLAG_HV_LOCKED 0x60

#define FLAG_HALF_MODE 0x40
#define FLAG_MEGA_MODE 0x20

#define CHANNELS_PER_CHIP 4
#define MAX_CHIPS 8
#define SUCCESS 0
#define FAILURE (-1)

#define BRIGHTNESS 0x10
#define CONTRAST 0x11
#define SATURATION 0x12
#define HUE 0X13
#define SHARPNESS 0X14


#define CXSEL_REG7B 0x7B // Added CXSEL - 10/16/2018

#define MAX_COUNT 0xffff

typedef struct _tp2802_register {
    unsigned char chip;
    unsigned char ch;
    unsigned int reg_addr;
    unsigned int value;
} tp2802_register;

typedef struct _tp2802_work_mode {
    unsigned char chip;
    unsigned char ch;
    unsigned char mode;
} tp2802_work_mode;

typedef struct _tp2802_video_mode {
    unsigned char chip;
    unsigned char ch;
    unsigned char mode;
    unsigned char std;
    unsigned char output;
} tp2802_video_mode;

typedef struct _tp2802_video_loss {
    unsigned char chip;
    unsigned char ch;
    unsigned char is_lost;
} tp2802_video_loss;

typedef struct _tp2802_image_adjust {
    unsigned char chip;
    unsigned char ch;
    unsigned int hue;
    unsigned int contrast;
    unsigned int brightness;
    unsigned int saturation;
    unsigned int sharpness;
} tp2802_image_adjust;

typedef struct _tp2802_PTZ_data {
    unsigned char chip;
    unsigned char ch;
    unsigned char mode;
    unsigned char data[16];
} tp2802_PTZ_data;

typedef enum _tp2802_audio_samplerate {
    SAMPLE_RATE_8000,
    SAMPLE_RATE_16000,
} tp2802_audio_samplerate;

typedef struct _tp2802_audio_playback {
    unsigned int chip;
    unsigned int chn;
} tp2802_audio_playback;

typedef struct _tp2802_audio_da_volume {
    unsigned int chip;
    unsigned int volume;
} tp2802_audio_da_volume;

typedef struct _tp2802_audio_da_mute {
    unsigned int chip;
    unsigned int flag;
} tp2802_audio_da_mute;

typedef struct _tp2833_audio_format {
    unsigned int chip;
    unsigned int chn;
    unsigned int format;    /* 0:i2s; 1:dsp */
    unsigned int mode;      /* 0:slave 1:master */
    unsigned int clkdir;    /* 0:inverted;1:non-inverted */
    unsigned int bitrate;   /* 0:256fs 1:320fs */
    unsigned int precision; /* 0:16bit;1:8bit */
} tp2833_audio_format;

typedef struct _tp2830_audio_coax_config { // Added audio coax configuration - 10/16/2018
    unsigned int chip;                     // set chip
    unsigned int chn;                      // set input channel
    unsigned int start_line;               // set audio coax decoding starting line control.
    unsigned int position;                 // set audio position relative to the sync.
    unsigned int length;                   // set audio sample width control.
    unsigned int threshold;                // set audio
} tp2830_audio_coax_config;

typedef struct _tp2830_audio_coax_channel { // Added audio coax channel selection - 10/16/2018
    unsigned int chip;                       // set chip
    unsigned int chn;                       // set input channel
    unsigned int input_type;                // set audio input type ( 0 : AIN base-band audio, 1 : Coaxial audio )
} tp2830_audio_coax_channel;

typedef struct _tp2830_audio_coax_detect { // Added audio coax detection status - 10/16/2018
    unsigned int chip;                     // set chip
    unsigned int chn;                      // set input channel
    unsigned int is_detected;              // check audio coax detection status
} tp2830_audio_coax_detect;

// IOCTL Definitions
#define TP2802_IOC_MAGIC 'v'
#define TP2802_SET_VIDEO_MODE _IOW(TP2802_IOC_MAGIC, 3, tp2802_video_mode)
#define TP2802_GET_VIDEO_MODE _IOWR(TP2802_IOC_MAGIC, 4, tp2802_video_mode)
#endif
