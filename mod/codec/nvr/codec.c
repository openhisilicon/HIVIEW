#include <unistd.h>
#include <stdlib.h>
#include <assert.h>

#include "inc/gsf.h"
#include "mod/bsp/inc/bsp.h"

#include "codec.h"
#include "cfg.h"
#include "msg_func.h"
#include "mpp.h"
#include "live.h"


GSF_LOG_GLOBAL_INIT("CODEC", 8*1024);


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



int main(int argc, char *argv[])
{
    gsf_bsp_def_t bsp_def;  
    
    #if 1
      
    if(argc < 2)
    {
      printf("pls input: %s codec_parm.json\n", argv[0]);
      return -1;
    }
    if(json_parm_load(argv[1], &codec_nvr) < 0)
    {
      json_parm_save(argv[1], &codec_nvr);
      json_parm_load(argv[1], &codec_nvr);
    }
    
    while(1)
    {
      //register To;
      GSF_MSG_DEF(gsf_mod_reg_t, reg, 8*1024);
      reg->mid = GSF_MOD_ID_CODEC;
      strcpy(reg->uri, GSF_IPC_CODEC);
      int ret = GSF_MSG_SENDTO(GSF_ID_MOD_CLI, 0, SET, GSF_CLI_REGISTER, sizeof(gsf_mod_reg_t), GSF_IPC_BSP, 2000);
      printf("GSF_CLI_REGISTER To:%s, ret:%d, size:%d\n", GSF_IPC_BSP, ret, __rsize);
      
      static int cnt = 3;
      if(ret == 0)
        break;
      if(cnt-- < 0)
        return -1;
      sleep(1);
    }
    if(1)
    {
      //get bsp_def;
      GSF_MSG_DEF(gsf_msg_t, msg, 4*1024);
      int ret = GSF_MSG_SENDTO(GSF_ID_BSP_DEF, 0, GET, 0, 0, GSF_IPC_BSP, 2000);
      gsf_bsp_def_t *cfg = (gsf_bsp_def_t*)__pmsg->data;
      printf("GET GSF_ID_BSP_DEF To:%s, ret:%d, model:%s\n", GSF_IPC_BSP, ret, cfg->board.model);
    
      if(ret < 0)
        return 0;
        
      bsp_def = *cfg;
    }
    
    #endif
    #if defined(GSF_CPU_3559a)
    gsf_mpp_cfg("/mnt/app/59a", NULL);
	#else
	gsf_mpp_cfg("/vap/app", NULL);
	#endif
    //gsf_mpp_vo_start(VODEV_HD0, VO_INTF_VGA|VO_INTF_HDMI, VO_OUTPUT_1280x1024_60, 0);
    gsf_mpp_vo_start(VODEV_HD0, VO_INTF_HDMI, VO_OUTPUT_1080P60, 0);
    gsf_mpp_fb_start(VOFB_GUI, VO_OUTPUT_1080P60, 0);
    
    live_mon();
    
    GSF_LOG_CONN(0, 100);
    void* rep = nm_rep_listen(GSF_IPC_CODEC, NM_REP_MAX_WORKERS, NM_REP_OSIZE_MAX, req_recv);
    
    while(1)
    {
      sleep(1);
    }
    
    gsf_mpp_fb_hide(VOFB_GUI, 1);
    gsf_mpp_vo_layout(VOLAYER_HD0, VO_LAYOUT_NONE, NULL);
    gsf_mpp_vo_stop(VODEV_HD0);
    
    GSF_LOG_DISCONN();
    return 0;
}