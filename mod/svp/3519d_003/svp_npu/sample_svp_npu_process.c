/*
  Copyright (c), 2001-2022, Shenshu Tech. Co., Ltd.
 */
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>

#include "sample_svp_npu_process.h"
#include <pthread.h>
#include <sys/prctl.h>
#include "svp_acl_rt.h"
#include "svp_acl.h"
#include "svp_acl_ext.h"
#include "sample_common_svp.h"
#include "sample_common_svp_npu.h"
#include "sample_common_svp_npu_model.h"

#include "sample_ive_kcf.h"

#define SAMPLE_SVP_NPU_RESNET50_INPUT_FILE_NUM 1
#define SAMPLE_SVP_NPU_LSTM_INPUT_FILE_NUM     4
#define SAMPLE_SVP_NPU_SHAERD_WORK_BUF_NUM     1
#define SAMPLE_SVP_NPU_YOLO_TYPE_NUM           12
#define SAMPLE_SVP_NPU_YOLO_THRESHOLD_NUM      1

#define SAMPLE_SVP_NPU_PREEMPTION_SLEEP_SECOND    30
#define SAMPLE_SVP_NPU_PATH_LEN 0x100

#if 1 //maohw
#define __KCF_ENABLE__ 0 //vpss0 must Less than 1080p;

static sample_svp_npu_thread_args *__args = NULL;
static yolo_boxs_t *__boxs = NULL;
static char* sample_svp_model_path(char *path)
{
  static char _path[256];
  if(path)
  {
    strncpy(_path, path, sizeof(_path)-1);
  }
  return _path;
}
#endif

static hi_bool g_svp_npu_terminate_signal = HI_FALSE;
static hi_s32 g_svp_npu_dev_id = 0;
static sample_svp_npu_task_info g_svp_npu_task[SAMPLE_SVP_NPU_MAX_TASK_NUM] = {0};
static svp_acl_rt_stream g_svp_npu_stream[SAMPLE_SVP_NPU_MAX_MODEL_NUM] = {0};
static sample_svp_npu_shared_work_buf g_svp_npu_shared_work_buf[SAMPLE_SVP_NPU_SHAERD_WORK_BUF_NUM] = {0};
static hi_bool g_svp_npu_is_thread_start = HI_FALSE;

static sample_svp_npu_threshold g_svp_npu_rfcn_threshold[SAMPLE_SVP_NPU_RFCN_THRESHOLD_NUM] = {
    {0.7, 0.0, 16.0, 16.0, "rpn_data"}, {0.3, 0.9, 16.0, 16.0, "rpn_data1"} };

static sample_svp_npu_threshold g_svp_npu_yolo_threshold[SAMPLE_SVP_NPU_YOLO_TYPE_NUM] = {
    {0.0, 0.0, 0.0, 0.0, "rpn_data"},  // reserve
    {0.5, 0.2, 1.0, 1.0, "rpn_data"},  // yolov1
    {0.3, 0.5, 1.0, 1.0, "rpn_data"},  // yolov2
    {0.45, 0.5, 1.0, 1.0, "rpn_data"}, // yolov3
    {0.45, 0.5, 1.0, 1.0, "rpn_data"}, // yolov4
    {0.45, 0.5, 1.0, 1.0, "rpn_data"}, // yolov5
    {0.0, 0.0, 0.0, 0.0, "rpn_data"},  // reserve
    {0.45, 0.5, 1.0, 1.0, "rpn_data"}, // yolov7
    {0.9, 0.15, 1.0, 1.0, "rpn_data"},  // yolov8
    {0.0, 0.0, 0.0, 0.0, "rpn_data"}   // reserve
    };

static sample_svp_npu_roi_info g_svp_npu_yolo_roi_info[SAMPLE_SVP_NPU_YOLO_TYPE_NUM] = {
    {"reserve", "reserve"},  // reserve
    {"yolov1_nms", "yolov1_nms_"},  // yolov1
    {"DetectionOut0_nms_3", "DetectionOut0_nms_3_"},  // yolov2
    {"detection_nms_2", "detection_nms_2_"},  // yolov3
    {"detection_nms_2", "detection_nms_2_"},  // yolov4
    {"output0", "output0_"},  // yolov5
    {"reserve", "reserve"},  // reserve
    {"output", "output_"},  // yolov7
    {"output0", "output0_"},  // yolov8
    {"reserve", "reserve"},  // reserve
    };

static sample_vi_cfg g_vi_config;

static hi_sample_svp_rect_info g_svp_npu_rect_info = {0};
static hi_sample_svp_coords_info g_svp_npu_coords_info = {0};
static hi_bool g_svp_npu_thread_stop = HI_FALSE;
static pthread_t g_svp_npu_thread = 0;
static pthread_t g_svp_npu_vdec_thread = 0;
static hi_vb_pool_info g_svp_npu_vb_pool_info;
static hi_void *g_svp_npu_vb_virt_addr = HI_NULL;

#define SVP_NPU_VO_WIDTH    1920
#define SVP_NPU_VO_HEIGHT   1080
static sample_vo_cfg g_svp_npu_vo_cfg = {
    .vo_dev            = SAMPLE_VO_DEV_UHD,
    .vo_layer          = SAMPLE_VO_LAYER_VHD0,
    .vo_intf_type      = HI_VO_INTF_BT1120,
    .intf_sync         = HI_VO_OUT_1080P60,
    .bg_color          = COLOR_RGB_BLACK,
    .pix_format        = HI_PIXEL_FORMAT_YVU_SEMIPLANAR_420,
    .disp_rect         = {0, 0, SVP_NPU_VO_WIDTH, SVP_NPU_VO_HEIGHT},
    .image_size        = {SVP_NPU_VO_WIDTH, SVP_NPU_VO_HEIGHT},
    .vo_part_mode      = HI_VO_PARTITION_MODE_SINGLE,
    .dis_buf_len       = 3, /* 3: def buf len for single */
    .dst_dynamic_range = HI_DYNAMIC_RANGE_SDR8,
    .vo_mode           = VO_MODE_1MUX,
    .compress_mode     = HI_COMPRESS_MODE_NONE,
};

static hi_sample_svp_media_cfg g_svp_npu_media_cfg = {
    .svp_switch = {HI_FALSE, HI_TRUE},
    .pic_type = {PIC_1080P, PIC_CIF},
    .chn_num = HI_SVP_MAX_VPSS_CHN_NUM,
};

static sample_vdec_attr g_svp_npu_vdec_cfg = {
    .type = HI_PT_H264,
    .mode = HI_VDEC_SEND_MODE_FRAME,
    .width = _4K_WIDTH,
    .height = _4K_HEIGHT,
    .sample_vdec_video.dec_mode = HI_VIDEO_DEC_MODE_IP,
    .sample_vdec_video.bit_width = HI_DATA_BIT_WIDTH_8,
    .sample_vdec_video.ref_frame_num = 2, /* 2:ref_frame_num */
    .display_frame_num = 2,               /* 2:display_frame_num */
    .frame_buf_cnt = 5,                   /* 5:2+2+1 */
};

static vdec_thread_param g_svp_npu_vdec_param = {
    .chn_id = 0,
    .type = HI_PT_H264,
    .stream_mode = HI_VDEC_SEND_MODE_FRAME,
    .interval_time = 1000, /* 1000:interval_time */
    .pts_init = 0,
    .pts_increase = 0,
    .e_thread_ctrl = THREAD_CTRL_START,
    .circle_send = HI_TRUE,
    .milli_sec = 0,
    .min_buf_size = (_4K_WIDTH * _4K_HEIGHT * 3) >> 1, /* 3:chn_size */
    .c_file_path = "./data/image/",
    .c_file_name = "dolls_video.h264",
    .fps = 30,
};

static hi_void sample_svp_npu_acl_terminate(hi_void)
{
    if (g_svp_npu_terminate_signal == HI_TRUE) {
        printf("\033[0;31mprogram termination abnormally!\033[0;39m\n");
    }
}

/* function : svp npu signal handle */
hi_void sample_svp_npu_acl_handle_sig(hi_void)
{
    g_svp_npu_terminate_signal = HI_TRUE;
}

static hi_void sample_svp_npu_acl_deinit(hi_void)
{
    svp_acl_error ret;

    ret = svp_acl_rt_reset_device(g_svp_npu_dev_id);
    if (ret != SVP_ACL_SUCCESS) {
        sample_svp_trace_err("reset device fail\n");
    }
    sample_svp_trace_info("end to reset device is %d\n", g_svp_npu_dev_id);

    ret = svp_acl_finalize();
    if (ret != SVP_ACL_SUCCESS) {
        sample_svp_trace_err("finalize acl fail\n");
    }
    sample_svp_trace_info("end to finalize acl\n");
    (hi_void)sample_common_svp_check_sys_exit();
}

static hi_s32 sample_svp_npu_acl_init(const hi_char *acl_config_path)
{
    /* svp acl init */
    svp_acl_rt_run_mode run_mode;
    svp_acl_error ret;
    hi_bool is_mpi_init;

    is_mpi_init = sample_common_svp_check_sys_init();
    sample_svp_check_exps_return(is_mpi_init != HI_TRUE, HI_FAILURE, SAMPLE_SVP_ERR_LEVEL_ERROR, "mpi init failed!\n");

    ret = svp_acl_init(acl_config_path);
    sample_svp_check_exps_return(ret != SVP_ACL_SUCCESS, HI_FAILURE, SAMPLE_SVP_ERR_LEVEL_ERROR, "acl init failed!\n");

    sample_svp_trace_info("svp acl init success!\n");

    /* open device */
    ret = svp_acl_rt_set_device(g_svp_npu_dev_id);
    if (ret != SVP_ACL_SUCCESS) {
        (hi_void)svp_acl_finalize();
        sample_svp_trace_err("svp acl open device %d failed!\n", g_svp_npu_dev_id);
        return HI_FAILURE;
    }
    sample_svp_trace_info("open device %d success!\n", g_svp_npu_dev_id);

    /* get run mode */
    ret = svp_acl_rt_get_run_mode(&run_mode);
    if ((ret != SVP_ACL_SUCCESS) || (run_mode != SVP_ACL_DEVICE)) {
        (hi_void)svp_acl_rt_reset_device(g_svp_npu_dev_id);
        (hi_void)svp_acl_finalize();
        sample_svp_trace_err("acl get run mode failed!\n");
        return HI_FAILURE;
    }
    sample_svp_trace_info("get run mode success!\n");

    return HI_SUCCESS;
}

