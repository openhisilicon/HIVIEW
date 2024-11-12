/*
  Copyright (c), 2001-2024, Shenshu Tech. Co., Ltd.
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
#ifdef CONFIG_OT_ISP_THERMO_SUPPORT
#include "hi_mpi_thermo.h"
#endif
#include "hi_sns_ctrl.h"
#include "hi_mpi_isp.h"
#include "hi_common_isp.h"
#include "hi_common_video.h"
#include "sample_comm.h"

static sample_sns_type g_sns_type[HI_VI_MAX_PIPE_NUM] = {SNS_TYPE_BUTT};
static pthread_t g_isp_pid[HI_VI_MAX_PIPE_NUM] = {0};

/* IspPub attr */

static hi_isp_pub_attr g_isp_pub_attr_sc4336p_mipi_4m_30fps = {
    { 0, 0, 2560, 1440 },
    { 2560, 1440 },
    30,
    HI_ISP_BAYER_BGGR,
    HI_WDR_MODE_NONE,
    0,
    0,
    0,
    {
        0,
        { 0, 0, 2560, 1440 },
    },
};

static hi_isp_pub_attr g_isp_pub_attr_sc4336p_mipi_3m_30fps = {
    { 0, 0, 2304, 1296 },
    { 2304, 1296 },
    30,
    HI_ISP_BAYER_BGGR,
    HI_WDR_MODE_NONE,
    0,
    0,
    0,
    {
        0,
        { 0, 0, 2304, 1296 },
    },
};

static hi_isp_pub_attr g_isp_pub_attr_os04d10_mipi_4m_30fps = {
    { 0, 0, 2560, 1440 },
    { 2560, 1440 },
    30,
    OT_ISP_BAYER_BGGR,
    HI_WDR_MODE_NONE,
    0,
    0,
    0,
    {
        0,
        { 0, 0, 2560, 1440 },
    },
};

