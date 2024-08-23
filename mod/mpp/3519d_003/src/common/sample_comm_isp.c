/*
  Copyright (c), 2001-2022, Shenshu Tech. Co., Ltd.
 */
#include <sched.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <poll.h>
#ifdef __LITEOS__
#include <sys/select.h>
#endif
#include <fcntl.h>
#include <errno.h>
#include <pthread.h>
#include <sys/time.h>
#include <sys/mman.h>
#include <signal.h>
#include <math.h>
#include <poll.h>
#include <sys/prctl.h>

#include "hi_mpi_awb.h"
#include "hi_mpi_ae.h"
#include "hi_mpi_thermo.h"
#include "hi_sns_ctrl.h"
#include "hi_mpi_isp.h"
#include "hi_common_isp.h"
#include "hi_common_video.h"
#include "sample_comm.h"

#include "mppex.h"

static sample_sns_type g_sns_type[HI_VI_MAX_PIPE_NUM] = {SNS_TYPE_BUTT};
static pthread_t g_isp_pid[HI_VI_MAX_DEV_NUM] = {0};

/* IspPub attr */
static hi_isp_pub_attr g_isp_pub_attr_os08a20_mipi_8m_30fps = {
    {0, 24, 3840, 2160},
    {3840, 2160},
    30,
    HI_ISP_BAYER_BGGR,
    HI_WDR_MODE_NONE,
    0,
    0,
    0,
    {
        0,
        {0, 0, 3840, 2180},
    },
};

static hi_isp_pub_attr g_isp_pub_attr_os08a20_mipi_8m_30fps_wdr2to1 = {
    {0, 24, 3840, 2160},
    {3840, 2160},
    30,
    HI_ISP_BAYER_BGGR,
    HI_WDR_MODE_2To1_LINE,
    0,
    0,
    0,
    {
        0,
        {0, 0, 3840, 2180},
    },
};

static hi_isp_pub_attr g_isp_pub_attr_os04a10_mipi_4m_30fps = {
    {0, 0, WIDTH_2688, HEIGHT_1520},
    {WIDTH_2688, HEIGHT_1520},
    30,
    HI_ISP_BAYER_RGGB,
    HI_WDR_MODE_NONE,
    0,
    0,
    0,
    {
        0,
        {0, 0, 2688, 1520},
    },
};

static hi_isp_pub_attr g_isp_pub_attr_os04a10_mipi_4m_30fps_wdr2to1 = {
    {0, 0, WIDTH_2688, HEIGHT_1520},
    {WIDTH_2688, HEIGHT_1520},
    30,
    HI_ISP_BAYER_RGGB,
    HI_WDR_MODE_2To1_LINE,
    0,
    0,
    0,
    {
        0,
        {0, 0, 2688, 1520},
    },
};

static hi_isp_pub_attr g_isp_pub_attr_thermo_gst412_30fps = {
    {8, 10, 384, 288},
    {384, 288},
    30,
    HI_ISP_BAYER_RGGB,
    HI_WDR_MODE_NONE,
    0,
    0,
    0,
    {
        0,
        {0, 0, 384, 288},
    },
};

static hi_isp_pub_attr g_isp_pub_attr_sc450ai_mipi_4m_30fps = {
    {0, 0, WIDTH_2688, HEIGHT_1520},
    {WIDTH_2688, HEIGHT_1520},
    30,
    HI_ISP_BAYER_BGGR,
    HI_WDR_MODE_NONE,
    0,
    0,
    0,
    {
        0,
        {0, 0, 2688, 1520},
    },
};

static hi_isp_pub_attr g_isp_pub_attr_sc450ai_mipi_4m_30fps_wdr2to1 = {
    {0, 0, WIDTH_2688, HEIGHT_1520},
    {WIDTH_2688, HEIGHT_1520},
    30,
    HI_ISP_BAYER_BGGR,
    HI_WDR_MODE_2To1_LINE,
    0,
    0,
    0,
    {
        0,
        {0, 0, 2688, 1520},
    },
};

static hi_isp_pub_attr g_isp_pub_attr_sc850sl_mipi_8m_30fps = {
    {0, 0, 3840, 2160},
    {3840, 2160},
    30,
    HI_ISP_BAYER_BGGR,
    HI_WDR_MODE_NONE,
    0,
    0,
    0,
    {
        0,
        {0, 0, 3840, 2180},
    },
};

static hi_isp_pub_attr g_isp_pub_attr_sc850sl_mipi_8m_30fps_wdr2to1 = {
    {0, 0, 3840, 2160},
    {3840, 2160},
    30,
    HI_ISP_BAYER_BGGR,
    HI_WDR_MODE_2To1_LINE,
    0,
    0,
    0,
    {
        0,
        {0, 0, 3840, 2180},
    },
};

static hi_isp_pub_attr g_isp_pub_attr_imx347_slave_mipi_4m_30fps = {
    {0, 20, WIDTH_2688, HEIGHT_1520},
    {WIDTH_2688, HEIGHT_1520},
    30,
    HI_ISP_BAYER_RGGB,
    HI_WDR_MODE_NONE,
    0,
    0,
    0,
    {
        0,
        {0, 0, 2688, 1540},
    },
};

static hi_isp_pub_attr g_isp_pub_attr_imx515_mipi_8m_30fps = {
    {0, 24, 3840, 2160},
    {3840, 2160},
    30,
    HI_ISP_BAYER_GBRG,
    HI_WDR_MODE_NONE,
    0,
    0,
    0,
    {
        0,
        {0, 0, 3840, 2180},
    },
};

static hi_void sample_comm_isp_get_pub_attr_by_sns_part1(sample_sns_type sns_type, hi_isp_pub_attr *pub_attr)
{
    switch (sns_type) {
        case OV_OS08A20_MIPI_8M_30FPS_12BIT:
            (hi_void)memcpy_s(pub_attr, sizeof(hi_isp_pub_attr),
                &g_isp_pub_attr_os08a20_mipi_8m_30fps, sizeof(hi_isp_pub_attr));
            break;
        case OV_OS08A20_MIPI_8M_30FPS_10BIT_WDR2TO1:
            (hi_void)memcpy_s(pub_attr, sizeof(hi_isp_pub_attr),
                &g_isp_pub_attr_os08a20_mipi_8m_30fps_wdr2to1, sizeof(hi_isp_pub_attr));
            break;

        case OV_OS04A10_MIPI_4M_30FPS_12BIT:
        case OV_OS04A10_SLAVE_MIPI_4M_30FPS_12BIT:
            (hi_void)memcpy_s(pub_attr, sizeof(hi_isp_pub_attr),
                &g_isp_pub_attr_os04a10_mipi_4m_30fps, sizeof(hi_isp_pub_attr));
            break;
        case OV_OS04A10_MIPI_4M_30FPS_12BIT_WDR2TO1:
            (hi_void)memcpy_s(pub_attr, sizeof(hi_isp_pub_attr),
                &g_isp_pub_attr_os04a10_mipi_4m_30fps_wdr2to1, sizeof(hi_isp_pub_attr));
            break;
        case GST_412C_SLAVE_THERMO_T3_384_288_30FPS_14BIT:
            (hi_void)memcpy_s(pub_attr, sizeof(hi_isp_pub_attr),
                &g_isp_pub_attr_thermo_gst412_30fps, sizeof(hi_isp_pub_attr));
            break;

        default:
            break;
    }
}


