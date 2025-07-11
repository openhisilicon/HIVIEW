/*
  Copyright (c), 2001-2024, Shenshu Tech. Co., Ltd.
 */
#include "sample_rotation_compensation.h"

#include <fcntl.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include "hi_motionsensor_mng_cmd.h"
#include "hi_common_motionfusion.h"
#include "hi_mpi_motionfusion.h"
#include "sample_comm.h"
#include "sample_dis.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

typedef struct {
    pthread_t pthread_id;
    hi_msensor_buf_attr msensor_buf_attr;
} sample_dis_pthread_info;
sample_dis_pthread_info g_pthread_info = { 0 };
hi_s32 g_msensor_userfd = -1;
hi_s32 g_msensor_mng_dev_fd = -1;

const hi_mfusion_quaternion_data g_quaternion_data = {    // 90°
    (hi_s32)(0.7071 * (1 << 20)),  // w
    0,                             // x
    0,                             // y
    (hi_s32)(0.7071 * (1 << 20)), // z
    0                             // pts
};

static hi_s32 sample_dis_pthread_get_msensordata(sample_dis_pthread_info *pthread_info,
    hi_u64 begin_pts, hi_u64 end_pts, hi_mfusion_quaternion_buf *quaternion_buf)
{
    hi_u32 i, j;
    hi_s32 ret;
    hi_u32 num;
    hi_u64 *pts_viraddr = HI_NULL;
    hi_msensor_data_info msensor_data_info = {0};
    msensor_data_info.id = g_msensor_userfd;
    msensor_data_info.data_type = MSENSOR_DATA_GYRO;
    msensor_data_info.begin_pts = begin_pts;
    msensor_data_info.end_pts = end_pts;

    ret = ioctl(g_msensor_mng_dev_fd, MSENSOR_CMD_GET_DATA, &msensor_data_info);
    if (ret != HI_SUCCESS) {
        sample_print("MSENSOR_CMD_GET_DATA failed\n");
        return HI_FAILURE;
    }

    quaternion_buf->num = 0;
    for (i = 0; i < 2; i++) { /* cyclic buffer 2 */
        num = msensor_data_info.data[i].num;
        if (num == 0) {
            continue;
        }
        pts_viraddr = (hi_u64 *)(hi_uintptr_t)(pthread_info->msensor_buf_attr.virt_addr +
            ((hi_u64)(hi_uintptr_t)msensor_data_info.data[i].pts_phys_addr - pthread_info->msensor_buf_attr.phys_addr));

        for (j = 0; j < num; j++) {
            quaternion_buf->data[quaternion_buf->num + j].pts = pts_viraddr[j];
            quaternion_buf->data[quaternion_buf->num + j].w = g_quaternion_data.w;
            quaternion_buf->data[quaternion_buf->num + j].x = g_quaternion_data.x;
            quaternion_buf->data[quaternion_buf->num + j].y = g_quaternion_data.y;
            quaternion_buf->data[quaternion_buf->num + j].z = g_quaternion_data.z;
        }

        quaternion_buf->num += num;
    }

    return ret;
}

hi_bool g_send_quaternion_en = HI_TRUE;
hi_void *sample_dis_pthread_send_quaternion(void *arg)
{
    hi_s32 ret;
    hi_u64 begin_pts = 0;
    hi_u64 end_pts = 0;
    hi_u32 fusion_id = 0;

    hi_mfusion_quaternion_buf quaternion_buf;
    sample_dis_pthread_info *pthread_info = (sample_dis_pthread_info *)arg;

    while (g_send_quaternion_en == HI_TRUE) {
        hi_mpi_sys_get_cur_pts(&end_pts);

        usleep(2000); /* sleep 2000 us */
        if (begin_pts != 0) {
            sample_dis_pthread_get_msensordata(pthread_info, begin_pts, end_pts, &quaternion_buf);
            ret = hi_mpi_mfusion_send_quaternion(fusion_id, &quaternion_buf);
            if (ret != HI_SUCCESS) {
                sample_print("hi_mpi_motionfusion_send_quaternion failed!\n");
                return HI_NULL;
            }
            begin_pts = end_pts + 1;
        } else {
            begin_pts = end_pts - 10000; /* get for 10000 us */
        }
    }

    return HI_NULL;
}

