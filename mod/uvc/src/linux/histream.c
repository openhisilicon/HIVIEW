#include <errno.h>

#include "histream.h"
#include "frame_cache.h"
#include "uvc_gadgete.h"

#ifndef __HuaweiLite__
#include "log.h"
#include "hi_ctrl.h"
#include "config_svc.h"
#include "video.h"
#endif

static struct histream __hi_stream = {
    .mpi_sc_ops = NULL,
    .mpi_pu_ops = NULL,
    .mpi_it_ops = NULL,
    .mpi_eu_ops = NULL,
    .streaming = 0,
    .exposure_auto_stall = 0,
    .brightness_stall = 0,
};

static histream *get_histream(void)
{
    return &__hi_stream;
}

/* Histream Stream Control Operation Functions End */

#ifndef __HuaweiLite__
static void _histream_event_default_control(uint8_t req, struct uvc_request_data *resp)
{
    switch (req)
    {
    case UVC_GET_MIN:
        resp->length = 4;
        break;
    case UVC_GET_LEN:
        resp->data[0] = 0x04;
        resp->data[1] = 0x00;
        resp->length = 2;
        break;
    case UVC_GET_INFO:
        resp->data[0] = 0x03;
        resp->length = 1;
        break;
    case UVC_GET_CUR:
        resp->length = 1;
        resp->data[0] = 0x01;
        break;
    default:
        resp->length = 1;
        resp->data[0] = 0x06;
        break;
    }
}

/*******************************************************
 *	Histream Processing Unit Operation Functions
 *******************************************************/
static void _histream_pu_brightness_ctrl(struct uvc_device *dev, uint8_t req, struct uvc_request_data *resp)
{
    uint16_t v = 50;

    if (get_histream()->streaming == 1)
    {
        if (get_histream()->mpi_pu_ops &&
            get_histream()->mpi_pu_ops->brightness_get)
            v = get_histream()->mpi_pu_ops->brightness_get();
    }

    switch (req)
    {
        case UVC_SET_CUR:
            resp->length = 2;
            //RLOG("brightness_control set cur	setup\n");
            dev->request_error_code.data[0] = 0x00;
            dev->request_error_code.length = 1;
            break;
        case UVC_GET_CUR:
            if(get_histream()->brightness_stall)
            {
                resp->length = -EL2HLT;
                dev->request_error_code.data[0] = 0x04;
                dev->request_error_code.length = 1;
                get_histream()->brightness_stall = 0;
            }else{
                resp->length = 2;
                resp->data[0] = (v & 0xff);
                resp->data[1] = ((v >> 8) & 0xff);
                dev->request_error_code.data[0] = 0x00;
                dev->request_error_code.length = 1;
                //RLOG("brightness_control get cur %d\n",stCSCAttr.u32LumaVal);
            }
            break;
        case UVC_GET_MIN:
            resp->length = 2;
            resp->data[0] = 0x00;
            //resp->data[1] = 0x00;
            dev->request_error_code.data[0] = 0x00;
            dev->request_error_code.length = 1;
            //RLOG("brightness_control get min %d \n",0);
            break;
        case UVC_GET_MAX:
            resp->length = 2;
            resp->data[0] = 0x64;
            dev->request_error_code.data[0] = 0x00;
            dev->request_error_code.length = 1;
            //RLOG("brightness_control get max %d \n",100);
            break;
        case UVC_GET_DEF:
            resp->length = 2;
            resp->data[0] = 0x32;
            //resp->data[1] = 0x00;
            dev->request_error_code.data[0] = 0x00;
            dev->request_error_code.length = 1;
            //RLOG("brightness_control get max %d \n",100);
            break;
        case UVC_GET_RES:
            resp->data[0] = 0x01;
            //resp->data[1] = 0x00;
            resp->length = 2;
            //LOG("GET_LEN for UVCX_PICRURE_TYPE_CONTROL\n");
            dev->request_error_code.data[0] = 0x00;
            dev->request_error_code.length = 1;
            break;
        case UVC_GET_INFO:
            resp->data[0] = 0x03;
            resp->length = 1;
            dev->request_error_code.data[0] = 0x00;
            dev->request_error_code.length = 1;
            //LOG("get info\n");
            break;
        default:
            resp->length = -EL2HLT;
            dev->request_error_code.data[0] = 0x07;
            dev->request_error_code.length = 1;
            break;
    }
}

