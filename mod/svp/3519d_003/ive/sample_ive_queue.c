/*
  Copyright (c), 2001-2022, Shenshu Tech. Co., Ltd.
 */
#include "sample_ive_queue.h"
#include <malloc.h>
#include "sample_common_ive.h"

static hi_s32 g_max_queue_len = 0;
static hi_s32 g_cur_queue_len = 0;

hi_sample_ive_queue *sample_ive_create_queue(hi_s32 len)
{
    hi_sample_ive_queue *queue_head = HI_NULL;

    if ((len < -1) || (len == 0)) {
        return HI_NULL;
    }
    g_cur_queue_len = 0;
    queue_head = (hi_sample_ive_queue *)malloc(sizeof(hi_sample_ive_queue));
    sample_svp_check_exps_return(queue_head == HI_NULL, HI_NULL, SAMPLE_SVP_ERR_LEVEL_ERROR, "malloc failed!\n");

    queue_head->front = HI_NULL;
    queue_head->rear = HI_NULL;
    g_max_queue_len = len;

    return queue_head;
}

hi_void sample_ive_destory_queue(hi_sample_ive_queue **queue_head)
{
    hi_sample_ive_node *queue_node_tmp = HI_NULL;
    if (queue_head == HI_NULL || (*queue_head) == HI_NULL) {
        return;
    }

    queue_node_tmp = (*queue_head)->front;
    while (queue_node_tmp != HI_NULL) {
        (*queue_head)->front = queue_node_tmp->next;
        free(queue_node_tmp);
        queue_node_tmp = (*queue_head)->front;
    }
    (*queue_head)->rear = (*queue_head)->front;
    g_cur_queue_len = 0;
    free(*queue_head);
    (*queue_head) = HI_NULL;
}


hi_void sample_ive_clear_queue(hi_sample_ive_queue *queue_head)
{
    hi_sample_ive_node *queue_node_tmp = HI_NULL;

    if (queue_head == HI_NULL) {
        return;
    }

    queue_node_tmp = queue_head->front;
    while (queue_node_tmp != HI_NULL) {
        queue_head->front = queue_node_tmp->next;
        free(queue_node_tmp);
        queue_node_tmp = queue_head->front;
    }
    queue_head->rear = queue_head->front;
    g_cur_queue_len = 0;

    return;
}

hi_bool sample_ive_is_queue_empty(hi_sample_ive_queue *queue_head)
{
    if (queue_head == HI_NULL) {
        return HI_TRUE;
    }

    if (queue_head->front != HI_NULL) {
        return HI_FALSE;
    }

    return HI_TRUE;
}

hi_s32 sample_ive_queue_size(hi_sample_ive_queue *queue_head)
{
    if (queue_head == HI_NULL) {
        return 0;
    }

    return g_cur_queue_len;
}

hi_s32 sample_ive_add_queue_node(hi_sample_ive_queue *queue_head, hi_video_frame_info *added_frm_info)
{
    hi_sample_ive_node *queue_node = HI_NULL;
    hi_s32 ret;

    if ((queue_head == HI_NULL) || (added_frm_info == HI_NULL)) {
        return QUEUE_NULL_POINTER;
    }

    if ((g_max_queue_len != -1) && (g_cur_queue_len >= g_max_queue_len)) {
        return QUEUE_ILLEGAL_STATE;
    }

    queue_node = (hi_sample_ive_node *)malloc(sizeof(hi_sample_ive_node));
    sample_svp_check_exps_return(queue_node == HI_NULL, QUEUE_OUT_OF_MEMORY,
        SAMPLE_SVP_ERR_LEVEL_ERROR, "malloc failed, out of memory!\n");

    ret = memcpy_s(&queue_node->frame_info, sizeof(hi_video_frame_info),
        added_frm_info, sizeof(hi_video_frame_info));
    if (ret != EOK) {
        free(queue_node);
        sample_svp_check_exps_return(1, HI_ERR_IVE_ILLEGAL_PARAM,
            SAMPLE_SVP_ERR_LEVEL_ERROR, "memcpy_s node failed!\n");
    }
    queue_node->next = HI_NULL;
    if (sample_ive_is_queue_empty(queue_head) != 0) {
        queue_head->front = queue_node;
        queue_head->rear = queue_node;
    } else {
        queue_head->rear->next = queue_node;
        queue_head->rear = queue_node;
    }

    g_cur_queue_len++;

    return HI_SUCCESS;
}

hi_sample_ive_node *sample_ive_get_queue_head_node(const hi_sample_ive_queue *queue_node)
{
    if ((queue_node == HI_NULL) || (queue_node->front == HI_NULL)) {
        return HI_NULL;
    }

    return queue_node->front;
}

hi_sample_ive_node *sample_ive_get_queue_node(hi_sample_ive_queue *queue_head)
{
    hi_sample_ive_node *queue_node = HI_NULL;

    if ((queue_head == HI_NULL) || (queue_head->front == HI_NULL)) {
        return HI_NULL;
    }

    queue_node = queue_head->front;
    queue_head->front = queue_node->next;
    if (queue_head->front == HI_NULL) {
        queue_head->rear = queue_head->front;
    }
    g_cur_queue_len--;

    return queue_node;
}

hi_void sample_ive_free_queue_node(hi_sample_ive_node **free_node)
{
    if (free_node != HI_NULL && (*free_node) != HI_NULL) {
        free(*free_node);
        *free_node = HI_NULL;
    }

    return;
}
