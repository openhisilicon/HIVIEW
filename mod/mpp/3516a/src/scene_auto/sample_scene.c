
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
#include "mpi_awb.h"
#include "mpi_isp.h"
#include "hi_type.h"
#include "hi_srdk_sceneauto_define_ext.h"
#include "hi_srdk_sceneauto_ext.h"

#define CFGFILE "sceneauto.ini"


void SAMPLE_SCENE_HandleSig(HI_S32 signo)
{
    int ret;

    if (SIGINT == signo || SIGTERM == signo)
    {
        ret = HI_SRDK_SCENEAUTO_Stop();
        if (HI_SUCCESS != ret)
        {
            printf("HI_SRDK_SCENEAUTO_Stop failed\n");
            exit(-1);
        }
        ret = HI_SRDK_SCENEAUTO_DeInit();
        if (ret != 0)
        {
            printf("HI_SRDK_SCENEAUTO_DeInit failed\n");
            exit(-1);
        }
    }
    exit(-1);
}


int main(int argc, char* argv[])
{
    int ret;
    int choice;
    int specialscene;
    SRDK_SCENEAUTO_SEPCIAL_SCENE_E eSpecialScene;
    char* file_name;
    HI_BOOL b_help = 0;

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
        printf("for example: ./sample_scene ini/IPC/sceneauto_178.ini\n\n");
        printf("/***************************************************************/\n\n");
        return 0;
    }

    ret = HI_SRDK_SCENEAUTO_Init(file_name);
    if (ret != 0)
    {
        printf("HI_SRDK_SCENEAUTO_Init failed\n");
        return -1;
    }

    signal(SIGINT, SAMPLE_SCENE_HandleSig);
    signal(SIGTERM, SAMPLE_SCENE_HandleSig);

    printf("init success\n");


    while (1)
    {
        printf("1.sceneauto start\n");
        printf("2.sceneauto stop\n");
        printf("3.set specialmode\n");
        printf("4.get specialmode\n");
		printf("5.exit the sample\n");

        scanf("%d", &choice);
        switch (choice)
        {
            case 1:
                ret = HI_SRDK_SCENEAUTO_Start();
                if (HI_SUCCESS != ret)
                {
                    printf("HI_SRDK_SCENEAUTO_Start failed\n");
                    return -1;
                }
                break;
            case 2:
                ret = HI_SRDK_SCENEAUTO_Stop();
                if (HI_SUCCESS != ret)
                {
                    printf("HI_SRDK_SCENEAUTO_Stop failed\n");
                    return -1;
                }
                break;
            case 3:
                printf("please input the specialscene(0-blc, 1-ir, 2-hlc, 3-dynamic, 4-drc, 5-specialnone)");
                scanf("%d", &specialscene);
                switch (specialscene)
                {
                    case 0:
                        eSpecialScene = SRDK_SCENEAUTO_SPECIAL_SCENE_BLC;
                        ret = HI_SRDK_SCENEAUTO_SetSpecialMode(&eSpecialScene);
                        if (HI_SUCCESS != ret)
                        {
                            printf("HI_SRDK_SCENEAUTO_SetSpecialMode failed\n");
                            return -1;
                        }
                        break;
                    case 1:
                        eSpecialScene = SRDK_SCENEAUTO_SPECIAL_SCENE_IR;
                        ret = HI_SRDK_SCENEAUTO_SetSpecialMode(&eSpecialScene);
                        if (HI_SUCCESS != ret)
                        {
                            printf("HI_SRDK_SCENEAUTO_SetSpecialMode failed\n");
                            return -1;
                        }
                        break;
                    case 2:
                        eSpecialScene = SRDK_SCENEAUTO_SPECIAL_SCENE_HLC;
                        ret = HI_SRDK_SCENEAUTO_SetSpecialMode(&eSpecialScene);
                        if (HI_SUCCESS != ret)
                        {
                            printf("HI_SRDK_SCENEAUTO_SetSpecialMode failed\n");
                            return -1;
                        }
                        break;
                    case 3:
                        eSpecialScene = SRDK_SCENEAUTO_SPECIAL_SCENE_DYNAMIC;
                        ret = HI_SRDK_SCENEAUTO_SetSpecialMode(&eSpecialScene);
                        if (HI_SUCCESS != ret)
                        {
                            printf("HI_SRDK_SCENEAUTO_SetSpecialMode failed\n");
                            return -1;
                        }
                        break;
                    case 4:
                        eSpecialScene = SRDK_SCENEAUTO_SPECIAL_SCENE_DRC;
                        ret = HI_SRDK_SCENEAUTO_SetSpecialMode(&eSpecialScene);
                        if (HI_SUCCESS != ret)
                        {
                            printf("HI_SRDK_SCENEAUTO_SetSpecialMode failed\n");
                            return -1;
                        }
                        break;
                    case 5:
                        eSpecialScene = SRDK_SCENEAUTO_SPECIAL_SCENE_NONE;
                        ret = HI_SRDK_SCENEAUTO_SetSpecialMode(&eSpecialScene);
                        if (HI_SUCCESS != ret)
                        {
                            printf("HI_SRDK_SCENEAUTO_SetSpecialMode failed\n");
                            return -1;
                        }
                        break;
                    default:
                        printf("unkonw input\n");
                        break;
                }
                break;
            case 4:
                ret = HI_SRDK_SCENEAUTO_GetSpecialMode(&eSpecialScene);
                if (HI_SUCCESS != ret)
                {
                    printf("HI_SRDK_SCENEAUTO_GetSpecialMode failed\n");
                    return -1;
                }
                printf("current scene is %d\n", eSpecialScene);
                break;
			case 5:
				ret = HI_SRDK_SCENEAUTO_Stop();
				if (HI_SUCCESS != ret)
				{
					printf("HI_SRDK_SCENEAUTO_Stop failed\n");
					exit(-1);
				}
				ret = HI_SRDK_SCENEAUTO_DeInit();
				if (ret != 0)
				{
					printf("HI_SRDK_SCENEAUTO_DeInit failed\n");
					exit(-1);
				}
				exit(-1);
				break;
            default:
                printf("unkonw input\n");
                break;
        }

        sleep(1);
    }

    ret = HI_SRDK_SCENEAUTO_DeInit();
    if (ret != 0)
    {
        printf("HI_SRDK_SCENEAUTO_DeInit failed\n");
        return -1;
    }
    printf("deinit success\n");

    return 0;
}
