/*
  Copyright (c), 2001-2022, Shenshu Tech. Co., Ltd.
 */

#ifndef __HI_COMMON_SVP_H__
#define __HI_COMMON_SVP_H__

#include "hi_type.h"
#include "hi_errno.h"
#include "ot_common_svp.h"


#ifdef __cplusplus
extern "C" {
#endif

#define HI_SVP_IMG_ADDR_NUM OT_SVP_IMG_ADDR_NUM
#define HI_SVP_IMG_STRIDE_NUM OT_SVP_IMG_STRIDE_NUM
#define HI_SVP_BLOB_TYPE_S20Q12 OT_SVP_BLOB_TYPE_S20Q12
#define HI_SVP_BLOB_TYPE_U8 OT_SVP_BLOB_TYPE_U8
#define HI_SVP_BLOB_TYPE_YVU420SP OT_SVP_BLOB_TYPE_YVU420SP
#define HI_SVP_BLOB_TYPE_YVU422SP OT_SVP_BLOB_TYPE_YVU422SP
#define HI_SVP_BLOB_TYPE_VEC_S20Q12 OT_SVP_BLOB_TYPE_VEC_S20Q12
#define HI_SVP_BLOB_TYPE_SEQ_S20Q12 OT_SVP_BLOB_TYPE_SEQ_S20Q12
#define HI_SVP_BLOB_TYPE_BBOX_S20Q12 OT_SVP_BLOB_TYPE_BBOX_S20Q12
#define HI_SVP_BLOB_TYPE_BSI_SQ32 OT_SVP_BLOB_TYPE_BSI_SQ32
#define HI_SVP_BLOB_TYPE_S12Q20 OT_SVP_BLOB_TYPE_S12Q20
#define HI_SVP_BLOB_TYPE_VEC_S12Q20 OT_SVP_BLOB_TYPE_VEC_S12Q20
#define HI_SVP_BLOB_TYPE_S32 OT_SVP_BLOB_TYPE_S32
#define HI_SVP_BLOB_TYPE_U32 OT_SVP_BLOB_TYPE_U32
#define HI_SVP_BLOB_TYPE_FP32 OT_SVP_BLOB_TYPE_FP32
#define HI_SVP_BLOB_TYPE_FP16 OT_SVP_BLOB_TYPE_FP16
#define HI_SVP_BLOB_TYPE_S8 OT_SVP_BLOB_TYPE_S8
#define HI_SVP_BLOB_TYPE_S16 OT_SVP_BLOB_TYPE_S16
#define HI_SVP_BLOB_TYPE_U16 OT_SVP_BLOB_TYPE_U16
#define HI_SVP_BLOB_TYPE_BUTT OT_SVP_BLOB_TYPE_BUTT
typedef ot_svp_blob_type hi_svp_blob_type;
typedef ot_svp_blob hi_svp_blob;
typedef ot_svp_src_blob hi_svp_src_blob;
typedef ot_svp_dst_blob hi_svp_dst_blob;
typedef ot_svp_mem_info hi_svp_mem_info;
typedef ot_svp_src_mem_info hi_svp_src_mem_info;
typedef ot_svp_dst_mem_info hi_svp_dst_mem_info;
#define HI_SVP_IMG_TYPE_U8C1 OT_SVP_IMG_TYPE_U8C1
#define HI_SVP_IMG_TYPE_S8C1 OT_SVP_IMG_TYPE_S8C1
#define HI_SVP_IMG_TYPE_YUV420SP OT_SVP_IMG_TYPE_YUV420SP
#define HI_SVP_IMG_TYPE_YUV422SP OT_SVP_IMG_TYPE_YUV422SP
#define HI_SVP_IMG_TYPE_YUV420P OT_SVP_IMG_TYPE_YUV420P
#define HI_SVP_IMG_TYPE_YUV422P OT_SVP_IMG_TYPE_YUV422P
#define HI_SVP_IMG_TYPE_S8C2_PACKAGE OT_SVP_IMG_TYPE_S8C2_PACKAGE
#define HI_SVP_IMG_TYPE_S8C2_PLANAR OT_SVP_IMG_TYPE_S8C2_PLANAR
#define HI_SVP_IMG_TYPE_S16C1 OT_SVP_IMG_TYPE_S16C1
#define HI_SVP_IMG_TYPE_U16C1 OT_SVP_IMG_TYPE_U16C1
#define HI_SVP_IMG_TYPE_U8C3_PACKAGE OT_SVP_IMG_TYPE_U8C3_PACKAGE
#define HI_SVP_IMG_TYPE_U8C3_PLANAR OT_SVP_IMG_TYPE_U8C3_PLANAR
#define HI_SVP_IMG_TYPE_S32C1 OT_SVP_IMG_TYPE_S32C1
#define HI_SVP_IMG_TYPE_U32C1 OT_SVP_IMG_TYPE_U32C1
#define HI_SVP_IMG_TYPE_S64C1 OT_SVP_IMG_TYPE_S64C1
#define HI_SVP_IMG_TYPE_U64C1 OT_SVP_IMG_TYPE_U64C1
#define HI_SVP_IMG_TYPE_BUTT OT_SVP_IMG_TYPE_BUTT
typedef ot_svp_img_type hi_svp_img_type;
typedef ot_svp_img hi_svp_img;
typedef ot_svp_src_img hi_svp_src_img;
typedef ot_svp_dst_img hi_svp_dst_img;
typedef td_u0q8 hi_u0q8;
typedef td_u1q7 hi_u1q7;
typedef td_u5q3 hi_u5q3;
typedef td_u3q5 hi_u3q5;
typedef td_u0q16 hi_u0q16;
typedef td_u4q12 hi_u4q12;
typedef td_u6q10 hi_u6q10;
typedef td_u8q8 hi_u8q8;
typedef td_u9q7 hi_u9q7;
typedef td_u12q4 hi_u12q4;
typedef td_u14q2 hi_u14q2;
typedef td_u5q11 hi_u5q11;
typedef td_u1q15 hi_u1q15;
typedef td_u2q14 hi_u2q14;
typedef td_ufp16 hi_ufp16;
typedef td_s9q7 hi_s9q7;
typedef td_s14q2 hi_s14q2;
typedef td_s1q15 hi_s1q15;
typedef td_u22q10 hi_u22q10;
typedef td_u25q7 hi_u25q7;
typedef td_u21q11 hi_u21q11;
typedef td_u14q18 hi_u14q18;
typedef td_u8q24 hi_u8q24;
typedef td_u4q28 hi_u4q28;
typedef td_s25q7 hi_s25q7;
typedef td_s16q16 hi_s16q16;
typedef td_s14q18 hi_s14q18;
typedef td_s20q12 hi_s20q12;
typedef td_s24q8 hi_s24q8;
typedef td_u8q4f4 hi_u8q4f4;
typedef ot_svp_data hi_svp_data;
typedef ot_svp_src_data hi_svp_src_data;
typedef ot_svp_dst_data hi_svp_dst_data;
typedef ot_svp_8bit hi_svp_8bit;
typedef ot_svp_point_u16 hi_svp_point_u16;
typedef ot_svp_point_s16 hi_svp_point_s16;
typedef ot_svp_point_s25q7 hi_svp_point_s25q7;
typedef ot_svp_point_u14q2 hi_svp_point_u14q2;
typedef ot_svp_rect_u16 hi_svp_rect_u16;
typedef ot_svp_rect_u32 hi_svp_rect_u32;
typedef ot_svp_rect_s24q8 hi_svp_rect_s24q8;
typedef ot_svp_lut hi_svp_lut;

#ifdef __cplusplus
}
#endif
#endif /* __HI_COMMON_SVP_H__ */
