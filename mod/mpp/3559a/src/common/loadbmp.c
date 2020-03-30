#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "loadbmp.h"

OSD_COMP_INFO s_OSDCompInfo[OSD_COLOR_FMT_BUTT] = {{0, 4, 4, 4},   /*RGB444*/
    {4, 4, 4, 4},   /*ARGB4444*/
    {0, 5, 5, 5},   /*RGB555*/
    {0, 5, 6, 5},   /*RGB565*/
    {1, 5, 5, 5},   /*ARGB1555*/
    {0, 0, 0, 0},   /*RESERVED*/
    {0, 8, 8, 8},   /*RGB888*/
    {8, 8, 8, 8}    /*ARGB8888*/
};
HI_U16 OSD_MAKECOLOR_U16(HI_U8 r, HI_U8 g, HI_U8 b, OSD_COMP_INFO compinfo)
{
    HI_U8 r1, g1, b1;
    HI_U16 pixel = 0;
    HI_U32 tmp = 15;

    r1 = g1 = b1 = 0;
    r1 = r >> (8 - compinfo.rlen);
    g1 = g >> (8 - compinfo.glen);
    b1 = b >> (8 - compinfo.blen);
    while (compinfo.alen)
    {
        pixel |= (1 << tmp);
        tmp --;
        compinfo.alen--;
    }

    pixel |= (r1 | (g1 << compinfo.blen) | (b1 << (compinfo.blen + compinfo.glen)));
    return pixel;
}

HI_S32 GetBmpInfo(const char* filename,     OSD_BITMAPFILEHEADER*  pBmpFileHeader
                  , OSD_BITMAPINFO* pBmpInfo)
{
    FILE* pFile;

    HI_U16    bfType;

    if (NULL == filename)
    {
        printf("OSD_LoadBMP: filename=NULL\n");
        return -1;
    }

    if ((pFile = fopen((char*)filename, "rb")) == NULL)
    {
        printf("Open file faild:%s!\n", filename);
        return -1;
    }

    (void)fread(&bfType, 1, sizeof(bfType), pFile);
    if (bfType != 0x4d42)
    {
        printf("not bitmap file\n");
        fclose(pFile);
        return -1;
    }

    (void)fread(pBmpFileHeader, 1, sizeof(OSD_BITMAPFILEHEADER), pFile);
    (void)fread(pBmpInfo, 1, sizeof(OSD_BITMAPINFO), pFile);
    fclose(pFile);

    return 0;
}

