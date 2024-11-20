#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "fw/comm/inc/serial.h"
#include "cfg.h"
#include "lens.h"
#include "mpp.h"
#include "fw/libaf/inc/af_ptz.h"

#define DEBUG 0
#define SUM6(buf) do{buf[6] = (buf[1]+buf[2]+buf[3]+buf[4]+buf[5])&0xFF;}while(0)
extern int dzoom_plus;

static int _sensor_flag = 0;
static int _flash_emmc = 0;

enum {
  LENS_TYPE_UART,  // 0: fixed-lens & uart-zoom
  LENS_TYPE_SONY,  // 1: sony-zoom
  LENS_TYPE_GPIO,  // 2: computar
  LENS_TYPE_HIVIEW,// 3: hiview-zoom
}; static int _lens_type = LENS_TYPE_UART;

enum {
  IRCUT_TYPE_V0_MANUAL, //0: manual(demo_board_v0)
  IRCUT_TYPE_V1_AUTO,   //1: auto(demo_board_v1)
  IRCUT_TYPE_V1_CDS,    //2: cds(demo_board_v1)
}; static int _ircut_type = IRCUT_TYPE_V0_MANUAL;

enum {
  IRCUT_CTL_EDGE,  // 0: [0_1-0_0 -> 1_0-0_0]
  IRCUT_CTL_LEVEL, // 1: [0_0 -> 1_1]
}; static int _ircut_ctl  = IRCUT_CTL_EDGE;

static gsf_lens_ini_t _ini;
static pthread_t serial_tid;
static int serial_fd = -1;
static int _zoomValue = 0, _cdsValue = 0, _dayNight = 0; // 0: day,  1: night;
static void* serial_task(void *parm);
static int pelco_d_write(char *cmd, int size);
static int af_zoom_value(int z);
static int ptz_led_set(int stat);
static int zoom2led(int z);

#if defined(GSF_CPU_3516d) || defined(GSF_CPU_3559)

#warning "3516d&3559 lens is implemented"

int flash_is_emmc()
{
  int ret = 0;
  char str[256];
  sprintf(str, "%s", "cat /proc/cmdline  | grep mmcblk");
  FILE* fd = popen(str, "r");
  if (fd && fgets(str, sizeof(str), fd))
  {
    ret = strstr(str, "mmcblk")?1:0;
    pclose(fd);
  }
  return ret;
}

int lens16x_lens_init(gsf_lens_ini_t *ini)
{
  _ini = *ini;
  _sensor_flag = strstr(_ini.sns, "imx")?1:0;
  _flash_emmc  = flash_is_emmc();
  _lens_type   = codec_ipc.lenscfg.lens;
  _ircut_type  = codec_ipc.lenscfg.ircut;
  _ircut_ctl   = (strstr(_ini.sns, "imx585") || strstr(_ini.sns, "imx482"))?IRCUT_CTL_LEVEL:IRCUT_CTL_EDGE;
  
  printf("_sensor_flag:%d, _flash_emmc:%d, _lens_type:%d, _ircut_type:%d\n", _sensor_flag, _flash_emmc, _lens_type, _ircut_type);
    
  if(!_sensor_flag)
  {  
    return 0;
  }
   
  if(_lens_type == LENS_TYPE_HIVIEW)
  {   
     //GPIO6_4 GPIO6_3 IRCUT
    system("himm 0x112F0018 0x0;himm 0x112F0014 0x0;echo 51 > /sys/class/gpio/export;echo 52 > /sys/class/gpio/export;");
    system("echo low > /sys/class/gpio/gpio51/direction;echo low > /sys/class/gpio/gpio52/direction");
    
    //night
    //system("echo 1 > /sys/class/gpio/gpio51/value;echo 0 > /sys/class/gpio/gpio52/value;sleep 0.1;echo 0 > /sys/class/gpio/gpio51/value");
    //day
    system("echo 0 > /sys/class/gpio/gpio51/value;echo 1 > /sys/class/gpio/gpio52/value;sleep 0.1;echo 0 > /sys/class/gpio/gpio52/value");  
   
  } 
    
  if(_flash_emmc)
  {
    return 0;
  }    

  //#10-3, 10-4; IRCUT1
  {
    system("himm 0x111F0030 0x401;himm 0x111F0034 0x401;echo 83 > /sys/class/gpio/export;echo 84 > /sys/class/gpio/export;");
    system("echo low > /sys/class/gpio/gpio83/direction;echo low > /sys/class/gpio/gpio84/direction");
    
    //night
    //system("echo 0 > /sys/class/gpio/gpio83/value;echo 1 > /sys/class/gpio/gpio84/value;sleep 0.1;echo 0 > /sys/class/gpio/gpio84/value");
    //day
    system("echo 1 > /sys/class/gpio/gpio83/value;echo 0 > /sys/class/gpio/gpio84/value;sleep 0.1;echo 0 > /sys/class/gpio/gpio83/value");
  }


  //#6-6 10-5; IRCUT2
  //emmc-ver for emmc-pinmux;
  {
    system("himm 0x10FF0018 0x0502;himm 0x111F0024 0x0600;echo 85 > /sys/class/gpio/export;echo 54 > /sys/class/gpio/export;");
    system("echo low > /sys/class/gpio/gpio85/direction;echo low > /sys/class/gpio/gpio54/direction");
    
    //night
    //system("echo 1 > /sys/class/gpio/gpio85/value;echo 0 > /sys/class/gpio/gpio54/value;sleep 0.1;echo 0 > /sys/class/gpio/gpio85/value");
    //day
    system("echo 0 > /sys/class/gpio/gpio85/value;echo 1 > /sys/class/gpio/gpio54/value;sleep 0.1;echo 0 > /sys/class/gpio/gpio54/value");
  }
  
  //#11-1, 11-2 IRlamp out 0: open,  1: close;
  {
    system("himm 0x12090000 0x00100be2;"); //gpio11_xxx;
    
    system("himm 0x112F00AC 0x0601;echo 89 > /sys/class/gpio/export;echo out > /sys/class/gpio/gpio89/direction;");
    system("echo 1 > /sys/class/gpio/gpio89/value;");
    
    system("himm 0x112F00B0 0x0601;echo 90 > /sys/class/gpio/export;echo out > /sys/class/gpio/gpio90/direction;");
    system("echo 1 > /sys/class/gpio/gpio90/value;");
  }
  
  //emmc-ver for 41908 iris
  {
    //#6-7 CDS in   0: night,  1: day;
    system("himm 0x111F0028 0x0600;echo 55 > /sys/class/gpio/export;echo in > /sys/class/gpio/gpio55/direction");
  }
  
  return 0;
}

