/*
 * uvc interface
 */

#ifndef __UVC_GADGETE_H__
#define __UVC_GADGETE_H__
#include "linux_uvc.h"
#include "video.h"
#include <stdint.h>

typedef struct uvc_probe_t
{
    unsigned char set;
    unsigned char get;
    unsigned char max;
    unsigned char min;
} uvc_probe_t;

struct uvc_device
{
    int fd;
    struct uvc_streaming_control probe;
    struct uvc_streaming_control commit;
    int control;
    int unit_id;
    int interface_id;
    unsigned int fcc;
    unsigned int width;
    unsigned int height;
    unsigned int nbufs;
    unsigned int bulk;
    uint8_t      color;
    unsigned int imgsize;
    unsigned int bulk_size;
    uvc_probe_t  probe_status;
    int streaming;

    /* USB speed specific */
    int mult;
    int burst;
    int maxpkt;
    enum usb_device_speed speed;

    struct uvc_request_data request_error_code;
};

int open_uvc_device();
int close_uvc_device();
int run_uvc_device();
int run_uvc_data();

#endif //__UVC_GADGETE_H__
