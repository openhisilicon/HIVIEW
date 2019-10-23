#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/time.h>

#include "rtc.h"
#include "cfg.h"
#include "hi_rtc.h"

#define DEV_NAME "/dev/hi_rtc"

int rtc_fd = -1;


gsf_time_t* rtc2gsf(rtc_time_t *rtc, gsf_time_t *gsf)
{
	if (rtc != NULL && gsf != NULL)
	{
		gsf->year  = rtc->year - 1900;
		gsf->month = rtc->month  % 13;
		gsf->day   = rtc->date % 32;
		gsf->hour  = rtc->hour % 24;
		gsf->minute= rtc->minute % 60;
		gsf->second= rtc->second % 60;
    gsf->wday  = rtc->weekday % 7;
	}
	return gsf;
}


rtc_time_t*	gsf2rtc(gsf_time_t *gsf, rtc_time_t *rtc)
{
	if (gsf != NULL && rtc != NULL)
	{
		rtc->year 	  = gsf->year + 1900;
		rtc->month    = gsf->month % 13;
		rtc->date     = gsf->day % 32;
		rtc->hour	    = gsf->hour % 24;			
		rtc->minute   = gsf->minute % 60;		
		rtc->second   = gsf->second % 60;
		rtc->weekday  = gsf->wday % 8;
	}
	return rtc;
}


struct tm*	gsf2tm(gsf_time_t *gsf, struct tm* tm)
{
	if (tm != NULL && gsf != NULL)
	{
		tm->tm_year = gsf->year;
		tm->tm_mon  = gsf->month;
		tm->tm_mday = gsf->day;
		tm->tm_hour = gsf->hour;	
		tm->tm_min  = gsf->minute;
		tm->tm_sec  = gsf->second;
		tm->tm_wday = gsf->wday;
	}
	return tm;
}


gsf_time_t*	tm2gsf(struct tm* tm, gsf_time_t* gsf)
{
	if (gsf != NULL && tm != NULL)
	{
		gsf->year   = tm->tm_year;
		gsf->month  = tm->tm_mon;
		gsf->day    = tm->tm_mday;
		gsf->hour   = tm->tm_hour;
		gsf->minute = tm->tm_min;
 		gsf->second = tm->tm_sec;
		gsf->wday   = tm->tm_wday;
	}
	return gsf;
}


int rtc_init()
{
  rtc_fd = open(DEV_NAME, O_RDWR);
  if (rtc_fd < 0) 
  {
    printf("open %s failed\n", DEV_NAME);
    return -1;
  }
  if(rtc_fd > 0)
  {
    rtc_time_t rtc;
    gsf_time_t gsf;
    int ret = ioctl(rtc_fd, HI_RTC_RD_TIME, &rtc);
    if(ret < 0)
      return ret;
    
    struct timeval 	tv;
	  struct timezone tz;
    gettimeofday(&tv, &tz);
  
    struct tm tm;
    rtc2gsf(&rtc, &gsf);
    gsf2tm(&gsf, &tm);
		tv.tv_sec  = mktime(&tm);
		tv.tv_usec = 0;
		settimeofday(&tv, &tz);
  }
  return -1;
}


int rtc_set(gsf_time_t *gsf)
{
  if(rtc_fd > 0)
  {
    rtc_time_t rtc;
    gsf2rtc(gsf, &rtc);
    
    struct timeval 	tv;
	  struct timezone tz;
    gettimeofday(&tv, &tz);
  
    struct tm tm;
    gsf2tm(gsf, &tm);
		tv.tv_sec  = mktime(&tm);
		tv.tv_usec = 0;
		settimeofday(&tv, &tz);
    
    int ret = ioctl(rtc_fd, HI_RTC_SET_TIME, &rtc);
    return ret;
  }
  return -1;
}