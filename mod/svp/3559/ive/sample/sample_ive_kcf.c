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
#include <pthread.h>
#include <sys/prctl.h>
#include <math.h>

#include "hi_common.h"
#include "hi_comm_sys.h"
#include "hi_comm_svp.h"
#include "sample_comm.h"
#include "sample_comm_svp.h"
#include "sample_comm_nnie.h"
#include "sample_ive_main.h"
#include "sample_comm_ive.h"
#include "sample_ive_queue.h"


typedef enum hiSAMPLE_IVE_CNN_GET_FRM_STATUS_E
{
    CNN_GET_FRM_START = 0x0,
    CNN_GET_FRM_END = 0x1,

    CNN_GET_FRM_BUTT
}SAMPLE_IVE_CNN_GET_FRM_STATUS_E;

typedef enum hiSAMPLE_IVE_CNN_PROC_STATUS_E
{
    CNN_PROC_START = 0x0,
    CNN_PROC_END = 0x1,

    CNN_PROC_BUTT
}SAMPLE_IVE_CNN_PROC_STATUS_E;

typedef struct hiSAMPLE_IVE_KCF_S
{
    IVE_ROI_INFO_S astRoiInfo[64];
    HI_U32 u32RoiNum;
    IVE_MEM_INFO_S stTotalMem;
    IVE_MEM_INFO_S stListMem;
    IVE_MEM_INFO_S stCosWinX;
    IVE_MEM_INFO_S stCosWinY;
    IVE_MEM_INFO_S stGaussPeak;
    IVE_KCF_OBJ_LIST_S stObjList;
    IVE_KCF_PRO_CTRL_S stKcfProCtrl;
    IVE_KCF_BBOX_S astBbox[64];
    HI_U32 u32BboxObjNum;
    IVE_KCF_BBOX_CTRL_S stKcfBboxCtrl;
    HI_U3Q5 u3q5Padding;
    HI_U8 u8Reserved;

    SAMPLE_IVE_CNN_GET_FRM_STATUS_E enCnnGetFrmStat;
    SAMPLE_IVE_CNN_PROC_STATUS_E enCnnProcStat;
    HI_BOOL bNewDetect;
    HI_BOOL bFirstDetect;

    SAMPLE_IVE_QUEUE_S *pstQueueHead;
    VIDEO_FRAME_INFO_S astFrameInfo[2];
    pthread_mutex_t CnnDetectMutex;
    pthread_mutex_t QueueMutex;
    pthread_mutex_t GetFrmMutex;
}SAMPLE_IVE_KCF_S;


#define SAMPLE_IVE_MAX(a,b)    (((a) > (b)) ? (a) : (b))
#define SAMPLE_IVE_MIN(a,b)    (((a) < (b)) ? (a) : (b))

#define SAMPLE_IVE_MUTEX_INIT_LOCK(mutex)	\
do                                          \
{											\
	(void)pthread_mutex_init(&mutex, NULL); \
}while(0)
#define SAMPLE_IVE_MUTEX_LOCK(mutex)		\
do                                          \
{                                           \
	(void)pthread_mutex_lock(&mutex);       \
}while(0)
#define SAMPLE_IVE_MUTEX_UNLOCK(mutex)		\
do                                          \
{                                           \
	(void)pthread_mutex_unlock(&mutex);     \
}while(0)
#define SAMPLE_IVE_MUTEX_DESTROY(mutex)		\
do                                          \
{                                           \
	(void)pthread_mutex_destroy(&mutex);    \
}while(0)


/*rfcn para*/
static SAMPLE_SVP_NNIE_MODEL_S s_stRfcnModel = {0};
static SAMPLE_SVP_NNIE_PARAM_S s_stRfcnNnieParam = {0};
static SAMPLE_SVP_NNIE_RFCN_SOFTWARE_PARAM_S s_stRfcnSoftwareParam = {0};
static HI_BOOL s_bIveKcfStopSignal = HI_FALSE;
static pthread_t s_IveDetectThread = 0;
static pthread_t s_IveGetFrmThread = 0;
static pthread_t s_IveTrackThread = 0;
static SAMPLE_VI_CONFIG_S s_stViConfig = {0};
static SAMPLE_VO_CONFIG_S s_stVoConfig = {0};
static SAMPLE_IVE_KCF_S s_stIveKcfInfo = {0};


#define SAMPLE_IVE_KCF_NODE_MAX_NUM          64
#define SAMPLE_IVE_KCF_GAUSS_PEAK_TOTAL_SIZE          455680
#define SAMPLE_IVE_KCF_COS_WINDOW_TOTAL_SIZE          832
#define SAMPLE_IVE_KCF_COS_WINDOW_SIZE                64
#define SAMPLE_IVE_KCF_TEMP_BUF_SIZE                  47616
#define SAMPLE_IVE_KCF_HOG_FEATRUE_BUF_SIZE           47616

#define SAMPLE_IVE_KCF_ROI_PADDING_MAX_WIDTH          1024
#define SAMPLE_IVE_KCF_ROI_PADDING_MIN_WIDTH          40
#define SAMPLE_IVE_KCF_ROI_PADDING_MAX_HEIGHT         1024
#define SAMPLE_IVE_KCF_ROI_PADDING_MIN_HEIGHT         40


#define SAMPLE_IVE_QUEUE_LEN  16

#define SAMPLE_IVE_PROPOSAL_WIDTH  6  /*the number of proposal values*/
#define SAMPLE_IVE_COORDI_NUM  4      /*coordinate numbers*/
#define SAMPLE_IVE_QUANT_BASE 4096    /*the base value*/
#define SAMPLE_IVE_SCORE_NUM  2       /*the num of RPN scores*/
#define SAMPLE_IVE_HALF 0.5f          /*the half value*/

/*stack for sort*/
typedef struct hiSAMPLE_IVE_STACK
{
    HI_S32 s32Min;
    HI_S32 s32Max;
}SAMPLE_IVE_STACK_S;

static HI_FLOAT s_af32ExpCoef[10][16] = {
    {1.0f, 1.00024f, 1.00049f, 1.00073f, 1.00098f, 1.00122f, 1.00147f, 1.00171f, 1.00196f, 1.0022f, 1.00244f, 1.00269f, 1.00293f, 1.00318f, 1.00342f, 1.00367f},
    {1.0f, 1.00391f, 1.00784f, 1.01179f, 1.01575f, 1.01972f, 1.02371f, 1.02772f, 1.03174f, 1.03578f, 1.03984f, 1.04391f, 1.04799f, 1.05209f, 1.05621f, 1.06034f},
    {1.0f, 1.06449f, 1.13315f, 1.20623f, 1.28403f, 1.36684f, 1.45499f, 1.54883f, 1.64872f, 1.75505f, 1.86825f, 1.98874f, 2.117f, 2.25353f, 2.39888f, 2.55359f},
    {1.0f, 2.71828f, 7.38906f, 20.0855f, 54.5981f, 148.413f, 403.429f, 1096.63f, 2980.96f, 8103.08f, 22026.5f, 59874.1f, 162755.0f, 442413.0f, 1.2026e+006f, 3.26902e+006f},
    {1.0f, 8.88611e+006f, 7.8963e+013f, 7.01674e+020f, 6.23515e+027f, 5.54062e+034f, 5.54062e+034f, 5.54062e+034f, 5.54062e+034f, 5.54062e+034f, 5.54062e+034f, 5.54062e+034f, 5.54062e+034f, 5.54062e+034f, 5.54062e+034f, 5.54062e+034f},
    {1.0f, 0.999756f, 0.999512f, 0.999268f, 0.999024f, 0.99878f, 0.998536f, 0.998292f, 0.998049f, 0.997805f, 0.997562f, 0.997318f, 0.997075f, 0.996831f, 0.996588f, 0.996345f},
    {1.0f, 0.996101f, 0.992218f, 0.98835f, 0.984496f, 0.980658f, 0.976835f, 0.973027f, 0.969233f, 0.965455f, 0.961691f, 0.957941f, 0.954207f, 0.950487f, 0.946781f, 0.94309f},
    {1.0f, 0.939413f, 0.882497f, 0.829029f, 0.778801f, 0.731616f, 0.687289f, 0.645649f, 0.606531f, 0.569783f, 0.535261f, 0.502832f, 0.472367f, 0.443747f, 0.416862f, 0.391606f},
    {1.0f, 0.367879f, 0.135335f, 0.0497871f, 0.0183156f, 0.00673795f, 0.00247875f, 0.000911882f, 0.000335463f, 0.00012341f, 4.53999e-005f, 1.67017e-005f, 6.14421e-006f, 2.26033e-006f, 8.31529e-007f, 3.05902e-007f},
    {1.0f, 1.12535e-007f, 1.26642e-014f, 1.42516e-021f, 1.60381e-028f, 1.80485e-035f, 2.03048e-042f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f}
};

static HI_FLOAT IVE_QuickExp( HI_S32 s32Value )
{
    if( s32Value & 0x80000000 )
    {
        s32Value = ~s32Value + 0x00000001;
        return s_af32ExpCoef[5][s32Value & 0x0000000F] * s_af32ExpCoef[6][(s32Value>>4) & 0x0000000F] * s_af32ExpCoef[7][(s32Value>>8) & 0x0000000F] * s_af32ExpCoef[8][(s32Value>>12) & 0x0000000F] * s_af32ExpCoef[9][(s32Value>>16) & 0x0000000F ];
    }
    else
    {
        return s_af32ExpCoef[0][s32Value & 0x0000000F] * s_af32ExpCoef[1][(s32Value>>4) & 0x0000000F] * s_af32ExpCoef[2][(s32Value>>8) & 0x0000000F] * s_af32ExpCoef[3][(s32Value>>12) & 0x0000000F] * s_af32ExpCoef[4][(s32Value>>16) & 0x0000000F ];
    }
}

static HI_S32 IVE_SoftMax( HI_FLOAT* pf32Src, HI_U32 u32Num)
{
    HI_FLOAT f32Max = 0;
    HI_FLOAT f32Sum = 0;
    HI_U32 i = 0;

    for(i = 0; i < u32Num; ++i)
    {
        if(f32Max < pf32Src[i])
        {
            f32Max = pf32Src[i];
        }
    }

    for(i = 0; i < u32Num; ++i)
    {
        pf32Src[i] = (HI_FLOAT)IVE_QuickExp((HI_S32)((pf32Src[i] - f32Max)*SAMPLE_IVE_QUANT_BASE));
        f32Sum += pf32Src[i];
    }

    for(i = 0; i < u32Num; ++i)
    {
        pf32Src[i] /= f32Sum;
    }
    return HI_SUCCESS;
}

static void IVE_Argswap(HI_S32* ps32Src1, HI_S32* ps32Src2)
{
    HI_U32 i = 0;
    HI_S32 u32Tmp = 0;
    for( i = 0; i < SAMPLE_IVE_PROPOSAL_WIDTH; i++ )
    {
        u32Tmp = ps32Src1[i];
        ps32Src1[i] = ps32Src2[i];
        ps32Src2[i] = u32Tmp;
    }
}

static HI_S32 IVE_NonRecursiveArgQuickSort(HI_S32* ps32Array,
    HI_S32 s32Low, HI_S32 s32High, SAMPLE_IVE_STACK_S *pstStack,HI_U32 u32MaxNum)
{
    HI_S32 i = s32Low;
    HI_S32 j = s32High;
    HI_S32 s32Top = 0;
    HI_S32 s32KeyConfidence = ps32Array[SAMPLE_IVE_PROPOSAL_WIDTH * s32Low + 4];
    pstStack[s32Top].s32Min = s32Low;
    pstStack[s32Top].s32Max = s32High;

    while(s32Top > -1)
    {
        s32Low = pstStack[s32Top].s32Min;
        s32High = pstStack[s32Top].s32Max;
        i = s32Low;
        j = s32High;
        s32Top--;

        s32KeyConfidence = ps32Array[SAMPLE_IVE_PROPOSAL_WIDTH * s32Low + 4];

        while(i < j)
        {
            while((i < j) && (s32KeyConfidence > ps32Array[j * SAMPLE_IVE_PROPOSAL_WIDTH + 4]))
            {
                j--;
            }
            if(i < j)
            {
                IVE_Argswap(&ps32Array[i*SAMPLE_IVE_PROPOSAL_WIDTH], &ps32Array[j*SAMPLE_IVE_PROPOSAL_WIDTH]);
                i++;
            }

            while((i < j) && (s32KeyConfidence < ps32Array[i*SAMPLE_IVE_PROPOSAL_WIDTH + 4]))
            {
                i++;
            }
            if(i < j)
            {
                IVE_Argswap(&ps32Array[i*SAMPLE_IVE_PROPOSAL_WIDTH], &ps32Array[j*SAMPLE_IVE_PROPOSAL_WIDTH]);
                j--;
            }
        }

        if(s32Low <= u32MaxNum)
        {
                if(s32Low < i-1)
                {
                    s32Top++;
                    pstStack[s32Top].s32Min = s32Low;
                    pstStack[s32Top].s32Max = i-1;
                }

                if(s32High > i+1)
                {
                    s32Top++;
                    pstStack[s32Top].s32Min = i+1;
                    pstStack[s32Top].s32Max = s32High;
                }
        }
    }
    return HI_SUCCESS;
}

static HI_S32 IVE_Overlap(HI_S32 s32XMin1, HI_S32 s32YMin1, HI_S32 s32XMax1, HI_S32 s32YMax1, HI_S32 s32XMin2,
    HI_S32 s32YMin2, HI_S32 s32XMax2, HI_S32 s32YMax2,  HI_S32* s32AreaSum, HI_S32* s32AreaInter)
{
    /*** Check the input, and change the Return value  ***/
    HI_S32 s32Inter = 0;
    HI_S32 s32Total = 0;
    HI_S32 s32XMin = 0;
    HI_S32 s32YMin = 0;
    HI_S32 s32XMax = 0;
    HI_S32 s32YMax = 0;
    HI_S32 s32Area1 = 0;
    HI_S32 s32Area2 = 0;
    HI_S32 s32InterWidth = 0;
    HI_S32 s32InterHeight = 0;

    s32XMin = SAMPLE_IVE_MAX(s32XMin1, s32XMin2);
    s32YMin = SAMPLE_IVE_MAX(s32YMin1, s32YMin2);
    s32XMax = SAMPLE_IVE_MIN(s32XMax1, s32XMax2);
    s32YMax = SAMPLE_IVE_MIN(s32YMax1, s32YMax2);

    s32InterWidth = s32XMax - s32XMin + 1;
    s32InterHeight = s32YMax - s32YMin + 1;

    s32InterWidth = ( s32InterWidth >= 0 ) ? s32InterWidth : 0;
    s32InterHeight = ( s32InterHeight >= 0 ) ? s32InterHeight : 0;

    s32Inter = s32InterWidth * s32InterHeight;
    s32Area1 = (s32XMax1 - s32XMin1 + 1) * (s32YMax1 - s32YMin1 + 1);
    s32Area2 = (s32XMax2 - s32XMin2 + 1) * (s32YMax2 - s32YMin2 + 1);

    s32Total = s32Area1 + s32Area2 - s32Inter;

    *s32AreaSum = s32Total;
    *s32AreaInter = s32Inter;
    return HI_SUCCESS;
}

static HI_S32 IVE_FilterLowScoreBbox(HI_S32* ps32Proposals, HI_U32 u32AnchorsNum,
    HI_U32 u32FilterThresh, HI_U32* u32NumAfterFilter)
{
    HI_U32 u32ProposalCnt = u32AnchorsNum;
    HI_U32 i = 0;

    if( u32FilterThresh > 0 )
    {
        for( i = 0; i < u32AnchorsNum; i++ )
        {
            if( ps32Proposals[SAMPLE_IVE_PROPOSAL_WIDTH * i + 4 ] < (HI_S32)u32FilterThresh )
            {
                ps32Proposals[SAMPLE_IVE_PROPOSAL_WIDTH * i + 5 ] = 1;
            }
        }

        u32ProposalCnt = 0;
        for( i = 0; i < u32AnchorsNum; i++ )
        {
            if( 0 == ps32Proposals[ SAMPLE_IVE_PROPOSAL_WIDTH * i + 5 ] )
            {
                ps32Proposals[ SAMPLE_IVE_PROPOSAL_WIDTH * u32ProposalCnt ] = ps32Proposals[ SAMPLE_IVE_PROPOSAL_WIDTH * i ];
                ps32Proposals[ SAMPLE_IVE_PROPOSAL_WIDTH * u32ProposalCnt + 1 ] = ps32Proposals[ SAMPLE_IVE_PROPOSAL_WIDTH * i + 1 ];
                ps32Proposals[ SAMPLE_IVE_PROPOSAL_WIDTH * u32ProposalCnt + 2 ] = ps32Proposals[ SAMPLE_IVE_PROPOSAL_WIDTH * i + 2 ];
                ps32Proposals[ SAMPLE_IVE_PROPOSAL_WIDTH * u32ProposalCnt + 3 ] = ps32Proposals[ SAMPLE_IVE_PROPOSAL_WIDTH * i + 3 ];
                ps32Proposals[ SAMPLE_IVE_PROPOSAL_WIDTH * u32ProposalCnt + 4 ] = ps32Proposals[ SAMPLE_IVE_PROPOSAL_WIDTH * i + 4 ];
                ps32Proposals[ SAMPLE_IVE_PROPOSAL_WIDTH * u32ProposalCnt + 5 ] = ps32Proposals[ SAMPLE_IVE_PROPOSAL_WIDTH * i + 5 ];
                u32ProposalCnt++;
            }
        }
    }
    *u32NumAfterFilter = u32ProposalCnt;
    return HI_SUCCESS;
}

static HI_S32 IVE_NonMaxSuppression( HI_S32* ps32Proposals, HI_U32 u32AnchorsNum,
    HI_U32 u32NmsThresh,HI_U32 u32MaxRoiNum)
{
    /****** define variables *******/
    HI_S32 s32XMin1 = 0;
    HI_S32 s32YMin1 = 0;
    HI_S32 s32XMax1 = 0;
    HI_S32 s32YMax1 = 0;
    HI_S32 s32XMin2 = 0;
    HI_S32 s32YMin2 = 0;
    HI_S32 s32XMax2 = 0;
    HI_S32 s32YMax2 = 0;
    HI_S32 s32AreaTotal = 0;
    HI_S32 s32AreaInter = 0;
    HI_U32 i = 0;
    HI_U32 j = 0;
    HI_U32 u32Num = 0;
    HI_BOOL bNoOverlap  = HI_TRUE;
    for (i = 0; i < u32AnchorsNum && u32Num < u32MaxRoiNum; i++)
    {
        if( ps32Proposals[SAMPLE_IVE_PROPOSAL_WIDTH*i+5] == 0 )
        {
            u32Num++;
            s32XMin1 =  ps32Proposals[SAMPLE_IVE_PROPOSAL_WIDTH*i];
            s32YMin1 =  ps32Proposals[SAMPLE_IVE_PROPOSAL_WIDTH*i+1];
            s32XMax1 =  ps32Proposals[SAMPLE_IVE_PROPOSAL_WIDTH*i+2];
            s32YMax1 =  ps32Proposals[SAMPLE_IVE_PROPOSAL_WIDTH*i+3];
            for(j= i+1;j< u32AnchorsNum; j++)
            {
                if( ps32Proposals[SAMPLE_IVE_PROPOSAL_WIDTH*j+5] == 0 )
                {
                    s32XMin2 = ps32Proposals[SAMPLE_IVE_PROPOSAL_WIDTH*j];
                    s32YMin2 = ps32Proposals[SAMPLE_IVE_PROPOSAL_WIDTH*j+1];
                    s32XMax2 = ps32Proposals[SAMPLE_IVE_PROPOSAL_WIDTH*j+2];
                    s32YMax2 = ps32Proposals[SAMPLE_IVE_PROPOSAL_WIDTH*j+3];
                    bNoOverlap = (s32XMin2>s32XMax1)||(s32XMax2<s32XMin1)||(s32YMin2>s32YMax1)||(s32YMax2<s32YMin1);
                    if(bNoOverlap)
                    {
                        continue;
                    }
                    (void)IVE_Overlap(s32XMin1, s32YMin1, s32XMax1, s32YMax1, s32XMin2, s32YMin2, s32XMax2, s32YMax2, &s32AreaTotal, &s32AreaInter);
                    if(s32AreaInter*SAMPLE_IVE_QUANT_BASE > ((HI_S32)u32NmsThresh*s32AreaTotal))
                    {
                        if( ps32Proposals[SAMPLE_IVE_PROPOSAL_WIDTH*i+4] >= ps32Proposals[SAMPLE_IVE_PROPOSAL_WIDTH*j+4] )
                        {
                            ps32Proposals[SAMPLE_IVE_PROPOSAL_WIDTH*j+5] = 1;
                        }
                        else
                        {
                            ps32Proposals[SAMPLE_IVE_PROPOSAL_WIDTH*i+5] = 1;
                        }
                    }
                }
            }
        }
    }
    return HI_SUCCESS;
}

