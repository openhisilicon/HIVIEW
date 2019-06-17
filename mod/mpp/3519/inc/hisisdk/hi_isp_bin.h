/******************************************************************************

  Copyright (C), 2001-2015, Hisilicon Tech. Co., Ltd.

 ******************************************************************************
  File Name     : hi_isp_bin.h
  Version       : Initial Draft
  Author        : Hisilicon multimedia software group
  Created       : 2015/01/14
  Description   : 
  History       :
  1.Date        : 2015/01/14
    Author      : l00184411
    Modification: Created file

******************************************************************************/

#ifndef __HI_ISP_BIN_H__
#define __HI_ISP_BIN_H__

#include "hi_type.h"
#include "hi_comm_isp.h"

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* End of #ifdef __cplusplus */

/****************************************************************************
 * MACRO DEFINITION                                                         *
 ****************************************************************************/
#define MAX_BIN_REG_NUM 7

/****************************************************************************
 * GENERAL STRUCTURES                                                       *
 ****************************************************************************/
typedef struct hiISP_BIN_REG_ATTR_S
{
    HI_U32  u32Addr;    /* register addr */
    HI_U8   u8StartBit; /* start bit of register addr */
    HI_U8   u8EndBit;   /* end bit of register addr */
} ISP_BIN_REG_ATTR_S;

/* The base addr of ISP0/ISP1 logic register is 0x113A0000/0x114A0000 */
/* The base addr of ISP ext register is 0x200000 */
/* The base addr of Hisi AE ext register is 0x250000 */
/* The base addr of Hisi AWB ext register is 0x260000 */
ISP_BIN_REG_ATTR_S g_astIspBinRegAttr[ISP_MAX_DEV_NUM][MAX_BIN_REG_NUM] = 
{
    {
        {0x113A0010,  0, 15},   /* ISP0 image width */
        {0x113A0014,  0, 15},   /* ISP0 image height */
        {0x113A0124,  0, 15},   /* ISP0 image height */
        {0x113E003c,  0, 15},   /* ISP0 crop width */
        {0x113E003e,  0, 15},   /* ISP0 crop height */
        {0x201400,    0, 31},   /* ISP0 fps */
        {0x250180,    0, 31}    /* AE0 fps */
    },
    {
        {0x114A0010,  0, 15},   /* ISP1 image width */
        {0x114A0014,  0, 15},   /* ISP1 image height */
        {0x114A0124,  0, 15},   /* ISP1 image height */
        {0x114E003c,  0, 15},   /* ISP1 crop width */
        {0x114E003e,  0, 15},   /* ISP1 crop height */
        {0x211400,    0, 31},   /* ISP1 fps */
        {0x251180,    0, 31}    /* AE1 fps */
    }
};


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */

#endif /*__HI_ISP_BIN_H__ */

