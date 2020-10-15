#ifndef _HPT_LIST_H_
#define _HPT_LIST_H_

#ifndef _LINUX_LIST_H

#ifndef HPT_INLINE
#define HPT_INLINE __inline
#endif

typedef unsigned long HPT_UPTR;

struct cbb_list_head
{
    struct cbb_list_head *next, *prev;
};

#define LIST_HEAD_INIT(name) { &(name), &(name) }

#define LIST_HEAD(name) \
    (struct list_head name = LIST_HEAD_INIT(name))
#define INIT_LIST_HEAD(ptr) do { (ptr)->next = (ptr); (ptr)->prev = (ptr); } while (0)

static HPT_INLINE void __cbb_list_add(struct cbb_list_head *_new, struct cbb_list_head *prev, struct cbb_list_head *next)
{
    next->prev = _new;
    _new->next = next;
    _new->prev = prev;
    prev->next = _new;
}

static HPT_INLINE void cbb_list_add(struct cbb_list_head *_new, struct cbb_list_head *head)
{
    __cbb_list_add(_new, head, head->next);
}

static HPT_INLINE void cbb_list_add_tail(struct cbb_list_head *_new, struct cbb_list_head *head)
{
    __cbb_list_add(_new, head->prev, head);
}

static HPT_INLINE void __cbb_list_del(struct cbb_list_head *prev, struct cbb_list_head *next)
{
    next->prev = prev;
    prev->next = next;
}

static HPT_INLINE void cbb_list_del(struct cbb_list_head *entry)
{
    __cbb_list_del(entry->prev, entry->next);
}

static HPT_INLINE void cbb_list_del_init(struct cbb_list_head *entry)
{
    __cbb_list_del(entry->prev, entry->next);
    INIT_LIST_HEAD(entry);
}

static HPT_INLINE int cbb_list_empty(struct cbb_list_head *head)
{
    return head->next == head;
}

static HPT_INLINE void __cbb_list_splice(struct cbb_list_head *list,
        struct cbb_list_head *head)
{
    struct cbb_list_head *first = list->next;
    struct cbb_list_head *last = list->prev;
    struct cbb_list_head *at = head->next;

    first->prev = head;
    head->next = first;

    last->next = at;
    at->prev = last;
}

static HPT_INLINE void cbb_list_splice(struct cbb_list_head *list, struct cbb_list_head *head)
{
    if (!cbb_list_empty(list))
    {
        __cbb_list_splice(list, head);
    }
}

static HPT_INLINE void cbb_list_splice_init(struct cbb_list_head *list, struct cbb_list_head *head)
{
    if (!cbb_list_empty(list))
    {
        __cbb_list_splice(list, head);
        INIT_LIST_HEAD(list);
    }
}

/*#define list_entry(ptr, type, member) \
 ((type *)((char *)(ptr)-(HPT_UPTR)(&((type *)0)->member))) */
#define cbb_list_entry(ptr, type, member) \
    ((type *)((unsigned long)(ptr)-((unsigned long)(&((type *)0)->member))))

#define cbb_list_for_each(pos, head) \
    for (pos = (head)->next; ((pos != (head)) && (pos != NULL)); \
         pos = (pos)->next)

#define cbb_list_for_each_safe(pos, n, head) \
    for (pos = (head)->next, n = pos->next; pos != (head); \
         pos = n, n = pos->next)

#define cbb_get_first_item(attached, type, member) \
    ((type *)((char *)((attached)->next)-(HPT_UPTR)(&((type *)0)->member)))

#endif

#define cbb_list_for_each_entry_safe(pos, n, head, member) \
    for (pos = cbb_list_entry((head)->next, typeof(*pos), member), \
         n = cbb_list_entry(pos->member.next, typeof(*pos), member); \
         &pos->member != (head); \
         pos = n, n = cbb_list_entry(n->member.next, typeof(*n), member))


#endif
