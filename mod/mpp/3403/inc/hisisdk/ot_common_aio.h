/*
  Copyright (c), 2001-2022, Shenshu Tech. Co., Ltd.
 */

#ifndef OT_COMMON_AIO_H
#define OT_COMMON_AIO_H

#include "ot_common.h"
#include "ot_errno.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* End of #ifdef __cplusplus */


#define OT_MAX_AUDIO_FRAME_NUM    300      /* max count of audio frame in Buffer */
#define OT_MAX_AUDIO_POINT_BYTES  4        /* max bytes of one sample point(now 32bit max) */

#define OT_MAX_VOICE_POINT_NUM    480      /* max sample per frame for voice encode */

#define OT_MAX_AUDIO_POINT_NUM    2048     /* max sample per frame for all encoder(aacplus:2048) */
#define OT_MAX_AO_POINT_NUM       4096     /* support 4096 framelen */
#define OT_MIN_AUDIO_POINT_NUM    80       /* min sample per frame */
#define OT_MAX_AI_POINT_NUM       2048     /* max sample per frame for all encoder(aacplus:2048) */

#define OT_AUDIO_FRAME_CHN_NUM    2        /* channel number in frame */

/* max length of audio frame by bytes, one frame contain many sample point */
#define OT_MAX_AUDIO_FRAME_LEN    (OT_MAX_AUDIO_POINT_BYTES * OT_MAX_AO_POINT_NUM)

/* max length of audio stream by bytes */
#define OT_MAX_AUDIO_STREAM_LEN   OT_MAX_AUDIO_FRAME_LEN

#define OT_MAX_AI_USRFRM_DEPTH     30      /* max depth of user frame buf */

#define OT_MAX_AUDIO_FILE_PATH_LEN 256
#define OT_MAX_AUDIO_FILE_NAME_LEN 256

/* The VQE EQ Band num. */
#define OT_VQE_EQ_BAND_NUM  10
#define OT_VQE_DRC_SEC_NUM   5

/* The VQE AEC Band num. */
#define OT_AEC_ERL_BAND_NUM       6
#define OT_AEC_ERL_BAND_VALUE_NUM 7

/* The TalkVQEV2 EQ Band num. */
#define OT_TALKVQEV2_EQ_BAND_NUM   30

#define OT_AI_TALKVQEV2_MASK_PNR       0x1
#define OT_AI_TALKVQEV2_MASK_NR        0x2
#define OT_AI_TALKVQEV2_MASK_EQ        0x4
#define OT_AI_TALKVQEV2_MASK_AGC       0x8
#define OT_AI_TALKVQEV2_MASK_DRC       0x10
#define OT_AI_TALKVQEV2_MASK_LIMITER   0x20
#define OT_AI_TALKVQEV2_MASK_DEREVERB  0x40
#define OT_AI_TALKVQEV2_MASK_FMP       0x80
#define OT_AI_TALKVQEV2_MASK_WNR       0x100

#define OT_AI_RECORDVQE_MASK_HPF       0x1
#define OT_AI_RECORDVQE_MASK_RNR       0x2
#define OT_AI_RECORDVQE_MASK_HDR       0x4
#define OT_AI_RECORDVQE_MASK_DRC       0x8
#define OT_AI_RECORDVQE_MASK_EQ        0x10
#define OT_AI_RECORDVQE_MASK_AGC       0x20

#define OT_AI_TALKVQE_MASK_HPF         0x1
#define OT_AI_TALKVQE_MASK_AEC         0x2
#define OT_AI_TALKVQE_MASK_AGC         0x8
#define OT_AI_TALKVQE_MASK_EQ          0x10
#define OT_AI_TALKVQE_MASK_ANR         0x20

#define OT_AO_VQE_MASK_HPF             0x1
#define OT_AO_VQE_MASK_ANR             0x2
#define OT_AO_VQE_MASK_AGC             0x4
#define OT_AO_VQE_MASK_EQ              0x8

typedef enum {
    OT_AUDIO_SAMPLE_RATE_8000   = 8000,    /* 8kHz sample rate */
    OT_AUDIO_SAMPLE_RATE_12000  = 12000,   /* 12kHz sample rate */
    OT_AUDIO_SAMPLE_RATE_11025  = 11025,   /* 11.025kHz sample rate */
    OT_AUDIO_SAMPLE_RATE_16000  = 16000,   /* 16kHz sample rate */
    OT_AUDIO_SAMPLE_RATE_22050  = 22050,   /* 22.05kHz sample rate */
    OT_AUDIO_SAMPLE_RATE_24000  = 24000,   /* 24kHz sample rate */
    OT_AUDIO_SAMPLE_RATE_32000  = 32000,   /* 32kHz sample rate */
    OT_AUDIO_SAMPLE_RATE_44100  = 44100,   /* 44.1kHz sample rate */
    OT_AUDIO_SAMPLE_RATE_48000  = 48000,   /* 48kHz sample rate */
    OT_AUDIO_SAMPLE_RATE_64000  = 64000,   /* 64kHz sample rate */
    OT_AUDIO_SAMPLE_RATE_96000  = 96000,   /* 96kHz sample rate */
    OT_AUDIO_SAMPLE_RATE_BUTT,
} ot_audio_sample_rate;

