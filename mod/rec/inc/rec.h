#ifndef __rec_h__
#define  __rec_h__

// SD卡录像模块:
// 使用日志文件记录文件名(仅在打开/关闭文件时,APPEND写操作一次);
// 索引信息在内存,提高检索速度,减少磁盘操作;
// 使用 fragment MP4格式,异常掉电时未关闭的文件可正常访问;
// 所有数据(日志,视频)循环覆盖,支持视频文件写保护,防止被覆盖;
// 支持FAT32,EXT4文件系统;

#ifdef __cplusplus
extern "C" {
#endif


#include "inc/gsf.h"

//for json cfg;
#include "mod/rec/inc/sjb_rec.h"


#define GSF_IPC_REC "ipc:///tmp/rec_rep"

enum {
  GSF_ID_REC_QDISK = 1,  // [gsf_disk_t .... N];
  GSF_ID_REC_QREC  = 2,  // [gsf_rec_q_t, gsf_file_t .... N];
  GSF_ID_REC_CFG   = 3,  // [gsf_rec_cfg_t];
  GSF_ID_REC_VOD   = 4,  // [gsf_file_t, gsf_rec_media_t];
  GSF_ID_REC_END
};

enum {
  GSF_REC_ERR = -1,
};

enum {
  GSF_FILE_TAGS_PIC     = (1<<0),  // bit-mask picture;
  GSF_FILE_TAGS_TIMER   = (1<<1),  // bit-mask timer ;
	GSF_FILE_TAGS_MANUAL  = (1<<2),  // bit-mask manual; 
  GSF_FILE_TAGS_MD      = (1<<3),  // bit-mask motion;
	GSF_FILE_TAGS_IO      = (1<<4),  // bit-mask gpio
	GSF_FILE_TAGS_PIR     = (1<<5),  // bit-mask pir;
};

typedef struct {
  int av_shmid;  // av buffer id;
  // sdp info;
}gsf_rec_media_t;


#ifdef __cplusplus
}
#endif

#endif