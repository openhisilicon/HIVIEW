

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* End of #ifdef __cplusplus */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <sys/poll.h>
#include <sys/time.h>
#include <fcntl.h>
#include <errno.h>
#include <pthread.h>
#include <math.h>
#include <unistd.h>
#include <signal.h>
#include <sys/prctl.h>

#include "sample_comm.h"

#if 0 //maohw
#define MAX_SENSOR_NUM      1
#define ISP_MAX_DEV_NUM     2
#endif

static pthread_t    g_IspPid[ISP_MAX_DEV_NUM] = {0};
static HI_U32       g_au32IspSnsId[ISP_MAX_DEV_NUM] = {0, 1};

#if 0 //maohw
SAMPLE_SNS_TYPE_E g_enSnsType[MAX_SENSOR_NUM] =
{
    SENSOR0_TYPE,
};
#endif

ISP_PUB_ATTR_S ISP_PUB_ATTR_IMX327_2M_30FPS =
{
    {0, 0, 1920, 1080},
    {1920, 1080},
    30,
    BAYER_RGGB,
    WDR_MODE_NONE,
    0,
};

ISP_PUB_ATTR_S ISP_PUB_ATTR_IMX327_MIPI_2M_30FPS_WDR2TO1_LINE =
{
    {0, 0, 1920, 1080},
    {1920, 1080},
    30,
    BAYER_RGGB,
    WDR_MODE_2To1_LINE,
    0,
};

ISP_PUB_ATTR_S ISP_PUB_ATTR_IMX307_MIPI_2M_30FPS =
{
    {0, 0, 1920, 1080},
    {1920, 1080},
    30,
    BAYER_RGGB,
    WDR_MODE_NONE,
    0,
};

ISP_PUB_ATTR_S ISP_PUB_ATTR_IMX307_MIPI_2M_30FPS_WDR2TO1_LINE =
{
    {0, 0, 1920, 1080},
    {1920, 1080},
    30,
    BAYER_RGGB,
    WDR_MODE_2To1_LINE,
    0,
};

ISP_PUB_ATTR_S ISP_PUB_ATTR_SC4236_MIPI_3M_30FPS =
{
    {0, 0, 2304, 1296},
    {2304, 1296},
    30,
    BAYER_BGGR,
    WDR_MODE_NONE,
    0,
};

ISP_PUB_ATTR_S ISP_PUB_ATTR_SC4236_MIPI_3M_20FPS =
{
    {0, 0, 2304, 1296},
    {2304, 1296},
    20,
    BAYER_BGGR,
    WDR_MODE_NONE,
    0,
};

ISP_PUB_ATTR_S ISP_PUB_ATTR_SC2231_MIPI_2M_30FPS =
{
    {0, 0, 1920, 1080},
    {1920, 1080},
    30,
    BAYER_BGGR,
    WDR_MODE_NONE,
    0,
};

ISP_PUB_ATTR_S ISP_PUB_ATTR_SC3235_MIPI_3M_30FPS =
{
    {0, 0, 2304, 1296},
    {2304, 1296},
    30,
    BAYER_BGGR,
    WDR_MODE_NONE,
    0,
};

ISP_PUB_ATTR_S ISP_PUB_ATTR_SC2235_MIPI_2M_30FPS =
{
    {0, 0, 1920, 1080},
    {1920, 1080},
    30,
    BAYER_BGGR,
    WDR_MODE_NONE,
    0,
};

ISP_PUB_ATTR_S ISP_PUB_ATTR_AR0130_MIPI_1M_30FPS =
{
    {0, 0, 1280, 960},
    {1280, 960},
    30,
    BAYER_GRBG,
    WDR_MODE_NONE,
    0,
};

ISP_PUB_ATTR_S ISP_PUB_ATTR_GC2053_MIPI_2M_30FPS =
{
    {0, 0, 1920, 1080},
    {1920, 1080},
    30,
    BAYER_RGGB,
    WDR_MODE_NONE,
    0,
};


ISP_PUB_ATTR_S ISP_PUB_ATTR_IMX335_MIPI_5M_30FPS =
{
    {0, 0, 2592, 1944},
    {2592, 1944},
    30,
    BAYER_RGGB,
    WDR_MODE_NONE,
    0,
};

