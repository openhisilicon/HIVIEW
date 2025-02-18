#include "mpp.h"
#include "rgn.h"
#include "cfg.h"

static gsf_rgn_ini_t rgn_ini = {.ch_num = 1, .st_num = 2};

int gsf_rgn_init(gsf_rgn_ini_t *ini)
{
  if(ini)
    rgn_ini = *ini;
  return 0;
}

typedef struct {
    char  lines[16][128];//osd叠加行信息
  	int   lineN;        //osd行数
  	int   colN;         //osd列数
    int   fontW;        //位图字体宽度
    int   fontH;        //位图字体高度
    int   fontS;        //位图字体间隔  
    int   osdW;         //位图宽度
    int   osdH;         //位图高度    
}gsf_rgn_osd_t;

typedef struct {
  int cnt;
  struct {
    //HI_U16 *a;int w;int h;  
    int a;int w;int h;  
  }box[128];
}box_t;

enum {
  GSF_RGN_OBJ_NONE   = 0,
  GSF_RGN_OBJ_CREATE = 1,
  GSF_RGN_OBJ_ATTACH = 2,
};
typedef struct {
  int stat;
  gsf_mpp_rgn_t rgn;
  gsf_rgn_osd_t *osd_info;
  char          *osd_bmp;
  box_t         box_tmp;
}gsf_rgn_obj_t;

enum {
  OBJ_OSD   = 0,
  OBJ_VMASK = 1,
};

/****** ch i *******
* strame0 osd   8   on venc
* stream1 osd   8   on venc
* streamN osd   8   on venc
* stremaAll vmask 8 on vpss
********************/

#define GSF_RGN_OBJ_HANDLE(ch, type, st, idx) ((ch)*(8*GSF_CODEC_VENC_NUM+8*1) + (type)*(8*GSF_CODEC_VENC_NUM) + (st)*(8) + idx)
static gsf_rgn_obj_t rgn_obj[(GSF_CODEC_IPC_CHN)*(8*GSF_CODEC_VENC_NUM/*osd*/+8*1/*vmask*/)];

int utf8_byte_num(unsigned char firstCh)
{
	unsigned char  temp = 0x80;
	int num = 0;
	while (temp & firstCh)
	{
		num++;
		temp = (temp >> 1);
	}
	return num;
}

int gsf_parse_text(char *text, char lines[16][128], int *lineN, int *colN)
{
	int nIndex = 0, nMaxLen = 0, nLen = 0, nLinePos = 0;

  int u8LineNum = 1;

	while(nIndex < 128 
		  && text[nIndex] != '\0'
		  && u8LineNum <= 16)
	{
		if (text[nIndex] == '\n')
		{
	    memset(lines[u8LineNum-1], 0, 128);
	    strncpy(lines[u8LineNum-1], &text[nLinePos], nIndex - nLinePos);
			if ((++u8LineNum) > 16) //行数超出后不再检测
				break;
			else 
				nLinePos = nIndex + 1;  //标记下一行开始位置信息
							
			nMaxLen = (nLen > nMaxLen) ? nLen : nMaxLen;
			nLen = 0;

			++nIndex;
		}
		else
		{
			int nByteNum = utf8_byte_num((unsigned char)text[nIndex]);
			nIndex += ((nByteNum >= 1 && nByteNum <= 6) ? nByteNum : 1);
			++nLen;
		}
	}
	
	if (u8LineNum <= 16)
	{
    memset(lines[u8LineNum-1], 0, 128);
		strncpy(lines[u8LineNum-1], &text[nLinePos], nIndex - nLinePos);
		nMaxLen = (nLen > nMaxLen) ? nLen : nMaxLen;
	}
	else
	{
		u8LineNum = 16;
	}
	
	*lineN = u8LineNum;
	*colN = nMaxLen;

	return 0;	
}

int gsf_calc_fontsize(int nVencWidth, int nVencHeight, int u8FontSize, int *pu8FontW, int *pu8FontH, int *pFontS)
{
	int u8FontWTmp = 0, u8FontHTmp = 0, FontSTmp = 0;
	
	u8FontSize = (u8FontSize > 2) ? 2 : u8FontSize;
 	
 	if (nVencWidth >= 2592)
 	{
	  u8FontWTmp = 48+u8FontSize*16;
	}
 	else if (nVencWidth >= 2048)
 	{
	  u8FontWTmp = 32+u8FontSize*16;
  }	
 	else if (nVencWidth >= 1920)
 	{
  	if (u8FontSize == 0)
  		u8FontWTmp = 24;
  	else if (u8FontSize == 1)
  		u8FontWTmp = 32;
  	else
  		u8FontWTmp = 48;
	}
  else if (nVencWidth >= 1280)
  {
  	if (u8FontSize == 0)
  		u8FontWTmp = 16;
  	else if (u8FontSize == 1)
  		u8FontWTmp = 24;
  	else
  		u8FontWTmp = 32;
  }	
	else if (nVencWidth >= 704)
	{
		if (u8FontSize == 0)
			u8FontWTmp = 16;
		else if (u8FontSize == 1)
			u8FontWTmp = 24;
		else
			u8FontWTmp = 32;
	}
	else if (nVencWidth >= 352)
	{
		if (u8FontSize == 0)
			u8FontWTmp = 12;
		else if (u8FontSize == 1)
			u8FontWTmp = 16;
		else
			u8FontWTmp = 24;
	}	
	else
	{
		if (u8FontSize == 0)
			u8FontWTmp = 8;
		else if (u8FontSize == 1)
			u8FontWTmp = 12;
		else
			u8FontWTmp = 16;
	}

	u8FontHTmp = u8FontWTmp* 3 / 2; // 20141110

 	if (NULL != pu8FontW)
 		(*pu8FontW) = u8FontWTmp;

 	if (NULL != pu8FontH)
 		(*pu8FontH) = u8FontHTmp;

 	if (NULL != pFontS)
 		(*pFontS) = FontSTmp; 	
 		
	return 0;
}