static HI_S32 IVE_Rpn(HI_S32** pps32Src,HI_U32 u32NumRatioAnchors,
    HI_U32 u32NumScaleAnchors,HI_U32* au32Scales,HI_U32* au32Ratios,HI_U32 u32OriImHeight,
    HI_U32 u32OriImWidth,HI_U32* pu32ConvHeight,HI_U32* pu32ConvWidth,HI_U32* pu32ConvChannel,
    HI_U32  u32ConvStride,HI_U32 u32MaxRois,HI_U32 u32MinSize,HI_U32 u32SpatialScale,
    HI_U32 u32NmsThresh,HI_U32 u32FilterThresh,HI_U32 u32NumBeforeNms,HI_U32 *pu32MemPool,
    HI_S32 *ps32ProposalResult,HI_U32* pu32NumRois)
{
    /******************** define parameters ****************/
    HI_U32 u32Size = 0;
    HI_S32* ps32Anchors = NULL;
    HI_S32* ps32BboxDelta = NULL;
    HI_S32* ps32Proposals = NULL;
    HI_U32* pu32Ptr = NULL;
    HI_S32* ps32Ptr = NULL;
    HI_U32 u32NumAfterFilter = 0;
    HI_U32 u32NumAnchors = 0;
    HI_FLOAT f32BaseW = 0;
    HI_FLOAT f32BaseH = 0;
    HI_FLOAT f32BaseXCtr = 0;
    HI_FLOAT f32BaseYCtr = 0;
    HI_FLOAT f32SizeRatios = 0;
    HI_FLOAT* pf32RatioAnchors = NULL;
    HI_FLOAT* pf32Ptr = NULL;
    HI_FLOAT *pf32Ptr2 = NULL;
    HI_FLOAT* pf32ScaleAnchors = NULL;
    HI_FLOAT* pf32Scores = NULL;
    HI_FLOAT f32Ratios = 0;
    HI_FLOAT f32Size = 0;
    HI_U32 u32PixelInterval = 0;
    HI_U32 u32SrcBboxIndex = 0;
    HI_U32 u32SrcFgProbIndex = 0;
    HI_U32 u32SrcBgProbIndex = 0;
    HI_U32 u32SrcBboxBias = 0;
    HI_U32 u32SrcProbBias = 0;
    HI_U32 u32DesBox = 0;
    HI_U32 u32BgBlobSize = 0;
    HI_U32 u32AnchorsPerPixel = 0;
    HI_U32 u32MapSize = 0;
    HI_U32 u32LineSize = 0;
    HI_S32* ps32Ptr2 = NULL;
    HI_S32* ps32Ptr3 = NULL;
    HI_S32 s32ProposalWidth = 0;
    HI_S32 s32ProposalHeight = 0;
    HI_S32 s32ProposalCenterX = 0;
    HI_S32 s32ProposalCenterY = 0;
    HI_S32 s32PredW = 0;
    HI_S32 s32PredH = 0;
    HI_S32 s32PredCenterX = 0;
    HI_S32 s32PredCenterY = 0;
    HI_U32 u32DesBboxDeltaIndex = 0;
    HI_U32 u32DesScoreIndex = 0;
    HI_U32 u32RoiCount = 0;
    SAMPLE_IVE_STACK_S* pstStack = NULL;
    HI_S32 s32Ret = HI_SUCCESS;
    HI_U32 c = 0;
    HI_U32 h = 0;
    HI_U32 w = 0;
    HI_U32 i = 0;
    HI_U32 j = 0;
    HI_U32 p = 0;
    HI_U32 q = 0;
    HI_U32 z = 0;
    HI_U32 au32BaseAnchor[4] = {0, 0, (u32MinSize -1), (u32MinSize -1)};

    /*********************************** Faster RCNN *********************************************/
    /********* calculate the start pointer of each part in MemPool *********/
    pu32Ptr = (HI_U32*)pu32MemPool;
    ps32Anchors = (HI_S32*)pu32Ptr;
    u32NumAnchors = u32NumRatioAnchors * u32NumScaleAnchors * ( pu32ConvHeight[0] * pu32ConvWidth[0] );
    u32Size = SAMPLE_IVE_COORDI_NUM * u32NumAnchors;
    pu32Ptr += u32Size;

    ps32BboxDelta = (HI_S32*)pu32Ptr;
    pu32Ptr += u32Size;

    ps32Proposals = (HI_S32*)pu32Ptr;
    u32Size = SAMPLE_IVE_PROPOSAL_WIDTH * u32NumAnchors;
    pu32Ptr += u32Size;

    pf32RatioAnchors = (HI_FLOAT*)pu32Ptr;
    pf32Ptr = (HI_FLOAT*)pu32Ptr;
    u32Size = u32NumRatioAnchors * SAMPLE_IVE_COORDI_NUM;
    pf32Ptr = pf32Ptr + u32Size;

    pf32ScaleAnchors = pf32Ptr;
    u32Size = u32NumScaleAnchors * u32NumRatioAnchors * SAMPLE_IVE_COORDI_NUM;
    pf32Ptr = pf32Ptr + u32Size;

    pf32Scores = pf32Ptr;
    u32Size = u32NumAnchors * SAMPLE_IVE_SCORE_NUM;
    pf32Ptr = pf32Ptr + u32Size;

    pstStack = (SAMPLE_IVE_STACK_S*)pf32Ptr;

    /********************* Generate the base anchor ***********************/
    f32BaseW = (HI_FLOAT)(au32BaseAnchor[2] - au32BaseAnchor[0] + 1 );
    f32BaseH = (HI_FLOAT)(au32BaseAnchor[3] - au32BaseAnchor[1] + 1 );
    f32BaseXCtr = (HI_FLOAT)(au32BaseAnchor[0] + ( ( f32BaseW - 1 ) * 0.5 ) );
    f32BaseYCtr = (HI_FLOAT)(au32BaseAnchor[1] + ( ( f32BaseH - 1 ) * 0.5 ) );

    /*************** Generate Ratio Anchors for the base anchor ***********/
    pf32Ptr = pf32RatioAnchors;
    f32Size = f32BaseW * f32BaseH;
    for (i = 0; i < u32NumRatioAnchors; i++)
    {
        f32Ratios = (HI_FLOAT)au32Ratios[i]/SAMPLE_IVE_QUANT_BASE;
        f32SizeRatios = f32Size / f32Ratios;
        f32BaseW = sqrt(f32SizeRatios);
        f32BaseW = (HI_FLOAT)(1.0 * ( (f32BaseW) >= 0 ? (HI_S32)(f32BaseW+SAMPLE_IVE_HALF) : (HI_S32)(f32BaseW-SAMPLE_IVE_HALF)));
        f32BaseH = f32BaseW * f32Ratios;
        f32BaseH = (HI_FLOAT)(1.0 * ( (f32BaseH) >= 0 ? (HI_S32)(f32BaseH+SAMPLE_IVE_HALF) : (HI_S32)(f32BaseH-SAMPLE_IVE_HALF)));

        *pf32Ptr++ = (HI_FLOAT)(f32BaseXCtr - ( ( f32BaseW - 1 ) * SAMPLE_IVE_HALF ));
        *(pf32Ptr++) = (HI_FLOAT)(f32BaseYCtr - ( ( f32BaseH - 1 ) * SAMPLE_IVE_HALF ));
        *(pf32Ptr++) = (HI_FLOAT)(f32BaseXCtr + ( ( f32BaseW - 1 ) * SAMPLE_IVE_HALF ));
        *(pf32Ptr++) =  (HI_FLOAT)( f32BaseYCtr + ( ( f32BaseH - 1 ) * SAMPLE_IVE_HALF ));
    }

    /********* Generate Scale Anchors for each Ratio Anchor **********/
    pf32Ptr = pf32RatioAnchors;
    pf32Ptr2 = pf32ScaleAnchors;
    /* Generate Scale Anchors for one pixel */
    for( i = 0; i < u32NumRatioAnchors; i++ )
    {
        for( j = 0; j < u32NumScaleAnchors; j++ )
        {
            f32BaseW = *( pf32Ptr + 2 ) - *( pf32Ptr ) + 1;
            f32BaseH = *( pf32Ptr + 3 ) - *( pf32Ptr + 1 ) + 1;
            f32BaseXCtr = (HI_FLOAT)( *( pf32Ptr ) + ( ( f32BaseW - 1 ) * SAMPLE_IVE_HALF ));
            f32BaseYCtr = (HI_FLOAT)( *( pf32Ptr + 1 ) + ( ( f32BaseH - 1 ) * SAMPLE_IVE_HALF ));

            *( pf32Ptr2++ ) = (HI_FLOAT) (f32BaseXCtr - ((f32BaseW * ((HI_FLOAT)au32Scales[j]/SAMPLE_IVE_QUANT_BASE) - 1) * SAMPLE_IVE_HALF));
            *( pf32Ptr2++ ) = (HI_FLOAT)(f32BaseYCtr - ((f32BaseH * ((HI_FLOAT)au32Scales[j]/SAMPLE_IVE_QUANT_BASE) - 1) * SAMPLE_IVE_HALF));
            *( pf32Ptr2++ ) = (HI_FLOAT)(f32BaseXCtr + ((f32BaseW * ((HI_FLOAT)au32Scales[j]/SAMPLE_IVE_QUANT_BASE) - 1) * SAMPLE_IVE_HALF));
            *( pf32Ptr2++ ) = (HI_FLOAT)(f32BaseYCtr + ((f32BaseH * ((HI_FLOAT)au32Scales[j]/SAMPLE_IVE_QUANT_BASE) - 1) * SAMPLE_IVE_HALF));
        }
            pf32Ptr += SAMPLE_IVE_COORDI_NUM;
    }


    /******************* Copy the anchors to every pixel in the feature map ******************/
    ps32Ptr = ps32Anchors;
    u32PixelInterval = SAMPLE_IVE_QUANT_BASE/ u32SpatialScale;

    for ( p = 0; p < pu32ConvHeight[0]; p++  )
    {
        for ( q = 0; q < pu32ConvWidth[0]; q++ )
        {
            pf32Ptr2 = pf32ScaleAnchors;
            for ( z = 0 ; z < u32NumScaleAnchors * u32NumRatioAnchors; z++ )
            {
                *(ps32Ptr++) = (HI_S32)(q * u32PixelInterval + *(pf32Ptr2++) );
                *(ps32Ptr++) = (HI_S32)( p * u32PixelInterval + *( pf32Ptr2++ ));
                *(ps32Ptr++) = (HI_S32)( q * u32PixelInterval + *( pf32Ptr2++ ));
                *(ps32Ptr++) = (HI_S32)( p * u32PixelInterval + *( pf32Ptr2++ ));
            }
        }
    }

    /********** do transpose, convert the blob from (M,C,H,W) to (M,H,W,C) **********/
    u32MapSize = pu32ConvHeight[1] * u32ConvStride / sizeof(HI_U32);
    u32AnchorsPerPixel = u32NumRatioAnchors * u32NumScaleAnchors;
    u32BgBlobSize = u32AnchorsPerPixel * u32MapSize;
    u32LineSize = u32ConvStride / sizeof(HI_U32);
    u32SrcProbBias = 0;
    u32SrcBboxBias = 0;

    for ( c = 0; c < pu32ConvChannel[1]; c++ )
    {
        for ( h = 0; h < pu32ConvHeight[1]; h++ )
        {
            for ( w = 0; w < pu32ConvWidth[1]; w++ )
            {
                u32SrcBboxIndex = u32SrcBboxBias + c * u32MapSize + h * u32LineSize + w;
                u32SrcBgProbIndex = u32SrcProbBias + (c/SAMPLE_IVE_COORDI_NUM) * u32MapSize + h * u32LineSize + w;
                u32SrcFgProbIndex = u32BgBlobSize + u32SrcBgProbIndex;

                u32DesBox = ( u32AnchorsPerPixel ) * ( h * pu32ConvWidth[1] + w) + c/SAMPLE_IVE_COORDI_NUM ;

                u32DesBboxDeltaIndex = SAMPLE_IVE_COORDI_NUM * u32DesBox + c % SAMPLE_IVE_COORDI_NUM;
                ps32BboxDelta[u32DesBboxDeltaIndex] = (HI_S32)pps32Src[1][u32SrcBboxIndex];

                u32DesScoreIndex = ( SAMPLE_IVE_SCORE_NUM ) * u32DesBox;
                pf32Scores[u32DesScoreIndex] = (HI_FLOAT)((HI_S32)pps32Src[0][u32SrcBgProbIndex]) / SAMPLE_IVE_QUANT_BASE;
                pf32Scores[u32DesScoreIndex + 1] = (HI_FLOAT)((HI_S32)pps32Src[0][u32SrcFgProbIndex]) / SAMPLE_IVE_QUANT_BASE;
            }
        }
    }


    /************************* do softmax ****************************/
    pf32Ptr = pf32Scores;
    for( i = 0; i<u32NumAnchors; i++ )
    {
        s32Ret = IVE_SoftMax(pf32Ptr, SAMPLE_IVE_SCORE_NUM);
        pf32Ptr += SAMPLE_IVE_SCORE_NUM;
    }


    /************************* BBox Transform *****************************/
    /* use parameters from Conv3 to adjust the coordinates of anchors */
    ps32Ptr = ps32Anchors;
    ps32Ptr2 = ps32Proposals;
    ps32Ptr3 = ps32BboxDelta;
    for( i = 0; i < u32NumAnchors; i++)
    {
        ps32Ptr = ps32Anchors;
        ps32Ptr = ps32Ptr + SAMPLE_IVE_COORDI_NUM * i;
        ps32Ptr2 = ps32Proposals;
        ps32Ptr2 = ps32Ptr2 + SAMPLE_IVE_PROPOSAL_WIDTH * i;
        ps32Ptr3 = ps32BboxDelta;
        ps32Ptr3 = ps32Ptr3 + SAMPLE_IVE_COORDI_NUM * i;
        pf32Ptr = pf32Scores;
        pf32Ptr = pf32Ptr + i* ( SAMPLE_IVE_SCORE_NUM );

        s32ProposalWidth = *(ps32Ptr+2) - *(ps32Ptr) + 1;
        s32ProposalHeight = *(ps32Ptr+3) - *(ps32Ptr+1) + 1;
        s32ProposalCenterX = *(ps32Ptr) + (HI_S32)( s32ProposalWidth * SAMPLE_IVE_HALF );
        s32ProposalCenterY = *(ps32Ptr+1) + (HI_S32)( s32ProposalHeight * SAMPLE_IVE_HALF);
        s32PredCenterX = (HI_S32)( ((HI_FLOAT)(*(ps32Ptr3))/SAMPLE_IVE_QUANT_BASE) * s32ProposalWidth + s32ProposalCenterX );
        s32PredCenterY = (HI_S32)( ((HI_FLOAT)(*(ps32Ptr3 + 1))/SAMPLE_IVE_QUANT_BASE) * s32ProposalHeight + s32ProposalCenterY);

        s32PredW = (HI_S32)(s32ProposalWidth * IVE_QuickExp((HI_S32)( *(ps32Ptr3+2) )));
        s32PredH = (HI_S32)(s32ProposalHeight * IVE_QuickExp((HI_S32)( *(ps32Ptr3+3) )));
        *(ps32Ptr2) = (HI_S32)( s32PredCenterX - SAMPLE_IVE_HALF * s32PredW);
        *(ps32Ptr2+1) = (HI_S32)( s32PredCenterY - SAMPLE_IVE_HALF * s32PredH );
        *(ps32Ptr2+2) = (HI_S32)( s32PredCenterX + SAMPLE_IVE_HALF * s32PredW );
        *(ps32Ptr2+3) = (HI_S32)( s32PredCenterY + SAMPLE_IVE_HALF * s32PredH );
        *(ps32Ptr2+4) = (HI_S32)(*(pf32Ptr+1) * SAMPLE_IVE_QUANT_BASE);
        *(ps32Ptr2+5) = 0;
    }


    /************************ clip bbox *****************************/
    for( i = 0; i < u32NumAnchors; i++)
    {
        ps32Ptr = ps32Proposals;
        ps32Ptr = ps32Ptr + SAMPLE_IVE_PROPOSAL_WIDTH * i;
        *ps32Ptr = SAMPLE_IVE_MAX(SAMPLE_IVE_MIN(*ps32Ptr, (HI_S32)u32OriImWidth-1),0 );
        *(ps32Ptr+1) = SAMPLE_IVE_MAX(SAMPLE_IVE_MIN(*(ps32Ptr+1), (HI_S32)u32OriImHeight-1),0 );
        *(ps32Ptr+2) = SAMPLE_IVE_MAX(SAMPLE_IVE_MIN(*(ps32Ptr+2), (HI_S32)u32OriImWidth-1),0 );
        *(ps32Ptr+3) = SAMPLE_IVE_MAX(SAMPLE_IVE_MIN(*(ps32Ptr+3), (HI_S32)u32OriImHeight-1),0 );
    }

    /************ remove the bboxes which are too small *************/
    for( i = 0; i< u32NumAnchors; i++)
    {
        ps32Ptr = ps32Proposals;
        ps32Ptr = ps32Ptr + SAMPLE_IVE_PROPOSAL_WIDTH * i;
        s32ProposalWidth = *(ps32Ptr+2)-*(ps32Ptr) + 1;
        s32ProposalHeight = *(ps32Ptr+3)-*(ps32Ptr+1) + 1;
        if(s32ProposalWidth < (HI_S32)u32MinSize  || s32ProposalHeight < (HI_S32)u32MinSize  )
        {
            *(ps32Ptr+5) = 1;
        }
    }

    /********** remove low score bboxes ************/
    (void)IVE_FilterLowScoreBbox( ps32Proposals, u32NumAnchors, u32FilterThresh, &u32NumAfterFilter );


    /********** sort ***********/
    (void)IVE_NonRecursiveArgQuickSort( ps32Proposals, 0, u32NumAfterFilter - 1, pstStack ,u32NumBeforeNms);
    u32NumAfterFilter = ( u32NumAfterFilter < u32NumBeforeNms ) ? u32NumAfterFilter : u32NumBeforeNms;

    /* do nms to remove highly overlapped bbox */
    (void)IVE_NonMaxSuppression(ps32Proposals, u32NumAfterFilter, u32NmsThresh, u32MaxRois);     /* function NMS */

    /************** write the final result to output ***************/
    u32RoiCount = 0;
    for( i = 0; i < u32NumAfterFilter; i++)
    {
        ps32Ptr = ps32Proposals;
        ps32Ptr = ps32Ptr + SAMPLE_IVE_PROPOSAL_WIDTH * i;
        if( *(ps32Ptr+5) == 0)
        {
            /*In this sample,the output Roi coordinates will be input in hardware,
            so the type coordinates are convert to HI_S20Q12*/
            ps32ProposalResult[SAMPLE_IVE_COORDI_NUM * u32RoiCount] = *ps32Ptr*SAMPLE_IVE_QUANT_BASE;
            ps32ProposalResult[SAMPLE_IVE_COORDI_NUM * u32RoiCount + 1] = *(ps32Ptr+1)*SAMPLE_IVE_QUANT_BASE;
            ps32ProposalResult[SAMPLE_IVE_COORDI_NUM * u32RoiCount + 2] = *(ps32Ptr+2)*SAMPLE_IVE_QUANT_BASE;
            ps32ProposalResult[SAMPLE_IVE_COORDI_NUM * u32RoiCount + 3] = *(ps32Ptr+3)*SAMPLE_IVE_QUANT_BASE;
            u32RoiCount++;
        }
        if(u32RoiCount >= u32MaxRois)
        {
            break;
        }
    }

    *pu32NumRois = u32RoiCount;

    return s32Ret;

}

