/*
 * Copyright (c) Hisilicon Technologies Co., Ltd. 2019-2020. All rights reserved.
 * Description: hi_common_aio.h
 * Author: Hisilicon multimedia software group
 * Create: 2019/06/15
 * History:
 *  1.Date                 :   2019/06/15
 *    Modification         :   Created file
 */

#ifndef __HI_COMMON_AIO_H__
#define __HI_COMMON_AIO_H__

#include "hi_common.h"
#include "hi_errno.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* End of #ifdef __cplusplus */


#define HI_MAX_AUDIO_FRAME_NUM    300      /* max count of audio frame in Buffer */
#define HI_MAX_AUDIO_POINT_BYTES  4        /* max bytes of one sample point(now 32bit max) */

#define HI_MAX_VOICE_POINT_NUM    480      /* max sample per frame for voice encode */

#define HI_MAX_AUDIO_POINT_NUM    2048     /* max sample per frame for all encoder(aacplus:2048) */
#define HI_MAX_AO_POINT_NUM       4096     /* from h3;support 4096 framelen */
#define HI_MIN_AUDIO_POINT_NUM    80       /* min sample per frame */
#define HI_MAX_AI_POINT_NUM       2048     /* max sample per frame for all encoder(aacplus:2048) */

#define HI_AUDIO_FRAME_CHN_NUM    2        /* channel number in frame */

/* max length of audio frame by bytes, one frame contain many sample point */
#define HI_MAX_AUDIO_FRAME_LEN    (HI_MAX_AUDIO_POINT_BYTES * HI_MAX_AO_POINT_NUM)

/* max length of audio stream by bytes */
#define HI_MAX_AUDIO_STREAM_LEN   HI_MAX_AUDIO_FRAME_LEN

#define HI_MAX_AI_USRFRM_DEPTH     30      /* max depth of user frame buf */

#define HI_MAX_AUDIO_FILE_PATH_LEN 256
#define HI_MAX_AUDIO_FILE_NAME_LEN 256

/* The VQE EQ Band num. */
#define HI_VQE_EQ_BAND_NUM  10
#define HI_VQE_DRC_SEC_NUM   5

/* The VQE AEC Band num. */
#define HI_AEC_ERL_BAND_NUM       6
#define HI_AEC_ERL_BAND_VALUE_NUM 7

/* The TalkVQEV2 EQ Band num. */
#define HI_TALKVQEV2_EQ_BAND_NUM   30

#define HI_AI_TALKVQEV2_MASK_PNR       0x1
#define HI_AI_TALKVQEV2_MASK_NR        0x2
#define HI_AI_TALKVQEV2_MASK_EQ        0x4
#define HI_AI_TALKVQEV2_MASK_AGC       0x8
#define HI_AI_TALKVQEV2_MASK_DRC       0x10
#define HI_AI_TALKVQEV2_MASK_LIMITER   0x20
#define HI_AI_TALKVQEV2_MASK_DEREVERB  0x40
#define HI_AI_TALKVQEV2_MASK_FMP       0x80

#define HI_AI_RECORDVQE_MASK_HPF       0x1
#define HI_AI_RECORDVQE_MASK_RNR       0x2
#define HI_AI_RECORDVQE_MASK_HDR       0x4
#define HI_AI_RECORDVQE_MASK_DRC       0x8
#define HI_AI_RECORDVQE_MASK_EQ        0x10
#define HI_AI_RECORDVQE_MASK_AGC       0x20

#define HI_AI_TALKVQE_MASK_HPF         0x1
#define HI_AI_TALKVQE_MASK_AEC         0x2
#define HI_AI_TALKVQE_MASK_AGC         0x8
#define HI_AI_TALKVQE_MASK_EQ          0x10
#define HI_AI_TALKVQE_MASK_ANR         0x20

#define HI_AO_VQE_MASK_HPF             0x1
#define HI_AO_VQE_MASK_ANR             0x2
#define HI_AO_VQE_MASK_AGC             0x4
#define HI_AO_VQE_MASK_EQ              0x8

typedef enum {
    HI_AUDIO_SAMPLE_RATE_8000   = 8000,    /* 8kHz sample rate */
    HI_AUDIO_SAMPLE_RATE_12000  = 12000,   /* 12kHz sample rate */
    HI_AUDIO_SAMPLE_RATE_11025  = 11025,   /* 11.025kHz sample rate */
    HI_AUDIO_SAMPLE_RATE_16000  = 16000,   /* 16kHz sample rate */
    HI_AUDIO_SAMPLE_RATE_22050  = 22050,   /* 22.05kHz sample rate */
    HI_AUDIO_SAMPLE_RATE_24000  = 24000,   /* 24kHz sample rate */
    HI_AUDIO_SAMPLE_RATE_32000  = 32000,   /* 32kHz sample rate */
    HI_AUDIO_SAMPLE_RATE_44100  = 44100,   /* 44.1kHz sample rate */
    HI_AUDIO_SAMPLE_RATE_48000  = 48000,   /* 48kHz sample rate */
    HI_AUDIO_SAMPLE_RATE_64000  = 64000,   /* 64kHz sample rate */
    HI_AUDIO_SAMPLE_RATE_96000  = 96000,   /* 96kHz sample rate */
    HI_AUDIO_SAMPLE_RATE_BUTT,
} hi_audio_sample_rate;

