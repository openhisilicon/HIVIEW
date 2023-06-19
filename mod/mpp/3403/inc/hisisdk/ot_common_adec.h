/*
  Copyright (c), 2001-2022, Shenshu Tech. Co., Ltd.
 */

#ifndef  OT_COMMON_ADEC_H
#define  OT_COMMON_ADEC_H

#include "ot_type.h"
#include "ot_common.h"
#include "ot_common_aio.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* End of #ifdef __cplusplus */

#define OT_MAX_DECODER_NAME_LEN 17

typedef struct {
    td_u32 reserved;
} ot_adec_attr_g711;

typedef struct {
    ot_g726_bps g726bps;
} ot_adec_attr_g726;

typedef struct {
    ot_adpcm_type adpcm_type;
} ot_adec_attr_adpcm;

typedef struct {
    td_u32 reserved;
} ot_adec_attr_lpcm;

typedef enum {
    OT_ADEC_MODE_PACK = 0, /* require input is valid dec pack(a
                              complete frame encode result),
                              e.g.the stream get from AENC is a
                              valid dec pack, the stream know actually
                              pack len from file is also a dec pack.
                              this mode is high-performative */
    OT_ADEC_MODE_STREAM,  /* input is stream, low-performative,
                             if you couldn't find out whether a stream is
                             valid dec pack,you could use
                             this mode */
    OT_ADEC_MODE_BUTT
} ot_adec_mode;

typedef struct {
    ot_payload_type type;
    td_u32          buf_size;   /* buf size[2~OT_MAX_AUDIO_FRAME_NUM] */
    ot_adec_mode    mode;       /* decode mode */
    td_void ATTRIBUTE *value;
} ot_adec_chn_attr;

typedef struct {
    td_bool end_of_stream;      /* EOS flag */
    td_u32  buf_total_num;   /* total number of channel buffer */
    td_u32  buf_free_num;    /* free number of channel buffer */
    td_u32  buf_busy_num;    /* busy number of channel buffer */
} ot_adec_chn_state;

typedef struct {
    ot_payload_type type;
    td_char         name[OT_MAX_DECODER_NAME_LEN];

    td_s32  (*func_open_decoder)(td_void *decoder_attr, td_void **decoder); /* struct decoder is packed by user,
                                                                               user malloc and free memory
                                                                               for this struct */
    td_s32  (*func_dec_frame)(td_void *decoder, td_u8 **in_buf, td_s32 *left_byte,
                              td_u16 *out_buf, td_u32 *out_len, td_u32 *chns);
    td_s32  (*func_get_frame_info)(td_void *decoder, td_void *info);
    td_s32  (*func_close_decoder)(td_void *decoder);
    td_s32  (*func_reset_decoder)(td_void *decoder);
} ot_adec_decoder;

typedef enum {
    OT_ADEC_ERR_DECODER_ERR = 64,
    OT_ADEC_ERR_BUF_LACK,
} ot_adec_err_code;

/* invalid device ID */
#define OT_ERR_ADEC_INVALID_DEV_ID    OT_DEFINE_ERR(OT_ID_ADEC, OT_ERR_LEVEL_ERROR, OT_ERR_INVALID_DEV_ID)
/* invalid channel ID */
#define OT_ERR_ADEC_INVALID_CHN_ID    OT_DEFINE_ERR(OT_ID_ADEC, OT_ERR_LEVEL_ERROR, OT_ERR_INVALID_CHN_ID)
/* at lease one parameter is illegal, eg, an illegal enumeration value */
#define OT_ERR_ADEC_ILLEGAL_PARAM     OT_DEFINE_ERR(OT_ID_ADEC, OT_ERR_LEVEL_ERROR, OT_ERR_ILLEGAL_PARAM)
/* channel exists */
#define OT_ERR_ADEC_EXIST             OT_DEFINE_ERR(OT_ID_ADEC, OT_ERR_LEVEL_ERROR, OT_ERR_EXIST)
/* channel unexist */
#define OT_ERR_ADEC_UNEXIST           OT_DEFINE_ERR(OT_ID_ADEC, OT_ERR_LEVEL_ERROR, OT_ERR_UNEXIST)
/* using a null pointer */
#define OT_ERR_ADEC_NULL_PTR          OT_DEFINE_ERR(OT_ID_ADEC, OT_ERR_LEVEL_ERROR, OT_ERR_NULL_PTR)
/* try to enable or initialize system, device or channel, before configing attribute */
#define OT_ERR_ADEC_NOT_CFG           OT_DEFINE_ERR(OT_ID_ADEC, OT_ERR_LEVEL_ERROR, OT_ERR_NOT_CFG)
/* operation is not supported by now */
#define OT_ERR_ADEC_NOT_SUPPORT       OT_DEFINE_ERR(OT_ID_ADEC, OT_ERR_LEVEL_ERROR, OT_ERR_NOT_SUPPORT)
/* operation is not permitted, eg, try to change static attribute */
#define OT_ERR_ADEC_NOT_PERM          OT_DEFINE_ERR(OT_ID_ADEC, OT_ERR_LEVEL_ERROR, OT_ERR_NOT_PERM)
/* failure caused by malloc memory */
#define OT_ERR_ADEC_NO_MEM            OT_DEFINE_ERR(OT_ID_ADEC, OT_ERR_LEVEL_ERROR, OT_ERR_NO_MEM)
/* failure caused by malloc buffer */
#define OT_ERR_ADEC_NO_BUF            OT_DEFINE_ERR(OT_ID_ADEC, OT_ERR_LEVEL_ERROR, OT_ERR_NO_BUF)
/* no data in buffer */
#define OT_ERR_ADEC_BUF_EMPTY         OT_DEFINE_ERR(OT_ID_ADEC, OT_ERR_LEVEL_ERROR, OT_ERR_BUF_EMPTY)
/* no buffer for new data */
#define OT_ERR_ADEC_BUF_FULL          OT_DEFINE_ERR(OT_ID_ADEC, OT_ERR_LEVEL_ERROR, OT_ERR_BUF_FULL)
/* system is not ready, had not initialed or loaded */
#define OT_ERR_ADEC_NOT_READY         OT_DEFINE_ERR(OT_ID_ADEC, OT_ERR_LEVEL_ERROR, OT_ERR_NOT_READY)
/* decoder internal err */
#define OT_ERR_ADEC_DECODER_ERR       OT_DEFINE_ERR(OT_ID_ADEC, OT_ERR_LEVEL_ERROR, OT_ADEC_ERR_DECODER_ERR)
/* input buffer not enough to decode one frame */
#define OT_ERR_ADEC_BUF_LACK          OT_DEFINE_ERR(OT_ID_ADEC, OT_ERR_LEVEL_ERROR, OT_ADEC_ERR_BUF_LACK)
#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */

#endif /* End of #ifndef OT_COMMON_ADEC_H */
