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
#include "sample_nnie_main.h"
#include "sample_svp_nnie_software.h"
#include "sample_comm_ive.h"

/*cnn para*/
static SAMPLE_SVP_NNIE_MODEL_S s_stCnnModel = {0};
static SAMPLE_SVP_NNIE_PARAM_S s_stCnnNnieParam = {0};
static SAMPLE_SVP_NNIE_CNN_SOFTWARE_PARAM_S s_stCnnSoftwareParam = {0};
/*segment para*/
static SAMPLE_SVP_NNIE_MODEL_S s_stSegnetModel = {0};
static SAMPLE_SVP_NNIE_PARAM_S s_stSegnetNnieParam = {0};
/*fasterrcnn para*/
static SAMPLE_SVP_NNIE_MODEL_S s_stFasterRcnnModel = {0};
static SAMPLE_SVP_NNIE_PARAM_S s_stFasterRcnnNnieParam = {0};
static SAMPLE_SVP_NNIE_FASTERRCNN_SOFTWARE_PARAM_S s_stFasterRcnnSoftwareParam = {0};
static SAMPLE_SVP_NNIE_NET_TYPE_E s_enNetType;
/*rfcn para*/
static SAMPLE_SVP_NNIE_MODEL_S s_stRfcnModel = {0};
static SAMPLE_SVP_NNIE_PARAM_S s_stRfcnNnieParam = {0};
static SAMPLE_SVP_NNIE_RFCN_SOFTWARE_PARAM_S s_stRfcnSoftwareParam = {0};
static SAMPLE_IVE_SWITCH_S s_stRfcnSwitch = {HI_FALSE,HI_FALSE};
static HI_BOOL s_bNnieStopSignal = HI_FALSE;
static pthread_t s_hNnieThread = 0;
static SAMPLE_VI_CONFIG_S s_stViConfig = {0};

/*ssd para*/
static SAMPLE_SVP_NNIE_MODEL_S s_stSsdModel = {0};
static SAMPLE_SVP_NNIE_PARAM_S s_stSsdNnieParam = {0};
static SAMPLE_SVP_NNIE_SSD_SOFTWARE_PARAM_S s_stSsdSoftwareParam = {0};
/*yolov1 para*/
static SAMPLE_SVP_NNIE_MODEL_S s_stYolov1Model = {0};
static SAMPLE_SVP_NNIE_PARAM_S s_stYolov1NnieParam = {0};
static SAMPLE_SVP_NNIE_YOLOV1_SOFTWARE_PARAM_S s_stYolov1SoftwareParam = {0};
/*yolov2 para*/
static SAMPLE_SVP_NNIE_MODEL_S s_stYolov2Model = {0};
static SAMPLE_SVP_NNIE_PARAM_S s_stYolov2NnieParam = {0};
static SAMPLE_SVP_NNIE_YOLOV2_SOFTWARE_PARAM_S s_stYolov2SoftwareParam = {0};
/*lstm para*/
static SAMPLE_SVP_NNIE_MODEL_S s_stLstmModel = {0};
static SAMPLE_SVP_NNIE_PARAM_S s_stLstmNnieParam = {0};
/*pvanet para*/
static SAMPLE_SVP_NNIE_MODEL_S s_stPvanetModel = {0};
static SAMPLE_SVP_NNIE_PARAM_S s_stPvanetNnieParam = {0};
static SAMPLE_SVP_NNIE_FASTERRCNN_SOFTWARE_PARAM_S s_stPvanetSoftwareParam = {0};

#ifdef SAMPLE_SVP_NNIE_PERF_STAT
#define SAMPLE_SVP_NNIE_PERF_STAT_OP_FORWARD_CLREAR()  memset(&s_stOpForwardPerfTmp,0,sizeof(s_stOpForwardPerfTmp));
#define SAMPLE_SVP_NNIE_PERF_STAT_OP_FORWARD_SRC_FLUSH_TIME() SAMPLE_SVP_NNIE_PERF_STAT_GET_DIFF_TIME(s_stOpForwardPerfTmp.u64SrcFlushTime)
#define SAMPLE_SVP_NNIE_PERF_STAT_OP_FORWARD_PRE_DST_FLUSH_TIME() SAMPLE_SVP_NNIE_PERF_STAT_GET_DIFF_TIME(s_stOpForwardPerfTmp.u64PreDstFulshTime)
#define SAMPLE_SVP_NNIE_PERF_STAT_OP_FORWARD_AFTER_DST_FLUSH_TIME() SAMPLE_SVP_NNIE_PERF_STAT_GET_DIFF_TIME(s_stOpForwardPerfTmp.u64AferDstFulshTime)
#define SAMPLE_SVP_NNIE_PERF_STAT_OP_FORWARD_OP_TIME() SAMPLE_SVP_NNIE_PERF_STAT_GET_DIFF_TIME(s_stOpForwardPerfTmp.u64OPTime)

/*YoloV1*/
#define SAMPLE_SVP_NNIE_PERF_STAT_YOLOV1_FORWARD_SRC_FLUSH_TIME() s_stYolov1Perf.stForwardPerf.u64SrcFlushTime += s_stOpForwardPerfTmp.u64SrcFlushTime;
#define SAMPLE_SVP_NNIE_PERF_STAT_YOLOV1_FORWARD_PRE_DST_FLUSH_TIME() s_stYolov1Perf.stForwardPerf.u64PreDstFulshTime += s_stOpForwardPerfTmp.u64PreDstFulshTime;
#define SAMPLE_SVP_NNIE_PERF_STAT_YOLOV1_FORWARD_AFTER_DST_FLUSH_TIME() s_stYolov1Perf.stForwardPerf.u64AferDstFulshTime += s_stOpForwardPerfTmp.u64AferDstFulshTime;
#define SAMPLE_SVP_NNIE_PERF_STAT_YOLOV1_FORWARD_OP_TIME() s_stYolov1Perf.stForwardPerf.u64OPTime += s_stOpForwardPerfTmp.u64OPTime;
#define SAMPLE_SVP_NNIE_PERF_STAT_YOLOV1_GR_SRC_FLUSH_TIME() SAMPLE_SVP_NNIE_PERF_STAT_ADD_DIFF_TIME(s_stYolov1Perf.stGRPerf.u64SrcFlushTime);
#define SAMPLE_SVP_NNIE_PERF_STAT_YOLOV1_GR_PRE_DST_FLUSH_TIME() SAMPLE_SVP_NNIE_PERF_STAT_ADD_DIFF_TIME(s_stYolov1Perf.stGRPerf.u64PreDstFulshTime);
#define SAMPLE_SVP_NNIE_PERF_STAT_YOLOV1_GR_AFTER_DST_FLUSH_TIME() SAMPLE_SVP_NNIE_PERF_STAT_ADD_DIFF_TIME(s_stYolov1Perf.stGRPerf.u64AferDstFulshTime);
#define SAMPLE_SVP_NNIE_PERF_STAT_YOLOV1_GR_OP_TIME() SAMPLE_SVP_NNIE_PERF_STAT_ADD_DIFF_TIME(s_stYolov1Perf.stGRPerf.u64OPTime);
#define SAMPLE_SVP_NNIE_PERF_STAT_YOLOV1_PRINT() printf("Yolov1 Forward time: %llu us,GR time:%llu us,Flush time: %llu us\n",\
    s_stYolov1Perf.stForwardPerf.u64OPTime/SAMPLE_SVP_NNIE_PERF_STAT_LOOP_TIMES,s_stYolov1Perf.stGRPerf.u64OPTime/SAMPLE_SVP_NNIE_PERF_STAT_LOOP_TIMES,\
    (s_stYolov1Perf.stForwardPerf.u64SrcFlushTime + s_stYolov1Perf.stForwardPerf.u64PreDstFulshTime + s_stYolov1Perf.stForwardPerf.u64AferDstFulshTime\
    + s_stYolov1Perf.stGRPerf.u64SrcFlushTime + s_stYolov1Perf.stGRPerf.u64PreDstFulshTime + s_stYolov1Perf.stGRPerf.u64AferDstFulshTime)/SAMPLE_SVP_NNIE_PERF_STAT_LOOP_TIMES );
/*Yolov2*/
#define SAMPLE_SVP_NNIE_PERF_STAT_YOLOV2_FORWARD_SRC_FLUSH_TIME() s_stYolov2Perf.stForwardPerf.u64SrcFlushTime += s_stOpForwardPerfTmp.u64SrcFlushTime;
#define SAMPLE_SVP_NNIE_PERF_STAT_YOLOV2_FORWARD_PRE_DST_FLUSH_TIME() s_stYolov2Perf.stForwardPerf.u64PreDstFulshTime += s_stOpForwardPerfTmp.u64PreDstFulshTime;
#define SAMPLE_SVP_NNIE_PERF_STAT_YOLOV2_FORWARD_AFTER_DST_FLUSH_TIME() s_stYolov2Perf.stForwardPerf.u64AferDstFulshTime += s_stOpForwardPerfTmp.u64AferDstFulshTime;
#define SAMPLE_SVP_NNIE_PERF_STAT_YOLOV2_FORWARD_OP_TIME() s_stYolov2Perf.stForwardPerf.u64OPTime += s_stOpForwardPerfTmp.u64OPTime;
#define SAMPLE_SVP_NNIE_PERF_STAT_YOLOV2_GR_SRC_FLUSH_TIME() SAMPLE_SVP_NNIE_PERF_STAT_ADD_DIFF_TIME(s_stYolov2Perf.stGRPerf.u64SrcFlushTime);
#define SAMPLE_SVP_NNIE_PERF_STAT_YOLOV2_GR_PRE_DST_FLUSH_TIME() SAMPLE_SVP_NNIE_PERF_STAT_ADD_DIFF_TIME(s_stYolov2Perf.stGRPerf.u64PreDstFulshTime);
#define SAMPLE_SVP_NNIE_PERF_STAT_YOLOV2_GR_AFTER_DST_FLUSH_TIME() SAMPLE_SVP_NNIE_PERF_STAT_ADD_DIFF_TIME(s_stYolov2Perf.stGRPerf.u64AferDstFulshTime);
#define SAMPLE_SVP_NNIE_PERF_STAT_YOLOV2_GR_OP_TIME() SAMPLE_SVP_NNIE_PERF_STAT_ADD_DIFF_TIME(s_stYolov2Perf.stGRPerf.u64OPTime);
#define SAMPLE_SVP_NNIE_PERF_STAT_YOLOV2_PRINT() printf("Yolov2 Forward time: %llu us,GR time:%llu us,Flush time: %llu us\n",\
    s_stYolov2Perf.stForwardPerf.u64OPTime/SAMPLE_SVP_NNIE_PERF_STAT_LOOP_TIMES,s_stYolov2Perf.stGRPerf.u64OPTime/SAMPLE_SVP_NNIE_PERF_STAT_LOOP_TIMES,\
    (s_stYolov2Perf.stForwardPerf.u64SrcFlushTime + s_stYolov2Perf.stForwardPerf.u64PreDstFulshTime + s_stYolov2Perf.stForwardPerf.u64AferDstFulshTime\
    + s_stYolov2Perf.stGRPerf.u64SrcFlushTime + s_stYolov2Perf.stGRPerf.u64PreDstFulshTime + s_stYolov2Perf.stGRPerf.u64AferDstFulshTime)/SAMPLE_SVP_NNIE_PERF_STAT_LOOP_TIMES );

/*SSD*/
#define SAMPLE_SVP_NNIE_PERF_STAT_SSD_FORWARD_SRC_FLUSH_TIME() s_stSsdPerf.stForwardPerf.u64SrcFlushTime += s_stOpForwardPerfTmp.u64SrcFlushTime;
#define SAMPLE_SVP_NNIE_PERF_STAT_SSD_FORWARD_PRE_DST_FLUSH_TIME() s_stSsdPerf.stForwardPerf.u64PreDstFulshTime += s_stOpForwardPerfTmp.u64PreDstFulshTime;
#define SAMPLE_SVP_NNIE_PERF_STAT_SSD_FORWARD_AFTER_DST_FLUSH_TIME() s_stSsdPerf.stForwardPerf.u64AferDstFulshTime += s_stOpForwardPerfTmp.u64AferDstFulshTime;
#define SAMPLE_SVP_NNIE_PERF_STAT_SSD_FORWARD_OP_TIME() s_stSsdPerf.stForwardPerf.u64OPTime += s_stOpForwardPerfTmp.u64OPTime;
#define SAMPLE_SVP_NNIE_PERF_STAT_SSD_GR_SRC_FLUSH_TIME() SAMPLE_SVP_NNIE_PERF_STAT_ADD_DIFF_TIME(s_stSsdPerf.stGRPerf.u64SrcFlushTime);
#define SAMPLE_SVP_NNIE_PERF_STAT_SSD_GR_PRE_DST_FLUSH_TIME() SAMPLE_SVP_NNIE_PERF_STAT_ADD_DIFF_TIME(s_stSsdPerf.stGRPerf.u64PreDstFulshTime);
#define SAMPLE_SVP_NNIE_PERF_STAT_SSD_GR_AFTER_DST_FLUSH_TIME() SAMPLE_SVP_NNIE_PERF_STAT_ADD_DIFF_TIME(s_stSsdPerf.stGRPerf.u64AferDstFulshTime);
#define SAMPLE_SVP_NNIE_PERF_STAT_SSD_GR_OP_TIME() SAMPLE_SVP_NNIE_PERF_STAT_ADD_DIFF_TIME(s_stSsdPerf.stGRPerf.u64OPTime);
#define SAMPLE_SVP_NNIE_PERF_STAT_SSD_PRINT() printf("SSD Forward time: %llu us,GR time:%llu us,Flush time: %llu us\n",\
    s_stSsdPerf.stForwardPerf.u64OPTime/SAMPLE_SVP_NNIE_PERF_STAT_LOOP_TIMES,s_stSsdPerf.stGRPerf.u64OPTime/SAMPLE_SVP_NNIE_PERF_STAT_LOOP_TIMES,\
    (s_stSsdPerf.stForwardPerf.u64SrcFlushTime + s_stSsdPerf.stForwardPerf.u64PreDstFulshTime + s_stSsdPerf.stForwardPerf.u64AferDstFulshTime\
    + s_stSsdPerf.stGRPerf.u64SrcFlushTime + s_stSsdPerf.stGRPerf.u64PreDstFulshTime + s_stSsdPerf.stGRPerf.u64AferDstFulshTime)/SAMPLE_SVP_NNIE_PERF_STAT_LOOP_TIMES );

/*Pvanet*/
#define SAMPLE_SVP_NNIE_PERF_STAT_PVANET_FORWARD_SRC_FLUSH_TIME() s_stPvanetPerf.stForwardPerf.u64SrcFlushTime += s_stOpForwardPerfTmp.u64SrcFlushTime;
#define SAMPLE_SVP_NNIE_PERF_STAT_PVANET_FORWARD_PRE_DST_FLUSH_TIME() s_stPvanetPerf.stForwardPerf.u64PreDstFulshTime += s_stOpForwardPerfTmp.u64PreDstFulshTime;
#define SAMPLE_SVP_NNIE_PERF_STAT_PVANET_FORWARD_AFTER_DST_FLUSH_TIME() s_stPvanetPerf.stForwardPerf.u64AferDstFulshTime += s_stOpForwardPerfTmp.u64AferDstFulshTime;
#define SAMPLE_SVP_NNIE_PERF_STAT_PVANET_FORWARD_OP_TIME() s_stPvanetPerf.stForwardPerf.u64OPTime += s_stOpForwardPerfTmp.u64OPTime;

#define SAMPLE_SVP_NNIE_PERF_STAT_PVANET_RPN_SRC_FLUSH_TIME() s_stPvanetPerf.stRpnPerf.u64SrcFlushTime += g_stOpRpnPerfTmp.u64SrcFlushTime;
#define SAMPLE_SVP_NNIE_PERF_STAT_PVANET_RPN_PRE_DST_FLUSH_TIME() s_stPvanetPerf.stRpnPerf.u64PreDstFulshTime += g_stOpRpnPerfTmp.u64PreDstFulshTime;
#define SAMPLE_SVP_NNIE_PERF_STAT_PVANET_RPN_AFTER_DST_FLUSH_TIME() s_stPvanetPerf.stRpnPerf.u64AferDstFulshTime += g_stOpRpnPerfTmp.u64AferDstFulshTime;
#define SAMPLE_SVP_NNIE_PERF_STAT_PVANET_RPN_OP_TIME() s_stPvanetPerf.stRpnPerf.u64OPTime += g_stOpRpnPerfTmp.u64OPTime;

#define SAMPLE_SVP_NNIE_PERF_STAT_PVANET_ROIPOOLING_SRC_FLUSH_TIME() s_stPvanetPerf.stRoiPoolingPerf.u64SrcFlushTime += s_stOpForwardPerfTmp.u64SrcFlushTime;
#define SAMPLE_SVP_NNIE_PERF_STAT_PVANET_ROIPOOLING_PRE_DST_FLUSH_TIME() s_stPvanetPerf.stRoiPoolingPerf.u64PreDstFulshTime += s_stOpForwardPerfTmp.u64PreDstFulshTime;
#define SAMPLE_SVP_NNIE_PERF_STAT_PVANET_ROIPOOLING_AFTER_DST_FLUSH_TIME() s_stPvanetPerf.stRoiPoolingPerf.u64AferDstFulshTime += s_stOpForwardPerfTmp.u64AferDstFulshTime;
#define SAMPLE_SVP_NNIE_PERF_STAT_PVANET_ROIPOOLING_OP_TIME() s_stPvanetPerf.stRoiPoolingPerf.u64OPTime += s_stOpForwardPerfTmp.u64OPTime;

#define SAMPLE_SVP_NNIE_PERF_STAT_PVANET_GR_SRC_FLUSH_TIME() SAMPLE_SVP_NNIE_PERF_STAT_ADD_DIFF_TIME(s_stPvanetPerf.stGRPerf.u64SrcFlushTime);
#define SAMPLE_SVP_NNIE_PERF_STAT_PVANET_GR_PRE_DST_FLUSH_TIME() SAMPLE_SVP_NNIE_PERF_STAT_ADD_DIFF_TIME(s_stPvanetPerf.stGRPerf.u64PreDstFulshTime);
#define SAMPLE_SVP_NNIE_PERF_STAT_PVANET_GR_AFTER_DST_FLUSH_TIME() SAMPLE_SVP_NNIE_PERF_STAT_ADD_DIFF_TIME(s_stPvanetPerf.stGRPerf.u64AferDstFulshTime);
#define SAMPLE_SVP_NNIE_PERF_STAT_PVANET_GR_OP_TIME() SAMPLE_SVP_NNIE_PERF_STAT_ADD_DIFF_TIME(s_stPvanetPerf.stGRPerf.u64OPTime);
#define SAMPLE_SVP_NNIE_PERF_STAT_PVANET_PRINT() printf("Pvanet Forward time: %llu us,Rpn time:%llu us,RoiPooling time:%llu us,GR time:%llu us,Flush time: %llu us\n",\
    s_stPvanetPerf.stForwardPerf.u64OPTime/SAMPLE_SVP_NNIE_PERF_STAT_LOOP_TIMES,s_stPvanetPerf.stRpnPerf.u64OPTime/SAMPLE_SVP_NNIE_PERF_STAT_LOOP_TIMES,\
    s_stPvanetPerf.stRoiPoolingPerf.u64OPTime/SAMPLE_SVP_NNIE_PERF_STAT_LOOP_TIMES, s_stPvanetPerf.stGRPerf.u64OPTime/SAMPLE_SVP_NNIE_PERF_STAT_LOOP_TIMES,\
    (s_stPvanetPerf.stForwardPerf.u64SrcFlushTime + s_stPvanetPerf.stForwardPerf.u64PreDstFulshTime + s_stPvanetPerf.stForwardPerf.u64AferDstFulshTime\
    + s_stPvanetPerf.stRpnPerf.u64SrcFlushTime + s_stPvanetPerf.stRpnPerf.u64PreDstFulshTime + s_stPvanetPerf.stRpnPerf.u64AferDstFulshTime\
    + s_stPvanetPerf.stRoiPoolingPerf.u64SrcFlushTime + s_stPvanetPerf.stRoiPoolingPerf.u64PreDstFulshTime + s_stPvanetPerf.stRoiPoolingPerf.u64AferDstFulshTime\
    + s_stPvanetPerf.stGRPerf.u64SrcFlushTime + s_stPvanetPerf.stGRPerf.u64PreDstFulshTime + s_stPvanetPerf.stGRPerf.u64AferDstFulshTime)/SAMPLE_SVP_NNIE_PERF_STAT_LOOP_TIMES );

/*RFCN*/
#define SAMPLE_SVP_NNIE_PERF_STAT_RFCN_FORWARD_SRC_FLUSH_TIME() s_stRfcnPerf.stForwardPerf.u64SrcFlushTime += s_stOpForwardPerfTmp.u64SrcFlushTime;
#define SAMPLE_SVP_NNIE_PERF_STAT_RFCN_FORWARD_PRE_DST_FLUSH_TIME() s_stRfcnPerf.stForwardPerf.u64PreDstFulshTime += s_stOpForwardPerfTmp.u64PreDstFulshTime;
#define SAMPLE_SVP_NNIE_PERF_STAT_RFCN_FORWARD_AFTER_DST_FLUSH_TIME() s_stRfcnPerf.stForwardPerf.u64AferDstFulshTime += s_stOpForwardPerfTmp.u64AferDstFulshTime;
#define SAMPLE_SVP_NNIE_PERF_STAT_RFCN_FORWARD_OP_TIME() s_stRfcnPerf.stForwardPerf.u64OPTime += s_stOpForwardPerfTmp.u64OPTime;

#define SAMPLE_SVP_NNIE_PERF_STAT_RFCN_RPN_SRC_FLUSH_TIME() s_stRfcnPerf.stRpnPerf.u64SrcFlushTime += g_stOpRpnPerfTmp.u64SrcFlushTime;
#define SAMPLE_SVP_NNIE_PERF_STAT_RFCN_RPN_PRE_DST_FLUSH_TIME() s_stRfcnPerf.stRpnPerf.u64PreDstFulshTime += g_stOpRpnPerfTmp.u64PreDstFulshTime;
#define SAMPLE_SVP_NNIE_PERF_STAT_RFCN_RPN_AFTER_DST_FLUSH_TIME() s_stRfcnPerf.stRpnPerf.u64AferDstFulshTime += g_stOpRpnPerfTmp.u64AferDstFulshTime;
#define SAMPLE_SVP_NNIE_PERF_STAT_RFCN_RPN_OP_TIME() s_stRfcnPerf.stRpnPerf.u64OPTime += g_stOpRpnPerfTmp.u64OPTime;

#define SAMPLE_SVP_NNIE_PERF_STAT_RFCN_PSROIPOOLING1_SRC_FLUSH_TIME() s_stRfcnPerf.stPsRoiPooling1Perf.u64SrcFlushTime += s_stOpForwardPerfTmp.u64SrcFlushTime;
#define SAMPLE_SVP_NNIE_PERF_STAT_RFCN_PSROIPOOLING1_PRE_DST_FLUSH_TIME() s_stRfcnPerf.stPsRoiPooling1Perf.u64PreDstFulshTime += s_stOpForwardPerfTmp.u64PreDstFulshTime;
#define SAMPLE_SVP_NNIE_PERF_STAT_RFCN_PSROIPOOLING1_AFTER_DST_FLUSH_TIME() s_stRfcnPerf.stPsRoiPooling1Perf.u64AferDstFulshTime += s_stOpForwardPerfTmp.u64AferDstFulshTime;
#define SAMPLE_SVP_NNIE_PERF_STAT_RFCN_PSROIPOOLING1_OP_TIME() s_stRfcnPerf.stPsRoiPooling1Perf.u64OPTime += s_stOpForwardPerfTmp.u64OPTime;

#define SAMPLE_SVP_NNIE_PERF_STAT_RFCN_PSROIPOOLING2_SRC_FLUSH_TIME() s_stRfcnPerf.stPsRoiPooling2Perf.u64SrcFlushTime += s_stOpForwardPerfTmp.u64SrcFlushTime;
#define SAMPLE_SVP_NNIE_PERF_STAT_RFCN_PSROIPOOLING2_PRE_DST_FLUSH_TIME() s_stRfcnPerf.stPsRoiPooling2Perf.u64PreDstFulshTime += s_stOpForwardPerfTmp.u64PreDstFulshTime;
#define SAMPLE_SVP_NNIE_PERF_STAT_RFCN_PSROIPOOLING2_AFTER_DST_FLUSH_TIME() s_stRfcnPerf.stPsRoiPooling2Perf.u64AferDstFulshTime += s_stOpForwardPerfTmp.u64AferDstFulshTime;
#define SAMPLE_SVP_NNIE_PERF_STAT_RFCN_PSROIPOOLING2_OP_TIME() s_stRfcnPerf.stPsRoiPooling2Perf.u64OPTime += s_stOpForwardPerfTmp.u64OPTime;

#define SAMPLE_SVP_NNIE_PERF_STAT_RFCN_GR_SRC_FLUSH_TIME() SAMPLE_SVP_NNIE_PERF_STAT_ADD_DIFF_TIME(s_stRfcnPerf.stGRPerf.u64SrcFlushTime);
#define SAMPLE_SVP_NNIE_PERF_STAT_RFCN_GR_PRE_DST_FLUSH_TIME() SAMPLE_SVP_NNIE_PERF_STAT_ADD_DIFF_TIME(s_stRfcnPerf.stGRPerf.u64PreDstFulshTime);
#define SAMPLE_SVP_NNIE_PERF_STAT_RFCN_GR_AFTER_DST_FLUSH_TIME() SAMPLE_SVP_NNIE_PERF_STAT_ADD_DIFF_TIME(s_stRfcnPerf.stGRPerf.u64AferDstFulshTime);
#define SAMPLE_SVP_NNIE_PERF_STAT_RFCN_GR_OP_TIME() SAMPLE_SVP_NNIE_PERF_STAT_ADD_DIFF_TIME(s_stRfcnPerf.stGRPerf.u64OPTime);
#define SAMPLE_SVP_NNIE_PERF_STAT_RFCN_PRINT_VITOVO() printf("Rfcn_ViToVo Forward time: %llu us,Rpn time:%llu us,PsRoiPooling1 time:%llu us,PsRoiPooling2 time:%llu us,GR time:%llu us,Flush time: %llu us\n",\
    s_stRfcnPerf.stForwardPerf.u64OPTime,s_stRfcnPerf.stRpnPerf.u64OPTime,\
    s_stRfcnPerf.stPsRoiPooling1Perf.u64OPTime, s_stRfcnPerf.stPsRoiPooling2Perf.u64OPTime,s_stRfcnPerf.stGRPerf.u64OPTime,\
    (s_stRfcnPerf.stForwardPerf.u64SrcFlushTime + s_stRfcnPerf.stForwardPerf.u64PreDstFulshTime + s_stRfcnPerf.stForwardPerf.u64AferDstFulshTime\
    + s_stRfcnPerf.stRpnPerf.u64SrcFlushTime + s_stRfcnPerf.stRpnPerf.u64PreDstFulshTime + s_stRfcnPerf.stRpnPerf.u64AferDstFulshTime\
    + s_stRfcnPerf.stPsRoiPooling1Perf.u64SrcFlushTime + s_stRfcnPerf.stPsRoiPooling1Perf.u64PreDstFulshTime + s_stRfcnPerf.stPsRoiPooling1Perf.u64AferDstFulshTime\
    + s_stRfcnPerf.stPsRoiPooling2Perf.u64SrcFlushTime + s_stRfcnPerf.stPsRoiPooling2Perf.u64PreDstFulshTime + s_stRfcnPerf.stPsRoiPooling2Perf.u64AferDstFulshTime\
    + s_stRfcnPerf.stGRPerf.u64SrcFlushTime + s_stRfcnPerf.stGRPerf.u64PreDstFulshTime + s_stRfcnPerf.stGRPerf.u64AferDstFulshTime));

#define SAMPLE_SVP_NNIE_PERF_STAT_RFCN_PRINT_FILE() printf("Rfcn_File Forward time: %llu us,Rpn time:%llu us,PsRoiPooling1 time:%llu us,PsRoiPooling2 time:%llu us,GR time:%llu us,Flush time: %llu us\n",\
    s_stRfcnPerf.stForwardPerf.u64OPTime/SAMPLE_SVP_NNIE_PERF_STAT_LOOP_TIMES,s_stRfcnPerf.stRpnPerf.u64OPTime/SAMPLE_SVP_NNIE_PERF_STAT_LOOP_TIMES,\
    s_stRfcnPerf.stPsRoiPooling1Perf.u64OPTime/SAMPLE_SVP_NNIE_PERF_STAT_LOOP_TIMES,s_stRfcnPerf.stPsRoiPooling2Perf.u64OPTime/SAMPLE_SVP_NNIE_PERF_STAT_LOOP_TIMES,\
    s_stRfcnPerf.stGRPerf.u64OPTime/SAMPLE_SVP_NNIE_PERF_STAT_LOOP_TIMES,\
    (s_stRfcnPerf.stForwardPerf.u64SrcFlushTime + s_stRfcnPerf.stForwardPerf.u64PreDstFulshTime + s_stRfcnPerf.stForwardPerf.u64AferDstFulshTime\
    + s_stRfcnPerf.stRpnPerf.u64SrcFlushTime + s_stRfcnPerf.stRpnPerf.u64PreDstFulshTime + s_stRfcnPerf.stRpnPerf.u64AferDstFulshTime\
    + s_stRfcnPerf.stPsRoiPooling1Perf.u64SrcFlushTime + s_stRfcnPerf.stPsRoiPooling1Perf.u64PreDstFulshTime + s_stRfcnPerf.stPsRoiPooling1Perf.u64AferDstFulshTime\
    + s_stRfcnPerf.stPsRoiPooling2Perf.u64SrcFlushTime + s_stRfcnPerf.stPsRoiPooling2Perf.u64PreDstFulshTime + s_stRfcnPerf.stPsRoiPooling2Perf.u64AferDstFulshTime\
    + s_stRfcnPerf.stGRPerf.u64SrcFlushTime + s_stRfcnPerf.stGRPerf.u64PreDstFulshTime + s_stRfcnPerf.stGRPerf.u64AferDstFulshTime)/SAMPLE_SVP_NNIE_PERF_STAT_LOOP_TIMES);



static SAMPLE_SVP_NNIE_YOLO_PERF_STAT_S s_stYolov1Perf = {0};
static SAMPLE_SVP_NNIE_YOLO_PERF_STAT_S s_stYolov2Perf = {0};
static SAMPLE_SVP_NNIE_SSD_PERF_STAT_S  s_stSsdPerf = {0};
static SAMPLE_SVP_NNIE_PVANET_PERF_STAT_S s_stPvanetPerf = {0};
static SAMPLE_SVP_NNIE_RFCN_PERF_STAT_S s_stRfcnPerf = {0};

