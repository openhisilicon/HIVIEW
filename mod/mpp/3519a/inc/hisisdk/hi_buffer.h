
/******************************************************************************
Copyright (C), 2016, Hisilicon Tech. Co., Ltd.
******************************************************************************
File Name     : hi_buffer.h
Version       : Initial Draft
Author        : Hisilicon multimedia software group
Created       : 2016/11/15
Last Modified :
Description   : The count defination of buffer size
Function List :
******************************************************************************/

#ifndef __HI_BUFFER_H__
#define __HI_BUFFER_H__

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

#include "hi_math.h"
#include "hi_type.h"
#include "hi_common.h"
#include "hi_comm_video.h"


__inline static HI_VOID COMMON_GetPicBufferConfig(HI_U32 u32Width, HI_U32 u32Height,PIXEL_FORMAT_E enPixelFormat,
            DATA_BITWIDTH_E enBitWidth, COMPRESS_MODE_E enCmpMode, HI_U32 u32Align, VB_CAL_CONFIG_S* pstCalConfig)
{
    HI_U32 u32BitWidth = 0;
    HI_U32 u32HeadStride  = 0;
    HI_U32 u32VBSize      = 0;
    HI_U32 u32HeadSize    = 0;
    HI_U32 u32AlignHeight = 0;
    HI_U32 u32MainStride  = 0;
    HI_U32 u32MainSize    = 0;
    HI_U32 u32ExtStride   = 0;
    HI_U32 u32ExtSize     = 0;
    HI_U32 u32ExtYSize    = 0;
    HI_U32 u32HeadYSize   = 0;
    HI_U32 u32YSize       = 0;

    /* u32Align: 0 is automatic mode, alignment size following system. Non-0 for specified alignment size */
    if(0 == u32Align)
    {
        u32Align = DEFAULT_ALIGN;
    }
    else if(u32Align > MAX_ALIGN)
    {
        u32Align = MAX_ALIGN;
    }
    else
    {
        u32Align = (ALIGN_UP(u32Align, DEFAULT_ALIGN));
    }

    switch (enBitWidth)
    {
        case DATA_BITWIDTH_8:
        {
            u32BitWidth = 8;
            break;
        }
        case DATA_BITWIDTH_16:
        {
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

    if (COMPRESS_MODE_NONE == enCmpMode)
    {
        u32MainStride = ALIGN_UP((u32Width * u32BitWidth + 7) >> 3, u32Align);
        u32YSize = u32MainStride * u32AlignHeight;

        if(PIXEL_FORMAT_YVU_SEMIPLANAR_420 == enPixelFormat || PIXEL_FORMAT_YUV_SEMIPLANAR_420 == enPixelFormat)
        {
            u32MainSize = (u32MainStride * u32AlignHeight)*3 >> 1;
        }
        else if (PIXEL_FORMAT_YVU_SEMIPLANAR_422 == enPixelFormat || PIXEL_FORMAT_YUV_SEMIPLANAR_422 == enPixelFormat)
        {
            u32MainSize = u32MainStride * u32AlignHeight * 2;
        }
        else if ((PIXEL_FORMAT_YUV_400 == enPixelFormat) || (PIXEL_FORMAT_S16C1 == enPixelFormat))
        {
            u32MainSize = u32MainStride * u32AlignHeight;
        }
        else
        {
            u32MainSize = u32MainStride * u32AlignHeight * 3;
        }

        u32VBSize   = u32MainSize;
    }
    else
    {
        if (u32Width <= 4096)
        {
            u32HeadStride = 16;
        }
        else if (u32Width <= 8192)
        {
            u32HeadStride = 32;
        }
        else
        {
            u32HeadStride = 64;
        }

        if (u32BitWidth == 8)
        {
            u32MainStride  = ALIGN_UP(u32Width, u32Align);
            u32HeadYSize   = u32HeadStride * u32AlignHeight;
            u32YSize       = u32MainStride * u32AlignHeight;

            if(PIXEL_FORMAT_YVU_SEMIPLANAR_420 == enPixelFormat || PIXEL_FORMAT_YUV_SEMIPLANAR_420 == enPixelFormat)
            {
                u32HeadSize = (u32HeadStride * u32AlignHeight * 3) >> 1;
                u32MainSize = (u32MainStride * u32AlignHeight * 3) >> 1;
            }
            else if (PIXEL_FORMAT_YVU_SEMIPLANAR_422 == enPixelFormat || PIXEL_FORMAT_YUV_SEMIPLANAR_422 == enPixelFormat)
            {
                u32HeadSize = u32HeadStride * u32AlignHeight * 2;
                u32MainSize = u32MainStride * u32AlignHeight * 2;
            }
            else if (PIXEL_FORMAT_YUV_400 == enPixelFormat)
            {
                u32HeadSize = u32HeadStride * u32AlignHeight;
                u32MainSize = u32MainStride * u32AlignHeight;
            }
            else
            {
                u32HeadSize = u32HeadStride * u32AlignHeight * 3;
                u32MainSize = u32MainStride * u32AlignHeight * 3;
            }
        }
        else
        {
            u32VBSize     = 0;
            u32HeadYSize  = 0;
            u32HeadSize   = 0;
            u32HeadStride = 0;
            u32MainStride = 0;
            u32YSize      = 0;
            u32MainSize   = 0;
            u32ExtStride  = 0;
            u32ExtYSize   = 0;
        }

        u32HeadSize = ALIGN_UP(u32HeadSize, u32Align);

        u32VBSize = u32HeadSize + u32MainSize + u32ExtSize;
    }

    pstCalConfig->u32VBSize     = u32VBSize;
    pstCalConfig->u32HeadYSize  = u32HeadYSize;
    pstCalConfig->u32HeadSize   = u32HeadSize;
    pstCalConfig->u32HeadStride = u32HeadStride;
    pstCalConfig->u32MainStride = u32MainStride;
    pstCalConfig->u32MainYSize  = u32YSize;
    pstCalConfig->u32MainSize   = u32MainSize;
    pstCalConfig->u32ExtStride  = u32ExtStride;
    pstCalConfig->u32ExtYSize   = u32ExtYSize;

    return;
}

__inline static HI_U32 COMMON_GetPicBufferSize(HI_U32 u32Width, HI_U32 u32Height,
        PIXEL_FORMAT_E enPixelFormat, DATA_BITWIDTH_E enBitWidth, COMPRESS_MODE_E enCmpMode, HI_U32 u32Align)
{
    VB_CAL_CONFIG_S stCalConfig;

    COMMON_GetPicBufferConfig(u32Width, u32Height,enPixelFormat,enBitWidth, enCmpMode, u32Align, &stCalConfig);

    return stCalConfig.u32VBSize;
}

__inline static HI_U32 VI_GetRawBufferSizeEx(HI_U32 u32Width, HI_U32 u32Height,
                PIXEL_FORMAT_E enPixelFormat, COMPRESS_MODE_E enCmpMode, HI_U32 u32CmpRatio, HI_U32 u32Align)
{
    HI_U32 u32BitWidth;
    HI_U32 u32Size = 0;
    HI_U32 u32Stride = 0;
    HI_U32 u32RawCmpRatio = 1600;

    if(COMPRESS_MODE_LINE == enCmpMode)
    {
        u32RawCmpRatio = 1600;

    }
    else if(COMPRESS_MODE_FRAME == enCmpMode)
    {
        if(0 == u32CmpRatio)
        {
            u32RawCmpRatio = 2000;
        }
        else
        {
            u32RawCmpRatio = u32CmpRatio;
        }
    }

    /* u32Align: 0 is automatic mode, alignment size following system. Non-0 for specified alignment size */
    if(0 == u32Align)
    {
        u32Align = DEFAULT_ALIGN;
    }
    else if(u32Align > MAX_ALIGN)
    {
        u32Align = MAX_ALIGN;
    }
    else
    {
        u32Align = (ALIGN_UP(u32Align, DEFAULT_ALIGN));
    }

    switch (enPixelFormat)
    {
        case PIXEL_FORMAT_RGB_BAYER_8BPP:
        {
            u32BitWidth      = 8;
            break;
        }

        case PIXEL_FORMAT_RGB_BAYER_10BPP:
        {
            u32BitWidth      = 10;
            break;
        }

        case PIXEL_FORMAT_RGB_BAYER_12BPP:
        {
            u32BitWidth      = 12;
            break;
        }

        case PIXEL_FORMAT_RGB_BAYER_14BPP:
        {
            u32BitWidth      = 14;
            break;
        }

        case PIXEL_FORMAT_RGB_BAYER_16BPP:
        {
            u32BitWidth      = 16;
            break;
        }

        default:
        {
            u32BitWidth     = 0;
            break;
        }
    }

    if(COMPRESS_MODE_NONE == enCmpMode)
    {
        u32Stride = ALIGN_UP(ALIGN_UP(u32Width * u32BitWidth, 8)/8, u32Align);
        u32Size = u32Stride * u32Height;
    }
    else if(COMPRESS_MODE_LINE == enCmpMode)
    {
        HI_U32 u32Tmp;
        u32Tmp    = ALIGN_UP( (16 + u32Width*u32BitWidth*1000ULL/u32RawCmpRatio + 8192 + 127)/128, 2);
        u32Stride = ALIGN_UP(u32Tmp * 16, u32Align);
        u32Size   = u32Stride * u32Height;
    }
    else if(COMPRESS_MODE_FRAME == enCmpMode)
    {
        u32Size = ALIGN_UP(u32Height*u32Width*u32BitWidth*1000ULL/(u32RawCmpRatio*8), u32Align);
    }

    return u32Size;
}

__inline static HI_U32 VI_GetRawBufferSize(HI_U32 u32Width, HI_U32 u32Height,
                PIXEL_FORMAT_E enPixelFormat, COMPRESS_MODE_E enCmpMode, HI_U32 u32Align)
{
    HI_U32 u32Size = 0;

    u32Size = VI_GetRawBufferSizeEx(u32Width, u32Height, enPixelFormat, enCmpMode, 0, u32Align);

    return u32Size;
}


__inline static HI_U32 AVS_GetPicBufferSize(HI_U32 u32Width, HI_U32 u32Height, COMPRESS_MODE_E enCmpMode, HI_U32 u32Align)
{
    HI_U32 u32Size = 0;

    u32Size =  ALIGN_UP(u32Width, 128) * ALIGN_UP(u32Height, 64) * 2;

    return u32Size;
}



__inline static HI_U32 VDEC_GetPicBufferSize(PAYLOAD_TYPE_E enType, HI_U32 u32Width,
        HI_U32 u32Height, PIXEL_FORMAT_E enPixelFormat, DATA_BITWIDTH_E enBitWidth, HI_U32 u32Align)
{
    HI_U32 u32AlignWidth, u32AlignHeight;
    HI_U32 u32Size = 0;

    if (PT_H264 == enType)
    {
        u32AlignWidth  = ALIGN_UP(u32Width, H264D_ALIGN_W);
        u32AlignHeight = ALIGN_UP(u32Height, H264D_ALIGN_H);
        u32Size = ( (u32AlignWidth * u32AlignHeight ) * 3 ) >> 1;
    }
    else if (PT_H265 == enType)
    {
        u32AlignWidth  = ALIGN_UP(u32Width, H265D_ALIGN_W);
        u32AlignHeight = ALIGN_UP(u32Height, H265D_ALIGN_H);
        u32Size = ( (u32AlignWidth * u32AlignHeight) * 3 ) >> 1;
    }
    else if ( (PT_JPEG == enType) || (PT_MJPEG == enType))
    {
        /* for PIXEL_FORMAT_YVU_SEMIPLANAR_420 */
        u32AlignWidth  = ALIGN_UP(u32Width, JPEGD_ALIGN_W);
        u32AlignHeight = ALIGN_UP(u32Height, JPEGD_ALIGN_H);
        u32Size = (u32AlignWidth * u32AlignHeight * 3) >> 1;
    }
    else
    {
        u32Size = 0;
    }

    return u32Size;
}


__inline static HI_U32 VDEC_GetTmvBufferSize(PAYLOAD_TYPE_E enType, HI_U32 u32Width, HI_U32 u32Height)
{
    HI_U32 WidthInMb, HeightInMb;
    HI_U32 ColMbSize;
    HI_U32 u32Size = 0;

    if (PT_H264 == enType)
    {
        WidthInMb  = ALIGN_UP(u32Width, 16) >> 4;
        HeightInMb = ALIGN_UP(u32Height, 16) >> 4;
        ColMbSize  = 16 * 4;
        u32Size    = ALIGN_UP((ColMbSize * WidthInMb * HeightInMb), 128);
    }
    else if (PT_H265 == enType)
    {
        WidthInMb  = ALIGN_UP(u32Width, 64) >> 4;
        HeightInMb = ALIGN_UP(u32Height, 64) >> 4;
        ColMbSize  = 4 * 4;
        u32Size    = ALIGN_UP((ColMbSize * WidthInMb * HeightInMb), 128);
    }
    else
    {
        u32Size = 0;
    }

    return u32Size;
}


__inline static HI_U32 VENC_GetRefPicInfoBufferSize(PAYLOAD_TYPE_E enType,HI_U32 u32Width, HI_U32 u32Height, HI_U32 u32Align)
{
    HI_U32 u32Size;
    HI_U32 u32AlignWidth, u32AlignHeight;
    HI_U32 u32TmvSize,u32PmeSize,u32PmeInfoSize;


    if( (0 == u32Width) || (0 == u32Height) )
    {
        return 0;
    }

    if(PT_H265 == enType)
    {
        u32AlignWidth  = ALIGN_UP(u32Width,64)>>6;
        u32AlignHeight = ALIGN_UP(u32Height,64)>>6;

        u32TmvSize = u32AlignWidth * u32AlignHeight <<8;
        u32PmeSize = (u32AlignWidth<<4) * (u32AlignHeight<<4);

        u32AlignWidth  = ALIGN_UP(u32Width,1024)>>10;
        u32AlignHeight = ALIGN_UP(u32Height,64)>>6;
        u32PmeInfoSize = (u32AlignWidth * u32AlignHeight)<<5;

        u32Size = u32TmvSize + u32PmeSize + u32PmeInfoSize;

    }
    else if(PT_H264 == enType)
    {
        u32AlignWidth  = ALIGN_UP(u32Width,16)>>4;
        u32AlignHeight = ALIGN_UP(u32Height,16)>>4;
        u32TmvSize =  u32AlignWidth * u32AlignHeight <<5;

        u32AlignWidth  = ALIGN_UP(u32Width,64)>>6;
        u32AlignHeight = ALIGN_UP(u32Height,64)>>6;
        u32PmeSize =  (u32AlignWidth<<4)*(u32AlignHeight<<4);

        u32AlignWidth  = ALIGN_UP(u32Width,4096)>>12;
        u32AlignHeight = ALIGN_UP(u32Height,16)>>4;
        u32PmeInfoSize =  (u32AlignWidth*u32AlignHeight)<<5;

        u32Size =  u32TmvSize + u32PmeSize + u32PmeInfoSize;
    }
    else
    {
        u32Size = 0;
    }
    return u32Size;
}



__inline static HI_U32 VENC_GetRefBufferSize(PAYLOAD_TYPE_E enType, HI_U32 u32Width, HI_U32 u32Height,
                                            DATA_BITWIDTH_E enBitWidth, HI_U32 u32Align)
{
    HI_U32 u32Size = 0;
    HI_U32 u32AlignWidth, u32AlignHeight,u32BitWidth;
    HI_U32 u32YHeaderSize,u32CHeaderSize,u32YSize,u32CSize;

    if( (0 == u32Width) || (0 == u32Height) )
    {
        return 0;
    }

    if (DATA_BITWIDTH_8 == enBitWidth)
    {
        u32BitWidth = 8;
    }
    else
    {
        return 0;
    }

    if(PT_H265 == enType)
    {
        u32AlignWidth  = ALIGN_UP(u32Width, 128);
        u32AlignHeight = ALIGN_UP(u32Height,64)>>6;

        u32YHeaderSize = u32AlignWidth/64*32*u32AlignHeight;
        u32CHeaderSize = u32YHeaderSize;

        u32AlignWidth  = ALIGN_UP(u32Width, 64);
        u32AlignHeight = ALIGN_UP(u32Height, 16);
        u32YSize       = (u32AlignWidth * u32AlignHeight*u32BitWidth)>>3;
        u32CSize       = u32YSize>>1;


        u32Size = u32YHeaderSize + u32CHeaderSize + u32YSize + u32CSize;
    }
    else if(PT_H264 == enType)
    {
        u32AlignWidth  = ALIGN_UP(u32Width, 512);
        u32AlignHeight = ALIGN_UP(u32Height, 16)>>4;
        u32YHeaderSize  = ((u32AlignWidth>>8)<<4)*u32AlignHeight ;
        u32CHeaderSize  = u32YHeaderSize;

        u32AlignWidth  = ALIGN_UP(u32Width, 64);
        u32AlignHeight = ALIGN_UP(u32Height, 16);
        u32YSize = u32AlignWidth * u32AlignHeight;
        u32CSize = u32YSize>>1;

        u32Size = u32YHeaderSize + u32CHeaderSize + u32YSize + u32CSize;
    }
    else
    {
        u32Size = 0;
    }

    return u32Size;
}


__inline static HI_U32  VENC_GetQpmapSizeStride(HI_U32 u32Width)
{
    return DIV_UP(u32Width, 512) * 32;
}

__inline static HI_U32  VENC_GetQpmapSize(HI_U32 u32Width,HI_U32 u32Height)
{
    HI_U32 u32Stride,u32AlignHeight;

    u32Stride      = VENC_GetQpmapSizeStride(u32Width);
    u32AlignHeight = DIV_UP(u32Height,16);

    return u32Stride *  u32AlignHeight;
}

__inline static HI_U32  VENC_GetSkipWeightSizeStride(PAYLOAD_TYPE_E enType,HI_U32 u32Width)
{
    HI_U32 u32Stride;

    if(PT_H265 == enType)
    {
        u32Stride =  DIV_UP(u32Width, 2048) * 16;
    }
    else if(PT_H264 == enType)
    {
        u32Stride =  DIV_UP(u32Width, 512) * 16;
    }
    else
    {
        u32Stride = 0;
    }

    return u32Stride;
}
__inline static HI_U32  VENC_GetSkipWeightSize(PAYLOAD_TYPE_E enType,HI_U32 u32Width,HI_U32 u32Height)
{
    HI_U32 u32Stride,u32AlignHeight;

    u32Stride = VENC_GetSkipWeightSizeStride(enType, u32Width);

    if(PT_H265 == enType)
    {
        u32AlignHeight = DIV_UP(u32Height,64);
    }
    else if(PT_H264 == enType)
    {
        u32AlignHeight = DIV_UP(u32Height,16);
    }
    else
    {
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

