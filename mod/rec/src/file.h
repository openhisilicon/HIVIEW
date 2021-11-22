#ifndef __file_h__
#define __file_h__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>
#include <utime.h>
#include <pthread.h>
#include <semaphore.h>
#include <assert.h>
#include <fcntl.h>
#include <dirent.h>
#include <sys/statvfs.h>

#include "mov-format.h"
#include "mpeg4-avc.h"
#include "mpeg4-hevc.h"
#include "mpeg4-aac.h"
#include "fmp4-writer.h"
#include "mov-reader.h"

#include "rec.h"


#define strcat_s strcat
#define strcpy_s strcpy
#define sprintf_s snprintf
#define localtime_s(s, i) localtime_r(i, s)

typedef struct rec_v_info_s
{
    int w;  //宽度
    int h;  //高度
    int fps;//帧率
}rec_v_info_t;

typedef struct rec_a_info_s
{
    int sp;  //采样率
    int bps; //bit_per_sample
    int chs; //通道数
}rec_a_info_t;

typedef struct rec_rw_info_s
{
    int type;   // GSF_FRM_
    int enc;    // GSF_ENC_
    int time;   // localtime sec
    int ms;     // pts ms
    union {
        rec_v_info_t v;
        rec_a_info_t a;
    };
}rec_rw_info_t;


typedef struct _rec_media_info_s
{
	int aenc;   // GSF_ENC_
	int venc;   // GSF_ENC_
	rec_v_info_t v;
  rec_a_info_t a;
}rec_media_info_t;


typedef struct fd_av_s {
    FILE *fd; // first field;
    uint32_t stime;
    uint32_t size;
    
    // track;
    int vtrack;
    int atrack;
    
    union { 
      fmp4_writer_t *w; 
      mov_reader_t  *r;
    };
    
    // mp4-frame buffer;
	  int s_length; 
	  uint8_t *s_buffer;
	    
	  // out-mp4-frame-info;
    int type;                           // GSF_FRM_
	  int64_t pts;                        // ms;
    rec_v_info_t v;                     // media info;
    rec_a_info_t a;                     // media info;
	  
	  // out-mp4-frame-buffer;
	  int *out_size;                      // out size;
	  uint8_t *out_buffer;                // out buffer;
	  
	  int vobject, aobject;               // MOV_OBJECT;
	  union {
  	  struct mpeg4_hevc_t s_hevc;       // extra
      struct mpeg4_avc_t s_avc;         // extra
	  };
	  union {
  	  struct mpeg4_aac_t s_aac;         // extra
	  };

}fd_av_t;


typedef struct {
    FILE *fd; // first field;
}file_t;

/* 文件读写 */
file_t*
    fd_open(char *name, int type);              /* 打开文件，不存在时创建文件 */
int fd_close(file_t *fd);                       /* 关闭文件 */
int fd_flush(file_t *fd);                       /* 刷数据到磁盘 */
int fd_seek(file_t *fd, int offset, int origin);/* SEEK */
int fd_write(file_t *fd, char *buf, int size);  /* 写文件 */
int fd_read(file_t *fd, char *buf, int size);   /* 读文件 */
int fd_size(file_t *fd);                        /* 文件大小 */
int fd_rm(char *name);                          /* 删除文件 */

/* 视频文件写 */
fd_av_t*
    fd_av_open(char *name);               /* 打开视频，不存在时创建文件 */
int fd_av_close(fd_av_t *fd);             /* 关闭视频文件 */
int fd_av_flush(fd_av_t *fd);             /* 刷数据到磁盘 */
int fd_av_write(fd_av_t *fd, char *buf, int size, rec_rw_info_t *info);   /* 写视频文件 */
uint32_t fd_av_size(fd_av_t *fd);         /* 获取文件大小 */

/* 视频文件读 */
fd_av_t*
    fd_av_ropen(char *name, int type, rec_media_info_t *media); /* 打开视频，type: 用作扩展视频文件格式 */
int fd_av_rclose(fd_av_t *fd);            /* 关闭视频文件 */
int fd_av_rread(fd_av_t *fd, char *buf, int *size, rec_rw_info_t *info);    /* 读视频文件 */
int fd_av_rseek(fd_av_t *fd, int sec);    /* SEEK到指定的sec秒 */
int fd_av_rsize(fd_av_t *fd);             /* 获取文件大小 */

/* 目录操作 */
int fd_access(char *name);                /* 判断一个文件或目录是否存在 0: 不存在, 1: 存在 */
int fd_dir_mk(char *name);                /* 创建一个目录 如果此目录已存返回成功 */
int fd_dir_mv(char *src, char *dst);      /* 目录改名 */
int fd_dir_rm(char *name);                /* 删除目录及目录中的所有文件 */
int fd_dir_each(char *name, int (*cb)(char *name, int is_dir, void *u), void *u); /* 遍历目录下的所有文件及子目录 */



#endif //__file_h__
