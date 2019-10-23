/*****************************************************************************
*             Copyright 2004 - 2050, Hisilicon Tech. Co., Ltd.
*                           ALL RIGHTS RESERVED
* FileName: aacdec.h
* Description:
*
* History:
* Version   Date         Author     DefectNum    Description
* 0.01      2006-11-01   z40717    NULL         Create this file.
*
*****************************************************************************/
/**
 * \file
 * \brief Describes the information about AACDEC.
 */

#ifndef _AACDEC_H
#define _AACDEC_H

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif  /* __cpluscplus */
#endif  /* __cpluscplus */

#include "hi_type.h"


/********************************Macro Definition********************************/
/** \addtogroup      AACDEC */
/** @{ */  /** <!-- [AACDEC] */

#ifndef AAC_MAX_NCHANS
#define AAC_MAX_NCHANS      2
#endif
#define AAC_MAX_NSAMPS      1024
#define AAC_MAINBUF_SIZE    (768 * AAC_MAX_NCHANS)  /**<according to spec (13818-7 section 8.2.2, 14496-3 section 4.5.3),6144 bits =  768 bytes per SCE or CCE-I,12288 bits = 1536 bytes per CPE*/

#define AAC_NUM_PROFILES    3
#define AAC_PROFILE_MP      0
#define AAC_PROFILE_LC      1
#define AAC_PROFILE_SSR     2

#ifndef HI_SUCCESS
#define HI_SUCCESS          0
#endif
/** @} */  /** <!-- ==== Macro Definition end ==== */

/*************************** Structure Definition ****************************/
/** \addtogroup      AACDEC */
/** @{ */  /** <!-- [AACDEC] */

/**Defines AACDEC error code*/
typedef enum
{
    ERR_AAC_NONE                          =   0,        /**<no decode error*/
    ERR_AAC_INDATA_UNDERFLOW              =  -1,        /**<not enough input data*/
    ERR_AAC_NULL_POINTER                  =  -2,        /**<null pointer*/
    ERR_AAC_INVALID_ADTS_HEADER           =  -3,        /**<invalid adts header*/
    ERR_AAC_INVALID_ADIF_HEADER           =  -4,        /**<invalid adif header*/
    ERR_AAC_INVALID_FRAME                 =  -5,        /**<invalid frame*/
    ERR_AAC_MPEG4_UNSUPPORTED             =  -6,        /**<upsupport mpeg4 format*/
    ERR_AAC_CHANNEL_MAP                   =  -7,        /**<channel map error*/
    ERR_AAC_SYNTAX_ELEMENT                =  -8,        /**<element error*/
    ERR_AAC_DEQUANT                       =  -9,        /**<dequant error*/
    ERR_AAC_STEREO_PROCESS                = -10,        /**<stereo process error*/
    ERR_AAC_PNS                           = -11,        /**<pns process error*/
    ERR_AAC_SHORT_BLOCK_DEINT             = -12,        /**<reserved*/
    ERR_AAC_TNS                           = -13,        /**<TNS process error*/
    ERR_AAC_IMDCT                         = -14,        /**<IMDCT process error*/
    ERR_AAC_NCHANS_TOO_HIGH               = -15,        /**<unsupport mutil channel*/
    ERR_AAC_SBR_INIT                      = -16,        /**<SBR init error*/
    ERR_AAC_SBR_BITSTREAM                 = -17,        /**<SBR bitstream error*/
    ERR_AAC_SBR_DATA                      = -18,        /**<SBR data error*/
    ERR_AAC_SBR_PCM_FORMAT                = -19,        /**<SBR pcm data error*/
    ERR_AAC_SBR_NCHANS_TOO_HIGH           = -20,        /**<unsupport SBR multi channel*/
    ERR_AAC_SBR_SINGLERATE_UNSUPPORTED    = -21,        /**<SBR invalid samplerate*/
    ERR_AAC_RAWBLOCK_PARAMS               = -22,        /**<invalid RawBlock params*/
    ERR_AAC_PS_INIT                       = -23,        /**<PS init error*/
    ERR_AAC_CH_MAPPING                    = -24,
    ERR_UNKNOWN               = -9999                   /**<reserved*/
} HI_AACDEC_ERR_E;

typedef struct _AACFrameInfo
{
    int bitRate;
    int nChans;                      /**<channels,range:1,2*/
    int sampRateCore;                /**<inner sample rate*/
    int sampRateOut;                 /**<output samplerate*/
    int bitsPerSample;               /**<bitwidth ,range:16*/
    int outputSamps;                 /**<output samples*/
    int profile;                     /**<profile*/
    int tnsUsed;                     /**<tns tools*/
    int pnsUsed;                     /**<pns tools*/
} AACFrameInfo;

typedef enum
{
    AACDEC_ADTS = 0,
    AACDEC_LOAS = 1,
    AACDEC_LATM_MCP1 = 2,
} AACDECTransportType;