hi_s32 sample_comm_isp_get_pub_attr_by_sns(sample_sns_type sns_type, hi_isp_pub_attr *pub_attr)
{
    switch (sns_type) {
        case SONY_IMX347_SLAVE_MIPI_4M_30FPS_12BIT:
            (hi_void)memcpy_s(pub_attr, sizeof(hi_isp_pub_attr),
                &g_isp_pub_attr_imx347_slave_mipi_4m_30fps, sizeof(hi_isp_pub_attr));
            break;

        case SC450AI_MIPI_4M_30FPS_10BIT:
            (hi_void)memcpy_s(pub_attr, sizeof(hi_isp_pub_attr),
                &g_isp_pub_attr_sc450ai_mipi_4m_30fps, sizeof(hi_isp_pub_attr));
            break;
        case SC450AI_MIPI_4M_30FPS_10BIT_WDR2TO1:
            (hi_void)memcpy_s(pub_attr, sizeof(hi_isp_pub_attr),
                &g_isp_pub_attr_sc450ai_mipi_4m_30fps_wdr2to1, sizeof(hi_isp_pub_attr));
            break;

        case SC850SL_MIPI_8M_30FPS_12BIT:
            (hi_void)memcpy_s(pub_attr, sizeof(hi_isp_pub_attr),
                &g_isp_pub_attr_sc850sl_mipi_8m_30fps, sizeof(hi_isp_pub_attr));
            break;
        case SC850SL_MIPI_8M_30FPS_10BIT_WDR2TO1:
            (hi_void)memcpy_s(pub_attr, sizeof(hi_isp_pub_attr),
                &g_isp_pub_attr_sc850sl_mipi_8m_30fps_wdr2to1, sizeof(hi_isp_pub_attr));
            break;

        case SONY_IMX515_MIPI_8M_30FPS_12BIT:
            (hi_void)memcpy_s(pub_attr, sizeof(hi_isp_pub_attr),
                &g_isp_pub_attr_imx515_mipi_8m_30fps, sizeof(hi_isp_pub_attr));
            break;

        default:
            (hi_void)memcpy_s(pub_attr, sizeof(hi_isp_pub_attr),
                &g_isp_pub_attr_os08a20_mipi_8m_30fps, sizeof(hi_isp_pub_attr));
            break;
    }
    sample_comm_isp_get_pub_attr_by_sns_part1(sns_type, pub_attr);
    mppex_comm_isp_get_pub_attr_by_sns(sns_type, pub_attr);
    return HI_SUCCESS;
}

#if 0 //maohw mvto mpp.c;
hi_isp_sns_obj *sample_comm_isp_get_sns_obj(sample_sns_type sns_type)
{
    switch (sns_type) {
        case OV_OS08A20_MIPI_8M_30FPS_12BIT:
        case OV_OS08A20_MIPI_8M_30FPS_10BIT_WDR2TO1:
            return &g_sns_os08a20_obj;
        case OV_OS04A10_MIPI_4M_30FPS_12BIT:
        case OV_OS04A10_MIPI_4M_30FPS_12BIT_WDR2TO1:
            return &g_sns_os04a10_obj;
        case SC450AI_MIPI_4M_30FPS_10BIT:
        case SC450AI_MIPI_4M_30FPS_10BIT_WDR2TO1:
            return &g_sns_sc450ai_obj;
        case SC850SL_MIPI_8M_30FPS_12BIT:
        case SC850SL_MIPI_8M_30FPS_10BIT_WDR2TO1:
            return &g_sns_sc850sl_obj;
        case OV_OS04A10_SLAVE_MIPI_4M_30FPS_12BIT:
            return &g_sns_os04a10_slave_obj;
        case SONY_IMX347_SLAVE_MIPI_4M_30FPS_12BIT:
            return &g_sns_imx347_slave_obj;
        case SONY_IMX515_MIPI_8M_30FPS_12BIT:
            return &g_sns_imx515_obj;
        case GST_412C_SLAVE_THERMO_T3_384_288_30FPS_14BIT:
            return &g_sns_gst412c_obj;
        default:
            return HI_NULL;
    }
}
#endif

hi_isp_sns_type sample_comm_get_sns_bus_type(sample_sns_type sns_type)
{
    hi_unused(sns_type);
    return HI_ISP_SNS_TYPE_I2C;
}

