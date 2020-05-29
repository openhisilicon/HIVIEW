#include <malloc.h>
#include "sample_comm_ive.h"
#include "sample_ive_queue.h"


static HI_S32 s_s32MaxQueuelen = 0;
static HI_S32 s_s32CurQueueLen = 0;

SAMPLE_IVE_QUEUE_S* SAMPLE_IVE_QueueCreate(HI_S32 s32Len)
{
	SAMPLE_IVE_QUEUE_S *pstQueueHead = NULL;

	if (s32Len < -1 || s32Len == 0)
	{
		return NULL;
	}
	s_s32CurQueueLen = 0;
	pstQueueHead = (SAMPLE_IVE_QUEUE_S*)malloc(sizeof(SAMPLE_IVE_QUEUE_S));
	if (NULL == pstQueueHead)
	{
		return NULL;
	}
	pstQueueHead->front = NULL;
	pstQueueHead->rear = NULL;
	s_s32MaxQueuelen = s32Len;

	return pstQueueHead;
}

HI_VOID SAMPLE_IVE_QueueDestory(SAMPLE_IVE_QUEUE_S* pstQueueHead)
{
	SAMPLE_IVE_NODE_S *pstQueueTmp = NULL;

	if (NULL == pstQueueHead)
	{
		return;
	}

	pstQueueTmp = pstQueueHead->front;
	while(NULL != pstQueueTmp)
	{
		pstQueueHead->front = pstQueueTmp->next;
		free(pstQueueTmp);
		pstQueueTmp = pstQueueHead->front;
	}
	pstQueueHead->rear = pstQueueHead->front;
	s_s32CurQueueLen = 0;
	free(pstQueueHead);
	pstQueueHead = NULL;

	return;
}

HI_VOID SAMPLE_IVE_QueueClear(SAMPLE_IVE_QUEUE_S* pstQueueHead)
{
	SAMPLE_IVE_NODE_S *pstQueueTmp = NULL;

	if (NULL == pstQueueHead)
	{
		return;
	}

	pstQueueTmp = pstQueueHead->front;
	while(NULL != pstQueueTmp)
	{
		pstQueueHead->front = pstQueueTmp->next;
		free(pstQueueTmp);
		pstQueueTmp = pstQueueHead->front;
	}
	pstQueueHead->rear = pstQueueHead->front;
	s_s32CurQueueLen = 0;

	return;
}

HI_BOOL SAMPLE_IVE_QueueIsEmpty(SAMPLE_IVE_QUEUE_S* pstQueueHead)
{
	if (NULL == pstQueueHead)
	{
		return HI_TRUE;
	}

	if (NULL != pstQueueHead->front)
	{
		return HI_FALSE;
	}

	return HI_TRUE;
}

HI_S32 SAMPLE_IVE_QueueSize(SAMPLE_IVE_QUEUE_S* pstQueueHead)
{
	if (NULL == pstQueueHead)
	{
		return 0;
	}

	return s_s32CurQueueLen;
}

HI_S32 SAMPLE_IVE_QueueAddNode(SAMPLE_IVE_QUEUE_S* pstQueueHead, VIDEO_FRAME_INFO_S *pstFrameInfo)
{
	SAMPLE_IVE_NODE_S *pstQueueNode = NULL;

	if ((NULL == pstQueueHead) || (NULL == pstFrameInfo))
	{
		return QUEUE_NULL_POINTER;
	}

	if ((s_s32MaxQueuelen != -1) && (s_s32CurQueueLen >= s_s32MaxQueuelen))
	{
		return QUEUE_ILLEGAL_STATE;
	}

	pstQueueNode = (SAMPLE_IVE_NODE_S*)malloc(sizeof(SAMPLE_IVE_NODE_S));
	if (NULL == pstQueueNode)
	{
		return QUEUE_OUT_OF_MEMORY;
	}

    memcpy(&pstQueueNode->stFrameInfo, pstFrameInfo, sizeof(VIDEO_FRAME_INFO_S));
	pstQueueNode->next = NULL;
	if (SAMPLE_IVE_QueueIsEmpty(pstQueueHead))
	{
		pstQueueHead->front = pstQueueNode;
		pstQueueHead->rear = pstQueueNode;
	}
	else
	{
		pstQueueHead->rear->next = pstQueueNode;
		pstQueueHead->rear = pstQueueNode;
	}

	s_s32CurQueueLen++;

	return HI_SUCCESS;
}

SAMPLE_IVE_NODE_S* SAMPLE_IVE_QueueGetHeadNode(SAMPLE_IVE_QUEUE_S* pstQueueHead)
{
	if ((NULL == pstQueueHead) || (NULL == pstQueueHead->front))
	{
		return NULL;
	}

	return pstQueueHead->front;
}

SAMPLE_IVE_NODE_S* SAMPLE_IVE_QueueGetNode(SAMPLE_IVE_QUEUE_S* pstQueueHead)
{
	SAMPLE_IVE_NODE_S *pstQueueTmp = NULL;

	if ((NULL == pstQueueHead) || (NULL == pstQueueHead->front))
	{
		return NULL;
	}

	pstQueueTmp = pstQueueHead->front;
	pstQueueHead->front = pstQueueTmp->next;
	if (NULL == pstQueueHead->front)
	{
		pstQueueHead->rear = pstQueueHead->front;
	}
	s_s32CurQueueLen--;

	return pstQueueTmp;
}

HI_VOID SAMPLE_IVE_QueueFreeNode(SAMPLE_IVE_NODE_S *pstNode)
{
    if (NULL != pstNode)
    {
        free(pstNode);
        pstNode = NULL;
    }

    return;
}