static SAMPLE_SVP_NNIE_OP_PERF_STAT_S   s_stOpForwardPerfTmp = {0};
extern SAMPLE_SVP_NNIE_OP_PERF_STAT_S   g_stOpRpnPerfTmp;
#else

#define SAMPLE_SVP_NNIE_PERF_STAT_OP_FORWARD_CLREAR()
#define SAMPLE_SVP_NNIE_PERF_STAT_OP_FORWARD_SRC_FLUSH_TIME()
#define SAMPLE_SVP_NNIE_PERF_STAT_OP_FORWARD_PRE_DST_FLUSH_TIME()
#define SAMPLE_SVP_NNIE_PERF_STAT_OP_FORWARD_AFTER_DST_FLUSH_TIME()
#define SAMPLE_SVP_NNIE_PERF_STAT_OP_FORWARD_OP_TIME()

#define SAMPLE_SVP_NNIE_PERF_STAT_YOLOV1_FORWARD_SRC_FLUSH_TIME()
#define SAMPLE_SVP_NNIE_PERF_STAT_YOLOV1_FORWARD_PRE_DST_FLUSH_TIME()
#define SAMPLE_SVP_NNIE_PERF_STAT_YOLOV1_FORWARD_AFTER_DST_FLUSH_TIME()
#define SAMPLE_SVP_NNIE_PERF_STAT_YOLOV1_FORWARD_OP_TIME()
#define SAMPLE_SVP_NNIE_PERF_STAT_YOLOV1_GR_SRC_FLUSH_TIME()
#define SAMPLE_SVP_NNIE_PERF_STAT_YOLOV1_GR_PRE_DST_FLUSH_TIME()
#define SAMPLE_SVP_NNIE_PERF_STAT_YOLOV1_GR_AFTER_DST_FLUSH_TIME()
#define SAMPLE_SVP_NNIE_PERF_STAT_YOLOV1_GR_OP_TIME()
#define SAMPLE_SVP_NNIE_PERF_STAT_YOLOV1_PRINT()

/*Yolov2*/
#define SAMPLE_SVP_NNIE_PERF_STAT_YOLOV2_FORWARD_SRC_FLUSH_TIME()
#define SAMPLE_SVP_NNIE_PERF_STAT_YOLOV2_FORWARD_PRE_DST_FLUSH_TIME()
#define SAMPLE_SVP_NNIE_PERF_STAT_YOLOV2_FORWARD_AFTER_DST_FLUSH_TIME()
#define SAMPLE_SVP_NNIE_PERF_STAT_YOLOV2_FORWARD_OP_TIME()
#define SAMPLE_SVP_NNIE_PERF_STAT_YOLOV2_GR_SRC_FLUSH_TIME()
#define SAMPLE_SVP_NNIE_PERF_STAT_YOLOV2_GR_PRE_DST_FLUSH_TIME()
#define SAMPLE_SVP_NNIE_PERF_STAT_YOLOV2_GR_AFTER_DST_FLUSH_TIME()
#define SAMPLE_SVP_NNIE_PERF_STAT_YOLOV2_GR_OP_TIME()
#define SAMPLE_SVP_NNIE_PERF_STAT_YOLOV2_PRINT()
/*SSD*/
#define SAMPLE_SVP_NNIE_PERF_STAT_SSD_FORWARD_SRC_FLUSH_TIME()
#define SAMPLE_SVP_NNIE_PERF_STAT_SSD_FORWARD_PRE_DST_FLUSH_TIME()
#define SAMPLE_SVP_NNIE_PERF_STAT_SSD_FORWARD_AFTER_DST_FLUSH_TIME()
#define SAMPLE_SVP_NNIE_PERF_STAT_SSD_FORWARD_OP_TIME()
#define SAMPLE_SVP_NNIE_PERF_STAT_SSD_GR_SRC_FLUSH_TIME()
#define SAMPLE_SVP_NNIE_PERF_STAT_SSD_GR_PRE_DST_FLUSH_TIME()
#define SAMPLE_SVP_NNIE_PERF_STAT_SSD_GR_AFTER_DST_FLUSH_TIME()
#define SAMPLE_SVP_NNIE_PERF_STAT_SSD_GR_OP_TIME()
#define SAMPLE_SVP_NNIE_PERF_STAT_SSD_PRINT()

/*Pvanet*/
#define SAMPLE_SVP_NNIE_PERF_STAT_PVANET_FORWARD_SRC_FLUSH_TIME()
#define SAMPLE_SVP_NNIE_PERF_STAT_PVANET_FORWARD_PRE_DST_FLUSH_TIME()
#define SAMPLE_SVP_NNIE_PERF_STAT_PVANET_FORWARD_AFTER_DST_FLUSH_TIME()
#define SAMPLE_SVP_NNIE_PERF_STAT_PVANET_FORWARD_OP_TIME()

#define SAMPLE_SVP_NNIE_PERF_STAT_PVANET_RPN_SRC_FLUSH_TIME()
#define SAMPLE_SVP_NNIE_PERF_STAT_PVANET_RPN_PRE_DST_FLUSH_TIME()
#define SAMPLE_SVP_NNIE_PERF_STAT_PVANET_RPN_AFTER_DST_FLUSH_TIME()
#define SAMPLE_SVP_NNIE_PERF_STAT_PVANET_RPN_OP_TIME()

#define SAMPLE_SVP_NNIE_PERF_STAT_PVANET_ROIPOOLING_SRC_FLUSH_TIME()
#define SAMPLE_SVP_NNIE_PERF_STAT_PVANET_ROIPOOLING_PRE_DST_FLUSH_TIME()
#define SAMPLE_SVP_NNIE_PERF_STAT_PVANET_ROIPOOLING_AFTER_DST_FLUSH_TIME()
#define SAMPLE_SVP_NNIE_PERF_STAT_PVANET_ROIPOOLING_OP_TIME()

#define SAMPLE_SVP_NNIE_PERF_STAT_PVANET_GR_SRC_FLUSH_TIME()
#define SAMPLE_SVP_NNIE_PERF_STAT_PVANET_GR_PRE_DST_FLUSH_TIME()
#define SAMPLE_SVP_NNIE_PERF_STAT_PVANET_GR_AFTER_DST_FLUSH_TIME()
#define SAMPLE_SVP_NNIE_PERF_STAT_PVANET_GR_OP_TIME()
#define SAMPLE_SVP_NNIE_PERF_STAT_PVANET_PRINT()

/*RFCN*/
#define SAMPLE_SVP_NNIE_PERF_STAT_RFCN_FORWARD_SRC_FLUSH_TIME()
#define SAMPLE_SVP_NNIE_PERF_STAT_RFCN_FORWARD_PRE_DST_FLUSH_TIME()
#define SAMPLE_SVP_NNIE_PERF_STAT_RFCN_FORWARD_AFTER_DST_FLUSH_TIME()
#define SAMPLE_SVP_NNIE_PERF_STAT_RFCN_FORWARD_OP_TIME()

#define SAMPLE_SVP_NNIE_PERF_STAT_RFCN_RPN_SRC_FLUSH_TIME()
#define SAMPLE_SVP_NNIE_PERF_STAT_RFCN_RPN_PRE_DST_FLUSH_TIME()
#define SAMPLE_SVP_NNIE_PERF_STAT_RFCN_RPN_AFTER_DST_FLUSH_TIME()
#define SAMPLE_SVP_NNIE_PERF_STAT_RFCN_RPN_OP_TIME()

#define SAMPLE_SVP_NNIE_PERF_STAT_RFCN_PSROIPOOLING1_SRC_FLUSH_TIME()
#define SAMPLE_SVP_NNIE_PERF_STAT_RFCN_PSROIPOOLING1_PRE_DST_FLUSH_TIME()
#define SAMPLE_SVP_NNIE_PERF_STAT_RFCN_PSROIPOOLING1_AFTER_DST_FLUSH_TIME()
#define SAMPLE_SVP_NNIE_PERF_STAT_RFCN_PSROIPOOLING1_OP_TIME()

#define SAMPLE_SVP_NNIE_PERF_STAT_RFCN_PSROIPOOLING2_SRC_FLUSH_TIME()
#define SAMPLE_SVP_NNIE_PERF_STAT_RFCN_PSROIPOOLING2_PRE_DST_FLUSH_TIME()
#define SAMPLE_SVP_NNIE_PERF_STAT_RFCN_PSROIPOOLING2_AFTER_DST_FLUSH_TIME()
#define SAMPLE_SVP_NNIE_PERF_STAT_RFCN_PSROIPOOLING2_OP_TIME()

#define SAMPLE_SVP_NNIE_PERF_STAT_RFCN_GR_SRC_FLUSH_TIME()
#define SAMPLE_SVP_NNIE_PERF_STAT_RFCN_GR_PRE_DST_FLUSH_TIME()
#define SAMPLE_SVP_NNIE_PERF_STAT_RFCN_GR_AFTER_DST_FLUSH_TIME()
#define SAMPLE_SVP_NNIE_PERF_STAT_RFCN_GR_OP_TIME()
#define SAMPLE_SVP_NNIE_PERF_STAT_RFCN_PRINT_VITOVO()
#define SAMPLE_SVP_NNIE_PERF_STAT_RFCN_PRINT_FILE()

#endif
/******************************************************************************
* function : NNIE Forward
******************************************************************************/
static HI_S32 SAMPLE_SVP_NNIE_Forward(SAMPLE_SVP_NNIE_PARAM_S *pstNnieParam,
    SAMPLE_SVP_NNIE_INPUT_DATA_INDEX_S* pstInputDataIdx,
    SAMPLE_SVP_NNIE_PROCESS_SEG_INDEX_S* pstProcSegIdx,HI_BOOL bInstant)
{
    HI_S32 s32Ret = HI_SUCCESS;
    HI_U32 i = 0, j = 0;
    HI_BOOL bFinish = HI_FALSE;
    SVP_NNIE_HANDLE hSvpNnieHandle = 0;
    HI_U32 u32TotalStepNum = 0;
    SAMPLE_SVP_NIE_PERF_STAT_DEF_VAR()

    SAMPLE_SVP_NNIE_PERF_STAT_OP_FORWARD_CLREAR()

    SAMPLE_COMM_SVP_FlushCache(pstNnieParam->astForwardCtrl[pstProcSegIdx->u32SegIdx].stTskBuf.u64PhyAddr,
        SAMPLE_SVP_NNIE_CONVERT_64BIT_ADDR(HI_VOID,pstNnieParam->astForwardCtrl[pstProcSegIdx->u32SegIdx].stTskBuf.u64VirAddr),
        pstNnieParam->astForwardCtrl[pstProcSegIdx->u32SegIdx].stTskBuf.u32Size);

    SAMPLE_SVP_NNIE_PERF_STAT_BEGIN()
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
    SAMPLE_SVP_NNIE_PERF_STAT_END()
    SAMPLE_SVP_NNIE_PERF_STAT_OP_FORWARD_PRE_DST_FLUSH_TIME()

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
    SAMPLE_SVP_NNIE_PERF_STAT_BEGIN()
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
    SAMPLE_SVP_NNIE_PERF_STAT_END()
    SAMPLE_SVP_NNIE_PERF_STAT_OP_FORWARD_OP_TIME()
    u32TotalStepNum = 0;

    SAMPLE_SVP_NNIE_PERF_STAT_BEGIN()
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
    SAMPLE_SVP_NNIE_PERF_STAT_END()
    SAMPLE_SVP_NNIE_PERF_STAT_OP_FORWARD_AFTER_DST_FLUSH_TIME()

    return s32Ret;
}

/******************************************************************************
* function : NNIE ForwardWithBbox
******************************************************************************/
static HI_S32 SAMPLE_SVP_NNIE_ForwardWithBbox(SAMPLE_SVP_NNIE_PARAM_S *pstNnieParam,
    SAMPLE_SVP_NNIE_INPUT_DATA_INDEX_S* pstInputDataIdx,SVP_SRC_BLOB_S astBbox[],
    SAMPLE_SVP_NNIE_PROCESS_SEG_INDEX_S* pstProcSegIdx,HI_BOOL bInstant)
{
    HI_S32 s32Ret = HI_SUCCESS;
    HI_BOOL bFinish = HI_FALSE;
    SVP_NNIE_HANDLE hSvpNnieHandle = 0;
    HI_U32 u32TotalStepNum = 0;
    HI_U32 i, j;
    SAMPLE_SVP_NIE_PERF_STAT_DEF_VAR()

    SAMPLE_SVP_NNIE_PERF_STAT_OP_FORWARD_CLREAR()

    SAMPLE_COMM_SVP_FlushCache(pstNnieParam->astForwardWithBboxCtrl[pstProcSegIdx->u32SegIdx].stTskBuf.u64PhyAddr,
        SAMPLE_SVP_NNIE_CONVERT_64BIT_ADDR(HI_VOID,pstNnieParam->astForwardWithBboxCtrl[pstProcSegIdx->u32SegIdx].stTskBuf.u64VirAddr),
        pstNnieParam->astForwardWithBboxCtrl[pstProcSegIdx->u32SegIdx].stTskBuf.u32Size);

    SAMPLE_SVP_NNIE_PERF_STAT_BEGIN()

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
    SAMPLE_SVP_NNIE_PERF_STAT_END()
    SAMPLE_SVP_NNIE_PERF_STAT_OP_FORWARD_PRE_DST_FLUSH_TIME()

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

    SAMPLE_SVP_NNIE_PERF_STAT_BEGIN()
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
    SAMPLE_SVP_NNIE_PERF_STAT_END()
    SAMPLE_SVP_NNIE_PERF_STAT_OP_FORWARD_OP_TIME()

    u32TotalStepNum = 0;
    SAMPLE_SVP_NNIE_PERF_STAT_BEGIN()

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
    SAMPLE_SVP_NNIE_PERF_STAT_END()
    SAMPLE_SVP_NNIE_PERF_STAT_OP_FORWARD_AFTER_DST_FLUSH_TIME()

    return s32Ret;
}


/******************************************************************************
* function : Fill Src Data
******************************************************************************/
static HI_S32 SAMPLE_SVP_NNIE_FillSrcData(SAMPLE_SVP_NNIE_CFG_S* pstNnieCfg,
    SAMPLE_SVP_NNIE_PARAM_S *pstNnieParam, SAMPLE_SVP_NNIE_INPUT_DATA_INDEX_S* pstInputDataIdx)
{
    FILE* fp = NULL;
    HI_U32 i =0, j = 0, n = 0;
    HI_U32 u32Height = 0, u32Width = 0, u32Chn = 0, u32Stride = 0, u32Dim = 0;
    HI_U32 u32VarSize = 0;
    HI_S32 s32Ret = HI_SUCCESS;
    HI_U8*pu8PicAddr = NULL;
    HI_U32*pu32StepAddr = NULL;
    HI_U32 u32SegIdx = pstInputDataIdx->u32SegIdx;
    HI_U32 u32NodeIdx = pstInputDataIdx->u32NodeIdx;
    HI_U32 u32TotalStepNum = 0;

    /*open file*/
    if (NULL != pstNnieCfg->pszPic)
    {
        fp = fopen(pstNnieCfg->pszPic,"rb");
        SAMPLE_SVP_CHECK_EXPR_RET(NULL == fp,HI_INVALID_VALUE,SAMPLE_SVP_ERR_LEVEL_ERROR,
            "Error, open file failed!\n");
    }

    /*get data size*/
    if(SVP_BLOB_TYPE_U8 <= pstNnieParam->astSegData[u32SegIdx].astSrc[u32NodeIdx].enType &&
        SVP_BLOB_TYPE_YVU422SP >= pstNnieParam->astSegData[u32SegIdx].astSrc[u32NodeIdx].enType)
    {
        u32VarSize = sizeof(HI_U8);
    }
    else
    {
        u32VarSize = sizeof(HI_U32);
    }

    /*fill src data*/
    if(SVP_BLOB_TYPE_SEQ_S32 == pstNnieParam->astSegData[u32SegIdx].astSrc[u32NodeIdx].enType)
    {
        u32Dim = pstNnieParam->astSegData[u32SegIdx].astSrc[u32NodeIdx].unShape.stSeq.u32Dim;
        u32Stride = pstNnieParam->astSegData[u32SegIdx].astSrc[u32NodeIdx].u32Stride;
        pu32StepAddr = SAMPLE_SVP_NNIE_CONVERT_64BIT_ADDR(HI_U32,pstNnieParam->astSegData[u32SegIdx].astSrc[u32NodeIdx].unShape.stSeq.u64VirAddrStep);
        pu8PicAddr = SAMPLE_SVP_NNIE_CONVERT_64BIT_ADDR(HI_U8,pstNnieParam->astSegData[u32SegIdx].astSrc[u32NodeIdx].u64VirAddr);
        for(n = 0; n < pstNnieParam->astSegData[u32SegIdx].astSrc[u32NodeIdx].u32Num; n++)
        {
            for(i = 0;i < *(pu32StepAddr+n); i++)
            {
                s32Ret = fread(pu8PicAddr,u32Dim*u32VarSize,1,fp);
                SAMPLE_SVP_CHECK_EXPR_GOTO(1 != s32Ret,FAIL,SAMPLE_SVP_ERR_LEVEL_ERROR,"Error,Read image file failed!\n");
                pu8PicAddr += u32Stride;
            }
            u32TotalStepNum += *(pu32StepAddr+n);
        }
        SAMPLE_COMM_SVP_FlushCache(pstNnieParam->astSegData[u32SegIdx].astSrc[u32NodeIdx].u64PhyAddr,
            SAMPLE_SVP_NNIE_CONVERT_64BIT_ADDR(HI_VOID,pstNnieParam->astSegData[u32SegIdx].astSrc[u32NodeIdx].u64VirAddr),
            u32TotalStepNum*u32Stride);
    }
    else
    {
        u32Height = pstNnieParam->astSegData[u32SegIdx].astSrc[u32NodeIdx].unShape.stWhc.u32Height;
        u32Width = pstNnieParam->astSegData[u32SegIdx].astSrc[u32NodeIdx].unShape.stWhc.u32Width;
        u32Chn = pstNnieParam->astSegData[u32SegIdx].astSrc[u32NodeIdx].unShape.stWhc.u32Chn;
        u32Stride = pstNnieParam->astSegData[u32SegIdx].astSrc[u32NodeIdx].u32Stride;
        pu8PicAddr = SAMPLE_SVP_NNIE_CONVERT_64BIT_ADDR(HI_U8,pstNnieParam->astSegData[u32SegIdx].astSrc[u32NodeIdx].u64VirAddr);
        if(SVP_BLOB_TYPE_YVU420SP== pstNnieParam->astSegData[u32SegIdx].astSrc[u32NodeIdx].enType)
        {
            for(n = 0; n < pstNnieParam->astSegData[u32SegIdx].astSrc[u32NodeIdx].u32Num; n++)
            {
                for(i = 0; i < u32Chn*u32Height/2; i++)
                {
                    s32Ret = fread(pu8PicAddr,u32Width*u32VarSize,1,fp);
                    SAMPLE_SVP_CHECK_EXPR_GOTO(1 != s32Ret,FAIL,SAMPLE_SVP_ERR_LEVEL_ERROR,"Error,Read image file failed!\n");
                    pu8PicAddr += u32Stride;
                }
            }
        }
        else if(SVP_BLOB_TYPE_YVU422SP== pstNnieParam->astSegData[u32SegIdx].astSrc[u32NodeIdx].enType)
        {
            for(n = 0; n < pstNnieParam->astSegData[u32SegIdx].astSrc[u32NodeIdx].u32Num; n++)
            {
                for(i = 0; i < u32Height*2; i++)
                {
                    s32Ret = fread(pu8PicAddr,u32Width*u32VarSize,1,fp);
                    SAMPLE_SVP_CHECK_EXPR_GOTO(1 != s32Ret,FAIL,SAMPLE_SVP_ERR_LEVEL_ERROR,"Error,Read image file failed!\n");
                    pu8PicAddr += u32Stride;
                }
            }
        }
        else
        {
            for(n = 0; n < pstNnieParam->astSegData[u32SegIdx].astSrc[u32NodeIdx].u32Num; n++)
            {
                for(i = 0;i < u32Chn; i++)
                {
                    for(j = 0; j < u32Height; j++)
                    {
                        s32Ret = fread(pu8PicAddr,u32Width*u32VarSize,1,fp);
                        SAMPLE_SVP_CHECK_EXPR_GOTO(1 != s32Ret,FAIL,SAMPLE_SVP_ERR_LEVEL_ERROR,"Error,Read image file failed!\n");
                        pu8PicAddr += u32Stride;
                    }
                }
            }
        }
        SAMPLE_COMM_SVP_FlushCache(pstNnieParam->astSegData[u32SegIdx].astSrc[u32NodeIdx].u64PhyAddr,
            SAMPLE_SVP_NNIE_CONVERT_64BIT_ADDR(HI_VOID,pstNnieParam->astSegData[u32SegIdx].astSrc[u32NodeIdx].u64VirAddr),
            pstNnieParam->astSegData[u32SegIdx].astSrc[u32NodeIdx].u32Num*u32Chn*u32Height*u32Stride);
    }

    fclose(fp);
    return HI_SUCCESS;
FAIL:

    fclose(fp);
    return HI_FAILURE;
}

/******************************************************************************
* function : print report result
******************************************************************************/
static HI_S32 SAMPLE_SVP_NNIE_PrintReportResult(SAMPLE_SVP_NNIE_PARAM_S *pstNnieParam)
{
    HI_U32 u32SegNum = pstNnieParam->pstModel->u32NetSegNum;
    HI_U32 i = 0, j = 0, k = 0, n = 0;
    HI_U32 u32SegIdx = 0, u32NodeIdx = 0;
    HI_S32 s32Ret = HI_SUCCESS;
    HI_CHAR acReportFileName[SAMPLE_SVP_NNIE_REPORT_NAME_LENGTH] = {'\0'};
    FILE* fp = NULL;
    HI_U32*pu32StepAddr = NULL;
    HI_S32*ps32ResultAddr = NULL;
    HI_U32 u32Height = 0, u32Width = 0, u32Chn = 0, u32Stride = 0, u32Dim = 0;

    for(u32SegIdx = 0; u32SegIdx < u32SegNum; u32SegIdx++)
    {
        for(u32NodeIdx = 0; u32NodeIdx < pstNnieParam->pstModel->astSeg[u32SegIdx].u16DstNum; u32NodeIdx++)
        {
            s32Ret = snprintf(acReportFileName,SAMPLE_SVP_NNIE_REPORT_NAME_LENGTH,
                "seg%d_layer%d_output%d_inst.linear.hex",u32SegIdx,
                pstNnieParam->pstModel->astSeg[u32SegIdx].astDstNode[u32NodeIdx].u32NodeId,0);
            SAMPLE_SVP_CHECK_EXPR_RET(s32Ret < 0,HI_INVALID_VALUE,SAMPLE_SVP_ERR_LEVEL_ERROR,
                "Error,create file name failed!\n");

            fp = fopen(acReportFileName,"w");
            SAMPLE_SVP_CHECK_EXPR_RET(NULL == fp,HI_INVALID_VALUE,SAMPLE_SVP_ERR_LEVEL_ERROR,
                "Error,open file failed!\n");

            if(SVP_BLOB_TYPE_SEQ_S32 == pstNnieParam->astSegData[u32SegIdx].astDst[u32NodeIdx].enType)
            {
                u32Dim = pstNnieParam->astSegData[u32SegIdx].astDst[u32NodeIdx].unShape.stSeq.u32Dim;
                u32Stride = pstNnieParam->astSegData[u32SegIdx].astDst[u32NodeIdx].u32Stride;
                pu32StepAddr = SAMPLE_SVP_NNIE_CONVERT_64BIT_ADDR(HI_U32,pstNnieParam->astSegData[u32SegIdx].astDst[u32NodeIdx].unShape.stSeq.u64VirAddrStep);
                ps32ResultAddr = SAMPLE_SVP_NNIE_CONVERT_64BIT_ADDR(HI_S32,pstNnieParam->astSegData[u32SegIdx].astDst[u32NodeIdx].u64VirAddr);

                for(n = 0; n < pstNnieParam->astSegData[u32SegIdx].astDst[u32NodeIdx].u32Num; n++)
                {
                    for(i = 0;i < *(pu32StepAddr+n); i++)
                    {
                        for(j = 0; j < u32Dim; j++)
                        {
                            s32Ret = fprintf(fp ,"%08x\n",*(ps32ResultAddr+j));
                            SAMPLE_SVP_CHECK_EXPR_GOTO(s32Ret < 0,PRINT_FAIL,
                                SAMPLE_SVP_ERR_LEVEL_ERROR,"Error,write report result file failed!\n");
                        }
                        ps32ResultAddr += u32Stride/sizeof(HI_U32);
                    }
                }
            }
            else
            {
                u32Height = pstNnieParam->astSegData[u32SegIdx].astDst[u32NodeIdx].unShape.stWhc.u32Height;
                u32Width = pstNnieParam->astSegData[u32SegIdx].astDst[u32NodeIdx].unShape.stWhc.u32Width;
                u32Chn = pstNnieParam->astSegData[u32SegIdx].astDst[u32NodeIdx].unShape.stWhc.u32Chn;
                u32Stride = pstNnieParam->astSegData[u32SegIdx].astDst[u32NodeIdx].u32Stride;
                ps32ResultAddr = SAMPLE_SVP_NNIE_CONVERT_64BIT_ADDR(HI_S32,pstNnieParam->astSegData[u32SegIdx].astDst[u32NodeIdx].u64VirAddr);
                for(n = 0; n < pstNnieParam->astSegData[u32SegIdx].astDst[u32NodeIdx].u32Num; n++)
                {
                    for(i = 0;i < u32Chn; i++)
                    {
                        for(j = 0; j < u32Height; j++)
                        {
                            for(k = 0; k < u32Width; k++)
                            {
                                s32Ret = fprintf(fp,"%08x\n",*(ps32ResultAddr+k));
                                SAMPLE_SVP_CHECK_EXPR_GOTO(s32Ret < 0,PRINT_FAIL,
                                    SAMPLE_SVP_ERR_LEVEL_ERROR,"Error,write report result file failed!\n");
                            }
                            ps32ResultAddr += u32Stride/sizeof(HI_U32);
                        }
                    }
                }
            }
            fclose(fp);
        }
    }
    return HI_SUCCESS;

PRINT_FAIL:
    fclose(fp);
    return HI_FAILURE;
}


/******************************************************************************
* function : Cnn software deinit
******************************************************************************/
static HI_S32 SAMPLE_SVP_NNIE_Cnn_SoftwareDeinit(SAMPLE_SVP_NNIE_CNN_SOFTWARE_PARAM_S* pstCnnSoftWarePara)
{
    HI_S32 s32Ret = HI_SUCCESS;
    SAMPLE_SVP_CHECK_EXPR_RET(NULL == pstCnnSoftWarePara,HI_INVALID_VALUE,SAMPLE_SVP_ERR_LEVEL_ERROR,
        "Error, pstCnnSoftWarePara can't be NULL!\n");
    if(0!=pstCnnSoftWarePara->stGetTopN.u64PhyAddr && 0!=pstCnnSoftWarePara->stGetTopN.u64VirAddr)
    {
        SAMPLE_SVP_MMZ_FREE(pstCnnSoftWarePara->stGetTopN.u64PhyAddr,
            pstCnnSoftWarePara->stGetTopN.u64VirAddr);
        pstCnnSoftWarePara->stGetTopN.u64PhyAddr = 0;
        pstCnnSoftWarePara->stGetTopN.u64VirAddr = 0;
    }
    return s32Ret;
}


