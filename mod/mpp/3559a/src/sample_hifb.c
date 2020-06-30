
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
#include <sys/mman.h>
#include <sys/prctl.h>

#include "sample_comm.h"

#include <linux/fb.h>
#include "hifb.h"
#include "loadbmp.h"
#include "hi_tde_api.h"
#include "hi_tde_type.h"
#include "hi_tde_errcode.h"
#include "hi_math.h"

static HI_CHAR   gs_cExitFlag   = 0;
VO_INTF_TYPE_E   g_enVoIntfType = VO_INTF_BT1120;
OSD_COLOR_FMT_E  g_osdColorFmt  = OSD_COLOR_FMT_RGB1555;


static struct fb_bitfield s_r16 = {10, 5, 0};
static struct fb_bitfield s_g16 = {5, 5, 0};
static struct fb_bitfield s_b16 = {0, 5, 0};
static struct fb_bitfield s_a16 = {15, 1, 0};

static struct fb_bitfield s_a32 = {24, 8, 0};
static struct fb_bitfield s_r32 = {16, 8, 0};
static struct fb_bitfield s_g32 = {8,  8, 0};
static struct fb_bitfield s_b32 = {0,  8, 0};



#define SAMPLE_IMAGE_WIDTH     300
#define SAMPLE_IMAGE_HEIGHT    150
#define SAMPLE_IMAGE_NUM       20
#define HIFB_RED_1555          0xFC00
#define HIFB_RED_8888          0xFFff0000

#define GRAPHICS_LAYER_G0      0
#define GRAPHICS_LAYER_G1      1
#define GRAPHICS_LAYER_G3      2

#define SAMPLE_IMAGE1_PATH        "./res/%d.bmp"
#define SAMPLE_IMAGE2_PATH        "./res/1280_720.bits"
#define SAMPLE_CURSOR_PATH        "./res/cursor.bmp"

pthread_t        g_stHifbThread = 0;
pthread_t        g_stHifbThread1= 0;

HI_U64           g_Phyaddr      = 0;
HI_U64           g_CanvasAddr   = 0;

#if (defined(CONFIG_HI_HIFB_SUPPORT) && defined(CONFIG_HI_VO_SUPPORT))
#define          HIFB_BE_WITH_VO       1
#else
#define          HIFB_BE_WITH_VO       0
#endif

typedef struct hiPTHREAD_HIFB_SAMPLE
{
    HI_S32           fd;          /* fb's file describ */
    HI_S32           layer;       /* which graphic layer */
    HI_S32           ctrlkey;     /* {0,1,2,3}={1buffer, 2buffer, 0buffer pan display, 0buffer refresh} */
    HI_BOOL          bCompress;   /* image compressed or not */
    HIFB_COLOR_FMT_E enColorFmt;  /* color format. */
} PTHREAD_HIFB_SAMPLE_INFO;

typedef struct hiVO_DEVICE_INFO
{
    VO_DEV           VoDev;
    VO_INTF_TYPE_E   enVoIntfType;
 }VO_DEVICE_INFO;

HI_S32 SAMPLE_HIFB_LoadBmp(const char* filename, HI_U8* pAddr)
{
    OSD_SURFACE_S        Surface;
    OSD_BITMAPFILEHEADER bmpFileHeader;
    OSD_BITMAPINFO       bmpInfo;

    if (GetBmpInfo(filename, &bmpFileHeader, &bmpInfo) < 0)
    {
        SAMPLE_PRT("GetBmpInfo err!\n");
        return HI_FAILURE;
    }
    Surface.enColorFmt = g_osdColorFmt;
    CreateSurfaceByBitMap(filename, &Surface, pAddr);

    return HI_SUCCESS;
}

HI_VOID SAMPLE_HIFB_TO_EXIT(HI_VOID)
{
    HI_CHAR ch;
    while (1)
    {
        printf("\npress 'q' to exit this sample.\n");
        while('\n' == (ch = (char)getchar()));
        getchar();
        if ('q' == ch)
        {
            gs_cExitFlag = ch;
            break;
        }
        else
        {
            printf("input invaild! please try again.\n");
        }
    }
    if (0 != g_stHifbThread)
    {
        pthread_join(g_stHifbThread, 0);
        g_stHifbThread = 0;
    }

    if (0 != g_stHifbThread1)
    {
        pthread_join(g_stHifbThread1, 0);
        g_stHifbThread1 = 0;
    }

}