typedef enum {
    HI_AUDIO_BIT_WIDTH_8   = 0,   /* 8bit width */
    HI_AUDIO_BIT_WIDTH_16  = 1,   /* 16bit width */
    HI_AUDIO_BIT_WIDTH_24  = 2,   /* 24bit width */
    HI_AUDIO_BIT_WIDTH_BUTT,
} hi_audio_bit_width;

typedef enum {
    HI_AIO_MODE_I2S_MASTER  = 0,    /* AIO I2S master mode */
    HI_AIO_MODE_I2S_SLAVE,          /* AIO I2S slave mode */
    HI_AIO_MODE_PCM_SLAVE_STD,      /* AIO PCM slave standard mode */
    HI_AIO_MODE_PCM_SLAVE_NON_STD,  /* AIO PCM slave non-standard mode */
    HI_AIO_MODE_PCM_MASTER_STD,     /* AIO PCM master standard mode */
    HI_AIO_MODE_PCM_MASTER_NON_STD, /* AIO PCM master non-standard mode */
    HI_AIO_MODE_BUTT
} hi_aio_mode;

typedef enum {
    HI_AIO_I2STYPE_INNERCODEC = 0, /* AIO I2S connect inner audio CODEC */
    HI_AIO_I2STYPE_INNERHDMI,      /* AIO I2S connect Inner HDMI */
    HI_AIO_I2STYPE_EXTERN,         /* AIO I2S connect extern hardware */
} hi_aio_i2s_type;

typedef enum {
    HI_AUDIO_SOUND_MODE_MONO   = 0, /* mono */
    HI_AUDIO_SOUND_MODE_STEREO = 1, /* stereo */
    HI_AUDIO_SOUND_MODE_BUTT
} hi_audio_snd_mode;

/*
An example of the packing scheme for G726-32 codewords is as shown,
and bit A3 is the least significant bit of the first codeword:
RTP G726-32:
0                   1
0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
|B B B B|A A A A|D D D D|C C C C| ...
|0 1 2 3|0 1 2 3|0 1 2 3|0 1 2 3|
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-

MEDIA G726-32:
0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
|A A A A|B B B B|C C C C|D D D D| ...
|3 2 1 0|3 2 1 0|3 2 1 0|3 2 1 0|
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
*/
typedef enum {
    HI_G726_16K = 0,       /* G726 16kbps, see RFC3551.txt  4.5.4 G726-16 */
    HI_G726_24K,           /* G726 24kbps, see RFC3551.txt  4.5.4 G726-24 */
    HI_G726_32K,           /* G726 32kbps, see RFC3551.txt  4.5.4 G726-32 */
    HI_G726_40K,           /* G726 40kbps, see RFC3551.txt  4.5.4 G726-40 */
    HI_MEDIA_G726_16K,     /* G726 16kbps for ASF ... */
    HI_MEDIA_G726_24K,     /* G726 24kbps for ASF ... */
    HI_MEDIA_G726_32K,     /* G726 32kbps for ASF ... */
    HI_MEDIA_G726_40K,     /* G726 40kbps for ASF ... */
    HI_G726_BUTT,
} hi_g726_bps;

typedef enum {
    /* see DVI4 diiffers in three respects from the IMA ADPCM at RFC3551.txt 4.5.1 DVI4 */
    HI_ADPCM_TYPE_DVI4 = 0,    /* 32kbps ADPCM(DVI4) for RTP */
    HI_ADPCM_TYPE_IMA,         /* 32kbps ADPCM(IMA),NOTICE:point num must be 161/241/321/481 */
    HI_ADPCM_TYPE_ORG_DVI4,
    HI_ADPCM_TYPE_BUTT,
} hi_adpcm_type;

#define HI_AI_EXPAND  0x01
#define HI_AI_CUT     0x02

typedef struct {
    hi_audio_sample_rate sample_rate;    /* sample rate */
    hi_audio_bit_width   bit_width;      /* bit_width */
    hi_aio_mode          work_mode;      /* master or slave mode */
    hi_audio_snd_mode    snd_mode;       /* momo or stereo */
    hi_u32               expand_flag;    /* expand 8bit to 16bit, use HI_AI_EXPAND(only valid for AI 8bit),
                                            use HI_AI_CUT(only valid for extern codec for 24bit) */
    hi_u32               frame_num;      /* frame num in buf[2,HI_MAX_AUDIO_FRAME_NUM] */
    hi_u32               point_num_per_frame;   /* point num per frame (80/160/240/320/480/1024/2048)
                                                   (ADPCM IMA should add 1 point, AMR only support 160) */
    hi_u32               chn_cnt;        /* channle number on FS, valid value:1/2/4/8 */
    hi_u32               clk_share;      /* 0: AI and AO clock is separate
                                            1: AI and AO clock is inseparate, AI use AO's clock */
    hi_aio_i2s_type      i2s_type;       /* i2s type */
} hi_aio_attr;

