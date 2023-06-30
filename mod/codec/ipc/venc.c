#include <unistd.h>
#include <stdlib.h>
#include <assert.h>
#include <time.h>

#include "venc.h"

#include "inc/frm.h"

#define AFRAME_MAX_SIZE (2*1024)
  
static gsf_venc_ini_t venc_ini = {.ch_num = 1, .st_num = 2};
venc_mgr_t venc_mgr[GSF_CODEC_IPC_CHN*GSF_CODEC_VENC_NUM];
int audio_shmid = -1;
struct cfifo_ex* audio_fifo = NULL;

 
unsigned int cfifo_recrel(unsigned char *p1, unsigned int n1, unsigned char *p2)
{
  
#ifdef __FRM_PHY__ 
  unsigned int size = sizeof(gsf_frm_t) + sizeof(venc_phyaddr_t);
  if(n1 >= size)
  {
      gsf_frm_t *rec = (gsf_frm_t*)p1;
      venc_phyaddr_t *phyaddr = (venc_phyaddr_t*)(p1+sizeof(gsf_frm_t));
      assert(rec->flag & GSF_FRM_FLAG_PHY);
          
      int VeChn = *((int*)phyaddr->opaque);
      VENC_STREAM_S* pstStream = (VENC_STREAM_S*)(phyaddr->opaque + sizeof(int));
      int ret = HI_MPI_VENC_ReleaseStream(VeChn, pstStream);
      free(pstStream->pstPack);
      //printf("HI_MPI_VENC_ReleaseStream VeChn:%d, ret:%d\n", VeChn, ret);
      return 0;
  
  }
  else
  {
      char buf[sizeof(gsf_frm_t) + sizeof(venc_phyaddr_t)];
      char *p = buf;
      memcpy(p, p1, n1);
      memcpy(p+n1, p2, size-n1);
      
      gsf_frm_t *rec = (gsf_frm_t*)buf;
      venc_phyaddr_t *phyaddr = (venc_phyaddr_t*)(buf+sizeof(gsf_frm_t));
      assert(rec->flag & GSF_FRM_FLAG_PHY);
      
      int VeChn = *((int*)phyaddr->opaque);
      VENC_STREAM_S* pstStream = (VENC_STREAM_S*)(phyaddr->opaque + sizeof(int));
      int ret = HI_MPI_VENC_ReleaseStream(VeChn, pstStream);
      free(pstStream->pstPack);
      //printf("HI_MPI_VENC_ReleaseStream VeChn:%d, ret:%d\n", VeChn, ret);
      return 0;
  }
#endif
    return 0;
}

