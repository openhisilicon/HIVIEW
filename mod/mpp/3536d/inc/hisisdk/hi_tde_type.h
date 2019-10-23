/******************************************************************************

  Copyright (C), 2001-2011, Huawei Tech. Co., Ltd.

 ******************************************************************************
 File Name     : tde_type.h
Version       : Initial Draft
Author        : w54130
Created       : 2007/5/21
Last Modified :
Description   : TDE public type
Function List :
History       :
1.Date        : 2007/5/21
Author      : w54130
Modification: Created file

 ******************************************************************************/
#ifndef __TDE_TYPE_H__
#define __TDE_TYPE_H__


#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* End of #ifdef __cplusplus */

#include "hi_tde_errcode.h"

/****************************************************************************/
/*                             TDE2 types define                             */
/****************************************************************************/
/** TDE handle */
typedef HI_S32 TDE_HANDLE;

/** TDE callback */
typedef HI_VOID (* TDE_FUNC_CB) (HI_VOID *pParaml, HI_VOID *pParamr);


/* color format */
typedef enum hiTDE2_COLOR_FMT_E
{
    TDE2_COLOR_FMT_RGB444 = 0,          /**< RGB444 format */
    TDE2_COLOR_FMT_BGR444,              /**< BGR444 format */    
    TDE2_COLOR_FMT_RGB555,              /**< RGB555 format */
    TDE2_COLOR_FMT_BGR555,              /**< BGR555 format */
    TDE2_COLOR_FMT_RGB565,              /**< RGB565 format */
    TDE2_COLOR_FMT_BGR565,              /**< BGR565 format */
    TDE2_COLOR_FMT_RGB888,              /**< RGB888 format */
    TDE2_COLOR_FMT_BGR888,              /**< BGR888 format */
    TDE2_COLOR_FMT_ARGB4444,            /**< ARGB4444 format */
    TDE2_COLOR_FMT_ABGR4444,            /**< ABGR4444 format */
    TDE2_COLOR_FMT_RGBA4444,            /**< RGBA4444 format */
    TDE2_COLOR_FMT_BGRA4444,            /**< BGRA4444 format */
    TDE2_COLOR_FMT_ARGB1555,            /**< ARGB1555 format */
    TDE2_COLOR_FMT_ABGR1555,            /**< ABGR1555 format */
    TDE2_COLOR_FMT_RGBA1555,            /**< RGBA1555 format */
    TDE2_COLOR_FMT_BGRA1555,            /**< BGRA1555 format */
    TDE2_COLOR_FMT_ARGB8565,            /**< ARGB8565 format */
    TDE2_COLOR_FMT_ABGR8565,            /**< ABGR8565 format */
    TDE2_COLOR_FMT_RGBA8565,            /**< RGBA8565 format */
    TDE2_COLOR_FMT_BGRA8565,            /**< BGRA8565 format */
    TDE2_COLOR_FMT_ARGB8888,            /**< ARGB8888 format */
    TDE2_COLOR_FMT_ABGR8888,            /**< ABGR8888 format */
    TDE2_COLOR_FMT_RGBA8888,            /**< RGBA8888 format */
    TDE2_COLOR_FMT_BGRA8888,            /**< BGRA8888 format */
    TDE2_COLOR_FMT_RABG8888,            /**<RABG8888 format*/
    TDE2_COLOR_FMT_CLUT1,               /**CLUT1 */
    TDE2_COLOR_FMT_CLUT2,               /**CLUT2 */
    TDE2_COLOR_FMT_CLUT4,               /**CLUT4 */
    TDE2_COLOR_FMT_CLUT8,               /**CLUT8 */
    TDE2_COLOR_FMT_ACLUT44,             /**CLUT44 */
    TDE2_COLOR_FMT_ACLUT88,             /**CLUT88 */
    TDE2_COLOR_FMT_A1,                  /**<alpha format??1bit */
    TDE2_COLOR_FMT_A8,                  /**<alpha format??8bit */
    TDE2_COLOR_FMT_YCbCr888,            /**<YUV packet format??no alpha*/
    TDE2_COLOR_FMT_AYCbCr8888,          /**<YUV packet format??with alpha*/
    TDE2_COLOR_FMT_YCbCr422,            /**<YUV packet422 format */
    TDE2_COLOR_FMT_byte,                /**<byte*/
    TDE2_COLOR_FMT_halfword,            /**<halfword*/
    TDE2_COLOR_FMT_JPG_YCbCr400MBP,     /**<Semi-planar YUV400 format in the JPEG encoding format */
    TDE2_COLOR_FMT_JPG_YCbCr422MBHP,    /**<Semi-planar YUV422 format (half of the horizontal sampling)*/
    TDE2_COLOR_FMT_JPG_YCbCr422MBVP,    /**<Semi-planar YUV422 format (half of the vertical sampling) */
    TDE2_COLOR_FMT_MP1_YCbCr420MBP,     /**<Semi-planar YUV420 format */
    TDE2_COLOR_FMT_MP2_YCbCr420MBP,     /**<Semi-planar YUV420 format */
    TDE2_COLOR_FMT_MP2_YCbCr420MBI,     /**<Semi-planar YUV400 format */
    TDE2_COLOR_FMT_JPG_YCbCr420MBP,     /**<Semi-planar YUV400 format in the JPEG encoding format */
    TDE2_COLOR_FMT_JPG_YCbCr444MBP,     /**<Semi-planar YUV444 format */
    TDE2_COLOR_FMT_BUTT                 
} TDE2_COLOR_FMT_E;