static HI_S32  IVE_Rfcn_GetResult(HI_S32 *ps32FcScore,
    HI_U32 u32FcScoreStride,HI_S32* ps32FcBbox,HI_U32 u32FcBboxStride,
    HI_S32 *ps32Proposals, HI_U32 u32RoiCnt, HI_U32 *pu32ConfThresh,
    HI_U32 u32MaxRoi,HI_U32 u32ClassNum,HI_U32 u32OriImWidth,HI_U32 u32OriImHeight,
    HI_U32 u32NmsThresh, HI_U32* pu32MemPool,HI_S32 *ps32DstScores, HI_S32 *ps32DstRoi,
    HI_S32 *ps32ClassRoiNum)
{
    /************* define variables *****************/
    HI_U32 u32Size = 0;
    HI_U32 u32ClsScoreChannels = 0;
    HI_U32 u32FcScoreWidth = 0;
    HI_FLOAT f32ProposalWidth = 0.0;
    HI_FLOAT f32ProposalHeight = 0.0;
    HI_FLOAT f32ProposalCenterX = 0.0;
    HI_FLOAT f32ProposalCenterY = 0.0;
    HI_FLOAT f32PredW = 0.0;
    HI_FLOAT f32PredH = 0.0;
    HI_FLOAT f32PredCenterX = 0.0;
    HI_FLOAT f32PredCenterY = 0.0;
    HI_FLOAT* pf32FcScoresMemPool = NULL;
    HI_S32* ps32FcBboxMemPool = NULL;
    HI_S32* ps32ProposalMemPool = NULL;
    HI_S32* ps32ProposalTmp = NULL;
    HI_U32 u32FcBboxIndex = 0;
    HI_U32 u32ProposalMemPoolIndex = 0;
    HI_FLOAT* pf32Ptr = NULL;
    HI_S32* ps32Ptr = NULL;
    HI_S32* ps32DstScore = NULL;
    HI_S32* ps32DstBbox = NULL;
    HI_U32 u32RoiOutCnt = 0;
    HI_U32 u32SrcIndex = 0;
    HI_U32 u32DstIndex = 0;
    HI_U32 i = 0;
    HI_U32 j = 0;
    HI_U32 u32OffSet = 0;
    SAMPLE_IVE_STACK_S* pstStack = NULL;
    HI_S32 s32Ret = HI_SUCCESS;

    /******************* Get or calculate parameters **********************/
    u32ClsScoreChannels = u32ClassNum;   /*channel num is equal to class size, cls_score class*/
    u32FcScoreWidth = u32ClsScoreChannels;

    /*************** Get Start Pointer of MemPool ******************/
    pf32FcScoresMemPool = (HI_FLOAT*)(pu32MemPool);
    pf32Ptr = pf32FcScoresMemPool;
    u32Size = u32MaxRoi * u32ClsScoreChannels;
    pf32Ptr += u32Size;

    ps32FcBboxMemPool = (HI_S32*)pf32Ptr;
    ps32Ptr = (HI_S32*)pf32Ptr;
    u32Size = u32MaxRoi * SAMPLE_IVE_COORDI_NUM;
    ps32Ptr += u32Size;

    ps32ProposalMemPool = (HI_S32*)ps32Ptr;
    ps32Ptr = ps32ProposalMemPool;
    u32Size = u32MaxRoi * SAMPLE_IVE_PROPOSAL_WIDTH;
    ps32Ptr += u32Size;
    pstStack = (SAMPLE_IVE_STACK_S*)ps32Ptr;

    // prepare input data
    for (i = 0; i < u32RoiCnt; i++)
    {
        for (j = 0; j < u32ClsScoreChannels; j++)
        {
            u32DstIndex = u32FcScoreWidth * i + j;
            u32SrcIndex = u32FcScoreStride/sizeof(HI_U32) * i + j;
            pf32FcScoresMemPool[u32DstIndex] = (HI_FLOAT)(ps32FcScore[u32SrcIndex]) / SAMPLE_IVE_QUANT_BASE;
        }
    }

    for (i = 0; i < u32RoiCnt; i++)
    {
        for (j = 0; j < SAMPLE_IVE_COORDI_NUM; j++)
        {
            u32SrcIndex = u32FcBboxStride/sizeof(HI_U32) * i + SAMPLE_IVE_COORDI_NUM + j;
            u32DstIndex = SAMPLE_IVE_COORDI_NUM * i + j;
            ps32FcBboxMemPool[u32DstIndex] = ps32FcBbox[u32SrcIndex];
        }
    }
    /************** bbox tranform ************
    change the fc output to Proposal temp MemPool.
    Each Line of the Proposal has 6 bits.
    The Format of the Proposal is:
    0-3: The four coordinate of the bbox, x1,y1,x2, y2
    4: The Confidence Score of the bbox
    5: The suprressed flag
    ******************************************/
    for (j = 0; j < u32ClsScoreChannels; j++)
    {
        for (i = 0; i < u32RoiCnt; i++)
        {
            f32ProposalWidth = ps32Proposals[SAMPLE_IVE_COORDI_NUM*i + 2] - ps32Proposals[SAMPLE_IVE_COORDI_NUM*i] + 1;
            f32ProposalHeight = ps32Proposals[SAMPLE_IVE_COORDI_NUM*i + 3] - ps32Proposals[SAMPLE_IVE_COORDI_NUM*i + 1] + 1;
            f32ProposalCenterX = ps32Proposals[SAMPLE_IVE_COORDI_NUM*i] + 0.5 * f32ProposalWidth;
            f32ProposalCenterY = ps32Proposals[SAMPLE_IVE_COORDI_NUM*i + 1] + 0.5 * f32ProposalHeight;

            u32FcBboxIndex = SAMPLE_IVE_COORDI_NUM * i;
            f32PredCenterX = ((HI_FLOAT)ps32FcBboxMemPool[u32FcBboxIndex] / 4096) * f32ProposalWidth + f32ProposalCenterX;
            f32PredCenterY = ((HI_FLOAT)ps32FcBboxMemPool[u32FcBboxIndex + 1] / 4096) * f32ProposalHeight + f32ProposalCenterY;
            f32PredW = f32ProposalWidth * IVE_QuickExp(ps32FcBboxMemPool[u32FcBboxIndex + 2]);
            f32PredH = f32ProposalHeight * IVE_QuickExp(ps32FcBboxMemPool[u32FcBboxIndex + 3]);

            u32ProposalMemPoolIndex = SAMPLE_IVE_PROPOSAL_WIDTH * i;
            ps32ProposalMemPool[u32ProposalMemPoolIndex] = (HI_S32)(f32PredCenterX - 0.5 * f32PredW);
            ps32ProposalMemPool[u32ProposalMemPoolIndex + 1] = (HI_S32)(f32PredCenterY - 0.5 * f32PredH);
            ps32ProposalMemPool[u32ProposalMemPoolIndex + 2] = (HI_S32)(f32PredCenterX + 0.5 * f32PredW);
            ps32ProposalMemPool[u32ProposalMemPoolIndex + 3] = (HI_S32)(f32PredCenterY + 0.5 * f32PredH);
            ps32ProposalMemPool[u32ProposalMemPoolIndex + 4] = (HI_S32)(pf32FcScoresMemPool[u32ClsScoreChannels*i + j] * 4096);
            ps32ProposalMemPool[u32ProposalMemPoolIndex + 5] = 0;   /* suprressed flag */
        }

        /* clip bbox */
        for (i = 0; i < u32RoiCnt; i++)
        {
            u32ProposalMemPoolIndex = SAMPLE_IVE_PROPOSAL_WIDTH * i;
            ps32ProposalMemPool[u32ProposalMemPoolIndex] = ((ps32ProposalMemPool[u32ProposalMemPoolIndex]) >((HI_S32)u32OriImWidth - 1) ? ((HI_S32)u32OriImWidth - 1) : (ps32ProposalMemPool[u32ProposalMemPoolIndex]))>0 ? ((ps32ProposalMemPool[u32ProposalMemPoolIndex])>((HI_S32)u32OriImWidth) ? (u32OriImWidth - 1) : (ps32ProposalMemPool[u32ProposalMemPoolIndex])) : 0;
            ps32ProposalMemPool[u32ProposalMemPoolIndex + 1] = ((ps32ProposalMemPool[u32ProposalMemPoolIndex + 1]) > ((HI_S32)u32OriImHeight - 1) ? ((HI_S32)u32OriImHeight - 1) : (ps32ProposalMemPool[u32ProposalMemPoolIndex + 1]))>0 ? ((ps32ProposalMemPool[u32ProposalMemPoolIndex + 1])>((HI_S32)u32OriImHeight) ? (u32OriImHeight - 1) : (ps32ProposalMemPool[u32ProposalMemPoolIndex + 1])) : 0;
            ps32ProposalMemPool[u32ProposalMemPoolIndex + 2] = ((ps32ProposalMemPool[u32ProposalMemPoolIndex + 2]) > ((HI_S32)u32OriImWidth - 1) ? ((HI_S32)u32OriImWidth - 1) : (ps32ProposalMemPool[u32ProposalMemPoolIndex + 2]))>0 ? ((ps32ProposalMemPool[u32ProposalMemPoolIndex + 2])>((HI_S32)u32OriImWidth) ? (u32OriImWidth - 1) : (ps32ProposalMemPool[u32ProposalMemPoolIndex + 2])) : 0;
            ps32ProposalMemPool[u32ProposalMemPoolIndex + 3] = ((ps32ProposalMemPool[u32ProposalMemPoolIndex + 3]) > ((HI_S32)u32OriImHeight - 1) ? ((HI_S32)u32OriImHeight - 1) : (ps32ProposalMemPool[u32ProposalMemPoolIndex + 3]))>0 ? ((ps32ProposalMemPool[u32ProposalMemPoolIndex + 3])>((HI_S32)u32OriImHeight) ? (u32OriImHeight - 1) : (ps32ProposalMemPool[u32ProposalMemPoolIndex + 3])) : 0;
        }

        ps32ProposalTmp = ps32ProposalMemPool;
        s32Ret = IVE_NonRecursiveArgQuickSort(ps32ProposalTmp, 0, u32RoiCnt - 1, pstStack,u32RoiCnt);
        s32Ret = IVE_NonMaxSuppression(ps32ProposalTmp, u32RoiCnt, u32NmsThresh, u32RoiCnt);
        u32RoiOutCnt = 0;

        ps32DstScore = (HI_S32*)ps32DstScores;
        ps32DstBbox = (HI_S32*)ps32DstRoi;

        ps32DstScore += (HI_S32)u32OffSet;
        ps32DstBbox += (HI_S32)(SAMPLE_IVE_COORDI_NUM * u32OffSet);
        for (i = 0; i < u32RoiCnt; i++)
        {
            u32ProposalMemPoolIndex = SAMPLE_IVE_PROPOSAL_WIDTH * i;
            if (0 == ps32ProposalMemPool[u32ProposalMemPoolIndex + 5] && ps32ProposalMemPool[u32ProposalMemPoolIndex + 4] >(HI_S32)pu32ConfThresh[j]) //Suppression = 0; CONF_THRESH == 0.8
            {
                ps32DstScore[u32RoiOutCnt] = ps32ProposalMemPool[u32ProposalMemPoolIndex + 4];
                ps32DstBbox[u32RoiOutCnt * SAMPLE_IVE_COORDI_NUM] = ps32ProposalMemPool[u32ProposalMemPoolIndex];
                ps32DstBbox[u32RoiOutCnt * SAMPLE_IVE_COORDI_NUM + 1] = ps32ProposalMemPool[u32ProposalMemPoolIndex + 1];
                ps32DstBbox[u32RoiOutCnt * SAMPLE_IVE_COORDI_NUM + 2] = ps32ProposalMemPool[u32ProposalMemPoolIndex + 2];
                ps32DstBbox[u32RoiOutCnt * SAMPLE_IVE_COORDI_NUM + 3] = ps32ProposalMemPool[u32ProposalMemPoolIndex + 3];
                u32RoiOutCnt++;
            }
            if (u32RoiOutCnt >= u32RoiCnt)
            {
                break;
            }
        }
        ps32ClassRoiNum[j] = (HI_S32)u32RoiOutCnt;
        u32OffSet = u32OffSet + u32RoiOutCnt;
    }

    return s32Ret;
}

static HI_U32 SAMPLE_IVE_RpnTmpBufSize(HI_U32 u32NumRatioAnchors,
    HI_U32 u32NumScaleAnchors, HI_U32 u32ConvHeight, HI_U32 u32ConvWidth)
{
    HI_U32 u32AnchorsNum = u32NumRatioAnchors * u32NumScaleAnchors * u32ConvHeight * u32ConvWidth;
    HI_U32 u32AnchorsSize = sizeof(HI_U32) * SAMPLE_IVE_COORDI_NUM * u32AnchorsNum;
    HI_U32 u32BboxDeltaSize = u32AnchorsSize;
    HI_U32 u32ProposalSize = sizeof(HI_U32) * SAMPLE_IVE_PROPOSAL_WIDTH * u32AnchorsNum;
    HI_U32 u32RatioAnchorsSize = sizeof(HI_FLOAT) * u32NumRatioAnchors * SAMPLE_IVE_COORDI_NUM;
    HI_U32 u32ScaleAnchorsSize = sizeof(HI_FLOAT) * u32NumRatioAnchors * u32NumScaleAnchors * SAMPLE_IVE_COORDI_NUM;
    HI_U32 u32ScoreSize = sizeof(HI_FLOAT) * u32AnchorsNum * 2;
    HI_U32 u32StackSize = sizeof( SAMPLE_IVE_STACK_S ) * u32AnchorsNum;
    HI_U32 u32TotalSize = u32AnchorsSize + u32BboxDeltaSize + u32ProposalSize + u32RatioAnchorsSize + u32ScaleAnchorsSize + u32ScoreSize + u32StackSize;
    return u32TotalSize;
}

static HI_U32 SAMPLE_IVE_Rfcn_GetResultTmpBuf(HI_U32 u32MaxRoiNum, HI_U32 u32ClassNum)
{
    HI_U32 u32ScoreSize = sizeof(HI_FLOAT) * u32MaxRoiNum * u32ClassNum;
    HI_U32 u32ProposalSize = sizeof(HI_U32) * u32MaxRoiNum * SAMPLE_IVE_PROPOSAL_WIDTH;
    HI_U32 u32BboxSize = sizeof(HI_U32) * u32MaxRoiNum * SAMPLE_IVE_COORDI_NUM;
    HI_U32 u32StackSize = sizeof(SAMPLE_IVE_STACK_S) * u32MaxRoiNum;
    HI_U32 u32TotalSize = u32ScoreSize + u32ProposalSize + u32BboxSize+u32StackSize;
    return u32TotalSize;
}

static HI_S32 SAMPLE_IVE_Rfcn_Rpn(SAMPLE_SVP_NNIE_PARAM_S*pstNnieParam,
    SAMPLE_SVP_NNIE_RFCN_SOFTWARE_PARAM_S* pstSoftwareParam)
{
    HI_S32 s32Ret = HI_SUCCESS;

    s32Ret = IVE_Rpn(pstSoftwareParam->aps32Conv,pstSoftwareParam->u32NumRatioAnchors,
        pstSoftwareParam->u32NumScaleAnchors,pstSoftwareParam->au32Scales,
        pstSoftwareParam->au32Ratios,pstSoftwareParam->u32OriImHeight,
        pstSoftwareParam->u32OriImWidth,pstSoftwareParam->au32ConvHeight,
        pstSoftwareParam->au32ConvWidth,pstSoftwareParam->au32ConvChannel,
        pstSoftwareParam->u32ConvStride,pstSoftwareParam->u32MaxRoiNum,
        pstSoftwareParam->u32MinSize,pstSoftwareParam->u32SpatialScale,
        pstSoftwareParam->u32NmsThresh,pstSoftwareParam->u32FilterThresh,
        pstSoftwareParam->u32NumBeforeNms,
        SAMPLE_SVP_NNIE_CONVERT_64BIT_ADDR(HI_U32,pstSoftwareParam->stRpnTmpBuf.u64VirAddr),
        SAMPLE_SVP_NNIE_CONVERT_64BIT_ADDR(HI_S32,pstSoftwareParam->stRpnBbox.u64VirAddr),
        &pstSoftwareParam->stRpnBbox.unShape.stWhc.u32Height);

    SAMPLE_COMM_SVP_FlushCache(pstSoftwareParam->stRpnBbox.u64PhyAddr,
        SAMPLE_SVP_NNIE_CONVERT_64BIT_ADDR(HI_VOID,pstSoftwareParam->stRpnBbox.u64VirAddr),
        pstSoftwareParam->stRpnBbox.u32Num*
        pstSoftwareParam->stRpnBbox.unShape.stWhc.u32Chn*
        pstSoftwareParam->stRpnBbox.unShape.stWhc.u32Height*
        pstSoftwareParam->stRpnBbox.u32Stride);
    SAMPLE_SVP_CHECK_EXPR_RET(HI_SUCCESS != s32Ret,s32Ret,SAMPLE_SVP_ERR_LEVEL_ERROR,
        "Error,IVE_Rpn failed!\n");

    return s32Ret;
}

static HI_S32 SAMPLE_IVE_Rfcn_GetResult(SAMPLE_SVP_NNIE_PARAM_S*pstNnieParam,
    SAMPLE_SVP_NNIE_RFCN_SOFTWARE_PARAM_S* pstSoftwareParam)
{
    HI_S32 s32Ret = HI_SUCCESS;
    HI_U32 i = 0;
    HI_S32* ps32Proposal = SAMPLE_SVP_NNIE_CONVERT_64BIT_ADDR(HI_S32,pstSoftwareParam->stRpnBbox.u64VirAddr);

    SAMPLE_SVP_CHECK_EXPR_RET(0 == pstSoftwareParam->stRpnBbox.u64VirAddr,HI_INVALID_VALUE,
        SAMPLE_SVP_ERR_LEVEL_ERROR,"Error,pstSoftwareParam->stRpnBbox.u64VirAddr can't be 0!\n");
    for(i = 0; i < pstSoftwareParam->stRpnBbox.unShape.stWhc.u32Height; i++)
    {
        *(ps32Proposal+SAMPLE_IVE_COORDI_NUM*i) /= SAMPLE_IVE_QUANT_BASE;
        *(ps32Proposal+SAMPLE_IVE_COORDI_NUM*i+1) /= SAMPLE_IVE_QUANT_BASE;
        *(ps32Proposal+SAMPLE_IVE_COORDI_NUM*i+2) /= SAMPLE_IVE_QUANT_BASE;
        *(ps32Proposal+SAMPLE_IVE_COORDI_NUM*i+3) /= SAMPLE_IVE_QUANT_BASE;
    }
    s32Ret = IVE_Rfcn_GetResult(
        SAMPLE_SVP_NNIE_CONVERT_64BIT_ADDR(HI_S32,pstNnieParam->astSegData[1].astDst[0].u64VirAddr),
        pstNnieParam->astSegData[1].astDst[0].u32Stride,
        SAMPLE_SVP_NNIE_CONVERT_64BIT_ADDR(HI_S32,pstNnieParam->astSegData[2].astDst[0].u64VirAddr),
        pstNnieParam->astSegData[2].astDst[0].u32Stride,
        SAMPLE_SVP_NNIE_CONVERT_64BIT_ADDR(HI_S32,pstSoftwareParam->stRpnBbox.u64VirAddr),
        pstSoftwareParam->stRpnBbox.unShape.stWhc.u32Height,
        pstSoftwareParam->au32ConfThresh,pstSoftwareParam->u32MaxRoiNum,
        pstSoftwareParam->u32ClassNum,pstSoftwareParam->u32OriImWidth,
        pstSoftwareParam->u32OriImHeight,pstSoftwareParam->u32ValidNmsThresh,
        SAMPLE_SVP_NNIE_CONVERT_64BIT_ADDR(HI_U32,pstSoftwareParam->stGetResultTmpBuf.u64VirAddr),
        SAMPLE_SVP_NNIE_CONVERT_64BIT_ADDR(HI_S32,pstSoftwareParam->stDstScore.u64VirAddr),
        SAMPLE_SVP_NNIE_CONVERT_64BIT_ADDR(HI_S32,pstSoftwareParam->stDstRoi.u64VirAddr),
        SAMPLE_SVP_NNIE_CONVERT_64BIT_ADDR(HI_S32,pstSoftwareParam->stClassRoiNum.u64VirAddr));
    SAMPLE_SVP_CHECK_EXPR_RET(HI_SUCCESS != s32Ret,s32Ret,SAMPLE_SVP_ERR_LEVEL_ERROR,
        "Error,IVE_Rfcn_GetResult failed!\n");
    return s32Ret;
}

