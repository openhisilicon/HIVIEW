#include <unistd.h>
#include <stdlib.h>
#include <signal.h>

#include "inc/gsf.h"

#include "gb28181.h"
//#include "cfg.h"
// #include "msg_func.h"
// #include "rtsp-st.h"

#include "mod/bsp/inc/bsp.h"

GSF_LOG_GLOBAL_INIT("GB28181", 8*1024);

static void main_exit(int signal)
{
    //TODO;
    exit(0);
}

int main(int argc, char *argv[])
{
    signal(SIGPIPE, SIG_IGN);
    signal(SIGTERM, main_exit);
    signal(SIGINT, main_exit);

    if (argc < 2)
    {
        printf("pls input: %s gb28181_parm.json\n", argv[0]);
        return -1;
    }

    while (1)
    {
        sleep(6);
    }
}