/** Semi-planar YUV format */
typedef enum hiTDE2_MB_COLORFMT_E
{
    TDE2_MB_COLOR_FMT_JPG_YCbCr400MBP = 0,/**<Macroblock 400 in the JPEG encoding format*/
    TDE2_MB_COLOR_FMT_JPG_YCbCr422MBHP,   /**<Macroblock 422 in the JPEG encoding format (half of the horizontal sampling) */
    TDE2_MB_COLOR_FMT_JPG_YCbCr422MBVP,   /**<Macroblock 422 in the JPEG encoding format (half of the vertical sampling) */
    TDE2_MB_COLOR_FMT_MP1_YCbCr420MBP,    /**<Macroblock 420 in the MPEG-1 encoding format */
    TDE2_MB_COLOR_FMT_MP2_YCbCr420MBP,    /**<Macroblock 420 in the MPEG-2 encoding format */
    TDE2_MB_COLOR_FMT_MP2_YCbCr420MBI,    /**<Macroblock 420 in the MPEG-2 encoding format (interlaced) */
    TDE2_MB_COLOR_FMT_JPG_YCbCr420MBP,    /**<Macroblock 420 in the JPEG encoding format */
    TDE2_MB_COLOR_FMT_JPG_YCbCr444MBP,    /**<Macroblock 444 in the JPEG encoding format */
    TDE2_MB_COLOR_FMT_BUTT
} TDE2_MB_COLOR_FMT_E;

/* raster picture information */
typedef struct hiTDE2_SURFACE_S
{
    /* the physical address of picture data */
    HI_U32 u32PhyAddr;

    /* color format */
    TDE2_COLOR_FMT_E enColorFmt;

    /* picture height */
    HI_U32 u32Height;

    /* picture width */
    HI_U32 u32Width;

    /* picture stride */
    HI_U32 u32Stride;

    /* the physical address of Clut data */
    HI_U8* pu8ClutPhyAddr;

    /* if the Clut is in YCbCr space */
    HI_BOOL bYCbCrClut;

    /* if the alpha max value is 255 */
    HI_BOOL bAlphaMax255;

    /* for ARGB1555 to extend alpha */
    HI_BOOL bAlphaExt1555; 
    HI_U8 u8Alpha0;
    HI_U8 u8Alpha1;

    HI_U32 u32CbCrPhyAddr;          /**< CbCr address */        
    HI_U32 u32CbCrStride;           /**< CbCr stride */
} TDE2_SURFACE_S;

/** Semi-planar YUV format */
typedef struct hiTDE2_MB_S
{
    TDE2_MB_COLOR_FMT_E enMbFmt;
    HI_U32              u32YPhyAddr;
    HI_U32              u32YWidth;
    HI_U32              u32YHeight;
    HI_U32              u32YStride;
    HI_U32              u32CbCrPhyAddr;
    HI_U32              u32CbCrStride;
} TDE2_MB_S;

