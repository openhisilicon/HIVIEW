#include <unistd.h>
#include <stdlib.h>
#include <sys/time.h>
#include "inc/gsf.h"

#include "rec.h"
#include "cfg.h"
#include "msg_func.h"
#include "file.h"
#include "disk.h"
#include "list.h"
#include "server.h"

#define SER_USED_PER  (95)           // 98%
//#define SER_FILE_SIZE (64*1024*1024) // BYTE
//#define SER_FILE_SIZE (3*1024*1024*1024U) // BYTE


#define SER_FILENAME(mnt, info, name) \
        sprintf(name, "%s/%s/%08d_%03d_%03d.%s", mnt, ((info)->tags&GSF_FILE_TAGS_PIC)?"image":"data",\
        (info)->btime, (info)->btime_ms, (info)->channel, \
        (((info)->tags&GSF_FILE_TAGS_PIC)||((info)->tags&GSF_FILE_TAGS_THUMB))?"jpg":"mp4")
enum {
  SER_STAT_ERR  = -1,
  SER_STAT_NONE = 0,
  SER_STAT_INIT = 1,
};

typedef struct {
  struct list_head list;
  gsf_disk_t disk;
  void* ti;
}ser_disk_t;

typedef struct {
  fd_av_t* fd;
  unsigned int sync_file_sec;
  ti_file_info_t info;
}ser_file_t;


typedef struct {
  int stat;
  pthread_t pid;
  pthread_mutex_t lock;
  struct list_head disks;
  ser_disk_t *curr;
  unsigned int check_disk_sec;
  ser_file_t chs[GSF_REC_CH_NUM];
}ser_t;
static ser_t ser;

#include "inc/frm.h"

static int format_cb(struct gsf_disk_f_s *owner, int percent)
{
  printf("format disk_id:%d, percent:%d%%\n"
        , owner->disk->disk_id, percent);
  return 0;
}

static int unclosed_cb(void *_ti, ti_file_info_t *info)
{
  struct timeval tv;
  gettimeofday(&tv, NULL);
  
  // commit TI_CLOSE;
  char filename[256];
  SER_FILENAME(ser.curr->disk.mnt_dir, info, filename);
  fd_stat(filename, &info->size, &info->etime, NULL);
  int err = ti_sync(_ti, TI_CLOSE, info, 1);
  return 0;
}


static int pattern_name(char *pt, char *name)
{
  char a[8], b[8], c[8], d[8];
  return sscanf(name, pt, a, b, c, d);
}


static int scan_cb(struct gsf_disk_q_s *owner, gsf_disk_t *disk)
{
  int ret = 0;
  char mnt_dir[256] = {0};
  
  printf("%s => dev_name:%s, mnt_dir:%s\n", __func__, disk->dev_name, disk->mnt_dir);
  
  if(pattern_name(rec_parm.pattern, disk->dev_name) <= 0)
  {
    warn("UNMATCH devname:%s, pattern:%s\n", disk->dev_name, rec_parm.pattern);
    return 0;
  }
  info("MATCH devname:%s, pattern:%s\n", disk->dev_name, rec_parm.pattern);
  
  if(disk->grp_id == -1)
  {
    gsf_disk_f_t f;
    f.disk = disk;
    f.uargs = NULL;
    f.cb = format_cb;
    ret = disk_format(&f);
  }
  
  if(ret == 0)
  {
    char cmd[256] = {0};
    sprintf(mnt_dir, "/mnt/rec%08d", disk->disk_id);
    sprintf(cmd, "mkdir -p %s", mnt_dir);
    system(cmd);
    strcpy(disk->mnt_dir, mnt_dir);
    ret = disk_mount(disk);
  }
  
  ser_disk_t *ser_disk = (ser_disk_t *)calloc(1, sizeof(ser_disk_t));
  ser_disk->disk = *disk;
  list_add(&ser_disk->list, &ser.disks);
  ser.curr = ser_disk;
  
  if(ret == 0)
  {
    ti_init_t ini;
    memset(&ini, 0, sizeof(ini));
    snprintf(ini.path, sizeof(ini.path), "%s", mnt_dir);
    ini.unclosed = unclosed_cb;
    ser_disk->ti = ti_int(&ini, &ret);
  }
  
  if(ret == 0)
  {
    char cmd[256] = {0};
    sprintf(cmd, "mkdir -p %s/data %s/image", mnt_dir, mnt_dir);
    system(cmd);
  }

  ser.stat = ret;
  return 0;
}