static void _histream_pu_brightness_set(struct uvc_request_data *data)
{
    uint16_t v = 0;

    v = data->data[0] + (data->data[1] << 8);
    if (v > 0x64 || v < 0)
    {
        get_histream()->brightness_stall = 1;
    }

    if (get_histream()->mpi_pu_ops &&
        get_histream()->mpi_pu_ops->brightness_set)
    {
        get_histream()->mpi_pu_ops->brightness_set(v);
    }
}

void _histream_pu_contrast_ctrl(struct uvc_device *dev, uint8_t req, struct uvc_request_data *resp)
{
#if 0
    uint16_t v = 50;

    if (get_histream()->streaming == 1)
    {
        if (get_histream()->mpi_pu_ops &&
            get_histream()->mpi_pu_ops->contrast_get)
            v = get_histream()->mpi_pu_ops->contrast_get();
    }
#endif

    switch (req)
    {
        case UVC_SET_CUR:
            resp->length = 2;
            //RLOG("brightness_control set cur  setup\n");
            break;
        case UVC_GET_CUR:
            //resp->length = 2;
            //resp->data[0] = (v & 0xff);
            //resp->data[1] = ((v >> 8) & 0xff);
            dev->request_error_code.data[0] = 0x06;
            dev->request_error_code.length = 1;
            //RLOG("brightness_control get cur %d\n",stCSCAttr.u32LumaVal);
            break;
        case UVC_GET_MIN:
            resp->length = 2;
            resp->data[0] = 0x00;
            resp->data[1] = 0x00;
            //RLOG("brightness_control get min %d \n",0);
            break;
        case UVC_GET_MAX:
            resp->length = 2;
            resp->data[0] = 0x64;
            resp->data[1] = 0x00;
            //RLOG("brightness_control get max %d \n",100);
            break;
        case UVC_GET_LEN:
            resp->data[0] = 0x02;
            resp->data[1] = 0x00;
            resp->length = 2;
            //LOG("GET_LEN for UVCX_PICRURE_TYPE_CONTROL\n");
            break;
        case UVC_GET_INFO:
            resp->data[0] = 0x03;
            resp->length = 1;
            //LOG("get info\n");
            break;
        default:
            resp->length = -EL2HLT;
            dev->request_error_code.data[0] = 0x07;
            dev->request_error_code.length = 1;
            break;
    }
}

static void _histream_pu_contrast_set(struct uvc_request_data *data)
{
    uint16_t v = 0;

    if (get_histream()->streaming != 1)
    {
        return;
    }

    v = data->data[0] + (data->data[1] << 8);

    if (get_histream()->mpi_pu_ops &&
        get_histream()->mpi_pu_ops->contrast_set)
    {
        get_histream()->mpi_pu_ops->contrast_set(v);
    }
}

void _histream_pu_hue_ctrl(struct uvc_device *dev, uint8_t req, struct uvc_request_data *resp)
{
#if 0
    uint16_t v = 50;

    if (get_histream()->streaming == 1)
    {
        if (get_histream()->mpi_pu_ops &&
            get_histream()->mpi_pu_ops->hue_get)
            v = get_histream()->mpi_pu_ops->hue_get();
    }
#endif

    switch (req)
    {
        case UVC_SET_CUR:
            resp->length = 2;
            //RLOG("brightness_control set cur  setup\n");
            break;
        case UVC_GET_CUR:
            //resp->length = 2;
            //resp->data[0] = (v & 0xff);
            //resp->data[1] = ((v >> 8) & 0xff);
            dev->request_error_code.data[0] = 0x06;
            dev->request_error_code.length = 1;
            //RLOG("brightness_control get cur %d\n",stCSCAttr.u32LumaVal);
            break;
        case UVC_GET_MIN:
            resp->length = 2;
            resp->data[0] = 0x00;
            resp->data[1] = 0x00;
            //RLOG("brightness_control get min %d \n",0);
            break;
        case UVC_GET_MAX:
            resp->length = 2;
            resp->data[0] = 0x64;
            resp->data[1] = 0x00;
            //RLOG("brightness_control get max %d \n",100);
            break;
        case UVC_GET_LEN:
            resp->data[0] = 0x02;
            resp->data[1] = 0x00;
            resp->length = 2;
            //LOG("GET_LEN for UVCX_PICRURE_TYPE_CONTROL\n");
            break;
        case UVC_GET_INFO:
            resp->data[0] = 0x03;
            resp->length = 1;
            //LOG("get info\n");
            break;
        default:
            resp->length = -EL2HLT;
            dev->request_error_code.data[0] = 0x07;
            dev->request_error_code.length = 1;
            break;
    }
}

