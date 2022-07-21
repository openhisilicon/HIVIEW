#ifndef __frm_h__
#define __frm_h__

#ifdef __FRM_PHY__

typedef struct {
  unsigned char opaque[512];
  unsigned int u32FrameSize;
  unsigned int u32PackCount;
  struct {
    unsigned long long u64PhyAddr;
    unsigned long long u64PhyAddr2;
    unsigned char* u8Addr;
    unsigned char* u8Addr2;
    unsigned int u32Len;
    unsigned int u32Len2;
  }stPack[16];
  unsigned int  ulen;
  unsigned char usei[256];
}venc_phyaddr_t;

//stream => venc_phyaddr_t;
extern int venc_phyaddr_init(venc_phyaddr_t* phyaddr, void *streamInfo, void* stream);
extern int venc_phyaddr_mmap(venc_phyaddr_t* phyaddr);
extern int venc_phyaddr_munmap(venc_phyaddr_t* phyaddr);
 
//dma memory;
extern unsigned char* venc_viraddr_alloc(unsigned int u32Len);
extern int venc_viraddr_free(unsigned char *virAddr);

//dma copy;
#ifdef __DMA_COPY__
extern int venc_viraddr_copy( unsigned char *dstVirAddr, int dstOffset, unsigned char *srcVirAddr, int srcOffset, unsigned int u32Len);
#endif //__DMA_COPY__

#endif //__FRM_PHY__
 

//media;
enum {
  GSF_FRM_VIDEO  = 0,
  GSF_FRM_AUDIO  = 1,
};

enum {
  GSF_FRM_FLAG_IDR  = (1 << 0), //IDR;
  GSF_FRM_FLAG_SHM  = (1 << 1), //SHM - shm_ptr_t;
  GSF_FRM_FLAG_EOF  = (1 << 2), //EOF;
  GSF_FRM_FLAG_PHY  = (1 << 3), //PHY - venc_phyaddr_t;
};

enum {
	GSF_FRM_VIDEO_P      = 0, /* P  */
	GSF_FRM_VIDEO_I      = 1, /* I  */
  GSF_FRM_VIDEO_B      = 2, /* B  */
  GSF_FRM_AUDIO_1      = 3, /* 1  */
  GSF_FRM_BUTT,
};

#ifdef GSF_CPU_3559a
#define GSF_FRM_NAL_NUM 10
#define GSF_FRM_MAX_SIZE (4000*1024)
#else
#define GSF_FRM_NAL_NUM 6
#define GSF_FRM_MAX_SIZE (1000*1024)
#endif

typedef struct {
  unsigned char  type;   // GSF_FRM_VIDEO_P - GSF_FRM_VIDEO_B
  unsigned char  encode; // GSF_ENC_H264 - GSF_ENC_MPEG4
  unsigned char  res[2]; // res;
  unsigned short width;  // picture width;
  unsigned short height; // picture height;
  unsigned int   nal[GSF_FRM_NAL_NUM]; // nal size;
}gsf_frm_video_t;

typedef struct {
  unsigned char encode; // GSF_ENC_ADPCM - GSF_ENC_AAC
  unsigned char chn;    // ChannelNum
  unsigned char bps;    // BitsPerSample
  unsigned char res;    // res;
  unsigned short sp;    // SamplesPerSec(k)
  unsigned short res1;  // res;
  unsigned int  res2[6];// res;
}gsf_frm_audio_t;

typedef struct {
  unsigned char type;   // GSF_FRM_VIDEO - GSF_FRM_AUDIO
  unsigned char flag;   // GSF_FRM_FLAG_;
  unsigned short seq;   // sequence;
  unsigned int   utc;   // localtime ms;
  unsigned int   pts;   // timestamp ms;
  unsigned int   size;  // data size;
  union
  {
    gsf_frm_video_t video;
    gsf_frm_audio_t audio;
  };
  unsigned char data[0]; //line-data or shm_ptr_t or venc_phyaddr_t;
}/*__attribute__((packed))*/gsf_frm_t;


// calc frame size;
static unsigned int cfifo_recsize(unsigned char *p1, unsigned int n1, unsigned char *p2)
{
  unsigned int size = sizeof(gsf_frm_t);

  if(n1 >= size)
  {
    gsf_frm_t *rec = (gsf_frm_t*)p1;
    return  sizeof(gsf_frm_t) + rec->size;
  }
  else
  {
    gsf_frm_t rec;
    char *p = (char*)(&rec);
    memcpy(p, p1, n1);
    memcpy(p+n1, p2, size-n1);
    return  sizeof(gsf_frm_t) + rec.size;
  }
  return 0;
}
  
