/*
 * The header file about frame cache
 */

#ifndef __FRAME_CACHE_H__
#define __FRAME_CACHE_H__

#include <pthread.h>

#define CACHE_MEM_SIZE (1843200) // 1280*720*2
#define CACHE_NODE_SIZE (6)

#define CACHE_MEM_SIZE2 (1024)
#define CACHE_NODE_SIZE2 (5)

#define CACHE_MEM_START (0)

typedef struct frame_node_t
{
    unsigned char *mem;
    unsigned int length;
    unsigned int used;
    unsigned int index;
    unsigned int offset;
    struct frame_node_t *next;
} frame_node_t;

typedef struct frame_cache_t
{
    struct frame_node_t *head;
    struct frame_node_t *tail;
    unsigned int count;
} frame_cache_t;

typedef struct frame_queue_t
{
    struct frame_cache_t *cache;
    pthread_mutex_t locker;
} frame_queue_t;

typedef struct uvc_cache_t
{
    frame_queue_t *ok_queue;
    frame_queue_t *free_queue;
} uvc_cache_t;

typedef struct uac_cache_t
{
    frame_queue_t *ok_queue;
    frame_queue_t *free_queue;
} uac_cache_t;

int create_uvc_cache(void);
void destroy_uvc_cache(void);
uvc_cache_t *uvc_cache_get(void);

int create_uac_cache(void);
void destroy_uac_cache(void);
uac_cache_t *uac_cache_get(void);

int put_node_to_queue(frame_queue_t *q, frame_node_t *node);
int get_node_from_queue(frame_queue_t *q, frame_node_t **node);
void clear_uvc_cache(void);
void debug_dump_node(frame_node_t *node);

void clear_ok_queue(void);

#endif //__FRAME_CACHE_H__