static int gsf_bitmap_make_edge(BITMAP_S* bitMap)
{
  int i, j;
  
  //assert(pbitmap_fmt == PIXEL_FORMAT_RGB_1555);
  
	//level scan;
	char *lbuf = (char*)calloc(1, (pbitmap_w<<1)*pbitmap_h);
	memcpy(lbuf, (char*)pbitmap_data, (pbitmap_w<<1)*pbitmap_h);
	unsigned short *lpix = (unsigned short *)lbuf;
	
	for(i = 0; i < pbitmap_h; i++)
	{
	  for(j = 0; j < pbitmap_w-1; j++)
	  {
	    if(lpix[i*pbitmap_w+j] ^ lpix[i*pbitmap_w+j + 1])
	    {
	      if(lpix[i*pbitmap_w+j] == 0)
	      {
	        lpix[i*pbitmap_w+j] = 0x8000;
	      }
	      else
	      {
	        lpix[i*pbitmap_w+j+1] = 0x8000;
	        j++;        
	      }
	    }
	  }
	}

  //vertical scan;
	char *vbuf = (char*)calloc(1, (pbitmap_w<<1)*pbitmap_h);
	memcpy(vbuf, (char*)pbitmap_data, (pbitmap_w<<1)*pbitmap_h);
	unsigned short *vpix = (unsigned short *)vbuf;

	for(i = 0; i < pbitmap_w; i++)
	{
	  for(j = 0; j < pbitmap_h-1; j++)
	  {
	    if(vpix[j*pbitmap_w+i] ^ vpix[(j+1)*pbitmap_w+i])
	    {
	      if(vpix[j*pbitmap_w+i] == 0)
	      {
	        vpix[j*pbitmap_w+i] = 0x8000;
	      }
	      else
	      {
	        vpix[(j+1)*pbitmap_w+i] = 0x8000;
	        j++;
	      }
	    }
	  }
	}
	
	//mixture;
  unsigned short *pData = (unsigned short *)pbitmap_data;
  
  for(i = 0; i < pbitmap_h; i++)
	{
	  for(j = 0; j < pbitmap_w; j++)
	  {
	    pData[i*pbitmap_w+j] = lpix[i*pbitmap_w+j];
	    pData[i*pbitmap_w+j] |= vpix[i*pbitmap_w+j];
	  }
	}
	
	free(lpix);
	free(vpix);
}

static unsigned short argb8888_1555(unsigned int color)
{
  unsigned char a = (color >> 24) & 0xff;
	unsigned char r = (color >> 16) & 0xff;
	unsigned char g = (color >> 8) & 0xff;
	unsigned char b = color & 0xff;

  a = a?1:0;
	r >>= 3;
	g >>= 3;
	b >>= 3;

  return (unsigned short)(a << 15 | r<<(10) | g<<5 | b);
}

extern int venc_fixed_sdp(int ch, int st, gsf_sdp_t *sdp);
static int codec_venc_width(int ch, int i)
{
  gsf_sdp_t sdp;
  if(venc_fixed_sdp(ch, i, &sdp) == 0)
  {
     return sdp.venc.width;
  }
  return codec_ipc.venc[i].width;
}

static int codec_venc_height(int ch, int i)
{
  gsf_sdp_t sdp;
  if(venc_fixed_sdp(ch, i, &sdp) == 0)
  {
    return sdp.venc.height;
  }
  return codec_ipc.venc[i].height;
}

//#define __RGN_CANVAS // __RGN_CANVAS is bad

