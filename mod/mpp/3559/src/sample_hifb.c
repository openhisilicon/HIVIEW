
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

#ifdef __HuaweiLite__
#include <fb.h>
#else
#include <linux/fb.h>
#endif
#include "hifb.h"
#include "loadbmp.h"
#include "hi_tde_api.h"
#include "hi_tde_type.h"
#include "hi_tde_errcode.h"
#include "hi_math.h"

static HI_CHAR   gs_cExitFlag   = 0;
VO_INTF_TYPE_E   g_enVoIntfType = VO_INTF_BT1120;
OSD_COLOR_FMT_E  g_osdColorFmt  = OSD_COLOR_FMT_RGB1555;

#ifndef __HuaweiLite__
static struct fb_bitfield s_r16 = {10, 5, 0};
static struct fb_bitfield s_g16 = {5, 5, 0};
static struct fb_bitfield s_b16 = {0, 5, 0};
static struct fb_bitfield s_a16 = {15, 1, 0};

static struct fb_bitfield s_a32 = {24, 8, 0};
static struct fb_bitfield s_r32 = {16, 8, 0};
static struct fb_bitfield s_g32 = {8,  8, 0};
static struct fb_bitfield s_b32 = {0,  8, 0};
#endif


#define SAMPLE_IMAGE_WIDTH     300
#define SAMPLE_IMAGE_HEIGHT    150
#define SAMPLE_IMAGE_NUM       20
#define HIFB_RED_1555          0xFC00
#define HIFB_RED_8888          0xFFff0000

#define GRAPHICS_LAYER_G0      0
#define GRAPHICS_LAYER_G1      1
#define GRAPHICS_LAYER_G3      2

#ifndef __HuaweiLite__
#define SAMPLE_IMAGE1_PATH        "./res/%d.bmp"
#define SAMPLE_IMAGE2_PATH        "./res/1280_720.bits"
#define SAMPLE_CURSOR_PATH        "./res/cursor.bmp"
#else
#define SAMPLE_IMAGE1_PATH        "/sharefs/res/%d.bmp"
#define SAMPLE_IMAGE2_PATH        "/sharefs/res/1280_720.bits"
#define SAMPLE_CURSOR_PATH        "/sharefs/res/cursor.bmp"
#endif

pthread_t        g_stHifbThread = 0;
pthread_t        g_stHifbThread1= 0;

HI_U64           g_Phyaddr      = 0;
HI_U64           g_CanvasAddr   = 0;

#if (defined(CONFIG_HI_HIFB_SUPPORT) && defined(CONFIG_HI_VO_SUPPORT))
#define          HIFB_BE_WITH_VO       1
#else
#define          HIFB_BE_WITH_VO       0
#endif


#ifdef __HuaweiLite__
#define NFS_FILE "/sharefs/"
#define CHDIR chdir(NFS_FILE);
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

#ifdef CONFIG_SUPPORT_SAMPLE_ROTATION
#define WIDTH_800 800
#define HEIGHT_500 500
#define SAMPLE_IMAGE_WIDTH     300
#define SAMPLE_IMAGE_HEIGHT    150

static hi_u64 g_canvas_phy = 0;
static hi_void *g_canvas_vir = NULL;
static hi_u64 g_picture_phy = 0;
static hi_void *g_picture_vir = NULL;

static hi_s32 hifb_put_layer_info(PTHREAD_HIFB_SAMPLE_INFO *info);
static hi_s32 hifb_get_canvas(PTHREAD_HIFB_SAMPLE_INFO* info);
static hi_void hifb_put_canvas(hi_void);
static hi_void hifb_init_surface(TDE2_SURFACE_S*, TDE2_SURFACE_S*,TDE2_RECT_S*, TDE2_RECT_S*, hi_u32);
static hi_s32 hifb_draw(PTHREAD_HIFB_SAMPLE_INFO *info);
static hi_s32 hifb_refresh(PTHREAD_HIFB_SAMPLE_INFO *info);
static hi_void hifb_get_var_by_format(PTHREAD_HIFB_SAMPLE_INFO *info, struct fb_var_screeninfo *var_info);
static hi_void hifb_put_rotation_degree(struct fb_var_screeninfo *var_info, HIFB_ROTATE_MODE_E *rotate_mode);
static hi_s32 hifb_put_rotation(PTHREAD_HIFB_SAMPLE_INFO *info);
static hi_void hifb_rotate(VO_DEV vo_dev);
#endif

