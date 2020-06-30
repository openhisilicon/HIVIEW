/*
* Copyright (c) Hisilicon Technologies Co., Ltd. 2011-2019. All rights reserved.
* Description:
* Author: Hisilicon multimedia software group
* Create: 2011/06/28
 */

#ifndef __HI_BUFFER_H__
#define __HI_BUFFER_H__

#include "hi_math.h"
#include "hi_type.h"
#include "hi_common.h"
#include "hi_comm_video.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

#define HI_MAXINUM_LIMIT 100000

__inline static HI_VOID COMMON_GetPicBufferConfig(HI_U32 u32Width, HI_U32 u32Height,
    PIXEL_FORMAT_E enPixelFormat, DATA_BITWIDTH_E enBitWidth,
    COMPRESS_MODE_E enCmpMode, HI_U32 u32Align, VB_CAL_CONFIG_S *pstCalConfig)
{
    HI_U32 u32TailInBytes = 0;
    HI_U32 u32BitWidth = 0;
    HI_U32 u32HeadStride = 0;
    HI_U32 u32VBSize = 0;
    HI_U32 u32HeadSize = 0;
    HI_U32 u32AlignHeight;
    HI_U32 u32MainStride = 0;
    HI_U32 u32MainSize = 0;
    HI_U32 u32ExtStride = 0;
    HI_U32 u32ExtSize = 0;
    HI_U32 u32ExtYSize = 0;
    HI_U32 u32HeadYSize = 0;
    HI_U32 u32YSize = 0;

    if ((u32Width > HI_MAXINUM_LIMIT) || (u32Height > HI_MAXINUM_LIMIT)) {
        pstCalConfig->u32VBSize = 0;
    }

    /* u32Align: 0 is automatic mode, alignment size following system. Non-0 for specified alignment size */
    if (u32Align == 0) {
        u32Align = DEFAULT_ALIGN;
    } else if (u32Align > MAX_ALIGN) {
        u32Align = MAX_ALIGN;
    } else {
        u32Align = (ALIGN_UP(u32Align, DEFAULT_ALIGN));
    }

    switch (enBitWidth) {
        case DATA_BITWIDTH_8: {
            u32BitWidth = 8;
            break;
        }
        case DATA_BITWIDTH_10: {
            u32BitWidth = 10;
            break;
        }
        case DATA_BITWIDTH_12: {
            u32BitWidth = 12;
            break;
        }
        case DATA_BITWIDTH_14: {
            u32BitWidth = 14;
            break;
        }
        case DATA_BITWIDTH_16: {
            u32BitWidth = 16;
            break;
        }
        default:
        {
            u32BitWidth = 0;
            break;
        }
    }

    u32AlignHeight = ALIGN_UP(u32Height, 2);

    if (enCmpMode == COMPRESS_MODE_NONE) {
        u32MainStride = ALIGN_UP((u32Width * u32BitWidth + 7) >> 3, u32Align);
        u32YSize = u32MainStride * u32AlignHeight;

        if ((PIXEL_FORMAT_YVU_SEMIPLANAR_420 == enPixelFormat) || (PIXEL_FORMAT_YUV_SEMIPLANAR_420 == enPixelFormat)) {
            u32MainSize = (u32MainStride * u32AlignHeight * 3) >> 1;
        } else if (PIXEL_FORMAT_YVU_SEMIPLANAR_422 == enPixelFormat ||
                   PIXEL_FORMAT_YUV_SEMIPLANAR_422 == enPixelFormat) {
            u32MainSize = u32MainStride * u32AlignHeight * 2;
        } else if ((enPixelFormat == PIXEL_FORMAT_YUV_400) || (PIXEL_FORMAT_S16C1 == enPixelFormat)) {
            u32MainSize = u32MainStride * u32AlignHeight;
        } else {
            u32MainSize = u32MainStride * u32AlignHeight * 3;
        }

        u32VBSize = u32MainSize;
    } else {
        if (u32Width <= 4096) {
            u32HeadStride = 16;
        } else if (u32Width <= 8192) {
            u32HeadStride = 32;
        } else {
            u32HeadStride = 64;
        }

        if (u32BitWidth == 8) {
            u32MainStride = ALIGN_UP(u32Width, u32Align);
            u32HeadYSize = u32HeadStride * u32AlignHeight;
            u32YSize = u32MainStride * u32AlignHeight;

            if ((PIXEL_FORMAT_YVU_SEMIPLANAR_420 == enPixelFormat) ||
                (PIXEL_FORMAT_YUV_SEMIPLANAR_420 == enPixelFormat)) {
                u32HeadSize = (u32HeadStride * u32AlignHeight * 3) >> 1;
                u32MainSize = (u32MainStride * u32AlignHeight * 3) >> 1;
            } else if (PIXEL_FORMAT_YVU_SEMIPLANAR_422 == enPixelFormat ||
                       PIXEL_FORMAT_YUV_SEMIPLANAR_422 == enPixelFormat) {
                u32HeadSize = u32HeadStride * u32AlignHeight * 2;
                u32MainSize = u32MainStride * u32AlignHeight * 2;
            } else if (enPixelFormat == PIXEL_FORMAT_YUV_400) {
                u32HeadSize = u32HeadStride * u32AlignHeight;
                u32MainSize = u32MainStride * u32AlignHeight;
            } else {
                u32HeadSize = u32HeadStride * u32AlignHeight * 3;
                u32MainSize = u32MainStride * u32AlignHeight * 3;
            }
        } else if (u32BitWidth == 10) {
            u32TailInBytes = DIV_UP(u32Width % SEG_CMP_LENGTH * u32BitWidth, 8);
            u32MainStride = ALIGN_DOWN(u32Width,
                                       SEG_CMP_LENGTH) +
                            ((u32TailInBytes > SEG_CMP_LENGTH) ? SEG_CMP_LENGTH : u32TailInBytes);
            u32MainStride = ALIGN_UP(u32MainStride, u32Align);
            u32ExtStride = (u32TailInBytes > SEG_CMP_LENGTH) ? (ALIGN_UP(DIV_UP(u32Width, 4), u32Align)) :
                ALIGN_UP((ALIGN_DOWN(u32Width, SEG_CMP_LENGTH) / 4), u32Align);

            u32HeadYSize = u32HeadStride * u32AlignHeight;
            u32YSize = u32MainStride * u32AlignHeight;
            u32ExtYSize = u32ExtStride * u32AlignHeight;

            if ((PIXEL_FORMAT_YVU_SEMIPLANAR_420 == enPixelFormat) ||
                (PIXEL_FORMAT_YUV_SEMIPLANAR_420 == enPixelFormat)) {
                u32HeadSize = (u32HeadStride * u32AlignHeight * 3) >> 1;
                u32MainSize = (u32MainStride * u32AlignHeight * 3) >> 1;
                u32ExtSize = (u32ExtStride * u32AlignHeight * 3) >> 1;
            } else if (PIXEL_FORMAT_YVU_SEMIPLANAR_422 == enPixelFormat ||
                       PIXEL_FORMAT_YUV_SEMIPLANAR_422 == enPixelFormat) {
                u32HeadSize = u32HeadStride * u32AlignHeight * 2;
                u32MainSize = u32MainStride * u32AlignHeight * 2;
                u32ExtSize = u32ExtStride * u32AlignHeight * 2;
            } else if (enPixelFormat == PIXEL_FORMAT_YUV_400) {
                u32HeadSize = u32HeadStride * u32AlignHeight;
                u32MainSize = u32MainStride * u32AlignHeight;
                u32ExtSize = u32ExtStride * u32AlignHeight;
            } else {
                u32HeadSize = u32HeadStride * u32AlignHeight * 3;
                u32MainSize = u32MainStride * u32AlignHeight * 3;
                u32ExtSize = u32ExtStride * u32AlignHeight * 3;
            }

        } else {
            u32VBSize = 0;
            u32HeadYSize = 0;
            u32HeadSize = 0;
            u32HeadStride = 0;
            u32MainStride = 0;
            u32YSize = 0;
            u32MainSize = 0;
            u32ExtStride = 0;
            u32ExtYSize = 0;
        }

        u32HeadSize = ALIGN_UP(u32HeadSize, u32Align);

        u32VBSize = u32HeadSize + u32MainSize + u32ExtSize;
    }

    pstCalConfig->u32VBSize = u32VBSize;
    pstCalConfig->u32HeadYSize = u32HeadYSize;
    pstCalConfig->u32HeadSize = u32HeadSize;
    pstCalConfig->u32HeadStride = u32HeadStride;
    pstCalConfig->u32MainStride = u32MainStride;
    pstCalConfig->u32MainYSize = u32YSize;
    pstCalConfig->u32MainSize = u32MainSize;
    pstCalConfig->u32ExtStride = u32ExtStride;
    pstCalConfig->u32ExtYSize = u32ExtYSize;

    return;
}