static int ir_cb(int ViPipe, int dayNight, void* uargs)
{
  _dayNight = dayNight;
  
  if(_lens_type == LENS_TYPE_HIVIEW)
  {
    if(dayNight)
      system("echo 1 > /sys/class/gpio/gpio51/value;echo 0 > /sys/class/gpio/gpio52/value;sleep 0.1;echo 0 > /sys/class/gpio/gpio51/value");
    else
      system("echo 0 > /sys/class/gpio/gpio51/value;echo 1 > /sys/class/gpio/gpio52/value;sleep 0.1;echo 0 > /sys/class/gpio/gpio52/value");  
    
    if(dayNight)
    {
      zoom2led(_zoomValue); 
    }
    else 
    {
      ptz_led_set(0);
    }  
    return 0;
  }
  

  else if(_ircut_type)
  {
    if(_ircut_ctl == IRCUT_CTL_LEVEL)
    {
      if(dayNight) 
        system("echo 1 > /sys/class/gpio/gpio83/value;echo 1 > /sys/class/gpio/gpio84/value");
      else
        system("echo 0 > /sys/class/gpio/gpio83/value;echo 0 > /sys/class/gpio/gpio84/value");
    }
    else
    {
      if(dayNight) 
        system("echo 0 > /sys/class/gpio/gpio83/value;echo 1 > /sys/class/gpio/gpio84/value;sleep 0.1;echo 0 > /sys/class/gpio/gpio84/value");
      else
        system("echo 1 > /sys/class/gpio/gpio83/value;echo 0 > /sys/class/gpio/gpio84/value;sleep 0.1;echo 0 > /sys/class/gpio/gpio83/value");
    }
  }
  else
  {
    if(_ircut_ctl == IRCUT_CTL_LEVEL)
    {
      if(dayNight) 
        system("echo 1 > /sys/class/gpio/gpio27/value;echo 1 > /sys/class/gpio/gpio28/value");
      else
        system("echo 0 > /sys/class/gpio/gpio27/value;echo 0 > /sys/class/gpio/gpio28/value");
    }
    else
    {
      if(dayNight)
        system("echo 0 > /sys/class/gpio/gpio27/value;echo 1 > /sys/class/gpio/gpio28/value;sleep 0.1;echo 0 > /sys/class/gpio/gpio28/value;");
      else
        system("echo 1 > /sys/class/gpio/gpio27/value;echo 0 > /sys/class/gpio/gpio28/value;sleep 0.1;echo 0 > /sys/class/gpio/gpio27/value;");
    }  
  }
  
  printf("IR Lamp night:%d\n", dayNight);
  if(dayNight)
  {  
    system("echo 0 > /sys/class/gpio/gpio89/value;");
    system("echo 0 > /sys/class/gpio/gpio90/value;");
  }
  else
  {   
    system("echo 1 > /sys/class/gpio/gpio89/value;");
    system("echo 1 > /sys/class/gpio/gpio90/value;");
  }
  return 0;
}

