/**
 * \file
 * \brief Describes the information about MP3DEC.
 */

#ifndef _MP3DEC_H
#define _MP3DEC_H

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif  /* __cpluscplus */
#endif  /* __cpluscplus */

#include "hi_type.h"

/********************************Macro Definition********************************/
/** \addtogroup      MP3DEC */
/** @{ */  /** <!-- [MP3DEC] */

#define MP3_MAX_NCHANS      2         /**<mp3 max number of channels*/

#define MP3_MAX_OUT_NSAMPS  1152     /**<mp3 max output samples per-frame, per-channel*/
#define MP3_MAINBUF_SIZE    4096     /**<mp3 minium size of input buffer. UNIT:bytes*/

/** @} */  /** <!-- ==== Macro Definition end ==== */

/*************************** Structure Definition ****************************/
/** \addtogroup      MP3DEC */
/** @{ */  /** <!-- [MP3DEC] */

/**Defines MP3DEC Version*/
typedef enum
{
    MPEG1 =  0,
    MPEG2 =  1,
    MPEG25 = 2
} MPEGVersion;

typedef void* HMP3Decoder;

/**Defines MP3DEC error*/
enum
{
    ERR_MP3_NONE =                 0,   /**<no decode error*/
    ERR_MP3_INDATA_UNDERFLOW =    -1,   /**<not enough input data*/
    ERR_MP3_MAINDATA_UNDERFLOW =  -2,   /**<not enough input main data*/
    ERR_MP3_FREE_BITRATE_SYNC =   -3,   /**<free mode bitrate error*/
    ERR_MP3_OUT_OF_MEMORY =       -4,   /**<decoder not enough memory*/
    ERR_MP3_NULL_POINTER =        -5,   /**<input null pointer*/
    ERR_MP3_INVALID_FRAMEHEADER = -6,   /**<invalid frame header*/
    ERR_MP3_INVALID_SIDEINFO =    -7,   /**<invalid side information*/
    ERR_MP3_INVALID_SCALEFACT =   -8,   /**<invalid scale factors*/
    ERR_MP3_INVALID_HUFFCODES =   -9,   /**<Huffman decoder error*/
    ERR_MP3_FAIL_SYNC =           -10,  /**<find sync word error*/

    ERR_MP3_UNKNOWN =             -9999 /**<reserved*/
};

/**Defines MP3DEC frame infomation*/
typedef struct _MP3FrameInfo
{
    int bitrate;              /**<output bitrate*/
    int nChans;               /**<output channels,range:1,2*/
    int samprate;             /**<output samplerate*/
    int bitsPerSample;        /**<output bitwidth*/
    int outputSamps;          /**<output samples,range:nChans*SamplePerFrame*/
    int layer;                /**<output layer*/
    int version;              /**<output version*/
} MP3FrameInfo;

/** @} */  /** <!-- ==== Structure Definition End ==== */

/******************************* API declaration *****************************/
/** \addtogroup      MP3DEC */
/** @{ */  /** <!--  [MP3DEC] */

/**
\brief create and initial decoder device.
\attention \n
Before before deocede,you must call this application programming interface (API) first.
\param N/A
\retval ::HMP3Decoder   : Success
\retval ::NULL          : FAILURE.
\see \n
N/A
*/
HMP3Decoder MP3InitDecoder(HI_VOID);

/**
\brief Free MP3 decoder.
\attention \n
\param[in] hMP3Decoder MP3decode handle
\retval \n
\see \n
N/A
*/
HI_VOID MP3FreeDecoder(HMP3Decoder hMP3Decoder);

/**
\brief Find Sync word before decode.
\attention \n
\param[in] hMP3Decoder    MP3-Decoder handle
\param[in] ppInbufPtr     address of the pointer of start-point of the bitstream(little endian format)
\param[in] pBytesLeft     pointer to BytesLeft that indicates bitstream numbers at input buffer,indicates the left bytes
\retval :: other : Success, return number bytes  of current frame
\retval ::<0 ERR_MP3_INDATA_UNDERFLOW
\see \n
N/A
*/
HI_S32 MP3DecodeFindSyncHeader(HMP3Decoder hMP3Decoder, HI_U8** ppInbufPtr, HI_S32* pBytesLeft);

/**
\brief decoding MPEG frame and output 1152(L2/L3) OR 384(L1) 16bit PCM samples per channel.
\attention \n
\param[in] hMP3Decoder    MP3-Decoder handle
\param[in] ppInbufPtr     address of the pointer of start-point of the bitstream
\param[in] pBytesLeft     pointer to BytesLeft that indicates bitstream numbers at input buffer,indicates the left bytes
\param[in] pOutPcm        the address of the out pcm buffer,pcm data in noninterlaced fotmat: L/L/L/... R/R/R/...
\param[in] nReserved      reserved
\retval :: ERR_MP3_NONE : Success
\retval :: ERROR_CODE :FAILURE
\see \n
N/A
*/
HI_S32  MP3Decode(HMP3Decoder hMP3Decoder, HI_U8** ppInbufPtr, HI_S32* pBytesLeft, HI_S16* pOutPcm, HI_S32 nReserved);

/**
\brief get the frame information.
\attention \n
\param[in] hMP3Decoder    MP3-Decoder handle
\param[out] mp3FrameInfo  frame information
\retval \n
\see \n
N/A
*/
HI_VOID MP3GetLastFrameInfo(HMP3Decoder hMP3Decoder, MP3FrameInfo* mp3FrameInfo);

/** @} */  /** <!-- ==== API declaration end ==== */

#ifdef __cplusplus
#if __cplusplus
}
#endif  /* __cpluscplus */
#endif  /* __cpluscplus */

#endif  /* _MP3DEC_H */