unsigned int cfifo_recput_v(unsigned char *p1, unsigned int n1, unsigned char *p2, void *u)
{
  int i = 0, a = 0, l = 0, _n1 = n1;
  unsigned char *p = NULL, *_p1 = p1, *_p2 = p2;
  venc_mgr_t *mgr = (venc_mgr_t*)u;
  VENC_STREAM_S* pstStream = (VENC_STREAM_S*)mgr->pstStream;
  VENC_STREAM_BUF_INFO_S* pstStreamBufInfo = (VENC_STREAM_BUF_INFO_S*)mgr->pstStreamBufInfo;
  
  //printf("\n------------------\n");
	struct timespec _ts;  
  clock_gettime(CLOCK_MONOTONIC, &_ts);
    
  gsf_frm_t rec;
  rec.type = GSF_FRM_VIDEO;
  rec.flag = (venc_pack_count > 1)?GSF_FRM_FLAG_IDR:0;
  rec.seq  = venc_seq;
  rec.utc  = _ts.tv_sec*1000 + _ts.tv_nsec/1000000;
  rec.pts  = venc_pts/1000;
  rec.video.encode = codec_ipc.venc[mgr->vst%GSF_CODEC_VENC_NUM].type;
  rec.video.width  = codec_ipc.venc[mgr->vst%GSF_CODEC_VENC_NUM].width;
  rec.video.height = codec_ipc.venc[mgr->vst%GSF_CODEC_VENC_NUM].height;
  rec.size = 0;
  
  if(mgr->vst/GSF_CODEC_VENC_NUM == 1)
  {  
    gsf_sdp_t sdp;
    extern int second_sdp(int i, gsf_sdp_t *sdp);
    if(second_sdp(mgr->vst%GSF_CODEC_VENC_NUM, &sdp) == 0)
    {
      rec.video.width = sdp.venc.width;
      rec.video.height = sdp.venc.height;
    }
  }

#ifdef __FRM_PHY__

  rec.flag |= GSF_FRM_FLAG_PHY;
  rec.size = sizeof(venc_phyaddr_t);
  
  venc_phyaddr_t phyaddr = {0};
  phyaddr.u32FrameSize = mgr->frameSize;
  assert(sizeof(VENC_STREAM_S) + sizeof(int) < sizeof(phyaddr.opaque));
  *((int*)phyaddr.opaque) = mgr->vst;
  memcpy(phyaddr.opaque + sizeof(int), pstStream, sizeof(VENC_STREAM_S));
  
  //USEI
  if(mgr->usei_len)
  {
    phyaddr.ulen = mgr->usei_len;
    memcpy(phyaddr.usei, mgr->usei, phyaddr.ulen);
  }
  
  //if(mgr->vst == 1)
  //  printf("vst:%d, rec.seq:%u, utc:%u\n", mgr->vst, rec.seq, rec.utc);
  assert(pstStreamBufInfo && pstStream);
  venc_phyaddr_init(&phyaddr, pstStreamBufInfo, pstStream);

  p = (unsigned char*)(&rec);
  a = sizeof(gsf_frm_t);
  
  l = CFIFO_MIN(a, _n1);
  memcpy(_p1, p, l);
  memcpy(_p2, p+l, a-l);
  _n1-=l;_p1+=l;_p2+=a-l;
  
  
  p = (unsigned char*)(&phyaddr);
  a = sizeof(venc_phyaddr_t);
  
  l = CFIFO_MIN(a, _n1);
  memcpy(_p1, p, l);
  memcpy(_p2, p+l, a-l);
  _n1-=l;_p1+=l;_p2+=a-l;

#else

  for (i = 0; i < venc_pack_count && i < GSF_FRM_NAL_NUM; i++)
  {
    rec.video.nal[i] = venc_pack_len - venc_pack_off;
    rec.size += rec.video.nal[i];
  }
  assert(i < GSF_FRM_NAL_NUM);
  if(i < GSF_FRM_NAL_NUM) rec.video.nal[i] = 0;
  
  //USEI SIZE;
  rec.size += mgr->usei_len;
  
  //printf("rec[flag:%d, seq:%d, pts:%d, size:%d]\n"
  //      , rec.flag, rec.seq, rec.pts, rec.size);
  
  p = (unsigned char*)(&rec);
  a = sizeof(gsf_frm_t);
  
  l = CFIFO_MIN(a, _n1);
  memcpy(_p1, p, l);
  memcpy(_p2, p+l, a-l);
  _n1-=l;_p1+=l;_p2+=a-l;
  //printf("a:%d, _n1:%d, _p1+:%d, _p2+:%d\n", a, _n1, l, a-l);

  //USEI
  if(mgr->usei_len)
  {
    p = (unsigned char*)(mgr->usei);
    a = mgr->usei_len;
    
    l = CFIFO_MIN(a, _n1);
    memcpy(_p1, p, l);
    memcpy(_p2, p+l, a-l);
    _n1-=l;_p1+=l;_p2+=a-l;
  }

  for (i = 0; i < venc_pack_count; i++)
  {
    p = venc_pack_addr + venc_pack_off;
    a = venc_pack_len - venc_pack_off;
    
    l = CFIFO_MIN(a, _n1);
    memcpy(_p1, p, l);
    memcpy(_p2, p+l, a-l);
    _n1-=l;_p1+=l;_p2+=a-l;
    //printf("a:%d, _n1:%d, _p1+:%d, _p2+:%d\n", a, _n1, l, a-l);
  }
  
#endif
  //printf("\n------------------\n");
  return 0;
}