// get frame IDR/UTC;
static unsigned int cfifo_rectag(unsigned char *p1, unsigned int n1, unsigned char *p2)
{
  unsigned int size = sizeof(gsf_frm_t);

  if(n1 >= size)
  {
    gsf_frm_t *rec = (gsf_frm_t*)p1;
    return (rec->flag & GSF_FRM_FLAG_IDR)?rec->utc:0;
  }
  else
  {
    gsf_frm_t rec;
    char *p = (char*)(&rec);
    memcpy(p, p1, n1);
    memcpy(p+n1, p2, size-n1);
    return (rec.flag & GSF_FRM_FLAG_IDR)?rec.utc:0;
  }
  return 0;
}
 
// get frame data;
static unsigned int cfifo_recgut(unsigned char *p1, unsigned int n1, unsigned char *p2, void *u)
{
  gsf_frm_t *rec = (gsf_frm_t *)u;
  unsigned int len = cfifo_recsize(p1, n1, p2);
  unsigned int l = CFIFO_MIN(len, n1);
  
  char *p = (char*)u;
  memcpy(p, p1, l);
  memcpy(p+l, p2, len-l);
  
  #if 1
  {
  	struct timespec _ts;  
    clock_gettime(CLOCK_MONOTONIC, &_ts);
    int cost = (_ts.tv_sec*1000 + _ts.tv_nsec/1000000) - rec->utc;
    if(cost > 33)
      printf("get rec->type:%d ok [delay:%d ms].\n", rec->type, cost);
  }
  #endif

  if(rec->flag & GSF_FRM_FLAG_PHY)
  {
    #ifdef __FRM_PHY__
    int i = 0;
    venc_phyaddr_t _phyaddr = *((venc_phyaddr_t*)rec->data);
    venc_phyaddr_t *phyaddr = &_phyaddr;
    
    //phyaddr => linebuffer;
    rec->flag &= ~GSF_FRM_FLAG_PHY;
    rec->size = 0;

    struct timespec ts1, ts2;  
    clock_gettime(CLOCK_MONOTONIC, &ts1);

    venc_phyaddr_mmap(phyaddr);
    
    for(i = 0; i < phyaddr->u32PackCount; i++)
    {
      int done = 0;
      if(phyaddr->stPack[i].u8Addr)
      {
        #ifdef __DMA_COPY__
        done = venc_viraddr_copy(rec->data, rec->size, phyaddr->stPack[i].u8Addr, 0, phyaddr->stPack[i].u32Len);
        done = (done >= 0)?done:0;
        #endif
        memcpy(rec->data + rec->size + done, phyaddr->stPack[i].u8Addr + done, phyaddr->stPack[i].u32Len - done);
        rec->size += phyaddr->stPack[i].u32Len;
      }
      
      done = 0; 
      if(phyaddr->stPack[i].u8Addr2)
      {
        #ifdef __DMA_COPY__
        done = venc_viraddr_copy(rec->data, rec->size, phyaddr->stPack[i].u8Addr2, 0, phyaddr->stPack[i].u32Len2);
        done = (done >= 0)?done:0;
        #endif
        memcpy(rec->data + rec->size + done, phyaddr->stPack[i].u8Addr2 + done, phyaddr->stPack[i].u32Len2 - done);
        rec->size += phyaddr->stPack[i].u32Len2;
      }
    }
    venc_phyaddr_munmap(phyaddr);
    
    clock_gettime(CLOCK_MONOTONIC, &ts2);
    int cost = (ts2.tv_sec*1000 + ts2.tv_nsec/1000000) - (ts1.tv_sec*1000 + ts1.tv_nsec/1000000);
    if(cost > 3)
    {
      #ifdef __DMA_COPY__  
      printf("DMA_COPY u32FrameSize:%d, cost:%d ms\n", phyaddr->u32FrameSize, cost);
      #else
      printf("CPU_COPY u32FrameSize:%d, cost:%d ms\n", phyaddr->u32FrameSize, cost);
      #endif
    }
    #else
    printf("%s => error unknow GSF_FRM_FLAG_PHY\n", __func__);
    #endif
  }
  
  return len;
}

#endif //__frm_h__