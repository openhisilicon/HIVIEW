#ifndef __log_h__
#define  __log_h__

#include <pthread.h>
#include <string.h>
#include <stdarg.h>
#include <stdio.h>
#include <unistd.h>

#ifdef __cplusplus
extern "C" {
#endif

#define GSF_IPC_LOG     "ipc:///tmp/log_pull"


#define COLOR_NONE              "\033[0000m"
#define COLOR_BLACK             "\033[0;30m"
#define COLOR_LIGHT_GRAY        "\033[0;37m"
#define COLOR_DARK_GRAY         "\033[1;30m"
#define COLOR_BLUE              "\033[0;34m"
#define COLOR_LIGHT_BLUE        "\033[1;34m"
#define COLOR_GREEN             "\033[0;32m"
#define COLOR_LIGHT_GREEN       "\033[1;32m"
#define COLOR_CYAN              "\033[0;36m"
#define COLOR_LIGHT_CYAN        "\033[1;36m"
#define COLOR_RED               "\033[0;31m"
#define COLOR_LIGHT_RED         "\033[1;31m"
#define COLOR_PURPLE            "\033[0;35m"
#define COLOR_LIGHT_PURPLE      "\033[1;35m"
#define COLOR_BROWN             "\033[0;33m"
#define COLOR_YELLOW            "\033[1;33m"
#define COLOR_WHITE             "\033[1;37m"

#define DEBUG_LEVEL_ERROR   0x001
#define DEBUG_LEVEL_WARNING 0x002
#define DEBUG_LEVEL_INFO    0x003
#define DEBUG_LEVEL_PRINTF  0x004


extern int   __log_push;
extern char  __log_buf[];
extern int   __log_buf_size;
extern char* __log_mod;
extern int   __log_flag;
extern pthread_mutex_t __log_mutex;

/*
 * name: mod_name, size: buf_size;
 */
#define GSF_LOG_GLOBAL_INIT(name, size) \
    int   __log_push;\
    char  __log_buf[size];\
    int   __log_buf_size = size;\
    char* __log_mod = name;\
    int   __log_flag = 0;\
    pthread_mutex_t __log_mutex = PTHREAD_MUTEX_INITIALIZER;

/*
 * flag: 0-stderr 1-log, ms: connect wait time (ms);
 */
#define GSF_LOG_CONN(flag, ms) do{ \
    __log_flag = flag; \
    if(__log_flag) {\
        __log_push = nm_push_conn(GSF_IPC_LOG);\
        usleep((ms)*1000);}\
    }while(0)
    
#define GSF_LOG_DISCONN() do{ \
    if(__log_flag)\
        nm_push_close(__log_push);\
    }while(0)


static inline void GSF_LOG_WRITE( int level
                , const char *mod_name
                , const char *file_name
                , unsigned int line_no
                , const char *func_name
                , const char *fmt,...)
{
	va_list ap;
    va_start(ap,fmt);
    
    pthread_mutex_lock(&__log_mutex);
    sprintf(__log_buf, COLOR_DARK_GRAY "%d:%s:%s:%s>" COLOR_NONE, level, mod_name,file_name,func_name);
    int head_len = strlen(__log_buf);
    vsnprintf(__log_buf+head_len, __log_buf_size-head_len, fmt, ap);
    if(__log_flag)
        nm_push_send(__log_push, __log_buf, strlen(__log_buf)+1);
    else 
        fprintf(stderr, "%s", __log_buf);
    pthread_mutex_unlock(&__log_mutex);
    
    va_end(ap);
    return;
}

#define __printf(level, args...)  GSF_LOG_WRITE(level,__log_mod,__FILE__,__LINE__,__FUNCTION__,##args)
#undef printf
#define printf(fmt, args...)    __printf(DEBUG_LEVEL_PRINTF,COLOR_WHITE fmt COLOR_NONE, ##args)
#undef error
#define error(fmt, args...)     __printf(DEBUG_LEVEL_ERROR,COLOR_LIGHT_RED fmt COLOR_NONE, ##args)
#undef warn
#define warn(fmt, args...)      __printf(DEBUG_LEVEL_WARNING,COLOR_YELLOW fmt COLOR_NONE, ##args)
#undef info
#define info(fmt, args...)      __printf(DEBUG_LEVEL_INFO,COLOR_LIGHT_GREEN fmt COLOR_NONE, ##args)

#ifdef __cplusplus
}
#endif

#endif