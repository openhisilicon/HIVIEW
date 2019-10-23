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

#ifdef __cplusplus
}
#endif

#endif
