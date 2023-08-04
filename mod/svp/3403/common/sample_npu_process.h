/*
  Copyright (c), 2001-2022, Shenshu Tech. Co., Ltd.
 */

#ifndef SAMPLE_NPU_PROCESS_H
#define SAMPLE_NPU_PROCESS_H

#include "hi_type.h"

/* function : show the sample of acl resnet50 */
hi_void sample_svp_npu_acl_resnet50(hi_void);

/* function : show the sample of acl resnet50_multithread */
hi_void sample_svp_npu_acl_resnet50_multithread(hi_void);

/* function : show the sample of acl resnet50 sign handle */
hi_void sample_svp_npu_acl_resnet50_handle_sig(hi_void);

/* function : show the sample of acl mobilenet_v3 dyanamicbatch with mmz cached */
hi_void sample_svp_npu_acl_mobilenet_v3_dynamicbatch(hi_void);

#endif
