/*
  Copyright (c), 2001-2022, Shenshu Tech. Co., Ltd.
 */

#ifndef OT_SSP_H
#define OT_SSP_H

#include "ot_type.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* End of #ifdef __cplusplus */

#define OT_SSP_READ_ALT  0x1
#define OT_SSP_WRITE_ALT 0X3

typedef struct {
    td_u32 spi_no;
    td_u8  dev_addr;
    td_u32 dev_byte_num;
    td_u32 reg_addr;
    td_u32 addr_byte_num;
    td_u32 data;
    td_u32 data_byte_num;
} ot_spi_data;

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */

#endif
