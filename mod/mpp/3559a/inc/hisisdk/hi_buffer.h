
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



__inline static HI_U32 COMMON_GetPicBufferSize(HI_U32 u32Width, HI_U32 u32Height,
        PIXEL_FORMAT_E enPixelFormat, DATA_BITWIDTH_E enBitWidth, COMPRESS_MODE_E enCmpMode, HI_U32 u32Align)
{
    HI_U32 u32Size = 0;
    HI_U32 u32Stride = 0;
    HI_U32 u32BitWidth = 0;
    HI_U32 u32HeadStride = 0;
    HI_U32 u32HeadSize = 0;
    HI_U32 u32AlignHeight = 0;
    HI_U32 u32MainStride = 0;
    HI_U32 u32MainSize = 0;
    HI_U32 u32ExtStride = 0;
    HI_U32 u32ExtSize = 0;
    HI_U32 u32TailInBytes = 0;

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

        case DATA_BITWIDTH_10:
        {
            u32BitWidth = 10;
            break;
        }

        case DATA_BITWIDTH_12:
        {
            u32BitWidth = 12;
            break;
        }

        case DATA_BITWIDTH_14:
        {
            u32BitWidth = 14;
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
        u32Stride = ALIGN_UP((u32Width * u32BitWidth + 7) >> 3, u32Align);

        if(PIXEL_FORMAT_YVU_SEMIPLANAR_420 == enPixelFormat)
        {
            u32Size = (u32Stride * u32AlignHeight) *3 >> 1;
        }
        else if (PIXEL_FORMAT_YVU_SEMIPLANAR_422 == enPixelFormat)
        {
            u32Size = u32Stride * u32AlignHeight * 2;
        }
        else if ((PIXEL_FORMAT_YUV_400 == enPixelFormat) || (PIXEL_FORMAT_S16C1 == enPixelFormat))
        {
            u32Size = u32Stride * u32AlignHeight;
        }
        else
        {
            u32Size = u32Stride * u32AlignHeight * 3;
        }
    }
    else if(COMPRESS_MODE_SEG == enCmpMode)
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
        u32HeadStride = (u32Align == DEFAULT_ALIGN) ? u32HeadStride : (ALIGN_UP(u32HeadStride, u32Align));

        if(8 == u32BitWidth)
        {
            u32MainStride = ALIGN_UP(u32Width, u32Align);

            if(PIXEL_FORMAT_YVU_SEMIPLANAR_420 == enPixelFormat)
            {
                u32HeadSize = (u32HeadStride * u32AlignHeight * 3) >> 1;
                u32MainSize = (u32MainStride * u32AlignHeight * 3) >> 1;
            }
            else if (PIXEL_FORMAT_YVU_SEMIPLANAR_422 == enPixelFormat)
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
        else if (10 == u32BitWidth)
        {
            u32TailInBytes = DIV_UP(u32Width%SEG_CMP_LENGTH*u32BitWidth, 8);
            u32MainStride  = ALIGN_DOWN(u32Width, SEG_CMP_LENGTH) + ((u32TailInBytes > SEG_CMP_LENGTH) ? SEG_CMP_LENGTH : u32TailInBytes);
            u32MainStride  = ALIGN_UP(u32MainStride, u32Align);
            u32ExtStride   = (u32TailInBytes > SEG_CMP_LENGTH) ? (ALIGN_UP(DIV_UP(u32Width,4), u32Align)) :\
                             ALIGN_UP((ALIGN_DOWN(u32Width, SEG_CMP_LENGTH)/4), u32Align);

            if(PIXEL_FORMAT_YVU_SEMIPLANAR_420 == enPixelFormat)
            {
                u32HeadSize = (u32HeadStride * u32AlignHeight * 3) >> 1;
                u32MainSize = (u32MainStride * u32AlignHeight * 3) >> 1;
                u32ExtSize  = (u32ExtStride * u32AlignHeight * 3) >> 1;
            }
            else if (PIXEL_FORMAT_YVU_SEMIPLANAR_422 == enPixelFormat)
            {
                u32HeadSize = u32HeadStride * u32AlignHeight * 2;
                u32MainSize = u32MainStride * u32AlignHeight * 2;
                u32ExtSize  = u32ExtStride * u32AlignHeight * 2;
            }
            else if (PIXEL_FORMAT_YUV_400 == enPixelFormat)
            {
                u32HeadSize = u32HeadStride * u32AlignHeight;
                u32MainSize = u32MainStride * u32AlignHeight;
                u32ExtSize  = u32ExtStride * u32AlignHeight;
            }
            else
            {
                u32HeadSize = u32HeadStride * u32AlignHeight * 3;
                u32MainSize = u32MainStride * u32AlignHeight * 3;
                u32ExtSize  = u32ExtStride * u32AlignHeight * 3;
            }
        }
        else
        {
            u32HeadSize = 0;
            u32MainSize = 0;
            u32ExtSize  = 0;
        }
        u32Size = u32HeadSize + u32MainSize + u32ExtSize;
    }

    return u32Size;
}