static void _histream_pu_hue_set(struct uvc_request_data *data)
{
    uint16_t v = 0;

    if (get_histream()->streaming != 1)
    {
        return;
    }

    v = data->data[0] + (data->data[1] << 8);

    if (get_histream()->mpi_pu_ops &&
        get_histream()->mpi_pu_ops->hue_set)
    {
        get_histream()->mpi_pu_ops->hue_set(v);
    }
}

void _histream_pu_power_line_frequency_ctrl(struct uvc_device *dev, uint8_t req, struct uvc_request_data *resp)
{
#if 0
    uint8_t v = 0;

    if (get_histream()->streaming == 1)
    {
        if (get_histream()->mpi_pu_ops &&
            get_histream()->mpi_pu_ops->power_line_frequency_get)
            v = get_histream()->mpi_pu_ops->power_line_frequency_get();
    }
#endif

    switch (req)
    {
        case UVC_SET_CUR:
            resp->length = 1;
            //RLOG("brightness_control set cur  setup\n");
            break;
        case UVC_GET_CUR:
            //resp->length = 1;
            //resp->data[0] = v;
            dev->request_error_code.data[0] = 0x06;
            dev->request_error_code.length = 1;
               //RLOG("power_line_frequency get cur %d\n",stExpAttr.stAuto.stAntiflicker.u8Frequency);
            break;
        case UVC_GET_DEF:
            resp->length = 1;
            resp->data[0] = 0x1;
            //RLOG("brightness_control get max %d \n",100);
            break;
        case UVC_GET_INFO:
            resp->data[0] = 0x03;
            resp->length = 1;
            //LOG("get info\n");
            break;
        default:
            resp->length = -EL2HLT;
            dev->request_error_code.data[0] = 0x07;
            dev->request_error_code.length = 1;
            break;
    }
}

static void _histream_pu_power_line_frequency_set(struct uvc_request_data *data)
{
    uint8_t v = 0;

    if (get_histream()->streaming != 1)
    {
        return;
    }

    v = data->data[0];

    if (get_histream()->mpi_pu_ops &&
        get_histream()->mpi_pu_ops->power_line_frequency_set)
    {
        get_histream()->mpi_pu_ops->power_line_frequency_set(v);
    }
}

void _histream_pu_saturation_ctrl(struct uvc_device *dev, uint8_t req, struct uvc_request_data *resp)
{
#if 0
    uint16_t v = 50;

    if (get_histream()->streaming == 1)
    {
        if (get_histream()->mpi_pu_ops &&
            get_histream()->mpi_pu_ops->saturation_get)
            v = get_histream()->mpi_pu_ops->saturation_get();
    }
#endif

    switch (req)
    {
        case UVC_SET_CUR:
            resp->length = 2;
            //RLOG("brightness_control set cur  setup\n");
            break;
        case UVC_GET_CUR:
            //resp->length = 2;
            //resp->data[0] = (v & 0xff);
            //resp->data[1] = ((v >> 8) & 0xff);
            dev->request_error_code.data[0] = 0x06;
            dev->request_error_code.length = 1;
            //RLOG("brightness_control get cur %d\n",stCSCAttr.u32LumaVal);
            break;
        case UVC_GET_MIN:
            resp->length = 2;
            resp->data[0] = 0x00;
            resp->data[1] = 0x00;
            //RLOG("brightness_control get min %d \n",0);
            break;
        case UVC_GET_MAX:
            resp->length = 2;
            resp->data[0] = 0x64;
            resp->data[1] = 0x00;
            //RLOG("brightness_control get max %d \n",100);
            break;
        case UVC_GET_LEN:
            resp->data[0] = 0x02;
            resp->data[1] = 0x00;
            resp->length = 2;
            //LOG("GET_LEN for UVCX_PICRURE_TYPE_CONTROL\n");
            break;
        case UVC_GET_INFO:
            resp->data[0] = 0x03;
            resp->length = 1;
            //LOG("get info\n");
            break;
        default:
            resp->length = -EL2HLT;
            dev->request_error_code.data[0] = 0x07;
            dev->request_error_code.length = 1;
            break;
    }
}

static void _histream_pu_saturation_set(struct uvc_request_data *data)
{
    uint16_t v = 0;

    if (get_histream()->streaming != 1)
    {
        return;
    }

    v = data->data[0] + (data->data[1] << 8);

    if (get_histream()->mpi_pu_ops &&
        get_histream()->mpi_pu_ops->saturation_set)
    {
        get_histream()->mpi_pu_ops->saturation_set(v);
    }
}

