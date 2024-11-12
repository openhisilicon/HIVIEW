/*
  Copyright (c), 2001-2024, Shenshu Tech. Co., Ltd.
 */

#ifndef __SAMPLE_IPC_H__
#define __SAMPLE_IPC_H__

#include "hi_type.h"

#define SAMPLE_MSG_TYPE_REQ_MAX_NUM 100
#define SAMPLE_MSG_TYPE_REQ_NO      1
#define SAMPLE_MSG_TYPE_RES_NO      (SAMPLE_MSG_TYPE_REQ_NO + SAMPLE_MSG_TYPE_REQ_MAX_NUM)

typedef struct {
    hi_s32 pid;
    /* other data fields ... */
} sample_ipc_client_req_data;

typedef struct {
    hi_bool is_req_success;
    /* other data fields ... */
} sample_ipc_client_res_data;

typedef struct {
    long msg_type;  /* message type, must be > 0 */
    sample_ipc_client_req_data msg_data;
} sample_ipc_msg_req_buf;

typedef struct {
    long msg_type;  /* message type, must be > 0 */
    sample_ipc_client_res_data msg_data;
} sample_ipc_msg_res_buf;

typedef enum {
    /* request type */
    SAMPLE_MSG_TYPE_SYS_MEM_SHARE_REQ = SAMPLE_MSG_TYPE_REQ_NO,
    SAMPLE_MSG_TYPE_SYS_MEM_UNSHARE_REQ,
    SAMPLE_MSG_TYPE_VB_POOL_SHARE_REQ,
    SAMPLE_MSG_TYPE_VB_POOL_UNSHARE_REQ,

    /* response type */
    SAMPLE_MSG_TYPE_SYS_MEM_SHARE_RES = SAMPLE_MSG_TYPE_RES_NO,
    SAMPLE_MSG_TYPE_SYS_MEM_UNSHARE_RES,
    SAMPLE_MSG_TYPE_VB_POOL_SHARE_RES,
    SAMPLE_MSG_TYPE_VB_POOL_UNSHARE_RES,

    SAMPLE_MSG_TYPE_BUTT
} sample_msg_type;

#ifdef __cplusplus
extern "C" {
#endif

/* sample IPC server API */
typedef hi_s32 (*sample_ipc_msg_proc)(const sample_ipc_msg_req_buf *msg_req_buf,
    hi_bool *is_need_fb, sample_ipc_msg_res_buf *msg_res_buf);
hi_s32 sample_ipc_server_init(sample_ipc_msg_proc msg_proc_func);
hi_void sample_ipc_server_deinit(hi_void);

/* sample IPC client API */
hi_s32 sample_ipc_client_init(hi_void);
hi_void sample_ipc_client_deinit(hi_void);
hi_s32 sample_ipc_client_send_data(const sample_ipc_msg_req_buf *msg_req_buf,
    hi_bool is_need_fb, sample_ipc_msg_res_buf *msg_res_buf);

#ifdef __cplusplus
}
#endif

#endif /* End of #ifndef __SAMPLE_IPC_H__ */
