/******************************************************************************

  Copyright (C), 2001-2018, Hisilicon Tech. Co., Ltd.

 ******************************************************************************
  File Name     : aenc_mp3_adp.c
  Version       : Initial Draft
  Author        : Hisilicon multimedia software group
  Created       : 2011/02/26
  Description   :
  History       :
  1.Date        : 2011/02/26
    Author      : n00168968
    Modification: Created file

******************************************************************************/


#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#include "mpi_audio.h"
#include "audio_mp3_adp.h"

HI_S32 OpenMP3Decoder(HI_VOID *pDecoderAttr, HI_VOID **ppDecoder)
{
    ADEC_MP3_DECODER_S *pstDecoder = NULL;
    ADEC_ATTR_MP3_S *pstAttr = NULL;

    HI_ASSERT(pDecoderAttr != NULL);
    HI_ASSERT(ppDecoder != NULL);

    pstAttr = (ADEC_ATTR_MP3_S *)pDecoderAttr;

    /*allocate memory for  decoder*/
    pstDecoder = (ADEC_MP3_DECODER_S *)malloc(sizeof(ADEC_MP3_DECODER_S));
    if(NULL == pstDecoder)
    {
    	printf("%s, %d, malloc MP3 decoder context failed!\n", __FUNCTION__, __LINE__);
        return HI_ERR_ADEC_NOMEM;
    }
    memset_s(pstDecoder, sizeof(ADEC_MP3_DECODER_S), 0, sizeof(ADEC_MP3_DECODER_S));
    *ppDecoder = (HI_VOID *)pstDecoder;

    /* create decoder */
    pstDecoder->pstMP3State = MP3InitDecoder();
    if(!pstDecoder->pstMP3State)
    {
    	printf("%s, %d, MP3 decoder is not ready!\n", __FUNCTION__, __LINE__);
        return HI_ERR_ADEC_DECODER_ERR;
    }
    memcpy_s(&pstDecoder->stMP3Attr, sizeof(ADEC_ATTR_MP3_S), pstAttr, sizeof(ADEC_ATTR_MP3_S));

    return HI_SUCCESS;
}

HI_S32 DecodeMP3Frm(HI_VOID *pDecoder, HI_U8 **pu8Inbuf,HI_S32 *ps32LeftByte,
                        HI_U16 *pu16Outbuf,HI_U32 *pu32OutLen,HI_U32 *pu32Chns)
{
    HI_S32 s32Ret = HI_SUCCESS;
    ADEC_MP3_DECODER_S *pstDecoder = NULL;
    HI_S32 s32FrameLen;
    MP3FrameInfo mp3FrameInfo;


    HI_ASSERT(pDecoder != NULL);
    HI_ASSERT(pu8Inbuf != NULL);
    HI_ASSERT(ps32LeftByte != NULL);
    HI_ASSERT(pu16Outbuf != NULL);
    HI_ASSERT(pu32OutLen != NULL);
    HI_ASSERT(pu32Chns != NULL);

    *pu32Chns = 1;/*voice encoder only one channle */

    pstDecoder = (ADEC_MP3_DECODER_S *)pDecoder;

    s32FrameLen = MP3DecodeFindSyncHeader(pstDecoder->pstMP3State, pu8Inbuf, ps32LeftByte);
    if(s32FrameLen < 0)
    {
        return HI_ERR_ADEC_BUF_LACK;
    }
    s32Ret = MP3Decode(pstDecoder->pstMP3State, pu8Inbuf, ps32LeftByte, (HI_S16 *)pu16Outbuf, 0);
    if(s32Ret)
    {
        if (NO_ENOUGH_MAIN_DATA_ERROR == s32Ret)
        {
            return HI_ERR_ADEC_BUF_LACK;
        }
        return s32Ret;
    }

    MP3GetLastFrameInfo(pstDecoder->pstMP3State,&mp3FrameInfo);
    *pu32Chns = mp3FrameInfo.nChans;
    *pu32OutLen = (mp3FrameInfo.outputSamps/mp3FrameInfo.nChans)*(mp3FrameInfo.bitsPerSample/8);

    return s32Ret;
}

HI_S32 GetMP3FrmInfo(HI_VOID *pDecoder, HI_VOID *pInfo)
{
    ADEC_MP3_DECODER_S *pstDecoder = NULL;
    MP3FrameInfo stMp3FrameInfo;
    MP3_FRAME_INFO_S *pstMp3Frm = NULL;

    HI_ASSERT(pDecoder != NULL);
    HI_ASSERT(pInfo != NULL);

    pstDecoder = (ADEC_MP3_DECODER_S *)pDecoder;
    pstMp3Frm = (MP3_FRAME_INFO_S *)pInfo;

    MP3GetLastFrameInfo(pstDecoder->pstMP3State,&stMp3FrameInfo);

    pstMp3Frm->s32BitRate       = stMp3FrameInfo.bitrate;
    pstMp3Frm->s32BitsPerSample = stMp3FrameInfo.bitsPerSample;
    pstMp3Frm->s32Chans         = stMp3FrameInfo.nChans;
    pstMp3Frm->s32Layer         = stMp3FrameInfo.layer;
    pstMp3Frm->s32OutPutSamps   = stMp3FrameInfo.outputSamps;
    pstMp3Frm->s32SampRate      = stMp3FrameInfo.samprate;
    pstMp3Frm->s32Version       = stMp3FrameInfo.version;

    return HI_SUCCESS;
}

HI_S32 CloseMP3Decoder(HI_VOID *pDecoder)
{
    ADEC_MP3_DECODER_S *pstDecoder = NULL;

    HI_ASSERT(pDecoder != NULL);
    pstDecoder = (ADEC_MP3_DECODER_S *)pDecoder;

    MP3FreeDecoder(pstDecoder->pstMP3State);

    free(pstDecoder);
    return HI_SUCCESS;
}

HI_S32 ResetMP3Decoder(HI_VOID *pDecoder)
{
    ADEC_MP3_DECODER_S *pstDecoder = NULL;

    HI_ASSERT(pDecoder != NULL);
    pstDecoder = (ADEC_MP3_DECODER_S *)pDecoder;

    MP3FreeDecoder(pstDecoder->pstMP3State);

    /* create decoder*/
    pstDecoder->pstMP3State = MP3InitDecoder();
    if (!pstDecoder->pstMP3State)
    {
        printf("[Func]:%s [Line]:%d [Info]:%s\n", __FUNCTION__, __LINE__, "MP3ResetDecoder failed");
        return HI_ERR_ADEC_DECODER_ERR;
    }

    return HI_SUCCESS;
}

HI_S32 HI_MPI_ADEC_Mp3Init(HI_VOID)
{
    HI_S32 s32Handle;

    ADEC_DECODER_S stMp3;

    stMp3.enType = PT_MP3;
    hi_snprintf(stMp3.aszName, 17, sizeof(stMp3.aszName), "Mp3");
    stMp3.pfnOpenDecoder = OpenMP3Decoder;
    stMp3.pfnDecodeFrm = DecodeMP3Frm;
    stMp3.pfnGetFrmInfo = GetMP3FrmInfo;
    stMp3.pfnCloseDecoder = CloseMP3Decoder;
    stMp3.pfnResetDecoder = ResetMP3Decoder;
    return HI_MPI_ADEC_RegisterDecoder(&s32Handle, &stMp3);
}

