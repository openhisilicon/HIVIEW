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
static int _flash_emmc  = 0;

enum {
  LENS_TYPE_UART,  // 0: fixed-lens & uart-zoom
  LENS_TYPE_SONY,  // 1: sony-zoom
  LENS_TYPE_GPIO,  // 2: computar
  LENS_TYPE_HIVIEW,// 3: hiview-zoom
}; static int _lens_type = LENS_TYPE_UART;

enum {
  IRCUT_TYPE_MANUAL, //0: manual
  IRCUT_TYPE_AUTO,   //1: auto
  IRCUT_TYPE_CDS,    //2: cds
}; static int _ircut_type = IRCUT_TYPE_MANUAL;

enum {
  IRCUT_CTL_EDGE,  // 0: [0_1-0_0 -> 1_0-0_0]
  IRCUT_CTL_LEVEL, // 1: [0_0 -> 1_1]
}; static int _ircut_ctl  = IRCUT_CTL_EDGE;

enum {
  LAMP_IR = 0,
  LAMP_WHITE = 1,
}; static int _lamp_type = LAMP_IR;


enum {
  IRCUT_REV_0,
  IRCUT_REV_1,
}; static int _ircut_rev = IRCUT_REV_0;


static gsf_lens_ini_t _ini;
static pthread_t serial_tid;
static int serial_fd = -1;
static int _zoomValue = 0, _cdsValue = 0, _dayNight = 0; // 0: day,  1: night;
static void* serial_task(void *parm);
static int pelco_d_write(char *cmd, int size);

static int ptz_led_set(int stat);


#if defined(GSF_CPU_3516c)

#warning "3516cv610 lens is implemented"

//#9-2, 9-3; IRCUT1
#define IRCUT0_INIT() do {\
    system("bspmm 0x17940058 0x0;bspmm 0x1794005C 0x0;echo 74 > /sys/class/gpio/export;echo 75 > /sys/class/gpio/export;");\
    system("echo low > /sys/class/gpio/gpio74/direction;echo low > /sys/class/gpio/gpio75/direction");\
  }while(0)

#define __IRCUT0_DAY(ctl) do {\
    if(ctl == IRCUT_CTL_LEVEL)\
      system("echo 0 > /sys/class/gpio/gpio74/value;echo 0 > /sys/class/gpio/gpio75/value;");\
    else \
      system("echo 0 > /sys/class/gpio/gpio74/value;echo 1 > /sys/class/gpio/gpio75/value;sleep 0.1;echo 0 > /sys/class/gpio/gpio75/value");\
  }while(0)

#define __IRCUT0_NIGHT(ctl) do {\
    if(ctl == IRCUT_CTL_LEVEL)\
      system("echo 1 > /sys/class/gpio/gpio74/value;echo 1 > /sys/class/gpio/gpio75/value;");\
    else \
      system("echo 1 > /sys/class/gpio/gpio74/value;echo 0 > /sys/class/gpio/gpio75/value;sleep 0.1;echo 0 > /sys/class/gpio/gpio74/value");\
  }while(0)

//#; IRCUT2
#define IRCUT1_INIT() do {\
  }while(0)

#define __IRCUT1_DAY(ctl) do {\
  }while(0)

#define __IRCUT1_NIGHT(ctl) do {\
  }while(0)
  
//ircut reversed
#define IRCUT0_DAY(ctl) do{ \
    if(_ircut_rev) __IRCUT0_NIGHT(ctl); else __IRCUT0_DAY(ctl);\
  }while(0)

#define IRCUT0_NIGHT(ctl) do{ \
    if(_ircut_rev) __IRCUT0_DAY(ctl); else __IRCUT0_NIGHT(ctl);\
  }while(0)

#define IRCUT1_DAY(ctl) do{ \
    if(_ircut_rev) __IRCUT1_NIGHT(ctl); else __IRCUT1_DAY(ctl);\
  }while(0)

#define IRCUT1_NIGHT(ctl) do{ \
    if(_ircut_rev) __IRCUT1_DAY(ctl); else __IRCUT1_NIGHT(ctl);\
  }while(0)

/*
  bspmm 0x17940090 0x0; #0x0: GPIO7_0 0x3: SPI0_CSN0 0x5: I2C1_SDA 0x7: SENSOR0_HS                 
  bspmm 0x17940094 0x0; #0x0: GPIO7_1 0x3: SPI0_SDI 0x4: SENSOR0_RSTN 0x5: I2C1_SCL 0x7: SENSOR0_VS 
  echo 56 > /sys/class/gpio/export;
  echo 57 > /sys/class/gpio/export;
  echo high > /sys/class/gpio/gpio56/direction;
  echo high > /sys/class/gpio/gpio57/direction;
*/

