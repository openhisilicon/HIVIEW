#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <limits.h>

#include "hi_type.h"
#include "hi_scene.h"
#include "hi_scene_loadparam.h"
#include "hi_scenecomm_log.h"
#include "sample_comm.h"
#include "hi_scene_prev.h"

HI_SCENE_PARAM_S g_stSceneParam;

HI_VOID SAMPLE_SCENE_HandleSig(HI_S32 signo)
{
    HI_S32 s32ret;

    if (SIGINT == signo || SIGTERM == signo)
    {
        s32ret = HI_SCENE_Deinit();
        if (s32ret != 0)
        {
            printf("HI_SCENE_DeInit failed\n");
            exit(-1);
        }
    }
    exit(-1);
}


#ifdef __HuaweiLite__
HI_S32 app_main(HI_S32 argc, HI_CHAR *argv[])
#else
HI_S32 scene_sample_main(HI_S32 argc, HI_CHAR *argv[])
#endif
{
    HI_S32 s32ret = HI_SUCCESS;
    HI_S32 s32choice = -1;
    HI_CHAR aszinput[10];
    HI_CHAR *pszdirname;
    HI_SCENE_VIDEO_MODE_S stVideoMode;

    if (argc < 2)
    {
        printf("Usage : %s <inidir>\n\t\tfor example :./sample_scene ./param/sensor_imx290\n", argv[0]);
        return HI_SUCCESS;
    }

    pszdirname = argv[1];

#ifndef __HuaweiLite__
    signal(SIGINT, SAMPLE_SCENE_HandleSig);
    signal(SIGTERM, SAMPLE_SCENE_HandleSig);
#else
    SCENE_PREV_S stScenePrev;
    s32ret = SAMPLE_SCENE_START_PREV(&stScenePrev);
    if (HI_SUCCESS == s32ret)
    {
        SAMPLE_PRT("ISP is now running normally\n");
    }
    else
    {
        SAMPLE_PRT("ISP is not running normally!Please check it\n");
        return -1;
    }
    printf("Please waite for a while\n");

    sleep(5);

#endif

    printf("init success\n");

    while (1)
    {
        printf("1.scene start\n");
        printf("2.scene pause and reset media route\n");
        printf("3.scene resume and set a new media type\n");
        printf("4.exit the sample\n");

        s32choice = -1;
        fgets(aszinput, 10, stdin);
        sscanf(aszinput, "%d", &s32choice);


        switch (s32choice)
        {
            case 1:
                printf("When We Start SceneAuto, we neen to set video Pipe mode\n");
                printf("Please input videomode index, without which we couldn't work  effectively.\n");
                s32choice = -1;
                fgets(aszinput, 10, stdin);
                sscanf(aszinput, "%d", &s32choice);
                printf("videomode type has already been input.\n");
                s32ret = HI_SCENE_CreateParam(pszdirname,&g_stSceneParam, &stVideoMode);  //stSceneParam & stVideoMode get value
                if (HI_SUCCESS != s32ret)
                {
                    printf("SCENETOOL_CreateParam failed\n");
                    return HI_FAILURE;
                }

                s32ret = HI_SCENE_Init(&g_stSceneParam);
                if (HI_SUCCESS != s32ret)
                {
                    printf("HI_SCENE_Init failed\n");
                    return HI_FAILURE;
                }

                s32ret = HI_SCENE_SetSceneMode(&(stVideoMode.astVideoMode[s32choice]));
                if (HI_SUCCESS != s32ret)
                {
                    printf("HI_SRDK_SCENEAUTO_Start failed\n");
                    return HI_FAILURE;
                }
                printf("The sceneauto is started already.\n");
                break;
            case 2:
                s32ret = HI_SCENE_Pause(HI_TRUE);
                if (HI_SUCCESS != s32ret)
                {
                    printf("HI_SCENE_Pause failed\n");
                    return HI_FAILURE;
                }
                printf("The sceneauto is pause.\n");
				printf("If you want to change videomode, please enter 3.\n");
                break;
            case 3:
				printf("Please input videomode index, without which we couldn't work  effectively.\n");
                s32choice = -1;
                fgets(aszinput, 10, stdin);
                sscanf(aszinput, "%d", &s32choice);
                printf("videomode type has already been input.\n");

                s32ret = HI_SCENE_SetSceneMode(&(stVideoMode.astVideoMode[s32choice]));
                if (HI_SUCCESS != s32ret)
                {
                    printf("HI_SRDK_SCENEAUTO_Start failed\n");
                    return HI_FAILURE;
                }

                s32ret = HI_SCENE_Pause(HI_FALSE);
                if (HI_SUCCESS != s32ret)
                {
                    printf("HI_SCENE_Resume failed\n");
                    return HI_FAILURE;
                }
                printf("The sceneauto is started already.\n");
                break;
            case 4:
                s32ret = HI_SCENE_Deinit();
                if (HI_SUCCESS != s32ret)
                {
                    printf("HI_SCENE_Deinit failed\n");
                    return HI_FAILURE;
                }
                printf("The scene sample is end.\n");
                goto EXIT;
            default:
                printf("unknown input\n");
                break;
        }

        sleep(1);
    }

EXIT:
#ifdef __HuaweiLite__
    SAMPLE_SCENE_STOP_PREV(&stScenePrev);
#endif
    return HI_SUCCESS;


}
