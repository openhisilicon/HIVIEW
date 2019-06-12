#ifndef __netbox_h__
#define  __netbox_h__

#ifdef __cplusplus
extern "C" {
#endif

#include "inc/msg.h"


enum {
    GSF_ID_NETBOX_MAIL_CFG  = 1,
    GSF_ID_NETBOX_FTP_CFG   = 2,
    GSF_ID_NETBOX_SENDMAIL  = 3,
    GSF_ID_NETBOX_SENDFTP   = 4,
};

enum {
    GSF_NETBOX_ERR = -1,
};




#ifdef __cplusplus
}
#endif

#endif