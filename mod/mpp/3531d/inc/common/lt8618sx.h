/*
 * Copyright (c) Lontium Technologies Co., Ltd. 2019-2020. All rights reserved.
 * Description: lt8618sxb driver
 * Author: Lontium Technologies Co., Ltd.
 * Create: 2019/02/20
 * Modification: 2020/02/25, Hisilicon.
 */

#ifndef _LT8618SX_H_
#define _LT8618SX_H_

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* End of #ifdef __cplusplus */

#define HDMI_PAL             0
#define HDMI_NTSC            1
#define HDMI_960H_PAL        2
#define HDMI_960H_NTSC       3
#define HDMI_640x480_60      4
#define HDMI_480P60          5
#define HDMI_576P50          6
#define HDMI_800x600_60      7
#define HDMI_1024x768_60     8
#define HDMI_720P50          9
#define HDMI_720P60          10
#define HDMI_1280x800_60     11
#define HDMI_1280x1024_60    12
#define HDMI_1366x768_60     13
#define HDMI_1400x1050_60    14
#define HDMI_1440x900_60     15
#define HDMI_1600x1200_60    16
#define HDMI_1680x1050_60    17
#define HDMI_1080P24         18
#define HDMI_1080P25         19
#define HDMI_1080P30         20
#define HDMI_1080I50         21
#define HDMI_1080I60         22
#define HDMI_1080P50         23
#define HDMI_1080P60         24
#define HDMI_1920x1200_60    25
#define HDMI_1920x2160_30    26
#define HDMI_2560x1440_30    27
#define HDMI_2560x1440_60    28
#define HDMI_2560x1600_60    29
#define HDMI_3840x2160_24    30
#define HDMI_3840x2160_25    31
#define HDMI_3840x2160_30    32

#define LT_DESC(x) 1

#define HI_LT_IOC_MAGIC   's'

#define LT_CMD_SETMODE   _IOW(HI_LT_IOC_MAGIC, 0x01, unsigned int)
void Resolution_change(unsigned int Resolution);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */

#endif    /* _LT8618SX_H_ */
