#include "rtp-media.h"

enum {
  MEDIA_TRACK_VIDEO = 0, // a=control:video
  MEDIA_TRACK_AUDIO = 1, // a=control:audio
  MEDIA_TRACK_META  = 2, // a=control:meta
  MEDIA_TRACK_BUTT
};

struct rtp_media_file_t
{
  struct rtp_media_t base;
  // private;
  struct rtp_transport_t* rtp_transport[MEDIA_TRACK_BUTT];
};

static int rtp_file_get_sdp(struct rtp_media_t* _m, char *sdp, int fmt, int ssrc)
{
  struct rtp_media_file_t *m = (struct rtp_media_file_t*)_m;
  return 0;
}

static int rtp_file_get_duration(struct rtp_media_t* _m, int64_t* duration)
{
  struct rtp_media_file_t *m = (struct rtp_media_file_t*)_m;
  return 0;
}

static int rtp_file_play(struct rtp_media_t* _m)
{
  struct rtp_media_file_t *m = (struct rtp_media_file_t*)_m;
  return 0;
}

static int rtp_file_pause(struct rtp_media_t* _m)
{
  struct rtp_media_file_t *m = (struct rtp_media_file_t*)_m;
  return 0;
}

static int rtp_file_set_speed(struct rtp_media_t* _m, double speed)
{
  struct rtp_media_file_t *m = (struct rtp_media_file_t*)_m;
  return 0;
}

static int rtp_file_seek(struct rtp_media_t* _m, int64_t pos)
{
  struct rtp_media_file_t *m = (struct rtp_media_file_t*)_m;
  return 0;
}


static int rtp_file_get_rtpinfo(struct rtp_media_t* _m, const char* uri, char *rtpinfo, size_t bytes)
{
  struct rtp_media_file_t *m = (struct rtp_media_file_t*)_m;
  return 0;
}


static int rtp_file_add_transport(struct rtp_media_t* _m, const char* track, struct rtp_transport_t* transport)
{
  struct rtp_media_file_t *m = (struct rtp_media_file_t*)_m;
  return 0;
}


struct rtp_media_t* rtp_media_file_new(int ch, int st, int64_t beg, int64_t end)
{
  struct rtp_media_file_t *mf = calloc(1, sizeof(struct rtp_media_file_t));
 
  mf->base.get_sdp        = rtp_file_get_sdp;      
  mf->base.get_duration   = rtp_file_get_duration;
  mf->base.play           = rtp_file_play;
  mf->base.pause          = rtp_file_pause;        
  mf->base.set_speed      = rtp_file_set_speed;
  mf->base.seek           = rtp_file_seek;   
  mf->base.get_rtpinfo    = rtp_file_get_rtpinfo;
  mf->base.add_transport  = rtp_file_add_transport;
  
  // private beg, end;
  
  return (struct rtp_media_t*)mf;
}

int rtp_media_file_free(struct rtp_media_t* m)
{
  struct rtp_media_file_t *mf = (struct rtp_media_file_t*)m;
  if(mf)
  {
    free(mf); 
  }
}