__inline static HI_U32 COMMON_GetPicBufferSize(HI_U32 u32Width, HI_U32 u32Height,
                                               PIXEL_FORMAT_E enPixelFormat,
                                               DATA_BITWIDTH_E enBitWidth, COMPRESS_MODE_E enCmpMode, HI_U32 u32Align)
{
    VB_CAL_CONFIG_S stCalConfig;

    COMMON_GetPicBufferConfig(u32Width, u32Height, enPixelFormat, enBitWidth, enCmpMode, u32Align, &stCalConfig);

    return stCalConfig.u32VBSize;
}

__inline static HI_U32 VI_GetRawBufferSizeEx(HI_U32 u32Width, HI_U32 u32Height,
                                             PIXEL_FORMAT_E enPixelFormat,
                                             COMPRESS_MODE_E enCmpMode, HI_U32 u32CmpRatio, HI_U32 u32Align)
{
    HI_U32 u32BitWidth;
    HI_U32 u32Size = 0;
    HI_U32 u32Stride = 0;
    HI_U32 u32RawCmpRatio = 2000;

    if ((u32Width > HI_MAXINUM_LIMIT) || (u32Height > HI_MAXINUM_LIMIT)) {
        return 0;
    }

    if (enCmpMode == COMPRESS_MODE_LINE) {
        u32RawCmpRatio = 2000;

    } else if (enCmpMode == COMPRESS_MODE_FRAME) {
        if (u32CmpRatio == 0) {
            u32RawCmpRatio = 3000;
        } else {
            u32RawCmpRatio = u32CmpRatio;
        }
    }

    /* u32Align: 0 is automatic mode, alignment size following system. Non-0 for specified alignment size */
    if (u32Align == 0) {
        u32Align = DEFAULT_ALIGN;
    } else if (u32Align > MAX_ALIGN) {
        u32Align = MAX_ALIGN;
    } else {
        u32Align = (ALIGN_UP(u32Align, DEFAULT_ALIGN));
    }

    switch (enPixelFormat) {
        case PIXEL_FORMAT_RGB_BAYER_8BPP: {
            u32BitWidth = 8;
            break;
        }

        case PIXEL_FORMAT_RGB_BAYER_10BPP: {
            u32BitWidth = 10;
            break;
        }

        case PIXEL_FORMAT_RGB_BAYER_12BPP: {
            u32BitWidth = 12;
            break;
        }

        case PIXEL_FORMAT_RGB_BAYER_14BPP: {
            u32BitWidth = 14;
            break;
        }

        case PIXEL_FORMAT_RGB_BAYER_16BPP: {
            u32BitWidth = 16;
            break;
        }

        default:
        {
            u32BitWidth = 0;
            break;
        }
    }

    if (enCmpMode == COMPRESS_MODE_NONE) {
        u32Stride = ALIGN_UP(ALIGN_UP(u32Width * u32BitWidth, 8) / 8, u32Align);
        u32Size = u32Stride * u32Height;
    } else if (enCmpMode == COMPRESS_MODE_LINE) {
        HI_U32 u32Tmp;
        u32Tmp = ALIGN_UP((16 + u32Width * u32BitWidth * 1000ULL / u32RawCmpRatio + 8192 + 127) / 128, 2);
        u32Stride = ALIGN_UP(u32Tmp * 16, u32Align);
        u32Size = u32Stride * u32Height;
    } else if (enCmpMode == COMPRESS_MODE_FRAME) {
        u32Size = ALIGN_UP(u32Height * u32Width * u32BitWidth * 1000ULL / (u32RawCmpRatio * 8), u32Align);
    }

    return u32Size;
}