//#7-0  white-lamp
//#7-1-ir-lamp
#define LAMP0_INIT() do {\
    system("bspmm 0x17940094 0x0;echo 57 > /sys/class/gpio/export");\
    system("echo high > /sys/class/gpio/gpio57/direction");\
  }while(0)

#define LAMP0_DAY() do {\
      system("echo 1 > /sys/class/gpio/gpio57/value");\
  }while(0)

#define LAMP0_NIGHT() do {\
      system("echo 0 > /sys/class/gpio/gpio57/value");\
  }while(0)


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

int lens610_lens_init(gsf_lens_ini_t *ini)
{
  _ini = *ini;
  _sensor_flag = (strstr(_ini.sns, "imx") ||strstr(_ini.sns, "os"))?1:0;
  _flash_emmc  = flash_is_emmc();
  _lens_type   = codec_ipc.lenscfg.lens;
  _ircut_type  = codec_ipc.lenscfg.ircut;
  _ircut_ctl   = (strstr(_ini.sns, "imx585") || strstr(_ini.sns, "imx482"))?IRCUT_CTL_LEVEL:IRCUT_CTL_EDGE;
  _ircut_rev   = codec_ipc.lenscfg.ircut_rev;
  
  printf("_sensor_flag:%d, _flash_emmc:%d, _lens_type:%d, _ircut_type:%d, _ircut_rev:%d\n"
        , _sensor_flag, _flash_emmc, _lens_type, _ircut_type, _ircut_rev);
    
  if(!_sensor_flag)
  {
    return 0;
  }
   
  if(_lens_type == LENS_TYPE_HIVIEW)
  {
    //IRCUT && DAY
  } 
    
  IRCUT0_INIT();
  IRCUT0_DAY(_ircut_ctl);

  IRCUT1_INIT();
  IRCUT1_DAY(_ircut_ctl);

  LAMP0_INIT();

  return 0;
}

static int ir_cb(int ViPipe, int dayNight, void* uargs)
{
  _dayNight = dayNight;
  
  if(_lens_type == LENS_TYPE_HIVIEW)
  { 
    return 0;
  }
  else
  {
    if(dayNight)
    {   
      IRCUT0_NIGHT(_ircut_ctl);
      LAMP0_NIGHT();
    }
    else
    {  
      IRCUT0_DAY(_ircut_ctl);
      LAMP0_DAY();
    } 
  }
  
  printf("ViPipe:%d, IR night:%d\n", ViPipe, dayNight);
  return 0;
}

//return  0: day, 1: night;
static int cds_cb(int ViPipe, void* uargs)
{
  if(_lens_type == LENS_TYPE_HIVIEW)
  {
    return _cdsValue; 
  } 
   
  int value = 0;
 
  FILE* fp = fopen("/sys/class/gpio/gpioXX/value", "rb+");
  if(fp)
  {
    unsigned char buf[10] = {0};
    fread(buf, sizeof(char), sizeof(buf) - 1, fp);
    value = (buf[0] == '0')?1:0;
    fclose(fp);
  }
  else 
  {
    printf("err: open fp:%p\n", fp);
  }

  printf("ViPipe:%d, CDS night:%d\n", ViPipe, value);
  return value;
}

int lens610_lens_ircut(int ch, int dayNight)
{
  if(!_sensor_flag)
  {  
    return -1;
  }
  ir_cb(ch, dayNight, NULL);
  if(_lamp_type == LAMP_IR)
  {  
    gsf_mpp_isp_ctl(0, GSF_MPP_ISP_CTL_IR, (void*)dayNight);
  }
  return 0;
}

int lens610_uart_write(unsigned char *buf, int size)
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

  //FV: A5+4字节AF数据（高位在前）+ 2字节增益(模拟增益)+ 1字节颜色（彩色/黑白）
  buf[0] = 0xa5;
  buf[1] = (Fv >> 24) & 0xFF;	
  buf[2] = (Fv >> 16) & 0xFF;
  buf[3] = (Fv >> 8) & 0xFF;
  buf[4] = Fv & 0xFF;

  buf[5] = (Gain >> 8) & 0xFF;
  buf[6] = Gain & 0xFF;

  buf[7] = _dayNight; //彩色是0 黑白是1

  int ret = lens610_uart_write(buf, 8);
  return 0;
}