int LoadBMP(const char* filename, OSD_LOGO_T* pVideoLogo)
{
    FILE* pFile;
    HI_U16  i, j;

    HI_U32  w, h;
    HI_U16 Bpp;
    HI_U16 dstBpp;

    OSD_BITMAPFILEHEADER  bmpFileHeader;
    OSD_BITMAPINFO            bmpInfo;

    HI_U8* pOrigBMPBuf;
    HI_U8* pRGBBuf;
    HI_U32 stride;

    if (NULL == filename)
    {
        printf("OSD_LoadBMP: filename=NULL\n");
        return -1;
    }

    if (GetBmpInfo(filename, &bmpFileHeader, &bmpInfo) < 0)
    {
        return -1;
    }

    Bpp = bmpInfo.bmiHeader.biBitCount / 8;
    if (Bpp < 2)
    {
        /* only support 1555 8888 888 bitmap */
        printf("bitmap format not supported!\n");
        return -1;
    }

    if (bmpInfo.bmiHeader.biCompression != 0)
    {
        printf("not support compressed bitmap file!\n");
        return -1;
    }

    if (bmpInfo.bmiHeader.biHeight < 0)
    {
        printf("bmpInfo.bmiHeader.biHeight < 0\n");
        return -1;
    }

    if ( (pFile = fopen((char*)filename, "rb")) == NULL)
    {
        printf("Open file faild:%s!\n", filename);
        return -1;
    }

    pVideoLogo->width = (HI_U16)bmpInfo.bmiHeader.biWidth;
    pVideoLogo->height = (HI_U16)((bmpInfo.bmiHeader.biHeight > 0) ? bmpInfo.bmiHeader.biHeight : (-bmpInfo.bmiHeader.biHeight));
    w = pVideoLogo->width;
    h = pVideoLogo->height;

    stride = w * Bpp;
#if 1
    if (stride % 4)
    {
        stride = (stride & 0xfffc) + 4;
    }
#endif
    /* RGB8888 or RGB1555 */
    pOrigBMPBuf = (HI_U8*)malloc(h * stride);
    if (NULL == pOrigBMPBuf)
    {
        printf("not enough memory to malloc!\n");
        fclose(pFile);
        return -1;
    }

    pRGBBuf = pVideoLogo->pRGBBuffer;

    fseek(pFile, bmpFileHeader.bfOffBits, 0);
    if (fread(pOrigBMPBuf, 1, h * stride, pFile) != (h * stride) )
    {
        printf("fread error!line:%d\n", __LINE__);
        perror("fread:");
    }

    if (Bpp > 2)
    {
        dstBpp = 4;
    }
    else
    {
        dstBpp = 2;
    }

    if (0 == pVideoLogo->stride)
    {
        pVideoLogo->stride = pVideoLogo->width * dstBpp;
    }

    for (i = 0; i < h; i++)
    {
        for (j = 0; j < w; j++)
        {
            memcpy(pRGBBuf + i * pVideoLogo->stride + j * dstBpp, pOrigBMPBuf + ((h - 1) - i)*stride + j * Bpp, Bpp);

            if (dstBpp == 4)
            {
                //*(pRGBBuf + i*pVideoLogo->stride + j*dstbpp + 3) = random()&0xff; /*alpha*/
                *(pRGBBuf + i * pVideoLogo->stride + j * dstBpp + 3) = 0x80; /*alpha*/
            }
        }

    }

    free(pOrigBMPBuf);
    pOrigBMPBuf = NULL;

    fclose(pFile);
    return 0;
}

