/*
 * Copyright (c) Hisilicon Technologies Co., Ltd. 2019-2020. All rights reserved.
 * Description: hi_common_adec.h
 * Author: Hisilicon multimedia software group
 * Create: 2019/06/15
 * History                 :
 *  1.Date                 :   2019/06/15
 *    Modification         :   Created file
 */

#ifndef  __HI_COMMON_ADEC_H__
#define  __HI_COMMON_ADEC_H__

#include "hi_type.h"
#include "hi_common.h"
#include "hi_common_aio.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* End of #ifdef __cplusplus */

#define HI_MAX_DECODER_NAME_LEN 17

typedef struct {
    hi_u32 reserved;
} hi_adec_attr_g711;

typedef struct {
    hi_g726_bps g726bps;
} hi_adec_attr_g726;

typedef struct {
    hi_adpcm_type adpcm_type;
} hi_adec_attr_adpcm;

typedef struct {
    hi_u32 reserved;
} hi_adec_attr_lpcm;

typedef enum {
    HI_ADEC_MODE_PACK = 0, /* require input is valid dec pack(a
                              complete frame encode result),
                              e.g.the stream get from AENC is a
                              valid dec pack, the stream know actually
                              pack len from file is also a dec pack.
                              this mode is high-performative */
    HI_ADEC_MODE_STREAM,  /* input is stream, low-performative,
                             if you couldn't find out whether a stream is
                             vaild dec pack,you could use
                             this mode */
    HI_ADEC_MODE_BUTT
} hi_adec_mode;

typedef struct {
    hi_payload_type type;
    hi_u32          buf_size;   /* buf size[2~HI_MAX_AUDIO_FRAME_NUM] */
    hi_adec_mode    mode;       /* decode mode */
    hi_void ATTRIBUTE *value;
} hi_adec_chn_attr;

typedef struct {
    hi_bool end_of_stream;      /* EOS flag */
    hi_u32  buf_total_num;   /* total number of channel buffer */
    hi_u32  buf_free_num;    /* free number of channel buffer */
    hi_u32  buf_busy_num;    /* busy number of channel buffer */
} hi_adec_chn_state;

typedef struct {
    hi_payload_type type;
    hi_char         name[HI_MAX_DECODER_NAME_LEN];

    hi_s32  (*func_open_decoder)(hi_void *decoder_attr, hi_void **decoder); /* struct decoder is packed by user,
                                                                               user malloc and free memory
                                                                               for this struct */
    hi_s32  (*func_dec_frame)(hi_void *decoder, hi_u8 **in_buf, hi_s32 *left_byte,
                              hi_u16 *out_buf, hi_u32 *out_len, hi_u32 *chns);
    hi_s32  (*func_get_frame_info)(hi_void *decoder, hi_void *info);
    hi_s32  (*func_close_decoder)(hi_void *decoder);
    hi_s32  (*func_reset_decoder)(hi_void *decoder);
} hi_adec_decoder;

typedef enum {
    HI_ADEC_ERR_DECODER_ERR = 64,
    HI_ADEC_ERR_BUF_LACK,
} hi_adec_err_code;

/* invalid device ID */
#define HI_ERR_ADEC_INVALID_DEV_ID    HI_DEFINE_ERR(HI_ID_ADEC, HI_ERR_LEVEL_ERROR, HI_ERR_INVALID_DEV_ID)
/* invalid channel ID */
#define HI_ERR_ADEC_INVALID_CHN_ID    HI_DEFINE_ERR(HI_ID_ADEC, HI_ERR_LEVEL_ERROR, HI_ERR_INVALID_CHN_ID)
/* at lease one parameter is illagal ,eg, an illegal enumeration value  */
#define HI_ERR_ADEC_ILLEGAL_PARAM     HI_DEFINE_ERR(HI_ID_ADEC, HI_ERR_LEVEL_ERROR, HI_ERR_ILLEGAL_PARAM)
/* channel exists */
#define HI_ERR_ADEC_EXIST             HI_DEFINE_ERR(HI_ID_ADEC, HI_ERR_LEVEL_ERROR, HI_ERR_EXIST)
/* channel unexists */
#define HI_ERR_ADEC_UNEXIST           HI_DEFINE_ERR(HI_ID_ADEC, HI_ERR_LEVEL_ERROR, HI_ERR_UNEXIST)
/* using a NULL point */
#define HI_ERR_ADEC_NULL_PTR          HI_DEFINE_ERR(HI_ID_ADEC, HI_ERR_LEVEL_ERROR, HI_ERR_NULL_PTR)
/* try to enable or initialize system,device or channel, before configing attribute */
#define HI_ERR_ADEC_NOT_CFG           HI_DEFINE_ERR(HI_ID_ADEC, HI_ERR_LEVEL_ERROR, HI_ERR_NOT_CFG)
/* operation is not supported by NOW */
#define HI_ERR_ADEC_NOT_SUPPORT       HI_DEFINE_ERR(HI_ID_ADEC, HI_ERR_LEVEL_ERROR, HI_ERR_NOT_SUPPORT)
/* operation is not permitted ,eg, try to change static attribute */
#define HI_ERR_ADEC_NOT_PERM          HI_DEFINE_ERR(HI_ID_ADEC, HI_ERR_LEVEL_ERROR, HI_ERR_NOT_PERM)
/* failure caused by malloc memory */
#define HI_ERR_ADEC_NO_MEM            HI_DEFINE_ERR(HI_ID_ADEC, HI_ERR_LEVEL_ERROR, HI_ERR_NO_MEM)
/* failure caused by malloc buffer */
#define HI_ERR_ADEC_NO_BUF            HI_DEFINE_ERR(HI_ID_ADEC, HI_ERR_LEVEL_ERROR, HI_ERR_NO_BUF)
/* no data in buffer */
#define HI_ERR_ADEC_BUF_EMPTY         HI_DEFINE_ERR(HI_ID_ADEC, HI_ERR_LEVEL_ERROR, HI_ERR_BUF_EMPTY)
/* no buffer for new data */
#define HI_ERR_ADEC_BUF_FULL          HI_DEFINE_ERR(HI_ID_ADEC, HI_ERR_LEVEL_ERROR, HI_ERR_BUF_FULL)
/* system is not ready,had not initialed or loaded */
#define HI_ERR_ADEC_NOT_READY         HI_DEFINE_ERR(HI_ID_ADEC, HI_ERR_LEVEL_ERROR, HI_ERR_NOT_READY)
/* decoder internal err */
#define HI_ERR_ADEC_DECODER_ERR       HI_DEFINE_ERR(HI_ID_ADEC, HI_ERR_LEVEL_ERROR, HI_ADEC_ERR_DECODER_ERR)
/* input buffer not enough to decode one frame */
#define HI_ERR_ADEC_BUF_LACK          HI_DEFINE_ERR(HI_ID_ADEC, HI_ERR_LEVEL_ERROR, HI_ADEC_ERR_BUF_LACK)
#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */

#endif/* End of #ifndef __HI_COMMON_ADEC_H__*/