//return  0: day, 1: night;
static int cds_cb(int ViPipe, void* uargs)
{
  if(_lens_type == LENS_TYPE_HIVIEW)
  {
    //printf("_cdsValue:%d\n", _cdsValue);
    return _cdsValue; 
  } 
   
  int value = 0;
 
  FILE* fp = fopen("/sys/class/gpio/gpio55/value", "rb+");
  if(fp)
  {
    unsigned char buf[10] = {0};
    fread(buf, sizeof(char), sizeof(buf) - 1, fp);
    value = (buf[0] == '0')?1:0;
    fclose(fp);
  }
  else 
  {
    printf("er: open fp:%p\n", fp);
  }

  //printf("night:%d\n", value);
  return value;
}

int lens16x_lens_ircut(int ch, int dayNight)
{
  if(!_sensor_flag)
  {  
    return -1;
  }
  ir_cb(ch, dayNight, NULL);
  gsf_mpp_isp_ctl(0, GSF_MPP_ISP_CTL_IR, (void*)dayNight);

  return 0;
}

int lens16x_uart_write(unsigned char *buf, int size)
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
  unsigned char buf[8];
  HI_U32 Fv = Fv1 + Fv2;

#if 0
  if(Gain <= 20*1000 && Fv1) //is daylight;
  {
    enum {
      SMT_NONE = 0,
      SMT_TIME = 1,
      SMT_QUIET= 2,
    }; static int smt = SMT_NONE;
    
    static HI_U32 _cnt = 0, _Fv1 = 0;
    static struct timespec _ts = {0,}, ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
  
    _ts = (_ts.tv_sec == 0)?ts:_ts;
    //printf("Gain:%d*1000, Fv1:%d*1000\n", Gain/1000, Fv1/1000);
    switch(smt)
    {
      case SMT_NONE:
          smt = (ts.tv_sec - _ts.tv_sec >= 10*60)?SMT_TIME:smt; //is 10 minutes
          if(smt == SMT_TIME)
          {
            printf("SMT_NONE => SMT_TIME Gain:%d\n", Gain);
          }
          break;
      case SMT_TIME:
          if(abs(Fv1-_Fv1) >= Gain*4) // is motion
          {
            printf("SMT_TIME => motion Gain:%d*1000, Fv1:%d*1000\n", Gain/1000, Fv1/1000);
            _cnt = 0;
          }
          else if(++_cnt >= 60) // is quiet
          {
            printf("SMT_TIME => SMT_QUIET Fv1:%d\n", Fv1);
            smt = SMT_QUIET;
          }
          _Fv1 = Fv1;
          break;
       case SMT_QUIET:
          {
            
            int ret = 0;
            unsigned char add[8] = {0xc5,0x00,0x00,0x20,0x00,0x00,0x00,0x5c}; SUM6(add);
            unsigned char sub[8] = {0xc5,0x00,0x00,0x40,0x00,0x00,0x00,0x5c}; SUM6(sub);
            unsigned char stop[8]= {0xc5,0x00,0x00,0x00,0x00,0x00,0x00,0x5c}; SUM6(stop);
            static unsigned char *buf = NULL;
            
            buf = (buf==add)?sub:add;
            ret = gsf_uart_write(buf, 8);
            usleep(10*1000);
            ret = gsf_uart_write(stop, 8);
            printf("SMT_QUIET => trigger focus\n");
            smt = SMT_NONE;
            _ts = ts;
          }
          break;
    }
  }
#endif

  //FV: A5+4字节AF数据（高位在前）+ 2字节增益(模拟增益)+ 1字节颜色（彩色/黑白）
  buf[0] = 0xa5;
  buf[1] = (Fv >> 24) & 0xFF;	
  buf[2] = (Fv >> 16) & 0xFF;
  buf[3] = (Fv >> 8) & 0xFF;
  buf[4] = Fv & 0xFF;

  buf[5] = (Gain >> 8) & 0xFF;
  buf[6] = Gain & 0xFF;

  buf[7] = _dayNight; //彩色是0 黑白是1

  int ret = gsf_uart_write(buf, 8);
  return 0;
}

