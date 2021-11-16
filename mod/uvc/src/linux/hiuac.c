#include "hiuac.h"
#include "hiaudio.h"

#ifdef __HuaweiLite__
#include "uac_gadget.h"
#endif

static int uac_started;

static int __init(void)
{
    return 0;
}

static int __open(void)
{
    return 0;
}

static int __close(void)
{
    hiaudio_shutdown();
    hiaudio_deinit();
    uac_started = 0;
    return 0;
}

#ifndef __HuaweiLite__
static int __run(void)
{
    hiaudio_init();
    hiaudio_startup();
    uac_started = 1;
    return 0;
}
#else
static int __run(void)
{
    int ret = wait_forever_uac_connect();
    if (ret < 0)
    {
        return -1;
    }

    uac_started = 1;

    if (uac_started)
    {
        int r = 0;
        while (uac_started)
        {
            r = run_uac_device();
            if (r < 0)
            {
                if (uac_started)
                {
                    hiaudio_shutdown();
                    hiaudio_deinit();
                    uac_started = 0;
                }
                break;
            }
            else if (r == 0)
            {
                if (uac_started)
                {
                    hiaudio_shutdown();
                    hiaudio_deinit();
                    uac_started = 0;
                }
                else
                {
                    break;
                }

                hiaudio_init();
                hiaudio_startup();
                uac_started = 1;
            }
        }
    }

    return 0;
}
#endif

/* ---------------------------------------------------------------------- */

static hiuac __hi_uac =
{
    .init = __init,
    .open = __open,
    .close = __close,
    .run = __run,
};

hiuac *get_hiuac(void)
{
    return &__hi_uac;
}
