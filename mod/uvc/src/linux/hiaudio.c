#include <stdlib.h>

#include "hiaudio.h"

static struct hiaudio __hi_audio = {
    .mpi_ac_ops = NULL,
    .audioing = 0,
};

hiaudio *get_hiaudio(void)
{
    return &__hi_audio;
}

int hiaudio_init(void)
{
    if (get_hiaudio()->mpi_ac_ops &&
        get_hiaudio()->mpi_ac_ops->init)
        return get_hiaudio()->mpi_ac_ops->init();

    return 0;
}

int hiaudio_deinit(void)
{
    if (get_hiaudio()->mpi_ac_ops &&
        get_hiaudio()->mpi_ac_ops->deinit)
        return get_hiaudio()->mpi_ac_ops->deinit();

    return 0;
}

int hiaudio_startup(void)
{
    int ret = 0;

    if (get_hiaudio()->mpi_ac_ops &&
        get_hiaudio()->mpi_ac_ops->startup)
        ret = get_hiaudio()->mpi_ac_ops->startup();

    if (ret == 0)
        get_hiaudio()->audioing = 1;
    return ret;
}

int hiaudio_shutdown(void)
{
    int ret = 0;

    if (get_hiaudio()->mpi_ac_ops &&
        get_hiaudio()->mpi_ac_ops->shutdown &&
        get_hiaudio()->audioing == 1)
        ret = get_hiaudio()->mpi_ac_ops->shutdown();

    if (ret == 0)
        get_hiaudio()->audioing = 0;
    return ret;
}

int hiaudio_register_mpi_ops(struct audio_control_ops *ac_ops)
{
    if (ac_ops)
    {
        get_hiaudio()->mpi_ac_ops = ac_ops;
    }

    return 0;
}
