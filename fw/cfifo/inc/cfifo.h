/* cfifo 接口定义 
 * qq: 25088970 maohw
 */

#ifndef _LINUX_CFIFO_H
#define _LINUX_CFIFO_H

#ifdef __cplusplus
extern "C" {
#endif

#define CFIFO_MIN(x,y) ({     \
        typeof(x) _x = (x);   \
        typeof(y) _y = (y);   \
        (void) (&_x == &_y);  \
        _x < _y ? _x : _y; })

typedef unsigned int(*cfifo_recsize_t)(unsigned char *p1, unsigned int n1, unsigned char *p2);
typedef unsigned int(*cfifo_rectag_t)(unsigned char *p1, unsigned int n1, unsigned char *p2);
typedef unsigned int(*cfifo_recput_t)(unsigned char *p1, unsigned int n1, unsigned char *p2, void *u);
typedef unsigned int(*cfifo_recgut_t)(unsigned char *p1, unsigned int n1, unsigned char *p2, void *u);
typedef unsigned int(*cfifo_recrel_t)(unsigned char *p1, unsigned int n1, unsigned char *p2);

// cfifo op
struct cfifo_ex;

// create 
struct cfifo_ex *cfifo_alloc(unsigned int size
                        , cfifo_recsize_t rec_size    /* parse recsize */
                        , cfifo_rectag_t rec_tag      /* parse rectag  */
                        , cfifo_recrel_t rec_rel      /* release rec   */
                        , int *shmid, int vod);
// shmat
struct cfifo_ex *cfifo_shmat(cfifo_recsize_t rec_size
                        , cfifo_rectag_t rec_tag
                        , int shmid);
// free                
void cfifo_free(struct cfifo_ex *fifo_ex);

// newest
// param: tag =0, rec_newest; =1, rec_newest_tag;
// return: rec_tag;
unsigned int cfifo_newest(struct cfifo_ex *fifo_ex, unsigned int tag);

// oldest 
// param: tag =0, rec_oldest; =1, rec_oldest_tag; >1, rec_oldset_seek(rec_tag >= tag)
// return: rec_tag;
unsigned int cfifo_oldest(struct cfifo_ex *fifo_ex, unsigned int tag);

// put
signed int cfifo_put(struct cfifo_ex *fifo_ex
                    , unsigned int len
                    , cfifo_recput_t put_cb, void *u);
// get
signed int cfifo_get(struct cfifo_ex *fifo_ex
                    , cfifo_recgut_t get_cb, void *u);
                    
// epoll
enum {
  CFIFO_EP_ADD = 0,
  CFIFO_EP_DEL = 1,
};

// create
signed int cfifo_ep_alloc(int size);

// free
void cfifo_ep_free(int epoll_fd);

// add & del
signed int cfifo_ep_ctl(int epoll_fd, int op, struct cfifo_ex *fifo_ex);

// wait
signed int cfifo_ep_wait(int epoll_fd, int to, struct cfifo_ex **result, int n);

// info
void cfifo_info(struct cfifo_ex *fifo_ex);

// in-out
int cfifo_get_io(struct cfifo_ex *fifo_ex
                , unsigned int* in, unsigned int* out);
                
// fd
int cfifo_take_fd(struct cfifo_ex *fifo_ex);

// fd is ET or LT
int cfifo_fd_et(int fd);

// fd after POLL;
int cfifo_fd_resume(int fd);

int cfifo_set_u(struct cfifo_ex *fifo_ex, void *u);
void* cfifo_get_u(struct cfifo_ex *fifo_ex);




#ifdef __cplusplus
}
#endif

#endif
