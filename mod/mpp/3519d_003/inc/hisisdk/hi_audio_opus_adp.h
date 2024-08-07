/*
  Copyright (c), 2001-2022, Shenshu Tech. Co., Ltd.
 */

#ifndef HI_AUDIO_OPUS_ADP_H
#define HI_AUDIO_OPUS_ADP_H

#include "hi_type.h"
#include "ot_audio_opus_adp.h"

#ifdef __cplusplus
extern "C" {
#endif /* End of #ifdef __cplusplus */

#define HI_MAX_OPUS_MAINBUF_SIZE OT_MAX_OPUS_MAINBUF_SIZE

#define HI_OPUS_BPS_16K OT_OPUS_BPS_16K
#define HI_OPUS_BPS_22K OT_OPUS_BPS_22K
#define HI_OPUS_BPS_24K OT_OPUS_BPS_24K
#define HI_OPUS_BPS_32K OT_OPUS_BPS_32K
#define HI_OPUS_BPS_44K OT_OPUS_BPS_44K
#define HI_OPUS_BPS_48K OT_OPUS_BPS_48K
#define HI_OPUS_BPS_64K OT_OPUS_BPS_64K
#define HI_OPUS_BPS_96K OT_OPUS_BPS_96K
#define HI_OPUS_BPS_128K OT_OPUS_BPS_128K
#define HI_OPUS_BPS_132K OT_OPUS_BPS_132K
#define HI_OPUS_BPS_144K OT_OPUS_BPS_144K
#define HI_OPUS_BPS_192K OT_OPUS_BPS_192K
#define HI_OPUS_BPS_256K OT_OPUS_BPS_256K
#define HI_OPUS_BPS_288K OT_OPUS_BPS_288K
#define HI_OPUS_BPS_320K OT_OPUS_BPS_320K
#define HI_OPUS_BPS_BUTT OT_OPUS_BPS_BUTT

typedef ot_opus_bps hi_opus_bps;
typedef ot_opus_frame_info hi_opus_frame_info;
#define HI_OPUS_APPLICATION_VOIP OT_OPUS_APPLICATION_VOIP
#define HI_OPUS_APPLICATION_AUDIO OT_OPUS_APPLICATION_AUDIO
#define HI_OPUS_APPLICATION_RESTRICTED_LOWDELAY OT_OPUS_APPLICATION_RESTRICTED_LOWDELAY
typedef ot_opus_application hi_opus_application;
typedef ot_adec_attr_opus hi_adec_attr_opus;
typedef ot_aenc_attr_opus hi_aenc_attr_opus;

hi_s32 hi_mpi_aenc_opus_init(hi_void);
hi_s32 hi_mpi_aenc_opus_deinit(hi_void);
hi_s32 hi_mpi_adec_opus_init(hi_void);
hi_s32 hi_mpi_adec_opus_deinit(hi_void);

#ifdef __cplusplus
}
#endif /* End of #ifdef __cplusplus */

#endif