HI_S32 SAMPLE_HIFB_LoadBmp(const char* filename, HI_U8* pAddr)
{
    OSD_SURFACE_S        Surface;
    OSD_BITMAPFILEHEADER bmpFileHeader;
    OSD_BITMAPINFO       bmpInfo;
#ifdef __HuaweiLite__
    CHDIR
#endif
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
#ifndef __HuaweiLite__
    struct fb_fix_screeninfo fix;
    struct fb_var_screeninfo var;
#endif
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
    TDE2_RECT_S              stSrcRect, stDstRect;
    TDE2_SURFACE_S           stSrc = {0};
    TDE2_SURFACE_S           stDst = {0};
    HI_VOID*                 Viraddr            = NULL;
    HI_U32                   u32Width;
    HI_U32                   u32Height;
    HI_U32                   u32BytePerPixel = 2;
    HIFB_COLOR_FMT_E         enClrFmt = HIFB_FMT_ARGB1555;
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
#ifdef __HuaweiLite__
    struct hifb_info info;
    if (ioctl(pstInfo->fd, FBIOGET_SCREENINFO_HIFB, &info) < 0)
    {
        SAMPLE_PRT("FBIOGET_SCREENINFO_HIFB failed!\n");
        close(pstInfo->fd);
        return HI_NULL;
    }
    info.vinfo.xres = u32Width;
    info.vinfo.yres = u32Height;
    info.oinfo.sarea.w = u32Width;
    info.oinfo.sarea.h = u32Height;
    info.oinfo.bpp = 16;
    info.activate = 0;
    info.vinfo.fmt = HIFB_FMT_ARGB1555;

    if (ioctl(pstInfo->fd, FBIOPUT_SCREENINFO_HIFB, &info) < 0)
    {
        SAMPLE_PRT("Put variable screen info failed!\n");
        close(pstInfo->fd);
        return HI_NULL;
    }

    if (ioctl(pstInfo->fd, FBIOGET_SCREENINFO_HIFB, &info) < 0)
    {
        SAMPLE_PRT("FBIOGET_SCREENINFO_HIFB failed!\n");
        close(pstInfo->fd);
        return HI_NULL;
    }

    u32FixScreenStride = info.oinfo.stride;
    pShowScreen = (HI_U8*)(info.oinfo.fbmem);
    memset(pShowScreen, 0, info.oinfo.fblen);
    SAMPLE_PRT("[Begin]\n");
    SAMPLE_PRT("wait 4 seconds\n");
    usleep(4 * 1000 * 1000);
#else
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
    pShowScreen = mmap(HI_NULL, fix.smem_len, PROT_READ | PROT_WRITE, MAP_SHARED, pstInfo->fd, 0);
    if (MAP_FAILED == pShowScreen)
    {
        SAMPLE_PRT("mmap framebuffer failed!\n");
        close(pstInfo->fd);
        return HI_NULL;
    }
    memset(pShowScreen, 0x0, fix.smem_len);

#endif

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
#ifndef __HuaweiLite__
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
#else
        if (i % 2)
        {
            info.oinfo.sarea.y = info.vinfo.yres;
        }
        else
        {
            info.oinfo.sarea.y = 0;
        }
        ptemp = (pShowScreen + info.vinfo.yres * u32FixScreenStride * (i % 2));
        for (y = 100; y < 300; y++)
        {
            for (x = 0; x < 300; x++)
            {
                if (HIFB_FMT_ARGB8888 == info.vinfo.fmt)
                {
                    *((HI_U32*)ptemp + y * info.vinfo.xres + x) = HIFB_RED_8888;
                }else
                {
                    *((HI_U16*)ptemp + y * info.vinfo.xres + x) = HIFB_RED_1555;
                }
            }
        }
#endif
            /*
            * Note : Not acting on ARGB8888, for ARGB8888 format image's alpha, you can change ptemp[x][y]'s value
            * HIFB_RED_8888 = 0xffff00000 means alpha=255(show),red.0x00ff0000 means alpha=0,red(hide).
            */
            SAMPLE_PRT("expected: the red box will appear!\n");
            sleep(2);
            stAlpha.bAlphaEnable = HI_TRUE;
            stAlpha.u8Alpha0     = 0x0;
            stAlpha.u8Alpha1     = 0xff;
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
#ifndef __HuaweiLite__
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
#else
    /* TDE step1: draw two red line*/
            if ('q' == gs_cExitFlag)
            {
                printf("process exit...\n");
                break;
            }
            if (i % 2)
            {
                info.oinfo.sarea.y = info.vinfo.yres;
            }
            else
            {
                info.oinfo.sarea.y = 0;
            }
            info.oinfo.bpp = 16;
            pHideScreen = pShowScreen + (u32FixScreenStride * info.vinfo.yres) * (i % 2);
            memset(pHideScreen, 0x00, u32FixScreenStride * info.vinfo.yres);
            u64HideScreenPhy = info.oinfo.fbmem + (i % 2) * u32FixScreenStride * info.vinfo.yres;
            pShowLine = pHideScreen;
            for (y = (u32Height / 2 - 2); y < (u32Height / 2 + 2); y++)
            {
                for (x = 0; x < u32Width; x++)
                {
                    if (enClrFmt==HIFB_FMT_ARGB8888)
                    {
                        *((HI_U32*)pShowLine + y * info.vinfo.xres + x) = u32Color;
                    }else
                    {
                        *((HI_U16*)pShowLine + y * info.vinfo.xres + x) = u32Color;
                    }
                }
            }
            for (y = 0; y < u32Height; y++)
            {
                for (x = (u32Width / 2 - 2); x < (u32Width / 2 + 2); x++)
                {
                    if (enClrFmt==HIFB_FMT_ARGB8888)
                    {
                        *((HI_U32*)pShowLine + y * info.vinfo.xres + x) = u32Color;
                    }else
                    {
                        *((HI_U16*)pShowLine + y * info.vinfo.xres + x) = u32Color;
                    }
                }
            }
#endif
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
                stDst.bAlphaExt1555 = HI_TRUE;
                stDst.bAlphaMax255  = HI_TRUE;
                stDst.u8Alpha0      = 0XFF;
                stDst.u8Alpha1      = 0XFF;
                stDst.bYCbCrClut    = HI_FALSE;

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
#ifdef __HuaweiLite__
                if (ioctl(pstInfo->fd, FBIOPAN_DISPLAY_HIFB, &info.oinfo) < 0)
                {
                    SAMPLE_PRT("FBIOPAN_DISPLAY failed!\n");
                    HI_MPI_SYS_MmzFree(g_Phyaddr, Viraddr);
                    g_Phyaddr = 0;
                    close(pstInfo->fd);
                    return HI_NULL;
                }
#else
                if (ioctl(pstInfo->fd, FBIOPAN_DISPLAY, &var) < 0)
                {
                    SAMPLE_PRT("FBIOPAN_DISPLAY failed!\n");
                    HI_MPI_SYS_MmzFree(g_Phyaddr, Viraddr);
                    g_Phyaddr = 0;
                    close(pstInfo->fd);
                    return HI_NULL;
                }
#endif
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
#ifndef __HuaweiLite__
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
                    #ifndef __HuaweiLite__
                    munmap(pShowScreen, fix.smem_len);
                    #endif
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
                    #ifndef __HuaweiLite__
                    munmap(pShowScreen, fix.smem_len);
                    #endif
                    close(pstInfo->fd);
                    return HI_NULL;
                }
                usleep(70*1000);
            }
            SAMPLE_PRT("move the cursor\n");
            sleep(1);
#endif

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

    HI_CHAR                    file[12]        = {0};
    HI_U32                     maxW;
    HI_U32                     maxH;
    PTHREAD_HIFB_SAMPLE_INFO*  pstInfo;
    HIFB_COLORKEY_S            stColorKey;
    TDE2_RECT_S                stSrcRect, stDstRect;
    TDE2_SURFACE_S             stSrc = {0};
    TDE2_SURFACE_S             stDst = {0};
    HI_VOID*                   Viraddr         = NULL;
    TDE_HANDLE                 s32Handle;
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
#ifndef __HuaweiLite__
    struct fb_var_screeninfo   stVarInfo;
    s32Ret = ioctl(pstInfo->fd, FBIOGET_VSCREENINFO, &stVarInfo);
    if (s32Ret < 0)
    {
        SAMPLE_PRT("GET_VSCREENINFO failed!\n");
        close(pstInfo->fd);
        return HI_NULL;
    }
#endif
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
#ifndef __HuaweiLite__
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
#else
    enClrFmt         = HIFB_FMT_ARGB1555;
    struct hifb_info info;
    if (ioctl(pstInfo->fd, FBIOGET_SCREENINFO_HIFB, &info) < 0)
    {
        SAMPLE_PRT("FBIOGET_SCREENINFO_HIFB failed!\n");
        close(pstInfo->fd);
        return HI_NULL;
    }
    info.vinfo.xres = maxW;
    info.vinfo.yres = maxH;
    info.oinfo.sarea.w = maxH;
    info.oinfo.sarea.h = maxH;
    info.oinfo.bpp = 16;
    info.activate = 0;
    info.vinfo.fmt = HIFB_FMT_ARGB1555;
    if (ioctl(pstInfo->fd, FBIOPUT_SCREENINFO_HIFB, &info) < 0)
    {
        SAMPLE_PRT("Put variable screen info failed!\n");
        close(pstInfo->fd);
        return HI_NULL;
    }
    if (ioctl(pstInfo->fd, FBIOGET_SCREENINFO_HIFB, &info) < 0)
    {
        SAMPLE_PRT("Get fix screen info failed!\n");
        close(pstInfo->fd);
        return HI_NULL;
    }
    SAMPLE_PRT("[Begin]\n");
    SAMPLE_PRT("wait 4 seconds\n");
    usleep(4 * 1000 * 1000);