void _histream_pu_white_balance_temperature_auto_ctrl(struct uvc_device *dev, uint8_t req, struct uvc_request_data *resp)
{
#if 0
    uint8_t v = 0;

    if (get_histream()->streaming == 1)
    {
        if (get_histream()->mpi_pu_ops &&
            get_histream()->mpi_pu_ops->white_balance_temperature_auto_get)
            v = get_histream()->mpi_pu_ops->white_balance_temperature_auto_get();
    }
#endif

    switch (req)
    {
        case UVC_SET_CUR:
            resp->length = 1;
            //RLOG("brightness_control set cur  setup\n");
            break;
        case UVC_GET_CUR:
            //resp->length = 1;
            //resp->data[0] = v;
            dev->request_error_code.data[0] = 0x06;
            dev->request_error_code.length = 1;
            //RLOG("brightness_control get cur %d\n",stCSCAttr.u32LumaVal);
            break;
        case UVC_GET_DEF:
            resp->length = 1;
            resp->data[0] = 0x1;
            //RLOG("brightness_control get max %d \n",100);
            break;
        case UVC_GET_INFO:
            resp->data[0] = 0x03;
            resp->length = 1;
            //LOG("get info\n");
            break;
        default:
            resp->length = -EL2HLT;
            dev->request_error_code.data[0] = 0x07;
            dev->request_error_code.length = 1;
             break;
     }
}

static void _histream_pu_white_balance_temperature_auto_set(struct uvc_request_data *data)
{
    uint8_t v = 0;

    if (get_histream()->streaming != 1)
    {
        return;
    }

    v = data->data[0];

    if (get_histream()->mpi_pu_ops &&
        get_histream()->mpi_pu_ops->white_balance_temperature_auto_set)
    {
        get_histream()->mpi_pu_ops->white_balance_temperature_auto_set(v);
    }
}

void _histream_pu_white_balance_temperature_ctrl(struct uvc_device *dev, uint8_t req, struct uvc_request_data *resp)
{
#if 0
    uint16_t v = 15000;

    if (get_histream()->streaming == 1)
    {
        if (get_histream()->mpi_pu_ops &&
            get_histream()->mpi_pu_ops->white_balance_temperature_get)
            v = get_histream()->mpi_pu_ops->white_balance_temperature_get();
    }
#endif

    switch (req)
    {
        case UVC_SET_CUR:
            resp->length = 2;
            //RLOG("brightness_control set cur  setup\n");
            break;
        case UVC_GET_CUR:
            //resp->length = 2;
            //resp->data[0] = (v & 0xff);
            //resp->data[1] = ((v >> 8) & 0xff);
            dev->request_error_code.data[0] = 0x06;
            dev->request_error_code.length = 1;
            //RLOG("brightness_control get cur %d\n",stCSCAttr.u32LumaVal);
            break;
        case UVC_GET_MIN:
            resp->length = 2;
            resp->data[0] = 1500&0xff;
            resp->data[1] = 1500/256;
            //RLOG("brightness_control get min %d \n",0);
            break;
        case UVC_GET_MAX:
            resp->length = 2;
            resp->data[0] = 15000&0xff;
            resp->data[1] = 15000/256;
            //RLOG("brightness_control get max %d \n",100);
            break;
        case UVC_GET_LEN:
            resp->data[0] = 0x02;
            resp->data[1] = 0x00;
            resp->length = 2;
            //LOG("GET_LEN for UVCX_PICRURE_TYPE_CONTROL\n");
            break;
        case UVC_GET_INFO:
            resp->data[0] = 0x03;
            resp->length = 1;
            //LOG("get info\n");
            break;
        default:
            resp->length = -EL2HLT;
            dev->request_error_code.data[0] = 0x07;
            dev->request_error_code.length = 1;
            break;
    }
}

static void _histream_pu_white_balance_temperature_set(struct uvc_request_data *data)
{
    uint16_t v = 0;

    if (get_histream()->streaming != 1)
    {
        return;
    }

    v = data->data[0] + (data->data[1] << 8);

    if (get_histream()->mpi_pu_ops &&
        get_histream()->mpi_pu_ops->white_balance_temperature_set)
    {
        get_histream()->mpi_pu_ops->white_balance_temperature_set(v);
    }
}

void histream_event_pu_control(struct uvc_device *dev,
                               uint8_t req,
                               uint8_t unit_id,
                               uint8_t cs,
                               struct uvc_request_data* resp)
{
    if (unit_id != 2)
        return ;

