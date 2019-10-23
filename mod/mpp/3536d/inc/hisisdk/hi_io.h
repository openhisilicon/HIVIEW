#ifndef __HI_IO_H__
#define __HI_IO_H__

#include "hi_type.h"

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

/* user and kernel are different. */
HI_U8 IO_READ8(HI_U32 u32Addr);
HI_S32 IO_WRITE8(HI_U32 u32Addr, HI_U32 u32Value);
HI_U16 IO_READ16(HI_U32 u32Addr);
HI_S32 IO_WRITE16(HI_U32 u32Addr, HI_U32 u32Value);
HI_U32 IO_READ32(HI_U32 u32Addr);
HI_S32 IO_WRITE32(HI_U32 u32Addr, HI_U32 u32Value);
HI_U32 ISP_Mmap(void);
HI_U32 ISP_Munmap(void);
HI_U32 ISP_GetExtRegAddr(void);

#define REG_ACCESS_WIDTH	0				// 1: 16bit   2: 8bit
#define EXT_REG_BASE		0x10200
#define ISP_REG_BASE_ADDR	0x205a0000

#define EXT_REG_BASE_ISP 0x8000
#define EXT_REG_BASE_VIU 0x8400
#define EXT_REG_BASE_VPP 0x8600
#define EXT_REG_BASE_VEDU 0x8800
#define EXT_REG_BASE_VOU 0x8A00

/* Dynamic bus access functions, 4 byte align access */
//TODO: allocate dev addr (such as ISP_REG_BASE_ADDR) according to devId.
#define __IO_CALC_ADDRESS_DYNAMIC(BASE, OFFSET)	((HI_U32)(((BASE >= EXT_REG_BASE)? 0 : ISP_REG_BASE_ADDR) + ((((BASE >= EXT_REG_BASE)? (BASE - EXT_REG_BASE) : BASE) + OFFSET)<<REG_ACCESS_WIDTH)))

#define IORD_32DIRECT(BASE, OFFSET) 		IO_READ32(__IO_CALC_ADDRESS_DYNAMIC ((BASE), (OFFSET)))
#define IORD_16DIRECT(BASE, OFFSET) 		IO_READ16(__IO_CALC_ADDRESS_DYNAMIC ((BASE), (OFFSET)))
#define IORD_8DIRECT(BASE,  OFFSET) 		IO_READ8(__IO_CALC_ADDRESS_DYNAMIC ((BASE), (OFFSET)))

#define IOWR_32DIRECT(BASE, OFFSET, DATA)	IO_WRITE32(__IO_CALC_ADDRESS_DYNAMIC ((BASE), (OFFSET)), (DATA))
#define IOWR_16DIRECT(BASE, OFFSET, DATA) 	IO_WRITE16(__IO_CALC_ADDRESS_DYNAMIC ((BASE), (OFFSET)), (DATA))
#define IOWR_8DIRECT(BASE,  OFFSET, DATA) 	IO_WRITE8(__IO_CALC_ADDRESS_DYNAMIC ((BASE), (OFFSET)), (DATA))


inline static void HI_RegSetBit(unsigned long value, unsigned long offset,
    unsigned long addr)
{
	unsigned long t, mask;

	mask = 1 << offset;
	t = readl((const volatile void *)addr);
	t &= ~mask;
	t |= (value << offset) & mask;
	writel(t, (volatile void *)addr);
}

inline static void HI_RegWrite32(unsigned long value, unsigned long mask,
    unsigned long addr)
{
	unsigned long t;

	t = readl((const volatile void *)addr);
	t &= ~mask;
	t |= value & mask;
	writel(t, (volatile void *)addr);
}

inline static void HI_RegRead(unsigned long *pvalue, unsigned long addr)
{
	*pvalue = readl((const volatile void *)addr);
}

inline static void HI_RegSetBitEx(unsigned long value, unsigned long offset, 
    unsigned long bitwidth, unsigned long addr)
{
	unsigned long t, mask=0;
    int i = 0;

    while (i < bitwidth)
    {
        mask |= (1<<i);
        i++;
    };
	mask = (mask << offset);
	t = readl((const volatile void *)addr);
	t &= ~mask;
	t |= (value << offset) & mask;
	writel(t, (volatile void *)addr);    
}

#ifdef __cplusplus
}
#endif

#endif /* __HI_IO_H__ */