#endif
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

    memset(pBuf, 0x0, stCanvasBuf.stCanvas.u32Pitch * stCanvasBuf.stCanvas.u32Height);
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
        stDst.bAlphaExt1555 = HI_TRUE;
        stDst.bAlphaMax255  = HI_TRUE;
        stDst.u8Alpha0      = 0XFF;
        stDst.u8Alpha1      = 0XFF;
        stDst.bYCbCrClut    = HI_FALSE;

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
        s32Ret = HI_TDE2_EndJob(s32Handle, HI_FALSE, HI_TRUE, 10000);
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

        SAMPLE_COMM_VO_HdmiStop();
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
    printf("\t4:  ROTATION 90 180 270\n");
    return;
}

HI_VOID SAMPLE_HIFB_Usage1(HI_CHAR* sPrgNm)
{
    printf("Usage : %s <index>\n", sPrgNm);
    SAMPLE_HIFB_Usage2();
    return;
}


HI_S32 SAMPLE_HIFB_StandardMode(SAMPLE_VO_CONFIG_S*   pstVoDevInfo)
{
    HI_S32                   s32Ret        = HI_SUCCESS;
    PTHREAD_HIFB_SAMPLE_INFO stInfo0;
   // PTHREAD_HIFB_SAMPLE_INFO stInfo1;
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
    s32Ret = SAMPLE_COMM_VO_StartVO(pstVoDevInfo);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("SAMPLE_COMM_VO_StartVO failed with %d!\n", s32Ret);
        goto SAMPLE_HIFB_StandarMode_0;
    }

    /******************************************
     step 4:  start hifb.
    *****************************************/
    stInfo0.layer     =  VoDev;    /* VO device number */
    stInfo0.fd        = -1;
    stInfo0.ctrlkey   =  2;        /* None buffer */
    stInfo0.bCompress =  HI_FALSE; /* Compress opened or not */
    stInfo0.enColorFmt = HIFB_FMT_ARGB1555;
    if (0 != pthread_create(&g_stHifbThread, 0, SAMPLE_HIFB_PANDISPLAY, (void*)(&stInfo0)))
    {
        SAMPLE_PRT("start hifb thread0 failed!\n");
        goto SAMPLE_HIFB_StandarMode_1;
    }
#if 0
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
#endif
    SAMPLE_HIFB_TO_EXIT();

SAMPLE_HIFB_StandarMode_1:
    SAMPLE_COMM_VO_StopVO(pstVoDevInfo);
SAMPLE_HIFB_StandarMode_0:
    SAMPLE_COMM_SYS_Exit();

    return s32Ret;
}

HI_S32 SAMPLE_HIFB_DoubleBufMode(SAMPLE_VO_CONFIG_S * pstVoDevInfo)
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
    s32Ret = SAMPLE_COMM_VO_StartVO(pstVoDevInfo);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("SAMPLE_COMM_VO_StartVO failed with %d!\n", s32Ret);
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
    SAMPLE_COMM_VO_StopVO(pstVoDevInfo);
SAMPLE_HIFB_DoubleBufMode_0:
    SAMPLE_COMM_SYS_Exit();

    return s32Ret;
}

