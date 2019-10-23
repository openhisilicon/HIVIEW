#ifndef _GSF_FONT_UTF8_DRAW_LINE_H
#define _GSF_FONT_UTF8_DRAW_LINE_H

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */

int gsf_font_utf8_draw_line(unsigned char u8FontW, unsigned short u16OsdWidth, int nFontInt, char *pBuff,
							char *pNewTitle, char *pOldTitle, unsigned short u16FontColor, 
							unsigned short u16EdgeColor, unsigned short u16OldFontColor, 
							unsigned short u16OldEdgeColor);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* _GSF_FONT_UTF8_DRAW_LINE_H */