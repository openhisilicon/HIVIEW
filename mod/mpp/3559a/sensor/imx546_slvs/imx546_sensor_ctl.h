#ifndef __IMX546_SENSOR_CTL_H__
#define __IMX546_SENSOR_CTL_H__

#include "hi_type.h"
#include "hi_comm_video.h"
#include "hi_comm_isp.h"

HI_S32 imx546_set_bus(VI_PIPE ViPipe, HI_S8 s8I2cDev);
HI_S32 imx546_i2c_init(VI_PIPE ViPipe);
HI_S32 imx546_i2c_exit(VI_PIPE ViPipe);
HI_S32 imx546_write_register(VI_PIPE ViPipe, HI_U32 addr, HI_U32 data);
HI_S32 imx546_read_register(VI_PIPE ViPipe, HI_U32 addr);
HI_VOID imx546_linear_8m60_slvs8ch_init(VI_PIPE ViPipe);
HI_VOID imx546_stream_on(VI_PIPE ViPipe);
HI_VOID imx546_stream_off(VI_PIPE ViPipe);

#endif