static hi_s32 sample_svp_npu_acl_create_stream(hi_u32 stream_num)
{
    td_u32 i, j;
    hi_s32 ret;

    sample_svp_check_exps_return(stream_num > SAMPLE_SVP_NPU_MAX_STREAM_NUM, HI_FAILURE, SAMPLE_SVP_ERR_LEVEL_ERROR,
        "check stream num failed!\n");

    for (i = 0; i < stream_num; i++) {
        ret = svp_acl_rt_create_stream(&g_svp_npu_stream[i]);
        sample_svp_check_exps_goto(ret != HI_SUCCESS, create_stream_err, SAMPLE_SVP_ERR_LEVEL_ERROR,
            "create stream failed!\n");
    }

    return SVP_ACL_SUCCESS;

create_stream_err:
    for (j = 0; j < i; j++) {
        (hi_void)svp_acl_rt_destroy_stream(g_svp_npu_stream[j]);
    }

    return ret;
}

static hi_void sample_svp_npu_acl_destroy_stream(hi_u32 stream_num)
{
    td_u32 i;

    for (i = 0; i < stream_num; i++) {
        (hi_void)svp_acl_rt_destroy_stream(g_svp_npu_stream[i]);
    }
}

static hi_s32 sample_svp_npu_acl_dataset_init(hi_u32 task_idx)
{
    hi_s32 ret = sample_common_svp_npu_create_input(&g_svp_npu_task[task_idx]);
    sample_svp_check_exps_return(ret != HI_SUCCESS, HI_FAILURE, SAMPLE_SVP_ERR_LEVEL_ERROR, "create input failed!\n");

    ret = sample_common_svp_npu_create_output(&g_svp_npu_task[task_idx]);
    if (ret != HI_SUCCESS) {
        sample_common_svp_npu_destroy_input(&g_svp_npu_task[task_idx]);
        sample_svp_trace_err("execute create output fail.\n");
        return HI_FAILURE;
    }
    return HI_SUCCESS;
}

static hi_void sample_svp_npu_acl_dataset_deinit(hi_u32 task_idx)
{
    (hi_void)sample_common_svp_npu_destroy_input(&g_svp_npu_task[task_idx]);
    (hi_void)sample_common_svp_npu_destroy_output(&g_svp_npu_task[task_idx]);
}

static hi_void *sample_svp_npu_acl_thread_multi_model_process(hi_void *args)
{
    hi_s32 ret;
    hi_u32 task_idx = *(hi_u32 *)args;
    td_u32 proc_cnt = 0;

    ret = svp_acl_rt_set_device(g_svp_npu_dev_id);
    sample_svp_check_exps_return(ret != HI_SUCCESS, HI_NULL, SAMPLE_SVP_ERR_LEVEL_ERROR,
        "open device %d failed!\n", g_svp_npu_dev_id);

    while (g_svp_npu_is_thread_start == HI_TRUE && g_svp_npu_terminate_signal == HI_FALSE) {
        ret = sample_common_svp_npu_model_execute(&g_svp_npu_task[task_idx]);
        sample_svp_check_exps_goto(ret != HI_SUCCESS, process_end0, SAMPLE_SVP_ERR_LEVEL_ERROR, "proc sync failed!\n");
        sample_svp_trace_info("model_index:%u complete cnt:%u\n", g_svp_npu_task[task_idx].cfg.model_idx, ++proc_cnt);
    }

process_end0:
    ret = svp_acl_rt_reset_device(g_svp_npu_dev_id);
    if (ret != SVP_ACL_SUCCESS) {
        sample_svp_trace_err("task[%u] reset device failed!\n", task_idx);
    }
    return HI_NULL;
}

static hi_void *sample_svp_npu_acl_thread_execute(hi_void *args)
{
    hi_s32 ret;
    hi_u32 task_idx = *(hi_u32 *)args;

    ret = svp_acl_rt_set_device(g_svp_npu_dev_id);
    sample_svp_check_exps_return(ret != HI_SUCCESS, HI_NULL, SAMPLE_SVP_ERR_LEVEL_ERROR,
        "open device %d failed!\n", g_svp_npu_dev_id);

    ret = sample_common_svp_npu_model_execute(&g_svp_npu_task[task_idx]);
    if (ret != SVP_ACL_SUCCESS) {
        sample_svp_trace_err("execute inference failed of task[%u]!\n", task_idx);
    }

    ret = svp_acl_rt_reset_device(g_svp_npu_dev_id);
    if (ret != SVP_ACL_SUCCESS) {
        sample_svp_trace_err("task[%u] reset device failed!\n", task_idx);
    }
    return HI_NULL;
}

static hi_void sample_svp_npu_acl_model_execute_multithread()
{
    pthread_t execute_threads[SAMPLE_SVP_NPU_MAX_THREAD_NUM] = {0};
    hi_u32 idx[SAMPLE_SVP_NPU_MAX_THREAD_NUM] = {0};
    hi_u32 task_idx;

    for (task_idx = 0; task_idx < SAMPLE_SVP_NPU_MAX_THREAD_NUM; task_idx++) {
        idx[task_idx] = task_idx;
        pthread_create(&execute_threads[task_idx], NULL, sample_svp_npu_acl_thread_execute, &idx[task_idx]);
    }

    hi_void *waitret[SAMPLE_SVP_NPU_MAX_THREAD_NUM];
    for (task_idx = 0; task_idx < SAMPLE_SVP_NPU_MAX_THREAD_NUM; task_idx++) {
        pthread_join(execute_threads[task_idx], &waitret[task_idx]);
    }

    for (task_idx = 0; task_idx < SAMPLE_SVP_NPU_MAX_THREAD_NUM; task_idx++) {
        sample_svp_trace_info("output %u-th task data\n", task_idx);
        sample_common_svp_npu_output_classification_result(&g_svp_npu_task[task_idx]);
    }
}

static hi_void sample_svp_npu_acl_deinit_task(hi_u32 task_num, hi_u32 shared_work_buf_idx)
{
    hi_u32 task_idx;

    for (task_idx = 0; task_idx < task_num; task_idx++) {
        (hi_void)sample_common_svp_npu_destroy_work_buf(&g_svp_npu_task[task_idx]);
        (hi_void)sample_common_svp_npu_destroy_task_buf(&g_svp_npu_task[task_idx]);
        (hi_void)sample_svp_npu_acl_dataset_deinit(task_idx);
        (hi_void)memset_s(&g_svp_npu_task[task_idx], sizeof(sample_svp_npu_task_cfg), 0,
            sizeof(sample_svp_npu_task_cfg));
    }
    if (g_svp_npu_shared_work_buf[shared_work_buf_idx].work_buf_ptr != HI_NULL) {
        (hi_void)svp_acl_rt_free(g_svp_npu_shared_work_buf[shared_work_buf_idx].work_buf_ptr);
        g_svp_npu_shared_work_buf[shared_work_buf_idx].work_buf_ptr = HI_NULL;
        g_svp_npu_shared_work_buf[shared_work_buf_idx].work_buf_size = 0;
        g_svp_npu_shared_work_buf[shared_work_buf_idx].work_buf_stride = 0;
    }
}

static hi_s32 sample_svp_npu_acl_create_shared_work_buf(hi_u32 task_num, hi_u32 shared_work_buf_idx)
{
    hi_u32 task_idx, work_buf_size, work_buf_stride;
    hi_s32 ret;

    for (task_idx = 0; task_idx < task_num; task_idx++) {
        ret = sample_common_svp_npu_get_work_buf_info(&g_svp_npu_task[task_idx], &work_buf_size, &work_buf_stride);
        sample_svp_check_exps_return(ret != HI_SUCCESS, ret, SAMPLE_SVP_ERR_LEVEL_ERROR,
            "get %u-th task work buf info failed!\n", task_idx);

        if (g_svp_npu_shared_work_buf[shared_work_buf_idx].work_buf_size < work_buf_size) {
            g_svp_npu_shared_work_buf[shared_work_buf_idx].work_buf_size = work_buf_size;
            g_svp_npu_shared_work_buf[shared_work_buf_idx].work_buf_stride = work_buf_stride;
        }
    }
    ret = svp_acl_rt_malloc_cached(&g_svp_npu_shared_work_buf[shared_work_buf_idx].work_buf_ptr,
        g_svp_npu_shared_work_buf[shared_work_buf_idx].work_buf_size, SVP_ACL_MEM_MALLOC_NORMAL_ONLY);
    sample_svp_check_exps_return(ret != HI_SUCCESS, HI_FAILURE, SAMPLE_SVP_ERR_LEVEL_ERROR,
        "malloc %u-th shared work buf failed!\n", shared_work_buf_idx);

    (hi_void)svp_acl_rt_mem_flush(g_svp_npu_shared_work_buf[shared_work_buf_idx].work_buf_ptr,
        g_svp_npu_shared_work_buf[shared_work_buf_idx].work_buf_size);
    return HI_SUCCESS;
}

