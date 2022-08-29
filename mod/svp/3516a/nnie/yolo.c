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
#include "cfg.h"
#include "polyiou.h"
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
  yolos->cnt = 0;

  for(i=0;i<boxs->size;i++)
	{
	  if(yolos->cnt >= sizeof(yolos->box)/sizeof(yolos->box[0]))
	  {  
	    break;
	  }
	  
	  boxs->box[i].x = (boxs->box[i].x > 0.0)?boxs->box[i].x:0;
	  boxs->box[i].y = (boxs->box[i].y > 0.0)?boxs->box[i].y:0;
	  boxs->box[i].w = (boxs->box[i].w < yolos->w)?boxs->box[i].w:yolos->w;
	  boxs->box[i].h = (boxs->box[i].h < yolos->h)?boxs->box[i].h:yolos->h;
	  
	  //only used polygons[0];
    double iou = 0.1;
	  if(svp_parm.yolo.det_polygon.polygon_num && svp_parm.yolo.det_polygon.polygons[0].point_num)
	  {
	    int pn = svp_parm.yolo.det_polygon.polygons[0].point_num;
	    
	    double P[10*2] = {0};
	    double Q[4*2] = {0};
	    
	    for(int p = 0; p < pn; p++)
	    {
	      P[p*2+0] = svp_parm.yolo.det_polygon.polygons[0].points[p].x * yolos->w;
	      P[p*2+1] = svp_parm.yolo.det_polygon.polygons[0].points[p].y * yolos->h;
	    }
	    
	    Q[0] = boxs->box[i].x;
	    Q[1] = boxs->box[i].y;
	    
	    Q[2] = (boxs->box[i].x + boxs->box[i].w);
	    Q[3] = boxs->box[i].y;
	    
	    Q[4] = (boxs->box[i].x + boxs->box[i].w);
	    Q[5] = (boxs->box[i].y + boxs->box[i].h);
	    
	    Q[6] = boxs->box[i].x;
	    Q[7] = (boxs->box[i].y + boxs->box[i].h);

	    #if 0
	    printf("P(%d)[%.4f %.4f, %.4f %.4f, %.4f %.4f, %.4f %.4f, %.4f %.4f, %.4f %.4f, %.4f %.4f, %.4f %.4f]\n"           
	           , pn, P[0],P[1], P[2],P[3], P[4],P[5], P[6],P[7], P[8],P[9], P[10],P[11], P[12],P[13], P[14],P[15]);
	    printf("Q(%d)[ %.4f %.4f, %.4f %.4f, %.4f %.4f, %.4f %.4f]\n"
	          , 4, Q[0],Q[1], Q[2],Q[3], Q[4],Q[5], Q[6],Q[7]);
      #endif
      
	    iou = poly_iou(P, pn*2, Q, 4*2);
	    
	    if(iou < 0.001)
	      continue;
	  }
	  
	  yolos->box[yolos->cnt].score   = boxs->box[i].score;
		yolos->box[yolos->cnt].rect[0] = boxs->box[i].x;
		yolos->box[yolos->cnt].rect[1] = boxs->box[i].y;
		yolos->box[yolos->cnt].rect[2] = boxs->box[i].w;
		yolos->box[yolos->cnt].rect[3] = boxs->box[i].h;
		strncpy(yolos->box[yolos->cnt].label, boxs->box[i].label, sizeof(yolos->box[yolos->cnt].label)-1);

  	#if 0
	  printf("iou: %.4f, w:%d,h:%d, rect[%d,%d,%d,%d], label[%s], score:%f\n"
	        , iou, yolos->w, yolos->h
	        , yolos->box[yolos->cnt].rect[0]
	        , yolos->box[yolos->cnt].rect[1]
	        , yolos->box[yolos->cnt].rect[2]
	        , yolos->box[yolos->cnt].rect[3]
	        , yolos->box[yolos->cnt].label, yolos->box[i].score);
  	#endif
  	
  	yolos->cnt++;
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
