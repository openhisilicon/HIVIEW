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

#define SER_USED_PER  (98)           // 98%
#define SER_FILE_SIZE (64*1024*1024) // BYTE

#define SER_FILENAME(mnt, info, name) \
        sprintf(name, "%s/data/%08d_%03d_%03d.%s", mnt, \
        (info)->btime, (info)->btime_ms, (info)->channel, ((info)->tags&GSF_FILE_TAGS_PIC)?"jpg":"mp4")
enum {
  SER_STAT_ERR  = -1,
  SER_STAT_NONE = 0,
  SER_STAT_INIT = 1,
};

#define FRAME_MAX_SIZE (800*1024)


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


static unsigned int cfifo_recsize(unsigned char *p1, unsigned int n1, unsigned char *p2)
{
    unsigned int size = sizeof(gsf_frm_t);

    if(n1 >= size)
    {
        gsf_frm_t *rec = (gsf_frm_t*)p1;
        return  sizeof(gsf_frm_t) + rec->size;
    }
    else
    {
        gsf_frm_t rec;
        char *p = (char*)(&rec);
        memcpy(p, p1, n1);
        memcpy(p+n1, p2, size-n1);
        return  sizeof(gsf_frm_t) + rec.size;
    }
    
    return 0;
}

static unsigned int cfifo_rectag(unsigned char *p1, unsigned int n1, unsigned char *p2)
{
    unsigned int size = sizeof(gsf_frm_t);

    if(n1 >= size)
    {
        gsf_frm_t *rec = (gsf_frm_t*)p1;
        return rec->flag & GSF_FRM_FLAG_IDR;
    }
    else
    {
        gsf_frm_t rec;
        char *p = (char*)(&rec);
        memcpy(p, p1, n1);
        memcpy(p+n1, p2, size-n1);
        return rec.flag & GSF_FRM_FLAG_IDR;
    }
    
    return 0;
}

static unsigned int cfifo_recgut(unsigned char *p1, unsigned int n1, unsigned char *p2, void *u)
{
    unsigned int len = cfifo_recsize(p1, n1, p2);
    unsigned int l = CFIFO_MIN(len, n1);
    
    char *p = (char*)u;
    memcpy(p, p1, l);
    memcpy(p+l, p2, len-l);

    if(0)
    {
      gsf_frm_t *rec = (gsf_frm_t *)u;
    	struct timespec _ts;  
      clock_gettime(CLOCK_MONOTONIC, &_ts);
      int cost = (_ts.tv_sec*1000 + _ts.tv_nsec/1000000) - rec->utc;
      if(cost > 33)
        printf("u:%p, get rec ok [delay:%d ms].\n", u, cost);
        
      assert(rec->data[0] == 00 && rec->data[1] == 00 && rec->data[2] == 00 && rec->data[3] == 01);
    }
    return len;
}



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
  struct stat st;
  char filename[256];
  SER_FILENAME(ser.curr->disk.mnt_dir, info, filename);
  if(stat(filename, &st) == 0)
  {
    info->size  = st.st_size;
    info->etime = st.st_mtime;
  }
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
    sprintf(cmd, "mkdir -p %s/data", mnt_dir);
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
  
  int percent = disk_percent(ser.curr->disk.mnt_dir);
  if(percent < 0)
    return -1;
    
  if(percent >= SER_USED_PER)
  {
    ti_file_info_t _info;
    ti_file_info_t *info = &_info;
    ti_oldset_get(ser.curr->ti, info, NULL);
    
    char filename[256];
    SER_FILENAME(ser.curr->disk.mnt_dir, info, filename);
    printf("del filename[%s]\n", filename);
    fd_rm(filename);
    ti_oldset_rm(ser.curr->ti, 1);
  }
  return 0;
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
  
__open:
  if(*fd == NULL)
  {
    gettimeofday(&tv, NULL);
    info->btime    = tv.tv_sec;
    info->btime_ms = tv.tv_usec/1000;
    info->channel  = ch;
    info->etime    = info->btime;
    info->tags     = 0;
    info->size     = 0;
    
    pthread_mutex_lock(&ser.lock);
    err = ti_sync(ser.curr->ti, TI_OPEN, info, 1);
    pthread_mutex_unlock(&ser.lock);

    SER_FILENAME(ser.curr->disk.mnt_dir, info, filename);
    *fd = fd_av_open(filename);
    
    printf("open filename[%s]\n", filename);
  }
  
  if((fd_av_size(*fd) > SER_FILE_SIZE) && idr)
  {
    SER_FILENAME(ser.curr->disk.mnt_dir, info, filename);
        
    printf("close filename[%s]\n", filename);
    
    gettimeofday(&tv, NULL);
    info->etime = tv.tv_sec;
    info->tags  = 0;
    info->size  = fd_av_size(*fd);
    pthread_mutex_lock(&ser.lock);
    err = ti_sync(ser.curr->ti, TI_CLOSE, info, 1);
    pthread_mutex_unlock(&ser.lock);

    fd_av_close(*fd);
    *fd = NULL;
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
  } 
  else
  {
    winfo.type = GSF_FRM_AUDIO_1;
    winfo.enc  = frm->audio.encode;
    winfo.a.sp = frm->audio.sp;
    winfo.a.bps = frm->audio.bps;
    winfo.a.chs= frm->audio.chn;
  }
  
  fd_av_write(*fd, frm->data, frm->size, &winfo);

  if(tv.tv_sec - *sync_file_sec < 10)
    return 0;
  *sync_file_sec = tv.tv_sec;
  
  info->etime = tv.tv_sec;
  info->tags = 0;
  info->size = fd_av_size(*fd);
  pthread_mutex_lock(&ser.lock);
  err = ti_sync(ser.curr->ti, TI_UPDATE, info, 1);
  pthread_mutex_unlock(&ser.lock);
 
  SER_FILENAME(ser.curr->disk.mnt_dir, info, filename);
  printf("write filename[%s]\n", filename);
  
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
  
  char *buf = malloc(FRAME_MAX_SIZE);
  gsf_frm_t *frm = (gsf_frm_t *)buf;
  
  void* cfifo[GSF_REC_CH_NUM] = {NULL};
  void* cfifo_au[GSF_REC_CH_NUM] = {NULL};
  
  int ep = cfifo_ep_alloc(1);
  
  while(ser.stat == SER_STAT_INIT)
  {
    // disk check;
    ser_disk_check();
    
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
            cfifo_set_u(cfifo_au[i], (void*)i);
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

    for(i = 0; i < fds; i++)
    {
      do{
        struct cfifo_ex* fifo = result[i];
        int ret = cfifo_get(fifo, cfifo_recgut, (void*)frm);
        if(ret <= 0)
        {
          break;
        }
        // file write;
        void* ch = cfifo_get_u(fifo);
        ser_file_writer((int)ch, frm);
      }while(1);
    }
  }
  
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
  uargs->files[uargs->c++] = *((gsf_file_t*)info);
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