typedef enum {
    OT_AUDIO_BIT_WIDTH_8   = 0,   /* 8bit width */
    OT_AUDIO_BIT_WIDTH_16  = 1,   /* 16bit width */
    OT_AUDIO_BIT_WIDTH_24  = 2,   /* 24bit width */
    OT_AUDIO_BIT_WIDTH_BUTT,
} ot_audio_bit_width;

typedef enum {
    OT_AIO_MODE_I2S_MASTER  = 0,    /* AIO I2S master mode */
    OT_AIO_MODE_I2S_SLAVE,          /* AIO I2S slave mode */
    OT_AIO_MODE_PCM_SLAVE_STD,      /* AIO PCM slave standard mode */
    OT_AIO_MODE_PCM_SLAVE_NON_STD,  /* AIO PCM slave non-standard mode */
    OT_AIO_MODE_PCM_MASTER_STD,     /* AIO PCM master standard mode */
    OT_AIO_MODE_PCM_MASTER_NON_STD, /* AIO PCM master non-standard mode */
    OT_AIO_MODE_BUTT
} ot_aio_mode;

typedef enum {
    OT_AIO_I2STYPE_INNERCODEC = 0, /* AIO I2S connect inner audio CODEC */
    OT_AIO_I2STYPE_INNERHDMI,      /* AIO I2S connect Inner HDMI */
    OT_AIO_I2STYPE_EXTERN,         /* AIO I2S connect extern hardware */
} ot_aio_i2s_type;

typedef enum {
    OT_AUDIO_SOUND_MODE_MONO   = 0, /* mono */
    OT_AUDIO_SOUND_MODE_STEREO = 1, /* stereo */
    OT_AUDIO_SOUND_MODE_BUTT
} ot_audio_snd_mode;

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
    OT_G726_16K = 0,       /* G726 16kbps, see RFC3551.txt  4.5.4 G726-16 */
    OT_G726_24K,           /* G726 24kbps, see RFC3551.txt  4.5.4 G726-24 */
    OT_G726_32K,           /* G726 32kbps, see RFC3551.txt  4.5.4 G726-32 */
    OT_G726_40K,           /* G726 40kbps, see RFC3551.txt  4.5.4 G726-40 */
    OT_MEDIA_G726_16K,     /* G726 16kbps for ASF ... */
    OT_MEDIA_G726_24K,     /* G726 24kbps for ASF ... */
    OT_MEDIA_G726_32K,     /* G726 32kbps for ASF ... */
    OT_MEDIA_G726_40K,     /* G726 40kbps for ASF ... */
    OT_G726_BUTT,
} ot_g726_bps;

typedef enum {
    /* see DVI4 differs in three respects from the IMA ADPCM at RFC3551.txt 4.5.1 DVI4 */
    OT_ADPCM_TYPE_DVI4 = 0,    /* 32kbps ADPCM(DVI4) for RTP */
    OT_ADPCM_TYPE_IMA,         /* 32kbps ADPCM(IMA),NOTICE:point num must be 161/241/321/481 */
    OT_ADPCM_TYPE_ORG_DVI4,
    OT_ADPCM_TYPE_BUTT,
} ot_adpcm_type;

#define OT_AI_EXPAND  0x01
#define OT_AI_CUT     0x02

typedef struct {
    ot_audio_sample_rate sample_rate;    /* sample rate */
    ot_audio_bit_width   bit_width;      /* bit_width */
    ot_aio_mode          work_mode;      /* master or slave mode */
    ot_audio_snd_mode    snd_mode;       /* mono or stereo */
    td_u32               expand_flag;    /* expand 8bit to 16bit, use OT_AI_EXPAND(only valid for AI 8bit),
                                            use OT_AI_CUT(only valid for extern codec for 24bit) */
    td_u32               frame_num;      /* frame num in buf[2,OT_MAX_AUDIO_FRAME_NUM] */
    td_u32               point_num_per_frame;   /* point num per frame (80/160/240/320/480/1024/2048)
                                                   (ADPCM IMA should add 1 point, AMR only support 160) */
    td_u32               chn_cnt;        /* channel number on FS, valid value:1/2/4/8 */
    td_u32               clk_share;      /* 0: AI and AO clock is separate
                                            1: AI and AO clock is inseparate, AI use AO's clock */
    ot_aio_i2s_type      i2s_type;       /* i2s type */
} ot_aio_attr;