    switch(cs)
    {
        case UVC_PU_BRIGHTNESS_CONTROL:
            _histream_pu_brightness_ctrl(dev, req, resp);
            break;
        case UVC_PU_HUE_CONTROL:
            //_histream_pu_hue_ctrl(dev, req, resp);
            //break;
        case UVC_PU_CONTRAST_CONTROL:
            //_histream_pu_contrast_ctrl(dev, req, resp);
            //break;
        case UVC_PU_SATURATION_CONTROL:
            //_histream_pu_saturation_ctrl(dev, req, resp);
            //break;
        case UVC_PU_WHITE_BALANCE_COMPONENT_AUTO_CONTROL:
            //break;
        case UVC_PU_WHITE_BALANCE_COMPONENT_CONTROL:
            //break;
        case UVC_PU_WHITE_BALANCE_TEMPERATURE_CONTROL:
            //_histream_pu_white_balance_temperature_ctrl(dev, req, resp);
            //break;
        case UVC_PU_WHITE_BALANCE_TEMPERATURE_AUTO_CONTROL:
            //_histream_pu_white_balance_temperature_auto_ctrl(dev, req, resp);
            //break;
        case UVC_PU_POWER_LINE_FREQUENCY_CONTROL:
            //_histream_pu_power_line_frequency_ctrl(dev, req, resp);
            //break;
        default:
            //_histream_event_default_control(req, resp);
            resp->length = -EL2HLT;
            dev->request_error_code.data[0] = 0x06;
            dev->request_error_code.length = 1;
            break;
    }
}

void histream_event_pu_data(int unit_id, int control, struct uvc_request_data *data)
{
    if (unit_id != 2)
        return ;

    switch(control)
    {
        case UVC_PU_BRIGHTNESS_CONTROL:
            return _histream_pu_brightness_set(data);
        case UVC_PU_CONTRAST_CONTROL:
            return _histream_pu_contrast_set(data);
        case UVC_PU_HUE_CONTROL:
            return _histream_pu_hue_set(data);
        case UVC_PU_SATURATION_CONTROL:
            return _histream_pu_saturation_set(data);
        case UVC_PU_WHITE_BALANCE_TEMPERATURE_AUTO_CONTROL:
            return _histream_pu_white_balance_temperature_auto_set(data);
        case UVC_PU_WHITE_BALANCE_TEMPERATURE_CONTROL:
            return _histream_pu_white_balance_temperature_set(data);
        case UVC_PU_POWER_LINE_FREQUENCY_CONTROL:
            return _histream_pu_power_line_frequency_set(data);
        default:
            break;
    }
}
/* Histream Processing Unit Operation Functions End */

/*******************************************************
 *	Histream Input Terminal Operation Functions
 *******************************************************/
static void _histream_it_exposure_auto_mode_ctrl(struct uvc_device *dev,
                                                 uint8_t req,
                                                 struct uvc_request_data *resp)
{
    uint8_t v = 0x04;

    if (get_histream()->streaming == 1)
    {
        if (get_histream()->mpi_it_ops &&
            get_histream()->mpi_it_ops->exposure_auto_mode_get)
            v = get_histream()->mpi_it_ops->exposure_auto_mode_get();
    }

    switch (req)
    {
        case UVC_SET_CUR:
            resp->data[0] = 0x0;
            resp->length = 1;
            dev->request_error_code.data[0] = 0x00;
            dev->request_error_code.length = 1;
            //RLOG("brightness_control set cur  setup\n");
            //LOG("it_exposure_auto_mode_ctrl_set cur  setup\n");
            break;
        case UVC_GET_CUR:
            if (get_histream()->exposure_auto_stall)
            {
                //resp->length = -EL2HLT;
                dev->request_error_code.data[0] = 0x04;
                dev->request_error_code.length = 1;
                get_histream()->exposure_auto_stall = 0;
            }else{
                resp->data[0] = v;
                resp->length = 1;
                dev->request_error_code.data[0] = 0x00;
                dev->request_error_code.length = 1;
            }
            break;
        case UVC_GET_RES:
            resp->data[0] = 0x05;
            resp->length = 1;
            dev->request_error_code.data[0] = 0x00;
            dev->request_error_code.length = 1;
            break;
        case UVC_GET_DEF:
            resp->data[0] = 0x04;
            resp->length = 1;
            dev->request_error_code.data[0] = 0x00;
            dev->request_error_code.length = 1;
            //LOG("GET_LEN for UVCX_PICRURE_TYPE_CONTROL\n");
            break;
        case UVC_GET_INFO:
            resp->data[0] = 0x3;
            resp->length = 1;
            dev->request_error_code.data[0] = 0x00;
            dev->request_error_code.length = 1;
            //LOG("get info\n");
            break;
        default:
            resp->length = -EL2HLT;
            dev->request_error_code.data[0] = 0x07;
            dev->request_error_code.length = 1;
            break;
    }
}

