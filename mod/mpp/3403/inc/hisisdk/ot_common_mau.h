/*
  Copyright (c), 2001-2022, Shenshu Tech. Co., Ltd.
 */

#ifndef OT_COMMON_MAU_H
#define OT_COMMON_MAU_H

#include "ot_common_svp.h"
#include "ot_common.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/* SVP_MAU Error Code */
typedef enum {
    OT_SVP_MAU_ERR_SYS_TIMEOUT = 0x40, /* SVP MAU process timeout */
    OT_SVP_MAU_ERR_QUERY_TIMEOUT = 0x41, /* SVP MAU query timeout */
    OT_SVP_MAU_ERR_CFG_ERR = 0x42, /* SVP MAU Configure error */
    OT_SVP_MAU_ERR_BUS_ERR = 0x43, /* SVP  MAU Configure error */
    OT_SVP_MAU_ERR_OPEN_FILE = 0x44, /* SVP MAU open file error */
    OT_SVP_MAU_ERR_READ_FILE = 0x45, /* SVP MAU read file error */

    OT_SVP_MAU_ERR_BUTT
} ot_svp_mau_err_code;
/* Invalid device ID */
#define OT_ERR_SVP_MAU_INVALID_DEV_ID     OT_DEFINE_ERR(OT_ID_SVP_MAU, OT_ERR_LEVEL_ERROR, OT_ERR_INVALID_DEV_ID)
/* Invalid channel ID */
#define OT_ERR_SVP_MAU_INVALID_CHN_ID     OT_DEFINE_ERR(OT_ID_SVP_MAU, OT_ERR_LEVEL_ERROR, OT_ERR_INVALID_CHN_ID)
/* At least one parameter is illegal. For example, an illegal enumeration value exists. */
#define OT_ERR_SVP_MAU_ILLEGAL_PARAM     OT_DEFINE_ERR(OT_ID_SVP_MAU, OT_ERR_LEVEL_ERROR, OT_ERR_ILLEGAL_PARAM)
/* The channel exists. */
#define OT_ERR_SVP_MAU_EXIST             OT_DEFINE_ERR(OT_ID_SVP_MAU, OT_ERR_LEVEL_ERROR, OT_ERR_EXIST)
/* The UN exists. */
#define OT_ERR_SVP_MAU_UNEXIST           OT_DEFINE_ERR(OT_ID_SVP_MAU, OT_ERR_LEVEL_ERROR, OT_ERR_UNEXIST)
/* A null point is used. */
#define OT_ERR_SVP_MAU_NULL_PTR          OT_DEFINE_ERR(OT_ID_SVP_MAU, OT_ERR_LEVEL_ERROR, OT_ERR_NULL_PTR)
/* Try to enable or initialize the system, device, or channel before configuring attributes. */
#define OT_ERR_SVP_MAU_NOT_CFG        OT_DEFINE_ERR(OT_ID_SVP_MAU, OT_ERR_LEVEL_ERROR, OT_ERR_NOT_CFG)
/* The operation is not supported currently. */
#define OT_ERR_SVP_MAU_NOT_SUPPORT      OT_DEFINE_ERR(OT_ID_SVP_MAU, OT_ERR_LEVEL_ERROR, OT_ERR_NOT_SUPPORT)
/* The operation, changing static attributes for example, is not permitted. */
#define OT_ERR_SVP_MAU_NOT_PERM          OT_DEFINE_ERR(OT_ID_SVP_MAU, OT_ERR_LEVEL_ERROR, OT_ERR_NOT_PERM)
/* A failure caused by the malloc memory occurs. */
#define OT_ERR_SVP_MAU_NO_MEM             OT_DEFINE_ERR(OT_ID_SVP_MAU, OT_ERR_LEVEL_ERROR, OT_ERR_NO_MEM)
/* A failure caused by the malloc buffer occurs. */
#define OT_ERR_SVP_MAU_NO_BUF             OT_DEFINE_ERR(OT_ID_SVP_MAU, OT_ERR_LEVEL_ERROR, OT_ERR_NO_BUF)
/* The buffer is empty. */
#define OT_ERR_SVP_MAU_BUF_EMPTY         OT_DEFINE_ERR(OT_ID_SVP_MAU, OT_ERR_LEVEL_ERROR, OT_ERR_BUF_EMPTY)
/* No buffer is provided for storing new data. */
#define OT_ERR_SVP_MAU_BUF_FULL          OT_DEFINE_ERR(OT_ID_SVP_MAU, OT_ERR_LEVEL_ERROR, OT_ERR_BUF_FULL)
/*
 * The system is not ready because it may be not initialized or loaded.
 * The error code is returned when a device file fails to be opened.
 */
