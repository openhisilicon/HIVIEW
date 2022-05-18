/*
 * Copyright (c) Hisilicon Technologies Co., Ltd. 2019-2019. All rights reserved.
 * Description: NNIE common header file
 * Author: Hisilicon multimedia software (SVP) group
 * Create: 2019-05-05
 */

#ifndef _HI_COMMON_NNIE_H_
#define _HI_COMMON_NNIE_H_

#include "hi_common_svp.h"
#include "hi_common.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/* SVP NNIE Error Code */
typedef enum {
    HI_SVP_NNIE_ERR_SYS_TIMEOUT = 0x40,   /* SVP_NNIE process timeout */
    HI_SVP_NNIE_ERR_QUERY_TIMEOUT = 0x41, /* SVP_NNIE query timeout */
    HI_SVP_NNIE_ERR_CFG_ERR = 0x42,       /* SVP_NNIE Configure error */
    HI_SVP_NNIE_ERR_BUS_ERR = 0x43,       /* SVP_NNIE Configure error */
    HI_SVP_NNIE_ERR_OPEN_FILE = 0x44,     /* SVP NNIE open file error */
    HI_SVP_NNIE_ERR_READ_FILE = 0x45,     /* SVP NNIE read file error */

    HI_SVP_NNIE_ERR_BUTT
} hi_svp_nnie_err_code;

/* invlalid device ID */
#define HI_ERR_SVP_NNIE_INVALID_DEV_ID     HI_DEFINE_ERR(HI_ID_SVP_NNIE, HI_ERR_LEVEL_ERROR, HI_ERR_INVALID_DEV_ID)
/* invlalid pipe ID */
#define HI_ERR_SVP_NNIE_INVALID_PIPE_ID    HI_DEFINE_ERR(HI_ID_SVP_NNIE, HI_ERR_LEVEL_ERROR, HI_ERR_INVALID_PIPE_ID)
/* invlalid channel ID */
#define HI_ERR_SVP_NNIE_INVALID_CHN_ID     HI_DEFINE_ERR(HI_ID_SVP_NNIE, HI_ERR_LEVEL_ERROR, HI_ERR_INVALID_CHN_ID)
#define HI_ERR_SVP_NNIE_INVALID_LAYER_ID   HI_DEFINE_ERR(HI_ID_SVP_NNIE, HI_ERR_LEVEL_ERROR, HI_ERR_INVALID_LAYER_ID)

/* at lease one parameter is illagal */
#define HI_ERR_SVP_NNIE_ILLEGAL_PARAM      HI_DEFINE_ERR(HI_ID_SVP_NNIE, HI_ERR_LEVEL_ERROR, HI_ERR_ILLEGAL_PARAM)
/* resource exists */
#define HI_ERR_SVP_NNIE_EXIST              HI_DEFINE_ERR(HI_ID_SVP_NNIE, HI_ERR_LEVEL_ERROR, HI_ERR_EXIST)
/* resource unexists */
#define HI_ERR_SVP_NNIE_UNEXIST            HI_DEFINE_ERR(HI_ID_SVP_NNIE, HI_ERR_LEVEL_ERROR, HI_ERR_UNEXIST)
/* using a HI_NULL pointer */
#define HI_ERR_SVP_NNIE_NULL_PTR           HI_DEFINE_ERR(HI_ID_SVP_NNIE, HI_ERR_LEVEL_ERROR, HI_ERR_NULL_PTR)
/* try to enable or initialize the system, device, or channel before configuring attributes. */
#define HI_ERR_SVP_NNIE_NOT_CFG            HI_DEFINE_ERR(HI_ID_SVP_NNIE, HI_ERR_LEVEL_ERROR, HI_ERR_NOT_CFG)
/* operation or type is not supported by NOW */
#define HI_ERR_SVP_NNIE_NOT_SUPPORT        HI_DEFINE_ERR(HI_ID_SVP_NNIE, HI_ERR_LEVEL_ERROR, HI_ERR_NOT_SUPPORT)
/* operation is not permitted. */
#define HI_ERR_SVP_NNIE_NOT_PERM           HI_DEFINE_ERR(HI_ID_SVP_NNIE, HI_ERR_LEVEL_ERROR, HI_ERR_NOT_PERM)

