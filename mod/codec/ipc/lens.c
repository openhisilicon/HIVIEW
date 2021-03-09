#include "cfg.h"

#include "lens.h"

static gsf_lens_ini_t _ini;

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
  
  return 0;
}
int gsf_lens_zoom(int ch,  int dir, int speed)
{
  return 0;
}
int gsf_lens_focus(int ch, int dir, int speed)
{
  return 0;
}