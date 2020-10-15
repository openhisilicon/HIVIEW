#ifndef _SAMPLE_SVP_USER_KERNEL_H_
#define _SAMPLE_SVP_USER_KERNEL_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "hi_comm_svp.h"
#include "hi_nnie.h"
#include "mpi_nnie.h"
#include "sample_comm_svp.h"
#include "sample_comm_nnie.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>


#define SAMPLE_SVP_NNIE_ADDR_ALIGN_16          16  /*16 byte alignment*/
#define SAMPLE_SVP_NNIE_MAX(a,b)    (((a) > (b)) ? (a) : (b))
#define SAMPLE_SVP_NNIE_MIN(a,b)    (((a) < (b)) ? (a) : (b))
#define SAMPLE_SVP_NNIE_SIGMOID(x)   (HI_FLOAT)(1.0f/(1+exp(-x)))

#define SAMPLE_SVP_NNIE_COORDI_NUM  4      /*coordinate numbers*/
#define SAMPLE_SVP_NNIE_PROPOSAL_WIDTH  6  /*the number of proposal values*/
#define SAMPLE_SVP_NNIE_QUANT_BASE 4096    /*the base value*/
#define SAMPLE_SVP_NNIE_SCORE_NUM  2       /*the num of RPN scores*/
#define SAMPLE_SVP_NNIE_HALF 0.5f          /*the half value*/
#define SAMPLE_SVP_NNIE_YOLOV3_REPORT_BLOB_NUM    3 /*yolov3 report blob num*/
#define SAMPLE_SVP_NNIE_YOLOV3_EACH_GRID_BIAS_NUM 6 /*yolov3 bias num of each grid*/
#define SAMPLE_SVP_NNIE_YOLOV3_EACH_BBOX_INFER_RESULT_NUM   85 /*yolov3 inference result num of each bbox*/

/*CNN GetTopN unit*/
typedef struct hiSAMPLE_SVP_NNIE_CNN_GETTOPN_UNIT_S
{
    HI_U32   u32ClassId;
    HI_U32   u32Confidence;
}SAMPLE_SVP_NNIE_CNN_GETTOPN_UNIT_S;

/*stack for sort*/
typedef struct hiSAMPLE_SVP_NNIE_STACK
{
    HI_S32 s32Min;
    HI_S32 s32Max;
}SAMPLE_SVP_NNIE_STACK_S;

/*stack for sort*/
typedef struct hiSAMPLE_SVP_NNIE_YOLOV1_SCORE
{
    HI_U32 u32Idx;
    HI_S32 s32Score;
}SAMPLE_SVP_NNIE_YOLOV1_SCORE_S;

typedef struct hiSAMPLE_SVP_NNIE_YOLOV2_BBOX
{
    HI_FLOAT f32Xmin;
    HI_FLOAT f32Xmax;
    HI_FLOAT f32Ymin;
    HI_FLOAT f32Ymax;
    HI_S32 s32ClsScore;
    HI_U32 u32ClassIdx;
    HI_U32 u32Mask;
}SAMPLE_SVP_NNIE_YOLOV2_BBOX_S;

typedef SAMPLE_SVP_NNIE_YOLOV2_BBOX_S SAMPLE_SVP_NNIE_YOLOV3_BBOX_S;

/*CNN*/
HI_S32 SAMPLE_SVP_NNIE_Cnn_GetTopN(SAMPLE_SVP_NNIE_PARAM_S*pstNnieParam,
    SAMPLE_SVP_NNIE_CNN_SOFTWARE_PARAM_S* pstSoftwareParam);

/*FasterRcnn*/
HI_U32 SAMPLE_SVP_NNIE_RpnTmpBufSize(HI_U32 u32RatioAnchorsNum,
    HI_U32 u32ScaleAnchorsNum, HI_U32 u32ConvHeight, HI_U32 u32ConvWidth);

HI_U32 SAMPLE_SVP_NNIE_FasterRcnn_GetResultTmpBufSize(
    HI_U32 u32MaxRoiNum, HI_U32 u32ClassNum);

HI_S32 SAMPLE_SVP_NNIE_FasterRcnn_Rpn(SAMPLE_SVP_NNIE_PARAM_S*pstNnieParam,
    SAMPLE_SVP_NNIE_FASTERRCNN_SOFTWARE_PARAM_S* pstSoftwareParam);

