/*
  Copyright (c), 2001-2024, Shenshu Tech. Co., Ltd.
 */

#ifndef OT_ISP_BIN_H
#define OT_ISP_BIN_H

#include "ot_type.h"
#include "ot_common_isp.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* End of #ifdef __cplusplus */

/* MACRO DEFINITION */
#define MAX_BIN_REG_NUM 5

/* GENERAL STRUCTURES */
typedef struct {
    td_u32  addr;    /* register addr */
    td_u8   start_bit; /* start bit of register addr */
    td_u8   end_bit;   /* end bit of register addr */
} ot_isp_bin_reg_attr;

/*
 * The base addr of ISP logic register
 * The base addr of ISP ext register
 * The base addr of AE ext register
 * The base addr of AWB ext register
 */
ot_isp_bin_reg_attr g_isp_bin_reg_attr[OT_ISP_MAX_PIPE_NUM][MAX_BIN_REG_NUM] = {
    [0 ...(OT_ISP_MAX_PIPE_NUM - 1)] = {0}
};

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */

#endif /* OT_ISP_BIN_H */