int gsf_rgn_osd_set(int ch, int idx, gsf_osd_t *_osd)
{
  
  int i = 0;
  unsigned int ARG1555_RED = argb8888_1555(0x01FF0000);
  gsf_osd_t __osd = *_osd;
  gsf_osd_t *osd = &__osd;
  
  for(i = 0; i < GSF_CODEC_VENC_NUM; i++)
  {
    
    if(!codec_ipc.venc[i].en)
      continue;
    
    int handle = GSF_RGN_OBJ_HANDLE(ch, OBJ_OSD, i, idx);
    
    memset(&rgn_obj[handle].rgn, 0, sizeof(rgn_obj[handle].rgn));
    
    
    float wr = codec_venc_width(ch, i);
    wr /=  codec_venc_width(ch, 0);
    float hr = codec_venc_height(ch, i);
    hr /= codec_venc_height(ch, 0);
    
    //printf("ch:%d, idx:%d, en:%d, point[%d,%d], wr:%0.4f, hr:%0.4f\n", ch, idx, osd->en, osd->point[0], osd->point[1], wr, hr);
    
    osd->point[0] *= wr;
    osd->point[1] *= hr;
    
    osd->wh[0] *= wr;
    osd->wh[1] *= hr;
    
    
    rgn_type = OVERLAY_RGN;
    rgn_attr_fmt = PIXEL_FORMAT_ARGB_1555;
    rgn_attr_bg = 0x00000000;//ARG1555_RED;
    rgn_attr_w = 2;
    rgn_attr_h = 2;
    rgn_attr_canvas = 2;
    
    rgn_ch_mod = HI_ID_VENC;
    rgn_ch_dev = 0;
    rgn_ch_chn = ch*GSF_CODEC_VENC_NUM+i;
    //printf("handle:%d, ch:%d, i:%d, s32ChnId:%d\n", handle, ch, i, rgn_ch_chn);
    rgn_chattr_show = osd->en;//HI_TRUE;
    rgn_chattr_type = OVERLAY_RGN;
    rgn_chattr_over_x = ALIGN_UP(osd->point[0], 2);
    rgn_chattr_over_y = ALIGN_UP(osd->point[1], 2);
    rgn_chattr_over_layer   = idx;
    rgn_chattr_over_bgalpha = 0;//(osd->wh[0] && osd->wh[1])?80:0;//0;
    rgn_chattr_over_fgalpha = FG_ALPHA_MAX;//128;
        
    if(rgn_obj[handle].osd_info == NULL)
    {
      rgn_obj[handle].osd_info = (gsf_rgn_osd_t*)calloc(1, sizeof(gsf_rgn_osd_t));
    }

    gsf_rgn_osd_t *info = rgn_obj[handle].osd_info;
    
    int _osdW = info->osdW;
    int _osdH = info->osdH;
    
    if(osd->text[0])
    {
      gsf_parse_text(osd->text, info->lines, &info->lineN, &info->colN);
      gsf_calc_fontsize(codec_venc_width(ch, i), codec_venc_height(ch, i), osd->fontsize, &info->fontW, &info->fontH, &info->fontS);
    }
    else
    {
      info->fontW = info->fontH = info->fontS = info->lineN = info->colN = 0;
    }
    
    info->osdW = (info->fontW + info->fontS)*info->colN - info->fontS+1;
    info->osdH = info->fontH * info->lineN + 1;
      
    if(osd->wh[0] && osd->wh[1])
    {
       info->osdW = (osd->wh[0] > info->osdW)?ALIGN_UP(osd->wh[0], 2):info->osdW;
       info->osdH = (osd->wh[1] > info->osdH)?ALIGN_UP(osd->wh[1], 2):info->osdH;
    }
    
    rgn_attr_w = ALIGN_UP(info->osdW, 2);
    rgn_attr_h = ALIGN_UP(info->osdH, 2);
    
    //check;
    if(rgn_obj[handle].stat >= GSF_RGN_OBJ_ATTACH)
    {
      if(_osdW != info->osdW || _osdH != info->osdH)
      {  
         gsf_mpp_rgn_ctl(handle, GSF_MPP_RGN_DETACH, &rgn_obj[handle].rgn);
         rgn_obj[handle].stat = GSF_RGN_OBJ_CREATE;
      }
    }

    //create;
    if(rgn_obj[handle].stat < GSF_RGN_OBJ_CREATE)
    {
       gsf_mpp_rgn_ctl(handle, GSF_MPP_RGN_CREATE, &rgn_obj[handle].rgn);
       rgn_obj[handle].stat = GSF_RGN_OBJ_CREATE;
    }
    
    //set attr; 
    if(rgn_obj[handle].osd_bmp == NULL)
    {
      #ifdef __RGN_CANVAS
      rgn_obj[handle].osd_bmp = (char*)1;
      #else
      rgn_obj[handle].osd_bmp = malloc(info->osdW*info->osdH*2);
      #endif
      gsf_mpp_rgn_ctl(handle, GSF_MPP_RGN_SETATTR, &rgn_obj[handle].rgn);
    }
    else if(_osdW != info->osdW || _osdH != info->osdH)
    {
      #ifdef __RGN_CANVAS
      rgn_obj[handle].osd_bmp = (char*)1;
      #else
      rgn_obj[handle].osd_bmp = realloc(rgn_obj[handle].osd_bmp, info->osdW*info->osdH*2);
      #endif
      gsf_mpp_rgn_ctl(handle, GSF_MPP_RGN_SETATTR, &rgn_obj[handle].rgn);
    }

    //attach;
    if(rgn_obj[handle].stat < GSF_RGN_OBJ_ATTACH)
    {
      //printf("GSF_MPP_RGN_ATTACH ch:%d, i:%d, idx:%d\n", ch, i, idx);
      gsf_mpp_rgn_ctl(handle, GSF_MPP_RGN_ATTACH, &rgn_obj[handle].rgn);
      rgn_obj[handle].stat = GSF_RGN_OBJ_ATTACH;
    }
    
    if((info->lineN == 0) && osd->wh[0] == 0 && osd->wh[1] == 0)
    {
      return 0;
    }
    
    // bitmap;
    BITMAP_S bitMap;
    
    #ifdef __RGN_CANVAS
      RGN_CANVAS_INFO_S stRgnCanvasInfo = {0};
      if((gsf_mpp_rgn_canvas_get(handle, &stRgnCanvasInfo) < 0)
        || (stRgnCanvasInfo.stSize.u32Width == 0)
        || (stRgnCanvasInfo.stSize.u32Height == 0))
      {
        printf("gsf_mpp_rgn_canvas_get err, ch:%d, i:%d, idx:%d\n", ch, i, idx);
        return -1;
      }
      HI_U16 *p = bitmap_data = (HI_VOID*)(HI_UL)stRgnCanvasInfo.u64VirtAddr;
      memset(p, 0, stRgnCanvasInfo.stSize.u32Height*stRgnCanvasInfo.u32Stride);
      bitmap_w	    = stRgnCanvasInfo.u32Stride/2;
      bitmap_h	  = info->osdH;
      bitmap_fmt= PIXEL_FORMAT_ARGB_1555;
    #else
      bitmap_w	    = info->osdW;
    	bitmap_h	  = info->osdH;
    	bitmap_fmt= PIXEL_FORMAT_ARGB_1555;
    	bitmap_data        = rgn_obj[handle].osd_bmp;
    	memset(rgn_obj[handle].osd_bmp, 0, info->osdW*info->osdH*2);
    #endif
    
    //bitmap;
    {
      	// draw string;
      	int l = 0;
      	for (l = 0; l < info->lineN; l++)
      	{
      	    gsf_font_utf8_draw_line(info->fontW,
      	          bitmap_w,
                  info->fontS,
                  bitmap_data + l * bitmap_w * info->fontH*2, //maohw
          		    info->lines[l], "",
          		    (osd->wh[0] && osd->wh[1])?ARG1555_RED:0xffff, 0x0000, 0xffff, 0x0000);
      	}
      	// draw rect;
      	if((osd->wh[0] && osd->wh[1]))
      	{
          HI_U16 *p = (HI_U16*)bitmap_data; //+ (info->lineN*info->fontH)*(bitmap_w*2);
          HI_U16 boxH = info->osdH; //- (info->lineN*info->fontH);
          HI_U16 boxW = info->osdW;
          
        	int j = 0;
          for(j = 0; j < boxW; j++)
          {
            p[0*bitmap_w + j]
            = p[1*bitmap_w + j]  
            = p[2*bitmap_w + j] 
            = p[3*bitmap_w + j] 
            = p[(boxH-1)*bitmap_w + j]
            = p[(boxH-2)*bitmap_w + j]
            = p[(boxH-3)*bitmap_w + j] 
            = p[(boxH-4)*bitmap_w + j]
            = ARG1555_RED;
          }
          for(j = 0; j < boxH; j++)
          {
            p[j*bitmap_w + 0]
            = p[j*bitmap_w + 1]
            = p[j*bitmap_w + 2]
            = p[j*bitmap_w + 3]
            = p[j*bitmap_w + boxW-1] 
            = p[j*bitmap_w + boxW-2]
            = p[j*bitmap_w + boxW-3] 
            = p[j*bitmap_w + boxW-4]
            = ARG1555_RED;
          }
      	}
    }
    
    //printf("lineN:%d, wh:%d,%d\n", info->lineN, osd->wh[0], osd->wh[1]);
  	if(info->lineN && osd->wh[0] == 0 && osd->wh[1] == 0)
  	  gsf_bitmap_make_edge(&bitMap);

    #ifdef __RGN_CANVAS
    gsf_mpp_rgn_canvas_update(handle);
    #else
    gsf_mpp_rgn_bitmap(handle, &bitMap);
    gsf_mpp_rgn_ctl(handle, GSF_MPP_RGN_SETDISPLAY, &rgn_obj[handle].rgn);
    #endif
  }
  
  return 0;
}