static void _histream_it_exposure_auto_mode_set(struct uvc_device* dev,struct uvc_request_data *data)
{
    uint8_t v = 0x04;

    if (get_histream()->streaming != 1)
    {
        return;
    }

    v = data->data[0];
    switch(v)
    {
    case 0x01:
    case 0x02:
    case 0x04:
    case 0x08:
        get_histream()->exposure_auto_stall = 0;
        break;
    default:
        get_histream()->exposure_auto_stall = 1;
        return;
    }
    if (get_histream()->mpi_it_ops &&
        get_histream()->mpi_it_ops->exposure_auto_mode_set)
    {
        get_histream()->mpi_it_ops->exposure_auto_mode_set(v);
    }
}

static void _histream_it_exposure_ansolute_time_ctrl(struct uvc_device *dev,
                                                     uint8_t req,
                                                     struct uvc_request_data *resp)
{
    uint32_t v = 2000;

    if (get_histream()->streaming == 1)
    {
        if (get_histream()->mpi_it_ops &&
            get_histream()->mpi_it_ops->exposure_ansolute_time_get)
            v = get_histream()->mpi_it_ops->exposure_ansolute_time_get();
    }

    switch (req)
    {
        case UVC_SET_CUR:
            resp->length = 4;
            resp->data[0] = 0x0;
            //RLOG("brightness_control set cur  setup\n");
            break;
        case UVC_GET_CUR:
            resp->length = 4;
        //    RLOG("exposure_ansolute_time %d \n",stExpAttr.stManual.u32ExpTime);
            resp->data[0] = (v & 0xff);
            resp->data[1] = ((v >> 8) & 0xff);
            resp->data[2] = ((v >> 16) & 0xff);
            resp->data[3] = ((v >> 24) & 0xff);
            //resp->length = -EL2HLT;
            //dev->request_error_code.length = 1;
            //dev->request_error_code.data[0] = 0x06;

            //RLOG("brightness_control get cur %d\n",stCSCAttr.u32LumaVal);
            break;
        case UVC_GET_MAX:
         //   RLOG("exposure_ansolute_time UVC_GET_MAX\n");
            resp->length = 4;
            resp->data[0] = (2000)&0xff;
            resp->data[1] = (2000>>8)&0xff;
            resp->data[2] = 0x0;
            resp->data[3] = 0x0;
            //RLOG("brightness_control get cur %d\n",stCSCAttr.u32LumaVal);
            break;
        case UVC_GET_MIN:
         //   RLOG("exposure_ansolute_time UVC_GET_MIN\n");
            resp->length = 4;
            resp->data[0] = 0x0a;
            resp->data[1] = 0x0;
            resp->data[2] = 0x0;
            resp->data[3] = 0x0;
            //RLOG("brightness_control get cur %d\n",stCSCAttr.u32LumaVal);
            break;
#if 0
        case UVC_GET_LEN:
            resp->data[0] = 0x04;
            resp->data[1] = 0x00;
            resp->length = 2;
            //LOG("GET_LEN for UVCX_PICRURE_TYPE_CONTROL\n");
            break;
#endif
        case UVC_GET_RES:
            resp->data[0] = 0x0a;
            resp->length = 4;
            //LOG("GET_LEN for UVCX_PICRURE_TYPE_CONTROL\n");
            break;
         case UVC_GET_DEF:
            resp->data[0] = 0x64;
            resp->length = 4;
            //LOG("GET_LEN for UVCX_PICRURE_TYPE_CONTROL\n");
            break;
        case UVC_GET_INFO:
            resp->data[0] = 0xf;
            resp->length = 1;
            //LOG("get info\n");
            break;
        default:
            resp->length = -EL2HLT;
            dev->request_error_code.length = 1;
            dev->request_error_code.data[0] = 0x07;
            break;
    }
}

static void _histream_it_exposure_absolute_time_set(struct uvc_request_data *data)
{
    uint32_t v = 0;

    if (get_histream()->streaming != 1)
    {
        return;
    }

    v = (data->data[0] + (data->data[1]<<8) + (data->data[2]<<16) + (data->data[3]<<24))*100;

    if (get_histream()->mpi_it_ops &&
        get_histream()->mpi_it_ops->exposure_ansolute_time_set)
    {
        get_histream()->mpi_it_ops->exposure_ansolute_time_set(v);
    }
}