typedef struct {
    td_u32 usr_frame_depth;
} ot_ai_chn_param;

typedef enum {
    OT_AI_CHN_MODE_NORMAL = 0,     /* normal mode for AI chn */
    OT_AI_CHN_MODE_FAST,           /* fast mode for AI chn */
    OT_AI_CHN_MODE_BUTT,
} ot_ai_chn_mode;

typedef struct {
    ot_ai_chn_mode mode;       /* operating mode */
} ot_ai_chn_attr;

typedef struct {
    ot_audio_bit_width   bit_width;     /* audio frame bit_width */
    ot_audio_snd_mode    snd_mode;      /* audio frame mono or stereo mode */
    td_u8 ATTRIBUTE *virt_addr[OT_AUDIO_FRAME_CHN_NUM];
    td_phys_addr_t ATTRIBUTE phys_addr[OT_AUDIO_FRAME_CHN_NUM];
    td_u64  time_stamp;                 /* audio frame time stamp */
    td_u32  seq;                        /* audio frame seq */
    td_u32  len;                        /* data length per channel in frame */
    td_u32  pool_id[OT_AUDIO_FRAME_CHN_NUM];
} ot_audio_frame;

typedef struct {
    ot_audio_frame ref_frame;      /* AEC reference audio frame */
    td_bool     valid;             /* whether frame is valid */
    td_bool     sys_bind;          /* whether is sysbind */
} ot_aec_frame;

typedef struct {
    ot_audio_frame *frame;         /* frame pointer */
    td_u32         id;             /* frame id */
} ot_audio_frame_info;

typedef struct {
    td_u8 ATTRIBUTE *stream;    /* the virtual address of stream */
    td_phys_addr_t ATTRIBUTE phys_addr;  /* the physics address of stream */
    td_u32 len;                 /* stream length, by bytes */
    td_u64 time_stamp;          /* frame time stamp */
    td_u32 seq;                 /* frame seq, if stream is not a valid frame,seq is 0 */
} ot_audio_stream;

typedef struct {
    td_u32 chn_total_num;       /* total number of channel buffer */
    td_u32 chn_free_num;        /* free number of channel buffer */
    td_u32 chn_busy_num;        /* busy number of channel buffer */
} ot_ao_chn_state;

typedef enum {
    OT_AUDIO_TRACK_NORMAL      = 0,
    OT_AUDIO_TRACK_BOTH_LEFT   = 1,
    OT_AUDIO_TRACK_BOTH_RIGHT  = 2,
    OT_AUDIO_TRACK_EXCHANGE    = 3,
    OT_AUDIO_TRACK_MIX         = 4,
    OT_AUDIO_TRACK_LEFT_MUTE   = 5,
    OT_AUDIO_TRACK_RIGHT_MUTE  = 6,
    OT_AUDIO_TRACK_BOTH_MUTE   = 7,

    OT_AUDIO_TRACK_BUTT
} ot_audio_track_mode;

typedef enum {
    OT_AUDIO_FADE_RATE_1   = 0,
    OT_AUDIO_FADE_RATE_2   = 1,
    OT_AUDIO_FADE_RATE_4   = 2,
    OT_AUDIO_FADE_RATE_8   = 3,
    OT_AUDIO_FADE_RATE_16  = 4,
    OT_AUDIO_FADE_RATE_32  = 5,
    OT_AUDIO_FADE_RATE_64  = 6,
    OT_AUDIO_FADE_RATE_128 = 7,

    OT_AUDIO_FADE_RATE_BUTT
} ot_audio_fade_rate;

typedef struct {
    td_bool fade;
    ot_audio_fade_rate fade_in_rate;
    ot_audio_fade_rate fade_out_rate;
} ot_audio_fade;

