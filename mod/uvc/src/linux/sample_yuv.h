/*
 * the header file defines some APIs to access  YUV stream
 */

#ifndef __SAMPLE_YUV_H__
#define __SAMPLE_YUV_H__

#include "hi_type.h"

#define UVC_SAVE_FILE 0

HI_S32 do_close_yuv_capture(HI_VOID);
HI_VOID set_yuv_property(HI_VOID);
HI_VOID set_compsite_yuv_property(HI_VOID);

#endif //__SAMPLE_YUV_H__