__inline static HI_U32 VI_GetRawBufferSize(HI_U32 u32Width, HI_U32 u32Height,
                                           PIXEL_FORMAT_E enPixelFormat, COMPRESS_MODE_E enCmpMode, HI_U32 u32Align)
{
    return VI_GetRawBufferSizeEx(u32Width, u32Height, enPixelFormat, enCmpMode, 0, u32Align);
}

__inline static HI_U32 AVS_GetPicBufferSize(HI_U32 u32Width, HI_U32 u32Height, COMPRESS_MODE_E enCmpMode,
                                            HI_U32 u32Align)
{
    if ((u32Width > HI_MAXINUM_LIMIT) || (u32Height > HI_MAXINUM_LIMIT)) {
        return 0;
    }

    return ALIGN_UP(u32Width, 128) * ALIGN_UP(u32Height, 64) * 2;
}

__inline static HI_U32 VDEC_GetPicBufferSize(PAYLOAD_TYPE_E enType, HI_U32 u32Width,
                                             HI_U32 u32Height,
                                             PIXEL_FORMAT_E enPixelFormat, DATA_BITWIDTH_E enBitWidth, HI_U32 u32Align)
{
    HI_U32 u32AlignWidth, u32AlignHeight;
    HI_U32 u32HeaderSize = 0;
    HI_U32 u32HeaderStride = 0;
    HI_U32 u32ExtraSize = 0;
    HI_U32 u32Size = 0;

    if ((u32Width > HI_MAXINUM_LIMIT) || (u32Height > HI_MAXINUM_LIMIT)) {
        return 0;
    }

    if (enType == PT_H264) {
        u32AlignWidth = ALIGN_UP(u32Width, H264D_ALIGN_W);
        u32AlignHeight = ALIGN_UP(u32Height, H264D_ALIGN_H);
        if (u32Width <= 8192) {
            u32HeaderStride = ALIGN_UP(u32AlignWidth, 2048) >> 6;
        } else {
            u32HeaderStride = ALIGN_UP(u32AlignWidth, 8192) >> 6;
        }
        u32HeaderSize = u32HeaderStride * (ALIGN_UP(u32AlignHeight, H264D_ALIGN_H) >> 2);
        u32Size = ((u32HeaderSize + u32AlignWidth * u32AlignHeight) * 3) >> 1;
    } else if (enType == PT_H265) {
        u32AlignWidth = ALIGN_UP(u32Width, H265D_ALIGN_W);
        u32AlignHeight = ALIGN_UP(u32Height, H265D_ALIGN_H);
        if (u32Width <= 8192) {
            u32HeaderStride = ALIGN_UP(u32AlignWidth, 2048) >> 6;
        } else {
            u32HeaderStride = ALIGN_UP(u32AlignWidth, 8192) >> 6;
        }
        u32HeaderSize = u32HeaderStride * (ALIGN_UP(u32AlignHeight, H265D_ALIGN_H) >> 2);

        if (enBitWidth == DATA_BITWIDTH_10) {
            u32ExtraSize = ALIGN_UP((u32AlignWidth * 2) >> 3, 16) * ALIGN_UP(u32AlignHeight, 32);
        }

        u32Size = ((u32HeaderSize + u32AlignWidth * u32AlignHeight + u32ExtraSize) * 3) >> 1;
    } else if ((enType == PT_JPEG) || (enType == PT_MJPEG)) {
        if ((enPixelFormat == PIXEL_FORMAT_ARGB_1555) || (enPixelFormat == PIXEL_FORMAT_ABGR_1555) ||
            (enPixelFormat == PIXEL_FORMAT_RGB_565) || (enPixelFormat == PIXEL_FORMAT_BGR_565)) {
            u32AlignWidth = ALIGN_UP(u32Width * 2, JPEGD_RGB_ALIGN);
            u32AlignHeight = ALIGN_UP(u32Height, JPEGD_RGB_ALIGN);
            u32Size = u32AlignWidth * u32AlignHeight;
        } else if ((enPixelFormat == PIXEL_FORMAT_RGB_888) || (enPixelFormat == PIXEL_FORMAT_BGR_888)) {
            u32AlignWidth = ALIGN_UP(u32Width * 3, JPEGD_RGB_ALIGN);
            u32AlignHeight = ALIGN_UP(u32Height, JPEGD_RGB_ALIGN);
            u32Size = u32AlignWidth * u32AlignHeight;
        } else if ((enPixelFormat == PIXEL_FORMAT_ARGB_8888) || (enPixelFormat == PIXEL_FORMAT_ABGR_8888)) {
            u32AlignWidth = ALIGN_UP(u32Width * 4, JPEGD_RGB_ALIGN);
            u32AlignHeight = ALIGN_UP(u32Height, JPEGD_RGB_ALIGN);
            u32Size = u32AlignWidth * u32AlignHeight;
        } else {
            /* for PIXEL_FORMAT_YVU_SEMIPLANAR_420 */
            u32AlignWidth = ALIGN_UP(u32Width, JPEGD_ALIGN_W);
            u32AlignHeight = ALIGN_UP(u32Height, JPEGD_ALIGN_H);
            u32Size = (u32AlignWidth * u32AlignHeight * 3) >> 1;
        }
    } else {
        u32Size = 0;
    }

    return u32Size;
}

__inline static HI_U32 VDEC_GetTmvBufferSize(PAYLOAD_TYPE_E enType, HI_U32 u32Width, HI_U32 u32Height)
{
    HI_U32 WidthInMb, HeightInMb;
    HI_U32 ColMbSize;
    HI_U32 u32Size = 0;

    if ((u32Width > HI_MAXINUM_LIMIT) || (u32Height > HI_MAXINUM_LIMIT)) {
        return 0;
    }

    if (enType == PT_H264) {
        WidthInMb = ALIGN_UP(u32Width, 16) >> 4;
        HeightInMb = ALIGN_UP(u32Height, 16) >> 4;
        ColMbSize = 16 * 4;
        u32Size = ALIGN_UP((ColMbSize * WidthInMb * HeightInMb), 128);
    } else if (enType == PT_H265) {
        WidthInMb = ALIGN_UP(u32Width, 64) >> 4;
        HeightInMb = ALIGN_UP(u32Height, 64) >> 4;
        ColMbSize = 4 * 4;
        u32Size = ALIGN_UP((ColMbSize * WidthInMb * HeightInMb), 128);
    } else {
        u32Size = 0;
    }

    return u32Size;
}

__inline static HI_U32 VENC_GetRefPicInfoBufferSize(PAYLOAD_TYPE_E enType, HI_U32 u32Width, HI_U32 u32Height,
                                                    HI_U32 u32Align)
{
    HI_U32 u32Size;
    HI_U32 u32AlignWidth, u32AlignHeight;
    HI_U32 u32TmvSize, u32PmeSize, u32PmeInfoSize;

    if ((u32Width > HI_MAXINUM_LIMIT) || (u32Height > HI_MAXINUM_LIMIT)) {
        return 0;
    }

    if (enType == PT_H265) {
        u32AlignWidth = ALIGN_UP(u32Width, 64) >> 6;
        u32AlignHeight = ALIGN_UP(u32Height, 64) >> 6;

        u32TmvSize = (u32AlignWidth * u32AlignHeight) << 7;
        u32PmeSize = (u32AlignWidth << 4) * (u32AlignHeight << 4);

        u32AlignWidth = ALIGN_UP(u32Width, 128) >> 7;
        u32AlignHeight = ALIGN_UP(u32Height, 256) >> 8;
        u32PmeInfoSize = (u32AlignWidth * u32AlignHeight) << 5;

        u32Size = u32TmvSize + u32PmeSize + u32PmeInfoSize;

    } else if (enType == PT_H264) {
        u32AlignWidth = ALIGN_UP(u32Width, 16) >> 4;
        u32AlignHeight = ALIGN_UP(u32Height, 16) >> 4;
        u32TmvSize = (u32AlignWidth * u32AlignHeight) << 5;

        u32AlignWidth = ALIGN_UP(u32Width, 64) >> 6;
        u32AlignHeight = ALIGN_UP(u32Height, 64) >> 6;
        u32PmeSize = (u32AlignWidth << 4) * (u32AlignHeight << 4);

        u32AlignWidth = ALIGN_UP(u32Width, 4096) >> 12;
        u32AlignHeight = ALIGN_UP(u32Height, 16) >> 4;
        u32PmeInfoSize = (u32AlignWidth * u32AlignHeight) << 5;

        u32Size = u32TmvSize + u32PmeSize + u32PmeInfoSize;
    } else {
        u32Size = 0;
    }
    return u32Size;
}