HI_VOID* SAMPLE_HIFB_PANDISPLAY(void* pData)
{
    HI_S32                   i, x, y, s32Ret;
    TDE_HANDLE               s32Handle;
    struct fb_fix_screeninfo fix;
    struct fb_var_screeninfo var;
    HI_U32                   u32FixScreenStride = 0;
    HI_U8*                   pShowScreen;
    HI_U8*                   pHideScreen;
    HI_U64                   u64HideScreenPhy   = 0;
    HI_VOID*                  pShowLine;
    HI_VOID*                  ptemp              = NULL;
    HIFB_ALPHA_S             stAlpha            = {0};
    HIFB_POINT_S             stPoint            = {0, 0};
    HI_CHAR                  file[12]           = {0};

    HI_CHAR                  image_name[128];
    HI_U8*                   pDst               = NULL;
    HI_BOOL                  bShow;
    PTHREAD_HIFB_SAMPLE_INFO* pstInfo;
    HIFB_COLORKEY_S          stColorKey;
    TDE2_RECT_S              stSrcRect={0}, stDstRect={0};
    TDE2_SURFACE_S           stSrc={0}, stDst={0};
    HI_VOID*                 Viraddr            = NULL;
    HI_U32                   u32Width;
    HI_U32                   u32Height;
    HI_U32                   u32BytePerPixel = 2;
    HIFB_COLOR_FMT_E         enClrFmt;
    TDE2_COLOR_FMT_E         enTdeClrFmt     = TDE2_COLOR_FMT_ARGB1555;
    HI_U32                   u32Color        = HIFB_RED_1555;
    HI_CHAR                  thdname[64];

    if (HI_NULL == pData)
    {
        return HI_NULL;
    }
    pstInfo = (PTHREAD_HIFB_SAMPLE_INFO*)pData;
    snprintf(thdname,sizeof(thdname), "HIFB%d_PANDISPLAY",pstInfo->layer);
    prctl(PR_SET_NAME, thdname, 0,0,0);

    if (VO_INTF_HDMI == g_enVoIntfType)
    {
        u32Width  = 1920;
        u32Height = 1080;
    }
    else
    {
        u32Width  = 1920;
        u32Height = 1080;
    }

    switch (pstInfo->layer)
    {
        case GRAPHICS_LAYER_G0 :
            strncpy(file, "/dev/fb0", 12);
            break;
        case GRAPHICS_LAYER_G1 :
            strncpy(file, "/dev/fb1", 12);
            break;
        case GRAPHICS_LAYER_G3:
            strncpy(file, "/dev/fb2", 12);
            break;
        default:
            strncpy(file, "/dev/fb0", 12);
            break;
    }

    /********************************
    * Step 1. open framebuffer device overlay 0
    **********************************/
    pstInfo->fd = open(file, O_RDWR, 0);
    if (pstInfo->fd < 0)
    {
        SAMPLE_PRT("open %s failed!\n", file);
        return HI_NULL;
    }

    bShow = HI_FALSE;
    if (ioctl(pstInfo->fd, FBIOPUT_SHOW_HIFB, &bShow) < 0)
    {
        SAMPLE_PRT("FBIOPUT_SHOW_HIFB failed!\n");
        return HI_NULL;
    }
    /********************************
    * Step 2. set the screen original position
    **********************************/
    /* 2. set the screen original position */
    switch(pstInfo->ctrlkey)
    {
        case 3:
        {
            stPoint.s32XPos = 150;
            stPoint.s32YPos = 150;
        }
        break;
        default:
        {
            stPoint.s32XPos = 0;
            stPoint.s32YPos = 0;
        }
    }

    if (ioctl(pstInfo->fd, FBIOPUT_SCREEN_ORIGIN_HIFB, &stPoint) < 0)
    {
        SAMPLE_PRT("set screen original show position failed!\n");
        close(pstInfo->fd);
        return HI_NULL;
    }

    /********************************
    * Step 3. get the variable screen information
    **********************************/
    if (ioctl(pstInfo->fd, FBIOGET_VSCREENINFO, &var) < 0)
    {
        SAMPLE_PRT("Get variable screen info failed!\n");
        close(pstInfo->fd);
        return HI_NULL;
    }

    /* **********************************************************
    *Step 4. modify the variable screen info
    *            the screen size: IMAGE_WIDTH*IMAGE_HEIGHT
    *            the virtual screen size: VIR_SCREEN_WIDTH*VIR_SCREEN_HEIGHT
    *            (which equals to VIR_SCREEN_WIDTH*(IMAGE_HEIGHT*2))
    *            the pixel format: ARGB1555
    **************************************************************/
    SAMPLE_PRT("[Begin]\n");
    SAMPLE_PRT("wait 4 seconds\n");
    usleep(4 * 1000 * 1000);


    switch (enClrFmt = pstInfo->enColorFmt)
    {
        case HIFB_FMT_ARGB8888:
            var.transp = s_a32;
            var.red    = s_r32;
            var.green  = s_g32;
            var.blue   = s_b32;
            var.bits_per_pixel = 32;
            u32Color         = HIFB_RED_8888;
            enTdeClrFmt      = TDE2_COLOR_FMT_ARGB8888;
            g_osdColorFmt    = OSD_COLOR_FMT_RGB8888;
            break;
        default:
            var.transp = s_a16;
            var.red    = s_r16;
            var.green  = s_g16;
            var.blue   = s_b16;
            var.bits_per_pixel = 16;
            enClrFmt         = HIFB_FMT_ARGB1555;
            u32Color         = HIFB_RED_1555;
            enTdeClrFmt     = TDE2_COLOR_FMT_ARGB1555;
            break;
    }
    u32BytePerPixel    = var.bits_per_pixel/8;

    switch(pstInfo->ctrlkey)
    {
        case 3:
        {
            var.xres_virtual = 48;
            var.yres_virtual = 48;
            var.xres = 48;
            var.yres = 48;
        }
        break;
        default:
        {
            var.xres_virtual = u32Width;
            var.yres_virtual = u32Height * 2;
            var.xres         = u32Width;
            var.yres         = u32Height;
        }
    }

    var.activate       = FB_ACTIVATE_NOW;

    /*********************************
    * Step 5. set the variable screen information
    ***********************************/
    if (ioctl(pstInfo->fd, FBIOPUT_VSCREENINFO, &var) < 0)
    {
        SAMPLE_PRT("Put variable screen info failed!\n");
        close(pstInfo->fd);
        return HI_NULL;
    }

    /**********************************
    * Step 6. get the fix screen information
    ************************************/
    if (ioctl(pstInfo->fd, FBIOGET_FSCREENINFO, &fix) < 0)
    {
        SAMPLE_PRT("Get fix screen info failed!\n");
        close(pstInfo->fd);
        return HI_NULL;
    }
    u32FixScreenStride = fix.line_length;   /*fix screen stride*/

    /***************************************
    * Step 7. map the physical video memory for user use
    ******************************************/
#ifdef __HuaweiLite__
    pShowScreen = fix.smem_start;
#else
    pShowScreen = mmap(HI_NULL, fix.smem_len, PROT_READ | PROT_WRITE, MAP_SHARED, pstInfo->fd, 0);
    if (MAP_FAILED == pShowScreen)
    {
        SAMPLE_PRT("mmap framebuffer failed!\n");
        close(pstInfo->fd);
        return HI_NULL;
    }
#endif

    memset(pShowScreen, 0x0, fix.smem_len);

    /* time to play*/
    bShow = HI_TRUE;
    if (ioctl(pstInfo->fd, FBIOPUT_SHOW_HIFB, &bShow) < 0)
    {
        SAMPLE_PRT("FBIOPUT_SHOW_HIFB failed!\n");
#ifndef __HuaweiLite__
        munmap(pShowScreen, fix.smem_len);
#endif
        close(pstInfo->fd);
        return HI_NULL;
    }

    /* Only for G0 or G1 */
    if (GRAPHICS_LAYER_G0 == pstInfo->layer || GRAPHICS_LAYER_G1 == pstInfo->layer)
    {
        for (i = 0; i < 1; i++)
        {
            if (i % 2)
            {
                var.yoffset = var.yres;
            }
            else
            {
                var.yoffset = 0;
            }
            ptemp = (pShowScreen + var.yres * u32FixScreenStride * (i % 2));
            for (y = 100; y < 300; y++)
            {
                for (x = 0; x < 300; x++)
                {
                    if (HIFB_FMT_ARGB8888 == enClrFmt)
                    {
                        *((HI_U32*)ptemp + y * var.xres + x) = HIFB_RED_8888;
                    }else
                    {
                        *((HI_U16*)ptemp + y * var.xres + x) = HIFB_RED_1555;
                    }
                }
            }
            /*
            * Note : Not acting on ARGB8888, for ARGB8888 format image's alpha, you can change ptemp[x][y]'s value
            * HIFB_RED_8888 = 0xffff00000 means alpha=255(show),red.0x00ff0000 means alpha=0,red(hide).
            */
            SAMPLE_PRT("expected: the red box will appear!\n");
            sleep(2);
            stAlpha.bAlphaEnable = HI_TRUE;
            stAlpha.u8Alpha0     = 0x0;
            stAlpha.u8Alpha1     = 0x0;
            if (ioctl(pstInfo->fd, FBIOPUT_ALPHA_HIFB,  &stAlpha) < 0)
            {
                SAMPLE_PRT("Set alpha failed!\n");
                close(pstInfo->fd);
                return HI_NULL;
            }
            SAMPLE_PRT("expected: after set alpha = 0, the red box will disappear!\n");
            sleep(2);

            stAlpha.u8Alpha0 = 0;
            stAlpha.u8Alpha1 = 0xFF;
            if (ioctl(pstInfo->fd, FBIOPUT_ALPHA_HIFB,  &stAlpha) < 0)
            {
                SAMPLE_PRT("Set alpha failed!\n");
                close(pstInfo->fd);
                return HI_NULL;
            }
            SAMPLE_PRT("expected:after set set alpha = 0xFF, the red box will appear again!\n");
            sleep(2);

            SAMPLE_PRT("expected: the red box will erased by colorkey!\n");
            stColorKey.bKeyEnable = HI_TRUE;
            stColorKey.u32Key     = (enClrFmt==HIFB_FMT_ARGB8888 ? HIFB_RED_8888 : HIFB_RED_1555);
            s32Ret = ioctl(pstInfo->fd, FBIOPUT_COLORKEY_HIFB, &stColorKey);
            if (s32Ret < 0)
            {
                SAMPLE_PRT("FBIOPUT_COLORKEY_HIFB failed!\n");
                close(pstInfo->fd);
                return HI_NULL;
            }
            sleep(2);
            SAMPLE_PRT("expected: the red box will appear again!\n");
            stColorKey.bKeyEnable = HI_FALSE;
            s32Ret = ioctl(pstInfo->fd, FBIOPUT_COLORKEY_HIFB, &stColorKey);
            if (s32Ret < 0)
            {
                SAMPLE_PRT("FBIOPUT_COLORKEY_HIFB failed!\n");
                close(pstInfo->fd);
                return HI_NULL;
            }
            sleep(2);
        }
    }

    /* show bitmap */
    switch (pstInfo->ctrlkey)
    {
        /* 2 means none buffer and just for pan display.*/
        case 2:
        {
            /*change bmp*/
            if (HI_FAILURE == HI_MPI_SYS_MmzAlloc(&g_Phyaddr, ((void**)&Viraddr),
                                                  NULL, NULL, SAMPLE_IMAGE_WIDTH * SAMPLE_IMAGE_HEIGHT * u32BytePerPixel))
            {
                SAMPLE_PRT("allocate memory (maxW*maxH*%d bytes) failed\n",u32BytePerPixel);
                close(pstInfo->fd);
                return HI_NULL;
            }

            s32Ret = HI_TDE2_Open();
            if (s32Ret < 0)
            {
                SAMPLE_PRT("HI_TDE2_Open failed :%d!\n", s32Ret);
                HI_MPI_SYS_MmzFree(g_Phyaddr, Viraddr);
                g_Phyaddr = 0;
                close(pstInfo->fd);
                return HI_NULL;
            }

            SAMPLE_PRT("expected:two red line!\n");
            for (i = 0; i < SAMPLE_IMAGE_NUM; i++)
            {
                if ('q' == gs_cExitFlag)
                {
                    printf("process exit...\n");
                    break;
                }
                /* TDE step1: draw two red line*/
                if (i % 2)
                {
                    var.yoffset = var.yres;
                }
                else
                {
                    var.yoffset = 0;
                }

                pHideScreen = pShowScreen + (u32FixScreenStride * var.yres) * (i % 2);
                memset(pHideScreen, 0x00, u32FixScreenStride * var.yres);
                u64HideScreenPhy = fix.smem_start + (i % 2) * u32FixScreenStride * var.yres;

                pShowLine = pHideScreen;
                for (y = (u32Height / 2 - 2); y < (u32Height / 2 + 2); y++)
                {
                    for (x = 0; x < u32Width; x++)
                    {
                        if (enClrFmt==HIFB_FMT_ARGB8888)
                        {
                            *((HI_U32*)pShowLine + y * var.xres + x) = u32Color;
                        }else
                        {
                            *((HI_U16*)pShowLine + y * var.xres + x) = u32Color;
                        }
                    }
                }
                for (y = 0; y < u32Height; y++)
                {
                    for (x = (u32Width / 2 - 2); x < (u32Width / 2 + 2); x++)
                    {
                        if (enClrFmt==HIFB_FMT_ARGB8888)
                        {
                            *((HI_U32*)pShowLine + y * var.xres + x) = u32Color;
                        }else
                        {
                            *((HI_U16*)pShowLine + y * var.xres + x) = u32Color;
                        }
                    }
                }

                if (ioctl(pstInfo->fd, FBIOPAN_DISPLAY, &var) < 0)
                {
                    SAMPLE_PRT("FBIOPAN_DISPLAY failed!\n");
                    HI_MPI_SYS_MmzFree(g_Phyaddr, Viraddr);
                    g_Phyaddr = 0;
                    close(pstInfo->fd);
                    return HI_NULL;
                }

                /* TDE step2: draw gui picture*/
                snprintf(image_name, sizeof(image_name), SAMPLE_IMAGE1_PATH, i % 2);
#ifdef __HuaweiLite__
                pDst = g_Phyaddr;
#else
                pDst = (HI_U8*)Viraddr;
#endif
                SAMPLE_HIFB_LoadBmp(image_name, pDst);

                /* TDE job step 0. open tde */
                stSrcRect.s32Xpos   = 0;
                stSrcRect.s32Ypos   = 0;
                stSrcRect.u32Height = SAMPLE_IMAGE_HEIGHT;
                stSrcRect.u32Width  = SAMPLE_IMAGE_WIDTH;
                stDstRect.s32Xpos   = 0;
                stDstRect.s32Ypos   = 0;
                stDstRect.u32Height = stSrcRect.u32Height;
                stDstRect.u32Width  = stSrcRect.u32Width;

                stDst.enColorFmt    = enTdeClrFmt;
                stDst.u32Width      = u32Width;
                stDst.u32Height     = u32Height;
                stDst.u32Stride     = u32FixScreenStride;
                stDst.PhyAddr       = u64HideScreenPhy;

                stSrc.enColorFmt    = enTdeClrFmt;
                stSrc.u32Width      = SAMPLE_IMAGE_WIDTH;
                stSrc.u32Height     = SAMPLE_IMAGE_HEIGHT;
                stSrc.u32Stride     = u32BytePerPixel* SAMPLE_IMAGE_WIDTH;
                stSrc.PhyAddr       = g_Phyaddr;
                stSrc.bAlphaExt1555 = HI_TRUE;
                stSrc.bAlphaMax255  = HI_TRUE;
                stSrc.u8Alpha0      = 0XFF;
                stSrc.u8Alpha1      = 0XFF;

                /* TDE job step 1. start job */
                s32Handle = HI_TDE2_BeginJob();
                if (HI_ERR_TDE_INVALID_HANDLE == s32Handle)
                {
                    SAMPLE_PRT("start job failed!\n");
                    HI_MPI_SYS_MmzFree(g_Phyaddr, Viraddr);
                    g_Phyaddr = 0;
                    close(pstInfo->fd);
                    return HI_NULL;
                }

                s32Ret = HI_TDE2_QuickCopy(s32Handle, &stSrc, &stSrcRect, &stDst, &stDstRect);
                if (s32Ret < 0)
                {
                    SAMPLE_PRT("HI_TDE2_QuickCopy:%d failed,ret=0x%x!\n", __LINE__, s32Ret);
                    HI_TDE2_CancelJob(s32Handle);
                    HI_MPI_SYS_MmzFree(g_Phyaddr, Viraddr);
                    g_Phyaddr = 0;
                    close(pstInfo->fd);
                    return HI_NULL;
                }

                /* TDE job step 2. submit job */
                s32Ret = HI_TDE2_EndJob(s32Handle, HI_FALSE, HI_TRUE, 10);
                if (s32Ret < 0)
                {
                    SAMPLE_PRT("Line:%d,HI_TDE2_EndJob failed,ret=0x%x!\n", __LINE__, s32Ret);
                    HI_TDE2_CancelJob(s32Handle);
                    HI_MPI_SYS_MmzFree(g_Phyaddr, Viraddr);
                    g_Phyaddr = 0;
                    close(pstInfo->fd);
                    return HI_NULL;
                }

                if (ioctl(pstInfo->fd, FBIOPAN_DISPLAY, &var) < 0)
                {
                    SAMPLE_PRT("FBIOPAN_DISPLAY failed!\n");
                    HI_MPI_SYS_MmzFree(g_Phyaddr, Viraddr);
                    g_Phyaddr = 0;
                    close(pstInfo->fd);
                    return HI_NULL;
                }
                SAMPLE_PRT("expected: one image in upper left corner of the screen!\n");
                SAMPLE_PRT("Wait 1 seconde.\n");
                sleep(1);

            }
            HI_TDE2_Close();
            HI_MPI_SYS_MmzFree(g_Phyaddr, Viraddr);
            g_Phyaddr = 0;
        }
        break;
        case 3:
        {
            /* move cursor */
            SAMPLE_HIFB_LoadBmp(SAMPLE_CURSOR_PATH,pShowScreen);
            if (ioctl(pstInfo->fd, FBIOPAN_DISPLAY, &var) < 0)
            {
                SAMPLE_PRT("FBIOPAN_DISPLAY failed!\n");
                munmap(pShowScreen, fix.smem_len);
                close(pstInfo->fd);
                return HI_NULL;
            }
            SAMPLE_PRT("show cursor\n");
            sleep(2);
            for(i=0;i<100;i++)
            {
                if ('q' == gs_cExitFlag)
                {
                    printf("process exit...\n");
                    break;
                }
                stPoint.s32XPos += 2;
                stPoint.s32YPos += 2;
                if(ioctl(pstInfo->fd, FBIOPUT_SCREEN_ORIGIN_HIFB, &stPoint) < 0)
                {
                    SAMPLE_PRT("set screen original show position failed!\n");
                    munmap(pShowScreen, fix.smem_len);
                    close(pstInfo->fd);
                    return HI_NULL;
                }
                usleep(70*1000);
            }
            for(i=0;i<100;i++)
            {
                if ('q' == gs_cExitFlag)
                {
                    printf("process exit...\n");
                    break;
                }
                stPoint.s32XPos += 2;
                stPoint.s32YPos -= 2;
                if(ioctl(pstInfo->fd, FBIOPUT_SCREEN_ORIGIN_HIFB, &stPoint) < 0)
                {
                    SAMPLE_PRT("set screen original show position failed!\n");
                    munmap(pShowScreen, fix.smem_len);
                    close(pstInfo->fd);
                    return HI_NULL;
                }
                usleep(70*1000);
            }
            SAMPLE_PRT("move the cursor\n");
            sleep(1);

        }
        break;
        default:
        {
        }
    }

    /* unmap the physical memory */
    #ifndef __HuaweiLite__
    munmap(pShowScreen, fix.smem_len);
    #endif
    bShow = HI_FALSE;
    if (ioctl(pstInfo->fd, FBIOPUT_SHOW_HIFB, &bShow) < 0)
    {
        SAMPLE_PRT("FBIOPUT_SHOW_HIFB failed!\n");
        close(pstInfo->fd);
        return HI_NULL;
    }
    close(pstInfo->fd);
    SAMPLE_PRT("[End]\n");

    return HI_NULL;
}

