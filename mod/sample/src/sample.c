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
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "sample.h"
#include "msg_func.h"
#include "cfg.h"

#include "fw/comm/inc/serial.h"
#include "mod/bsp/inc/bsp.h"
#include "mod/codec/inc/codec.h"
#include "mod/svp/inc/svp.h"

GSF_LOG_GLOBAL_INIT("SAMPLE", 8*1024);

static int serial_fd = -1;

// req msg;
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

// svp event;
static int sub_recv(char *msg, int size, int err)
{
  gsf_msg_t *pmsg = (gsf_msg_t*)msg;

  if(pmsg->id == GSF_EV_SVP_LPR)
  {
    gsf_svp_lprs_t *lprs = (gsf_svp_lprs_t*) pmsg->data;
    
    int i = 0;
    //lprs->cnt = 1;
    for(i = 0; i < 4 && i < lprs->cnt; i++)
    {     
      // send LPR to uart;
      if(serial_fd > 0 && lprs->result[i].number[0])
      {
        printf("GSF_EV_SVP_LPR idx: %d, w:%d,h:%d rect:[%d,%d,%d,%d], number:[%s]\n"
            , i, lprs->w, lprs->h 
            , lprs->result[i].rect[0], lprs->result[i].rect[1], lprs->result[2].rect[3], lprs->result[i].rect[4]
            , lprs->result[i].number);
        //0xAA size [data0 data1 ... sum];
        int l = 0;
        unsigned char buf[16];
        unsigned char sum = 0;
        buf[0] = 0xAA;
        buf[1] = strlen(lprs->result[i].number);
        for(l = 0; l < buf[1]; l++)
        {
          buf[2+l] = lprs->result[i].number[l];
          sum += buf[2+l];
        }
        buf[2+l] = sum;
        buf[1] += 1;
        
        int ret = write(serial_fd, buf, (2+buf[1]));
        if(ret != (2+buf[1]))
        {
           printf("err: write serial_fd:%d, ret:%d\n", serial_fd, ret);
        }  
      }
    }
  }
  return 0;
}

int main(int argc, char *argv[])
{
    if(argc < 2)
    {
      printf("pls input: %s sample_parm.json\n", argv[0]);
      return -1;
    }
    
    // load sample_parm.json;
    strncpy(sample_parm_path, argv[1], sizeof(sample_parm_path)-1);
    if(json_parm_load(sample_parm_path, &sample_parm) < 0)
    {
      json_parm_save(sample_parm_path, &sample_parm);
      json_parm_load(sample_parm_path, &sample_parm);
    }

    // listen req;
    GSF_LOG_CONN(0, 100);
    void* rep = nm_rep_listen(GSF_IPC_SAMPLE, NM_REP_MAX_WORKERS, NM_REP_OSIZE_MAX, req_recv);

    //wait codec.exe;
    while(1)
    {
      GSF_MSG_DEF(gsf_resolu_t, _res, 8*1024);
      int ret = GSF_MSG_SENDTO(GSF_ID_CODEC_VORES, 0, GET, 0, 0, GSF_IPC_CODEC, 2000);
      printf("GSF_ID_CODEC_VORES To:%s, ret:%d, size:%d\n", GSF_IPC_CODEC, ret, __rsize);
      static int cnt = 6;
      if(ret == 0)
        break;
      if(cnt-- < 0)
        return -1;
      sleep(1);
    }

    //open UART;
    if(sample_parm.en)
    {
      char ttyAMA[64] = {0};
      sprintf(ttyAMA, "/dev/%s", sample_parm.uart);
      
      if(strstr(ttyAMA, "ttyAMA4"))
        system("himm 0x111F0000 2;himm 0x111F0004 2;"); //UART4 MUX
      else if(strstr(ttyAMA, "ttyAMA2"))
        system("himm 0x114F0058 3; himm 0x114F005c 3;"); //UART2 MUX
      else if(strstr(ttyAMA, "ttyAMA3"))
        system("himm 0x114F0000 2; himm 0x114F0004 2;"); //UART3 MUX
      else 
        return -1;

      serial_fd = open(ttyAMA, O_RDWR | O_NOCTTY /*| O_NDELAY*/);
      if (serial_fd < 0)
      {
        printf("err: open %s serial_fd:%d\n", ttyAMA, serial_fd);
      }
      if(serial_set_param(serial_fd, 115200, 0, 1, 8) < 0)
      {
        printf("err: serial_set_param %s serial_fd:%d\n", ttyAMA, serial_fd);
      }
    }
    
    // subscribe SVP-EVENT;
    void* sub = nm_sub_conn(GSF_PUB_SVP, sub_recv);

    // while read UART;
    while(1)
    {
      char buf[256] = {0};
      int ret = read(serial_fd, buf, 4);
      if(ret != 4)
        usleep(10);
      //TODO;  
    }

    GSF_LOG_DISCONN();
    return 0;
}