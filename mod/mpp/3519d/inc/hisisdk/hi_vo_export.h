/*
  Copyright (c), 2001-2022, Shenshu Tech. Co., Ltd.
 */

#ifndef HI_VO_EXPORT_H
#define HI_VO_EXPORT_H

#include "hi_common.h"
#include "ot_vo_export.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef ot_vo_export_callback hi_vo_export_callback;
typedef ot_vo_register_export_callback hi_vo_register_export_callback;
typedef ot_vo_export_symbol hi_vo_export_symbol;

#define hi_vo_disable_video_export ot_vo_disable_video_export

#ifdef __cplusplus
}
#endif
#endif /* HI_VO_EXPORT_H */
