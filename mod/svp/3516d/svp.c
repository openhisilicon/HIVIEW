#include <unistd.h>
#include <stdlib.h>
#include <assert.h>

#include "sample_ive_main.h"  // SAMPLE_IVE_Md();
#include "sample_nnie_main.h"

#include "svp.h"
#include "cfg.h"
#include "msg_func.h"
#include "nnie_face_api.h"
#include "FD.h"

void* svp_pub = NULL;
GSF_LOG_GLOBAL_INIT("SVP", 8*1024);

static int svp_fd_start(int para);
static int svp_fd_stop(int para);
static int svp_lpr_start(int para);
static int svp_lpr_stop(int para);

static gsf_svp_alginfo AlgInfo[] =
{
	{GSF_SVP_LPR,0,svp_lpr_start,svp_lpr_stop},	
	{GSF_SVP_FD,0,svp_fd_start,svp_fd_stop},
	{GSF_SVP_MD,0,svp_lpr_start,svp_lpr_stop},	
};
static int svp_fd_start(int para)
{
	fd_start();
	return 0;
}

static int svp_fd_stop(int para)
{

	fd_stop();
	return 0;
}
static int svp_lpr_start(int para)
{
	extern int lpr_start(); 
	lpr_start();
	return 0;
}

static int svp_lpr_stop(int para)
{
	extern int lpr_stop(); 
	lpr_stop();
	return 0;
}

static int param_setting(gsf_svp_t *svp)
{
	if(1 == svp->md_alg)
		AlgInfo[GSF_SVP_MD].enble = 1;
	if(1 == svp->fd_alg)
		AlgInfo[GSF_SVP_FD].enble = 1;
	if(1 == svp->lpr_alg)
		AlgInfo[GSF_SVP_LPR].enble = 1;
}
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

HI_VOID SAMPLE_IVE_HandleSig2(HI_S32 s32Signo)
{
  signal(SIGINT,SIG_IGN);
  signal(SIGTERM,SIG_IGN);
  
    int i = 0;
	for(i = 0; i<GSF_SVP_MAX; i++)
	{
		if(1 == AlgInfo[i].enble)
		{
			AlgInfo[i].stop(i);
		}
	}
  exit(-1);
}


int main(int argc, char *argv[])
{
    signal(SIGINT, SAMPLE_IVE_HandleSig2);
    signal(SIGTERM, SAMPLE_IVE_HandleSig2);
    
    if(argc < 2)
    {
      printf("pls input: %s svp_parm.json\n", argv[0]);
      return -1;
    }
    
    strncpy(svp_parm_path, argv[1], sizeof(svp_parm_path)-1);
    
    if(json_parm_load(svp_parm_path, &svp_cfg) < 0)
    {
      json_parm_save(svp_parm_path, &svp_cfg);
      json_parm_load(svp_parm_path, &svp_cfg);
    }
    
    svp_pub = nm_pub_listen(GSF_PUB_SVP);

	//which alg should be running
	param_setting(&svp_cfg);

	//init for alg
    int i = 0;
	for(i = 0; i<GSF_SVP_MAX; i++)
	{
		if(1 == AlgInfo[i].enble)
		{
			AlgInfo[i].start(i);
		}
	}
    
    //init listen;
    GSF_LOG_CONN(0, 100);
    void* rep = nm_rep_listen(GSF_IPC_SVP
                    , NM_REP_MAX_WORKERS
                    , NM_REP_OSIZE_MAX
                    , req_recv);
    reg2bsp();
        
    while(1)
    {                        
      sleep(5);
    }
    
    SAMPLE_IVE_Md_HandleSig();
    
    GSF_LOG_DISCONN();

  return 0;
}
