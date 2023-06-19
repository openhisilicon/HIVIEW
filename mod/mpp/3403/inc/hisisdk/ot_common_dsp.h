/*
  Copyright (c), 2001-2022, Shenshu Tech. Co., Ltd.
 */
#ifndef OT_COMMON_DSP_H
#define OT_COMMON_DSP_H

#include "ot_common_svp.h"
#include "ot_common.h"
#include "ot_errno.h"
#include "ot_debug.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/* SVP_DSP Error Code */
typedef enum {
    OT_ERR_SVP_DSP_SYS_TIMEOUT = 0x40,   /* SVP_DSP process timeout */
    OT_ERR_SVP_DSP_QUERY_TIMEOUT = 0x41, /* SVP_DSP query timeout */
    OT_ERR_SVP_DSP_OPEN_FILE = 0x42,     /* SVP_DSP open file error */
    OT_ERR_SVP_DSP_READ_FILE = 0x43,     /* SVP_DSP read file error */

    OT_ERR_SVP_DSP_BUTT
} ot_svp_dsp_err_code;

/* Invalid device ID */
#define OT_ERR_SVP_DSP_INVALID_DEV_ID OT_DEFINE_ERR(OT_ID_SVP_DSP, OT_ERR_LEVEL_ERROR, OT_ERR_INVALID_DEV_ID)
/* Invalid channel ID */
#define OT_ERR_SVP_DSP_INVALID_CHN_ID OT_DEFINE_ERR(OT_ID_SVP_DSP, OT_ERR_LEVEL_ERROR, OT_ERR_INVALID_CHN_ID)
/* At least one parameter is illegal. For example, an illegal enumeration value exists. */
#define OT_ERR_SVP_DSP_ILLEGAL_PARAM OT_DEFINE_ERR(OT_ID_SVP_DSP, OT_ERR_LEVEL_ERROR, OT_ERR_ILLEGAL_PARAM)
/* The channel exists. */
#define OT_ERR_SVP_DSP_EXIST OT_DEFINE_ERR(OT_ID_SVP_DSP, OT_ERR_LEVEL_ERROR, OT_ERR_EXIST)
/* The UN exists. */
#define OT_ERR_SVP_DSP_UNEXIST OT_DEFINE_ERR(OT_ID_SVP_DSP, OT_ERR_LEVEL_ERROR, OT_ERR_UNEXIST)
/* A null point is used. */
#define OT_ERR_SVP_DSP_NULL_PTR OT_DEFINE_ERR(OT_ID_SVP_DSP, OT_ERR_LEVEL_ERROR, OT_ERR_NULL_PTR)
/* Try to enable or initialize the system, device, or channel before configuring attributes. */
#define OT_ERR_SVP_DSP_NOT_CONFIG OT_DEFINE_ERR(OT_ID_SVP_DSP, OT_ERR_LEVEL_ERROR, OT_ERR_NOT_CFG)
/* The operation is not supported currently. */
#define OT_ERR_SVP_DSP_NOT_SURPPORT OT_DEFINE_ERR(OT_ID_SVP_DSP, OT_ERR_LEVEL_ERROR, OT_ERR_NOT_SUPPORT)
/* The operation, changing static attributes for example, is not permitted. */
#define OT_ERR_SVP_DSP_NOT_PERM OT_DEFINE_ERR(OT_ID_SVP_DSP, OT_ERR_LEVEL_ERROR, OT_ERR_NOT_PERM)
/* A failure caused by the malloc memory occurs. */
#define OT_ERR_SVP_DSP_NO_MEM OT_DEFINE_ERR(OT_ID_SVP_DSP, OT_ERR_LEVEL_ERROR, OT_ERR_NO_MEM)
/* A failure caused by the malloc buffer occurs. */
#define OT_ERR_SVP_DSP_NO_BUF OT_DEFINE_ERR(OT_ID_SVP_DSP, OT_ERR_LEVEL_ERROR, OT_ERR_NO_BUF)
/* The buffer is empty. */
#define OT_ERR_SVP_DSP_BUF_EMPTY OT_DEFINE_ERR(OT_ID_SVP_DSP, OT_ERR_LEVEL_ERROR, OT_ERR_BUF_EMPTY)
/* No buffer is provided for storing new data. */
#define OT_ERR_SVP_DSP_BUF_FULL OT_DEFINE_ERR(OT_ID_SVP_DSP, OT_ERR_LEVEL_ERROR, OT_ERR_BUF_FULL)
/* The system is not ready because it may be not initialized or loaded.
 * The error code is returned when a device file fails to be opened. */
