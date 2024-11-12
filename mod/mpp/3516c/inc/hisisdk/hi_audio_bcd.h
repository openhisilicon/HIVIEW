/*
  Copyright (c), 2001-2024, Shenshu Tech. Co., Ltd.
 */

#ifndef HI_AUDIO_BCD_H
#define HI_AUDIO_BCD_H

#include "hi_type.h"
#include "ot_audio_bcd.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* End of #ifdef __cplusplus */

#define HI_ERR_BCD_INVALID_HANDLE OT_ERR_BCD_INVALID_HANDLE
#define HI_ERR_BCD_MEMORY_ERROR OT_ERR_BCD_MEMORY_ERROR
#define HI_ERR_BCD_ILLEGAL_PARAM OT_ERR_BCD_ILLEGAL_PARAM
#define HI_ERR_BCD_NULL_PTR OT_ERR_BCD_NULL_PTR
#define HI_ERR_BCD_NOT_PERM OT_ERR_BCD_NOT_PERM
#define HI_ERR_BCD_PROCESS_ERROR OT_ERR_BCD_PROCESS_ERROR
typedef ot_bcd_handle hi_bcd_handle;
typedef ot_bcd_config hi_bcd_config;
typedef ot_bcd_process_data hi_bcd_process_data;

hi_s32 hi_baby_crying_detection_init(hi_bcd_handle *bcd, hi_s32 sample_rate, const hi_bcd_config *bcd_config);

hi_s32 hi_baby_crying_detection_process(hi_bcd_handle bcd, hi_bcd_process_data *input_data,
    hi_bcd_process_data *output_data);

hi_s32 hi_baby_crying_detection_deinit(hi_bcd_handle bcd);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */

#endif
