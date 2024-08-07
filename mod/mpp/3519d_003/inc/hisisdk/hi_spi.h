/*
  Copyright (c), 2001-2022, Shenshu Tech. Co., Ltd.
 */

#ifndef HI_SPI_H
#define HI_SPI_H

#include "hi_type.h"
#include <spi.h>
#include "ot_spi.h"

#ifdef __cplusplus
extern "C" {
#endif

#define HI_SPI_CPHA OT_SPI_CPHA
#define HI_SPI_CPOL OT_SPI_CPOL
#define HI_SPI_MODE_0 OT_SPI_MODE_0
#define HI_SPI_MODE_1 OT_SPI_MODE_1
#define HI_SPI_MODE_2 OT_SPI_MODE_2
#define HI_SPI_MODE_3 OT_SPI_MODE_3
#define HI_SPI_CS_HIGH OT_SPI_CS_HIGH
#define HI_SPI_LSB_FIRST OT_SPI_LSB_FIRST
#define HI_SPI_3WIRE OT_SPI_3WIRE
#define HI_SPI_LOOP OT_SPI_LOOP
#define HI_SPI_NO_CS OT_SPI_NO_CS
#define HI_SPI_READY OT_SPI_READY
#define HI_SPI_IOC_MAGIC OT_SPI_IOC_MAGIC
typedef ot_spi_ioc_transfer hi_spi_ioc_transfer;
#define HI_SPI_IOC_RD_MODE OT_SPI_IOC_RD_MODE
#define HI_SPI_IOC_WR_MODE OT_SPI_IOC_WR_MODE
#define HI_SPI_IOC_RD_LSB_FIRST OT_SPI_IOC_RD_LSB_FIRST
#define HI_SPI_IOC_WR_LSB_FIRST OT_SPI_IOC_WR_LSB_FIRST
#define HI_SPI_IOC_RD_BITS_PER_WORD OT_SPI_IOC_RD_BITS_PER_WORD
#define HI_SPI_IOC_WR_BITS_PER_WORD OT_SPI_IOC_WR_BITS_PER_WORD
#define HI_SPI_IOC_RD_MAX_SPEED_HZ OT_SPI_IOC_RD_MAX_SPEED_HZ
#define HI_SPI_IOC_WR_MAX_SPEED_HZ OT_SPI_IOC_WR_MAX_SPEED_HZ

#ifdef __cplusplus
}
#endif
#endif /* HI_SPI_H */