typedef struct {
    hi_u32 usr_frame_depth;
} hi_ai_chn_param;

typedef enum {
    HI_AI_CHN_MODE_NORMAL = 0,     /* normal mode for AI chn */
    HI_AI_CHN_MODE_FAST,           /* fast mode for AI chn */
    HI_AI_CHN_MODE_BUTT,
} hi_ai_chn_mode;

typedef struct {
    hi_ai_chn_mode mode;       /* operating mode */
} hi_ai_chn_attr;

typedef struct {
    hi_audio_bit_width   bit_width;     /* audio frame bit_width */
    hi_audio_snd_mode    snd_mode;      /* audio frame momo or stereo mode */
    hi_u8 ATTRIBUTE *virt_addr[HI_AUDIO_FRAME_CHN_NUM];
    hi_phys_addr_t ATTRIBUTE phys_addr[HI_AUDIO_FRAME_CHN_NUM];
    hi_u64  time_stamp;                 /* audio frame time stamp */
    hi_u32  seq;                        /* audio frame seq */
    hi_u32  len;                        /* data lenth per channel in frame */
    hi_u32  pool_id[HI_AUDIO_FRAME_CHN_NUM];
} hi_audio_frame;

typedef struct {
    hi_audio_frame ref_frame;      /* AEC reference audio frame */
    hi_bool     valid;             /* whether frame is valid */
    hi_bool     sys_bind;          /* whether is sysbind */
} hi_aec_frame;

typedef struct {
    hi_audio_frame *frame;         /* frame pointor */
    hi_u32         id;             /* frame id */
} hi_audio_frame_info;

typedef struct {
    hi_u8 ATTRIBUTE *stream;    /* the virtual address of stream */
    hi_phys_addr_t ATTRIBUTE phys_addr;  /* the physics address of stream */
    hi_u32 len;                 /* stream length, by bytes */
    hi_u64 time_stamp;          /* frame time stamp */
    hi_u32 seq;                 /* frame seq, if stream is not a valid frame,seq is 0 */
} hi_audio_stream;

typedef struct {
    hi_u32 chn_total_num;       /* total number of channel buffer */
    hi_u32 chn_free_num;        /* free number of channel buffer */
    hi_u32 chn_busy_num;        /* busy number of channel buffer */
} hi_ao_chn_state;

typedef enum {
    HI_AUDIO_TRACK_NORMAL      = 0,
    HI_AUDIO_TRACK_BOTH_LEFT   = 1,
    HI_AUDIO_TRACK_BOTH_RIGHT  = 2,
    HI_AUDIO_TRACK_EXCHANGE    = 3,
    HI_AUDIO_TRACK_MIX         = 4,
    HI_AUDIO_TRACK_LEFT_MUTE   = 5,
    HI_AUDIO_TRACK_RIGHT_MUTE  = 6,
    HI_AUDIO_TRACK_BOTH_MUTE   = 7,

    HI_AUDIO_TRACK_BUTT
} hi_audio_track_mode;

typedef enum {
    HI_AUDIO_FADE_RATE_1   = 0,
    HI_AUDIO_FADE_RATE_2   = 1,
    HI_AUDIO_FADE_RATE_4   = 2,
    HI_AUDIO_FADE_RATE_8   = 3,
    HI_AUDIO_FADE_RATE_16  = 4,
    HI_AUDIO_FADE_RATE_32  = 5,
    HI_AUDIO_FADE_RATE_64  = 6,
    HI_AUDIO_FADE_RATE_128 = 7,

    HI_AUDIO_FADE_RATE_BUTT
} hi_audio_fade_rate;

typedef struct {
    hi_bool fade;
    hi_audio_fade_rate fade_in_rate;
    hi_audio_fade_rate fade_out_rate;
} hi_audio_fade;

