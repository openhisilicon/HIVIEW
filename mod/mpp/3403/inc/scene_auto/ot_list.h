/*
  Copyright (c), 2001-2022, Shenshu Tech. Co., Ltd.
 */

#ifndef HI_LIST_H
#define HI_LIST_H

#include "linux_cbb_list.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* free list node */
#define hi_free(p) do {    \
        if ((p) != NULL) { \
            free(p);       \
            (p) = NULL;    \
        }                  \
    } while (0)

/* define list head struct */
typedef struct cbb_list_head ot_list_head;

/* init list head node */
#define ot_list_init_head_define(name) list_head_init(name)

/* list head node define */
#define ot_list_head_define(name) LIST_HEAD(name)

/* init list head node pointer */
#define ot_list_init_head_ptr(ptr) init_list_head(ptr)

/* *
 * list_add - add a new entry
 * @new: new entry to be added
 * @head: list head to add
 *
 * Insert a new entry between two known consecutive entries.
 * This is only for internal list manipulation where we know
 * the prev/next entries already!
 */
#define ot_list_add(new, head) cbb_list_add(new, head)

/* *
 * list_add_tail - add a new entry
 * @new: new entry to be added
 * @head: list head to add it before
 *
 * Insert a new entry before the specified head.
 * This is useful for implementing queues.
 */
#define ot_list_add_tail(new, head) cbb_list_add_tail(new, head)

/*
 * Delete a list entry by making the prev/next entries
 * point to each other.
 *
 * This is only for internal list manipulation where we know
 * the prev/next entries already!
 */
#define ot_list_del(entry) cbb_list_del(entry)

/* *
 * list_del_init - deletes entry from list and reinitialize it.
 * @entry: the element to delete from the list.
 */
#define ot_list_del_init(entry) cbb_list_del_init(entry)

/* *
 * list_empty - tests whether a list is empty
 * @head: the list to test.
 */
#define ot_list_empty(head) cbb_list_empty(head)

/* *
 * list_splice - join two lists
 * @list: the new list to add.
 * @head: the place to add it in the first list.
 */
#define ot_list_splice(list, head) cbb_list_splice(list, head)

/* *
 *
 * list_entry - get the struct for this entry
 * @ptr:    the &struct list_head pointer.
 * @type:   the type of the struct this is embedded in.
 * @member: the name of the list_struct within the struct.
 */
#define ot_list_entry(ptr, type, member) cbb_list_entry(ptr, type, member)

/* *
 * list_for_each    -   iterate over a list
 * @pos:    the &struct list_head to use as a loop counter.
 * @head:   the head for your list.
 */
#define ot_list_for_each(pos, head) cbb_list_for_each(pos, head)

/* *
 * list_for_each_safe   -   iterate over a list safe against removal of list entry
 * @pos:    the &struct list_head to use as a loop counter.
 * @n:      another &struct list_head to use as temporary storage
 * @head:   the head for your list.
 */
#define ot_list_for_each_safe(pos, n, head) cbb_list_for_each_safe(pos, n, head)

/* *
 * list_for_each_entry_safe -   iterate over list of given type
 * pos:    the type * to use as a loop cursor.
 * @head:   the head for your list.
 * @member: the name of the list_struct within the struct.
 */
#define ot_list_for_each_entry_safe(pos, n, head, member) cbb_list_for_each_entry_safe(pos, n, head, member)

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* HI_LIST_H */