static int ser_disk_check()
{
  struct timeval tv;
  
  gettimeofday(&tv, NULL);
  if(tv.tv_sec - ser.check_disk_sec < 10)
    return 0;
    
  ser.check_disk_sec = tv.tv_sec;

  int cnt = 10, ret = 0;
  do{
  
    int percent = disk_percent(ser.curr->disk.mnt_dir);
    if(percent < 0)
    {
      printf("err percent:%d\n", percent);
      return -1;
    }
    if(percent >= SER_USED_PER)
    {
      ti_file_info_t _info;
      ti_file_info_t *info = &_info;
      if((ret = ti_oldset_get(ser.curr->ti, info, NULL)) < 0)
      {
        printf("err ti_oldset_get ret:%d\n", ret);  
        return -1;
      }
      char filename[256];
      SER_FILENAME(ser.curr->disk.mnt_dir, info, filename);
      printf("fd_rm filename[%s]\n", filename);
      fd_rm(filename);
      
      if((ret = ti_oldset_rm(ser.curr->ti, 1)) < 0)
      {
        printf("err ti_oldset_rm ret:%d\n", ret); 
        return -1;
      }
    }
    else 
    {
      break;
    }
  }while(cnt-- > 0);
  
  return 0;
}

#define IMAGE_CHN_BASE 8 //image channel base;

int ser_image_writer(int ch, int tags, char *filename)
{
  int err = 0;
  if(ser.stat < SER_STAT_INIT)
    return -1;  
  
  ti_file_info_t _info;
  ti_file_info_t *info = &_info;
  fd_stat(filename, &info->size, &info->btime, &info->btime_ms);
  info->channel  = IMAGE_CHN_BASE + ch;
  info->etime    = info->btime;
  info->tags     = GSF_FILE_TAGS_PIC;

  pthread_mutex_lock(&ser.lock);
  err = ti_sync(ser.curr->ti, TI_OPEN, info, 1);
  pthread_mutex_unlock(&ser.lock);
  
  char dst[256];
  SER_FILENAME(ser.curr->disk.mnt_dir, info, dst);
  
  char cmd[256];
  sprintf(cmd, "cp %s %s", filename, dst);
  printf("%s => %s\n", __func__, cmd);
  system(cmd);

  struct timeval tv;
  gettimeofday(&tv, NULL); 
  info->etime = tv.tv_sec;
  pthread_mutex_lock(&ser.lock);
  err = ti_sync(ser.curr->ti, TI_CLOSE, info, 1);
  pthread_mutex_unlock(&ser.lock);
  
  return err;
}


