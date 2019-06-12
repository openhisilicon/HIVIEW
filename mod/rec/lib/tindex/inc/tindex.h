// Tindex is used to record the creation order of files and provide time-based retrieval
// Author: maohw 25088970@qq.com

#ifndef __tindex_h__
#define __tindex_h__

#include <stddef.h>
#include <stdint.h>
#include <string.h>

//errno;
enum {
  TI_ERR_0  =  0,
  TI_ERR_U  = -1,
  TI_ERR_
};

// btree key = (btime << 32)|(btime_ms<<16)|(channel&0xffff)
typedef struct {
  uint32_t btime;    // file btime;
  uint16_t btime_ms; // btime ms;
  uint16_t channel;  // channel no;
  uint32_t etime;    // file endtime;
  uint32_t tags;     // file tags;
  uint32_t size;     // file size;
  uint32_t res[3];   // res;
}ti_file_info_t;


typedef struct ti_init_t {
  char path[128];   // work path;
  void *uargs;      // user args;
  int (*logger)(void *_ti, char *msg);  // debug cb;
  // user need sync unclosed file use ti_sync(ti, TI_CLOSE, file);
  int (*unclosed)(void *_ti, ti_file_info_t *info); // unclosed cb;
}ti_init_t;

//get uargs;
void* ti_uargs_get(void *_ti);

//create ti;
void* ti_int(ti_init_t *ini, int *err);

//destroy ti;
int ti_det(void *_ti);

enum {
  TI_OPEN, TI_CLOSE, TI_UPDATE
};

// sync file info to ti;
int ti_sync(void *_ti, int type, ti_file_info_t *info, int cnt);

// get oldset file info;
int ti_oldset_get(void *_ti, ti_file_info_t *info, int *cnt);

// remove oldset file info;
int ti_oldset_rm(void *_ti, int cnt);

typedef struct ti_query_cond_t {
  uint32_t btime; // file btime;
  uint32_t etime; // file etime;
  void *uargs;    // user args;
  // user terminate the current query when return -1;
  int (*cb)(struct ti_query_cond_t *cond, ti_file_info_t *info); // query cb;
}ti_query_cond_t;

// query file info;
int ti_query(void *_ti, ti_query_cond_t *cond);


#endif // __tindex_h__