/******************************************************************************
* function : Rfcn software deinit
******************************************************************************/
static HI_S32 SAMPLE_IVE_Rfcn_SoftwareDeinit(SAMPLE_SVP_NNIE_RFCN_SOFTWARE_PARAM_S* pstSoftWareParam)
{
    HI_S32 s32Ret = HI_SUCCESS;
    SAMPLE_SVP_CHECK_EXPR_RET(NULL== pstSoftWareParam,HI_INVALID_VALUE,SAMPLE_SVP_ERR_LEVEL_ERROR,
        "Error, pstSoftWareParam can't be NULL!\n");
    if(0!=pstSoftWareParam->stRpnTmpBuf.u64PhyAddr && 0!=pstSoftWareParam->stRpnTmpBuf.u64VirAddr)
    {
        SAMPLE_SVP_MMZ_FREE(pstSoftWareParam->stRpnTmpBuf.u64PhyAddr,
            pstSoftWareParam->stRpnTmpBuf.u64VirAddr);
        pstSoftWareParam->stRpnTmpBuf.u64PhyAddr = 0;
        pstSoftWareParam->stRpnTmpBuf.u64VirAddr = 0;
    }
    return s32Ret;
}

/******************************************************************************
* function : Rfcn Deinit
******************************************************************************/
static HI_S32 SAMPLE_IVE_RfcnDeinit(SAMPLE_SVP_NNIE_PARAM_S *pstNnieParam,
    SAMPLE_SVP_NNIE_RFCN_SOFTWARE_PARAM_S* pstSoftWareParam,SAMPLE_SVP_NNIE_MODEL_S *pstNnieModel)
{
    HI_S32 s32Ret = HI_SUCCESS;
    /*hardware deinit*/
    if(pstNnieParam!=NULL)
    {
        s32Ret = SAMPLE_COMM_SVP_NNIE_ParamDeinit(pstNnieParam);
        SAMPLE_SVP_CHECK_EXPR_TRACE(HI_SUCCESS != s32Ret,SAMPLE_SVP_ERR_LEVEL_ERROR,
            "Error,SAMPLE_COMM_SVP_NNIE_ParamDeinit failed!\n");
    }
    /*software deinit*/
    if(pstSoftWareParam!=NULL)
    {
        s32Ret = SAMPLE_IVE_Rfcn_SoftwareDeinit(pstSoftWareParam);
        SAMPLE_SVP_CHECK_EXPR_TRACE(HI_SUCCESS != s32Ret,SAMPLE_SVP_ERR_LEVEL_ERROR,
            "Error,SAMPLE_SVP_NNIE_Rfcn_SoftwareDeinit failed!\n");
    }
    /*model deinit*/
    if(pstNnieModel!=NULL)
    {
        s32Ret = SAMPLE_COMM_SVP_NNIE_UnloadModel(pstNnieModel);
        SAMPLE_SVP_CHECK_EXPR_TRACE(HI_SUCCESS != s32Ret,SAMPLE_SVP_ERR_LEVEL_ERROR,
            "Error,SAMPLE_COMM_SVP_NNIE_UnloadModel failed!\n");
    }
    return s32Ret;
}

/******************************************************************************
* function : Rfcn software para init
******************************************************************************/
static HI_S32 SAMPLE_IVE_Rfcn_SoftwareInit(SAMPLE_SVP_NNIE_CFG_S* pstCfg,
    SAMPLE_SVP_NNIE_PARAM_S *pstNnieParam, SAMPLE_SVP_NNIE_RFCN_SOFTWARE_PARAM_S* pstSoftWareParam)
{
    HI_U32 i = 0, j = 0;
    HI_U32 u32RpnTmpBufSize = 0;
    HI_U32 u32RpnBboxBufSize = 0;
    HI_U32 u32GetResultTmpBufSize = 0;
    HI_U32 u32DstRoiSize = 0;
    HI_U32 u32DstScoreSize = 0;
    HI_U32 u32ClassRoiNumSize = 0;
    HI_U32 u32ClassNum = 0;
    HI_U32 u32TotalSize = 0;
    HI_S32 s32Ret = HI_SUCCESS;
    HI_U64 u64PhyAddr = 0;
    HI_U8* pu8VirAddr = NULL;

    /*init Rpn para*/
    pstSoftWareParam->u32MaxRoiNum = pstCfg->u32MaxRoiNum;
    pstSoftWareParam->u32ClassNum = 21;
    pstSoftWareParam->u32NumRatioAnchors = 3;
    pstSoftWareParam->u32NumScaleAnchors = 3;
    pstSoftWareParam->au32Scales[0] = 8 * SAMPLE_IVE_QUANT_BASE;
    pstSoftWareParam->au32Scales[1] = 16 * SAMPLE_IVE_QUANT_BASE;
    pstSoftWareParam->au32Scales[2] = 32 * SAMPLE_IVE_QUANT_BASE;
    pstSoftWareParam->au32Ratios[0] = 0.5 * SAMPLE_IVE_QUANT_BASE;
    pstSoftWareParam->au32Ratios[1] = 1 * SAMPLE_IVE_QUANT_BASE;
    pstSoftWareParam->au32Ratios[2] = 2 * SAMPLE_IVE_QUANT_BASE;
    pstSoftWareParam->u32OriImHeight = pstNnieParam->astSegData[0].astSrc[0].unShape.stWhc.u32Height;
    pstSoftWareParam->u32OriImWidth = pstNnieParam->astSegData[0].astSrc[0].unShape.stWhc.u32Width;
    pstSoftWareParam->u32MinSize = 16;
    pstSoftWareParam->u32FilterThresh = 0;
    pstSoftWareParam->u32SpatialScale = (HI_U32)(0.0625 * SAMPLE_IVE_QUANT_BASE);
    pstSoftWareParam->u32NmsThresh = (HI_U32)(0.7 * SAMPLE_IVE_QUANT_BASE);
    pstSoftWareParam->u32FilterThresh = 0;
    pstSoftWareParam->u32NumBeforeNms = 6000;
    for(i = 0; i < pstSoftWareParam->u32ClassNum; i++)
    {
        pstSoftWareParam->au32ConfThresh[i] = 1;
        pstSoftWareParam->af32ScoreThr[i] = 0.8f;
    }
    pstSoftWareParam->u32ValidNmsThresh = (HI_U32)(0.3 * 4096);

    /*set rpn input data info, the input info is set according to RPN data layers' name*/
    for(i = 0; i < 2; i++)
    {
        for(j = 0; j < pstNnieParam->pstModel->astSeg[0].u16DstNum; j++)
        {
            if(0 == strncmp(pstNnieParam->pstModel->astSeg[0].astDstNode[j].szName,
                    pstSoftWareParam->apcRpnDataLayerName[i],
                    SVP_NNIE_NODE_NAME_LEN))
            {
                pstSoftWareParam->aps32Conv[i] = SAMPLE_SVP_NNIE_CONVERT_64BIT_ADDR(HI_S32,pstNnieParam->astSegData[0].astDst[j].u64VirAddr);
                pstSoftWareParam->au32ConvHeight[i] = pstNnieParam->pstModel->astSeg[0].astDstNode[j].unShape.stWhc.u32Height;
                pstSoftWareParam->au32ConvWidth[i] = pstNnieParam->pstModel->astSeg[0].astDstNode[j].unShape.stWhc.u32Width;
                pstSoftWareParam->au32ConvChannel[i] = pstNnieParam->pstModel->astSeg[0].astDstNode[j].unShape.stWhc.u32Chn;
                break;
            }
        }
        SAMPLE_SVP_CHECK_EXPR_RET((j == pstNnieParam->pstModel->astSeg[0].u16DstNum),
            HI_FAILURE,SAMPLE_SVP_ERR_LEVEL_ERROR,"Error,failed to find report node %s!\n",
            pstSoftWareParam->apcRpnDataLayerName[i]);
        if(0 == i)
        {
            pstSoftWareParam->u32ConvStride = pstNnieParam->astSegData[0].astDst[j].u32Stride;
        }
    }

    pstSoftWareParam->stRpnBbox.enType = SVP_BLOB_TYPE_S32;
    pstSoftWareParam->stRpnBbox.unShape.stWhc.u32Chn = 1;
    pstSoftWareParam->stRpnBbox.unShape.stWhc.u32Height = pstCfg->u32MaxRoiNum;
    pstSoftWareParam->stRpnBbox.unShape.stWhc.u32Width = SAMPLE_SVP_COORDI_NUM;
    pstSoftWareParam->stRpnBbox.u32Stride = SAMPLE_SVP_NNIE_ALIGN16(SAMPLE_SVP_COORDI_NUM*sizeof(HI_U32));
    pstSoftWareParam->stRpnBbox.u32Num = 1;

    /*malloc software mem*/
    u32RpnTmpBufSize = SAMPLE_IVE_RpnTmpBufSize(pstSoftWareParam->u32NumRatioAnchors,
        pstSoftWareParam->u32NumScaleAnchors,pstSoftWareParam->au32ConvHeight[0],
        pstSoftWareParam->au32ConvWidth[0]);
    u32RpnTmpBufSize = SAMPLE_SVP_NNIE_ALIGN16(u32RpnTmpBufSize);
    u32RpnBboxBufSize = pstSoftWareParam->stRpnBbox.u32Num*
        pstSoftWareParam->stRpnBbox.unShape.stWhc.u32Height*pstSoftWareParam->stRpnBbox.u32Stride;
    u32GetResultTmpBufSize = SAMPLE_IVE_Rfcn_GetResultTmpBuf(pstCfg->u32MaxRoiNum,pstSoftWareParam->u32ClassNum);
    u32GetResultTmpBufSize = SAMPLE_SVP_NNIE_ALIGN16(u32GetResultTmpBufSize);
    u32ClassNum = pstSoftWareParam->u32ClassNum;
    u32DstRoiSize = SAMPLE_SVP_NNIE_ALIGN16(u32ClassNum*pstCfg->u32MaxRoiNum*sizeof(HI_U32)*SAMPLE_IVE_COORDI_NUM);
    u32DstScoreSize = SAMPLE_SVP_NNIE_ALIGN16(u32ClassNum*pstCfg->u32MaxRoiNum*sizeof(HI_U32));
    u32ClassRoiNumSize = SAMPLE_SVP_NNIE_ALIGN16(u32ClassNum*sizeof(HI_U32));
    u32TotalSize = u32RpnTmpBufSize + u32RpnBboxBufSize + u32GetResultTmpBufSize + u32DstRoiSize +
        u32DstScoreSize + u32ClassRoiNumSize;

    s32Ret = SAMPLE_COMM_SVP_MallocCached("SAMPLE_RFCN_INIT",NULL,(HI_U64*)&u64PhyAddr,
        (void**)&pu8VirAddr,u32TotalSize);
    SAMPLE_SVP_CHECK_EXPR_RET(HI_SUCCESS != s32Ret,s32Ret,SAMPLE_SVP_ERR_LEVEL_ERROR,
        "Error,Malloc memory failed!\n");
    memset(pu8VirAddr,0, u32TotalSize);
    SAMPLE_COMM_SVP_FlushCache(u64PhyAddr,(void*)pu8VirAddr,u32TotalSize);

    pstSoftWareParam->stRpnTmpBuf.u64PhyAddr = u64PhyAddr;
    pstSoftWareParam->stRpnTmpBuf.u64VirAddr = (HI_U64)((HI_UL)pu8VirAddr);
    pstSoftWareParam->stRpnTmpBuf.u32Size = u32RpnTmpBufSize;

    pstSoftWareParam->stRpnBbox.u64PhyAddr = u64PhyAddr+u32RpnTmpBufSize;
    pstSoftWareParam->stRpnBbox.u64VirAddr = (HI_U64)((HI_UL)pu8VirAddr)+u32RpnTmpBufSize;

    pstSoftWareParam->stGetResultTmpBuf.u64PhyAddr = u64PhyAddr+u32RpnTmpBufSize+u32RpnBboxBufSize;
    pstSoftWareParam->stGetResultTmpBuf.u64VirAddr = (HI_U64)((HI_UL)pu8VirAddr+u32RpnTmpBufSize+u32RpnBboxBufSize);
    pstSoftWareParam->stGetResultTmpBuf.u32Size = u32GetResultTmpBufSize;

    pstSoftWareParam->stDstRoi.enType = SVP_BLOB_TYPE_S32;
    pstSoftWareParam->stDstRoi.u64PhyAddr = u64PhyAddr+u32RpnTmpBufSize+u32RpnBboxBufSize+u32GetResultTmpBufSize;
    pstSoftWareParam->stDstRoi.u64VirAddr = (HI_U64)((HI_UL)pu8VirAddr+u32RpnTmpBufSize+u32RpnBboxBufSize+u32GetResultTmpBufSize);
    pstSoftWareParam->stDstRoi.u32Stride = SAMPLE_SVP_NNIE_ALIGN16(u32ClassNum*pstSoftWareParam->u32MaxRoiNum*sizeof(HI_U32)*SAMPLE_IVE_COORDI_NUM);
    pstSoftWareParam->stDstRoi.u32Num = 1;
    pstSoftWareParam->stDstRoi.unShape.stWhc.u32Chn = 1;
    pstSoftWareParam->stDstRoi.unShape.stWhc.u32Height = 1;
    pstSoftWareParam->stDstRoi.unShape.stWhc.u32Width = u32ClassNum*pstSoftWareParam->u32MaxRoiNum*SAMPLE_IVE_COORDI_NUM;

    pstSoftWareParam->stDstScore.enType = SVP_BLOB_TYPE_S32;
    pstSoftWareParam->stDstScore.u64PhyAddr = u64PhyAddr+u32RpnTmpBufSize+u32RpnBboxBufSize+u32GetResultTmpBufSize+u32DstRoiSize;
    pstSoftWareParam->stDstScore.u64VirAddr = (HI_U64)((HI_UL)pu8VirAddr+u32RpnTmpBufSize+u32RpnBboxBufSize+u32GetResultTmpBufSize+u32DstRoiSize);
    pstSoftWareParam->stDstScore.u32Stride = SAMPLE_SVP_NNIE_ALIGN16(u32ClassNum*pstSoftWareParam->u32MaxRoiNum*sizeof(HI_U32));
    pstSoftWareParam->stDstScore.u32Num = 1;
    pstSoftWareParam->stDstScore.unShape.stWhc.u32Chn = 1;
    pstSoftWareParam->stDstScore.unShape.stWhc.u32Height = 1;
    pstSoftWareParam->stDstScore.unShape.stWhc.u32Width = u32ClassNum*pstSoftWareParam->u32MaxRoiNum;

    pstSoftWareParam->stClassRoiNum.enType = SVP_BLOB_TYPE_S32;
    pstSoftWareParam->stClassRoiNum.u64PhyAddr = u64PhyAddr+u32RpnTmpBufSize+u32RpnBboxBufSize+u32GetResultTmpBufSize+u32DstRoiSize+u32DstScoreSize;
    pstSoftWareParam->stClassRoiNum.u64VirAddr = (HI_U64)((HI_UL)pu8VirAddr+u32RpnTmpBufSize+u32RpnBboxBufSize+u32GetResultTmpBufSize+u32DstRoiSize+u32DstScoreSize);
    pstSoftWareParam->stClassRoiNum.u32Stride = SAMPLE_SVP_NNIE_ALIGN16(u32ClassNum*sizeof(HI_U32));
    pstSoftWareParam->stClassRoiNum.u32Num = 1;
    pstSoftWareParam->stClassRoiNum.unShape.stWhc.u32Chn = 1;
    pstSoftWareParam->stClassRoiNum.unShape.stWhc.u32Height = 1;
    pstSoftWareParam->stClassRoiNum.unShape.stWhc.u32Width = u32ClassNum;
    return s32Ret;
}

/******************************************************************************
* function : Rfcn init
******************************************************************************/
static HI_S32 SAMPLE_IVE_Rfcn_ParamInit(SAMPLE_SVP_NNIE_CFG_S* pstCfg,
    SAMPLE_SVP_NNIE_PARAM_S *pstNnieParam, SAMPLE_SVP_NNIE_RFCN_SOFTWARE_PARAM_S* pstSoftWareParam)
{
    HI_S32 s32Ret = HI_SUCCESS;
    /*init hardware para*/
    s32Ret = SAMPLE_COMM_SVP_NNIE_ParamInit(pstCfg,pstNnieParam);
    SAMPLE_SVP_CHECK_EXPR_GOTO(HI_SUCCESS != s32Ret,INIT_FAIL_0,SAMPLE_SVP_ERR_LEVEL_ERROR,
        "Error(%#x),SAMPLE_COMM_SVP_NNIE_ParamInit failed!\n",s32Ret);

    /*init software para*/
    s32Ret = SAMPLE_IVE_Rfcn_SoftwareInit(pstCfg,pstNnieParam,pstSoftWareParam);
    SAMPLE_SVP_CHECK_EXPR_GOTO(HI_SUCCESS != s32Ret,INIT_FAIL_0,SAMPLE_SVP_ERR_LEVEL_ERROR,
        "Error(%#x),SAMPLE_SVP_NNIE_Rfcn_SoftwareInit failed!\n",s32Ret);

    return s32Ret;
INIT_FAIL_0:
    s32Ret = SAMPLE_IVE_RfcnDeinit(pstNnieParam,pstSoftWareParam,NULL);
    SAMPLE_SVP_CHECK_EXPR_RET(HI_SUCCESS != s32Ret,s32Ret,SAMPLE_SVP_ERR_LEVEL_ERROR,
            "Error(%#x),SAMPLE_SVP_NNIE_Rfcn_Deinit failed!\n",s32Ret);
    return HI_FAILURE;

}

