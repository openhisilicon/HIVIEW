#include <stdlib.h>
#include<stdio.h>
#include<string.h>
#include <sys/timeb.h>
#include <sys/time.h>
#include <time.h>
#include <unistd.h>

#include "yolo_sample.h"

#include "sample_comm_ive.h"
#include "ivs_md.h"
#include "svp.h"
extern void* svp_pub;

static pthread_t s_hMdThread = 0;
static HI_BOOL s_bStopSignal = HI_FALSE;
static void* pHand = NULL;
static void* yolo_task(void* p);

int yolo_start(char *home_path)
{
  int VpssGrp = 0;
  int VpssChn = 1;
  char ModelPath[256];
  sprintf(ModelPath, "%s/model/yolov5-voc.wk", home_path);
  printf("ModelPath:[%s]\n", ModelPath);
  if(yolo_init(VpssGrp, VpssChn,  ModelPath) < 0)
  {
      printf("yolo_init err.\n");
      return -1;
  }
  s_bStopSignal = HI_FALSE;
  return pthread_create(&s_hMdThread, NULL, yolo_task, NULL);
}

int yolo_stop()
{
  if(s_hMdThread)
  {
    s_bStopSignal = HI_TRUE;
    pthread_join(s_hMdThread, NULL);
    s_hMdThread = 0;
  }
}


static pub_send(yolo_boxs_t *boxs)
{
  int i;
  char buf[sizeof(gsf_msg_t) + sizeof(gsf_svp_yolos_t)];
  gsf_msg_t *msg = (gsf_msg_t*)buf;
  
  memset(msg, 0, sizeof(*msg));
  msg->id = GSF_EV_SVP_YOLO;
  msg->ts = time(NULL)*1000;
  msg->sid = 0;
  msg->err = 0;
  msg->size = sizeof(gsf_svp_yolos_t);
  
  gsf_svp_yolos_t *yolos = (gsf_svp_yolos_t*)msg->data;
  
  yolos->pts = 0;
  yolos->w = boxs->w;
  yolos->h = boxs->h;
  
  yolos->cnt = boxs->size;
  yolos->cnt = (yolos->cnt > sizeof(yolos->box)/sizeof(yolos->box[0]))?sizeof(yolos->box)/sizeof(yolos->box[0]):yolos->cnt;
    
  for(i=0;i<yolos->cnt;i++)
	{
	  yolos->box[i].score   = boxs->box[i].score;
		yolos->box[i].rect[0] = (boxs->box[i].x > 0.0)?boxs->box[i].x:0;
		yolos->box[i].rect[1] = (boxs->box[i].y > 0.0)?boxs->box[i].y:0;
		yolos->box[i].rect[2] = (boxs->box[i].w < yolos->w)?boxs->box[i].w:yolos->w;
		yolos->box[i].rect[3] = (boxs->box[i].h < yolos->h)?boxs->box[i].h:yolos->h;
		strncpy(yolos->box[i].label, boxs->box[i].label, sizeof(yolos->box[i].label)-1);
	#if 0
	  printf("i: %d w:%d,h:%d, rect[%d,%d,%d,%d], label[%s], score:%f\n"
	        , i, yolos->w, yolos->h
	        , yolos->box[i].rect[0]
	        , yolos->box[i].rect[1]
	        , yolos->box[i].rect[2]
	        , yolos->box[i].rect[3]
	        , yolos->box[i].label, yolos->box[i].score);
	#endif
	}
  nm_pub_send(svp_pub, (char*)msg, sizeof(*msg)+msg->size);
}



static void* yolo_task(void* p)
{
  while (HI_FALSE == s_bStopSignal)
  {
      usleep(33/3*1000);

      yolo_boxs_t boxs = {0};
      int ret = yolo_detect(&boxs);
      if(ret == 0)
        pub_send(&boxs);
  }
  
  yolo_boxs_t boxs = {0};
  pub_send(&boxs);
  yolo_deinit();
  return NULL;
}