int gsf_rgn_vmask_set(int ch, int idx, gsf_vmask_t *vmask)
{
  int i = 0;
  for(i = 0; i < 1; i++)
  {
    int handle = GSF_RGN_OBJ_HANDLE(ch, OBJ_VMASK, i, idx);
    
    memset(&rgn_obj[handle].rgn, 0, sizeof(rgn_obj[handle].rgn));
    
    rgn_type = COVER_RGN;
    
    rgn_ch_mod = HI_ID_VPSS;
    rgn_ch_dev = ch;
    rgn_ch_chn = 0;
    //printf("handle:%d, ch:%d, i:%d, s32ChnId:%d\n", handle, ch, i, rgn_ch_chn);
    rgn_chattr_show = vmask->en;//HI_TRUE;
    rgn_chattr_type = COVER_RGN;
    rgn_chattr_cover_type = AREA_RECT;
    rgn_chattr_cover_x = ALIGN_UP(vmask->rect[0], 2);
    rgn_chattr_cover_y = ALIGN_UP(vmask->rect[1], 2);
    rgn_chattr_cover_w  = ALIGN_UP(vmask->rect[2], 2)?:2;
    rgn_chattr_cover_h = ALIGN_UP(vmask->rect[3], 2)?:2;
    rgn_chattr_cover_layer         = idx;
    rgn_chattr_cover_color         = vmask->color;//0x0000ffff;
    //rgn_obj[handle].rgn.stChnAttr.unChnAttr.stCoverChn.enCoordinate     = RGN_ABS_COOR;
    
    //create;
    if(rgn_obj[handle].stat < GSF_RGN_OBJ_CREATE)
    {
       gsf_mpp_rgn_ctl(handle, GSF_MPP_RGN_CREATE, &rgn_obj[handle].rgn);
       rgn_obj[handle].stat = GSF_RGN_OBJ_CREATE;
    }
    //attach;
    if(rgn_obj[handle].stat < GSF_RGN_OBJ_ATTACH)
    {
      gsf_mpp_rgn_ctl(handle, GSF_MPP_RGN_ATTACH, &rgn_obj[handle].rgn);
      rgn_obj[handle].stat = GSF_RGN_OBJ_ATTACH;
    }
    //display;
    gsf_mpp_rgn_ctl(handle, GSF_MPP_RGN_SETDISPLAY, &rgn_obj[handle].rgn);
    
  }
  
  return 0;
}