/* Defines the configure parameters of AEC. */
typedef struct {
    td_bool usr_mode;               /* mode 0: auto mode 1: manual. */
    td_s8 cozy_noisy_mode;          /* cozy noisy mode: 0 close, 1 open, recommend 1 */
    td_s8 near_all_pass_energy;     /* the far-end energy threshold for judging whether unvarnished transmission:
                                       0 -59dBm0, 1 -49dBm0, 2 -39dBm0, recommend 1 */
    td_s8 near_clean_sup_energy;    /* the energy threshold for compelling reset of near-end signal: 0 12dB,
                                       1 15dB, 2 18dB, recommend 2 */

    td_s16 double_talk_threshold;   /* the threshold of judging single or double talk, recommend 16384, [0, 32767] */

    td_s16 echo_band_low;           /* voice processing band1, low frequency parameter, [1, 63) for 8k,
                                       [1, 127) for 16k, recommend 10 */
    td_s16 echo_band_high;          /* voice processing band1, high frequency parameter, (echo_band_low, 63] for 8k,
                                       (echo_band_low, 127] for 16k, recommend 41 */
                                    /* echo_band_high must be greater than echo_band_low */
    td_s16 echo_band_low2;          /* voice processing band2, low frequency parameter,
                                       [1, 63) for 8k, [1, 127) for 16k, recommend 47 */
    td_s16 echo_band_high2;         /* voice processing band2, high frequency parameter, (echo_band_low2, 63] for 8k,
                                       (echo_band_low2, 127] for 16k, recommend 72,
                                       echo_band_high2 must be greater than echo_band_low2 */

    td_s16 erl_band[OT_AEC_ERL_BAND_NUM];    /* ERL protect area, [1, 63] for 8k, [1, 127] for 16k,
                                                frequency band calculated by erl_band * 62.5 */
                                             /* besides, erl_band[n+1] should be greater than erl_band[n] */
    td_s16 erl[OT_AEC_ERL_BAND_VALUE_NUM];   /* ERL protect value of ERL protect area, the smaller its value,
                                                the more strength its protect ability,[0, 18] */
    td_s16 voice_protect_freq_l;    /* protect area of near-end low frequency,
                                       [1, 63) for 8k, [1, 127) for 16k, recommend 3 */
    td_s16 voice_protect_freq_l1;   /* protect area of near-end low frequency1, (voice_protect_freq_l, 63] for 8k,
                                       (voice_protect_freq_l, 127] for 16k, recommend 6 */
    td_s32 reserved;                /* voice_protect_freq_l1 must be greater than voice_protect_freq_l */
} ot_ai_aec_cfg;

/* Defines the configure parameters of ANR. */
typedef struct {
    td_bool usr_mode;       /* mode 0: auto, mode 1: manual. */

    td_s16 nr_intensity;    /* noise reduce intensity, range: [0, 25] */
    td_s16 noise_db_threshold;      /* noise threshold, range: [30, 60] */
    td_s8  music_probe_switch;      /* switch for music probe, range: [0:close, 1:open] */

    td_s32 reserved;
} ot_audio_anr_cfg;

/* Defines the configure parameters of AGC. */
typedef struct {
    td_bool usr_mode;           /* mode 0: auto, mode 1: manual. */

    td_s8 target_level;         /* target voltage level, range: [-40, -1]dB */
    td_s8 noise_floor;          /* noise floor, range: TalkVQE/AoVQE[-65, -20]dB, RecordVQE[-50, -20]dB */
    td_s8 max_gain;             /* max gain, range: [0, 30]dB */
    td_s8 adjust_speed;         /* adjustable speed, range: [0, 10]dB/s */

    td_s8 improve_snr;          /* switch for improving SNR, range: [0:close, 1:upper limit 3dB, 2:upper limit 6dB] */
    td_s8 use_hpf;              /* switch for using high pass filt,
                                   range: [0:close, 1:80Hz, 2:120Hz, 3:150Hz, 4:300Hz, 5:500Hz] */
    td_s8 output_mode;          /* output mode, mute when lower than noise floor, range: [0:close, 1:open] */
    td_s16 noise_suppress_switch;    /* switch for noise suppression, range: [0:close, 1:open] */

    td_s32 reserved;
} ot_audio_agc_cfg;

/* Defines the configure parameters of HPF. */
typedef enum {
    OT_AUDIO_HPF_FREQ_80   = 80,    /* 80Hz */
    OT_AUDIO_HPF_FREQ_120  = 120,   /* 120Hz */
    OT_AUDIO_HPF_FREQ_150  = 150,   /* 150Hz */
    OT_AUDIO_HPF_FREQ_BUTT,
} ot_audio_hpf_freq;

typedef struct {
    td_bool usr_mode;              /* mode 0: auto mode 1: manual. */
    ot_audio_hpf_freq hpf_freq;    /* freq to be processed, value:HiFiVQE/TalkVQE/AoVQE(80Hz, 120Hz, 150Hz),
                                      RecordVQE(80Hz) */
} ot_audio_hpf_cfg;

typedef struct {
    td_bool usr_mode;       /* mode 0: auto, mode 1: manual. */
    td_s32  nr_mode;        /* mode 0: floor noise; 1:ambient noise */
    td_s32  max_nr_level;   /* max NR level range:[2,20]dB */
    td_s32  noise_threshold;   /* noise threshold, range:[-80, -20] */
} ot_ai_rnr_cfg;

typedef struct {
    td_s8  gain_db[OT_VQE_EQ_BAND_NUM];   /* EQ band, include 100, 200, 250, 350, 500, 800, 1.2k, 2.5k, 4k, 8k in turn,
                                             range:TalkVQE/AoVQE[-100, 20], RecordVQE[-50, 20] */
    td_s32 reserved;
} ot_audio_eq_cfg;

