#include <unistd.h>
#include <stdlib.h>
#include <assert.h>

#include "svp.h"
#include "cfg.h"
#include "msg_func.h"

#include "hi_type.h"
#include "hi_debug.h"
#include "sample_comm.h"


GSF_LOG_GLOBAL_INIT("SVP", 8*1024);

#include <signal.h>

HI_VOID SAMPLE_EXIT_HandleSig(HI_S32 s32Signo)
{
  signal(SIGINT,SIG_IGN);
  signal(SIGTERM,SIG_IGN);
  
  extern int sample_stop(); sample_stop();
  
  exit(-1);
}


int main(int argc, char *argv[])
{
    signal(SIGINT, SAMPLE_EXIT_HandleSig);
    signal(SIGTERM, SAMPLE_EXIT_HandleSig);
    
    printf("init algorithm library...\n");
    //TODO;
    extern int sample_start(); sample_start();
     
    //init listen;
    GSF_LOG_CONN(1, 100);

    while(1)
    {                        
      sleep(5);
    }
    
    GSF_LOG_DISCONN();

    return 0;
}