static inline int _venc_sdp_fill(VENC_CHN VeChn, PAYLOAD_TYPE_E pt, VENC_PACK_S* pstPack, int pack_size)
{
  switch(pt)
  {
    case PT_H264:
      {
        int sdp_val = (pack_264_type == H264E_NALU_SPS)? GSF_SDP_VAL_SPS:
                      (pack_264_type == H264E_NALU_PPS)? GSF_SDP_VAL_PPS:
                      (pack_264_type == H264E_NALU_SEI)? GSF_SDP_VAL_SEI: -1;
        if(sdp_val < 0)
        {
          break;
        }
        assert(pack_size <= sizeof(venc_mgr[VeChn].val[sdp_val].data));
        venc_mgr[VeChn].val[sdp_val].size =pack_size-4;
        memcpy(venc_mgr[VeChn].val[sdp_val].data, pack_addr + pack_off+4, venc_mgr[VeChn].val[sdp_val].size);
        break;
      }
    case PT_H265:
      {
        int sdp_val = (pack_265_type == H265E_NALU_SPS)? GSF_SDP_VAL_SPS:
                      (pack_265_type == H265E_NALU_PPS)? GSF_SDP_VAL_PPS:
                      (pack_265_type == H265E_NALU_VPS)? GSF_SDP_VAL_VPS:
                      (pack_265_type == H265E_NALU_SEI)? GSF_SDP_VAL_SEI: -1;
        if(sdp_val < 0)
        {
          break;
        }
        assert(pack_size <= sizeof(venc_mgr[VeChn].val[sdp_val].data));
        venc_mgr[VeChn].val[sdp_val].size =pack_size-4;
        memcpy(venc_mgr[VeChn].val[sdp_val].data, pack_addr + pack_off+4, venc_mgr[VeChn].val[sdp_val].size);
        break;
      }
    default:
      break;
  }
  return 0;
}

static int (*_sei_fill)(char *buf, int size);

int gsf_venc_usei(int (*sei_fill)(char *buf, int size))
{
  _sei_fill = sei_fill;
}

typedef struct {
  int idx;
  int time_sec;
  int x, y;
}osd_time_mng_t;

static osd_time_mng_t osd_time_mng[GSF_CODEC_IPC_CHN] = {{-1},{-1},{-1},{-1}};

int gsf_venc_set_osd_time_idx(int ch, int idx, int x, int y)
{
  printf("ch:%d, idx:%d\n", ch, idx);
  osd_time_mng[ch].idx = idx;
  osd_time_mng[ch].x = x;
  osd_time_mng[ch].y = y;
  return 0;
}

