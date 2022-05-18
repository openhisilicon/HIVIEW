/*
 * Copyright (c) Hisilicon Technologies Co., Ltd. 2019-2020. All rights reserved.
 * Description: vda common head file
 * Author: Hisilicon multimedia software group
 * Create: 2019-05-21
 */

#ifndef __HI_COMMON_VDA_H__
#define __HI_COMMON_VDA_H__

#include "hi_type.h"
#include "hi_debug.h"
#include "hi_common_video.h"
#include "hi_common.h"
#include "hi_errno.h"
#include "hi_defines.h"

#ifdef __cplusplus
extern "C" {
#endif

#define HI_VDA_OD_RGN_NUM_MAX 4

typedef enum {
    HI_VDA_REF_MODE_DYNAMIC = 0, /* reference picture dynamic */
    HI_VDA_REF_MODE_STATIC, /* reference picture static */
    HI_VDA_REF_MODE_USER, /* reference picture user */
    HI_VDA_REF_MODE_BUTT
} hi_vda_ref_mode;

typedef enum {
    HI_VDA_ALG_BG = 0, /* base on background picture */
    HI_VDA_ALG_REF, /* base on reference picture */
    HI_VDA_ALG_BUTT
} hi_vda_alg;

typedef enum {
    HI_VDA_MB_8PIXEL, /* 8*8 */
    HI_VDA_MB_16PIXEL, /* 16*16 */
    HI_VDA_MB_BUTT
} hi_vda_mb_size;

typedef enum {
    HI_VDA_MB_SAD_8BIT = 0, /* SAD precision 8bits */
    HI_VDA_MB_SAD_16BIT, /* SAD precision 16bits */
    HI_VDA_MB_SAD_BUTT
} hi_vda_mb_sad_bits;

typedef enum {
    HI_VDA_WORK_MODE_MD = 0, /* motion detection */
    HI_VDA_WORK_MODE_OD, /* Occlusion detection */
    HI_VDA_WORK_MODE_BUTT
} hi_vda_work_mode;

typedef struct {
    hi_u16 upper_left_x;
    hi_u16 upper_left_y;
    hi_u16 lower_right_x;
    hi_u16 lower_right_y;
} hi_vda_rgn;

typedef struct {
    /* static attribute */
    hi_rect rect; /*
                   * region rect
                   * X:[0,HI_VDA_MAX_WIDTH),align:16
                   * Y:[0,HI_VDA_MAX_HEIGHT)
                   * W:[16,HI_VDA_MAX_WIDTH],align:16
                   * H:[16,HI_VDA_MAX_HEIGHT],align:16
                   * X+W <= channel wight
                   * Y+H <= channel height
                   */

    /* dynamic attribute */
    hi_u32 sad_threshold; /* SAD threshold,range:[0,4080] */
    hi_u32 area_threshold; /* alarm area threshold,range:[0,100] */
    hi_u32 occlusion_cnt_threshold; /* alarm frame count threshold,range:[1,256] */
    hi_u32 uncover_cnt_threshold; /* the max uncover count,range:[0,256] */
} hi_vda_od_rgn_attr;

typedef struct {
    /* static attribute */
    hi_vda_alg         vda_alg; /* arithmetic */
    hi_vda_mb_size     mb_size; /* MB size */
    hi_vda_mb_sad_bits mb_sad_bits; /* MB SAD size */
    hi_vda_ref_mode    ref_mode; /* reference picture mode */
    hi_u32             md_buf_num; /* result buffer number,range:[1,16] */

    /* dynamic attribute */
    hi_u32             vda_interval; /* VDA interval,range:[0,256] */

    hi_u32             bg_up_src_wgt;  /*
                                        * the source picture weight,
                                        * back ground update total weight 256,
                                        * range:[1,255],recommendatory value:128
                                        */

    hi_u32          sad_threshold; /* SAD threshold,range:[0,4080],recommendatory value:100 */
    hi_u32          obj_num_max;   /* max OBJ number,range:[1,128] */
} hi_vda_md_attr;

typedef struct {
    hi_u32             rgn_num; /* region number */
    hi_vda_od_rgn_attr od_rgn_attr[HI_VDA_OD_RGN_NUM_MAX]; /* region attribute */
    /* static attribute */
    hi_vda_alg         vda_alg;   /* arithmetic */
    hi_vda_mb_size     mb_size;   /* MB size */
    hi_vda_mb_sad_bits mb_sad_bits; /* MB SAD size */
    hi_vda_ref_mode    ref_mode; /* reference picture mode */
    /* dynamic attribute */
    hi_u32             vda_interval;  /* VDA interval,[0,256] */
    hi_u32             bg_up_src_wgt; /* back ground update total weight 256,the source picture weight,[1,255] */
} hi_vda_od_attr;

typedef union {
    hi_vda_md_attr md_attr;  /* MD attribute. AUTO:hi_vda_work_mode:HI_VDA_WORK_MODE_MD; */
    hi_vda_od_attr od_attr;  /* OD attribute. AUTO:hi_vda_work_mode:HI_VDA_WORK_MODE_OD; */
} hi_vda_work_mode_attr;

typedef struct {
    hi_vda_work_mode work_mode; /* work mode */
    hi_vda_work_mode_attr attr; /* work mode attribute */
    hi_u32 width;               /* the width of CHNL,[HI_VDA_MIN_WIDTH,HI_VDA_MAX_WIDTH] */
    hi_u32 height;              /* the height of CHNL,[HI_VDA_MIN_HEIGHT,HI_VDA_MAX_HEIGHT] */
} hi_vda_chn_attr;


typedef struct {
    hi_u32             stride;  /* stride */
    hi_vda_mb_sad_bits mb_sad_bits; /* the MB SAD size */
    hi_void ATTRIBUTE  *addr; /* address */
} hi_vda_mb_sad_data;

typedef struct {
    hi_u32 obj_num;  /* OBJ number */
    hi_vda_rgn  *addr; /* OBJ data address */

    hi_u32 idx_of_max_obj; /* index of max OBJ */
    hi_u32 size_of_max_obj; /* size of max OBJ */
    hi_u32 size_of_total_obj; /* total size of all OBJ */
} hi_vda_obj_data;


typedef struct {
    hi_bool            mb_sad_valid; /* SAD data is valid */
    hi_vda_mb_sad_data mb_sad_data; /* MB SAD data */

    hi_bool            obj_valid; /* OBJ data is valid */
    hi_vda_obj_data    obj_data;  /* OBJ data */

    hi_bool            pixel_num_valid; /* alarm pixel number data is valid */
    hi_u32             alarm_pixel_cnt; /* motion pix of picture */
} hi_vda_md_data;


typedef struct {
    hi_u32  rgn_num; /* region number */
    hi_bool rgn_alarm_en[HI_VDA_OD_RGN_NUM_MAX]; /* HI_TRUE:alarm */
} hi_vda_od_data;

typedef union {
    hi_vda_md_data md_data; /* MD data. AUTO:hi_vda_work_mode:HI_VDA_WORK_MODE_MD; */
    hi_vda_od_data od_data; /* OD data. AUTO:hi_vda_work_mode:HI_VDA_WORK_MODE_OD; */
} hi_vda_data_type;

typedef struct {
    hi_vda_work_mode work_mode; /* work mode */
    hi_vda_data_type data;  /* VDA data type */
    hi_vda_mb_size   mb_size;  /* MB size */
    hi_u32           mb_width;  /* VDA channel width in MB */
    hi_u32           mb_height; /* VDA channel height in MB */
    hi_u64           pts;  /* time */
} hi_vda_data;

typedef struct {
    hi_bool recv_pic_en; /* start receive picture */
    hi_u32  left_pic; /* left picture */
    hi_u32  left_data; /* left data */
} hi_vda_chn_status;

#define HI_ERR_VDA_INVALID_DEV_ID    HI_DEFINE_ERR(HI_ID_VDA, HI_ERR_LEVEL_ERROR, HI_ERR_INVALID_DEV_ID)
#define HI_ERR_VDA_INVALID_CHN_ID    HI_DEFINE_ERR(HI_ID_VDA, HI_ERR_LEVEL_ERROR, HI_ERR_INVALID_CHN_ID)
#define HI_ERR_VDA_ILLEGAL_PARAM     HI_DEFINE_ERR(HI_ID_VDA, HI_ERR_LEVEL_ERROR, HI_ERR_ILLEGAL_PARAM)
#define HI_ERR_VDA_EXIST             HI_DEFINE_ERR(HI_ID_VDA, HI_ERR_LEVEL_ERROR, HI_ERR_EXIST)
#define HI_ERR_VDA_UNEXIST           HI_DEFINE_ERR(HI_ID_VDA, HI_ERR_LEVEL_ERROR, HI_ERR_UNEXIST)
#define HI_ERR_VDA_NULL_PTR          HI_DEFINE_ERR(HI_ID_VDA, HI_ERR_LEVEL_ERROR, HI_ERR_NULL_PTR)
#define HI_ERR_VDA_NOT_CFG           HI_DEFINE_ERR(HI_ID_VDA, HI_ERR_LEVEL_ERROR, HI_ERR_NOT_CFG)
#define HI_ERR_VDA_NOT_SUPPORT       HI_DEFINE_ERR(HI_ID_VDA, HI_ERR_LEVEL_ERROR, HI_ERR_NOT_SUPPORT)
#define HI_ERR_VDA_NOT_PERM          HI_DEFINE_ERR(HI_ID_VDA, HI_ERR_LEVEL_ERROR, HI_ERR_NOT_PERM)
#define HI_ERR_VDA_NO_MEM            HI_DEFINE_ERR(HI_ID_VDA, HI_ERR_LEVEL_ERROR, HI_ERR_NO_MEM)
#define HI_ERR_VDA_NO_BUF            HI_DEFINE_ERR(HI_ID_VDA, HI_ERR_LEVEL_ERROR, HI_ERR_NO_BUF)
#define HI_ERR_VDA_BUF_EMPTY         HI_DEFINE_ERR(HI_ID_VDA, HI_ERR_LEVEL_ERROR, HI_ERR_BUF_EMPTY)
#define HI_ERR_VDA_BUF_FULL          HI_DEFINE_ERR(HI_ID_VDA, HI_ERR_LEVEL_ERROR, HI_ERR_BUF_FULL)
#define HI_ERR_VDA_BAD_ADDR          HI_DEFINE_ERR(HI_ID_VDA, HI_ERR_LEVEL_ERROR, HI_ERR_BAD_ADDR)
#define HI_ERR_VDA_BUSY              HI_DEFINE_ERR(HI_ID_VDA, HI_ERR_LEVEL_ERROR, HI_ERR_BUSY)
#define HI_ERR_VDA_NOT_READY         HI_DEFINE_ERR(HI_ID_VDA, HI_ERR_LEVEL_ERROR, HI_ERR_NOT_READY)

#ifdef __cplusplus
}
#endif

#endif