/******************************************************************************
* funciton : ISP init
******************************************************************************/
hi_s32 sample_comm_isp_sensor_regiter_callback(hi_isp_dev isp_dev, sample_sns_type sns_type)
{
    hi_s32 ret;
    hi_isp_3a_alg_lib ae_lib;
    hi_isp_3a_alg_lib thermo_lib;
    hi_isp_3a_alg_lib awb_lib;
    hi_isp_sns_obj *sns_obj;

    sns_obj = sample_comm_isp_get_sns_obj(sns_type);
    if (sns_obj == HI_NULL) {
        printf("sensor %d not exist!\n", sns_type);
        return HI_FAILURE;
    }

    ae_lib.id = isp_dev;
    thermo_lib.id = isp_dev;
    awb_lib.id = isp_dev;
    ret = strncpy_s(ae_lib.lib_name, sizeof(ae_lib.lib_name), HI_AE_LIB_NAME, sizeof(HI_AE_LIB_NAME));
    sample_check_eok_return(ret, HI_FAILURE);
    ret = strncpy_s(thermo_lib.lib_name, sizeof(thermo_lib.lib_name), HI_THERMO_LIB_NAME, sizeof(HI_THERMO_LIB_NAME));
    sample_check_eok_return(ret, HI_FAILURE);
    strncpy_s(awb_lib.lib_name, sizeof(awb_lib.lib_name), HI_AWB_LIB_NAME, sizeof(HI_AWB_LIB_NAME));
    sample_check_eok_return(ret, HI_FAILURE);
    if (sns_obj->pfn_register_callback != HI_NULL) {
        if (sns_type == GST_412C_SLAVE_THERMO_T3_384_288_30FPS_14BIT) {
            ret = sns_obj->pfn_register_callback(isp_dev, &thermo_lib, &awb_lib);
            if (ret != HI_SUCCESS) {
                printf("sensor_register_callback failed with %#x!\n", ret);
                return ret;
            }
        } else {
            ret = sns_obj->pfn_register_callback(isp_dev, &ae_lib, &awb_lib);
            if (ret != HI_SUCCESS) {
                printf("sensor_register_callback failed with %#x!\n", ret);
                return ret;
            }
        }
    } else {
        printf("sensor_register_callback failed with HI_NULL!\n");
    }

    g_sns_type[isp_dev] = sns_type;

    return HI_SUCCESS;
}

hi_s32 sample_comm_isp_sensor_unregiter_callback(hi_isp_dev isp_dev)
{
    hi_isp_3a_alg_lib ae_lib;
    hi_isp_3a_alg_lib thermo_lib;
    hi_isp_3a_alg_lib awb_lib;
    hi_isp_sns_obj *sns_obj;
    hi_s32 ret;

    sns_obj = sample_comm_isp_get_sns_obj(g_sns_type[isp_dev]);
    if (sns_obj == HI_NULL) {
        printf("sensor %d not exist!\n", g_sns_type[isp_dev]);
        return HI_FAILURE;
    }

    ae_lib.id = isp_dev;
    thermo_lib.id = isp_dev;
    awb_lib.id = isp_dev;
    ret = strncpy_s(ae_lib.lib_name, sizeof(ae_lib.lib_name), HI_AE_LIB_NAME, sizeof(HI_AE_LIB_NAME));
    sample_check_eok_return(ret, HI_FAILURE);
    ret = strncpy_s(thermo_lib.lib_name, sizeof(thermo_lib.lib_name), HI_THERMO_LIB_NAME, sizeof(HI_THERMO_LIB_NAME));
    sample_check_eok_return(ret, HI_FAILURE);
    ret = strncpy_s(awb_lib.lib_name, sizeof(awb_lib.lib_name), HI_AWB_LIB_NAME, sizeof(HI_AWB_LIB_NAME));
    sample_check_eok_return(ret, HI_FAILURE);
    if (sns_obj->pfn_un_register_callback != HI_NULL) {
        if (g_sns_type[isp_dev] == GST_412C_SLAVE_THERMO_T3_384_288_30FPS_14BIT) {
            ret = sns_obj->pfn_un_register_callback(isp_dev, &thermo_lib, &awb_lib);
        } else {
            ret = sns_obj->pfn_un_register_callback(isp_dev, &ae_lib, &awb_lib);
        }
        if (ret != HI_SUCCESS) {
            printf("sensor_unregister_callback failed with %#x!\n", ret);
            return ret;
        }
    } else {
        printf("sensor_unregister_callback failed with HI_NULL!\n");
    }

    g_sns_type[isp_dev] = SNS_TYPE_BUTT;

    return HI_SUCCESS;
}


