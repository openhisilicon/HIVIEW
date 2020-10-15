#include "audio_dl_adp.h"

#include <stdio.h>

#ifndef __HuaweiLite__
#include <dlfcn.h>
#else
#include <los_ld_elflib.h>
#endif

HI_S32 Audio_Dlpath(HI_CHAR* pChLibPath)
{
#ifndef __HuaweiLite__
#else
    if(HI_SUCCESS != LOS_PathAdd(pChLibPath))
    {
        printf("add path %s failed!\n", pChLibPath);
        return HI_FAILURE;
    }
#endif

    return HI_SUCCESS;
}

HI_S32 Audio_Dlopen(HI_VOID** pLibhandle, HI_CHAR* pChLibName)
{
    if (HI_NULL == pLibhandle || HI_NULL == pChLibName)
    {
        return HI_FAILURE;
    }

    *pLibhandle = HI_NULL;
#ifndef __HuaweiLite__
    *pLibhandle = dlopen(pChLibName, RTLD_LAZY | RTLD_LOCAL);
#else
    *pLibhandle = LOS_SoLoad(pChLibName);
#endif

    if (HI_NULL == *pLibhandle)
    {
        printf("dlopen %s failed!\n", pChLibName);
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

HI_S32 Audio_Dlsym(HI_VOID** pFunchandle, HI_VOID* Libhandle, HI_CHAR* pChFuncName)
{
    if (HI_NULL == Libhandle)
    {
        printf("LibHandle is empty!");
        return HI_FAILURE;
    }

    *pFunchandle = HI_NULL;
#ifndef __HuaweiLite__
    *pFunchandle = dlsym(Libhandle, pChFuncName);
#else
    *pFunchandle = LOS_FindSymByName(Libhandle, pChFuncName);
#endif

    if (HI_NULL == *pFunchandle)
    {
#ifndef __HuaweiLite__
        printf("dlsym %s fail,error msg is %s!\n", pChFuncName, dlerror());
#else
        printf("dlsym %s fail!\n", pChFuncName);
#endif
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

HI_S32 Audio_Dlclose(HI_VOID* Libhandle)
{
    if (HI_NULL == Libhandle)
    {
        printf("LibHandle is empty!");
        return HI_FAILURE;
    }

#ifndef __HuaweiLite__
    dlclose(Libhandle);
#else
    LOS_ModuleUnload(Libhandle);
#endif

    return HI_SUCCESS;
}