/* Defines the configure parameters of AEC. */
typedef struct {
    hi_bool usr_mode;               /* mode 0: auto mode 1: mannual. */
    hi_s8 cozy_noisy_mode;          /* cozy noisy mode: 0 close, 1 open, recommend 1 */
    hi_s8 near_all_pass_energy;     /* the far-end energy threshold for judging whether unvarnished transmission:
                                       0 -59dBm0, 1 -49dBm0, 2 -39dBm0, recommend 1 */
    hi_s8 near_clean_sup_energy;    /* the energy threshold for compelling reset of near-end signal: 0 12dB,
                                       1 15dB, 2 18dB, recommend 2 */

    hi_s16 double_talk_threshold;   /* the threshold of judging single or double talk, recommend 16384, [0, 32767] */

    hi_s16 echo_band_low;           /* voice processing band1, low frequency parameter, [1, 63) for 8k,
                                       [1, 127) for 16k, recommend 10 */
    hi_s16 echo_band_high;          /* voice processing band1, high frequency parameter, (echo_band_low, 63] for 8k,
                                       (echo_band_low, 127] for 16k, recommend 41 */
                                    /* echo_band_high must be greater than echo_band_low */
    hi_s16 echo_band_low2;          /* voice processing band2, low frequency parameter,
                                       [1, 63) for 8k, [1, 127) for 16k, recommend 47 */
    hi_s16 echo_band_high2;         /* voice processing band2, high frequency parameter, (echo_band_low2, 63] for 8k,
                                       (echo_band_low2, 127] for 16k, recommend 72,
                                       echo_band_high2 must be greater than echo_band_low2 */

    hi_s16 erl_band[HI_AEC_ERL_BAND_NUM];    /* ERL protect area, [1, 63] for 8k, [1, 127] for 16k,
                                                frequency band calculated by erl_band * 62.5 */
                                             /* besides, erl_band[n+1] should be greater than erl_band[n] */
    hi_s16 erl[HI_AEC_ERL_BAND_VALUE_NUM];   /* ERL protect value of ERL protect area, the smaller its value,
                                                the more strength its protect ability,[0, 18] */
    hi_s16 voice_protect_freq_l;    /* protect area of near-end low frequency,
                                       [1, 63) for 8k, [1, 127) for 16k, recommend 3 */
    hi_s16 voice_protect_freq_l1;   /* protect area of near-end low frequency1, (voice_protect_freq_l, 63] for 8k,
                                       (voice_protect_freq_l, 127] for 16k, recommend 6 */
    hi_s32 reserved;                /* voice_protect_freq_l1 must be greater than voice_protect_freq_l */
} hi_ai_aec_cfg;

/* Defines the configure parameters of ANR. */
typedef struct {
    hi_bool usr_mode;       /* mode 0: auto, mode 1: manual. */

    hi_s16 nr_intensity;    /* noise reduce intensity, range: [0, 25] */
    hi_s16 noise_db_threshold;      /* noise threshold, range: [30, 60] */
    hi_s8  music_probe_switch;      /* switch for music probe, range: [0:close, 1:open] */

    hi_s32 reserved;
} hi_audio_anr_cfg;

/* Defines the configure parameters of AGC. */
typedef struct {
    hi_bool usr_mode;           /* mode 0: auto, mode 1: manual. */

    hi_s8 target_level;         /* target voltage level, range: [-40, -1]dB */
    hi_s8 noise_floor;          /* noise floor, range: TalkVQE/AoVQE[-65, -20]dB, RecordVQE[-50, -20]dB */
    hi_s8 max_gain;             /* max gain, range: [0, 30]dB */
    hi_s8 adjust_speed;         /* adjustable speed, range: [0, 10]dB/s */

    hi_s8 improve_snr;          /* switch for improving SNR, range: [0:close, 1:upper limit 3dB, 2:upper limit 6dB] */
    hi_s8 use_hpf;              /* switch for using high pass filt,
                                   range: [0:close, 1:80Hz, 2:120Hz, 3:150Hz, 4:300Hz, 5:500Hz] */
    hi_s8 output_mode;          /* output mode, mute when lower than noise floor, range: [0:close, 1:open] */
    hi_s16 noise_suppress_switch;    /* switch for noise suppression, range: [0:close, 1:open] */

    hi_s32 reserved;
} hi_audio_agc_cfg;

/* Defines the configure parameters of HPF. */
typedef enum {
    HI_AUDIO_HPF_FREQ_80   = 80,    /* 80Hz */
    HI_AUDIO_HPF_FREQ_120  = 120,   /* 120Hz */
    HI_AUDIO_HPF_FREQ_150  = 150,   /* 150Hz */
    HI_AUDIO_HPF_FREQ_BUTT,
} hi_audio_hpf_freq;

typedef struct {
    hi_bool usr_mode;              /* mode 0: auto mode 1: mannual. */
    hi_audio_hpf_freq hpf_freq;    /* freq to be processed, value:HiFiVQE/TalkVQE/AoVQE(80Hz, 120Hz, 150Hz),
                                      RecordVQE(80Hz) */
} hi_audio_hpf_cfg;

typedef struct {
    hi_bool usr_mode;       /* mode 0: auto, mode 1: mannual. */
    hi_s32  nr_mode;        /* mode 0: floor noise; 1:ambient noise */
    hi_s32  max_nr_level;   /* max NR level range:[2,20]dB */
    hi_s32  noise_threshold;   /* noise threshold, range:[-80, -20] */
} hi_ai_rnr_cfg;