HI_S32 SAMPLE_HIFB_OneBufMode(SAMPLE_VO_CONFIG_S * pstVoDevInfo)
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
    s32Ret = SAMPLE_COMM_VO_StartVO(pstVoDevInfo);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("SAMPLE_COMM_VO_StartVO failed with %d!\n", s32Ret);
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
    SAMPLE_COMM_VO_StopVO(pstVoDevInfo);
SAMPLE_HIFB_OneBufMode_0:
    SAMPLE_COMM_SYS_Exit();

    return s32Ret;
}

HI_S32 SAMPLE_HIFB_NoneBufMode(SAMPLE_VO_CONFIG_S * pstVoDevInfo)
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
    s32Ret = SAMPLE_COMM_VO_StartVO(pstVoDevInfo);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("SAMPLE_COMM_VO_StartVO failed with %d!\n", s32Ret);
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
    SAMPLE_COMM_VO_StopVO(pstVoDevInfo);
SAMPLE_HIFB_NoneBufMode_0:
    SAMPLE_COMM_SYS_Exit();

    return s32Ret;
}

#ifdef CONFIG_SUPPORT_SAMPLE_ROTATION
hi_s32 sample_hifb_rotation(hi_void)
{
    HI_S32 ret;
    VB_CONFIG_S vb_conf;
    SAMPLE_VO_CONFIG_S vo_dev_info;

    ret = SAMPLE_COMM_VO_GetDefConfig(&vo_dev_info);
    if (ret != HI_SUCCESS) {
        SAMPLE_PRT("SAMPLE_COMM_VO_GetDefConfig failed with %d!\n", ret);
        return HI_FAILURE;
    }

    memset(&vb_conf, 0, sizeof(VB_CONFIG_S));
    ret = SAMPLE_COMM_SYS_Init(&vb_conf);
    if (ret != HI_SUCCESS) {
        SAMPLE_PRT("system init failed with %d!\n", ret);
        return HI_FAILURE;
    }

    /* open display */
    ret = SAMPLE_COMM_VO_StartVO(&vo_dev_info);
    if (ret != HI_SUCCESS) {
        SAMPLE_COMM_SYS_Exit();
        SAMPLE_PRT("SAMPLE_COMM_VO_StartVO failed with %d!\n", ret);
        return HI_FAILURE;
    }

    hifb_rotate(vo_dev_info.VoDev);

    /* close display */
    SAMPLE_COMM_VO_StopVO(&vo_dev_info);

    SAMPLE_COMM_SYS_Exit();

    return (ret);
}

static hi_void hifb_rotate(VO_DEV vo_dev)
{
    hi_s32 ret;
    PTHREAD_HIFB_SAMPLE_INFO info;

    info.layer   =  vo_dev;
    info.fd      = -1;
    info.ctrlkey =  1;
    info.bCompress = HI_FALSE;
    info.enColorFmt = HIFB_FMT_ARGB1555;

    info.fd = open("/dev/fb0", O_RDWR, 0);
    if (info.fd < 0) {
        SAMPLE_PRT("open /dev/fb0 failed!\n");
        return;
    }

    ret = hifb_get_canvas(&info);
    if (ret != HI_SUCCESS) {
        close(info.fd);
        return;
    }

    ret = hifb_draw(&info);
    if (ret != HI_SUCCESS) {
        hifb_put_canvas();
        close(info.fd);
        return;
    }

    ret = hifb_put_rotation(&info);
    if (ret != HI_SUCCESS) {
        hifb_put_canvas();
        close(info.fd);
        return;
    }

    ret = hifb_refresh(&info);
    if (ret != HI_SUCCESS) {
        hifb_put_canvas();
        close(info.fd);
        return;
    }

    hifb_put_canvas();
    close(info.fd);

    return;
}

static hi_s32 hifb_put_layer_info(PTHREAD_HIFB_SAMPLE_INFO *info)
{
    hi_s32 ret;
    HIFB_LAYER_INFO_S layer_info = {0};

    ret = ioctl(info->fd, FBIOGET_LAYER_INFO, &layer_info);
    if (ret < 0) {
        SAMPLE_PRT("GET_LAYER_INFO failed!\n");
        return HI_FAILURE;
    }
    layer_info.u32Mask=0;
    layer_info.eAntiflickerLevel=HIFB_LAYER_ANTIFLICKER_AUTO;
    layer_info.u32Mask |= HIFB_LAYERMASK_BUFMODE;
    layer_info.u32Mask |= HIFB_LAYERMASK_ANTIFLICKER_MODE;
    switch (info->ctrlkey) {
        case 0 :
            layer_info.BufMode = HIFB_LAYER_BUF_ONE;
            break;
        case 1 :
            layer_info.BufMode = HIFB_LAYER_BUF_DOUBLE;
            break;
        default:
            layer_info.BufMode = HIFB_LAYER_BUF_NONE;
    }

    ret = ioctl(info->fd, FBIOPUT_LAYER_INFO, &layer_info);
    if (ret < 0) {
        SAMPLE_PRT("PUT_LAYER_INFO failed!\n");
        return HI_FAILURE;
    }
    return HI_SUCCESS;

}

