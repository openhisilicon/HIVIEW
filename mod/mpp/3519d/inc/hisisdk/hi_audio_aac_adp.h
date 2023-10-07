/*
  Copyright (c), 2001-2022, Shenshu Tech. Co., Ltd.
 */

#ifndef HI_AUDIO_AAC_ADP_H
#define HI_AUDIO_AAC_ADP_H

#include "hi_type.h"
#include "ot_audio_aac_adp.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* End of #ifdef __cplusplus */

#define HI_AACLD_SAMPLES_PER_FRAME OT_AACLD_SAMPLES_PER_FRAME
#define HI_AACLC_SAMPLES_PER_FRAME OT_AACLC_SAMPLES_PER_FRAME
#define HI_AACPLUS_SAMPLES_PER_FRAME OT_AACPLUS_SAMPLES_PER_FRAME
#define HI_MAX_AAC_MAINBUF_SIZE OT_MAX_AAC_MAINBUF_SIZE
#define HI_AAC_TYPE_AACLC OT_AAC_TYPE_AACLC
#define HI_AAC_TYPE_EAAC OT_AAC_TYPE_EAAC
#define HI_AAC_TYPE_EAACPLUS OT_AAC_TYPE_EAACPLUS
#define HI_AAC_TYPE_AACLD OT_AAC_TYPE_AACLD
#define HI_AAC_TYPE_AACELD OT_AAC_TYPE_AACELD
#define HI_AAC_TYPE_BUTT OT_AAC_TYPE_BUTT
typedef ot_aac_type hi_aac_type;
#define HI_AAC_BPS_8K OT_AAC_BPS_8K
#define HI_AAC_BPS_16K OT_AAC_BPS_16K
#define HI_AAC_BPS_22K OT_AAC_BPS_22K
#define HI_AAC_BPS_24K OT_AAC_BPS_24K
#define HI_AAC_BPS_32K OT_AAC_BPS_32K
#define HI_AAC_BPS_48K OT_AAC_BPS_48K
#define HI_AAC_BPS_64K OT_AAC_BPS_64K
#define HI_AAC_BPS_96K OT_AAC_BPS_96K
#define HI_AAC_BPS_128K OT_AAC_BPS_128K
#define HI_AAC_BPS_256K OT_AAC_BPS_256K
#define HI_AAC_BPS_320K OT_AAC_BPS_320K
#define HI_AAC_BPS_BUTT OT_AAC_BPS_BUTT
typedef ot_aac_bps hi_aac_bps;
#define HI_AAC_TRANSPORT_TYPE_ADTS OT_AAC_TRANSPORT_TYPE_ADTS
#define HI_AAC_TRANSPORT_TYPE_LOAS OT_AAC_TRANSPORT_TYPE_LOAS
#define HI_AAC_TRANSPORT_TYPE_LATM_MCP1 OT_AAC_TRANSPORT_TYPE_LATM_MCP1
#define HI_AAC_TRANSPORT_TYPE_BUTT OT_AAC_TRANSPORT_TYPE_BUTT
typedef ot_aac_transport_type hi_aac_transport_type;
typedef ot_aac_frame_info hi_aac_frame_info;
typedef ot_aenc_attr_aac hi_aenc_attr_aac;
typedef ot_adec_attr_aac hi_adec_attr_aac;

hi_s32 hi_mpi_aenc_aac_init(hi_void);
hi_s32 hi_mpi_aenc_aac_deinit(hi_void);

hi_s32 hi_mpi_adec_aac_init(hi_void);
hi_s32 hi_mpi_adec_aac_deinit(hi_void);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */

#endif