typedef struct hiTDE2_RECT_S
{
    HI_S32 s32Xpos;
    HI_S32 s32Ypos;
    HI_U32 u32Width;
    HI_U32 u32Height;
} TDE2_RECT_S;

/* the operation modes for raster picture */
typedef enum hiTDE2_ALUCMD_E
{
    TDE2_ALUCMD_NONE = 0x0,         /**< none */    
    TDE2_ALUCMD_BLEND = 0x1,        /**< Alpha blending type */
    TDE2_ALUCMD_ROP = 0x2,          /**< rop */
    TDE2_ALUCMD_COLORIZE = 0x4,     /**< Colorize */
    TDE2_ALUCMD_BUTT = 0x8          
} TDE2_ALUCMD_E;

/** the ROP type supported by the TDE */
typedef enum hiTDE2_ROP_CODE_E
{
    TDE2_ROP_BLACK = 0,     /**<Blackness*/
    TDE2_ROP_NOTMERGEPEN,   /**<~(S2 | S1)*/
    TDE2_ROP_MASKNOTPEN,    /**<~S2&S1*/
    TDE2_ROP_NOTCOPYPEN,    /**< ~S2*/
    TDE2_ROP_MASKPENNOT,    /**< S2&~S1 */
    TDE2_ROP_NOT,           /**< ~S1 */
    TDE2_ROP_XORPEN,        /**< S2^S1 */
    TDE2_ROP_NOTMASKPEN,    /**< ~(S2 & S1) */
    TDE2_ROP_MASKPEN,       /**< S2&S1 */
    TDE2_ROP_NOTXORPEN,     /**< ~(S2^S1) */
    TDE2_ROP_NOP,           /**< S1 */
    TDE2_ROP_MERGENOTPEN,   /**< ~S2|S1 */
    TDE2_ROP_COPYPEN,       /**< S2 */
    TDE2_ROP_MERGEPENNOT,   /**< S2|~S1 */
    TDE2_ROP_MERGEPEN,      /**< S2|S1 */
    TDE2_ROP_WHITE,         /**< Whiteness */
    TDE2_ROP_BUTT
} TDE2_ROP_CODE_E;

/** the mirror attributes of a picture */
typedef enum hiTDE2_MIRROR_E
{
    TDE2_MIRROR_NONE = 0,       
    TDE2_MIRROR_HORIZONTAL,     
    TDE2_MIRROR_VERTICAL,       
    TDE2_MIRROR_BOTH,           
    TDE2_MIRROR_BUTT
} TDE2_MIRROR_E;

/* Clip mode */
typedef enum hiTDE2_CLIPMODE_E
{
    TDE2_CLIPMODE_NONE = 0, 
    TDE2_CLIPMODE_INSIDE,   
    TDE2_CLIPMODE_OUTSIDE,  
    TDE2_CLIPMODE_BUTT
} TDE2_CLIPMODE_E;

/* the resize mode of macro picture */
typedef enum hiTDE2_MBRESIZE_E
{
    TDE2_MBRESIZE_NONE = 0,         
    TDE2_MBRESIZE_QUALITY_LOW,      
    TDE2_MBRESIZE_QUALITY_MIDDLE,   
    TDE2_MBRESIZE_QUALITY_HIGH,    
    TDE2_MBRESIZE_BUTT
} TDE2_MBRESIZE_E;

/** the attributes of the picture fill colors */
typedef struct hiTDE2_FILLCOLOR_S
{
    TDE2_COLOR_FMT_E enColorFmt;    
    HI_U32           u32FillColor;  
} TDE2_FILLCOLOR_S;

/** colorkey mode */
typedef enum hiTDE2_COLORKEY_MODE_E
{
    TDE2_COLORKEY_MODE_NONE = 0,     /**< no color key operation*/
    TDE2_COLORKEY_MODE_FOREGROUND,   /**< Performs the colorkey operation in the foreground mode*/
    TDE2_COLORKEY_MODE_BACKGROUND,   /**< Performs the colorkey operation in the background mode*/
    TDE2_COLORKEY_MODE_BUTT
} TDE2_COLORKEY_MODE_E;

