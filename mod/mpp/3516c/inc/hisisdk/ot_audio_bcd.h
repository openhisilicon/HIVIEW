/*
  Copyright (c), 2001-2024, Shenshu Tech. Co., Ltd.
 */

#ifndef OT_AUDIO_BCD_H
#define OT_AUDIO_BCD_H

#include "ot_type.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Defines the error of Baby Crying Detection. */
#define OT_ERR_BCD_INVALID_HANDLE (td_u32)(0xA0148100) /* Handle passed to function call was invalid. */
#define OT_ERR_BCD_MEMORY_ERROR   (td_u32)(0xA0148101) /* Memory allocation failed. */
#define OT_ERR_BCD_ILLEGAL_PARAM  (td_u32)(0xA0148102) /* Parameter not illegal. */
#define OT_ERR_BCD_NULL_PTR       (td_u32)(0xA0148103) /* Pointer is null. */
#define OT_ERR_BCD_NOT_PERM       (td_u32)(0xA0148104) /* Operation not permitted. */
#define OT_ERR_BCD_PROCESS_ERROR  (td_u32)(0xA0148105) /* Process failed. */

typedef td_void *ot_bcd_handle;
typedef td_s32 (*fn_bcd_callback)(td_void *); /* Define noticed function handle of Baby Crying Detection. */

/* Defines the configure parameters of Baby Crying Detection. */
typedef struct {
    td_bool usr_mode;             /* mode of user parameter, 0: auto, 1: manual. */
    td_bool bypass;               /* bypass bcd process, 0: no, 1: bypass. */
    td_u32 alarm_sensitivity;     /* alarm sensitivity of crying, range is [0, 100]. */
    td_u32 alarm_period;          /* alarm period(ms). */
    td_u32 alarm_count_threshold; /* crying count threshold in one period. */
    td_u32 alarm_interval;        /* alarm interval(ms) between two period. */
    fn_bcd_callback callback;     /* the callback function pointer. */
} ot_bcd_config;

/* Defines the process data configure of Baby Crying Detection. */
typedef struct {
    td_s16 *data;     /* the pointer to data buf. */
    td_s32 data_size; /* the size of data buf. */
} ot_bcd_process_data;

td_s32 ot_baby_crying_detection_init(ot_bcd_handle *bcd, td_s32 sample_rate, const ot_bcd_config *bcd_config);

td_s32 ot_baby_crying_detection_process(ot_bcd_handle bcd, ot_bcd_process_data *input_data,
    ot_bcd_process_data *output_data);

td_s32 ot_baby_crying_detection_deinit(ot_bcd_handle bcd);

#ifdef __cplusplus
}
#endif
#endif
