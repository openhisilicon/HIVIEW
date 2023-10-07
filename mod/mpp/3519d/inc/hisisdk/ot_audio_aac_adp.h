/*
  Copyright (c), 2001-2022, Shenshu Tech. Co., Ltd.
 */

#ifndef OT_AUDIO_AAC_ADP_H
#define OT_AUDIO_AAC_ADP_H

#include "hi_mpi_audio.h"
#include "ot_common_aenc.h"
#include "ot_common_adec.h"
#include "ot_common_aio.h"
#include "ot_aacdec.h"
#include "ot_aacenc.h"

/* samples per frame for AACLC and aacPlus */
#define OT_AACLD_SAMPLES_PER_FRAME         512
#define OT_AACLC_SAMPLES_PER_FRAME         1024
#define OT_AACPLUS_SAMPLES_PER_FRAME       2048

/* max length of AAC stream by bytes */
#define OT_MAX_AAC_MAINBUF_SIZE    (768 * 2)

typedef enum {
    OT_AAC_TYPE_AACLC      = 0,            /* AAC LC */
    OT_AAC_TYPE_EAAC       = 1,            /* EAAC(HEAAC or AAC+ or aac_plus_v1) */
    OT_AAC_TYPE_EAACPLUS   = 2,            /* EAAC+(AAC++ or aac_plus_v2) */
    OT_AAC_TYPE_AACLD      = 3,
    OT_AAC_TYPE_AACELD     = 4,
    OT_AAC_TYPE_BUTT,
} ot_aac_type;

typedef enum {
    OT_AAC_BPS_8K      = 8000,
    OT_AAC_BPS_16K     = 16000,
    OT_AAC_BPS_22K     = 22000,
    OT_AAC_BPS_24K     = 24000,
    OT_AAC_BPS_32K     = 32000,
    OT_AAC_BPS_48K     = 48000,
    OT_AAC_BPS_64K     = 64000,
    OT_AAC_BPS_96K     = 96000,
    OT_AAC_BPS_128K    = 128000,
    OT_AAC_BPS_256K    = 256000,
    OT_AAC_BPS_320K    = 320000,
    OT_AAC_BPS_BUTT
} ot_aac_bps;

typedef enum {
    OT_AAC_TRANSPORT_TYPE_ADTS = 0,
    OT_AAC_TRANSPORT_TYPE_LOAS = 1,
    OT_AAC_TRANSPORT_TYPE_LATM_MCP1 = 2,
    OT_AAC_TRANSPORT_TYPE_BUTT
} ot_aac_transport_type;

typedef struct {
    td_s32 sample_rate; /* sample rate */
    td_s32 bit_rate;    /* bitrate */
    td_s32 profile;     /* profile */
    td_s32 tns_used;    /* TNS tools */
    td_s32 pns_used;    /* PNS tools */
    td_s32 chn_num;     /* channel num */
} ot_aac_frame_info;

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
    ot_aac_type aac_type; /* AAC profile type */
    ot_aac_bps bit_rate;  /* AAC bitrate(LC:16~320, EAAC:24~128, EAAC+:16~64, AACLD:16~320, AACELD:32~320) */
    ot_audio_sample_rate sample_rate; /* AAC sample rate(LC:8~48, EAAC:16~48, EAAC+:16~48, AACLD:8~48, AACELD:8~48) */
    ot_audio_bit_width bit_width;     /* AAC bit width (only support 16bit) */
    ot_audio_snd_mode snd_mode;       /* sound mode of inferent audio frame */
    ot_aac_transport_type transport_type;
    td_s16 band_width; /* targeted audio bandwidth in hz (0 or 1000~smp_rate/2), the default is 0 */
} ot_aenc_attr_aac;

typedef struct {
    ot_aac_transport_type transport_type;
} ot_adec_attr_aac;

td_s32 hi_mpi_aenc_aac_init(td_void);
td_s32 hi_mpi_aenc_aac_deinit(td_void);

td_s32 hi_mpi_adec_aac_init(td_void);
td_s32 hi_mpi_adec_aac_deinit(td_void);

#endif
