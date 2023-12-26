#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <pthread.h>
#include "fw/comm/inc/serial.h"
#include "cfg.h"
#include "kbd.h"
#include "msg_func.h"

static pthread_t tid;
static int serial_fd;
static void* kbd_task(void *parm);

int kbd_mon(char *ttyAMA)
{
  system("himm 0x120F002C  1;");
  system("himm 0x120F0030  1;");
   
  if(!ttyAMA || strlen(ttyAMA) < 1)
    return -1;
    
  serial_fd = open(ttyAMA, O_RDWR | O_NOCTTY /*| O_NDELAY*/);
  if (serial_fd < 0)
  {
      return -2;
  }
  
	// set serial param
  if(serial_set_param(serial_fd, 9600, 0, 1, 8) < 0)
  {
    return -3;
  }
  
  return pthread_create(&tid, NULL, kbd_task, (void*)NULL);
}


static char onvif_url[128];

int kbd_set_url(char *url)
{
  if(url)
  {
    strncpy(onvif_url, url, sizeof(onvif_url)-1);
    printf("onvif_url:[%s]\n", onvif_url);
  } 
  return 0;
}



#if 0
//
//[开]        FF 01 00 53 0E 01 63 // 长按3秒
//[关]        FF 01 00 53 0E 00 62  // 长按3秒
//[开始录像]  FF 01 00 DB 00 01 DD
//[结束录像]  FF 01 00 DB 00 00 DC
//
//[开始报警]  FF 01 00 19 00 01 1B
//[清除报警]  FF 01 00 19 00 00 1A
//[F1]        FF 01 00 72 00 00 73
//[打开自动]  FF 01 00 53 05 00 59
//[关闭自动]  FF 01 00 53 05 03 5C
//
//[手动]
// 在红外模式下 // FF 01 00 53 05 11 6A
// 在车牌模式下 // FF 01 00 53 05 21 7A
// 在初始模式下 // FF 01 00 53 05 01 5A
// 在手动模式下 // FF 01 00 53 05 03 5C
//[红外]      // FF 01 00 56 00 01 58, FF 01 00 53 05 03 5C
//[车牌]      // FF 01 00 57 00 00 58, FF 01 00 53 05 02 5B         
//[聚光]         FF 01 00 53 0D 00 61
//[聚光+]        FF 01 00 53 0F 00 63
//[聚光-]        FF 01 00 53 0F 01 64
//[F2]           FF 01 00 70 00 00 71
//[泛光/红外]    FF 01 00 53 0D 01 62
//[泛光+]        FF 01 00 53 0F 10 73
//[泛光-]        FF 01 00 53 0F 11 74

//[1][2][3] 长按设置预置位 FF 01 00 03 00 XX SUM  XX表示预置位, SUM = (01+00+03+00+XX)%256
//[4][5][6] 短按调用预置位 FF 01 00 07 00 XX SUM  XX表示预置位, SUM = (01+00+07+00+XX)%256
//
//[巡航]FF 01 00 07 00 23 2B
//[复位]FF 01 00 53 00 00 54
//
//[聚焦+]FF 01 01 00 00 00 02
//[聚焦-]FF 01 00 80 00 00 81
//[光圈+]FF 01 02 00 00 00 03
//[光圈-]FF 01 04 00 00 00 05
//
// /////////////////////////////
//[抓拍] FF 01 00 53 10 01 65
//
//[变倍+]FF 01 00 40 00 00 41 
//[变倍-]FF 01 00 20 00 00 21 
//
//[上]  FF 01 00 10 XX YY SUM // XX表示水平方向的速度, 范围00~3F
//[下]  FF 01 00 08 XX YY SUM // YY表示垂直方向的速度, 范围00~3F
//[左]  FF 01 00 04 XX YY SUM // SUM=除FF数据之和取模256
//[右]  FF 01 00 02 XX YY SUM
//[左上]FF 01 00 14 XX YY SUM
//[右上]FF 01 00 12 XX YY SUM
//[左下]FF 01 00 0C XX YY SUM
//[右下]FF 01 00 0A XX YY SUM
//
#endif