#define OT_ERR_SVP_DSP_NOT_READY OT_DEFINE_ERR(OT_ID_SVP_DSP, OT_ERR_LEVEL_ERROR, OT_ERR_NOT_READY)
/* The source address or target address is incorrect during the operations
such as calling copy_from_user or copy_to_user. */
#define OT_ERR_SVP_DSP_BAD_ADDR OT_DEFINE_ERR(OT_ID_SVP_DSP, OT_ERR_LEVEL_ERROR, OT_ERR_BAD_ADDR)
/* The resource is busy during the operations such as destroying a VENC channel without deregistering it. */
#define OT_ERR_SVP_DSP_BUSY OT_DEFINE_ERR(OT_ID_SVP_DSP, OT_ERR_LEVEL_ERROR, OT_ERR_BUSY)
/* SVP_DSP process timeout */
#define OT_ERR_SVP_DSP_SYS_TIMEOUT OT_DEFINE_ERR(OT_ID_SVP_DSP, OT_ERR_LEVEL_ERROR, OT_ERR_SVP_DSP_SYS_TIMEOUT)
/* SVP_DSP query timeout */
#define OT_ERR_SVP_DSP_QUERY_TIMEOUT OT_DEFINE_ERR(OT_ID_SVP_DSP, OT_ERR_LEVEL_ERROR, OT_ERR_SVP_DSP_QUERY_TIMEOUT)
/* SVP_DSP open file error */
#define OT_ERR_SVP_DSP_OPEN_FILE OT_DEFINE_ERR(OT_ID_SVP_DSP, OT_ERR_LEVEL_ERROR, OT_ERR_SVP_DSP_OPEN_FILE)
/* SVP_DSP read file error */
#define OT_ERR_SVP_DSP_READ_FILE OT_DEFINE_ERR(OT_ID_SVP_DSP, OT_ERR_LEVEL_ERROR, OT_ERR_SVP_DSP_READ_FILE)

/* SVP_DSP core id */
typedef enum {
    OT_SVP_DSP_ID_0 = 0x0,
	OT_SVP_DSP_ID_1 = 0x1,

    OT_SVP_DSP_ID_BUTT
} ot_svp_dsp_id;

/* SVP_DSP  priority */
typedef enum {
    OT_SVP_DSP_PRI_0 = 0x0,
    OT_SVP_DSP_PRI_1 = 0x1,
    OT_SVP_DSP_PRI_2 = 0x2,

    OT_SVP_DSP_PRI_BUTT
} ot_svp_dsp_pri;

/* SVP_DSP memory type */
typedef enum {
    OT_SVP_DSP_MEM_TYPE_SYS_DDR_DSP_0 = 0x0,
    OT_SVP_DSP_MEM_TYPE_IRAM_DSP_0 = 0x1,
    OT_SVP_DSP_MEM_TYPE_DRAM_0_DSP_0 = 0x2,
    OT_SVP_DSP_MEM_TYPE_DRAM_1_DSP_0 = 0x3,

    OT_SVP_DSP_MEM_TYPE_SYS_DDR_DSP_1 = 0x4,
    OT_SVP_DSP_MEM_TYPE_IRAM_DSP_1 = 0x5,
    OT_SVP_DSP_MEM_TYPE_DRAM_0_DSP_1 = 0x6,
    OT_SVP_DSP_MEM_TYPE_DRAM_1_DSP_1 = 0x7,

    OT_SVP_DSP_MEM_TYPE_BUTT
} ot_svp_dsp_mem_type;

/* SVP_DSP  cmd */
typedef enum {
    OT_SVP_DSP_CMD_INIT = 0x0,
    OT_SVP_DSP_CMD_EXIT = 0x1,
    OT_SVP_DSP_CMD_DILATE_3X3 = 0x2,
    OT_SVP_DSP_CMD_ERODE_3X3 = 0x3,
#ifdef CONFIG_OT_PHOTO_SUPPORT
    OT_SVP_DSP_CMD_PHOTO_PROC = 0x4,
#endif

    OT_SVP_DSP_CMD_BUTT
} ot_svp_dsp_cmd;

/* SVP_DSP ARM->DSP request message */
typedef struct {
    td_u32 cmd;     /* CMD ID, user-defined SVP_DSP_CMD_BUTT + */
    td_u32 msg_id;   /* Message ID */
    td_u64 body;    /* Message body */
    td_u32 body_len; /* Length of pBody */
} ot_svp_dsp_msg;

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif
#endif