typedef struct {
    hi_s8  gain_db[HI_VQE_EQ_BAND_NUM];   /* EQ band, include 100, 200, 250, 350, 500, 800, 1.2k, 2.5k, 4k, 8k in turn,
                                             range:TalkVQE/AoVQE[-100, 20], RecordVQE[-50, 20] */
    hi_s32 reserved;
} hi_audio_eq_cfg;

/* Defines the configure parameters of UPVQE work state. */
typedef enum {
    HI_VQE_WORK_STATE_COMMON  = 0,   /* common environment, Applicable to the family of voice calls. */
    HI_VQE_WORK_STATE_MUSIC   = 1,   /* music environment , Applicable to the family of music environment. */
    HI_VQE_WORK_STATE_NOISY   = 2,   /* noisy environment , Applicable to the noisy voice calls.  */
} hi_vqe_work_state;

/* Defines record type */
typedef enum {
    HI_VQE_RECORD_NORMAL      = 0,  /* double micphone recording. */
    HI_VQE_RECORD_BUTT,
} hi_vqe_record_type;

/* HDR Set CODEC GAIN Function Handle type */
typedef hi_s32 (*func_gain_callback)(hi_s32 set_gain);

typedef struct {
    hi_bool usr_mode;               /* mode 0: auto mode 1: mannual. */

    hi_s32 min_gain_db;             /* the minimum of MIC(AI) CODEC gain, [0, 120] */
    hi_s32 max_gain_db;             /* the maximum of MIC(AI) CODEC gain, [0, 120] */

    hi_s32 mic_gain_db;             /* the current gain of MIC(AI) CODEC,[min_gain_db, max_gain_db] */
    hi_s32 mic_gain_step_db;        /* the step size of gain adjustment, [1, 3], recommemd 2 */
    func_gain_callback callback;    /* the callback function pointer of CODEC gain adjustment */
} hi_ai_hdr_cfg;

typedef struct {
    hi_bool usr_mode;       /* enable user mode or not,default 0: disable user mode,1: user mode. */

    hi_s16  attack_time;    /* time of signal change from large to small (ms), range:HiFiVQE[10, 250]ms,
                               RecordVQE[10, 126]ms */
    hi_s16  release_time;   /* time of signal change from small to large (ms), range:HiFiVQE[10, 250]ms,
                               RecordVQE[10, 126]ms */

    hi_s16 old_level_db[HI_VQE_DRC_SEC_NUM];  /* old curve level(dB), default[0, -472, -792, -960, -1280],
                                                 range:[-1440, 0]ms,store from big to small,scale:Q4 */
    hi_s16 new_level_db[HI_VQE_DRC_SEC_NUM];  /* new curve level(dB), default[0, -174, -410, -608, -1021],
                                                 range:[-1440, 0]ms,store from big to small,scale:Q4 */
} hi_ai_drc_cfg;

typedef struct {
    hi_bool usr_mode;                           /* mode 0: auto, mode 1: mannual. */
    hi_s8  gain_db[HI_TALKVQEV2_EQ_BAND_NUM];   /* EQ band, range:[-50, 20]dB */
} hi_ai_talk_v2_eq_cfg;

typedef struct {
    hi_bool usr_mode;           /* mode 0: auto, mode 1: mannual. */

    hi_s16 min_gain_limit;      /* maximum allowable noise rejection, default:5827, range:[0, 32767] */
    hi_s16 snr_prior_limit;     /* minimum prior signal-to-noise ratio, default:3276, range:[0, 32767] */
    hi_s16 ht_threshold;        /* voice decision threshold, default:10, range:[0, 80] */
    hi_s16 hs_threshold;        /* harmonic detection threshold, default:100, range:[100, 1100] */
    hi_s16 alpha_ph;            /* speech probability smoothing coefficient, default:90, range:[0, 100] */
    hi_s16 alpha_psd;           /* noise estimation smoothing factor, default:65, range:[0, 100] */
    hi_s16 prior_snr_fixed;     /* fixed prior signal-to-noise ratio, default:30, range:[1, 499] */
    hi_s16 cep_threshold;       /* cepstrum smooth speech threshold, default:16, range:[0, 100] */
    hi_s16 speech_protect_threshold;    /* speech protect threshold, default:90, range:[0, 100] */
    hi_s16 hem_enable;          /* harmonic enhancement enable, default:1, range:[0, 1] */
    hi_s16 tcs_enable;          /* cepstral smoothing switch, default:0, range:[0, 1] */

    hi_s16 reserved[3];         /* 3: reserved */
} hi_ai_talk_v2_pnr_cfg;

