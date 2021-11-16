#ifndef __UVC_LOG_H__
#define __UVC_LOG_H__

#include "uvc.h"

#define LOG(fmt, args...)  printf(fmt, ##args)
#define RLOG(fmt, args...) error(fmt, ##args)

#define INFO(fmt, args...) info(fmt, ##args)
#define ERR(fmt, args...) error(fmt, ##args)
#define ERR_ON(cond, fmt, args...) ((cond) ? ERR(fmt, ##args) : 0)

#define CRIT(fmt, args...) \
    do { \
        INFO(fmt, ##args); \
        exit(0); \
    } while (0)

#define CRIT_ON(cond, fmt, args...) do { if (cond) CRIT(fmt, ##args);} while (0)


#endif //__UVC_LOG_H__
