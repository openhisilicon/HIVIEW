#include <stdlib.h>


#include "sip-timer.h"
#include "st_timer.h"


static void *st_timer;

void sip_timer_init(void)
{
  st_timer = st_timer_new();
}

void sip_timer_cleanup(void)
{
  st_timer_free(st_timer);
  st_timer = NULL;
}

sip_timer_t sip_timer_start(int timeout, sip_timer_handle handler, void* usrptr)
{
  if(!st_timer)
    return NULL;
    
  int id = st_timer_add(st_timer, 1, timeout, "", (timer_cb)handler, usrptr);
	return (id > 0)?(sip_timer_t)id:NULL;
}

int sip_timer_stop(sip_timer_t* id)
{
  if(st_timer && id && *id)
  {
    int r = st_timer_del(st_timer, *((int*)id));
    *id = NULL;
    return r;
  }
	return -1;
}