/******************************************************************************
* function : roi to rect
******************************************************************************/
HI_S32 SAMPLE_IVE_RoiToRect(SVP_BLOB_S *pstDstScore,
    SVP_BLOB_S *pstDstRoi, SVP_BLOB_S *pstClassRoiNum, HI_FLOAT *paf32ScoreThr,
    HI_BOOL bRmBg,SAMPLE_SVP_NNIE_RECT_ARRAY_S *pstRect,
    HI_U32 u32SrcWidth, HI_U32 u32SrcHeight,HI_U32 u32DstWidth,HI_U32 u32DstHeight)
{
    HI_U32 i = 0, j = 0;
    HI_U32 u32RoiNumBias = 0;
    HI_U32 u32ScoreBias = 0;
    HI_U32 u32BboxBias = 0;
    HI_FLOAT f32Score = 0.0f;
    HI_S32* ps32Score = SAMPLE_SVP_NNIE_CONVERT_64BIT_ADDR(HI_S32,pstDstScore->u64VirAddr);
    HI_S32* ps32Roi = SAMPLE_SVP_NNIE_CONVERT_64BIT_ADDR(HI_S32,pstDstRoi->u64VirAddr);
    HI_S32* ps32ClassRoiNum = SAMPLE_SVP_NNIE_CONVERT_64BIT_ADDR(HI_S32,pstClassRoiNum->u64VirAddr);
    HI_U32 u32ClassNum = pstClassRoiNum->unShape.stWhc.u32Width;
    HI_U32 u32RoiNumTmp = 0;

    SAMPLE_SVP_CHECK_EXPR_RET(u32ClassNum > SAMPLE_SVP_NNIE_MAX_CLASS_NUM ,HI_ERR_SVP_NNIE_ILLEGAL_PARAM,SAMPLE_SVP_ERR_LEVEL_ERROR,
        "Error(%#x),u32ClassNum(%u) must be less than or equal %u to!\n",HI_ERR_SVP_NNIE_ILLEGAL_PARAM,u32ClassNum, SAMPLE_SVP_NNIE_MAX_CLASS_NUM);
    pstRect->u32TotalNum = 0;
    pstRect->u32ClsNum = u32ClassNum;
    if (bRmBg)
    {
        pstRect->au32RoiNum[0] = 0;
        u32RoiNumBias += ps32ClassRoiNum[0];
        for (i = 1; i < u32ClassNum; i++)
        {
            u32ScoreBias = u32RoiNumBias;
            u32BboxBias = u32RoiNumBias * SAMPLE_IVE_COORDI_NUM;
            u32RoiNumTmp = 0;
            /*if the confidence score greater than result thresh, the result will be drawed*/
            if(((HI_FLOAT)ps32Score[u32ScoreBias] / SAMPLE_IVE_QUANT_BASE >=
                paf32ScoreThr[i])  &&  (ps32ClassRoiNum[i] != 0))
            {
                for (j = 0; j < (HI_U32)ps32ClassRoiNum[i]; j++)
                {
                    /*Score is descend order*/
                    f32Score = (HI_FLOAT)ps32Score[u32ScoreBias + j] / SAMPLE_IVE_QUANT_BASE;
                    if ((f32Score < paf32ScoreThr[i]) || (u32RoiNumTmp >= SAMPLE_SVP_NNIE_MAX_ROI_NUM_OF_CLASS))
                    {
                        break;
                    }

                    pstRect->astRect[i][u32RoiNumTmp].astPoint[0].s32X = (HI_U32)((HI_FLOAT)ps32Roi[u32BboxBias + j*SAMPLE_IVE_COORDI_NUM] / (HI_FLOAT)u32SrcWidth * (HI_FLOAT)u32DstWidth) & (~1) ;
                    pstRect->astRect[i][u32RoiNumTmp].astPoint[0].s32Y = (HI_U32)((HI_FLOAT)ps32Roi[u32BboxBias + j*SAMPLE_IVE_COORDI_NUM + 1] / (HI_FLOAT)u32SrcHeight * (HI_FLOAT)u32DstHeight) & (~1);

                    pstRect->astRect[i][u32RoiNumTmp].astPoint[1].s32X = (HI_U32)((HI_FLOAT)ps32Roi[u32BboxBias + j*SAMPLE_IVE_COORDI_NUM + 2]/ (HI_FLOAT)u32SrcWidth * (HI_FLOAT)u32DstWidth) & (~1);
                    pstRect->astRect[i][u32RoiNumTmp].astPoint[1].s32Y = pstRect->astRect[i][u32RoiNumTmp].astPoint[0].s32Y;

                    pstRect->astRect[i][u32RoiNumTmp].astPoint[2].s32X = pstRect->astRect[i][u32RoiNumTmp].astPoint[1].s32X;
                    pstRect->astRect[i][u32RoiNumTmp].astPoint[2].s32Y = (HI_U32)((HI_FLOAT)ps32Roi[u32BboxBias + j*SAMPLE_IVE_COORDI_NUM + 3] / (HI_FLOAT)u32SrcHeight * (HI_FLOAT)u32DstHeight) & (~1);

                    pstRect->astRect[i][u32RoiNumTmp].astPoint[3].s32X =  pstRect->astRect[i][u32RoiNumTmp].astPoint[0].s32X;
                    pstRect->astRect[i][u32RoiNumTmp].astPoint[3].s32Y =  pstRect->astRect[i][u32RoiNumTmp].astPoint[2].s32Y;

                    u32RoiNumTmp++;
                }

            }

            pstRect->au32RoiNum[i] = u32RoiNumTmp;
            pstRect->u32TotalNum += u32RoiNumTmp;
            u32RoiNumBias += ps32ClassRoiNum[i];
        }

    }
    return HI_SUCCESS;
}

/******************************************************************************
* function : NNIE Forward
******************************************************************************/
static HI_S32 SAMPLE_IVE_Forward(SAMPLE_SVP_NNIE_PARAM_S *pstNnieParam,
    SAMPLE_SVP_NNIE_INPUT_DATA_INDEX_S* pstInputDataIdx,
    SAMPLE_SVP_NNIE_PROCESS_SEG_INDEX_S* pstProcSegIdx,HI_BOOL bInstant)
{
    HI_S32 s32Ret = HI_SUCCESS;
    HI_U32 i = 0, j = 0;
    HI_BOOL bFinish = HI_FALSE;
    SVP_NNIE_HANDLE hSvpNnieHandle = 0;
    HI_U32 u32TotalStepNum = 0;

    SAMPLE_COMM_SVP_FlushCache(pstNnieParam->astForwardCtrl[pstProcSegIdx->u32SegIdx].stTskBuf.u64PhyAddr,
        SAMPLE_SVP_NNIE_CONVERT_64BIT_ADDR(HI_VOID,pstNnieParam->astForwardCtrl[pstProcSegIdx->u32SegIdx].stTskBuf.u64VirAddr),
        pstNnieParam->astForwardCtrl[pstProcSegIdx->u32SegIdx].stTskBuf.u32Size);

    for(i = 0; i < pstNnieParam->astForwardCtrl[pstProcSegIdx->u32SegIdx].u32DstNum; i++)
    {
        if(SVP_BLOB_TYPE_SEQ_S32 == pstNnieParam->astSegData[pstProcSegIdx->u32SegIdx].astDst[i].enType)
        {
            for(j = 0; j < pstNnieParam->astSegData[pstProcSegIdx->u32SegIdx].astDst[i].u32Num; j++)
            {
                u32TotalStepNum += *(SAMPLE_SVP_NNIE_CONVERT_64BIT_ADDR(HI_U32,pstNnieParam->astSegData[pstProcSegIdx->u32SegIdx].astDst[i].unShape.stSeq.u64VirAddrStep)+j);
            }
            SAMPLE_COMM_SVP_FlushCache(pstNnieParam->astSegData[pstProcSegIdx->u32SegIdx].astDst[i].u64PhyAddr,
                SAMPLE_SVP_NNIE_CONVERT_64BIT_ADDR(HI_VOID,pstNnieParam->astSegData[pstProcSegIdx->u32SegIdx].astDst[i].u64VirAddr),
                u32TotalStepNum*pstNnieParam->astSegData[pstProcSegIdx->u32SegIdx].astDst[i].u32Stride);

        }
        else
        {
            SAMPLE_COMM_SVP_FlushCache(pstNnieParam->astSegData[pstProcSegIdx->u32SegIdx].astDst[i].u64PhyAddr,
                SAMPLE_SVP_NNIE_CONVERT_64BIT_ADDR(HI_VOID,pstNnieParam->astSegData[pstProcSegIdx->u32SegIdx].astDst[i].u64VirAddr),
                pstNnieParam->astSegData[pstProcSegIdx->u32SegIdx].astDst[i].u32Num*
                pstNnieParam->astSegData[pstProcSegIdx->u32SegIdx].astDst[i].unShape.stWhc.u32Chn*
                pstNnieParam->astSegData[pstProcSegIdx->u32SegIdx].astDst[i].unShape.stWhc.u32Height*
                pstNnieParam->astSegData[pstProcSegIdx->u32SegIdx].astDst[i].u32Stride);
        }
    }

    /*set input blob according to node name*/
    if(pstInputDataIdx->u32SegIdx != pstProcSegIdx->u32SegIdx)
    {
        for(i = 0; i < pstNnieParam->pstModel->astSeg[pstProcSegIdx->u32SegIdx].u16SrcNum; i++)
        {
            for(j = 0; j < pstNnieParam->pstModel->astSeg[pstInputDataIdx->u32SegIdx].u16DstNum; j++)
            {
                if(0 == strncmp(pstNnieParam->pstModel->astSeg[pstInputDataIdx->u32SegIdx].astDstNode[j].szName,
                    pstNnieParam->pstModel->astSeg[pstProcSegIdx->u32SegIdx].astSrcNode[i].szName,
                    SVP_NNIE_NODE_NAME_LEN))
                {
                    pstNnieParam->astSegData[pstProcSegIdx->u32SegIdx].astSrc[i] =
                        pstNnieParam->astSegData[pstInputDataIdx->u32SegIdx].astDst[j];
                    break;
                }
            }
            SAMPLE_SVP_CHECK_EXPR_RET((j == pstNnieParam->pstModel->astSeg[pstInputDataIdx->u32SegIdx].u16DstNum),
                HI_FAILURE,SAMPLE_SVP_ERR_LEVEL_ERROR,"Error,can't find %d-th seg's %d-th src blob!\n",
                pstProcSegIdx->u32SegIdx,i);
        }
    }

    /*NNIE_Forward*/
    s32Ret = HI_MPI_SVP_NNIE_Forward(&hSvpNnieHandle,
        pstNnieParam->astSegData[pstProcSegIdx->u32SegIdx].astSrc,
        pstNnieParam->pstModel, pstNnieParam->astSegData[pstProcSegIdx->u32SegIdx].astDst,
        &pstNnieParam->astForwardCtrl[pstProcSegIdx->u32SegIdx], bInstant);
    SAMPLE_SVP_CHECK_EXPR_RET(HI_SUCCESS != s32Ret,s32Ret,SAMPLE_SVP_ERR_LEVEL_ERROR,
        "Error,HI_MPI_SVP_NNIE_Forward failed!\n");

    if(bInstant)
    {
        /*Wait NNIE finish*/
        while(HI_ERR_SVP_NNIE_QUERY_TIMEOUT == (s32Ret = HI_MPI_SVP_NNIE_Query(pstNnieParam->astForwardCtrl[pstProcSegIdx->u32SegIdx].enNnieId,
            hSvpNnieHandle, &bFinish, HI_TRUE)))
        {
            usleep(100);
            SAMPLE_SVP_TRACE(SAMPLE_SVP_ERR_LEVEL_INFO,
                "HI_MPI_SVP_NNIE_Query Query timeout!\n");
        }
    }
    u32TotalStepNum = 0;

    for(i = 0; i < pstNnieParam->astForwardCtrl[pstProcSegIdx->u32SegIdx].u32DstNum; i++)
    {
        if(SVP_BLOB_TYPE_SEQ_S32 == pstNnieParam->astSegData[pstProcSegIdx->u32SegIdx].astDst[i].enType)
        {
            for(j = 0; j < pstNnieParam->astSegData[pstProcSegIdx->u32SegIdx].astDst[i].u32Num; j++)
            {
                u32TotalStepNum += *(SAMPLE_SVP_NNIE_CONVERT_64BIT_ADDR(HI_U32,pstNnieParam->astSegData[pstProcSegIdx->u32SegIdx].astDst[i].unShape.stSeq.u64VirAddrStep)+j);
            }
            SAMPLE_COMM_SVP_FlushCache(pstNnieParam->astSegData[pstProcSegIdx->u32SegIdx].astDst[i].u64PhyAddr,
                SAMPLE_SVP_NNIE_CONVERT_64BIT_ADDR(HI_VOID,pstNnieParam->astSegData[pstProcSegIdx->u32SegIdx].astDst[i].u64VirAddr),
                u32TotalStepNum*pstNnieParam->astSegData[pstProcSegIdx->u32SegIdx].astDst[i].u32Stride);

        }
        else
        {
            SAMPLE_COMM_SVP_FlushCache(pstNnieParam->astSegData[pstProcSegIdx->u32SegIdx].astDst[i].u64PhyAddr,
                SAMPLE_SVP_NNIE_CONVERT_64BIT_ADDR(HI_VOID,pstNnieParam->astSegData[pstProcSegIdx->u32SegIdx].astDst[i].u64VirAddr),
                pstNnieParam->astSegData[pstProcSegIdx->u32SegIdx].astDst[i].u32Num*
                pstNnieParam->astSegData[pstProcSegIdx->u32SegIdx].astDst[i].unShape.stWhc.u32Chn*
                pstNnieParam->astSegData[pstProcSegIdx->u32SegIdx].astDst[i].unShape.stWhc.u32Height*
                pstNnieParam->astSegData[pstProcSegIdx->u32SegIdx].astDst[i].u32Stride);
        }
    }

    return s32Ret;
}

/******************************************************************************
* function : NNIE ForwardWithBbox
******************************************************************************/
static HI_S32 SAMPLE_IVE_ForwardWithBbox(SAMPLE_SVP_NNIE_PARAM_S *pstNnieParam,
    SAMPLE_SVP_NNIE_INPUT_DATA_INDEX_S* pstInputDataIdx,SVP_SRC_BLOB_S astBbox[],
    SAMPLE_SVP_NNIE_PROCESS_SEG_INDEX_S* pstProcSegIdx,HI_BOOL bInstant)
{
    HI_S32 s32Ret = HI_SUCCESS;
    HI_BOOL bFinish = HI_FALSE;
    SVP_NNIE_HANDLE hSvpNnieHandle = 0;
    HI_U32 u32TotalStepNum = 0;
    HI_U32 i, j;

    SAMPLE_COMM_SVP_FlushCache(pstNnieParam->astForwardWithBboxCtrl[pstProcSegIdx->u32SegIdx].stTskBuf.u64PhyAddr,
        SAMPLE_SVP_NNIE_CONVERT_64BIT_ADDR(HI_VOID,pstNnieParam->astForwardWithBboxCtrl[pstProcSegIdx->u32SegIdx].stTskBuf.u64VirAddr),
        pstNnieParam->astForwardWithBboxCtrl[pstProcSegIdx->u32SegIdx].stTskBuf.u32Size);

    for(i = 0; i < pstNnieParam->astForwardWithBboxCtrl[pstProcSegIdx->u32SegIdx].u32DstNum; i++)
    {
        if(SVP_BLOB_TYPE_SEQ_S32 == pstNnieParam->astSegData[pstProcSegIdx->u32SegIdx].astDst[i].enType)
        {
            for(j = 0; j < pstNnieParam->astSegData[pstProcSegIdx->u32SegIdx].astDst[i].u32Num; j++)
            {
                u32TotalStepNum += *(SAMPLE_SVP_NNIE_CONVERT_64BIT_ADDR(HI_U32,pstNnieParam->astSegData[pstProcSegIdx->u32SegIdx].astDst[i].unShape.stSeq.u64VirAddrStep)+j);
            }
            SAMPLE_COMM_SVP_FlushCache(pstNnieParam->astSegData[pstProcSegIdx->u32SegIdx].astDst[i].u64PhyAddr,
                SAMPLE_SVP_NNIE_CONVERT_64BIT_ADDR(HI_VOID,pstNnieParam->astSegData[pstProcSegIdx->u32SegIdx].astDst[i].u64VirAddr),
                u32TotalStepNum*pstNnieParam->astSegData[pstProcSegIdx->u32SegIdx].astDst[i].u32Stride);
        }
        else
        {
            SAMPLE_COMM_SVP_FlushCache(pstNnieParam->astSegData[pstProcSegIdx->u32SegIdx].astDst[i].u64PhyAddr,
                SAMPLE_SVP_NNIE_CONVERT_64BIT_ADDR(HI_VOID,pstNnieParam->astSegData[pstProcSegIdx->u32SegIdx].astDst[i].u64VirAddr),
                pstNnieParam->astSegData[pstProcSegIdx->u32SegIdx].astDst[i].u32Num*
                pstNnieParam->astSegData[pstProcSegIdx->u32SegIdx].astDst[i].unShape.stWhc.u32Chn*
                pstNnieParam->astSegData[pstProcSegIdx->u32SegIdx].astDst[i].unShape.stWhc.u32Height*
                pstNnieParam->astSegData[pstProcSegIdx->u32SegIdx].astDst[i].u32Stride);
        }
    }

    /*set input blob according to node name*/
    if(pstInputDataIdx->u32SegIdx != pstProcSegIdx->u32SegIdx)
    {
        for(i = 0; i < pstNnieParam->pstModel->astSeg[pstProcSegIdx->u32SegIdx].u16SrcNum; i++)
        {
            for(j = 0; j < pstNnieParam->pstModel->astSeg[pstInputDataIdx->u32SegIdx].u16DstNum; j++)
            {
                if(0 == strncmp(pstNnieParam->pstModel->astSeg[pstInputDataIdx->u32SegIdx].astDstNode[j].szName,
                    pstNnieParam->pstModel->astSeg[pstProcSegIdx->u32SegIdx].astSrcNode[i].szName,
                    SVP_NNIE_NODE_NAME_LEN))
                {
                    pstNnieParam->astSegData[pstProcSegIdx->u32SegIdx].astSrc[i] =
                        pstNnieParam->astSegData[pstInputDataIdx->u32SegIdx].astDst[j];
                    break;
                }
            }
            SAMPLE_SVP_CHECK_EXPR_RET((j == pstNnieParam->pstModel->astSeg[pstInputDataIdx->u32SegIdx].u16DstNum),
                HI_FAILURE,SAMPLE_SVP_ERR_LEVEL_ERROR,"Error,can't find %d-th seg's %d-th src blob!\n",
                pstProcSegIdx->u32SegIdx,i);
        }
    }
    /*NNIE_ForwardWithBbox*/
    s32Ret = HI_MPI_SVP_NNIE_ForwardWithBbox(&hSvpNnieHandle,
        pstNnieParam->astSegData[pstProcSegIdx->u32SegIdx].astSrc,astBbox,
        pstNnieParam->pstModel, pstNnieParam->astSegData[pstProcSegIdx->u32SegIdx].astDst,
        &pstNnieParam->astForwardWithBboxCtrl[pstProcSegIdx->u32SegIdx], bInstant);
    SAMPLE_SVP_CHECK_EXPR_RET(HI_SUCCESS != s32Ret,s32Ret,SAMPLE_SVP_ERR_LEVEL_ERROR,
        "Error,HI_MPI_SVP_NNIE_ForwardWithBbox failed!\n");

    if(bInstant)
    {
        /*Wait NNIE finish*/
        while(HI_ERR_SVP_NNIE_QUERY_TIMEOUT == (s32Ret = HI_MPI_SVP_NNIE_Query(pstNnieParam->astForwardWithBboxCtrl[pstProcSegIdx->u32SegIdx].enNnieId,
            hSvpNnieHandle, &bFinish, HI_TRUE)))
        {
            usleep(100);
            SAMPLE_SVP_TRACE(SAMPLE_SVP_ERR_LEVEL_INFO,
                "HI_MPI_SVP_NNIE_Query Query timeout!\n");
        }
    }

    u32TotalStepNum = 0;

    for(i = 0; i < pstNnieParam->astForwardWithBboxCtrl[pstProcSegIdx->u32SegIdx].u32DstNum; i++)
    {
        if(SVP_BLOB_TYPE_SEQ_S32 == pstNnieParam->astSegData[pstProcSegIdx->u32SegIdx].astDst[i].enType)
        {
            for(j = 0; j < pstNnieParam->astSegData[pstProcSegIdx->u32SegIdx].astDst[i].u32Num; j++)
            {
                u32TotalStepNum += *(SAMPLE_SVP_NNIE_CONVERT_64BIT_ADDR(HI_U32,pstNnieParam->astSegData[pstProcSegIdx->u32SegIdx].astDst[i].unShape.stSeq.u64VirAddrStep)+j);
            }
            SAMPLE_COMM_SVP_FlushCache(pstNnieParam->astSegData[pstProcSegIdx->u32SegIdx].astDst[i].u64PhyAddr,
                SAMPLE_SVP_NNIE_CONVERT_64BIT_ADDR(HI_VOID,pstNnieParam->astSegData[pstProcSegIdx->u32SegIdx].astDst[i].u64VirAddr),
                u32TotalStepNum*pstNnieParam->astSegData[pstProcSegIdx->u32SegIdx].astDst[i].u32Stride);
        }
        else
        {
            SAMPLE_COMM_SVP_FlushCache(pstNnieParam->astSegData[pstProcSegIdx->u32SegIdx].astDst[i].u64PhyAddr,
                SAMPLE_SVP_NNIE_CONVERT_64BIT_ADDR(HI_VOID,pstNnieParam->astSegData[pstProcSegIdx->u32SegIdx].astDst[i].u64VirAddr),
                pstNnieParam->astSegData[pstProcSegIdx->u32SegIdx].astDst[i].u32Num*
                pstNnieParam->astSegData[pstProcSegIdx->u32SegIdx].astDst[i].unShape.stWhc.u32Chn*
                pstNnieParam->astSegData[pstProcSegIdx->u32SegIdx].astDst[i].unShape.stWhc.u32Height*
                pstNnieParam->astSegData[pstProcSegIdx->u32SegIdx].astDst[i].u32Stride);
        }
    }

    return s32Ret;
}