static hi_s32 sample_svp_npu_acl_init_task(hi_u32 task_num, hi_bool is_share_work_buf,
    hi_u32 shared_work_buf_idx)
{
    hi_u32 task_idx;
    hi_s32 ret;

    if (is_share_work_buf == HI_TRUE) {
        ret = sample_svp_npu_acl_create_shared_work_buf(task_num, shared_work_buf_idx);
        sample_svp_check_exps_return(ret != HI_SUCCESS, HI_FAILURE, SAMPLE_SVP_ERR_LEVEL_ERROR,
            "create shared work buf failed!\n");
    }

    for (task_idx = 0; task_idx < task_num; task_idx++) {
        ret = sample_svp_npu_acl_dataset_init(task_idx);
        if (ret != HI_SUCCESS) {
            goto task_init_end_0;
        }
        ret = sample_common_svp_npu_create_task_buf(&g_svp_npu_task[task_idx]);
        if (ret != HI_SUCCESS) {
            sample_svp_trace_err("create task buf failed.\n");
            goto task_init_end_0;
        }
        if (is_share_work_buf == HI_FALSE) {
            ret = sample_common_svp_npu_create_work_buf(&g_svp_npu_task[task_idx]);
        } else {
            /* if all tasks are on the same stream, work buf can be shared */
            ret = sample_common_svp_npu_share_work_buf(&g_svp_npu_shared_work_buf[shared_work_buf_idx],
                &g_svp_npu_task[task_idx]);
        }
        if (ret != HI_SUCCESS) {
            sample_svp_trace_err("create work buf failed.\n");
            goto task_init_end_0;
        }
    }
    return HI_SUCCESS;

task_init_end_0:
    (hi_void)sample_svp_npu_acl_deinit_task(task_num, shared_work_buf_idx);
    return ret;
}

static hi_s32 sample_svp_npu_acl_load_multi_model(const hi_char *model_path,
    hi_u32 model_num, hi_bool is_with_config, const hi_char *src_file[], hi_u32 file_num)
{
    hi_u32 i, j;
    hi_s32 ret;

    for (i = 0; i < model_num; i++) {
        if (is_with_config == HI_TRUE) {
            ret = sample_common_svp_npu_load_model_with_config(model_path, i, HI_FALSE);
        } else {
            ret = sample_common_svp_npu_load_model(model_path, i, HI_FALSE);
        }
        sample_svp_check_exps_goto(ret != HI_SUCCESS, process_end0, SAMPLE_SVP_ERR_LEVEL_ERROR,
            "load model failed!\n");

        /* set task cfg */
        g_svp_npu_task[i].cfg.max_batch_num = 1;
        g_svp_npu_task[i].cfg.dynamic_batch_num = 1;
        g_svp_npu_task[i].cfg.total_t = 0;
        g_svp_npu_task[i].cfg.is_cached = HI_FALSE;
        g_svp_npu_task[i].cfg.model_idx = i;
    }

    ret = sample_svp_npu_acl_init_task(model_num, HI_FALSE, 0);
    sample_svp_check_exps_goto(ret != HI_SUCCESS, process_end0, SAMPLE_SVP_ERR_LEVEL_ERROR, "init task failed!\n");

    for (i = 0; i < model_num; i++) {
        ret = sample_common_svp_npu_get_input_data(src_file, file_num, &g_svp_npu_task[i]);
        sample_svp_check_exps_goto(ret != HI_SUCCESS, process_end1, SAMPLE_SVP_ERR_LEVEL_ERROR, "get data failed!\n");
    }

    return HI_SUCCESS;

process_end1:
    (hi_void)sample_svp_npu_acl_deinit_task(model_num, 0);

process_end0:
    for (j = 0; j < i; j++) {
        (hi_void)sample_common_svp_npu_unload_model(j);
    }

    return ret;
}

static hi_void sample_svp_npu_acl_unload_multi_model(hi_u32 model_num)
{
    hi_u32 i;

    (hi_void)sample_svp_npu_acl_deinit_task(model_num, 0);

    for (i = 0; i < model_num; i++) {
        (hi_void)sample_common_svp_npu_unload_model(i);
    }
}

static const char* class_names[] = {
    "person", "bicycle", "car", "motorcycle", "airplane", "bus", "train", "truck", "boat", "traffic light",
    "fire hydrant", "stop sign", "parking meter", "bench", "bird", "cat", "dog", "horse", "sheep", "cow",
    "elephant", "bear", "zebra", "giraffe", "backpack", "umbrella", "handbag", "tie", "suitcase", "frisbee",
    "skis", "snowboard", "sports ball", "kite", "baseball bat", "baseball glove", "skateboard", "surfboard",
    "tennis racket", "bottle", "wine glass", "cup", "fork", "knife", "spoon", "bowl", "banana", "apple",
    "sandwich", "orange", "broccoli", "carrot", "hot dog", "pizza", "donut", "cake", "chair", "couch",
    "potted plant", "bed", "dining table", "toilet", "tv", "laptop", "mouse", "remote", "keyboard", "cell phone",
    "microwave", "oven", "toaster", "sink", "refrigerator", "book", "clock", "vase", "scissors", "teddy bear",
    "hair drier", "toothbrush"
    };

static hi_s32 sample_svp_npu_acl_frame_proc(const hi_video_frame_info *ext_frame,
    const hi_video_frame_info *base_frame, hi_void *args)
{
    hi_s32 ret;
    hi_void *virt_addr = HI_NULL;
    sample_svp_npu_detection_info *detection_info = HI_NULL;
    sample_svp_npu_thread_args *thread_args = (sample_svp_npu_thread_args *)args;
    hi_u32 size = (hi_u32)(ext_frame->video_frame.height * ext_frame->video_frame.stride[0] *
        SAMPLE_SVP_NPU_IMG_THREE_CHN / SAMPLE_SVP_NPU_DOUBLE);

    if(g_svp_npu_vb_virt_addr)
    {  
      virt_addr = g_svp_npu_vb_virt_addr +
          (ext_frame->video_frame.phys_addr[0] - g_svp_npu_vb_pool_info.pool_phy_addr);
    }
    else 
    {
      virt_addr = hi_mpi_sys_mmap(ext_frame->video_frame.phys_addr[0], size);//map virt_addr;
    }
    
    ret = sample_common_svp_npu_update_input_data_buffer_info(virt_addr, size,
        ext_frame->video_frame.stride[0], 0, &g_svp_npu_task[0]);
    sample_svp_check_exps_return(ret != HI_SUCCESS, ret, SAMPLE_SVP_ERR_LEVEL_ERROR, "update data buffer failed!\n");

    ret = sample_common_svp_npu_model_execute(&g_svp_npu_task[0]);
    sample_svp_check_exps_return(ret != HI_SUCCESS, ret, SAMPLE_SVP_ERR_LEVEL_ERROR, "model execute failed!\n");

    if(!g_svp_npu_vb_virt_addr)
    {
       hi_mpi_sys_munmap(virt_addr, size); //unmap virt_addr;
    }

    if (thread_args->model_name == SAMPLE_SVP_NPU_RFCN || thread_args->model_name == SAMPLE_SVP_NPU_YOLO) {
        detection_info = thread_args->detection_info;
        ret = sample_common_svp_npu_roi_to_rect(&g_svp_npu_task[0], detection_info, ext_frame, base_frame,
            &g_svp_npu_rect_info);
        sample_svp_check_exps_return(ret != HI_SUCCESS, ret, SAMPLE_SVP_ERR_LEVEL_ERROR, "roi to rect failed!\n");
        if(g_svp_npu_vb_virt_addr)
        {  
          ret = sample_common_svp_vgs_fill_rect(base_frame, &g_svp_npu_rect_info, SAMPLE_SVP_NPU_RECT_COLOR);
          sample_svp_check_exps_return(ret != HI_SUCCESS, ret, SAMPLE_SVP_ERR_LEVEL_ERROR, "vgs fill rect failed!\n");
        }
        else 
        {
          int i = 0;
          hi_sample_svp_rect_info *rect_info = &g_svp_npu_rect_info;
          
          //maohw
          #if __KCF_ENABLE__
          //if(rect_info->num > 0)
          if(1)
          {
            hi_sample_svp_ive_rect_arr rect_arr;
            
            rect_arr.class_num = 1;
            rect_arr.roi_num_array[0] = rect_info->num;
            rect_arr.total_num = rect_arr.roi_num_array[0];
            memcpy_s(rect_arr.rect, sizeof(hi_sample_svp_rect) * 64, rect_info->rect, sizeof(hi_sample_svp_rect) * rect_info->num);
       
            rect_info = sample_ive_kcf_tracking2(base_frame, &rect_arr);
          }
          #endif
         
          if(__boxs)
          {
            __boxs->chn = 0;
            __boxs->w = base_frame->video_frame.width;
            __boxs->h = base_frame->video_frame.height;
            __boxs->size = rect_info->num;          
            
            for (i = 0; i < rect_info->num; i++) {
              
              int x, y, w, h;
              __boxs->box[i].x = rect_info->rect[i].point[SAMPLE_SVP_NPU_RECT_LEFT_TOP].x;
              __boxs->box[i].y = rect_info->rect[i].point[SAMPLE_SVP_NPU_RECT_LEFT_TOP].y;
              __boxs->box[i].w = rect_info->rect[i].point[SAMPLE_SVP_NPU_RECT_RIGHT_TOP].x - __boxs->box[i].x;
              __boxs->box[i].h = rect_info->rect[i].point[SAMPLE_SVP_NPU_RECT_LEFT_BOTTOM].y - __boxs->box[i].y;
              #if __KCF_ENABLE__ //maohw
              __boxs->box[i].score = rect_info->id[i];
              __boxs->box[i].label = NULL;
              #else
              __boxs->box[i].score = rect_info->score[i];
              __boxs->box[i].label = class_names[rect_info->id[i]];
              #endif
            }
          }
        }
    } else if (thread_args->model_name == SAMPLE_SVP_NPU_HRNET) {
        ret = sample_common_svp_npu_get_joint_coords(&g_svp_npu_task[0], ext_frame, base_frame, &g_svp_npu_coords_info);
        sample_svp_check_exps_return(ret != HI_SUCCESS, ret, SAMPLE_SVP_ERR_LEVEL_ERROR, "roi to joint failed!\n");
        if(g_svp_npu_vb_virt_addr)
        {  
          ret = sample_common_svp_vgs_fill_joint_coords_lines(base_frame, &g_svp_npu_coords_info);
          sample_svp_check_exps_return(ret != HI_SUCCESS, ret, SAMPLE_SVP_ERR_LEVEL_ERROR, "vgs fill line failed!\n");
        }
        else 
        {
          ret = 0; //sendmsg;
        }
    }
    return ret;
}

