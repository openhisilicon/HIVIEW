/*
  Copyright (c), 2001-2022, Shenshu Tech. Co., Ltd.
 */

#ifndef SAMPLE_IVE_QUEUE_H
#define SAMPLE_IVE_QUEUE_H

#include "hi_type.h"
#include "hi_common_video.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

typedef struct tag_hi_sample_ive_node {
    hi_video_frame_info frame_info;
    struct tag_hi_sample_ive_node *next;
} hi_sample_ive_node;

typedef struct {
    hi_sample_ive_node *front, *rear;
} hi_sample_ive_queue;

#define QUEUE_CORE_ERROR_BASE           1
#define QUEUE_CORE_FRAMEWORK_ERROR_BASE (QUEUE_CORE_ERROR_BASE + 10000)

#define QUEUE_NULL_POINTER  (QUEUE_CORE_FRAMEWORK_ERROR_BASE + 1)
#define QUEUE_ILLEGAL_STATE (QUEUE_CORE_FRAMEWORK_ERROR_BASE + 2)
#define QUEUE_OUT_OF_MEMORY (QUEUE_CORE_FRAMEWORK_ERROR_BASE + 3)

hi_sample_ive_queue *sample_ive_create_queue(hi_s32 len);
hi_void sample_ive_destory_queue(hi_sample_ive_queue **queue_head);
hi_void sample_ive_clear_queue(hi_sample_ive_queue *queue_head);
hi_bool sample_ive_is_queue_empty(hi_sample_ive_queue *queue_head);
hi_s32 sample_ive_queue_size(hi_sample_ive_queue *queue_head);
hi_s32 sample_ive_add_queue_node(hi_sample_ive_queue *queue_head, hi_video_frame_info *added_frm_info);
hi_sample_ive_node *sample_ive_get_queue_head_node(const hi_sample_ive_queue *queue_head);
hi_sample_ive_node *sample_ive_get_queue_node(hi_sample_ive_queue *queue_head);
hi_void sample_ive_free_queue_node(hi_sample_ive_node **free_node);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* SAMPLE_IVE_QUEUE_H */

