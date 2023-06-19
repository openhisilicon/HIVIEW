/*
  Copyright (c), 2001-2022, Shenshu Tech. Co., Ltd.
 */

#ifndef __HI_COMMON_AVS_POS_QUERY_H__
#define __HI_COMMON_AVS_POS_QUERY_H__

#include "hi_common_avs.h"
#include "ot_common_avs_pos_query.h"


#ifdef __cplusplus
extern "C" {
#endif

#define HI_AVS_MAX_INPUT_NUM OT_AVS_MAX_INPUT_NUM
#define HI_AVS_LUT_SIZE OT_AVS_LUT_SIZE
#define HI_AVS_DST_QUERY_SRC OT_AVS_DST_QUERY_SRC
#define HI_AVS_SRC_QUERY_DST OT_AVS_SRC_QUERY_DST
#define HI_AVS_QUERY_MODE_BUTT OT_AVS_QUERY_MODE_BUTT
typedef ot_avs_query_mode hi_avs_query_mode;
typedef ot_avs_pos_cfg hi_avs_pos_cfg;

#ifdef __cplusplus
}
#endif
#endif /* __HI_COMMON_AVS_POS_QUERY_H__ */
