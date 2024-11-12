#include <unistd.h>
#include <stdlib.h>
#include <assert.h>

#include "fw/comm/inc/proc.h"
#include "mod/codec/inc/codec.h"
#include "svp.h"
#include "cfg.h"
#include "msg_func.h"

#include "sample_svp_npu_process.h"

void* svp_pub = NULL;
GSF_LOG_GLOBAL_INIT("SVP", 8*1024);

static int req_recv(char *in, int isize, char *out, int *osize, int err)
{
    int ret = 0;
    gsf_msg_t *req = (gsf_msg_t*)in;
    gsf_msg_t *rsp = (gsf_msg_t*)out;

    *rsp  = *req;
    rsp->err  = 0;
    rsp->size = 0;

    ret = msg_func_proc(req, isize, rsp, osize);

    rsp->err = (ret == TRUE)?rsp->err:GSF_ERR_MSG;
    *osize = sizeof(gsf_msg_t)+rsp->size;

    return 0;
}
static int reg2bsp()
{
  while(1)
  {
    //register To;
    GSF_MSG_DEF(gsf_mod_reg_t, reg, 8*1024);
    reg->mid = GSF_MOD_ID_SVP;
    strcpy(reg->uri, GSF_IPC_SVP);
    int ret = GSF_MSG_SENDTO(GSF_ID_MOD_CLI, 0, SET, GSF_CLI_REGISTER, sizeof(gsf_mod_reg_t), GSF_IPC_BSP, 2000);
    printf("GSF_CLI_REGISTER To:%s, ret:%d, size:%d\n", GSF_IPC_BSP, ret, __rsize);
    
    static int cnt = 3;
    if(ret == 0)
      break;
    if(cnt-- < 0)
      return -1;
    sleep(1);
  }
  return 0;
}

#include <signal.h>

static void _handle_sig(int signo)
{
    if (signo == SIGINT || signo == SIGTERM) 
    {
       //extern int yolo_stop(); yolo_stop();
       sample_svp_npu_destroy();
    }
    exit(-1);
}

int sample_svp_reg_signal()
{
  struct sigaction sa;
  sa.sa_handler = _handle_sig;
  sa.sa_flags = 0;
  sigaction(SIGINT, &sa, NULL);
  sigaction(SIGTERM, &sa, NULL);
  return 0;
}

static int vores_get(gsf_resolu_t *res)
{
  while(1)
  {
    //register To;
    GSF_MSG_DEF(gsf_resolu_t, _res, 8*1024);
    int ret = GSF_MSG_SENDTO(GSF_ID_CODEC_VORES, 0, GET, 0, 0, GSF_IPC_CODEC, 2000);
    printf("GSF_ID_CODEC_VORES To:%s, ret:%d, size:%d\n", GSF_IPC_CODEC, ret, __rsize);
    static int cnt = 6;
    if(ret == 0)
    {
      *res = *_res;
      break;
    }
    if(cnt-- < 0)
      return -1;
    sleep(1);
  }
  return 0;
}


static int pub_send(yolo_boxs_t *boxs)
{
  int i = 0, j = 0;
  char buf[sizeof(gsf_msg_t) + sizeof(gsf_svp_yolos_t)];
  gsf_msg_t *msg = (gsf_msg_t*)buf;
  
  memset(msg, 0, sizeof(*msg));
  msg->id = GSF_EV_SVP_YOLO;
  msg->ts = time(NULL)*1000;
  msg->sid = 0;
  msg->err = 0;
  msg->size = sizeof(gsf_svp_yolos_t);
  msg->ch = boxs->chn;
  
  gsf_svp_yolos_t *yolos = (gsf_svp_yolos_t*)msg->data;
  
  yolos->pts = 0;
  yolos->w = boxs->w;
  yolos->h = boxs->h;
  
  yolos->cnt = boxs->size;
  yolos->cnt = (yolos->cnt > sizeof(yolos->box)/sizeof(yolos->box[0]))?sizeof(yolos->box)/sizeof(yolos->box[0]):yolos->cnt;

  for(i = 0; i< yolos->cnt; i++)
	{
	  yolos->box[j].score   = boxs->box[i].score;
		yolos->box[j].rect[0] = boxs->box[i].x;
		yolos->box[j].rect[1] = boxs->box[i].y;
		yolos->box[j].rect[2] = boxs->box[i].w;
		yolos->box[j].rect[3] = boxs->box[i].h;
		if(boxs->box[i].label)
		{
		  strncpy(yolos->box[j].label, boxs->box[i].label, sizeof(yolos->box[j].label)-1);
		  yolos->box[j].label[sizeof(yolos->box[j].label)-1] = '\0';//warn: strncpy without eof;
		}
		else 
    {
      sprintf(yolos->box[j].label, "%.0f", boxs->box[i].score);
    }
		
    #if 1
	  printf("chn:%d, j: %d w:%d,h:%d, rect[%d,%d,%d,%d], label[%s], score:%f\n"
	        , msg->ch, j, yolos->w, yolos->h
	        , yolos->box[j].rect[0]
	        , yolos->box[j].rect[1]
	        , yolos->box[j].rect[2]
	        , yolos->box[j].rect[3]
	        , yolos->box[j].label, yolos->box[j].score);
	  #endif
	  j++;
	} yolos->cnt = j;
	//printf("chn:%d, yolos->cnt:%d\n", msg->ch, yolos->cnt);
	
  return nm_pub_send(svp_pub, (char*)msg, sizeof(*msg)+msg->size);
}