#define HI_ERR_SVP_NNIE_NOT_ENABLE         HI_DEFINE_ERR(HI_ID_SVP_NNIE, HI_ERR_LEVEL_ERROR, HI_ERR_NOT_ENABLE)
#define HI_ERR_SVP_NNIE_NOT_DISABLE        HI_DEFINE_ERR(HI_ID_SVP_NNIE, HI_ERR_LEVEL_ERROR, HI_ERR_NOT_DISABLE)
#define HI_ERR_SVP_NNIE_NOT_START          HI_DEFINE_ERR(HI_ID_SVP_NNIE, HI_ERR_LEVEL_ERROR, HI_ERR_NOT_START)
#define HI_ERR_SVP_NNIE_NOT_STOP           HI_DEFINE_ERR(HI_ID_SVP_NNIE, HI_ERR_LEVEL_ERROR, HI_ERR_NOT_STOP)
/* failure caused by malloc memory */
#define HI_ERR_SVP_NNIE_NO_MEM             HI_DEFINE_ERR(HI_ID_SVP_NNIE, HI_ERR_LEVEL_ERROR, HI_ERR_NO_MEM)
/* failure caused by malloc buffer */
#define HI_ERR_SVP_NNIE_NO_BUF             HI_DEFINE_ERR(HI_ID_SVP_NNIE, HI_ERR_LEVEL_ERROR, HI_ERR_NO_BUF)
/* no data in buffer */
#define HI_ERR_SVP_NNIE_BUF_EMPTY          HI_DEFINE_ERR(HI_ID_SVP_NNIE, HI_ERR_LEVEL_ERROR, HI_ERR_BUF_EMPTY)
/* no buffer for new data */
#define HI_ERR_SVP_NNIE_BUF_FULL           HI_DEFINE_ERR(HI_ID_SVP_NNIE, HI_ERR_LEVEL_ERROR, HI_ERR_BUF_FULL)
/* The system is not ready because it may be not initialized or loaded. */
/* The error code is returned when a device file fails to be opened. */
#define HI_ERR_SVP_NNIE_NOT_READY          HI_DEFINE_ERR(HI_ID_SVP_NNIE, HI_ERR_LEVEL_ERROR, HI_ERR_NOT_READY)

#define HI_ERR_SVP_NNIE_TIMEOUT            HI_DEFINE_ERR(HI_ID_SVP_NNIE, HI_ERR_LEVEL_ERROR, HI_ERR_TIMEOUT)
/* bad address eg. used for copy_from_user & copy_to_user */
#define HI_ERR_SVP_NNIE_BAD_ADDR           HI_DEFINE_ERR(HI_ID_SVP_NNIE, HI_ERR_LEVEL_ERROR, HI_ERR_BAD_ADDR)
/* resource is busy */
#define HI_ERR_SVP_NNIE_BUSY               HI_DEFINE_ERR(HI_ID_SVP_NNIE, HI_ERR_LEVEL_ERROR, HI_ERR_BUSY)
/* buffer size is smaller than the actual size required */
#define HI_ERR_SVP_NNIE_SIZE_NOT_ENOUGH    HI_DEFINE_ERR(HI_ID_SVP_NNIE, HI_ERR_LEVEL_ERROR, HI_ERR_SIZE_NOT_ENOUGH)
#define HI_ERR_SVP_NNIE_NOT_BINDED         HI_DEFINE_ERR(HI_ID_SVP_NNIE, HI_ERR_LEVEL_ERROR, HI_ERR_NOT_BINDED)
#define HI_ERR_SVP_NNIE_BINDED             HI_DEFINE_ERR(HI_ID_SVP_NNIE, HI_ERR_LEVEL_ERROR, HI_ERR_BINDED)

/* SVP process timeout */
#define HI_ERR_SVP_NNIE_SYS_TIMEOUT    HI_DEFINE_ERR(HI_ID_SVP_NNIE, HI_ERR_LEVEL_ERROR, HI_SVP_NNIE_ERR_SYS_TIMEOUT)
/* SVP query timeout */
#define HI_ERR_SVP_NNIE_QUERY_TIMEOUT  HI_DEFINE_ERR(HI_ID_SVP_NNIE, HI_ERR_LEVEL_ERROR, HI_SVP_NNIE_ERR_QUERY_TIMEOUT)
/* SVP configure error */
#define HI_ERR_SVP_NNIE_CFG_ERR        HI_DEFINE_ERR(HI_ID_SVP_NNIE, HI_ERR_LEVEL_ERROR, HI_SVP_NNIE_ERR_CFG_ERR)
#define HI_ERR_SVP_NNIE_BUS_ERR        HI_DEFINE_ERR(HI_ID_SVP_NNIE, HI_ERR_LEVEL_ERROR, HI_SVP_NNIE_ERR_BUS_ERR)

