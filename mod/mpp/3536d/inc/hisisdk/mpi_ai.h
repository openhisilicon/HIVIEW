/******************************************************************************

  Copyright (C), 2001-2011, Hisilicon Tech. Co., Ltd.

 ******************************************************************************
  File Name     : mpi_ai.h
  Version       : Initial Draft
  Author        : Hisilicon multimedia software group
  Created       : 2009/5/5
  Description   :
  History       :
  1.Date        : 2009/5/5
    Author      : p00123320
    Modification: Created file
******************************************************************************/
#ifndef __MPI_AI_H__
#define __MPI_AI_H__

#include "hi_type.h"
#include "hi_common.h"
#include "hi_comm_aio.h"

#ifdef __cplusplus
#if __cplusplus
extern "C"
{
#endif
#endif /* __cplusplus */

    HI_S32 HI_MPI_AI_SetPubAttr(AUDIO_DEV AiDevId, const AIO_ATTR_S* pstAttr);
    HI_S32 HI_MPI_AI_GetPubAttr(AUDIO_DEV AiDevId, AIO_ATTR_S* pstAttr);

    HI_S32 HI_MPI_AI_Enable(AUDIO_DEV AiDevId);
    HI_S32 HI_MPI_AI_Disable(AUDIO_DEV AiDevId);

    HI_S32 HI_MPI_AI_EnableChn(AUDIO_DEV AiDevId, AI_CHN AiChn);
    HI_S32 HI_MPI_AI_DisableChn(AUDIO_DEV AiDevId, AI_CHN AiChn);

    HI_S32 HI_MPI_AI_GetFrame(AUDIO_DEV AiDevId, AI_CHN AiChn, AUDIO_FRAME_S* pstFrm, AEC_FRAME_S* pstAecFrm, HI_S32 s32MilliSec);
    HI_S32 HI_MPI_AI_ReleaseFrame(AUDIO_DEV AiDevId, AI_CHN AiChn, AUDIO_FRAME_S* pstFrm, AEC_FRAME_S* pstAecFrm);

    HI_S32 HI_MPI_AI_SetChnParam(AUDIO_DEV AiDevId, AI_CHN AiChn, AI_CHN_PARAM_S* pstChnParam);
    HI_S32 HI_MPI_AI_GetChnParam(AUDIO_DEV AiDevId, AI_CHN AiChn, AI_CHN_PARAM_S* pstChnParam);

    HI_S32 HI_MPI_AI_SetVqeAttr(AUDIO_DEV AiDevId, AI_CHN AiChn, AUDIO_DEV AoDevId, AO_CHN AoChn, AI_VQE_CONFIG_S* pstVqeConfig);
    HI_S32 HI_MPI_AI_GetVqeAttr(AUDIO_DEV AiDevId, AI_CHN AiChn, AI_VQE_CONFIG_S* pstVqeConfig);
    HI_S32 HI_MPI_AI_EnableVqe(AUDIO_DEV AiDevId, AI_CHN AiChn);
    HI_S32 HI_MPI_AI_DisableVqe(AUDIO_DEV AiDevId, AI_CHN AiChn);

    HI_S32 HI_MPI_AI_EnableReSmp(AUDIO_DEV AiDevId, AI_CHN AiChn, AUDIO_SAMPLE_RATE_E enOutSampleRate);
    HI_S32 HI_MPI_AI_DisableReSmp(AUDIO_DEV AiDevId, AI_CHN AiChn);

    HI_S32 HI_MPI_AI_SetTrackMode(AUDIO_DEV AiDevId, AUDIO_TRACK_MODE_E enTrackMode);
    HI_S32 HI_MPI_AI_GetTrackMode(AUDIO_DEV AiDevId, AUDIO_TRACK_MODE_E* penTrackMode);
    HI_S32 HI_MPI_AI_SaveFile(AUDIO_DEV AiDevId, AI_CHN AiChn, AUDIO_SAVE_FILE_INFO_S* pstSaveFileInfo);
    HI_S32 HI_MPI_AI_QueryFileStatus(AUDIO_DEV AiDevId, AI_CHN AiChn, AUDIO_FILE_STATUS_S* pstFileStatus);

    HI_S32 HI_MPI_AI_ClrPubAttr(AUDIO_DEV AiDevId);

    HI_S32 HI_MPI_AI_GetFd(AUDIO_DEV AiDevId, AI_CHN AiChn);

    HI_S32 HI_MPI_AI_SetVqeVolume(AUDIO_DEV AiDevId, AI_CHN AiChn, HI_S32 s32VolumeDb);
    HI_S32 HI_MPI_AI_GetVqeVolume(AUDIO_DEV AiDevId, AI_CHN AiChn, HI_S32* ps32VolumeDb);




#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* __MPI_AI_H__ */