int lens16x_lens_start(int ch, char *ttyAMA)
{
  if(_lens_type == LENS_TYPE_HIVIEW)
  {
    af_ptz_fv_t fvcb = {
      .get_vd = af_get_vd,
      .get_value = af_get_value,
    };
    
    af_ptz_cb_t ptzcb = {
      .uart_write = pelco_d_write,
      .zoom_value = af_zoom_value,
    };
    
    af_ptz_init(&fvcb, &ptzcb);
    //ptz;
    gsf_uart_open(ttyAMA, 115200);
  }  
  else if(_lens_type == LENS_TYPE_GPIO)
  {
    system("himm 0x114F0058 0; himm 0x114F005c 0");
    system("echo 30 > /sys/class/gpio/export; echo 31 > /sys/class/gpio/export");
    system("echo low > /sys/class/gpio/gpio30/direction; echo low > /sys/class/gpio/gpio31/direction");
    
    system("himm 0x114F0048 0; himm 0x114F0054 0");
    system("echo 26 > /sys/class/gpio/export; echo 29 > /sys/class/gpio/export");
    system("echo low > /sys/class/gpio/gpio26/direction; echo low > /sys/class/gpio/gpio29/direction");
    return -1;
  }  
  else if(_lens_type == LENS_TYPE_SONY)
  {
    gsf_uart_open(ttyAMA, 9600);
    return -1;
  }
  else
  {
    if(gsf_uart_open(ttyAMA, 115200) < 0)
    {  
      return -1;
    }
  }
  
  if(!_sensor_flag)
  {  
    return -1;
  }
  
  if(_ircut_type)
  {
    gsf_mpp_ir_t ir = {.cds = (_ircut_type == IRCUT_TYPE_V1_CDS)?cds_cb:NULL, .cb = ir_cb};
    gsf_mpp_isp_ctl(0, GSF_MPP_ISP_CTL_IR, &ir);
  }
  else if (_lens_type == LENS_TYPE_HIVIEW)
  {
    gsf_mpp_ir_t ir = {.cds = cds_cb, .cb = ir_cb};
    gsf_mpp_isp_ctl(0, GSF_MPP_ISP_CTL_IR, &ir);
  }
  
  gsf_mpp_af_t af = {
      .uargs = (void*)ch,
      .cb = (_lens_type == LENS_TYPE_HIVIEW)?NULL:af_cb,
  };
  
  if(ch < 0)
  {
  	return 0;
  }
  
  return gsf_mpp_af_start(&af);
}


int lens16x_lens_stop(int ch)
{
  int ret = 0;
  
  dzoom_plus = 0;
  
  if(_lens_type == LENS_TYPE_HIVIEW)
  {
    unsigned char buf[1] = {AF_PTZ_CMD_ZOOM_STOP};
    af_ptz_ctl(buf, 1);
  }
  else if(_lens_type == LENS_TYPE_GPIO)
  {
    system("echo 0 > /sys/class/gpio/gpio30/value;echo 0 > /sys/class/gpio/gpio31/value;"
          "echo 0 > /sys/class/gpio/gpio26/value;echo 0 > /sys/class/gpio/gpio29/value;");
    return 0;
  }
  else if(_lens_type == LENS_TYPE_SONY)
  {
    unsigned char buf[6] = {0x81, 0x01, 0x04, 0x07, 0x00, 0xFF};
    ret = gsf_uart_write(buf, 6);
    return 0;
  }
  else 
  {
    unsigned char buf[8] = {0xc5,0x00,0x00,0x00,0x00,0x00,0x00,0x5c}; SUM6(buf);
    ret = gsf_uart_write(buf, 8);
  }
  return 0;
}

int lens16x_lens_zoom(int ch,  int dir, int speed)
{
  int ret = 0;
  
  if(strstr(_ini.sns, "imx585") || strstr(_ini.sns, "imx482"))
  {
    dzoom_plus = (dir)?1:-1;
    return 0;
  }
  
  if(_lens_type == LENS_TYPE_HIVIEW)
  {
    unsigned char buf[1];
    buf[0] = (dir)?AF_PTZ_CMD_ZOOM_WIDE:AF_PTZ_CMD_ZOOM_TELE;
    af_ptz_ctl(buf, 1);
  }  
  else if(_lens_type == LENS_TYPE_GPIO)
  {
    if(dir)
      system("echo 1 > /sys/class/gpio/gpio30/value; echo 0 > /sys/class/gpio/gpio31/value;"
            "echo 1 > /sys/class/gpio/gpio29/value; echo 0 > /sys/class/gpio/gpio26/value;");
    else 
      system("echo 0 > /sys/class/gpio/gpio30/value; echo 1 > /sys/class/gpio/gpio31/value;"
            "echo 0 > /sys/class/gpio/gpio29/value; echo 1 > /sys/class/gpio/gpio26/value;");
    return 0;
  }
  else if(_lens_type == LENS_TYPE_SONY)
  {
    unsigned char add[6] = {0x81, 0x01, 0x04, 0x07, 0x25, 0xFF}; //buf[4] = 0x20 | (speed&0x0F);
    unsigned char sub[6] = {0x81, 0x01, 0x04, 0x07, 0x35, 0xFF}; //buf[4] = 0x30 | (speed&0x0F);
    unsigned char *buf = (dir)?add:sub;
    ret = gsf_uart_write(buf, 6);
    return 0;
  }
  else 
  {
  	// 派尔高D协议开始字节FF换成C5,最后补充一个字节5C组成8个字节
    unsigned char add[8] = {0xc5,0x00,0x00,0x20,0x00,0x00,0x00,0x5c}; SUM6(add);
    unsigned char sub[8] = {0xc5,0x00,0x00,0x40,0x00,0x00,0x00,0x5c}; SUM6(sub);
    unsigned char *buf = (dir)?add:sub;
    ret = gsf_uart_write(buf, 8);
  }
  return 0;
}

