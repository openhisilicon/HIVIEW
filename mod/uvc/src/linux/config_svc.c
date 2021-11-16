#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <assert.h>
#include <string.h>

#include "config_svc.h"
#include "log.h"


#define CONFIG_MAX_PATH 0xff

typedef struct config_svc_t
{
  unsigned int stream_resolution; //;720: 720P, 1080: 1080P
	unsigned int stream_format;		//;0: MJPG, 1: H264
	unsigned int uvc_imagesize;		//; set max PayloadImage size (e.g 1920 * 1080 * 2)
	unsigned int uvc_cache_count;	//; set cache node number
	unsigned int iomethod;			//; 0x1: IO_METHOD_MMAP, 0x2: IO_METHOD_USERPTR
  unsigned char is_multi_payload;
} config_svc_t;

static config_svc_t *__config_svc = 0;

int create_config_svc(char *config_path)
{
    assert(config_path != 0);

    if (__config_svc != 0)
    {
        free(__config_svc);
    }

    __config_svc = (config_svc_t*) malloc(sizeof * __config_svc);
    if (__config_svc == 0)
    {
        LOG("create config svc failure\n");
        goto ERR;
    }


    get_all_default_conf();

    return 0;

ERR:
    if (__config_svc)
    {
        free(__config_svc);
    }

    __config_svc = 0;

    return -1;
}

void release_cofnig_svc()
{
    if (__config_svc)
    {
        free(__config_svc);
    }

    __config_svc = 0;
}

int get_config_value(const char *key, int default_value)
{
    int value = default_value;

    assert(key != 0);
    assert(__config_svc != 0);

	if(strstr(key, "stream:is_multi_payload"))
		value = __config_svc->is_multi_payload;
	else if(strstr(key, "uvc:imagesize"))
		value = __config_svc->uvc_imagesize;
	else if(strstr(key, "uvc:cache_count"))
		value = __config_svc->uvc_cache_count;	

    return value;
}

int get_all_default_conf()
{
    __config_svc->is_multi_payload 	= 0;
    __config_svc->stream_resolution = 1080; 			//;720: 720P, 1080: 1080P
	__config_svc->stream_format		= 1;			//;0: MJPG, 1: H264
	__config_svc->uvc_imagesize		= 1920*1080*2;	//; set max PayloadImage size (e.g 1920 * 1080 * 2)
	__config_svc->uvc_cache_count = 6;			//; set cache node number
	__config_svc->iomethod			  = 0x2;			//; 0x1: IO_METHOD_MMAP, 0x2: IO_METHOD_USERPTR
    return 0;
}

int is_multi_payload_conf()
{
    return __config_svc->is_multi_payload;
}