/* Defines the configure parameters of UPVQE work state. */
typedef enum {
    OT_VQE_WORK_STATE_COMMON  = 0,   /* common environment, Applicable to the family of voice calls. */
    OT_VQE_WORK_STATE_MUSIC   = 1,   /* music environment , Applicable to the family of music environment. */
    OT_VQE_WORK_STATE_NOISY   = 2,   /* noisy environment , Applicable to the noisy voice calls.  */
} ot_vqe_work_state;

/* Defines record type */
typedef enum {
    OT_VQE_RECORD_NORMAL      = 0,  /* double micphone recording. */
    OT_VQE_RECORD_BUTT,
} ot_vqe_record_type;

/* HDR Set CODEC GAIN Function Handle type */
typedef td_s32 (*func_gain_callback)(td_s32 set_gain);

typedef struct {
    td_bool usr_mode;               /* mode 0: auto mode 1: manual. */

    td_s32 min_gain_db;             /* the minimum of MIC(AI) CODEC gain, [0, 120] */
    td_s32 max_gain_db;             /* the maximum of MIC(AI) CODEC gain, [0, 120] */

    td_s32 mic_gain_db;             /* the current gain of MIC(AI) CODEC,[min_gain_db, max_gain_db] */
    td_s32 mic_gain_step_db;        /* the step size of gain adjustment, [1, 3], recommend 2 */
    func_gain_callback callback;    /* the callback function pointer of CODEC gain adjustment */
} ot_ai_hdr_cfg;

typedef struct {
    td_bool usr_mode;       /* enable user mode or not,default 0: disable user mode,1: user mode. */

    td_s16  attack_time;    /* time of signal change from large to small (ms), range:HiFiVQE[10, 250]ms,
                               RecordVQE[10, 126]ms */
    td_s16  release_time;   /* time of signal change from small to large (ms), range:HiFiVQE[10, 250]ms,
                               RecordVQE[10, 126]ms */

    td_s16 old_level_db[OT_VQE_DRC_SEC_NUM];  /* old curve level(dB), default[0, -472, -792, -960, -1280],
                                                 range:[-1440, 0]ms,store from big to small,scale:Q4 */
    td_s16 new_level_db[OT_VQE_DRC_SEC_NUM];  /* new curve level(dB), default[0, -174, -410, -608, -1021],
                                                 range:[-1440, 0]ms,store from big to small,scale:Q4 */
} ot_ai_drc_cfg;

typedef struct {
    td_bool usr_mode;                           /* mode 0: auto, mode 1: manual. */
    td_s8  gain_db[OT_TALKVQEV2_EQ_BAND_NUM];   /* EQ band, range:[-50, 20]dB */
} ot_ai_talk_v2_eq_cfg;

typedef struct {
    td_bool usr_mode;           /* mode 0: auto, mode 1: manual. */

    td_s16 min_gain_limit;      /* maximum allowable noise rejection, default:5827, range:[0, 32767] */
    td_s16 snr_prior_limit;     /* minimum prior signal-to-noise ratio, default:3276, range:[0, 32767] */
    td_s16 ht_threshold;        /* voice decision threshold, default:10, range:[0, 80] */
    td_s16 hs_threshold;        /* harmonic detection threshold, default:100, range:[100, 1100] */
    td_s16 alpha_ph;            /* speech probability smoothing coefficient, default:90, range:[0, 100] */
    td_s16 alpha_psd;           /* noise estimation smoothing factor, default:65, range:[0, 100] */
    td_s16 prior_snr_fixed;     /* fixed prior signal-to-noise ratio, default:30, range:[1, 499] */
    td_s16 cep_threshold;       /* cepstrum smooth speech threshold, default:16, range:[0, 100] */
    td_s16 speech_protect_threshold;    /* speech protect threshold, default:90, range:[0, 100] */
    td_s16 hem_enable;          /* harmonic enhancement enable, default:1, range:[0, 1] */
    td_s16 tcs_enable;          /* cepstral smoothing switch, default:0, range:[0, 1] */

    td_s16 reserved[3];         /* 3: reserved */
} ot_ai_talk_v2_pnr_cfg;

typedef struct {
    td_bool usr_mode;           /* mode 0: auto, mode 1: manual. */

    td_s16 min_gain_limit;      /* maximum allowable noise rejection, default:3276, range:[0, 32767] */
    td_s16 snr_prior_limit;     /* minimum prior signal-to-noise ratio, default:327, range:[0, 32767] */
    td_s16 ht_threshold;        /* voice decision threshold, default:10, range:[0, 80] */
    td_s16 hs_threshold;        /* harmonic detection threshold, default:100, range:[100, 1100] */
    td_s16 pri_noise_esti_mode; /* single mic noise estimation algorithm mode, default:0, range:[0, 2] */
    td_s16 prior_snr_fixed;     /* fixed prior signal-to-noise ratio, default:20, range:[1, 499] */
    td_s16 prio_snr_threshold;  /* instantaneous prior snr, default:7, range:[5, 20] */
    td_s16 sm_prio_snr_threshold;   /* smooth prior snr, default:10, range:[8, 30] */

    td_s16 reserved[16];        /* 16: reserved */
} ot_ai_talk_v2_nr_cfg;

typedef struct {
    td_bool usr_mode;           /* mode 0: auto, mode 1: manual. */

    td_s8 target_level;         /* target level, default:-16, range:[-120, 120] */
    td_u8 max_gain;             /* maximum gain, default:80, range:[0, 240] */
    td_s8 min_gain;             /* minimum gain, default:-60, range:[-120, 0] */
    td_s8 adjust_speed;         /* adjustable speed, default:5, range: [1, 10] */

    td_s16 reserved[3];         /* 3: reserved */
} ot_ai_talk_v2_agc_cfg;

typedef struct {
    td_s8 reserved[6];          /* 6: reserved */
} ot_ai_talk_v2_wnr_cfg;

/* Defines the configure parameters of TalkVQEV2. */
typedef struct {
    td_u32            open_mask;

    td_s32            work_sample_rate; /* sample rate:16kHz */
    td_s32            frame_sample;     /* VQE frame length:80-4096 */
    ot_vqe_work_state work_state;

    td_s32            in_chn_num;
    td_s32            out_chn_num;

    ot_ai_talk_v2_pnr_cfg   pnr_cfg;
    ot_ai_talk_v2_nr_cfg    nr_cfg;
    ot_ai_talk_v2_eq_cfg    eq_cfg;
    ot_ai_talk_v2_agc_cfg   agc_cfg;

    td_bool reserved0;
    td_s16 reserved1[19];   /* 19: reserved num */
    td_u8 reserved2[8];     /* 8: reserved */

    ot_ai_talk_v2_wnr_cfg   wnr_cfg;
} ot_ai_talk_vqe_v2_cfg;

/* Defines the configure parameters of record VQE. */
typedef struct {
    td_u32            open_mask;

    td_s32            work_sample_rate; /* sample rate:16kHz/48kHz */
    td_s32            frame_sample;     /* VQE frame length:80-4096 */
    ot_vqe_work_state  work_state;

    td_s32            in_chn_num;
    td_s32            out_chn_num;
    ot_vqe_record_type   record_type;

    ot_audio_hpf_cfg  hpf_cfg;
    ot_ai_rnr_cfg     rnr_cfg;
    ot_ai_hdr_cfg     hdr_cfg;
    ot_ai_drc_cfg     drc_cfg;
    ot_audio_eq_cfg   eq_cfg;
    ot_audio_agc_cfg  agc_cfg;
} ot_ai_record_vqe_cfg;

/* Defines the configure parameters of talk VQE. */
typedef struct {
    td_u32            open_mask;

    td_s32            work_sample_rate; /* sample rate: 8kHz/16kHz. default: 8kHz */
    td_s32            frame_sample;     /* VQE frame length: 80-4096 */
    ot_vqe_work_state  work_state;

    ot_audio_hpf_cfg  hpf_cfg;
    ot_ai_aec_cfg     aec_cfg;
    ot_audio_anr_cfg  anr_cfg;
    ot_audio_agc_cfg  agc_cfg;
    ot_audio_eq_cfg   eq_cfg;
} ot_ai_talk_vqe_cfg;

typedef struct {
    td_u32 open_mask;

    td_s32 work_sample_rate;    /* sample rate: 8kHz/16kHz/48kHz. default: 8kHz */
    td_s32 frame_sample;        /* VQE frame length: 80-4096 */
    ot_vqe_work_state work_state;

    ot_audio_hpf_cfg hpf_cfg;
    ot_audio_anr_cfg anr_cfg;
    ot_audio_agc_cfg agc_cfg;
    ot_audio_eq_cfg  eq_cfg;
} ot_ao_vqe_cfg;

/* Defines the module register configure of VQE. */
typedef struct {
    td_void *handle;
} ot_vqe_mod_cfg;

typedef struct {
    ot_vqe_mod_cfg resample_mod_cfg;
    ot_vqe_mod_cfg hpf_mod_cfg;
    ot_vqe_mod_cfg hdr_mod_cfg;
    ot_vqe_mod_cfg gain_mod_cfg;

    /* record VQE */
    ot_vqe_mod_cfg record_mod_cfg;

    /* talk VQE */
    ot_vqe_mod_cfg aec_mod_cfg;
    ot_vqe_mod_cfg anr_mod_cfg;
    ot_vqe_mod_cfg agc_mod_cfg;
    ot_vqe_mod_cfg eq_mod_cfg;

    /* hifi VQE */
    ot_vqe_mod_cfg rnr_mod_cfg;
    ot_vqe_mod_cfg drc_mod_cfg;
    ot_vqe_mod_cfg peq_mod_cfg;

    /* talkv2 VQE */
    ot_vqe_mod_cfg talkv2_mod_cfg;
    ot_vqe_mod_cfg wnr_mod_cfg;
} ot_audio_vqe_register;