/******************************************************************************
* function : Rfcn Proc
******************************************************************************/
static HI_S32 SAMPLE_IVE_Rfcn_Proc(SAMPLE_SVP_NNIE_PARAM_S *pstParam,
    SAMPLE_SVP_NNIE_RFCN_SOFTWARE_PARAM_S *pstSwParam)
{
    HI_S32 s32Ret = HI_FAILURE;
    SAMPLE_SVP_NNIE_INPUT_DATA_INDEX_S stInputDataIdx = {0};
    SAMPLE_SVP_NNIE_PROCESS_SEG_INDEX_S stProcSegIdx = {0};

    stInputDataIdx.u32SegIdx = 0;
    stInputDataIdx.u32NodeIdx = 0;

    /*NNIE process 0-th seg*/
    stProcSegIdx.u32SegIdx = 0;
    s32Ret = SAMPLE_IVE_Forward(pstParam,&stInputDataIdx,&stProcSegIdx,HI_TRUE);
    SAMPLE_SVP_CHECK_EXPR_RET(HI_SUCCESS != s32Ret,s32Ret,SAMPLE_SVP_ERR_LEVEL_ERROR,
        "Error,SAMPLE_IVE_Forward failed!\n");

    /*RPN*/
    s32Ret = SAMPLE_IVE_Rfcn_Rpn(pstParam, pstSwParam);
    SAMPLE_SVP_CHECK_EXPR_RET(HI_SUCCESS != s32Ret,s32Ret,SAMPLE_SVP_ERR_LEVEL_ERROR,
        "Error,SAMPLE_IVE_Rfcn_Rpn failed!\n");

    /*NNIE process 1-th seg, the input data comes from 3-rd report node of 0-th seg,
      the input roi comes from RPN results*/
    stInputDataIdx.u32SegIdx = 0;
    stInputDataIdx.u32NodeIdx = 3;
    stProcSegIdx.u32SegIdx = 1;
    s32Ret = SAMPLE_IVE_ForwardWithBbox(pstParam,&stInputDataIdx,
        &pstSwParam->stRpnBbox,&stProcSegIdx,HI_TRUE);
    SAMPLE_SVP_CHECK_EXPR_RET(HI_SUCCESS != s32Ret,s32Ret,SAMPLE_SVP_ERR_LEVEL_ERROR,
        "Error,SAMPLE_IVE_ForwardWithBbox failed!\n");

    /*NNIE process 2-nd seg, the input data comes from 4-th report node of 0-th seg
      the input roi comes from RPN results*/
    stInputDataIdx.u32SegIdx = 0;
    stInputDataIdx.u32NodeIdx = 4;
    stProcSegIdx.u32SegIdx = 2;
    s32Ret = SAMPLE_IVE_ForwardWithBbox(pstParam,&stInputDataIdx,
        &pstSwParam->stRpnBbox,&stProcSegIdx,HI_TRUE);
    SAMPLE_SVP_CHECK_EXPR_RET(HI_SUCCESS != s32Ret,s32Ret,SAMPLE_SVP_ERR_LEVEL_ERROR,
        "Error,SAMPLE_IVE_ForwardWithBbox failed!\n");

    /*GetResult*/
    /*if user has changed net struct, please make sure SAMPLE_IVE_Rfcn_GetResult
     function's input datas are correct*/
    s32Ret = SAMPLE_IVE_Rfcn_GetResult(pstParam,pstSwParam);
    SAMPLE_SVP_CHECK_EXPR_RET(HI_SUCCESS != s32Ret,s32Ret,SAMPLE_SVP_ERR_LEVEL_ERROR,
        "Error,SAMPLE_IVE_Rfcn_GetResult failed!\n");

    return s32Ret;
}


/******************************************************************************
* function : Rfcn Proc
******************************************************************************/
static HI_S32 SAMPLE_IVE_Rfcn_GetRect(SAMPLE_SVP_NNIE_PARAM_S *pstParam,
    SAMPLE_SVP_NNIE_RFCN_SOFTWARE_PARAM_S *pstSwParam, VIDEO_FRAME_INFO_S* pstExtFrmInfo,
    HI_U32 u32BaseWidth,HI_U32 u32BaseHeight)
{
    HI_S32 s32Ret = HI_FAILURE;
    SAMPLE_SVP_NNIE_INPUT_DATA_INDEX_S stInputDataIdx = {0};

    stInputDataIdx.u32SegIdx = 0;
    stInputDataIdx.u32NodeIdx = 0;
    /*SP420*/
    pstParam->astSegData[stInputDataIdx.u32SegIdx].astSrc[stInputDataIdx.u32NodeIdx].u64VirAddr = pstExtFrmInfo->stVFrame.u64VirAddr[0];
    pstParam->astSegData[stInputDataIdx.u32SegIdx].astSrc[stInputDataIdx.u32NodeIdx].u64PhyAddr = pstExtFrmInfo->stVFrame.u64PhyAddr[0];
    pstParam->astSegData[stInputDataIdx.u32SegIdx].astSrc[stInputDataIdx.u32NodeIdx].u32Stride  = pstExtFrmInfo->stVFrame.u32Stride[0];

    s32Ret = SAMPLE_IVE_Rfcn_Proc(pstParam,pstSwParam);
    SAMPLE_SVP_CHECK_EXPR_RET(HI_SUCCESS != s32Ret,s32Ret,SAMPLE_SVP_ERR_LEVEL_ERROR,
        "Error,SAMPLE_SVP_NNIE_Rfcn_Proc failed!\n");
    /*draw result, this sample has 21 classes:
     class 0:background     class 1:plane           class 2:bicycle
     class 3:bird           class 4:boat            class 5:bottle
     class 6:bus            class 7:car             class 8:cat
     class 9:chair          class10:cow             class11:diningtable
     class 12:dog           class13:horse           class14:motorbike
     class 15:person        class16:pottedplant     class17:sheep
     class 18:sofa          class19:train           class20:tvmonitor*/
    s32Ret = SAMPLE_IVE_RoiToRect(&(pstSwParam->stDstScore),
    &(pstSwParam->stDstRoi), &(pstSwParam->stClassRoiNum), pstSwParam->af32ScoreThr,HI_TRUE,&(pstSwParam->stRect),
    pstExtFrmInfo->stVFrame.u32Width, pstExtFrmInfo->stVFrame.u32Height,u32BaseWidth,u32BaseHeight);
    SAMPLE_SVP_CHECK_EXPR_RET(HI_SUCCESS != s32Ret,s32Ret,SAMPLE_SVP_ERR_LEVEL_ERROR,
        "Error(%#x),SAMPLE_SVP_NNIE_RoiToRect failed!\n",s32Ret);

    return s32Ret;

}

static HI_VOID SAMPLE_IVE_RectToRoiInfo(SAMPLE_SVP_NNIE_RECT_ARRAY_S *pstRect,
    HI_U32 u32BaseWidth, HI_U32 u32BaseHeight, SAMPLE_IVE_KCF_S *pstKcfInfo)
{
    HI_U32 i, j, k;
    HI_U32 u32PaddW, u32PaddH;

    k = 0;

    for (i = 0; i < pstRect->u32ClsNum; i++)
    {
        if (i != 7)
        {
            continue;
        }
        for(j = 0; j < pstRect->au32RoiNum[i]; j++)
        {
            pstKcfInfo->astRoiInfo[k].stRoi.s24q8X = pstRect->astRect[i][j].astPoint[0].s32X * 256;
            pstKcfInfo->astRoiInfo[k].stRoi.s24q8Y = pstRect->astRect[i][j].astPoint[0].s32Y * 256;

            pstKcfInfo->astRoiInfo[k].stRoi.u32Width = pstRect->astRect[i][j].astPoint[2].s32X
                - pstRect->astRect[i][j].astPoint[0].s32X;
            pstKcfInfo->astRoiInfo[k].stRoi.u32Height = pstRect->astRect[i][j].astPoint[2].s32Y
                - pstRect->astRect[i][j].astPoint[0].s32Y;

            if ((pstKcfInfo->astRoiInfo[k].stRoi.s24q8X >= u32BaseWidth * 256)
                || (pstKcfInfo->astRoiInfo[k].stRoi.s24q8Y >= u32BaseHeight * 256))
            {
                continue;
            }

            u32PaddW = pstKcfInfo->astRoiInfo[k].stRoi.u32Width * pstKcfInfo->u3q5Padding / 32;
            if (u32PaddW > SAMPLE_IVE_KCF_ROI_PADDING_MAX_WIDTH)
            {
                pstKcfInfo->astRoiInfo[k].stRoi.u32Width = SAMPLE_IVE_KCF_ROI_PADDING_MAX_WIDTH * 32 / pstKcfInfo->u3q5Padding;
            }
            else if (u32PaddW < SAMPLE_IVE_KCF_ROI_PADDING_MIN_WIDTH)
            {
                pstKcfInfo->astRoiInfo[k].stRoi.u32Width = SAMPLE_IVE_KCF_ROI_PADDING_MIN_WIDTH * 32 / pstKcfInfo->u3q5Padding + 2;
            }

            u32PaddH = pstKcfInfo->astRoiInfo[k].stRoi.u32Height * pstKcfInfo->u3q5Padding / 32;
            if (u32PaddH > SAMPLE_IVE_KCF_ROI_PADDING_MAX_WIDTH)
            {
                pstKcfInfo->astRoiInfo[k].stRoi.u32Height = SAMPLE_IVE_KCF_ROI_PADDING_MAX_HEIGHT * 32 / pstKcfInfo->u3q5Padding;
            }
            else if (u32PaddH < SAMPLE_IVE_KCF_ROI_PADDING_MIN_WIDTH)
            {
                pstKcfInfo->astRoiInfo[k].stRoi.u32Height = SAMPLE_IVE_KCF_ROI_PADDING_MIN_HEIGHT * 32 / pstKcfInfo->u3q5Padding + 2;
            }

            k++;
            pstKcfInfo->astRoiInfo[k - 1].u32RoiId = k;
            pstKcfInfo->u32RoiNum = k;
            if (k >= 64)
            {
                return;
            }
        }
    }
}

/******************************************************************************
* function : Rfcn detect thread entry
******************************************************************************/
static HI_VOID* SAMPLE_IVE_Rfcn_Detect(HI_VOID* pArgs)
{
    HI_S32 s32Ret;
    SAMPLE_SVP_NNIE_PARAM_S *pstParam;
    SAMPLE_SVP_NNIE_RFCN_SOFTWARE_PARAM_S *pstSwParam;
    HI_U32 u32Width, u32Height;
    VIDEO_FRAME_INFO_S stExtFrmInfo;
    HI_S32 s32VpssGrp = 0;

    pstParam = &s_stRfcnNnieParam;
    pstSwParam = &s_stRfcnSoftwareParam;

    while (HI_FALSE == s_bIveKcfStopSignal)
    {
        SAMPLE_IVE_MUTEX_LOCK(s_stIveKcfInfo.GetFrmMutex);
        if (CNN_PROC_START == s_stIveKcfInfo.enCnnProcStat)
        {
            u32Width = s_stIveKcfInfo.astFrameInfo[0].stVFrame.u32Width;
            u32Height = s_stIveKcfInfo.astFrameInfo[0].stVFrame.u32Height;
            memcpy(&stExtFrmInfo, &s_stIveKcfInfo.astFrameInfo[1], sizeof(VIDEO_FRAME_INFO_S));
        }
        else
        {
            SAMPLE_IVE_MUTEX_UNLOCK(s_stIveKcfInfo.GetFrmMutex);
            continue;
        }
        SAMPLE_IVE_MUTEX_UNLOCK(s_stIveKcfInfo.GetFrmMutex);

        s32Ret = SAMPLE_IVE_Rfcn_GetRect(pstParam, pstSwParam, &stExtFrmInfo, u32Width, u32Height);
        SAMPLE_CHECK_EXPR_GOTO(HI_SUCCESS!=s32Ret, BASE_RELEASE,
            "Error(%#x),SAMPLE_SVP_NNIE_Rfcn_Proc failed!\n", s32Ret);

        SAMPLE_IVE_MUTEX_LOCK(s_stIveKcfInfo.CnnDetectMutex);
        SAMPLE_IVE_RectToRoiInfo(&pstSwParam->stRect, u32Width, u32Height, &s_stIveKcfInfo);
        if (s_stIveKcfInfo.u32RoiNum)
        {
            s_stIveKcfInfo.bNewDetect = HI_TRUE;
        }
        SAMPLE_IVE_MUTEX_UNLOCK(s_stIveKcfInfo.CnnDetectMutex);


        BASE_RELEASE:
            s32Ret = HI_MPI_VPSS_ReleaseChnFrame(s32VpssGrp, 1, &stExtFrmInfo);
            if (HI_SUCCESS != s32Ret)
            {
                SAMPLE_PRT("Error(%#x),HI_MPI_VPSS_ReleaseChnFrame failed!\n",
                    s32Ret);
            }

            SAMPLE_IVE_MUTEX_LOCK(s_stIveKcfInfo.GetFrmMutex);
            s_stIveKcfInfo.enCnnGetFrmStat = CNN_GET_FRM_END;
            s_stIveKcfInfo.enCnnProcStat = CNN_PROC_END;
            SAMPLE_IVE_MUTEX_UNLOCK(s_stIveKcfInfo.GetFrmMutex);
    }

    return HI_NULL;
}

/******************************************************************************
* function : Get frame thread entry
******************************************************************************/
static HI_VOID* SAMPLE_IVE_GetFrame(HI_VOID* pArgs)
{
    VIDEO_FRAME_INFO_S astVideoFrame[2];
    HI_S32 s32MilliSec = 20000;
    HI_S32 s32Ret;

    while (HI_FALSE == s_bIveKcfStopSignal)
    {
    	s32Ret = HI_MPI_VPSS_GetChnFrame(0,0,&astVideoFrame[0], s32MilliSec);
        if (HI_SUCCESS != s32Ret)
        {
            continue;
        }

        SAMPLE_IVE_MUTEX_LOCK(s_stIveKcfInfo.QueueMutex);
        s32Ret = SAMPLE_IVE_QueueAddNode(s_stIveKcfInfo.pstQueueHead, &astVideoFrame[0]);
        if (HI_SUCCESS != s32Ret)
        {
            SAMPLE_PRT("Error(%#x),SAMPLE_IVE_QueueAddNode failed!\n",
                    s32Ret);
            SAMPLE_IVE_MUTEX_UNLOCK(s_stIveKcfInfo.QueueMutex);
            goto RELEASE0;
        }
        SAMPLE_IVE_MUTEX_UNLOCK(s_stIveKcfInfo.QueueMutex);

        SAMPLE_IVE_MUTEX_LOCK(s_stIveKcfInfo.GetFrmMutex);
        if (CNN_GET_FRM_END == s_stIveKcfInfo.enCnnGetFrmStat)
        {
            s_stIveKcfInfo.enCnnGetFrmStat = CNN_GET_FRM_START;
            s32Ret = HI_MPI_VPSS_GetChnFrame(0,1,&astVideoFrame[1],s32MilliSec);
            if (HI_SUCCESS != s32Ret)
            {
                SAMPLE_IVE_MUTEX_UNLOCK(s_stIveKcfInfo.GetFrmMutex);
                continue;
            }
            memcpy(&s_stIveKcfInfo.astFrameInfo[0], &astVideoFrame[0], sizeof(VIDEO_FRAME_INFO_S));
            memcpy(&s_stIveKcfInfo.astFrameInfo[1], &astVideoFrame[1], sizeof(VIDEO_FRAME_INFO_S));
            s_stIveKcfInfo.enCnnProcStat = CNN_PROC_START;
        }
        SAMPLE_IVE_MUTEX_UNLOCK(s_stIveKcfInfo.GetFrmMutex);

        continue;

        RELEASE0:
            s32Ret = HI_MPI_VPSS_ReleaseChnFrame(0,0, &astVideoFrame[0]);
            if (HI_SUCCESS != s32Ret)
            {
                SAMPLE_PRT("Error(%#x),HI_MPI_VPSS_ReleaseChnFrame failed,Grp(0) chn(0)!\n",
                    s32Ret);
            }
    }

	return NULL;
}

HI_FLOAT SAMPLE_IVE_KcfCalcIOU(IVE_RECT_S24Q8_S *pstRect1, IVE_RECT_S24Q8_S *pstRect2)
{
    HI_S32 s32MinX, s32MinY, s32MaxX, s32MaxY;
    HI_FLOAT fArea1, fArea2, fInterArea, fIou;
    HI_S32 s32Width, s32Height;

    s32MinX = SAMPLE_IVE_MAX(pstRect1->s24q8X / 256, pstRect2->s24q8X / 256);
    s32MinY = SAMPLE_IVE_MAX(pstRect1->s24q8Y / 256, pstRect2->s24q8Y / 256);
    s32MaxX = SAMPLE_IVE_MIN(pstRect1->s24q8X / 256 + pstRect1->u32Width, pstRect2->s24q8X / 256 + pstRect2->u32Width);
    s32MaxY = SAMPLE_IVE_MIN(pstRect1->s24q8Y / 256 + pstRect1->u32Height, pstRect2->s24q8Y / 256 + pstRect2->u32Height);

    s32Width = s32MaxX - s32MinX + 1;
    s32Height = s32MaxY - s32MinY + 1;

    s32Width = s32Width > 0 ? s32Width : 0;
    s32Height = s32Height > 0 ? s32Height : 0;

    fInterArea = (HI_FLOAT)((HI_U32)s32Width * (HI_U32)s32Height);

    fArea1 = (HI_FLOAT)(pstRect1->u32Width  * pstRect1->u32Height);
    fArea2 = (pstRect2->u32Width * pstRect2->u32Height);

    fIou = fInterArea / (fArea1 + fArea2 - fInterArea);

    return fIou;
}

