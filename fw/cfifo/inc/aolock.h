/* aolock Ô­×Ó×ÔÐýËø
 * qq: 25088970 maohw
 */
 
#ifndef __aolock_h__
#define __aolock_h__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

typedef struct
{
    volatile uint64_t       shared  : 1;
    volatile uint64_t       magic   : 7;
    volatile uint64_t       pid     : 32;
    volatile uint64_t       value   : 24;
} AO_SpinlockT;
 
#define AO_LOCK_INLOCK          (1)
#define AO_LOCK_UNLOCK          (0)
#define AO_LOCK_MAGIC           (119)
    
void(AO_SpinlockInit)(AO_SpinlockT * lock, bool shared);
bool(AO_SpinTrylock)(AO_SpinlockT * lock, long val);
void(AO_SpinLock)(AO_SpinlockT * lock, long val);
void(AO_SpinUnlock)(AO_SpinlockT * lock, long val);

#define AO_SpinlockInit(l, s) \
    ((AO_SpinlockInit)((l), (s)))
 
#define AO_SpinTrylock(l) \
    ((AO_SpinTrylock)((l), (long)AO_LOCK_INLOCK))
 
#define AO_SpinLock(l) \
    ((AO_SpinLock)((l), (long)AO_LOCK_INLOCK))
 
#define AO_SpinUnlock(l) \
    ((AO_SpinUnlock)((l), (long)AO_LOCK_INLOCK))

#ifdef __cplusplus
}
#endif

#endif //__aolock_h__