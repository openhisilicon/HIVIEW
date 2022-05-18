/*
 * Copyright (c) NEXTCHIP Inc. 2017-2020. All rights reserved.
 * Description: NEXTCHIP driver
 * Author: NEXTCHIP Inc.
 * Create: 2020/03/20
 * Modification: 2020/06/20, Hisilicon.
 */

#ifndef _NVP6158_VIDEO_HI_
#define _NVP6158_VIDEO_HI_

#include "common.h"

/* define and enum */
typedef enum _nvp6158_outmode_sel {
    NVP6158_OUTMODE_1MUX_SD = 0,
    NVP6158_OUTMODE_1MUX_HD,
    NVP6158_OUTMODE_1MUX_FHD,
    NVP6158_OUTMODE_2MUX_SD,
    NVP6158_OUTMODE_2MUX_HD,
    NVP6158_OUTMODE_2MUX_MIX,
    NVP6158_OUTMODE_2MUX_FHD,
    NVP6158_OUTMODE_4MUX_SD,
    NVP6158_OUTMODE_4MUX_HD,
    NVP6158_OUTMODE_4MUX_MIX,
    NVP6158_OUTMODE_1MUX_297MHz,
    NVP6158_OUTMODE_1MUX_8M,
    NVP6158_OUTMODE_1MUX_BT1120S,
    NVP6158_OUTMODE_2MUX_BT1120S,
    NVP6158_OUTMODE_4MUX_BT1120S,
    NVP6158_OUTMODE_BUTT
} NVP6158_OUTMODE_SEL;

typedef enum _nvp6158_det_sel {
    NVP6158_DET_MODE_AUTO = 0,
    NVP6158_DET_MODE_AHD,
    NVP6158_DET_MODE_CVI,
    NVP6158_DET_MODE_TVI,
    NVP6158_DET_MODE_OTHER,
    NVP6158_DET_MODE_BUTT
} NVP6158_DET_SEL;

/* external api */
void nvp6158_common_init(unsigned char chip);
int nvp6158_set_portmode(const unsigned char chip, const unsigned char portsel, const unsigned char portmode,
    const unsigned char chid);
int nvp6158_set_chnmode(const unsigned char ch, const unsigned char chnmode);
int nvp6168_set_chnmode(const unsigned char ch, const unsigned char chnmode);
/* new add chnl mode function */
void nvp6158_set_chn_commonvalue(const unsigned char ch, const unsigned char chnmode);

unsigned int video_fmt_det(const unsigned char ch, NVP6158_INFORMATION_S *ps_nvp6158_vfmts);
unsigned int nvp6168_video_fmt_det(const unsigned char ch, NVP6158_INFORMATION_S *ps_nvp6158_vfmts);
unsigned char video_fmt_debounce(unsigned char ch, unsigned char keep_fmt, unsigned int keep_sync_width);
unsigned int nvp6158_getvideoloss(void);

void nvp6158_video_set_sharpness(unsigned char ch, unsigned int value);
void nvp6158_hide_ch(unsigned char ch);
void nvp6158_show_ch(unsigned char ch);

void video_input_new_format_set(const unsigned char ch, const unsigned char chnmode);
NC_FORMAT_STANDARD NVP6158_GetFmtStd_from_Fmtdef(NC_VIVO_CH_FORMATDEF vivofmt);
void nvp6158_additional_for3MoverDef(unsigned char chip);

#endif // End of _NVP6158_VIDEO_HI_
