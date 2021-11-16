#ifndef __STREAM_H__
#define __STREAM_H__

#include "uvc_gadgete.h"
#include "sample_venc.h"

#ifndef __HuaweiLite__
#include "uvc.h"
#endif

//USB_Video_Payload_H_264_1.0 3.3
#define UVCX_PICTURE_TYPE_CONTROL 0x09

// H264 extension unit id, must same as to driver
#define UNIT_XU_H264 10

// HICAMERA
#define UNIT_XU_HICAMERA (0x11)

typedef struct stream_control_ops {
    int (*init)(void);
    int (*startup)(void);
    int (*shutdown)(void);
    int (*set_idr)(void);
    int (*set_property)(struct encoder_property *p);
} stream_control_ops_st;

typedef struct processing_unit_ops {
    uint16_t (*brightness_get)(void);
    uint16_t (*contrast_get)(void);
    uint16_t (*hue_get)(void);
    uint8_t (*power_line_frequency_get)(void);
    uint16_t (*saturation_get)(void);
    uint8_t (*white_balance_temperature_auto_get)(void);
    uint16_t (*white_balance_temperature_get)(void);

    void (*brightness_set)(uint16_t v);
    void (*contrast_set)(uint16_t v);
    void (*hue_set)(uint16_t v);
    void (*power_line_frequency_set)(uint8_t v);
    void (*saturation_set)(uint16_t v);
    void (*white_balance_temperature_auto_set)(uint8_t v);
    void (*white_balance_temperature_set)(uint16_t v);
} processing_unit_ops_st;

typedef struct input_terminal_ops {
    uint32_t (*exposure_ansolute_time_get)(void);
    uint8_t (*exposure_auto_mode_get)(void);
    void (*exposure_ansolute_time_set)(uint32_t v);
    void (*exposure_auto_mode_set)(uint8_t v);
} input_terminal_ops_st;

typedef struct extension_unit_ops {
    /*todo eu-h264 ops*/

    /*todo eu-camera ops*/
} extension_unit_ops_st;

typedef struct histream
{
    struct stream_control_ops *mpi_sc_ops;
    struct processing_unit_ops *mpi_pu_ops;
    struct input_terminal_ops *mpi_it_ops;
    struct extension_unit_ops *mpi_eu_ops;
    int streaming;
    int exposure_auto_stall;
    int brightness_stall;
} histream;

/* media control functions */
extern int histream_register_mpi_ops(struct stream_control_ops *sc_ops,
        struct processing_unit_ops *pu_ops,
        struct input_terminal_ops *it_ops,
        struct extension_unit_ops *eu_ops);

extern int histream_set_enc_property(struct encoder_property *p);
extern int histream_init(void);
extern int histream_shutdown(void);
extern int histream_startup(void);
extern int histream_set_enc_idr(void);

#ifndef __HuaweiLite__
extern void histream_event_pu_control(struct uvc_device *dev,
                               uint8_t req,
                               uint8_t unit_id,
                               uint8_t cs,
                               struct uvc_request_data* resp);

extern void histream_event_it_control(struct uvc_device *dev,
                               uint8_t req,
                               uint8_t unit_id,
                               uint8_t cs,
                               struct uvc_request_data *resp);

extern void histream_event_eu_h264_control(uint8_t req,
                                   uint8_t unit_id,
                                   uint8_t cs,
                                   struct uvc_request_data *resp);

extern void histream_event_eu_camera_control(uint8_t req,
                                       uint8_t unit_id,
                                       uint8_t cs,
                                       struct uvc_request_data *resp);

extern void histream_event_eu_h264_data(int unit_id, int control, struct uvc_request_data *data);
extern void histream_event_pu_data(int unit_id, int control, struct uvc_request_data *data);
extern void histream_event_it_data(struct uvc_device *dev, int unit_id, int control, struct uvc_request_data *data);
extern void histream_event_eu_camera_data(int unit_id, int control, struct uvc_request_data *data);
#endif

#endif //__STREAM_H__