static hi_isp_pub_attr g_isp_pub_attr_os04a10_mipi_4m_30fps = {
    {0, 0, 2688, 1520},
    {2688, 1520},
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
    {0, 0, 2688, 1520},
    {2688, 1520},
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


static hi_isp_pub_attr g_isp_pub_attr_sc431hai_mipi_4m_30fps = {
    {0, 0, 2560, 1440},
    { 2560, 1440 },
    30,
    HI_ISP_BAYER_BGGR,
    HI_WDR_MODE_NONE,
    0,
    0,
    0,
    {
        0,
        {0, 0, 2560, 1440},
    },
};

static hi_isp_pub_attr g_isp_pub_attr_sc431hai_mipi_4m_30fps_wdr2to1 = {
    {0, 0, 2560, 1440},
    { 2560, 1440 },
    30,
    HI_ISP_BAYER_BGGR,
    HI_WDR_MODE_2To1_LINE,
    0,
    0,
    0,
    {
        0,
        {0, 0, 2560, 1440},
    },
};

static hi_isp_pub_attr g_isp_pub_attr_sc450ai_mipi_4m_30fps = {
    {0, 0, 2688, 1520},
    { 2688, 1520 },
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
    {0, 0, 2688, 1520},
    { 2688, 1520 },
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

static hi_isp_pub_attr g_isp_pub_attr_sc500ai_mipi_5m_30fps = {
    {0, 0, 2880, 1620},
    { 2880, 1620 },
    30,
    HI_ISP_BAYER_BGGR,
    HI_WDR_MODE_NONE,
    0,
    0,
    0,
    {
        0,
        {0, 0, 2880, 1620},
    },
};

static hi_isp_pub_attr g_isp_pub_attr_sc500ai_mipi_5m_30fps_wdr2to1 = {
    {0, 0, 2880, 1620},
    { 2880, 1620 },
    30,
    HI_ISP_BAYER_BGGR,
    HI_WDR_MODE_2To1_LINE,
    0,
    0,
    0,
    {
        0,
        {0, 0, 2880, 1620},
    },
};

static hi_isp_pub_attr g_isp_pub_attr_gc4023_mipi_4m_30fps = {
    { 0, 0, 2560, 1440 },
    { 2560, 1440 },
    30,
    HI_ISP_BAYER_RGGB,
    HI_WDR_MODE_NONE,
    0,
    0,
    0,
    {
        0,
        { 0, 0, 2560, 1440 },
    },
};

hi_void sample_comm_isp_get_pub_attr_by_sns_part1(sample_sns_type sns_type, hi_isp_pub_attr *pub_attr)
{
    switch (sns_type) {
        case SC500AI_MIPI_5M_30FPS_10BIT:
            (hi_void)memcpy_s(pub_attr, sizeof(hi_isp_pub_attr),
                &g_isp_pub_attr_sc500ai_mipi_5m_30fps, sizeof(hi_isp_pub_attr));
            break;
        case SC500AI_MIPI_5M_30FPS_10BIT_WDR2TO1:
            (hi_void)memcpy_s(pub_attr, sizeof(hi_isp_pub_attr),
                &g_isp_pub_attr_sc500ai_mipi_5m_30fps_wdr2to1, sizeof(hi_isp_pub_attr));
            break;
        case OS04D10_MIPI_4M_30FPS_10BIT:
            (hi_void)memcpy_s(pub_attr, sizeof(hi_isp_pub_attr),
                &g_isp_pub_attr_os04d10_mipi_4m_30fps, sizeof(hi_isp_pub_attr));
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
        default:
            break;
    }
}


hi_s32 sample_comm_isp_get_pub_attr_by_sns(sample_sns_type sns_type, hi_isp_pub_attr *pub_attr)
{
    switch (sns_type) {
        case SC4336P_MIPI_4M_30FPS_10BIT:
            (hi_void)memcpy_s(pub_attr, sizeof(hi_isp_pub_attr),
                &g_isp_pub_attr_sc4336p_mipi_4m_30fps, sizeof(hi_isp_pub_attr));
            break;
        case SC4336P_MIPI_3M_30FPS_10BIT:
            (hi_void)memcpy_s(pub_attr, sizeof(hi_isp_pub_attr),
                &g_isp_pub_attr_sc4336p_mipi_3m_30fps, sizeof(hi_isp_pub_attr));
            break;
        case GC4023_MIPI_4M_30FPS_10BIT:
            (hi_void)memcpy_s(pub_attr, sizeof(hi_isp_pub_attr),
                &g_isp_pub_attr_gc4023_mipi_4m_30fps, sizeof(hi_isp_pub_attr));
            break;
        case SC431HAI_MIPI_4M_30FPS_10BIT:
            (hi_void)memcpy_s(pub_attr, sizeof(hi_isp_pub_attr),
                &g_isp_pub_attr_sc431hai_mipi_4m_30fps, sizeof(hi_isp_pub_attr));
            break;
        case SC431HAI_MIPI_4M_30FPS_10BIT_WDR2TO1:
            (hi_void)memcpy_s(pub_attr, sizeof(hi_isp_pub_attr),
                &g_isp_pub_attr_sc431hai_mipi_4m_30fps_wdr2to1, sizeof(hi_isp_pub_attr));
            break;
        case SC450AI_MIPI_4M_30FPS_10BIT:
            (hi_void)memcpy_s(pub_attr, sizeof(hi_isp_pub_attr),
                &g_isp_pub_attr_sc450ai_mipi_4m_30fps, sizeof(hi_isp_pub_attr));
            break;
        case SC450AI_MIPI_4M_30FPS_10BIT_WDR2TO1:
            (hi_void)memcpy_s(pub_attr, sizeof(hi_isp_pub_attr),
                &g_isp_pub_attr_sc450ai_mipi_4m_30fps_wdr2to1, sizeof(hi_isp_pub_attr));
            break;

        default:
            (hi_void)memcpy_s(pub_attr, sizeof(hi_isp_pub_attr),
                &g_isp_pub_attr_sc4336p_mipi_4m_30fps, sizeof(hi_isp_pub_attr));
            break;
    }
    sample_comm_isp_get_pub_attr_by_sns_part1(sns_type, pub_attr);
    return HI_SUCCESS;
}
#if 0 //maohw
hi_isp_sns_obj *sample_comm_isp_get_sns_obj(sample_sns_type sns_type)
{
    switch (sns_type) {
#ifdef SC4336P_MIPI_4M_30FPS_10BIT_SELECT
        case SC4336P_MIPI_4M_30FPS_10BIT:
            return &g_sns_sc4336p_obj;
#endif
#ifdef SC4336P_MIPI_3M_30FPS_10BIT_SELECT
        case SC4336P_MIPI_3M_30FPS_10BIT:
            return &g_sns_sc4336p_obj;
#endif
#ifdef GC4023_MIPI_4M_30FPS_10BIT_SELECT
        case GC4023_MIPI_4M_30FPS_10BIT:
            return &g_sns_gc4023_obj;
#endif
#ifdef OS04D10_MIPI_4M_30FPS_10BIT_SELECT
        case OS04D10_MIPI_4M_30FPS_10BIT:
            return &g_sns_os04d10_obj;
#endif
#ifdef SC431HAI_MIPI_4M_30FPS_10BIT_SELECT
        case SC431HAI_MIPI_4M_30FPS_10BIT:
        case SC431HAI_MIPI_4M_30FPS_10BIT_WDR2TO1:
            return &g_sns_sc431hai_obj;
#endif
#ifdef SC450AI_MIPI_4M_30FPS_10BIT_SELECT
        case SC450AI_MIPI_4M_30FPS_10BIT:
        case SC450AI_MIPI_4M_30FPS_10BIT_WDR2TO1:
            return &g_sns_sc450ai_obj;
#endif
#ifdef SC500AI_MIPI_5M_30FPS_10BIT_SELECT
        case SC500AI_MIPI_5M_30FPS_10BIT:
        case SC500AI_MIPI_5M_30FPS_10BIT_WDR2TO1:
            return &g_sns_sc500ai_obj;
#endif
#ifdef OV_OS04A10_MIPI_4M_30FPS_12BIT_SELECT
      case OV_OS04A10_MIPI_4M_30FPS_12BIT:
      case OV_OS04A10_MIPI_4M_30FPS_12BIT_WDR2TO1:
          return &g_sns_os04a10_obj;
      //case OV_OS04A10_SLAVE_MIPI_4M_30FPS_12BIT:
      //    return &g_sns_os04a10_slave_obj;
#endif
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
        ret = sns_obj->pfn_register_callback(isp_dev, &ae_lib, &awb_lib);
        if (ret != HI_SUCCESS) {
            printf("sensor_register_callback failed with %#x!\n", ret);
            return ret;
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
#ifdef CONFIG_OT_ISP_THERMO_SUPPORT
    hi_isp_3a_alg_lib thermo_lib;
#endif
    hi_isp_3a_alg_lib awb_lib;
    hi_isp_sns_obj *sns_obj;
    hi_s32 ret;

    sns_obj = sample_comm_isp_get_sns_obj(g_sns_type[isp_dev]);
    if (sns_obj == HI_NULL) {
        printf("sensor %d not exist!\n", g_sns_type[isp_dev]);
        return HI_FAILURE;
    }

    ae_lib.id = isp_dev;
#ifdef CONFIG_OT_ISP_THERMO_SUPPORT
    thermo_lib.id = isp_dev;
#endif
    awb_lib.id = isp_dev;
    ret = strncpy_s(ae_lib.lib_name, sizeof(ae_lib.lib_name), HI_AE_LIB_NAME, sizeof(HI_AE_LIB_NAME));
    sample_check_eok_return(ret, HI_FAILURE);
#ifdef CONFIG_OT_ISP_THERMO_SUPPORT
    ret = strncpy_s(thermo_lib.lib_name, sizeof(thermo_lib.lib_name), HI_THERMO_LIB_NAME, sizeof(HI_THERMO_LIB_NAME));
    sample_check_eok_return(ret, HI_FAILURE);
#endif
    ret = strncpy_s(awb_lib.lib_name, sizeof(awb_lib.lib_name), HI_AWB_LIB_NAME, sizeof(HI_AWB_LIB_NAME));
    sample_check_eok_return(ret, HI_FAILURE);
    if (sns_obj->pfn_un_register_callback != HI_NULL) {
        ret = sns_obj->pfn_un_register_callback(isp_dev, &ae_lib, &awb_lib);
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

hi_s32 sample_comm_isp_thermo_lib_uncallback(hi_isp_dev isp_dev)
{
#ifdef CONFIG_OT_ISP_THERMO_SUPPORT
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
#endif
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
        
		//maohw;
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
