#ifndef __HI_CTRL_H__
#define __HI_CTRL_H__

// CTRL id
#define HI_XUID_SET_RESET 0x01
#define HI_XUID_SET_STREAM 0x02
#define HI_XUID_SET_RESOLUTION 0x03
#define HI_XUID_SET_IFRAME 0x04
#define HI_XUID_SET_BITRATE 0x05
#define HI_XUID_UPDATE_SYSTEM 0x06

typedef struct _uvcx_base_ctrl_t
{
    uint16_t selector;
    uint16_t index;
} __attribute__ ((__packed__)) uvcx_base_ctrl_t;


typedef struct uvcx_camera_stream_t
{
    uint16_t format;
    uint16_t resolution;
} __attribute__ ((__packed__)) uvcx_camera_stream_t;


#endif //__HI_CTRL_H__
