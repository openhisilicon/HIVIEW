/*
  Copyright (c), 2001-2024, Shenshu Tech. Co., Ltd.
 */
#ifndef SAMPLE_SVP_NPU_PROCESS_H
#define SAMPLE_SVP_NPU_PROCESS_H

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <time.h>
#include <sys/time.h>
#include <math.h>
#include "hi_type.h"


#include "sample_comm.h"
#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

#ifndef YOLO_BOX_MAX
#define YOLO_BOX_MAX 64
typedef struct {
  int chn;
  int w, h;
  int size;
  struct{
    float score;
    float x;
    float y;
    float w;
    float h;
    const char *label;
    }box[YOLO_BOX_MAX];
}yolo_boxs_t;
#endif

int sample_svp_npu_init(char *model_path);
int sample_svp_npu_detect(hi_video_frame_info *ext_frame, hi_video_frame_info *base_frame, yolo_boxs_t *boxs);
int sample_svp_npu_destroy(void);

/* function : show the sample of acl resnet50 */
hi_void sample_svp_npu_acl_resnet50(hi_void);

/* function : show the sample of acl resnet50_multithread */
hi_void sample_svp_npu_acl_resnet50_multi_thread(hi_void);

/* function : show the sample of resnet50 dyanamic batch with mem cached */
hi_void sample_svp_npu_acl_resnet50_dynamic_batch(hi_void);

/* function : show the sample of lstm */
hi_void sample_svp_npu_acl_lstm(hi_void);

/* function : show the sample of preemption */
hi_void sample_svp_npu_acl_preemption(hi_void);

/* function : show the sample of aipp */
hi_void sample_svp_npu_acl_aipp(hi_void);

/* function : show the sample of e2e yolo */
hi_void sample_svp_npu_acl_e2e_yolo(hi_u32 index);

/* function : show the sample of e2e hrnet */
hi_void sample_svp_npu_acl_e2e_hrnet(hi_void);

/* function : show the sample of sign handle */
hi_void sample_svp_npu_acl_handle_sig(hi_void);

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif
