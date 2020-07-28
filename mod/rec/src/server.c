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

typedef struct {
  struct list_head list;
  gsf_disk_t disk;
  void* ti;
}ser_disk_t;

typedef struct {
  int stat;
  pthread_t pid;
  pthread_mutex_t lock;
  struct list_head disks;
  ser_disk_t *curr;
  fd_av_t* fd;
  unsigned int check_disk_sec;
  unsigned int sync_file_sec;
  ti_file_info_t info;
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

static int ser_file_write(gsf_frm_t *frm)
{
  int idr = 0, err = 0;
  char filename[256];
  struct timeval tv;
  ti_file_info_t *info = &ser.info;
  
  if(!frm || !frm->size)
    return -1;
  
  idr = (frm->flag & GSF_FRM_FLAG_IDR);
  
__open:
  if(ser.fd == NULL)
  {
    gettimeofday(&tv, NULL);
    info->btime    = tv.tv_sec;
    info->btime_ms = tv.tv_usec/1000;
    info->channel  = 0;
    info->etime    = info->btime;
    info->tags     = 0;
    info->size     = 0;
    
    pthread_mutex_lock(&ser.lock);
    err = ti_sync(ser.curr->ti, TI_OPEN, info, 1);
    pthread_mutex_unlock(&ser.lock);

    SER_FILENAME(ser.curr->disk.mnt_dir, info, filename);
    ser.fd = fd_av_open(filename);
    
    printf("open filename[%s]\n", filename);
  }
  
  if((fd_av_size(ser.fd) > SER_FILE_SIZE) && idr)
  {
    SER_FILENAME(ser.curr->disk.mnt_dir, info, filename);
        
    printf("close filename[%s]\n", filename);
    
    gettimeofday(&tv, NULL);
    info->etime = tv.tv_sec;
    info->tags  = 0;
    info->size  = fd_av_size(ser.fd);
    pthread_mutex_lock(&ser.lock);
    err = ti_sync(ser.curr->ti, TI_CLOSE, info, 1);
    pthread_mutex_unlock(&ser.lock);

    fd_av_close(ser.fd);
    ser.fd = NULL;
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
    winfo.a.sp = 8000;
    winfo.a.ss = 160;
    winfo.a.chs= 1;
  }
  
  fd_av_write(ser.fd, frm->data, frm->size, &winfo);

  if(tv.tv_sec - ser.sync_file_sec < 10)
    return 0;
  ser.sync_file_sec = tv.tv_sec;
  
  info->etime = tv.tv_sec;
  info->tags = 0;
  info->size = fd_av_size(ser.fd);
  pthread_mutex_lock(&ser.lock);
  err = ti_sync(ser.curr->ti, TI_UPDATE, info, 1);
  pthread_mutex_unlock(&ser.lock);
 
  SER_FILENAME(ser.curr->disk.mnt_dir, info, filename);
  printf("write filename[%s]\n", filename);
  
  return 0;
}


static void* ser_thread(void *param)
{
  gsf_disk_q_t q = {NULL, scan_cb};
  disk_scan(&q);
  
  if(ser.stat == SER_STAT_ERR || ser.curr == NULL)
  {
    printf("%s => init err.\n", __func__);
    return NULL;
  }
  
  ser.stat = SER_STAT_INIT;
  printf("%s => init ok.\n", __func__);
  
  char *buf = malloc(512*1024);
  gsf_frm_t *frm = (gsf_frm_t *)buf;
  
  
  GSF_MSG_DEF(gsf_sdp_t, gsf_sdp, sizeof(gsf_msg_t)+sizeof(gsf_sdp_t));
  gsf_sdp->video_shmid = -1;
  gsf_sdp->audio_shmid = -1;
  if(GSF_MSG_SENDTO(GSF_ID_CODEC_SDP, 0, GET, 0
                        , 0
                        , GSF_IPC_CODEC
                        , 2000) < 0)
  {
    printf("%s => get GSF_ID_CODEC_SDP err.\n" ,__func__);
    return 0;
  }
  printf("%s => get GSF_ID_CODEC_SDP ok.\n" ,__func__);
  
  void* cfifo = cfifo_shmat(cfifo_recsize, cfifo_rectag, gsf_sdp->video_shmid);
  
  cfifo_newest(cfifo, 1);
  
  
  while(ser.stat == SER_STAT_INIT)
  {
    // disk check;
    ser_disk_check();
    
    do{
      //cfifo get;
      int ret = cfifo_get(cfifo, cfifo_recgut, (unsigned char*)frm);
      if(ret <= 0)
          break;
      // file write;
      ser_file_write(frm);
    }while(0);
    
    usleep(10*1000);
  }
  
  if(buf)
    free(buf);
    
  if(cfifo)
    cfifo_free(cfifo);
    
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