/******************************************************************************
* function : Cnn Deinit
******************************************************************************/
static HI_S32 SAMPLE_SVP_NNIE_Cnn_Deinit(SAMPLE_SVP_NNIE_PARAM_S *pstNnieParam,
    SAMPLE_SVP_NNIE_CNN_SOFTWARE_PARAM_S* pstSoftWareParam,SAMPLE_SVP_NNIE_MODEL_S* pstNnieModel)
{
    HI_S32 s32Ret = HI_SUCCESS;
    /*hardware para deinit*/
    if(pstNnieParam!=NULL)
    {
        s32Ret = SAMPLE_COMM_SVP_NNIE_ParamDeinit(pstNnieParam);
        SAMPLE_SVP_CHECK_EXPR_TRACE(HI_SUCCESS != s32Ret,SAMPLE_SVP_ERR_LEVEL_ERROR,
            "Error,SAMPLE_COMM_SVP_NNIE_ParamDeinit failed!\n");
    }
    /*software para deinit*/
    if(pstSoftWareParam!=NULL)
    {
        s32Ret = SAMPLE_SVP_NNIE_Cnn_SoftwareDeinit(pstSoftWareParam);
        SAMPLE_SVP_CHECK_EXPR_TRACE(HI_SUCCESS != s32Ret,SAMPLE_SVP_ERR_LEVEL_ERROR,
            "Error,SAMPLE_SVP_NNIE_Cnn_SoftwareDeinit failed!\n");
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
* function : Cnn software para init
******************************************************************************/
static HI_S32 SAMPLE_SVP_NNIE_Cnn_SoftwareParaInit(SAMPLE_SVP_NNIE_CFG_S* pstNnieCfg,
    SAMPLE_SVP_NNIE_PARAM_S *pstCnnPara, SAMPLE_SVP_NNIE_CNN_SOFTWARE_PARAM_S* pstCnnSoftWarePara)
{
    HI_U32 u32GetTopNMemSize = 0;
    HI_U32 u32GetTopNAssistBufSize = 0;
    HI_U32 u32GetTopNPerFrameSize = 0;
    HI_U32 u32TotalSize = 0;
    HI_U32 u32ClassNum = pstCnnPara->pstModel->astSeg[0].astDstNode[0].unShape.stWhc.u32Width;
    HI_U64 u64PhyAddr = 0;
    HI_U8* pu8VirAddr = NULL;
    HI_S32 s32Ret = HI_SUCCESS;

    /*get mem size*/
    u32GetTopNPerFrameSize = pstCnnSoftWarePara->u32TopN*sizeof(SAMPLE_SVP_NNIE_CNN_GETTOPN_UNIT_S);
    u32GetTopNMemSize = SAMPLE_SVP_NNIE_ALIGN16(u32GetTopNPerFrameSize)*pstNnieCfg->u32MaxInputNum;
    u32GetTopNAssistBufSize = u32ClassNum*sizeof(SAMPLE_SVP_NNIE_CNN_GETTOPN_UNIT_S);
    u32TotalSize = u32GetTopNMemSize+u32GetTopNAssistBufSize;

    /*malloc mem*/
    s32Ret = SAMPLE_COMM_SVP_MallocMem("SAMPLE_CNN_INIT",NULL,(HI_U64*)&u64PhyAddr,
        (void**)&pu8VirAddr,u32TotalSize);
    SAMPLE_SVP_CHECK_EXPR_RET(HI_SUCCESS != s32Ret,s32Ret,SAMPLE_SVP_ERR_LEVEL_ERROR,
        "Error,Malloc memory failed!\n");
    memset(pu8VirAddr, 0, u32TotalSize);

    /*init GetTopn */
    pstCnnSoftWarePara->stGetTopN.u32Num= pstNnieCfg->u32MaxInputNum;
    pstCnnSoftWarePara->stGetTopN.unShape.stWhc.u32Chn = 1;
    pstCnnSoftWarePara->stGetTopN.unShape.stWhc.u32Height = 1;
    pstCnnSoftWarePara->stGetTopN.unShape.stWhc.u32Width = u32GetTopNPerFrameSize/sizeof(HI_U32);
    pstCnnSoftWarePara->stGetTopN.u32Stride = SAMPLE_SVP_NNIE_ALIGN16(u32GetTopNPerFrameSize);
    pstCnnSoftWarePara->stGetTopN.u64PhyAddr = u64PhyAddr;
    pstCnnSoftWarePara->stGetTopN.u64VirAddr = (HI_U64)(HI_UL)pu8VirAddr;

    /*init AssistBuf */
    pstCnnSoftWarePara->stAssistBuf.u32Size = u32GetTopNAssistBufSize;
    pstCnnSoftWarePara->stAssistBuf.u64PhyAddr = u64PhyAddr+u32GetTopNMemSize;
    pstCnnSoftWarePara->stAssistBuf.u64VirAddr = (HI_U64)(HI_UL)pu8VirAddr+u32GetTopNMemSize;

    return s32Ret;
}

/******************************************************************************
* function : Cnn init
******************************************************************************/
static HI_S32 SAMPLE_SVP_NNIE_Cnn_ParamInit(SAMPLE_SVP_NNIE_CFG_S* pstNnieCfg,
    SAMPLE_SVP_NNIE_PARAM_S *pstCnnPara, SAMPLE_SVP_NNIE_CNN_SOFTWARE_PARAM_S* pstCnnSoftWarePara)
{
    HI_S32 s32Ret = HI_SUCCESS;
    /*init hardware para*/
    s32Ret = SAMPLE_COMM_SVP_NNIE_ParamInit(pstNnieCfg,pstCnnPara);
    SAMPLE_SVP_CHECK_EXPR_GOTO(HI_SUCCESS != s32Ret,INIT_FAIL_0,SAMPLE_SVP_ERR_LEVEL_ERROR,
        "Error(%#x),SAMPLE_COMM_SVP_NNIE_ParamInit failed!\n",s32Ret);

    /*init software para*/
    if(pstCnnSoftWarePara!=NULL)
    {
        s32Ret = SAMPLE_SVP_NNIE_Cnn_SoftwareParaInit(pstNnieCfg,pstCnnPara,pstCnnSoftWarePara);
        SAMPLE_SVP_CHECK_EXPR_GOTO(HI_SUCCESS != s32Ret,INIT_FAIL_0,SAMPLE_SVP_ERR_LEVEL_ERROR,
            "Error(%#x),SAMPLE_SVP_NNIE_Cnn_SoftwareParaInit failed!\n",s32Ret);
    }

    return s32Ret;
INIT_FAIL_0:
    s32Ret = SAMPLE_SVP_NNIE_Cnn_Deinit(pstCnnPara,pstCnnSoftWarePara,NULL);
    SAMPLE_SVP_CHECK_EXPR_RET(HI_SUCCESS != s32Ret,s32Ret,SAMPLE_SVP_ERR_LEVEL_ERROR,
            "Error(%#x),SAMPLE_SVP_NNIE_Cnn_Deinit failed!\n",s32Ret);
    return HI_FAILURE;

}

/******************************************************************************
* function : Cnn process
******************************************************************************/
HI_S32 SAMPLE_SVP_NNIE_Cnn_PrintResult(SVP_BLOB_S *pstGetTopN,HI_U32 u32TopN)
{
    HI_U32 i = 0, j = 0;
    HI_U32 *pu32Tmp = NULL;
    HI_U32 u32Stride = pstGetTopN->u32Stride;
    SAMPLE_SVP_CHECK_EXPR_RET(NULL == pstGetTopN,HI_INVALID_VALUE,SAMPLE_SVP_ERR_LEVEL_ERROR,
        "Error,pstGetTopN can't be NULL!\n");
    for(j = 0; j < pstGetTopN->u32Num; j++)
    {
        SAMPLE_SVP_TRACE_INFO("==== The %dth image info====\n",j);
        pu32Tmp = (HI_U32*)((HI_UL)pstGetTopN->u64VirAddr + j * u32Stride);
        for(i = 0; i < u32TopN * 2; i+= 2)
        {
            SAMPLE_SVP_TRACE_INFO("%d:%d\n",pu32Tmp[i],pu32Tmp[i+1]);
        }
    }
    return HI_SUCCESS;
}

/******************************************************************************
* function : show Cnn sample(image 28x28 U8_C1)
******************************************************************************/
void SAMPLE_SVP_NNIE_Cnn(void)
{
    HI_CHAR *pcSrcFile = "./data/nnie_image/y/0_28x28.y";
    HI_CHAR *pcModelName = "./data/nnie_model/classification/inst_mnist_cycle.wk";
    HI_U32 u32PicNum = 1;
    HI_S32 s32Ret = HI_SUCCESS;
    SAMPLE_SVP_NNIE_CFG_S   stNnieCfg = {0};
    SAMPLE_SVP_NNIE_INPUT_DATA_INDEX_S stInputDataIdx = {0};
    SAMPLE_SVP_NNIE_PROCESS_SEG_INDEX_S stProcSegIdx = {0};

    /*Set configuration parameter*/
    stNnieCfg.pszPic= pcSrcFile;
    stNnieCfg.u32MaxInputNum = u32PicNum; //max input image num in each batch
    stNnieCfg.u32MaxRoiNum = 0;
    stNnieCfg.aenNnieCoreId[0] = SVP_NNIE_ID_0;//set NNIE core
    s_stCnnSoftwareParam.u32TopN = 5;

    /*Sys init*/
    SAMPLE_COMM_SVP_CheckSysInit();

    /*CNN Load model*/
    SAMPLE_SVP_TRACE_INFO("Cnn Load model!\n");
    s32Ret = SAMPLE_COMM_SVP_NNIE_LoadModel(pcModelName,&s_stCnnModel);
    SAMPLE_SVP_CHECK_EXPR_GOTO(HI_SUCCESS != s32Ret,CNN_FAIL_0,SAMPLE_SVP_ERR_LEVEL_ERROR,
        "Error,SAMPLE_COMM_SVP_NNIE_LoadModel failed!\n");

    /*CNN parameter initialization*/
    /*Cnn software parameters are set in SAMPLE_SVP_NNIE_Cnn_SoftwareParaInit,
     if user has changed net struct, please make sure the parameter settings in
     SAMPLE_SVP_NNIE_Cnn_SoftwareParaInit function are correct*/
    SAMPLE_SVP_TRACE_INFO("Cnn parameter initialization!\n");
    s_stCnnNnieParam.pstModel = &s_stCnnModel.stModel;
    s32Ret = SAMPLE_SVP_NNIE_Cnn_ParamInit(&stNnieCfg,&s_stCnnNnieParam,&s_stCnnSoftwareParam);
    SAMPLE_SVP_CHECK_EXPR_GOTO(HI_SUCCESS != s32Ret,CNN_FAIL_0,SAMPLE_SVP_ERR_LEVEL_ERROR,
        "Error,SAMPLE_SVP_NNIE_Cnn_ParamInit failed!\n");

    /*record tskBuf*/
    s32Ret = HI_MPI_SVP_NNIE_AddTskBuf(&(s_stCnnNnieParam.astForwardCtrl[0].stTskBuf));
    SAMPLE_SVP_CHECK_EXPR_GOTO(HI_SUCCESS != s32Ret,CNN_FAIL_0,SAMPLE_SVP_ERR_LEVEL_ERROR,
        "Error,HI_MPI_SVP_NNIE_AddTskBuf failed!\n");

    /*Fill src data*/
    SAMPLE_SVP_TRACE_INFO("Cnn start!\n");
    stInputDataIdx.u32SegIdx = 0;
    stInputDataIdx.u32NodeIdx = 0;
    s32Ret = SAMPLE_SVP_NNIE_FillSrcData(&stNnieCfg,&s_stCnnNnieParam,&stInputDataIdx);
    SAMPLE_SVP_CHECK_EXPR_GOTO(HI_SUCCESS != s32Ret,CNN_FAIL_1,SAMPLE_SVP_ERR_LEVEL_ERROR,
        "Error,SAMPLE_SVP_NNIE_FillSrcData failed!\n");

    /*NNIE process(process the 0-th segment)*/
    stProcSegIdx.u32SegIdx = 0;
    s32Ret = SAMPLE_SVP_NNIE_Forward(&s_stCnnNnieParam,&stInputDataIdx,&stProcSegIdx,HI_TRUE);
    SAMPLE_SVP_CHECK_EXPR_GOTO(HI_SUCCESS != s32Ret,CNN_FAIL_1,SAMPLE_SVP_ERR_LEVEL_ERROR,
        "Error,SAMPLE_SVP_NNIE_Forward failed!\n");

    /*Software process*/
    /*if user has changed net struct, please make sure SAMPLE_SVP_NNIE_Cnn_GetTopN
     function's input datas are correct*/
    s32Ret = SAMPLE_SVP_NNIE_Cnn_GetTopN(&s_stCnnNnieParam,&s_stCnnSoftwareParam);
    SAMPLE_SVP_CHECK_EXPR_GOTO(HI_SUCCESS != s32Ret,CNN_FAIL_1,SAMPLE_SVP_ERR_LEVEL_ERROR,
        "Error,SAMPLE_SVP_NNIE_CnnGetTopN failed!\n");

    /*Print result*/
    SAMPLE_SVP_TRACE_INFO("Cnn result:\n");
    s32Ret = SAMPLE_SVP_NNIE_Cnn_PrintResult(&(s_stCnnSoftwareParam.stGetTopN),
        s_stCnnSoftwareParam.u32TopN);
    SAMPLE_SVP_CHECK_EXPR_GOTO(HI_SUCCESS != s32Ret,CNN_FAIL_1,SAMPLE_SVP_ERR_LEVEL_ERROR,
        "Error,SAMPLE_SVP_NNIE_Cnn_PrintResult failed!\n");

CNN_FAIL_1:
    /*Remove TskBuf*/
    s32Ret = HI_MPI_SVP_NNIE_RemoveTskBuf(&(s_stCnnNnieParam.astForwardCtrl[0].stTskBuf));
    SAMPLE_SVP_CHECK_EXPR_GOTO(HI_SUCCESS != s32Ret,CNN_FAIL_0,SAMPLE_SVP_ERR_LEVEL_ERROR,
        "Error,HI_MPI_SVP_NNIE_RemoveTskBuf failed!\n");
CNN_FAIL_0:
    SAMPLE_SVP_NNIE_Cnn_Deinit(&s_stCnnNnieParam,&s_stCnnSoftwareParam,&s_stCnnModel);
    SAMPLE_COMM_SVP_CheckSysExit();
}

/******************************************************************************
* function : Cnn sample signal handle
******************************************************************************/
void SAMPLE_SVP_NNIE_Cnn_HandleSig(void)
{
    SAMPLE_SVP_NNIE_Cnn_Deinit(&s_stCnnNnieParam,&s_stCnnSoftwareParam,&s_stCnnModel);
    memset(&s_stCnnNnieParam,0,sizeof(SAMPLE_SVP_NNIE_PARAM_S));
    memset(&s_stCnnSoftwareParam,0,sizeof(SAMPLE_SVP_NNIE_CNN_SOFTWARE_PARAM_S));
    memset(&s_stCnnModel,0,sizeof(SAMPLE_SVP_NNIE_MODEL_S));
    SAMPLE_COMM_SVP_CheckSysExit();
}


/******************************************************************************
* function : show Segnet sample(image 224x224 U8_C3)
******************************************************************************/
void SAMPLE_SVP_NNIE_Segnet(void)
{
    HI_CHAR *pcSrcFile = "./data/nnie_image/rgb_planar/segnet_image_224x224.bgr";
    HI_CHAR *pcModelName = "./data/nnie_model/segmentation/inst_segnet_cycle.wk";
    HI_U32 u32PicNum = 1;
    HI_S32 s32Ret = HI_SUCCESS;
    SAMPLE_SVP_NNIE_CFG_S   stNnieCfg = {0};
    SAMPLE_SVP_NNIE_INPUT_DATA_INDEX_S stInputDataIdx = {0};
    SAMPLE_SVP_NNIE_PROCESS_SEG_INDEX_S stProcSegIdx = {0};

    /*Set configuration parameter*/
    stNnieCfg.pszPic= pcSrcFile;
    stNnieCfg.u32MaxInputNum = u32PicNum; //max input image num in each batch
    stNnieCfg.u32MaxRoiNum = 0;
    stNnieCfg.aenNnieCoreId[0] = SVP_NNIE_ID_0;

    /*Sys init*/
    SAMPLE_COMM_SVP_CheckSysInit();

    /*Segnet Load model*/
    SAMPLE_SVP_TRACE_INFO("Segnet Load model!\n");
    s32Ret = SAMPLE_COMM_SVP_NNIE_LoadModel(pcModelName,&s_stSegnetModel);
    SAMPLE_SVP_CHECK_EXPR_GOTO(HI_SUCCESS != s32Ret,SEGNET_FAIL_0,SAMPLE_SVP_ERR_LEVEL_ERROR,
        "Error,SAMPLE_COMM_SVP_NNIE_LoadModel failed!\n");

    /*Segnet parameter initialization*/
    SAMPLE_SVP_TRACE_INFO("Segnet parameter initialization!\n");
    s_stSegnetNnieParam.pstModel = &s_stSegnetModel.stModel;
    s32Ret = SAMPLE_SVP_NNIE_Cnn_ParamInit(&stNnieCfg,&s_stSegnetNnieParam,NULL);
    SAMPLE_SVP_CHECK_EXPR_GOTO(HI_SUCCESS != s32Ret,SEGNET_FAIL_0,SAMPLE_SVP_ERR_LEVEL_ERROR,
        "Error,SAMPLE_SVP_NNIE_Cnn_ParamInit failed!\n");

    /*Fill src data*/
    SAMPLE_SVP_TRACE_INFO("Segnet start!\n");
    stInputDataIdx.u32SegIdx = 0;
    stInputDataIdx.u32NodeIdx = 0;
    s32Ret = SAMPLE_SVP_NNIE_FillSrcData(&stNnieCfg,&s_stSegnetNnieParam,&stInputDataIdx);
    SAMPLE_SVP_CHECK_EXPR_GOTO(HI_SUCCESS != s32Ret,SEGNET_FAIL_0,SAMPLE_SVP_ERR_LEVEL_ERROR,
        "Error,SAMPLE_SVP_NNIE_FillSrcData failed!\n");

    /*NNIE process(process the 0-th segment)*/
    stProcSegIdx.u32SegIdx = 0;
    s32Ret = SAMPLE_SVP_NNIE_Forward(&s_stSegnetNnieParam,&stInputDataIdx,&stProcSegIdx,HI_TRUE);
    SAMPLE_SVP_CHECK_EXPR_GOTO(HI_SUCCESS != s32Ret,SEGNET_FAIL_0,SAMPLE_SVP_ERR_LEVEL_ERROR,
        "Error,SAMPLE_SVP_NNIE_Forward failed!\n");

    /*print report result*/
    s32Ret = SAMPLE_SVP_NNIE_PrintReportResult(&s_stSegnetNnieParam);
    SAMPLE_SVP_CHECK_EXPR_GOTO(HI_SUCCESS != s32Ret,SEGNET_FAIL_0,SAMPLE_SVP_ERR_LEVEL_ERROR,
        "Error,SAMPLE_SVP_NNIE_PrintReportResult failed!\n");

    SAMPLE_SVP_TRACE_INFO("Segnet is successfully processed!\n");

SEGNET_FAIL_0:
    SAMPLE_SVP_NNIE_Cnn_Deinit(&s_stSegnetNnieParam,NULL,&s_stSegnetModel);
    SAMPLE_COMM_SVP_CheckSysExit();
}

/******************************************************************************
* function : Segnet sample signal handle
******************************************************************************/
void SAMPLE_SVP_NNIE_Segnet_HandleSig(void)
{
    SAMPLE_SVP_NNIE_Cnn_Deinit(&s_stSegnetNnieParam,NULL,&s_stSegnetModel);
    memset(&s_stSegnetNnieParam,0,sizeof(SAMPLE_SVP_NNIE_PARAM_S));
    memset(&s_stSegnetModel,0,sizeof(SAMPLE_SVP_NNIE_MODEL_S));
    SAMPLE_COMM_SVP_CheckSysExit();
}

/******************************************************************************
* function : print detection result
******************************************************************************/
static HI_S32 SAMPLE_SVP_NNIE_Detection_PrintResult(SVP_BLOB_S *pstDstScore,
    SVP_BLOB_S *pstDstRoi, SVP_BLOB_S *pstClassRoiNum, HI_FLOAT f32PrintResultThresh)
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
    HI_S32 s32XMin = 0,s32YMin= 0,s32XMax = 0,s32YMax = 0;

    u32RoiNumBias += ps32ClassRoiNum[0];
    for (i = 1; i < u32ClassNum; i++)
    {
        u32ScoreBias = u32RoiNumBias;
        u32BboxBias = u32RoiNumBias * SAMPLE_SVP_NNIE_COORDI_NUM;
        /*if the confidence score greater than result threshold, the result will be printed*/
        if((HI_FLOAT)ps32Score[u32ScoreBias] / SAMPLE_SVP_NNIE_QUANT_BASE >=
            f32PrintResultThresh && ps32ClassRoiNum[i]!=0)
        {
            SAMPLE_SVP_TRACE_INFO("==== The %dth class box info====\n", i);
        }
        for (j = 0; j < (HI_U32)ps32ClassRoiNum[i]; j++)
        {
            f32Score = (HI_FLOAT)ps32Score[u32ScoreBias + j] / SAMPLE_SVP_NNIE_QUANT_BASE;
            if (f32Score < f32PrintResultThresh)
            {
                break;
            }
            s32XMin = ps32Roi[u32BboxBias + j*SAMPLE_SVP_NNIE_COORDI_NUM];
            s32YMin = ps32Roi[u32BboxBias + j*SAMPLE_SVP_NNIE_COORDI_NUM + 1];
            s32XMax = ps32Roi[u32BboxBias + j*SAMPLE_SVP_NNIE_COORDI_NUM + 2];
            s32YMax = ps32Roi[u32BboxBias + j*SAMPLE_SVP_NNIE_COORDI_NUM + 3];
            SAMPLE_SVP_TRACE_INFO("%d %d %d %d %f\n", s32XMin, s32YMin, s32XMax, s32YMax, f32Score);
        }
        u32RoiNumBias += ps32ClassRoiNum[i];
    }
    return HI_SUCCESS;
}


/******************************************************************************
* function : FasterRcnn software deinit
******************************************************************************/
static HI_S32 SAMPLE_SVP_NNIE_FasterRcnn_SoftwareDeinit(SAMPLE_SVP_NNIE_FASTERRCNN_SOFTWARE_PARAM_S* pstSoftWareParam)
{
    HI_S32 s32Ret = HI_SUCCESS;
    SAMPLE_SVP_CHECK_EXPR_RET(NULL == pstSoftWareParam,HI_INVALID_VALUE,SAMPLE_SVP_ERR_LEVEL_ERROR,
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
* function : FasterRcnn Deinit
******************************************************************************/
static HI_S32 SAMPLE_SVP_NNIE_FasterRcnn_Deinit(SAMPLE_SVP_NNIE_PARAM_S *pstNnieParam,
    SAMPLE_SVP_NNIE_FASTERRCNN_SOFTWARE_PARAM_S* pstSoftWareParam,SAMPLE_SVP_NNIE_MODEL_S* pstNnieModel)
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
        s32Ret = SAMPLE_SVP_NNIE_FasterRcnn_SoftwareDeinit(pstSoftWareParam);
        SAMPLE_SVP_CHECK_EXPR_TRACE(HI_SUCCESS != s32Ret,SAMPLE_SVP_ERR_LEVEL_ERROR,
            "Error,SAMPLE_SVP_NNIE_FasterRcnn_SoftwareDeinit failed!\n");
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
* function : FasterRcnn software para init
******************************************************************************/
static HI_S32 SAMPLE_SVP_NNIE_FasterRcnn_SoftwareInit(SAMPLE_SVP_NNIE_CFG_S* pstCfg,
    SAMPLE_SVP_NNIE_PARAM_S *pstNnieParam, SAMPLE_SVP_NNIE_FASTERRCNN_SOFTWARE_PARAM_S* pstSoftWareParam)
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

    /*RPN parameter init*/
    pstSoftWareParam->u32MaxRoiNum = pstCfg->u32MaxRoiNum;
    if(SAMPLE_SVP_NNIE_VGG16_FASTER_RCNN == s_enNetType)
    {
        pstSoftWareParam->u32ClassNum = 4;
        pstSoftWareParam->u32NumRatioAnchors = 3;
        pstSoftWareParam->u32NumScaleAnchors = 3;
        pstSoftWareParam->au32Scales[0] = 8 * SAMPLE_SVP_QUANT_BASE;
        pstSoftWareParam->au32Scales[1] = 16 * SAMPLE_SVP_QUANT_BASE;
        pstSoftWareParam->au32Scales[2] = 32 * SAMPLE_SVP_QUANT_BASE;
        pstSoftWareParam->au32Ratios[0] = 0.5 * SAMPLE_SVP_QUANT_BASE;
        pstSoftWareParam->au32Ratios[1] = 1 * SAMPLE_SVP_QUANT_BASE;
        pstSoftWareParam->au32Ratios[2] = 2 * SAMPLE_SVP_QUANT_BASE;
    }
    else
    {
        pstSoftWareParam->u32ClassNum = 2;
        pstSoftWareParam->u32NumRatioAnchors = 1;
        pstSoftWareParam->u32NumScaleAnchors = 9;
        pstSoftWareParam->au32Scales[0] = 1.5 * SAMPLE_SVP_QUANT_BASE;
        pstSoftWareParam->au32Scales[1] = 2.1 * SAMPLE_SVP_QUANT_BASE;
        pstSoftWareParam->au32Scales[2] = 2.9 * SAMPLE_SVP_QUANT_BASE;
        pstSoftWareParam->au32Scales[3] = 4.1 * SAMPLE_SVP_QUANT_BASE;
        pstSoftWareParam->au32Scales[4] = 5.8 * SAMPLE_SVP_QUANT_BASE;
        pstSoftWareParam->au32Scales[5] = 8.0 * SAMPLE_SVP_QUANT_BASE;
        pstSoftWareParam->au32Scales[6] = 11.3 * SAMPLE_SVP_QUANT_BASE;
        pstSoftWareParam->au32Scales[7] = 15.8 * SAMPLE_SVP_QUANT_BASE;
        pstSoftWareParam->au32Scales[8] = 22.1 * SAMPLE_SVP_QUANT_BASE;
        pstSoftWareParam->au32Ratios[0] = 2.44 * SAMPLE_SVP_QUANT_BASE;
    }

    pstSoftWareParam->u32OriImHeight = pstNnieParam->astSegData[0].astSrc[0].unShape.stWhc.u32Height;
    pstSoftWareParam->u32OriImWidth = pstNnieParam->astSegData[0].astSrc[0].unShape.stWhc.u32Width;
    pstSoftWareParam->u32MinSize = 16;
    pstSoftWareParam->u32FilterThresh = 16;
    pstSoftWareParam->u32SpatialScale = (HI_U32)(0.0625 * SAMPLE_SVP_QUANT_BASE);
    pstSoftWareParam->u32NmsThresh = (HI_U32)(0.7 * SAMPLE_SVP_QUANT_BASE);
    pstSoftWareParam->u32FilterThresh = 0;
    pstSoftWareParam->u32NumBeforeNms = 6000;
    for(i = 0; i < pstSoftWareParam->u32ClassNum; i++)
    {
        pstSoftWareParam->au32ConfThresh[i] = 1;
    }
    pstSoftWareParam->u32ValidNmsThresh = (HI_U32)(0.3 * SAMPLE_SVP_QUANT_BASE);
    pstSoftWareParam->stRpnBbox.enType = SVP_BLOB_TYPE_S32;
    pstSoftWareParam->stRpnBbox.unShape.stWhc.u32Chn = 1;
    pstSoftWareParam->stRpnBbox.unShape.stWhc.u32Height = pstCfg->u32MaxRoiNum;
    pstSoftWareParam->stRpnBbox.unShape.stWhc.u32Width = SAMPLE_SVP_COORDI_NUM;
    pstSoftWareParam->stRpnBbox.u32Stride = SAMPLE_SVP_NNIE_ALIGN16(SAMPLE_SVP_COORDI_NUM*sizeof(HI_U32));
    pstSoftWareParam->stRpnBbox.u32Num = 1;
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

    /*calculate software mem size*/
    u32ClassNum = pstSoftWareParam->u32ClassNum;
    u32RpnTmpBufSize = SAMPLE_SVP_NNIE_RpnTmpBufSize(pstSoftWareParam->u32NumRatioAnchors,
        pstSoftWareParam->u32NumScaleAnchors,pstSoftWareParam->au32ConvHeight[0],
        pstSoftWareParam->au32ConvWidth[0]);
    u32RpnTmpBufSize = SAMPLE_SVP_NNIE_ALIGN16(u32RpnTmpBufSize);
    u32RpnBboxBufSize = pstSoftWareParam->stRpnBbox.u32Num*
        pstSoftWareParam->stRpnBbox.unShape.stWhc.u32Height*pstSoftWareParam->stRpnBbox.u32Stride;
    u32GetResultTmpBufSize = SAMPLE_SVP_NNIE_FasterRcnn_GetResultTmpBufSize(pstCfg->u32MaxRoiNum,u32ClassNum);
    u32GetResultTmpBufSize = SAMPLE_SVP_NNIE_ALIGN16(u32GetResultTmpBufSize);
    u32DstRoiSize = SAMPLE_SVP_NNIE_ALIGN16(u32ClassNum*pstCfg->u32MaxRoiNum*sizeof(HI_U32)*SAMPLE_SVP_COORDI_NUM);
    u32DstScoreSize = SAMPLE_SVP_NNIE_ALIGN16(u32ClassNum*pstCfg->u32MaxRoiNum*sizeof(HI_U32));
    u32ClassRoiNumSize = SAMPLE_SVP_NNIE_ALIGN16(u32ClassNum*sizeof(HI_U32));
    u32TotalSize = u32RpnTmpBufSize + u32RpnBboxBufSize + u32GetResultTmpBufSize + u32DstRoiSize +
        u32DstScoreSize + u32ClassRoiNumSize;

    /*malloc mem*/
    s32Ret = SAMPLE_COMM_SVP_MallocCached("SAMPLE_RCNN_INIT",NULL,(HI_U64*)&u64PhyAddr,
        (void**)&pu8VirAddr,u32TotalSize);
    SAMPLE_SVP_CHECK_EXPR_RET(HI_SUCCESS != s32Ret,s32Ret,SAMPLE_SVP_ERR_LEVEL_ERROR,
        "Error,Malloc memory failed!\n");
    memset(pu8VirAddr,0, u32TotalSize);
    SAMPLE_COMM_SVP_FlushCache(u64PhyAddr,(void*)pu8VirAddr,u32TotalSize);

    /*set addr*/
    pstSoftWareParam->stRpnTmpBuf.u64PhyAddr = u64PhyAddr;
    pstSoftWareParam->stRpnTmpBuf.u64VirAddr = (HI_U64)((HI_UL)pu8VirAddr);
    pstSoftWareParam->stRpnTmpBuf.u32Size = u32RpnTmpBufSize;

    pstSoftWareParam->stRpnBbox.u64PhyAddr = u64PhyAddr+u32RpnTmpBufSize;
    pstSoftWareParam->stRpnBbox.u64VirAddr = (HI_U64)((HI_UL)pu8VirAddr)+u32RpnTmpBufSize;

    pstSoftWareParam->stGetResultTmpBuf.u64PhyAddr = u64PhyAddr+u32RpnBboxBufSize+u32RpnTmpBufSize;
    pstSoftWareParam->stGetResultTmpBuf.u64VirAddr = (HI_U64)((HI_UL)pu8VirAddr+u32RpnBboxBufSize+u32RpnTmpBufSize);
    pstSoftWareParam->stGetResultTmpBuf.u32Size = u32GetResultTmpBufSize;

    pstSoftWareParam->stDstRoi.enType = SVP_BLOB_TYPE_S32;
    pstSoftWareParam->stDstRoi.u64PhyAddr = u64PhyAddr+u32RpnBboxBufSize+u32RpnTmpBufSize+u32GetResultTmpBufSize;
    pstSoftWareParam->stDstRoi.u64VirAddr = (HI_U64)((HI_UL)pu8VirAddr+u32RpnBboxBufSize+u32RpnTmpBufSize+u32GetResultTmpBufSize);
    pstSoftWareParam->stDstRoi.u32Stride = SAMPLE_SVP_NNIE_ALIGN16(u32ClassNum*pstSoftWareParam->u32MaxRoiNum*sizeof(HI_U32)*SAMPLE_SVP_COORDI_NUM);
    pstSoftWareParam->stDstRoi.u32Num = 1;
    pstSoftWareParam->stDstRoi.unShape.stWhc.u32Chn = 1;
    pstSoftWareParam->stDstRoi.unShape.stWhc.u32Height = 1;
    pstSoftWareParam->stDstRoi.unShape.stWhc.u32Width = u32ClassNum*pstSoftWareParam->u32MaxRoiNum*SAMPLE_SVP_COORDI_NUM;

    pstSoftWareParam->stDstScore.enType = SVP_BLOB_TYPE_S32;
    pstSoftWareParam->stDstScore.u64PhyAddr = u64PhyAddr+u32RpnBboxBufSize+u32RpnTmpBufSize+u32GetResultTmpBufSize+u32DstRoiSize;
    pstSoftWareParam->stDstScore.u64VirAddr = (HI_U64)((HI_UL)pu8VirAddr+u32RpnBboxBufSize+u32RpnTmpBufSize+u32GetResultTmpBufSize+u32DstRoiSize);
    pstSoftWareParam->stDstScore.u32Stride = SAMPLE_SVP_NNIE_ALIGN16(u32ClassNum*pstSoftWareParam->u32MaxRoiNum*sizeof(HI_U32));
    pstSoftWareParam->stDstScore.u32Num = 1;
    pstSoftWareParam->stDstScore.unShape.stWhc.u32Chn = 1;
    pstSoftWareParam->stDstScore.unShape.stWhc.u32Height = 1;
    pstSoftWareParam->stDstScore.unShape.stWhc.u32Width = u32ClassNum*pstSoftWareParam->u32MaxRoiNum;

    pstSoftWareParam->stClassRoiNum.enType = SVP_BLOB_TYPE_S32;
    pstSoftWareParam->stClassRoiNum.u64PhyAddr = u64PhyAddr+u32RpnBboxBufSize+u32RpnTmpBufSize+u32GetResultTmpBufSize+u32DstRoiSize+u32DstScoreSize;
    pstSoftWareParam->stClassRoiNum.u64VirAddr = (HI_U64)((HI_UL)pu8VirAddr+u32RpnBboxBufSize+u32RpnTmpBufSize+u32GetResultTmpBufSize+u32DstRoiSize+u32DstScoreSize);
    pstSoftWareParam->stClassRoiNum.u32Stride = SAMPLE_SVP_NNIE_ALIGN16(u32ClassNum*sizeof(HI_U32));
    pstSoftWareParam->stClassRoiNum.u32Num = 1;
    pstSoftWareParam->stClassRoiNum.unShape.stWhc.u32Chn = 1;
    pstSoftWareParam->stClassRoiNum.unShape.stWhc.u32Height = 1;
    pstSoftWareParam->stClassRoiNum.unShape.stWhc.u32Width = u32ClassNum;

    return s32Ret;
}

/******************************************************************************
* function : FasterRcnn parameter initialization
******************************************************************************/
static HI_S32 SAMPLE_SVP_NNIE_FasterRcnn_ParamInit(SAMPLE_SVP_NNIE_CFG_S* pstFasterRcnnCfg,
    SAMPLE_SVP_NNIE_PARAM_S *pstNnieParam, SAMPLE_SVP_NNIE_FASTERRCNN_SOFTWARE_PARAM_S* pstSoftWareParam)
{
    HI_S32 s32Ret = HI_SUCCESS;
    /*init hardware parameter*/
    s32Ret = SAMPLE_COMM_SVP_NNIE_ParamInit(pstFasterRcnnCfg,pstNnieParam);
    SAMPLE_SVP_CHECK_EXPR_GOTO(HI_SUCCESS != s32Ret,INIT_FAIL_0,SAMPLE_SVP_ERR_LEVEL_ERROR,
        "Error(%#x),SAMPLE_COMM_SVP_NNIE_ParamInit failed!\n",s32Ret);

    /*init software parameter*/
    s32Ret = SAMPLE_SVP_NNIE_FasterRcnn_SoftwareInit(pstFasterRcnnCfg,pstNnieParam,
        pstSoftWareParam);
    SAMPLE_SVP_CHECK_EXPR_GOTO(HI_SUCCESS != s32Ret,INIT_FAIL_0,SAMPLE_SVP_ERR_LEVEL_ERROR,
        "Error(%#x),SAMPLE_SVP_NNIE_FasterRcnn_SoftwareInit failed!\n",s32Ret);

    return s32Ret;
INIT_FAIL_0:
    s32Ret = SAMPLE_SVP_NNIE_FasterRcnn_Deinit(pstNnieParam,pstSoftWareParam,NULL);
    SAMPLE_SVP_CHECK_EXPR_RET(HI_SUCCESS != s32Ret,s32Ret,SAMPLE_SVP_ERR_LEVEL_ERROR,
            "Error(%#x),SAMPLE_SVP_NNIE_FasterRcnn_Deinit failed!\n",s32Ret);
    return HI_FAILURE;

}

/******************************************************************************
* function : show fasterRcnn sample(image 1240x375 U8_C3)
******************************************************************************/
void SAMPLE_SVP_NNIE_FasterRcnn(void)
{
    HI_CHAR *pcSrcFile = "./data/nnie_image/rgb_planar/single_person_1240x375.bgr";
    HI_CHAR *pcModelName = "./data/nnie_model/detection/inst_alexnet_frcnn_cycle.wk";
    HI_U32 u32PicNum = 1;
    HI_FLOAT f32PrintResultThresh = 0.0f;
    HI_S32 s32Ret = HI_SUCCESS;
    HI_U32 i = 0;
    SAMPLE_SVP_NNIE_CFG_S   stNnieCfg = {0};
    SAMPLE_SVP_NNIE_INPUT_DATA_INDEX_S stInputDataIdx = {0};
    SAMPLE_SVP_NNIE_PROCESS_SEG_INDEX_S stProcSegIdx = {0};

    /*Set configuration parameter*/
    s_enNetType = SAMPLE_SVP_NNIE_ALEXNET_FASTER_RCNN;
    f32PrintResultThresh = 0.8f;
    stNnieCfg.pszPic= pcSrcFile;
    stNnieCfg.u32MaxInputNum = u32PicNum; //max input image num in each batch
    stNnieCfg.u32MaxRoiNum = 300;
    stNnieCfg.aenNnieCoreId[0] = SVP_NNIE_ID_0; //set NNIE core for 0-th Seg
    stNnieCfg.aenNnieCoreId[1] = SVP_NNIE_ID_0; //set NNIE core for 1-th Seg

    /*Sys init*/
    SAMPLE_COMM_SVP_CheckSysInit();

    /*FasterRcnn Load model*/
    SAMPLE_SVP_TRACE_INFO("FasterRcnn Load model!\n");
    s32Ret = SAMPLE_COMM_SVP_NNIE_LoadModel(pcModelName,&s_stFasterRcnnModel);
    SAMPLE_SVP_CHECK_EXPR_GOTO(HI_SUCCESS != s32Ret,FRCNN_FAIL_0,SAMPLE_SVP_ERR_LEVEL_ERROR,
        "Error,SAMPLE_COMM_SVP_NNIE_LoadModel failed!\n");

    /*FasterRcnn para init*/
    /*apcRpnDataLayerName is used to set RPN data layer name
      and search RPN input data,if user has changed network struct, please
      make sure the data layer names are correct*/
    /*FasterRcnn parameters are set in SAMPLE_SVP_NNIE_FasterRcnn_SoftwareInit,
     if user has changed network struct, please make sure the parameter settings in
     SAMPLE_SVP_NNIE_FasterRcnn_SoftwareInit function are correct*/
    SAMPLE_SVP_TRACE_INFO("FasterRcnn parameter initialization!\n");
    s_stFasterRcnnNnieParam.pstModel = &s_stFasterRcnnModel.stModel;
    s_stFasterRcnnSoftwareParam.apcRpnDataLayerName[0] = "rpn_cls_score";
    s_stFasterRcnnSoftwareParam.apcRpnDataLayerName[1] = "rpn_bbox_pred";
    s32Ret = SAMPLE_SVP_NNIE_FasterRcnn_ParamInit(&stNnieCfg,&s_stFasterRcnnNnieParam,
        &s_stFasterRcnnSoftwareParam);
    SAMPLE_SVP_CHECK_EXPR_GOTO(HI_SUCCESS != s32Ret,FRCNN_FAIL_0,SAMPLE_SVP_ERR_LEVEL_ERROR,
        "Error,SAMPLE_SVP_NNIE_FasterRcnn_ParamInit failed!\n");

    /*Fill 0-th input node of 0-th seg*/
    SAMPLE_SVP_TRACE_INFO("FasterRcnn start!\n");
    stInputDataIdx.u32SegIdx = 0;
    stInputDataIdx.u32NodeIdx = 0;
    s32Ret = SAMPLE_SVP_NNIE_FillSrcData(&stNnieCfg,&s_stFasterRcnnNnieParam,&stInputDataIdx);
    SAMPLE_SVP_CHECK_EXPR_GOTO(HI_SUCCESS != s32Ret,FRCNN_FAIL_0,SAMPLE_SVP_ERR_LEVEL_ERROR,
        "Error,SAMPLE_SVP_NNIE_FillSrcData failed!\n");

    /*NNIE process 0-th seg*/
    stProcSegIdx.u32SegIdx = 0;
    s32Ret = SAMPLE_SVP_NNIE_Forward(&s_stFasterRcnnNnieParam,&stInputDataIdx,&stProcSegIdx,HI_TRUE);
    SAMPLE_SVP_CHECK_EXPR_GOTO(HI_SUCCESS != s32Ret,FRCNN_FAIL_0,SAMPLE_SVP_ERR_LEVEL_ERROR,
        "Error,SAMPLE_SVP_NNIE_Forward failed!\n");

    /*RPN*/
    s32Ret = SAMPLE_SVP_NNIE_FasterRcnn_Rpn(&s_stFasterRcnnNnieParam,&s_stFasterRcnnSoftwareParam);
    SAMPLE_SVP_CHECK_EXPR_GOTO(HI_SUCCESS != s32Ret,FRCNN_FAIL_0,SAMPLE_SVP_ERR_LEVEL_ERROR,
        "Error,SAMPLE_SVP_NNIE_FasterRcnn_Rpn failed!\n");

    if(0 != s_stFasterRcnnSoftwareParam.stRpnBbox.unShape.stWhc.u32Height)
    {
        /*NNIE process 1-th seg, the input conv data comes from 0-th seg's 0-th report node,
         the input roi comes from RPN results*/
        stInputDataIdx.u32SegIdx = 0;
        stInputDataIdx.u32NodeIdx = 0;
        stProcSegIdx.u32SegIdx = 1;
        s32Ret = SAMPLE_SVP_NNIE_ForwardWithBbox(&s_stFasterRcnnNnieParam,&stInputDataIdx,
            &s_stFasterRcnnSoftwareParam.stRpnBbox,&stProcSegIdx,HI_TRUE);
        SAMPLE_SVP_CHECK_EXPR_GOTO(HI_SUCCESS != s32Ret,FRCNN_FAIL_0,SAMPLE_SVP_ERR_LEVEL_ERROR,
            "Error,SAMPLE_SVP_NNIE_Forward failed!\n");

        /*GetResult*/
        /*if user has changed net struct, please make sure SAMPLE_SVP_NNIE_FasterRcnn_GetResult
         function's input datas are correct*/
        s32Ret = SAMPLE_SVP_NNIE_FasterRcnn_GetResult(&s_stFasterRcnnNnieParam,&s_stFasterRcnnSoftwareParam);
        SAMPLE_SVP_CHECK_EXPR_GOTO(HI_SUCCESS != s32Ret,FRCNN_FAIL_0,SAMPLE_SVP_ERR_LEVEL_ERROR,
            "Error,SAMPLE_SVP_NNIE_FasterRcnn_GetResult failed!\n");
    }
    else
    {
        for (i = 0; i < s_stFasterRcnnSoftwareParam.stClassRoiNum.unShape.stWhc.u32Width; i++)
        {
            *(((HI_U32*)(HI_UL)s_stFasterRcnnSoftwareParam.stClassRoiNum.u64VirAddr)+i) = 0;
        }
    }
    /*print result, Alexnet_FasterRcnn has 2 classes:
     class 0:background     class 1:pedestrian */
    SAMPLE_SVP_TRACE_INFO("FasterRcnn result:\n");
    (void)SAMPLE_SVP_NNIE_Detection_PrintResult(&s_stFasterRcnnSoftwareParam.stDstScore,
        &s_stFasterRcnnSoftwareParam.stDstRoi, &s_stFasterRcnnSoftwareParam.stClassRoiNum,
        f32PrintResultThresh);

FRCNN_FAIL_0:
    SAMPLE_SVP_NNIE_FasterRcnn_Deinit(&s_stFasterRcnnNnieParam,&s_stFasterRcnnSoftwareParam,
        &s_stFasterRcnnModel);
    SAMPLE_COMM_SVP_CheckSysExit();
}

/******************************************************************************
* function :show fasterrcnn double_roipooling sample(image 224x224 U8_C3)
******************************************************************************/
void SAMPLE_SVP_NNIE_FasterRcnn_DoubleRoiPooling(void)
{
    HI_CHAR *pcSrcFile = "./data/nnie_image/rgb_planar/double_roipooling_224_224.bgr";
    HI_CHAR *pcModelName = "./data/nnie_model/detection/inst_fasterrcnn_double_roipooling_cycle.wk";
    HI_U32 u32PicNum = 1;
    HI_FLOAT f32PrintResultThresh = 0.0f;
    HI_S32 s32Ret = HI_SUCCESS;
    HI_U32 i = 0;
    SAMPLE_SVP_NNIE_CFG_S   stNnieCfg = {0};
    SAMPLE_SVP_NNIE_INPUT_DATA_INDEX_S stInputDataIdx = {0};
    SAMPLE_SVP_NNIE_PROCESS_SEG_INDEX_S stProcSegIdx = {0};

    /*Set configuration parameter*/
    s_enNetType = SAMPLE_SVP_NNIE_VGG16_FASTER_RCNN;
    f32PrintResultThresh = 0.8f;
    stNnieCfg.pszPic= pcSrcFile;
    stNnieCfg.u32MaxInputNum = u32PicNum; //max input image num in each batch
    stNnieCfg.u32MaxRoiNum = 300;
    stNnieCfg.aenNnieCoreId[0] = SVP_NNIE_ID_0; //set NNIE core for 0-th Seg
    stNnieCfg.aenNnieCoreId[1] = SVP_NNIE_ID_0; //set NNIE core for 1-th Seg

    /*Sys init*/
    SAMPLE_COMM_SVP_CheckSysInit();

    /*FasterRcnn Load model*/
    SAMPLE_SVP_TRACE_INFO("FasterRcnn Load model!\n");
    s32Ret = SAMPLE_COMM_SVP_NNIE_LoadModel(pcModelName,&s_stFasterRcnnModel);
    SAMPLE_SVP_CHECK_EXPR_GOTO(HI_SUCCESS != s32Ret,FRCNN_FAIL_0,SAMPLE_SVP_ERR_LEVEL_ERROR,
        "Error,SAMPLE_COMM_SVP_NNIE_LoadModel failed!\n");

    /*FasterRcnn para init*/
    /*apcRpnDataLayerName is used to set RPN data layer name
      and search RPN input data,if user has changed network struct, please
      make sure the data layer names are correct*/
    /*FasterRcnn parameters are set in SAMPLE_SVP_NNIE_FasterRcnn_SoftwareInit,
     if user has changed network struct, please make sure the parameter settings in
     SAMPLE_SVP_NNIE_FaasterRcnn_SoftwareInit function are correct*/
    SAMPLE_SVP_TRACE_INFO("FasterRcnn parameter initialization!\n");
    s_stFasterRcnnNnieParam.pstModel = &s_stFasterRcnnModel.stModel;
    s_stFasterRcnnSoftwareParam.apcRpnDataLayerName[0] = "rpn_cls_score";
    s_stFasterRcnnSoftwareParam.apcRpnDataLayerName[1] = "rpn_bbox_pred";
    s32Ret = SAMPLE_SVP_NNIE_FasterRcnn_ParamInit(&stNnieCfg,&s_stFasterRcnnNnieParam,
        &s_stFasterRcnnSoftwareParam);
    SAMPLE_SVP_CHECK_EXPR_GOTO(HI_SUCCESS != s32Ret,FRCNN_FAIL_0,SAMPLE_SVP_ERR_LEVEL_ERROR,
        "Error,SAMPLE_SVP_NNIE_FasterRcnn_ParamInit failed!\n");

    /*Fill 0-th input node of 0-th seg*/
    SAMPLE_SVP_TRACE_INFO("FasterRcnn start!\n");
    stInputDataIdx.u32SegIdx = 0;
    stInputDataIdx.u32NodeIdx = 0;
    s32Ret = SAMPLE_SVP_NNIE_FillSrcData(&stNnieCfg,&s_stFasterRcnnNnieParam,&stInputDataIdx);
    SAMPLE_SVP_CHECK_EXPR_GOTO(HI_SUCCESS != s32Ret,FRCNN_FAIL_0,SAMPLE_SVP_ERR_LEVEL_ERROR,
        "Error,SAMPLE_SVP_NNIE_FillSrcData failed!\n");

    /*NNIE process 0-th seg*/
    stProcSegIdx.u32SegIdx = 0;
    s32Ret = SAMPLE_SVP_NNIE_Forward(&s_stFasterRcnnNnieParam,&stInputDataIdx,&stProcSegIdx,HI_TRUE);
    SAMPLE_SVP_CHECK_EXPR_GOTO(HI_SUCCESS != s32Ret,FRCNN_FAIL_0,SAMPLE_SVP_ERR_LEVEL_ERROR,
        "Error,SAMPLE_SVP_NNIE_Forward failed!\n");

    /*RPN*/
    s32Ret = SAMPLE_SVP_NNIE_FasterRcnn_Rpn(&s_stFasterRcnnNnieParam,&s_stFasterRcnnSoftwareParam);
    SAMPLE_SVP_CHECK_EXPR_GOTO(HI_SUCCESS != s32Ret,FRCNN_FAIL_0,SAMPLE_SVP_ERR_LEVEL_ERROR,
        "Error,SAMPLE_SVP_NNIE_FasterRcnn_Rpn failed!\n");
    if(0 != s_stFasterRcnnSoftwareParam.stRpnBbox.unShape.stWhc.u32Height)
    {
        /*NNIE process 1-st seg, the input conv data comes from 0-th seg's 0-th and
          1-st report node,the input roi comes from RPN results*/
        stInputDataIdx.u32SegIdx = 0;
        stInputDataIdx.u32NodeIdx = 0;
        stProcSegIdx.u32SegIdx = 1;
        s32Ret = SAMPLE_SVP_NNIE_ForwardWithBbox(&s_stFasterRcnnNnieParam,&stInputDataIdx,
            &s_stFasterRcnnSoftwareParam.stRpnBbox,&stProcSegIdx,HI_TRUE);
        SAMPLE_SVP_CHECK_EXPR_GOTO(HI_SUCCESS != s32Ret,FRCNN_FAIL_0,SAMPLE_SVP_ERR_LEVEL_ERROR,
            "Error,SAMPLE_SVP_NNIE_Forward failed!\n");

        /*GetResult*/
        /*if user has changed net struct, please make sure SAMPLE_SVP_NNIE_FasterRcnn_GetResult
         function's input datas are correct*/
        s32Ret = SAMPLE_SVP_NNIE_FasterRcnn_GetResult(&s_stFasterRcnnNnieParam,&s_stFasterRcnnSoftwareParam);
        SAMPLE_SVP_CHECK_EXPR_GOTO(HI_SUCCESS != s32Ret,FRCNN_FAIL_0,SAMPLE_SVP_ERR_LEVEL_ERROR,
            "Error,SAMPLE_SVP_NNIE_FasterRcnn_GetResult failed!\n");
    }
    else
    {
        for (i = 0; i < s_stFasterRcnnSoftwareParam.stClassRoiNum.unShape.stWhc.u32Width; i++)
        {
            *(((HI_U32*)(HI_UL)s_stFasterRcnnSoftwareParam.stClassRoiNum.u64VirAddr)+i) = 0;
        }
    }
    /*print result, FasterRcnn has 4 classes:
     class 0:background  class 1:person  class 2:people  class 3:person sitting */
    SAMPLE_SVP_TRACE_INFO("FasterRcnn result:\n");
    (void)SAMPLE_SVP_NNIE_Detection_PrintResult(&s_stFasterRcnnSoftwareParam.stDstScore,
        &s_stFasterRcnnSoftwareParam.stDstRoi, &s_stFasterRcnnSoftwareParam.stClassRoiNum,
        f32PrintResultThresh);
FRCNN_FAIL_0:
    SAMPLE_SVP_NNIE_FasterRcnn_Deinit(&s_stFasterRcnnNnieParam,&s_stFasterRcnnSoftwareParam,
        &s_stFasterRcnnModel);
    SAMPLE_COMM_SVP_CheckSysExit();
}


/******************************************************************************
* function : fasterRcnn sample signal handle
******************************************************************************/
void SAMPLE_SVP_NNIE_FasterRcnn_HandleSig(void)
{
    SAMPLE_SVP_NNIE_FasterRcnn_Deinit(&s_stFasterRcnnNnieParam,&s_stFasterRcnnSoftwareParam,
        &s_stFasterRcnnModel);
    memset(&s_stFasterRcnnNnieParam,0,sizeof(SAMPLE_SVP_NNIE_PARAM_S));
    memset(&s_stFasterRcnnSoftwareParam,0,sizeof(SAMPLE_SVP_NNIE_FASTERRCNN_SOFTWARE_PARAM_S));
    memset(&s_stFasterRcnnModel,0,sizeof(SAMPLE_SVP_NNIE_MODEL_S));
    SAMPLE_COMM_SVP_CheckSysExit();
}

/******************************************************************************
* function : Rfcn software deinit
******************************************************************************/
static HI_S32 SAMPLE_SVP_NNIE_Rfcn_SoftwareDeinit(SAMPLE_SVP_NNIE_RFCN_SOFTWARE_PARAM_S* pstSoftWareParam)
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
static HI_S32 SAMPLE_SVP_NNIE_Rfcn_Deinit(SAMPLE_SVP_NNIE_PARAM_S *pstNnieParam,
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
        s32Ret = SAMPLE_SVP_NNIE_Rfcn_SoftwareDeinit(pstSoftWareParam);
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
static HI_S32 SAMPLE_SVP_NNIE_Rfcn_SoftwareInit(SAMPLE_SVP_NNIE_CFG_S* pstCfg,
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
    pstSoftWareParam->au32Scales[0] = 8 * SAMPLE_SVP_NNIE_QUANT_BASE;
    pstSoftWareParam->au32Scales[1] = 16 * SAMPLE_SVP_NNIE_QUANT_BASE;
    pstSoftWareParam->au32Scales[2] = 32 * SAMPLE_SVP_NNIE_QUANT_BASE;
    pstSoftWareParam->au32Ratios[0] = 0.5 * SAMPLE_SVP_NNIE_QUANT_BASE;
    pstSoftWareParam->au32Ratios[1] = 1 * SAMPLE_SVP_NNIE_QUANT_BASE;
    pstSoftWareParam->au32Ratios[2] = 2 * SAMPLE_SVP_NNIE_QUANT_BASE;
    pstSoftWareParam->u32OriImHeight = pstNnieParam->astSegData[0].astSrc[0].unShape.stWhc.u32Height;
    pstSoftWareParam->u32OriImWidth = pstNnieParam->astSegData[0].astSrc[0].unShape.stWhc.u32Width;
    pstSoftWareParam->u32MinSize = 16;
    pstSoftWareParam->u32FilterThresh = 0;
    pstSoftWareParam->u32SpatialScale = (HI_U32)(0.0625 * SAMPLE_SVP_NNIE_QUANT_BASE);
    pstSoftWareParam->u32NmsThresh = (HI_U32)(0.7 * SAMPLE_SVP_NNIE_QUANT_BASE);
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
    u32RpnTmpBufSize = SAMPLE_SVP_NNIE_RpnTmpBufSize(pstSoftWareParam->u32NumRatioAnchors,
        pstSoftWareParam->u32NumScaleAnchors,pstSoftWareParam->au32ConvHeight[0],
        pstSoftWareParam->au32ConvWidth[0]);
    u32RpnTmpBufSize = SAMPLE_SVP_NNIE_ALIGN16(u32RpnTmpBufSize);
    u32RpnBboxBufSize = pstSoftWareParam->stRpnBbox.u32Num*
        pstSoftWareParam->stRpnBbox.unShape.stWhc.u32Height*pstSoftWareParam->stRpnBbox.u32Stride;
    u32GetResultTmpBufSize = SAMPLE_SVP_NNIE_Rfcn_GetResultTmpBuf(pstCfg->u32MaxRoiNum,pstSoftWareParam->u32ClassNum);
    u32GetResultTmpBufSize = SAMPLE_SVP_NNIE_ALIGN16(u32GetResultTmpBufSize);
    u32ClassNum = pstSoftWareParam->u32ClassNum;
    u32DstRoiSize = SAMPLE_SVP_NNIE_ALIGN16(u32ClassNum*pstCfg->u32MaxRoiNum*sizeof(HI_U32)*SAMPLE_SVP_NNIE_COORDI_NUM);
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
    pstSoftWareParam->stDstRoi.u32Stride = SAMPLE_SVP_NNIE_ALIGN16(u32ClassNum*pstSoftWareParam->u32MaxRoiNum*sizeof(HI_U32)*SAMPLE_SVP_NNIE_COORDI_NUM);
    pstSoftWareParam->stDstRoi.u32Num = 1;
    pstSoftWareParam->stDstRoi.unShape.stWhc.u32Chn = 1;
    pstSoftWareParam->stDstRoi.unShape.stWhc.u32Height = 1;
    pstSoftWareParam->stDstRoi.unShape.stWhc.u32Width = u32ClassNum*pstSoftWareParam->u32MaxRoiNum*SAMPLE_SVP_NNIE_COORDI_NUM;

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
static HI_S32 SAMPLE_SVP_NNIE_Rfcn_ParamInit(SAMPLE_SVP_NNIE_CFG_S* pstCfg,
    SAMPLE_SVP_NNIE_PARAM_S *pstNnieParam, SAMPLE_SVP_NNIE_RFCN_SOFTWARE_PARAM_S* pstSoftWareParam)
{
    HI_S32 s32Ret = HI_SUCCESS;
    /*init hardware para*/
    s32Ret = SAMPLE_COMM_SVP_NNIE_ParamInit(pstCfg,pstNnieParam);
    SAMPLE_SVP_CHECK_EXPR_GOTO(HI_SUCCESS != s32Ret,INIT_FAIL_0,SAMPLE_SVP_ERR_LEVEL_ERROR,
        "Error(%#x),SAMPLE_COMM_SVP_NNIE_ParamInit failed!\n",s32Ret);

    /*init software para*/
    s32Ret = SAMPLE_SVP_NNIE_Rfcn_SoftwareInit(pstCfg,pstNnieParam,pstSoftWareParam);
    SAMPLE_SVP_CHECK_EXPR_GOTO(HI_SUCCESS != s32Ret,INIT_FAIL_0,SAMPLE_SVP_ERR_LEVEL_ERROR,
        "Error(%#x),SAMPLE_SVP_NNIE_Rfcn_SoftwareInit failed!\n",s32Ret);

    return s32Ret;
INIT_FAIL_0:
    s32Ret = SAMPLE_SVP_NNIE_Rfcn_Deinit(pstNnieParam,pstSoftWareParam,NULL);
    SAMPLE_SVP_CHECK_EXPR_RET(HI_SUCCESS != s32Ret,s32Ret,SAMPLE_SVP_ERR_LEVEL_ERROR,
            "Error(%#x),SAMPLE_SVP_NNIE_Rfcn_Deinit failed!\n",s32Ret);
    return HI_FAILURE;

}

/******************************************************************************
* function : roi to rect
******************************************************************************/
HI_S32 SAMPLE_SVP_NNIE_RoiToRect(SVP_BLOB_S *pstDstScore,
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
            u32BboxBias = u32RoiNumBias * SAMPLE_SVP_NNIE_COORDI_NUM;
            u32RoiNumTmp = 0;
            /*if the confidence score greater than result thresh, the result will be drawed*/
            if(((HI_FLOAT)ps32Score[u32ScoreBias] / SAMPLE_SVP_NNIE_QUANT_BASE >=
                paf32ScoreThr[i])  &&  (ps32ClassRoiNum[i] != 0))
            {
                for (j = 0; j < (HI_U32)ps32ClassRoiNum[i]; j++)
                {
                    /*Score is descend order*/
                    f32Score = (HI_FLOAT)ps32Score[u32ScoreBias + j] / SAMPLE_SVP_NNIE_QUANT_BASE;
                    if ((f32Score < paf32ScoreThr[i]) || (u32RoiNumTmp >= SAMPLE_SVP_NNIE_MAX_ROI_NUM_OF_CLASS))
                    {
                        break;
                    }

                    pstRect->astRect[i][u32RoiNumTmp].astPoint[0].s32X = (HI_U32)((HI_FLOAT)ps32Roi[u32BboxBias + j*SAMPLE_SVP_NNIE_COORDI_NUM] / (HI_FLOAT)u32SrcWidth * (HI_FLOAT)u32DstWidth) & (~1) ;
                    pstRect->astRect[i][u32RoiNumTmp].astPoint[0].s32Y = (HI_U32)((HI_FLOAT)ps32Roi[u32BboxBias + j*SAMPLE_SVP_NNIE_COORDI_NUM + 1] / (HI_FLOAT)u32SrcHeight * (HI_FLOAT)u32DstHeight) & (~1);

                    pstRect->astRect[i][u32RoiNumTmp].astPoint[1].s32X = (HI_U32)((HI_FLOAT)ps32Roi[u32BboxBias + j*SAMPLE_SVP_NNIE_COORDI_NUM + 2]/ (HI_FLOAT)u32SrcWidth * (HI_FLOAT)u32DstWidth) & (~1);
                    pstRect->astRect[i][u32RoiNumTmp].astPoint[1].s32Y = pstRect->astRect[i][u32RoiNumTmp].astPoint[0].s32Y;

                    pstRect->astRect[i][u32RoiNumTmp].astPoint[2].s32X = pstRect->astRect[i][u32RoiNumTmp].astPoint[1].s32X;
                    pstRect->astRect[i][u32RoiNumTmp].astPoint[2].s32Y = (HI_U32)((HI_FLOAT)ps32Roi[u32BboxBias + j*SAMPLE_SVP_NNIE_COORDI_NUM + 3] / (HI_FLOAT)u32SrcHeight * (HI_FLOAT)u32DstHeight) & (~1);

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
* function : Rfcn Proc
******************************************************************************/
static HI_S32 SAMPLE_SVP_NNIE_Rfcn_Proc(SAMPLE_SVP_NNIE_PARAM_S *pstParam,
    SAMPLE_SVP_NNIE_RFCN_SOFTWARE_PARAM_S *pstSwParam)
{
    HI_S32 s32Ret = HI_FAILURE;
    HI_U32 i = 0;
    SAMPLE_SVP_NNIE_INPUT_DATA_INDEX_S stInputDataIdx = {0};
    SAMPLE_SVP_NNIE_PROCESS_SEG_INDEX_S stProcSegIdx = {0};
    SAMPLE_SVP_NIE_PERF_STAT_DEF_VAR()

    stInputDataIdx.u32SegIdx = 0;
    stInputDataIdx.u32NodeIdx = 0;
    SAMPLE_SVP_NNIE_PERF_STAT_BEGIN()

    /*NNIE process 0-th seg*/
    stProcSegIdx.u32SegIdx = 0;
    s32Ret = SAMPLE_SVP_NNIE_Forward(pstParam,&stInputDataIdx,&stProcSegIdx,HI_TRUE);
    SAMPLE_SVP_CHECK_EXPR_RET(HI_SUCCESS != s32Ret,s32Ret,SAMPLE_SVP_ERR_LEVEL_ERROR,
        "Error,SAMPLE_SVP_NNIE_Forward failed!\n");
     SAMPLE_SVP_NNIE_PERF_STAT_END()

    SAMPLE_SVP_NNIE_PERF_STAT_RFCN_FORWARD_SRC_FLUSH_TIME()
    SAMPLE_SVP_NNIE_PERF_STAT_RFCN_FORWARD_PRE_DST_FLUSH_TIME()
    SAMPLE_SVP_NNIE_PERF_STAT_RFCN_FORWARD_AFTER_DST_FLUSH_TIME()
    SAMPLE_SVP_NNIE_PERF_STAT_RFCN_FORWARD_OP_TIME()

    /*RPN*/
    s32Ret = SAMPLE_SVP_NNIE_Rfcn_Rpn(pstParam, pstSwParam);
    SAMPLE_SVP_CHECK_EXPR_RET(HI_SUCCESS != s32Ret,s32Ret,SAMPLE_SVP_ERR_LEVEL_ERROR,
        "Error,SAMPLE_SVP_NNIE_RFCN_Rpn failed!\n");
    SAMPLE_SVP_NNIE_PERF_STAT_RFCN_RPN_SRC_FLUSH_TIME()
    SAMPLE_SVP_NNIE_PERF_STAT_RFCN_RPN_PRE_DST_FLUSH_TIME()
    SAMPLE_SVP_NNIE_PERF_STAT_RFCN_RPN_AFTER_DST_FLUSH_TIME()
    SAMPLE_SVP_NNIE_PERF_STAT_RFCN_RPN_OP_TIME()
    if(0 != pstSwParam->stRpnBbox.unShape.stWhc.u32Height)
    {
        /*NNIE process 1-th seg, the input data comes from 3-rd report node of 0-th seg,
          the input roi comes from RPN results*/
        stInputDataIdx.u32SegIdx = 0;
        stInputDataIdx.u32NodeIdx = 3;
        stProcSegIdx.u32SegIdx = 1;
        s32Ret = SAMPLE_SVP_NNIE_ForwardWithBbox(pstParam,&stInputDataIdx,
            &pstSwParam->stRpnBbox,&stProcSegIdx,HI_TRUE);
        SAMPLE_SVP_CHECK_EXPR_RET(HI_SUCCESS != s32Ret,s32Ret,SAMPLE_SVP_ERR_LEVEL_ERROR,
            "Error,SAMPLE_SVP_NNIE_Forward failed!\n");
        SAMPLE_SVP_NNIE_PERF_STAT_RFCN_PSROIPOOLING1_SRC_FLUSH_TIME()
        SAMPLE_SVP_NNIE_PERF_STAT_RFCN_PSROIPOOLING1_PRE_DST_FLUSH_TIME()
        SAMPLE_SVP_NNIE_PERF_STAT_RFCN_PSROIPOOLING1_AFTER_DST_FLUSH_TIME()
        SAMPLE_SVP_NNIE_PERF_STAT_RFCN_PSROIPOOLING1_OP_TIME()

        /*NNIE process 2-nd seg, the input data comes from 4-th report node of 0-th seg
          the input roi comes from RPN results*/
        stInputDataIdx.u32SegIdx = 0;
        stInputDataIdx.u32NodeIdx = 4;
        stProcSegIdx.u32SegIdx = 2;
        s32Ret = SAMPLE_SVP_NNIE_ForwardWithBbox(pstParam,&stInputDataIdx,
            &pstSwParam->stRpnBbox,&stProcSegIdx,HI_TRUE);
        SAMPLE_SVP_CHECK_EXPR_RET(HI_SUCCESS != s32Ret,s32Ret,SAMPLE_SVP_ERR_LEVEL_ERROR,
            "Error,SAMPLE_SVP_NNIE_Forward failed!\n");
        SAMPLE_SVP_NNIE_PERF_STAT_RFCN_PSROIPOOLING2_SRC_FLUSH_TIME()
        SAMPLE_SVP_NNIE_PERF_STAT_RFCN_PSROIPOOLING2_PRE_DST_FLUSH_TIME()
        SAMPLE_SVP_NNIE_PERF_STAT_RFCN_PSROIPOOLING2_AFTER_DST_FLUSH_TIME()
        SAMPLE_SVP_NNIE_PERF_STAT_RFCN_PSROIPOOLING2_OP_TIME()

        /*GetResult*/
        /*if user has changed net struct, please make sure SAMPLE_SVP_NNIE_Rfcn_GetResult
         function's input datas are correct*/
        SAMPLE_SVP_NNIE_PERF_STAT_BEGIN()
        s32Ret = SAMPLE_SVP_NNIE_Rfcn_GetResult(pstParam,pstSwParam);
        SAMPLE_SVP_CHECK_EXPR_RET(HI_SUCCESS != s32Ret,s32Ret,SAMPLE_SVP_ERR_LEVEL_ERROR,
            "Error,SAMPLE_SVP_NNIE_Rfcn_GetResult failed!\n");
        SAMPLE_SVP_NNIE_PERF_STAT_END()
        SAMPLE_SVP_NNIE_PERF_STAT_RFCN_GR_OP_TIME()
    }
    else
    {
        for (i = 0; i < pstSwParam->stClassRoiNum.unShape.stWhc.u32Width; i++)
        {
            *(((HI_U32*)(HI_UL)pstSwParam->stClassRoiNum.u64VirAddr)+i) = 0;
        }
    }
    return s32Ret;
}


/******************************************************************************
* function : Rfcn Proc
******************************************************************************/
static HI_S32 SAMPLE_SVP_NNIE_Rfcn_Proc_ViToVo(SAMPLE_SVP_NNIE_PARAM_S *pstParam,
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

    s32Ret = SAMPLE_SVP_NNIE_Rfcn_Proc(pstParam,pstSwParam);
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
    s32Ret = SAMPLE_SVP_NNIE_RoiToRect(&(pstSwParam->stDstScore),
    &(pstSwParam->stDstRoi), &(pstSwParam->stClassRoiNum), pstSwParam->af32ScoreThr,HI_TRUE,&(pstSwParam->stRect),
    pstExtFrmInfo->stVFrame.u32Width, pstExtFrmInfo->stVFrame.u32Height,u32BaseWidth,u32BaseHeight);
    SAMPLE_SVP_CHECK_EXPR_RET(HI_SUCCESS != s32Ret,s32Ret,SAMPLE_SVP_ERR_LEVEL_ERROR,
        "Error(%#x),SAMPLE_SVP_NNIE_RoiToRect failed!\n",s32Ret);

    return s32Ret;

}
/******************************************************************************
* function : Rfcn vi to vo thread entry
******************************************************************************/
static HI_VOID* SAMPLE_SVP_NNIE_Rfcn_ViToVo(HI_VOID* pArgs)
{
    HI_S32 s32Ret;
    SAMPLE_SVP_NNIE_PARAM_S *pstParam;
    SAMPLE_SVP_NNIE_RFCN_SOFTWARE_PARAM_S *pstSwParam;
    VIDEO_FRAME_INFO_S stBaseFrmInfo;
    VIDEO_FRAME_INFO_S stExtFrmInfo;
    HI_S32 s32MilliSec = 20000;
    VO_LAYER voLayer = 0;
    VO_CHN voChn = 0;
    HI_S32 s32VpssGrp = 0;
    HI_S32 as32VpssChn[] = {VPSS_CHN0, VPSS_CHN1};

    pstParam = &s_stRfcnNnieParam;
    pstSwParam = &s_stRfcnSoftwareParam;

    while (HI_FALSE == s_bNnieStopSignal)
    {
        s32Ret = HI_MPI_VPSS_GetChnFrame(s32VpssGrp, as32VpssChn[1], &stExtFrmInfo, s32MilliSec);
        if(HI_SUCCESS != s32Ret)
        {
            SAMPLE_PRT("Error(%#x),HI_MPI_VPSS_GetChnFrame failed, VPSS_GRP(%d), VPSS_CHN(%d)!\n",
                s32Ret,s32VpssGrp, as32VpssChn[1]);
            continue;
        }

        s32Ret = HI_MPI_VPSS_GetChnFrame(s32VpssGrp, as32VpssChn[0], &stBaseFrmInfo, s32MilliSec);
        SAMPLE_CHECK_EXPR_GOTO(HI_SUCCESS!=s32Ret, EXT_RELEASE,
            "Error(%#x),HI_MPI_VPSS_GetChnFrame failed, VPSS_GRP(%d), VPSS_CHN(%d)!\n",
            s32Ret,s32VpssGrp, as32VpssChn[0]);

        s32Ret = SAMPLE_SVP_NNIE_Rfcn_Proc_ViToVo(pstParam,pstSwParam, &stExtFrmInfo,
        stBaseFrmInfo.stVFrame.u32Width,stBaseFrmInfo.stVFrame.u32Height);
        SAMPLE_CHECK_EXPR_GOTO(HI_SUCCESS!=s32Ret, BASE_RELEASE,
            "Error(%#x),SAMPLE_SVP_NNIE_Rfcn_Proc failed!\n", s32Ret);

        //Draw rect
        s32Ret = SAMPLE_COMM_SVP_NNIE_FillRect(&stBaseFrmInfo, &(pstSwParam->stRect), 0x0000FF00);
        SAMPLE_CHECK_EXPR_GOTO(HI_SUCCESS!=s32Ret, BASE_RELEASE,
            "SAMPLE_COMM_SVP_NNIE_FillRect failed, Error(%#x)!\n", s32Ret);

        s32Ret = HI_MPI_VO_SendFrame(voLayer, voChn, &stBaseFrmInfo, s32MilliSec);
        SAMPLE_CHECK_EXPR_GOTO(HI_SUCCESS!=s32Ret, BASE_RELEASE,
            "HI_MPI_VO_SendFrame failed, Error(%#x)!\n", s32Ret);

        BASE_RELEASE:
            s32Ret = HI_MPI_VPSS_ReleaseChnFrame(s32VpssGrp,as32VpssChn[0], &stBaseFrmInfo);
            if (HI_SUCCESS != s32Ret)
            {
                SAMPLE_PRT("Error(%#x),HI_MPI_VPSS_ReleaseChnFrame failed,Grp(%d) chn(%d)!\n",
                    s32Ret,s32VpssGrp,as32VpssChn[0]);
            }

        EXT_RELEASE:
            s32Ret = HI_MPI_VPSS_ReleaseChnFrame(s32VpssGrp,as32VpssChn[1], &stExtFrmInfo);
            if (HI_SUCCESS != s32Ret)
            {
                SAMPLE_PRT("Error(%#x),HI_MPI_VPSS_ReleaseChnFrame failed,Grp(%d) chn(%d)!\n",
                    s32Ret,s32VpssGrp,as32VpssChn[1]);
            }

    }

    return HI_NULL;
}

/******************************************************************************
* function : Rfcn Vi->VO
******************************************************************************/
void SAMPLE_SVP_NNIE_Rfcn(void)
{
    HI_CHAR *pcModelName = "./data/nnie_model/detection/inst_rfcn_resnet50_cycle_352x288.wk";
    SAMPLE_SVP_NNIE_CFG_S   stNnieCfg = {0};
    SIZE_S stSize;
    PIC_SIZE_E enSize = PIC_CIF;
    HI_S32 s32Ret = HI_SUCCESS;
    HI_CHAR acThreadName[16] = {0};

    memset(&s_stRfcnModel,0,sizeof(s_stRfcnModel));
    memset(&s_stRfcnNnieParam,0,sizeof(s_stRfcnNnieParam));
    memset(&s_stRfcnSoftwareParam,0,sizeof(s_stRfcnSoftwareParam));

    /******************************************
     step 1: start vi vpss vo
     ******************************************/
    s_stRfcnSwitch.bVenc = HI_FALSE;
    s_stRfcnSwitch.bVo   = HI_TRUE;
    s32Ret = SAMPLE_COMM_IVE_StartViVpssVencVo(&s_stViConfig,&s_stRfcnSwitch,&enSize);
    SAMPLE_CHECK_EXPR_GOTO(HI_SUCCESS != s32Ret, END_RFCN_0,
        "Error(%#x),SAMPLE_COMM_IVE_StartViVpssVencVo failed!\n", s32Ret);

    s32Ret = SAMPLE_COMM_SYS_GetPicSize(enSize, &stSize);
    SAMPLE_CHECK_EXPR_GOTO(HI_SUCCESS != s32Ret, END_RFCN_0,
        "Error(%#x),SAMPLE_COMM_SYS_GetPicSize failed!\n", s32Ret);

    /******************************************
     step 2: init NNIE param
     ******************************************/
    stNnieCfg.pszPic= NULL;
    stNnieCfg.u32MaxInputNum = 1; //max input image num in each batch
    stNnieCfg.u32MaxRoiNum = 300;
    stNnieCfg.aenNnieCoreId[0] = SVP_NNIE_ID_0; //set NNIE core for 0-th Seg
    stNnieCfg.aenNnieCoreId[1] = SVP_NNIE_ID_0; //set NNIE core for 1-th Seg
    stNnieCfg.aenNnieCoreId[2] = SVP_NNIE_ID_0; //set NNIE core for 2-th Seg

    s32Ret = SAMPLE_COMM_SVP_NNIE_LoadModel(pcModelName,&s_stRfcnModel);
    SAMPLE_SVP_CHECK_EXPR_GOTO(HI_SUCCESS != s32Ret,END_RFCN_0,SAMPLE_SVP_ERR_LEVEL_ERROR,
        "Error,SAMPLE_COMM_SVP_NNIE_LoadModel failed!\n");

    /*apcRpnDataLayerName is used to set RPN data layer name
      and search RPN input data,if user has changed network struct, please
      make sure the data layer names are correct*/
    s_stRfcnNnieParam.pstModel = &s_stRfcnModel.stModel;
    s_stRfcnSoftwareParam.apcRpnDataLayerName[0] = "rpn_cls_score";
    s_stRfcnSoftwareParam.apcRpnDataLayerName[1] = "rpn_bbox_pred";
    s32Ret = SAMPLE_SVP_NNIE_Rfcn_ParamInit(&stNnieCfg,&s_stRfcnNnieParam,
           &s_stRfcnSoftwareParam);
    SAMPLE_SVP_CHECK_EXPR_GOTO(HI_SUCCESS != s32Ret,END_RFCN_1,SAMPLE_SVP_ERR_LEVEL_ERROR,
       "Error,SAMPLE_SVP_NNIE_Rfcn_ParamInit failed!\n");

    s_bNnieStopSignal = HI_FALSE;

    /******************************************
      step 3: Create work thread
     ******************************************/
    snprintf(acThreadName, 16, "NNIE_ViToVo");
    prctl(PR_SET_NAME, (unsigned long)acThreadName, 0,0,0);
    pthread_create(&s_hNnieThread, 0, SAMPLE_SVP_NNIE_Rfcn_ViToVo, NULL);

    SAMPLE_PAUSE();

    s_bNnieStopSignal = HI_TRUE;
    pthread_join(s_hNnieThread, HI_NULL);
    s_hNnieThread = 0;
END_RFCN_1:

    SAMPLE_SVP_NNIE_Rfcn_Deinit(&s_stRfcnNnieParam,&s_stRfcnSoftwareParam,&s_stRfcnModel);
END_RFCN_0:
    SAMPLE_COMM_IVE_StopViVpssVencVo(&s_stViConfig,&s_stRfcnSwitch);
    return ;

}

/******************************************************************************
* function : rfcn sample signal handle
******************************************************************************/
void SAMPLE_SVP_NNIE_Rfcn_HandleSig(void)
{
    s_bNnieStopSignal = HI_TRUE;
    if (0 != s_hNnieThread)
    {
        pthread_join(s_hNnieThread, HI_NULL);
        s_hNnieThread = 0;
    }

    SAMPLE_SVP_NNIE_Rfcn_Deinit(&s_stRfcnNnieParam,&s_stRfcnSoftwareParam,&s_stRfcnModel);
    memset(&s_stRfcnNnieParam,0,sizeof(SAMPLE_SVP_NNIE_PARAM_S));
    memset(&s_stRfcnSoftwareParam,0,sizeof(SAMPLE_SVP_NNIE_RFCN_SOFTWARE_PARAM_S));
    memset(&s_stRfcnModel,0,sizeof(SAMPLE_SVP_NNIE_MODEL_S));

    SAMPLE_COMM_IVE_StopViVpssVencVo(&s_stViConfig,&s_stRfcnSwitch);

}

/******************************************************************************
* function : rfcn sample signal handle
******************************************************************************/
void SAMPLE_SVP_NNIE_Rfcn_HandleSig_File(void)
{
   SAMPLE_SVP_NNIE_Rfcn_Deinit(&s_stRfcnNnieParam,&s_stRfcnSoftwareParam,&s_stRfcnModel);
   memset(&s_stRfcnModel,0,sizeof(s_stRfcnModel));
   memset(&s_stRfcnNnieParam,0,sizeof(s_stRfcnNnieParam));
   memset(&s_stRfcnSoftwareParam,0,sizeof(s_stRfcnSoftwareParam));
   SAMPLE_COMM_SVP_CheckSysExit();
}
/******************************************************************************
* function : Rfcn Read file
******************************************************************************/
void SAMPLE_SVP_NNIE_Rfcn_File(void)
{
    HI_CHAR *pcSrcFile = "./data/nnie_image/rgb_planar/hoser_dog_car_person_800x600.bgr";
    HI_CHAR *pcModelName = "./data/nnie_model/detection/inst_rfcn_resnet50_cycle.wk";
    SAMPLE_SVP_NNIE_CFG_S   stNnieCfg = {0};
    SAMPLE_SVP_NNIE_INPUT_DATA_INDEX_S stInputDataIdx = {0};
    HI_FLOAT f32PrintResultThresh = 0.0f;
    HI_S32 s32Ret = HI_SUCCESS;
    SAMPLE_SVP_NIE_PERF_STAT_DEF_FRM_VAR()

    memset(&s_stRfcnModel,0,sizeof(s_stRfcnModel));
    memset(&s_stRfcnNnieParam,0,sizeof(s_stRfcnNnieParam));
    memset(&s_stRfcnSoftwareParam,0,sizeof(s_stRfcnSoftwareParam));

    stNnieCfg.pszPic= pcSrcFile;
    stNnieCfg.u32MaxInputNum = 1; //max input image num in each batch
    stNnieCfg.u32MaxRoiNum = 300;
    stNnieCfg.aenNnieCoreId[0] = SVP_NNIE_ID_0; //set NNIE core for 0-th Seg
    stNnieCfg.aenNnieCoreId[1] = SVP_NNIE_ID_0; //set NNIE core for 1-th Seg
    stNnieCfg.aenNnieCoreId[2] = SVP_NNIE_ID_0; //set NNIE core for 2-th Seg
    /*Sys init*/
    SAMPLE_COMM_SVP_CheckSysInit();

    s32Ret = SAMPLE_COMM_SVP_NNIE_LoadModel(pcModelName,&s_stRfcnModel);
    SAMPLE_SVP_CHECK_EXPR_GOTO(HI_SUCCESS != s32Ret,END_RFCN_0,SAMPLE_SVP_ERR_LEVEL_ERROR,
        "Error,SAMPLE_COMM_SVP_NNIE_LoadModel failed!\n");

    /*apcRpnDataLayerName is used to set RPN data layer name
      and search RPN input data,if user has changed network struct, please
      make sure the data layer names are correct*/
    s_stRfcnNnieParam.pstModel = &s_stRfcnModel.stModel;
    s_stRfcnSoftwareParam.apcRpnDataLayerName[0] = "rpn_cls_score";
    s_stRfcnSoftwareParam.apcRpnDataLayerName[1] = "rpn_bbox_pred";
    s32Ret = SAMPLE_SVP_NNIE_Rfcn_ParamInit(&stNnieCfg,&s_stRfcnNnieParam,
           &s_stRfcnSoftwareParam);
    SAMPLE_SVP_CHECK_EXPR_GOTO(HI_SUCCESS != s32Ret,END_RFCN_0,SAMPLE_SVP_ERR_LEVEL_ERROR,
       "Error,SAMPLE_SVP_NNIE_Rfcn_ParamInit failed!\n");

    SAMPLE_SVP_NNIE_PERF_STAT_BEGIN_LOOP()

    stInputDataIdx.u32SegIdx = 0;
    stInputDataIdx.u32NodeIdx = 0;
    s32Ret = SAMPLE_SVP_NNIE_FillSrcData(&stNnieCfg,&s_stRfcnNnieParam,&stInputDataIdx);
    SAMPLE_SVP_CHECK_EXPR_GOTO(HI_SUCCESS != s32Ret,END_RFCN_0,SAMPLE_SVP_ERR_LEVEL_ERROR,
        "Error,SAMPLE_SVP_NNIE_FillSrcData failed!\n");

    s32Ret = SAMPLE_SVP_NNIE_Rfcn_Proc(&s_stRfcnNnieParam,&s_stRfcnSoftwareParam);
    SAMPLE_SVP_CHECK_EXPR_GOTO(HI_SUCCESS != s32Ret,END_RFCN_0,SAMPLE_SVP_ERR_LEVEL_ERROR,
        "Error,SAMPLE_SVP_NNIE_Rfcn_Proc failed!\n");

    /*print result, this sample has 21 classes:
     class 0:background     class 1:plane           class 2:bicycle
     class 3:bird           class 4:boat            class 5:bottle
     class 6:bus            class 7:car             class 8:cat
     class 9:chair          class10:cow             class11:diningtable
     class 12:dog           class13:horse           class14:motorbike
     class 15:person        class16:pottedplant     class17:sheep
     class 18:sofa          class19:train           class20:tvmonitor*/

    SAMPLE_SVP_TRACE_INFO("Rfcn result:\n");
   (void)SAMPLE_SVP_NNIE_Detection_PrintResult(&s_stRfcnSoftwareParam.stDstScore,
       &s_stRfcnSoftwareParam.stDstRoi, &s_stRfcnSoftwareParam.stClassRoiNum,f32PrintResultThresh);
   SAMPLE_SVP_NNIE_PERF_STAT_END_LOOP()

   SAMPLE_SVP_NNIE_PERF_STAT_RFCN_PRINT_FILE()

END_RFCN_0:

    SAMPLE_SVP_NNIE_Rfcn_Deinit(&s_stRfcnNnieParam,&s_stRfcnSoftwareParam,&s_stRfcnModel);
    memset(&s_stRfcnModel,0,sizeof(s_stRfcnModel));
    memset(&s_stRfcnNnieParam,0,sizeof(s_stRfcnNnieParam));
    memset(&s_stRfcnSoftwareParam,0,sizeof(s_stRfcnSoftwareParam));
    SAMPLE_COMM_SVP_CheckSysExit();

    return ;

}

/******************************************************************************
* function : SSD software deinit
******************************************************************************/
static HI_S32 SAMPLE_SVP_NNIE_Ssd_SoftwareDeinit(SAMPLE_SVP_NNIE_SSD_SOFTWARE_PARAM_S* pstSoftWareParam)
{
    HI_S32 s32Ret = HI_SUCCESS;
    SAMPLE_SVP_CHECK_EXPR_RET(NULL== pstSoftWareParam,HI_INVALID_VALUE,SAMPLE_SVP_ERR_LEVEL_ERROR,
        "Error, pstSoftWareParam can't be NULL!\n");
    if(0!=pstSoftWareParam->stPriorBoxTmpBuf.u64PhyAddr && 0!=pstSoftWareParam->stPriorBoxTmpBuf.u64VirAddr)
    {
        SAMPLE_SVP_MMZ_FREE(pstSoftWareParam->stPriorBoxTmpBuf.u64PhyAddr,
            pstSoftWareParam->stPriorBoxTmpBuf.u64VirAddr);
        pstSoftWareParam->stPriorBoxTmpBuf.u64PhyAddr = 0;
        pstSoftWareParam->stPriorBoxTmpBuf.u64VirAddr = 0;
    }
    return s32Ret;
}


/******************************************************************************
* function : Ssd Deinit
******************************************************************************/
static HI_S32 SAMPLE_SVP_NNIE_Ssd_Deinit(SAMPLE_SVP_NNIE_PARAM_S *pstNnieParam,
    SAMPLE_SVP_NNIE_SSD_SOFTWARE_PARAM_S* pstSoftWareParam,SAMPLE_SVP_NNIE_MODEL_S *pstNnieModel)
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
        s32Ret = SAMPLE_SVP_NNIE_Ssd_SoftwareDeinit(pstSoftWareParam);
        SAMPLE_SVP_CHECK_EXPR_TRACE(HI_SUCCESS != s32Ret,SAMPLE_SVP_ERR_LEVEL_ERROR,
            "Error,SAMPLE_SVP_NNIE_Ssd_SoftwareDeinit failed!\n");
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
* function : Ssd software para init
******************************************************************************/
static HI_S32 SAMPLE_SVP_NNIE_Ssd_SoftwareInit(SAMPLE_SVP_NNIE_CFG_S* pstCfg,
    SAMPLE_SVP_NNIE_PARAM_S *pstNnieParam, SAMPLE_SVP_NNIE_SSD_SOFTWARE_PARAM_S* pstSoftWareParam)
{
    HI_U32 i = 0;
    HI_S32 s32Ret = HI_SUCCESS;
    HI_U32 u32ClassNum = 0;
    HI_U32 u32TotalSize = 0;
    HI_U32 u32DstRoiSize = 0;
    HI_U32 u32DstScoreSize = 0;
    HI_U32 u32ClassRoiNumSize = 0;
    HI_U32 u32TmpBufTotalSize = 0;
    HI_U64 u64PhyAddr = 0;
    HI_U8* pu8VirAddr = NULL;

    /*Set Conv Parameters*/
    /*the SSD sample report resule is after permute operation,
     conv result is (C, H, W), after permute, the report node's
     (C1, H1, W1) is (H, W, C), the stride of report result is aligned according to C dim*/
    for(i = 0; i < 12; i++)
    {
        pstSoftWareParam->au32ConvHeight[i] = pstNnieParam->pstModel->astSeg[0].astDstNode[i].unShape.stWhc.u32Chn;
        pstSoftWareParam->au32ConvWidth[i] = pstNnieParam->pstModel->astSeg[0].astDstNode[i].unShape.stWhc.u32Height;
        pstSoftWareParam->au32ConvChannel[i] = pstNnieParam->pstModel->astSeg[0].astDstNode[i].unShape.stWhc.u32Width;
        if(i%2==1)
        {
            pstSoftWareParam->au32ConvStride[i/2] = SAMPLE_SVP_NNIE_ALIGN16(pstSoftWareParam->au32ConvChannel[i]*sizeof(HI_U32))/sizeof(HI_U32);
        }
    }

    /*Set PriorBox Parameters*/
    pstSoftWareParam->au32PriorBoxWidth[0] = 38;
    pstSoftWareParam->au32PriorBoxWidth[1] = 19;
    pstSoftWareParam->au32PriorBoxWidth[2] = 10;
    pstSoftWareParam->au32PriorBoxWidth[3] = 5;
    pstSoftWareParam->au32PriorBoxWidth[4] = 3;
    pstSoftWareParam->au32PriorBoxWidth[5] = 1;

    pstSoftWareParam->au32PriorBoxHeight[0] = 38;
    pstSoftWareParam->au32PriorBoxHeight[1] = 19;
    pstSoftWareParam->au32PriorBoxHeight[2] = 10;
    pstSoftWareParam->au32PriorBoxHeight[3] = 5;
    pstSoftWareParam->au32PriorBoxHeight[4] = 3;
    pstSoftWareParam->au32PriorBoxHeight[5] = 1;

    pstSoftWareParam->u32OriImHeight = pstNnieParam->astSegData[0].astSrc[0].unShape.stWhc.u32Height;
    pstSoftWareParam->u32OriImWidth = pstNnieParam->astSegData[0].astSrc[0].unShape.stWhc.u32Width;

    pstSoftWareParam->af32PriorBoxMinSize[0][0] = 30.0f;
    pstSoftWareParam->af32PriorBoxMinSize[1][0] = 60.0f;
    pstSoftWareParam->af32PriorBoxMinSize[2][0] = 111.0f;
    pstSoftWareParam->af32PriorBoxMinSize[3][0] = 162.0f;
    pstSoftWareParam->af32PriorBoxMinSize[4][0] = 213.0f;
    pstSoftWareParam->af32PriorBoxMinSize[5][0] = 264.0f;

    pstSoftWareParam->af32PriorBoxMaxSize[0][0] = 60.0f;
    pstSoftWareParam->af32PriorBoxMaxSize[1][0] = 111.0f;
    pstSoftWareParam->af32PriorBoxMaxSize[2][0] = 162.0f;
    pstSoftWareParam->af32PriorBoxMaxSize[3][0] = 213.0f;
    pstSoftWareParam->af32PriorBoxMaxSize[4][0] = 264.0f;
    pstSoftWareParam->af32PriorBoxMaxSize[5][0] = 315.0f;

    pstSoftWareParam->u32MinSizeNum = 1;
    pstSoftWareParam->u32MaxSizeNum = 1;
    pstSoftWareParam->bFlip= HI_TRUE;
    pstSoftWareParam->bClip= HI_FALSE;

    pstSoftWareParam->au32InputAspectRatioNum[0] = 1;
    pstSoftWareParam->au32InputAspectRatioNum[1] = 2;
    pstSoftWareParam->au32InputAspectRatioNum[2] = 2;
    pstSoftWareParam->au32InputAspectRatioNum[3] = 2;
    pstSoftWareParam->au32InputAspectRatioNum[4] = 1;
    pstSoftWareParam->au32InputAspectRatioNum[5] = 1;

    pstSoftWareParam->af32PriorBoxAspectRatio[0][0] = 2;
    pstSoftWareParam->af32PriorBoxAspectRatio[0][1] = 0;
    pstSoftWareParam->af32PriorBoxAspectRatio[1][0] = 2;
    pstSoftWareParam->af32PriorBoxAspectRatio[1][1] = 3;
    pstSoftWareParam->af32PriorBoxAspectRatio[2][0] = 2;
    pstSoftWareParam->af32PriorBoxAspectRatio[2][1] = 3;
    pstSoftWareParam->af32PriorBoxAspectRatio[3][0] = 2;
    pstSoftWareParam->af32PriorBoxAspectRatio[3][1] = 3;
    pstSoftWareParam->af32PriorBoxAspectRatio[4][0] = 2;
    pstSoftWareParam->af32PriorBoxAspectRatio[4][1] = 0;
    pstSoftWareParam->af32PriorBoxAspectRatio[5][0] = 2;
    pstSoftWareParam->af32PriorBoxAspectRatio[5][1] = 0;

    pstSoftWareParam->af32PriorBoxStepWidth[0] = 8;
    pstSoftWareParam->af32PriorBoxStepWidth[1] = 16;
    pstSoftWareParam->af32PriorBoxStepWidth[2] = 32;
    pstSoftWareParam->af32PriorBoxStepWidth[3] = 64;
    pstSoftWareParam->af32PriorBoxStepWidth[4] = 100;
    pstSoftWareParam->af32PriorBoxStepWidth[5] = 300;

    pstSoftWareParam->af32PriorBoxStepHeight[0] = 8;
    pstSoftWareParam->af32PriorBoxStepHeight[1] = 16;
    pstSoftWareParam->af32PriorBoxStepHeight[2] = 32;
    pstSoftWareParam->af32PriorBoxStepHeight[3] = 64;
    pstSoftWareParam->af32PriorBoxStepHeight[4] = 100;
    pstSoftWareParam->af32PriorBoxStepHeight[5] = 300;

    pstSoftWareParam->f32Offset = 0.5f;

    pstSoftWareParam->as32PriorBoxVar[0] = (HI_S32)(0.1f*SAMPLE_SVP_NNIE_QUANT_BASE);
    pstSoftWareParam->as32PriorBoxVar[1] = (HI_S32)(0.1f*SAMPLE_SVP_NNIE_QUANT_BASE);
    pstSoftWareParam->as32PriorBoxVar[2] = (HI_S32)(0.2f*SAMPLE_SVP_NNIE_QUANT_BASE);
    pstSoftWareParam->as32PriorBoxVar[3] = (HI_S32)(0.2f*SAMPLE_SVP_NNIE_QUANT_BASE);

    /*Set Softmax Parameters*/
    pstSoftWareParam->u32SoftMaxInHeight = 21;
    pstSoftWareParam->au32SoftMaxInChn[0] = 121296;
    pstSoftWareParam->au32SoftMaxInChn[1] = 45486;
    pstSoftWareParam->au32SoftMaxInChn[2] = 12600;
    pstSoftWareParam->au32SoftMaxInChn[3] = 3150;
    pstSoftWareParam->au32SoftMaxInChn[4] = 756;
    pstSoftWareParam->au32SoftMaxInChn[5] = 84;

    pstSoftWareParam->u32ConcatNum = 6;
    pstSoftWareParam->u32SoftMaxOutWidth = 1;
    pstSoftWareParam->u32SoftMaxOutHeight = 21;
    pstSoftWareParam->u32SoftMaxOutChn = 8732;

    /*Set DetectionOut Parameters*/
    pstSoftWareParam->u32ClassNum = 21;
    pstSoftWareParam->u32TopK = 400;
    pstSoftWareParam->u32KeepTopK = 200;
    pstSoftWareParam->u32NmsThresh = (HI_U16)(0.3f*SAMPLE_SVP_NNIE_QUANT_BASE);
    pstSoftWareParam->u32ConfThresh = 1;
    pstSoftWareParam->au32DetectInputChn[0] = 23104;
    pstSoftWareParam->au32DetectInputChn[1] = 8664;
    pstSoftWareParam->au32DetectInputChn[2] = 2400;
    pstSoftWareParam->au32DetectInputChn[3] = 600;
    pstSoftWareParam->au32DetectInputChn[4] = 144;
    pstSoftWareParam->au32DetectInputChn[5] = 16;

    /*Malloc assist buffer memory*/
    u32ClassNum = pstSoftWareParam->u32ClassNum;
    u32TotalSize = SAMPLE_SVP_NNIE_Ssd_GetResultTmpBuf(pstNnieParam,pstSoftWareParam);
    u32DstRoiSize = SAMPLE_SVP_NNIE_ALIGN16(u32ClassNum*pstSoftWareParam->u32TopK*sizeof(HI_U32)*SAMPLE_SVP_NNIE_COORDI_NUM);
    u32DstScoreSize = SAMPLE_SVP_NNIE_ALIGN16(u32ClassNum*pstSoftWareParam->u32TopK*sizeof(HI_U32));
    u32ClassRoiNumSize = SAMPLE_SVP_NNIE_ALIGN16(u32ClassNum*sizeof(HI_U32));
    u32TotalSize = u32TotalSize+u32DstRoiSize+u32DstScoreSize+u32ClassRoiNumSize;
    s32Ret = SAMPLE_COMM_SVP_MallocCached("SAMPLE_SSD_INIT",NULL,(HI_U64*)&u64PhyAddr,
        (void**)&pu8VirAddr,u32TotalSize);
    SAMPLE_SVP_CHECK_EXPR_RET(HI_SUCCESS != s32Ret,s32Ret,SAMPLE_SVP_ERR_LEVEL_ERROR,
        "Error,Malloc memory failed!\n");
    memset(pu8VirAddr,0, u32TotalSize);
    SAMPLE_COMM_SVP_FlushCache(u64PhyAddr,(void*)pu8VirAddr,u32TotalSize);

   /*set each tmp buffer addr*/
    pstSoftWareParam->stPriorBoxTmpBuf.u64PhyAddr = u64PhyAddr;
    pstSoftWareParam->stPriorBoxTmpBuf.u64VirAddr = (HI_U64)((HI_UL)pu8VirAddr);

    pstSoftWareParam->stSoftMaxTmpBuf.u64PhyAddr = u64PhyAddr+
        pstSoftWareParam->stPriorBoxTmpBuf.u32Size;
    pstSoftWareParam->stSoftMaxTmpBuf.u64VirAddr = (HI_U64)((HI_UL)pu8VirAddr+
        pstSoftWareParam->stPriorBoxTmpBuf.u32Size);

    pstSoftWareParam->stGetResultTmpBuf.u64PhyAddr = u64PhyAddr+
        pstSoftWareParam->stPriorBoxTmpBuf.u32Size+pstSoftWareParam->stSoftMaxTmpBuf.u32Size;
    pstSoftWareParam->stGetResultTmpBuf.u64VirAddr = (HI_U64)((HI_UL)pu8VirAddr+
        pstSoftWareParam->stPriorBoxTmpBuf.u32Size+ pstSoftWareParam->stSoftMaxTmpBuf.u32Size);

    u32TmpBufTotalSize = pstSoftWareParam->stPriorBoxTmpBuf.u32Size+
        pstSoftWareParam->stSoftMaxTmpBuf.u32Size + pstSoftWareParam->stGetResultTmpBuf.u32Size;

    /*set result blob*/
    pstSoftWareParam->stDstRoi.enType = SVP_BLOB_TYPE_S32;
    pstSoftWareParam->stDstRoi.u64PhyAddr = u64PhyAddr+u32TmpBufTotalSize;
    pstSoftWareParam->stDstRoi.u64VirAddr = (HI_U64)((HI_UL)pu8VirAddr+u32TmpBufTotalSize);
    pstSoftWareParam->stDstRoi.u32Stride = SAMPLE_SVP_NNIE_ALIGN16(u32ClassNum*
        pstSoftWareParam->u32TopK*sizeof(HI_U32)*SAMPLE_SVP_NNIE_COORDI_NUM);
    pstSoftWareParam->stDstRoi.u32Num = 1;
    pstSoftWareParam->stDstRoi.unShape.stWhc.u32Chn = 1;
    pstSoftWareParam->stDstRoi.unShape.stWhc.u32Height = 1;
    pstSoftWareParam->stDstRoi.unShape.stWhc.u32Width = u32ClassNum*
        pstSoftWareParam->u32TopK*SAMPLE_SVP_NNIE_COORDI_NUM;

    pstSoftWareParam->stDstScore.enType = SVP_BLOB_TYPE_S32;
    pstSoftWareParam->stDstScore.u64PhyAddr = u64PhyAddr+u32TmpBufTotalSize+u32DstRoiSize;
    pstSoftWareParam->stDstScore.u64VirAddr = (HI_U64)((HI_UL)pu8VirAddr+u32TmpBufTotalSize+u32DstRoiSize);
    pstSoftWareParam->stDstScore.u32Stride = SAMPLE_SVP_NNIE_ALIGN16(u32ClassNum*
        pstSoftWareParam->u32TopK*sizeof(HI_U32));
    pstSoftWareParam->stDstScore.u32Num = 1;
    pstSoftWareParam->stDstScore.unShape.stWhc.u32Chn = 1;
    pstSoftWareParam->stDstScore.unShape.stWhc.u32Height = 1;
    pstSoftWareParam->stDstScore.unShape.stWhc.u32Width = u32ClassNum*
        pstSoftWareParam->u32TopK;

    pstSoftWareParam->stClassRoiNum.enType = SVP_BLOB_TYPE_S32;
    pstSoftWareParam->stClassRoiNum.u64PhyAddr = u64PhyAddr+u32TmpBufTotalSize+
        u32DstRoiSize+u32DstScoreSize;
    pstSoftWareParam->stClassRoiNum.u64VirAddr = (HI_U64)((HI_UL)pu8VirAddr+u32TmpBufTotalSize+
        u32DstRoiSize+u32DstScoreSize);
    pstSoftWareParam->stClassRoiNum.u32Stride = SAMPLE_SVP_NNIE_ALIGN16(u32ClassNum*sizeof(HI_U32));
    pstSoftWareParam->stClassRoiNum.u32Num = 1;
    pstSoftWareParam->stClassRoiNum.unShape.stWhc.u32Chn = 1;
    pstSoftWareParam->stClassRoiNum.unShape.stWhc.u32Height = 1;
    pstSoftWareParam->stClassRoiNum.unShape.stWhc.u32Width = u32ClassNum;

    return s32Ret;
}


/******************************************************************************
* function : Ssd init
******************************************************************************/
static HI_S32 SAMPLE_SVP_NNIE_Ssd_ParamInit(SAMPLE_SVP_NNIE_CFG_S* pstCfg,
    SAMPLE_SVP_NNIE_PARAM_S *pstNnieParam, SAMPLE_SVP_NNIE_SSD_SOFTWARE_PARAM_S* pstSoftWareParam)
{
    HI_S32 s32Ret = HI_SUCCESS;
    /*init hardware para*/
    s32Ret = SAMPLE_COMM_SVP_NNIE_ParamInit(pstCfg,pstNnieParam);
    SAMPLE_SVP_CHECK_EXPR_GOTO(HI_SUCCESS != s32Ret,INIT_FAIL_0,SAMPLE_SVP_ERR_LEVEL_ERROR,
        "Error(%#x),SAMPLE_COMM_SVP_NNIE_ParamInit failed!\n",s32Ret);

    /*init software para*/
    s32Ret = SAMPLE_SVP_NNIE_Ssd_SoftwareInit(pstCfg,pstNnieParam,
        pstSoftWareParam);
    SAMPLE_SVP_CHECK_EXPR_GOTO(HI_SUCCESS != s32Ret,INIT_FAIL_0,SAMPLE_SVP_ERR_LEVEL_ERROR,
        "Error(%#x),SAMPLE_SVP_NNIE_Ssd_SoftwareInit failed!\n",s32Ret);

    return s32Ret;
INIT_FAIL_0:
    s32Ret = SAMPLE_SVP_NNIE_Ssd_Deinit(pstNnieParam,pstSoftWareParam,NULL);
    SAMPLE_SVP_CHECK_EXPR_RET(HI_SUCCESS != s32Ret,s32Ret,SAMPLE_SVP_ERR_LEVEL_ERROR,
            "Error(%#x),SAMPLE_SVP_NNIE_Ssd_Deinit failed!\n",s32Ret);
    return HI_FAILURE;

}


/******************************************************************************
* function : show SSD sample(image 300x300 U8_C3)
******************************************************************************/
void SAMPLE_SVP_NNIE_Ssd(void)
{
    HI_CHAR *pcSrcFile = "./data/nnie_image/rgb_planar/dog_bike_car_300x300.bgr";
    HI_CHAR *pcModelName = "./data/nnie_model/detection/inst_ssd_cycle.wk";
    HI_U32 u32PicNum = 1;
    HI_FLOAT f32PrintResultThresh = 0.0f;
    HI_S32 s32Ret = HI_SUCCESS;
    SAMPLE_SVP_NNIE_CFG_S   stNnieCfg = {0};
    SAMPLE_SVP_NNIE_INPUT_DATA_INDEX_S stInputDataIdx = {0};
    SAMPLE_SVP_NNIE_PROCESS_SEG_INDEX_S stProcSegIdx = {0};
    SAMPLE_SVP_NIE_PERF_STAT_DEF_FRM_VAR()
    SAMPLE_SVP_NIE_PERF_STAT_DEF_VAR()

    /*Set configuration parameter*/
    f32PrintResultThresh = 0.8f;
    stNnieCfg.pszPic= pcSrcFile;
    stNnieCfg.u32MaxInputNum = u32PicNum; //max input image num in each batch
    stNnieCfg.u32MaxRoiNum = 0;
    stNnieCfg.aenNnieCoreId[0] = SVP_NNIE_ID_0;//set NNIE core

    /*Sys init*/
    SAMPLE_COMM_SVP_CheckSysInit();

    /*Ssd Load model*/
    SAMPLE_SVP_TRACE_INFO("Ssd Load model!\n");
    s32Ret = SAMPLE_COMM_SVP_NNIE_LoadModel(pcModelName,&s_stSsdModel);
    SAMPLE_SVP_CHECK_EXPR_GOTO(HI_SUCCESS != s32Ret,SSD_FAIL_0,SAMPLE_SVP_ERR_LEVEL_ERROR,
        "Error,SAMPLE_COMM_SVP_NNIE_LoadModel failed!\n");

    /*Ssd parameter initialization*/
    /*Ssd parameters are set in SAMPLE_SVP_NNIE_Ssd_SoftwareInit,
      if user has changed net struct, please make sure the parameter settings in
      SAMPLE_SVP_NNIE_Ssd_SoftwareInit function are correct*/
    SAMPLE_SVP_TRACE_INFO("Ssd parameter initialization!\n");
    s_stSsdNnieParam.pstModel = &s_stSsdModel.stModel;
    s32Ret = SAMPLE_SVP_NNIE_Ssd_ParamInit(&stNnieCfg,&s_stSsdNnieParam,&s_stSsdSoftwareParam);
    SAMPLE_SVP_CHECK_EXPR_GOTO(HI_SUCCESS != s32Ret,SSD_FAIL_0,SAMPLE_SVP_ERR_LEVEL_ERROR,
        "Error,SAMPLE_SVP_NNIE_Ssd_ParamInit failed!\n");

    /*Fill src data*/
    SAMPLE_SVP_TRACE_INFO("Ssd start!\n");

    SAMPLE_SVP_NNIE_PERF_STAT_BEGIN_LOOP()
    stInputDataIdx.u32SegIdx = 0;
    stInputDataIdx.u32NodeIdx = 0;
    s32Ret = SAMPLE_SVP_NNIE_FillSrcData(&stNnieCfg,&s_stSsdNnieParam,&stInputDataIdx);
    SAMPLE_SVP_CHECK_EXPR_GOTO(HI_SUCCESS != s32Ret,SSD_FAIL_0,SAMPLE_SVP_ERR_LEVEL_ERROR,
        "Error,SAMPLE_SVP_NNIE_FillSrcData failed!\n");

    /*NNIE process(process the 0-th segment)*/
    stProcSegIdx.u32SegIdx = 0;
    s32Ret = SAMPLE_SVP_NNIE_Forward(&s_stSsdNnieParam,&stInputDataIdx,&stProcSegIdx,HI_TRUE);
    SAMPLE_SVP_CHECK_EXPR_GOTO(HI_SUCCESS != s32Ret,SSD_FAIL_0,SAMPLE_SVP_ERR_LEVEL_ERROR,
        "Error,SAMPLE_SVP_NNIE_Forward failed!\n");
    SAMPLE_SVP_NNIE_PERF_STAT_SSD_FORWARD_SRC_FLUSH_TIME()
    SAMPLE_SVP_NNIE_PERF_STAT_SSD_FORWARD_PRE_DST_FLUSH_TIME()
    SAMPLE_SVP_NNIE_PERF_STAT_SSD_FORWARD_AFTER_DST_FLUSH_TIME()
    SAMPLE_SVP_NNIE_PERF_STAT_SSD_FORWARD_OP_TIME()


    /*software process*/
    /*if user has changed net struct, please make sure SAMPLE_SVP_NNIE_Ssd_GetResult
     function's input datas are correct*/
    SAMPLE_SVP_NNIE_PERF_STAT_BEGIN()
    s32Ret = SAMPLE_SVP_NNIE_Ssd_GetResult(&s_stSsdNnieParam,&s_stSsdSoftwareParam);
    SAMPLE_SVP_CHECK_EXPR_GOTO(HI_SUCCESS != s32Ret,SSD_FAIL_0,SAMPLE_SVP_ERR_LEVEL_ERROR,
        "Error,SAMPLE_SVP_NNIE_Ssd_GetResult failed!\n");
    SAMPLE_SVP_NNIE_PERF_STAT_END()
    SAMPLE_SVP_NNIE_PERF_STAT_SSD_GR_OP_TIME()

    /*print result, this sample has 21 classes:
     class 0:background     class 1:plane           class 2:bicycle
     class 3:bird           class 4:boat            class 5:bottle
     class 6:bus            class 7:car             class 8:cat
     class 9:chair          class10:cow             class11:diningtable
     class 12:dog           class13:horse           class14:motorbike
     class 15:person        class16:pottedplant     class17:sheep
     class 18:sofa          class19:train           class20:tvmonitor*/
     SAMPLE_SVP_TRACE_INFO("Ssd result:\n");
    (void)SAMPLE_SVP_NNIE_Detection_PrintResult(&s_stSsdSoftwareParam.stDstScore,
        &s_stSsdSoftwareParam.stDstRoi, &s_stSsdSoftwareParam.stClassRoiNum,f32PrintResultThresh);
    SAMPLE_SVP_NNIE_PERF_STAT_END_LOOP()

    SAMPLE_SVP_NNIE_PERF_STAT_SSD_PRINT()


SSD_FAIL_0:
    SAMPLE_SVP_NNIE_Ssd_Deinit(&s_stSsdNnieParam,&s_stSsdSoftwareParam,&s_stSsdModel);
    SAMPLE_COMM_SVP_CheckSysExit();
}

/******************************************************************************
* function : SSD sample signal handle
******************************************************************************/
void SAMPLE_SVP_NNIE_Ssd_HandleSig(void)
{
    SAMPLE_SVP_NNIE_Ssd_Deinit(&s_stSsdNnieParam,&s_stSsdSoftwareParam,&s_stSsdModel);
    memset(&s_stSsdNnieParam,0,sizeof(SAMPLE_SVP_NNIE_PARAM_S));
    memset(&s_stSsdSoftwareParam,0,sizeof(SAMPLE_SVP_NNIE_SSD_SOFTWARE_PARAM_S));
    memset(&s_stSsdModel,0,sizeof(SAMPLE_SVP_NNIE_MODEL_S));
    SAMPLE_COMM_SVP_CheckSysExit();
}

/******************************************************************************
* function : Yolov1 software deinit
******************************************************************************/
static HI_S32 SAMPLE_SVP_NNIE_Yolov1_SoftwareDeinit(SAMPLE_SVP_NNIE_YOLOV1_SOFTWARE_PARAM_S* pstSoftWareParam)
{
    HI_S32 s32Ret = HI_SUCCESS;
    SAMPLE_SVP_CHECK_EXPR_RET(NULL== pstSoftWareParam,HI_INVALID_VALUE,SAMPLE_SVP_ERR_LEVEL_ERROR,
        "Error, pstSoftWareParam can't be NULL!\n");
    if(0!=pstSoftWareParam->stGetResultTmpBuf.u64PhyAddr && 0!=pstSoftWareParam->stGetResultTmpBuf.u64VirAddr)
    {
        SAMPLE_SVP_MMZ_FREE(pstSoftWareParam->stGetResultTmpBuf.u64PhyAddr,
            pstSoftWareParam->stGetResultTmpBuf.u64VirAddr);
        pstSoftWareParam->stGetResultTmpBuf.u64PhyAddr = 0;
        pstSoftWareParam->stGetResultTmpBuf.u64VirAddr = 0;
        pstSoftWareParam->stDstRoi.u64PhyAddr = 0;
        pstSoftWareParam->stDstRoi.u64VirAddr = 0;
        pstSoftWareParam->stDstScore.u64PhyAddr = 0;
        pstSoftWareParam->stDstScore.u64VirAddr = 0;
        pstSoftWareParam->stClassRoiNum.u64PhyAddr = 0;
        pstSoftWareParam->stClassRoiNum.u64VirAddr = 0;
    }
    return s32Ret;
}


/******************************************************************************
* function : Yolov1 Deinit
******************************************************************************/
static HI_S32 SAMPLE_SVP_NNIE_Yolov1_Deinit(SAMPLE_SVP_NNIE_PARAM_S *pstNnieParam,
    SAMPLE_SVP_NNIE_YOLOV1_SOFTWARE_PARAM_S* pstSoftWareParam,SAMPLE_SVP_NNIE_MODEL_S *pstNnieModel)
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
        s32Ret = SAMPLE_SVP_NNIE_Yolov1_SoftwareDeinit(pstSoftWareParam);
        SAMPLE_SVP_CHECK_EXPR_TRACE(HI_SUCCESS != s32Ret,SAMPLE_SVP_ERR_LEVEL_ERROR,
            "Error,SAMPLE_SVP_NNIE_Yolov1_SoftwareDeinit failed!\n");
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
* function : Yolov1 software para init
******************************************************************************/
static HI_S32 SAMPLE_SVP_NNIE_Yolov1_SoftwareInit(SAMPLE_SVP_NNIE_CFG_S* pstCfg,
    SAMPLE_SVP_NNIE_PARAM_S *pstNnieParam, SAMPLE_SVP_NNIE_YOLOV1_SOFTWARE_PARAM_S* pstSoftWareParam)
{
    HI_S32 s32Ret = HI_SUCCESS;
    HI_U32 u32ClassNum = 0;
    HI_U32 u32BboxNum = 0;
    HI_U32 u32TotalSize = 0;
    HI_U32 u32DstRoiSize = 0;
    HI_U32 u32DstScoreSize = 0;
    HI_U32 u32ClassRoiNumSize = 0;
    HI_U32 u32TmpBufTotalSize = 0;
    HI_U64 u64PhyAddr = 0;
    HI_U8* pu8VirAddr = NULL;

    pstSoftWareParam->u32OriImHeight = pstNnieParam->astSegData[0].astSrc[0].unShape.stWhc.u32Height;
    pstSoftWareParam->u32OriImWidth = pstNnieParam->astSegData[0].astSrc[0].unShape.stWhc.u32Width;
    pstSoftWareParam->u32BboxNumEachGrid = 2;
    pstSoftWareParam->u32ClassNum = 20;
    pstSoftWareParam->u32GridNumHeight = 7;
    pstSoftWareParam->u32GridNumWidth = 7;
    pstSoftWareParam->u32NmsThresh = (HI_U32)(0.5f*SAMPLE_SVP_NNIE_QUANT_BASE);
    pstSoftWareParam->u32ConfThresh = (HI_U32)(0.2f*SAMPLE_SVP_NNIE_QUANT_BASE);

    /*Malloc assist buffer memory*/
    u32ClassNum = pstSoftWareParam->u32ClassNum+1;
    u32BboxNum = pstSoftWareParam->u32BboxNumEachGrid*pstSoftWareParam->u32GridNumHeight*
        pstSoftWareParam->u32GridNumWidth;
    u32TmpBufTotalSize = SAMPLE_SVP_NNIE_Yolov1_GetResultTmpBuf(pstNnieParam,pstSoftWareParam);
    u32DstRoiSize = SAMPLE_SVP_NNIE_ALIGN16(u32ClassNum*u32BboxNum*SAMPLE_SVP_NNIE_COORDI_NUM);
    u32DstScoreSize = SAMPLE_SVP_NNIE_ALIGN16(u32ClassNum*u32BboxNum*sizeof(HI_U32));
    u32ClassRoiNumSize = SAMPLE_SVP_NNIE_ALIGN16(u32ClassNum*sizeof(HI_U32));
    u32TotalSize = u32TotalSize+u32DstRoiSize+u32DstScoreSize+u32ClassRoiNumSize+u32TmpBufTotalSize;
    s32Ret = SAMPLE_COMM_SVP_MallocCached("SAMPLE_YOLOV1_INIT",NULL,(HI_U64*)&u64PhyAddr,
        (void**)&pu8VirAddr,u32TotalSize);
    SAMPLE_SVP_CHECK_EXPR_RET(HI_SUCCESS != s32Ret,s32Ret,SAMPLE_SVP_ERR_LEVEL_ERROR,
        "Error,Malloc memory failed!\n");
    memset(pu8VirAddr,0, u32TotalSize);
    SAMPLE_COMM_SVP_FlushCache(u64PhyAddr,(void*)pu8VirAddr,u32TotalSize);

   /*set each tmp buffer addr*/
    pstSoftWareParam->stGetResultTmpBuf.u64PhyAddr = u64PhyAddr;
    pstSoftWareParam->stGetResultTmpBuf.u64VirAddr = (HI_U64)((HI_UL)pu8VirAddr);

    /*set result blob*/
    pstSoftWareParam->stDstRoi.enType = SVP_BLOB_TYPE_S32;
    pstSoftWareParam->stDstRoi.u64PhyAddr = u64PhyAddr+u32TmpBufTotalSize;
    pstSoftWareParam->stDstRoi.u64VirAddr = (HI_U64)((HI_UL)pu8VirAddr+u32TmpBufTotalSize);
    pstSoftWareParam->stDstRoi.u32Stride = SAMPLE_SVP_NNIE_ALIGN16(u32ClassNum*
        u32BboxNum*sizeof(HI_U32)*SAMPLE_SVP_NNIE_COORDI_NUM);
    pstSoftWareParam->stDstRoi.u32Num = 1;
    pstSoftWareParam->stDstRoi.unShape.stWhc.u32Chn = 1;
    pstSoftWareParam->stDstRoi.unShape.stWhc.u32Height = 1;
    pstSoftWareParam->stDstRoi.unShape.stWhc.u32Width = u32ClassNum*
        u32BboxNum*SAMPLE_SVP_NNIE_COORDI_NUM;

    pstSoftWareParam->stDstScore.enType = SVP_BLOB_TYPE_S32;
    pstSoftWareParam->stDstScore.u64PhyAddr = u64PhyAddr+u32TmpBufTotalSize+u32DstRoiSize;
    pstSoftWareParam->stDstScore.u64VirAddr = (HI_U64)((HI_UL)pu8VirAddr+u32TmpBufTotalSize+u32DstRoiSize);
    pstSoftWareParam->stDstScore.u32Stride = SAMPLE_SVP_NNIE_ALIGN16(u32ClassNum*
        u32BboxNum*sizeof(HI_U32));
    pstSoftWareParam->stDstScore.u32Num = 1;
    pstSoftWareParam->stDstScore.unShape.stWhc.u32Chn = 1;
    pstSoftWareParam->stDstScore.unShape.stWhc.u32Height = 1;
    pstSoftWareParam->stDstScore.unShape.stWhc.u32Width = u32ClassNum*u32BboxNum;

    pstSoftWareParam->stClassRoiNum.enType = SVP_BLOB_TYPE_S32;
    pstSoftWareParam->stClassRoiNum.u64PhyAddr = u64PhyAddr+u32TmpBufTotalSize+
        u32DstRoiSize+u32DstScoreSize;
    pstSoftWareParam->stClassRoiNum.u64VirAddr = (HI_U64)((HI_UL)pu8VirAddr+u32TmpBufTotalSize+
        u32DstRoiSize+u32DstScoreSize);
    pstSoftWareParam->stClassRoiNum.u32Stride = SAMPLE_SVP_NNIE_ALIGN16(u32ClassNum*sizeof(HI_U32));
    pstSoftWareParam->stClassRoiNum.u32Num = 1;
    pstSoftWareParam->stClassRoiNum.unShape.stWhc.u32Chn = 1;
    pstSoftWareParam->stClassRoiNum.unShape.stWhc.u32Height = 1;
    pstSoftWareParam->stClassRoiNum.unShape.stWhc.u32Width = u32ClassNum;

    return s32Ret;
}


/******************************************************************************
* function : Yolov1 init
******************************************************************************/
static HI_S32 SAMPLE_SVP_NNIE_Yolov1_ParamInit(SAMPLE_SVP_NNIE_CFG_S* pstCfg,
    SAMPLE_SVP_NNIE_PARAM_S *pstNnieParam, SAMPLE_SVP_NNIE_YOLOV1_SOFTWARE_PARAM_S* pstSoftWareParam)
{
    HI_S32 s32Ret = HI_SUCCESS;
    /*init hardware para*/
    s32Ret = SAMPLE_COMM_SVP_NNIE_ParamInit(pstCfg,pstNnieParam);
    SAMPLE_SVP_CHECK_EXPR_GOTO(HI_SUCCESS != s32Ret,INIT_FAIL_0,SAMPLE_SVP_ERR_LEVEL_ERROR,
        "Error(%#x),SAMPLE_COMM_SVP_NNIE_ParamInit failed!\n",s32Ret);

    /*init software para*/
    s32Ret = SAMPLE_SVP_NNIE_Yolov1_SoftwareInit(pstCfg,pstNnieParam,
        pstSoftWareParam);
    SAMPLE_SVP_CHECK_EXPR_GOTO(HI_SUCCESS != s32Ret,INIT_FAIL_0,SAMPLE_SVP_ERR_LEVEL_ERROR,
        "Error(%#x),SAMPLE_SVP_NNIE_Yolov1_SoftwareInit failed!\n",s32Ret);

    return s32Ret;
INIT_FAIL_0:
    s32Ret = SAMPLE_SVP_NNIE_Yolov1_Deinit(pstNnieParam,pstSoftWareParam,NULL);
    SAMPLE_SVP_CHECK_EXPR_RET(HI_SUCCESS != s32Ret,s32Ret,SAMPLE_SVP_ERR_LEVEL_ERROR,
            "Error(%#x),SAMPLE_SVP_NNIE_Yolov1_Deinit failed!\n",s32Ret);
    return HI_FAILURE;

}


/******************************************************************************
* function : show YOLOV1 sample(image 448x448 U8_C3)
******************************************************************************/
void SAMPLE_SVP_NNIE_Yolov1(void)
{
    HI_CHAR *pcSrcFile = "./data/nnie_image/rgb_planar/dog_bike_car_448x448.bgr";
    HI_CHAR *pcModelName = "./data/nnie_model/detection/inst_yolov1_cycle.wk";
    HI_U32 u32PicNum = 1;
    HI_FLOAT f32PrintResultThresh = 0.0f;
    HI_S32 s32Ret = HI_SUCCESS;
    SAMPLE_SVP_NNIE_CFG_S   stNnieCfg = {0};
    SAMPLE_SVP_NNIE_INPUT_DATA_INDEX_S stInputDataIdx = {0};
    SAMPLE_SVP_NNIE_PROCESS_SEG_INDEX_S stProcSegIdx = {0};
    SAMPLE_SVP_NIE_PERF_STAT_DEF_FRM_VAR()
    SAMPLE_SVP_NIE_PERF_STAT_DEF_VAR()

    /*Set configuration parameter*/
    f32PrintResultThresh = 0.3f;
    stNnieCfg.pszPic= pcSrcFile;
    stNnieCfg.u32MaxInputNum = u32PicNum; //max input image num in each batch
    stNnieCfg.u32MaxRoiNum = 0;
    stNnieCfg.aenNnieCoreId[0] = SVP_NNIE_ID_0;//set NNIE core

    /*Sys init*/
    SAMPLE_COMM_SVP_CheckSysInit();

    /*Yolov1 Load model*/
    SAMPLE_SVP_TRACE_INFO("Yolov1 Load model!\n");
    s32Ret = SAMPLE_COMM_SVP_NNIE_LoadModel(pcModelName,&s_stYolov1Model);
    SAMPLE_SVP_CHECK_EXPR_GOTO(HI_SUCCESS != s32Ret,YOLOV1_FAIL_0,SAMPLE_SVP_ERR_LEVEL_ERROR,
        "Error,SAMPLE_COMM_SVP_NNIE_LoadModel failed!\n");

    /*Yolov1 parameter initialization*/
    /*Yolov1 software parameters are set in SAMPLE_SVP_NNIE_Yolov1_SoftwareInit,
      if user has changed net struct, please make sure the parameter settings in
      SAMPLE_SVP_NNIE_Yolov1_SoftwareInit function are correct*/
    SAMPLE_SVP_TRACE_INFO("Yolov1 parameter initialization!\n");
    s_stYolov1NnieParam.pstModel = &s_stYolov1Model.stModel;
    s32Ret = SAMPLE_SVP_NNIE_Yolov1_ParamInit(&stNnieCfg,&s_stYolov1NnieParam,&s_stYolov1SoftwareParam);
    SAMPLE_SVP_CHECK_EXPR_GOTO(HI_SUCCESS != s32Ret,YOLOV1_FAIL_0,SAMPLE_SVP_ERR_LEVEL_ERROR,
        "Error,SAMPLE_SVP_NNIE_Yolov1_ParamInit failed!\n");

    /*Fill src data*/
    SAMPLE_SVP_TRACE_INFO("Yolov1 start!\n");

    SAMPLE_SVP_NNIE_PERF_STAT_BEGIN_LOOP()
    stInputDataIdx.u32SegIdx = 0;
    stInputDataIdx.u32NodeIdx = 0;
    s32Ret = SAMPLE_SVP_NNIE_FillSrcData(&stNnieCfg,&s_stYolov1NnieParam,&stInputDataIdx);
    SAMPLE_SVP_CHECK_EXPR_GOTO(HI_SUCCESS != s32Ret,YOLOV1_FAIL_0,SAMPLE_SVP_ERR_LEVEL_ERROR,
        "Error,SAMPLE_SVP_NNIE_FillSrcData failed!\n");


    /*NNIE process(process the 0-th segment)*/
    stProcSegIdx.u32SegIdx = 0;
    s32Ret = SAMPLE_SVP_NNIE_Forward(&s_stYolov1NnieParam,&stInputDataIdx,&stProcSegIdx,HI_TRUE);
    SAMPLE_SVP_CHECK_EXPR_GOTO(HI_SUCCESS != s32Ret,YOLOV1_FAIL_0,SAMPLE_SVP_ERR_LEVEL_ERROR,
        "Error,SAMPLE_SVP_NNIE_Forward failed!\n");
    SAMPLE_SVP_NNIE_PERF_STAT_YOLOV1_FORWARD_SRC_FLUSH_TIME()
    SAMPLE_SVP_NNIE_PERF_STAT_YOLOV1_FORWARD_PRE_DST_FLUSH_TIME()
    SAMPLE_SVP_NNIE_PERF_STAT_YOLOV1_FORWARD_AFTER_DST_FLUSH_TIME()
    SAMPLE_SVP_NNIE_PERF_STAT_YOLOV1_FORWARD_OP_TIME()

    /*software process*/
    /*if user has changed net struct, please make sure SAMPLE_SVP_NNIE_Yolov1_GetResult
     function input datas are correct*/
    SAMPLE_SVP_NNIE_PERF_STAT_BEGIN()
    s32Ret = SAMPLE_SVP_NNIE_Yolov1_GetResult(&s_stYolov1NnieParam,&s_stYolov1SoftwareParam);
    SAMPLE_SVP_CHECK_EXPR_GOTO(HI_SUCCESS != s32Ret,YOLOV1_FAIL_0,SAMPLE_SVP_ERR_LEVEL_ERROR,
        "Error,SAMPLE_SVP_NNIE_Yolov1_GetResult failed!\n");
    SAMPLE_SVP_NNIE_PERF_STAT_END()
    SAMPLE_SVP_NNIE_PERF_STAT_YOLOV1_GR_OP_TIME()

    /*print result, this sample has 21 classes:
     class 0:background     class 1:plane           class 2:bicycle
     class 3:bird           class 4:boat            class 5:bottle
     class 6:bus            class 7:car             class 8:cat
     class 9:chair          class10:cow             class11:diningtable
     class 12:dog           class13:horse           class14:motorbike
     class 15:person        class16:pottedplant     class17:sheep
     class 18:sofa          class19:train           class20:tvmonitor*/
     SAMPLE_SVP_TRACE_INFO("Yolov1 result:\n");
    (void)SAMPLE_SVP_NNIE_Detection_PrintResult(&s_stYolov1SoftwareParam.stDstScore,
        &s_stYolov1SoftwareParam.stDstRoi, &s_stYolov1SoftwareParam.stClassRoiNum,f32PrintResultThresh);

    SAMPLE_SVP_NNIE_PERF_STAT_END_LOOP()

    SAMPLE_SVP_NNIE_PERF_STAT_YOLOV1_PRINT()


YOLOV1_FAIL_0:
    SAMPLE_SVP_NNIE_Yolov1_Deinit(&s_stYolov1NnieParam,&s_stYolov1SoftwareParam,&s_stYolov1Model);
    SAMPLE_COMM_SVP_CheckSysExit();
}

/******************************************************************************
* function : Yolov1 sample signal handle
******************************************************************************/
void SAMPLE_SVP_NNIE_Yolov1_HandleSig(void)
{
    SAMPLE_SVP_NNIE_Yolov1_Deinit(&s_stYolov1NnieParam,&s_stYolov1SoftwareParam,&s_stYolov1Model);
    memset(&s_stYolov1NnieParam,0,sizeof(SAMPLE_SVP_NNIE_PARAM_S));
    memset(&s_stYolov1SoftwareParam,0,sizeof(SAMPLE_SVP_NNIE_YOLOV1_SOFTWARE_PARAM_S));
    memset(&s_stYolov1Model,0,sizeof(SAMPLE_SVP_NNIE_MODEL_S));
    SAMPLE_COMM_SVP_CheckSysExit();
}

/******************************************************************************
* function : Yolov2 software deinit
******************************************************************************/
static HI_S32 SAMPLE_SVP_NNIE_Yolov2_SoftwareDeinit(SAMPLE_SVP_NNIE_YOLOV2_SOFTWARE_PARAM_S* pstSoftWareParam)
{
    HI_S32 s32Ret = HI_SUCCESS;
    SAMPLE_SVP_CHECK_EXPR_RET(NULL== pstSoftWareParam,HI_INVALID_VALUE,SAMPLE_SVP_ERR_LEVEL_ERROR,
        "Error, pstSoftWareParam can't be NULL!\n");
    if(0!=pstSoftWareParam->stGetResultTmpBuf.u64PhyAddr && 0!=pstSoftWareParam->stGetResultTmpBuf.u64VirAddr)
    {
        SAMPLE_SVP_MMZ_FREE(pstSoftWareParam->stGetResultTmpBuf.u64PhyAddr,
            pstSoftWareParam->stGetResultTmpBuf.u64VirAddr);
        pstSoftWareParam->stGetResultTmpBuf.u64PhyAddr = 0;
        pstSoftWareParam->stGetResultTmpBuf.u64VirAddr = 0;
        pstSoftWareParam->stDstRoi.u64PhyAddr = 0;
        pstSoftWareParam->stDstRoi.u64VirAddr = 0;
        pstSoftWareParam->stDstScore.u64PhyAddr = 0;
        pstSoftWareParam->stDstScore.u64VirAddr = 0;
        pstSoftWareParam->stClassRoiNum.u64PhyAddr = 0;
        pstSoftWareParam->stClassRoiNum.u64VirAddr = 0;
    }
    return s32Ret;
}


/******************************************************************************
* function : Yolov2 Deinit
******************************************************************************/
static HI_S32 SAMPLE_SVP_NNIE_Yolov2_Deinit(SAMPLE_SVP_NNIE_PARAM_S *pstNnieParam,
    SAMPLE_SVP_NNIE_YOLOV2_SOFTWARE_PARAM_S* pstSoftWareParam,SAMPLE_SVP_NNIE_MODEL_S *pstNnieModel)
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
        s32Ret = SAMPLE_SVP_NNIE_Yolov2_SoftwareDeinit(pstSoftWareParam);
        SAMPLE_SVP_CHECK_EXPR_TRACE(HI_SUCCESS != s32Ret,SAMPLE_SVP_ERR_LEVEL_ERROR,
            "Error,SAMPLE_SVP_NNIE_Yolov2_SoftwareDeinit failed!\n");
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
* function : Yolov2 software para init
******************************************************************************/
static HI_S32 SAMPLE_SVP_NNIE_Yolov2_SoftwareInit(SAMPLE_SVP_NNIE_CFG_S* pstCfg,
    SAMPLE_SVP_NNIE_PARAM_S *pstNnieParam, SAMPLE_SVP_NNIE_YOLOV2_SOFTWARE_PARAM_S* pstSoftWareParam)
{
    HI_S32 s32Ret = HI_SUCCESS;
    HI_U32 u32ClassNum = 0;
    HI_U32 u32BboxNum = 0;
    HI_U32 u32TotalSize = 0;
    HI_U32 u32DstRoiSize = 0;
    HI_U32 u32DstScoreSize = 0;
    HI_U32 u32ClassRoiNumSize = 0;
    HI_U32 u32TmpBufTotalSize = 0;
    HI_U64 u64PhyAddr = 0;
    HI_U8* pu8VirAddr = NULL;

    pstSoftWareParam->u32OriImHeight = pstNnieParam->astSegData[0].astSrc[0].unShape.stWhc.u32Height;
    pstSoftWareParam->u32OriImWidth = pstNnieParam->astSegData[0].astSrc[0].unShape.stWhc.u32Width;
    pstSoftWareParam->u32BboxNumEachGrid = 5;
    pstSoftWareParam->u32ClassNum = 5;
    pstSoftWareParam->u32GridNumHeight = 13;
    pstSoftWareParam->u32GridNumWidth = 13;
    pstSoftWareParam->u32NmsThresh = (HI_U32)(0.3f*SAMPLE_SVP_NNIE_QUANT_BASE);
    pstSoftWareParam->u32ConfThresh = (HI_U32)(0.25f*SAMPLE_SVP_NNIE_QUANT_BASE);
    pstSoftWareParam->u32MaxRoiNum = 10;
    pstSoftWareParam->af32Bias[0] = 1.08;
    pstSoftWareParam->af32Bias[1] = 1.19;
    pstSoftWareParam->af32Bias[2] = 3.42;
    pstSoftWareParam->af32Bias[3] = 4.41;
    pstSoftWareParam->af32Bias[4] = 6.63;
    pstSoftWareParam->af32Bias[5] = 11.38;
    pstSoftWareParam->af32Bias[6] = 9.42;
    pstSoftWareParam->af32Bias[7] = 5.11;
    pstSoftWareParam->af32Bias[8] = 16.62;
    pstSoftWareParam->af32Bias[9] = 10.52;

    /*Malloc assist buffer memory*/
    u32ClassNum = pstSoftWareParam->u32ClassNum+1;
    u32BboxNum = pstSoftWareParam->u32BboxNumEachGrid*pstSoftWareParam->u32GridNumHeight*
        pstSoftWareParam->u32GridNumWidth;
    u32TmpBufTotalSize = SAMPLE_SVP_NNIE_Yolov2_GetResultTmpBuf(pstNnieParam,pstSoftWareParam);
    u32DstRoiSize = SAMPLE_SVP_NNIE_ALIGN16(u32ClassNum*u32BboxNum*SAMPLE_SVP_NNIE_COORDI_NUM);
    u32DstScoreSize = SAMPLE_SVP_NNIE_ALIGN16(u32ClassNum*u32BboxNum*sizeof(HI_U32));
    u32ClassRoiNumSize = SAMPLE_SVP_NNIE_ALIGN16(u32ClassNum*sizeof(HI_U32));
    u32TotalSize = u32TotalSize+u32DstRoiSize+u32DstScoreSize+u32ClassRoiNumSize+u32TmpBufTotalSize;
    s32Ret = SAMPLE_COMM_SVP_MallocCached("SAMPLE_YOLOV2_INIT",NULL,(HI_U64*)&u64PhyAddr,
        (void**)&pu8VirAddr,u32TotalSize);
    SAMPLE_SVP_CHECK_EXPR_RET(HI_SUCCESS != s32Ret,s32Ret,SAMPLE_SVP_ERR_LEVEL_ERROR,
        "Error,Malloc memory failed!\n");
    memset(pu8VirAddr,0, u32TotalSize);
    SAMPLE_COMM_SVP_FlushCache(u64PhyAddr,(void*)pu8VirAddr,u32TotalSize);

   /*set each tmp buffer addr*/
    pstSoftWareParam->stGetResultTmpBuf.u64PhyAddr = u64PhyAddr;
    pstSoftWareParam->stGetResultTmpBuf.u64VirAddr = (HI_U64)((HI_UL)pu8VirAddr);

    /*set result blob*/
    pstSoftWareParam->stDstRoi.enType = SVP_BLOB_TYPE_S32;
    pstSoftWareParam->stDstRoi.u64PhyAddr = u64PhyAddr+u32TmpBufTotalSize;
    pstSoftWareParam->stDstRoi.u64VirAddr = (HI_U64)((HI_UL)pu8VirAddr+u32TmpBufTotalSize);
    pstSoftWareParam->stDstRoi.u32Stride = SAMPLE_SVP_NNIE_ALIGN16(u32ClassNum*
        u32BboxNum*sizeof(HI_U32)*SAMPLE_SVP_NNIE_COORDI_NUM);
    pstSoftWareParam->stDstRoi.u32Num = 1;
    pstSoftWareParam->stDstRoi.unShape.stWhc.u32Chn = 1;
    pstSoftWareParam->stDstRoi.unShape.stWhc.u32Height = 1;
    pstSoftWareParam->stDstRoi.unShape.stWhc.u32Width = u32ClassNum*
        u32BboxNum*SAMPLE_SVP_NNIE_COORDI_NUM;

    pstSoftWareParam->stDstScore.enType = SVP_BLOB_TYPE_S32;
    pstSoftWareParam->stDstScore.u64PhyAddr = u64PhyAddr+u32TmpBufTotalSize+u32DstRoiSize;
    pstSoftWareParam->stDstScore.u64VirAddr = (HI_U64)((HI_UL)pu8VirAddr+u32TmpBufTotalSize+u32DstRoiSize);
    pstSoftWareParam->stDstScore.u32Stride = SAMPLE_SVP_NNIE_ALIGN16(u32ClassNum*
        u32BboxNum*sizeof(HI_U32));
    pstSoftWareParam->stDstScore.u32Num = 1;
    pstSoftWareParam->stDstScore.unShape.stWhc.u32Chn = 1;
    pstSoftWareParam->stDstScore.unShape.stWhc.u32Height = 1;
    pstSoftWareParam->stDstScore.unShape.stWhc.u32Width = u32ClassNum*u32BboxNum;

    pstSoftWareParam->stClassRoiNum.enType = SVP_BLOB_TYPE_S32;
    pstSoftWareParam->stClassRoiNum.u64PhyAddr = u64PhyAddr+u32TmpBufTotalSize+
        u32DstRoiSize+u32DstScoreSize;
    pstSoftWareParam->stClassRoiNum.u64VirAddr = (HI_U64)((HI_UL)pu8VirAddr+u32TmpBufTotalSize+
        u32DstRoiSize+u32DstScoreSize);
    pstSoftWareParam->stClassRoiNum.u32Stride = SAMPLE_SVP_NNIE_ALIGN16(u32ClassNum*sizeof(HI_U32));
    pstSoftWareParam->stClassRoiNum.u32Num = 1;
    pstSoftWareParam->stClassRoiNum.unShape.stWhc.u32Chn = 1;
    pstSoftWareParam->stClassRoiNum.unShape.stWhc.u32Height = 1;
    pstSoftWareParam->stClassRoiNum.unShape.stWhc.u32Width = u32ClassNum;

    return s32Ret;
}


/******************************************************************************
* function : Yolov1 init
******************************************************************************/
static HI_S32 SAMPLE_SVP_NNIE_Yolov2_ParamInit(SAMPLE_SVP_NNIE_CFG_S* pstCfg,
    SAMPLE_SVP_NNIE_PARAM_S *pstNnieParam, SAMPLE_SVP_NNIE_YOLOV2_SOFTWARE_PARAM_S* pstSoftWareParam)
{
    HI_S32 s32Ret = HI_SUCCESS;
    /*init hardware para*/
    s32Ret = SAMPLE_COMM_SVP_NNIE_ParamInit(pstCfg,pstNnieParam);
    SAMPLE_SVP_CHECK_EXPR_GOTO(HI_SUCCESS != s32Ret,INIT_FAIL_0,SAMPLE_SVP_ERR_LEVEL_ERROR,
        "Error(%#x),SAMPLE_COMM_SVP_NNIE_ParamInit failed!\n",s32Ret);

    /*init software para*/
    s32Ret = SAMPLE_SVP_NNIE_Yolov2_SoftwareInit(pstCfg,pstNnieParam,
        pstSoftWareParam);
    SAMPLE_SVP_CHECK_EXPR_GOTO(HI_SUCCESS != s32Ret,INIT_FAIL_0,SAMPLE_SVP_ERR_LEVEL_ERROR,
        "Error(%#x),SAMPLE_SVP_NNIE_Yolov1_SoftwareInit failed!\n",s32Ret);

    return s32Ret;
INIT_FAIL_0:
    s32Ret = SAMPLE_SVP_NNIE_Yolov2_Deinit(pstNnieParam,pstSoftWareParam,NULL);
    SAMPLE_SVP_CHECK_EXPR_RET(HI_SUCCESS != s32Ret,s32Ret,SAMPLE_SVP_ERR_LEVEL_ERROR,
            "Error(%#x),SAMPLE_SVP_NNIE_Yolov1_Deinit failed!\n",s32Ret);
    return HI_FAILURE;

}


/******************************************************************************
* function : show YOLOV2 sample(image 416x416 U8_C3)
******************************************************************************/
void SAMPLE_SVP_NNIE_Yolov2(void)
{
    HI_CHAR *pcSrcFile = "./data/nnie_image/rgb_planar/street_cars_416x416.bgr";
    HI_CHAR *pcModelName = "./data/nnie_model/detection/inst_yolov2_cycle.wk";
    HI_U32 u32PicNum = 1;
    HI_FLOAT f32PrintResultThresh = 0.0f;
    HI_S32 s32Ret = HI_SUCCESS;
    SAMPLE_SVP_NNIE_CFG_S   stNnieCfg = {0};
    SAMPLE_SVP_NNIE_INPUT_DATA_INDEX_S stInputDataIdx = {0};
    SAMPLE_SVP_NNIE_PROCESS_SEG_INDEX_S stProcSegIdx = {0};
    SAMPLE_SVP_NIE_PERF_STAT_DEF_FRM_VAR()
    SAMPLE_SVP_NIE_PERF_STAT_DEF_VAR()

    /*Set configuration parameter*/
    f32PrintResultThresh = 0.25f;
    stNnieCfg.pszPic= pcSrcFile;
    stNnieCfg.u32MaxInputNum = u32PicNum; //max input image num in each batch
    stNnieCfg.u32MaxRoiNum = 0;
    stNnieCfg.aenNnieCoreId[0] = SVP_NNIE_ID_0;//set NNIE core

    /*Sys init*/
    SAMPLE_COMM_SVP_CheckSysInit();

    /*Yolov2 Load model*/
    SAMPLE_SVP_TRACE_INFO("Yolov2 Load model!\n");
    s32Ret = SAMPLE_COMM_SVP_NNIE_LoadModel(pcModelName,&s_stYolov2Model);
    SAMPLE_SVP_CHECK_EXPR_GOTO(HI_SUCCESS != s32Ret,YOLOV2_FAIL_0,SAMPLE_SVP_ERR_LEVEL_ERROR,
        "Error,SAMPLE_COMM_SVP_NNIE_LoadModel failed!\n");

    /*Yolov2 parameter initialization*/
    /*Yolov2 software parameters are set in SAMPLE_SVP_NNIE_Yolov2_SoftwareInit,
      if user has changed net struct, please make sure the parameter settings in
      SAMPLE_SVP_NNIE_Yolov2_SoftwareInit function are correct*/
    SAMPLE_SVP_TRACE_INFO("Yolov2 parameter initialization!\n");
    s_stYolov2NnieParam.pstModel = &s_stYolov2Model.stModel;
    s32Ret = SAMPLE_SVP_NNIE_Yolov2_ParamInit(&stNnieCfg,&s_stYolov2NnieParam,&s_stYolov2SoftwareParam);
    SAMPLE_SVP_CHECK_EXPR_GOTO(HI_SUCCESS != s32Ret,YOLOV2_FAIL_0,SAMPLE_SVP_ERR_LEVEL_ERROR,
        "Error,SAMPLE_SVP_NNIE_Yolov2_ParamInit failed!\n");

    /*Fill src data*/
    SAMPLE_SVP_TRACE_INFO("Yolov2 start!\n");
    SAMPLE_SVP_NNIE_PERF_STAT_BEGIN_LOOP()
    stInputDataIdx.u32SegIdx = 0;
    stInputDataIdx.u32NodeIdx = 0;
    s32Ret = SAMPLE_SVP_NNIE_FillSrcData(&stNnieCfg,&s_stYolov2NnieParam,&stInputDataIdx);
    SAMPLE_SVP_CHECK_EXPR_GOTO(HI_SUCCESS != s32Ret,YOLOV2_FAIL_0,SAMPLE_SVP_ERR_LEVEL_ERROR,
        "Error,SAMPLE_SVP_NNIE_FillSrcData failed!\n");

    /*NNIE process(process the 0-th segment)*/
    stProcSegIdx.u32SegIdx = 0;
    s32Ret = SAMPLE_SVP_NNIE_Forward(&s_stYolov2NnieParam,&stInputDataIdx,&stProcSegIdx,HI_TRUE);
    SAMPLE_SVP_CHECK_EXPR_GOTO(HI_SUCCESS != s32Ret,YOLOV2_FAIL_0,SAMPLE_SVP_ERR_LEVEL_ERROR,
        "Error,SAMPLE_SVP_NNIE_Forward failed!\n");
    SAMPLE_SVP_NNIE_PERF_STAT_YOLOV2_FORWARD_SRC_FLUSH_TIME()
    SAMPLE_SVP_NNIE_PERF_STAT_YOLOV2_FORWARD_PRE_DST_FLUSH_TIME()
    SAMPLE_SVP_NNIE_PERF_STAT_YOLOV2_FORWARD_AFTER_DST_FLUSH_TIME()
    SAMPLE_SVP_NNIE_PERF_STAT_YOLOV2_FORWARD_OP_TIME()

    /*Software process*/
    /*if user has changed net struct, please make sure SAMPLE_SVP_NNIE_Yolov2_GetResult
     function input datas are correct*/
    SAMPLE_SVP_NNIE_PERF_STAT_BEGIN()
    s32Ret = SAMPLE_SVP_NNIE_Yolov2_GetResult(&s_stYolov2NnieParam,&s_stYolov2SoftwareParam);
    SAMPLE_SVP_CHECK_EXPR_GOTO(HI_SUCCESS != s32Ret,YOLOV2_FAIL_0,SAMPLE_SVP_ERR_LEVEL_ERROR,
        "Error,SAMPLE_SVP_NNIE_Yolov2_GetResult failed!\n");
    SAMPLE_SVP_NNIE_PERF_STAT_END()
    SAMPLE_SVP_NNIE_PERF_STAT_YOLOV2_GR_OP_TIME()

    /*print result, this sample has 6 classes:
     class 0:background     class 1:Carclass           class 2:Vanclass
     class 3:Truckclass     class 4:Pedestrianclass    class 5:Cyclist*/
     SAMPLE_SVP_TRACE_INFO("Yolov2 result:\n");
    (void)SAMPLE_SVP_NNIE_Detection_PrintResult(&s_stYolov2SoftwareParam.stDstScore,
        &s_stYolov2SoftwareParam.stDstRoi, &s_stYolov2SoftwareParam.stClassRoiNum,f32PrintResultThresh);

    SAMPLE_SVP_NNIE_PERF_STAT_END_LOOP()

    SAMPLE_SVP_NNIE_PERF_STAT_YOLOV2_PRINT()


YOLOV2_FAIL_0:
    SAMPLE_SVP_NNIE_Yolov2_Deinit(&s_stYolov2NnieParam,&s_stYolov2SoftwareParam,&s_stYolov2Model);
    SAMPLE_COMM_SVP_CheckSysExit();
}

/******************************************************************************
* function : Yolov2 sample signal handle
******************************************************************************/
void SAMPLE_SVP_NNIE_Yolov2_HandleSig(void)
{
    SAMPLE_SVP_NNIE_Yolov2_Deinit(&s_stYolov2NnieParam,&s_stYolov2SoftwareParam,&s_stYolov2Model);
    memset(&s_stYolov2NnieParam,0,sizeof(SAMPLE_SVP_NNIE_PARAM_S));
    memset(&s_stYolov2SoftwareParam,0,sizeof(SAMPLE_SVP_NNIE_YOLOV2_SOFTWARE_PARAM_S));
    memset(&s_stYolov2Model,0,sizeof(SAMPLE_SVP_NNIE_MODEL_S));
    SAMPLE_COMM_SVP_CheckSysExit();
}


/******************************************************************************
* function : Lstm Deinit
******************************************************************************/
static HI_S32 SAMPLE_SVP_NNIE_Lstm_Deinit(SAMPLE_SVP_NNIE_PARAM_S *pstNnieParamm,
    SAMPLE_SVP_NNIE_MODEL_S *pstNnieModel)
{
    HI_S32 s32Ret = HI_SUCCESS;
    /*hardware deinit*/
    if(pstNnieParamm!=NULL)
    {
        s32Ret = SAMPLE_COMM_SVP_NNIE_ParamDeinit(pstNnieParamm);
        SAMPLE_SVP_CHECK_EXPR_TRACE(HI_SUCCESS != s32Ret,SAMPLE_SVP_ERR_LEVEL_ERROR,
            "Error,SAMPLE_COMM_SVP_NNIE_ParamDeinit failed!\n");
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
* function : Lstm init
******************************************************************************/
static HI_S32 SAMPLE_SVP_NNIE_Lstm_ParamInit(SAMPLE_SVP_NNIE_CFG_S* pstNnieCfg,
    SAMPLE_SVP_NNIE_PARAM_S *pstLstmPara)
{
    HI_S32 s32Ret = HI_SUCCESS;
    /*init hardware para*/
    s32Ret = SAMPLE_COMM_SVP_NNIE_ParamInit(pstNnieCfg,pstLstmPara);
    SAMPLE_SVP_CHECK_EXPR_GOTO(HI_SUCCESS != s32Ret,INIT_FAIL_0,SAMPLE_SVP_ERR_LEVEL_ERROR,
        "Error(%#x),SAMPLE_COMM_SVP_NNIE_ParamInit failed!\n",s32Ret);
    return s32Ret;
INIT_FAIL_0:
    s32Ret = SAMPLE_SVP_NNIE_Lstm_Deinit(pstLstmPara,NULL);
    SAMPLE_SVP_CHECK_EXPR_RET(HI_SUCCESS != s32Ret,s32Ret,SAMPLE_SVP_ERR_LEVEL_ERROR,
            "Error(%#x),SAMPLE_SVP_NNIE_Lstm_Deinit failed!\n",s32Ret);
    return HI_FAILURE;

}

/******************************************************************************
* function : show Lstm sample(vector)
******************************************************************************/
void SAMPLE_SVP_NNIE_Lstm(void)
{
    HI_CHAR *apcSrcFile[3] = {"./data/nnie_image/vector/Seq.SEQ_S32",
                              "./data/nnie_image/vector/Vec1.VEC_S32",
                              "./data/nnie_image/vector/Vec2.VEC_S32"};
    HI_CHAR *pchModelName = "./data/nnie_model/recurrent/lstm_3_3.wk";
    HI_U8* pu8VirAddr = NULL;
    HI_U32 u32SegNum = 0;
    HI_U32 u32Step = 0;
    HI_U32 u32Offset = 0;
    HI_U32 u32TotalSize = 0;
    HI_U32 i = 0, j = 0;
    HI_S32 s32Ret = HI_SUCCESS;
    SAMPLE_SVP_NNIE_CFG_S   stNnieCfg = {0};
    SAMPLE_SVP_NNIE_INPUT_DATA_INDEX_S stInputDataIdx = {0};
    SAMPLE_SVP_NNIE_PROCESS_SEG_INDEX_S stProcSegIdx = {0};

    /*Set configuration parameter*/
    stNnieCfg.u32MaxInputNum = 16; //max input data num in each batch
    stNnieCfg.u32MaxRoiNum = 0;
    stNnieCfg.aenNnieCoreId[0] = SVP_NNIE_ID_0;//set NNIE core
    u32Step = 20; //time step

    /*Sys init*/
    SAMPLE_COMM_SVP_CheckSysInit();

    /*Lstm Load model*/
    SAMPLE_SVP_TRACE_INFO("Lstm Load model!\n");
    s32Ret = SAMPLE_COMM_SVP_NNIE_LoadModel(pchModelName,&s_stLstmModel);
    SAMPLE_SVP_CHECK_EXPR_GOTO(HI_SUCCESS != s32Ret,LSTM_FAIL_0,SAMPLE_SVP_ERR_LEVEL_ERROR,
        "Error,SAMPLE_COMM_SVP_NNIE_LoadModel failed!\n");

    /*Lstm step initialization*/
    u32SegNum = s_stLstmModel.stModel.u32NetSegNum;
    u32TotalSize = stNnieCfg.u32MaxInputNum*sizeof(HI_S32)*u32SegNum*2;
    s32Ret = SAMPLE_COMM_SVP_MallocMem("SVP_NNIE_STEP",NULL,(HI_U64*)&s_stLstmNnieParam.stStepBuf.u64PhyAddr,
        (void**)&pu8VirAddr,u32TotalSize);
    SAMPLE_SVP_CHECK_EXPR_GOTO(HI_SUCCESS != s32Ret,LSTM_FAIL_0,SAMPLE_SVP_ERR_LEVEL_ERROR,
        "Error,Malloc memory failed!\n");
    /*Get step virtual addr*/
    s_stLstmNnieParam.stStepBuf.u64VirAddr = (HI_U64)((HI_UL)pu8VirAddr);
    for(i = 0; i < u32SegNum*SAMPLE_SVP_NNIE_EACH_SEG_STEP_ADDR_NUM; i++)
    {
        stNnieCfg.au64StepVirAddr[i] = s_stLstmNnieParam.stStepBuf.u64VirAddr +
            i*stNnieCfg.u32MaxInputNum*sizeof(HI_S32);
    }
    /*Set step value, in this sample, the step values are set to be 20,
    if user has changed input network, please set correct step
    values according to the input network*/
    for(i = 0; i < u32SegNum; i++)
    {
        u32Offset = i*SAMPLE_SVP_NNIE_EACH_SEG_STEP_ADDR_NUM;
        for(j = 0; j < stNnieCfg.u32MaxInputNum; j++)
        {
            *((HI_U32*)((HI_UL)stNnieCfg.au64StepVirAddr[u32Offset])+j) = u32Step;//step of input x_t
            *((HI_U32*)((HI_UL)stNnieCfg.au64StepVirAddr[u32Offset+1])+j) = u32Step;//step of output h_t
        }
    }

    /*Lstm parameter initialization */
    SAMPLE_SVP_TRACE_INFO("Lstm parameter initialization!\n");
    s_stLstmNnieParam.pstModel = &(s_stLstmModel.stModel);
    s32Ret = SAMPLE_SVP_NNIE_Lstm_ParamInit(&stNnieCfg,&s_stLstmNnieParam);
    SAMPLE_SVP_CHECK_EXPR_GOTO(HI_SUCCESS != s32Ret,LSTM_FAIL_0,SAMPLE_SVP_ERR_LEVEL_ERROR,
        "Error,SAMPLE_SVP_NNIE_Lstm_ParamInit failed!\n");

    /*Fill src data, in this sample, the 0-th seg is lstm network,if user has
     changed input network,please make sure the value of stInputDataIdx.u32SegIdx
     is correct*/
    SAMPLE_SVP_TRACE_INFO("Lstm start!\n");
    stInputDataIdx.u32SegIdx = 0;
    for(i = 0; i < s_stLstmNnieParam.pstModel->astSeg[stInputDataIdx.u32SegIdx].u16SrcNum; i++)
    {
        stNnieCfg.pszPic = apcSrcFile[i];
        stInputDataIdx.u32NodeIdx = i;
        s32Ret = SAMPLE_SVP_NNIE_FillSrcData(&stNnieCfg,&s_stLstmNnieParam,&stInputDataIdx);
        SAMPLE_SVP_CHECK_EXPR_GOTO(HI_SUCCESS != s32Ret,LSTM_FAIL_0,SAMPLE_SVP_ERR_LEVEL_ERROR,
            "Error,SAMPLE_SVP_NNIE_FillSrcData failed!\n");
    }

    /*NNIE process(process the 0-th segment)*/
    stInputDataIdx.u32SegIdx = 0;
    stInputDataIdx.u32NodeIdx = 0;
    stProcSegIdx.u32SegIdx = 0;
    s32Ret = SAMPLE_SVP_NNIE_Forward(&s_stLstmNnieParam,&stInputDataIdx,&stProcSegIdx,HI_TRUE);
    SAMPLE_SVP_CHECK_EXPR_GOTO(HI_SUCCESS != s32Ret,LSTM_FAIL_0,SAMPLE_SVP_ERR_LEVEL_ERROR,
        "Error,SAMPLE_SVP_NNIE_Forward failed!\n");

    /*print report result*/
    s32Ret = SAMPLE_SVP_NNIE_PrintReportResult(&s_stLstmNnieParam);
    SAMPLE_SVP_CHECK_EXPR_GOTO(HI_SUCCESS != s32Ret,LSTM_FAIL_0,SAMPLE_SVP_ERR_LEVEL_ERROR,
        "Error,SAMPLE_SVP_NNIE_PrintReportResult failed!\n");

    SAMPLE_SVP_TRACE_INFO("Lstm is successfully processed!\n");

LSTM_FAIL_0:
    SAMPLE_SVP_NNIE_Lstm_Deinit(&s_stLstmNnieParam,&s_stLstmModel);
    SAMPLE_COMM_SVP_CheckSysExit();
}

/******************************************************************************
* function : Lstm sample signal handle
******************************************************************************/
void SAMPLE_SVP_NNIE_Lstm_HandleSig(void)
{
    SAMPLE_SVP_NNIE_Lstm_Deinit(&s_stLstmNnieParam,&s_stLstmModel);
    memset(&s_stLstmNnieParam,0,sizeof(SAMPLE_SVP_NNIE_PARAM_S));
    memset(&s_stLstmModel,0,sizeof(SAMPLE_SVP_NNIE_MODEL_S));
    SAMPLE_COMM_SVP_CheckSysExit();
}

/******************************************************************************
* function : Pavnet software deinit
******************************************************************************/
static HI_S32 SAMPLE_SVP_NNIE_Pvanet_SoftwareDeinit(SAMPLE_SVP_NNIE_FASTERRCNN_SOFTWARE_PARAM_S* pstSoftWareParam)
{
    HI_S32 s32Ret = HI_SUCCESS;
    SAMPLE_SVP_CHECK_EXPR_RET(NULL == pstSoftWareParam,HI_INVALID_VALUE,SAMPLE_SVP_ERR_LEVEL_ERROR,
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
* function : Pvanet Deinit
******************************************************************************/
static HI_S32 SAMPLE_SVP_NNIE_Pvanet_Deinit(SAMPLE_SVP_NNIE_PARAM_S *pstNnieParam,
    SAMPLE_SVP_NNIE_FASTERRCNN_SOFTWARE_PARAM_S* pstSoftWareParam,SAMPLE_SVP_NNIE_MODEL_S* pstNnieModel)
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
        s32Ret = SAMPLE_SVP_NNIE_Pvanet_SoftwareDeinit(pstSoftWareParam);
        SAMPLE_SVP_CHECK_EXPR_TRACE(HI_SUCCESS != s32Ret,SAMPLE_SVP_ERR_LEVEL_ERROR,
            "Error,SAMPLE_SVP_NNIE_FasterRcnn_SoftwareDeinit failed!\n");
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
* function : Pvanet software para init
******************************************************************************/
static HI_S32 SAMPLE_SVP_NNIE_Pvanet_SoftwareInit(SAMPLE_SVP_NNIE_CFG_S* pstCfg,
    SAMPLE_SVP_NNIE_PARAM_S *pstNnieParam, SAMPLE_SVP_NNIE_FASTERRCNN_SOFTWARE_PARAM_S* pstSoftWareParam)
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

    /*RPN parameter init*/
    pstSoftWareParam->u32MaxRoiNum = pstCfg->u32MaxRoiNum;
    pstSoftWareParam->u32ClassNum = 21;
    pstSoftWareParam->u32NumRatioAnchors = 7;
    pstSoftWareParam->u32NumScaleAnchors =6;
    pstSoftWareParam->au32Ratios[0] = (HI_S32)(0.333 * SAMPLE_SVP_QUANT_BASE);
    pstSoftWareParam->au32Ratios[1] = (HI_S32)(0.5 * SAMPLE_SVP_QUANT_BASE);
    pstSoftWareParam->au32Ratios[2] = (HI_S32)(0.667 * SAMPLE_SVP_QUANT_BASE);
    pstSoftWareParam->au32Ratios[3] = (HI_S32)(1 * SAMPLE_SVP_QUANT_BASE);
    pstSoftWareParam->au32Ratios[4] = (HI_S32)(1.5 * SAMPLE_SVP_QUANT_BASE);
    pstSoftWareParam->au32Ratios[5] = (HI_S32)(2 * SAMPLE_SVP_QUANT_BASE);
    pstSoftWareParam->au32Ratios[6] = (HI_S32)(3 * SAMPLE_SVP_QUANT_BASE);

    pstSoftWareParam->au32Scales[0] = 2 * SAMPLE_SVP_QUANT_BASE;
    pstSoftWareParam->au32Scales[1] = 3 * SAMPLE_SVP_QUANT_BASE;
    pstSoftWareParam->au32Scales[2] = 5 * SAMPLE_SVP_QUANT_BASE;
    pstSoftWareParam->au32Scales[3] = 9 * SAMPLE_SVP_QUANT_BASE;
    pstSoftWareParam->au32Scales[4] = 16 * SAMPLE_SVP_QUANT_BASE;
    pstSoftWareParam->au32Scales[5] = 32 * SAMPLE_SVP_QUANT_BASE;



    /*set origin image height & width from src[0] shape*/
    pstSoftWareParam->u32OriImHeight = pstNnieParam->astSegData[0].astSrc[0].unShape.stWhc.u32Height;
    pstSoftWareParam->u32OriImWidth = pstNnieParam->astSegData[0].astSrc[0].unShape.stWhc.u32Width;

    pstSoftWareParam->u32MinSize = 16;
    pstSoftWareParam->u32SpatialScale = (HI_U32)(0.0625 * SAMPLE_SVP_QUANT_BASE);
    pstSoftWareParam->u32NmsThresh = (HI_U32)(0.7 * SAMPLE_SVP_QUANT_BASE);
    pstSoftWareParam->u32FilterThresh = 0;
    pstSoftWareParam->u32ValidNmsThresh = (HI_U32)(0.3 * SAMPLE_SVP_QUANT_BASE);
    pstSoftWareParam->u32NumBeforeNms = 12000;
    pstSoftWareParam->u32MaxRoiNum = 200;


    for(i = 0; i < pstSoftWareParam->u32ClassNum; i++)
    {
        pstSoftWareParam->au32ConfThresh[i] = 1;
    }

    pstSoftWareParam->stRpnBbox.enType = SVP_BLOB_TYPE_S32;
    pstSoftWareParam->stRpnBbox.unShape.stWhc.u32Chn = 1;
    pstSoftWareParam->stRpnBbox.unShape.stWhc.u32Height = pstCfg->u32MaxRoiNum;
    pstSoftWareParam->stRpnBbox.unShape.stWhc.u32Width = SAMPLE_SVP_COORDI_NUM;
    pstSoftWareParam->stRpnBbox.u32Stride = SAMPLE_SVP_NNIE_ALIGN16(SAMPLE_SVP_COORDI_NUM*sizeof(HI_U32));
    pstSoftWareParam->stRpnBbox.u32Num = 1;
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
    /*calculate software mem size*/
    u32ClassNum = pstSoftWareParam->u32ClassNum;
    u32RpnTmpBufSize = SAMPLE_SVP_NNIE_RpnTmpBufSize(pstSoftWareParam->u32NumRatioAnchors,
        pstSoftWareParam->u32NumScaleAnchors,pstSoftWareParam->au32ConvHeight[0],
        pstSoftWareParam->au32ConvWidth[0]);
    u32RpnTmpBufSize = SAMPLE_SVP_NNIE_ALIGN16(u32RpnTmpBufSize);
    u32RpnBboxBufSize = pstSoftWareParam->stRpnBbox.u32Num*
        pstSoftWareParam->stRpnBbox.unShape.stWhc.u32Height*pstSoftWareParam->stRpnBbox.u32Stride;
    u32GetResultTmpBufSize = SAMPLE_SVP_NNIE_Pvanet_GetResultTmpBufSize(pstCfg->u32MaxRoiNum,u32ClassNum);
    u32GetResultTmpBufSize = SAMPLE_SVP_NNIE_ALIGN16(u32GetResultTmpBufSize);
    u32DstRoiSize = SAMPLE_SVP_NNIE_ALIGN16(u32ClassNum*pstCfg->u32MaxRoiNum*sizeof(HI_U32)*SAMPLE_SVP_COORDI_NUM);
    u32DstScoreSize = SAMPLE_SVP_NNIE_ALIGN16(u32ClassNum*pstCfg->u32MaxRoiNum*sizeof(HI_U32));
    u32ClassRoiNumSize = SAMPLE_SVP_NNIE_ALIGN16(u32ClassNum*sizeof(HI_U32));
    u32TotalSize = u32RpnTmpBufSize + u32RpnBboxBufSize + u32GetResultTmpBufSize + u32DstRoiSize +
        u32DstScoreSize + u32ClassRoiNumSize;

    /*malloc mem*/
    s32Ret = SAMPLE_COMM_SVP_MallocCached("SAMPLE_Pvanet_INIT",NULL,(HI_U64*)&u64PhyAddr,
        (void**)&pu8VirAddr,u32TotalSize);
    SAMPLE_SVP_CHECK_EXPR_RET(HI_SUCCESS != s32Ret,s32Ret,SAMPLE_SVP_ERR_LEVEL_ERROR,
        "Error,Malloc memory failed!\n");
    memset(pu8VirAddr,0, u32TotalSize);
    SAMPLE_COMM_SVP_FlushCache(u64PhyAddr,(void*)pu8VirAddr,u32TotalSize);

    /*set addr*/
    pstSoftWareParam->stRpnTmpBuf.u64PhyAddr = u64PhyAddr;
    pstSoftWareParam->stRpnTmpBuf.u64VirAddr = (HI_U64)((HI_UL)pu8VirAddr);
    pstSoftWareParam->stRpnTmpBuf.u32Size = u32RpnTmpBufSize;

    pstSoftWareParam->stRpnBbox.u64PhyAddr = u64PhyAddr+u32RpnTmpBufSize;
    pstSoftWareParam->stRpnBbox.u64VirAddr = (HI_U64)((HI_UL)pu8VirAddr)+u32RpnTmpBufSize;

    pstSoftWareParam->stGetResultTmpBuf.u64PhyAddr = u64PhyAddr+u32RpnBboxBufSize+u32RpnTmpBufSize;
    pstSoftWareParam->stGetResultTmpBuf.u64VirAddr = (HI_U64)((HI_UL)pu8VirAddr+u32RpnBboxBufSize+u32RpnTmpBufSize);
    pstSoftWareParam->stGetResultTmpBuf.u32Size = u32GetResultTmpBufSize;

    pstSoftWareParam->stDstRoi.enType = SVP_BLOB_TYPE_S32;
    pstSoftWareParam->stDstRoi.u64PhyAddr = u64PhyAddr+u32RpnBboxBufSize+u32RpnTmpBufSize+u32GetResultTmpBufSize;
    pstSoftWareParam->stDstRoi.u64VirAddr = (HI_U64)((HI_UL)pu8VirAddr+u32RpnBboxBufSize+u32RpnTmpBufSize+u32GetResultTmpBufSize);
    pstSoftWareParam->stDstRoi.u32Stride = SAMPLE_SVP_NNIE_ALIGN16(u32ClassNum*pstSoftWareParam->u32MaxRoiNum*sizeof(HI_U32)*SAMPLE_SVP_COORDI_NUM);
    pstSoftWareParam->stDstRoi.u32Num = 1;
    pstSoftWareParam->stDstRoi.unShape.stWhc.u32Chn = 1;
    pstSoftWareParam->stDstRoi.unShape.stWhc.u32Height = 1;
    pstSoftWareParam->stDstRoi.unShape.stWhc.u32Width = u32ClassNum*pstSoftWareParam->u32MaxRoiNum*SAMPLE_SVP_COORDI_NUM;

    pstSoftWareParam->stDstScore.enType = SVP_BLOB_TYPE_S32;
    pstSoftWareParam->stDstScore.u64PhyAddr = u64PhyAddr+u32RpnBboxBufSize+u32RpnTmpBufSize+u32GetResultTmpBufSize+u32DstRoiSize;
    pstSoftWareParam->stDstScore.u64VirAddr = (HI_U64)((HI_UL)pu8VirAddr+u32RpnBboxBufSize+u32RpnTmpBufSize+u32GetResultTmpBufSize+u32DstRoiSize);
    pstSoftWareParam->stDstScore.u32Stride = SAMPLE_SVP_NNIE_ALIGN16(u32ClassNum*pstSoftWareParam->u32MaxRoiNum*sizeof(HI_U32));
    pstSoftWareParam->stDstScore.u32Num = 1;
    pstSoftWareParam->stDstScore.unShape.stWhc.u32Chn = 1;
    pstSoftWareParam->stDstScore.unShape.stWhc.u32Height = 1;
    pstSoftWareParam->stDstScore.unShape.stWhc.u32Width = u32ClassNum*pstSoftWareParam->u32MaxRoiNum;

    pstSoftWareParam->stClassRoiNum.enType = SVP_BLOB_TYPE_S32;
    pstSoftWareParam->stClassRoiNum.u64PhyAddr = u64PhyAddr+u32RpnBboxBufSize+u32RpnTmpBufSize+u32GetResultTmpBufSize+u32DstRoiSize+u32DstScoreSize;
    pstSoftWareParam->stClassRoiNum.u64VirAddr = (HI_U64)((HI_UL)pu8VirAddr+u32RpnBboxBufSize+u32RpnTmpBufSize+u32GetResultTmpBufSize+u32DstRoiSize+u32DstScoreSize);
    pstSoftWareParam->stClassRoiNum.u32Stride = SAMPLE_SVP_NNIE_ALIGN16(u32ClassNum*sizeof(HI_U32));
    pstSoftWareParam->stClassRoiNum.u32Num = 1;
    pstSoftWareParam->stClassRoiNum.unShape.stWhc.u32Chn = 1;
    pstSoftWareParam->stClassRoiNum.unShape.stWhc.u32Height = 1;
    pstSoftWareParam->stClassRoiNum.unShape.stWhc.u32Width = u32ClassNum;

    return s32Ret;
}

/******************************************************************************
* function : Pvanet parameter initialization
******************************************************************************/
static HI_S32 SAMPLE_SVP_NNIE_Pvanet_ParamInit(SAMPLE_SVP_NNIE_CFG_S* pstFasterRcnnCfg,
    SAMPLE_SVP_NNIE_PARAM_S *pstNnieParam, SAMPLE_SVP_NNIE_FASTERRCNN_SOFTWARE_PARAM_S* pstSoftWareParam)
{
    HI_S32 s32Ret = HI_SUCCESS;
    /*init hardware parameter*/
    s32Ret = SAMPLE_COMM_SVP_NNIE_ParamInit(pstFasterRcnnCfg,pstNnieParam);
    SAMPLE_SVP_CHECK_EXPR_GOTO(HI_SUCCESS != s32Ret,INIT_FAIL_0,SAMPLE_SVP_ERR_LEVEL_ERROR,
        "Error(%#x),SAMPLE_COMM_SVP_NNIE_ParamInit failed!\n",s32Ret);

    /*init software parameter*/
    s32Ret = SAMPLE_SVP_NNIE_Pvanet_SoftwareInit(pstFasterRcnnCfg,pstNnieParam,
        pstSoftWareParam);
    SAMPLE_SVP_CHECK_EXPR_GOTO(HI_SUCCESS != s32Ret,INIT_FAIL_0,SAMPLE_SVP_ERR_LEVEL_ERROR,
        "Error(%#x),SAMPLE_SVP_NNIE_Pvanet_SoftwareInit failed!\n",s32Ret);

    return s32Ret;
INIT_FAIL_0:
    s32Ret = SAMPLE_SVP_NNIE_FasterRcnn_Deinit(pstNnieParam,pstSoftWareParam,NULL);
    SAMPLE_SVP_CHECK_EXPR_RET(HI_SUCCESS != s32Ret,s32Ret,SAMPLE_SVP_ERR_LEVEL_ERROR,
            "Error(%#x),SAMPLE_SVP_NNIE_FasterRcnn_Deinit failed!\n",s32Ret);
    return HI_FAILURE;
}

/******************************************************************************
* function : show Pvanet fasterRcnn sample(image 224x224 U8_C3)
******************************************************************************/
void SAMPLE_SVP_NNIE_Pvanet(void)
{
    HI_CHAR *pcSrcFile = "./data/nnie_image/rgb_planar/horse_dog_car_person_224x224.bgr";
    HI_CHAR *pcModelName = "./data/nnie_model/detection/inst_fasterrcnn_pvanet_inst.wk";
    HI_U32 u32PicNum =1;
    HI_FLOAT f32PrintResultThresh = 0.0f;
    HI_S32 s32Ret = HI_SUCCESS;
    HI_U32 i = 0;
    SAMPLE_SVP_NNIE_CFG_S stNnieCfg = {0};
    SAMPLE_SVP_NNIE_INPUT_DATA_INDEX_S stInputDataIdx = {0};
    SAMPLE_SVP_NNIE_PROCESS_SEG_INDEX_S stProcSegIdx = {0};
    SAMPLE_SVP_NIE_PERF_STAT_DEF_FRM_VAR()
    SAMPLE_SVP_NIE_PERF_STAT_DEF_VAR()

    /*Set configuration parameter*/
    s_enNetType = SAMPLE_SVP_NNIE_PVANET_FASTER_RCNN;
    f32PrintResultThresh = 0.8f;
    stNnieCfg.u32MaxInputNum = u32PicNum;
    stNnieCfg.u32MaxRoiNum= 200;
    stNnieCfg.pszPic = pcSrcFile;
    stNnieCfg.aenNnieCoreId[0] = SVP_NNIE_ID_0;
    stNnieCfg.aenNnieCoreId[1] = SVP_NNIE_ID_0;

    /*Sys_init*/
    SAMPLE_COMM_SVP_CheckSysInit();

    /*FasterRcnn Load model*/
    SAMPLE_SVP_TRACE_INFO("Pvanet load Model!!\n");
    s32Ret = SAMPLE_COMM_SVP_NNIE_LoadModel(pcModelName, &s_stPvanetModel);
    SAMPLE_SVP_CHECK_EXPR_GOTO(HI_SUCCESS != s32Ret, PVANET_FAIL_0, SAMPLE_SVP_ERR_LEVEL_ERROR,
    "ERROR, SAMPLE_COMM_SVP_NNIE_LoadModel failed\n");

   /*Pvanet para init*/
   /*apcRpnDataLayerName is used to set RPN data layer name
      and search RPN input data,if user has changed network struct, please
      make sure the data layer names are correct*/
    /*Pvanet parameters are set in SAMPLE_SVP_NNIE_Pvanet_SoftwareInit,
     if user has changed network struct, please make sure the parameter settings in
     SAMPLE_SVP_NNIE_FasterRcnn_SoftwareInit function are correct*/
    SAMPLE_SVP_TRACE_INFO("Pvanet parameter initialization!\n");
    s_stPvanetNnieParam.pstModel = &s_stPvanetModel.stModel;
    s_stPvanetSoftwareParam.apcRpnDataLayerName[0] = "rpn_cls_score";
    s_stPvanetSoftwareParam.apcRpnDataLayerName[1] = "rpn_bbox_pred";
    s32Ret = SAMPLE_SVP_NNIE_Pvanet_ParamInit(&stNnieCfg,  &s_stPvanetNnieParam,  &s_stPvanetSoftwareParam);
    SAMPLE_SVP_CHECK_EXPR_GOTO(HI_SUCCESS != s32Ret,PVANET_FAIL_0,SAMPLE_SVP_ERR_LEVEL_ERROR,
        "Error,SAMPLE_SVP_NNIE_Pvanet_ParamInit failed!\n");

    /*Fill 0-th input node of 0-th seg*/
    SAMPLE_SVP_TRACE_INFO("Pvanet start!\n");
    SAMPLE_SVP_NNIE_PERF_STAT_BEGIN_LOOP()

    stInputDataIdx.u32SegIdx = 0;
    stInputDataIdx.u32NodeIdx =0;
    s32Ret = SAMPLE_SVP_NNIE_FillSrcData( &stNnieCfg , &s_stPvanetNnieParam, &stInputDataIdx);
    SAMPLE_SVP_CHECK_EXPR_GOTO(HI_SUCCESS != s32Ret, PVANET_FAIL_0, SAMPLE_SVP_ERR_LEVEL_ERROR,
     "ERROR, SAMPLE_SVP_NNIE_FillSrcData Failed!!\n");

    /*NNIE process 0-th seg*/
    stProcSegIdx.u32SegIdx = 0;
    s32Ret = SAMPLE_SVP_NNIE_Forward( &s_stPvanetNnieParam, &stInputDataIdx,  &stProcSegIdx, HI_TRUE);
    SAMPLE_SVP_CHECK_EXPR_GOTO(HI_SUCCESS != s32Ret,PVANET_FAIL_0,SAMPLE_SVP_ERR_LEVEL_ERROR,
        "Error,SAMPLE_SVP_NNIE_Forward failed!\n");
    SAMPLE_SVP_NNIE_PERF_STAT_PVANET_FORWARD_SRC_FLUSH_TIME()
    SAMPLE_SVP_NNIE_PERF_STAT_PVANET_FORWARD_PRE_DST_FLUSH_TIME()
    SAMPLE_SVP_NNIE_PERF_STAT_PVANET_FORWARD_AFTER_DST_FLUSH_TIME()
    SAMPLE_SVP_NNIE_PERF_STAT_PVANET_FORWARD_OP_TIME()

    /*Do RPN*/
    s32Ret = SAMPLE_SVP_NNIE_Pvanet_Rpn( &s_stPvanetNnieParam, &s_stPvanetSoftwareParam);
    SAMPLE_SVP_CHECK_EXPR_GOTO(HI_SUCCESS != s32Ret,PVANET_FAIL_0,SAMPLE_SVP_ERR_LEVEL_ERROR,
        "Error,SAMPLE_SVP_NNIE_Pvanet_Rpn failed!\n");
    SAMPLE_SVP_NNIE_PERF_STAT_PVANET_RPN_SRC_FLUSH_TIME()
    SAMPLE_SVP_NNIE_PERF_STAT_PVANET_RPN_PRE_DST_FLUSH_TIME()
    SAMPLE_SVP_NNIE_PERF_STAT_PVANET_RPN_AFTER_DST_FLUSH_TIME()
    SAMPLE_SVP_NNIE_PERF_STAT_PVANET_RPN_OP_TIME()
    if(0 != s_stPvanetSoftwareParam.stRpnBbox.unShape.stWhc.u32Height)
    {
       /*NNIE process 1-th seg, the input conv data comes from 0-th seg's 0-th report node,
         the input roi comes from RPN results*/
        stInputDataIdx.u32NodeIdx = 0;
        stInputDataIdx.u32SegIdx = 0;
        stProcSegIdx.u32SegIdx = 1;
        s32Ret = SAMPLE_SVP_NNIE_ForwardWithBbox( &s_stPvanetNnieParam,  &stInputDataIdx, &s_stPvanetSoftwareParam.stRpnBbox,  &stProcSegIdx, HI_TRUE);
        SAMPLE_SVP_CHECK_EXPR_GOTO(HI_SUCCESS != s32Ret,PVANET_FAIL_0,SAMPLE_SVP_ERR_LEVEL_ERROR,
            "Error,SAMPLE_SVP_NNIE_ForwardWithBbox failed!\n");
        SAMPLE_SVP_NNIE_PERF_STAT_PVANET_ROIPOOLING_SRC_FLUSH_TIME()
        SAMPLE_SVP_NNIE_PERF_STAT_PVANET_ROIPOOLING_PRE_DST_FLUSH_TIME()
        SAMPLE_SVP_NNIE_PERF_STAT_PVANET_ROIPOOLING_AFTER_DST_FLUSH_TIME()
        SAMPLE_SVP_NNIE_PERF_STAT_PVANET_ROIPOOLING_OP_TIME()

        /*GetResult*/
        /*if user has changed net struct, please make sure SAMPLE_SVP_NNIE_FasterRcnn_GetResult
         function's input datas are correct*/
        SAMPLE_SVP_NNIE_PERF_STAT_BEGIN()
        s32Ret = SAMPLE_SVP_NNIE_Pvanet_GetResult( &s_stPvanetNnieParam,  &s_stPvanetSoftwareParam);
        SAMPLE_SVP_CHECK_EXPR_GOTO(HI_SUCCESS != s32Ret, PVANET_FAIL_0, SAMPLE_SVP_ERR_LEVEL_ERROR,
          "ERROR, SAMPLE_SVP_NNIE_Pvanet_GetResult Failed!\n");
        SAMPLE_SVP_NNIE_PERF_STAT_END()
        SAMPLE_SVP_NNIE_PERF_STAT_PVANET_GR_OP_TIME()
    }
    else
    {
        for (i = 0; i < s_stPvanetSoftwareParam.stClassRoiNum.unShape.stWhc.u32Width; i++)
        {
            *(((HI_U32*)(HI_UL)s_stPvanetSoftwareParam.stClassRoiNum.u64VirAddr)+i) = 0;
        }
    }

    /*print result, this sample has 21 classes:
     class 0:background     class 1:plane           class 2:bicycle
     class 3:bird           class 4:boat            class 5:bottle
     class 6:bus            class 7:car             class 8:cat
     class 9:chair          class10:cow             class11:diningtable
     class 12:dog           class13:horse           class14:motorbike
     class 15:person        class16:pottedplant     class17:sheep
     class 18:sofa          class19:train           class20:tvmonitor*/
    SAMPLE_SVP_TRACE_INFO("Pvanet result:\n");
    (void)SAMPLE_SVP_NNIE_Detection_PrintResult( &s_stPvanetSoftwareParam.stDstScore,
    &s_stPvanetSoftwareParam.stDstRoi, &s_stPvanetSoftwareParam.stClassRoiNum,
    f32PrintResultThresh);
    SAMPLE_SVP_NNIE_PERF_STAT_END_LOOP()

    SAMPLE_SVP_NNIE_PERF_STAT_PVANET_PRINT()


PVANET_FAIL_0:
    SAMPLE_SVP_NNIE_Pvanet_Deinit(&s_stPvanetNnieParam,  &s_stPvanetSoftwareParam,  &s_stPvanetModel);

    SAMPLE_COMM_SVP_CheckSysExit();
}

/******************************************************************************
* function : Pvanet sample signal handle
******************************************************************************/
void SAMPLE_SVP_NNIE_Pvanet_HandleSig(void)
{
    SAMPLE_SVP_NNIE_FasterRcnn_Deinit(&s_stPvanetNnieParam,&s_stPvanetSoftwareParam,
        &s_stPvanetModel);
    memset(&s_stPvanetNnieParam,0,sizeof(SAMPLE_SVP_NNIE_PARAM_S));
    memset(&s_stPvanetSoftwareParam,0,sizeof(SAMPLE_SVP_NNIE_FASTERRCNN_SOFTWARE_PARAM_S));
    memset(&s_stPvanetModel,0,sizeof(SAMPLE_SVP_NNIE_MODEL_S));
    SAMPLE_COMM_SVP_CheckSysExit();
}

