#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#include <stdlib.h>
#include <time.h>
#include <linux/rtc.h>

#include "rtc.h"
#include "cfg.h"
#include "hi_rtc.h"

#define HI_DEV_NAME "/dev/hi_rtc"
#define DEV_NAME "/dev/rtc0"

int hirtc_fd = -1;
int rtc_fd = -1;

gsf_time_t* hirtc2gsf(rtc_time_t *rtc, gsf_time_t *gsf)
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


rtc_time_t*	gsf2hirtc(gsf_time_t *gsf, rtc_time_t *rtc)
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

gsf_time_t* rtc2gsf(struct rtc_time *rtc, gsf_time_t *gsf)
{
	if (rtc != NULL && gsf != NULL)
	{
		gsf->year  = rtc->tm_year - 1900;
		gsf->month = rtc->tm_mon  % 13;
		gsf->day   = rtc->tm_mday % 32;
		gsf->hour  = rtc->tm_hour % 24;
		gsf->minute= rtc->tm_min % 60;
		gsf->second= rtc->tm_sec % 60;
    gsf->wday  = rtc->tm_wday % 7;
    gsf->isdst = rtc->tm_isdst;
    //tm_yday;
	}
	return gsf;
}

struct rtc_time*	gsf2rtc(gsf_time_t *gsf, struct rtc_time *rtc)
{
	if (gsf != NULL && rtc != NULL)
	{
		rtc->tm_year  = gsf->year + 1900;
		rtc->tm_mon   = gsf->month % 13;
		rtc->tm_mday  = gsf->day % 32;
		rtc->tm_hour  = gsf->hour % 24;			
		rtc->tm_min   = gsf->minute % 60;		
		rtc->tm_sec   = gsf->second % 60;
		rtc->tm_wday  = gsf->wday % 8;
		rtc->tm_isdst = gsf->isdst;
    rtc->tm_yday  = 0;
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
		tm->tm_isdst = gsf->isdst;
		//tm_yday;
		printf("[%d-%d-%d %d:%d:%d]\n"
		    , tm->tm_year, tm->tm_mon, tm->tm_mday
		    , tm->tm_hour, tm->tm_min, tm->tm_sec);
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
		gsf->isdst  = tm->tm_isdst;
		//tm_yday;
		printf("[%d-%d-%d %d:%d:%d]\n"
		    , tm->tm_year, tm->tm_mon, tm->tm_mday
		    , tm->tm_hour, tm->tm_min, tm->tm_sec);
	}
	return gsf;
}

