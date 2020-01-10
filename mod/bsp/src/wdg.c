#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/time.h>

#include "wdg.h"
#include "cfg.h"
#include "watchdog.h"

#define WDG_TO  15
#define DEV_FILE "/dev/watchdog"

int wdg_fd = -1;
void *wdg_timer = NULL;
int wdg_func(void *u)
{
  if(wdg_fd > 0)
    ioctl(wdg_fd, WDIOC_KEEPALIVE);
  return 0;
}

#if 0
  insmod hi_osal.ko anony=1;insmod hi_wdt.ko;
  watchdog -T 10 /dev/watchdog
  watchdog -t 5 /dev/watchdog
#endif

int wdg_open(void)
{
  int ret = 0;
  int to = WDG_TO;
  to = (to <= 15)?15/2:to/2; //hi_wdg.c
  
	if(wdg_fd < 0)
	{
	  wdg_fd  = open(DEV_FILE, O_RDWR);
  	if (wdg_fd < 0)
  	{
  		printf("fail to open file:%s\n", DEV_FILE);
  		return -1;
  	}
  	
  	int option = WDIOS_ENABLECARD;
  	ret = ioctl(wdg_fd, WDIOC_SETTIMEOUT, &to);
  	ret = ioctl(wdg_fd, WDIOC_SETOPTIONS, &option);
  	
  	wdg_timer = timer_add(to*1000, wdg_func, NULL);
  	
  }
  return 0;
}


int wdg_reboot(void)
{
  if(wdg_fd > 0)
  {
    int timeout = 1;
    int option  = WDIOS_ENABLECARD;    
  	ioctl(wdg_fd, WDIOC_SETTIMEOUT, &timeout);
  	ioctl(wdg_fd, WDIOC_SETOPTIONS, &option);
  }
  return 0;
}


int wdg_close(void)
{
  if(wdg_fd > 0)
  {
    int option = WDIOS_DISABLECARD;
	  int ret = ioctl(wdg_fd, WDIOC_SETOPTIONS, &option);
	  if(wdg_timer)
	  {
      timer_del(wdg_timer);  
      wdg_timer = NULL;
	  }
    close(wdg_fd);
    wdg_fd = -1;
  }
  return 0;
}

