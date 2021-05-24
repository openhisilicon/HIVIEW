#ifndef __SAMPLE_COMM_IVE_H__
#define __SAMPLE_COMM_IVE_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>

#include "hi_debug.h"
#include "hi_comm_ive.h"
#include "mpi_ive.h"
#include "sample_comm.h"

#define VIDEO_WIDTH 352
#define VIDEO_HEIGHT 288
#define IVE_ALIGN 16
#define IVE_CHAR_CALW 8
#define IVE_CHAR_CALH 8
#define IVE_CHAR_NUM (IVE_CHAR_CALW *IVE_CHAR_CALH)
#define IVE_FILE_NAME_LEN 256
#define IVE_RECT_NUM   64
#define VPSS_CHN_NUM 2

#define SAMPLE_ALIGN_BACK(x, a)     ((a) * (((x) / (a))))

typedef struct hiSAMPLE_IVE_SWITCH_S
{
   HI_BOOL bVenc;
   HI_BOOL bVo;
}SAMPLE_IVE_SWITCH_S;

typedef struct hiSAMPLE_IVE_RECT_S
{
    POINT_S astPoint[4];
} SAMPLE_IVE_RECT_S;

typedef struct hiSAMPLE_RECT_ARRAY_S
{
    HI_U16 u16Num;
    SAMPLE_IVE_RECT_S astRect[IVE_RECT_NUM];
} SAMPLE_RECT_ARRAY_S;

typedef struct hiIVE_LINEAR_DATA_S
{
    HI_S32 s32LinearNum;
    HI_S32 s32ThreshNum;
    POINT_S* pstLinearPoint;
} IVE_LINEAR_DATA_S;

typedef struct hiSAMPLE_IVE_DRAW_RECT_MSG_S
{
    VIDEO_FRAME_INFO_S stFrameInfo;
    SAMPLE_RECT_ARRAY_S stRegion;
} SAMPLE_IVE_DRAW_RECT_MSG_S;

//free mmz
#define IVE_MMZ_FREE(phy,vir)\
    do{\
        if ((0 != (phy)) && (0 != (vir)))\
        {\
            HI_MPI_SYS_MmzFree((phy),(HI_VOID *)(HI_UL)(vir));\
            (phy) = 0;\
            (vir) = 0;\
        }\
    }while(0)

#define IVE_CLOSE_FILE(fp)\
    do{\
        if (NULL != (fp))\
        {\
            fclose((fp));\
            (fp) = NULL;\
        }\
    }while(0)

#define SAMPLE_PAUSE()\
    do {\
        printf("---------------press Enter key to exit!---------------\n");\
        (void)getchar();\
    } while (0)
#define SAMPLE_CHECK_EXPR_RET(expr, ret, fmt...)\
do\
{\
    if(expr)\
    {\
        SAMPLE_PRT(fmt);\
        return (ret);\
    }\
}while(0)
#define SAMPLE_CHECK_EXPR_GOTO(expr, label, fmt...)\
do\
{\
    if(expr)\
    {\
        SAMPLE_PRT(fmt);\
        goto label;\
    }\
}while(0)

#define SAMPLE_COMM_IVE_CONVERT_64BIT_ADDR(Type,Addr) (Type*)(HI_UL)(Addr)

/******************************************************************************
* function : Mpi init
******************************************************************************/
HI_VOID SAMPLE_COMM_IVE_CheckIveMpiInit(HI_VOID);
/******************************************************************************
* function : Mpi exit
******************************************************************************/
HI_S32 SAMPLE_COMM_IVE_IveMpiExit(HI_VOID);
/******************************************************************************
* function :Read file
******************************************************************************/
HI_S32 SAMPLE_COMM_IVE_ReadFile(IVE_IMAGE_S* pstImg, FILE* pFp);
/******************************************************************************
* function :Write file
******************************************************************************/
HI_S32 SAMPLE_COMM_IVE_WriteFile(IVE_IMAGE_S* pstImg, FILE* pFp);
/******************************************************************************
* function :Calc stride
******************************************************************************/
HI_U16 SAMPLE_COMM_IVE_CalcStride(HI_U32 u32Width, HI_U8 u8Align);
/******************************************************************************
* function : Copy blob to rect
******************************************************************************/
HI_VOID SAMPLE_COMM_IVE_BlobToRect(IVE_CCBLOB_S *pstBlob, SAMPLE_RECT_ARRAY_S *pstRect,
                                            HI_U16 u16RectMaxNum,HI_U16 u16AreaThrStep,
                                            HI_U32 u32SrcWidth, HI_U32 u32SrcHeight,
                                            HI_U32 u32DstWidth,HI_U32 u32DstHeight);
