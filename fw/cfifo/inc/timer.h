/* timer 接口定义 
 * qq: 25088970 maohw
 */

#ifndef _LINUX_TIMER_H
#define _LINUX_TIMER_H

#ifdef __cplusplus
extern "C" {
#endif

// return: -1: auto free timer;
typedef int (timer_func_t)(void *u);

void* timer_add(int ms, timer_func_t *func, void *u);

int timer_del(void* id);

  
#ifdef __cplusplus
}
#endif

#endif