static int func_zoom_add(unsigned char *buf)
{
  GSF_MSG_DEF(gsf_onvif_ptz_ctl_t, ptz, 8*1024);
  
  memset(ptz, 0, sizeof(gsf_onvif_ptz_ctl_t));
  strcpy(ptz->url, onvif_url);
  ptz->ctl.cmd = NVC_PTZ_ZOOM_ADD_START;
  ptz->ctl.speed = 5;
  
  GSF_MSG_SENDTO(GSF_ID_ONVIF_C_PTZCTL, 0, SET, 0
                , sizeof(gsf_onvif_ptz_ctl_t)
                , GSF_IPC_ONVIF, 2000);
  return 0;
}

static int func_zoom_sub(unsigned char *buf)
{
  GSF_MSG_DEF(gsf_onvif_ptz_ctl_t, ptz, 8*1024);
  
  memset(ptz, 0, sizeof(gsf_onvif_ptz_ctl_t));
  strcpy(ptz->url, onvif_url);
  ptz->ctl.cmd = NVC_PTZ_ZOOM_SUB_START;
  ptz->ctl.speed = 5;
  
  GSF_MSG_SENDTO(GSF_ID_ONVIF_C_PTZCTL, 0, SET, 0
                , sizeof(gsf_onvif_ptz_ctl_t)
                , GSF_IPC_ONVIF, 2000);
  return 0;
}

static int func_zoom_stop(unsigned char *buf)
{
  GSF_MSG_DEF(gsf_onvif_ptz_ctl_t, ptz, 8*1024);
  
  memset(ptz, 0, sizeof(gsf_onvif_ptz_ctl_t));
  strcpy(ptz->url, onvif_url);
  ptz->ctl.cmd = NVC_PTZ_ZOOM_SUB_STOP;
  ptz->ctl.speed = 5;
  
  GSF_MSG_SENDTO(GSF_ID_ONVIF_C_PTZCTL, 0, SET, 0
                , sizeof(gsf_onvif_ptz_ctl_t)
                , GSF_IPC_ONVIF, 2000);
  return 0;
}

static int func_layout(unsigned char *buf)
{
  static int num = 0;
  extern int vo_ly(int num); vo_ly(num++%5+1);
}


typedef int(func_t)(unsigned char *buf);

static func_t* func[0x08ff] = {
  [0x0000] = func_zoom_stop,
  [0x0040] = func_zoom_add,
  [0x0020] = func_zoom_sub,
  [0x0053] = func_layout,
};


static void* kbd_task(void *parm)
{
  int ret = 0;
  unsigned short cmd = 0;
  unsigned char buf[128] = {0};
  unsigned char buf2[128] = {0};
  while(1)
  {
    ret = read(serial_fd, buf, 7);
    
    cmd = (buf[2] << 8) | buf[3];
    
    if(  cmd == 0x0000 // rocker
      || cmd == 0x0040 || cmd == 0x0020
      || cmd == 0x0010 || cmd == 0x0008
      || cmd == 0x0004 || cmd == 0x0002
      || cmd == 0x0014 || cmd == 0x0012
      || cmd == 0x000C || cmd == 0x000A)
    {
      if(!memcmp(buf2, buf, 7))
      {
        memcpy(buf2, buf, 7);
        continue;
      }
    }
    memcpy(buf2, buf, 7);
    
    printf("ret:%d, buf[%02x %02x %02x %02x %02x %02x %02x]\n"
          , ret, buf[0], buf[1], buf[2], buf[3], buf[4], buf[5], buf[6]);

    if(buf[0] == 0xff)
    {  
      if(cmd >= 0 && cmd < 0x08ff && func[cmd])
      {
        func[cmd](buf);
      }
    }
        
  }
  return NULL;
}