int lens16x_lens_focus(int ch, int dir, int speed)
{
  int ret = 0;
  if(_lens_type == LENS_TYPE_HIVIEW)
  {
    ;
  }  
  else if(_lens_type == LENS_TYPE_GPIO)
  {
    if(dir)
      system("echo 1 > /sys/class/gpio/gpio29/value; echo 0 > /sys/class/gpio/gpio26/value;");
    else 
      system("echo 0 > /sys/class/gpio/gpio29/value; echo 1 > /sys/class/gpio/gpio26/value;");
    return 0;
  }
  else if(_lens_type == LENS_TYPE_SONY)
  {
    unsigned char add[6] = {0x81, 0x01, 0x04, 0x08, 0x25, 0xFF};
    unsigned char sub[6] = {0x81, 0x01, 0x04, 0x08, 0x35, 0xFF};
    unsigned char *buf = (dir)?add:sub;
    ret = gsf_uart_write(buf, 6);
    return 0;
  }
  else 
  {
  	// 派尔高D协议开始字节FF换成C5,最后补充一个字节5C组成8个字节
    unsigned char add[8] = {0xc5,0x00,0x01,0x00,0x00,0x00,0x00,0x5c}; SUM6(add);
    unsigned char sub[8] = {0xc5,0x00,0x00,0x80,0x00,0x00,0x00,0x5c}; SUM6(sub);
    unsigned char *buf = (dir)?add:sub;
    ret = gsf_uart_write(buf, 8);
  }
  return 0;
}

//#define SUM6(buf) do{buf[6] = (buf[1]+buf[2]+buf[3]+buf[4]+buf[5])&0xFF;}while(0)

int lens16x_lens_cal(int ch)
{
	// lens calibration
  if(_lens_type == LENS_TYPE_HIVIEW)
  {
    unsigned char buf[1] = {AF_PTZ_CMD_LENS_CORRECT};
    af_ptz_ctl(buf, 1);
  }  
  else
  {
    unsigned char buf[8] = {0xc5,0x00,0x00,0x07,0x00,250,0x00,0x5c}; SUM6(buf);
    int ret = gsf_uart_write(buf, 8);
    usleep(100*1000);
    ret |= gsf_uart_write(buf, 8);
  }
  return 0;
}

int lens16x_uart_open(char *ttyAMA, int baudrate)
{
  if(strstr(ttyAMA, "ttyAMA4"))
    system("himm 0x111F0000 2;himm 0x111F0004 2;"); //UART4 MUX
  else if(strstr(ttyAMA, "ttyAMA2"))
    system("himm 0x114F0058 3; himm 0x114F005c 3;"); //UART2 MUX
  else if(strstr(ttyAMA, "ttyAMA3"))
    system("himm 0x114F0000 2; himm 0x114F0004 2;"); //UART3 MUX
  else 
    return -1;
    
  if(!ttyAMA || strlen(ttyAMA) < 1)
    return -1;
  //O_NDELAY blocking read;
  serial_fd = open(ttyAMA, O_RDWR | O_NOCTTY /*| O_NDELAY*/);
  if (serial_fd < 0)
  {
      return -2;
  }
	// set serial param
  if(serial_set_param(serial_fd, baudrate, 0, 1, 8) < 0)
  {
    return -3;
  }
  return pthread_create(&serial_tid, NULL, serial_task, (void*)NULL);
}

