#ifndef __venc_h__
#define __venc_h__

#include "codec.h"
#include "mpp.h"
#include "cfg.h"
#include "msg_func.h"

#if defined(GSF_CPU_3531d) || defined(GSF_CPU_3403) || defined(GSF_CPU_3519d)
#define venc_seq        pstStream->seq
#define venc_pts        pstStream->pack[0].pts
#define venc_pack_count pstStream->pack_cnt
#define venc_pack       pstStream->pack[i]
#define venc_pack_addr  pstStream->pack[i].addr
#define venc_pack_len   pstStream->pack[i].len
#define venc_pack_off   pstStream->pack[i].offset

#define aenc_seq pstStream->seq
#define aenc_pts pstStream->time_stamp
#define aenc_len pstStream->len
#define aenc_addr pstStream->stream

#define pack_addr pstPack->addr
#define pack_off  pstPack->offset
#define pack_264_type pstPack->data_type.h264_type
#define pack_265_type pstPack->data_type.h265_type

#else
#define venc_seq        pstStream->u32Seq
#define venc_pts        pstStream->pstPack[0].u64PTS
#define venc_pack_count pstStream->u32PackCount
#define venc_pack       pstStream->pstPack[i]
#define venc_pack_addr  pstStream->pstPack[i].pu8Addr
#define venc_pack_len   pstStream->pstPack[i].u32Len
#define venc_pack_off   pstStream->pstPack[i].u32Offset

#define aenc_seq pstStream->u32Seq
#define aenc_pts pstStream->u64TimeStamp
#define aenc_len pstStream->u32Len
#define aenc_addr pstStream->pStream

#define pack_addr pstPack->pu8Addr
#define pack_off  pstPack->u32Offset
#define pack_264_type pstPack->DataType.enH264EType
#define pack_265_type pstPack->DataType.enH265EType
#endif

typedef struct {
  int ch_num;
  int st_num;
}gsf_venc_ini_t;

int gsf_venc_init(gsf_venc_ini_t *ini);

int gsf_venc_recv(VENC_CHN VeChn, PAYLOAD_TYPE_E PT, VENC_STREAM_S* pstStream, void* uargs);

int gsf_aenc_recv(int AeChn, PAYLOAD_TYPE_E PT, AUDIO_STREAM_S* pstStream, void* uargs);

int gsf_venc_usei(int (*sei_fill)(char *buf, int size));

int gsf_venc_set_osd_time_idx(int ch, int idx, int x, int y);

#endif // __venc_h__