static hi_s32 sample_svp_npu_acl_vb_map(hi_u32 vb_pool_idx)
{
    hi_s32 ret;

    if (g_svp_npu_vb_virt_addr == HI_NULL) {
        ret = hi_mpi_vb_get_pool_info(g_svp_npu_media_cfg.vb_pool[vb_pool_idx], &g_svp_npu_vb_pool_info);
        sample_svp_check_exps_return(ret != HI_SUCCESS, HI_FAILURE, SAMPLE_SVP_ERR_LEVEL_ERROR,
            "get pool info failed!\n");
        g_svp_npu_vb_virt_addr = hi_mpi_sys_mmap(g_svp_npu_vb_pool_info.pool_phy_addr,
            g_svp_npu_vb_pool_info.pool_size);
        sample_svp_check_exps_return(g_svp_npu_vb_virt_addr == HI_NULL, HI_FAILURE, SAMPLE_SVP_ERR_LEVEL_ERROR,
            "map vb pool failed!\n");
    }
    return HI_SUCCESS;
}

static hi_void *sample_svp_npu_acl_vdec_to_vo(hi_void *args)
{
    hi_s32 ret;
    hi_video_frame_info base_frame, ext_frame;
    const hi_s32 milli_sec = SAMPLE_SVP_NPU_MILLIC_SEC;
    const hi_vo_layer vo_layer = 0;
    const hi_vo_chn vo_chn = 0;
    const hi_s32 vpss_grp = 0;
    hi_s32 vpss_chn[] = { HI_VPSS_CHN0, HI_VPSS_CHN1 };
    hi_u32 size, stride;
    hi_u8 *data = HI_NULL;

    (hi_void)prctl(PR_SET_NAME, "svp_npu_vdec_to_vo", 0, 0, 0);

    ret = svp_acl_rt_set_device(g_svp_npu_dev_id);
    sample_svp_check_exps_return(ret != HI_SUCCESS, HI_NULL, SAMPLE_SVP_ERR_LEVEL_ERROR, "open device failed!\n");

#if 0 //maohw
    ret = sample_svp_npu_acl_vb_map(HI_VPSS_CHN1);
    sample_svp_check_exps_goto(ret != HI_SUCCESS, fail_0, SAMPLE_SVP_ERR_LEVEL_ERROR, "map vb pool failed!\n");
#else
    hi_vpss_chn_attr chn_attr;
    ret = hi_mpi_vpss_get_chn_attr(vpss_grp, vpss_chn[0], &chn_attr);
    chn_attr.depth = 2;
    chn_attr.chn_mode = HI_VPSS_CHN_MODE_USER;
    ret = hi_mpi_vpss_set_chn_attr(vpss_grp, vpss_chn[0], &chn_attr);

    ret = hi_mpi_vpss_get_chn_attr(vpss_grp, vpss_chn[1], &chn_attr);
    chn_attr.depth = 2;
    chn_attr.chn_mode = HI_VPSS_CHN_MODE_USER;
    ret = hi_mpi_vpss_set_chn_attr(vpss_grp, vpss_chn[1], &chn_attr);
#endif

    ret = sample_common_svp_npu_get_input_data_buffer_info(&g_svp_npu_task[0], 0, &data, &size, &stride);
    sample_svp_check_exps_goto(ret != HI_SUCCESS, fail_1, SAMPLE_SVP_ERR_LEVEL_ERROR,
        "Error(%#x),get_input_data_buffer_info failed!\n", ret);

    while (g_svp_npu_thread_stop == HI_FALSE) {
        ret = hi_mpi_vpss_get_chn_frame(vpss_grp, vpss_chn[1], &ext_frame, milli_sec);
        sample_svp_check_exps_continue(ret != HI_SUCCESS, SAMPLE_SVP_ERR_LEVEL_ERROR,
            "Error(%#x),hi_mpi_vpss_get_chn_frame failed, vpss_grp(%d), vpss_chn(%d)!\n", ret, vpss_grp, vpss_chn[1]);

        ret = hi_mpi_vpss_get_chn_frame(vpss_grp, vpss_chn[0], &base_frame, milli_sec);
        sample_svp_check_exps_goto(ret != HI_SUCCESS, ext_release, SAMPLE_SVP_ERR_LEVEL_ERROR,
            "Error(%#x),hi_mpi_vpss_get_chn_frame failed, VPSS_GRP(%d), VPSS_CHN(%d)!\n", ret, vpss_grp, vpss_chn[0]);

        ret = sample_svp_npu_acl_frame_proc(&ext_frame, &base_frame, args);
        sample_svp_check_exps_goto(ret != HI_SUCCESS, base_release, SAMPLE_SVP_ERR_LEVEL_ERROR,
            "Error(%#x),sample_svp_npu_acl_frame_proc failed!\n", ret);
            
        #if 0 //maohw
        ret = sample_common_svp_venc_vo_send_stream(&g_svp_npu_media_cfg.svp_switch, 0, vo_layer, vo_chn, &base_frame);
        sample_svp_check_exps_goto(ret != HI_SUCCESS, base_release, SAMPLE_SVP_ERR_LEVEL_ERROR,
            "Error(%#x),sample_common_svp_venc_vo_send_stream failed!\n", ret);
        #else
        usleep(10*1000);
        #endif
            
base_release:
        ret = hi_mpi_vpss_release_chn_frame(vpss_grp, vpss_chn[0], &base_frame);
        sample_svp_check_exps_trace(ret != HI_SUCCESS, SAMPLE_SVP_ERR_LEVEL_ERROR,
            "Error(%#x),release_frame failed,grp(%d) chn(%d)!\n", ret, vpss_grp, vpss_chn[0]);
ext_release:
        ret = hi_mpi_vpss_release_chn_frame(vpss_grp, vpss_chn[1], &ext_frame);
        sample_svp_check_exps_trace(ret != HI_SUCCESS, SAMPLE_SVP_ERR_LEVEL_ERROR,
            "Error(%#x),release_frame failed,grp(%d) chn(%d)!\n", ret, vpss_grp, vpss_chn[1]);
    }
    
    ret = sample_common_svp_npu_update_input_data_buffer_info(data, size, stride, 0, &g_svp_npu_task[0]);
    sample_svp_check_exps_trace(ret != HI_SUCCESS, SAMPLE_SVP_ERR_LEVEL_ERROR, "update buffer failed!\n");
fail_1:
    //maohw (hi_void)hi_mpi_sys_munmap(g_svp_npu_vb_virt_addr, g_svp_npu_vb_pool_info.pool_size);
fail_0:
    (hi_void)svp_acl_rt_reset_device(g_svp_npu_dev_id);
    return HI_NULL;
}


static hi_void sample_svp_npu_acl_pause(hi_void)
{
    printf("---------------press Enter key to exit!---------------\n");
    if (g_svp_npu_terminate_signal == HI_TRUE) {
        return;
    }
    (hi_void)getchar();
    if (g_svp_npu_terminate_signal == HI_TRUE) {
        return;
    }
}

/* function : show the sample of svp npu resnet50 */
hi_void sample_svp_npu_acl_resnet50(hi_void)
{
    hi_s32 ret;
    const hi_u32 model_idx = 0;
    const hi_char *acl_config_path = "";
    const hi_char *src_file[SAMPLE_SVP_NPU_RESNET50_INPUT_FILE_NUM] = {"./data/image/3_224_224_batch_1.bgr"};
    const hi_char *om_model_path = "./data/model/resnet50.om";

    g_svp_npu_terminate_signal = HI_FALSE;

    if (g_svp_npu_terminate_signal == HI_FALSE) {
        /* init acl */
        ret = sample_svp_npu_acl_init(acl_config_path);
        sample_svp_check_exps_return_void(ret != HI_SUCCESS, SAMPLE_SVP_ERR_LEVEL_ERROR, "init failed!\n");

        /* load model */
        ret = sample_common_svp_npu_load_model(om_model_path, model_idx, HI_FALSE);
        sample_svp_check_exps_goto(ret != HI_SUCCESS, process_end0, SAMPLE_SVP_ERR_LEVEL_ERROR,
            "load model failed!\n");

        /* set task cfg */
        g_svp_npu_task[0].cfg.max_batch_num = 1;
        g_svp_npu_task[0].cfg.dynamic_batch_num = 1;
        g_svp_npu_task[0].cfg.total_t = 0;
        g_svp_npu_task[0].cfg.is_cached = HI_FALSE;
        g_svp_npu_task[0].cfg.model_idx = model_idx;

        ret = sample_svp_npu_acl_init_task(1, HI_FALSE, 0);
        sample_svp_check_exps_goto(ret != HI_SUCCESS, process_end1, SAMPLE_SVP_ERR_LEVEL_ERROR,
            "init task failed!\n");
    }

    if (g_svp_npu_terminate_signal == HI_FALSE) {
        ret = sample_common_svp_npu_get_input_data(src_file, SAMPLE_SVP_NPU_RESNET50_INPUT_FILE_NUM,
            &g_svp_npu_task[0]);
        sample_svp_check_exps_goto(ret != HI_SUCCESS, process_end2, SAMPLE_SVP_ERR_LEVEL_ERROR, "get data failed!\n");
    }
    if (g_svp_npu_terminate_signal == HI_FALSE) {
        ret = sample_common_svp_npu_model_execute(&g_svp_npu_task[0]);
        sample_svp_check_exps_goto(ret != HI_SUCCESS, process_end2, SAMPLE_SVP_ERR_LEVEL_ERROR, "execute failed!\n");
        (hi_void)sample_common_svp_npu_output_classification_result(&g_svp_npu_task[0]);
    }

process_end2:
    (hi_void)sample_svp_npu_acl_deinit_task(1, 0);
process_end1:
    (hi_void)sample_common_svp_npu_unload_model(model_idx);
process_end0:
    (hi_void)sample_svp_npu_acl_deinit();
    (hi_void)sample_svp_npu_acl_terminate();
}