/* Defines the configure parameters of AI saving file. */
typedef struct {
    td_bool cfg;
    td_char file_path[OT_MAX_AUDIO_FILE_PATH_LEN];
    td_char file_name[OT_MAX_AUDIO_FILE_NAME_LEN];
    td_u32  file_size;  /* in KB */
} ot_audio_save_file_info;

/* Defines whether the file is saving or not . */
typedef struct {
    td_bool saving;
} ot_audio_file_status;

/* Defines audio clksel type */
typedef enum {
    OT_AUDIO_CLK_SELECT_BASE       = 0,  /* Audio base clk. */
    OT_AUDIO_CLK_SELECT_SPARE,           /* Audio spare clk. */

    OT_AUDIO_CLK_SELECT_BUTT,
} ot_audio_clk_select;

/* Defines audio mode parameter. */
typedef struct {
    ot_audio_clk_select clk_select;
} ot_audio_mod_param;

typedef enum {
    OT_AIO_ERR_VQE_ERR        = 65, /* vqe error */
} ot_aio_err_code;

/* at lease one parameter is illegal, eg, an illegal enumeration value */
#define OT_ERR_AIO_ILLEGAL_PARAM    OT_DEFINE_ERR(OT_ID_AIO, OT_ERR_LEVEL_ERROR, OT_ERR_ILLEGAL_PARAM)
/* using a NULL pointer */
#define OT_ERR_AIO_NULL_PTR         OT_DEFINE_ERR(OT_ID_AIO, OT_ERR_LEVEL_ERROR, OT_ERR_NULL_PTR)
/* operation is not supported by now */
#define OT_ERR_AIO_NOT_PERM         OT_DEFINE_ERR(OT_ID_AIO, OT_ERR_LEVEL_ERROR, OT_ERR_NOT_PERM)
/* system is not ready, had not initialed or loaded */
#define OT_ERR_AIO_NOT_READY        OT_DEFINE_ERR(OT_ID_AIO, OT_ERR_LEVEL_ERROR, OT_ERR_NOT_READY)
/* aio is working */
#define OT_ERR_AIO_BUSY             OT_DEFINE_ERR(OT_ID_AIO, OT_ERR_LEVEL_ERROR, OT_ERR_BUSY)
/* vqe register err */
#define OT_ERR_AIO_REGISTER_ERR     OT_DEFINE_ERR(OT_ID_AIO, OT_ERR_LEVEL_ERROR, OT_AIO_ERR_VQE_ERR)

/* invalid device ID */
#define OT_ERR_AI_INVALID_DEV_ID    OT_DEFINE_ERR(OT_ID_AI, OT_ERR_LEVEL_ERROR, OT_ERR_INVALID_DEV_ID)
/* invalid channel ID */
#define OT_ERR_AI_INVALID_CHN_ID    OT_DEFINE_ERR(OT_ID_AI, OT_ERR_LEVEL_ERROR, OT_ERR_INVALID_CHN_ID)
/* at lease one parameter is illegal, eg, an illegal enumeration value */
#define OT_ERR_AI_ILLEGAL_PARAM     OT_DEFINE_ERR(OT_ID_AI, OT_ERR_LEVEL_ERROR, OT_ERR_ILLEGAL_PARAM)
/* using a null pointer */
#define OT_ERR_AI_NULL_PTR          OT_DEFINE_ERR(OT_ID_AI, OT_ERR_LEVEL_ERROR, OT_ERR_NULL_PTR)
/* try to enable or initialize system, device or channel, before configing attribute */
#define OT_ERR_AI_NOT_CFG           OT_DEFINE_ERR(OT_ID_AI, OT_ERR_LEVEL_ERROR, OT_ERR_NOT_CFG)
/* operation is not supported by now */
#define OT_ERR_AI_NOT_SUPPORT       OT_DEFINE_ERR(OT_ID_AI, OT_ERR_LEVEL_ERROR, OT_ERR_NOT_SUPPORT)
/* operation is not permitted, eg, try to change static attribute */
#define OT_ERR_AI_NOT_PERM          OT_DEFINE_ERR(OT_ID_AI, OT_ERR_LEVEL_ERROR, OT_ERR_NOT_PERM)
/* the device is not enabled */
#define OT_ERR_AI_NOT_ENABLED       OT_DEFINE_ERR(OT_ID_AI, OT_ERR_LEVEL_ERROR, OT_ERR_NOT_ENABLE)
/* failure caused by malloc memory */
#define OT_ERR_AI_NO_MEM            OT_DEFINE_ERR(OT_ID_AI, OT_ERR_LEVEL_ERROR, OT_ERR_NO_MEM)
/* failure caused by malloc buffer */
#define OT_ERR_AI_NO_BUF            OT_DEFINE_ERR(OT_ID_AI, OT_ERR_LEVEL_ERROR, OT_ERR_NO_BUF)
/* no data in buffer */
#define OT_ERR_AI_BUF_EMPTY         OT_DEFINE_ERR(OT_ID_AI, OT_ERR_LEVEL_ERROR, OT_ERR_BUF_EMPTY)
/* no buffer for new data */
#define OT_ERR_AI_BUF_FULL          OT_DEFINE_ERR(OT_ID_AI, OT_ERR_LEVEL_ERROR, OT_ERR_BUF_FULL)
/* system is not ready, had not initialed or loaded */
#define OT_ERR_AI_NOT_READY         OT_DEFINE_ERR(OT_ID_AI, OT_ERR_LEVEL_ERROR, OT_ERR_NOT_READY)
/* ai is working */
#define OT_ERR_AI_BUSY              OT_DEFINE_ERR(OT_ID_AI, OT_ERR_LEVEL_ERROR, OT_ERR_BUSY)
/* vqe process err */
#define OT_ERR_AI_VQE_ERR           OT_DEFINE_ERR(OT_ID_AI, OT_ERR_LEVEL_ERROR, OT_AIO_ERR_VQE_ERR)