int LoadBMPEx(const char* filename, OSD_LOGO_T* pVideoLogo, OSD_COLOR_FMT_E enFmt)
{
    FILE* pFile;
    HI_U16  i, j;

    HI_U32  w, h;
    HI_U16 Bpp;

    OSD_BITMAPFILEHEADER  bmpFileHeader;
    OSD_BITMAPINFO            bmpInfo;

    HI_U8* pOrigBMPBuf;
    HI_U8* pRGBBuf;
    HI_U32 stride;
    HI_U8 r, g, b;
    HI_U8* pStart;
    HI_U16* pDst;

    if (NULL == filename)
    {
        printf("OSD_LoadBMP: filename=NULL\n");
        return -1;
    }

    if (GetBmpInfo(filename, &bmpFileHeader, &bmpInfo) < 0)
    {
        return -1;
    }

    Bpp = bmpInfo.bmiHeader.biBitCount / 8;
    if (Bpp < 2)
    {
        /* only support 1555.8888  888 bitmap */
        printf("bitmap format not supported!\n");
        return -1;
    }

    if (bmpInfo.bmiHeader.biCompression != 0)
    {
        printf("not support compressed bitmap file!\n");
        return -1;
    }

    if (bmpInfo.bmiHeader.biHeight < 0)
    {
        printf("bmpInfo.bmiHeader.biHeight < 0\n");
        return -1;
    }

    if ( (pFile = fopen((char*)filename, "rb")) == NULL)
    {
        printf("Open file faild:%s!\n", filename);
        return -1;
    }

    pVideoLogo->width = (HI_U16)bmpInfo.bmiHeader.biWidth;
    pVideoLogo->height = (HI_U16)((bmpInfo.bmiHeader.biHeight > 0) ? bmpInfo.bmiHeader.biHeight : (-bmpInfo.bmiHeader.biHeight));
    w = pVideoLogo->width;
    h = pVideoLogo->height;

    stride = w * Bpp;
#if 1
    if (stride % 4)
    {
        stride = (stride & 0xfffc) + 4;
    }
#endif

    /* RGB8888 or RGB1555 */
    pOrigBMPBuf = (HI_U8*)malloc(h * stride);
    if (NULL == pOrigBMPBuf)
    {
        printf("not enough memory to malloc!\n");
        fclose(pFile);
        return -1;
    }

    pRGBBuf = pVideoLogo->pRGBBuffer;

    fseek(pFile, bmpFileHeader.bfOffBits, 0);
    if (fread(pOrigBMPBuf, 1, h * stride, pFile) != (h * stride) )
    {
        printf("fread (%d*%d)error!line:%d\n", h, stride, __LINE__);
        perror("fread:");
    }

    if (enFmt >= OSD_COLOR_FMT_RGB888)
    {
        pVideoLogo->stride = pVideoLogo->width * 4;
    }
    else
    {
        pVideoLogo->stride = pVideoLogo->width * 2;
    }

    for (i = 0; i < h; i++)
    {
        for (j = 0; j < w; j++)
        {
            if (Bpp == 3)
            {
                switch (enFmt)
                {
                    case OSD_COLOR_FMT_RGB444:
                    case OSD_COLOR_FMT_RGB555:
                    case OSD_COLOR_FMT_RGB565:
                    case OSD_COLOR_FMT_RGB1555:
                    case OSD_COLOR_FMT_RGB4444:
                        /* start color convert */
                        pStart = pOrigBMPBuf + ((h - 1) - i) * stride + j * Bpp;
                        pDst = (HI_U16*)(pRGBBuf + i * pVideoLogo->stride + j * 2);
                        r = *(pStart);
                        g = *(pStart + 1);
                        b = *(pStart + 2);
                        *pDst = OSD_MAKECOLOR_U16(r, g, b, s_OSDCompInfo[enFmt]);
                        break;

                    case OSD_COLOR_FMT_RGB888:
                    case OSD_COLOR_FMT_RGB8888:
                        memcpy(pRGBBuf + i * pVideoLogo->stride + j * 4, pOrigBMPBuf + ((h - 1) - i)*stride + j * Bpp, Bpp);
                        *(pRGBBuf + i * pVideoLogo->stride + j * 4 + 3) = 0xff; /*alpha*/
                        break;

                    default:
                        printf("file(%s), line(%d), no such format!\n", __FILE__, __LINE__);
                        break;
                }
            }
            else if ((Bpp == 2) || (Bpp == 4)) /*..............*/
            {
                memcpy(pRGBBuf + i * pVideoLogo->stride + j * Bpp, pOrigBMPBuf + ((h - 1) - i)*stride + j * Bpp, Bpp);
            }

        }

    }

    free(pOrigBMPBuf);
    pOrigBMPBuf = NULL;

    fclose(pFile);
    return 0;
}


