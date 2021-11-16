/*
 * sample venc interface
 */

#ifndef __SAMPLE_VENC_H__
#define __SAMPLE_VENC_H__

#include "hi_type.h"
#include "sample_comm.h"

typedef struct encoder_property
{
    unsigned int format;
    unsigned int width;
    unsigned int height;
    unsigned char compsite;
} encoder_property;

HI_S32 __SAMPLE_COMM_VENC_StartGetStream(VENC_CHN VeChn[], HI_S32 s32Cnt);
HI_S32 __SAMPLE_COMM_VENC_StopGetStream(HI_VOID);
HI_S32 __SAMPLE_COMM_VENC_FORMAT(HI_VOID);

HI_S32 init_hi_encoder(HI_VOID);
HI_S32 start_hi_encoder(HI_VOID);
HI_S32 stop_hi_encoder(HI_VOID);
HI_S32 set_encoder_idr(HI_VOID);

HI_VOID set_user_config_format(PAYLOAD_TYPE_E *format, PIC_SIZE_E *wh, HI_S32 *c);

#endif //__SAMPLE_VENC_H__