int rtc_init(void)
{
  //rtc_fd = open(DEV_NAME, O_RDWR); // hwclock: can't open '/dev/rtc0': Device or resource busy
  hirtc_fd = open(HI_DEV_NAME, O_RDWR);
  
  if (hirtc_fd < 0 && rtc_fd < 0) 
  {
    printf("open [%s && %s] failed\n", HI_DEV_NAME, DEV_NAME);
    return -1;
  }
  
  if(hirtc_fd > 0)
  {
    rtc_time_t rtc;
    gsf_time_t gsf;
    int ret = ioctl(hirtc_fd, HI_RTC_RD_TIME, &rtc);
    if(ret < 0)
      return ret;
    
    struct timeval 	tv;
	  struct timezone tz;
    gettimeofday(&tv, &tz);
  
    struct tm tm;
    hirtc2gsf(&rtc, &gsf);
    gsf2tm(&gsf, &tm);
		tv.tv_sec  = mktime(&tm);
		tv.tv_usec = 0;
		settimeofday(&tv, &tz);
  }
  else if(rtc_fd > 0)
  {
    struct rtc_time rtc;
    gsf_time_t gsf;
    int ret = ioctl(rtc_fd, RTC_RD_TIME, &rtc);
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
  else
  {
    system("hwclock -s");
  }
  return 0;
}

int rtc_get(gsf_time_t *gsf)
{ 
  time_t _time = time(NULL);
  struct tm _tm;
  localtime_r(&_time, &_tm);
  tm2gsf(&_tm, gsf);
  
  char *z = getenv("TZ");
  printf("getenv >>> TZ=[%s]\n", z);
  
  if(z)
  {
    char s = '+';
    int hour = 0, min = 0;
    
    sscanf(z, "UTC%c%d:%02d", &s, &hour, &min);
    gsf->zone = (hour*60 + min) * (s=='+'?1:-1);
  }
  
  return 0;
}


int rtc_set(gsf_time_t *gsf)
{
  if(hirtc_fd > 0)
  {
    rtc_time_t rtc;
    gsf2hirtc(gsf, &rtc);
    
    struct timeval 	tv;
	  struct timezone tz;
    gettimeofday(&tv, &tz);
  
    struct tm tm;
    gsf2tm(gsf, &tm);
		tv.tv_sec  = mktime(&tm);
		tv.tv_usec = 0;
		settimeofday(&tv, &tz);
    
    int ret = ioctl(hirtc_fd, HI_RTC_SET_TIME, &rtc);
    return ret;
  }
  else if(rtc_fd > 0)
  {
    struct rtc_time rtc;
    gsf2rtc(gsf, &rtc);
    
    struct timeval 	tv;
	  struct timezone tz;
    gettimeofday(&tv, &tz);
  
    struct tm tm;
    gsf2tm(gsf, &tm);
		tv.tv_sec  = mktime(&tm);
		tv.tv_usec = 0;
		settimeofday(&tv, &tz);
    
    int ret = ioctl(rtc_fd, RTC_SET_TIME, &rtc);
    return ret;
  }
  else
  {
    struct timeval 	tv;
	  struct timezone tz;
    gettimeofday(&tv, &tz);
  
    struct tm tm;
    gsf2tm(gsf, &tm);
		tv.tv_sec  = mktime(&tm);
		tv.tv_usec = 0;
		settimeofday(&tv, &tz);
		
		return system("hwclock -w");
  }
  return -1;
}

int ntp_set(gsf_ntp_t *ntp)
{
  system("killall watch ntpd");
  
  if(ntp->prog > 0)
  {
    //hwclock -s; ntpd -qNn -p 0.cn.pool.ntp.org; hwclock -w;
    char cmd[128];
    
    sprintf(cmd, "echo \"%s\" > /etc/ntp.conf", ntp->server1);
    system(cmd);
    
    sprintf(cmd, "echo \"$s\" >> /etc/ntp.conf", ntp->server2);
    system(cmd);
    
    system("hwclock -s");
    
    sprintf(cmd, "watch -n %d \"ntpd -qNn &> /dev/null; hwclock -w;\" &> /dev/null &", ntp->prog);
    system(cmd);
  }
  
  return 0;
}

static char zone_env[128];

int zone_set(int zone)
{
  int hour = abs(zone)/60;
  int min  = abs(zone)%60;
  char z[32];
  char cmd[128];
  
  sprintf(z, "UTC%c%d:%02d", zone>0?'+':'-', hour, min);
  sprintf(zone_env, "TZ=%s", z);
  printf("zone_env:[%s]\n", zone_env);
  putenv(zone_env);
  
  //sprintf(cmd, "sed -i '/^export TZ=/{h;s/=.*/=%s/};${x;/^$/{s//export TZ=%s/;H};x}' /etc/profile", z, z);
  //sprintf(cmd, "echo -e \"\nexport TZ=%s\" >> /etc/profile", z);
  //printf("cmd:[%s]\n", cmd);
  //system(cmd);

  FILE *fp=fopen("/etc/profile","rb+");
  if(fp)
  {
  	char data[16*1024] = {0};
  	char *end = data + fread(data,1,sizeof(data),fp);fseek(fp,0,SEEK_SET);
  	char *tzb = strstr(data, "export TZ=");
  	if(!tzb)
  	{
  	  fwrite(data, 1, end-data, fp);
  	}
  	else
  	{ 
  	  fwrite(data, 1, tzb-data, fp);
  	  char *tze = strstr(tzb, "\n");
  	  if(tze) fwrite(tze+1, 1, end-(tze+1), fp);
  	}
  	
  	if(end[-1] != '\n') fprintf(fp, "\n");
    fprintf(fp, "export TZ=%s\n", z);
    
    fflush(fp);fdatasync(fileno(fp));fclose(fp);
  }
  tzset();
  
  printf( "daylight = %d\n", daylight);
  printf( "timezone = %ld\n", timezone/60);
  printf( "tzname[0] = %s\n", tzname[0]);
  
  
  return 0;
}