HI_S32 SAMPLE_SVP_NNIE_FasterRcnn_GetResult(SAMPLE_SVP_NNIE_PARAM_S*pstNnieParam,
    SAMPLE_SVP_NNIE_FASTERRCNN_SOFTWARE_PARAM_S* pstSoftwareParam);


/*Pvanet*/
HI_U32 SAMPLE_SVP_NNIE_Pvanet_GetResultTmpBufSize(
    HI_U32 u32MaxRoiNum, HI_U32 u32ClassNum);

HI_S32 SAMPLE_SVP_NNIE_Pvanet_Rpn(SAMPLE_SVP_NNIE_PARAM_S*pstNnieParam,
    SAMPLE_SVP_NNIE_FASTERRCNN_SOFTWARE_PARAM_S* pstSoftwareParam);

HI_S32 SAMPLE_SVP_NNIE_Pvanet_GetResult(SAMPLE_SVP_NNIE_PARAM_S*pstNnieParam,
    SAMPLE_SVP_NNIE_FASTERRCNN_SOFTWARE_PARAM_S* pstSoftwareParam);

/*RFCN*/
HI_U32 SAMPLE_SVP_NNIE_Rfcn_GetResultTmpBuf(HI_U32 u32MaxRoiNum, HI_U32 u32ClassNum);

HI_S32 SAMPLE_SVP_NNIE_Rfcn_Rpn(SAMPLE_SVP_NNIE_PARAM_S*pstNnieParam,
    SAMPLE_SVP_NNIE_RFCN_SOFTWARE_PARAM_S* pstSoftwareParam);

HI_S32 SAMPLE_SVP_NNIE_Rfcn_GetResult(SAMPLE_SVP_NNIE_PARAM_S*pstNnieParam,
    SAMPLE_SVP_NNIE_RFCN_SOFTWARE_PARAM_S* pstSoftwareParam);

/*SSD*/
HI_U32 SAMPLE_SVP_NNIE_Ssd_GetResultTmpBuf(SAMPLE_SVP_NNIE_PARAM_S*pstNnieParam,
    SAMPLE_SVP_NNIE_SSD_SOFTWARE_PARAM_S* pstSoftwareParam);

HI_S32 SAMPLE_SVP_NNIE_Ssd_GetResult(SAMPLE_SVP_NNIE_PARAM_S*pstNnieParam,
    SAMPLE_SVP_NNIE_SSD_SOFTWARE_PARAM_S* pstSoftwareParam);

/*YOLOV1*/
HI_U32 SAMPLE_SVP_NNIE_Yolov1_GetResultTmpBuf(SAMPLE_SVP_NNIE_PARAM_S*pstNnieParam,
    SAMPLE_SVP_NNIE_YOLOV1_SOFTWARE_PARAM_S* pstSoftwareParam);

HI_S32 SAMPLE_SVP_NNIE_Yolov1_GetResult(SAMPLE_SVP_NNIE_PARAM_S*pstNnieParam,
    SAMPLE_SVP_NNIE_YOLOV1_SOFTWARE_PARAM_S* pstSoftwareParam);

/*YOLOV2*/
HI_U32 SAMPLE_SVP_NNIE_Yolov2_GetResultTmpBuf(SAMPLE_SVP_NNIE_PARAM_S*pstNnieParam,
    SAMPLE_SVP_NNIE_YOLOV2_SOFTWARE_PARAM_S* pstSoftwareParam);

HI_S32 SAMPLE_SVP_NNIE_Yolov2_GetResult(SAMPLE_SVP_NNIE_PARAM_S*pstNnieParam,
    SAMPLE_SVP_NNIE_YOLOV2_SOFTWARE_PARAM_S* pstSoftwareParam);

/*YOLOV3*/
HI_U32 SAMPLE_SVP_NNIE_Yolov3_GetResultTmpBuf(SAMPLE_SVP_NNIE_PARAM_S*pstNnieParam,
    SAMPLE_SVP_NNIE_YOLOV3_SOFTWARE_PARAM_S* pstSoftwareParam);

HI_S32 SAMPLE_SVP_NNIE_Yolov3_GetResult(SAMPLE_SVP_NNIE_PARAM_S*pstNnieParam,
    SAMPLE_SVP_NNIE_YOLOV3_SOFTWARE_PARAM_S* pstSoftwareParam);
#ifdef __cplusplus
}
#endif

#endif /* _SAMPLE_SVP_USER_KERNEL_H_ */