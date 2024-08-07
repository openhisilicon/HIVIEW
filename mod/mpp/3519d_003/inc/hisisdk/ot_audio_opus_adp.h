/*
  Copyright (c), 2001-2022, Shenshu Tech. Co., Ltd.
 */

#ifndef OT_AUDIO_OPUS_ADP_H
#define OT_AUDIO_OPUS_ADP_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#include "ot_common.h"
#include "ot_common_aio.h"

#include "ot_opusdec.h"
#include "ot_opusenc.h"

/* max length of OPUS stream by bytes */
#define OT_MAX_OPUS_MAINBUF_SIZE 1500

typedef enum {
    OT_OPUS_BPS_16K = 16000,
    OT_OPUS_BPS_22K = 22000,
    OT_OPUS_BPS_24K = 24000,
    OT_OPUS_BPS_32K = 32000,
    OT_OPUS_BPS_44K = 44000,
    OT_OPUS_BPS_48K = 48000,
    OT_OPUS_BPS_64K = 64000,
    OT_OPUS_BPS_96K = 96000,
    OT_OPUS_BPS_128K = 128000,
    OT_OPUS_BPS_132K = 132000,
    OT_OPUS_BPS_144K = 144000,
    OT_OPUS_BPS_192K = 192000,
    OT_OPUS_BPS_256K = 256000,
    OT_OPUS_BPS_288K = 288000,
    OT_OPUS_BPS_320K = 320000,
    OT_OPUS_BPS_BUTT
} ot_opus_bps;

typedef enum {
    /* Best for most VoIP/videoconference applications where listening quality and intelligibility matter most */
    OT_OPUS_APPLICATION_VOIP = 2048,

    /*
     * Best for broadcast/high-fidelity application where the decoded audio
     * should be as close as possible to the input
     */
    OT_OPUS_APPLICATION_AUDIO = 2049,

    /* Only use when lowest-achievable latency is what matters most. Voice-optimized modes cannot be used. */
    OT_OPUS_APPLICATION_RESTRICTED_LOWDELAY = 2051,
} ot_opus_application;

typedef struct {
    td_s32 reserved;
} ot_opus_frame_info;

typedef struct {
    ot_audio_sample_rate sample_rate; /* Opus sample rate, support [8k, 48k] */
    ot_audio_bit_width bit_width;     /* Opus bit width (only support 16bit) */
    ot_audio_snd_mode snd_mode;       /* sound mode of inferent audio frame */
    ot_opus_bps bit_rate;             /* encode bitrate. */
    ot_opus_application app;
} ot_aenc_attr_opus;

typedef struct {
    ot_audio_sample_rate sample_rate; /* Opus sample rate, support [8k, 48k] */
    ot_audio_snd_mode snd_mode;       /* sound mode of inferent audio frame */
} ot_adec_attr_opus;

td_s32 hi_mpi_adec_opus_init(td_void);
td_s32 hi_mpi_adec_opus_deinit(td_void);

td_s32 hi_mpi_aenc_opus_init(td_void);
td_s32 hi_mpi_aenc_opus_deinit(td_void);

#endif