/* spin 接口定义 
 * qq: 25088970 maohw
 */

#ifndef _LINUX_SPIN_H
#define _LINUX_SPIN_H

#ifdef __cplusplus
extern "C" {
#endif

typedef struct 
{
 volatile long  flag_;
 volatile long* spin_;
}spin_lock_t;

void spin_init(spin_lock_t* lock,long* flag);

void spin_lock(spin_lock_t* lock);

int spin_trylock(spin_lock_t* lock);

void spin_unlock(spin_lock_t* lock);

int spin_is_lock(spin_lock_t* lock);

#ifdef __cplusplus
}
#endif

#endif