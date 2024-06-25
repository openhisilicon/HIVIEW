#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <assert.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>

#include "inc/gsf.h"

#include "srts.h"
#include "cfg.h"
#include "msg_func.h"

#include "mod/bsp/inc/bsp.h"
#include "mod/codec/inc/codec.h"

GSF_LOG_GLOBAL_INIT("SRTS", 8*1024);

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

extern int srts_open(int ch, int st, char *url);
extern int srts_close();
extern int recv_open(int ch, int st, char *url);
extern int recv_close();

int main(int argc, char *argv[])
{
    if(argc < 2)
    {
      printf("pls input: %s srts_parm.json\n", argv[0]);
      return -1;
    }
    
    strncpy(srts_parm_path, argv[1], sizeof(srts_parm_path)-1);
    
    if(json_parm_load(srts_parm_path, &srts_parm) < 0)
    {
      json_parm_save(srts_parm_path, &srts_parm);
      json_parm_load(srts_parm_path, &srts_parm);
    }

    GSF_LOG_CONN(0, 100);
    void* rep = nm_rep_listen(GSF_IPC_SRTS, NM_REP_MAX_WORKERS
                              , NM_REP_OSIZE_MAX, req_recv);

    int en = 0, recv_en = 0;
#if 1
    while(1)
    {
      if(en != srts_parm.en)
      {
        if(en == 0 && srts_parm.en > 0)
        {          
          srts_open(0, 0, srts_parm.url);
        }
        else if(en > 0 && srts_parm.en == 0)
        {
          srts_close();
        }
        en = srts_parm.en;
      }
      
      if(recv_en != srts_parm.recv_en)
      {
        if(recv_en == 0 && srts_parm.recv_en > 0)
        {          
          recv_open(0, 0, srts_parm.recv_url);
        }
        else if(recv_en > 0 && srts_parm.recv_en == 0)
        {
          recv_close();
        }
        recv_en = srts_parm.recv_en;
      }
      
      sleep(1);
    }
#else
  while(1)
  {
    //test ctl msg;
    //?????¡ê?¨¦¡¤¡é?¨ª GSF_ID_SRTS_CTL???¡é????¡¤¡é?¨ªstrlen(dst)==0¨º¡À??1?¡À?2?¡¤¡é?¨ª;
    GSF_MSG_DEF(gsf_srts_ctl_t, ctl, sizeof(gsf_msg_t)+sizeof(gsf_srts_ctl_t));

    //channel=0, ¨º¨®?¦Ì¨ª¡§¦Ì¨¤o?- ?¨¦??1a-0, ¨¨¨¨??¨ª¡§¦Ì¨¤o?-1
    //stream=0,  ??¨¢¡Âo?-  ????¨ª¡§¦Ì¨¤¨®D?¡Â??¨¢¡Â-0, ¡ä???¨¢¡Â-1
    //dst=udp://226.0.0.22:7980; ??¡À¨º¦Ì??¡¤- udp://-????¨¢¡Â(?¨¦¨®?VLC2£¤¡¤?), xy://-???¨®¡Á??¡§¨°?UDP¨ª¡¤(?¨¦¨®?wireshark¡Á£¤¡ã¨¹?¨¬2¨¦)
    ctl->ch = 0; ctl->st = 0;
    strcpy(ctl->dst, "udp://226.0.0.22:7980");
    GSF_MSG_SENDTO(GSF_ID_SRTS_CTL, 0, SET, 0, sizeof(gsf_srts_ctl_t), GSF_IPC_SRTS, 2000);
    sleep(11);

    ctl->ch = 1; ctl->st = 0;
    strcpy(ctl->dst, "udp://226.0.0.22:7980");
    GSF_MSG_SENDTO(GSF_ID_SRTS_CTL, 0, SET, 0, sizeof(gsf_srts_ctl_t), GSF_IPC_SRTS, 2000);
    sleep(11);

    //channel=1,stream=0,dst=udp://226.0.0.22:7980;
    ctl->ch = 0, ctl->st = 0;
    ctl->dst[0] = '\0';
    GSF_MSG_SENDTO(GSF_ID_SRTS_CTL, 0, SET, 0, sizeof(gsf_srts_ctl_t), GSF_IPC_SRTS, 2000);
    sleep(11);
  }
#endif

    GSF_LOG_DISCONN();
    return 0;
}