ISP_PUB_ATTR_S ISP_PUB_ATTR_IMX335_MIPI_5M_30FPS_WDR2TO1 =
{
    {0, 0, 2592, 1944},
    {2592, 1944},
    30,
    BAYER_RGGB,
    WDR_MODE_2To1_LINE,
    0,
};

ISP_PUB_ATTR_S ISP_PUB_ATTR_IMX335_MIPI_4M_30FPS =
{
    {0, 0, 2592, 1520},
    {2592, 1520},
    30,
    BAYER_RGGB,
    WDR_MODE_NONE,
    0,
};

ISP_PUB_ATTR_S ISP_PUB_ATTR_IMX335_MIPI_4M_30FPS_WDR2TO1 =
{
    {0, 0, 2592, 1520},
    {2592, 1520},
    30,
    BAYER_RGGB,
    WDR_MODE_2To1_LINE,
    0,
};

ISP_PUB_ATTR_S ISP_PUB_ATTR_OS05A_MIPI_5M_30FPS =
{
    {0, 0, 2688, 1944},
    {2688, 1944},
    30,
    BAYER_BGGR,
    WDR_MODE_NONE,
    0,
};

ISP_PUB_ATTR_S ISP_PUB_ATTR_OS05A_MIPI_4M_30FPS =
{
    {0, 0, 2688, 1536},
    {2688, 1944},
    30,
    BAYER_BGGR,
    WDR_MODE_NONE,
    0,
};

ISP_PUB_ATTR_S ISP_PUB_ATTR_OS05A_MIPI_4M_30FPS_WDR2TO1 =
{
    {0, 0, 2688, 1536},
    {2688, 1944},
    30,
    BAYER_BGGR,
    WDR_MODE_2To1_LINE,
    0,
};