static hi_s32 hifb_get_canvas(PTHREAD_HIFB_SAMPLE_INFO* info)
{
    hi_s32 ret;
    hi_u32 byte_per_pixel = (info->enColorFmt == HIFB_FMT_ARGB8888) ? 4 : 2;

    ret = HI_MPI_SYS_MmzAlloc(&g_canvas_phy, ((hi_void**)&g_canvas_vir), NULL, NULL, WIDTH_800 * HEIGHT_500 * 2);
    if (ret != HI_SUCCESS) {
        SAMPLE_PRT("allocate memory (maxW*maxH*%d bytes) failed\n", byte_per_pixel);
        return HI_FAILURE;
    }

    if ((g_canvas_phy == 0) || (g_canvas_vir == NULL)) {
        return HI_FAILURE;
    }
    memset(g_canvas_vir, 0xff, WIDTH_800 * HEIGHT_500 * byte_per_pixel);

    ret = HI_MPI_SYS_MmzAlloc(&g_picture_phy, ((hi_void**)&g_picture_vir), NULL, NULL, SAMPLE_IMAGE_WIDTH * SAMPLE_IMAGE_HEIGHT * 2);
    if (ret != HI_SUCCESS) {
        SAMPLE_PRT("allocate memory (maxW*maxH*%d bytes) failed\n", byte_per_pixel);
        return HI_FAILURE;
    }
    if ((g_picture_phy == 0) || (g_picture_vir == NULL)) {
        return HI_FAILURE;
    }
    memset(g_picture_vir, 0xff, SAMPLE_IMAGE_WIDTH * SAMPLE_IMAGE_HEIGHT * byte_per_pixel);
    return HI_SUCCESS;
}

static hi_void hifb_put_canvas(hi_void)
{
    if ((g_canvas_phy == 0) || (g_canvas_vir == NULL) || (g_picture_phy == 0) || (g_picture_vir == NULL)) {
        return;
    }

    HI_MPI_SYS_MmzFree(g_canvas_phy, g_canvas_vir);
    g_canvas_phy = 0;
    g_canvas_vir = NULL;
    HI_MPI_SYS_MmzFree(g_picture_phy, g_picture_vir);
    g_picture_phy = 0;
    g_picture_vir = NULL;

    return;
}

static hi_void hifb_init_surface(TDE2_SURFACE_S *src_surface, TDE2_SURFACE_S *dst_surface,
    TDE2_RECT_S *src_rect, TDE2_RECT_S *dst_rect, hi_u32 byte_per_pixel)
{
    src_rect->s32Xpos = dst_rect->s32Xpos = 0;
    src_rect->s32Ypos = dst_rect->s32Ypos = 0;
    src_rect->u32Height = dst_rect->u32Height = SAMPLE_IMAGE_HEIGHT;
    src_rect->u32Width = dst_rect->u32Width = SAMPLE_IMAGE_WIDTH;
    src_surface->enColorFmt = dst_surface->enColorFmt = TDE2_COLOR_FMT_ARGB1555;
    src_surface->u32Width = SAMPLE_IMAGE_WIDTH;
    src_surface->u32Height = SAMPLE_IMAGE_HEIGHT;
    src_surface->u32Stride = byte_per_pixel * SAMPLE_IMAGE_WIDTH;
    src_surface->PhyAddr = g_picture_phy;
    src_surface->bAlphaExt1555 = dst_surface->bAlphaExt1555 = HI_TRUE;
    src_surface->bAlphaMax255 = dst_surface->bAlphaMax255 = HI_TRUE;
    src_surface->u8Alpha0 = dst_surface->u8Alpha0 = 0XFF;
    src_surface->u8Alpha1 = dst_surface->u8Alpha1 = 0XFF;
    dst_surface->u32Width = WIDTH_800;
    dst_surface->u32Height = HEIGHT_500;
    dst_surface->u32Stride = WIDTH_800 * byte_per_pixel;
    dst_surface->PhyAddr = g_canvas_phy;
    dst_surface->bYCbCrClut = HI_FALSE;
    return;
}

static hi_s32 hifb_draw(PTHREAD_HIFB_SAMPLE_INFO *info)
{
    hi_s32 ret, handle;
    TDE2_RECT_S dst_rect, src_rect;
    TDE2_SURFACE_S dst_surface = {0};
    TDE2_SURFACE_S src_surface = {0};
    hi_u32 byte_per_pixel = (info->enColorFmt == HIFB_FMT_ARGB8888) ? 4 : 2;

    ret = HI_TDE2_Open();
    if (ret < 0) {
        SAMPLE_PRT("HI_TDE2_Open failed :%d!\n", ret);
        return HI_FAILURE;
    }

    hifb_init_surface(&src_surface, &dst_surface, &src_rect, &dst_rect, byte_per_pixel);

    SAMPLE_HIFB_LoadBmp("./res/1.bmp", g_picture_vir);

    /* 1. start job */
    handle = HI_TDE2_BeginJob();
    if (handle == HI_ERR_TDE_INVALID_HANDLE) {
        SAMPLE_PRT("start job failed!\n");
        return HI_FAILURE;
    }

    ret = HI_TDE2_QuickCopy(handle, &src_surface, &src_rect, &dst_surface, &dst_rect);
    if (ret < 0) {
        SAMPLE_PRT("HI_TDE2_QuickCopy:%d failed,ret=0x%x!\n", __LINE__, ret);
        HI_TDE2_CancelJob(handle);
        return HI_FAILURE;
    }

    /* 3. submit job */
    ret = HI_TDE2_EndJob(handle, HI_FALSE, HI_TRUE, 10000);
    if (ret < 0) {
        SAMPLE_PRT("Line:%d,HI_TDE2_EndJob failed,ret=0x%x!\n", __LINE__, ret);
        HI_TDE2_CancelJob(handle);
        return HI_FAILURE;
    }

    HI_TDE2_Close();
    return HI_SUCCESS;
}

static hi_s32 hifb_refresh(PTHREAD_HIFB_SAMPLE_INFO *info)
{
    hi_s32 ret;
    HIFB_BUFFER_S canvas_buf;
    hi_u32 byte_per_pixel;

    switch (info->enColorFmt) {
        case HIFB_FMT_ARGB8888:
            byte_per_pixel = 4;
            break;
    default:
            byte_per_pixel = 2;
            break;
    }
    canvas_buf.stCanvas.u64PhyAddr = g_canvas_phy;
    canvas_buf.stCanvas.u32Height  = HEIGHT_500;
    canvas_buf.stCanvas.u32Width   = WIDTH_800;
    canvas_buf.stCanvas.u32Pitch   = WIDTH_800 * (byte_per_pixel);
    canvas_buf.stCanvas.enFmt      = info->enColorFmt;
    canvas_buf.UpdateRect.x = 0;
    canvas_buf.UpdateRect.y = 0;
    canvas_buf.UpdateRect.w = WIDTH_800;
    canvas_buf.UpdateRect.h = HEIGHT_500;
    ret = ioctl(info->fd, FBIO_REFRESH, &canvas_buf);
    if (ret < 0) {
        SAMPLE_PRT("REFRESH failed!\n");
        return HI_FAILURE;
    }

    SAMPLE_PRT("wait 4 seconds\n");
    usleep(4 * 1000 * 1000);

    return HI_SUCCESS;
}

static hi_void hifb_get_var_by_format(PTHREAD_HIFB_SAMPLE_INFO *info, struct fb_var_screeninfo *var_info)
{
    switch (info->enColorFmt) {
        case HIFB_FMT_ARGB8888:
            var_info->transp = s_a32;
            var_info->red    = s_r32;
            var_info->green  = s_g32;
            var_info->blue   = s_b32;
            var_info->bits_per_pixel = 32;
            break;
        default:
            var_info->transp = s_a16;
            var_info->red    = s_r16;
            var_info->green  = s_g16;
            var_info->blue   = s_b16;
            var_info->bits_per_pixel = 16;
            break;
    }
}

static hi_void hifb_put_rotation_degree(struct fb_var_screeninfo *var_info, HIFB_ROTATE_MODE_E *rotate_mode)
{
    hi_char input;
    printf("\n\n/****************index******************/\n");
    printf("please choose the case which you want to run:\n");
    printf("\t1:  rotate 90\n");
    printf("\t2:  rotate 180\n");
    printf("\t3:  rotate 270\n");
    printf("\t4:  rotate 360\n");

    input = getchar();
    if(input == '1') {
        var_info->xres = var_info->xres_virtual = HEIGHT_500;
        var_info->yres = var_info->yres_virtual = WIDTH_800;
        *rotate_mode = HIFB_ROTATE_90;
    } else if (input == '2') {
        var_info->xres = var_info->xres_virtual = WIDTH_800;
        var_info->yres = var_info->yres_virtual = HEIGHT_500;
        *rotate_mode = HIFB_ROTATE_180;
    } else if (input == '3') {
        var_info->xres = var_info->xres_virtual = HEIGHT_500;
        var_info->yres = var_info->yres_virtual = WIDTH_800;
        *rotate_mode = HIFB_ROTATE_270;
    } else {
        var_info->xres = var_info->xres_virtual = WIDTH_800;
        var_info->yres = var_info->yres_virtual = HEIGHT_500;
        *rotate_mode = HIFB_ROTATE_NONE;
    }

}

static hi_s32 hifb_put_rotation(PTHREAD_HIFB_SAMPLE_INFO *info)
{
    hi_s32 ret;
    struct fb_var_screeninfo var_info = {0};
    HIFB_ROTATE_MODE_E rotate_mode = HIFB_ROTATE_BUTT;

    ret = ioctl(info->fd, FBIOGET_VSCREENINFO, &var_info);
    if (ret < 0) {
        SAMPLE_PRT("FBIOGET_VSCREENINFO failed!\n");
        return HI_FAILURE;
    }

    hifb_get_var_by_format(info, &var_info);

    hifb_put_rotation_degree(&var_info, &rotate_mode);

    ret = ioctl(info->fd, FBIOPUT_VSCREENINFO, &var_info);
    if (ret < 0) {
        SAMPLE_PRT("PUT_VSCREENINFO failed!\n");
        return HI_FAILURE;
    }

    ret = hifb_put_layer_info(info);
    if (ret != HI_SUCCESS) {
        SAMPLE_PRT("PUT_LAYER_INFO failed!\n");
        return HI_FAILURE;
    }

    ret = ioctl (info->fd, FBIOPUT_ROTATE_MODE, &rotate_mode);
    if (ret < 0) {
        SAMPLE_PRT("rotate failed!\n");
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}
#endif

#ifdef __HuaweiLite__
#define SAMPLE_HIFB_NAME "sample"
void SAMPLE_VO_SEL_Usage(HI_VOID)
{
    printf("Usage : %s <index>\n", SAMPLE_HIFB_NAME);
    SAMPLE_HIFB_Usage2();
    return;
}

int app_main(int argc, char *argv[])
#else
int sample_hifb_main(int argc, char* argv[])
#endif
{
    HI_S32           s32Ret       = HI_FAILURE;
    HI_CHAR          ch;
    SAMPLE_VO_CONFIG_S      stVoDevInfo;
#ifdef __HuaweiLite__
    HI_CHAR SelDev;  /* '0': VO_INTF_CVBS, else: BT1120 */

#else
    if (argc != 2)
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
    if (argc < 2)
    {
        SAMPLE_VO_SEL_Usage();
        return HI_FAILURE;
    }
#endif

    if (!strncmp(argv[1], "-h", 2)) {
        SAMPLE_HIFB_Usage1(argv[0]);
        return HI_SUCCESS;
    }

    if (strlen(argv[1]) != 1) {
        SAMPLE_PRT("index invaild!please try again.\n");
        SAMPLE_HIFB_Usage1(argv[0]);
        return HI_FAILURE;
    }

    s32Ret = SAMPLE_COMM_VO_GetDefConfig(&stVoDevInfo);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("SAMPLE_COMM_VO_GetDefConfig failed with %d!\n", s32Ret);
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
#ifdef CONFIG_SUPPORT_SAMPLE_ROTATION
        case '4':
            SAMPLE_PRT("\nindex 4 selected.\n");
            s32Ret = sample_hifb_rotation();
            break;
#endif
        default :
        {
            printf("index invaild! please try again.\n");
            SAMPLE_HIFB_Usage1(argv[0]);
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