HI_S32 SAMPLE_IVE_KCF_ObjIOU(IVE_ROI_INFO_S astRoiInfo[], HI_U32 u32RoiNum, IVE_KCF_BBOX_S astBbox[],
    HI_U32 u32BboxNum, IVE_ROI_INFO_S astNewRoiInfo[], HI_U32 u32MaxRoiNum, HI_U32 *pu32NewRoiNum)
{
    HI_S32 s32Ret = HI_SUCCESS;
    HI_FLOAT f32NmsThr = 0.3f;
    HI_BOOL bTrackOk;
    HI_U32 u32NewRoiNum = 0;
    HI_U32 i, j;

    if(u32BboxNum==0)
    {
        memcpy_s(astNewRoiInfo, sizeof(IVE_ROI_INFO_S) * u32RoiNum, astRoiInfo, sizeof(IVE_ROI_INFO_S) * u32RoiNum);
        *pu32NewRoiNum = u32RoiNum;
        return s32Ret;
    }

    if(u32RoiNum==0)
    {
        *pu32NewRoiNum = 0;
        return s32Ret;
    }

    for(i=0; i<u32RoiNum; i++)
    {
        for(j=0; j<u32BboxNum; j++)
        {
            HI_BOOL bSuppressed;
            HI_FLOAT f32Iou;

            if(astRoiInfo[i].u32RoiId == 0)
            {
                break;
            }
            if((astBbox[j].stRoiInfo.u32RoiId == 0) || (HI_FALSE == astBbox[j].bTrackOk))
            {
                continue;
            }

            f32Iou = SAMPLE_IVE_KcfCalcIOU(&astRoiInfo[i].stRoi, &astBbox[j].stRoiInfo.stRoi);
            bSuppressed = (HI_BOOL)(f32Iou >= f32NmsThr);

            if(bSuppressed)
            {
                s32Ret = HI_MPI_IVE_KCF_JudgeObjBboxTrackState(&astRoiInfo[i], &astBbox[j], &bTrackOk);
                if(HI_SUCCESS != s32Ret)
                {
                    SAMPLE_PRT("Error(%#x),HI_MPI_IVE_KCF_JudgeObjBboxTrackState failed!\n", s32Ret);
                    return s32Ret;
                }

                // do not update the bbox's u32RoiId
                if(bTrackOk)
                {
                    astBbox[j].stRoiInfo.stRoi.s24q8X = astRoiInfo[i].stRoi.s24q8X;
                    astBbox[j].stRoiInfo.stRoi.s24q8Y = astRoiInfo[i].stRoi.s24q8Y;
                    astBbox[j].stRoiInfo.stRoi.u32Width = astRoiInfo[i].stRoi.u32Width;
                    astBbox[j].stRoiInfo.stRoi.u32Height = astRoiInfo[i].stRoi.u32Height;
                    astBbox[j].bRoiRefresh = HI_TRUE;

                    astRoiInfo[i].u32RoiId = 0;
                }
                else
                {
                    astBbox[j].bTrackOk = HI_FALSE;
                }
                break;
            }
        }

        if(u32NewRoiNum==u32MaxRoiNum)
        {
            SAMPLE_PRT("reaching the maxRoiNum(%d), the rest roi will be cast away!\n", u32MaxRoiNum);
            *pu32NewRoiNum = u32NewRoiNum;
            return HI_SUCCESS;
        }

        // u32RoiId!=0, means it hasn't been suppressed
        if(astRoiInfo[i].u32RoiId != 0)
        {
            memcpy(&astNewRoiInfo[u32NewRoiNum++], &astRoiInfo[i], sizeof(IVE_ROI_INFO_S));
        }
    }

    *pu32NewRoiNum = u32NewRoiNum;
    return s32Ret;
}

static void SAMPLE_IVE_FillImage(VIDEO_FRAME_INFO_S *pstFrameInfo, IVE_IMAGE_S *pstImage)
{
    pstImage->au64PhyAddr[0] = pstFrameInfo->stVFrame.u64PhyAddr[0];
    pstImage->au64PhyAddr[1] = pstFrameInfo->stVFrame.u64PhyAddr[1];
    pstImage->au32Stride[0] = pstFrameInfo->stVFrame.u32Stride[0];
    pstImage->au32Stride[1] = pstFrameInfo->stVFrame.u32Stride[1];
    pstImage->u32Width = pstFrameInfo->stVFrame.u32Width;
    pstImage->u32Height = pstFrameInfo->stVFrame.u32Height;

    if(pstFrameInfo->stVFrame.enPixelFormat == PIXEL_FORMAT_YVU_SEMIPLANAR_420)
    {
        pstImage->enType = IVE_IMAGE_TYPE_YUV420SP;
    }
    else
    {

    }

    return ;
}

HI_VOID SAMPLE_IVE_RectToPoint(IVE_KCF_BBOX_S astBbox[], HI_U32 u32BboxObjNum, SAMPLE_RECT_ARRAY_S *pstRect)
{
    HI_U32 i;

    pstRect->u16Num = 0;
    for(i = 0; i < u32BboxObjNum; i++)
    {
        pstRect->astRect[i].astPoint[0].s32X = astBbox[i].stRoiInfo.stRoi.s24q8X / 256 & (~1);
        pstRect->astRect[i].astPoint[0].s32Y = astBbox[i].stRoiInfo.stRoi.s24q8Y / 256 & (~1);
        if ((pstRect->astRect[i].astPoint[0].s32X < 0) || (pstRect->astRect[i].astPoint[0].s32Y < 0))
        {
            pstRect->astRect[i].astPoint[0].s32X = 0;
            pstRect->astRect[i].astPoint[0].s32Y = 0;
        }
        pstRect->astRect[i].astPoint[1].s32X = pstRect->astRect[i].astPoint[0].s32X + (astBbox[i].stRoiInfo.stRoi.u32Width & (~1));
        pstRect->astRect[i].astPoint[1].s32Y = pstRect->astRect[i].astPoint[0].s32Y;
        pstRect->astRect[i].astPoint[2].s32X = pstRect->astRect[i].astPoint[0].s32X + (astBbox[i].stRoiInfo.stRoi.u32Width & (~1));
        pstRect->astRect[i].astPoint[2].s32Y = pstRect->astRect[i].astPoint[0].s32Y + (astBbox[i].stRoiInfo.stRoi.u32Height & (~1));
        pstRect->astRect[i].astPoint[3].s32X = pstRect->astRect[i].astPoint[0].s32X;
        pstRect->astRect[i].astPoint[3].s32Y = pstRect->astRect[i].astPoint[0].s32Y + (astBbox[i].stRoiInfo.stRoi.u32Height & (~1));

        pstRect->u16Num++;
    }
}

HI_S32 SAMPLE_IVE_DispProcess(VIDEO_FRAME_INFO_S *pstFrameInfo, SAMPLE_RECT_ARRAY_S *pstRect)
{
    HI_S32 s32Ret;
    VO_LAYER voLayer = 0;
	VO_CHN voChn = 0;
    HI_S32 s32MilliSec = 20000;

    s32Ret = SAMPLE_COMM_VGS_FillRect(pstFrameInfo, pstRect, 0x0000FF00);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("Error(%#x),SAMPLE_COMM_VGS_FillRect failed!\n", s32Ret);
    }

    s32Ret = HI_MPI_VO_SendFrame(voLayer,voChn,pstFrameInfo,s32MilliSec);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("Error(%#x),HI_MPI_VO_SendFrame failed!\n", s32Ret);
        return s32Ret;
    }

    return s32Ret;
}

/******************************************************************************
* function : Kcf tracking and vo display thread entry
******************************************************************************/
static HI_VOID* SAMPLE_IVE_KcfTracking(HI_VOID* pArgs)
{
    IVE_KCF_BBOX_S astBbox[64] = {0};
    IVE_ROI_INFO_S astRoiInfo[64] = {0};
    IVE_ROI_INFO_S astNewRoiInfo[64] = {0};
    SAMPLE_RECT_ARRAY_S stRect = {0};
    HI_U32 u32BboxNum = 0;
    HI_U32 u32RoiNum = 0;
    HI_U32 u32MaxNewRoiNum = 64;
    HI_U32 u32NewRoiNum = 0;
    IVE_HANDLE hIveHandle;
    HI_BOOL bInstant = HI_TRUE;
    IVE_IMAGE_S stSrc;
    SAMPLE_IVE_NODE_S *pstQueueNode;
    HI_S32 s32QueueLen;
    HI_S32 s32Ret;
    HI_BOOL bFinish;
    HI_BOOL bBlock = HI_TRUE;

    while (HI_FALSE == s_bIveKcfStopSignal)
    {
        SAMPLE_IVE_MUTEX_LOCK(s_stIveKcfInfo.QueueMutex);
        s32QueueLen = SAMPLE_IVE_QueueSize(s_stIveKcfInfo.pstQueueHead);
        if (s32QueueLen <= 0)
        {
            SAMPLE_IVE_MUTEX_UNLOCK(s_stIveKcfInfo.QueueMutex);
            continue;
        }
        pstQueueNode = SAMPLE_IVE_QueueGetNode(s_stIveKcfInfo.pstQueueHead);
        if (NULL == pstQueueNode)
        {
            SAMPLE_IVE_MUTEX_UNLOCK(s_stIveKcfInfo.QueueMutex);
            continue;
        }
        SAMPLE_IVE_MUTEX_UNLOCK(s_stIveKcfInfo.QueueMutex);
        SAMPLE_IVE_FillImage(&pstQueueNode->stFrameInfo, &stSrc);

        if (s_stIveKcfInfo.bNewDetect)
        {
            SAMPLE_IVE_MUTEX_LOCK(s_stIveKcfInfo.CnnDetectMutex);
            s_stIveKcfInfo.bNewDetect = HI_FALSE;
            u32RoiNum = s_stIveKcfInfo.u32RoiNum;
            s_stIveKcfInfo.u32RoiNum = 0;
            memcpy_s(astRoiInfo, sizeof(IVE_ROI_INFO_S) * u32RoiNum, s_stIveKcfInfo.astRoiInfo, sizeof(IVE_ROI_INFO_S) * u32RoiNum);
            SAMPLE_IVE_MUTEX_UNLOCK(s_stIveKcfInfo.CnnDetectMutex);

            s32Ret = SAMPLE_IVE_KCF_ObjIOU(astRoiInfo, u32RoiNum, astBbox, u32BboxNum,
                astNewRoiInfo, u32MaxNewRoiNum, &u32NewRoiNum);
            if (HI_SUCCESS != s32Ret)
            {
                SAMPLE_PRT("Error(%#x),SAMPLE_IVE_KCF_ObjIOU failed!\n", s32Ret);
                goto FAIL;
            }

            s32Ret = HI_MPI_IVE_KCF_ObjUpdate(&s_stIveKcfInfo.stObjList, astBbox, u32BboxNum);
            if (HI_SUCCESS != s32Ret)
            {
                SAMPLE_PRT("Error(%#x),HI_MPI_IVE_KCF_ObjUpdate failed!\n", s32Ret);
                goto FAIL;
            }

            s32Ret = HI_MPI_IVE_KCF_GetTrainObj(s_stIveKcfInfo.u3q5Padding, astNewRoiInfo, u32NewRoiNum,
                &s_stIveKcfInfo.stCosWinX, &s_stIveKcfInfo.stCosWinY, &s_stIveKcfInfo.stGaussPeak,
                &s_stIveKcfInfo.stObjList);
            if (HI_SUCCESS != s32Ret)
            {
                SAMPLE_PRT("Error(%#x),HI_MPI_IVE_KCF_GetTrainObj failed!\n", s32Ret);
                goto FAIL;
            }
        }

        if (s_stIveKcfInfo.stObjList.u32TrackObjNum != 0 || s_stIveKcfInfo.stObjList.u32TrainObjNum != 0)
        {
            s32Ret = HI_MPI_IVE_KCF_Process(&hIveHandle, &stSrc, &s_stIveKcfInfo.stObjList,
                &s_stIveKcfInfo.stKcfProCtrl, bInstant);
            if (HI_SUCCESS != s32Ret)
            {
                SAMPLE_PRT("Error(%#x),HI_MPI_IVE_KCF_Process failed!\n", s32Ret);
                goto FAIL;
            }

            s32Ret = HI_MPI_IVE_Query(hIveHandle, &bFinish, bBlock);
            while (HI_ERR_IVE_QUERY_TIMEOUT == s32Ret)
            {
                usleep(100);
                s32Ret = HI_MPI_IVE_Query(hIveHandle, &bFinish, bBlock);
            }

            SAMPLE_CHECK_EXPR_RET(HI_SUCCESS != s32Ret,HI_FALSE,"Error(%#x),HI_MPI_IVE_Query failed!\n",s32Ret);

            s32Ret = HI_MPI_IVE_KCF_GetObjBbox(&s_stIveKcfInfo.stObjList, astBbox,
                &u32BboxNum, &s_stIveKcfInfo.stKcfBboxCtrl);
            if (HI_SUCCESS != s32Ret)
            {
                SAMPLE_PRT("Error(%#x),HI_MPI_IVE_KCF_GetObjBbox failed!\n", s32Ret);
            }
            SAMPLE_IVE_RectToPoint(astBbox, u32BboxNum, &stRect);
        }
        else
        {
            stRect.u16Num = 0;
        }

        s32Ret = SAMPLE_IVE_DispProcess(&pstQueueNode->stFrameInfo, &stRect);
        if (HI_SUCCESS != s32Ret)
        {
            SAMPLE_PRT("Error(%#x),SAMPLE_IVE_DispProcess failed!\n", s32Ret);
        }

        FAIL:
            (HI_VOID)HI_MPI_VPSS_ReleaseChnFrame(0,0, &pstQueueNode->stFrameInfo);
            SAMPLE_IVE_QueueFreeNode(pstQueueNode);
    }

    return NULL;
}

static HI_VOID SAMPLE_IVE_KcfDeInit(SAMPLE_IVE_KCF_S *pstIveKcfInfo)
{
    SAMPLE_IVE_NODE_S *pstQueueNode;

    if (CNN_PROC_START == pstIveKcfInfo->enCnnProcStat)
    {
        (HI_VOID)HI_MPI_VPSS_ReleaseChnFrame(0,1, &pstIveKcfInfo->astFrameInfo[1]);
        SAMPLE_IVE_MUTEX_LOCK(pstIveKcfInfo->GetFrmMutex);
        pstIveKcfInfo->enCnnGetFrmStat = CNN_GET_FRM_END;
        pstIveKcfInfo->enCnnProcStat = CNN_PROC_END;
        SAMPLE_IVE_MUTEX_UNLOCK(pstIveKcfInfo->GetFrmMutex);
    }

    SAMPLE_IVE_MUTEX_DESTROY(pstIveKcfInfo->QueueMutex);
    SAMPLE_IVE_MUTEX_DESTROY(pstIveKcfInfo->GetFrmMutex);
    SAMPLE_IVE_MUTEX_DESTROY(pstIveKcfInfo->CnnDetectMutex);
    (HI_VOID)HI_MPI_IVE_KCF_DestroyObjList(&pstIveKcfInfo->stObjList);

   while (!SAMPLE_IVE_QueueIsEmpty(pstIveKcfInfo->pstQueueHead))
    {
        pstQueueNode = SAMPLE_IVE_QueueGetNode(pstIveKcfInfo->pstQueueHead);
        if (pstQueueNode)
        {
            (HI_VOID)HI_MPI_VPSS_ReleaseChnFrame(0,0, &pstQueueNode->stFrameInfo);
            SAMPLE_IVE_QueueFreeNode(pstQueueNode);
        }
    }
    SAMPLE_IVE_QueueDestory(pstIveKcfInfo->pstQueueHead);
    IVE_MMZ_FREE(pstIveKcfInfo->stTotalMem.u64PhyAddr, pstIveKcfInfo->stTotalMem.u64VirAddr);

    return ;
}

static HI_S32 SAMPLE_IVE_KcfInit(SAMPLE_IVE_KCF_S *pstIveKcfInfo)
{
    HI_S32 s32Ret = HI_SUCCESS;
    HI_U32 u32Size;
    HI_U32 u32TotalSize;
    HI_S32 s32Len;

    memset_s(pstIveKcfInfo, sizeof(SAMPLE_IVE_KCF_S), 0, sizeof(SAMPLE_IVE_KCF_S));

    s32Ret = HI_MPI_IVE_KCF_GetMemSize(SAMPLE_IVE_KCF_NODE_MAX_NUM, &u32Size);
    SAMPLE_CHECK_EXPR_GOTO(HI_SUCCESS != s32Ret, FAIL_0,
        "Error(%#x),HI_MPI_IVE_KCF_GetMemSize failed!\n", s32Ret);
    /* (HOGFeatrue + Alpha + DstBuf) + Guasspeak + CosWinX + CosWinY + TmpBuf*/
    u32TotalSize = u32Size + SAMPLE_IVE_KCF_GAUSS_PEAK_TOTAL_SIZE + SAMPLE_IVE_KCF_COS_WINDOW_TOTAL_SIZE * 2
        + SAMPLE_IVE_KCF_TEMP_BUF_SIZE;
    s32Ret = SAMPLE_COMM_IVE_CreateMemInfo(&pstIveKcfInfo->stTotalMem, u32TotalSize);
    SAMPLE_CHECK_EXPR_GOTO(HI_SUCCESS != s32Ret, FAIL_0,
        "Error(%#x),SAMPLE_COMM_IVE_CreateMemInfo failed!\n", s32Ret);
    memset((HI_U8*)(HI_UL)pstIveKcfInfo->stTotalMem.u64VirAddr, 0x0, u32TotalSize);

    pstIveKcfInfo->stListMem.u64PhyAddr = pstIveKcfInfo->stTotalMem.u64PhyAddr;
    pstIveKcfInfo->stListMem.u64VirAddr = pstIveKcfInfo->stTotalMem.u64VirAddr;
    pstIveKcfInfo->stListMem.u32Size = u32Size;

    pstIveKcfInfo->stGaussPeak.u64PhyAddr = pstIveKcfInfo->stListMem.u64PhyAddr + u32Size;
    pstIveKcfInfo->stGaussPeak.u64VirAddr = pstIveKcfInfo->stListMem.u64VirAddr + u32Size;
    pstIveKcfInfo->stGaussPeak.u32Size = SAMPLE_IVE_KCF_GAUSS_PEAK_TOTAL_SIZE;

    pstIveKcfInfo->stCosWinX.u64PhyAddr = pstIveKcfInfo->stGaussPeak.u64PhyAddr + SAMPLE_IVE_KCF_GAUSS_PEAK_TOTAL_SIZE;
    pstIveKcfInfo->stCosWinX.u64VirAddr = pstIveKcfInfo->stGaussPeak.u64VirAddr + SAMPLE_IVE_KCF_GAUSS_PEAK_TOTAL_SIZE;
    pstIveKcfInfo->stCosWinX.u32Size = SAMPLE_IVE_KCF_COS_WINDOW_TOTAL_SIZE;

    pstIveKcfInfo->stCosWinY.u64PhyAddr = pstIveKcfInfo->stCosWinX.u64PhyAddr + SAMPLE_IVE_KCF_COS_WINDOW_TOTAL_SIZE;
    pstIveKcfInfo->stCosWinY.u64VirAddr = pstIveKcfInfo->stCosWinX.u64VirAddr + SAMPLE_IVE_KCF_COS_WINDOW_TOTAL_SIZE;
    pstIveKcfInfo->stCosWinY.u32Size = SAMPLE_IVE_KCF_COS_WINDOW_TOTAL_SIZE;

    pstIveKcfInfo->stKcfProCtrl.stTmpBuf.u64PhyAddr = pstIveKcfInfo->stCosWinY.u64PhyAddr + SAMPLE_IVE_KCF_COS_WINDOW_TOTAL_SIZE;
    pstIveKcfInfo->stKcfProCtrl.stTmpBuf.u64VirAddr = pstIveKcfInfo->stCosWinY.u64VirAddr + SAMPLE_IVE_KCF_COS_WINDOW_TOTAL_SIZE;
    pstIveKcfInfo->stKcfProCtrl.stTmpBuf.u32Size = SAMPLE_IVE_KCF_TEMP_BUF_SIZE;

    pstIveKcfInfo->stKcfProCtrl.enCscMode = IVE_CSC_MODE_VIDEO_BT709_YUV2RGB;
    pstIveKcfInfo->stKcfProCtrl.u1q15InterFactor = 0.02 * 1024 * 32;
    pstIveKcfInfo->stKcfProCtrl.u0q16Lamda = 10;
    pstIveKcfInfo->stKcfProCtrl.u0q8Sigma = 0.5 * 256;
    pstIveKcfInfo->stKcfProCtrl.u4q12TrancAlfa = 0.2 * 4096;
    pstIveKcfInfo->stKcfProCtrl.u8RespThr = 32;

    pstIveKcfInfo->u3q5Padding = 1.5 * 32;

    pstIveKcfInfo->stKcfBboxCtrl.u32MaxBboxNum = SAMPLE_IVE_KCF_NODE_MAX_NUM;
    pstIveKcfInfo->stKcfBboxCtrl.s32RespThr = 0;

    s32Len = SAMPLE_IVE_QUEUE_LEN;
    pstIveKcfInfo->pstQueueHead = SAMPLE_IVE_QueueCreate(s32Len);
    SAMPLE_CHECK_EXPR_GOTO(NULL == pstIveKcfInfo->pstQueueHead, FAIL_1,
        "Error,SAMPLE_IVE_QueueCreate failed!\n");

    s32Ret = HI_MPI_IVE_KCF_CreateObjList(&pstIveKcfInfo->stListMem, SAMPLE_IVE_KCF_NODE_MAX_NUM, &pstIveKcfInfo->stObjList);
    SAMPLE_CHECK_EXPR_GOTO(HI_SUCCESS != s32Ret, FAIL_2,
        "Error(%#x),HI_MPI_IVE_KCF_CreateObjList failed!\n", s32Ret);

    s32Ret = HI_MPI_IVE_KCF_CreateGaussPeak(pstIveKcfInfo->u3q5Padding, &pstIveKcfInfo->stGaussPeak);
    SAMPLE_CHECK_EXPR_GOTO(HI_SUCCESS != s32Ret, FAIL_3,
        "Error(%#x),HI_MPI_IVE_KCF_CreateGaussPeak failed!\n", s32Ret);

    s32Ret = HI_MPI_IVE_KCF_CreateCosWin(&pstIveKcfInfo->stCosWinX, &pstIveKcfInfo->stCosWinY);
    SAMPLE_CHECK_EXPR_GOTO(HI_SUCCESS != s32Ret, FAIL_3,
        "Error(%#x),HI_MPI_IVE_KCF_CreateCosWin failed!\n", s32Ret);

    pstIveKcfInfo->enCnnGetFrmStat = CNN_GET_FRM_END;
    pstIveKcfInfo->enCnnProcStat = CNN_PROC_END;
    pstIveKcfInfo->bFirstDetect = HI_FALSE;
    SAMPLE_IVE_MUTEX_INIT_LOCK(pstIveKcfInfo->CnnDetectMutex);
    SAMPLE_IVE_MUTEX_INIT_LOCK(pstIveKcfInfo->GetFrmMutex);
    SAMPLE_IVE_MUTEX_INIT_LOCK(pstIveKcfInfo->QueueMutex);

    return s32Ret;

FAIL_3:
    (HI_VOID)HI_MPI_IVE_KCF_DestroyObjList(&pstIveKcfInfo->stObjList);
FAIL_2:
    SAMPLE_IVE_QueueDestory(pstIveKcfInfo->pstQueueHead);
FAIL_1:
    IVE_MMZ_FREE(pstIveKcfInfo->stTotalMem.u64PhyAddr, pstIveKcfInfo->stTotalMem.u64VirAddr);
FAIL_0:
    return s32Ret;
}