int gsf_venc_recv(VENC_CHN VeChn, PAYLOAD_TYPE_E PT, VENC_STREAM_S* pstStream, void* uargs)
{
  HI_S32 i = 0, len = 0;
    
  for (i = 0; i < venc_pack_count; i++)
  {
    int pack_size = venc_pack_len - venc_pack_off;
    len += pack_size;
    
    //sdp fill;
    if(venc_pack_count > 1)
      _venc_sdp_fill(VeChn, PT, &venc_pack, pack_size);
  }
  
  //USEI SIZE;
  venc_mgr[VeChn].usei_len = 0;
  if(PT == PT_H264 && _sei_fill)
  {
    int ret = _sei_fill(venc_mgr[VeChn].usei, sizeof(venc_mgr[VeChn].usei));
    if(ret > 0)
    {
      venc_mgr[VeChn].usei_len = ret;  
      len += venc_mgr[VeChn].usei_len;
    }
  }
  
  if(len+sizeof(gsf_frm_t) > GSF_FRM_MAX_SIZE)
  {
    printf("drop VeChn:%d, bigframe size:%d \n", VeChn, len+sizeof(gsf_frm_t));
    goto __err;
  }
  
  struct timespec ts1, ts2;  
  clock_gettime(CLOCK_MONOTONIC, &ts1);
 
  venc_mgr[VeChn].pstStream = (void*)pstStream;
  venc_mgr[VeChn].pstStreamBufInfo = uargs;
  venc_mgr[VeChn].frameSize = len;
 
  #ifdef __FRM_PHY__
  int ret = cfifo_put(venc_mgr[VeChn].video_fifo,  sizeof(venc_phyaddr_t)+sizeof(gsf_frm_t)
                    , cfifo_recput_v, (unsigned char*)&venc_mgr[VeChn]);
  #else
  int ret = cfifo_put(venc_mgr[VeChn].video_fifo,  len+sizeof(gsf_frm_t)
                    , cfifo_recput_v, (unsigned char*)&venc_mgr[VeChn]);
  #endif
   
  clock_gettime(CLOCK_MONOTONIC, &ts2);
  int cost = (ts2.tv_sec*1000 + ts2.tv_nsec/1000000) - (ts1.tv_sec*1000 + ts1.tv_nsec/1000000);
  if(cost > 10)
    printf("cfifo_put VeChn:%d, frame size:%d put cost:%d ms\n", VeChn, len+sizeof(gsf_frm_t), cost);

  if(ret < 0)
  {
    printf("cfifo VeChn:%d, err ret:%d\n", VeChn, ret);
    goto __err;
  }
  else if (ret == 0)
  {
    printf("cfifo VeChn:%d, is full, framesize:%d\n", VeChn, len+sizeof(gsf_frm_t));
    assert(0);
  }

  //osd_time;
  if(osd_time_mng[venc_mgr[VeChn].vst/GSF_CODEC_VENC_NUM].idx >= 0 
    &&  venc_mgr[VeChn].vst%GSF_CODEC_VENC_NUM == 0 
    && ts1.tv_sec != osd_time_mng[venc_mgr[VeChn].vst/GSF_CODEC_VENC_NUM].time_sec)
  {
    osd_time_mng[venc_mgr[VeChn].vst/GSF_CODEC_VENC_NUM].time_sec = ts1.tv_sec;
    
    time_t _time = time(NULL);
    struct tm _tm;
    localtime_r(&_time, &_tm);
   
    gsf_osd_t _osd;
    _osd.en = 1;
    _osd.type = 0;
    _osd.fontsize = 0;
    _osd.point[0] = osd_time_mng[venc_mgr[VeChn].vst/GSF_CODEC_VENC_NUM].x;
    _osd.point[1] = osd_time_mng[venc_mgr[VeChn].vst/GSF_CODEC_VENC_NUM].y;
    _osd.wh[0] = 0;
    _osd.wh[1] = 0;
    
    sprintf(_osd.text, "%04d-%02d-%02d %02d:%02d:%02d" 
        , _tm.tm_year+1900, _tm.tm_mon+1, _tm.tm_mday
		    , _tm.tm_hour, _tm.tm_min, _tm.tm_sec);
    //printf("ch:%d, idx:%d\n", venc_mgr[VeChn].vst/GSF_CODEC_VENC_NUM, osd_time_mng[venc_mgr[VeChn].vst/GSF_CODEC_VENC_NUM].idx);
    gsf_rgn_osd_set(venc_mgr[VeChn].vst/GSF_CODEC_VENC_NUM, osd_time_mng[venc_mgr[VeChn].vst/GSF_CODEC_VENC_NUM].idx, &_osd);
  }

  
  #ifdef __FRM_PHY__
  return 1;
  #endif

__err:
  return 0;
}



unsigned int cfifo_recput_a(unsigned char *p1, unsigned int n1, unsigned char *p2, void *u)
{
  int i = 0, a = 0, l = 0, _n1 = n1;
  unsigned char *p = NULL, *_p1 = p1, *_p2 = p2;

  AUDIO_STREAM_S* pstStream = (AUDIO_STREAM_S*)u;

  //printf("\n------------------\n");
	struct timespec _ts;  
  clock_gettime(CLOCK_MONOTONIC, &_ts);

  gsf_frm_t rec;
  rec.type = GSF_FRM_AUDIO;
  rec.flag = GSF_FRM_FLAG_IDR;
  rec.seq  = aenc_seq;
  rec.utc  = _ts.tv_sec*1000 + _ts.tv_nsec/1000000;
  rec.pts  = aenc_pts/1000;
  
  rec.audio.encode = codec_ipc.aenc.type;//GSF_ENC_AAC;
  rec.audio.chn = (codec_ipc.aenc.type == GSF_ENC_AAC)?2:1;//1<<codec_ipc.aenc.stereo;
  rec.audio.sp  = codec_ipc.aenc.sprate;//48; //k;
  rec.audio.bps = 16;
  rec.size = 0;
  
  rec.size = aenc_len;
  
  //printf("rec[flag:%d, seq:%d, pts:%d, size:%d]\n"
  //      , rec.flag, rec.seq, rec.pts, rec.size);
  
  p = (unsigned char*)(&rec);
  a = sizeof(gsf_frm_t);
  
  l = CFIFO_MIN(a, _n1);
  memcpy(_p1, p, l);
  memcpy(_p2, p+l, a-l);
  _n1-=l;_p1+=l;_p2+=a-l;
  //printf("a:%d, _n1:%d, _p1+:%d, _p2+:%d\n", a, _n1, l, a-l);

  p = aenc_addr;
  a = aenc_len;
  
  l = CFIFO_MIN(a, _n1);
  memcpy(_p1, p, l);
  memcpy(_p2, p+l, a-l);
  _n1-=l;_p1+=l;_p2+=a-l;
  //printf("a:%d, _n1:%d, _p1+:%d, _p2+:%d\n", a, _n1, l, a-l);

  //printf("\n------------------\n");
  return 0;
}



