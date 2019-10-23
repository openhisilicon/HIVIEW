#include <unistd.h>
#include <stdlib.h>
#include <assert.h>

#include "inc/gsf.h"
#include "mod/bsp/inc/bsp.h"

#include "codec.h"
#include "cfg.h"
#include "msg_func.h"
#include "mpp.h"
#include "rgn.h"


GSF_LOG_GLOBAL_INIT("CODEC", 8*1024);

#define FRAME_MAX_SIZE (500*1024)

venc_mgr_t venc_mgr[3];

static int req_recv(char *in, int isize, char *out, int *osize, int err)
{
    int ret = 0;
    gsf_msg_t *req = (gsf_msg_t*)in;
    gsf_msg_t *rsp = (gsf_msg_t*)out;

    *rsp  = *req;
    rsp->err  = 0;
    rsp->size = 0;

    ret = msg_func_proc(req, isize, rsp, osize);

    rsp->err = (ret == TRUE)?rsp->err:GSF_ERR_MSG;
    *osize = sizeof(gsf_msg_t)+rsp->size;

    return 0;
}

unsigned int cfifo_recsize(unsigned char *p1, unsigned int n1, unsigned char *p2)
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

unsigned int cfifo_rectag(unsigned char *p1, unsigned int n1, unsigned char *p2)
{
    unsigned int size = sizeof(gsf_frm_t);

    if(n1 >= size)
    {
        gsf_frm_t *rec = (gsf_frm_t*)p1;
        return rec->flag & GSF_FRM_FLAG_IDR;
    }
    else
    {
        gsf_frm_t rec;
        char *p = (char*)(&rec);
        memcpy(p, p1, n1);
        memcpy(p+n1, p2, size-n1);
        return rec.flag & GSF_FRM_FLAG_IDR;
    }
    
    return 0;
}

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
  rec.flag = (pstStream->u32PackCount > 1)?GSF_FRM_FLAG_IDR:0;
  rec.seq  = pstStream->u32Seq;
  rec.utc  = _ts.tv_sec*1000 + _ts.tv_nsec/1000000;
  rec.pts  = pstStream->pstPack[0].u64PTS/1000;
  rec.video.encode = codec_ipc.venc[mgr->vst].type;
  rec.video.width  = codec_ipc.venc[mgr->vst].width;
  rec.video.height = codec_ipc.venc[mgr->vst].height;
  rec.size = 0;
  
  
  for (i = 0; i < pstStream->u32PackCount && i < GSF_FRM_NAL_NUM; i++)
  {
    rec.video.nal[i] = pstStream->pstPack[i].u32Len - pstStream->pstPack[i].u32Offset;
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

  for (i = 0; i < pstStream->u32PackCount; i++)
  {
    p = pstStream->pstPack[i].pu8Addr + pstStream->pstPack[i].u32Offset;
    a = pstStream->pstPack[i].u32Len - pstStream->pstPack[i].u32Offset;
    
    l = CFIFO_MIN(a, _n1);
    memcpy(_p1, p, l);
    memcpy(_p2, p+l, a-l);
    _n1-=l;_p1+=l;_p2+=a-l;
    //printf("a:%d, _n1:%d, _p1+:%d, _p2+:%d\n", a, _n1, l, a-l);
  }
  //printf("\n------------------\n");
  return 0;
}