hi_void sample_svp_npu_acl_resnet50_dynamic_batch(hi_void)
{
    hi_s32 ret;
    const hi_u32 model_idx = 0;
    const hi_char *acl_config_path = "";
    const hi_char *src_file[SAMPLE_SVP_NPU_RESNET50_INPUT_FILE_NUM] = {"./data/image/3_224_224_batch_8.bgr"};
    const hi_char *om_model_path = "./data/model/resnet50.om";

    g_svp_npu_terminate_signal = HI_FALSE;

    if (g_svp_npu_terminate_signal == HI_FALSE) {
        /* init acl */
        ret = sample_svp_npu_acl_init(acl_config_path);
        sample_svp_check_exps_return_void(ret != HI_SUCCESS, SAMPLE_SVP_ERR_LEVEL_ERROR, "init failed!\n");

        /* load model */
        ret = sample_common_svp_npu_load_model(om_model_path, model_idx, HI_TRUE);
        sample_svp_check_exps_goto(ret != HI_SUCCESS, process_end0, SAMPLE_SVP_ERR_LEVEL_ERROR,
            "load model failed!\n");

        /* set task cfg */
        g_svp_npu_task[0].cfg.max_batch_num = 8; /* 8 is max batch num, it can't be less than dynamic_batch_num */
        g_svp_npu_task[0].cfg.dynamic_batch_num = 8; /* 8 is batch num of task to be processed */
        g_svp_npu_task[0].cfg.total_t = 0;
        g_svp_npu_task[0].cfg.is_cached = HI_TRUE;
        g_svp_npu_task[0].cfg.model_idx = model_idx;
        ret = sample_svp_npu_acl_init_task(1, HI_FALSE, 0);
        sample_svp_check_exps_goto(ret != HI_SUCCESS, process_end1, SAMPLE_SVP_ERR_LEVEL_ERROR, "init task failed!\n");
    }

    if (g_svp_npu_terminate_signal == HI_FALSE) {
        ret = sample_common_svp_npu_get_input_data(src_file, SAMPLE_SVP_NPU_RESNET50_INPUT_FILE_NUM,
            &g_svp_npu_task[0]);
        sample_svp_check_exps_goto(ret != HI_SUCCESS, process_end2, SAMPLE_SVP_ERR_LEVEL_ERROR,
            "get input data failed!\n");
        ret = sample_common_svp_npu_set_dynamic_batch(&g_svp_npu_task[0]);
        sample_svp_check_exps_goto(ret != HI_SUCCESS, process_end2, SAMPLE_SVP_ERR_LEVEL_ERROR,
            "set dynamic batch failed!\n");
    }
    if (g_svp_npu_terminate_signal == HI_FALSE) {
        ret = sample_common_svp_npu_model_execute(&g_svp_npu_task[0]);
        sample_svp_check_exps_goto(ret != HI_SUCCESS, process_end2, SAMPLE_SVP_ERR_LEVEL_ERROR, "execute failed!\n");
        (hi_void)sample_common_svp_npu_output_classification_result(&g_svp_npu_task[0]);
    }

process_end2:
    (hi_void)sample_svp_npu_acl_deinit_task(1, 0);
process_end1:
    (hi_void)sample_common_svp_npu_unload_model(model_idx);
process_end0:
    (hi_void)sample_svp_npu_acl_deinit();
    (hi_void)sample_svp_npu_acl_terminate();
}

/* function : show the sample of svp npu resnet50 multi thread */
hi_void sample_svp_npu_acl_resnet50_multi_thread(hi_void)
{
    hi_u32 i;
    hi_s32 ret;
    const hi_u32 model_idx = 0;
    const hi_char *acl_config_path = "";
    const hi_char *src_file[SAMPLE_SVP_NPU_RESNET50_INPUT_FILE_NUM] = {"./data/image/3_224_224_batch_1.bgr"};
    const hi_char *om_model_path = "./data/model/resnet50.om";

    g_svp_npu_terminate_signal = HI_FALSE;

    if (g_svp_npu_terminate_signal == HI_FALSE) {
        /* init acl */
        ret = sample_svp_npu_acl_init(acl_config_path);
        sample_svp_check_exps_return_void(ret != HI_SUCCESS, SAMPLE_SVP_ERR_LEVEL_ERROR, "init failed!\n");

        /* load model */
        ret = sample_common_svp_npu_load_model(om_model_path, model_idx, HI_FALSE);
        sample_svp_check_exps_goto(ret != HI_SUCCESS, process_end0, SAMPLE_SVP_ERR_LEVEL_ERROR,
            "load model failed!\n");

        /* set cfg */
        for (i = 0; i < SAMPLE_SVP_NPU_MAX_TASK_NUM; i++) {
            g_svp_npu_task[i].cfg.max_batch_num = 1;
            g_svp_npu_task[i].cfg.dynamic_batch_num = 1;
            g_svp_npu_task[i].cfg.total_t = 0;
            g_svp_npu_task[i].cfg.is_cached = HI_FALSE;
            g_svp_npu_task[i].cfg.model_idx = model_idx;
        }

        ret = sample_svp_npu_acl_init_task(SAMPLE_SVP_NPU_MAX_TASK_NUM, HI_TRUE, 0);
        sample_svp_check_exps_goto(ret != HI_SUCCESS, process_end1, SAMPLE_SVP_ERR_LEVEL_ERROR, "init task failed!\n");
    }

    if (g_svp_npu_terminate_signal == HI_FALSE) {
        for (i = 0; i < SAMPLE_SVP_NPU_MAX_TASK_NUM; i++) {
            ret = sample_common_svp_npu_get_input_data(src_file, SAMPLE_SVP_NPU_RESNET50_INPUT_FILE_NUM,
                &g_svp_npu_task[i]);
            sample_svp_check_exps_goto(ret != HI_SUCCESS, process_end2,
                SAMPLE_SVP_ERR_LEVEL_ERROR, "get %u-th input failed!\n", i);
        }
    }

    if (g_svp_npu_terminate_signal == HI_FALSE) {
        (hi_void)sample_svp_npu_acl_model_execute_multithread();
    }

process_end2:
    (hi_void)sample_svp_npu_acl_deinit_task(SAMPLE_SVP_NPU_MAX_TASK_NUM, 0);
process_end1:
    (hi_void)sample_common_svp_npu_unload_model(model_idx);
process_end0:
    (hi_void)sample_svp_npu_acl_deinit();
    (hi_void)sample_svp_npu_acl_terminate();
}

hi_void sample_svp_npu_acl_lstm(hi_void)
{
    hi_s32 ret;
    const hi_u32 model_idx = 0;
    const hi_char *src_file[SAMPLE_SVP_NPU_LSTM_INPUT_FILE_NUM] = {"./data/vector/xt.seq", "./data/vector/cont.vec",
        "./data/vector/h0.vec", "./data/vector/c0.vec"};
    const hi_char *om_model_path = "./data/model/lstm.om";

    g_svp_npu_terminate_signal = HI_FALSE;

    if (g_svp_npu_terminate_signal == HI_FALSE) {
        /* init acl */
        ret = sample_svp_npu_acl_init(HI_NULL);
        sample_svp_check_exps_return_void(ret != HI_SUCCESS, SAMPLE_SVP_ERR_LEVEL_ERROR, "init failed!\n");

        /* load model */
        ret = sample_common_svp_npu_load_model(om_model_path, model_idx, HI_TRUE);
        sample_svp_check_exps_goto(ret != HI_SUCCESS, process_end0, SAMPLE_SVP_ERR_LEVEL_ERROR,
            "load model failed!\n");

        /* set task cfg */
        g_svp_npu_task[0].cfg.max_batch_num = 1;
        g_svp_npu_task[0].cfg.dynamic_batch_num = 1;
        g_svp_npu_task[0].cfg.total_t = 557; /* 557 is total t */
        g_svp_npu_task[0].cfg.is_cached = HI_TRUE;
        g_svp_npu_task[0].cfg.model_idx = model_idx;
        ret = sample_svp_npu_acl_init_task(1, HI_FALSE, 0);
        sample_svp_check_exps_goto(ret != HI_SUCCESS, process_end1, SAMPLE_SVP_ERR_LEVEL_ERROR, "init task failed!\n");
    }

    if (g_svp_npu_terminate_signal == HI_FALSE) {
        ret = sample_common_svp_npu_get_input_data(src_file, SAMPLE_SVP_NPU_LSTM_INPUT_FILE_NUM, &g_svp_npu_task[0]);
        sample_svp_check_exps_goto(ret != HI_SUCCESS, process_end2, SAMPLE_SVP_ERR_LEVEL_ERROR,
            "get input data failed!\n");
        ret = sample_common_svp_npu_set_dynamic_batch(&g_svp_npu_task[0]);
        sample_svp_check_exps_goto(ret != HI_SUCCESS, process_end2, SAMPLE_SVP_ERR_LEVEL_ERROR,
            "set dynamic batch failed!\n");
    }
    if (g_svp_npu_terminate_signal == HI_FALSE) {
        ret = sample_common_svp_npu_model_execute(&g_svp_npu_task[0]);
        sample_svp_check_exps_goto(ret != HI_SUCCESS, process_end2, SAMPLE_SVP_ERR_LEVEL_ERROR, "execute failed!\n");
    }

process_end2:
    (hi_void)sample_svp_npu_acl_deinit_task(1, 0);
process_end1:
    (hi_void)sample_common_svp_npu_unload_model(model_idx);
process_end0:
    (hi_void)sample_svp_npu_acl_deinit();
    (hi_void)sample_svp_npu_acl_terminate();
}

static hi_void sample_svp_npu_acl_set_task_info(hi_u32 model_idx)
{
    g_svp_npu_task[0].cfg.max_batch_num = 1;
    g_svp_npu_task[0].cfg.dynamic_batch_num = 1;
    g_svp_npu_task[0].cfg.total_t = 0;
    g_svp_npu_task[0].cfg.is_cached = HI_TRUE;
    g_svp_npu_task[0].cfg.model_idx = model_idx;
}

