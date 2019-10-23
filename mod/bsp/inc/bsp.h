#ifndef __bsp_h__
#define  __bsp_h__

#ifdef __cplusplus
extern "C" {
#endif

#include "inc/gsf.h"

//for json cfg;
#include "mod/bsp/inc/sjb_bsp.h"

#define GSF_IPC_BSP     "ipc:///tmp/bsp_rep"
#define GSF_PUB_BSP     "ipc:///tmp/bsp_pub"

enum {
    GSF_ID_BSP_TIME = 1, // gsf_time_t
    GSF_ID_BSP_ETH  = 2, // gsf_eth_t
    GSF_ID_BSP_UPG  = 3, // /path/xxx.upg;
    GSF_ID_BSP_DEF  = 4, // gsf_bsp_def_t
    GSF_ID_BSP_END  
};


enum {
  GSF_EV_REBOOT = 1,
  GSF_EV_UPG    = 2,
};


enum {
    GSF_BSP_ERR = -1,
};


#ifdef __cplusplus
}
#endif

#endif