#define OT_ERR_SVP_MAU_NOT_READY         OT_DEFINE_ERR(OT_ID_SVP_MAU, OT_ERR_LEVEL_ERROR, OT_ERR_NOT_READY)
/*
 * The source address or target address is incorrect during the operations such as
 * calling copy_from_user or copy_to_user.
 */
#define OT_ERR_SVP_MAU_BAD_ADDR          OT_DEFINE_ERR(OT_ID_SVP_MAU, OT_ERR_LEVEL_ERROR, OT_ERR_BAD_ADDR)
/* The resource is busy during the operations. */
#define OT_ERR_SVP_MAU_BUSY              OT_DEFINE_ERR(OT_ID_SVP_MAU, OT_ERR_LEVEL_ERROR, OT_ERR_BUSY)
/* SVP_MAU process timeout */
#define OT_ERR_SVP_MAU_SYS_TIMEOUT       OT_DEFINE_ERR(OT_ID_SVP_MAU, OT_ERR_LEVEL_ERROR, OT_SVP_MAU_ERR_SYS_TIMEOUT)
/* SVP_MAU query timeout */
#define OT_ERR_SVP_MAU_QUERY_TIMEOUT     OT_DEFINE_ERR(OT_ID_SVP_MAU, OT_ERR_LEVEL_ERROR, OT_SVP_MAU_ERR_QUERY_TIMEOUT)
/* SVP_MAU configure error */
#define OT_ERR_SVP_MAU_CFG_ERR           OT_DEFINE_ERR(OT_ID_SVP_MAU, OT_ERR_LEVEL_ERROR, OT_SVP_MAU_ERR_CFG_ERR)

#define OT_ERR_SVP_MAU_BUS_ERR            OT_DEFINE_ERR(OT_ID_SVP, OT_ERR_LEVEL_ERROR, OT_SVP_MAU_ERR_BUS_ERR)

#define OT_ERR_SVP_MAU_OPEN_FILE         OT_DEFINE_ERR(OT_ID_SVP_MAU, OT_ERR_LEVEL_ERROR, OT_SVP_MAU_ERR_OPEN_FILE)
#define OT_ERR_SVP_MAU_READ_FILE         OT_DEFINE_ERR(OT_ID_SVP_MAU, OT_ERR_LEVEL_ERROR, OT_SVP_MAU_ERR_READ_FILE)

typedef struct {
    ot_svp_blob left_matrix;
    ot_svp_blob right_matrix;
} ot_svp_mau_double_matrix;

typedef struct {
    ot_svp_blob op_result;
    ot_svp_blob top_n;
    ot_svp_blob top_n_idx;
} ot_svp_mau_dist_result;

typedef ot_svp_mau_double_matrix ot_svp_mau_src_double_matrix;

typedef enum {
    OT_SVP_MAU_OUT_OP_RESULT = 0x0,
    OT_SVP_MAU_OUT_TOP_N     = 0x1,
    OT_SVP_MAU_OUT_BOTH      = 0x2,
    OT_SVP_MAU_OUT_BUTT
} ot_svp_mau_out_type;

typedef enum {
    OT_SVP_MAU_ID_0 = 0x0,
    OT_SVP_MAU_ID_BUTT
} ot_svp_mau_id;

typedef struct {
    ot_svp_mau_id mau_id;
    ot_svp_mau_out_type out_type;
    td_bool fp32_to_fp16_en;
    td_bool has_left_idx;
    td_bool has_right_idx;
    td_bool is_instant;
} ot_svp_mau_ctrl;

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif
#endif /* OT_COMMON_MAU_H */
