/**
* Copyright (C), 2016-2020, Hisilicon Tech. Co., Ltd.
* All rights reserved.
*
* @file        hi_list.h
* @brief      Simple doubly linked list implementation
* @author   HiMobileCam middleware develop team
* @date      2016.06.06
*/
#ifndef __HI_LIST_H__
#define __HI_LIST_H__

#include "linux_cbb_list.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

/** free list node*/
#define HI_Free(p)  do{if(NULL != (p)){free(p);(p) = NULL;}}while(0)

/**define list head struct*/
typedef struct cbb_list_head List_Head_S;

/**define hi list head struct*/
typedef List_Head_S HI_List_Head_S;

/**init list head node*/
#define HI_LIST_INIT_HEAD_DEFINE(name) LIST_HEAD_INIT(name)

/**list head node define*/
#define HI_LIST_HEAD_DEFINE(name) LIST_HEAD(name)

/**init list head node pointer*/
#define HI_LIST_INIT_HEAD_PTR(ptr) INIT_LIST_HEAD(ptr)

/**
 * list_add - add a new entry
 * @new: new entry to be added
 * @head: list head to add
 *
 * Insert a new entry between two known consecutive entries.
 * This is only for internal list manipulation where we know
 * the prev/next entries already!
 */
#define HI_List_Add(pstruNew, pstruHead) cbb_list_add(pstruNew, pstruHead)

/**
 * list_add_tail - add a new entry
 * @new: new entry to be added
 * @head: list head to add it before
 *
 * Insert a new entry before the specified head.
 * This is useful for implementing queues.
 */
#define HI_List_Add_Tail(pstruNew, pstruHead) cbb_list_add_tail(pstruNew, pstruHead)

/*
 * Delete a list entry by making the prev/next entries
 * point to each other.
 *
 * This is only for internal list manipulation where we know
 * the prev/next entries already!
 */
#define HI_List_Del(pstruEntry) cbb_list_del(pstruEntry)

/**
 * list_del_init - deletes entry from list and reinitialize it.
 * @entry: the element to delete from the list.
 */
#define HI_List_Del_Init(pstruEntry) cbb_list_del_init(pstruEntry)

/**
 * list_empty - tests whether a list is empty
 * @head: the list to test.
 */
#define HI_List_Empty(pstruHead) cbb_list_empty(pstruHead)

/**
 * list_splice - join two lists
 * @list: the new list to add.
 * @head: the place to add it in the first list.
 */
#define HI_List_Splice(pstruList, pstruHead) cbb_list_splice(pstruList, pstruHead)

/**
 *
 * list_entry - get the struct for this entry
 * @ptr:    the &struct list_head pointer.
 * @type:   the type of the struct this is embedded in.
 * @member: the name of the list_struct within the struct.
 */
#define HI_LIST_ENTRY(ptr, type, member) cbb_list_entry(ptr, type, member)

/**
 * list_for_each    -   iterate over a list
 * @pos:    the &struct list_head to use as a loop counter.
 * @head:   the head for your list.
 */
#define HI_List_For_Each(pos, head) cbb_list_for_each(pos, head)

/**
 * list_for_each_safe   -   iterate over a list safe against removal of list entry
 * @pos:    the &struct list_head to use as a loop counter.
 * @n:      another &struct list_head to use as temporary storage
 * @head:   the head for your list.
 */
#define HI_List_For_Each_Safe(pos, n, head) cbb_list_for_each_safe(pos, n, head)

/**
 * list_for_each_entry_safe -   iterate over list of given type
 * pos:    the type * to use as a loop cursor.
 * @head:   the head for your list.
 * @member: the name of the list_struct within the struct.
 */
#define HI_List_For_Each_Entry_Safe(pos, n, head, member) cbb_list_for_each_entry_safe(pos, n, head, member)

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /*__HI_LIST_H__*/
