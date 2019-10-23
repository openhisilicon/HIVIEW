
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

#include "hi_comm_isp.h"
//#include "mpi_awb.h"
//#include "mpi_isp.h"
#include "hi_type.h"
#include "hi_sceneauto_define_ext.h"
#include "hi_sceneauto_ext.h"

#define CFGFILE "sceneauto.ini"


void SAMPLE_SCENE_HandleSig(HI_S32 signo)
{
    int ret;

    if (SIGINT == signo || SIGTERM == signo)
    {
        ret = HI_SCENEAUTO_DeInit();
        if (ret != 0)
        {
            printf("HI_SCENEAUTO_DeInit failed\n");
            exit(-1);
        }
    }
    exit(-1);
}


#define FILENAME_SIZE (128)

#ifdef __HuaweiLite__
int app_main(int argc, char *argv[])
#else
int sample_scene_main(int argc, char* argv[])
#endif
{
    int ret = -1;
    int choice = -1;
    int specialscene = -1;
    SCENEAUTO_SEPCIAL_SCENE_E eSpecialScene = 0;
    char* file_name;
	char input[10];
    HI_BOOL b_help = 0;
    char acFileName[FILENAME_SIZE] = {0};

    file_name = (char*)CFGFILE;
    
    if (argc > 1)
    {
        file_name = argv[1];
        if (!strcmp(argv[1], "-h"))
        {
            b_help = 1;
        }
    }
    else
    {
        b_help = 1;
    }

    if (b_help)
    {
        printf("/***************************************************************/\n\n");
        printf("usage: ./sample_scene ini_path.\n\n");
        printf("/***************************************************************/\n\n");
        return 0;
    }

    if (strlen(file_name) >= FILENAME_SIZE)
    {
        printf("The size of ini_path is too long!\n");
        return 0;
    }

    memcpy(acFileName, file_name, FILENAME_SIZE * sizeof(char));

    ret = HI_SCENEAUTO_Init(acFileName);
    if (ret != 0)
    {
        printf("HI_SCENEAUTO_Init failed\n");
        return -1;
    }
    
#ifndef __HuaweiLite__
    signal(SIGINT, SAMPLE_SCENE_HandleSig);
    signal(SIGTERM, SAMPLE_SCENE_HandleSig);
#else
   
#endif
    printf("init success\n");

#if 1
    while (1)
    {
        printf("1.sceneauto start\n");
        printf("2.sceneauto stop\n");
        printf("3.set specialmode\n");
        printf("4.get specialmode\n");
		printf("5.exit the sample\n");

        choice = -1;
        if (HI_NULL == fgets(input, 10, stdin))
        {
            printf("fgets failed\n");
            return -1;
        }
		sscanf(input, "%d", &choice);

        switch (choice)
        {
            case 1:
                ret = HI_SCENEAUTO_Start();
                if (HI_SUCCESS != ret)
                {
                    printf("HI_SRDK_SCENEAUTO_Start failed\n");
                    return -1;
                }
                break;
            case 2:
                ret = HI_SCENEAUTO_Stop();
                if (HI_SUCCESS != ret)
                {
                    printf("HI_SRDK_SCENEAUTO_Stop failed\n");
                    return -1;
                }
                break;
            case 3:
                printf("please input the specialscene(1-ir, 2-hlc, 3-auto_fswdr, 4-manual_longframe, 5-manual_nomalwdr, 6-traffic, 7-face, 0-specialnone)");
                specialscene = -1;
                fgets(input, 10, stdin);
				sscanf(input, "%1d", &specialscene);
                switch (specialscene)
                {
                    case 1:
                        eSpecialScene = SCENEAUTO_SPECIAL_SCENE_IR;
                        break;
                    case 2:
                        eSpecialScene = SCENEAUTO_SPECIAL_SCENE_HLC;
                        break;
                    case 3:
                        eSpecialScene = SCENEAUTO_SPECIAL_SCENE_AUTO_FSWDR;
                        break;
                    case 4:
                        eSpecialScene = SCENEAUTO_SPECIAL_SCENE_MANUAL_LONG_FRAME;
                        break;
                    case 5:
                        eSpecialScene = SCENEAUTO_SPECIAL_SCENE_MANUAL_NORMAL_WDR;
                        break;
                    case 6:
                        eSpecialScene = SCENEAUTO_SPECIAL_SCENE_TRAFFIC;
                        break;
                    case 7:
                        eSpecialScene = SCENEAUTO_SPECIAL_SCENE_FACE;
                        break;
                    case 0:
                        eSpecialScene = SCENEAUTO_SPECIAL_SCENE_NONE;
                        break;
                    default:
                        printf("unknown input\n");
                        break;
                }
                ret = HI_SCENEAUTO_SetSpecialMode(&eSpecialScene);
                if (HI_SUCCESS != ret)
                {
                    printf("HI_SCENEAUTO_SetSpecialMode failed\n");
                    return -1;
                }
                break;
            case 4:
                ret = HI_SCENEAUTO_GetSpecialMode(&eSpecialScene);
                if (HI_SUCCESS != ret)
                {
                    printf("HI_SCENEAUTO_SetSpecialMode failed\n");
                    return -1;
                }
                printf("current scene is %d\n", eSpecialScene);
                break;
			case 5:
				ret = HI_SCENEAUTO_Stop();
				if (HI_SUCCESS != ret)
				{
					printf("HI_SCENEAUTO_Stop failed\n");
					exit(-1);
				}
				ret = HI_SCENEAUTO_DeInit();
				if (ret != 0)
				{
					printf("HI_SCENEAUTO_DeInit failed\n");
					exit(-1);
				}
				exit(-1);
				break;
            default:
                printf("unknown input\n");
                break;
        }

        sleep(1);
    }
#endif

    return 0;
}
