#ifndef _GSF_CHARACTER_CONV_H
#define _GSF_CHARACTER_CONV_H

/*
* Copyright (c) 2014
* All rights reserved.
* 
* filename:gsf_character_conv.h
* 
* func:本文件实现了gb2312 utf-8 unicode字符编码 相互间的转换
* 
* author : gsf
* createdate: 2014-3-10
*
* modify list: 
* 
*/

#ifdef __cplusplus
extern "C" {
#endif
/*********************************************************************

功能:gb2312编码转utf-8编码

gb2312(in) : 所要转的gb2312字符

gb2312Len(in): gb2312字符长度

utf8(in/out): 转换后的utf-8字符串

返回:utf8
*********************************************************************/	
char*  gsf_gb2312_to_utf8(const char* gb2312, int gb2312Len, char *utf8);

/*********************************************************************

功能:utf-8编码转gb2312编码

utf8(in)   : 所要转的utf-8字符

utf8Len(in): utf-8字符长度

gb2312(in/out): 转换后的gb2312字符串

返回:gb2312
*********************************************************************/	
char*  gsf_utf8_to_gb2312(const char* utf8, int utf8Len, char *gb2312);

/***********************************************************

功能:取utf-8 字符个数

utf8 : utf-8首字符
		
返回值：成功返回
***********************************************************/	
int gsf_get_Utf8_byte_num(unsigned char utf8);

/***********************************************************

功能:utf-8编码转unicode编码

byte : utf-8字符串

index: 所要转的字符在字符串的位置

count: 字符串的长度

unicode:转换后的unicode值
		
返回值：成功返回0，否则放回-1
***********************************************************/		
int  gsf_utf8_to_unicode(unsigned char  *byte, int index, int count,  unsigned int* unicode);

/*********************************************************************
功能:unicode编码转utf-8编码

unicode:所要转的unicode值

byte:转换的utf-8字符串存放的空间(确保大小大于6)

pCount : 转换后字符串的字符个数

返回值：成功返回utf-8字符串长度(1-6)，否则放回-1
*********************************************************************/	
int  gsf_unicode_to_utf8(unsigned int unicode, unsigned char *byte);

/*********************************************************************
功能:gb2312编码转unicode编码

gb2312:所要转的gb2312值

返回值：unicode编码
*********************************************************************/
unsigned int  gsf_gb2312_to_unicode(unsigned short gb2312);

/*********************************************************************
功能:unicode编码转gb2312编码

unicode:所要转的unicode值

返回值：gb2312编码
*********************************************************************/
unsigned short  gsf_unicode_to_gb2312(unsigned int unicode);

#ifdef __cplusplus
}
#endif

#endif