/* the colorkey attributes of each color component */
typedef struct hiTDE2_COLORKEY_COMP_S
{
    HI_U8 u8CompMin;           /*Minimum colorkey of a component.*/
    HI_U8 u8CompMax;           /*Maximum colorkey of a component.*/
    HI_U8 bCompOut;            /*The colorkey of a component is within or out of the range.*/
    HI_U8 bCompIgnore;         /*Whether to ignore a component.*/
    HI_U8 u8CompMask;          /**<Component mask*/
    HI_U8 u8Reserved;
    HI_U8 u8Reserved1;
    HI_U8 u8Reserved2;
} TDE2_COLORKEY_COMP_S;

/**  the attributes of the colorkey */
typedef union hiTDE2_COLORKEY_U
{
    struct
    {
        TDE2_COLORKEY_COMP_S stAlpha;   
        TDE2_COLORKEY_COMP_S stRed;     
        TDE2_COLORKEY_COMP_S stGreen;   
        TDE2_COLORKEY_COMP_S stBlue;    
    } struCkARGB;
    struct
    {
        TDE2_COLORKEY_COMP_S stAlpha;   
        TDE2_COLORKEY_COMP_S stY;       
        TDE2_COLORKEY_COMP_S stCb;      
        TDE2_COLORKEY_COMP_S stCr;      
    } struCkYCbCr;
    struct
    {
        TDE2_COLORKEY_COMP_S stAlpha;   
        TDE2_COLORKEY_COMP_S stClut;    
    } struCkClut;
} TDE2_COLORKEY_U;

/* Type of the alpha output source */
typedef enum hiTDE2_OUTALPHA_FROM_E
{
    TDE2_OUTALPHA_FROM_NORM = 0,    /* from the result of the alpha 
                                            blending or anti-flicker operation */
    TDE2_OUTALPHA_FROM_BACKGROUND,  /* from background */
    TDE2_OUTALPHA_FROM_FOREGROUND,  /* from foreground */
    TDE2_OUTALPHA_FROM_GLOBALALPHA, /* from the global alpha */
    TDE2_OUTALPHA_FROM_BUTT
} TDE2_OUTALPHA_FROM_E;

/** filtering mode */
typedef enum hiTDE2_FILTER_MODE_E
{
    TDE2_FILTER_MODE_COLOR = 0, /**< Filters the color */
    TDE2_FILTER_MODE_ALPHA,     /**< Filters the alpha */
    TDE2_FILTER_MODE_BOTH,      /* both color and alpha will be filter */
    TDE2_FILTER_MODE_NONE,      /**<No filter *//**<CNcomment:$)A2;=xPPBK2( */
    TDE2_FILTER_MODE_BUTT
} TDE2_FILTER_MODE_E;

/* the anti-flicker configuration of a channel */
typedef enum hiTDE2_DEFLICKER_MODE_E
{
    TDE2_DEFLICKER_MODE_NONE = 0,   /**< No anti-flicker */
    TDE2_DEFLICKER_MODE_RGB,        /**< Anti-flicker on RGB component */
    TDE2_DEFLICKER_MODE_BOTH,       /**< Anti-flicker on alpha component */
    TDE2_DEFLICKER_MODE_BUTT
}TDE2_DEFLICKER_MODE_E;

/* blend mode */
typedef enum hiTDE2_BLEND_MODE_E
{
    TDE2_BLEND_ZERO = 0x0,
    TDE2_BLEND_ONE,
    TDE2_BLEND_SRC2COLOR,
    TDE2_BLEND_INVSRC2COLOR,
    TDE2_BLEND_SRC2ALPHA,
    TDE2_BLEND_INVSRC2ALPHA,
    TDE2_BLEND_SRC1COLOR,
    TDE2_BLEND_INVSRC1COLOR,
    TDE2_BLEND_SRC1ALPHA,
    TDE2_BLEND_INVSRC1ALPHA,
    TDE2_BLEND_SRC2ALPHASAT,
    TDE2_BLEND_BUTT
}TDE2_BLEND_MODE_E;

/** Defines the alpha blending command */
/* pixel = (source * fs + destination * fd),
   sa = source alpha,
   da = destination alpha */
