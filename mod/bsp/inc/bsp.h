#ifndef __bsp_h__
#define  __bsp_h__

#ifdef __cplusplus
extern "C" {
#endif

#include "inc/gsf.h"

//for json cfg;
#include "mod/bsp/inc/sjb_bsp.h"

#define GSF_IPC_BSP     "ipc:///tmp/bsp_rep"

enum {
    GSF_ID_BSP_TIME = 1, // gsf_time_t
    GSF_ID_BSP_ETH  = 2, // gsf_eth_t
    GSF_ID_BSP_UPG  = 3, // gsf_upg_t
    GSF_ID_BSP_DEF  = 4, // gsf_bsp_def_t
    GSF_ID_BSP_END  
};

enum {
    GSF_BSP_ERR = -1,
};


typedef struct {
  int y;
  int m;
}gsf_time_t;




#ifdef __cplusplus
}
#endif

#endif