int lens610_lens_start(int ch, char *ttyAMA)
{
  int ret = 0;
  
  if(_lens_type == LENS_TYPE_HIVIEW)
  {
    ;
  }  
  else if(_lens_type == LENS_TYPE_GPIO)
  {
    ;
    return -1;
  }  
  else if(_lens_type == LENS_TYPE_SONY)
  {
    lens610_uart_open(ttyAMA, 9600);
    return -1;
  }
  else
  {
    if(lens610_uart_open(ttyAMA, 115200) < 0)
    {  
      printf("open error ttyAMA:[%s]\n", ttyAMA);
    }
  }
  
  printf("%s => _sensor_flag:%d\n", __func__, _sensor_flag);
  if(!_sensor_flag)
  {  
    return -1;
  }
  
  if(_ircut_type)
  {
    gsf_mpp_ir_t ir = {.cds = (_ircut_type == IRCUT_TYPE_CDS)?cds_cb:NULL, .cb = ir_cb};
    ret = gsf_mpp_isp_ctl(0, GSF_MPP_ISP_CTL_IR, &ir);
  }
  else if (_lens_type == LENS_TYPE_HIVIEW)
  {
    gsf_mpp_ir_t ir = {.cds = cds_cb, .cb = ir_cb};
    ret = gsf_mpp_isp_ctl(0, GSF_MPP_ISP_CTL_IR, &ir);
  }
  
  gsf_mpp_af_t af = {
      .ViPipe = ch,
      .uargs = (void*)ch,
      .cb = (_lens_type == LENS_TYPE_HIVIEW)?NULL:af_cb,
  };
  
  if(ch < 0)
  {
  	return 0;
  }
  printf("%s => gsf_mpp_af_start(%d)\n", __func__, ch);
  return gsf_mpp_af_start(&af);
}


int lens610_lens_stop(int ch)
{
  int ret = 0;
  
  dzoom_plus = 0;
  
  if(_lens_type == LENS_TYPE_HIVIEW)
  {
    ;
  }
  else if(_lens_type == LENS_TYPE_GPIO)
  {
    ;
    return 0;
  }
  else if(_lens_type == LENS_TYPE_SONY)
  {
    unsigned char buf[6] = {0x81, 0x01, 0x04, 0x07, 0x00, 0xFF};
    ret = lens610_uart_write(buf, 6);
    return 0;
  }
  else 
  {
    unsigned char buf[8] = {0xc5,0x00,0x00,0x00,0x00,0x00,0x00,0x5c}; SUM6(buf);
    ret = lens610_uart_write(buf, 8);
  }
  return 0;
}

int lens610_lens_zoom(int ch,  int dir, int speed)
{
  int ret = 0;
  
  if(strstr(_ini.sns, "imx585") || strstr(_ini.sns, "imx482"))
  {
    dzoom_plus = (dir)?1:-1;
    return 0;
  }
  
  if(_lens_type == LENS_TYPE_HIVIEW)
  {
    ;
  }  
  else if(_lens_type == LENS_TYPE_GPIO)
  {
    ;
    return 0;
  }
  else if(_lens_type == LENS_TYPE_SONY)
  {
    unsigned char add[6] = {0x81, 0x01, 0x04, 0x07, 0x25, 0xFF}; //buf[4] = 0x20 | (speed&0x0F);
    unsigned char sub[6] = {0x81, 0x01, 0x04, 0x07, 0x35, 0xFF}; //buf[4] = 0x30 | (speed&0x0F);
    unsigned char *buf = (dir)?add:sub;
    ret = lens610_uart_write(buf, 6);
    return 0;
  }
  else 
  {
  	// 派尔高D协议开始字节FF换成C5,最后补充一个字节5C组成8个字节
    unsigned char add[8] = {0xc5,0x00,0x00,0x20,0x00,0x00,0x00,0x5c}; SUM6(add);
    unsigned char sub[8] = {0xc5,0x00,0x00,0x40,0x00,0x00,0x00,0x5c}; SUM6(sub);
    unsigned char *buf = (dir)?add:sub;
    ret = lens610_uart_write(buf, 8);
  }
  return 0;
}