void histream_event_it_control(struct uvc_device *dev,
                               uint8_t req,
                               uint8_t unit_id,
                               uint8_t cs,
                               struct uvc_request_data *resp)
{
    if (unit_id != 1)
        return;
#if 0
    if (get_histream()->exposure_auto_stall)
    {
        resp->length = -EL2HLT;
        dev->request_error_code.data[0] = 0x04;
        dev->request_error_code.length = 1;
        get_histream()->exposure_auto_stall = 0;
        return;
    }
#endif
    switch(cs)
    {
        case UVC_CT_AE_MODE_CONTROL:
            //RLOG("UVC_CT_AE_MODE_CONTROL req %d--------\n",req);
            _histream_it_exposure_auto_mode_ctrl(dev, req, resp);
            break;
        case UVC_CT_EXPOSURE_TIME_ABSOLUTE_CONTROL:
            _histream_it_exposure_ansolute_time_ctrl(dev, req, resp);
            break;
        case UVC_CT_EXPOSURE_TIME_RELATIVE_CONTROL:
        case UVC_CT_AE_PRIORITY_CONTROL:
        default:
            //_histream_event_default_control(req, resp);
            resp->length = -EL2HLT;
            dev->request_error_code.data[0] = 0x06;
            dev->request_error_code.length = 1;
            break;
    }
}

void histream_event_it_data(struct uvc_device *dev, int unit_id, int control, struct uvc_request_data *data)
{
    if (unit_id != 1)
        return ;

    switch(control)
    {
        case UVC_CT_AE_MODE_CONTROL:
            _histream_it_exposure_auto_mode_set(dev, data);
            break;
        case UVC_CT_EXPOSURE_TIME_ABSOLUTE_CONTROL:
            _histream_it_exposure_absolute_time_set(data);
            break;
        default:
            break;
    }
}

/* Histream Input Terminal Operation Functions End */

/*******************************************************
 *	Histream Extension Unit H264 Operation Functions
 *******************************************************/
static void _histream_eu_h264_picture_type_control(uint8_t 				 req,
                                                struct uvc_request_data *resp)
{
    switch (req)
    {
        case UVC_SET_CUR:
            resp->length = 4;
    //		  RLOG("req idr frame\n");
            break;
        case UVC_GET_LEN:
            resp->data[0] = 0x04;
            resp->data[1] = 0x00;
            resp->length = 2;
    //		  LOG("GET_LEN for UVCX_PICRURE_TYPE_CONTROL\n");
            break;
        case UVC_GET_INFO:
            resp->data[0] = 0x03;
            resp->length = 1;
    //		  LOG("get info\n");
            break;
        default:
            break;
    }
}

/* Histream Extension Unit H264 Operation Functions End */
void histream_event_eu_h264_control(uint8_t req,
                                   uint8_t  unit_id,
                                   uint8_t  cs,
                                   struct uvc_request_data* resp)
{
    if (unit_id != 10)
        return;

    switch (cs)
    {
        case UVCX_PICTURE_TYPE_CONTROL:
            _histream_eu_h264_picture_type_control(req, resp);
            break;
        default:
            _histream_event_default_control(req, resp);
            break;
    }
}

void histream_event_eu_h264_data(int unit_id, int control, struct uvc_request_data *data)
{
    if (unit_id != 10)
        return;
    switch (control)
    {
        case UVCX_PICTURE_TYPE_CONTROL:
            /*todo*/
            break;
        default:
            break;
    }
}

/*******************************************************
 *	Histream Extension Unit Camera Operation Functions
 *******************************************************/
static void _histream_eu_camera_iframe_control(uint8_t req,
                                              struct uvc_request_data *resp)
{
    switch (req)
    {
        case UVC_SET_CUR:
            resp->length = 4;
      //      RLOG("req idr frame");
            break;
        case UVC_GET_LEN:
            resp->data[0] = 0x04;
            resp->data[1] = 0x00;
            resp->length = 2;
      //      LOG("GET_LEN for UVCX_PICRURE_TYPE_CONTROL\n");
            break;
        case UVC_GET_INFO:
            resp->data[0] = 0x03;
            resp->length = 1;
     //       LOG("get info\n");
            break;
        default:
            break;
     }
}

static void _histream_eu_camera_stream_control(uint8_t req,
                                              struct uvc_request_data *resp)
{
    switch (req)
    {
        case UVC_SET_CUR:
            resp->length = 4;
     //       RLOG("req hicamera_stream");
            break;
        case UVC_GET_LEN:
            resp->data[0] = 0x04;
            resp->data[1] = 0x00;
            resp->length = 2;
     //       LOG("GET_LEN for hicamera_stream\n");
            break;
        case UVC_GET_INFO:
            resp->data[0] = 0x03;
            resp->length = 1;
     //       LOG("get info hicamera_stream\n");
            break;
        default:
            break;
    }
}