static int ser_file_writer(int ch, gsf_frm_t *frm)
{
  int idr = 0, err = 0;
  char filename[256];
  struct timeval tv;
  ti_file_info_t *info = &ser.chs[ch].info;
  fd_av_t **fd = &ser.chs[ch].fd;
  unsigned int *sync_file_sec = &ser.chs[ch].sync_file_sec;
    
  if(!frm || !frm->size)
    return -1;
  
  idr = ((frm->type == GSF_FRM_VIDEO) && (frm->flag & GSF_FRM_FLAG_IDR));

  struct timespec ts1, ts2;  

__open:
  if(*fd == NULL)
  {
    clock_gettime(CLOCK_MONOTONIC, &ts1);
    gettimeofday(&tv, NULL);
    info->btime    = tv.tv_sec;
    info->btime_ms = tv.tv_usec/1000;
    info->channel  = ch;
    info->etime    = info->btime;
    info->tags     = GSF_FILE_TAGS_TIMER;
    info->size     = 0;
    
    pthread_mutex_lock(&ser.lock);
    err = ti_sync(ser.curr->ti, TI_OPEN, info, 1);
    pthread_mutex_unlock(&ser.lock);

    SER_FILENAME(ser.curr->disk.mnt_dir, info, filename);
    *fd = fd_av_open(filename);
    if(*fd == NULL)
      return -1;

    clock_gettime(CLOCK_MONOTONIC, &ts2);
    int cost = (ts2.tv_sec*1000 + ts2.tv_nsec/1000000) - (ts1.tv_sec*1000 + ts1.tv_nsec/1000000);

    printf("open filename[%s], cost:%dms\n", filename, cost);
  }
  
  gettimeofday(&tv, NULL);
  int segtime = rec_parm.cfg[ch].segtime?:5;
  if((abs(tv.tv_sec - info->btime) >= segtime) && idr)
  //if((fd_av_size(*fd) > SER_FILE_SIZE) && idr)
  {
    clock_gettime(CLOCK_MONOTONIC, &ts1);
    SER_FILENAME(ser.curr->disk.mnt_dir, info, filename);
    info->etime = tv.tv_sec;
    info->tags  = GSF_FILE_TAGS_TIMER;
    info->size  = fd_av_size(*fd);
    
    pthread_mutex_lock(&ser.lock);
    err = ti_sync(ser.curr->ti, TI_CLOSE, info, 1);
    pthread_mutex_unlock(&ser.lock);

    fd_av_close(*fd);
    *fd = NULL;
    
    clock_gettime(CLOCK_MONOTONIC, &ts2);
    int cost = (ts2.tv_sec*1000 + ts2.tv_nsec/1000000) - (ts1.tv_sec*1000 + ts1.tv_nsec/1000000);
    printf("close filename[%s], cost:%dms\n", filename, cost);
    goto __open;
  }
  
  rec_rw_info_t winfo;
  gettimeofday(&tv, NULL);
  winfo.time = tv.tv_sec;//frm->utc;
  winfo.ms   = frm->pts;
  
  if(frm->type == GSF_FRM_VIDEO)
  {
    //winfo.type = frm->video.type; // IDR
    winfo.type = (frm->flag & GSF_FRM_FLAG_IDR)?GSF_FRM_VIDEO_I:GSF_FRM_VIDEO_P;
    winfo.enc  = frm->video.encode;
    winfo.v.w  = frm->video.width;
    winfo.v.h  = frm->video.height;
    winfo.v.fps= 30;
    //printf("VIDEO ch:%d, type:%d, enc:%d, w:%d, h:%d, ms:%u\n", ch, winfo.type, winfo.enc, winfo.v.w, winfo.v.h, winfo.ms);
  } 
  else
  {
    winfo.type = GSF_FRM_AUDIO_1;
    winfo.enc  = frm->audio.encode;
    winfo.a.sp = frm->audio.sp;
    winfo.a.bps = frm->audio.bps;
    winfo.a.chs= frm->audio.chn;
    //printf("AUDIO ch:%d, type:%d, enc:%d, sp:%d, chs:%d, ms:%u\n", ch, winfo.type, winfo.enc, winfo.a.sp, winfo.a.chs, winfo.ms);
  }
  
  clock_gettime(CLOCK_MONOTONIC, &ts1);
  
  if(fd_av_write(*fd, frm->data, frm->size, &winfo) < 0)
    return -1;

  clock_gettime(CLOCK_MONOTONIC, &ts2);
  int cost = (ts2.tv_sec*1000 + ts2.tv_nsec/1000000) - (ts1.tv_sec*1000 + ts1.tv_nsec/1000000);
  if(cost > 100)
    warn("fd_av_write cost:%d ms\n", cost);
    
  if(tv.tv_sec - *sync_file_sec < 10)
    return 0;
  *sync_file_sec = tv.tv_sec;
  
  info->etime = tv.tv_sec;
  info->tags = GSF_FILE_TAGS_TIMER;
  info->size = fd_av_size(*fd);
  
  pthread_mutex_lock(&ser.lock);
  err = ti_sync(ser.curr->ti, TI_UPDATE, info, 1);
  pthread_mutex_unlock(&ser.lock);
 
  SER_FILENAME(ser.curr->disk.mnt_dir, info, filename);
  printf("ti_sync filename[%s]\n", filename);
  
  return 0;
}

static void* ser_thread(void *param)
{
  int i = 0;
  gsf_disk_q_t q = {NULL, scan_cb};
  disk_scan(&q);
  
  if(ser.stat == SER_STAT_ERR || ser.curr == NULL)
  {
    printf("%s => init err.\n", __func__);
    return NULL;
  }
  
  ser.stat = SER_STAT_INIT;
  printf("%s => init ok.\n", __func__);
  
  char *buf = malloc(GSF_FRM_MAX_SIZE);
  gsf_frm_t *frm = (gsf_frm_t *)buf;
  
  void* cfifo[GSF_REC_CH_NUM] = {NULL};
  void* cfifo_au[GSF_REC_CH_NUM] = {NULL};
  
  int ep = cfifo_ep_alloc(1);
  
  while(ser.stat == SER_STAT_INIT)
  {
    // disk check;
    if(ser_disk_check() < 0)
    {
      error("ser_disk_check err.\n");
      goto __exit;
    }
    
    for(i = 0; i < GSF_REC_CH_NUM; i++)
    {
      if(!rec_parm.cfg[i].en)
      {
        if(cfifo[i])
        {
            // stop rec;
            cfifo_ep_ctl(ep, CFIFO_EP_DEL, cfifo[i]);
            cfifo_free(cfifo[i]);
            cfifo[i] = NULL;
            
            if(cfifo_au[i])
            {
              cfifo_ep_ctl(ep, CFIFO_EP_DEL, cfifo_au[i]);
              cfifo_free(cfifo_au[i]);
              cfifo_au[i] = NULL;
            }
        }
      }
      else
      {
        if(!cfifo[i])
        {
          // start rec;
          GSF_MSG_DEF(gsf_sdp_t, gsf_sdp, sizeof(gsf_msg_t)+sizeof(gsf_sdp_t));
          gsf_sdp->video_shmid = -1;
          gsf_sdp->audio_shmid = -1;
          if(GSF_MSG_SENDTO(GSF_ID_CODEC_SDP, i, GET, 0
                                , 0
                                , GSF_IPC_CODEC
                                , 2000) < 0)
          {
            printf(" get GSF_ID_CODEC_SDP(%d) err.\n" , i);
            continue;
          }
          printf(" get GSF_ID_CODEC_SDP(%d) ok.\n" , i);
      
          cfifo[i] = cfifo_shmat(cfifo_recsize, cfifo_rectag, gsf_sdp->video_shmid);
          if(cfifo[i] == NULL)
          {
            continue;
          }
          cfifo_set_u(cfifo[i], (void*)i);
          unsigned int video_utc = cfifo_newest(cfifo[i], 1);
          cfifo_ep_ctl(ep, CFIFO_EP_ADD, cfifo[i]);
          
          cfifo_au[i] = cfifo_shmat(cfifo_recsize, cfifo_rectag, gsf_sdp->audio_shmid);
          if(cfifo_au[i])
          {
            cfifo_set_u(cfifo_au[i], (void*)(i|0xff00));
            unsigned int audio_utc = cfifo_oldest(cfifo_au[i], video_utc);
            printf("ch:%d, sync video_utc:%u, audio_utc:%u\n", i, video_utc, audio_utc);
            cfifo_ep_ctl(ep, CFIFO_EP_ADD, cfifo_au[i]);
          }
        }
      }
    }

    struct cfifo_ex* result[255] = {0};
    int fds = cfifo_ep_wait(ep, 2000, result, 255);
    if(fds <= 0)
    {
       printf("cfifo_ep_wait err fds:%d\n", fds);
    }
    
    { // interlaced write;
      struct timespec ts1, ts2;
      clock_gettime(CLOCK_MONOTONIC, &ts1);
      
      int remain = 1;
      while(remain)
      {
        remain = 0;
        for(i = 0; i < fds; i++)
        {
          int ret = 0;
          struct cfifo_ex* fifo = result[i];
__get:
          ret = cfifo_get(fifo, cfifo_recgut, (void*)frm);
          if(ret <= 0)
          {
            continue;
          }
          remain = 1;
          
          // file write;
          void* ch = cfifo_get_u(fifo);
          if(ser_file_writer((int)ch&0xff, frm) < 0)
          {
            error("ser_file_writer err.\n");
            goto __exit;
          }
          
          //get all audio frm;
          if((int)ch&0xff00)
          {
            goto __get;
          }
          
          clock_gettime(CLOCK_MONOTONIC, &ts2);
          int cost = (ts2.tv_sec*1000 + ts2.tv_nsec/1000000) - (ts1.tv_sec*1000 + ts1.tv_nsec/1000000);
          if(cost > 500)
          {
            warn("the disk is slowly.\n");
            remain = 0;
            break;
          }
        }
      }
    }
  }
  
__exit:  
  if(buf)
    free(buf);
    
  for(i = 0; i < GSF_REC_CH_NUM; i++)
  {
    if(cfifo[i])
    {
      // stop rec;
      cfifo_ep_ctl(ep, CFIFO_EP_DEL, cfifo[i]);
      cfifo_free(cfifo[i]);
      cfifo[i] = NULL;
      
      if(cfifo_au[i])
      {
        cfifo_ep_ctl(ep, CFIFO_EP_DEL, cfifo_au[i]);
        cfifo_free(cfifo_au[i]);
        cfifo_au[i] = NULL;
      }
    }
  }
  
  cfifo_ep_free(ep);
    
  printf("%s => exit.\n", __func__);
  return NULL;
}

