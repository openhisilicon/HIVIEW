/*
  Copyright (c), 2001-2022, Shenshu Tech. Co., Ltd.
 */

#ifndef OT_COMMON_PCIV_H
#define OT_COMMON_PCIV_H

#include "ot_type.h"
#include "ot_common.h"
#include "ot_errno.h"
#include "ot_common_video.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

#define OT_PCIV_MAX_BUF_NUM    16  /* Max pciv channel buffer number */
#define OT_PCIV_MAX_CHIP_NUM   32  /* Max pciv chip number which can join in the system */
#define OT_PCIV_TIMER_EXPIRES  10  /* Expires of clock interrupt */
#define OT_PCIV_MAX_DMA_BLK    128 /* Max block number of dma task */
#define OT_PCIV_MAX_VB_CNT     8   /* Max count of video buffer block for pci window */

typedef struct {
    td_u32 chip_num;                        /* R, remote chip number */
    td_s32 chip_id[OT_PCIV_MAX_CHIP_NUM];   /* R, remote chip id */
} ot_pciv_enum_chip;

/* remote pciv object */
typedef struct {
    td_s32      chip_id;  /* RW, Range:host:[1, OT_PCIV_MAX_CHIP_NUM), slave:0;Remote pciv chip id */
    ot_pciv_chn pciv_chn; /* RW, Range:[0, OT_PCIV_MAX_CHN_NUM - 1]; Pciv channel id of remote pciv chip */
} ot_pciv_remote_obj;

/* attribution of target picture  */
typedef struct {
    td_u32             width;          /* RW: Picture width of pciv channel */
    td_u32             height;         /* RW: Picture height of pciv channel */
    td_u32             stride[OT_MAX_COLOR_COMPONENT]; /* RW: Picture stride of pciv channel */
    ot_video_field     field;          /* RW; Video frame field type of pciv channel */
    ot_pixel_format    pixel_format;   /* RW; Pixel format of pciv channel */
    ot_dynamic_range   dynamic_range;  /* RW; Dynamic range of pciv channel */
    ot_compress_mode   compress_mode;  /* RW; Compress mode of pciv channel */
    ot_video_format    video_format;   /* RW; Video format of pciv channel */
} ot_pciv_pic_attr;

/* attribution of pciv chn */
typedef struct {
    ot_pciv_pic_attr    pic_attr;                       /* RW; Picture attributes */
    td_u32              blk_size;                       /* RW; Video buffer size of receiver for preview */
    td_u32              blk_cnt;                        /* RW, Range:[1, OT_PCIV_MAX_BUF_NUM]; Number of video buffer */
    td_phys_addr_t      phys_addr[OT_PCIV_MAX_BUF_NUM]; /* RW; Physical address list for video buffer */
    ot_pciv_remote_obj  remote_obj;                     /* RW; remote pciv object */
} ot_pciv_attr;

/* mpp video buffer config for pci window */
typedef struct {
    td_u32 pool_cnt;                      /* W, Range:[1, OT_PCIV_MAX_VB_CNT]; Total number of video buffer pool  */
    td_u32 blk_size[OT_PCIV_MAX_VB_CNT];  /* W, size of video buffer pool */
    td_u32 blk_cnt[OT_PCIV_MAX_VB_CNT];   /* W, number of video buffer pool */
} ot_pciv_window_vb_cfg;

typedef struct {
    td_s32         chip_id;      /* WW; Range:[0, OT_PCIV_MAX_CHIP_NUM) Pciv chip id */
    td_phys_addr_t np_addr;      /* R; Non-prefetch window pcie base address */
    td_phys_addr_t pf_addr;      /* R; Prefetch window pcie base address */
    td_phys_addr_t cfg_addr;     /* R; Config window pcie base address */
    td_phys_addr_t pf_ahb_addr;  /* R; Prefetch window AHB base address */
} ot_pciv_window_base;

typedef struct {
    td_phys_addr_t src_addr; /* W, Source address of dma task */
    td_phys_addr_t dst_addr; /* W, Destination address of dma task */
    td_u32         size;     /* W, Data block size of dma task */
} ot_pciv_dma_blk;

typedef struct {
    td_u32                    blk_cnt;   /* W, Range:[1, OT_PCIV_MAX_DMA_BLK]; Total dma task number */
    td_bool                   is_read;   /* W, Range:[0, 1]; Dam task is read or write mode */
    ot_pciv_dma_blk ATTRIBUTE *dma_blk;
} ot_pciv_dma_task;

#define OT_ERR_PCIV_INVALID_CHN_ID OT_DEFINE_ERR(OT_ID_PCIV, OT_ERR_LEVEL_ERROR, OT_ERR_INVALID_CHN_ID)
#define OT_ERR_PCIV_ILLEGAL_PARAM  OT_DEFINE_ERR(OT_ID_PCIV, OT_ERR_LEVEL_ERROR, OT_ERR_ILLEGAL_PARAM)
#define OT_ERR_PCIV_EXIST          OT_DEFINE_ERR(OT_ID_PCIV, OT_ERR_LEVEL_ERROR, OT_ERR_EXIST)
#define OT_ERR_PCIV_UNEXIST        OT_DEFINE_ERR(OT_ID_PCIV, OT_ERR_LEVEL_ERROR, OT_ERR_UNEXIST)
#define OT_ERR_PCIV_NULL_PTR       OT_DEFINE_ERR(OT_ID_PCIV, OT_ERR_LEVEL_ERROR, OT_ERR_NULL_PTR)
#define OT_ERR_PCIV_NOT_CFG        OT_DEFINE_ERR(OT_ID_PCIV, OT_ERR_LEVEL_ERROR, OT_ERR_NOT_CFG)
#define OT_ERR_PCIV_NOT_SUPPORT    OT_DEFINE_ERR(OT_ID_PCIV, OT_ERR_LEVEL_ERROR, OT_ERR_NOT_SUPPORT)
#define OT_ERR_PCIV_NOT_PERM       OT_DEFINE_ERR(OT_ID_PCIV, OT_ERR_LEVEL_ERROR, OT_ERR_NOT_PERM)
#define OT_ERR_PCIV_NO_MEM         OT_DEFINE_ERR(OT_ID_PCIV, OT_ERR_LEVEL_ERROR, OT_ERR_NO_MEM)
#define OT_ERR_PCIV_NO_BUF         OT_DEFINE_ERR(OT_ID_PCIV, OT_ERR_LEVEL_ERROR, OT_ERR_NO_BUF)
#define OT_ERR_PCIV_BUF_EMPTY      OT_DEFINE_ERR(OT_ID_PCIV, OT_ERR_LEVEL_ERROR, OT_ERR_BUF_EMPTY)
#define OT_ERR_PCIV_BUF_FULL       OT_DEFINE_ERR(OT_ID_PCIV, OT_ERR_LEVEL_ERROR, OT_ERR_BUF_FULL)
#define OT_ERR_PCIV_NOT_READY      OT_DEFINE_ERR(OT_ID_PCIV, OT_ERR_LEVEL_ERROR, OT_ERR_NOT_READY)
#define OT_ERR_PCIV_BUSY           OT_DEFINE_ERR(OT_ID_PCIV, OT_ERR_LEVEL_ERROR, OT_ERR_BUSY)
#define OT_ERR_PCIV_TIMEOUT        OT_DEFINE_ERR(OT_ID_PCIV, OT_ERR_LEVEL_ERROR, OT_ERR_TIMEOUT)

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */
#endif /* OT_COMM_PCIV_H */