__inline static HI_U32 VENC_GetRefBufferSize(PAYLOAD_TYPE_E enType, HI_U32 u32Width, HI_U32 u32Height,
                                             DATA_BITWIDTH_E enBitWidth, HI_U32 u32Align)
{
    HI_U32 u32Size = 0;
    HI_U32 u32AlignWidth, u32AlignHeight, u32BitWidth;
    HI_U32 u32YHeaderSize, u32CHeaderSize, u32YSize, u32CSize;

    if ((u32Width > HI_MAXINUM_LIMIT) || (u32Height > HI_MAXINUM_LIMIT)) {
        return 0;
    }

    if (enBitWidth == DATA_BITWIDTH_8) {
        u32BitWidth = 8;
    } else if (enBitWidth == DATA_BITWIDTH_10) {
        u32BitWidth = 10;
    } else {
        return 0;
    }

    if (enType == PT_H265) {
        u32AlignWidth = ALIGN_UP(u32Width, 128);
        u32AlignHeight = ALIGN_UP(u32Height, 64) >> 6;

        u32YHeaderSize = u32AlignWidth / 64 * 32 * u32AlignHeight;
        u32CHeaderSize = u32YHeaderSize;

        u32AlignWidth = ALIGN_UP(u32Width, 64);
        u32AlignHeight = ALIGN_UP(u32Height, 16);
        u32YSize = (u32AlignWidth * u32AlignHeight * u32BitWidth) >> 3;
        u32CSize = u32YSize >> 1;

        u32Size = u32YHeaderSize + u32CHeaderSize + u32YSize + u32CSize;
    } else if (enType == PT_H264) {
        u32AlignWidth = ALIGN_UP(u32Width, 512);
        u32AlignHeight = ALIGN_UP(u32Height, 16) >> 4;
        u32YHeaderSize = ((u32AlignWidth >> 8) << 4) * u32AlignHeight;
        u32CHeaderSize = u32YHeaderSize;

        u32AlignWidth = ALIGN_UP(u32Width, 64);
        u32AlignHeight = ALIGN_UP(u32Height, 16);
        u32YSize = u32AlignWidth * u32AlignHeight;
        u32CSize = u32YSize >> 1;

        u32Size = u32YHeaderSize + u32CHeaderSize + u32YSize + u32CSize;
    } else {
        u32Size = 0;
    }

    return u32Size;
}

