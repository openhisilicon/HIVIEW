#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>

#include "sample_dsp_main.h"

/******************************************************************************
* function : dsp sample
******************************************************************************/
#ifdef __HuaweiLite__
int app_main(int argc, char *argv[])
{
#else
/******************************************************************************
* function : to process abnormal case
******************************************************************************/
HI_VOID SAMPLE_SVP_DSP_HandleSig(HI_S32 s32Signo)
{
    signal(SIGINT,SIG_IGN);
    signal(SIGTERM,SIG_IGN);

    if (SIGINT == s32Signo || SIGTERM == s32Signo)
    {
        SAMPLE_SVP_DSP_DilateHandleSig();
        printf("\033[0;31mprogram termination abnormally!\033[0;39m\n");
    }
    exit(-1);
}
int sample_dsp_main(int argc, char *argv[])
{
    signal(SIGINT, SAMPLE_SVP_DSP_HandleSig);
    signal(SIGTERM, SAMPLE_SVP_DSP_HandleSig);
#endif
    SAMPLE_SVP_DSP_Dilate();
}