static void _histream_eu_camera_reset_control(uint8_t req,
                                             struct uvc_request_data *resp)
{
    switch (req)
    {
        case UVC_SET_CUR:
            resp->length = 4;
        //    RLOG("req reset frame");
            break;
        case UVC_GET_LEN:
            resp->data[0] = 0x04;
            resp->data[1] = 0x00;
            resp->length = 2;
        //    LOG("GET_LEN for ux_camera_reset\n");
            break;
        case UVC_GET_INFO:
            resp->data[0] = 0x03;
            resp->length = 1;
         //   LOG("get info for ux_camera_reset\n");
            break;
        default:
            break;
    }
}

static void _histream_eu_camera_update_system(uint8_t req,
                                             struct uvc_request_data *resp)
{
    switch (req)
    {
        case UVC_SET_CUR:
            resp->length = 4;
        //    RLOG("%s: update system\n", __func__);
            break;
        case UVC_GET_LEN:
            resp->data[0] = 0x04;
            resp->data[1] = 0x00;
            resp->length = 2;
       //     LOG("GET_LEN for %s\n", __func__);
            break;
        case UVC_GET_INFO:
            resp->data[0] = 0x03;
            resp->length = 1;
        //    LOG("get info for %s\n", __func__);
            break;
        default:
            break;
    }
}
void histream_event_eu_camera_control(uint8_t req,
                                       uint8_t unit_id,
                                       uint8_t cs,
                                       struct uvc_request_data* resp)
{
    if (unit_id != UNIT_XU_HICAMERA)
        return;

    switch (cs)
    {
        case HI_XUID_SET_RESET:
            _histream_eu_camera_reset_control(req, resp);
            break;

        case HI_XUID_SET_STREAM:
            _histream_eu_camera_stream_control(req, resp);
            break;

        case HI_XUID_SET_IFRAME:
            _histream_eu_camera_iframe_control(req, resp);
            break;

        case HI_XUID_UPDATE_SYSTEM:
            _histream_eu_camera_update_system(req, resp);
            break;

        default:
            _histream_event_default_control(req, resp);
            break;
    }
}

void histream_event_eu_camera_data(int unit_id, int control, struct uvc_request_data *data)
{
    if (unit_id != UNIT_XU_HICAMERA)
        return;

    switch (control)
    {
        case HI_XUID_SET_RESET:
        /*todo*/
            break;
        default:
            break;
    }
}
/* Histream Extension Unit Camera Operation Functions End */
#endif /* end of #ifndef __HuaweiLite__ */

/*******************************************************
 *  Histream Stream Control Operation Functions
 *******************************************************/
int histream_init(void)
{
    if (get_histream()->mpi_sc_ops &&
        get_histream()->mpi_sc_ops->init)
        return get_histream()->mpi_sc_ops->init();

    return 0;
}

int histream_startup(void)
{
    int ret = 0;

    if (get_histream()->mpi_sc_ops &&
        get_histream()->mpi_sc_ops->startup)
        ret = get_histream()->mpi_sc_ops->startup();

    if (ret == 0)
        get_histream()->streaming = 1;
    return ret;
}

int histream_shutdown(void)
{
    int ret = 0;

    if (get_histream()->mpi_sc_ops &&
        get_histream()->mpi_sc_ops->shutdown)
        ret = get_histream()->mpi_sc_ops->shutdown();

    if (ret == 0)
        get_histream()->streaming = 0;
    return ret;
}

int histream_set_enc_property(struct encoder_property *p)
{
    if (get_histream()->mpi_sc_ops &&
        get_histream()->mpi_sc_ops->set_property)
        return get_histream()->mpi_sc_ops->set_property(p);

    return 0;
}

int histream_set_enc_idr(void)
{
    if (get_histream()->mpi_sc_ops &&
        get_histream()->mpi_sc_ops->set_idr)
        return get_histream()->mpi_sc_ops->set_idr();

    return 0;
}

int histream_register_mpi_ops(struct stream_control_ops *sc_ops,
        struct processing_unit_ops *pu_ops,
        struct input_terminal_ops *it_ops,
        struct extension_unit_ops *eu_ops)
{
    if (sc_ops)
    {
        get_histream()->mpi_sc_ops = sc_ops;
    }

    if (pu_ops)
    {
        get_histream()->mpi_pu_ops = pu_ops;
    }

    if (it_ops)
    {
        get_histream()->mpi_it_ops = it_ops;
    }

    if (eu_ops)
    {
        get_histream()->mpi_eu_ops = eu_ops;
    }

    return 0;
}
