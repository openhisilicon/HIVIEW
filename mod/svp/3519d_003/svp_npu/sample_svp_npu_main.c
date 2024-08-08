/*
  Copyright (c), 2001-2022, Shenshu Tech. Co., Ltd.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>

#include "securec.h"
#include "sample_svp_npu_process.h"

#define SAMPLE_SVP_NPU_ARG_NUM_TWO 2
#define SAMPLE_SVP_NPU_ARG_NUM_THREE 3
#define SAMPLE_SVP_NPU_ARG_IDX_TWO 2
#define SAMPLE_SVP_NPU_CMP_STR_NUM 2
#define SAMPLE_SVP_NPU_ARG_STR_LEN 2
#define SAMPLE_SVP_NPU_ARG_IDX_BASE 10

static char **g_npu_cmd_argv = HI_NULL;

/*
 * function : to process abnormal case
 */
#ifndef __LITEOS__
static hi_void sample_svp_npu_handle_sig(hi_s32 signo)
{
    if (signo == SIGINT || signo == SIGTERM) {
        sample_svp_npu_acl_handle_sig();
    }
}
#endif

/*
 * function : show usage
 */
static hi_void sample_svp_npu_usage(const hi_char *prg_name)
{
    printf("Usage : %s <index>\n", prg_name);
    printf("index:\n");
    printf("\t 0) svp_acl_resnet50.\n");
    printf("\t 1) svp_acl_resnet50_multi_thread.\n");
    printf("\t 2) svp_acl_resnet50_dynamic_batch_with_mem_cached.\n");
    printf("\t 3) svp_acl_lstm.\n");
    printf("\t 4) svp_acl_rfcn.(FILE->VDEC->VPSS->SVP_NPU->VGS->VO_BT1120).\n");
    printf("\t 5) svp_acl_event.\n");
    printf("\t 6) svp_acl_aipp.\n");
    printf("\t 7) svp_acl_preemption.\n");
    printf("\t 8) [1, 5],yolov1 to yolov5; [7, 8],yolov7 to yolov8.(VI->VPSS->SVP_NPU->VGS->VO)\n");
    printf("\t 9) [1, 5],yolov1 to yolov5; [7, 8],yolov7 to yolov8.(FILE->VDEC->VPSS->SVP_NPU->VGS->VO)\n");
    printf("\t a) sample_svp_npu_acl_e2e_hrnet.(VI->VPSS->SVP_NPU->VGS->VO)\n");
}

static hi_s32 sample_svp_npu_case_with_one_arg(hi_char idx)
{
    hi_s32 ret = HI_SUCCESS;

    switch (idx) {
        case '0':
            sample_svp_npu_acl_resnet50();
            break;
        case '1':
            sample_svp_npu_acl_resnet50_multi_thread();
            break;
        case '2':
            sample_svp_npu_acl_resnet50_dynamic_batch();
            break;
        case '3':
            sample_svp_npu_acl_lstm();
            break;
        case '4':
            sample_svp_npu_acl_rfcn();
            break;
        case '5':
            sample_svp_npu_acl_event();
            break;
        case '6':
            sample_svp_npu_acl_aipp();
            break;
        case '7':
            sample_svp_npu_acl_preemption();
            break;
        case 'a':
        case 'A':
            sample_svp_npu_acl_e2e_hrnet();
            break;
        default:
            ret = HI_FAILURE;
            break;
    }
    return ret;
}

static hi_s32 sample_svp_npu_case_with_two_args(hi_char idx1, hi_char idx2)
{
    hi_s32 ret = HI_SUCCESS;

    if (!((idx2 <= '5' && idx2 >= '1') || (idx2 >= '7' && idx2 <= '8'))) {
        return HI_FAILURE;
    }
    switch (idx1) {
        case '8':
            sample_svp_npu_acl_e2e_yolo(idx2 - '0');
            break;
        case '9':
            sample_svp_npu_acl_yolo(idx2 - '0');
            break;
        default:
            ret = HI_FAILURE;
            break;
    }
    return ret;
}

/*
 * function : svp npu sample
 */
#ifdef __LITEOS__
int app_main(int argc, char *argv[])
#else
int sample_svp_npu_main(int argc, char *argv[])
#endif
{
    hi_s32 ret;
    size_t argv1_len, argv2_len;
#ifndef __LITEOS__
    struct sigaction sa;
#endif
    if (argc < SAMPLE_SVP_NPU_ARG_NUM_TWO || argc > SAMPLE_SVP_NPU_ARG_NUM_THREE) {
        sample_svp_npu_usage(argv[0]);
        return HI_FAILURE;
    }

    if (!strncmp(argv[1], "-h", SAMPLE_SVP_NPU_CMP_STR_NUM)) {
        sample_svp_npu_usage(argv[0]);
        return HI_SUCCESS;
    }

    g_npu_cmd_argv = argv;
#ifndef __LITEOS__
    (hi_void)memset_s(&sa, sizeof(struct sigaction), 0, sizeof(struct sigaction));
    sa.sa_handler = sample_svp_npu_handle_sig;
    sa.sa_flags = 0;
    sigaction(SIGINT, &sa, NULL);
    sigaction(SIGTERM, &sa, NULL);
#endif
    argv1_len = strlen(argv[1]);
    if (argv1_len != 1) {
        sample_svp_npu_usage(argv[0]);
        return HI_FAILURE;
    }
    if (argc == SAMPLE_SVP_NPU_ARG_NUM_THREE) {
        argv2_len = strlen(argv[SAMPLE_SVP_NPU_ARG_IDX_TWO]);
        if (argv2_len != 1) {
            sample_svp_npu_usage(argv[0]);
            return HI_FAILURE;
        }
    }
    if (argc == SAMPLE_SVP_NPU_ARG_NUM_TWO) {
        ret = sample_svp_npu_case_with_one_arg(*argv[1]);
    } else {
        ret = sample_svp_npu_case_with_two_args(*argv[1], *argv[SAMPLE_SVP_NPU_ARG_IDX_TWO]);
    }
    if (ret != HI_SUCCESS) {
        sample_svp_npu_usage(argv[0]);
        return HI_FAILURE;
    }

    return 0;
}