HI_VOID* SAMPLE_HIFB_REFRESH(void* pData)
{
    HI_S32                     s32Ret          = HI_SUCCESS;
    HIFB_LAYER_INFO_S          stLayerInfo     = {0};
    HIFB_BUFFER_S              stCanvasBuf;
    HI_VOID*                   pBuf;
    HI_U8*                     pDst            = NULL;
    HI_U32                     x;
    HI_U32                     y;
    HI_U32                     i;
    HI_CHAR                    image_name[128];
    HI_BOOL                    bShow;
    HIFB_POINT_S               stPoint         = {0};
    struct fb_var_screeninfo   stVarInfo;
    HI_CHAR                    file[12]        = {0};
    HI_U32                     maxW;
    HI_U32                     maxH;
    PTHREAD_HIFB_SAMPLE_INFO*  pstInfo;
    HIFB_COLORKEY_S            stColorKey;
    TDE2_RECT_S                stSrcRect={0}, stDstRect={0};
    TDE2_SURFACE_S             stSrc={0}, stDst={0};
    HI_VOID*                   Viraddr         = NULL;
    TDE_HANDLE                 s32Handle;
    HI_BOOL                    bCmp            = HI_FALSE;
    HI_U32                     u32BytePerPixel = 2;
    HIFB_COLOR_FMT_E           enClrFmt;
    TDE2_COLOR_FMT_E           enTdeClrFmt     = TDE2_COLOR_FMT_ARGB1555;
    HI_U32                     u32Color        = HIFB_RED_1555;

    prctl(PR_SET_NAME, "HIFB_REFRESH", 0,0,0);

    if (HI_NULL == pData)
    {
        return HI_NULL;
    }
    pstInfo = (PTHREAD_HIFB_SAMPLE_INFO*)pData;

    switch (pstInfo->layer)
    {
        case GRAPHICS_LAYER_G0:
            strncpy(file, "/dev/fb0", 12);
            break;
        case GRAPHICS_LAYER_G1:
            strncpy(file, "/dev/fb1", 12);
            break;
        default:
            strncpy(file, "/dev/fb0", 12);
            break;
    }

    /*************************************
    * 1. open framebuffer device overlay 0
    ****************************************/
    pstInfo->fd = open(file, O_RDWR, 0);
    if (pstInfo->fd < 0)
    {
        SAMPLE_PRT("open %s failed!\n", file);
        return HI_NULL;
    }
    /*all layer surport colorkey*/
    stColorKey.bKeyEnable = HI_TRUE;
    stColorKey.u32Key = 0x0;
    if (ioctl(pstInfo->fd, FBIOPUT_COLORKEY_HIFB, &stColorKey) < 0)
    {
        SAMPLE_PRT("FBIOPUT_COLORKEY_HIFB!\n");
        close(pstInfo->fd);
        return HI_NULL;
    }
    s32Ret = ioctl(pstInfo->fd, FBIOGET_VSCREENINFO, &stVarInfo);
    if (s32Ret < 0)
    {
        SAMPLE_PRT("GET_VSCREENINFO failed!\n");
        close(pstInfo->fd);
        return HI_NULL;
    }

    if (ioctl(pstInfo->fd, FBIOPUT_SCREEN_ORIGIN_HIFB, &stPoint) < 0)
    {
        SAMPLE_PRT("set screen original show position failed!\n");
        close(pstInfo->fd);
        return HI_NULL;
    }

    if (VO_INTF_HDMI == g_enVoIntfType)
    {
        maxW = 1920;
        maxH = 1080;
    }
    else
    {
        maxW = 1920;
        maxH = 1080;
    }

    switch (enClrFmt = pstInfo->enColorFmt)
    {
        case HIFB_FMT_ARGB8888:
            stVarInfo.transp = s_a32;
            stVarInfo.red    = s_r32;
            stVarInfo.green  = s_g32;
            stVarInfo.blue   = s_b32;
            stVarInfo.bits_per_pixel = 32;
            u32Color         = HIFB_RED_8888;
            enTdeClrFmt      = TDE2_COLOR_FMT_ARGB8888;
            g_osdColorFmt    = OSD_COLOR_FMT_RGB8888;
            break;
        default:
            stVarInfo.transp = s_a16;
            stVarInfo.red    = s_r16;
            stVarInfo.green  = s_g16;
            stVarInfo.blue   = s_b16;
            stVarInfo.bits_per_pixel = 16;
            enClrFmt         = HIFB_FMT_ARGB1555;
            u32Color         = HIFB_RED_1555;
            enTdeClrFmt     = TDE2_COLOR_FMT_ARGB1555;
            break;
    }
    u32BytePerPixel    = stVarInfo.bits_per_pixel/8;
    stVarInfo.activate = FB_ACTIVATE_NOW;
    stVarInfo.xres     = stVarInfo.xres_virtual = maxW;
    stVarInfo.yres     = stVarInfo.yres_virtual = maxH;
    s32Ret = ioctl(pstInfo->fd, FBIOPUT_VSCREENINFO, &stVarInfo);
    if (s32Ret < 0)
    {
        SAMPLE_PRT("PUT_VSCREENINFO failed!\n");
        close(pstInfo->fd);
        return HI_NULL;
    }
    switch (pstInfo->ctrlkey)
    {
        case 0 :
        {
            stLayerInfo.BufMode = HIFB_LAYER_BUF_ONE;
            stLayerInfo.u32Mask = HIFB_LAYERMASK_BUFMODE;
            break;
        }

        case 1 :
        {
            stLayerInfo.BufMode = HIFB_LAYER_BUF_DOUBLE;
            stLayerInfo.u32Mask = HIFB_LAYERMASK_BUFMODE;
            break;
        }

        default:
        {
            stLayerInfo.BufMode = HIFB_LAYER_BUF_NONE;
            stLayerInfo.u32Mask = HIFB_LAYERMASK_BUFMODE;
        }
    }
    s32Ret = ioctl(pstInfo->fd, FBIOPUT_LAYER_INFO, &stLayerInfo);
    if (s32Ret < 0)
    {
        SAMPLE_PRT("PUT_LAYER_INFO failed!\n");
        close(pstInfo->fd);
        return HI_NULL;
    }
    bShow = HI_TRUE;
    if (ioctl(pstInfo->fd, FBIOPUT_SHOW_HIFB, &bShow) < 0)
    {
        SAMPLE_PRT("FBIOPUT_SHOW_HIFB failed!\n");
        close(pstInfo->fd);
        return HI_NULL;
    }

    bCmp  =  pstInfo->bCompress;
    if (ioctl(pstInfo->fd, FBIOPUT_COMPRESSION_HIFB, &bCmp))
    {
        SAMPLE_PRT("FBIOPUT_COMPRESSION_HIFB failed!\n");
        close(pstInfo->fd);
        return HI_NULL;
    }

    if (HI_FAILURE == HI_MPI_SYS_MmzAlloc(&g_CanvasAddr, ((void**)&pBuf),
                                          NULL, NULL, maxW * maxH * (u32BytePerPixel)))
    {
        SAMPLE_PRT("allocate memory (maxW*maxH*%d bytes) failed\n",u32BytePerPixel);
        close(pstInfo->fd);
        return HI_NULL;
    }
    stCanvasBuf.stCanvas.u64PhyAddr = g_CanvasAddr;
    stCanvasBuf.stCanvas.u32Height  = maxH;
    stCanvasBuf.stCanvas.u32Width   = maxW;
    stCanvasBuf.stCanvas.u32Pitch   = maxW * (u32BytePerPixel);
    stCanvasBuf.stCanvas.enFmt      = enClrFmt;
    memset(pBuf, 0x00, stCanvasBuf.stCanvas.u32Pitch * stCanvasBuf.stCanvas.u32Height);

    /*change bmp*/
    if (HI_FAILURE == HI_MPI_SYS_MmzAlloc(&g_Phyaddr, ((void**)&Viraddr),
                                          NULL, NULL, SAMPLE_IMAGE_WIDTH * SAMPLE_IMAGE_HEIGHT * u32BytePerPixel))
    {
        SAMPLE_PRT("allocate memory (maxW*maxH*%d bytes) failed\n",u32BytePerPixel);
        HI_MPI_SYS_MmzFree(g_CanvasAddr, pBuf);
        g_CanvasAddr = 0;
        close(pstInfo->fd);
        return HI_NULL;
    }


    s32Ret = HI_TDE2_Open();
    if (s32Ret < 0)
    {
        SAMPLE_PRT("HI_TDE2_Open failed :%d!\n", s32Ret);
        HI_MPI_SYS_MmzFree(g_Phyaddr, Viraddr);
        g_Phyaddr = 0;

        HI_MPI_SYS_MmzFree(g_CanvasAddr, pBuf);
        g_CanvasAddr = 0;

        close(pstInfo->fd);
        return HI_NULL;
    }

    SAMPLE_PRT("[Begin]\n");
    SAMPLE_PRT("expected:two red line!\n");
    /*time to play*/
    for (i = 0; i < SAMPLE_IMAGE_NUM; i++)
    {
        if ('q' == gs_cExitFlag)
        {
            printf("process exit...\n");
            break;
        }

        for (y = (maxH / 2 - 2); y < (maxH / 2 + 2); y++)
        {
            for (x = 0; x < maxW; x++)
            {
                if(enClrFmt == HIFB_FMT_ARGB8888)
                {
                    *((HI_U32*)pBuf + y * maxW + x) = u32Color;
                }else
                {
                    *((HI_U16*)pBuf + y * maxW + x) = u32Color;
                }
            }
        }
        for (y = 0; y < maxH; y++)
        {
            for (x = (maxW / 2 - 2); x < (maxW / 2 + 2); x++)
            {
                if(enClrFmt == HIFB_FMT_ARGB8888)
                {
                    *((HI_U32*)pBuf + y * maxW + x) = u32Color;
                }else
                {
                    *((HI_U16*)pBuf + y * maxW + x) = u32Color;
                }
            }
        }

        stCanvasBuf.UpdateRect.x = 0;
        stCanvasBuf.UpdateRect.y = 0;
        stCanvasBuf.UpdateRect.w = maxW;
        stCanvasBuf.UpdateRect.h = maxH;
        s32Ret = ioctl(pstInfo->fd, FBIO_REFRESH, &stCanvasBuf);
        if (s32Ret < 0)
        {
            SAMPLE_PRT("REFRESH failed!\n");

            HI_MPI_SYS_MmzFree(g_Phyaddr, Viraddr);
            g_Phyaddr = 0;

            HI_MPI_SYS_MmzFree(g_CanvasAddr, pBuf);
            g_CanvasAddr = 0;

            close(pstInfo->fd);
            return HI_NULL;
        }
        sleep(2);

        snprintf(image_name, sizeof(image_name), SAMPLE_IMAGE1_PATH, i % 2);
#ifdef __HuaweiLite__
        pDst = g_Phyaddr;
#else
        pDst = (HI_U8*)Viraddr;
#endif
        SAMPLE_HIFB_LoadBmp(image_name, pDst);

        /* 0. open tde */
        stSrcRect.s32Xpos   = 0;
        stSrcRect.s32Ypos   = 0;
        stSrcRect.u32Height = SAMPLE_IMAGE_HEIGHT;
        stSrcRect.u32Width  = SAMPLE_IMAGE_WIDTH;
        stDstRect.s32Xpos   = 0;
        stDstRect.s32Ypos   = 0;
        stDstRect.u32Height = stSrcRect.u32Height;
        stDstRect.u32Width  = stSrcRect.u32Width;

        stDst.enColorFmt    = enTdeClrFmt;
        stDst.u32Width      = maxW;
        stDst.u32Height     = maxH;
        stDst.u32Stride     = maxW * u32BytePerPixel;
        stDst.PhyAddr       = stCanvasBuf.stCanvas.u64PhyAddr;

        stSrc.enColorFmt    = enTdeClrFmt;
        stSrc.u32Width      = SAMPLE_IMAGE_WIDTH;
        stSrc.u32Height     = SAMPLE_IMAGE_HEIGHT;
        stSrc.u32Stride     = u32BytePerPixel * SAMPLE_IMAGE_WIDTH;
        stSrc.PhyAddr       = g_Phyaddr;
        stSrc.bAlphaExt1555 = HI_TRUE;
        stSrc.bAlphaMax255  = HI_TRUE;
        stSrc.u8Alpha0      = 0XFF;
        stSrc.u8Alpha1      = 0XFF;

        /* 1. start job */
        s32Handle = HI_TDE2_BeginJob();
        if (HI_ERR_TDE_INVALID_HANDLE == s32Handle)
        {
            SAMPLE_PRT("start job failed!\n");

            HI_MPI_SYS_MmzFree(g_Phyaddr, Viraddr);
            g_Phyaddr = 0;

            HI_MPI_SYS_MmzFree(g_CanvasAddr, pBuf);
            g_CanvasAddr = 0;

            close(pstInfo->fd);
            return HI_NULL;
        }

        s32Ret = HI_TDE2_QuickCopy(s32Handle, &stSrc, &stSrcRect, &stDst, &stDstRect);
        if (s32Ret < 0)
        {
            SAMPLE_PRT("HI_TDE2_QuickCopy:%d failed,ret=0x%x!\n", __LINE__, s32Ret);
            HI_TDE2_CancelJob(s32Handle);
            HI_MPI_SYS_MmzFree(g_Phyaddr, Viraddr);
            g_Phyaddr = 0;

            HI_MPI_SYS_MmzFree(g_CanvasAddr, pBuf);
            g_CanvasAddr = 0;
            close(pstInfo->fd);
            return HI_NULL;
        }

        /* 3. submit job */
        s32Ret = HI_TDE2_EndJob(s32Handle, HI_FALSE, HI_TRUE, 10);
        if (s32Ret < 0)
        {
            SAMPLE_PRT("Line:%d,HI_TDE2_EndJob failed,ret=0x%x!\n", __LINE__, s32Ret);
            HI_TDE2_CancelJob(s32Handle);

            HI_MPI_SYS_MmzFree(g_Phyaddr, Viraddr);
            g_Phyaddr = 0;

            HI_MPI_SYS_MmzFree(g_CanvasAddr, pBuf);
            g_CanvasAddr = 0;

            close(pstInfo->fd);
            return HI_NULL;
        }

        stCanvasBuf.UpdateRect.x = 0;
        stCanvasBuf.UpdateRect.y = 0;
        stCanvasBuf.UpdateRect.w = maxW;
        stCanvasBuf.UpdateRect.h = maxH;
        s32Ret = ioctl(pstInfo->fd, FBIO_REFRESH, &stCanvasBuf);
        if (s32Ret < 0)
        {
            SAMPLE_PRT("REFRESH failed!\n");

            HI_MPI_SYS_MmzFree(g_Phyaddr, Viraddr);
            g_Phyaddr = 0;

            HI_MPI_SYS_MmzFree(g_CanvasAddr, pBuf);
            g_CanvasAddr = 0;

            close(pstInfo->fd);
            return HI_NULL;
        }
        sleep(2);
    }

    HI_MPI_SYS_MmzFree(g_Phyaddr, Viraddr);
    g_Phyaddr = 0;

    HI_MPI_SYS_MmzFree(g_CanvasAddr, pBuf);
    g_CanvasAddr = 0;

    close(pstInfo->fd);
    SAMPLE_PRT("[End]\n");

    return HI_NULL;
}