int gsf_rgn_rect_set(int ch, int idx, gsf_rgn_rects_t *rects, int mask)
{
  int i = 0, r = 0, b = 0;
  unsigned int ARG1555_RED = argb8888_1555(0x01FF0000);
   
  for(i = 0; i < GSF_CODEC_VENC_NUM; i++)
  {
    if(!codec_ipc.venc[i].en)
      continue;
    if(!((1<<i) & mask))
      continue;
    
    int handle = GSF_RGN_OBJ_HANDLE(ch, OBJ_OSD, i, idx);
    
    memset(&rgn_obj[handle].rgn, 0, sizeof(rgn_obj[handle].rgn));
    
    rgn_type = OVERLAY_RGN;
    rgn_attr_fmt = PIXEL_FORMAT_ARGB_1555;
    rgn_attr_bg = 0x00000000;//argb8888_1555(0x00FF0000);
    rgn_attr_w = ALIGN_UP(codec_venc_width(ch, i), 2);
    rgn_attr_h = ALIGN_UP(codec_venc_height(ch, i), 2);
    rgn_attr_canvas = 2;
    
    rgn_ch_mod = HI_ID_VENC;
    rgn_ch_dev = 0;
    rgn_ch_chn = ch*GSF_CODEC_VENC_NUM+i;
    rgn_chattr_show = HI_TRUE;
    rgn_chattr_type = OVERLAY_RGN;
    rgn_chattr_over_x = 0;
    rgn_chattr_over_y = 0;
    rgn_chattr_over_layer   = idx;
    rgn_chattr_over_bgalpha = 0;//80;
    rgn_chattr_over_fgalpha = FG_ALPHA_MAX;//128;
    
    if(rgn_obj[handle].osd_info == NULL)
    {
      rgn_obj[handle].osd_info = (gsf_rgn_osd_t*)calloc(1, sizeof(gsf_rgn_osd_t));
    }

    gsf_rgn_osd_t *info = rgn_obj[handle].osd_info;

    int _osdW = info->osdW;
    int _osdH = info->osdH;


    info->osdW = rgn_attr_w;
    info->osdH = rgn_attr_h;    
    
    //printf("_osdW:%d, osdW:%d, _osdH:%d, osdH:%d\n", _osdW, info->osdW, _osdH, info->osdH);
    
    //check;
    if(rgn_obj[handle].stat >= GSF_RGN_OBJ_ATTACH)
    {
      if(_osdW != info->osdW || _osdH != info->osdH)
      {  
         gsf_mpp_rgn_ctl(handle, GSF_MPP_RGN_DETACH, &rgn_obj[handle].rgn);
         rgn_obj[handle].stat = GSF_RGN_OBJ_CREATE;
      }
    }

    //create;
    if(rgn_obj[handle].stat < GSF_RGN_OBJ_CREATE)
    {
       gsf_mpp_rgn_ctl(handle, GSF_MPP_RGN_CREATE, &rgn_obj[handle].rgn);
       rgn_obj[handle].stat = GSF_RGN_OBJ_CREATE;
    }
    
    //set attr; 
    if(rgn_obj[handle].osd_bmp == NULL)
    {
      #ifdef __RGN_CANVAS
      rgn_obj[handle].osd_bmp = (char*)1;
      #else
      rgn_obj[handle].osd_bmp = malloc(info->osdW*info->osdH*2);
      memset(rgn_obj[handle].osd_bmp, 0, info->osdW*info->osdH*2);
      #endif
      gsf_mpp_rgn_ctl(handle, GSF_MPP_RGN_SETATTR, &rgn_obj[handle].rgn);
    }
    else if(_osdW != info->osdW || _osdH != info->osdH)
    {
      #ifdef __RGN_CANVAS
      rgn_obj[handle].osd_bmp = (char*)1;
      #else
      rgn_obj[handle].osd_bmp = realloc(rgn_obj[handle].osd_bmp, info->osdW*info->osdH*2);
      memset(rgn_obj[handle].osd_bmp, 0, info->osdW*info->osdH*2);
      #endif
      gsf_mpp_rgn_ctl(handle, GSF_MPP_RGN_SETATTR, &rgn_obj[handle].rgn);
    }
   
    //attach;
    if(rgn_obj[handle].stat < GSF_RGN_OBJ_ATTACH)
    {
      //printf("GSF_MPP_RGN_ATTACH ch:%d, i:%d, idx:%d\n", ch, i, idx);
      gsf_mpp_rgn_ctl(handle, GSF_MPP_RGN_ATTACH, &rgn_obj[handle].rgn);
      rgn_obj[handle].stat = GSF_RGN_OBJ_ATTACH;
    }
   
    BITMAP_S  bitMap;
    
    #ifdef __RGN_CANVAS
      RGN_CANVAS_INFO_S stRgnCanvasInfo = {0};
      if((gsf_mpp_rgn_canvas_get(handle, &stRgnCanvasInfo) < 0)
        || (stRgnCanvasInfo.stSize.u32Width == 0)
        || (stRgnCanvasInfo.stSize.u32Height == 0))
      {
        return -1;
      }
      bitmap_data = (HI_VOID*)(HI_UL)stRgnCanvasInfo.u64VirtAddr;
      bitmap_w	    = stRgnCanvasInfo.u32Stride/2;
      bitmap_h	  = info->osdH;
      bitmap_fmt= PIXEL_FORMAT_ARGB_1555;
      //memset(bitmap_data, 0, stRgnCanvasInfo.stSize.u32Height*stRgnCanvasInfo.u32Stride);
    #else
      bitmap_w	    = info->osdW;
    	bitmap_h	  = info->osdH;
    	bitmap_fmt= PIXEL_FORMAT_ARGB_1555;
    	bitmap_data        = rgn_obj[handle].osd_bmp;
    	//memset(rgn_obj[handle].osd_bmp, 0, info->osdW*info->osdH*2);
    #endif

    #if 0
    struct timespec ts1, ts2;
    clock_gettime(CLOCK_MONOTONIC, &ts1);
    #endif

    for(r = 0; r < rgn_obj[handle].box_tmp.cnt; r++)
  	{
      HI_U16 *p = (HI_U16*)((char*)bitmap_data + rgn_obj[handle].box_tmp.box[r].a);
      int w = rgn_obj[handle].box_tmp.box[r].w;
      int h = rgn_obj[handle].box_tmp.box[r].h;
      
    	int j = 0, k = 0;
    	for(k = 0; k < h; k++)
      for(j = 0; j < w; j++)
      {
        p[k*bitmap_w + j] = 0x00;
      }
  	}
    rgn_obj[handle].box_tmp.cnt = 0;
    
    for(r = 0; r < rects->size; r++)
    {
      int boxX = 0, boxY = 0, boxW = 0, boxH = 0;
      
      if(rects->box[r].label[0])
      {
        gsf_parse_text(rects->box[r].label, info->lines, &info->lineN, &info->colN);
        gsf_calc_fontsize(codec_venc_width(ch, i), codec_venc_height(ch, i), 0/*fontsize*/, &info->fontW, &info->fontH, &info->fontS);
      }
      else
      {
         info->fontW = info->fontH = info->fontS = info->lineN = info->colN = 0;
      }
      
      if(rects->box[r].rect[2] && rects->box[r].rect[3])
      {
        
        float wr = info->osdW;
        wr /= rects->w;
        float hr = info->osdH;
        hr /= rects->h;

        boxX = (unsigned int)(rects->box[r].rect[0] * wr)&(-1);
        boxY = (unsigned int)(rects->box[r].rect[1] * hr)&(-1);
        
        int _boxW = (unsigned int)(rects->box[r].rect[2] * wr)&(-1);
        int _boxH = (unsigned int)(rects->box[r].rect[3] * hr)&(-1); 
       
        boxW = (_boxW > boxW)?ALIGN_UP(_boxW, 2):boxW;
        boxH = (_boxH > boxH)?ALIGN_UP(_boxH, 2):boxH;
        #if 0  
        printf("r:%d, rect[%d,%d,%d,%d], box[%d,%d,%d,%d]\n"
              , r
              , rects->box[r].rect[0], rects->box[r].rect[1], rects->box[r].rect[2], rects->box[r].rect[3]
              , boxX, boxY, boxW, boxH);
        #endif
      }
      
      if((info->lineN == 0) && boxW == 0 && boxH == 0)
      {
        continue;
      }

      // bitmap;
      {
          char *osd_bmp = bitmap_data + boxX*2 + boxY*(bitmap_w*2);
        	
          // draw string;
          int labelW = (info->fontW + info->fontS)*info->colN - info->fontS+1;
          int labelH = info->fontH * info->lineN + 1;
          if((boxX + labelW <= info->osdW) && (boxY + labelH <= info->osdH))
        	{
        	  // add white background;
          	HI_U16 *p = (HI_U16*)(osd_bmp);
            int j = 0, k = 0;
          	for(k = 0; k < labelH; k++)
            for(j = 0; j < labelW; j++)
            {
              p[k*bitmap_w + j] = 0xffff; //white
            }
            
          	int l = 0;
          	for (l = 0; l < info->lineN; l++)
          	{
          	    gsf_font_utf8_draw_line(info->fontW,
          	          bitmap_w,
                      info->fontS,
                      osd_bmp + l * bitmap_w * info->fontH*2,
              		    info->lines[l], "",
              		    (boxW && boxH)?ARG1555_RED:0xffff, 0x0000, 0xffff, 0x0000);
          	}
          	rgn_obj[handle].box_tmp.box[rgn_obj[handle].box_tmp.cnt].a = (int)((char*)osd_bmp - (char*)bitmap_data);
          	rgn_obj[handle].box_tmp.box[rgn_obj[handle].box_tmp.cnt].w = labelW;
          	rgn_obj[handle].box_tmp.box[rgn_obj[handle].box_tmp.cnt].h = labelH;
          	rgn_obj[handle].box_tmp.cnt++;
        	}
        	
        	// draw rect;
        	if(boxW > 4 && boxH > 4)
        	{
            HI_U16 *p = (HI_U16*)osd_bmp;
            boxW = (boxX + boxW > info->osdW)?info->osdW-boxX:boxW;
            boxH = (boxY + boxH > info->osdH)?info->osdH-boxY:boxH;
            
          	int j = 0;
            for(j = 0; j < boxW; j++)
            {
              p[0*bitmap_w + j]
              = p[1*bitmap_w + j]  
              = p[2*bitmap_w + j] 
              = p[3*bitmap_w + j] 
              = p[(boxH-1)*bitmap_w + j]
              = p[(boxH-2)*bitmap_w + j]
              = p[(boxH-3)*bitmap_w + j] 
              = p[(boxH-4)*bitmap_w + j]
              = ARG1555_RED;
            }
            
            rgn_obj[handle].box_tmp.box[rgn_obj[handle].box_tmp.cnt].a = (int)((char*)p - (char*)bitmap_data);
          	rgn_obj[handle].box_tmp.box[rgn_obj[handle].box_tmp.cnt].w = boxW;
          	rgn_obj[handle].box_tmp.box[rgn_obj[handle].box_tmp.cnt].h = 4;
            rgn_obj[handle].box_tmp.cnt++;
            rgn_obj[handle].box_tmp.box[rgn_obj[handle].box_tmp.cnt].a = (int)((char*)(p+(boxH-4)*bitmap_w) - (char*)bitmap_data);
          	rgn_obj[handle].box_tmp.box[rgn_obj[handle].box_tmp.cnt].w = boxW;
          	rgn_obj[handle].box_tmp.box[rgn_obj[handle].box_tmp.cnt].h = 4;
            rgn_obj[handle].box_tmp.cnt++;
            
            for(j = 0; j < boxH; j++)
            {
              p[j*bitmap_w + 0]
              = p[j*bitmap_w + 1]
              = p[j*bitmap_w + 2]
              = p[j*bitmap_w + 3]
              = p[j*bitmap_w + boxW-1] 
              = p[j*bitmap_w + boxW-2]
              = p[j*bitmap_w + boxW-3] 
              = p[j*bitmap_w + boxW-4]
              = ARG1555_RED;
            }
            
            rgn_obj[handle].box_tmp.box[rgn_obj[handle].box_tmp.cnt].a = (int)((char*)p - (char*)bitmap_data);
          	rgn_obj[handle].box_tmp.box[rgn_obj[handle].box_tmp.cnt].w = 4;
          	rgn_obj[handle].box_tmp.box[rgn_obj[handle].box_tmp.cnt].h = boxH;
            rgn_obj[handle].box_tmp.cnt++;
            rgn_obj[handle].box_tmp.box[rgn_obj[handle].box_tmp.cnt].a = (int)((char*)(p+(boxW-4)) - (char*)bitmap_data);
          	rgn_obj[handle].box_tmp.box[rgn_obj[handle].box_tmp.cnt].w = 4;
          	rgn_obj[handle].box_tmp.box[rgn_obj[handle].box_tmp.cnt].h = boxH;
            rgn_obj[handle].box_tmp.cnt++;
            
        	}
      }
    }
    
  	#if 0
    clock_gettime(CLOCK_MONOTONIC, &ts2);
    int cost = (ts2.tv_sec*1000 + ts2.tv_nsec/1000000) - (ts1.tv_sec*1000 + ts1.tv_nsec/1000000);
    printf("raw bitmap cost:%d ms\n", cost);
  	#endif
    
    
    {
    #if 0
    struct timespec ts1, ts2;  
    clock_gettime(CLOCK_MONOTONIC, &ts1);
    #endif

    #ifdef __RGN_CANVAS
    gsf_mpp_rgn_canvas_update(handle);
    #else
    gsf_mpp_rgn_bitmap(handle, &bitMap);
    gsf_mpp_rgn_ctl(handle, GSF_MPP_RGN_SETDISPLAY, &rgn_obj[handle].rgn);
    #endif
    
  	#if 0
    clock_gettime(CLOCK_MONOTONIC, &ts2);
    int cost = (ts2.tv_sec*1000 + ts2.tv_nsec/1000000) - (ts1.tv_sec*1000 + ts1.tv_nsec/1000000);
    printf("gsf_mpp_rgn_bitmap cost:%d ms\n", cost);
  	#endif
  	}
  }
  
  return 0;
}


