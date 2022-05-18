/*
 * Copyright (c) Hisilicon Technologies Co., Ltd. 2019-2020. All rights reserved.
 * Description: hi_common_aenc.h
 * Author: Hisilicon multimedia software group
 * Create: 2019/06/15
 * History                 :
 *  1.Date                 :   2019/06/15
 *    Modification         :   Created file
 */

#ifndef  __HI_COMMON_AENC_H__
#define  __HI_COMMON_AENC_H__

#include "hi_type.h"
#include "hi_common.h"
#include "hi_common_aio.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* End of #ifdef __cplusplus */

#define HI_MAX_ENCODER_NAME_LEN 17

typedef struct {
    hi_u32 reserved;    /* reserve item */
} hi_aenc_attr_g711;

typedef struct {
    hi_g726_bps g726bps;
} hi_aenc_attr_g726;

typedef struct {
    hi_adpcm_type adpcm_type;
} hi_aenc_attr_adpcm;

typedef struct {
    hi_u32 reserved;    /* reserve item */
} hi_aenc_attr_lpcm;

typedef struct {
    hi_payload_type type;
    hi_u32          max_frame_len;
    hi_char         name[HI_MAX_ENCODER_NAME_LEN];    /* encoder type,be used to print proc information */
    hi_s32          (*func_open_encoder)(hi_void *encoder_attr, hi_void **encoder); /* encoder is the handle to
                                                                                       control the encoder */
    hi_s32          (*func_enc_frame)(hi_void *encoder, const hi_audio_frame *data,
                                      hi_u8 *out_buf, hi_u32 *out_len);
    hi_s32          (*func_close_encoder)(hi_void *encoder);
} hi_aenc_encoder;

typedef struct {
    hi_payload_type     type;
    hi_u32              point_num_per_frame;
    hi_u32              buf_size;      /* buf size [2~HI_MAX_AUDIO_FRAME_NUM] */
    hi_void ATTRIBUTE   *value;        /* point to attribute of definite audio encoder */
} hi_aenc_chn_attr;

typedef enum {
    HI_AENC_ERR_ENCODER_ERR = 64,
    HI_AENC_ERR_VQE_ERR     = 65,
} hi_aenc_err_code;

/* invalid device ID */
#define HI_ERR_AENC_INVALID_DEV_ID    HI_DEFINE_ERR(HI_ID_AENC, HI_ERR_LEVEL_ERROR, HI_ERR_INVALID_DEV_ID)
/* invalid channel ID */
#define HI_ERR_AENC_INVALID_CHN_ID    HI_DEFINE_ERR(HI_ID_AENC, HI_ERR_LEVEL_ERROR, HI_ERR_INVALID_CHN_ID)
/* at lease one parameter is illagal ,eg, an illegal enumeration value  */
#define HI_ERR_AENC_ILLEGAL_PARAM     HI_DEFINE_ERR(HI_ID_AENC, HI_ERR_LEVEL_ERROR, HI_ERR_ILLEGAL_PARAM)
/* channel exists */
#define HI_ERR_AENC_EXIST             HI_DEFINE_ERR(HI_ID_AENC, HI_ERR_LEVEL_ERROR, HI_ERR_EXIST)
/* channel unexists */
#define HI_ERR_AENC_UNEXIST           HI_DEFINE_ERR(HI_ID_AENC, HI_ERR_LEVEL_ERROR, HI_ERR_UNEXIST)
/* using a NULL point */
#define HI_ERR_AENC_NULL_PTR          HI_DEFINE_ERR(HI_ID_AENC, HI_ERR_LEVEL_ERROR, HI_ERR_NULL_PTR)
/* try to enable or initialize system,device or channel, before configing attribute */
#define HI_ERR_AENC_NOT_CFG           HI_DEFINE_ERR(HI_ID_AENC, HI_ERR_LEVEL_ERROR, HI_ERR_NOT_CFG)
/* operation is not supported by NOW */
#define HI_ERR_AENC_NOT_SUPPORT       HI_DEFINE_ERR(HI_ID_AENC, HI_ERR_LEVEL_ERROR, HI_ERR_NOT_SUPPORT)
/* operation is not permitted ,eg, try to change static attribute */
#define HI_ERR_AENC_NOT_PERM          HI_DEFINE_ERR(HI_ID_AENC, HI_ERR_LEVEL_ERROR, HI_ERR_NOT_PERM)
/* failure caused by malloc memory */
#define HI_ERR_AENC_NO_MEM            HI_DEFINE_ERR(HI_ID_AENC, HI_ERR_LEVEL_ERROR, HI_ERR_NO_MEM)
/* failure caused by malloc buffer */
#define HI_ERR_AENC_NO_BUF            HI_DEFINE_ERR(HI_ID_AENC, HI_ERR_LEVEL_ERROR, HI_ERR_NO_BUF)
/* no data in buffer */
#define HI_ERR_AENC_BUF_EMPTY         HI_DEFINE_ERR(HI_ID_AENC, HI_ERR_LEVEL_ERROR, HI_ERR_BUF_EMPTY)
/* no buffer for new data */
#define HI_ERR_AENC_BUF_FULL          HI_DEFINE_ERR(HI_ID_AENC, HI_ERR_LEVEL_ERROR, HI_ERR_BUF_FULL)
/* system is not ready,had not initialed or loaded */
#define HI_ERR_AENC_NOT_READY         HI_DEFINE_ERR(HI_ID_AENC, HI_ERR_LEVEL_ERROR, HI_ERR_NOT_READY)
/* encoder internal err */
#define HI_ERR_AENC_ENCODER_ERR       HI_DEFINE_ERR(HI_ID_AENC, HI_ERR_LEVEL_ERROR, HI_AENC_ERR_ENCODER_ERR)
/* vqe internal err */
#define HI_ERR_AENC_VQE_ERR           HI_DEFINE_ERR(HI_ID_AENC, HI_ERR_LEVEL_ERROR, HI_AENC_ERR_VQE_ERR)
#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */

#endif/* End of #ifndef __HI_COMMON_AENC_H__ */