/* function : show the sample of rfcn */
hi_void sample_svp_npu_acl_rfcn(hi_void)
{
    hi_s32 ret;
    const hi_u32 model_idx = 0;
    const hi_char *om_model_path = "./data/model/rfcn.om";
    sample_svp_npu_detection_info detection_info = {0};
    sample_svp_npu_thread_args args;

    detection_info.num_name = "detection_filter_3_0";
    detection_info.roi_name = "detection_filter_3_";
    detection_info.has_background = HI_TRUE;
    g_svp_npu_terminate_signal = HI_FALSE;
    args.model_name = SAMPLE_SVP_NPU_RFCN;
    args.detection_info = &detection_info;

    if (g_svp_npu_terminate_signal == HI_FALSE) {
        /* init acl */
        ret = sample_svp_npu_acl_init(HI_NULL);
        sample_svp_check_exps_return_void(ret != HI_SUCCESS, SAMPLE_SVP_ERR_LEVEL_ERROR, "init failed!\n");

        /* start vdec vpss venc vo */
        ret = sample_common_svp_create_vb_start_vdec_vpss_vo(&g_svp_npu_vdec_cfg, &g_svp_npu_vdec_param,
            &g_svp_npu_vdec_thread, &g_svp_npu_media_cfg, &g_svp_npu_vo_cfg);
        sample_svp_check_exps_goto(ret != HI_SUCCESS, process_end0, SAMPLE_SVP_ERR_LEVEL_DEBUG,
            "init media failed!\n");

        /* load model */
        ret = sample_common_svp_npu_load_model(om_model_path, model_idx, HI_FALSE);
        sample_svp_check_exps_goto(ret != HI_SUCCESS, process_end1, SAMPLE_SVP_ERR_LEVEL_ERROR,
            "load model failed!\n");

        /* set cfg */
        sample_svp_npu_acl_set_task_info(model_idx);

        ret = sample_svp_npu_acl_init_task(1, HI_FALSE, 0);
        sample_svp_check_exps_goto(ret != HI_SUCCESS, process_end2, SAMPLE_SVP_ERR_LEVEL_ERROR, "init task failed!\n");
    }
    /* process */
    if (g_svp_npu_terminate_signal == HI_FALSE) {
        ret = sample_common_svp_npu_set_threshold(g_svp_npu_rfcn_threshold, SAMPLE_SVP_NPU_RFCN_THRESHOLD_NUM,
            &g_svp_npu_task[0]);
        sample_svp_check_exps_goto(ret != HI_SUCCESS, process_end3, SAMPLE_SVP_ERR_LEVEL_ERROR,
            "set threshold failed!\n");

        g_svp_npu_thread_stop = HI_FALSE;
        ret = pthread_create(&g_svp_npu_thread, 0, sample_svp_npu_acl_vdec_to_vo, (hi_void*)&args);
        sample_svp_check_exps_goto(ret != 0, process_end3, SAMPLE_SVP_ERR_LEVEL_ERROR, "create thread failed!\n");

        (hi_void)sample_svp_npu_acl_pause();

        g_svp_npu_thread_stop = HI_TRUE;
        pthread_join(g_svp_npu_thread, HI_NULL);
    }

process_end3:
    (hi_void)sample_svp_npu_acl_deinit_task(1, 0);
process_end2:
    (hi_void)sample_common_svp_npu_unload_model(model_idx);
process_end1:
    (hi_void)sample_common_svp_destroy_vb_stop_vdec_vpss_vo(&g_svp_npu_vdec_param, &g_svp_npu_vdec_thread,
        &g_svp_npu_media_cfg, &g_svp_npu_vo_cfg);
process_end0:
    (hi_void)sample_svp_npu_acl_deinit();
    (hi_void)sample_svp_npu_acl_terminate();
}

hi_void sample_svp_npu_acl_event(hi_void)
{
    hi_s32 ret;
    const hi_u32 model_num = 2;
    const hi_char *acl_config_path = "";
    const hi_char *src_file[SAMPLE_SVP_NPU_RESNET50_INPUT_FILE_NUM] = {"./data/image/3_224_224_batch_1.bgr"};
    const hi_char *om_model_path = "./data/model/resnet50.om";
    const hi_u32 proc_cnt = 5;
    hi_u32 i;

    svp_acl_rt_event event;

    /* init acl */
    ret = sample_svp_npu_acl_init(acl_config_path);
    sample_svp_check_exps_return_void(ret != HI_SUCCESS, SAMPLE_SVP_ERR_LEVEL_ERROR, "init failed!\n");

    ret = sample_svp_npu_acl_create_stream(model_num);
    sample_svp_check_exps_goto(ret != HI_SUCCESS, process_end0, SAMPLE_SVP_ERR_LEVEL_ERROR, "create stream failed!\n");

    ret = svp_acl_rt_create_event(&event);
    sample_svp_check_exps_goto(ret != HI_SUCCESS, process_end1, SAMPLE_SVP_ERR_LEVEL_ERROR, "create events failed!\n");

    /* load model */
    ret = sample_svp_npu_acl_load_multi_model(om_model_path, model_num, HI_FALSE, src_file,
        SAMPLE_SVP_NPU_RESNET50_INPUT_FILE_NUM);
    sample_svp_check_exps_goto(ret != HI_SUCCESS, process_end2, SAMPLE_SVP_ERR_LEVEL_ERROR, "load model failed!\n");

    for (i = 0; i < proc_cnt; i++) {
        ret = sample_common_svp_npu_model_execute_async(&g_svp_npu_task[0], g_svp_npu_stream[0]);
        sample_svp_check_exps_goto(ret != HI_SUCCESS, process_end3, SAMPLE_SVP_ERR_LEVEL_ERROR, "execute failed!\n");

        ret = svp_acl_rt_record_event(event, g_svp_npu_stream[0]);
        sample_svp_check_exps_goto(ret != HI_SUCCESS, process_end3, SAMPLE_SVP_ERR_LEVEL_ERROR,
            "record event failed!\n");

        ret = svp_acl_rt_stream_wait_event(g_svp_npu_stream[1], event);
        sample_svp_check_exps_goto(ret != HI_SUCCESS, process_end3, SAMPLE_SVP_ERR_LEVEL_ERROR,
            "wait event failed!\n");

        ret = sample_common_svp_npu_model_execute_async(&g_svp_npu_task[1], g_svp_npu_stream[1]);
        sample_svp_check_exps_goto(ret != HI_SUCCESS, process_end3, SAMPLE_SVP_ERR_LEVEL_ERROR, "execute failed!\n");

        ret = svp_acl_rt_synchronize_event(event);
        sample_svp_check_exps_goto(ret != HI_SUCCESS, process_end3, SAMPLE_SVP_ERR_LEVEL_ERROR,
            "sync event failed!\n");

        ret = svp_acl_rt_reset_event(event, g_svp_npu_stream[0]);
        sample_svp_check_exps_goto(ret != HI_SUCCESS, process_end3, SAMPLE_SVP_ERR_LEVEL_ERROR,
            "reset event failed!\n");
    }

    ret = svp_acl_rt_synchronize_stream(g_svp_npu_stream[1]);
    sample_svp_check_exps_goto(ret != HI_SUCCESS, process_end3, SAMPLE_SVP_ERR_LEVEL_ERROR, "sync stream failed!\n");

process_end3:
    (hi_void)sample_svp_npu_acl_unload_multi_model(model_num);

process_end2:
    (hi_void)svp_acl_rt_destroy_event(event);

process_end1:
    (hi_void)sample_svp_npu_acl_destroy_stream(model_num);

process_end0:
    (hi_void)sample_svp_npu_acl_deinit();
    (hi_void)sample_svp_npu_acl_terminate();
}

static hi_s32 sample_svp_npu_acl_set_input_aipp_info(td_u32 model_index, td_u32 index)
{
    const sample_svp_npu_model_aipp model_aipp = {
        {
            SVP_ACL_YUV420SP_U8,
            1, 1, 0,
            720, 576,       /* src image size */
            1192, 2166, 0,  /* csc coef: yuv420 -> BGR */
            1192, -218, -547,
            1192, 0, 1836,
            0, 0, 0,
            16, 128, 128
        }, {
            1, AIPP_PADDING_SWITCH_REPLICATION, AIPP_RESIZE_SWITCH_BILINEAR, 1,
            0, 0, 716, 572, /* crop param */
            2, 2, 2, 2, /* padding param */
            720, 576, 1920, 1080, /* resize param */
            1, 1, 1, 1, 0, 0, 0, 0, -32768, -32768, -32768, -32768 /* preprocess param */
        }
    };
    return sample_common_svp_npu_set_input_aipp_info(model_index, index, &model_aipp, &g_svp_npu_task[model_index]);
}

static hi_s32 sample_svp_npu_acl_set_aipp_info(td_u32 model_index)
{
    hi_s32 ret;

    ret = sample_svp_npu_acl_set_input_aipp_info(model_index, 0);
    sample_svp_check_exps_goto(ret != HI_SUCCESS, fail, SAMPLE_SVP_ERR_LEVEL_ERROR, "set input aipp failed!\n");

    return HI_SUCCESS;

fail:
    return ret;
}