int gsf_rgn_canvas(int ch, int idx, gsf_osd_act_t *act)
{
  int sub = 0;
  int mask = 1;
  static int handle = -1;
  
  if(act->w != codec_ipc.venc[sub].width)
  {
    //warn("act->w:%d, codec_ipc width:%d\n", act->w, codec_ipc.venc[sub].width);
    return -1;
  }
  
  //printf("act->w:%d, codec_ipc width:%d\n", act->w, codec_ipc.venc[sub].width);
  
  if(handle == -1)
  {
    gsf_rgn_rects_t rects = {0};
    gsf_rgn_rect_set(ch, idx, &rects, mask);
    handle = GSF_RGN_OBJ_HANDLE(ch, OBJ_OSD, sub, idx);
  }
  
  BITMAP_S  bitMap;
  
  #ifdef __RGN_CANVAS
  RGN_CANVAS_INFO_S stRgnCanvasInfo = {0};
  if((gsf_mpp_rgn_canvas_get(handle, &stRgnCanvasInfo) < 0)
    || (stRgnCanvasInfo.stSize.u32Width == 0)
    || (stRgnCanvasInfo.stSize.u32Height == 0))
  {
    return -1;
  }
  
  HI_U16 *p = (HI_VOID*)(HI_UL)stRgnCanvasInfo.u64VirtAddr;
  int w     = stRgnCanvasInfo.u32Stride/2;
  int h	    = stRgnCanvasInfo.stSize.u32Height;
  HI_U16 *act_p = (HI_U16 *)act->data;
  
  #else
  HI_U16 *p = bitmap_data = rgn_obj[handle].osd_bmp;
  int w	    = bitmap_w = rgn_obj[handle].osd_info->osdW;
  int h	    = bitmap_h = rgn_obj[handle].osd_info->osdH;
  HI_U16 *act_p = (HI_U16 *)act->data;  
  bitmap_fmt= PIXEL_FORMAT_ARGB_1555;
  #endif
   
  uint16_t * fbp16 = (uint16_t *)p;
  int32_t y;
  for(y = act->y1; y <= act->y2; y++) 
  {
    int location = (act->x1 + 0) + (y + 0) * w;
    memcpy(&fbp16[location], (uint16_t*)act_p, (act->x2 - act->x1 + 1) * 2);
    act_p += (act->x2 - act->x1 + 1);
  }

  #ifdef __RGN_CANVAS
  gsf_mpp_rgn_canvas_update(handle);
  #else
  gsf_mpp_rgn_bitmap(handle, &bitMap);
  gsf_mpp_rgn_ctl(handle, GSF_MPP_RGN_SETDISPLAY, &rgn_obj[handle].rgn);
  #endif
  
  return 0;
}





