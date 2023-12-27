#include <unistd.h>
#include <stdlib.h>
#include <assert.h>

#include "venc.h"

static gsf_venc_ini_t venc_ini = {.ch_num = 1, .st_num = 2};
venc_mgr_t venc_mgr[GSF_CODEC_NVR_CHN*GSF_CODEC_VENC_NUM];

#include "inc/frm.h"

unsigned int cfifo_recrel(unsigned char *p1, unsigned int n1, unsigned char *p2)
{
    return 0;
}

unsigned int cfifo_recput(unsigned char *p1, unsigned int n1, unsigned char *p2, void *u)
{
  int i = 0, a = 0, l = 0, _n1 = n1;
  unsigned char *p = NULL, *_p1 = p1, *_p2 = p2;
  venc_mgr_t *mgr = (venc_mgr_t*)u;
  VENC_STREAM_S* pstStream = (VENC_STREAM_S*)mgr->pstStream;
  
  //printf("\n------------------\n");
	struct timespec _ts;  
  clock_gettime(CLOCK_MONOTONIC, &_ts);
    
  gsf_frm_t rec;
  rec.type = GSF_FRM_VIDEO;
  rec.flag = (venc_pack_count > 1)?GSF_FRM_FLAG_IDR:0;
  rec.seq  = venc_seq;
  rec.utc  = _ts.tv_sec*1000 + _ts.tv_nsec/1000000;
  rec.pts  = venc_pts/1000;
  rec.video.encode = codec_nvr.venc[mgr->vst%GSF_CODEC_VENC_NUM].type;
  rec.video.width  = codec_nvr.venc[mgr->vst%GSF_CODEC_VENC_NUM].width;
  rec.video.height = codec_nvr.venc[mgr->vst%GSF_CODEC_VENC_NUM].height;
  rec.size = 0;
  
  
  for (i = 0; i < venc_pack_count && i < GSF_FRM_NAL_NUM; i++)
  {
    rec.video.nal[i] = venc_pack_len - venc_pack_off;
    rec.size += rec.video.nal[i];
  }
  
  
  if(i < GSF_FRM_NAL_NUM) rec.video.nal[i] = 0;
  
  //printf("rec[flag:%d, seq:%d, pts:%d, size:%d]\n"
  //      , rec.flag, rec.seq, rec.pts, rec.size);
  
  p = (unsigned char*)(&rec);
  a = sizeof(gsf_frm_t);
  
  l = CFIFO_MIN(a, _n1);
  memcpy(_p1, p, l);
  memcpy(_p2, p+l, a-l);
  _n1-=l;_p1+=l;_p2+=a-l;
  //printf("a:%d, _n1:%d, _p1+:%d, _p2+:%d\n", a, _n1, l, a-l);

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
  
  if(len+sizeof(gsf_frm_t) > GSF_FRM_MAX_SIZE)
  {
    printf("drop VeChn:%d, bigframe size:%d \n", VeChn, len+sizeof(gsf_frm_t));
    goto __err;
  }

  struct timespec ts1, ts2;  
  clock_gettime(CLOCK_MONOTONIC, &ts1);
 
  venc_mgr[VeChn].pstStream = (void*)pstStream;
  int ret = cfifo_put(venc_mgr[VeChn].video_fifo,  len+sizeof(gsf_frm_t), cfifo_recput, (unsigned char*)&venc_mgr[VeChn]);
  
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
  
  venc_ini.ch_num = (venc_ini.ch_num > GSF_CODEC_NVR_CHN)?GSF_CODEC_NVR_CHN:venc_ini.ch_num;
  venc_ini.st_num = (venc_ini.st_num > GSF_CODEC_VENC_NUM)?GSF_CODEC_VENC_NUM:venc_ini.st_num;
  
  memset(venc_mgr, 0, sizeof(venc_mgr));

  for(i = 0; i < venc_ini.ch_num; i++)
  for(j = 0; j < venc_ini.st_num; j++)
  {
    venc_mgr[i*GSF_CODEC_VENC_NUM+j].vst = i*GSF_CODEC_VENC_NUM+j;
    
    #ifdef GSF_CPU_3516e
    int size = (j == 0)? 2*GSF_FRM_MAX_SIZE:
               (j == 1)? 1*GSF_FRM_MAX_SIZE:
               (j == 2)? 0.5*GSF_FRM_MAX_SIZE: 0;
    #else
    int size = (j == 0)? 3*GSF_FRM_MAX_SIZE:
               (j == 1)? 2*GSF_FRM_MAX_SIZE:
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
  return 0;
}