#define HI_ERR_SVP_NNIE_OPEN_FILE      HI_DEFINE_ERR(HI_ID_SVP_NNIE, HI_ERR_LEVEL_ERROR, HI_SVP_NNIE_ERR_OPEN_FILE)
#define HI_ERR_SVP_NNIE_READ_FILE      HI_DEFINE_ERR(HI_ID_SVP_NNIE, HI_ERR_LEVEL_ERROR, HI_SVP_NNIE_ERR_READ_FILE)

/* macro */
#define HI_SVP_NNIE_MAX_NET_SEG_NUM           8 /* NNIE max segment num that the net being cut into */
#define HI_SVP_NNIE_MAX_IN_NUM               16 /* NNIE max input num in each seg */
#define HI_SVP_NNIE_MAX_OUT_NUM              16 /* NNIE max output num in each seg */
#define HI_SVP_NNIE_MAX_ROI_LAYER_NUM_OF_SEG  2 /* NNIE max roi layer num in each seg */
#define HI_SVP_NNIE_MAX_ROI_LAYER_NUM         4 /* NNIE max roi layer num */
#define HI_SVP_NNIE_NODE_NAME_LEN            32 /* NNIE node name length */
#define HI_SVP_NNIE_MAX_THRESHOLD_NUM         2 /* NNIE max threshold num */

/* NNIE core id */
typedef enum {
    HI_SVP_NNIE_ID_0 = 0x0,

    HI_SVP_NNIE_ID_BUTT
} hi_svp_nnie_id;

/* Run Mode */
typedef enum {
    HI_SVP_NNIE_RUN_MODE_CHIP = 0x0,     /* on SOC chip running */
    HI_SVP_NNIE_RUN_MODE_FUNC_SIM = 0x1, /* functional simulation */

    HI_SVP_NNIE_RUN_MODE_BUTT
} hi_svp_nnie_run_mode;

/* Network type */
typedef enum {
    HI_SVP_NNIE_NET_TYPE_CNN = 0x0,       /* Non-ROI input cnn net */
    HI_SVP_NNIE_NET_TYPE_ROI = 0x1,       /* With ROI input cnn net */
    HI_SVP_NNIE_NET_TYPE_RECURRENT = 0x2, /* RNN or LSTM net */

    HI_SVP_NNIE_NET_TYPE_BUTT
} hi_svp_nnie_net_type;

/* Roi/Psroi Pooling type */
typedef enum {
    HI_SVP_NNIE_ROI_POOL_TYPE_NORM = 0x0, /* roipooling */
    HI_SVP_NNIE_ROI_POOL_TYPE_PS = 0x1,   /* Position-Sensitive roipooling */

    HI_SVP_NNIE_ROI_POOL_TYPE_BUTT
} hi_svp_nnie_roi_pool_type;

/* Proposal flag mask */
typedef enum {
    HI_SVP_NNIE_PROPOSAL_FLAG_MASK_DECODE_BBOX = 0x1,
    HI_SVP_NNIE_PROPOSAL_FLAG_MASK_NMS = 0x2,
    HI_SVP_NNIE_PROPOSAL_FLAG_MASK_FILTER = 0x4,
    HI_SVP_NNIE_PROPOSAL_FLAG_MASK_SORT = 0x8,
    HI_SVP_NNIE_PROPOSAL_FLAG_MASK_PRIOR_BOX = 0x10,
    HI_SVP_NNIE_PROPOSAL_FLAG_MASK_VAR = 0x20,
    HI_SVP_NNIE_PROPOSAL_FLAG_MASK_CLASS_BBOX_NUM = 0x40,

    HI_SVP_NNIE_PROPOSAL_FLAG_MASK_BUTT
} hi_svp_nnie_proposal_flag_mask;

/* Node information */
typedef struct {
    hi_svp_blob_type type;
    union {
        struct {
            hi_u32 width;
            hi_u32 height;
            hi_u32 chn;
        } whc;        /* AUTO:hi_svp_blob_type:other; */
        hi_u32 dim; /* AUTO:hi_svp_blob_type:HI_SVP_BLOB_TYPE_SEQ_S20Q12; */
    } shape;
    hi_u32 node_id;
    hi_char name[HI_SVP_NNIE_NODE_NAME_LEN]; /* Report layer bottom name or data layer bottom name */
} hi_svp_nnie_node;

/* RoiPooling information */
typedef struct {
    hi_u32 src_height;                       /* ROI/PSROI Pooling src height */
    hi_u32 src_width;                        /* ROI/PSROI Pooling src width */
    hi_u32 src_map_num;                      /* Input feature map channel */

    hi_u32 dst_height;                       /* ROI/PSROI Pooling dst height */
    hi_u32 dst_width;                        /* ROI/PSROI Pooling dst width */
    hi_u32 dst_map_num;                      /* Output feature map channel */

    hi_u32 norm_width_ram_offset;
    hi_u32 norm_height_ram_offset;
    hi_u32 total_width_ram_offset;
    hi_u32 total_height_ram_offset;

    hi_u32 blk_max_height;                   /* Block max height */
    hi_u32 blk_num;                          /* Block number */
    hi_u32 bbox_max_num;                     /* Bbox max number */
    hi_u32 spatial_scale;

    hi_bool norm_ping_pong_mode_en;          /* whether use PingPong mode */
    hi_bool total_ping_pong_mode_en;         /* whether use PingPong mode */
    hi_bool high_precision_en;               /* whether use high precision mode */
    hi_svp_nnie_roi_pool_type roi_pool_type; /* ROIPooling or PSROIPooling */
    hi_char name[HI_SVP_NNIE_NODE_NAME_LEN]; /* Report layer bottom name or data layer bottom name */
} hi_svp_nnie_roi_pool_info;

/* Segment information */
typedef struct {
    hi_svp_nnie_net_type net_type;
    hi_u16 src_num;
    hi_u16 dst_num;
    hi_u16 roi_pool_num;
    hi_u16 max_step;

    hi_u32 instr_offset;
    hi_u32 instr_len;

    hi_u32 proposal_flag;
    hi_u32 prior_box_num;
    hi_u32 var_num;

    hi_svp_nnie_node src_node[HI_SVP_NNIE_MAX_IN_NUM];
    hi_svp_nnie_node dst_node[HI_SVP_NNIE_MAX_OUT_NUM];
    hi_u32 roi_idx[HI_SVP_NNIE_MAX_ROI_LAYER_NUM_OF_SEG]; /* Roipooling info index */
} hi_svp_nnie_seg;

/* NNIE model */
typedef struct {
    hi_svp_nnie_run_mode run_mode;

    hi_u32 model_version; /* model version */
    hi_u32 arch_version;  /* arch version */

    hi_u32 tmp_buf_size;  /* temp buffer size */
    hi_u32 net_seg_num;
    hi_svp_nnie_seg seg[HI_SVP_NNIE_MAX_NET_SEG_NUM];
    hi_svp_nnie_roi_pool_info roi_info[HI_SVP_NNIE_MAX_ROI_LAYER_NUM];

    hi_svp_mem_info base;
    hi_svp_blob prior_box[HI_SVP_NNIE_MAX_NET_SEG_NUM];
} hi_svp_nnie_model;

/* Thresh */
typedef struct {
    hi_u32 min_width;       /* min width */
    hi_u32 min_height;      /* min height */
    hi_u32 nms_threshold;   /* nms threshold */
    hi_u32 score_threshold; /* score threshold */
} hi_svp_nnie_threshold;

/* Forward ctrl */
typedef struct {
    hi_u32 src_num;                                                 /* input node num, [1, 16] */
    hi_u32 dst_num;                                                 /* output node num, [1, 16] */
    hi_u32 threshold_num;                                           /* thresh num, [0, 2] */
    hi_u32 net_seg_id;                                              /* net segment index running on NNIE */
    hi_svp_nnie_id nnie_id;                                         /* device target which running the seg */
    hi_svp_nnie_threshold threshold[HI_SVP_NNIE_MAX_THRESHOLD_NUM]; /* threshold */
    hi_svp_mem_info tmp_buf;                                        /* auxiliary temp mem */
    hi_svp_mem_info task_buf;                                       /* auxiliary task mem */
} hi_svp_nnie_forward_ctrl;

/* Task buf info */
typedef struct {
    hi_u32 task_buf_size; /* task buf size */
    hi_u32 prior_box_num; /* priorbox num */
    hi_u32 var_num;       /* variance num */
} hi_svp_nnie_task_buf_info;

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif
#endif /* _HI_COMMON_NNIE_H_ */
