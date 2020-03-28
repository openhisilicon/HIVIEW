/**
* @file    hi_appcomm_log.h
* @brief   product log header file.
*
* Copyright (c) 2017 Huawei Tech.Co.,Ltd
*
* @author    HiMobileCam Reference Develop Team
* @date      2017/12/13
* @version   1.0

*/
#ifndef __HI_SCENECOMM_LOG_H__
#define __HI_SCENECOMM_LOG_H__

#include "hi_scenecomm.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

/** \addtogroup     APPCOMM */
/** @{ */  /** <!-- [APPCOMM] */

/** color log macro define */
#define NONE          "\033[m"
#define RED           "\033[0;32;31m"
#define GREEN         "\033[0;32;32m"
#define BLUE          "\033[0;32;34m"
#define YELLOW        "\033[1;33m"

/** product log level define */
#define HI_LOG_LEVEL_FATAL      (0)
#define HI_LOG_LEVEL_ERROR      (1)
#define HI_LOG_LEVEL_WARNING    (2)
#define HI_LOG_LEVEL_DEBUG      (3)

/** product log level */
#ifdef CFG_DEBUG_LOG_ON
#define HI_LOG_LEVEL  HI_LOG_LEVEL_DEBUG
#else
#define HI_LOG_LEVEL  HI_LOG_LEVEL_ERROR
#endif

#ifdef __HuaweiLite__
#define HI_LOG(level, module, fmt, args...)  \
    do {\
        extern HI_BOOL g_bLogOn ;\
        extern HI_U32 g_uart_fputc_en;\
        if ((level <= HI_LOG_LEVEL_ERROR)&&(g_bLogOn)) {\
            HI_U32 nTemp = g_uart_fputc_en;  \
            g_uart_fputc_en = 1; \
            dprintf(module "(%s-%d:%d): " fmt, __FUNCTION__, __LINE__, level, ##args);\
            g_uart_fputc_en = nTemp;  \
        }\
        else if ((level <= HI_LOG_LEVEL)&&(g_bLogOn)) {\
            printf(module "(%s-%d:%d): " fmt, __FUNCTION__, __LINE__, level, ##args);\
        }\
    } while(0)
#else
/** general product log macro define */
#define HI_LOG(level, module, fmt, args...)  \
    do {\
        extern HI_BOOL g_bLogOn ;\
        if ((level <= HI_LOG_LEVEL)&&(g_bLogOn)) {\
            fprintf(stdout, module "(%s-%d:%d): " fmt, __FUNCTION__, __LINE__, level, ##args);\
        }\
    } while(0)
#endif

/** product module name, canbe redefined in module */
#ifdef HI_MODULE
#undef HI_MODULE
#endif
#define HI_MODULE ""

#ifdef CFG_DEBUG_LOG_ON
/** unified log macro define */
#define MLOGE(fmt, args...)  HI_LOG(HI_LOG_LEVEL_ERROR, HI_MODULE, fmt, ##args)
#define MLOGW(fmt, args...)  HI_LOG(HI_LOG_LEVEL_WARNING, HI_MODULE, fmt, ##args)
#define MLOGD(fmt, args...)  HI_LOG(HI_LOG_LEVEL_DEBUG, HI_MODULE, fmt, ##args)
#else
#define MLOGE(fmt, args...)  HI_LOG(HI_LOG_LEVEL_ERROR, HI_MODULE, fmt, ##args)
#define MLOGW(fmt, args...)
#define MLOGD(fmt, args...)
#endif


/** @}*/  /** <!-- ==== SCENECOMM End ====*/

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* End of #ifndef __HI_SCENECOMM_LOG_H__ */

