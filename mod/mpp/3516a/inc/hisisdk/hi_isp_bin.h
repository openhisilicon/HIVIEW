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

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* End of #ifdef __cplusplus */

/****************************************************************************
 * MACRO DEFINITION                                                         *
 ****************************************************************************/
#define MAX_BIN_REG_NUM 6

/****************************************************************************
 * GENERAL STRUCTURES                                                       *
 ****************************************************************************/
typedef struct hiISP_BIN_REG_ATTR_S
{
    HI_U32  u32Addr;    /* register addr */
    HI_U8   u8StartBit; /* start bit of register addr */
    HI_U8   u8EndBit;   /* end bit of register addr */
} ISP_BIN_REG_ATTR_S;

/* The base addr of ISP logic register is 0x205A0000 */
/* The base addr of ISP ext register is 0x10000 */
/* The base addr of Hisi AE ext register is 0x20000 */
/* The base addr of Hisi AWB ext register is 0x30000 */
ISP_BIN_REG_ATTR_S g_astIspBinRegAttr[MAX_BIN_REG_NUM] = 
{
    {0x205A0010,  0, 15},   /* ISP image width */
    {0x205A0014,  0, 15},   /* ISP image height */

    {0x205A0110,  0, 15},   /* ISP crop width */
    {0x205A0124,  0, 15},   /* ISP crop height */

    {0x11400,     0, 31},   /* ISP fps */

    {0x20120,     0, 31}    /* AE fps */
};


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */

#endif /*__HI_ISP_BIN_H__ */