typedef enum hiTDE2_BLENDCMD_E
{
    TDE2_BLENDCMD_NONE = 0x0,     /**< fs: sa      fd: 1.0-sa */
    TDE2_BLENDCMD_CLEAR,    /**< fs: 0.0     fd: 0.0 */
    TDE2_BLENDCMD_SRC,      /**< fs: 1.0     fd: 0.0 */
    TDE2_BLENDCMD_SRCOVER,  /**< fs: 1.0     fd: 1.0-sa */
    TDE2_BLENDCMD_DSTOVER,  /**< fs: 1.0-da  fd: 1.0 */
    TDE2_BLENDCMD_SRCIN,    /**< fs: da      fd: 0.0 */
    TDE2_BLENDCMD_DSTIN,    /**< fs: 0.0     fd: sa */
    TDE2_BLENDCMD_SRCOUT,   /**< fs: 1.0-da  fd: 0.0 */
    TDE2_BLENDCMD_DSTOUT,   /**< fs: 0.0     fd: 1.0-sa */
    TDE2_BLENDCMD_SRCATOP,  /**< fs: da      fd: 1.0-sa */
    TDE2_BLENDCMD_DSTATOP,  /**< fs: 1.0-da  fd: sa */
    TDE2_BLENDCMD_ADD,      /**< fs: 1.0     fd: 1.0 */
    TDE2_BLENDCMD_XOR,      /**< fs: 1.0-da  fd: 1.0-sa */
    TDE2_BLENDCMD_DST,      /**< fs: 0.0     fd: 1.0 */
    TDE2_BLENDCMD_CONFIG,   /**< User-defined configuration */
    TDE2_BLENDCMD_BUTT
}TDE2_BLENDCMD_E;

/* the options for alpha blending */
typedef struct hiTDE2_BLEND_OPT_S
{
    HI_BOOL  bGlobalAlphaEnable;        /**< Global alpha enable */
    HI_BOOL  bPixelAlphaEnable;         /**< Pixel alpha enable */
    HI_BOOL bSrc1AlphaPremulti;         /**< Src1 alpha premultiply enable */
    HI_BOOL bSrc2AlphaPremulti;         /**< Src2 alpha premultiply enable */
    TDE2_BLENDCMD_E eBlendCmd;          /**< Alpha blending command */    
    TDE2_BLEND_MODE_E eSrc1BlendMode;   /**< Src1 blending mode select. It is valid when eBlendCmd is set to TDE2_BLENDCMD_CONFIG */
    TDE2_BLEND_MODE_E eSrc2BlendMode;   /**< Src2 blending mode select. It is valid when eBlendCmd is set to TDE2_BLENDCMD_CONFIG */
}TDE2_BLEND_OPT_S;

/* CSC parameter options */
typedef struct hiTDE2_CSC_OPT_S
{
    HI_BOOL bICSCUserEnable;		/**User-defined ICSC parameter enable*/
    HI_BOOL bICSCParamReload;	/**User-defined ICSC parameter reload enable*/
    HI_BOOL bOCSCUserEnable;		/**User-defined OCSC parameter enable*/
    HI_BOOL bOCSCParamReload;	/**User-defined OCSC parameter reload enable*/
    HI_U32 u32ICSCParamAddr;		/**ICSC parameter address. The address must be 128-bit aligned.*/
    HI_U32 u32OCSCParamAddr;	/**OCSC parameter address. The address must be 128-bit aligned.*/
}TDE2_CSC_OPT_S;

