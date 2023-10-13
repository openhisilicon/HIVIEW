/*
  Copyright (c), 2001-2022, Shenshu Tech. Co., Ltd.
 */
#include "sample_common_svp_npu.h"
#include "hi_common.h"
#include "hi_common_svp.h"
#include "sample_comm.h"
#include "sample_common_svp.h"

/* acl init */
 hi_s32 sample_common_svp_npu_acl_init(const hi_char *acl_config_path, hi_s32 dev_id)
{
    svp_acl_rt_run_mode run_mode;
    svp_acl_error ret;
    /* svp acl init */
    ret = svp_acl_init(acl_config_path);
    sample_svp_check_exps_return(ret != SVP_ACL_SUCCESS, HI_FAILURE, SAMPLE_SVP_ERR_LEVEL_ERROR, "acl init failed!\n");
    sample_svp_trace_info("svp acl init success!\n");
    /* open device */
    ret = svp_acl_rt_set_device(dev_id);
    sample_svp_check_exps_goto(ret != SVP_ACL_SUCCESS, end_0, SAMPLE_SVP_ERR_LEVEL_ERROR,
        "svp acl open device %d failed!\n", dev_id);

    sample_svp_trace_info("open device %d success!\n", dev_id);
    /* get run mode */
    ret = svp_acl_rt_get_run_mode(&run_mode);
    sample_svp_check_exps_goto(ret != SVP_ACL_SUCCESS, end_1, SAMPLE_SVP_ERR_LEVEL_ERROR,
        "acl get run mode failed!\n");
    sample_svp_check_exps_goto(run_mode != SVP_ACL_DEVICE, end_1, SAMPLE_SVP_ERR_LEVEL_ERROR,
        "run mode(%d) is not equal to %d!\n", run_mode, SVP_ACL_DEVICE);

    sample_svp_trace_info("get run mode success!\n");
    return HI_SUCCESS;
end_1:
    (hi_void)svp_acl_rt_reset_device(dev_id);
end_0:
    (hi_void)svp_acl_finalize();
    return HI_FAILURE;
}

/* acl deinit */
hi_void sample_common_svp_npu_acl_deinit(hi_s32 dev_id)
{
    svp_acl_error ret;

    ret = svp_acl_rt_reset_device(dev_id);
    if (ret != SVP_ACL_SUCCESS) {
        sample_svp_trace_err("reset device fail\n");
    }
    sample_svp_trace_info("end to reset device is %d\n", dev_id);

    ret = svp_acl_finalize();
    if (ret != SVP_ACL_SUCCESS) {
        sample_svp_trace_err("finalize acl fail\n");
    }
    sample_svp_trace_info("end to finalize acl\n");
}