HI_S32 SAMPLE_COMM_ISP_GetIspAttrBySns(SAMPLE_SNS_TYPE_E enSnsType, ISP_PUB_ATTR_S *pstPubAttr)
{
    switch (enSnsType)
    {
        case SMART_SC4236_MIPI_3M_30FPS_10BIT:
            memcpy(pstPubAttr, &ISP_PUB_ATTR_SC4236_MIPI_3M_30FPS, sizeof(ISP_PUB_ATTR_S));
            break;
        case SMART_SC4236_MIPI_3M_20FPS_10BIT:
            memcpy(pstPubAttr, &ISP_PUB_ATTR_SC4236_MIPI_3M_20FPS, sizeof(ISP_PUB_ATTR_S));
            break;

        case SMART_SC2231_MIPI_2M_30FPS_10BIT:
            memcpy(pstPubAttr, &ISP_PUB_ATTR_SC2231_MIPI_2M_30FPS, sizeof(ISP_PUB_ATTR_S));
            break;

        case SMART_SC2335_MIPI_2M_30FPS_10BIT:
            memcpy(pstPubAttr, &ISP_PUB_ATTR_SC2231_MIPI_2M_30FPS, sizeof(ISP_PUB_ATTR_S));
            break;

        case SMART_SC3235_MIPI_3M_30FPS_10BIT:
            memcpy(pstPubAttr, &ISP_PUB_ATTR_SC3235_MIPI_3M_30FPS, sizeof(ISP_PUB_ATTR_S));
            break;

        case SMART_SC2235_DC_2M_30FPS_10BIT:
            memcpy(pstPubAttr, &ISP_PUB_ATTR_SC2235_MIPI_2M_30FPS, sizeof(ISP_PUB_ATTR_S));
            break;

        case SMART_AR0130_DC_1M_30FPS_12BIT:
            memcpy(pstPubAttr, &ISP_PUB_ATTR_AR0130_MIPI_1M_30FPS, sizeof(ISP_PUB_ATTR_S));
            break;

        case GALAXYCORE_GC2053_MIPI_2M_30FPS_10BIT:
            memcpy(pstPubAttr, &ISP_PUB_ATTR_GC2053_MIPI_2M_30FPS, sizeof(ISP_PUB_ATTR_S));
            break;

        case SONY_IMX327_MIPI_2M_30FPS_12BIT:
        case SONY_IMX327_2L_MIPI_2M_30FPS_12BIT:
            memcpy(pstPubAttr, &ISP_PUB_ATTR_IMX327_2M_30FPS, sizeof(ISP_PUB_ATTR_S));
            break;

        case SONY_IMX327_MIPI_2M_30FPS_12BIT_WDR2TO1:
        case SONY_IMX327_2L_MIPI_2M_30FPS_12BIT_WDR2TO1:
            memcpy(pstPubAttr, &ISP_PUB_ATTR_IMX327_MIPI_2M_30FPS_WDR2TO1_LINE, sizeof(ISP_PUB_ATTR_S));
            break;

        case SONY_IMX307_MIPI_2M_30FPS_12BIT:
            memcpy(pstPubAttr, &ISP_PUB_ATTR_IMX307_MIPI_2M_30FPS, sizeof(ISP_PUB_ATTR_S));
            break;

        case SONY_IMX307_MIPI_2M_30FPS_12BIT_WDR2TO1:
            memcpy(pstPubAttr, &ISP_PUB_ATTR_IMX307_MIPI_2M_30FPS_WDR2TO1_LINE, sizeof(ISP_PUB_ATTR_S));
            break;

        case SONY_IMX335_MIPI_5M_30FPS_12BIT:
            memcpy(pstPubAttr, &ISP_PUB_ATTR_IMX335_MIPI_5M_30FPS, sizeof(ISP_PUB_ATTR_S));
            break;

        case SONY_IMX335_MIPI_4M_30FPS_12BIT:
            memcpy(pstPubAttr, &ISP_PUB_ATTR_IMX335_MIPI_4M_30FPS, sizeof(ISP_PUB_ATTR_S));
            break;

        case SONY_IMX335_MIPI_5M_30FPS_10BIT_WDR2TO1:
            memcpy(pstPubAttr, &ISP_PUB_ATTR_IMX335_MIPI_5M_30FPS_WDR2TO1, sizeof(ISP_PUB_ATTR_S));
            break;

        case SONY_IMX335_MIPI_4M_30FPS_10BIT_WDR2TO1:
            memcpy(pstPubAttr, &ISP_PUB_ATTR_IMX335_MIPI_4M_30FPS_WDR2TO1, sizeof(ISP_PUB_ATTR_S));
            break;

        case OMNIVISION_OS05A_MIPI_5M_30FPS_12BIT:
            memcpy(pstPubAttr, &ISP_PUB_ATTR_OS05A_MIPI_5M_30FPS, sizeof(ISP_PUB_ATTR_S));
            break;
        case OMNIVISION_OS05A_MIPI_4M_30FPS_12BIT:
            memcpy(pstPubAttr, &ISP_PUB_ATTR_OS05A_MIPI_4M_30FPS, sizeof(ISP_PUB_ATTR_S));
            break;

        case OMNIVISION_OS05A_MIPI_4M_30FPS_10BIT_WDR2TO1:
            memcpy(pstPubAttr, &ISP_PUB_ATTR_OS05A_MIPI_4M_30FPS_WDR2TO1, sizeof(ISP_PUB_ATTR_S));
            break;
        default:
            memcpy(pstPubAttr, &ISP_PUB_ATTR_IMX327_2M_30FPS, sizeof(ISP_PUB_ATTR_S));
            break;
    }

    return HI_SUCCESS;
}
#if 0 //maohw
ISP_SNS_OBJ_S *SAMPLE_COMM_ISP_GetSnsObj(HI_U32 u32SnsId)
{
    SAMPLE_SNS_TYPE_E enSnsType;

    enSnsType = g_enSnsType[u32SnsId];

    switch (enSnsType)
    {
        case SMART_SC4236_MIPI_3M_30FPS_10BIT:
        case SMART_SC4236_MIPI_3M_20FPS_10BIT:
            return &stSnsSc4236Obj;

        case SMART_SC2231_MIPI_2M_30FPS_10BIT:
            return &stSnsSc2231Obj;

        case SMART_SC2335_MIPI_2M_30FPS_10BIT:
            return  &stSnsSc2335Obj;

        case SMART_SC3235_MIPI_3M_30FPS_10BIT:
            return &stSnsSc3235Obj;

        case SMART_AR0130_DC_1M_30FPS_12BIT:
            return &stSnsAr0130Obj;
        
        case SMART_AR_DC_2M_30FPS_10BIT:
            return &stSnsSc2235Obj;


        case GALAXYCORE_GC2053_MIPI_2M_30FPS_10BIT:
            return &stSnsGc2053Obj;

        case GALAXYCORE_GC2053_MIPI_2M_30FPS_10BIT_FORCAR:
            return &stSnsGc2053ForCarObj;

        case SONY_IMX327_MIPI_2M_30FPS_12BIT:
        case SONY_IMX327_MIPI_2M_30FPS_12BIT_WDR2TO1:
            return &stSnsImx327Obj;

        case SONY_IMX327_2L_MIPI_2M_30FPS_12BIT:
        case SONY_IMX327_2L_MIPI_2M_30FPS_12BIT_WDR2TO1:
            return &stSnsImx327_2l_Obj;

        case SONY_IMX307_MIPI_2M_30FPS_12BIT:
        case SONY_IMX307_MIPI_2M_30FPS_12BIT_WDR2TO1:
            return &stSnsImx307Obj;

        case SONY_IMX307_2L_MIPI_2M_30FPS_12BIT:
        case SONY_IMX307_2L_MIPI_2M_30FPS_12BIT_WDR2TO1:
            return &stSnsImx307_2l_Obj;

        case SONY_IMX335_MIPI_5M_30FPS_12BIT:
        case SONY_IMX335_MIPI_5M_30FPS_10BIT_WDR2TO1:
        case SONY_IMX335_MIPI_4M_30FPS_12BIT:
        case SONY_IMX335_MIPI_4M_30FPS_10BIT_WDR2TO1:
            return &stSnsImx335Obj;

        case OMNIVISION_OS05A_MIPI_5M_30FPS_12BIT:
        case OMNIVISION_OS05A_MIPI_4M_30FPS_12BIT:
        case OMNIVISION_OS05A_MIPI_4M_30FPS_10BIT_WDR2TO1:
            return &stSnsOs05aObj;

        default:
            return HI_NULL;
    }
}
#endif
static void *SAMPLE_COMM_ISP_Thread(void *param)
{
    HI_S32 s32Ret;
    ISP_DEV IspDev;
    HI_CHAR szThreadName[20];

    IspDev = (ISP_DEV)param;

    snprintf(szThreadName, 20, "ISP%d_RUN", IspDev);
    prctl(PR_SET_NAME, szThreadName, 0, 0, 0);

    SAMPLE_PRT("ISP Dev %d running !\n", IspDev);
    s32Ret = HI_MPI_ISP_Run(IspDev);

    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("HI_MPI_ISP_Run failed with %#x!\n", s32Ret);
    }

    return NULL;
}

/******************************************************************************
* funciton : ISP init
******************************************************************************/

HI_S32 SAMPLE_COMM_ISP_Aelib_Callback(ISP_DEV IspDev)
{
    ALG_LIB_S stAeLib;

    stAeLib.s32Id = IspDev;
    strncpy(stAeLib.acLibName, HI_AE_LIB_NAME, sizeof(HI_AE_LIB_NAME));
    CHECK_RET(HI_MPI_AE_Register(IspDev, &stAeLib), "aelib register call back");
    return HI_SUCCESS;
}

HI_S32 SAMPLE_COMM_ISP_Aelib_UnCallback(ISP_DEV IspDev)
{
    ALG_LIB_S stAeLib;

    stAeLib.s32Id = IspDev;
    strncpy(stAeLib.acLibName, HI_AE_LIB_NAME, sizeof(HI_AE_LIB_NAME));
    CHECK_RET(HI_MPI_AE_UnRegister(IspDev, &stAeLib), "aelib unregister call back");
    return HI_SUCCESS;
}

HI_S32 SAMPLE_COMM_ISP_Awblib_Callback(ISP_DEV IspDev)
{
    ALG_LIB_S stAwbLib;

    stAwbLib.s32Id = IspDev;
    strncpy(stAwbLib.acLibName, HI_AWB_LIB_NAME, sizeof(HI_AWB_LIB_NAME));
    CHECK_RET(HI_MPI_AWB_Register(IspDev, &stAwbLib), "awblib register call back");
    return HI_SUCCESS;
}

HI_S32 SAMPLE_COMM_ISP_Awblib_UnCallback(ISP_DEV IspDev)
{
    ALG_LIB_S stAwbLib;

    stAwbLib.s32Id = IspDev;
    strncpy(stAwbLib.acLibName, HI_AWB_LIB_NAME, sizeof(HI_AWB_LIB_NAME));
    CHECK_RET(HI_MPI_AWB_UnRegister(IspDev, &stAwbLib), "awblib unregister call back");
    return HI_SUCCESS;
}