typedef struct {
    hi_bool usr_mode;           /* mode 0: auto, mode 1: mannual. */

    hi_s16 min_gain_limit;      /* maximum allowable noise rejection, default:3276, range:[0, 32767] */
    hi_s16 snr_prior_limit;     /* minimum prior signal-to-noise ratio, default:327, range:[0, 32767] */
    hi_s16 ht_threshold;        /* voice decision threshold, default:10, range:[0, 80] */
    hi_s16 hs_threshold;        /* harmonic detection threshold, default:100, range:[100, 1100] */
    hi_s16 pri_noise_esti_mode; /* single mic noise estimation algorithm mode, default:0, range:[0, 2] */
    hi_s16 prior_snr_fixed;     /* fixed prior signal-to-noise ratio, default:20, range:[1, 499] */
    hi_s16 prio_snr_threshold;  /* instantaneous prior snr, default:7, range:[5, 20] */
    hi_s16 sm_prio_snr_threshold;   /* smooth prior snr, default:10, range:[8, 30] */

    hi_s16 reserved[16];        /* 16: reserved */
} hi_ai_talk_v2_nr_cfg;

typedef struct {
    hi_bool usr_mode;           /* mode 0: auto, mode 1: mannual. */

    hi_s8 target_level;         /* target level, default:-16, range:[-120, 120] */
    hi_u8 max_gain;             /* maximum gain, default:80, range:[0, 240] */
    hi_s8 min_gain;             /* minimum gain, default:-60, range:[-120, 0] */
    hi_s8 adjust_speed;         /* adjustable speed, default:5, range: [1, 10] */

    hi_s16 reserved[3];         /* 3: reserved */
} hi_ai_talk_v2_agc_cfg;

/* Defines the configure parameters of TalkVQEV2. */
typedef struct {
    hi_u32            open_mask;

    hi_s32            work_sample_rate; /* sample rate:16kHz */
    hi_s32            frame_sample;     /* VQE frame length:80-4096 */
    hi_vqe_work_state work_state;

    hi_s32            in_chn_num;
    hi_s32            out_chn_num;

    hi_ai_talk_v2_pnr_cfg   pnr_cfg;
    hi_ai_talk_v2_nr_cfg    nr_cfg;
    hi_ai_talk_v2_eq_cfg    eq_cfg;
    hi_ai_talk_v2_agc_cfg   agc_cfg;

    hi_bool reserved0;
    hi_s16 reserved1[19];   /* 19: reserved num */
    hi_u8 reserved2[8];     /* 8: reserved */
} hi_ai_talk_vqe_v2_cfg;

/* Defines the configure parameters of record VQE. */
typedef struct {
    hi_u32            open_mask;

    hi_s32            work_sample_rate; /* sample rate:16kHz/48kHz */
    hi_s32            frame_sample;     /* VQE frame length:80-4096 */
    hi_vqe_work_state  work_state;

    hi_s32            in_chn_num;
    hi_s32            out_chn_num;
    hi_vqe_record_type   record_type;

    hi_audio_hpf_cfg  hpf_cfg;
    hi_ai_rnr_cfg     rnr_cfg;
    hi_ai_hdr_cfg     hdr_cfg;
    hi_ai_drc_cfg     drc_cfg;
    hi_audio_eq_cfg   eq_cfg;
    hi_audio_agc_cfg  agc_cfg;
} hi_ai_record_vqe_cfg;

/* Defines the configure parameters of talk VQE. */
typedef struct {
    hi_u32            open_mask;

    hi_s32            work_sample_rate; /* sample rate: 8kHz/16kHz. default: 8kHz */
    hi_s32            frame_sample;     /* VQE frame length: 80-4096 */
    hi_vqe_work_state  work_state;

    hi_audio_hpf_cfg  hpf_cfg;
    hi_ai_aec_cfg     aec_cfg;
    hi_audio_anr_cfg  anr_cfg;
    hi_audio_agc_cfg  agc_cfg;
    hi_audio_eq_cfg   eq_cfg;
} hi_ai_talk_vqe_cfg;

typedef struct {
    hi_u32 open_mask;

    hi_s32 work_sample_rate;    /* sample rate: 8kHz/16kHz/48kHz. default: 8kHz */
    hi_s32 frame_sample;        /* VQE frame length: 80-4096 */
    hi_vqe_work_state work_state;

    hi_audio_hpf_cfg hpf_cfg;
    hi_audio_anr_cfg anr_cfg;
    hi_audio_agc_cfg agc_cfg;
    hi_audio_eq_cfg  eq_cfg;
} hi_ao_vqe_cfg;

/* Defines the module register configure of VQE. */
typedef struct {
    hi_void *handle;
} hi_vqe_mod_cfg;

typedef struct {
    hi_vqe_mod_cfg resample_mod_cfg;
    hi_vqe_mod_cfg hpf_mod_cfg;
    hi_vqe_mod_cfg hdr_mod_cfg;
    hi_vqe_mod_cfg gain_mod_cfg;

    /* record VQE */
    hi_vqe_mod_cfg record_mod_cfg;

    /* talk VQE */
    hi_vqe_mod_cfg aec_mod_cfg;
    hi_vqe_mod_cfg anr_mod_cfg;
    hi_vqe_mod_cfg agc_mod_cfg;
    hi_vqe_mod_cfg eq_mod_cfg;

    /* hifi VQE */
    hi_vqe_mod_cfg rnr_mod_cfg;
    hi_vqe_mod_cfg drc_mod_cfg;
    hi_vqe_mod_cfg peq_mod_cfg;

    /* talkv2 VQE */
    hi_vqe_mod_cfg talkv2_mod_cfg;
} hi_audio_vqe_register;

