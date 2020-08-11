#ifndef _SAMPLE_NNIE_H_
#define _SAMPLE_NNIE_H_
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
#include <dirent.h>
#include "hi_common.h"
#include "hi_comm_sys.h"
#include "hi_comm_svp.h"
#include "sample_comm.h"
#include "sample_comm_svp.h"
#include "sample_comm_nnie.h"
#include "Tensor.h"
#define SAMPLE_SVP_NNIE_QUANT_BASE 4096
#ifdef __cplusplus
extern "C" {
#endif  

static HI_S32 NNIE_Forward(SAMPLE_SVP_NNIE_PARAM_S *pstNnieParam,
    SAMPLE_SVP_NNIE_INPUT_DATA_INDEX_S* pstInputDataIdx,
    SAMPLE_SVP_NNIE_PROCESS_SEG_INDEX_S* pstProcSegIdx,HI_BOOL bInstant);

int load_model(const char* model_path,SAMPLE_SVP_NNIE_MODEL_S* s_stModel);

static HI_S32 NNIE_FillData(const unsigned char* data,
    SAMPLE_SVP_NNIE_PARAM_S *pstNnieParam, SAMPLE_SVP_NNIE_INPUT_DATA_INDEX_S* pstInputDataIdx); 

void NNIE_Forward_From_Data(const unsigned char* data, SAMPLE_SVP_NNIE_MODEL_S* s_stModel,SAMPLE_SVP_NNIE_PARAM_S* s_stNnieParam,Tensor output_tensors[5]);

void nnie_param_init(SAMPLE_SVP_NNIE_MODEL_S* s_stModel,SAMPLE_SVP_NNIE_CFG_S* stNnieCfg,SAMPLE_SVP_NNIE_PARAM_S* s_stNnieParam);

void NNIE_Param_Deinit(SAMPLE_SVP_NNIE_PARAM_S *pstNnieParam, SAMPLE_SVP_NNIE_MODEL_S *pstNnieModel);

static HI_S32  NNIE_Get_Result(SAMPLE_SVP_NNIE_PARAM_S *s_stNnieParam,Tensor output_tensors[5]);

#ifdef __cplusplus
}
#endif

#endif