hi_s32 sample_comm_isp_bind_sns(hi_isp_dev isp_dev, sample_sns_type sns_type, hi_s8 sns_dev)
{
    hi_isp_sns_commbus sns_bus_info;
    hi_isp_sns_type    bus_type;
    hi_isp_sns_obj    *sns_obj;
    hi_s32 ret;

    sns_obj = sample_comm_isp_get_sns_obj(sns_type);
    if (sns_obj == HI_NULL) {
        printf("sensor %d not exist!\n", sns_type);
        return HI_FAILURE;
    }

    bus_type = sample_comm_get_sns_bus_type(sns_type);
    if (bus_type == HI_ISP_SNS_TYPE_I2C) {
        sns_bus_info.i2c_dev = sns_dev;
    } else {
        sns_bus_info.ssp_dev.bit4_ssp_dev = sns_dev;
        sns_bus_info.ssp_dev.bit4_ssp_cs  = 0;
    }

    if (sns_obj->pfn_set_bus_info != HI_NULL) {
        ret = sns_obj->pfn_set_bus_info(isp_dev, sns_bus_info);
        if (ret != HI_SUCCESS) {
            printf("set sensor bus info failed with %#x!\n", ret);
            return ret;
        }
    } else {
        printf("not support set sensor bus info!\n");
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

hi_s32 sample_comm_isp_thermo_lib_callback(hi_isp_dev isp_dev)
{
    hi_s32 ret;
    hi_isp_3a_alg_lib thermo_lib;

    thermo_lib.id = isp_dev;
    ret = strncpy_s(thermo_lib.lib_name, sizeof(thermo_lib.lib_name), HI_THERMO_LIB_NAME, sizeof(HI_THERMO_LIB_NAME));
    sample_check_eok_return(ret, HI_FAILURE);
    ret = hi_mpi_thermo_register(isp_dev, &thermo_lib);
    if (ret != HI_SUCCESS) {
        printf("hi_mpi_thermo_register failed with %#x!\n", ret);
        return ret;
    }

    return HI_SUCCESS;
}

hi_s32 sample_comm_isp_thermo_lib_uncallback(hi_isp_dev isp_dev)
{
    hi_s32 ret;
    hi_isp_3a_alg_lib thermo_lib;

    thermo_lib.id = isp_dev;
    ret = strncpy_s(thermo_lib.lib_name, sizeof(thermo_lib.lib_name), HI_THERMO_LIB_NAME, sizeof(HI_THERMO_LIB_NAME));
    sample_check_eok_return(ret, HI_FAILURE);
    ret = hi_mpi_thermo_unregister(isp_dev, &thermo_lib);
    if (ret != HI_SUCCESS) {
        printf("hi_mpi_thermo_unregister failed with %#x!\n", ret);
        return ret;
    }

    return HI_SUCCESS;
}

hi_s32 sample_comm_isp_ae_lib_callback(hi_isp_dev isp_dev)
{
    hi_s32 ret;
    hi_isp_3a_alg_lib ae_lib;

    ae_lib.id = isp_dev;
    ret = strncpy_s(ae_lib.lib_name, sizeof(ae_lib.lib_name), HI_AE_LIB_NAME, sizeof(HI_AE_LIB_NAME));
    sample_check_eok_return(ret, HI_FAILURE);
    ret = hi_mpi_ae_register(isp_dev, &ae_lib);
    if (ret != HI_SUCCESS) {
        printf("hi_mpi_ae_register failed with %#x!\n", ret);
        return ret;
    }

    return HI_SUCCESS;
}

hi_s32 sample_comm_isp_ae_lib_uncallback(hi_isp_dev isp_dev)
{
    hi_s32 ret;
    hi_isp_3a_alg_lib ae_lib;

    ae_lib.id = isp_dev;
    ret = strncpy_s(ae_lib.lib_name, sizeof(ae_lib.lib_name), HI_AE_LIB_NAME, sizeof(HI_AE_LIB_NAME));
    sample_check_eok_return(ret, HI_FAILURE);
    ret = hi_mpi_ae_unregister(isp_dev, &ae_lib);
    if (ret != HI_SUCCESS) {
        printf("hi_mpi_ae_unregister failed with %#x!\n", ret);
        return ret;
    }

    return HI_SUCCESS;
}

hi_s32 sample_comm_isp_awb_lib_callback(hi_isp_dev isp_dev)
{
    hi_s32 ret;
    hi_isp_3a_alg_lib awb_lib;

    awb_lib.id = isp_dev;
    ret = strncpy_s(awb_lib.lib_name, sizeof(awb_lib.lib_name), HI_AWB_LIB_NAME, sizeof(HI_AWB_LIB_NAME));
    sample_check_eok_return(ret, HI_FAILURE);
    ret = hi_mpi_awb_register(isp_dev, &awb_lib);
    if (ret != HI_SUCCESS) {
        printf("hi_mpi_awb_register failed with %#x!\n", ret);
        return ret;
    }

    return HI_SUCCESS;
}

hi_s32 sample_comm_isp_awb_lib_uncallback(hi_isp_dev isp_dev)
{
    hi_s32 ret;
    hi_isp_3a_alg_lib awb_lib;

    awb_lib.id = isp_dev;
    ret = strncpy_s(awb_lib.lib_name, sizeof(awb_lib.lib_name), HI_AWB_LIB_NAME, sizeof(HI_AWB_LIB_NAME));
    sample_check_eok_return(ret, HI_FAILURE);
    ret = hi_mpi_awb_unregister(isp_dev, &awb_lib);
    if (ret != HI_SUCCESS) {
        printf("hi_mpi_awb_unregister failed with %#x!\n", ret);
        return ret;
    }

    return HI_SUCCESS;
}

/******************************************************************************
* funciton : ISP Run
******************************************************************************/
static void *sample_comm_isp_thread(hi_void *param)
{
    hi_s32 ret;
    hi_isp_dev isp_dev;
    hi_char thread_name[20];
    isp_dev = (hi_isp_dev)(hi_uintptr_t)param;
    errno_t err;

    err = snprintf_s(thread_name, sizeof(thread_name), sizeof(thread_name) - 1, "ISP%d_RUN", isp_dev); /* 20,19 chars */
    if (err < 0) {
        return NULL;
    }
    prctl(PR_SET_NAME, thread_name, 0, 0, 0);

    printf("ISP Dev %d running !\n", isp_dev);
    ret = hi_mpi_isp_run(isp_dev);
    if (ret != HI_SUCCESS) {
        printf("HI_MPI_ISP_Run failed with %#x!\n", ret);
        return NULL;
    }

    return NULL;
}

hi_s32 sample_comm_isp_run(hi_isp_dev isp_dev)
{
    hi_s32 ret;
    pthread_attr_t *thread_attr = NULL;
    printf("%s => isp_dev:%d", __func__, isp_dev);
    ret = pthread_create(&g_isp_pid[isp_dev], thread_attr, sample_comm_isp_thread, (hi_void*)(hi_uintptr_t)isp_dev);
    if (ret != 0) {
        printf("create isp running thread failed!, error: %d\r\n", ret);
    }

    if (thread_attr != HI_NULL) {
        pthread_attr_destroy(thread_attr);
    }
    return ret;
}

hi_void sample_comm_isp_stop(hi_isp_dev isp_dev)
{
    if (g_isp_pid[isp_dev]) {
		    //maohw
        hi_mpi_isp_exit(isp_dev);
        pthread_join(g_isp_pid[isp_dev], NULL);
        sample_comm_isp_awb_lib_uncallback(isp_dev);
        sample_comm_isp_ae_lib_uncallback(isp_dev);
        sample_comm_isp_sensor_unregiter_callback(isp_dev);
        g_isp_pid[isp_dev] = 0;
    }
    return;
}


hi_void sample_comm_all_isp_stop(hi_void)
{
    hi_isp_dev isp_dev;

    for (isp_dev = 0; isp_dev < HI_VI_MAX_PIPE_NUM; isp_dev++) {
        sample_comm_isp_stop(isp_dev);
    }
}