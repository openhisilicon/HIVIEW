#include <pthread.h>
#include "cfg.h"
#include "nvr_live.h"

typedef struct {
  pthread_t tid;
  pthread_mutex_t mutex;
  pthread_cond_t cond;

  int onvif[GSF_CODEC_NVR_CHN];
  int rtsp[GSF_CODEC_NVR_CHN][2];
  
  gsf_chsrc_t chsrc[GSF_CODEC_NVR_CHN];
  gsf_shmid_t shmid[GSF_CODEC_NVR_CHN][2];
  
}live_mng_t;

static live_mng_t gmng;

static void* live_task(void *parm);

int live_mon()
{
  int i = 0;
  memset(&gmng, 0, sizeof(gmng));
  
  pthread_mutex_init(&gmng.mutex, NULL);
	pthread_cond_init(&gmng.cond, NULL);
	
	for(i = 0; i < GSF_CODEC_NVR_CHN; i++)
  {
    gmng.shmid[i][0].video_shmid = gmng.shmid[i][0].audio_shmid = -1;
    gmng.shmid[i][1].video_shmid = gmng.shmid[i][1].audio_shmid = -1;
  }
	
  return pthread_create(&gmng.tid, NULL, live_task, (void*)NULL);
}

int live_chsrc_modify(int i, gsf_chsrc_t *chsrc)
{
  pthread_mutex_lock(&gmng.mutex);
  
  app_nvr.chsrc[i] = *chsrc;
  pthread_cond_signal(&gmng.cond);
  
  pthread_mutex_unlock(&gmng.mutex);
  return 0;
}

int live_clear_rtsp()
{
  int i = 0;
  pthread_mutex_lock(&gmng.mutex);
  for(i = 0; i < GSF_CODEC_NVR_CHN; i++)
  {
    gmng.chsrc[i].st1[0] = gmng.chsrc[i].st2[0] = '\0';
  }
  pthread_cond_signal(&gmng.cond);
  pthread_mutex_unlock(&gmng.mutex);
}

int live_clear_onvif()
{
  int i = 0;
  pthread_mutex_lock(&gmng.mutex);
  for(i = 0; i < GSF_CODEC_NVR_CHN; i++)
  {
    gmng.chsrc[i].host[0] = '\0';
  }
  pthread_cond_signal(&gmng.cond);
  pthread_mutex_unlock(&gmng.mutex);
}


int live_get_shmid(int layout, int voch[GSF_CODEC_NVR_CHN]
                , int st, gsf_shmid_t shmid[GSF_CODEC_NVR_CHN])
{
  int i = 0;
  for(i = 0; i < layout; i++)
  {
    shmid[i] = gmng.shmid[voch[i]][st];
    printf("i:%d, ch:%d, st:%d, video_shmid:%d\n",
            i, voch[i], st, shmid[i].video_shmid);
  }
  return 0;
}

// [rtsp|onvif|...]://user:pwd@ip:port/path
int url_get_user_pwd(char *url, char *user_pwd)
{
    char *p1 = strstr(url, "://");
    if(p1)
    {
      char *p2 = strstr(url, "@");
      if(p2)
        strncpy(user_pwd, p1+3, p2-p1-3);
    }
    return 0;
}
int url_add_user_pwd(char *url, char *user_pwd)
{
  char *p1 = strstr(url, "@");
  if(p1)
    return 0;
  p1 = strstr(url, "://");
  if(p1)
  {
    char tail[256] = {0};
    strncpy(tail, p1+3, sizeof(tail)-1);
    strncpy(p1+3, user_pwd, 128);
    strncat(url, "@", 256);
    strncat(url, tail, 256);
  }
  return 0;
}