typedef void* HAACDecoder;

typedef struct hiAACDEC_VERSION_S
{
    HI_U8 aVersion[64];
} AACDEC_VERSION_S;


/** @} */  /** <!-- ==== Structure Definition End ==== */

/******************************* API declaration *****************************/
/** \addtogroup      AACDEC */
/** @{ */  /** <!--  [AACDEC]*/

/**
\brief Get version information.
\attention \n
N/A
\param[in] pVersion    :   version describe struct
\retval ::HI_SUCCESS   :   Success
\retval ::HI_FAILURE   :   pVersion is NULL, return HI_FAILURE
\see \n
N/A
*/
HI_S32 HI_AACDEC_GetVersion(AACDEC_VERSION_S* pVersion);

/**
\brief create and initial decoder device.
\attention \n
N/A
\param[in] enTranType   : transport type
\retval ::HAACDecoder   : init success, return non-NULL handle.
\retval ::NULL          : init failure, return NULL
\see \n
N/A
*/
HAACDecoder AACInitDecoder(AACDECTransportType enTranType);

/**
\brief destroy AAC-Decoder, free the memory.
\attention \n
N/A
\param[in] hAACDecoder  : AAC-Decoder handle
\see \n
N/A
*/
HI_VOID AACFreeDecoder(HAACDecoder hAACDecoder);

/**
\brief set RawMode before decode Raw Format aac bitstream(Reserved API, unused now.)
\attention \n
N/A
\param[in] hAACDecoder  : AAC-Decoder handle
\param[in] nChans       : inout channels
\param[in] sampRate     : input sampelrate
\retval ::HI_FAILURE    : RESERVED API, always return HI_FAILURE.
\see \n
N/A
*/
HI_S32  AACSetRawMode(HAACDecoder hAACDecoder, HI_S32 nChans, HI_S32 sampRate);

/**
\brief look for valid AAC sync header
\attention \n
N/A
\param[in] hAACDecoder      : AAC-Decoder handle
\param[in/out] ppInbufPtr   : address of the pointer of start-point of the bitstream
\param[in/out] pBytesLeft   : pointer to BytesLeft that indicates bitstream numbers at input buffer
\retval ::<0                : err, always return ERR_AAC_INDATA_UNDERFLOW
\retval ::other             : Success, return number bytes of current frame
\see \n
N/A
*/
HI_S32 AACDecodeFindSyncHeader(HAACDecoder hAACDecoder, HI_U8** ppInbufPtr, HI_S32* pBytesLeft);

/**
\brief decoding AAC frame and output 1024(LC) OR 2048(HEAAC/eAAC/eAAC+) 16bit PCM samples per channel.
\attention \n
\param[in] hAACDecoder       : AAC-Decoder handle
\param[in] ppInbufPtr        : address of the pointer of start-point of the bitstream
\param[in/out] pBytesLeft    : pointer to BytesLeft that indicates bitstream numbers at input buffer,indicates the left bytes
\param[in] pOutPcm           : the address of the out pcm buffer,pcm data in noninterlaced fotmat: L/L/L/... R/R/R/...
\retval :: SUCCESS           : Success
\retval :: ERROR_CODE        : FAILURE, return error_code.
\see \n
N/A
*/
HI_S32  AACDecodeFrame(HAACDecoder hAACDecoder, HI_U8** ppInbufPtr, HI_S32* pBytesLeft, HI_S16* pOutPcm);

/**
\brief get the frame information.
\attention \n
\param[in] hAACDecoder       : AAC-Decoder handle
\param[out] aacFrameInfo     : frame information
\retval :: HI_SUCCESS        : Success
\retval :: ERROR_CODE        : FAILURE, return error_code.
\see \n
N/A
*/
HI_S32 AACGetLastFrameInfo(HAACDecoder hAACDecoder, AACFrameInfo* aacFrameInfo);

/**
\brief set eosflag.
\attention \n
\param[in] hAACDecoder       : AAC-Decoder handle
\param[in] s32Eosflag        : end flag
\retval :: HI_SUCCESS        : Success
\retval :: ERROR_CODE        : FAILURE, return error_code.
\see \n
N/A
*/
HI_S32 AACDecoderSetEosFlag(HAACDecoder hAACDecoder, HI_S32 s32Eosflag);

/**
\brief flush internal codec state (after seeking, for example)
\attention \n
\param[in] hAACDecoder       : AAC-Decoder handle
\retval :: HI_SUCCESS        : Success
\retval :: ERROR_CODE        : FAILURE, return error_code.
\see \n
N/A
*/
HI_S32  AACFlushCodec(HAACDecoder hAACDecoder);

/** @} */  /** <!-- ==== API declaration end ==== */


#ifdef __cplusplus
#if __cplusplus
}
#endif  /* __cpluscplus */
#endif  /* __cpluscplus */

#endif  /* _AACDEC_H */
