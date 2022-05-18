/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2019-2020. All rights reserved.
 * Description:
 * Author:
 * Create: 2019-05-19
 */

#ifndef __HI_PLUGIN_TYPE_H__
#define __HI_PLUGIN_TYPE_H__

#include "hi_type.h"

#if defined(_MSC_VER)
#define EXPORT __declspec(dllexport)
#elif defined(__GNUC__)
#define EXPORT __attribute__((visibility("default")))
#else
#define EXPORT
#pragma warning cant export dynamic symbos
#endif

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

enum {
    MAX_OPERAND_NAME_LEN = 64,
};

typedef struct {
    hi_s32 height;
    hi_s32 width;
    hi_s32 chn;
} HiNodePluginShape;

typedef enum {
    ELEM_TYPE_U8,
    ELEM_TYPE_U16,
    ELEM_TYPE_U32
} HiNodePluginElemType;

typedef struct  {
    hi_u64 offset;  // addr
    hi_char name[MAX_OPERAND_NAME_LEN + 1];
    HiNodePluginElemType elemType;
    hi_u32 num;
    hi_u32 stride;
    HiNodePluginShape shape;
} HiNodePluginOperand;

typedef struct {
    hi_u32 pluginOperandNum;
    HiNodePluginOperand* pluginOperand;
} HiNodePluginOperandArray;

typedef struct {
    hi_void* param;
    hi_u32 size;
} HiNodePluginParamItem;

typedef struct {
    HiNodePluginParamItem* hyperParam;
    HiNodePluginParamItem* trainingParam;
} HiNodePluginParam;

EXPORT hi_s32 HiNodePluginCompute(const HiNodePluginOperandArray* inputs, HiNodePluginOperandArray* outputs,
    HiNodePluginParam* param);

EXPORT hi_s32 HiNodePluginGetNodeType(hi_char nodeType[]);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif
