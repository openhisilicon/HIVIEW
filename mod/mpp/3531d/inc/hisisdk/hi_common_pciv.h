/*
* Copyright (c) Hisilicon Technologies Co., Ltd. 2019-2020. All rights reserved.
* Description: Common definition of pciv
* Author: Hisilicon multimedia software group
* Create: 2019/07/01
*/

#ifndef __HI_COMMON_PCIV_H__
#define __HI_COMMON_PCIV_H__

#include "hi_type.h"
#include "hi_common.h"
#include "hi_errno.h"
#include "hi_common_video.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

#define HI_PCIV_MAX_BUF_NUM    16  /* Max pciv channel buffer number */
#define HI_PCIV_MAX_CHIP_NUM   32  /* Max pciv chip number which can join in the system */
#define HI_PCIV_TIMER_EXPIRES  10  /* Expires of clock interrupt */
#define HI_PCIV_MAX_DMA_BLK    128 /* Max block number of dma task */
#define HI_PCIV_MAX_VB_CNT     8   /* Max count of video buffer block for pci window */

typedef struct {
    hi_u32 chip_num;                        /* R, remote chip number */
    hi_s32 chip_id[HI_PCIV_MAX_CHIP_NUM];   /* R, remote chip id */
} hi_pciv_enum_chip;

/* remote pciv object */
typedef struct {
    hi_s32      chip_id;  /* RW, Range:host:[1, HI_PCIV_MAX_CHIP_NUM), slave:0;Remote pciv chip id */
    hi_pciv_chn pciv_chn; /* RW, Range:[0, HI_PCIV_MAX_CHN_NUM - 1]; Pciv channel id of remote pciv chip */
} hi_pciv_remote_obj;

/* attribution of target picture  */
typedef struct {
    hi_u32             width;          /* RW: Picture width of pciv channel */
    hi_u32             height;         /* RW: Picture height of pciv channel */
    hi_u32             stride[HI_MAX_COLOR_COMPONENT]; /* RW: Picture stride of pciv channel */
    hi_video_field     field;          /* RW; Video frame field type of pciv channel */
    hi_pixel_format    pixel_format;   /* RW; Pixel format of pciv channel */
    hi_dynamic_range   dynamic_range;  /* RW; Dynamic range of pciv channel */
    hi_compress_mode   compress_mode;  /* RW; Compress mode of pciv channel */
    hi_video_format    video_format;   /* RW; Video format of pciv channel */
} hi_pciv_pic_attr;

/* attribution of pciv chn */
typedef struct {
    hi_pciv_pic_attr    pic_attr;                       /* RW; Picture attribute */
    hi_u32              blk_size;                       /* RW; Video buffer size of receiver for preview */
    hi_u32              blk_cnt;                        /* RW, Range:[1, HI_PCIV_MAX_BUF_NUM]; Number of video buffer */
    hi_phys_addr_t      phys_addr[HI_PCIV_MAX_BUF_NUM]; /* RW; Physical address list for video buffer */
    hi_pciv_remote_obj  remote_obj;                     /* RW; remote pciv object */
} hi_pciv_attr;

/* mpp video buffer config for pci window */
typedef struct {
    hi_u32 pool_cnt;                      /* W, Range:[1, HI_PCIV_MAX_VB_CNT]; Total number of video buffer pool  */
    hi_u32 blk_size[HI_PCIV_MAX_VB_CNT];  /* W, size of video buffer pool */
    hi_u32 blk_cnt[HI_PCIV_MAX_VB_CNT];   /* W, number of video buffer pool */
} hi_pciv_window_vb_cfg;

typedef struct {
    hi_s32         chip_id;      /* WW; Range:[0, HI_PCIV_MAX_CHIP_NUM) Pciv chip id */
    hi_phys_addr_t np_addr;      /* R; Non-prefetch window pcie base address */
    hi_phys_addr_t pf_addr;      /* R; Prefetch window pcie base address */
    hi_phys_addr_t cfg_addr;     /* R; Config window pcie base address */
    hi_phys_addr_t pf_ahb_addr;  /* R; Prefetch window AHB base address */
} hi_pciv_window_base;

typedef struct {
    hi_phys_addr_t src_addr; /* W, Source address of dma task */
    hi_phys_addr_t dst_addr; /* W, Destination address of dma task */
    hi_u32         size;     /* W, Data block size of dma task */
} hi_pciv_dma_blk;

typedef struct {
    hi_u32                    blk_cnt;   /* W, Range:[1, HI_PCIV_MAX_DMA_BLK]; Total dma task number */
    hi_bool                   is_read;   /* W, Range:[0, 1]; Dam task is read or write mode */
    hi_pciv_dma_blk ATTRIBUTE *dma_blk;
} hi_pciv_dma_task;

#define HI_ERR_PCIV_INVALID_CHN_ID HI_DEFINE_ERR(HI_ID_PCIV, HI_ERR_LEVEL_ERROR, HI_ERR_INVALID_CHN_ID)
#define HI_ERR_PCIV_ILLEGAL_PARAM  HI_DEFINE_ERR(HI_ID_PCIV, HI_ERR_LEVEL_ERROR, HI_ERR_ILLEGAL_PARAM)
#define HI_ERR_PCIV_EXIST          HI_DEFINE_ERR(HI_ID_PCIV, HI_ERR_LEVEL_ERROR, HI_ERR_EXIST)
#define HI_ERR_PCIV_UNEXIST        HI_DEFINE_ERR(HI_ID_PCIV, HI_ERR_LEVEL_ERROR, HI_ERR_UNEXIST)
#define HI_ERR_PCIV_NULL_PTR       HI_DEFINE_ERR(HI_ID_PCIV, HI_ERR_LEVEL_ERROR, HI_ERR_NULL_PTR)
#define HI_ERR_PCIV_NOT_CFG        HI_DEFINE_ERR(HI_ID_PCIV, HI_ERR_LEVEL_ERROR, HI_ERR_NOT_CFG)
#define HI_ERR_PCIV_NOT_SUPPORT    HI_DEFINE_ERR(HI_ID_PCIV, HI_ERR_LEVEL_ERROR, HI_ERR_NOT_SUPPORT)
#define HI_ERR_PCIV_NOT_PERM       HI_DEFINE_ERR(HI_ID_PCIV, HI_ERR_LEVEL_ERROR, HI_ERR_NOT_PERM)
#define HI_ERR_PCIV_NO_MEM         HI_DEFINE_ERR(HI_ID_PCIV, HI_ERR_LEVEL_ERROR, HI_ERR_NO_MEM)
#define HI_ERR_PCIV_NO_BUF         HI_DEFINE_ERR(HI_ID_PCIV, HI_ERR_LEVEL_ERROR, HI_ERR_NO_BUF)
#define HI_ERR_PCIV_BUF_EMPTY      HI_DEFINE_ERR(HI_ID_PCIV, HI_ERR_LEVEL_ERROR, HI_ERR_BUF_EMPTY)
#define HI_ERR_PCIV_BUF_FULL       HI_DEFINE_ERR(HI_ID_PCIV, HI_ERR_LEVEL_ERROR, HI_ERR_BUF_FULL)
#define HI_ERR_PCIV_NOT_READY      HI_DEFINE_ERR(HI_ID_PCIV, HI_ERR_LEVEL_ERROR, HI_ERR_NOT_READY)
#define HI_ERR_PCIV_BUSY           HI_DEFINE_ERR(HI_ID_PCIV, HI_ERR_LEVEL_ERROR, HI_ERR_BUSY)
#define HI_ERR_PCIV_TIMEOUT        HI_DEFINE_ERR(HI_ID_PCIV, HI_ERR_LEVEL_ERROR, HI_ERR_TIMEOUT)

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */
#endif /* __HI_COMM_PCIV_H__ */