/* invalid device ID */
#define OT_ERR_AO_INVALID_DEV_ID    OT_DEFINE_ERR(OT_ID_AO, OT_ERR_LEVEL_ERROR, OT_ERR_INVALID_DEV_ID)
/* invalid channel ID */
#define OT_ERR_AO_INVALID_CHN_ID    OT_DEFINE_ERR(OT_ID_AO, OT_ERR_LEVEL_ERROR, OT_ERR_INVALID_CHN_ID)
/* at lease one parameter is illegal, eg, an illegal enumeration value */
#define OT_ERR_AO_ILLEGAL_PARAM     OT_DEFINE_ERR(OT_ID_AO, OT_ERR_LEVEL_ERROR, OT_ERR_ILLEGAL_PARAM)
/* using a null pointer */
#define OT_ERR_AO_NULL_PTR          OT_DEFINE_ERR(OT_ID_AO, OT_ERR_LEVEL_ERROR, OT_ERR_NULL_PTR)
/* try to enable or initialize system, device or channel, before configing attribute */
#define OT_ERR_AO_NOT_CFG           OT_DEFINE_ERR(OT_ID_AO, OT_ERR_LEVEL_ERROR, OT_ERR_NOT_CFG)
/* operation is not supported by now */
#define OT_ERR_AO_NOT_SUPPORT       OT_DEFINE_ERR(OT_ID_AO, OT_ERR_LEVEL_ERROR, OT_ERR_NOT_SUPPORT)
/* operation is not permitted, eg, try to change static attribute */
#define OT_ERR_AO_NOT_PERM          OT_DEFINE_ERR(OT_ID_AO, OT_ERR_LEVEL_ERROR, OT_ERR_NOT_PERM)
/* the device is not enabled */
#define OT_ERR_AO_NOT_ENABLED       OT_DEFINE_ERR(OT_ID_AO, OT_ERR_LEVEL_ERROR, OT_ERR_NOT_ENABLE)
/* failure caused by malloc memory */
#define OT_ERR_AO_NO_MEM            OT_DEFINE_ERR(OT_ID_AO, OT_ERR_LEVEL_ERROR, OT_ERR_NO_MEM)
/* failure caused by malloc buffer */
#define OT_ERR_AO_NO_BUF            OT_DEFINE_ERR(OT_ID_AO, OT_ERR_LEVEL_ERROR, OT_ERR_NO_BUF)
/* no data in buffer */
#define OT_ERR_AO_BUF_EMPTY         OT_DEFINE_ERR(OT_ID_AO, OT_ERR_LEVEL_ERROR, OT_ERR_BUF_EMPTY)
/* no buffer for new data */
#define OT_ERR_AO_BUF_FULL          OT_DEFINE_ERR(OT_ID_AO, OT_ERR_LEVEL_ERROR, OT_ERR_BUF_FULL)
/* system is not ready,had not initialed or loaded */
#define OT_ERR_AO_NOT_READY         OT_DEFINE_ERR(OT_ID_AO, OT_ERR_LEVEL_ERROR, OT_ERR_NOT_READY)

#define OT_ERR_AO_BUSY              OT_DEFINE_ERR(OT_ID_AO, OT_ERR_LEVEL_ERROR, OT_ERR_BUSY)
/* vqe process err */
#define OT_ERR_AO_VQE_ERR           OT_DEFINE_ERR(OT_ID_AO, OT_ERR_LEVEL_ERROR, OT_AIO_ERR_VQE_ERR)
#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */

#endif /* End of #ifndef OT_COMMON_AIO_H */
