/*
  Copyright (c), 2001-2022, Shenshu Tech. Co., Ltd.
 */

#ifndef OT_COMMON_SVP_H
#define OT_COMMON_SVP_H

#include "ot_type.h"
#include "ot_errno.h"

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */

#define OT_SVP_IMG_ADDR_NUM 3
#define OT_SVP_IMG_STRIDE_NUM 3

/* Blob type */
typedef enum {
    OT_SVP_BLOB_TYPE_S20Q12       =  0x0,

    OT_SVP_BLOB_TYPE_U8           =  0x1,

    /* channel is 3 */
    OT_SVP_BLOB_TYPE_YVU420SP     =  0x2,

    /* channel is 3 */
    OT_SVP_BLOB_TYPE_YVU422SP     =  0x3,

    OT_SVP_BLOB_TYPE_VEC_S20Q12   =  0x4,

    OT_SVP_BLOB_TYPE_SEQ_S20Q12   =  0x5,

    OT_SVP_BLOB_TYPE_BBOX_S20Q12  =  0x6,

    /* this type of blob contains bbox, score and class_id */
    OT_SVP_BLOB_TYPE_BSI_SQ32     =  0x7,

    OT_SVP_BLOB_TYPE_S12Q20       =  0x8,

    OT_SVP_BLOB_TYPE_VEC_S12Q20   =  0x9,

    OT_SVP_BLOB_TYPE_S32          =  0xa,

    OT_SVP_BLOB_TYPE_U32          =  0xb,

    OT_SVP_BLOB_TYPE_FP32         =  0xc,

    OT_SVP_BLOB_TYPE_FP16         =  0xd,

    OT_SVP_BLOB_TYPE_S8           =  0xe,

    OT_SVP_BLOB_TYPE_S16          =  0xf,

    OT_SVP_BLOB_TYPE_U16          =  0x10,

    OT_SVP_BLOB_TYPE_BUTT
} ot_svp_blob_type;

/*
 * Blob struct
 * In Caffe, the blob contain shape info as the following order:
 * Image\FeatureMap:               N       C       H       W
 * FC(normal vector):              N       C
 * RNN\LSTM(Recurrent) vector:     T       N       D
 *
 * The relationship of the following blob struct with Caffe blob is as follows:
 * Image\FeatureMap:               Num    Chn    Height   With
 * FC(VEC_S32):                    Num    Width
 * RNN\LSTM(SEQ_S32) vector:       Step   Num     Dim
 * The stride, which measuring unit is byte, is always aligned by the width or
 * dim direction.
 */
typedef struct {
    ot_svp_blob_type type;          /* Blob type */
    td_u32 stride;                  /* Stride, a line bytes num */

    td_u64 virt_addr;               /* virtual addr */
    td_u64 phys_addr;               /* physical addr */

    td_u32 num;                     /* N: frame num or sequence num, correspond to caffe blob's n */
    union {
        struct {
            td_u32 width;           /* W: frame width, correspond to caffe blob's w */
            td_u32 height;          /* H: frame height, correspond to caffe blob's h */
            td_u32 chn;             /* C: frame channel, correspond to caffe blob's c */
        } whc;                      /* AUTO:ot_svp_blob_type:other; */
        struct {
            td_u32 dim;             /* D: vector dimension */
            td_u64 virt_addr_step;  /* T: virtual address of time steps array in each sequence */
        } seq;                      /* AUTO:ot_svp_blob_type:OT_SVP_BLOB_TYPE_SEQ_S20Q12; */
    } shape;
} ot_svp_blob;

typedef ot_svp_blob  ot_svp_src_blob;
typedef ot_svp_blob  ot_svp_dst_blob;

/* Mem information */
typedef struct {
    td_u64  phys_addr; /* RW; The physical address of the memory */
    td_u64  virt_addr; /* RW; The virtual address of the memory */
    td_u32  size;      /* RW; The size of memory */
} ot_svp_mem_info;

typedef ot_svp_mem_info ot_svp_src_mem_info;
typedef ot_svp_mem_info ot_svp_dst_mem_info;

/* Image type */
typedef enum {
    OT_SVP_IMG_TYPE_U8C1           =  0x0,
    OT_SVP_IMG_TYPE_S8C1           =  0x1,

    OT_SVP_IMG_TYPE_YUV420SP       =  0x2,       /* YUV420 SemiPlanar */
    OT_SVP_IMG_TYPE_YUV422SP       =  0x3,       /* YUV422 SemiPlanar */
    OT_SVP_IMG_TYPE_YUV420P        =  0x4,       /* YUV420 Planar */
    OT_SVP_IMG_TYPE_YUV422P        =  0x5,       /* YUV422 planar */

    OT_SVP_IMG_TYPE_S8C2_PACKAGE   =  0x6,
    OT_SVP_IMG_TYPE_S8C2_PLANAR    =  0x7,

    OT_SVP_IMG_TYPE_S16C1          =  0x8,
    OT_SVP_IMG_TYPE_U16C1          =  0x9,

    OT_SVP_IMG_TYPE_U8C3_PACKAGE   =  0xa,
    OT_SVP_IMG_TYPE_U8C3_PLANAR    =  0xb,

    OT_SVP_IMG_TYPE_S32C1          =  0xc,
    OT_SVP_IMG_TYPE_U32C1          =  0xd,

    OT_SVP_IMG_TYPE_S64C1          =  0xe,
    OT_SVP_IMG_TYPE_U64C1          =  0xf,

    OT_SVP_IMG_TYPE_BUTT
} ot_svp_img_type;

