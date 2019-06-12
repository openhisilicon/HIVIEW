/* mutex 接口定义 
 * qq: 25088970 maohw
 */

#ifndef _LINUX_MUTEX_H
#define _LINUX_MUTEX_H

#ifdef __cplusplus
extern "C" {
#endif


int mutex_init(pthread_mutex_t *mutex);

int mutex_lock(pthread_mutex_t *mutex);

int mutex_trylock(pthread_mutex_t *mutex);

int mutex_unlock(pthread_mutex_t *mutex);

int mutex_is_lock(pthread_mutex_t *mutex);



#ifdef __cplusplus
}
#endif

#endif