int lens610_lens_focus(int ch, int dir, int speed)
{
  int ret = 0;
  if(_lens_type == LENS_TYPE_HIVIEW)
  {
    ;
  }  
  else if(_lens_type == LENS_TYPE_GPIO)
  {
    ;
    return 0;
  }
  else if(_lens_type == LENS_TYPE_SONY)
  {
    unsigned char add[6] = {0x81, 0x01, 0x04, 0x08, 0x25, 0xFF};
    unsigned char sub[6] = {0x81, 0x01, 0x04, 0x08, 0x35, 0xFF};
    unsigned char *buf = (dir)?add:sub;
    ret = lens610_uart_write(buf, 6);
    return 0;
  }
  else 
  {
  	// 派尔高D协议开始字节FF换成C5,最后补充一个字节5C组成8个字节
    unsigned char add[8] = {0xc5,0x00,0x01,0x00,0x00,0x00,0x00,0x5c}; SUM6(add);
    unsigned char sub[8] = {0xc5,0x00,0x00,0x80,0x00,0x00,0x00,0x5c}; SUM6(sub);
    unsigned char *buf = (dir)?add:sub;
    ret = lens610_uart_write(buf, 8);
  }
  return 0;
}

int lens610_lens_cal(int ch)
{
	// lens calibration
  if(_lens_type == LENS_TYPE_HIVIEW)
  {
    ;
  }  
  else
  {
    unsigned char buf[8] = {0xc5,0x00,0x00,0x07,0x00,250,0x00,0x5c}; SUM6(buf);
    int ret = lens610_uart_write(buf, 8);
    usleep(100*1000);
    ret |= lens610_uart_write(buf, 8);
  }
  return 0;
}

int lens610_uart_open(char *ttyAMA, int baudrate)
{
  if(strstr(ttyAMA, "ttyAMAxx"))
    ;//system("bspmm 0x0102600E0 2; bspmm 0x0102600E4 2;"); //UART3 MUX
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

static void* serial_task_ptz(void *parm)
{
  int ret = 0;
  unsigned short cmd = 0;
  unsigned char buf[256] = {0};
  
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


int lens610_lens_ptz(int ch, gsf_lens_t *lens)
{
  if(_lens_type != LENS_TYPE_HIVIEW)
    return 0;
    
  unsigned char buf[4] = {0};
  printf("cmd:%d\n", lens->cmd);
  
  switch(lens->cmd)
  { 
    case GSF_PTZ_STOP: 
    break;
    case GSF_PTZ_UP:
    case GSF_PTZ_DOWN:
    case GSF_PTZ_LEFT:
    case GSF_PTZ_RIGHT:
    break;
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
      lens610_uart_write(buf, sizeof(buf));
    }  
    break;
    case GSF_PTZ_UP:
    {
      unsigned char buf[7] = {0xff,0xff,0x00,0x08,0x00,0x31,0x00};
      buf[6] = (buf[1]+buf[2]+buf[3]+buf[4]+buf[5])&0xFF;
      lens610_uart_write(buf, sizeof(buf));
    }
    break;    
    case GSF_PTZ_DOWN:
    {
      unsigned char buf[7] = {0xff,0xff,0x00,0x10,0x00,0x31,0x00};
      buf[6] = (buf[1]+buf[2]+buf[3]+buf[4]+buf[5])&0xFF;
      lens610_uart_write(buf, sizeof(buf));
    }
    break;  
    case GSF_PTZ_LEFT:
    {
      unsigned char buf[7] = {0xff,0xff,0x00,0x04,0x31,0x00,0x00};
      buf[6] = (buf[1]+buf[2]+buf[3]+buf[4]+buf[5])&0xFF;
      lens610_uart_write(buf, sizeof(buf));
    }
    break;    
    case GSF_PTZ_RIGHT:
    {
      unsigned char buf[7] = {0xff,0xff,0x00,0x02,0x31,0x00,0x00};
      buf[6] = (buf[1]+buf[2]+buf[3]+buf[4]+buf[5])&0xFF;
      lens610_uart_write(buf, sizeof(buf));
    }
    break;
  }
  return 0;
}

int (*gsf_lens_start)(int ch, char *ttyAMA) = lens610_lens_start;
int (*gsf_lens_ircut)(int ch, int dayNight) = lens610_lens_ircut;
int (*gsf_lens_zoom)(int ch,  int dir, int speed) = lens610_lens_zoom;
int (*gsf_lens_focus)(int ch, int dir, int speed) = lens610_lens_focus;
int (*gsf_lens_stop)(int ch) = lens610_lens_stop;
int (*gsf_lens_cal)(int ch) = lens610_lens_cal;
int (*gsf_uart_open)(char *ttyAMA, int baudrate) = lens610_uart_open;
int (*gsf_uart_write)(unsigned char *buf, int size) = lens610_uart_write;
int (*gsf_lens_init)(gsf_lens_ini_t *ini) = lens610_lens_init;
int (*gsf_lens_ptz)(int ch, gsf_lens_t *lens) = lens610_lens_ptz;

#endif