static inline int venc_sdp_fill(VENC_CHN VeChn, PAYLOAD_TYPE_E pt, VENC_PACK_S* pstPack, int pack_size)
{
  switch(pt)
  {
    case PT_H264:
      {
        
        int sdp_val = (pstPack->DataType.enH264EType == H264E_NALU_SPS)? GSF_SDP_VAL_SPS:
                      (pstPack->DataType.enH264EType == H264E_NALU_PPS)? GSF_SDP_VAL_PPS:
                      (pstPack->DataType.enH264EType == H264E_NALU_SEI)? GSF_SDP_VAL_SEI: -1;
        if(sdp_val < 0)
        {
          break;
        }
        assert(pack_size <= sizeof(venc_mgr[VeChn].val[sdp_val].data));
        venc_mgr[VeChn].val[sdp_val].size =pack_size-4;
        memcpy(venc_mgr[VeChn].val[sdp_val].data, pstPack->pu8Addr + pstPack->u32Offset+4, venc_mgr[VeChn].val[sdp_val].size);
        break;
      }
    case PT_H265:
      {
        break; 
      }
    default:
      break;
  }
  return 0;
}



static int venc_recv(VENC_CHN VeChn, PAYLOAD_TYPE_E PT, VENC_STREAM_S* pstStream, void* uargs)
{
  HI_S32 i = 0, len = 0;
  
  for (i = 0; i < pstStream->u32PackCount; i++)
  {
    int pack_size = pstStream->pstPack[i].u32Len - pstStream->pstPack[i].u32Offset;
    len += pack_size;
    
    //sdp fill;
    if(pstStream->u32PackCount > 1)
      venc_sdp_fill(VeChn, PT, &pstStream->pstPack[i], pack_size);
  }
  
  if(len+sizeof(gsf_frm_t) > FRAME_MAX_SIZE)
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
  if(cost > 5)
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


int main(int argc, char *argv[])
{
    gsf_bsp_def_t bsp_def;
    if(argc < 2)
    {
      printf("pls input: %s codec_parm.json\n", argv[0]);
      return -1;
    }
    
    if(json_parm_load(argv[1], &codec_ipc) < 0)
    {
      json_parm_save(argv[1], &codec_ipc);
      json_parm_load(argv[1], &codec_ipc);
    }
    info("parm.venc[0].type:%d\n", codec_ipc.venc[0].type);
    
    while(1)
    {
      //register To;
      GSF_MSG_DEF(gsf_mod_reg_t, reg, 8*1024);
      reg->mid = GSF_MOD_ID_CODEC;
      strcpy(reg->uri, GSF_IPC_CODEC);
      int ret = GSF_MSG_SENDTO(GSF_ID_MOD_CLI, 0, SET, GSF_CLI_REGISTER, sizeof(gsf_mod_reg_t), GSF_IPC_BSP, 2000);
      printf("GSF_CLI_REGISTER To:%s, ret:%d, size:%d\n", GSF_IPC_BSP, ret, __rsize);
      
      static int cnt = 3;
      if(ret == 0)
        break;
      if(cnt-- < 0)
        return -1;
      sleep(1);
    }
    if(1)
    {
      //get bsp_def;
      GSF_MSG_DEF(gsf_msg_t, msg, 4*1024);
      int ret = GSF_MSG_SENDTO(GSF_ID_BSP_DEF, 0, GET, 0, 0, GSF_IPC_BSP, 2000);
      gsf_bsp_def_t *cfg = (gsf_bsp_def_t*)__pmsg->data;
      printf("GET GSF_ID_BSP_DEF To:%s, ret:%d, model:%s\n", GSF_IPC_BSP, ret, cfg->board.model);
    
      if(ret < 0)
        return 0;
        
     bsp_def = *cfg;
        
    }
    
    GSF_LOG_CONN(0, 100);
    void* rep = nm_rep_listen(GSF_IPC_CODEC, NM_REP_MAX_WORKERS, NM_REP_OSIZE_MAX, req_recv);

    memset(venc_mgr, 0, sizeof(venc_mgr));
    venc_mgr[0].vst = 0;
    venc_mgr[1].vst = 1;
    venc_mgr[2].vst = 2;
    venc_mgr[0].video_fifo = cfifo_alloc(2*1024*1024, cfifo_recsize, cfifo_rectag, cfifo_recrel, &venc_mgr[0].video_shmid, 0);
    venc_mgr[1].video_fifo = cfifo_alloc(0.5*1024*1024, cfifo_recsize, cfifo_rectag, cfifo_recrel, &venc_mgr[1].video_shmid, 0);


    gsf_mpp_cfg_t cfg = {
        .snsname = "xxx",
        .snscnt = 1,
        .lane = 0,
        .wdr  = 0,
        .res  = 4,
        .fps  = 30,
    };
    
    strcpy(cfg.snsname, bsp_def.board.sensor[0]);
    
    gsf_mpp_cfg(NULL, &cfg);
    
    gsf_mpp_vi_t vi = {
        .bLowDelay = HI_FALSE,
        .u32SupplementConfig = 0,
    };
    gsf_mpp_vi_start(&vi);
    
    gsf_mpp_vpss_t vpss = {
        .VpssGrp = 0,
        .ViPipe  = 0,
        .ViChn   = 0, 
        .enable  = {1, 1,},
        .enSize  = {PIC_2592x1536, PIC_720P,},
    };
    gsf_mpp_vpss_start(&vpss);

    char scene_ini[128];
    sprintf(scene_ini, "/var/app/cfg/%s.ini", bsp_def.board.sensor[0]);
    gsf_mpp_scene_start(scene_ini, 0);

    #define PIC_WIDTH(w) \
                (w >= 1920)?PIC_1080P:\
                (w >= 1280)?PIC_720P: \
                PIC_D1_NTSC
    
    int i = 0;
    for(i = 0; i < GSF_CODEC_VENC_NUM; i++)
    {
      if(!codec_ipc.venc[i].en)
        break;

      gsf_mpp_venc_t venc = {
        .VencChn = i,
        .VpssGrp = 0,
        .VpssChn = i,           //stream;
        .enPayLoad  = PT_H264,
        .enSize     = PIC_WIDTH(codec_ipc.venc[i].width), //vpss.enSize[0],
        .enRcMode   = SAMPLE_RC_CBR,
        .u32Profile = 0,
        .bRcnRefShareBuf = HI_TRUE,
        .enGopMode = VENC_GOPMODE_NORMALP,
      };
      
      gsf_mpp_venc_start(&venc);
    }
    printf(">>>>>>>> gsf_mpp_venc_recv i:%d\n", i);
    gsf_mpp_recv_t st = {
      .s32Cnt = i,
      .VeChn = {0,1,2},
      .uargs = NULL,
      .cb = venc_recv,
    };
    gsf_mpp_venc_recv(&st);
    
    
    gsf_osd_t osd;
    osd.en = 1;
    osd.type = 0;
    osd.fontsize = 0;
    osd.point[0] = 10;
    osd.point[1] = 10;


    gsf_vmask_t vmask;
    vmask.en = 1;
    vmask.color = 0x0000ffff;
    vmask.rect[0] = 100;
    vmask.rect[1] = 100;
    vmask.rect[2] = 200;
    vmask.rect[3] = 200;
    
    while(1)
    {
      time_t rawtime;
      time(&rawtime);
      struct tm *info = localtime(&rawtime);
      strftime(osd.text, 80, "%Y-%m-%d %H:%M:%S", info);
      
      osd.point[0] += 100;
      osd.point[0] %= 2000;
      osd.point[1] += 100;
      osd.point[1] %= 2000;
      printf("osd: x:%d,y:%d\n", osd.point[0], osd.point[1]);
      gsf_rgn_osd_set(0, 0, &osd);
      
      sleep(1);
      
      vmask.rect[0] += 100;
      vmask.rect[0] %= 2000;
      vmask.rect[1] += 100;
      vmask.rect[1] %= 2000;
      printf("vmask: x:%d,y:%d,w:%d,h:%d\n", vmask.rect[0], vmask.rect[1], vmask.rect[2], vmask.rect[3]);
      gsf_rgn_vmask_set(0, 0, &vmask);
    }
      
    GSF_LOG_DISCONN();
    return 0;
}