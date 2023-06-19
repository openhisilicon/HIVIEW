/*
  Copyright (c), 2001-2022, Shenshu Tech. Co., Ltd.
 */

#ifndef __HI_VO_EXPORT_H__
#define __HI_VO_EXPORT_H__

#include "hi_common.h"
#include "ot_vo_export.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef ot_vo_export_callback hi_vo_export_callback;
typedef ot_vo_register_export_callback hi_vo_register_export_callback;
typedef ot_vo_export_symbol hi_vo_export_symbol;

#define hi_vo_get_export_symbol  ot_vo_get_export_symbol
#define hi_vo_enable_dev_export  ot_vo_enable_dev_export
#define hi_vo_disable_dev_export ot_vo_disable_dev_export

#ifdef __cplusplus
}
#endif
#endif /* __HI_VO_EXPORT_H__ */