static void* live_task(void *parm)
{
  int i = 0;

  while(1)
  {
    struct timespec to;
		to.tv_sec  = time(NULL) + 3;
		to.tv_nsec = 0;
    
    pthread_mutex_lock(&gmng.mutex);
    pthread_cond_timedwait(&gmng.cond, &gmng.mutex, &to);
    pthread_mutex_unlock(&gmng.mutex);
    
    for(i = 0; i < GSF_CODEC_NVR_CHN; i++)
    {
      if(memcmp(&gmng.chsrc[i], &app_nvr.chsrc[i], sizeof(gsf_chsrc_t)))
      {
        if(gmng.rtsp[i][0])
        {
          printf("close >>>>>> i:%d, st1[%s]\n", i, gmng.chsrc[i].st1);
          GSF_MSG_DEF(gsf_rtsp_url_t, rtsp_url, 8*1024);
          rtsp_url->transp = 0;
          strcpy(rtsp_url->url, gmng.chsrc[i].st1);
          int ret = GSF_MSG_SENDTO(GSF_ID_RTSPS_C_CLOSE, 0, SET, 0
                                , sizeof(gsf_rtsp_url_t)
                                , GSF_IPC_RTSPS, 2000);
          gmng.rtsp[i][0] = 0;
        }
        if(gmng.rtsp[i][1])
        {
          printf("close >>>>>> i:%d, st2[%s]\n", i, gmng.chsrc[i].st2);
          GSF_MSG_DEF(gsf_rtsp_url_t, rtsp_url, 8*1024);
          rtsp_url->transp = 0;
          strcpy(rtsp_url->url, gmng.chsrc[i].st2);
          int ret = GSF_MSG_SENDTO(GSF_ID_RTSPS_C_CLOSE, 0, SET, 0
                                , sizeof(gsf_rtsp_url_t)
                                , GSF_IPC_RTSPS, 2000);
          gmng.rtsp[i][1] = 0;
        }
        
        if(gmng.onvif[i])
        {
          printf("close >>>>>> i:%d, host[%s]\n", i, gmng.chsrc[i].host);
          GSF_MSG_DEF(gsf_onvif_url_t, onvif_url, 8*1024);
          strcpy(onvif_url->url, gmng.chsrc[i].host);
          int ret = GSF_MSG_SENDTO(GSF_ID_ONVIF_C_CLOSE, 0, SET, 0
                                , sizeof(gsf_onvif_url_t)
                                , GSF_IPC_ONVIF, 2000);  
          gmng.onvif[i] = 0;
        }
        
        pthread_mutex_lock(&gmng.mutex);
        gmng.chsrc[i] = app_nvr.chsrc[i];
        pthread_mutex_unlock(&gmng.mutex);
        
        // unref shmid;
        gmng.shmid[i][0].video_shmid = gmng.shmid[i][0].audio_shmid = -1;
        gmng.shmid[i][1].video_shmid = gmng.shmid[i][1].audio_shmid = -1;
        extern int vo_ly(int num); vo_ly(0);
      }
      
      //printf("i:%d, en:%d, host:[%s], onvif:%d, rtsp:%d\n", i, gmng.chsrc[i].en, gmng.chsrc[i].host, gmng.onvif[i], gmng.rtsp[i][0]);
            
      if(gmng.chsrc[i].en)
      {
        if(strlen(gmng.chsrc[i].host) > 0 && gmng.onvif[i] == 0)
        {
          GSF_MSG_DEF(gsf_onvif_url_t, onvif_url, 8*1024);

          strcpy(onvif_url->url, gmng.chsrc[i].host);
          int ret = GSF_MSG_SENDTO(GSF_ID_ONVIF_C_OPEN, 0, SET, 0
                                , sizeof(gsf_onvif_url_t)
                                , GSF_IPC_ONVIF, 6000);

          printf("open >>>>>> i:%d, host[%s], ret:%d, err:%d\n", 
                i, gmng.chsrc[i].host, ret, __pmsg->err);
                
          if(ret == 0 && __pmsg->err == 0)
          {
            gsf_onvif_media_url_t *murl = (gsf_onvif_media_url_t*)__pmsg->data;
            
            char user_pwd[128] = {0};
            url_get_user_pwd(gmng.chsrc[i].host, user_pwd);
            
            url_add_user_pwd(murl->st1, user_pwd);
            strcpy(gmng.chsrc[i].st1, murl->st1);
            
            url_add_user_pwd(murl->st2, user_pwd);
            strcpy(gmng.chsrc[i].st2, murl->st2);
   
            printf("i:%d, get st1[%s], st2[%s]\n", i, gmng.chsrc[i].st1, gmng.chsrc[i].st2); 
            
            gmng.onvif[i] = 1;
            
            pthread_mutex_lock(&gmng.mutex);
            app_nvr.chsrc[i] = gmng.chsrc[i];
            pthread_mutex_unlock(&gmng.mutex);
          }
        }
        
        if(strlen(gmng.chsrc[i].st1) > 0 && gmng.rtsp[i][0] == 0)
        {
          GSF_MSG_DEF(gsf_rtsp_url_t, rtsp_url, 8*1024);
          rtsp_url->transp = 0;
          strcpy(rtsp_url->url, gmng.chsrc[i].st1);
          int ret = GSF_MSG_SENDTO(GSF_ID_RTSPS_C_OPEN, 0, SET, 0
                                , sizeof(gsf_rtsp_url_t)
                                , GSF_IPC_RTSPS, 3000); 
                                
          printf("open >>>>>> i:%d, st1[%s], ret:%d, err:%d\n", 
                i, gmng.chsrc[i].st1, ret, __pmsg->err);
                                
          if(ret == 0 && __pmsg->err == 0)
          {
            // ref shmid;
            gsf_shmid_t *shmid = (gsf_shmid_t*)__pmsg->data;
            gmng.shmid[i][0] = *shmid;
            gmng.rtsp[i][0]  = 1;
            extern int vo_ly(int num); vo_ly(0);
          }
        }
        
        if(strlen(gmng.chsrc[i].st2) > 0 && gmng.rtsp[i][1] == 0)
        {
          GSF_MSG_DEF(gsf_rtsp_url_t, rtsp_url, 8*1024);
          rtsp_url->transp = 0;
          strcpy(rtsp_url->url, gmng.chsrc[i].st2);
          int ret = GSF_MSG_SENDTO(GSF_ID_RTSPS_C_OPEN, 0, SET, 0
                                , sizeof(gsf_rtsp_url_t)
                                , GSF_IPC_RTSPS, 3000); 
                                
          printf("open >>>>>> i:%d, st2[%s], ret:%d, err:%d\n", 
                i, gmng.chsrc[i].st1, ret, __pmsg->err);
                                
          if(ret == 0 && __pmsg->err == 0)
          {
            // ref shmid;
            gsf_shmid_t *shmid = (gsf_shmid_t*)__pmsg->data;
            gmng.shmid[i][1] = *shmid;
            gmng.rtsp[i][1]  = 1;
            extern int vo_ly(int num); vo_ly(0);
          }
        }
      }
      
    }
  }
  return NULL;
}