hi_void sample_svp_npu_acl_aipp(hi_void)
{
    hi_s32 ret;
    const hi_u32 model_idx = 0;
    const hi_char *acl_config_path = "";
    const hi_char *src_file[SAMPLE_SVP_NPU_RESNET50_INPUT_FILE_NUM] = {"./data/image/st_lk_720x576_420sp.yuv"};
    const hi_char *om_model_path = "./data/model/aipp.om";

    /* init acl */
    ret = sample_svp_npu_acl_init(acl_config_path);
    sample_svp_check_exps_return_void(ret != HI_SUCCESS, SAMPLE_SVP_ERR_LEVEL_ERROR, "init failed!\n");

    /* load model */
    ret = sample_common_svp_npu_load_model(om_model_path, model_idx, HI_FALSE);
    sample_svp_check_exps_goto(ret != HI_SUCCESS, process_end0, SAMPLE_SVP_ERR_LEVEL_ERROR,
        "load model failed!\n");

    /* set task cfg */
    g_svp_npu_task[0].cfg.max_batch_num = 1;
    g_svp_npu_task[0].cfg.dynamic_batch_num = 1;
    g_svp_npu_task[0].cfg.total_t = 0;
    g_svp_npu_task[0].cfg.is_cached = HI_FALSE;
    g_svp_npu_task[0].cfg.model_idx = model_idx;

    ret = sample_svp_npu_acl_init_task(1, HI_FALSE, 0);
    sample_svp_check_exps_goto(ret != HI_SUCCESS, process_end1, SAMPLE_SVP_ERR_LEVEL_ERROR,
        "init task failed!\n");

    ret = sample_svp_npu_acl_set_aipp_info(model_idx);
    sample_svp_check_exps_goto(ret != HI_SUCCESS, process_end1, SAMPLE_SVP_ERR_LEVEL_ERROR,
        "set aipp info failed!\n");

    ret = sample_common_svp_npu_get_input_data(src_file, SAMPLE_SVP_NPU_RESNET50_INPUT_FILE_NUM,
        &g_svp_npu_task[0]);
    sample_svp_check_exps_goto(ret != HI_SUCCESS, process_end2, SAMPLE_SVP_ERR_LEVEL_ERROR, "get data failed!\n");

    ret = sample_common_svp_npu_model_execute(&g_svp_npu_task[0]);
    sample_svp_check_exps_goto(ret != HI_SUCCESS, process_end2, SAMPLE_SVP_ERR_LEVEL_ERROR, "execute failed!\n");

process_end2:
    (hi_void)sample_svp_npu_acl_deinit_task(1, 0);
process_end1:
    (hi_void)sample_common_svp_npu_unload_model(model_idx);
process_end0:
    (hi_void)sample_svp_npu_acl_deinit();
    (hi_void)sample_svp_npu_acl_terminate();
}

hi_void sample_svp_npu_acl_preemption(hi_void)
{
    hi_s32 ret;
    const hi_u32 model_num = 2;
    const hi_char *acl_config_path = "";
    const hi_char *src_file[SAMPLE_SVP_NPU_RESNET50_INPUT_FILE_NUM] = {"./data/image/3_224_224_batch_1.bgr"};
    const hi_char *om_model_path = "./data/model/resnet50_with_preemption.om";
    hi_u32 i, j;
    hi_u32 idx[SAMPLE_SVP_NPU_MAX_MODEL_NUM] = {0};
    pthread_t svp_npu_multi_thread[SAMPLE_SVP_NPU_MAX_MODEL_NUM] = {0};

    sample_svp_npu_model_prior_cfg prior_cfg[] = {
        { SAMPLE_SVP_NPU_PRIORITY_2, 1, 0, 0},
        { SAMPLE_SVP_NPU_PRIORITY_4, 1, SAMPLE_SVP_NPU_PRIOR_TIMEOUT, 0 }
    };

    /* init acl */
    ret = sample_svp_npu_acl_init(acl_config_path);
    sample_svp_check_exps_return_void(ret != HI_SUCCESS, SAMPLE_SVP_ERR_LEVEL_ERROR, "init failed!\n");

    for (i = 0; i < model_num; i++) {
        sample_svp_npu_set_model_prior_info(i, &prior_cfg[i]);
    }

    /* load model */
    ret = sample_svp_npu_acl_load_multi_model(om_model_path, model_num, HI_TRUE, src_file,
        SAMPLE_SVP_NPU_RESNET50_INPUT_FILE_NUM);
    sample_svp_check_exps_goto(ret != HI_SUCCESS, process_end0, SAMPLE_SVP_ERR_LEVEL_ERROR, "load model failed!\n");

    g_svp_npu_is_thread_start = HI_TRUE;

    for (i = 0; i < model_num; i++) {
        idx[i] = i;
        ret = pthread_create(&svp_npu_multi_thread[i], NULL, sample_svp_npu_acl_thread_multi_model_process, &idx[i]);
        sample_svp_check_exps_goto(ret != HI_SUCCESS, process_end1, TEST_PICO_ERR_LEVEL_ERROR,
            "pthread_create failed!\n");
    }

    sleep(SAMPLE_SVP_NPU_PREEMPTION_SLEEP_SECOND);

process_end1:
    g_svp_npu_is_thread_start = HI_FALSE;
    for (j = 0; j < i; j++) {
        pthread_join(svp_npu_multi_thread[j], NULL);
    }

    (hi_void)sample_svp_npu_acl_unload_multi_model(model_num);

process_end0:
    (hi_void)sample_svp_npu_acl_deinit();
    (hi_void)sample_svp_npu_acl_terminate();
}

static hi_void sample_svp_npu_acl_set_detection_info(sample_svp_npu_detection_info *detection_info, hi_u32 index)
{
    detection_info->num_name = g_svp_npu_yolo_roi_info[index].roi_num_name;
    detection_info->roi_name = g_svp_npu_yolo_roi_info[index].roi_class_name;
    detection_info->has_background = HI_FALSE;
    /* use PIC_BUTT to be a flag, get the input resolution form om */
    g_svp_npu_media_cfg.pic_type[1] = PIC_BUTT;
    g_svp_npu_terminate_signal = HI_FALSE;
}


/* function : show the sample of e2e(vi -> vpss -> npu -> vo) yolo */
hi_void sample_svp_npu_acl_e2e_yolo(hi_u32 index)
{
    hi_s32 ret;
    const hi_u32 model_idx = 0;
    hi_char om_model_path[SAMPLE_SVP_NPU_PATH_LEN] = {0};
    
    sample_ive_kcf_init2();
    
    //maohw for return;
    static sample_svp_npu_detection_info detection_info = {0};
    static sample_svp_npu_thread_args args;
    
    __args = &args;
    args.model_name = SAMPLE_SVP_NPU_YOLO;
    args.detection_info = &detection_info;

    g_svp_npu_terminate_signal = HI_FALSE;
    
    ret = sprintf_s(om_model_path, SAMPLE_SVP_NPU_PATH_LEN - 1, "%s", sample_svp_model_path(NULL));
    sample_svp_check_exps_return_void(ret <= 0, SAMPLE_SVP_ERR_LEVEL_ERROR, "sprintf_s failed!\n");

    sample_svp_npu_acl_set_detection_info(&detection_info, index);

    if (g_svp_npu_terminate_signal == HI_FALSE) {
        /* init acl */
        ret = sample_svp_npu_acl_init(HI_NULL);
        sample_svp_check_exps_return_void(ret != HI_SUCCESS, SAMPLE_SVP_ERR_LEVEL_ERROR, "init failed!\n");

        /* load model */
        ret = sample_common_svp_npu_load_model(om_model_path, model_idx, HI_FALSE);
        sample_svp_check_exps_goto(ret != HI_SUCCESS, process_end0, SAMPLE_SVP_ERR_LEVEL_ERROR,
            "load model failed!\n");

        /* get input resolution */
        ret = sample_common_svp_npu_get_input_resolution(model_idx, 0, &g_svp_npu_media_cfg.pic_size[1]);
        sample_svp_check_exps_goto(ret != HI_SUCCESS, process_end1, SAMPLE_SVP_ERR_LEVEL_ERROR,
            "load model failed!\n");
            
        printf("npu_get_input_resolution: w:%d, h:%d\n", g_svp_npu_media_cfg.pic_size[1].width, g_svp_npu_media_cfg.pic_size[1].height);    
   
        #if 0 //maohw
        /* start vi vpss vo */
        ret = sample_common_svp_npu_start_vi_vpss_vo(&g_vi_config, &g_svp_npu_media_cfg, &g_svp_npu_vo_cfg);
        sample_svp_check_exps_goto(ret != HI_SUCCESS, process_end1, SAMPLE_SVP_ERR_LEVEL_DEBUG,
            "init media failed!\n");
        #endif

        /* set cfg */
        sample_svp_npu_acl_set_task_info(model_idx);

        ret = sample_svp_npu_acl_init_task(1, HI_FALSE, 0);
        sample_svp_check_exps_goto(ret != HI_SUCCESS, process_end2, SAMPLE_SVP_ERR_LEVEL_ERROR, "init task failed!\n");
    }
    /* process */
    if (g_svp_npu_terminate_signal == HI_FALSE) {

        ret = sample_common_svp_npu_set_threshold(&g_svp_npu_yolo_threshold[index], SAMPLE_SVP_NPU_YOLO_THRESHOLD_NUM,
            &g_svp_npu_task[0]);
        sample_svp_check_exps_goto(ret != HI_SUCCESS, process_end3, SAMPLE_SVP_ERR_LEVEL_ERROR,
            "set threshold failed!\n");

        g_svp_npu_thread_stop = HI_FALSE;
        
      #if 0 //maohw  
        ret = pthread_create(&g_svp_npu_thread, 0, sample_svp_npu_acl_vdec_to_vo, (hi_void*)&args);
        sample_svp_check_exps_goto(ret != 0, process_end3, SAMPLE_SVP_ERR_LEVEL_ERROR, "create thread failed!\n");
          
        (hi_void)sample_svp_npu_acl_pause();

        g_svp_npu_thread_stop = HI_TRUE;
        pthread_join(g_svp_npu_thread, HI_NULL);
      #else 
       return;
      #endif
    }

process_end3:
    (hi_void)sample_svp_npu_acl_deinit_task(1, 0);
process_end2:
    //maohw (hi_void)sample_common_svp_destroy_vb_stop_vi_vpss_vo(&g_vi_config, &g_svp_npu_media_cfg, &g_svp_npu_vo_cfg);
process_end1:
    (hi_void)sample_common_svp_npu_unload_model(model_idx);
process_end0:
    (hi_void)sample_svp_npu_acl_deinit();
    (hi_void)sample_svp_npu_acl_terminate();
}

