/*
  Copyright (c), 2001-2022, Shenshu Tech. Co., Ltd.
 */

#ifndef OT_AUDIO_MP3_ADP_H
#define OT_AUDIO_MP3_ADP_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#include "ot_common.h"
#include "ot_common_aio.h"

#include "ot_mp3dec.h"
#include "ot_mp3enc.h"

#define OT_MP3_ADP_MAX_NAME_LEN 256
#define OT_MP3_POINT_NUM_PER_FRAME 1152

typedef enum {
    OT_MP3_BPS_32K = 32000,
    OT_MP3_BPS_40K = 40000,
    OT_MP3_BPS_48K = 48000,
    OT_MP3_BPS_56K = 56000,
    OT_MP3_BPS_64K = 64000,
    OT_MP3_BPS_80K = 80000,
    OT_MP3_BPS_96K = 96000,
    OT_MP3_BPS_112K = 112000,
    OT_MP3_BPS_128K = 128000,
    OT_MP3_BPS_160K = 160000,
    OT_MP3_BPS_192K = 192000,
    OT_MP3_BPS_224K = 224000,
    OT_MP3_BPS_256K = 256000,
    OT_MP3_BPS_320K = 320000,
} ot_mp3_bps;

typedef enum {
    OT_MP3_LAYER_1 = 1,
    OT_MP3_LAYER_2 = 2,
    OT_MP3_LAYER_3 = 3,
} ot_mp3_layer;


typedef struct {
    td_s32 bit_rate;
    td_s32 chn_num; /* 1 or 2 */
    td_s32 sample_rate;
    td_s32 bits_per_sample; /* only support 16 bits */
    td_s32 output_samples;  /* nChans*SamplePerFrame */
    td_s32 layer;           /* layer,the infomation of stream's encode profile */
    td_s32 version;         /* version,the infomation of stream's encode profile */
} ot_mp3_frame_info;

typedef struct {
    td_u32 reserved;
} ot_adec_attr_mp3;

typedef struct {
    ot_audio_sample_rate sample_rate; /* mp3 sample rate, support [8k, 48k] */
    ot_audio_bit_width bit_width;     /* mp3 bit width (only support 16bit) */
    ot_audio_snd_mode sound_mode;     /* sound mode of inferent audio frame */

    ot_mp3_bps bit_rate; /* encode bitrate. */
    td_u32 quality;      /* encode bitstream quality, support[0, 9].
                             0:  Highest quality, very slow; 9:  Poor quality, but fast. default 7. */
} ot_aenc_attr_mp3;

td_s32 hi_mpi_adec_mp3_init(td_void);
td_s32 hi_mpi_adec_mp3_deinit(td_void);

td_s32 hi_mpi_aenc_mp3_init(td_void);
td_s32 hi_mpi_aenc_mp3_deinit(td_void);

#endif
