#ifndef __HI_SPI_H__
#define __HI_SPI_H__

typedef unsigned long long  __u64;
typedef unsigned int        __u32;
typedef unsigned short      __u16;
typedef unsigned char       __u8;

#ifdef __HuaweiLite__
#include <spi.h>
#else
#include <linux/spi/spidev.h>
#endif

#endif /* __HI_SPI_H__ */
