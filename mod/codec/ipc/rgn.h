#ifndef __rgn_h__
#define __rgn_h__

#include "codec.h"
#include "fw/libfont/inc/gsf_font_draw.h"
#include "fw/libfont/inc/gsf_character_conv.h"


#if defined(GSF_CPU_3531d) || defined(GSF_CPU_3403)

#define rgn_type        rgn_obj[handle].rgn.stRegion.type
#define rgn_attr_fmt    rgn_obj[handle].rgn.stRegion.attr.overlay.pixel_format
#define rgn_attr_bg     rgn_obj[handle].rgn.stRegion.attr.overlay.bg_color
#define rgn_attr_w      rgn_obj[handle].rgn.stRegion.attr.overlay.size.width
#define rgn_attr_h      rgn_obj[handle].rgn.stRegion.attr.overlay.size.height
#define rgn_attr_canvas rgn_obj[handle].rgn.stRegion.attr.overlay.canvas_num

#define rgn_ch_mod      rgn_obj[handle].rgn.stChn.mod_id
#define rgn_ch_dev      rgn_obj[handle].rgn.stChn.dev_id
#define rgn_ch_chn      rgn_obj[handle].rgn.stChn.chn_id

#define rgn_chattr_show rgn_obj[handle].rgn.stChnAttr.is_show
#define rgn_chattr_type rgn_obj[handle].rgn.stChnAttr.type

#define rgn_chattr_over_x rgn_obj[handle].rgn.stChnAttr.attr.overlay_chn.point.x
#define rgn_chattr_over_y rgn_obj[handle].rgn.stChnAttr.attr.overlay_chn.point.y
#define rgn_chattr_over_layer rgn_obj[handle].rgn.stChnAttr.attr.overlay_chn.layer
#define rgn_chattr_over_bgalpha rgn_obj[handle].rgn.stChnAttr.attr.overlay_chn.bg_alpha
#define rgn_chattr_over_fgalpha rgn_obj[handle].rgn.stChnAttr.attr.overlay_chn.fg_alpha

#define rgn_chattr_cover_type    rgn_obj[handle].rgn.stChnAttr.attr.cover_chn.cover.type
#define rgn_chattr_cover_x    rgn_obj[handle].rgn.stChnAttr.attr.cover_chn.cover.rect.x
#define rgn_chattr_cover_y    rgn_obj[handle].rgn.stChnAttr.attr.cover_chn.cover.rect.y
#define rgn_chattr_cover_w    rgn_obj[handle].rgn.stChnAttr.attr.cover_chn.cover.rect.width
#define rgn_chattr_cover_h    rgn_obj[handle].rgn.stChnAttr.attr.cover_chn.cover.rect.height
#define rgn_chattr_cover_layer    rgn_obj[handle].rgn.stChnAttr.attr.cover_chn.layer
#define rgn_chattr_cover_color    rgn_obj[handle].rgn.stChnAttr.attr.cover_chn.cover.color

#define bitmap_w    bitMap.width
#define bitmap_h    bitMap.height
#define bitmap_fmt  bitMap.pixel_format  
#define bitmap_data bitMap.data
#define pbitmap_w    bitMap->width
#define pbitmap_h    bitMap->height
#define pbitmap_fmt  bitMap->pixel_format  
#define pbitmap_data bitMap->data

#define rect_w rect.width
#define rect_h rect.height

#else

#define rgn_type        rgn_obj[handle].rgn.stRegion.enType
#define rgn_attr_fmt    rgn_obj[handle].rgn.stRegion.unAttr.stOverlay.enPixelFmt
#define rgn_attr_bg     rgn_obj[handle].rgn.stRegion.unAttr.stOverlay.u32BgColor
#define rgn_attr_w      rgn_obj[handle].rgn.stRegion.unAttr.stOverlay.stSize.u32Width
#define rgn_attr_h      rgn_obj[handle].rgn.stRegion.unAttr.stOverlay.stSize.u32Height
#define rgn_attr_canvas rgn_obj[handle].rgn.stRegion.unAttr.stOverlay.u32CanvasNum

#define rgn_ch_mod      rgn_obj[handle].rgn.stChn.enModId
#define rgn_ch_dev      rgn_obj[handle].rgn.stChn.s32DevId
#define rgn_ch_chn      rgn_obj[handle].rgn.stChn.s32ChnId

#define rgn_chattr_show rgn_obj[handle].rgn.stChnAttr.bShow
#define rgn_chattr_type rgn_obj[handle].rgn.stChnAttr.enType

#define rgn_chattr_over_x rgn_obj[handle].rgn.stChnAttr.unChnAttr.stOverlayChn.stPoint.s32X
#define rgn_chattr_over_y rgn_obj[handle].rgn.stChnAttr.unChnAttr.stOverlayChn.stPoint.s32Y
#define rgn_chattr_over_layer rgn_obj[handle].rgn.stChnAttr.unChnAttr.stOverlayChn.u32Layer
#define rgn_chattr_over_bgalpha rgn_obj[handle].rgn.stChnAttr.unChnAttr.stOverlayChn.u32BgAlpha
#define rgn_chattr_over_fgalpha rgn_obj[handle].rgn.stChnAttr.unChnAttr.stOverlayChn.u32FgAlpha

#define rgn_chattr_cover_type    rgn_obj[handle].rgn.stChnAttr.unChnAttr.stCoverChn.enCoverType
#define rgn_chattr_cover_x    rgn_obj[handle].rgn.stChnAttr.unChnAttr.stCoverChn.stRect.s32X
#define rgn_chattr_cover_y    rgn_obj[handle].rgn.stChnAttr.unChnAttr.stCoverChn.stRect.s32Y
#define rgn_chattr_cover_w    rgn_obj[handle].rgn.stChnAttr.unChnAttr.stCoverChn.stRect.u32Width
#define rgn_chattr_cover_h    rgn_obj[handle].rgn.stChnAttr.unChnAttr.stCoverChn.stRect.u32Height
#define rgn_chattr_cover_layer    rgn_obj[handle].rgn.stChnAttr.unChnAttr.stCoverChn.u32Layer
#define rgn_chattr_cover_color    rgn_obj[handle].rgn.stChnAttr.unChnAttr.stCoverChn.u32Color

#define bitmap_w    bitMap.u32Width
#define bitmap_h    bitMap.u32Height
#define bitmap_fmt  bitMap.enPixelFormat  
#define bitmap_data bitMap.pData
#define pbitmap_w    bitMap->u32Width
#define pbitmap_h    bitMap->u32Height
#define pbitmap_fmt  bitMap->enPixelFormat  
#define pbitmap_data bitMap->pData

#define rect_w rect.u32Width
#define rect_h rect.u32Height

#endif


typedef struct {
  int ch_num;
  int st_num;
}gsf_rgn_ini_t;

typedef struct {
  int w, h;
  int size;
  struct{
    char label[32];
    int  rect[4];  
  }box[64];
}gsf_rgn_rects_t;


int gsf_rgn_init(gsf_rgn_ini_t *ini);

int gsf_rgn_osd_set(int ch, int idx, gsf_osd_t *osd);
int gsf_rgn_rect_set(int ch, int idx, gsf_rgn_rects_t *rects, int mask);
int gsf_rgn_vmask_set(int ch, int idx, gsf_vmask_t *vmask);
int gsf_rgn_canvas(int ch, int idx, gsf_osd_act_t *act);

#endif // __rgn_h__