#ifndef __netinf_h__
#define  __netinf_h__

#ifdef __cplusplus
extern "C" {
#endif

#include "bsp.h"

//init net interface;
int netinf_init(void);

//set eth;
int netinf_eth_set(gsf_eth_t *eth);
int netinf_eth_get(gsf_eth_t *eth);

// set wifi;
int netinf_wifi_set(gsf_wifi_t *wifi);

int netinf_wifi_list(gsf_wifi_list_t list[128]);

// set vpn;
int netinf_vpn_set(gsf_vpn_t *vpn);
int netinf_vpn_stat(gsf_vpn_stat_t *st);

#ifdef __cplusplus
}
#endif

#endif