/* function : show the sample of yolo */
hi_void sample_svp_npu_acl_yolo(hi_u32 index)
{
    hi_s32 ret;
    const hi_u32 model_idx = 0;
    hi_char om_model_path[SAMPLE_SVP_NPU_PATH_LEN] = {0};
    sample_svp_npu_detection_info detection_info = {0};
    sample_svp_npu_thread_args args = {SAMPLE_SVP_NPU_YOLO, &detection_info};

    ret = sprintf_s(om_model_path, SAMPLE_SVP_NPU_PATH_LEN - 1, "%s", sample_svp_model_path(NULL));    
    sample_svp_check_exps_return_void(ret <= 0, SAMPLE_SVP_ERR_LEVEL_ERROR, "sprintf_s failed!\n");

    sample_svp_npu_acl_set_detection_info(&detection_info, index);

    if (g_svp_npu_terminate_signal == HI_FALSE) {
        /* init acl */
        ret = sample_svp_npu_acl_init(HI_NULL);
        sample_svp_check_exps_return_void(ret != HI_SUCCESS, SAMPLE_SVP_ERR_LEVEL_ERROR, "init failed!\n");

        /* load model */
        ret = sample_common_svp_npu_load_model(om_model_path, model_idx, HI_FALSE);
        sample_svp_check_exps_goto(ret != HI_SUCCESS, process_end0, SAMPLE_SVP_ERR_LEVEL_ERROR,
            "load model failed!\n");

        /* get input resolution */
        ret = sample_common_svp_npu_get_input_resolution(model_idx, 0, &g_svp_npu_media_cfg.pic_size[1]);
        sample_svp_check_exps_goto(ret != HI_SUCCESS, process_end1, SAMPLE_SVP_ERR_LEVEL_ERROR,
            "load model failed!\n");

        /* start vdec vpss venc vo */
        ret = sample_common_svp_create_vb_start_vdec_vpss_vo(&g_svp_npu_vdec_cfg, &g_svp_npu_vdec_param,
            &g_svp_npu_vdec_thread, &g_svp_npu_media_cfg, &g_svp_npu_vo_cfg);
        sample_svp_check_exps_goto(ret != HI_SUCCESS, process_end1, SAMPLE_SVP_ERR_LEVEL_DEBUG,
            "init media failed!\n");

        /* set cfg */
        sample_svp_npu_acl_set_task_info(model_idx);

        ret = sample_svp_npu_acl_init_task(1, HI_FALSE, 0);
        sample_svp_check_exps_goto(ret != HI_SUCCESS, process_end2, SAMPLE_SVP_ERR_LEVEL_ERROR, "init task failed!\n");
    }
    /* process */
    if (g_svp_npu_terminate_signal == HI_FALSE) {
        ret = sample_common_svp_npu_set_threshold(&g_svp_npu_yolo_threshold[index], SAMPLE_SVP_NPU_YOLO_THRESHOLD_NUM,
            &g_svp_npu_task[0]);
        sample_svp_check_exps_goto(ret != HI_SUCCESS, process_end3, SAMPLE_SVP_ERR_LEVEL_ERROR,
            "set threshold failed!\n");

        g_svp_npu_thread_stop = HI_FALSE;
        ret = pthread_create(&g_svp_npu_thread, 0, sample_svp_npu_acl_vdec_to_vo, (hi_void*)&args);
        sample_svp_check_exps_goto(ret != 0, process_end3, SAMPLE_SVP_ERR_LEVEL_ERROR, "create thread failed!\n");

        (hi_void)sample_svp_npu_acl_pause();

        g_svp_npu_thread_stop = HI_TRUE;
        pthread_join(g_svp_npu_thread, HI_NULL);
    }

process_end3:
    (hi_void)sample_svp_npu_acl_deinit_task(1, 0);
process_end2:
    (hi_void)sample_common_svp_destroy_vb_stop_vdec_vpss_vo(&g_svp_npu_vdec_param, &g_svp_npu_vdec_thread,
        &g_svp_npu_media_cfg, &g_svp_npu_vo_cfg);
process_end1:
    (hi_void)sample_common_svp_npu_unload_model(model_idx);
process_end0:
    (hi_void)sample_svp_npu_acl_deinit();
    (hi_void)sample_svp_npu_acl_terminate();
}

/* function : show the sample of e2e(vi -> vpss -> npu -> vo) hrnet */
hi_void sample_svp_npu_acl_e2e_hrnet()
{
    hi_s32 ret;
    const hi_u32 model_idx = 0;
    const hi_char *om_model_path = "./data/model/hrnet_original.om";
    sample_svp_npu_thread_args args;

    args.model_name = SAMPLE_SVP_NPU_HRNET;
    args.detection_info = HI_NULL;

    if (g_svp_npu_terminate_signal == HI_FALSE) {
        /* init acl */
        ret = sample_svp_npu_acl_init(HI_NULL);
        sample_svp_check_exps_return_void(ret != HI_SUCCESS, SAMPLE_SVP_ERR_LEVEL_ERROR, "init failed!\n");

        /* load model */
        ret = sample_common_svp_npu_load_model(om_model_path, model_idx, HI_FALSE);
        sample_svp_check_exps_goto(ret != HI_SUCCESS, process_end0, SAMPLE_SVP_ERR_LEVEL_ERROR,
            "load model failed!\n");

        /* get input resolution */
        g_svp_npu_media_cfg.pic_type[1] = PIC_BUTT;
        ret = sample_common_svp_npu_get_input_resolution(model_idx, 0, &g_svp_npu_media_cfg.pic_size[1]);
        sample_svp_check_exps_goto(ret != HI_SUCCESS, process_end1, SAMPLE_SVP_ERR_LEVEL_ERROR,
            "load model failed!\n");

        /* start vi vpss vo */
        ret = sample_common_svp_npu_start_vi_vpss_vo(&g_vi_config, &g_svp_npu_media_cfg, &g_svp_npu_vo_cfg);
        sample_svp_check_exps_goto(ret != HI_SUCCESS, process_end1, SAMPLE_SVP_ERR_LEVEL_DEBUG,
            "init media failed!\n");

        /* set cfg */
        sample_svp_npu_acl_set_task_info(model_idx);

        ret = sample_svp_npu_acl_init_task(1, HI_FALSE, 0);
        sample_svp_check_exps_goto(ret != HI_SUCCESS, process_end2, SAMPLE_SVP_ERR_LEVEL_ERROR, "init task failed!\n");
    }
    /* process */
    if (g_svp_npu_terminate_signal == HI_FALSE) {
        g_svp_npu_thread_stop = HI_FALSE;
        ret = pthread_create(&g_svp_npu_thread, 0, sample_svp_npu_acl_vdec_to_vo, (hi_void*)&args);
        sample_svp_check_exps_goto(ret != 0, process_end3, SAMPLE_SVP_ERR_LEVEL_ERROR, "create thread failed!\n");

        (hi_void)sample_svp_npu_acl_pause();

        g_svp_npu_thread_stop = HI_TRUE;
        pthread_join(g_svp_npu_thread, HI_NULL);
    }
process_end3:
    (hi_void)sample_svp_npu_acl_deinit_task(1, 0);
process_end2:
    (hi_void)sample_common_svp_destroy_vb_stop_vi_vpss_vo(&g_vi_config, &g_svp_npu_media_cfg, &g_svp_npu_vo_cfg);
process_end1:
    (hi_void)sample_common_svp_npu_unload_model(model_idx);
process_end0:
    (hi_void)sample_svp_npu_acl_deinit();
    (hi_void)sample_svp_npu_acl_terminate();
}

#if 1 //maohw
int sample_svp_npu_init(char *model_path)
{
  sample_svp_model_path(model_path);
  sample_svp_npu_acl_e2e_yolo(5);
  return 0;
}

int sample_svp_npu_detect(hi_video_frame_info *ext_frame, hi_video_frame_info *base_frame, yolo_boxs_t *boxs)
{
    hi_s32 ret;
    hi_u32 size, stride;
    hi_u8 *data = HI_NULL;
    static int _init = 0;
    
    __boxs = boxs;
    
    if(_init == 0)
    {
      _init = 1;
      
      (hi_void)prctl(PR_SET_NAME, "svp_npu_vdec_to_vo", 0, 0, 0);

      ret = svp_acl_rt_set_device(g_svp_npu_dev_id);
      sample_svp_check_exps_return(ret != HI_SUCCESS, HI_NULL, SAMPLE_SVP_ERR_LEVEL_ERROR, "open device failed!\n");
      
      ret = sample_common_svp_npu_get_input_data_buffer_info(&g_svp_npu_task[0], 0, &data, &size, &stride);
      sample_svp_check_exps_goto(ret != HI_SUCCESS, fail_0, SAMPLE_SVP_ERR_LEVEL_ERROR,
          "Error(%#x),get_input_data_buffer_info failed!\n", ret);
    }

    ret = sample_svp_npu_acl_frame_proc(ext_frame, base_frame, __args);
    sample_svp_check_exps_goto(ret != HI_SUCCESS, fail_0, SAMPLE_SVP_ERR_LEVEL_ERROR,
        "Error(%#x),sample_svp_npu_acl_frame_proc failed!\n", ret);
fail_0:
    return ret;
}


int sample_svp_npu_destroy(void)
{
    sample_svp_npu_acl_handle_sig();
  
    (hi_void)svp_acl_rt_reset_device(g_svp_npu_dev_id);

    const hi_u32 model_idx = 0;
    (hi_void)sample_svp_npu_acl_deinit_task(1, 0);
    //(hi_void)sample_common_svp_destroy_vb_stop_vi_vpss_vo(&g_vi_config, &g_svp_npu_media_cfg, &g_svp_npu_vo_cfg);
    (hi_void)sample_common_svp_npu_unload_model(model_idx);
    (hi_void)sample_svp_npu_acl_deinit();
    (hi_void)sample_svp_npu_acl_terminate();

    sample_ive_kcf_deinit2();
    return 0;
}
#endif