int gsf_aenc_recv(int AeChn, PAYLOAD_TYPE_E PT, AUDIO_STREAM_S* pstStream, void* uargs)
{
#if 0
  printf("AeChn:%d, PT:%d, PTS:%llu ms, u32Len:%d, uargs:%p\n"
      , AeChn, PT, aenc_pts/1000, aenc_len, uargs);
#endif

  HI_S32 i = 0, len = 0;
  
  len = aenc_len;
  
  if(len+sizeof(gsf_frm_t) > AFRAME_MAX_SIZE)
  {
    printf("drop AeChn:%d, bigframe size:%d \n", AeChn, len+sizeof(gsf_frm_t));
    goto __err;
  }

  struct timespec ts1, ts2;  
  clock_gettime(CLOCK_MONOTONIC, &ts1);
 
  int ret = cfifo_put(audio_fifo,  len+sizeof(gsf_frm_t), cfifo_recput_a, (unsigned char*)pstStream);
  
  clock_gettime(CLOCK_MONOTONIC, &ts2);
  int cost = (ts2.tv_sec*1000 + ts2.tv_nsec/1000000) - (ts1.tv_sec*1000 + ts1.tv_nsec/1000000);
  if(cost > 8)
    printf("cfifo_put AeChn:%d, frame size:%d put cost:%d ms\n", AeChn, len+sizeof(gsf_frm_t), cost);
  
  if(ret < 0)
  {
    printf("cfifo AeChn:%d, err ret:%d\n", AeChn, ret);
    goto __err;
  }
  else if (ret == 0)
  {
    printf("cfifo AeChn:%d, is full, framesize:%d\n", AeChn, len+sizeof(gsf_frm_t));
    assert(0);
  }

__err:
  return 0;
}



int gsf_venc_init(gsf_venc_ini_t *ini)
{
  int i = 0, j = 0;
  
  if(ini)
  {
    venc_ini = *ini;
  }
  
  venc_ini.ch_num = (venc_ini.ch_num > GSF_CODEC_IPC_CHN)?GSF_CODEC_IPC_CHN:venc_ini.ch_num;
  venc_ini.st_num = (venc_ini.st_num > GSF_CODEC_VENC_NUM)?GSF_CODEC_VENC_NUM:venc_ini.st_num;
  
  memset(venc_mgr, 0, sizeof(venc_mgr));

  for(i = 0; i < venc_ini.ch_num; i++)
  for(j = 0; j < venc_ini.st_num; j++)
  {
    venc_mgr[i*GSF_CODEC_VENC_NUM+j].vst = i*GSF_CODEC_VENC_NUM+j;
    
    #ifdef __FRM_PHY__
    // The physical address of video frame is in CFIFO 
    int size = (j == 0)? 32*(sizeof(gsf_frm_t)+sizeof(venc_phyaddr_t)):
               (j == 1)? 32*(sizeof(gsf_frm_t)+sizeof(venc_phyaddr_t)):
               (j == 2)? 32*(sizeof(gsf_frm_t)+sizeof(venc_phyaddr_t)): 0;
    #else
    // The Video frame data is in CIFO
    int size = (j == 0)? 6*GSF_FRM_MAX_SIZE:
               (j == 1)? 3*GSF_FRM_MAX_SIZE:
               (j == 2)? 1*GSF_FRM_MAX_SIZE: 0;
    #endif
    
    if(size > 0)
    {
      venc_mgr[i*GSF_CODEC_VENC_NUM+j].video_fifo 
              = cfifo_alloc(size, 
                          cfifo_recsize, 
                          cfifo_rectag, 
                          cfifo_recrel, 
                          &venc_mgr[i*GSF_CODEC_VENC_NUM+j].video_shmid,
                          0);
    }
  }
  
  audio_fifo = cfifo_alloc(16*AFRAME_MAX_SIZE,
                  cfifo_recsize, 
                  cfifo_rectag, 
                  NULL, 
                  &audio_shmid,
                  0);
  return 0;
}