#define NK_INCLUDE_FIXED_TYPES
#define NK_INCLUDE_STANDARD_IO
#define NK_INCLUDE_STANDARD_VARARGS
#define NK_INCLUDE_DEFAULT_ALLOCATOR
#define NK_IMPLEMENTATION
#define NK_XLIBSHM_IMPLEMENTATION
#define NK_RAWFB_IMPLEMENTATION
#define NK_INCLUDE_FONT_BAKING
#define NK_INCLUDE_DEFAULT_FONT
#define NK_INCLUDE_SOFTWARE_FONT
#include <math.h>
#include "fw/nuklear/inc/nuklear.h"
#include "fw/nuklear/inc/nuklear_rawfb.h"

static int nk_win_id;

int gsf_rgn_nk_set(int ch, int idx, gsf_rgn_rects_t *rects, int mask)
{
  int i = 0, r = 0, b = 0;
  unsigned int ARG1555_RED = argb8888_1555(0x01FF0000);
  
  for(i = 0; i < GSF_CODEC_VENC_NUM; i++)
  {
    if(!codec_ipc.venc[i].en)
      continue;
    if(!((1<<i) & mask))
      continue;
    
    int handle = GSF_RGN_OBJ_HANDLE(ch, OBJ_OSD, i, idx);
    
    memset(&rgn_obj[handle].rgn, 0, sizeof(rgn_obj[handle].rgn));
    
    rgn_type = OVERLAY_RGN;
    rgn_attr_fmt = PIXEL_FORMAT_ARGB_1555;
    rgn_attr_bg = 0x00000000;//argb8888_1555(0x00FF0000);
    rgn_attr_w = ALIGN_UP(codec_venc_width(ch, i), 2);
    rgn_attr_h = ALIGN_UP(codec_venc_height(ch, i), 2);
    rgn_attr_canvas = 2;
    
    rgn_ch_mod = HI_ID_VENC;
    rgn_ch_dev = 0;
    rgn_ch_chn = ch*GSF_CODEC_VENC_NUM+i;
    rgn_chattr_show = HI_TRUE;
    rgn_chattr_type = OVERLAY_RGN;
    rgn_chattr_over_x = 0;
    rgn_chattr_over_y = 0;
    rgn_chattr_over_layer   = idx;
    
    rgn_chattr_over_bgalpha = 0;//80;
    rgn_chattr_over_fgalpha = FG_ALPHA_MAX;//128;
    
    if(rgn_obj[handle].osd_info == NULL)
    {
      rgn_obj[handle].osd_info = (gsf_rgn_osd_t*)calloc(1, sizeof(gsf_rgn_osd_t));
    }

    gsf_rgn_osd_t *info = rgn_obj[handle].osd_info;

    int _osdW = info->osdW;
    int _osdH = info->osdH;


    info->osdW = rgn_attr_w;
    info->osdH = rgn_attr_h;    
    
    //printf("_osdW:%d, osdW:%d, _osdH:%d, osdH:%d\n", _osdW, info->osdW, _osdH, info->osdH);
    
    //check;
    if(rgn_obj[handle].stat >= GSF_RGN_OBJ_ATTACH)
    {
      if(_osdW != info->osdW || _osdH != info->osdH)
      {  
         gsf_mpp_rgn_ctl(handle, GSF_MPP_RGN_DETACH, &rgn_obj[handle].rgn);
         rgn_obj[handle].stat = GSF_RGN_OBJ_CREATE;
      }
    }

    //create;
    if(rgn_obj[handle].stat < GSF_RGN_OBJ_CREATE)
    {
       gsf_mpp_rgn_ctl(handle, GSF_MPP_RGN_CREATE, &rgn_obj[handle].rgn);
       rgn_obj[handle].stat = GSF_RGN_OBJ_CREATE;
    }
    
    //set attr; 
    if(rgn_obj[handle].osd_bmp == NULL)
    {
      #ifdef __RGN_CANVAS
      rgn_obj[handle].osd_bmp = (char*)1;
      #else
      rgn_obj[handle].osd_bmp = malloc(info->osdW*info->osdH*2);
      memset(rgn_obj[handle].osd_bmp, 0, info->osdW*info->osdH*2);
      #endif
      gsf_mpp_rgn_ctl(handle, GSF_MPP_RGN_SETATTR, &rgn_obj[handle].rgn);
    }
    else if(_osdW != info->osdW || _osdH != info->osdH)
    {
      #ifdef __RGN_CANVAS
      rgn_obj[handle].osd_bmp = (char*)1;
      #else
      rgn_obj[handle].osd_bmp = realloc(rgn_obj[handle].osd_bmp, info->osdW*info->osdH*2);
      memset(rgn_obj[handle].osd_bmp, 0, info->osdW*info->osdH*2);
      #endif
      gsf_mpp_rgn_ctl(handle, GSF_MPP_RGN_SETATTR, &rgn_obj[handle].rgn);
    }
   
    //attach;
    if(rgn_obj[handle].stat < GSF_RGN_OBJ_ATTACH)
    {
      //printf("GSF_MPP_RGN_ATTACH ch:%d, i:%d, idx:%d\n", ch, i, idx);
      gsf_mpp_rgn_ctl(handle, GSF_MPP_RGN_ATTACH, &rgn_obj[handle].rgn);
      rgn_obj[handle].stat = GSF_RGN_OBJ_ATTACH;
    }
   
    BITMAP_S  bitMap;
    
    #ifdef __RGN_CANVAS
      RGN_CANVAS_INFO_S stRgnCanvasInfo = {0};
      if((gsf_mpp_rgn_canvas_get(handle, &stRgnCanvasInfo) < 0)
        || (stRgnCanvasInfo.stSize.u32Width == 0)
        || (stRgnCanvasInfo.stSize.u32Height == 0))
      {
        return -1;
      }
      bitmap_data = (HI_VOID*)(HI_UL)stRgnCanvasInfo.u64VirtAddr;
      bitmap_w	    = stRgnCanvasInfo.u32Stride/2;
      bitmap_h	  = info->osdH;
      bitmap_fmt = PIXEL_FORMAT_ARGB_1555;
      //memset(bitmap_data, 0, stRgnCanvasInfo.stSize.u32Height*stRgnCanvasInfo.u32Stride);
    #else
      bitmap_w	    = info->osdW;
    	bitmap_h	  = info->osdH;
    	bitmap_fmt= PIXEL_FORMAT_ARGB_1555;
    	bitmap_data        = rgn_obj[handle].osd_bmp;
    	//memset(rgn_obj[handle].osd_bmp, 0, info->osdW*info->osdH*2);
    #endif
       
          
    static struct rawfb_context *rawfb = NULL;
    static unsigned char tex_scratch[512 * 512];

    if(!rawfb)
    {
      rawfb = nk_rawfb_init(bitmap_data, tex_scratch, bitmap_w, bitmap_h, bitmap_w * 2, PIXEL_LAYOUT_XRGB_1555);
    }
      
    if(rawfb)
    {
      #if 0
      struct timespec ts1, ts2;  
      clock_gettime(CLOCK_MONOTONIC, &ts1);
      #endif
      
      //clear;
      if(rgn_obj[handle].box_tmp.cnt)
      {
        rawfb->ctx.style.window.header.active = nk_style_item_color(nk_rgba(52,168,83,1));
        rawfb->ctx.style.window.header.hover = nk_style_item_color(nk_rgba(52,168,83,1));
        rawfb->ctx.style.window.header.normal = nk_style_item_color(nk_rgba(52,168,83,1));
        rawfb->ctx.style.window.header.label_active = nk_rgba(255,0,0,1);
        rawfb->ctx.style.window.header.label_hover  = nk_rgba(255,0,0,1);
        rawfb->ctx.style.window.header.label_normal = nk_rgba(255,0,0,1);
        rawfb->ctx.style.window.fixed_background = nk_style_item_color(nk_rgba(255,0,0,0));
        rawfb->ctx.style.window.border_color = nk_rgba(255,0,0,1);
        rawfb->ctx.style.window.border = 2.0f;
        
        for(r = 0; r < rgn_obj[handle].box_tmp.cnt; r++)
      	{
          int boxX = rgn_obj[handle].box_tmp.box[r].a & 0xffff;
          int boxY = (rgn_obj[handle].box_tmp.box[r].a >> 16) & 0xffff;
          int boxW = rgn_obj[handle].box_tmp.box[r].w;
          int boxH = rgn_obj[handle].box_tmp.box[r].h;
          
          char name[16]; sprintf(name, "%08d", nk_win_id++);
          if (nk_begin_titled(&rawfb->ctx, name, "", nk_rect(boxX, boxY, boxW, boxH),
              NK_WINDOW_BORDER|NK_WINDOW_TITLE)) {
            } nk_end(&rawfb->ctx);
      	}
        rgn_obj[handle].box_tmp.cnt = 0;
      }
      //draw;
      rawfb->ctx.style.window.header.active = nk_style_item_color(nk_rgba(52,168,83,255));
      rawfb->ctx.style.window.header.hover = nk_style_item_color(nk_rgba(52,168,83,255));
      rawfb->ctx.style.window.header.normal = nk_style_item_color(nk_rgba(52,168,83,255));
      rawfb->ctx.style.window.header.label_active = nk_rgba(255,0,0,255);
      rawfb->ctx.style.window.header.label_hover  = nk_rgba(255,0,0,255);
      rawfb->ctx.style.window.header.label_normal = nk_rgba(255,0,0,255);
      rawfb->ctx.style.window.fixed_background = nk_style_item_color(nk_rgba(255,0,0,0));
      rawfb->ctx.style.window.border_color = nk_rgba(255,0,0,255);
      rawfb->ctx.style.window.border = 2.0f;
      
      for(r = 0; r < rects->size; r++)
      {
        int boxX = 0, boxY = 0, boxW = 0, boxH = 0;
        char *label = rects->box[r].label;
        
        if(!rects->w || !rects->h)
          break;

        if(rects->box[r].rect[2] && rects->box[r].rect[3])
        {
          
          float wr = info->osdW;
          wr /= rects->w;
          float hr = info->osdH;
          hr /= rects->h;
          
          boxX = (unsigned int)(rects->box[r].rect[0] * wr)&(-1);
          boxY = (unsigned int)(rects->box[r].rect[1] * hr)&(-1);
          
          boxW = (unsigned int)(rects->box[r].rect[2] * wr)&(-1);
          boxH = (unsigned int)(rects->box[r].rect[3] * hr)&(-1); 
          
          #if 1
          rgn_obj[handle].box_tmp.box[r].a =  boxY << 16;
          rgn_obj[handle].box_tmp.box[r].a |= boxX;
          rgn_obj[handle].box_tmp.box[r].w = boxW;
          rgn_obj[handle].box_tmp.box[r].h = boxH;
          rgn_obj[handle].box_tmp.cnt++;
          #endif
          
          char name[16]; sprintf(name, "%08d", nk_win_id++);
          if (nk_begin_titled(&rawfb->ctx, name, label, nk_rect(boxX, boxY, boxW, boxH),
              NK_WINDOW_BORDER|NK_WINDOW_TITLE)) {
            } nk_end(&rawfb->ctx);
        }
      }
      nk_rawfb_render(rawfb, nk_rgba(255,0,0,0), 0);
      
      #if 0
      clock_gettime(CLOCK_MONOTONIC, &ts2);
      int cost = (ts2.tv_sec*1000 + ts2.tv_nsec/1000000) - (ts1.tv_sec*1000 + ts1.tv_nsec/1000000);
      printf("nk_rawfb_render cost:%d ms\n", cost);
    	#endif
    }
  
    {
    #if 0
    struct timespec ts1, ts2;  
    clock_gettime(CLOCK_MONOTONIC, &ts1);
    #endif

    #ifdef __RGN_CANVAS
    gsf_mpp_rgn_canvas_update(handle);
    #else
    gsf_mpp_rgn_bitmap(handle, &bitMap);
    gsf_mpp_rgn_ctl(handle, GSF_MPP_RGN_SETDISPLAY, &rgn_obj[handle].rgn);
    #endif
    
  	#if 0
    clock_gettime(CLOCK_MONOTONIC, &ts2);
    int cost = (ts2.tv_sec*1000 + ts2.tv_nsec/1000000) - (ts1.tv_sec*1000 + ts1.tv_nsec/1000000);
    printf("gsf_mpp_rgn_bitmap cost:%d ms\n", cost);
  	#endif
  	}
  }
  
  return 0;
}