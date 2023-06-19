/*
  Copyright (c), 2001-2022, Shenshu Tech. Co., Ltd.
 */

#ifndef __HI_COMMON_PCIV_H__
#define __HI_COMMON_PCIV_H__

#include "hi_type.h"
#include "hi_common.h"
#include "hi_errno.h"
#include "hi_common_video.h"
#include "ot_common_pciv.h"


#ifdef __cplusplus
extern "C" {
#endif

#define HI_PCIV_MAX_BUF_NUM OT_PCIV_MAX_BUF_NUM
#define HI_PCIV_MAX_CHIP_NUM OT_PCIV_MAX_CHIP_NUM
#define HI_PCIV_TIMER_EXPIRES OT_PCIV_TIMER_EXPIRES
#define HI_PCIV_MAX_DMA_BLK OT_PCIV_MAX_DMA_BLK
#define HI_PCIV_MAX_VB_CNT OT_PCIV_MAX_VB_CNT
typedef ot_pciv_enum_chip hi_pciv_enum_chip;
typedef ot_pciv_remote_obj hi_pciv_remote_obj;
typedef ot_pciv_pic_attr hi_pciv_pic_attr;
typedef ot_pciv_attr hi_pciv_attr;
typedef ot_pciv_window_vb_cfg hi_pciv_window_vb_cfg;
typedef ot_pciv_window_base hi_pciv_window_base;
typedef ot_pciv_dma_blk hi_pciv_dma_blk;
typedef ot_pciv_dma_task hi_pciv_dma_task;
#define HI_ERR_PCIV_INVALID_CHN_ID OT_ERR_PCIV_INVALID_CHN_ID
#define HI_ERR_PCIV_ILLEGAL_PARAM OT_ERR_PCIV_ILLEGAL_PARAM
#define HI_ERR_PCIV_EXIST OT_ERR_PCIV_EXIST
#define HI_ERR_PCIV_UNEXIST OT_ERR_PCIV_UNEXIST
#define HI_ERR_PCIV_NULL_PTR OT_ERR_PCIV_NULL_PTR
#define HI_ERR_PCIV_NOT_CFG OT_ERR_PCIV_NOT_CFG
#define HI_ERR_PCIV_NOT_SUPPORT OT_ERR_PCIV_NOT_SUPPORT
#define HI_ERR_PCIV_NOT_PERM OT_ERR_PCIV_NOT_PERM
#define HI_ERR_PCIV_NO_MEM OT_ERR_PCIV_NO_MEM
#define HI_ERR_PCIV_NO_BUF OT_ERR_PCIV_NO_BUF
#define HI_ERR_PCIV_BUF_EMPTY OT_ERR_PCIV_BUF_EMPTY
#define HI_ERR_PCIV_BUF_FULL OT_ERR_PCIV_BUF_FULL
#define HI_ERR_PCIV_NOT_READY OT_ERR_PCIV_NOT_READY
#define HI_ERR_PCIV_BUSY OT_ERR_PCIV_BUSY
#define HI_ERR_PCIV_TIMEOUT OT_ERR_PCIV_TIMEOUT

#ifdef __cplusplus
}
#endif
#endif /* __HI_COMMON_PCIV_H__ */
