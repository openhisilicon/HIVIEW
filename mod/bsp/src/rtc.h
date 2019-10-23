#ifndef __rtc_h__
#define __rtc_h__

#include "bsp.h"

//from rtc to linux;
int rtc_init();

//set time to rtc;
int rtc_set(gsf_time_t *gsf);

#endif