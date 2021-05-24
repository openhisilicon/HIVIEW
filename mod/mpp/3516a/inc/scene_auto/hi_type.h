/**
* Copyright (C), 2016-2020, Hisilicon Tech. Co., Ltd.
* All rights reserved.
*
* @file      hi_type.h
* @brief     define of common type
* @author    HiMobileCam middleware develop team
* @date      2016.06.08
*/
#ifndef __HI_TYPE_H__
#define __HI_TYPE_H__


#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

/** \addtogroup     COMM */
/** @{ */  /** <!-- [COMM] */

/*----------------------------------------------*
 * The common data type, will be used in the whole project.*
 *----------------------------------------------*/

typedef unsigned char           HI_U8;
typedef unsigned short          HI_U16;
typedef unsigned int            HI_U32;

typedef signed char             HI_S8;
typedef short                   HI_S16;
typedef int                     HI_S32;

/*----------------------------------------------*
 * The fixed-point data type, used to represent float data in hardware calculations.*
 *----------------------------------------------*/
/*--u8bit---------------------------------------*/
typedef unsigned char           HI_U0Q8;
typedef unsigned char           HI_U1Q7;
typedef unsigned char           HI_U5Q3;

/*--u16bit---------------------------------------*/
typedef unsigned short          HI_U0Q16;
typedef unsigned short          HI_U4Q12;
typedef unsigned short          HI_U6Q10;
typedef unsigned short          HI_U8Q8;
typedef unsigned short          HI_U12Q4;
typedef unsigned short          HI_U14Q2;

/*--s16bit---------------------------------------*/
typedef short                   HI_S9Q7;
typedef short                   HI_S14Q2;
typedef short                   HI_S1Q15;

/*--u32bit---------------------------------------*/
typedef unsigned int            HI_U22Q10;
typedef unsigned int            HI_U25Q7;

/*--s32bit---------------------------------------*/
typedef int                     HI_S25Q7;
typedef int                     HI_S16Q16;

/*----------------------------------------------*
 * The fixed-point data type combine with FLAG bits.*
 *----------------------------------------------*/

/*8bits unsigned integer,4bits decimal fraction,4bits flag bits*/
typedef unsigned short          HI_U8Q4F4;

/*float*/
typedef float               HI_FLOAT;
/*double*/
typedef double                  HI_DOUBLE;


#ifndef _M_IX86
typedef unsigned long long  HI_U64;
typedef long long           HI_S64;
#else
typedef __int64             HI_U64;
typedef __int64             HI_S64;
#endif

typedef char                    HI_CHAR;
#define HI_VOID                 void

typedef HI_U32                  HI_HANDLE;


typedef unsigned long           HI_SIZE_T;

typedef long           HI_LONG;

#define HI_INVALID_HANDLE   (-1)

/*----------------------------------------------*
 * const defination                             *
 *----------------------------------------------*/
typedef enum
{
    HI_FALSE = 0,
    HI_TRUE  = 1,
} HI_BOOL;

#ifndef NULL
#define NULL    0L
#endif

#define HI_NULL     0L
#define HI_NULL_PTR 0L

#define HI_SUCCESS  0
#define HI_FAILURE  (-1)

/** @}*/  /** <!-- ==== COMM End ====*/
#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* __HI_TYPE_H__ */

