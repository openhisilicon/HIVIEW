#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "fw/comm/inc/serial.h"
#include "cfg.h"
#include "lens.h"
#include "mpp.h"

#define DEBUG 0

static gsf_lens_ini_t _ini;
static pthread_t serial_tid;
static int serial_fd = -1;
static void* serial_task(void *parm);
static int af_uart_open(char *ttyAMA);
static int _dayNight = 0;

int gsf_lens_init(gsf_lens_ini_t *ini)
{
  _ini = *ini;
  return 0;
}

int gsf_lens_ircut(int ch, int dayNight)
{
  if(strstr(_ini.sns, "imx334"))
  {
    if(dayNight)
      system("echo 0 > /sys/class/gpio/gpio27/value;echo 0 > /sys/class/gpio/gpio28/value;");
    else
      system("echo 1 > /sys/class/gpio/gpio27/value;echo 1 > /sys/class/gpio/gpio28/value;");
  }
  else
  {
    if(dayNight)
      system("echo 0 > /sys/class/gpio/gpio27/value;echo 1 > /sys/class/gpio/gpio28/value;sleep 0.1;echo 0 > /sys/class/gpio/gpio28/value;");
    else
      system("echo 1 > /sys/class/gpio/gpio27/value;echo 0 > /sys/class/gpio/gpio28/value;sleep 0.1;echo 0 > /sys/class/gpio/gpio27/value;");
  }
  
  #if defined(GSF_CPU_3516d) || defined(GSF_CPU_3559)
  gsf_mpp_isp_ctl(0, GSF_MPP_ISP_CTL_IR, (void*)dayNight);
  #endif

  _dayNight = dayNight;
  return 0;
}

#if defined(GSF_CPU_3516d) || defined(GSF_CPU_3559)

int af_uart_write(char *buf, int size)
{
  int ret = 0;

  if(serial_fd > 0)
  {
    struct timespec ts1;  
    clock_gettime(CLOCK_MONOTONIC, &ts1);
    
    ret = write(serial_fd, buf, size);
    #if DEBUG
    printf("ret:%d, ms:%u, buf[%02X %02X %02X %02X %02X %02X %02X %02X]\n"
        , ret, (ts1.tv_sec*1000 + ts1.tv_nsec/1000000)
        , buf[0], buf[1], buf[2], buf[3], buf[4], buf[5], buf[6], buf[7]);
    #endif
	}
	return ret;
}

static int af_cb(HI_U32 Fv1, HI_U32 Fv2, HI_U32 Gain, void* uargs)
{
  char buf[8];
  HI_U32 Fv = Fv1 + Fv2;

  //FV: A5+4字节AF数据（高位在前）+ 2字节增益(模拟增益)+ 1字节颜色（彩色/黑白）
  buf[0] = 0xa5;
  buf[1] = (Fv >> 24) & 0xFF;	
  buf[2] = (Fv >> 16) & 0xFF;
  buf[3] = (Fv >> 8) & 0xFF;
  buf[4] = Fv & 0xFF;

  buf[5] = (Gain >> 8) & 0xFF;
  buf[6] = Gain & 0xFF;

  buf[7] = _dayNight; //彩色是0 黑白是1

  int ret = af_uart_write(buf, 8);
  return 0;
}

int gsf_lens_af_start(int ch, char *ttyAMA)
{
  gsf_mpp_af_t af = {
      .uargs = (void*)ch,
      .cb = af_cb,
  };
  
  af_uart_open(ttyAMA);
  if(ch < 0)
  	return 0;
  return gsf_mpp_af_start(&af);
}

int gsf_lens_stop(int ch)
{
  char buf[8] = {0xc5,0x00,0x00,0x00,0x00,0x00,0x00,0x5c};
  int ret = af_uart_write(buf, 8);
  return 0;
}

int gsf_lens_zoom(int ch,  int dir, int speed)
{
	// 派尔高D协议开始字节FF换成C5,最后补充一个字节5C组成8个字节
  char add[8] = {0xc5,0x00,0x00,0x20,0x00,0x00,0x00,0x5c};
  char sub[8] = {0xc5,0x00,0x00,0x40,0x00,0x00,0x00,0x5c};
  char *buf = (dir)?add:sub;
  int ret = af_uart_write(buf, 8);
  return 0;
}
int gsf_lens_focus(int ch, int dir, int speed)
{
	// 派尔高D协议开始字节FF换成C5,最后补充一个字节5C组成8个字节
  char add[8] = {0xc5,0x00,0x01,0x00,0x00,0x00,0x00,0x5c};
  char sub[8] = {0xc5,0x00,0x00,0x80,0x00,0x00,0x00,0x5c};
  char *buf = (dir)?add:sub;
  int ret = af_uart_write(buf, 8);
  return 0;
}

int gsf_lens_cal(int ch)
{
	// lens calibration
  char buf[8] = {0xc5,0x00,0x00,0x07,0x00,250,0x00,0x5c};
  int ret = af_uart_write(buf, 8);
  usleep(100*1000);
  ret |= af_uart_write(buf, 8);
  return 0;
}

static int af_uart_open(char *ttyAMA)
{
  if(strstr(ttyAMA, "ttyAMA4"))
    system("himm 0x111F0000 2;himm 0x111F0004 2;"); //UART4 MUX
  else if(strstr(ttyAMA, "ttyAMA2"))
    system("himm 0x114F0058 3; himm 0x114F005c 3;"); //UART2 MUX
   
  if(!ttyAMA || strlen(ttyAMA) < 1)
    return -1;
  //O_NDELAY blocking read;
  serial_fd = open(ttyAMA, O_RDWR | O_NOCTTY /*| O_NDELAY*/);
  if (serial_fd < 0)
  {
      return -2;
  }
	// set serial param
  if(serial_set_param(serial_fd, 115200, 0, 1, 8) < 0)
  {
    return -3;
  }
  return pthread_create(&serial_tid, NULL, serial_task, (void*)NULL);
}

static void* serial_task(void *parm)
{
  int ret = 0;
  unsigned short cmd = 0;
  unsigned char buf[4+256] = {0};
  while(1)
  {
    //OSD: 0xEF，0x01, 0x00，len，0x00(前景),0x02(index),line，col，char0~charN
    buf[0] = buf[1] = buf[2] = buf[3] = 0;
    ret = read(serial_fd, buf, 4);
    if(buf[0] != 0xef || buf[1] != 0x01 || buf[2] != 0x00)
    {
      #if DEBUG
      printf("err read ret:%d, buf[%02X %02X %02X %02X]\n"
          , ret, buf[0], buf[1], buf[2], buf[3]);
      #endif
      usleep(10);
      continue;
    }
    
    ret += read(serial_fd, &buf[4], buf[3]);
    #if DEBUG
    int i = 0;
    char bufstr[sizeof(buf)*3] = {0};
    for(i = 0; i < ret && i < sizeof(buf); i++)
    {
      char token[4];
      sprintf(token, "%02X ", buf[i]);
      strcat(bufstr, token);
    }
    printf("ok read ret:%d, buf[%s]\n", ret, bufstr);
    #endif
  }
  return NULL;
}

#else // !((GSF_CPU_3516d) || (GSF_CPU_3559))

int gsf_lens_af_start(int ch, char *ttyAMA) { return -1;}
int gsf_lens_stop(int ch)     { return -1;}
int gsf_lens_zoom(int ch,  int dir, int speed) { return -1;}
int gsf_lens_focus(int ch, int dir, int speed) { return -1;}
int gsf_lens_cal(int ch)      { return -1;}

#endif