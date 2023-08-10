#ifndef __lens_h__
#define __lens_h__

#include "codec.h"

typedef struct {
  int ch_num;   // lens number;
  char sns[32]; // sensor name;
  char lens[32];// lens name;
}gsf_lens_ini_t;

extern int (*gsf_lens_init)(gsf_lens_ini_t *ini);
extern int (*gsf_lens_start)(int ch, char *ttyAMA);
extern int (*gsf_lens_ircut)(int ch, int dayNight);
extern int (*gsf_lens_zoom)(int ch,  int dir, int speed);
extern int (*gsf_lens_focus)(int ch, int dir, int speed);
extern int (*gsf_lens_stop)(int ch);
extern int (*gsf_lens_cal)(int ch);
/////////////////////////////////////////////
extern int (*gsf_uart_open)(char *ttyAMA, int baudrate);
extern int (*gsf_uart_write)(char *buf, int size);
/////////////////////////////////////////////
extern int (*gsf_lens_ptz)(int ch, gsf_lens_t *lens);

#endif // __lens_h__