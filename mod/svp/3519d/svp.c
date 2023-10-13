#include <unistd.h>
#include <stdlib.h>
#include <assert.h>

#include "fw/comm/inc/proc.h"

#include "svp.h"
#include "cfg.h"
#include "msg_func.h"

#include "sample_svp_npu_process.h"

void* svp_pub = NULL;
GSF_LOG_GLOBAL_INIT("SVP", 8*1024);


int main(int argc, char *argv[])
{
    if(argc < 2)
    {
      printf("pls input: %s svp_parm.json\n", argv[0]);
      return -1;
    }
     
    strncpy(svp_parm_path, argv[1], sizeof(svp_parm_path)-1);
    
    if(json_parm_load(svp_parm_path, &svp_parm) < 0)
    {
      json_parm_save(svp_parm_path, &svp_parm);
      json_parm_load(svp_parm_path, &svp_parm);
    }
 
    svp_pub = nm_pub_listen(GSF_PUB_SVP);
     
    char home_path[256] = {0};
    proc_absolute_path(home_path);
    sprintf(home_path, "%s/../", home_path);
    printf("home_path:[%s]\n", home_path);
    
    printf("init algorithm library...\n");
        
    sample_svp_model_path(home_path);
    sample_svp_npu_handle_signal();
    
    sample_svp_npu_acl_e2e_yolo(5);
    
    //init listen;
    GSF_LOG_CONN(0, 100);

    while(1)
    {                        
      sleep(5);
    }
    
    GSF_LOG_DISCONN();

    return 0;
}