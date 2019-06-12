/* shm 接口定义 
 * qq: 25088970 maohw
 */

#ifndef _LINUX_SHM_H
#define _LINUX_SHM_H

#ifdef __cplusplus
extern "C" {
#endif

//shm;
#define shm_alive(shmid)  ({\
    int alive = 0;\
    unsigned int cpid; \
    int refcnt = shm_refcnt(shmid, &cpid); \
    if( cpid && refcnt > 1) \
      alive = 1; \
    alive = alive; \
  })

void* shm_alloc(int size, int *shmid);

void  shm_free(void *p);

void* shm_ref(int shmid, int *size);

void  shm_unref(void *p);

int   shm_refcnt(int shmid, unsigned int *cpid);


// shm_ptr [ for muti-process ptr object ]

typedef struct {
  int shmid;    /* shmid */
  int size;     /* size  */
  char *data;   /* shmat */
}shm_ptr_t;

shm_ptr_t* shm_ptr_init(shm_ptr_t* p, int size);

// dt == unref
void  shm_ptr_dt(shm_ptr_t *p);

shm_ptr_t* shm_ptr_ref(shm_ptr_t *p);

// unref == dt
void  shm_ptr_unref(shm_ptr_t *p);

  
#ifdef __cplusplus
}
#endif

#endif