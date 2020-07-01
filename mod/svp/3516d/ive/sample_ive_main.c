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

#include "sample_comm_ive.h"
#include "sample_ive_main.h"

static char **s_ppChCmdArgv = NULL;

/******************************************************************************
* function : to process abnormal case
******************************************************************************/
#ifndef __HuaweiLite__
HI_VOID SAMPLE_IVE_HandleSig(HI_S32 s32Signo)
{
    signal(SIGINT,SIG_IGN);
    signal(SIGTERM,SIG_IGN);

    if (SIGINT == s32Signo || SIGTERM == s32Signo)
    {
        switch (*s_ppChCmdArgv[1])
        {
            case '0':
                {
                    SAMPLE_IVE_Od_HandleSig();
                }
                break;
            case '1':
                {
                    SAMPLE_IVE_Md_HandleSig();
                }
                break;
            case '2':
                {
                   SAMPLE_IVE_Canny_HandleSig();
                }
                break;
            case '3':
                {
                    SAMPLE_IVE_Gmm2_HandleSig();
                }
                break;
            case '4':
                {
                    SAMPLE_IVE_TestMemory_HandleSig();
                }
                break;
            case '5':
                {
                    SAMPLE_IVE_Sobel_HandleSig();
                }
                break;
            case '6':
                {
                    SAMPLE_IVE_St_Lk_HandleSig();
                }
                break;
            case '7':
                {
                    SAMPLE_IVE_Kcf_HandleSig();
                }
                break;
            case '8':
                {
                    SAMPLE_IVE_PerspTrans_HandleSig();
                }
                break;
            default :
                {
                }
                break;
        }

        printf("\033[0;31mprogram termination abnormally!\033[0;39m\n");
    }
    exit(-1);
}
#endif
/******************************************************************************
* function : show usage
******************************************************************************/
HI_VOID SAMPLE_IVE_Usage(HI_CHAR* pchPrgName)
{
    printf("Usage : %s <index> [complete] \n", pchPrgName);
    printf("index:\n");
    printf("\t 0)Occlusion detected.(VI->VPSS->IVE->VO_HDMI).\n");
    printf("\t 1)Motion detected.(VI->VPSS->IVE->VGS->VO_HDMI).\n");
    printf("\t 2)Canny,<complete>:0, part canny;1,complete canny.(FILE->IVE->FILE).\n");
    printf("\t 3)Gmm2.(FILE->IVE->FILE).\n");
    printf("\t 4)MemoryTest.(FILE->IVE->FILE).\n");
    printf("\t 5)Sobel.(FILE->IVE->FILE).\n");
    printf("\t 6)St Lk.(FILE->IVE->FILE).\n");
    printf("\t 7)Kcf track.(VI->VPSS->IVE->VO_HDMI).\n");
    printf("\t 8)PerspTrans.(FILE->IVE->FILE).\n");
}

/******************************************************************************
* function : ive sample
******************************************************************************/
#ifdef __HuaweiLite__
int app_main(int argc, char *argv[])
#else
int sample_ive_main(int argc, char *argv[])
#endif
{
    if (argc < 2 || argc > 3)
    {
        SAMPLE_IVE_Usage(argv[0]);
        return HI_FAILURE;
    }

    if (!strncmp(argv[1], "-h", 2))
    {
        SAMPLE_IVE_Usage(argv[0]);
        return HI_SUCCESS;
    }

    s_ppChCmdArgv = argv;
#ifndef __HuaweiLite__
    signal(SIGINT, SAMPLE_IVE_HandleSig);
    signal(SIGTERM, SAMPLE_IVE_HandleSig);
#endif

    switch (*argv[1])
    {
        case '0':
            {
                SAMPLE_IVE_Od();
            }
            break;
        case '1':
            {
                SAMPLE_IVE_Md();
            }
            break;
        case '2':
            {
                if ((argc < 3) || (('0' != *argv[2]) && ('1' != *argv[2])))
                {
                    SAMPLE_IVE_Usage(argv[0]);
                    return HI_FAILURE;
                }
                SAMPLE_IVE_Canny(*argv[2]);
            }
            break;
        case '3':
            {
                SAMPLE_IVE_Gmm2();
            }
            break;
        case '4':
            {
                SAMPLE_IVE_TestMemory();
            }
            break;
        case '5':
            {
                SAMPLE_IVE_Sobel();
            }
            break;
        case '6':
            {
                SAMPLE_IVE_St_Lk();
            }
            break;
        case '7':
            {
                SAMPLE_IVE_Kcf();
            }
            break;
        case '8':
            {
                SAMPLE_IVE_PerspTrans();
            }
            break;
        default :
            {
                SAMPLE_IVE_Usage(argv[0]);
            }
            break;
    }

    return 0;

}