/******************************************************************************
* funciton : ISP Run
******************************************************************************/
HI_S32 SAMPLE_COMM_ISP_Run(ISP_DEV IspDev)
{
    HI_S32 s32Ret = 0;
    pthread_attr_t *pstAttr = NULL;

    s32Ret = pthread_create(&g_IspPid[IspDev], pstAttr, SAMPLE_COMM_ISP_Thread, (HI_VOID *)IspDev);

    if (0 != s32Ret)
    {
        SAMPLE_PRT("create isp running thread failed!, error: %d, %s\r\n", s32Ret, strerror(s32Ret));
        goto out;
    }

out:

    if (NULL != pstAttr)
    {
        pthread_attr_destroy(pstAttr);
    }

    return s32Ret;
}

HI_S32 SAMPLE_COMM_ISP_Sensor_Regiter_callback(ISP_DEV IspDev, HI_U32 u32SnsId)
{
    ALG_LIB_S stAeLib;
    ALG_LIB_S stAwbLib;
    const ISP_SNS_OBJ_S *pstSnsObj;
    HI_S32    s32Ret = -1;

    if (MAX_SENSOR_NUM <= u32SnsId)
    {
        SAMPLE_PRT("invalid sensor id: %d\n", u32SnsId);
        return HI_FAILURE;
    }

    if (g_enSnsType[u32SnsId] == BT1120_2M_30FPS_8BIT ||
        g_enSnsType[u32SnsId] == BT656_2M_30FPS_8BIT ||
        g_enSnsType[u32SnsId] == BT601_2M_30FPS_8BIT)    {
        return HI_SUCCESS;
    }

    pstSnsObj = SAMPLE_COMM_ISP_GetSnsObj(u32SnsId);

    if (HI_NULL == pstSnsObj)
    {
        SAMPLE_PRT("sensor %d not exist!\n", u32SnsId);
        return HI_FAILURE;
    }

    stAeLib.s32Id = IspDev;
    stAwbLib.s32Id = IspDev;
    strncpy(stAeLib.acLibName, HI_AE_LIB_NAME, sizeof(HI_AE_LIB_NAME));
    strncpy(stAwbLib.acLibName, HI_AWB_LIB_NAME, sizeof(HI_AWB_LIB_NAME));
    //  strncpy(stAfLib.acLibName, HI_AF_LIB_NAME, sizeof(HI_AF_LIB_NAME));

    if (pstSnsObj->pfnRegisterCallback != HI_NULL)
    {
        s32Ret = pstSnsObj->pfnRegisterCallback(IspDev, &stAeLib, &stAwbLib);

        if (s32Ret != HI_SUCCESS)
        {
            SAMPLE_PRT("sensor_register_callback failed with %#x!\n", s32Ret);
            return s32Ret;
        }
    }
    else
    {
        SAMPLE_PRT("sensor_register_callback failed with HI_NULL!\n");
    }

    g_au32IspSnsId[IspDev] = u32SnsId;

    return HI_SUCCESS;
}

HI_S32 SAMPLE_COMM_ISP_Sensor_UnRegiter_callback(ISP_DEV IspDev)
{
    ALG_LIB_S stAeLib;
    ALG_LIB_S stAwbLib;
    HI_U32 u32SnsId;
    const ISP_SNS_OBJ_S *pstSnsObj;
    HI_S32    s32Ret = -1;

    u32SnsId = g_au32IspSnsId[IspDev];

    if (MAX_SENSOR_NUM <= u32SnsId)
    {
        SAMPLE_PRT("%s: invalid sensor id: %d\n", __FUNCTION__, u32SnsId);
        return HI_FAILURE;
    }

    pstSnsObj = SAMPLE_COMM_ISP_GetSnsObj(u32SnsId);

    if (HI_NULL == pstSnsObj)
    {
        return HI_FAILURE;
    }

    stAeLib.s32Id = IspDev;
    stAwbLib.s32Id = IspDev;
    strncpy(stAeLib.acLibName, HI_AE_LIB_NAME, sizeof(HI_AE_LIB_NAME));
    strncpy(stAwbLib.acLibName, HI_AWB_LIB_NAME, sizeof(HI_AWB_LIB_NAME));
    //   strncpy(stAfLib.acLibName, HI_AF_LIB_NAME, sizeof(HI_AF_LIB_NAME));

    if (pstSnsObj->pfnUnRegisterCallback != HI_NULL)
    {
        s32Ret = pstSnsObj->pfnUnRegisterCallback(IspDev, &stAeLib, &stAwbLib);

        if (s32Ret != HI_SUCCESS)
        {
            SAMPLE_PRT("sensor_unregister_callback failed with %#x!\n", s32Ret);
            return s32Ret;
        }
    }
    else
    {
        SAMPLE_PRT("sensor_unregister_callback failed with HI_NULL!\n");
    }

    return HI_SUCCESS;
}

