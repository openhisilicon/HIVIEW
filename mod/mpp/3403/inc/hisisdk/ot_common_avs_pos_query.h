/*
  Copyright (c), 2001-2022, Shenshu Tech. Co., Ltd.
 */

#ifndef OT_COMMON_AVS_POS_QUERY_H
#define OT_COMMON_AVS_POS_QUERY_H

#include "ot_common_avs.h"

#ifdef __cplusplus
extern "C" {
#endif

#define OT_AVS_MAX_INPUT_NUM 6
#define OT_AVS_LUT_SIZE  0x400000   /* 4MB */

/* Specification of the LUT accuracy */
typedef enum {
    OT_AVS_DST_QUERY_SRC = 0,
    OT_AVS_SRC_QUERY_DST,
    OT_AVS_QUERY_MODE_BUTT
} ot_avs_query_mode;

typedef struct {
    ot_avs_query_mode   mesh_mode;  /* mesh generating mode. */
    td_u32              camera_num; /* camera number */
    /*
     * range: [256, 8192]; size of source image.
     */
    ot_size             src_size;
    /*
     * range: [256, 16384]; size of target image.
     */
    ot_size             dst_size;
    ot_avs_projection_mode projection_mode; /* projection mode. */
    ot_point            center;       /* range: [-16383,16383]: center point. */
    ot_avs_fov          fov;          /* output FOV. */
    ot_avs_rotation     ori_rotation; /* output original rotation. */
    ot_avs_rotation     rotation;     /* output rotation. */
    td_u32              window_size;  /* range:[2,256], the window size of mesh data. */
    ot_avs_lut_accuracy lut_accuracy; /* accuracy of lut file. */
    td_u64              lut_addr[OT_AVS_MAX_INPUT_NUM]; /* the virtual address of lut data. */
} ot_avs_pos_cfg;

#ifdef __cplusplus
}
#endif

#endif /* ot_common_avs_pos_query.h */