/******************************************************************************
* function : Create ive image
******************************************************************************/
HI_S32 SAMPLE_COMM_IVE_CreateImage(IVE_IMAGE_S* pstImg, IVE_IMAGE_TYPE_E enType,
                                   HI_U32 u32Width, HI_U32 u32Height);
/******************************************************************************
* function : Create memory info
******************************************************************************/
HI_S32 SAMPLE_COMM_IVE_CreateMemInfo(IVE_MEM_INFO_S* pstMemInfo, HI_U32 u32Size);
/******************************************************************************
* function : Create ive image by cached
******************************************************************************/
HI_S32 SAMPLE_COMM_IVE_CreateImageByCached(IVE_IMAGE_S* pstImg,
        IVE_IMAGE_TYPE_E enType, HI_U32 u32Width, HI_U32 u32Height);
/******************************************************************************
* function : Create IVE_DATA_S
******************************************************************************/
HI_S32 SAMPLE_COMM_IVE_CreateData(IVE_DATA_S* pstData,HI_U32 u32Width, HI_U32 u32Height);
/******************************************************************************
* function : Init Vb
******************************************************************************/
HI_S32 SAMPLE_COMM_IVE_VbInit(PIC_SIZE_E *paenSize,SIZE_S *pastSize,HI_U32 u32VpssChnNum);
/******************************************************************************
* function : Dma frame info to  ive image
******************************************************************************/
HI_S32 SAMPLE_COMM_IVE_DmaImage(VIDEO_FRAME_INFO_S *pstFrameInfo,IVE_DST_IMAGE_S *pstDst,HI_BOOL bInstant);

/******************************************************************************
* function : Call vgs to fill rect
******************************************************************************/
HI_S32 SAMPLE_COMM_VGS_FillRect(VIDEO_FRAME_INFO_S* pstFrmInfo, SAMPLE_RECT_ARRAY_S* pstRect, HI_U32 u32Color);

/******************************************************************************
* function : Start Vpss
******************************************************************************/
HI_S32 SAMPLE_COMM_IVE_StartVpss(SIZE_S *pastSize,HI_U32 u32VpssChnNum);
/******************************************************************************
* function : Stop Vpss
******************************************************************************/
HI_VOID SAMPLE_COMM_IVE_StopVpss(HI_U32 u32VpssChnNum);
/******************************************************************************
* function : Start Vo
******************************************************************************/
HI_S32 SAMPLE_COMM_IVE_StartVo(HI_VOID);
/******************************************************************************
* function : Stop Vo
******************************************************************************/
HI_VOID SAMPLE_COMM_IVE_StopVo(HI_VOID);
/******************************************************************************
* function : Start Vi/Vpss/Venc/Vo
******************************************************************************/
HI_S32 SAMPLE_COMM_IVE_StartViVpssVencVo(SAMPLE_VI_CONFIG_S *pstViConfig,SAMPLE_IVE_SWITCH_S *pstSwitch,PIC_SIZE_E *penExtPicSize);
/******************************************************************************
* function : Stop Vi/Vpss/Venc/Vo
******************************************************************************/
HI_VOID SAMPLE_COMM_IVE_StopViVpssVencVo(SAMPLE_VI_CONFIG_S *pstViConfig,SAMPLE_IVE_SWITCH_S *pstSwitch);

#endif


