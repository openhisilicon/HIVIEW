/*
  Copyright (c), 2001-2022, Shenshu Tech. Co., Ltd.
 */

#ifndef HI_AUDIO_MP3_ADP_H
#define HI_AUDIO_MP3_ADP_H

#include "hi_type.h"
#include "ot_audio_mp3_adp.h"

#ifdef __cplusplus
extern "C" {
#endif /* End of #ifdef __cplusplus */
#define HI_MP3_ADP_MAX_NAME_LEN OT_MP3_ADP_MAX_NAME_LEN
#define HI_MP3_POINT_NUM_PER_FRAME OT_MP3_POINT_NUM_PER_FRAME
#define HI_MP3_BPS_32K OT_MP3_BPS_32K
#define HI_MP3_BPS_40K OT_MP3_BPS_40K
#define HI_MP3_BPS_48K OT_MP3_BPS_48K
#define HI_MP3_BPS_56K OT_MP3_BPS_56K
#define HI_MP3_BPS_64K OT_MP3_BPS_64K
#define HI_MP3_BPS_80K OT_MP3_BPS_80K
#define HI_MP3_BPS_96K OT_MP3_BPS_96K
#define HI_MP3_BPS_112K OT_MP3_BPS_112K
#define HI_MP3_BPS_128K OT_MP3_BPS_128K
#define HI_MP3_BPS_160K OT_MP3_BPS_160K
#define HI_MP3_BPS_192K OT_MP3_BPS_192K
#define HI_MP3_BPS_224K OT_MP3_BPS_224K
#define HI_MP3_BPS_256K OT_MP3_BPS_256K
#define HI_MP3_BPS_320K OT_MP3_BPS_320K
#define HI_MP3_LAYER_1 OT_MP3_LAYER_1
#define HI_MP3_LAYER_2 OT_MP3_LAYER_2
#define HI_MP3_LAYER_3 OT_MP3_LAYER_3

typedef ot_mp3_bps hi_mp3_bps;
typedef ot_mp3_layer hi_mp3_layer;
typedef ot_mp3_frame_info hi_mp3_frame_info;
typedef ot_adec_attr_mp3 hi_adec_attr_mp3;
typedef ot_aenc_attr_mp3 hi_aenc_attr_mp3;

hi_s32 hi_mpi_aenc_mp3_init(hi_void);
hi_s32 hi_mpi_aenc_mp3_deinit(hi_void);

hi_s32 hi_mpi_adec_mp3_init(hi_void);
hi_s32 hi_mpi_adec_mp3_deinit(hi_void);

#ifdef __cplusplus
}
#endif /* End of #ifdef __cplusplus */

#endif