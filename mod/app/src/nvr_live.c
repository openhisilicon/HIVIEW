#include <pthread.h>
#include "cfg.h"
#include "nvr_live.h"

typedef enum {
  CHSTAT_NONE  = 0,
  CHSTAT_ONVIF = 1,
  CHSTAT_RTSP  = 2,
}chstat_e;


typedef struct {
  pthread_t tid;
  pthread_mutex_t mutex;
  pthread_cond_t cond;

  chstat_e chstat[GSF_CODEC_NVR_CHN];
  gsf_chsrc_t chsrc[GSF_CODEC_NVR_CHN];
  gsf_shmid_t shmid[GSF_CODEC_NVR_CHN];
  
}live_mng_t;

static live_mng_t gmng;

static void* live_task(void *parm);

int live_mon()
{
  pthread_mutex_init(&gmng.mutex, NULL);
	pthread_cond_init(&gmng.cond, NULL);
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

int live_get_shmid(int layout, int voch[GSF_CODEC_NVR_CHN]
                , gsf_shmid_t shmid[GSF_CODEC_NVR_CHN])
{
  int i = 0;
  for(i = 0; i < layout; i++)
  {
    shmid[i] = gmng.shmid[voch[i]];
    printf("i:%d, video_shmid:%d\n", i, gmng.shmid[i].video_shmid);
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
        if(gmng.chstat[i] >= CHSTAT_RTSP)
        {
            GSF_MSG_DEF(gsf_rtsp_url_t, rtsp_url, 8*1024);
            rtsp_url->transp = 0;
            strcpy(rtsp_url->url, gmng.chsrc[i].st1);
            int ret = GSF_MSG_SENDTO(GSF_ID_RTSPS_C_CLOSE, 0, SET, 0
                                  , sizeof(gsf_rtsp_url_t)
                                  , GSF_IPC_RTSPS, 2000);                   
                                  
        }
        
        if(gmng.chstat[i] >= CHSTAT_ONVIF)
        {
            GSF_MSG_DEF(gsf_onvif_url_t, onvif_url, 8*1024);

            strcpy(onvif_url->url, gmng.chsrc[i].host);
            int ret = GSF_MSG_SENDTO(GSF_ID_ONVIF_C_CLOSE, 0, SET, 0
                                  , sizeof(gsf_onvif_url_t)
                                  , GSF_IPC_ONVIF, 2000);  
          
        }
        

        pthread_mutex_lock(&gmng.mutex);
        gmng.chsrc[i] = app_nvr.chsrc[i];
        pthread_mutex_unlock(&gmng.mutex);
        
        gmng.chstat[i] = CHSTAT_NONE;
        gmng.shmid[i].video_shmid = gmng.shmid[i].audio_shmid = -1;
        extern int vo_ly(); vo_ly();
      }
      
      if(gmng.chsrc[i].en)
      {
        if(strlen(gmng.chsrc[i].host) && gmng.chstat[i] < CHSTAT_ONVIF)
        {
            GSF_MSG_DEF(gsf_onvif_url_t, onvif_url, 8*1024);

            strcpy(onvif_url->url, gmng.chsrc[i].host);
            int ret = GSF_MSG_SENDTO(GSF_ID_ONVIF_C_OPEN, 0, SET, 0
                                  , sizeof(gsf_onvif_url_t)
                                  , GSF_IPC_ONVIF, 2000);
                                  
            // gmng.chsrc[i].st1 = "rtsp://admin:12345@192.168.1.60:554/1";
            
            gmng.chstat[i] = CHSTAT_ONVIF;
        }
        
        if(strlen(gmng.chsrc[i].st1) && gmng.chstat[i] < CHSTAT_RTSP)
        {
          GSF_MSG_DEF(gsf_rtsp_url_t, rtsp_url, 8*1024);
          rtsp_url->transp = 0;
          strcpy(rtsp_url->url, gmng.chsrc[i].st1);
          int ret = GSF_MSG_SENDTO(GSF_ID_RTSPS_C_OPEN, 0, SET, 0
                                , sizeof(gsf_rtsp_url_t)
                                , GSF_IPC_RTSPS, 2000); 
          if(ret == 0 && __pmsg->err == 0)
          {
            gsf_shmid_t *shmid = (gsf_shmid_t*)__pmsg->data;
            gmng.shmid[i] = *shmid;
            gmng.chstat[i] = CHSTAT_RTSP;
            extern int vo_ly(); vo_ly();
          }
        }
        
      }
      
    }
  }
  return NULL;
}
