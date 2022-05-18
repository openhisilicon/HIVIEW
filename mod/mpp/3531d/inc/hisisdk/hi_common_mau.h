/*
 * Copyright (c) Hisilicon Technologies Co., Ltd. 2019-2019. All rights reserved.
 * Description: hi_common_mau.h file
 * Author: Hisilicon multimedia software (SVP) group
 * Create: 2019-05-28
 */

#ifndef _HI_COMMON_MAU_H_
#define _HI_COMMON_MAU_H_

#include "hi_common_svp.h"
#include "hi_common.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/* SVP_MAU Error Code */
typedef enum {
    HI_SVP_MAU_ERR_SYS_TIMEOUT = 0x40, /* SVP MAU process timeout */
    HI_SVP_MAU_ERR_QUERY_TIMEOUT = 0x41, /* SVP MAU query timeout */
    HI_SVP_MAU_ERR_CFG_ERR = 0x42, /* SVP MAU Configure error */
    HI_SVP_MAU_ERR_BUS_ERR = 0x43, /* SVP  MAU Configure error */
    HI_SVP_MAU_ERR_OPEN_FILE = 0x44, /* SVP MAU open file error */
    HI_SVP_MAU_ERR_READ_FILE = 0x45, /* SVP MAU read file error */

    HI_SVP_MAU_ERR_BUTT
} hi_svp_mau_err_code;
/* Invalid device ID */
#define HI_ERR_SVP_MAU_INVALID_DEV_ID     HI_DEFINE_ERR(HI_ID_SVP_MAU, HI_ERR_LEVEL_ERROR, HI_ERR_INVALID_DEV_ID)
/* Invalid channel ID */
#define HI_ERR_SVP_MAU_INVALID_CHN_ID     HI_DEFINE_ERR(HI_ID_SVP_MAU, HI_ERR_LEVEL_ERROR, HI_ERR_INVALID_CHN_ID)
/* At least one parameter is illegal. For example, an illegal enumeration value exists. */
#define HI_ERR_SVP_MAU_ILLEGAL_PARAM     HI_DEFINE_ERR(HI_ID_SVP_MAU, HI_ERR_LEVEL_ERROR, HI_ERR_ILLEGAL_PARAM)
/* The channel exists. */
#define HI_ERR_SVP_MAU_EXIST             HI_DEFINE_ERR(HI_ID_SVP_MAU, HI_ERR_LEVEL_ERROR, HI_ERR_EXIST)
/* The UN exists. */
#define HI_ERR_SVP_MAU_UNEXIST           HI_DEFINE_ERR(HI_ID_SVP_MAU, HI_ERR_LEVEL_ERROR, HI_ERR_UNEXIST)
/* A null point is used. */
#define HI_ERR_SVP_MAU_NULL_PTR          HI_DEFINE_ERR(HI_ID_SVP_MAU, HI_ERR_LEVEL_ERROR, HI_ERR_NULL_PTR)
/* Try to enable or initialize the system, device, or channel before configuring attributes. */
#define HI_ERR_SVP_MAU_NOT_CFG        HI_DEFINE_ERR(HI_ID_SVP_MAU, HI_ERR_LEVEL_ERROR, HI_ERR_NOT_CFG)
/* The operation is not supported currently. */
#define HI_ERR_SVP_MAU_NOT_SUPPORT      HI_DEFINE_ERR(HI_ID_SVP_MAU, HI_ERR_LEVEL_ERROR, HI_ERR_NOT_SUPPORT)
/* The operation, changing static attributes for example, is not permitted. */
#define HI_ERR_SVP_MAU_NOT_PERM          HI_DEFINE_ERR(HI_ID_SVP_MAU, HI_ERR_LEVEL_ERROR, HI_ERR_NOT_PERM)
/* A failure caused by the malloc memory occurs. */
#define HI_ERR_SVP_MAU_NO_MEM             HI_DEFINE_ERR(HI_ID_SVP_MAU, HI_ERR_LEVEL_ERROR, HI_ERR_NO_MEM)
/* A failure caused by the malloc buffer occurs. */
#define HI_ERR_SVP_MAU_NO_BUF             HI_DEFINE_ERR(HI_ID_SVP_MAU, HI_ERR_LEVEL_ERROR, HI_ERR_NO_BUF)
/* The buffer is empty. */
#define HI_ERR_SVP_MAU_BUF_EMPTY         HI_DEFINE_ERR(HI_ID_SVP_MAU, HI_ERR_LEVEL_ERROR, HI_ERR_BUF_EMPTY)
/* No buffer is provided for storing new data. */
#define HI_ERR_SVP_MAU_BUF_FULL          HI_DEFINE_ERR(HI_ID_SVP_MAU, HI_ERR_LEVEL_ERROR, HI_ERR_BUF_FULL)
/*
 * The system is not ready because it may be not initialized or loaded.
 * The error code is returned when a device file fails to be opened.
 */