/* Image */
typedef struct {
    td_u64  phys_addr[OT_SVP_IMG_ADDR_NUM]; /* RW; The physical address of the image */
    td_u64  virt_addr[OT_SVP_IMG_ADDR_NUM]; /* RW; The virtual address of the image */
    td_u32  stride[OT_SVP_IMG_STRIDE_NUM];  /* RW; The stride of the image */
    td_u32  width;                          /* RW; The width of the image */
    td_u32  height;                         /* RW; The height of the image */
    ot_svp_img_type  type;                  /* RW; The type of the image */
} ot_svp_img;

typedef ot_svp_img ot_svp_src_img;
typedef ot_svp_img ot_svp_dst_img;

/*
 * The fixed-point data type, will be used to
 * represent float data in hardware calculations.
 */
/* u8bit */
typedef unsigned char td_u0q8;
typedef unsigned char td_u1q7;
typedef unsigned char td_u5q3;
typedef unsigned char td_u3q5;

/* u16bit */
typedef unsigned short td_u0q16;
typedef unsigned short td_u4q12;
typedef unsigned short td_u6q10;
typedef unsigned short td_u8q8;
typedef unsigned short td_u9q7;
typedef unsigned short td_u12q4;
typedef unsigned short td_u14q2;
typedef unsigned short td_u5q11;
typedef unsigned short td_u1q15;
typedef unsigned short td_u2q14;
typedef td_u6q10 td_ufp16;
/* s16bit */
typedef short td_s9q7;
typedef short td_s14q2;
typedef short td_s1q15;

/* u32bit */
typedef unsigned int td_u22q10;
typedef unsigned int td_u25q7;
typedef unsigned int td_u21q11;
typedef unsigned int td_u14q18;
typedef unsigned int td_u8q24;
typedef unsigned int td_u4q28;

/* s32bit */
typedef int td_s25q7;
typedef int td_s16q16;
typedef int td_s14q18;
typedef int td_s20q12;
typedef int td_s24q8;

/*
 * The fixed-point data type combine with flag_bits.
 */
/* 8bits unsigned integer,4bits decimal fraction,4bits flag_bits */
typedef unsigned short td_u8q4f4;

typedef struct {
    td_u64 phys_addr; /* RW; The physical address of the data */
    td_u64 virt_addr; /* RW; The virtual address of the data */

    td_u32 stride;   /* RW; The stride of 2D data by byte */
    td_u32 width;    /* RW; The width of 2D data by byte */
    td_u32 height;   /* RW; The height of 2D data by byte */

    td_u32 reserved;
} ot_svp_data;

typedef ot_svp_data ot_svp_src_data;
typedef ot_svp_data ot_svp_dst_data;

typedef union {
    td_s8 s8_val;
    td_u8 u8_val;
} ot_svp_8bit;

/* Definition of u16 point */
typedef struct {
    td_u16 x; /* RW; The X coordinate of the point */
    td_u16 y; /* RW; The Y coordinate of the point */
} ot_svp_point_u16;

/*
 * Definition of s16 point
 */
typedef struct {
    td_s16 x; /* RW; The X coordinate of the point */
    td_s16 y; /* RW; The Y coordinate of the point */
} ot_svp_point_s16;

/*
 * Float point represented by Fixed-point SQ25.7
 */
typedef struct {
    td_s25q7 x; /* RW; The X coordinate of the point */
    td_s25q7 y; /* RW; The Y coordinate of the point */
} ot_svp_point_s25q7;

/*
 * Point u14q2
 */
typedef struct {
    td_u14q2 x;
    td_u14q2 y;
} ot_svp_point_u14q2;

/* Definition of rectangle */
typedef struct {
    td_u16 x;      /* RW; The location of X axis of the rectangle */
    td_u16 y;      /* RW; The location of Y axis of the rectangle */
    td_u16 width;  /* RW; The width of the rectangle */
    td_u16 height; /* RW; The height of the rectangle */
} ot_svp_rect_u16;

typedef struct {
    td_u32 x;      /* RW; The location of X axis of the rectangle */
    td_u32 y;      /* RW; The location of Y axis of the rectangle */
    td_u32 width;  /* RW; The width of the rectangle */
    td_u32 height; /* RW; The height of the rectangle */
} ot_svp_rect_u32;

typedef struct {
    td_s24q8 x;
    td_s24q8 y;
    td_u32 width;
    td_u32 height;
} ot_svp_rect_s24q8;

typedef struct {
    ot_svp_mem_info table;
    td_u16 elem_num; /* RW; LUT's elements number */

    td_u8 table_in_precision;
    td_u8 table_out_norm;

    td_s32 table_in_lower; /* RW; LUT's original input lower limit */
    td_s32 table_in_upper; /* RW; LUT's original input upper limit */
} ot_svp_lut;

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* OT_COMMON_SVP_H */
