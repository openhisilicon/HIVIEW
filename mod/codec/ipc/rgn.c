#include "mpp.h"
#include "fw/libfont/inc/gsf_font_draw.h"
#include "rgn.h"
#include "cfg.h"

static gsf_rgn_ini_t rgn_ini = {.st_num = 3};

int gsf_rgn_init(gsf_rgn_ini_t *ini)
{
  if(ini)
    rgn_ini = *ini;
  return 0;
}

typedef struct {
    char  lines[8][128];//osd叠加行信息
  	int   lineN;        //osd行数
  	int   colN;         //osd列数
    int   fontW;        //位图字体宽度
    int   fontH;        //位图字体高度
    int   fontS;        //位图字体间隔  
    int   osdW;         //位图宽度
    int   osdH;         //位图高度    
}gsf_rgn_osd_t;


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
}gsf_rgn_obj_t;

enum {
  OBJ_OSD   = 0,
  OBJ_VMASK = 1,
};

#define GSF_RGN_OBJ_HANDLE(ch, type, st, idx) ((ch)*(8*3+8*3) + (type)*(8*3) + (st)*(8) + idx)
static gsf_rgn_obj_t rgn_obj[8*3+8*3 + 8*3+8*3];

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

int gsf_parse_text(char *text, char lines[8][128], int *lineN, int *colN)
{
	int nIndex = 0, nMaxLen = 0, nLen = 0, nLinePos = 0;

  int u8LineNum = 1;

	while(nIndex < 128 
		  && text[nIndex] != '\0'
		  && u8LineNum <= 8)
	{
		if (text[nIndex] == '\n')
		{
	    memset(lines[u8LineNum-1], 0, 128);
	    strncpy(lines[u8LineNum-1], &text[nLinePos], nIndex - nLinePos);
			if ((++u8LineNum) > 8) //行数超出后不再检测
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
	
	if (u8LineNum <= 8)
	{
    memset(lines[u8LineNum-1], 0, 128);
		strncpy(lines[u8LineNum-1], &text[nLinePos], nIndex - nLinePos);
		nMaxLen = (nLen > nMaxLen) ? nLen : nMaxLen;
	}
	else
	{
		u8LineNum = 8;
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
 		
	if (u8FontWTmp >= 48 && (FontSTmp >= 0))	
		FontSTmp -= 6;
  if (u8FontWTmp >= 32 && (FontSTmp >= 0))	
		FontSTmp -= 4;
  else if (u8FontWTmp >= 24 && (FontSTmp >= 0))	
		FontSTmp -= 2;
	else if (FontSTmp < 0 && u8FontWTmp < 24)
    FontSTmp = 0;

 	if (NULL != pFontS)
 		(*pFontS) = FontSTmp;
 		
	return 0;
}

static int gsf_bitmap_make_edge(BITMAP_S* bitMap)
{
  int i, j;
  
  //assert(bitMap->enPixelFormat == PIXEL_FORMAT_RGB_1555);
  
	//level scan;
	char *lbuf = (char*)calloc(1, (bitMap->u32Width<<1)*bitMap->u32Height);
	memcpy(lbuf, (char*)bitMap->pData, (bitMap->u32Width<<1)*bitMap->u32Height);
	unsigned short *lpix = (unsigned short *)lbuf;
	
	for(i = 0; i < bitMap->u32Height; i++)
	{
	  for(j = 0; j < bitMap->u32Width-1; j++)
	  {
	    if(lpix[i*bitMap->u32Width+j] ^ lpix[i*bitMap->u32Width+j + 1])
	    {
	      if(lpix[i*bitMap->u32Width+j] == 0)
	      {
	        lpix[i*bitMap->u32Width+j] = 0x8000;
	      }
	      else
	      {
	        lpix[i*bitMap->u32Width+j+1] = 0x8000;
	        j++;        
	      }
	    }
	  }
	}

  //vertical scan;
	char *vbuf = (char*)calloc(1, (bitMap->u32Width<<1)*bitMap->u32Height);
	memcpy(vbuf, (char*)bitMap->pData, (bitMap->u32Width<<1)*bitMap->u32Height);
	unsigned short *vpix = (unsigned short *)vbuf;

	for(i = 0; i < bitMap->u32Width; i++)
	{
	  for(j = 0; j < bitMap->u32Height-1; j++)
	  {
	    if(vpix[j*bitMap->u32Width+i] ^ vpix[(j+1)*bitMap->u32Width+i])
	    {
	      if(vpix[j*bitMap->u32Width+i] == 0)
	      {
	        vpix[j*bitMap->u32Width+i] = 0x8000;
	      }
	      else
	      {
	        vpix[(j+1)*bitMap->u32Width+i] = 0x8000;
	        j++;
	      }
	    }
	  }
	}
	
	//mixture;
  unsigned short *pData = (unsigned short *)bitMap->pData;
  
  for(i = 0; i < bitMap->u32Height; i++)
	{
	  for(j = 0; j < bitMap->u32Width; j++)
	  {
	    pData[i*bitMap->u32Width+j] = lpix[i*bitMap->u32Width+j];
	    pData[i*bitMap->u32Width+j] |= vpix[i*bitMap->u32Width+j];
	  }
	}
	
	free(lpix);
	free(vpix);
}



int gsf_rgn_osd_set(int ch, int idx, gsf_osd_t *osd)
{
  int i = 0;
  for(i = 0; i < rgn_ini.st_num; i++)
  {
    int handle = GSF_RGN_OBJ_HANDLE(ch, OBJ_OSD, i, idx);
    
    memset(&rgn_obj[handle].rgn, 0, sizeof(rgn_obj[handle].rgn));
    
    rgn_obj[handle].rgn.stRegion.enType = OVERLAY_RGN;
    rgn_obj[handle].rgn.stRegion.unAttr.stOverlay.enPixelFmt = PIXEL_FORMAT_ARGB_1555;
    rgn_obj[handle].rgn.stRegion.unAttr.stOverlay.u32BgColor = 0x00ff00ff;
    rgn_obj[handle].rgn.stRegion.unAttr.stOverlay.stSize.u32Width = 2;
    rgn_obj[handle].rgn.stRegion.unAttr.stOverlay.stSize.u32Height = 2;
    rgn_obj[handle].rgn.stRegion.unAttr.stOverlay.u32CanvasNum = 2;
    
    rgn_obj[handle].rgn.stChn.enModId = HI_ID_VENC;
    rgn_obj[handle].rgn.stChn.s32DevId = 0;
    rgn_obj[handle].rgn.stChn.s32ChnId = i;
    
    rgn_obj[handle].rgn.stChnAttr.bShow = osd->en;//HI_TRUE;
    rgn_obj[handle].rgn.stChnAttr.enType = OVERLAY_RGN;
    rgn_obj[handle].rgn.stChnAttr.unChnAttr.stOverlayChn.stPoint.s32X = ALIGN_UP(osd->point[0], 2);
    rgn_obj[handle].rgn.stChnAttr.unChnAttr.stOverlayChn.stPoint.s32Y = ALIGN_UP(osd->point[1], 2);
    rgn_obj[handle].rgn.stChnAttr.unChnAttr.stOverlayChn.u32Layer   = idx;
    rgn_obj[handle].rgn.stChnAttr.unChnAttr.stOverlayChn.u32BgAlpha = 0;
    rgn_obj[handle].rgn.stChnAttr.unChnAttr.stOverlayChn.u32FgAlpha = 128;
    
    if(rgn_obj[handle].osd_info == NULL)
    {
      rgn_obj[handle].osd_info = (gsf_rgn_osd_t*)calloc(1, sizeof(gsf_rgn_osd_t));
    }

    gsf_rgn_osd_t *info = rgn_obj[handle].osd_info;
    
    int _osdW = info->osdW;
    int _osdH = info->osdH;
    
    gsf_parse_text(osd->text, info->lines, &info->lineN, &info->colN);
    
    gsf_calc_fontsize(codec_ipc.venc[i].width, codec_ipc.venc[i].height, osd->fontsize, &info->fontW, &info->fontH, &info->fontS);
    
    info->osdW = (info->fontW + info->fontS)*info->colN - info->fontS+1;
    info->osdH = info->fontH * info->lineN + 1;
    
    rgn_obj[handle].rgn.stRegion.unAttr.stOverlay.stSize.u32Width = ALIGN_UP(info->osdW, 2);
    rgn_obj[handle].rgn.stRegion.unAttr.stOverlay.stSize.u32Height = ALIGN_UP(info->osdH, 2);
    
    
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
      rgn_obj[handle].osd_bmp = malloc(info->osdW*info->osdH*2);

      gsf_mpp_rgn_ctl(handle, GSF_MPP_RGN_SETATTR, &rgn_obj[handle].rgn);
    }
    else if(_osdW != info->osdW || _osdH != info->osdH)
    {
      rgn_obj[handle].osd_bmp = realloc(rgn_obj[handle].osd_bmp, info->osdW*info->osdH*2);
      gsf_mpp_rgn_ctl(handle, GSF_MPP_RGN_SETATTR, &rgn_obj[handle].rgn);
    }

    //attach;
    if(rgn_obj[handle].stat < GSF_RGN_OBJ_ATTACH)
    {
      gsf_mpp_rgn_ctl(handle, GSF_MPP_RGN_ATTACH, &rgn_obj[handle].rgn);
      rgn_obj[handle].stat = GSF_RGN_OBJ_ATTACH;
    }
    
    // bitmap;
    {
        BITMAP_S  bitMap;
      	memset(rgn_obj[handle].osd_bmp, 0, info->osdW*info->osdH*2);
      	int l = 0;
      	for (l = 0; l < info->lineN; l++)
      	{		
      	    gsf_font_utf8_draw_line(info->fontW,
      	          info->osdW,
                  info->fontS,
                  rgn_obj[handle].osd_bmp + l * info->osdW * info->fontH*2,  
          		    info->lines[l], "",
          		    0xffff, 0x0000, 0xffff, 0x0000);
      	}

        bitMap.u32Width	    = info->osdW;
      	bitMap.u32Height	  = info->osdH;
      	bitMap.enPixelFormat= PIXEL_FORMAT_ARGB_1555;
      	bitMap.pData        = rgn_obj[handle].osd_bmp;
         
      	gsf_bitmap_make_edge(&bitMap);
        gsf_mpp_rgn_bitmap(handle, &bitMap);
    }
    
    gsf_mpp_rgn_ctl(handle, GSF_MPP_RGN_SETDISPLAY, &rgn_obj[handle].rgn);
    
  }
}

int gsf_rgn_vmask_set(int ch, int idx, gsf_vmask_t *vmask)
{
  int i = 0;
  for(i = 0; i < 1; i++)
  {
    int handle = GSF_RGN_OBJ_HANDLE(ch, OBJ_VMASK, i, idx);
    
    memset(&rgn_obj[handle].rgn, 0, sizeof(rgn_obj[handle].rgn));
    
    rgn_obj[handle].rgn.stRegion.enType = COVER_RGN;
    
    rgn_obj[handle].rgn.stChn.enModId = HI_ID_VPSS;
    rgn_obj[handle].rgn.stChn.s32DevId = 0;
    rgn_obj[handle].rgn.stChn.s32ChnId = i;
    
    rgn_obj[handle].rgn.stChnAttr.bShow = vmask->en;//HI_TRUE;
    rgn_obj[handle].rgn.stChnAttr.enType = COVER_RGN;
    rgn_obj[handle].rgn.stChnAttr.unChnAttr.stCoverChn.enCoverType = AREA_RECT;
    rgn_obj[handle].rgn.stChnAttr.unChnAttr.stCoverChn.stRect.s32X = ALIGN_UP(vmask->rect[0], 2);
    rgn_obj[handle].rgn.stChnAttr.unChnAttr.stCoverChn.stRect.s32Y = ALIGN_UP(vmask->rect[1], 2);
    rgn_obj[handle].rgn.stChnAttr.unChnAttr.stCoverChn.stRect.u32Width  = ALIGN_UP(vmask->rect[2], 2)?:2;
    rgn_obj[handle].rgn.stChnAttr.unChnAttr.stCoverChn.stRect.u32Height = ALIGN_UP(vmask->rect[3], 2)?:2;
    rgn_obj[handle].rgn.stChnAttr.unChnAttr.stCoverChn.u32Layer         = idx;
    rgn_obj[handle].rgn.stChnAttr.unChnAttr.stCoverChn.u32Color         = vmask->color;//0x0000ffff;
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