hi_s32 sample_motionfuson_start_rotation_compensation(hi_msensor_param *msensor_param)
{
    const hi_u32 fusion_id = 0;
    hi_s32 ret;
    hi_mfusion_rotation_cfg cfg;
    hi_mfusion_rotation_compensation compensation;

    sample_dis_pause("Init rotation compensation");

    cfg.buf_size = 102400; /* buffer size is 102400 */
    ret = hi_mpi_mfusion_init_rotation_compensation(fusion_id, &cfg);
    if (ret != HI_SUCCESS) {
        sample_print("hi_mpi_mfusion_init_rotation_compensation failed!\n");
        return HI_FAILURE;
    }

    g_msensor_mng_dev_fd = open("/dev/motionsensor_mng", O_RDWR);
    if (g_msensor_mng_dev_fd < 0) {
        sample_print("open motionsensor_mng failed\n");
        ret = HI_FAILURE;
        goto deinit;
    }

    ret = ioctl(g_msensor_mng_dev_fd, MSENSOR_CMD_ADD_USER, &g_msensor_userfd);
    if (ret != HI_SUCCESS) {
        sample_print("MSENSOR_CMD_ADD_USER failed!\n");
        ret = HI_FAILURE;
        goto close_fd;
    }

    memcpy_s(&g_pthread_info.msensor_buf_attr, sizeof(hi_msensor_buf_attr), msensor_param, sizeof(hi_msensor_buf_attr));

    if (pthread_create(&g_pthread_info.pthread_id, NULL, sample_dis_pthread_send_quaternion, &g_pthread_info) != 0) {
        ret = HI_FAILURE;
        goto delete_user;
    }
    g_send_quaternion_en = HI_TRUE;

    sample_dis_pause("Enable rotation compensation!");

    ret = hi_mpi_mfusion_get_rotation_compensation(fusion_id, &compensation);
    if (ret != HI_SUCCESS) {
        sample_print("hi_mpi_mfusion_get_rotation_compensation failed!\n");
        goto pthread_exit;
    }

    compensation.enable = HI_TRUE;
    compensation.crop_ratio = 80; /* crop ratio as 80 */
    ret = hi_mpi_mfusion_set_rotation_compensation(fusion_id, &compensation);
    if (ret != HI_SUCCESS) {
        sample_print("hi_mpi_mfusion_set_rotation_compensation failed!\n");
        goto pthread_exit;
    }

    return HI_SUCCESS;

pthread_exit:
    g_send_quaternion_en = HI_FALSE;
    pthread_join(g_pthread_info.pthread_id, NULL);
delete_user:
    ioctl(g_msensor_mng_dev_fd, MSENSOR_CMD_DELETE_USER, &g_msensor_userfd);
    g_msensor_userfd = -1;
close_fd:
    close(g_msensor_mng_dev_fd);
    g_msensor_mng_dev_fd = -1;
deinit:
    hi_mpi_mfusion_deinit_rotation_compensation(fusion_id);

    return ret;
}

hi_s32 sample_motionfuson_stop_rotation_compensation(hi_void)
{
    hi_u32 fusion_id = 0;
    hi_s32 ret;
    hi_mfusion_rotation_compensation compensation;

    compensation.enable = HI_FALSE;
    compensation.crop_ratio = 100; /* crop ratio as 100 is full */
    ret = hi_mpi_mfusion_set_rotation_compensation(fusion_id, &compensation);
    if (ret != HI_SUCCESS) {
        sample_print("hi_mpi_motionfusion_set_rotation_compensation failed!\n");
        hi_mpi_mfusion_deinit_rotation_compensation(fusion_id);
        return HI_FAILURE;
    }

    if (g_send_quaternion_en == HI_TRUE) {
        g_send_quaternion_en = HI_FALSE;
        pthread_join(g_pthread_info.pthread_id, NULL);
    }

    if (g_msensor_userfd > 0) {
        ioctl(g_msensor_mng_dev_fd, MSENSOR_CMD_DELETE_USER, &g_msensor_userfd);
        g_msensor_userfd = -1;
    }

    if (g_msensor_mng_dev_fd > 0) {
        close(g_msensor_mng_dev_fd);
        g_msensor_mng_dev_fd = -1;
    }

    hi_mpi_sys_munmap((hi_void *)(hi_uintptr_t)g_pthread_info.msensor_buf_attr.virt_addr,
        g_pthread_info.msensor_buf_attr.buf_len);

    ret = hi_mpi_mfusion_deinit_rotation_compensation(fusion_id);

    return ret;
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of __cplusplus */

