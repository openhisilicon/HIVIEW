#ifndef __AUDIO_DL_ADP_H__
#define __AUDIO_DL_ADP_H__

#include "hi_type.h"

HI_S32 Audio_Dlpath(HI_CHAR* pChLibPath);

HI_S32 Audio_Dlopen(HI_VOID** pLibhandle, HI_CHAR* pChLibName);

HI_S32 Audio_Dlsym(HI_VOID** pFunchandle, HI_VOID* Libhandle, HI_CHAR* pChFuncName);

HI_S32 Audio_Dlclose(HI_VOID* Libhandle);

#endif