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

#include "sample_nnie_main.h"


static char **s_ppChCmdArgv = NULL;
/******************************************************************************
* function : to process abnormal case
******************************************************************************/
#ifndef __HuaweiLite__
void SAMPLE_SVP_HandleSig(int s32Signo)
{
    signal(SIGINT,SIG_IGN);
    signal(SIGTERM,SIG_IGN);

    if (SIGINT == s32Signo || SIGTERM == s32Signo)
    {
        switch (*s_ppChCmdArgv[1])
        {
            case '0':
                {
                   SAMPLE_SVP_NNIE_Rfcn_HandleSig();
                }
                break;
            case '1':
                {
                   SAMPLE_SVP_NNIE_Segnet_HandleSig();
                }
                break;
            case '2':
                {
                   SAMPLE_SVP_NNIE_FasterRcnn_HandleSig();
                }
                break;
            case '3':
                {
                   SAMPLE_SVP_NNIE_FasterRcnn_HandleSig();
                }
                break;
            case '4':
                {
                   SAMPLE_SVP_NNIE_Cnn_HandleSig();
                }
                break;
            case '5':
                {
                   SAMPLE_SVP_NNIE_Ssd_HandleSig();
                }
                break;
            case '6':
                {
                   SAMPLE_SVP_NNIE_Yolov1_HandleSig();
                }
                break;
            case '7':
                {
                   SAMPLE_SVP_NNIE_Yolov2_HandleSig();
                }
                break;

            case '8':
                {
                   SAMPLE_SVP_NNIE_Lstm_HandleSig();
                }
		       break;
    	    case '9':
    		   {
    		  	SAMPLE_SVP_NNIE_Pvanet_HandleSig();
    		   }
               break;
           case 'a':
              {
               SAMPLE_SVP_NNIE_Rfcn_HandleSig_File();
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
void SAMPLE_SVP_Usage(char* pchPrgName)
{
    printf("Usage : %s <index> \n", pchPrgName);
    printf("index:\n");
    printf("\t 0) RFCN(VI->VPSS->NNIE->VGS->VO).\n");
    printf("\t 1) Segnet(Read File).\n");
    printf("\t 2) FasterRcnnAlexnet(Read File).\n");
    printf("\t 3) FasterRcnnDoubleRoiPooling(Read File).\n");
    printf("\t 4) Cnn(Read File).\n");
    printf("\t 5) SSD(Read File).\n");
    printf("\t 6) Yolov1(Read File).\n");
    printf("\t 7) Yolov2(Read File).\n");
    printf("\t 8) LSTM(Read File).\n");
    printf("\t 9) Pvanet(Read File).\n");
    printf("\t a) Rfcn(Read File).\n");
}

/******************************************************************************
* function : ive sample
******************************************************************************/
#ifdef __HuaweiLite__
int app_main(int argc, char *argv[])
#else
int sample_nnie_main(int argc, char *argv[])
#endif
{
    int s32Ret = HI_SUCCESS;

    if (argc < 2)
    {
        SAMPLE_SVP_Usage(argv[0]);
        return HI_FAILURE;
    }
    s_ppChCmdArgv = argv;
#ifndef __HuaweiLite__
    signal(SIGINT, SAMPLE_SVP_HandleSig);
    signal(SIGTERM, SAMPLE_SVP_HandleSig);
#endif

    switch (*argv[1])
    {
        case '0':
            {
                SAMPLE_SVP_NNIE_Rfcn();
            }
            break;
        case '1':
            {
                SAMPLE_SVP_NNIE_Segnet();
            }
            break;
        case '2':
            {
                SAMPLE_SVP_NNIE_FasterRcnn();
            }
            break;
        case '3':
            {
                SAMPLE_SVP_NNIE_FasterRcnn_DoubleRoiPooling();
            }
            break;
        case '4':
            {
                SAMPLE_SVP_NNIE_Cnn();
            }
            break;
        case '5':
            {
                SAMPLE_SVP_NNIE_Ssd();
            }
            break;
        case '6':
            {
                SAMPLE_SVP_NNIE_Yolov1();
            }
            break;
        case '7':
            {
                SAMPLE_SVP_NNIE_Yolov2();
            }
            break;
        case '8':
            {
                SAMPLE_SVP_NNIE_Lstm();
            }
            break;
        case '9':
    	    {
    		  SAMPLE_SVP_NNIE_Pvanet();
    	    }
    	    break;
        case 'a':
    	    {
    		  SAMPLE_SVP_NNIE_Rfcn_File();
    	    }
    	    break;
        default :
            {
                SAMPLE_SVP_Usage(argv[0]);
            }
            break;
    }

    return s32Ret;
}