/* Defines the configure parameters of AI saving file. */
typedef struct {
    hi_bool cfg;
    hi_char file_path[HI_MAX_AUDIO_FILE_PATH_LEN];
    hi_char file_name[HI_MAX_AUDIO_FILE_NAME_LEN];
    hi_u32  file_size;  /* in KB */
} hi_audio_save_file_info;

/* Defines whether the file is saving or not . */
typedef struct {
    hi_bool saving;
} hi_audio_file_status;

/* Defines audio clksel type */
typedef enum {
    HI_AUDIO_CLK_SELECT_BASE       = 0,  /* Audio base clk. */
    HI_AUDIO_CLK_SELECT_SPARE,           /* Audio spare clk. */

    HI_AUDIO_CLK_SELECT_BUTT,
} hi_audio_clk_select;

/* Defines audio mode parameter. */
typedef struct {
    hi_audio_clk_select clk_select;
} hi_audio_mod_param;

typedef enum {
    HI_AIO_ERR_VQE_ERR        = 65, /* vqe error */
} hi_aio_err_code;

/* at lease one parameter is illagal ,eg, an illegal enumeration value  */
#define HI_ERR_AIO_ILLEGAL_PARAM    HI_DEFINE_ERR(HI_ID_AIO, HI_ERR_LEVEL_ERROR, HI_ERR_ILLEGAL_PARAM)
/* using a NULL point */
#define HI_ERR_AIO_NULL_PTR         HI_DEFINE_ERR(HI_ID_AIO, HI_ERR_LEVEL_ERROR, HI_ERR_NULL_PTR)
/* operation is not supported by NOW */
#define HI_ERR_AIO_NOT_PERM         HI_DEFINE_ERR(HI_ID_AIO, HI_ERR_LEVEL_ERROR, HI_ERR_NOT_PERM)
/* system is not ready, had not initialed or loaded */
#define HI_ERR_AIO_NOT_READY        HI_DEFINE_ERR(HI_ID_AIO, HI_ERR_LEVEL_ERROR, HI_ERR_NOT_READY)
/* aio is working */
#define HI_ERR_AIO_BUSY             HI_DEFINE_ERR(HI_ID_AIO, HI_ERR_LEVEL_ERROR, HI_ERR_BUSY)
/* vqe register err */
#define HI_ERR_AIO_REGISTER_ERR     HI_DEFINE_ERR(HI_ID_AIO, HI_ERR_LEVEL_ERROR, HI_AIO_ERR_VQE_ERR)

/* invalid device ID */
#define HI_ERR_AI_INVALID_DEV_ID    HI_DEFINE_ERR(HI_ID_AI, HI_ERR_LEVEL_ERROR, HI_ERR_INVALID_DEV_ID)
/* invalid channel ID */
#define HI_ERR_AI_INVALID_CHN_ID    HI_DEFINE_ERR(HI_ID_AI, HI_ERR_LEVEL_ERROR, HI_ERR_INVALID_CHN_ID)
/* at lease one parameter is illagal ,eg, an illegal enumeration value  */
#define HI_ERR_AI_ILLEGAL_PARAM     HI_DEFINE_ERR(HI_ID_AI, HI_ERR_LEVEL_ERROR, HI_ERR_ILLEGAL_PARAM)
/* using a NULL point */
#define HI_ERR_AI_NULL_PTR          HI_DEFINE_ERR(HI_ID_AI, HI_ERR_LEVEL_ERROR, HI_ERR_NULL_PTR)
/* try to enable or initialize system,device or channel, before configing attribute */
#define HI_ERR_AI_NOT_CFG           HI_DEFINE_ERR(HI_ID_AI, HI_ERR_LEVEL_ERROR, HI_ERR_NOT_CFG)
/* operation is not supported by NOW */
#define HI_ERR_AI_NOT_SUPPORT       HI_DEFINE_ERR(HI_ID_AI, HI_ERR_LEVEL_ERROR, HI_ERR_NOT_SUPPORT)
/* operation is not permitted ,eg, try to change static attribute */
#define HI_ERR_AI_NOT_PERM          HI_DEFINE_ERR(HI_ID_AI, HI_ERR_LEVEL_ERROR, HI_ERR_NOT_PERM)
/* the device is not enabled  */
#define HI_ERR_AI_NOT_ENABLED       HI_DEFINE_ERR(HI_ID_AI, HI_ERR_LEVEL_ERROR, HI_ERR_NOT_ENABLE)
/* failure caused by malloc memory */
#define HI_ERR_AI_NO_MEM            HI_DEFINE_ERR(HI_ID_AI, HI_ERR_LEVEL_ERROR, HI_ERR_NO_MEM)
/* failure caused by malloc buffer */
#define HI_ERR_AI_NO_BUF            HI_DEFINE_ERR(HI_ID_AI, HI_ERR_LEVEL_ERROR, HI_ERR_NO_BUF)
/* no data in buffer */
#define HI_ERR_AI_BUF_EMPTY         HI_DEFINE_ERR(HI_ID_AI, HI_ERR_LEVEL_ERROR, HI_ERR_BUF_EMPTY)
/* no buffer for new data */
#define HI_ERR_AI_BUF_FULL          HI_DEFINE_ERR(HI_ID_AI, HI_ERR_LEVEL_ERROR, HI_ERR_BUF_FULL)
/* system is not ready,had not initialed or loaded */
#define HI_ERR_AI_NOT_READY         HI_DEFINE_ERR(HI_ID_AI, HI_ERR_LEVEL_ERROR, HI_ERR_NOT_READY)
/* ai is working */
#define HI_ERR_AI_BUSY              HI_DEFINE_ERR(HI_ID_AI, HI_ERR_LEVEL_ERROR, HI_ERR_BUSY)
/* vqe process err */
#define HI_ERR_AI_VQE_ERR           HI_DEFINE_ERR(HI_ID_AI, HI_ERR_LEVEL_ERROR, HI_AIO_ERR_VQE_ERR)

