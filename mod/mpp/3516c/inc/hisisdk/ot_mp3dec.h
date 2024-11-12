/*
  Copyright (c), 2001-2024, Shenshu Tech. Co., Ltd.
 */

#ifndef OT_MP3DEC_H
#define OT_MP3DEC_H
#include "ot_type.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cpluscplus */

#define OT_MP3DEC_MAX_CHN_NUMS      2  /* mp3 max number of channels */

#define OT_MP3DEC_MAX_OUT_SAMPLES 1152 /* mp3 max output samples per-frame, per-channel */
#define OT_MP3DEC_MIN_BUF_SIZE    4096 /* mp3 minium size of input buffer. UNIT:bytes */

/* Defines MP3DEC Version */
typedef enum {
    OT_MPEG_1 =  0,
    OT_MPEG_2 =  1,
    OT_MPEG_25 = 2
} ot_mpeg_version;

typedef td_void *ot_mp3dec_handle;

/* Defines MP3DEC error */
typedef enum {
    OT_ERR_MP3_NONE =                 0, /* <no decode error */
    OT_ERR_MP3_INDATA_UNDERFLOW =    -1, /* <not enough input data */
    OT_ERR_MP3_MAINDATA_UNDERFLOW =  -2, /* <not enough input main data */
    OT_ERR_MP3_FREE_BITRATE_SYNC =   -3, /* <free mode bit_rate error */
    OT_ERR_MP3_OUT_OF_MEMORY =       -4, /* <decoder not enough memory */
    OT_ERR_MP3_NULL_POINTER =        -5, /* <input null pointer */
    OT_ERR_MP3_INVALID_FRAMEHEADER = -6, /* <invalid frame header */
    OT_ERR_MP3_INVALID_SIDEINFO =    -7, /* <invalid side information */
    OT_ERR_MP3_INVALID_SCALEFACT =   -8, /* <invalid scale factors */
    OT_ERR_MP3_INVALID_HUFFCODES =   -9, /* <Huffman decoder error */
    OT_ERR_MP3_FAIL_SYNC =          -10, /* <find sync word error */

    OT_ERR_MP3_UNKNOWN = -9999 /* <reserved */
} ot_mp3dec_error;

/* Defines MP3DEC frame information */
typedef struct {
    td_s32 bit_rate;        /* <output bit_rate */
    td_s32 chn_num;         /* <output channels,range:1,2 */
    td_s32 sample_rate;     /* <output samplerate */
    td_s32 bits_per_sample; /* <output bitwidth */
    td_s32 output_samples;  /* <output samples,range:chans*sample_per_frame */
    td_s32 layer;           /* <output layer */
    td_s32 version;         /* <output version */
} ot_mp3dec_frame_info;

/*
 * brief create and initial decoder device.
 * attention \n
 * Before deocede,you must call this application programming interface (API) first.
 * param N/A
 * retval ::ot_mp3dec_handle   : Success
 * retval ::NULL          : FAILURE.
 * see \n
 */
ot_mp3dec_handle ot_mp3dec_init_decoder(td_void);

/*
 * brief Free MP3 decoder.
 * attention \n
 * param[in] mp3_decoder MP3decode handle
 * retval \n
 * see \n
 */
td_void ot_mp3dec_free_decoder(ot_mp3dec_handle mp3_decoder);

/*
 * brief Find Sync word before decode.
 * attention \n
 * param[in] mp3_decoder    MP3-Decoder handle
 * param[in] in_buff        address of the pointer of start-point of the bitstream(little endian format)
 * param[in] bytes_left     pointer to bytes left that indicates bitstream numbers at input buffer,
 * indicates the left bytes
 * retval :: other : Success, return number bytes  of current frame
 * retval ::<0 ERR_MP3_INDATA_UNDERFLOW
 * see \n
 */
td_s32 ot_mp3dec_find_sync_header(ot_mp3dec_handle mp3_decoder, td_u8 **in_buff, td_s32 *bytes_left);

/*
 * brief decoding MPEG frame and output 1152(L2/L3) OR 384(L1) 16bit PCM samples per channel.
 * attention \n
 * param[in] mp3_decoder    MP3-Decoder handle
 * param[in] in_buff        address of the pointer of start-point of the bitstream
 * param[in] bytes_left     pointer to bytes left that indicates bitstream numbers at input buffer,
 * indicates the left bytes
 * param[in] out_pcm        the address of the out pcm buffer,pcm data in noninterlaced fotmat: L/L/L/... R/R/R/...
 * param[in] reserved_num   reserved
 * retval :: ERR_MP3_NONE : Success
 * retval :: ERROR_CODE :FAILURE
 * see \n
 */
td_s32 ot_mp3dec_frame(ot_mp3dec_handle mp3_decoder, td_u8 **in_buff, td_s32 *bytes_left, td_s16 *out_pcm,
    td_s32 reserved_num);

/*
 * brief get the frame information.
 * attention \n
 * param[in] mp3_decoder         MP3-Decoder handle
 * param[out] mp3dec_frame_info  frame information
 * retval \n
 * see \n
 */
td_s32 ot_mp3dec_get_last_frame_info(ot_mp3dec_handle mp3_decoder, ot_mp3dec_frame_info *mp3dec_frame_info);

#ifdef __cplusplus
}
#endif /* __cpluscplus */

#endif /* OT_MP3DEC_H */
