/******************************************************************************

  Copyright (C), 2001-2011, Hisilicon Tech. Co., Ltd.

 ******************************************************************************
  File Name     : mpi_ao.h
  Version       : Initial Draft
  Author        : Hisilicon multimedia software group
  Created       : 2009/5/5
  Description   :
  History       :
  1.Date        : 2009/5/5
    Author      : p00123320
    Modification: Created file
******************************************************************************/
#ifndef __MPI_AO_H__
#define __MPI_AO_H__

#include "hi_type.h"
#include "hi_common.h"
#include "hi_comm_aio.h"

#ifdef __cplusplus
#if __cplusplus
extern "C"
{
#endif
#endif /* __cplusplus */

    HI_S32 HI_MPI_AO_SetPubAttr(AUDIO_DEV AoDevId, const AIO_ATTR_S* pstAttr);
    HI_S32 HI_MPI_AO_GetPubAttr(AUDIO_DEV AoDevId, AIO_ATTR_S* pstAttr);

    HI_S32 HI_MPI_AO_Enable(AUDIO_DEV AoDevId);
    HI_S32 HI_MPI_AO_Disable(AUDIO_DEV AoDevId);

    HI_S32 HI_MPI_AO_EnableChn(AUDIO_DEV AoDevId, AO_CHN AoChn);
    HI_S32 HI_MPI_AO_DisableChn(AUDIO_DEV AoDevId, AO_CHN AoChn);

    HI_S32 HI_MPI_AO_SendFrame(AUDIO_DEV AoDevId, AO_CHN AoChn, const AUDIO_FRAME_S* pstData, HI_S32 s32MilliSec);

    HI_S32 HI_MPI_AO_EnableReSmp(AUDIO_DEV AoDevId, AO_CHN AoChn, AUDIO_SAMPLE_RATE_E enInSampleRate);
    HI_S32 HI_MPI_AO_DisableReSmp(AUDIO_DEV AoDevId, AO_CHN AoChn);

    HI_S32 HI_MPI_AO_ClearChnBuf(AUDIO_DEV AoDevId , AO_CHN AoChn);
    HI_S32 HI_MPI_AO_QueryChnStat(AUDIO_DEV AoDevId , AO_CHN AoChn, AO_CHN_STATE_S* pstStatus);

    HI_S32 HI_MPI_AO_PauseChn(AUDIO_DEV AoDevId, AO_CHN AoChn);
    HI_S32 HI_MPI_AO_ResumeChn(AUDIO_DEV AoDevId, AO_CHN AoChn);

    HI_S32 HI_MPI_AO_SetVolume(AUDIO_DEV AoDevId, HI_S32 s32VolumeDb);
    HI_S32 HI_MPI_AO_GetVolume(AUDIO_DEV AoDevId, HI_S32* ps32VolumeDb);

    HI_S32 HI_MPI_AO_SetMute(AUDIO_DEV AoDevId, HI_BOOL bEnable, AUDIO_FADE_S* pstFade);
    HI_S32 HI_MPI_AO_GetMute(AUDIO_DEV AoDevId, HI_BOOL* pbEnable, AUDIO_FADE_S* pstFade);

    HI_S32 HI_MPI_AO_SetTrackMode(AUDIO_DEV AoDevId, AUDIO_TRACK_MODE_E enTrackMode);
    HI_S32 HI_MPI_AO_GetTrackMode(AUDIO_DEV AoDevId, AUDIO_TRACK_MODE_E* penTrackMode);

    HI_S32 HI_MPI_AO_GetFd(AUDIO_DEV AoDevId, AO_CHN AoChn);

    HI_S32 HI_MPI_AO_SaveFile(AUDIO_DEV AoDevId, AO_CHN AoChn, AUDIO_SAVE_FILE_INFO_S* pstSaveFileInfo);
    HI_S32 HI_MPI_AO_QueryFileStatus(AUDIO_DEV AoDevId, AO_CHN AoChn, AUDIO_FILE_STATUS_S* pstFileStatus);

    HI_S32 HI_MPI_AO_ClrPubAttr(AUDIO_DEV AoDevId);
    HI_S32 HI_MPI_AO_SetVqeAttr(AUDIO_DEV AoDevId, AO_CHN AoChn, AO_VQE_CONFIG_S* pstVqeConfig);
    HI_S32 HI_MPI_AO_GetVqeAttr(AUDIO_DEV AoDevId, AO_CHN AoChn, AO_VQE_CONFIG_S* pstVqeConfig);
    HI_S32 HI_MPI_AO_EnableVqe(AUDIO_DEV AoDevId, AO_CHN AoChn);
    HI_S32 HI_MPI_AO_DisableVqe(AUDIO_DEV AoDevId, AO_CHN AoChn);




#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* __MPI_AO_H__ */

