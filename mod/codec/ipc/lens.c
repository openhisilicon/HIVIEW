#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "fw/comm/inc/serial.h"
#include "cfg.h"
#include "lens.h"
#include "mpp.h"

#if defined(GSF_CPU_3516d) || defined(GSF_CPU_3559)
//The 3516dv300 implement is lens16x.c
#elif defined(GSF_CPU_3519d)
//The 3519dv500 implement is lens19d.c
#elif defined(GSF_CPU_3403)
//The 3403 implement is lens3403.c
#elif defined(GSF_CPU_3516c)
//The 3516cv610 implement is lens610.c
#else
//The xxx not implement lensxxx.c
#warning "xxx lens not implement"
static int __lens_init(gsf_lens_ini_t *ini)         { return -1;}
static int __lens_start(int ch, char *ttyAMA)       { return -1;}
static int __lens_ircut(int ch, int dayNight)       { return -1;}
static int __lens_zoom(int ch,  int dir, int speed) { return -1;}
static int __lens_focus(int ch, int dir, int speed) { return -1;}
static int __lens_stop(int ch)                      { return -1;}
static int __lens_cal(int ch)                       { return -1;}
static int __uart_open(char *ttyAMA, int baudrate)  { return -1;}
static int __uart_write(unsigned char *buf, int size){ return -1;}
static int __lens_ptz(int ch, gsf_lens_t *lens)     { return -1;}

int (*gsf_lens_start)(int ch, char *ttyAMA) = __lens_start;
int (*gsf_lens_ircut)(int ch, int dayNight) = __lens_ircut;
int (*gsf_lens_zoom)(int ch,  int dir, int speed) = __lens_zoom;
int (*gsf_lens_focus)(int ch, int dir, int speed) = __lens_focus;
int (*gsf_lens_stop)(int ch) = __lens_stop;
int (*gsf_lens_cal)(int ch) = __lens_cal;
int (*gsf_uart_open)(char *ttyAMA, int baudrate) = __uart_open;
int (*gsf_uart_write)(unsigned char *buf, int size) = __uart_write;
int (*gsf_lens_init)(gsf_lens_ini_t *ini) = __lens_init;
int (*gsf_lens_ptz)(int ch, gsf_lens_t *lens) = __lens_ptz;
#endif