/**  the attributes of a TDE operation?*/
typedef struct hiTDE2_OPT_S
{
    /* Logical operation type */
    TDE2_ALUCMD_E enAluCmd;

    /* ROP type of the color space */
    TDE2_ROP_CODE_E enRopCode_Color;

    /* ROP type of the alpha */
    TDE2_ROP_CODE_E enRopCode_Alpha;

    /* Colorkey mode */
    TDE2_COLORKEY_MODE_E enColorKeyMode;

    /* Colorkey value */
    TDE2_COLORKEY_U unColorKeyValue;

    /* Intra-area clip or entra-area clip */
    TDE2_CLIPMODE_E enClipMode;

    /* Definition of the clipped area */
    TDE2_RECT_S stClipRect;

    /* anti-flicker mode */
    TDE2_DEFLICKER_MODE_E enDeflickerMode;  

    /* Whether to scale */
    HI_BOOL bResize;

    /* Filtering mode for scaling or anti-flicker */
    TDE2_FILTER_MODE_E enFilterMode;

    /* Mirror type */
    TDE2_MIRROR_E enMirror;

    /* Whether to reload the CLUT */
    HI_BOOL bClutReload;

    /* Global alpha value */
    HI_U8   u8GlobalAlpha;

    /* Alpha output source */
    TDE2_OUTALPHA_FROM_E enOutAlphaFrom;
    
    HI_U32 u32Colorize;                     /**< Colorize value */

    TDE2_BLEND_OPT_S stBlendOpt;

    TDE2_CSC_OPT_S stCscOpt;	
} TDE2_OPT_S;


/** the attributes of the macroblock surface operation */
typedef struct hiTDE2_MBOPT_S
{
    /* Clip mode: intra-area clip or entra-area clip */
    TDE2_CLIPMODE_E enClipMode;

    /* Definition of the clipped area */
    TDE2_RECT_S stClipRect;

    /* Whether to perform anti-flicker */
    HI_BOOL bDeflicker;

    /* scaling mode */
    TDE2_MBRESIZE_E enResize;

    /* Whether the alpha value of the output result bitmap is specified by users */
    HI_BOOL bSetOutAlpha;
    HI_U8   u8OutAlpha;
} TDE2_MBOPT_S;

/**  the information about pattern filling */
typedef struct hiTDE2_PATTERN_FILL_OPT_S
{
    TDE2_ALUCMD_E enAluCmd;                 /**< Logical operation type */

    TDE2_ROP_CODE_E enRopCode_Color;        /**< ROP type of the color space */

    TDE2_ROP_CODE_E enRopCode_Alpha;        /**< ROP type of the alpha */

    TDE2_COLORKEY_MODE_E enColorKeyMode;    /**< Colorkey mode*/

    TDE2_COLORKEY_U unColorKeyValue;        /**< Colorkey value*/

    TDE2_CLIPMODE_E enClipMode;             /**< Clip mode*/

    TDE2_RECT_S stClipRect;                 /**< Clipped area*/

    HI_BOOL bClutReload;                    /**< Whether to reload the CLUT*/

    HI_U8 u8GlobalAlpha;                    /**< Global alpha*/

    TDE2_OUTALPHA_FROM_E enOutAlphaFrom;    /**< Alpha output source*/

    HI_U32 u32Colorize;                     /* Colorize value*/

    TDE2_BLEND_OPT_S stBlendOpt;            /* Blending option */

    TDE2_CSC_OPT_S stCscOpt;		/* CSC parameter option */
    
}TDE2_PATTERN_FILL_OPT_S;



/** the anti-flicker level */
typedef enum hiTDE_DEFLICKER_LEVEL_E
{
    TDE_DEFLICKER_AUTO = 0, /**< *<The anti-flicker coefficient is selected by the TDE*/
    TDE_DEFLICKER_LOW,      /**< *Low-level anti-flicker*/
    TDE_DEFLICKER_MIDDLE,   /**< *Medium-level anti-flicker*/
    TDE_DEFLICKER_HIGH,     /**< *Intermediate-level anti-flicker*/
    TDE_DEFLICKER_BUTT
}TDE_DEFLICKER_LEVEL_E;

/* composed surface info */
typedef struct hiTDE_COMPOSOR_S
{
    TDE2_SURFACE_S stSrcSurface;
    TDE2_RECT_S stInRect;
    TDE2_RECT_S stOutRect;
    TDE2_OPT_S stOpt;
}TDE_COMPOSOR_S;

/* composed surface list */
typedef struct hiTDE_SURFACE_LIST_S
{
	HI_U32 u32SurfaceNum;
	TDE2_SURFACE_S *pDstSurface;
	TDE_COMPOSOR_S *pstComposor;
}TDE_SURFACE_LIST_S;
#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */


#endif /* End of #ifndef __TDE_TYPE_H__ */


