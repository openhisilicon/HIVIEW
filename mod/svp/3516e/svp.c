#include <unistd.h>
#include <stdlib.h>
#include <assert.h>

#include "svp.h"
#include "cfg.h"
#include "msg_func.h"

GSF_LOG_GLOBAL_INIT("SVP", 8*1024);

int main(int argc, char *argv[])
{
    printf("init algorithm library...\n");
    //TODO;
    
    //init listen;
    GSF_LOG_CONN(1, 100);

    while(1)
    {                        
      sleep(5);
    }
    
    GSF_LOG_DISCONN();

    return 0;
}