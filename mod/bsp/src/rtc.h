#ifndef __rtc_h__
#define __rtc_h__

#include "bsp.h"

//from rtc to linux;
int rtc_init(void);
//set time to rtc;
int rtc_set(gsf_time_t *gsf);
int rtc_get(gsf_time_t *gsf);

// set ntp;
int ntp_set(gsf_ntp_t *ntp);

// set zone;
int zone_set(int zone);


#endif