#define HI_ERR_SVP_MAU_NOT_READY         HI_DEFINE_ERR(HI_ID_SVP_MAU, HI_ERR_LEVEL_ERROR, HI_ERR_NOT_READY)
/*
 * The source address or target address is incorrect during the operations such as
 * calling copy_from_user or copy_to_user.
 */
#define HI_ERR_SVP_MAU_BAD_ADDR          HI_DEFINE_ERR(HI_ID_SVP_MAU, HI_ERR_LEVEL_ERROR, HI_ERR_BAD_ADDR)
/* The resource is busy during the operations. */
#define HI_ERR_SVP_MAU_BUSY              HI_DEFINE_ERR(HI_ID_SVP_MAU, HI_ERR_LEVEL_ERROR, HI_ERR_BUSY)
/* SVP_MAU process timeout */
#define HI_ERR_SVP_MAU_SYS_TIMEOUT       HI_DEFINE_ERR(HI_ID_SVP_MAU, HI_ERR_LEVEL_ERROR, HI_SVP_MAU_ERR_SYS_TIMEOUT)
/* SVP_MAU query timeout */
#define HI_ERR_SVP_MAU_QUERY_TIMEOUT     HI_DEFINE_ERR(HI_ID_SVP_MAU, HI_ERR_LEVEL_ERROR, HI_SVP_MAU_ERR_QUERY_TIMEOUT)
/* SVP_MAU configure error */
#define HI_ERR_SVP_MAU_CFG_ERR           HI_DEFINE_ERR(HI_ID_SVP_MAU, HI_ERR_LEVEL_ERROR, HI_SVP_MAU_ERR_CFG_ERR)

#define HI_ERR_SVP_MAU_BUS_ERR            HI_DEFINE_ERR(HI_ID_SVP, HI_ERR_LEVEL_ERROR, HI_SVP_MAU_ERR_BUS_ERR)

#define HI_ERR_SVP_MAU_OPEN_FILE         HI_DEFINE_ERR(HI_ID_SVP_MAU, HI_ERR_LEVEL_ERROR, HI_SVP_MAU_ERR_OPEN_FILE)
#define HI_ERR_SVP_MAU_READ_FILE         HI_DEFINE_ERR(HI_ID_SVP_MAU, HI_ERR_LEVEL_ERROR, HI_SVP_MAU_ERR_READ_FILE)

typedef struct {
    hi_svp_blob left_matrix;
    hi_svp_blob right_matrix;
} hi_svp_mau_double_matrix;

typedef struct {
    hi_svp_blob op_result;
    hi_svp_blob top_n;
    hi_svp_blob top_n_idx;
} hi_svp_mau_dist_result;

typedef hi_svp_mau_double_matrix hi_svp_mau_src_double_matrix;

typedef enum {
    HI_SVP_MAU_OUT_OP_RESULT = 0x0,
    HI_SVP_MAU_OUT_TOP_N     = 0x1,
    HI_SVP_MAU_OUT_BOTH      = 0x2,
    HI_SVP_MAU_OUT_BUTT
} hi_svp_mau_out_type;

typedef enum {
    HI_SVP_MAU_ID_0 = 0x0,
    HI_SVP_MAU_ID_BUTT
} hi_svp_mau_id;

typedef struct {
    hi_svp_mau_id mau_id;
    hi_svp_mau_out_type out_type;
    hi_bool fp32_to_fp16_en;
    hi_bool has_left_idx;
    hi_bool has_right_idx;
    hi_bool is_instant;
} hi_svp_mau_ctrl;

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif
#endif /* _HI_COMMON_MAU_H_ */