int main(int argc, char *argv[])
{
    if(argc < 2)
    {
      printf("pls input: %s svp_parm.json\n", argv[0]);
      return -1;
    }
     
    strncpy(svp_parm_path, argv[1], sizeof(svp_parm_path)-1);
    
    if(json_parm_load(svp_parm_path, &svp_parm) < 0)
    {
      json_parm_save(svp_parm_path, &svp_parm);
      json_parm_load(svp_parm_path, &svp_parm);
    }
 
    svp_pub = nm_pub_listen(GSF_PUB_SVP);
     
    //wait codec.exe startup
    gsf_resolu_t res = {0};
    vores_get(&res); 
     
    char home_path[256] = {0};
    proc_absolute_path(home_path);
    strcat(home_path, "/../");
    printf("home_path:[%s]\n", home_path);
    
    sample_svp_reg_signal();
    printf("init algorithm library...\n");
    
    char ModelPath[256];
    sprintf(ModelPath, "%s/model/yolov8.om", home_path);
    int ret = sample_svp_npu_init(ModelPath);
    
    //init listen;
    GSF_LOG_CONN(0, 100);
    void* rep = nm_rep_listen(GSF_IPC_SVP
                    , NM_REP_MAX_WORKERS
                    , NM_REP_OSIZE_MAX
                    , req_recv);
    //reg2bsp();
    
    {
      hi_vpss_chn_attr chn_attr;
      const hi_s32 vpss_grp = 0;
      hi_s32 vpss_chn[] = { HI_VPSS_CHN0, HI_VPSS_CHN1 };
      ret = hi_mpi_vpss_get_chn_attr(vpss_grp, vpss_chn[0], &chn_attr);
      chn_attr.depth = 2;
      chn_attr.chn_mode = HI_VPSS_CHN_MODE_USER;
      ret = hi_mpi_vpss_set_chn_attr(vpss_grp, vpss_chn[0], &chn_attr);

      ret = hi_mpi_vpss_get_chn_attr(vpss_grp, vpss_chn[1], &chn_attr);
      chn_attr.depth = 2;
      chn_attr.chn_mode = HI_VPSS_CHN_MODE_USER;
      ret = hi_mpi_vpss_set_chn_attr(vpss_grp, vpss_chn[1], &chn_attr);
    }
    
    while(1)
    {                        
      hi_video_frame_info base_frame, ext_frame;
      const hi_s32 milli_sec = 20000;
      const hi_s32 vpss_grp = 0;
      hi_s32 vpss_chn[] = { HI_VPSS_CHN0, HI_VPSS_CHN1 };
      yolo_boxs_t boxs = {0};
      
      ret = hi_mpi_vpss_get_chn_frame(vpss_grp, vpss_chn[1], &ext_frame, milli_sec);
      ret = hi_mpi_vpss_get_chn_frame(vpss_grp, vpss_chn[0], &base_frame, milli_sec);
      ret = sample_svp_npu_detect(&ext_frame, &base_frame, &boxs);
      if(ret == 0)
      {
        pub_send(&boxs);
      }  
      ret = hi_mpi_vpss_release_chn_frame(vpss_grp, vpss_chn[0], &base_frame);
      ret = hi_mpi_vpss_release_chn_frame(vpss_grp, vpss_chn[1], &ext_frame);
    }
    
    GSF_LOG_DISCONN();
    return 0;
}