__inline static HI_U32 VI_GetRawBufferSize(HI_U32 u32Width, HI_U32 u32Height,
                PIXEL_FORMAT_E enPixelFormat, COMPRESS_MODE_E enCmpMode, HI_U32 u32Align)
{
    HI_U32 u32BitWidth;
    HI_U32 u32Size = 0;
    HI_U32 u32Stride = 0;
    HI_U32 u32CmpRatioLine  = 2000;
    HI_U32 u32CmpRatioFrame = 3000;

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
        u32Tmp    = ALIGN_UP( (16 + u32Width*u32BitWidth*1000ULL/u32CmpRatioLine + 8192 + 127)/128, 2);
        u32Stride = ALIGN_UP(u32Tmp * 16, u32Align);
        u32Size   = u32Stride * u32Height;
    }
    else if(COMPRESS_MODE_FRAME == enCmpMode)
    {
        u32Size = ALIGN_UP(u32Height*u32Width*u32BitWidth*1000ULL/(u32CmpRatioFrame*8), u32Align);
    }

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
    HI_U32 u32HeaderSize = 0, u32HeaderStride = 0;
    HI_U32 u32ExtraSize = 0;
    HI_U32 u32Size = 0;

    if (PT_H264 == enType)
    {
        u32AlignWidth  = ALIGN_UP(u32Width, H264D_ALIGN_W);
        u32AlignHeight = ALIGN_UP(u32Height, H264D_ALIGN_H);
        if(u32Width <= 8192)
        {
            u32HeaderStride = ALIGN_UP(u32AlignWidth, 2048) >> 6;
        }
        else
        {
            u32HeaderStride = ALIGN_UP(u32AlignWidth, 8192) >> 6;
        }
        u32HeaderSize  = u32HeaderStride * (ALIGN_UP(u32AlignHeight, H264D_ALIGN_H) >> 2);
        u32Size = ( (u32HeaderSize + u32AlignWidth * u32AlignHeight ) * 3 ) >> 1;
    }
    else if (PT_H265 == enType)
    {
        u32AlignWidth  = ALIGN_UP(u32Width, H265D_ALIGN_W);
        u32AlignHeight = ALIGN_UP(u32Height, H265D_ALIGN_H);
        if(u32Width <= 8192)
        {
            u32HeaderStride = ALIGN_UP(u32AlignWidth, 2048) >> 6;
        }
        else
        {
            u32HeaderStride = ALIGN_UP(u32AlignWidth, 8192) >> 6;
        }
        u32HeaderSize  = u32HeaderStride * (ALIGN_UP(u32AlignHeight, H265D_ALIGN_H) >> 2);

        if (DATA_BITWIDTH_10 == enBitWidth)
        {
            u32ExtraSize = ALIGN_UP(u32AlignWidth * 2 >> 3, 16) * ALIGN_UP(u32AlignHeight, 32);
        }

        u32Size = ( (u32HeaderSize + u32AlignWidth * u32AlignHeight  + u32ExtraSize) * 3 ) >> 1;
    }
    else if ( (PT_JPEG == enType) || (PT_MJPEG == enType))
    {
        if ( (PIXEL_FORMAT_ARGB_1555 == enPixelFormat) || (PIXEL_FORMAT_ABGR_1555 == enPixelFormat) ||
             (PIXEL_FORMAT_RGB_565 == enPixelFormat) || (PIXEL_FORMAT_BGR_565 == enPixelFormat) )
        {
            u32AlignWidth  = ALIGN_UP(u32Width * 2, JPEGD_RGB_ALIGN);
            u32AlignHeight = ALIGN_UP(u32Height,  JPEGD_RGB_ALIGN);
            u32Size = u32AlignWidth * u32AlignHeight;
        }
        else if ( (PIXEL_FORMAT_RGB_888 == enPixelFormat) || (PIXEL_FORMAT_BGR_888 == enPixelFormat) )
        {
            u32AlignWidth  = ALIGN_UP(u32Width * 3, JPEGD_RGB_ALIGN);
            u32AlignHeight = ALIGN_UP(u32Height,  JPEGD_RGB_ALIGN);
            u32Size = u32AlignWidth * u32AlignHeight;
        }
        else if ( (PIXEL_FORMAT_ARGB_8888 == enPixelFormat) || (PIXEL_FORMAT_ABGR_8888 == enPixelFormat) )
        {
            u32AlignWidth  = ALIGN_UP(u32Width * 4, JPEGD_RGB_ALIGN);
            u32AlignHeight = ALIGN_UP(u32Height,  JPEGD_RGB_ALIGN);
            u32Size = u32AlignWidth * u32AlignHeight;
        }
        else
        {
            /* for PIXEL_FORMAT_YVU_SEMIPLANAR_420 */
            u32AlignWidth  = ALIGN_UP(u32Width, JPEGD_ALIGN_W);
            u32AlignHeight = ALIGN_UP(u32Height, JPEGD_ALIGN_H);
            u32Size = (u32AlignWidth * u32AlignHeight * 3) >> 1;
        }
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


__inline static HI_U32 H265eGetRefPicInfoBufferSize(HI_U32 u32Width, HI_U32 u32Height, HI_U32 u32Align)
{
    HI_U32 u32Size;
    HI_U32 u32AlignWidth, u32AlignHeight;
    HI_U32 u32TmvSize,u32PmeSize,u32PmeInfoSize;

    if( (0 == u32Width) || (0 == u32Height) )
    {
        return 0;
    }

    u32AlignWidth  = ALIGN_UP(u32Width,64)>>6;
    u32AlignHeight = ALIGN_UP(u32Height,64)>>6;

    u32TmvSize = u32AlignWidth * u32AlignHeight <<7;
    u32PmeSize = (u32AlignWidth<<4) * (u32AlignHeight<<4);

    u32AlignWidth  = ALIGN_UP(u32Width,128)>>7;
    u32AlignHeight = ALIGN_UP(u32Height,256)>>8;
    u32PmeInfoSize = (u32AlignWidth * u32AlignHeight)<<5;

    u32Size = u32TmvSize + u32PmeSize + u32PmeInfoSize;

    return u32Size;
}


__inline static HI_U32 H265eGetRefBufferSize(HI_U32 u32Width, HI_U32 u32Height,DATA_BITWIDTH_E enBitWidth, HI_U32 u32Align)
{
    HI_U32 u32Size;
    HI_U32 u32AlignWidth, u32AlignHeight,u32BitWidth;
    HI_U32 u32YHeaderSize,u32CHeaderSize,u32YSize,u32CSize;
    HI_U32 u32TmvSize,u32PmeSize,u32PmeInfoSize;

    if( (0 == u32Width) || (0 == u32Height) )
    {
        return 0;
    }

    switch (enBitWidth)
    {
    case DATA_BITWIDTH_8:
    {
        u32BitWidth = 8;
        break;
    }

    case DATA_BITWIDTH_10:
    {
        u32BitWidth = 10;
        break;
    }

    case DATA_BITWIDTH_12:
    {
        u32BitWidth = 12;
        break;
    }

    case DATA_BITWIDTH_14:
    {
        u32BitWidth = 14;
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


    u32AlignWidth  = ALIGN_UP(u32Width, 128);
    u32AlignHeight = ALIGN_UP(u32Height,64)>>6;

    u32YHeaderSize = u32AlignWidth/64*32*u32AlignHeight;
    u32CHeaderSize = u32YHeaderSize;

    u32AlignWidth  = ALIGN_UP(u32Width, 64);
    u32AlignHeight = ALIGN_UP(u32Height, 16);
    u32YSize       = (u32AlignWidth * u32AlignHeight*u32BitWidth)>>3;
    u32CSize       = u32YSize>>1;


    u32AlignWidth  = ALIGN_UP(u32Width,64)>>6;
    u32AlignHeight = ALIGN_UP(u32Height,64)>>6;

    u32TmvSize = u32AlignWidth * u32AlignHeight <<7;
    u32PmeSize = (u32AlignWidth<<4) * (u32AlignHeight<<4);

    u32AlignWidth  = ALIGN_UP(u32Width,128)>>7;
    u32AlignHeight = ALIGN_UP(u32Height,256)>>8;
    u32PmeInfoSize = (u32AlignWidth * u32AlignHeight)<<5;

    u32Size = u32YHeaderSize + u32CHeaderSize + u32YSize + u32CSize + u32TmvSize + u32PmeSize + u32PmeInfoSize;

    return u32Size;
}

__inline static HI_U32 H264eGetRefPicInfoBufferSize(HI_U32 u32Width, HI_U32 u32Height, HI_U32 u32Align)
{
    HI_U32 u32Size;
    HI_U32 u32AlignWidth, u32AlignHeight;
    HI_U32 u32TmvSize,u32PmeSize,u32PmeInfoSize;


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

    return u32Size;
}

__inline static HI_U32 H264eGetRefBufferSize(HI_U32 u32Width, HI_U32 u32Height, HI_U32 u32Align)
{
    HI_U32 u32Size;
    HI_U32 u32AlignWidth, u32AlignHeight;
    HI_U32 u32YHeaderSize,u32CHeaderSize,u32YSize,u32CSize;

    u32AlignWidth  = ALIGN_UP(u32Width, 512);
    u32AlignHeight = ALIGN_UP(u32Height, 16)>>4;
    u32YHeaderSize  = ((u32AlignWidth>>8)<<4)*u32AlignHeight ;
    u32CHeaderSize  = u32YHeaderSize;

    u32AlignWidth  = ALIGN_UP(u32Width, 64);
    u32AlignHeight = ALIGN_UP(u32Height, 16);
    u32YSize = u32AlignWidth * u32AlignHeight;
    u32CSize = u32YSize>>1;


    u32Size = u32YHeaderSize + u32CHeaderSize + u32YSize + u32CSize;

    return u32Size;
}

__inline static HI_U32 VENC_GetRefBufferSize(PAYLOAD_TYPE_E enType, HI_U32 u32Width, HI_U32 u32Height,
                                            DATA_BITWIDTH_E enBitWidth, HI_U32 u32Align)
{
    HI_U32 u32Size = 0;


    if(PT_H265 == enType)
    {
        u32Size = H265eGetRefBufferSize(u32Width, u32Height, enBitWidth, u32Align);
    }
    else if(PT_H264 == enType)
    {
        u32Size = H264eGetRefBufferSize(u32Width, u32Height, u32Align);
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