static void* serial_task_ldm(void *parm)
{
  int ret = 0;
  unsigned short cmd = 0;
  unsigned char buf[4+256] = {0};
  while(serial_fd > 0)
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
    unsigned char bufstr[sizeof(buf)*3] = {0};
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


static int ptz_cds_query()
{
  unsigned char buf[256] = {0};
  
  //0x3a  0xff  0xb1 0x00 0xa1 0x00 0x09 type  Para1  Para2
  //Para3 Para4 Para5 Para6 Para7 Para8 pend1 pend2 chksum 0x5c
  buf[0] = 0x3a;
  buf[1] = 0xff;
  buf[2] = 0xb1;
  buf[3] = 0x00;
  buf[4] = 0xa1;
  buf[5] = 0x00;
  buf[6] = 0x09; 
  buf[7] = 0x9e; //query
  buf[8] = 0x05; //cds;
  //Para2~Para8
  buf[9] = 0x00; //0xd1: H/L, 0xad: VALUE;
  buf[10] = 0x00;//0/1[H/L], 0-255[VALUE]
  buf[11] = 0x00;
  buf[12] = 0x00;
  buf[13] = 0x00;
  buf[14] = 0x00;
  buf[15] = 0x00;
  buf[16] = 0x00;
  buf[17] = 0x00;
  buf[18] = buf[1]+buf[2]+buf[3]+buf[4]+buf[5]+buf[6]+buf[7]+buf[8]+buf[9]+buf[10]+buf[11]+buf[12]+buf[13]+buf[14]+buf[15];
  buf[19] = 0x5c;
  return gsf_uart_write(buf, 20);
}


static int ptz_cds_report(int stat)
{
  unsigned char buf[256] = {0};
  
  //0x3a  0xff  0xb1 0x00 0xa1 0x00 0x09 type  Para1  Para2
  //Para3 Para4 Para5 Para6 Para7 Para8 pend1 pend2 chksum 0x5c
  buf[0] = 0x3a;
  buf[1] = 0xff;
  buf[2] = 0xb1;
  buf[3] = 0x00;
  buf[4] = 0xa1;
  buf[5] = 0x00;
  buf[6] = 0x09; 
  buf[7] = 0x5e; //set
  buf[8] = 0x05; //cds;
  buf[9] = 0xad; //0xd1: H/L, 0xad: VALUE;
  buf[10] = 0x00;//0/1[H/L], 0-255[VALUE]
  buf[11] = 0x00;
  buf[12] = 0x00;
  buf[13] = 0x00;
  buf[14] = 0x00;
  buf[15] = 0x00;
  buf[16] = 0x00;
  buf[17] = 0x00;
  buf[18] = buf[1]+buf[2]+buf[3]+buf[4]+buf[5]+buf[6]+buf[7]+buf[8]+buf[9]+buf[10]+buf[11]+buf[12]+buf[13]+buf[14]+buf[15];
  buf[19] = 0x5c;
  return gsf_uart_write(buf, 20);
}


static int ptz_led_set(int stat)
{
  unsigned char buf[256] = {0};
  
  //0x3a  0xff  0xb1 0x00 0xa3 0x00 0x09 type  Para1  Para2
  //Para3 Para4 Para5 Para6 Para7 待用 pend1 pend2 chksum 0x5c 
  buf[0] = 0x3a;
  buf[1] = 0xff;
  buf[2] = 0xb1;
  buf[3] = 0x00;
  buf[4] = 0xa3;
  buf[5] = 0x00;
  buf[6] = 0x09; 
  buf[7] = 0x5e; //set
  buf[8] = 0x1e; //ir-led;
  //para2-para7 (0-255)
  switch(stat)
  {
    case 1:
      buf[9] =  0xff; //近
      break;
    case 2:
      buf[9] =  0xff; //近
      buf[10] = 0xff; //中
      break;
    case 3:
      buf[10] = 0xff; //中
      buf[11] = 0xff; //远1
      break;
    case 4:
      buf[11] = 0xff; //远1
      buf[12] = 0xff; //远2
      break;  
  }
  buf[13] = 0x00; //远3
  buf[14] = 0x00; //远4
  buf[15] = 0x00;
  buf[16] = 0x00;
  buf[17] = 0x00;
  buf[18] = buf[1]+buf[2]+buf[3]+buf[4]+buf[5]+buf[6]+buf[7]+buf[8]+buf[9]+buf[10]+buf[11]+buf[12]+buf[13]+buf[14]+buf[15];
  buf[19] = 0x5c;
  return gsf_uart_write(buf, 20);
}


static int ptz_pos_query()
{
  unsigned char buf[256] = {0};
  
  //0x3a 0xff 0xb1 0x00 0x82 0x00 0x09 待用   xH    xL
  //yH   yL  待用 待用 待用 待用 pend1 pend2 chksum 0x5c
  buf[0] = 0x3a;
  buf[1] = 0xff;
  buf[2] = 0xb1;
  buf[3] = 0x00;
  buf[4] = 0x82;
  buf[5] = 0x00;
  buf[6] = 0x09; 
  buf[7] = 0x00;
  buf[8] = 0x00; //xH; 0-36000 (0.01 Angle *100)
  buf[9] = 0x00; //xL;
  buf[10] = 0x00;//yH; 27000-36000 (0.01 Angle *100)
  buf[11] = 0x00;//yL;
  buf[12] = 0x00;
  buf[13] = 0x00;
  buf[14] = 0x00;
  buf[15] = 0x00;
  buf[16] = 0x00;
  buf[17] = 0x00;
  buf[18] = buf[1]+buf[2]+buf[3]+buf[4]+buf[5]+buf[6]+buf[7]+buf[8]+buf[9]+buf[10]+buf[11]+buf[12]+buf[13]+buf[14]+buf[15];
  buf[19] = 0x5c;
  return gsf_uart_write(buf, 20);
  return 0;
}

static int ptz_pos_set(int x, int y)
{
  unsigned char buf[256] = {0};
  
  //0x3a 0xff 0xb1 0x00 0x81  0x00 0x09  待用  xH    xL
  //yH   yL   xsp  ysp  待用 待用 pend1 pend2 chksum 0x5c
  buf[0] = 0x3a;
  buf[1] = 0xff;
  buf[2] = 0xb1;
  buf[3] = 0x00;
  buf[4] = 0x81;
  buf[5] = 0x00;
  buf[6] = 0x09; 
  buf[7] = 0x00;
  buf[8] = (x>>8)&0xff; //xH; 0-36000 (0.01 Angle *100)
  buf[9] = (x>>0)&0xff; //xL;
  buf[10] = (y>>8)&0xff;//yH; 27000-36000 (0.01 Angle *100)
  buf[11] = (y>>0)&0xff;//yL;
  buf[12] = 0x00;
  buf[13] = 0x00;
  buf[14] = 0x00;
  buf[15] = 0x00;
  buf[16] = 0x00;
  buf[17] = 0x00;
  buf[18] = buf[1]+buf[2]+buf[3]+buf[4]+buf[5]+buf[6]+buf[7]+buf[8]+buf[9]+buf[10]+buf[11]+buf[12]+buf[13]+buf[14]+buf[15];
  buf[19] = 0x5c;
  return gsf_uart_write(buf, 20);
  return 0;
  
  
  
  return 0;
}

static int ptz_pos_report(int stat)
{
  unsigned char buf[256] = {0};
  
  //0x3a  0xff  0xb1 0x00 0x8a 0x00 0x09 type  Para1  Para2
  //Para3 待用 待用 待用 待用 待用 pend1 pend2 chksum 0x5c  
  buf[0] = 0x3a;
  buf[1] = 0xff;
  buf[2] = 0xb1;
  buf[3] = 0x00;
  buf[4] = 0x8a;
  buf[5] = 0x00;
  buf[6] = 0x09; 
  buf[7] = 0x5e; //set
  buf[8] = 0x01; //net board;
  buf[9] = stat; //0: disable, 1: stop once, 2: runing;
  buf[10] = 0x28; //40ms
  buf[11] = 0x00;
  buf[12] = 0x00;
  buf[13] = 0x00;
  buf[14] = 0x00;
  buf[15] = 0x00;
  buf[16] = 0x00;
  buf[17] = 0x00;
  buf[18] = buf[1]+buf[2]+buf[3]+buf[4]+buf[5]+buf[6]+buf[7]+buf[8]+buf[9]+buf[10]+buf[11]+buf[12]+buf[13]+buf[14]+buf[15];
  buf[19] = 0x5c;
  return gsf_uart_write(buf, 20);
}


static void* serial_task_ptz(void *parm)
{
  int ret = 0;
  unsigned short cmd = 0;
  unsigned char buf[256] = {0};
  
  #if 0
  //led control;
  ptz_led_set(1); sleep(1);
  ptz_led_set(2); sleep(1);
  ptz_led_set(3); sleep(1);
  ptz_led_set(4); sleep(1);
  ptz_led_set(0);
  
  //pos&cds report;
  ptz_pos_report(0x01);
  ptz_cds_report(0x01);
  
  //cds query;
  ptz_cds_query();
  #endif

  //lenght = 20 bytes;
  // byte1  byte2   byte3  byte4  byte5  byte6  byte7  byte8  byte9   byte10
  // 0x3a    0xff   0xb1   cmd1   cmd2   0x00   0x09   data1  data2   data3
  // byte11 byte12 byte13  byte14 byte15 byte16 byte17 byte18 byte19  byte20
  // data4  data5   data6  data7  data8  data9  pend1  pend2  chksum  0x5c
  // chksum: (byte2+byte3+…+byte16)%256;

  while(serial_fd > 0)
  {
HEAD:    
    buf[0] = buf[1] = buf[2] = 0;
    ret = read(serial_fd, buf, 3);
    
    if(ret < 0 || buf[0] != 0x3a || buf[1] != 0xff || buf[2] != 0xb1)
    {
      #if DEBUG
      printf("err read ret:%d, buf[%02X %02X %02X]\n"
          , ret, buf[0], buf[1], buf[2]);
      #endif
      
      usleep(10);
      continue;
    }
    
    while(ret < 20)
    {
      int r = read(serial_fd, &buf[ret], 20-ret);
      if(r < 0)
        goto HEAD;
       else if (r == 0)
        usleep(10); 
        
       ret += r;
    }
    
    #if DEBUG
    int i = 0;
    unsigned char bufstr[sizeof(buf)*3] = {0};
    for(i = 0; i < ret && i < sizeof(buf); i++)
    {
      char token[4];
      sprintf(token, "%02X ", buf[i]);
      strcat(bufstr, token);
    }
    printf("ok read ret:%d, buf[%s]\n", ret, bufstr);
    #endif
    
    if(buf[4] == 0xA1 && buf[7] == 0xAC && buf[8] == 0x05 && buf[9] == 0xD1)
    {
    //night;
    //[3A FF B1 00 A1 00 09 AC 05 D1 01 00 00 00 00 00 00 00 DD 5C ]

    //day;
    //[3A FF B1 00 A1 00 09 AC 05 D1 00 00 00 00 00 00 00 00 DC 5C ]
      _cdsValue = buf[10];
    }
    
    if(0)
    {  
     //version;
     //[3A FF B1 00 C2 00 09 AC 12 14 08 10 E8 17 00 00 00 00 64 5C ]
  
    //pos;
    //[3A FF B1 00 82 00 09 00 00 00 8A AC 00 00 00 00 00 00 71 5C ]
    }
  }
  return NULL;
}


static void* serial_task(void *parm)
{
  if(_lens_type == LENS_TYPE_HIVIEW)
  {
    return serial_task_ptz(parm);
  }
  else 
  {
    return serial_task_ldm(parm);
  }  
}


int lens16x_lens_ptz(int ch, gsf_lens_t *lens)
{
  if(_lens_type != 3)
    return 0;
    
  unsigned char buf[4] = {0};
  printf("cmd:%d\n", lens->cmd);
  switch(lens->cmd)
  { 
    case GSF_PTZ_STOP:
    {
      buf[0] = AF_PTZ_CMD_STOP;
      buf[1] = lens->cmd;
      af_ptz_ctl(buf, 2);
    }  
    break;
    case GSF_PTZ_UP:
    case GSF_PTZ_DOWN:
    case GSF_PTZ_LEFT:
    case GSF_PTZ_RIGHT:
    {
      buf[0] = AF_PTZ_CMD_MOVE;
      buf[1] = lens->cmd;
      buf[2] = lens->arg1;
      buf[3] = lens->arg2;
      af_ptz_ctl(buf, 4);
    }
    break;
  }
  return 0;
}

static int zoom2led(int z)
{
  //0-43
  if(z <= 4)
    ptz_led_set(1);
  else if (z <= 8)
    ptz_led_set(2);
  else if (z <= 16)
    ptz_led_set(3);
  else
    ptz_led_set(4);
  return 0;    
}


static int af_zoom_value(int z)
{
  //printf("z: %d\n", z);
  _zoomValue = z;
  if(_dayNight)
  {
    zoom2led(_zoomValue);
  }
  return 0;
}


static int pelco_d_write(char *cmd, int size)
{
  printf("cmd[%d], size:%d\n", cmd[0], size);
  switch(cmd[0])
  {
    case GSF_PTZ_STOP:
    {
      unsigned char buf[7] = {0xff,0xff,0x00,0x00,0x00,0x00,0x00};
      buf[6] = (buf[1]+buf[2]+buf[3]+buf[4]+buf[5])&0xFF;
      gsf_uart_write(buf, sizeof(buf));
    }  
    break;
    case GSF_PTZ_UP:
    {
      unsigned char buf[7] = {0xff,0xff,0x00,0x08,0x00,0x31,0x00};
      buf[6] = (buf[1]+buf[2]+buf[3]+buf[4]+buf[5])&0xFF;
      gsf_uart_write(buf, sizeof(buf));
    }
    break;    
    case GSF_PTZ_DOWN:
    {
      unsigned char buf[7] = {0xff,0xff,0x00,0x10,0x00,0x31,0x00};
      buf[6] = (buf[1]+buf[2]+buf[3]+buf[4]+buf[5])&0xFF;
      gsf_uart_write(buf, sizeof(buf));
    }
    break;  
    case GSF_PTZ_LEFT:
    {
      unsigned char buf[7] = {0xff,0xff,0x00,0x04,0x31,0x00,0x00};
      buf[6] = (buf[1]+buf[2]+buf[3]+buf[4]+buf[5])&0xFF;
      gsf_uart_write(buf, sizeof(buf));
    }
    break;    
    case GSF_PTZ_RIGHT:
    {
      unsigned char buf[7] = {0xff,0xff,0x00,0x02,0x31,0x00,0x00};
      buf[6] = (buf[1]+buf[2]+buf[3]+buf[4]+buf[5])&0xFF;
      gsf_uart_write(buf, sizeof(buf));
    }
    break;
  }
  return 0;
}

int (*gsf_lens_start)(int ch, char *ttyAMA) = lens16x_lens_start;
int (*gsf_lens_ircut)(int ch, int dayNight) = lens16x_lens_ircut;
int (*gsf_lens_zoom)(int ch,  int dir, int speed) = lens16x_lens_zoom;
int (*gsf_lens_focus)(int ch, int dir, int speed) = lens16x_lens_focus;
int (*gsf_lens_stop)(int ch) = lens16x_lens_stop;
int (*gsf_lens_cal)(int ch) = lens16x_lens_cal;
int (*gsf_uart_open)(char *ttyAMA, int baudrate) = lens16x_uart_open;
int (*gsf_uart_write)(unsigned char *buf, int size) = lens16x_uart_write;
int (*gsf_lens_init)(gsf_lens_ini_t *ini) = lens16x_lens_init;
int (*gsf_lens_ptz)(int ch, gsf_lens_t *lens) = lens16x_lens_ptz;

#endif