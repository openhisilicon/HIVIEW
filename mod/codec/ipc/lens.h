#ifndef __lens_h__
#define __lens_h__

#include "codec.h"

typedef struct {
  int ch_num;   // lens number;
  char sns[32]; // sensor name;
  char lens[32];// lens name;
}gsf_lens_ini_t;

int gsf_lens_init(gsf_lens_ini_t *ini);
int gsf_lens_start(int ch, char *ttyAMA);
int gsf_lens_ircut(int ch, int dayNight);
int gsf_lens_zoom(int ch,  int dir, int speed);
int gsf_lens_focus(int ch, int dir, int speed);
int gsf_lens_stop(int ch);
int gsf_lens_cal(int ch);

int gsf_uart_open(char *ttyAMA);
int gsf_uart_write(char *buf, int size);

#endif // __lens_h__