int LoadBMPCanvas(const char* filename, OSD_LOGO_T* pVideoLogo, OSD_COLOR_FMT_E enFmt)
{
    FILE* pFile;
    HI_U16  i, j;

    HI_U32  w, h;
    HI_U16 Bpp;

    OSD_BITMAPFILEHEADER  bmpFileHeader;
    OSD_BITMAPINFO            bmpInfo;

    HI_U8* pOrigBMPBuf;
    HI_U8* pRGBBuf;
    HI_U32 stride;
    HI_U8 r, g, b;
    HI_U8* pStart;
    HI_U16* pDst;

    if (NULL == filename)
    {
        printf("OSD_LoadBMP: filename=NULL\n");
        return -1;
    }

    if (GetBmpInfo(filename, &bmpFileHeader, &bmpInfo) < 0)
    {
        return -1;
    }

    Bpp = bmpInfo.bmiHeader.biBitCount / 8;
    if (Bpp < 2)
    {
        /* only support 1555.8888  888 bitmap */
        printf("bitmap format not supported!\n");
        return -1;
    }

    if (bmpInfo.bmiHeader.biCompression != 0)
    {
        printf("not support compressed bitmap file!\n");
        return -1;
    }

    if (bmpInfo.bmiHeader.biHeight < 0)
    {
        printf("bmpInfo.bmiHeader.biHeight < 0\n");
        return -1;
    }

    if ( (pFile = fopen((char*)filename, "rb")) == NULL)
    {
        printf("Open file faild:%s!\n", filename);
        return -1;
    }

    w = (HI_U16)bmpInfo.bmiHeader.biWidth;
    h = (HI_U16)((bmpInfo.bmiHeader.biHeight > 0) ? bmpInfo.bmiHeader.biHeight : (-bmpInfo.bmiHeader.biHeight));

    stride = w * Bpp;

#if 1
    if (stride % 4)
    {
        stride = (stride & 0xfffc) + 4;
    }
#endif

    /* RGB8888 or RGB1555 */
    pOrigBMPBuf = (HI_U8*)malloc(h * stride);
    if (NULL == pOrigBMPBuf)
    {
        printf("not enough memory to malloc!\n");
        fclose(pFile);
        return -1;
    }

    pRGBBuf = pVideoLogo->pRGBBuffer;

    if (stride > pVideoLogo->stride)
    {
        printf("Bitmap's stride(%d) is bigger than canvas's stide(%d). Load bitmap error!\n", stride, pVideoLogo->stride);
        free(pOrigBMPBuf);
        fclose(pFile);
        return -1;
    }

    if (h > pVideoLogo->height)
    {
        printf("Bitmap's height(%d) is bigger than canvas's height(%d). Load bitmap error!\n", h, pVideoLogo->height);
        free(pOrigBMPBuf);
        fclose(pFile);
        return -1;
    }

    if (w > pVideoLogo->width)
    {
        printf("Bitmap's width(%d) is bigger than canvas's width(%d). Load bitmap error!\n", w, pVideoLogo->width);
        free(pOrigBMPBuf);
        fclose(pFile);
        return -1;
    }

    fseek(pFile, bmpFileHeader.bfOffBits, 0);
    if (fread(pOrigBMPBuf, 1, h * stride, pFile) != (h * stride) )
    {
        printf("fread (%d*%d)error!line:%d\n", h, stride, __LINE__);
        perror("fread:");
    }

    for (i = 0; i < h; i++)
    {
        for (j = 0; j < w; j++)
        {
            if (Bpp == 3) /*.....*/
            {
                switch (enFmt)
                {
                    case OSD_COLOR_FMT_RGB444:
                    case OSD_COLOR_FMT_RGB555:
                    case OSD_COLOR_FMT_RGB565:
                    case OSD_COLOR_FMT_RGB1555:
                    case OSD_COLOR_FMT_RGB4444:
                        /* start color convert */
                        pStart = pOrigBMPBuf + ((h - 1) - i) * stride + j * Bpp;
                        pDst = (HI_U16*)(pRGBBuf + i * pVideoLogo->stride + j * 2);
                        r = *(pStart);
                        g = *(pStart + 1);
                        b = *(pStart + 2);
                        //printf("Func: %s, line:%d, Bpp: %d, bmp stride: %d, Canvas stride: %d, h:%d, w:%d.\n",
                        //    __FUNCTION__, __LINE__, Bpp, stride, pVideoLogo->stride, i, j);
                        *pDst = OSD_MAKECOLOR_U16(r, g, b, s_OSDCompInfo[enFmt]);

                        break;

                    case OSD_COLOR_FMT_RGB888:
                    case OSD_COLOR_FMT_RGB8888:
                        memcpy(pRGBBuf + i * pVideoLogo->stride + j * 4, pOrigBMPBuf + ((h - 1) - i)*stride + j * Bpp, Bpp);
                        *(pRGBBuf + i * pVideoLogo->stride + j * 4 + 3) = 0xff; /*alpha*/
                        break;

                    default:
                        printf("file(%s), line(%d), no such format!\n", __FILE__, __LINE__);
                        break;
                }
            }
            else if ((Bpp == 2) || (Bpp == 4)) /*..............*/
            {
                memcpy(pRGBBuf + i * pVideoLogo->stride + j * Bpp, pOrigBMPBuf + ((h - 1) - i)*stride + j * Bpp, Bpp);
            }

        }

    }

    free(pOrigBMPBuf);
    pOrigBMPBuf = NULL;

    fclose(pFile);
    return 0;
}