HI_VOID SAMPLE_HIFB_HandleSig(HI_S32 signo)
{
    static int sig_handled = 0;
    signal(SIGINT, SIG_IGN);
    signal(SIGTERM, SIG_IGN);

    if (!sig_handled && (SIGINT == signo || SIGTERM == signo))
    {
        sig_handled = 1;
        gs_cExitFlag = 'q';

        if (g_stHifbThread1)
        {
            pthread_join(g_stHifbThread1, 0);
            g_stHifbThread1 = 0;
        }

        if (g_stHifbThread)
        {
            pthread_join(g_stHifbThread, 0);
            g_stHifbThread = 0;
        }

        if (g_Phyaddr)
        {
            HI_MPI_SYS_MmzFree(g_Phyaddr, HI_NULL);
            g_Phyaddr = 0;
        }

        if (g_CanvasAddr)
        {
            HI_MPI_SYS_MmzFree(g_CanvasAddr, HI_NULL);
            g_CanvasAddr = 0;
        }

        SAMPLE_COMM_SYS_Exit();

        printf("\033[0;31mprogram exit abnormally!\033[0;39m\n");
    }

    exit(-1);
}

HI_VOID SAMPLE_HIFB_Usage2(HI_VOID)
{
    printf("\n\n/****************index******************/\n");
    printf("please choose the case which you want to run:\n");
    printf("\t0:  ARGB8888 standard mode\n");
    printf("\t1:  ARGB1555 BUF_DOUBLE mode\n");
    printf("\t2:  ARGB1555 BUF_ONE mode\n");
    printf("\t3:  ARGB1555 BUF_NONE mode\n");
    printf("\t4:  ARGB1555 BUF_DOUBLE mode with compress\n");
    printf("\t5:  ARGB8888 BUF_DOUBLE mode with compress\n");
    return;
}

HI_VOID SAMPLE_HIFB_Usage1(HI_CHAR* sPrgNm)
{
    printf("Usage : %s <index> <device> <intf>\n", sPrgNm);
    SAMPLE_HIFB_Usage2();
    printf("\n/****************device******************/\n");
    printf("\t 0) VO device 0#, default.\n");
    printf("\t 1) VO device 1#.\n");
    printf("\n/****************intf******************/\n");
    printf("\t 0) VO HDMI output, default.\n");
    printf("\t 1) VO BT1120 output.\n");

    return;
}



HI_S32 SAMPLE_HIFB_StartVO(VO_DEVICE_INFO*   pstVoDevInfo)
{
#if HIFB_BE_WITH_VO
    VO_INTF_TYPE_E           enVoIntfType  = pstVoDevInfo->enVoIntfType;
    g_enVoIntfType                         = pstVoDevInfo->enVoIntfType;
    VO_DEV                   VoDev         = pstVoDevInfo->VoDev;
    VO_PUB_ATTR_S            stPubAttr;
    HI_U32                   u32VoFrmRate;
    SIZE_S                   stSize;
    HI_S32                   s32Ret;

    /******************************************
     * step 1(start vo):  start vo device.
    *****************************************/
    if (VO_INTF_HDMI == enVoIntfType)
    {
        stPubAttr.enIntfSync = VO_OUTPUT_1080P60;
    }
    else
    {
        stPubAttr.enIntfSync = VO_OUTPUT_1080P60;
    }

    stPubAttr.enIntfType = enVoIntfType;
    stPubAttr.u32BgColor = COLOR_RGB_BLUE;
    s32Ret = SAMPLE_COMM_VO_GetWH(stPubAttr.enIntfSync, &stSize.u32Width, \
                                  &stSize.u32Height, &u32VoFrmRate);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("get vo width and height failed with %d!\n", s32Ret);
        return s32Ret;
    }
    s32Ret = SAMPLE_COMM_VO_StartDev(VoDev, &stPubAttr);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("start vo device failed with %d!\n", s32Ret);
        return s32Ret;
    }

    /******************************
    * step 2(start vo): Bind G3 to VO device.
    * Do this after VO Device started.
    * Set bind relationship.
    ********************************/
    s32Ret = HI_MPI_VO_UnBindGraphicLayer(GRAPHICS_LAYER_G3, VoDev);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("UnBindGraphicLayer failed with %d!\n", s32Ret);
        return s32Ret;
    }

    s32Ret = HI_MPI_VO_BindGraphicLayer(GRAPHICS_LAYER_G3, VoDev);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("BindGraphicLayer failed with %d!\n", s32Ret);
        return s32Ret;
    }

    /******************************
    * step 3(start vo): Start hdmi device.
    ********************************/
    if(VO_INTF_HDMI == enVoIntfType)
    {
        SAMPLE_COMM_VO_HdmiStart(stPubAttr.enIntfSync);
    }

    return HI_SUCCESS;
#else
    return HI_SUCCESS;
#endif
}

HI_S32 SAMPLE_HIFB_StopVO(VO_DEVICE_INFO*   pstVoDevInfo)
{
#if HIFB_BE_WITH_VO
    VO_INTF_TYPE_E           enVoIntfType  = pstVoDevInfo->enVoIntfType;
    VO_DEV                   VoDev         = pstVoDevInfo->VoDev;

    if(VO_INTF_HDMI == enVoIntfType)
    {
        SAMPLE_COMM_VO_HdmiStop();
    }
    SAMPLE_COMM_VO_StopDev(VoDev);

    return HI_SUCCESS;
#else
    return HI_SUCCESS;
#endif
}

HI_S32 SAMPLE_HIFB_StandardMode(VO_DEVICE_INFO*   pstVoDevInfo)
{
    HI_S32                   s32Ret        = HI_SUCCESS;
    PTHREAD_HIFB_SAMPLE_INFO stInfo0;
    PTHREAD_HIFB_SAMPLE_INFO stInfo1;
    VO_DEV                   VoDev         = pstVoDevInfo->VoDev;
    VB_CONFIG_S              stVbConf;

    /******************************************
     step  1: init variable
    ******************************************/
    memset(&stVbConf, 0, sizeof(VB_CONFIG_S));

    /******************************************
     step 2: mpp system init.
    ******************************************/
    s32Ret = SAMPLE_COMM_SYS_Init(&stVbConf);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("system init failed with %d!\n", s32Ret);
        goto SAMPLE_HIFB_StandarMode_0;
    }

    /******************************************
     step 3: Start VO device.
     NOTE: Step 3 is optional when VO is running on other system.
    ******************************************/
    s32Ret = SAMPLE_HIFB_StartVO(pstVoDevInfo);
    if(HI_SUCCESS !=s32Ret)
    {
        SAMPLE_PRT("VO device %d start failed\n", pstVoDevInfo->VoDev);
        goto SAMPLE_HIFB_StandarMode_0;
    }

    /******************************************
     step 4:  start hifb.
    *****************************************/
    stInfo0.layer     =  VoDev;    /* VO device number */
    stInfo0.fd        = -1;
    stInfo0.ctrlkey   =  2;        /* None buffer */
    stInfo0.bCompress =  HI_FALSE; /* Compress opened or not */
    stInfo0.enColorFmt = HIFB_FMT_ARGB8888;
    if (0 != pthread_create(&g_stHifbThread, 0, SAMPLE_HIFB_PANDISPLAY, (void*)(&stInfo0)))
    {
        SAMPLE_PRT("start hifb thread0 failed!\n");
        goto SAMPLE_HIFB_StandarMode_1;
    }

    stInfo1.layer   =  2;
    stInfo1.fd      = -1;
    stInfo1.ctrlkey =  3;
    stInfo1.bCompress =  HI_FALSE; /* Compress opened or not */
    stInfo1.enColorFmt = HIFB_FMT_ARGB8888;
    if (0 != pthread_create(&g_stHifbThread1,0,SAMPLE_HIFB_PANDISPLAY,(void *)(&stInfo1)))
    {
        SAMPLE_PRT("start hifb thread1 failed!\n");
        goto SAMPLE_HIFB_StandarMode_1;
    }

    SAMPLE_HIFB_TO_EXIT();

SAMPLE_HIFB_StandarMode_1:
    SAMPLE_HIFB_StopVO(pstVoDevInfo);
SAMPLE_HIFB_StandarMode_0:
    SAMPLE_COMM_SYS_Exit();

    return s32Ret;
}

HI_S32 SAMPLE_HIFB_DoubleBufMode(VO_DEVICE_INFO * pstVoDevInfo)
{
    HI_S32                   s32Ret          = HI_SUCCESS;
    PTHREAD_HIFB_SAMPLE_INFO stInfo0;
    VO_DEV                   VoDev           = pstVoDevInfo->VoDev;
    g_enVoIntfType                           = pstVoDevInfo->enVoIntfType;
    VB_CONFIG_S              stVbConf;

    /******************************************
     step  1: init variable
    ******************************************/
    memset(&stVbConf, 0, sizeof(VB_CONFIG_S));

    /******************************************
     step 2: mpp system init.
    ******************************************/
    s32Ret = SAMPLE_COMM_SYS_Init(&stVbConf);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("system init failed with %d!\n", s32Ret);
        goto SAMPLE_HIFB_DoubleBufMode_0;
    }

    /******************************************
     step 3: Start VO device.
     NOTE: Step 3 is optional when VO is running on other system.
    ******************************************/
    s32Ret = SAMPLE_HIFB_StartVO(pstVoDevInfo);
    if(HI_SUCCESS !=s32Ret)
    {
        SAMPLE_PRT("VO device %d start failed\n", pstVoDevInfo->VoDev);
        goto SAMPLE_HIFB_DoubleBufMode_0;
    }

    /******************************************
     step 4:  start hifb.
    *****************************************/
    stInfo0.layer   =  VoDev;
    stInfo0.fd      = -1;
    stInfo0.ctrlkey =  1;   /* Double buffer */
    stInfo0.bCompress = HI_FALSE;
    stInfo0.enColorFmt = HIFB_FMT_ABGR1555;
    if (0 != pthread_create(&g_stHifbThread, 0, SAMPLE_HIFB_REFRESH, (void*)(&stInfo0)))
    {
        SAMPLE_PRT("start hifb thread failed!\n");
        goto SAMPLE_HIFB_DoubleBufMode_1;
    }

    SAMPLE_HIFB_TO_EXIT();

SAMPLE_HIFB_DoubleBufMode_1:
    SAMPLE_HIFB_StopVO(pstVoDevInfo);
SAMPLE_HIFB_DoubleBufMode_0:
    SAMPLE_COMM_SYS_Exit();

    return s32Ret;
}

HI_S32 SAMPLE_HIFB_OneBufMode(VO_DEVICE_INFO * pstVoDevInfo)
{
    HI_S32                   s32Ret = HI_SUCCESS;

    PTHREAD_HIFB_SAMPLE_INFO stInfo0;

    VO_DEV                   VoDev           = pstVoDevInfo->VoDev;
    g_enVoIntfType                           = pstVoDevInfo->enVoIntfType;
    VB_CONFIG_S              stVbConf;

    /******************************************
     step  1: init variable
    ******************************************/
    memset(&stVbConf, 0, sizeof(VB_CONFIG_S));

    /******************************************
     step 2: mpp system init.
    ******************************************/
    s32Ret = SAMPLE_COMM_SYS_Init(&stVbConf);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("system init failed with %d!\n", s32Ret);
        goto SAMPLE_HIFB_OneBufMode_0;
    }

    /******************************************
     step 3: Start VO device.
     NOTE: Step 3 is optional when VO is running on other system.
    ******************************************/
    s32Ret = SAMPLE_HIFB_StartVO(pstVoDevInfo);
    if(HI_SUCCESS !=s32Ret)
    {
        SAMPLE_PRT("VO device %d start failed\n", pstVoDevInfo->VoDev);
        goto SAMPLE_HIFB_OneBufMode_0;
    }

    /******************************************
     step 4:  start hifb.
    *****************************************/
    stInfo0.layer   =  VoDev;
    stInfo0.fd      = -1;
    stInfo0.ctrlkey =  0;   /* One buffer */
    stInfo0.bCompress = HI_FALSE;
    stInfo0.enColorFmt = HIFB_FMT_ABGR1555;
    if (0 != pthread_create(&g_stHifbThread, 0, SAMPLE_HIFB_REFRESH, (void*)(&stInfo0)))
    {
        SAMPLE_PRT("start hifb thread failed!\n");
        goto SAMPLE_HIFB_OneBufMode_1;
    }

    SAMPLE_HIFB_TO_EXIT();

SAMPLE_HIFB_OneBufMode_1:
    SAMPLE_HIFB_StopVO(pstVoDevInfo);
SAMPLE_HIFB_OneBufMode_0:
    SAMPLE_COMM_SYS_Exit();

    return s32Ret;
}

HI_S32 SAMPLE_HIFB_NoneBufMode(VO_DEVICE_INFO * pstVoDevInfo)
{
    HI_S32                   s32Ret = HI_SUCCESS;
    PTHREAD_HIFB_SAMPLE_INFO stInfo0;
    VO_DEV                   VoDev           = pstVoDevInfo->VoDev;
    g_enVoIntfType                           = pstVoDevInfo->enVoIntfType;
    VB_CONFIG_S              stVbConf;

    /******************************************
     step  1: init variable
    ******************************************/
    memset(&stVbConf, 0, sizeof(VB_CONFIG_S));

    /******************************************
     step 2: mpp system init.
    ******************************************/
    s32Ret = SAMPLE_COMM_SYS_Init(&stVbConf);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("system init failed with %d!\n", s32Ret);
        goto SAMPLE_HIFB_NoneBufMode_0;
    }

    /******************************************
     step 3: Start VO device.
     NOTE: Step 3 is optional when VO is running on other system.
    ******************************************/
    s32Ret = SAMPLE_HIFB_StartVO(pstVoDevInfo);
    if(HI_SUCCESS !=s32Ret)
    {
        SAMPLE_PRT("VO device %d start failed\n", pstVoDevInfo->VoDev);
        goto SAMPLE_HIFB_NoneBufMode_0;
    }

    /******************************************
     step 4:  start hifb.
    *****************************************/
    stInfo0.layer   =  VoDev;
    stInfo0.fd      = -1;
    stInfo0.ctrlkey =  3;    /* None buffer */
    stInfo0.bCompress = HI_FALSE;
    stInfo0.enColorFmt = HIFB_FMT_ABGR1555;
    if (0 != pthread_create(&g_stHifbThread, 0, SAMPLE_HIFB_REFRESH, (void*)(&stInfo0)))
    {
        SAMPLE_PRT("start hifb thread failed!\n");
        goto SAMPLE_HIFB_NoneBufMode_1;
    }

    SAMPLE_HIFB_TO_EXIT();

SAMPLE_HIFB_NoneBufMode_1:
    SAMPLE_HIFB_StopVO(pstVoDevInfo);
SAMPLE_HIFB_NoneBufMode_0:
    SAMPLE_COMM_SYS_Exit();

    return s32Ret;
}

HI_S32 SAMPLE_HIFB_DCMP(VO_DEVICE_INFO * pstVoDevInfo)
{
    HI_S32                   s32Ret         = HI_SUCCESS;
    PTHREAD_HIFB_SAMPLE_INFO stInfo0;
    VO_DEV                   VoDev           = pstVoDevInfo->VoDev;
    g_enVoIntfType                           = pstVoDevInfo->enVoIntfType;
    VB_CONFIG_S              stVbConf;

    /******************************************
     step  1: init variable
    ******************************************/
    memset(&stVbConf, 0, sizeof(VB_CONFIG_S));

    /******************************************
     step 2: mpp system init.
    ******************************************/
    s32Ret = SAMPLE_COMM_SYS_Init(&stVbConf);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("system init failed with %d!\n", s32Ret);
        goto SAMPLE_HIFB_OneBufMode_0;
    }

    /******************************************
     step 3: Start VO device.
     NOTE: Step 3 is optional when VO is running on other system.
    ******************************************/
    s32Ret = SAMPLE_HIFB_StartVO(pstVoDevInfo);
    if(HI_SUCCESS !=s32Ret)
    {
        SAMPLE_PRT("VO device %d start failed\n", pstVoDevInfo->VoDev);
        goto SAMPLE_HIFB_OneBufMode_0;
    }

    /******************************************
     step 4:  start hifb.
    *****************************************/
    stInfo0.layer     =  VoDev;
    stInfo0.fd        = -1;
    stInfo0.ctrlkey   =  1;
    stInfo0.bCompress =  HI_TRUE;
    stInfo0.enColorFmt = HIFB_FMT_ABGR1555;
    if (0 != pthread_create(&g_stHifbThread, 0, SAMPLE_HIFB_REFRESH, (void*)(&stInfo0)))
    {
        SAMPLE_PRT("start hifb thread failed!\n");
        goto SAMPLE_HIFB_OneBufMode_1;
    }

    SAMPLE_HIFB_TO_EXIT();

SAMPLE_HIFB_OneBufMode_1:
    SAMPLE_HIFB_StopVO(pstVoDevInfo);
SAMPLE_HIFB_OneBufMode_0:
    SAMPLE_COMM_SYS_Exit();

    return s32Ret;
}

HI_S32 SAMPLE_HIFB_DCMP_ARGB8888(VO_DEVICE_INFO * pstVoDevInfo)
{
    HI_S32                   s32Ret         = HI_SUCCESS;
    PTHREAD_HIFB_SAMPLE_INFO stInfo0;
    VO_DEV                   VoDev           = pstVoDevInfo->VoDev;
    g_enVoIntfType                           = pstVoDevInfo->enVoIntfType;
    VB_CONFIG_S              stVbConf;

    /******************************************
     step  1: init variable
    ******************************************/
    memset(&stVbConf, 0, sizeof(VB_CONFIG_S));

    /******************************************
     step 2: mpp system init.
    ******************************************/
    s32Ret = SAMPLE_COMM_SYS_Init(&stVbConf);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("system init failed with %d!\n", s32Ret);
        goto SAMPLE_HIFB_OneBufMode_0;
    }

    /******************************************
     step 3: Start VO device.
     NOTE: Step 3 is optional when VO is running on other system.
    ******************************************/
    s32Ret = SAMPLE_HIFB_StartVO(pstVoDevInfo);
    if(HI_SUCCESS !=s32Ret)
    {
        SAMPLE_PRT("VO device %d start failed\n", pstVoDevInfo->VoDev);
        goto SAMPLE_HIFB_OneBufMode_0;
    }

    /******************************************
     step 4:  start hifb.
    *****************************************/
    stInfo0.layer     =  VoDev;
    stInfo0.fd        = -1;
    stInfo0.ctrlkey   =  1;
    stInfo0.bCompress =  HI_TRUE;
    stInfo0.enColorFmt = HIFB_FMT_ARGB8888;
    if (0 != pthread_create(&g_stHifbThread, 0, SAMPLE_HIFB_REFRESH, (void*)(&stInfo0)))
    {
        SAMPLE_PRT("start hifb thread failed!\n");
        goto SAMPLE_HIFB_OneBufMode_1;
    }

    SAMPLE_HIFB_TO_EXIT();

SAMPLE_HIFB_OneBufMode_1:
    SAMPLE_HIFB_StopVO(pstVoDevInfo);
SAMPLE_HIFB_OneBufMode_0:
    SAMPLE_COMM_SYS_Exit();

    return s32Ret;
}


#ifdef __HuaweiLite__
#define SAMPLE_HIFB_NAME "sample"
void SAMPLE_VO_SEL_Usage(HI_VOID)
{
    printf("Usage : %s <index> <device> <intf>\n", SAMPLE_HIFB_NAME);
    SAMPLE_HIFB_Usage2();
    printf("\n/****************device******************/\n");
    printf("\t 0) VO device 0#, default.\n");
    printf("\t 1) VO device 1#.\n");
    printf("\n/****************intf******************/\n");
    printf("\t 0) VO HDMI output, default.\n");
    printf("\t 1) VO BT1120 output.\n");
    return;
}

int app_main(int argc, char *argv[])
#else
int sample_hifb_main(int argc, char* argv[])
#endif
{
    HI_S32           s32Ret       = HI_FAILURE;
    HI_CHAR          ch;
    VO_DEVICE_INFO   stVoDevInfo;
    stVoDevInfo.VoDev             = SAMPLE_VO_DEV_DHD0;
    stVoDevInfo.enVoIntfType      = VO_INTF_HDMI;      /*default:HDMI or BT1120*/
#ifdef __HuaweiLite__
    HI_CHAR SelDev;  /* '0': VO_INTF_CVBS, else: BT1120 */

#else
    if ( (argc < 4) || (1 != strlen(argv[1])) || (1 != strlen(argv[2])))
    {
        SAMPLE_HIFB_Usage1(argv[0]);
        return HI_FAILURE;
    }
#endif

#ifdef __HuaweiLite__
#else
    signal(SIGINT, SAMPLE_HIFB_HandleSig);
    signal(SIGTERM, SAMPLE_HIFB_HandleSig);
#endif

#ifdef __HuaweiLite__
    if ((argc < 4) || (1 != strlen(argv[1])) || (1 != strlen(argv[2])))
    {
        SAMPLE_VO_SEL_Usage();
        return HI_FAILURE;
    }

    if ((argc > 2) && *argv[2] == '1')
#else
    if ((argc > 2) && *argv[2] == '1')  /* '0': DHD0, else: DHD1 */
#endif
    {
        stVoDevInfo.VoDev        = SAMPLE_VO_DEV_DHD1;
    }
    if ((argc > 2) && *argv[3] == '1')  /* '0': VO_INTF_HDMI, else: VO_INTF_BT1120 */
    {
        stVoDevInfo.enVoIntfType = VO_INTF_BT1120;
    }

    if (SAMPLE_VO_DEV_DHD1 == stVoDevInfo.VoDev && VO_INTF_HDMI == stVoDevInfo.enVoIntfType)
    {
        printf("\n**********************************************************\n*");
        SAMPLE_PRT("DHD1 does not support HDMI output interface*\n");
        printf("**********************************************************\n\n");
        SAMPLE_HIFB_Usage1(argv[0]);
        return HI_FAILURE;
    }

    /******************************************
     1 choose the case
    ******************************************/
    ch = *(argv[1]);
    gs_cExitFlag = 0;
    switch (ch)
    {
        case '0':
        {
            SAMPLE_PRT("\nindex 0 selected.\n");
            s32Ret = SAMPLE_HIFB_StandardMode(&stVoDevInfo);
            break;
        }
        case '1':
        {
            SAMPLE_PRT("\nindex 1 selected.\n");
            s32Ret = SAMPLE_HIFB_DoubleBufMode(&stVoDevInfo);
            break;
        }
        case '2':
        {
            SAMPLE_PRT("\nindex 2 selected.\n");
            s32Ret = SAMPLE_HIFB_OneBufMode(&stVoDevInfo);
            break;
        }
        case '3':
        {
            SAMPLE_PRT("\nindex 3 selected.\n");
            s32Ret = SAMPLE_HIFB_NoneBufMode(&stVoDevInfo);
            break;
        }
        case '4':
        {
            SAMPLE_PRT("\nindex 4 selected.\n");
            s32Ret= SAMPLE_HIFB_DCMP(&stVoDevInfo);
            break;
        }
        case '5':
        {
            SAMPLE_PRT("\nindex 5 selected.\n");
            s32Ret= SAMPLE_HIFB_DCMP_ARGB8888(&stVoDevInfo);
            break;
        }

        default :
        {
            printf("index invaild! please try again.\n");
            break;
        }
    }

    if (HI_SUCCESS == s32Ret)
    {
        SAMPLE_PRT("program exit normally!\n");
    }
    else
    {
        SAMPLE_PRT("program exit abnormally!\n");
    }

    return (s32Ret);
}

