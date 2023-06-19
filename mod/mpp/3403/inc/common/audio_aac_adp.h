/*
  Copyright (c), 2001-2022, Shenshu Tech. Co., Ltd.
 */

#ifndef AUDIO_AAC_ADP_H
#define AUDIO_AAC_ADP_H

#include <stdio.h>

#include "hi_mpi_audio.h"
#include "hi_common_aenc.h"
#include "hi_common_adec.h"
#include "hi_common_aio.h"
#include "hi_aacdec.h"
#include "hi_aacenc.h"

/* samples per frame for AACLC and aacPlus */
#define AACLD_SAMPLES_PER_FRAME         512
#define AACLC_SAMPLES_PER_FRAME         1024
#define AACPLUS_SAMPLES_PER_FRAME       2048

/* max length of AAC stream by bytes */
#define MAX_AAC_MAINBUF_SIZE    (768 * 2)

typedef enum {
    HI_AAC_TYPE_AACLC      = 0,            /* AAC LC */
    HI_AAC_TYPE_EAAC       = 1,            /* EAAC(HEAAC or AAC+ or aac_plus_v1) */
    HI_AAC_TYPE_EAACPLUS   = 2,            /* EAAC+(AAC++ or aac_plus_v2) */
    HI_AAC_TYPE_AACLD      = 3,
    HI_AAC_TYPE_AACELD     = 4,
    HI_AAC_TYPE_BUTT,
} hi_aac_type;

typedef enum {
    HI_AAC_BPS_8K      = 8000,
    HI_AAC_BPS_16K     = 16000,
    HI_AAC_BPS_22K     = 22000,
    HI_AAC_BPS_24K     = 24000,
    HI_AAC_BPS_32K     = 32000,
    HI_AAC_BPS_48K     = 48000,
    HI_AAC_BPS_64K     = 64000,
    HI_AAC_BPS_96K     = 96000,
    HI_AAC_BPS_128K    = 128000,
    HI_AAC_BPS_256K    = 256000,
    HI_AAC_BPS_320K    = 320000,
    HI_AAC_BPS_BUTT
} hi_aac_bps;

typedef enum {
    HI_AAC_TRANSPORT_TYPE_ADTS = 0,
    HI_AAC_TRANSPORT_TYPE_LOAS = 1,
    HI_AAC_TRANSPORT_TYPE_LATM_MCP1 = 2,
    HI_AAC_TRANSPORT_TYPE_BUTT
} hi_aac_transport_type;

typedef struct {
    hi_s32 sample_rate; /* sample rate */
    hi_s32 bit_rate;    /* bitrate */
    hi_s32 profile;     /* profile */
    hi_s32 tns_used;    /* TNS tools */
    hi_s32 pns_used;    /* PNS tools */
    hi_s32 chn_num;     /* channel num */
} aacdec_frame_info;

/*
 *  AAC commendatory parameter:
 *  sampling rate(HZ)    LC bit_rate(kbit/s)    EAAC bit_rate (kbit/s)    EAAC+ bit_rate (kbit/s)
 *  48000                128                    48                        32,24
 *  44100                128                    48                        32,24
 *  32000                96                     22                        16
 *  24000                64
 *  22050                64
 *  16000                48
 */
typedef struct {
    hi_aac_type     aac_type;   /* AAC profile type */
    hi_aac_bps      bit_rate;   /* AAC bitrate(LC:16~320, EAAC:24~128, EAAC+:16~64, AACLD:16~320, AACELD:32~320) */
    hi_audio_sample_rate sample_rate; /* AAC sample rate(LC:8~48, EAAC:16~48, EAAC+:16~48, AACLD:8~48, AACELD:8~48) */
    hi_audio_bit_width   bit_width;   /* AAC bit width (only support 16bit) */
    hi_audio_snd_mode    snd_mode;    /* sound mode of inferent audio frame */
    hi_aac_transport_type   transport_type;
    hi_s16          band_width;  /* targeted audio bandwidth in hz (0 or 1000~smp_rate/2), the default is 0 */
} hi_aenc_attr_aac;

typedef struct {
    hi_aac_encoder      *aac_state;
    hi_aenc_attr_aac    aac_attr;
} aenc_aac_encoder;

typedef struct {
    hi_aac_transport_type   transport_type;
} hi_adec_attr_aac;

typedef struct {
    hi_aac_decoder      aac_state;
    hi_adec_attr_aac    aac_attr;
} adec_aac_decoder;

hi_s32 hi_mpi_aenc_aac_init(hi_void);

hi_s32 hi_mpi_aenc_aac_deinit(hi_void);

hi_s32 hi_mpi_adec_aac_init(hi_void);

hi_s32 hi_mpi_adec_aac_deinit(hi_void);

#endif