__inline static HI_U32 VENC_GetQpmapSizeStride(HI_U32 u32Width)
{
    if (u32Width > HI_MAXINUM_LIMIT) {
        return 0;
    }

    return DIV_UP(u32Width, 512) * 32;
}

__inline static HI_U32 VENC_GetQpmapSize(HI_U32 u32Width, HI_U32 u32Height)
{
    HI_U32 u32Stride, u32AlignHeight;

    if ((u32Width > HI_MAXINUM_LIMIT) || (u32Height > HI_MAXINUM_LIMIT)) {
        return 0;
    }

    u32Stride = VENC_GetQpmapSizeStride(u32Width);
    u32AlignHeight = DIV_UP(u32Height, 16);

    return u32Stride * u32AlignHeight;
}

__inline static HI_U32 VENC_GetSkipWeightSizeStride(PAYLOAD_TYPE_E enType, HI_U32 u32Width)
{
    HI_U32 u32Stride;

    if (u32Width > HI_MAXINUM_LIMIT) {
        return 0;
    }

    if (enType == PT_H265) {
        u32Stride = DIV_UP(u32Width, 2048) * 16;
    } else if (enType == PT_H264) {
        u32Stride = DIV_UP(u32Width, 512) * 16;
    } else {
        u32Stride = 0;
    }

    return u32Stride;
}
__inline static HI_U32 VENC_GetSkipWeightSize(PAYLOAD_TYPE_E enType, HI_U32 u32Width, HI_U32 u32Height)
{
    HI_U32 u32Stride, u32AlignHeight;

    if ((u32Width > HI_MAXINUM_LIMIT) || (u32Height > HI_MAXINUM_LIMIT)) {
        return 0;
    }

    u32Stride = VENC_GetSkipWeightSizeStride(enType, u32Width);

    if (enType == PT_H265) {
        u32AlignHeight = DIV_UP(u32Height, 64);
    } else if (enType == PT_H264) {
        u32AlignHeight = DIV_UP(u32Height, 16);
    } else {
        u32AlignHeight = 0;
    }

    return u32Stride * u32AlignHeight;
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* __HI_BUFFER_H__ */