/******************************************************************************
* function : Rfcn + Kcf  detecting and tracking car.
******************************************************************************/
void SAMPLE_IVE_Kcf(void)
{
    HI_CHAR *pcModelName = "./data/input/kcf/inst_rfcn_resnet50_cycle_352x288.wk";
    SAMPLE_SVP_NNIE_CFG_S   stNnieCfg = {0};
    HI_S32 s32Ret = HI_SUCCESS;
    HI_CHAR acThreadName[16] = {0};
    HI_S32             s32ViCnt       = 1;
    VI_DEV             ViDev          = 0;
    VI_PIPE            ViPipe         = 0;
    VI_CHN             ViChn          = 0;
    HI_S32             s32WorkSnsId   = 0;

    SIZE_S             astSize[3];
    VB_CONFIG_S        stVbConf;
    PIC_SIZE_E         enPicSize;
    HI_U32             u32BlkSize;

    WDR_MODE_E         enWDRMode      = WDR_MODE_NONE;
    DYNAMIC_RANGE_E    enDynamicRange = DYNAMIC_RANGE_SDR8;
    PIXEL_FORMAT_E     enPixFormat    = PIXEL_FORMAT_YVU_SEMIPLANAR_420;
    VIDEO_FORMAT_E     enVideoFormat  = VIDEO_FORMAT_LINEAR;
    COMPRESS_MODE_E    enCompressMode = COMPRESS_MODE_NONE;
    VI_VPSS_MODE_E     enMastPipeMode = VI_ONLINE_VPSS_OFFLINE;

    VPSS_GRP           VpssGrp        = 0;
    VPSS_GRP_ATTR_S    stVpssGrpAttr;
    HI_BOOL            abChnEnable[VPSS_MAX_PHY_CHN_NUM] = {0};
    VPSS_CHN_ATTR_S    astVpssChnAttr[VPSS_MAX_PHY_CHN_NUM];
    HI_U32 i;

    /******************************************
     step 1: init vi vpss vo
     ******************************************/
    /*config vi*/
    SAMPLE_COMM_VI_GetSensorInfo(&s_stViConfig);

    s_stViConfig.s32WorkingViNum                                   = s32ViCnt;
    s_stViConfig.as32WorkingViId[0]                                = 0;
    s_stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.MipiDev         = ViDev;
    s_stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.s32BusId        = 0;
    s_stViConfig.astViInfo[s32WorkSnsId].stDevInfo.ViDev           = ViDev;
    s_stViConfig.astViInfo[s32WorkSnsId].stDevInfo.enWDRMode       = enWDRMode;
    s_stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.enMastPipeMode = enMastPipeMode;
    s_stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.aPipe[0]       = ViPipe;
    s_stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.aPipe[1]       = -1;
    s_stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.aPipe[2]       = -1;
    s_stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.aPipe[3]       = -1;
    s_stViConfig.astViInfo[s32WorkSnsId].stChnInfo.ViChn           = ViChn;
    s_stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enPixFormat     = enPixFormat;
    s_stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enDynamicRange  = enDynamicRange;
    s_stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enVideoFormat   = enVideoFormat;
    s_stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enCompressMode  = enCompressMode;

    /*get picture size*/
    s32Ret = SAMPLE_COMM_VI_GetSizeBySensor(s_stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.enSnsType, &enPicSize);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("get picture size by sensor failed!\n");
        return ;
    }

    s32Ret = SAMPLE_COMM_SYS_GetPicSize(enPicSize, &astSize[0]);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("get picture size failed!\n");
        return ;
    }

    enPicSize = PIC_1080P;
    s32Ret = SAMPLE_COMM_SYS_GetPicSize(enPicSize, &astSize[1]);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("get picture size failed!\n");
        return ;
    }

    enPicSize = PIC_CIF;
    s32Ret = SAMPLE_COMM_SYS_GetPicSize(enPicSize, &astSize[2]);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("get picture size failed!\n");
        return ;
    }

    /*config vb*/
    memset_s(&stVbConf, sizeof(VB_CONFIG_S), 0, sizeof(VB_CONFIG_S));
    stVbConf.u32MaxPoolCnt              = 4;

    u32BlkSize = COMMON_GetPicBufferSize(astSize[0].u32Width, astSize[0].u32Height, SAMPLE_PIXEL_FORMAT, DATA_BITWIDTH_8, COMPRESS_MODE_NONE, DEFAULT_ALIGN);
    stVbConf.astCommPool[0].u64BlkSize  = u32BlkSize;
    stVbConf.astCommPool[0].u32BlkCnt   = 10;

    u32BlkSize = VI_GetRawBufferSize(astSize[0].u32Width, astSize[0].u32Height, PIXEL_FORMAT_RGB_BAYER_16BPP, COMPRESS_MODE_NONE, DEFAULT_ALIGN);
    stVbConf.astCommPool[1].u64BlkSize  = u32BlkSize;
    stVbConf.astCommPool[1].u32BlkCnt   = 4;

    u32BlkSize = COMMON_GetPicBufferSize(astSize[1].u32Width, astSize[1].u32Height, SAMPLE_PIXEL_FORMAT, DATA_BITWIDTH_8, COMPRESS_MODE_NONE, DEFAULT_ALIGN);
    stVbConf.astCommPool[2].u64BlkSize  = u32BlkSize;
    stVbConf.astCommPool[2].u32BlkCnt   = 10;

    u32BlkSize = COMMON_GetPicBufferSize(astSize[2].u32Width, astSize[2].u32Height, SAMPLE_PIXEL_FORMAT, DATA_BITWIDTH_8, COMPRESS_MODE_NONE, DEFAULT_ALIGN);
    stVbConf.astCommPool[3].u64BlkSize  = u32BlkSize;
    stVbConf.astCommPool[3].u32BlkCnt   = 10;

    s32Ret = SAMPLE_COMM_SYS_Init(&stVbConf);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("system init failed with %d!\n", s32Ret);
        return ;
    }

    /*start vi*/
    s32Ret = SAMPLE_COMM_VI_StartVi(&s_stViConfig);
    SAMPLE_CHECK_EXPR_GOTO(HI_SUCCESS != s32Ret, END_0,
        "Error(%#x),SAMPLE_COMM_VI_StartVi failed!\n", s32Ret);

    /*config vpss*/
    memset_s(&stVpssGrpAttr, sizeof(VPSS_GRP_ATTR_S), 0, sizeof(VPSS_GRP_ATTR_S));
    stVpssGrpAttr.stFrameRate.s32SrcFrameRate    = -1;
    stVpssGrpAttr.stFrameRate.s32DstFrameRate    = -1;
    stVpssGrpAttr.enDynamicRange                 = DYNAMIC_RANGE_SDR8;
    stVpssGrpAttr.enPixelFormat                  = enPixFormat;
    stVpssGrpAttr.u32MaxW                        = astSize[0].u32Width;
    stVpssGrpAttr.u32MaxH                        = astSize[0].u32Height;
    stVpssGrpAttr.bNrEn                          = HI_FALSE;
    stVpssGrpAttr.stNrAttr.enCompressMode        = COMPRESS_MODE_NONE;
    stVpssGrpAttr.stNrAttr.enNrMotionMode        = NR_MOTION_MODE_NORMAL;

    for (i = 0; i < 2; i++)
    {
        astVpssChnAttr[i].u32Width                    = astSize[i+1].u32Width;
        astVpssChnAttr[i].u32Height                   = astSize[i+1].u32Height;
        astVpssChnAttr[i].enChnMode                   = VPSS_CHN_MODE_USER;
        astVpssChnAttr[i].enCompressMode              = enCompressMode;
        astVpssChnAttr[i].enDynamicRange              = enDynamicRange;
        astVpssChnAttr[i].enVideoFormat               = enVideoFormat;
        astVpssChnAttr[i].enPixelFormat               = enPixFormat;
        astVpssChnAttr[i].stFrameRate.s32SrcFrameRate = 30;
        astVpssChnAttr[i].stFrameRate.s32DstFrameRate = 30;
        astVpssChnAttr[i].u32Depth                    = 1;
        astVpssChnAttr[i].bMirror                     = HI_FALSE;
        astVpssChnAttr[i].bFlip                       = HI_FALSE;
        astVpssChnAttr[i].stAspectRatio.enMode        = ASPECT_RATIO_NONE;

        abChnEnable[i] = HI_TRUE;
    }
    /*start vpss*/
    s32Ret = SAMPLE_COMM_VPSS_Start(VpssGrp, abChnEnable, &stVpssGrpAttr, astVpssChnAttr);
    SAMPLE_CHECK_EXPR_GOTO(HI_SUCCESS != s32Ret, END_1,
        "Error(%#x),SAMPLE_COMM_VPSS_Start failed!\n", s32Ret);

    /*vi bind vpss*/
    s32Ret = SAMPLE_COMM_VI_Bind_VPSS(ViPipe, ViChn, VpssGrp);
    SAMPLE_CHECK_EXPR_GOTO(HI_SUCCESS != s32Ret, END_2,
        "Error(%#x),SAMPLE_COMM_VI_Bind_VPSS failed!\n", s32Ret);

    /*config vo*/
    SAMPLE_COMM_VO_GetDefConfig(&s_stVoConfig);
    s_stVoConfig.enDstDynamicRange = enDynamicRange;
    s_stVoConfig.enVoIntfType = VO_INTF_HDMI;
    s_stVoConfig.enPicSize = PIC_1080P;

    /*start vo*/
    s32Ret = SAMPLE_COMM_VO_StartVO(&s_stVoConfig);
    SAMPLE_CHECK_EXPR_GOTO(HI_SUCCESS != s32Ret, END_3,
        "Error(%#x),SAMPLE_COMM_VO_StartVO failed!\n", s32Ret);

    memset_s(&s_stRfcnModel,sizeof(s_stRfcnModel), 0,sizeof(s_stRfcnModel));
    memset_s(&s_stRfcnNnieParam,sizeof(s_stRfcnNnieParam), 0,sizeof(s_stRfcnNnieParam));
    memset_s(&s_stRfcnSoftwareParam,sizeof(s_stRfcnSoftwareParam), 0,sizeof(s_stRfcnSoftwareParam));

    /******************************************
     step 2: init Kcf param
     ******************************************/
    s32Ret = SAMPLE_IVE_KcfInit(&s_stIveKcfInfo);
    SAMPLE_CHECK_EXPR_GOTO(HI_SUCCESS != s32Ret, END_4,
        "Error(%#x),SAMPLE_IVE_KcfInit failed!\n", s32Ret);
    /******************************************
     step 3: init NNIE param
     ******************************************/
    stNnieCfg.pszPic= NULL;
    stNnieCfg.u32MaxInputNum = 1; //max input image num in each batch
    stNnieCfg.u32MaxRoiNum = 300;
    stNnieCfg.aenNnieCoreId[0] = SVP_NNIE_ID_0; //set NNIE core for 0-th Seg
    stNnieCfg.aenNnieCoreId[1] = SVP_NNIE_ID_0; //set NNIE core for 1-th Seg
    stNnieCfg.aenNnieCoreId[2] = SVP_NNIE_ID_0; //set NNIE core for 2-th Seg

    s32Ret = SAMPLE_COMM_SVP_NNIE_LoadModel(pcModelName,&s_stRfcnModel);
    SAMPLE_SVP_CHECK_EXPR_GOTO(HI_SUCCESS != s32Ret,END_5,SAMPLE_SVP_ERR_LEVEL_ERROR,
        "Error,SAMPLE_COMM_SVP_NNIE_LoadModel failed!\n");

    /*apcRpnDataLayerName is used to set RPN data layer name
      and search RPN input data,if user has changed network struct, please
      make sure the data layer names are correct*/
    s_stRfcnNnieParam.pstModel = &s_stRfcnModel.stModel;
    s_stRfcnSoftwareParam.apcRpnDataLayerName[0] = "rpn_cls_score";
    s_stRfcnSoftwareParam.apcRpnDataLayerName[1] = "rpn_bbox_pred";
    s32Ret = SAMPLE_IVE_Rfcn_ParamInit(&stNnieCfg,&s_stRfcnNnieParam,
           &s_stRfcnSoftwareParam);
    SAMPLE_SVP_CHECK_EXPR_GOTO(HI_SUCCESS != s32Ret,END_6,SAMPLE_SVP_ERR_LEVEL_ERROR,
       "Error,SAMPLE_SVP_NNIE_Rfcn_ParamInit failed!\n");

    s_bIveKcfStopSignal = HI_FALSE;

    /******************************************
      step 4: Create work thread
     ******************************************/
    snprintf(acThreadName, 16, "GetFrame");
    prctl(PR_SET_NAME, (unsigned long)acThreadName, 0,0,0);
    pthread_create(&s_IveGetFrmThread, 0, SAMPLE_IVE_GetFrame, NULL);

    snprintf(acThreadName, 16, "KcfTracking");
    prctl(PR_SET_NAME, (unsigned long)acThreadName, 0,0,0);
    pthread_create(&s_IveTrackThread, 0, SAMPLE_IVE_KcfTracking, NULL);

    snprintf(acThreadName, 16, "NNIEDetect");
    prctl(PR_SET_NAME, (unsigned long)acThreadName, 0,0,0);
    pthread_create(&s_IveDetectThread, 0, SAMPLE_IVE_Rfcn_Detect, NULL);

    SAMPLE_PAUSE();

    s_bIveKcfStopSignal = HI_TRUE;

    pthread_join(s_IveGetFrmThread, NULL);
    s_IveGetFrmThread = 0;

    pthread_join(s_IveTrackThread, NULL);
    s_IveTrackThread = 0;

    pthread_join(s_IveDetectThread, NULL);
    s_IveDetectThread = 0;

END_6:
    SAMPLE_IVE_RfcnDeinit(&s_stRfcnNnieParam,&s_stRfcnSoftwareParam,&s_stRfcnModel);
END_5:
    SAMPLE_IVE_KcfDeInit(&s_stIveKcfInfo);
END_4:
    SAMPLE_COMM_VO_StopVO(&s_stVoConfig);
END_3:
    SAMPLE_COMM_VI_UnBind_VPSS(ViPipe, ViChn, VpssGrp);
END_2:
    SAMPLE_COMM_VPSS_Stop(VpssGrp, abChnEnable);
END_1:
    SAMPLE_COMM_VI_StopVi(&s_stViConfig);
END_0:
    SAMPLE_COMM_SYS_Exit();

    return ;
}


/******************************************************************************
* function : Kcf sample signal handle
******************************************************************************/
void SAMPLE_IVE_Kcf_HandleSig(void)
{
    VI_PIPE            ViPipe         = 0;
    VI_CHN             ViChn          = 0;
    VPSS_GRP           VpssGrp        = 0;
    HI_BOOL            abChnEnable[VPSS_MAX_PHY_CHN_NUM] = {1, 1, 0};

    s_bIveKcfStopSignal = HI_TRUE;
    if (0 != s_IveGetFrmThread)
    {
        pthread_join(s_IveGetFrmThread, NULL);
        s_IveGetFrmThread = 0;
    }
    if (0 != s_IveTrackThread)
    {
        pthread_join(s_IveTrackThread, NULL);
        s_IveTrackThread = 0;
    }

    if (0 != s_IveDetectThread)
    {
        pthread_join(s_IveDetectThread, NULL);
        s_IveDetectThread = 0;
    }

    SAMPLE_IVE_RfcnDeinit(&s_stRfcnNnieParam,&s_stRfcnSoftwareParam,&s_stRfcnModel);
    memset_s(&s_stRfcnNnieParam,sizeof(SAMPLE_SVP_NNIE_PARAM_S), 0,sizeof(SAMPLE_SVP_NNIE_PARAM_S));
    memset_s(&s_stRfcnSoftwareParam, sizeof(SAMPLE_SVP_NNIE_RFCN_SOFTWARE_PARAM_S), 0,sizeof(SAMPLE_SVP_NNIE_RFCN_SOFTWARE_PARAM_S));
    memset_s(&s_stRfcnModel, sizeof(SAMPLE_SVP_NNIE_MODEL_S), 0,sizeof(SAMPLE_SVP_NNIE_MODEL_S));
    SAMPLE_IVE_KcfDeInit(&s_stIveKcfInfo);

    SAMPLE_COMM_VO_StopVO(&s_stVoConfig);
    SAMPLE_COMM_VI_UnBind_VPSS(ViPipe, ViChn, VpssGrp);
    SAMPLE_COMM_VPSS_Stop(VpssGrp, abChnEnable);
    SAMPLE_COMM_VI_StopVi(&s_stViConfig);
    SAMPLE_COMM_SYS_Exit();
}

