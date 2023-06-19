/*
  Copyright (c), 2001-2022, Shenshu Tech. Co., Ltd.
 */

#include "audio_dl_adp.h"
#include <stdio.h>
#ifndef __LITEOS__
#include <dlfcn.h>
#else
#include <los_ld_elflib.h>
#endif

hi_s32 audio_dlpath(hi_char *lib_path)
{
#ifndef __LITEOS__
    (hi_void)lib_path;
#else
    if (LOS_PathAdd(lib_path) != HI_SUCCESS) {
        printf("add path %s failed!\n", lib_path);
        return HI_FAILURE;
    }
#endif

    return HI_SUCCESS;
}

hi_s32 audio_dlopen(hi_void **lib_handle, hi_char *lib_name)
{
    if ((lib_handle == HI_NULL) || (lib_name == HI_NULL)) {
        return HI_FAILURE;
    }

    *lib_handle = HI_NULL;
#ifndef __LITEOS__
    *lib_handle = dlopen(lib_name, RTLD_LAZY | RTLD_LOCAL);
#else
    *lib_handle = LOS_SoLoad(lib_name);
#endif

    if (*lib_handle == HI_NULL) {
        printf("dlopen %s failed!\n", lib_name);
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

hi_s32 audio_dlsym(hi_void **func_handle, hi_void *lib_handle, hi_char *func_name)
{
    if (lib_handle == HI_NULL) {
        printf("LibHandle is empty!");
        return HI_FAILURE;
    }

    *func_handle = HI_NULL;
#ifndef __LITEOS__
    *func_handle = dlsym(lib_handle, func_name);
#else
    *func_handle = LOS_FindSymByName(lib_handle, func_name);
#endif

    if (*func_handle == HI_NULL) {
#ifndef __LITEOS__
        printf("dlsym %s fail, error msg is %s!\n", func_name, dlerror());
#else
        printf("dlsym %s fail!\n", func_name);
#endif
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

hi_s32 audio_dlclose(hi_void *lib_handle)
{
    if (lib_handle == HI_NULL) {
        printf("LibHandle is empty!");
        return HI_FAILURE;
    }

#ifndef __LITEOS__
    dlclose(lib_handle);
#else
    LOS_ModuleUnload(lib_handle);
#endif

    return HI_SUCCESS;
}
