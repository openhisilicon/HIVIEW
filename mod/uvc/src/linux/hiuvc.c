#include <pthread.h>
#include <stdio.h>

#include "hiuvc.h"
#include "uvc_gadgete.h"

#ifndef __HuaweiLite__
static pthread_t g_stUVC_Send_Data_Pid;
#endif

static int __init(void)
{
    return 0;
}

static int __open(void)
{
    return open_uvc_device();
}

static int __close(void)
{
    return close_uvc_device();
}

#ifndef __HuaweiLite__
void *uvc_send_data_thread(void *p)
{
    int status = 0;
    int running = 1;

    while (running)
    {
        status = run_uvc_data();
    }

    printf("uvc_send_data_thread exit, status: %d.\n", status);

    return NULL;
}

static int __run(void)
{
    int running = 1;
    int status = 0;

    pthread_create(&g_stUVC_Send_Data_Pid, 0, uvc_send_data_thread, NULL);

    while (running)
    {
        status = run_uvc_device();
        if (status < 0)
        {
            break;
        }
    }

    pthread_join(g_stUVC_Send_Data_Pid, NULL);

    return 0;
}
#else
static int __run(void)
{
    int running = 1;
    int status = 0;

    if (wait_forever_usb_connect() < 0)
    {
        return -1;
    }

    while (running)
    {
        status = run_uvc_device();
        if (status < 0)
        {
            get_hiuvc()->close();
            break;
        }
        else if (status == 0)
        {
            get_hiuvc()->close();
            get_hiuvc()->init();
            get_hiuvc()->open();
            wait_forever_usb_connect();
        }
    }

    return 0;
}
#endif /* end of #ifndef __HuaweiLite__ */

/* ---------------------------------------------------------------------- */

static hiuvc __hi_uvc =
{
    .init = __init,
    .open = __open,
    .close = __close,
    .run = __run,
};

hiuvc *get_hiuvc()
{
    return &__hi_uvc;
}
