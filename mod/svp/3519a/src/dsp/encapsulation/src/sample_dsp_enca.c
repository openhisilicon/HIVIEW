#include "sample_dsp_enca.h"
#include "mpi_dsp.h"
#include <string.h>
/*****************************************************************************
*    Prototype     : SAMPLE_SVP_DSP_ENCA_Dilate3x3
*    Description : Encapsulate Dilate 3x3
*
*    Parameters     :  SVP_DSP_HANDLE      *phHandle           Handle
*                      SVP_SRC_IMAGE_S     *pstSrc             Input image
*                      SVP_DST_IMAGE_S     *pstDst             Output image
*                      SVP_DSP_ID_E         enDspId             DSP Core ID.
*                      SVP_DSP_PRI_E        enPri               Priority
*                      SVP_MEM_INFO_S      *pstAssistBuf       Assist buffer
*
*    Return Value : HI_SUCCESS: Success;Error codes: Failure.
*    Spec         :
*
*    History:
*
*        1.    Date         : 2017-11-06
*            Author         :
*            Modification : Created function
*
****************************************************************************/
HI_S32 SAMPLE_SVP_DSP_ENCA_Dilate3x3(SVP_DSP_HANDLE *phHandle,SVP_DSP_ID_E enDspId,SVP_DSP_PRI_E enPri,
                    SVP_SRC_IMAGE_S *pstSrc,SVP_DST_IMAGE_S *pstDst,SVP_MEM_INFO_S *pstAssistBuf)
{
   SVP_DSP_MESSAGE_S stMsg = {0};
   HI_U8 *pu8Tmp =  NULL;
   /*Check parameter,But,we do not it in here*/
   /*Fill Message*/
   stMsg.u32CMD = SVP_DSP_CMD_DILATE_3X3;
   stMsg.u32MsgId = 0;
   stMsg.u64Body = pstAssistBuf->u64PhyAddr;
   stMsg.u32BodyLen = sizeof(SVP_SRC_IMAGE_S) + sizeof(SVP_DST_IMAGE_S); /*SRC + DST*/

   pu8Tmp = (HI_U8*)(HI_UL)pstAssistBuf->u64VirAddr;
   memcpy(pu8Tmp,pstSrc,sizeof(*pstSrc));
   pu8Tmp += sizeof(*pstSrc);
   memcpy(pu8Tmp,pstDst,sizeof(*pstDst));
   /*It must flush cache ,if the buffer pstAssistBuf.u64VirAddr malloc with cache!*/
   return HI_MPI_SVP_DSP_RPC(phHandle,&stMsg, enDspId, enPri);
}

/*****************************************************************************
*    Prototype     : SAMPLE_SVP_DSP_ENCA_Erode3x3
*    Description : Encapsulate Erode 3x3
*
*    Parameters     :  SVP_DSP_HANDLE      *phHandle           Handle
*                      SVP_SRC_IMAGE_S     *pstSrc             Input image
*                      SVP_DST_IMAGE_S     *pstDst             Output image
*                      SVP_DSP_ID_E         enDspId            DSP Core ID.
*                      SVP_DSP_PRI_E        enPri              Priority
*                      SVP_MEM_INFO_S      *pstAssistBuf       Assist buffer
*
*    Return Value : HI_SUCCESS: Success;Error codes: Failure.
*    Spec         :
*
*    History:
*
*        1.    Date         : 2017-11-06
*            Author         :
*            Modification : Created function
*
****************************************************************************/
HI_S32 SAMPLE_SVP_DSP_ENCA_Erode3x3(SVP_DSP_HANDLE *phHandle,SVP_DSP_ID_E enDspId,SVP_DSP_PRI_E enPri,
                    SVP_SRC_IMAGE_S *pstSrc,SVP_DST_IMAGE_S *pstDst,SVP_MEM_INFO_S *pstAssistBuf)
{
    SVP_DSP_MESSAGE_S stMsg = {0};
    HI_U8 *pu8Tmp =  NULL;
    /*Check parameter,But,we do not it in here*/
    /*Fill Message*/
    stMsg.u32CMD = SVP_DSP_CMD_ERODE_3X3;
    stMsg.u32MsgId = 0;
    stMsg.u64Body = pstAssistBuf->u64PhyAddr;
    stMsg.u32BodyLen = sizeof(SVP_SRC_IMAGE_S) + sizeof(SVP_DST_IMAGE_S); /*SRC + DST*/

    pu8Tmp = (HI_U8*)(HI_UL)pstAssistBuf->u64VirAddr;
    memcpy(pu8Tmp,pstSrc,sizeof(*pstSrc));
    pu8Tmp += sizeof(*pstSrc);
    memcpy(pu8Tmp,pstDst,sizeof(*pstDst));
    /*It must flush cache ,if the buffer pstAssistBuf.u64VirAddr malloc with cache!*/
    return HI_MPI_SVP_DSP_RPC(phHandle,&stMsg, enDspId, enPri);
}



