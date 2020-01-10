#ifndef __wifi_h__
#define __wifi_h__

#include "bsp.h"

//wifi;
int wifi_init(void);
int wifi_set(gsf_wifi_t *wifi);
int wifi_list(gsf_wifi_list_t list[128]);

//dhcp helper;
int dhcpc_start(char *name);
int dhcpc_stop(char *name);
int dhcpd_start(char *name);
int dhcpd_stop(char *name);

#endif // __wifi_h__