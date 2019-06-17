/******************************************************************************

  Copyright (C), 2001-2011, Hisilicon Tech. Co., Ltd.

 ******************************************************************************
  File Name     : hi_vreg.h
  Version       : Initial Draft
  Author        : Hisilicon multimedia software group
  Created       : 2013/01/09
  Description   : 
  History       :
  1.Date        : 2013/01/09
    Author      : n00168968
    Modification: Created file

******************************************************************************/

#ifndef __HI_VREG_H__
#define __HI_VREG_H__

#include "hi_type.h"
#include "hi_comm_isp.h"

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* End of #ifdef __cplusplus */

#define REG_ACCESS_WIDTH_1  0
#define REG_ACCESS_WIDTH    0                /* 1: 16bit   2: 8bit */

/* Vreg is a blok of memory alloced to simulate the regs.
 * We try to differentiate vregs by baseaddr.  Each vreg
 * block's size should at least 4k bytes.
 */
 
 /* ISP0 */
  /*----------------------------------*
   *| 0x1138_0000 |  0x20_0000 | ... |*
   *|-------------|------------|-----|*
   *| VI/ISP_REG  |  ISP_VREG  | ... |*
   *----------------------------------*/

/* ISP1 */
  /*----------------------------------*
   *| 0x1148_0000 |  0x21_0000 | ... |*
   *|-------------|------------|-----|*
   *| VI/ISP_REG  |  ISP_VREG  | ... |*
   *----------------------------------*/

/* AE/AWB */
  /*------------------------------------------------------------------*
   *|  0x25_0000 |  0x25_1000 | ... |  0x26_0000  | ... |  0x27_0000 |*
   *|------------|------------|-----|-------------|-----|------------|*
   *|  AE1_VREG  |  AE2_VREG  | ... |  AWB1_VREG  | ... |  AF1_VREG  |*
   *------------------------------------------------------------------*/

#define VREG_BASE               0x200000
#define VREG_SIZE_ALIGN         0x1000
//#define ISP_MAX_DEV_NUM         2


#define VI_ISP0_REG_BASE        0x11380000
#define VI_ISP1_REG_BASE        0x11480000
#define VI_ISP_REG_SIZE         0x6FFFF
#define VI_ISP_OFFSET           0x20000
#define ISP_REG_BASE(IspDev)    ((IspDev) ? VI_ISP1_REG_BASE : VI_ISP0_REG_BASE)

#define ISP_VREG_SIZE           (VREG_SIZE_ALIGN << 4)
#define ISP0_VREG_BASE          (VREG_BASE)
#define ISP1_VREG_BASE          (VREG_BASE + ISP_VREG_SIZE)
#define ISP_VREG_BASE(IspDev)   ((IspDev) ? ISP1_VREG_BASE : ISP0_VREG_BASE)

#define AE_VREG_BASE            0x250000
#define AWB_VREG_BASE           0x260000
#define AF_VREG_BASE            0x270000
#define AE_LIB_VREG_BASE(id)    (AE_VREG_BASE + VREG_SIZE_ALIGN * (id))
#define AWB_LIB_VREG_BASE(id)   (AWB_VREG_BASE + VREG_SIZE_ALIGN * (id))
#define AF_LIB_VREG_BASE(id)    (AF_VREG_BASE + VREG_SIZE_ALIGN * (id))
#define ALG_LIB_VREG_SIZE       (VREG_SIZE_ALIGN)
#define MAX_ALG_LIB_VREG_NUM    (1 << 4)

#define HISI_AE_LIB_EXTREG_ID_0 0
#define HISI_AE_LIB_EXTREG_ID_1 1

#define HISI_AWB_LIB_EXTREG_ID_0 0
#define HISI_AWB_LIB_EXTREG_ID_1 1


HI_S32 VReg_Init(HI_U32 u32BaseAddr, HI_U32 u32Size);
HI_S32 VReg_Exit(HI_U32 u32BaseAddr, HI_U32 u32Size);
HI_S32 VReg_ReleaseAll(HI_VOID);
HI_U32 VReg_GetVirtAddr(HI_U32 u32BaseAddr);
HI_S32 VReg_Munmap(HI_U32 u32BaseAddr, HI_U32 u32Size);
HI_U32 IO_READ32(HI_U32 u32Addr);
HI_S32 IO_WRITE32(HI_U32 u32Addr, HI_U32 u32Value);
HI_U16 IO_READ16(HI_U32 u32Addr);
HI_S32 IO_WRITE16(HI_U32 u32Addr, HI_U32 u32Value);
HI_U8  IO_READ8(HI_U32 u32Addr);
HI_S32 IO_WRITE8(HI_U32 u32Addr, HI_U32 u32Value);

/* Dynamic bus access functions, 4 byte align access */
//TODO: allocate dev addr (such as ISP_REG_BASE_ADDR) according to devId.
//#define __IO_CALC_ADDRESS_DYNAMIC(BASE)    (HI_U32)(((BASE >= EXT_REG_BASE) ? 0 : ISP_REG_BASE) + (BASE))
#define __IO_CALC_ADDRESS_DYNAMIC(BASE)    (BASE)


#define IORD_32DIRECT(BASE)             IO_READ32(__IO_CALC_ADDRESS_DYNAMIC(BASE))
#define IORD_16DIRECT(BASE)             IO_READ16(__IO_CALC_ADDRESS_DYNAMIC(BASE))
#define IORD_8DIRECT(BASE)              IO_READ8(__IO_CALC_ADDRESS_DYNAMIC(BASE))

#define IOWR_32DIRECT(BASE, DATA)       IO_WRITE32(__IO_CALC_ADDRESS_DYNAMIC(BASE), (DATA))
#define IOWR_16DIRECT(BASE, DATA)       IO_WRITE16(__IO_CALC_ADDRESS_DYNAMIC(BASE), (DATA))
#define IOWR_8DIRECT(BASE, DATA)        IO_WRITE8(__IO_CALC_ADDRESS_DYNAMIC(BASE), (DATA))


/*--------------------------------------------------------------------------------------*/

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */

#endif
