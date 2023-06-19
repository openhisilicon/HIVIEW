/*
  Copyright (c), 2001-2022, Shenshu Tech. Co., Ltd.
 */

#ifndef  OT_COMMON_AENC_H
#define  OT_COMMON_AENC_H

#include "ot_type.h"
#include "ot_common.h"
#include "ot_common_aio.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* End of #ifdef __cplusplus */

#define OT_MAX_ENCODER_NAME_LEN 17

typedef struct {
    td_u32 reserved;    /* reserve item */
} ot_aenc_attr_g711;

typedef struct {
    ot_g726_bps g726bps;
} ot_aenc_attr_g726;

typedef struct {
    ot_adpcm_type adpcm_type;
} ot_aenc_attr_adpcm;

typedef struct {
    td_u32 reserved;    /* reserve item */
} ot_aenc_attr_lpcm;

typedef struct {
    ot_payload_type type;
    td_u32          max_frame_len;
    td_char         name[OT_MAX_ENCODER_NAME_LEN];    /* encoder type,be used to print proc information */
    td_s32          (*func_open_encoder)(td_void *encoder_attr, td_void **encoder); /* encoder is the handle to
                                                                                       control the encoder */
    td_s32          (*func_enc_frame)(td_void *encoder, const ot_audio_frame *data,
                                      td_u8 *out_buf, td_u32 *out_len);
    td_s32          (*func_close_encoder)(td_void *encoder);
} ot_aenc_encoder;

typedef struct {
    ot_payload_type     type;
    td_u32              point_num_per_frame;
    td_u32              buf_size;      /* buf size [2~OT_MAX_AUDIO_FRAME_NUM] */
    td_void ATTRIBUTE   *value;        /* point to attribute of definite audio encoder */
} ot_aenc_chn_attr;

typedef enum {
    OT_AENC_ERR_ENCODER_ERR = 64,
    OT_AENC_ERR_VQE_ERR     = 65,
} ot_aenc_err_code;

/* invalid device ID */
#define OT_ERR_AENC_INVALID_DEV_ID    OT_DEFINE_ERR(OT_ID_AENC, OT_ERR_LEVEL_ERROR, OT_ERR_INVALID_DEV_ID)
/* invalid channel ID */
#define OT_ERR_AENC_INVALID_CHN_ID    OT_DEFINE_ERR(OT_ID_AENC, OT_ERR_LEVEL_ERROR, OT_ERR_INVALID_CHN_ID)
/* at lease one parameter is illegal, eg, an illegal enumeration value */
#define OT_ERR_AENC_ILLEGAL_PARAM     OT_DEFINE_ERR(OT_ID_AENC, OT_ERR_LEVEL_ERROR, OT_ERR_ILLEGAL_PARAM)
/* channel exists */
#define OT_ERR_AENC_EXIST             OT_DEFINE_ERR(OT_ID_AENC, OT_ERR_LEVEL_ERROR, OT_ERR_EXIST)
/* channel unexist */
#define OT_ERR_AENC_UNEXIST           OT_DEFINE_ERR(OT_ID_AENC, OT_ERR_LEVEL_ERROR, OT_ERR_UNEXIST)
/* using a null pointer */
#define OT_ERR_AENC_NULL_PTR          OT_DEFINE_ERR(OT_ID_AENC, OT_ERR_LEVEL_ERROR, OT_ERR_NULL_PTR)
/* try to enable or initialize system, device or channel, before configing attribute */
#define OT_ERR_AENC_NOT_CFG           OT_DEFINE_ERR(OT_ID_AENC, OT_ERR_LEVEL_ERROR, OT_ERR_NOT_CFG)
/* operation is not supported by now */
#define OT_ERR_AENC_NOT_SUPPORT       OT_DEFINE_ERR(OT_ID_AENC, OT_ERR_LEVEL_ERROR, OT_ERR_NOT_SUPPORT)
/* operation is not permitted, eg, try to change static attribute */
#define OT_ERR_AENC_NOT_PERM          OT_DEFINE_ERR(OT_ID_AENC, OT_ERR_LEVEL_ERROR, OT_ERR_NOT_PERM)
/* failure caused by malloc memory */
#define OT_ERR_AENC_NO_MEM            OT_DEFINE_ERR(OT_ID_AENC, OT_ERR_LEVEL_ERROR, OT_ERR_NO_MEM)
/* failure caused by malloc buffer */
#define OT_ERR_AENC_NO_BUF            OT_DEFINE_ERR(OT_ID_AENC, OT_ERR_LEVEL_ERROR, OT_ERR_NO_BUF)
/* no data in buffer */
#define OT_ERR_AENC_BUF_EMPTY         OT_DEFINE_ERR(OT_ID_AENC, OT_ERR_LEVEL_ERROR, OT_ERR_BUF_EMPTY)
/* no buffer for new data */
#define OT_ERR_AENC_BUF_FULL          OT_DEFINE_ERR(OT_ID_AENC, OT_ERR_LEVEL_ERROR, OT_ERR_BUF_FULL)
/* system is not ready,had not initialed or loaded */
#define OT_ERR_AENC_NOT_READY         OT_DEFINE_ERR(OT_ID_AENC, OT_ERR_LEVEL_ERROR, OT_ERR_NOT_READY)
/* encoder internal err */
#define OT_ERR_AENC_ENCODER_ERR       OT_DEFINE_ERR(OT_ID_AENC, OT_ERR_LEVEL_ERROR, OT_AENC_ERR_ENCODER_ERR)
/* vqe internal err */
#define OT_ERR_AENC_VQE_ERR           OT_DEFINE_ERR(OT_ID_AENC, OT_ERR_LEVEL_ERROR, OT_AENC_ERR_VQE_ERR)
#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */

#endif /* End of #ifndef OT_COMMON_AENC_H */
