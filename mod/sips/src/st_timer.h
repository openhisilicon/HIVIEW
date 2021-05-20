#ifndef __ST_TIMER_H__
#define __ST_TIMRE_H__

#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/time.h>

/*************************************************************
*
* timer_cb task callback function
* Input parameter: user parameter pointer;
* Output result:!=0, the timer automatically removes this task;
*
**************************************************************/
typedef int (*timer_cb)(void *param);


void *st_timer_new();
int st_timer_free(void *timer);
int st_timer_add(void *timer, int n, int to, char *name, timer_cb func, void *param);
int st_timer_del(void *timer, int handle);

#endif // __ST_TIMER_H__