int ser_init(void)
{
  if(ser.pid)
    return -1;
    
  disk_init();
  INIT_LIST_HEAD(&ser.disks);
  pthread_mutex_init(&ser.lock, NULL);
    
  return pthread_create(&ser.pid, NULL, ser_thread, NULL);
}

int ser_query_disk(gsf_disk_t disks[8])
{
  if(ser.stat < SER_STAT_INIT)
    return -1;
  
  int i = 0;
  ser_disk_t *n;
  list_for_each_entry(n, &ser.disks, list)
  {
    if(i >= 8)
      break;
    disks[i++] = n->disk;
  }
  
  return sizeof(gsf_disk_t)*i;
}

typedef struct {
  int c;
  gsf_rec_q_t *q;
  gsf_file_t *files;
}ti_q_uargs_t;

static int ti_q_cb(struct ti_query_cond_t *cond, ti_file_info_t *info)
{
  // user terminate the current query when return -1;
  ti_q_uargs_t *uargs = (ti_q_uargs_t*)cond->uargs;
   
  if(uargs->c >= 1024)
    return -1;
  #if 0
  printf("i:%d, ch:%d, btime:[%d.%d], etime[%d], size:%d\n"
        , uargs->c, info->channel, info->btime, info->btime_ms, info->etime, info->size);
  #endif
  
  if((info->channel == uargs->q->channel) && (info->tags & uargs->q->tags))
  {  
    uargs->files[uargs->c++] = *((gsf_file_t*)info);
  }
  return 0;
}


int ser_query_file(gsf_rec_q_t *q, gsf_file_t files[1024])
{
  if(ser.stat < SER_STAT_INIT)
    return -1;
    
  ti_q_uargs_t uargs;
  uargs.c     = 0;
  uargs.q     = q;
  uargs.files = files;
  
  ti_query_cond_t cond;
  cond.btime = q->btime;
  cond.etime = q->etime;
  cond.uargs = &uargs;
  cond.cb = ti_q_cb;
  
  pthread_mutex_lock(&ser.lock);
  ti_query(ser.curr->ti, &cond);
  pthread_mutex_unlock(&ser.lock);

  return sizeof(gsf_file_t)*uargs.c;
}

int ser_cfg_update(gsf_rec_cfg_t *cfg)
{
  // update mod cfg;
  // start/stop/...;
  return 0;
}

int ser_pattern_update(char *pattern)
{
  return 0;
}