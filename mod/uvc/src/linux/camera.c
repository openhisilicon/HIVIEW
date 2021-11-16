#include <stdio.h>
#include <string.h>
#include <pthread.h>

#include "camera.h"
#include "hiuvc.h"
#include "hiuac.h"
#include "histream.h"
#include "hiaudio.h"

/* -------------------------------------------------------------------------- */
extern unsigned int g_uvc;
extern unsigned int g_uac;
unsigned int g_uac_tid;

#ifndef __HuaweiLite__
static pthread_t g_stUVCPid;
static pthread_t g_stUACPid;
#endif

static int __init(void)
{
    sample_venc_config();

    if (get_hiuvc()->init() != 0 ||
        histream_init() != 0)
    {
        return -1;
    }

    if (g_uac)
    {
#ifdef HI_UAC_COMPILE
        sample_audio_config();
#endif

        if (get_hiuac()->init() != 0 ||
            hiaudio_init() != 0)
        {
            return -1;
        }
    }

    return 0;
}

static int __deinit(void)
{
    return 0;
}

static int __open(void)
{
    if (g_uvc)
    {
        if (get_hiuvc()->open() != 0)
        {
            return -1;
        }
    }

    if (g_uac)
    {
        if (get_hiuac()->open() != 0)
        {
            return -1;
        }
    }

    return 0;
}

static int __close(void)
{
    if (g_uvc)
    {
        get_hiuvc()->close();
        histream_shutdown();
    }

    if (g_uac)
    {
        get_hiuac()->close();
        hiaudio_shutdown();
    }

    return 0;
}

#ifndef __HuaweiLite__
void *uvc_thread(void *p)
{
    if (g_uvc)
    {
        get_hiuvc()->run();
    }
    return NULL;
}

void *uac_thread(void *p)
{
    if (g_uac)
    {
        get_hiuac()->run();
    }
    return NULL;
}

static int __run(void)
{
    pthread_create(&g_stUVCPid, 0, uvc_thread, NULL);
    pthread_create(&g_stUACPid, 0, uac_thread, NULL);
    pthread_join(g_stUVCPid, NULL);
    pthread_join(g_stUACPid, NULL);
    return 0;
}
#else
void *uac_create(unsigned int p0, unsigned int arg)
{
    get_hiuac()->run();
    return NULL;
}

static int __run(void)
{
    if (g_uac)
    {
        hicamera *camera = get_hicamera();
        TSK_INIT_PARAM_S st_task = {0};

        st_task.pfnTaskEntry = (TSK_ENTRY_FUNC)uac_create;
        st_task.uwStackSize  = LOSCFG_BASE_CORE_TSK_DEFAULT_STACK_SIZE;
        st_task.pcName       = "uac_thread";
        st_task.usTaskPrio   = 3;
        st_task.uwResved     = LOS_TASK_STATUS_DETACHED;
        st_task.auwArgs[0]   = 1;
        st_task.auwArgs[1]   = (UINT32)camera;

        if (LOS_TaskCreate(&g_uac_tid, &st_task) != LOS_OK)
        {
            printf("create uac task err!\n");
            return -1;
        }
    }

    if (g_uvc)
    {
        get_hiuvc()->run();
    }

    return 0;
}
#endif /* end of #ifndef __HuaweiLite__ */

static int __stop(void)
{
    return 0;
}

/* -------------------------------------------------------------------------- */

static hicamera __hi_camera =
{
    .init = __init,
    .deinit = __deinit,
    .open = __open,
    .close = __close,
    .run = __run,
    .stop = __stop,
};

hicamera *get_hicamera(void)
{
    return &__hi_camera;
}
