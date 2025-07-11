/*
  Copyright (c), 2001-2024, Shenshu Tech. Co., Ltd.
 */

#ifndef OT_SAMPLE_FOV2LDC_H
#define OT_SAMPLE_FOV2LDC_H

#include "hi_type.h"
#include "hi_common_video.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

#define FOV_PREC_BITS 20

typedef enum {
    HI_FOV_TYPE_DIAGONAL = 0,
    HI_FOV_TYPE_HORIZONTAL,
    HI_FOV_TYPE_VERTICAL,
    HI_FOV_TYPE_BUTT
} hi_fov_type;

typedef struct {
    hi_u32 width;
    hi_u32 height;
    hi_fov_type type; /* 0--diagonal,1--horizontal,2--vertical */
    hi_u32      fov; /* decimal bits 20bit */
} sample_fov_attr;

hi_s32 hi_sample_fov_to_ldcv2(const sample_fov_attr *fov_attr, hi_dis_ldc_attr *dis_ldc_attr);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of __cplusplus */

#endif /* OT_SAMPLE_FOV2LDC_H */