char* GetExtName(char* filename)
{
    char* pret = NULL;
    HI_U32 fnLen;

    if (NULL == filename)
    {
        printf("filename can't be null!");
        return NULL;
    }

    fnLen = strlen(filename);
    while (fnLen)
    {
        pret = filename + fnLen;
        if (*pret == '.')
        { return (pret + 1); }

        fnLen--;
    }

    return pret;
}


int LoadImage(const char* filename, OSD_LOGO_T* pVideoLogo)
{
    char* ext = GetExtName((char*)filename);
    if(HI_NULL == ext)
    {
        printf("GetExtName error!\n");
        return -1;
    }
    if (strcmp(ext, "bmp") == 0)
    {
        if (0 != LoadBMP(filename, pVideoLogo))
        {
            printf("OSD_LoadBMP error!\n");
            return -1;
        }
    }
    else
    {
        printf("not supported image file!\n");
        return -1;
    }

    return 0;
}

int LoadImageEx(const char* filename, OSD_LOGO_T* pVideoLogo, OSD_COLOR_FMT_E enFmt)
{
    char* ext = GetExtName((char*)filename);
    
    if(HI_NULL == ext)
    {
        printf("LoadImageEx error!\n");
        return -1;
    }
    
    if (strcmp(ext, "bmp") == 0)
    {
        if (0 != LoadBMPEx(filename, pVideoLogo, enFmt))
        {
            printf("OSD_LoadBMP error!\n");
            return -1;
        }
    }
    else
    {
        printf("not supported image file!\n");
        return -1;
    }

    return 0;
}


int LoadCanvasEx(const char* filename, OSD_LOGO_T* pVideoLogo, OSD_COLOR_FMT_E enFmt)
{
    char* ext = GetExtName((char*)filename);

    if(HI_NULL == ext)
    {
        printf("LoadCanvasEx error!\n");
        return -1;
    }
    
    if (strcmp(ext, "bmp") == 0)
    {
        if (0 != LoadBMPCanvas(filename, pVideoLogo, enFmt))
        {
            printf("OSD_LoadBMP error!\n");
            return -1;
        }
    }
    else
    {
        printf("not supported image file!\n");
        return -1;
    }

    return 0;
}


HI_S32 LoadBitMap2Surface(const HI_CHAR* pszFileName, const OSD_SURFACE_S* pstSurface, HI_U8* pu8Virt)
{
    OSD_LOGO_T stLogo;
    stLogo.stride = pstSurface->u16Stride;
    stLogo.pRGBBuffer = pu8Virt;

    return LoadImage(pszFileName, &stLogo);
}

HI_S32 CreateSurfaceByBitMap(const HI_CHAR* pszFileName, OSD_SURFACE_S* pstSurface, HI_U8* pu8Virt)
{
    OSD_LOGO_T stLogo;
    stLogo.pRGBBuffer = pu8Virt;
    if (LoadImageEx(pszFileName, &stLogo, pstSurface->enColorFmt) < 0)
    {
        printf("load bmp error!\n");
        return -1;
    }

    pstSurface->u16Height = stLogo.height;
    pstSurface->u16Width = stLogo.width;
    pstSurface->u16Stride = stLogo.stride;

    return 0;
}


HI_S32 CreateSurfaceByCanvas(const HI_CHAR* pszFileName, OSD_SURFACE_S* pstSurface, HI_U8* pu8Virt, HI_U32 u32Width, HI_U32 u32Height, HI_U32 u32Stride)
{
    OSD_LOGO_T stLogo;
    stLogo.pRGBBuffer = pu8Virt;
    stLogo.width = u32Width;
    stLogo.height = u32Height;
    stLogo.stride = u32Stride;
    if (LoadCanvasEx(pszFileName, &stLogo, pstSurface->enColorFmt) < 0)
    {
        printf("load bmp error!\n");
        return -1;
    }

    pstSurface->u16Height = u32Height;
    pstSurface->u16Width = u32Width;
    pstSurface->u16Stride = u32Stride;

    return 0;
}