/******************************************************************************
* funciton : stop ISP, and stop isp thread
******************************************************************************/
HI_VOID SAMPLE_COMM_ISP_Stop(ISP_DEV IspDev)
{
    if (g_IspPid[IspDev])
    {
        HI_MPI_ISP_Exit(IspDev);
        pthread_join(g_IspPid[IspDev], NULL);
        SAMPLE_COMM_ISP_Awblib_UnCallback(IspDev);
        SAMPLE_COMM_ISP_Aelib_UnCallback(IspDev);
        SAMPLE_COMM_ISP_Sensor_UnRegiter_callback(IspDev);
        g_IspPid[IspDev] = 0;
    }

    return;
}

HI_VOID SAMPLE_COMM_All_ISP_Stop(HI_VOID)
{
    ISP_DEV IspDev;

    for (IspDev = 0; IspDev < ISP_MAX_PIPE_NUM; IspDev++)
    {
        SAMPLE_COMM_ISP_Stop(IspDev);
    }
}

static ISP_SNS_TYPE_E SAMPLE_COMM_GetSnsBusType(SAMPLE_SNS_TYPE_E enSnsType)
{
    ISP_SNS_TYPE_E enBusType;

    switch (enSnsType)
    {
        default:
            enBusType = ISP_SNS_I2C_TYPE;
            break;
    }

    return enBusType;
}

HI_S32 SAMPLE_COMM_ISP_BindSns(ISP_DEV IspDev, HI_U32 u32SnsId, SAMPLE_SNS_TYPE_E enSnsType, HI_S8 s8SnsDev)
{
    ISP_SNS_COMMBUS_U uSnsBusInfo;
    ISP_SNS_TYPE_E enBusType;
    const ISP_SNS_OBJ_S *pstSnsObj;
    HI_S32 s32Ret;

    if (MAX_SENSOR_NUM <= u32SnsId)
    {
        SAMPLE_PRT("invalid sensor id: %d\n", u32SnsId);
        return HI_FAILURE;
    }

    if (g_enSnsType[u32SnsId] == BT1120_2M_30FPS_8BIT ||
        g_enSnsType[u32SnsId] == BT656_2M_30FPS_8BIT ||
        g_enSnsType[u32SnsId] == BT601_2M_30FPS_8BIT )
    {
        return HI_SUCCESS;
    }

    pstSnsObj = SAMPLE_COMM_ISP_GetSnsObj(u32SnsId);

    if (HI_NULL == pstSnsObj)
    {
        SAMPLE_PRT("sensor %d not exist!\n", u32SnsId);
        return HI_FAILURE;
    }

    enBusType = SAMPLE_COMM_GetSnsBusType(enSnsType);

    if (ISP_SNS_I2C_TYPE == enBusType)
    {
        uSnsBusInfo.s8I2cDev = s8SnsDev;
    }
    else
    {
        uSnsBusInfo.s8SspDev.bit4SspDev = s8SnsDev;
        uSnsBusInfo.s8SspDev.bit4SspCs  = 0;
    }

    if (HI_NULL != pstSnsObj->pfnSetBusInfo)
    {
        s32Ret = pstSnsObj->pfnSetBusInfo(IspDev, uSnsBusInfo);

        if (s32Ret != HI_SUCCESS)
        {
            SAMPLE_PRT("set sensor bus info failed with %#x!\n", s32Ret);
            return s32Ret;
        }
    }
    else
    {
        SAMPLE_PRT("not support set sensor bus info!\n");
        return HI_FAILURE;
    }

    return s32Ret;
}


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */
