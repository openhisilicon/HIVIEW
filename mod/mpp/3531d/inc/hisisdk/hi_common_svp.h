/*
 * Copyright (c) Hisilicon Technologies Co., Ltd. 2019-2019. All rights reserved.
 * Description: svp common header file
 * Author: Hisilicon multimedia software (SVP) group
 * Create: 2019-08-20
 */

#ifndef __HI_COMMON_SVP_H__
#define __HI_COMMON_SVP_H__

#include "hi_type.h"
#include "hi_errno.h"

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */

#define HI_SVP_IMG_ADDR_NUM 3
#define HI_SVP_IMG_STRIDE_NUM 3

/* Blob type */
typedef enum {
    HI_SVP_BLOB_TYPE_S20Q12       =  0x0,

    HI_SVP_BLOB_TYPE_U8           =  0x1,

    /* channel is 3 */
    HI_SVP_BLOB_TYPE_YVU420SP     =  0x2,

    /* channel is 3 */
    HI_SVP_BLOB_TYPE_YVU422SP     =  0x3,

    HI_SVP_BLOB_TYPE_VEC_S20Q12   =  0x4,

    HI_SVP_BLOB_TYPE_SEQ_S20Q12   =  0x5,

    HI_SVP_BLOB_TYPE_BBOX_S20Q12  =  0x6,

    /* this type of blob contains bbox, score and class_id */
    HI_SVP_BLOB_TYPE_BSI_SQ32     =  0x7,

    HI_SVP_BLOB_TYPE_S12Q20       =  0x8,

    HI_SVP_BLOB_TYPE_VEC_S12Q20   =  0x9,

    HI_SVP_BLOB_TYPE_S32          =  0xa,

    HI_SVP_BLOB_TYPE_U32          =  0xb,

    HI_SVP_BLOB_TYPE_FP32         =  0xc,

    HI_SVP_BLOB_TYPE_FP16         =  0xd,

    HI_SVP_BLOB_TYPE_S8           =  0xe,

    HI_SVP_BLOB_TYPE_S16          =  0xf,

    HI_SVP_BLOB_TYPE_U16          =  0x10,

    HI_SVP_BLOB_TYPE_BUTT
} hi_svp_blob_type;

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
 * The stride, which measuring unit is byte, is always algined by the width or
 * dim direction.
 */
typedef struct {
    hi_svp_blob_type type;          /* Blob type */
    hi_u32 stride;                  /* Stride, a line bytes num */

    hi_u64 virt_addr;               /* virtual addr */
    hi_u64 phys_addr;               /* physical addr */

    hi_u32 num;                     /* N: frame num or sequence num, correspond to caffe blob's n */
    union {
        struct {
            hi_u32 width;           /* W: frame width, correspond to caffe blob's w */
            hi_u32 height;          /* H: frame height, correspond to caffe blob's h */
            hi_u32 chn;             /* C: frame channel, correspond to caffe blob's c */
        } whc;                      /* AUTO:hi_svp_blob_type:other; */
        struct {
            hi_u32 dim;             /* D: vecotr dimension */
            hi_u64 virt_addr_step;  /* T: virtual address of time steps array in each sequence */
        } seq;                      /* AUTO:hi_svp_blob_type:HI_SVP_BLOB_TYPE_SEQ_S20Q12; */
    } shape;
} hi_svp_blob;

typedef hi_svp_blob  hi_svp_src_blob;
typedef hi_svp_blob  hi_svp_dst_blob;

/* Mem information */
typedef struct {
    hi_u64  phys_addr; /* RW; The physical address of the memory */
    hi_u64  virt_addr; /* RW; The virtual address of the memory */
    hi_u32  size;      /* RW; The size of memory */
} hi_svp_mem_info;

typedef hi_svp_mem_info hi_svp_src_mem_info;
typedef hi_svp_mem_info hi_svp_dst_mem_info;

/* Image type */
typedef enum {
    HI_SVP_IMG_TYPE_U8C1           =  0x0,
    HI_SVP_IMG_TYPE_S8C1           =  0x1,

    HI_SVP_IMG_TYPE_YUV420SP       =  0x2,       /* YUV420 SemiPlanar */
    HI_SVP_IMG_TYPE_YUV422SP       =  0x3,       /* YUV422 SemiPlanar */
    HI_SVP_IMG_TYPE_YUV420P        =  0x4,       /* YUV420 Planar */
    HI_SVP_IMG_TYPE_YUV422P        =  0x5,       /* YUV422 planar */

    HI_SVP_IMG_TYPE_S8C2_PACKAGE   =  0x6,
    HI_SVP_IMG_TYPE_S8C2_PLANAR    =  0x7,

    HI_SVP_IMG_TYPE_S16C1          =  0x8,
    HI_SVP_IMG_TYPE_U16C1          =  0x9,

    HI_SVP_IMG_TYPE_U8C3_PACKAGE   =  0xa,
    HI_SVP_IMG_TYPE_U8C3_PLANAR    =  0xb,

    HI_SVP_IMG_TYPE_S32C1          =  0xc,
    HI_SVP_IMG_TYPE_U32C1          =  0xd,

    HI_SVP_IMG_TYPE_S64C1          =  0xe,
    HI_SVP_IMG_TYPE_U64C1          =  0xf,

    HI_SVP_IMG_TYPE_BUTT
} hi_svp_img_type;

/* Image */
typedef struct {
    hi_u64  phys_addr[HI_SVP_IMG_ADDR_NUM]; /* RW; The physical address of the image */
    hi_u64  virt_addr[HI_SVP_IMG_ADDR_NUM]; /* RW; The virtual address of the image */
    hi_u32  stride[HI_SVP_IMG_STRIDE_NUM];  /* RW; The stride of the image */
    hi_u32  width;                          /* RW; The width of the image */
    hi_u32  height;                         /* RW; The height of the image */
    hi_svp_img_type  type;                  /* RW; The type of the image */
} hi_svp_img;

typedef hi_svp_img hi_svp_src_img;
typedef hi_svp_img hi_svp_dst_img;

/*
 * The fixed-point data type, will be used to
 * represent float data in hardware calculations.
 */
/* u8bit */
typedef unsigned char hi_u0q8;
typedef unsigned char hi_u1q7;
typedef unsigned char hi_u5q3;
typedef unsigned char hi_u3q5;

/* u16bit */
typedef unsigned short hi_u0q16;
typedef unsigned short hi_u4q12;
typedef unsigned short hi_u6q10;
typedef unsigned short hi_u8q8;
typedef unsigned short hi_u9q7;
typedef unsigned short hi_u12q4;
typedef unsigned short hi_u14q2;
typedef unsigned short hi_u5q11;
typedef unsigned short hi_u1q15;
typedef unsigned short hi_u2q14;
typedef hi_u6q10 hi_ufp16;
/* s16bit */
typedef short hi_s9q7;
typedef short hi_s14q2;
typedef short hi_s1q15;

/* u32bit */
typedef unsigned int hi_u22q10;
typedef unsigned int hi_u25q7;
typedef unsigned int hi_u21q11;
typedef unsigned int hi_u14q18;
typedef unsigned int hi_u8q24;
typedef unsigned int hi_u4q28;

/* s32bit */
typedef int hi_s25q7;
typedef int hi_s16q16;
typedef int hi_s14q18;
typedef int hi_s20q12;
typedef int hi_s24q8;

/*
 * The fixed-point data type combine with flag_bits.
 */
/* 8bits unsigned integer,4bits decimal fraction,4bits flag_bits */
typedef unsigned short hi_u8q4f4;

typedef struct {
    hi_u64 phys_addr; /* RW; The physical address of the data */
    hi_u64 virt_addr; /* RW; The virtaul address of the data */

    hi_u32 stride;   /* RW; The stride of 2D data by byte */
    hi_u32 width;    /* RW; The width of 2D data by byte */
    hi_u32 height;   /* RW; The height of 2D data by byte */

    hi_u32 reserved;
} hi_svp_data;

typedef hi_svp_data hi_svp_src_data;
typedef hi_svp_data hi_svp_dst_data;

typedef union {
    hi_s8 s8_val;
    hi_u8 u8_val;
} hi_svp_8bit;

/* Definition of u16 point */
typedef struct {
    hi_u16 x; /* RW; The X coordinate of the point */
    hi_u16 y; /* RW; The Y coordinate of the point */
} hi_svp_point_u16;

/*
 * Definition of s16 point
 */
typedef struct {
    hi_s16 x; /* RW; The X coordinate of the point */
    hi_s16 y; /* RW; The Y coordinate of the point */
} hi_svp_point_s16;

/*
 * Float point represented by Fixed-point SQ25.7
 */
typedef struct {
    hi_s25q7 x; /* RW; The X coordinate of the point */
    hi_s25q7 y; /* RW; The Y coordinate of the point */
} hi_svp_point_s25q7;

/*
 * Point u14q2
 */
typedef struct {
    hi_u14q2 x;
    hi_u14q2 y;
} hi_svp_point_u14q2;

/* Definition of rectangle */
typedef struct {
    hi_u16 x;      /* RW; The location of X axis of the rectangle */
    hi_u16 y;      /* RW; The location of Y axis of the rectangle */
    hi_u16 width;  /* RW; The width of the rectangle */
    hi_u16 height; /* RW; The height of the rectangle */
} hi_svp_rect_u16;

typedef struct {
    hi_u32 x;      /* RW; The location of X axis of the rectangle */
    hi_u32 y;      /* RW; The location of Y axis of the rectangle */
    hi_u32 width;  /* RW; The width of the rectangle */
    hi_u32 height; /* RW; The height of the rectangle */
} hi_svp_rect_u32;

typedef struct {
    hi_s24q8 x;
    hi_s24q8 y;
    hi_u32 width;
    hi_u32 height;
} hi_svp_rect_s24q8;

typedef struct {
    hi_svp_mem_info table;
    hi_u16 elem_num; /* RW; LUT's elements number */

    hi_u8 table_in_precision;
    hi_u8 table_out_norm;

    hi_s32 table_in_lower; /* RW; LUT's original input lower limit */
    hi_s32 table_in_upper; /* RW; LUT's original input upper limit */
} hi_svp_lut;

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* __HI_COMMON_SVP_H__ */