/* invalid device ID */
#define HI_ERR_AO_INVALID_DEV_ID    HI_DEFINE_ERR(HI_ID_AO, HI_ERR_LEVEL_ERROR, HI_ERR_INVALID_DEV_ID)
/* invalid channel ID */
#define HI_ERR_AO_INVALID_CHN_ID    HI_DEFINE_ERR(HI_ID_AO, HI_ERR_LEVEL_ERROR, HI_ERR_INVALID_CHN_ID)
/* at lease one parameter is illagal ,eg, an illegal enumeration value  */
#define HI_ERR_AO_ILLEGAL_PARAM     HI_DEFINE_ERR(HI_ID_AO, HI_ERR_LEVEL_ERROR, HI_ERR_ILLEGAL_PARAM)
/* using a NULL point */
#define HI_ERR_AO_NULL_PTR          HI_DEFINE_ERR(HI_ID_AO, HI_ERR_LEVEL_ERROR, HI_ERR_NULL_PTR)
/* try to enable or initialize system,device or channel, before configing attribute */
#define HI_ERR_AO_NOT_CFG           HI_DEFINE_ERR(HI_ID_AO, HI_ERR_LEVEL_ERROR, HI_ERR_NOT_CFG)
/* operation is not supported by NOW */
#define HI_ERR_AO_NOT_SUPPORT       HI_DEFINE_ERR(HI_ID_AO, HI_ERR_LEVEL_ERROR, HI_ERR_NOT_SUPPORT)
/* operation is not permitted ,eg, try to change static attribute */
#define HI_ERR_AO_NOT_PERM          HI_DEFINE_ERR(HI_ID_AO, HI_ERR_LEVEL_ERROR, HI_ERR_NOT_PERM)
/* the device is not enabled  */
#define HI_ERR_AO_NOT_ENABLED       HI_DEFINE_ERR(HI_ID_AO, HI_ERR_LEVEL_ERROR, HI_ERR_NOT_ENABLE)
/* failure caused by malloc memory */
#define HI_ERR_AO_NO_MEM            HI_DEFINE_ERR(HI_ID_AO, HI_ERR_LEVEL_ERROR, HI_ERR_NO_MEM)
/* failure caused by malloc buffer */
#define HI_ERR_AO_NO_BUF            HI_DEFINE_ERR(HI_ID_AO, HI_ERR_LEVEL_ERROR, HI_ERR_NO_BUF)
/* no data in buffer */
#define HI_ERR_AO_BUF_EMPTY         HI_DEFINE_ERR(HI_ID_AO, HI_ERR_LEVEL_ERROR, HI_ERR_BUF_EMPTY)
/* no buffer for new data */
#define HI_ERR_AO_BUF_FULL          HI_DEFINE_ERR(HI_ID_AO, HI_ERR_LEVEL_ERROR, HI_ERR_BUF_FULL)
/* system is not ready,had not initialed or loaded */
#define HI_ERR_AO_NOT_READY         HI_DEFINE_ERR(HI_ID_AO, HI_ERR_LEVEL_ERROR, HI_ERR_NOT_READY)

#define HI_ERR_AO_BUSY              HI_DEFINE_ERR(HI_ID_AO, HI_ERR_LEVEL_ERROR, HI_ERR_BUSY)
/* vqe process err */
#define HI_ERR_AO_VQE_ERR           HI_DEFINE_ERR(HI_ID_AO, HI_ERR_LEVEL_ERROR, HI_AIO_ERR_VQE_ERR)
#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */

#endif /* End of #ifndef __HI_COMMON_AIO_H__ */
