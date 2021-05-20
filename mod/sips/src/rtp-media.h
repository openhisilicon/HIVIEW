#ifndef _rtp_media_h_
#define _rtp_media_h_

#include "rtp-profile.h"
#include "rtp-payload.h"
#include "rtp-transport.h"

struct rtp_media_t
{
  int (*get_sdp)(struct rtp_media_t* m, char *sdp);
  int (*get_duration)(struct rtp_media_t* m, int64_t* duration);
  int (*play)(struct rtp_media_t* m);
  int (*pause)(struct rtp_media_t* m);
  int (*set_speed)(struct rtp_media_t* m, double speed);
  int (*seek)(struct rtp_media_t* m, int64_t pos);
  int (*get_rtpinfo)(struct rtp_media_t* m, const char* uri, char *rtpinfo, size_t bytes);
  int (*add_transport)(struct rtp_media_t* m, const char* track, struct rtp_transport_t* transport);
};

struct rtp_media_t* 
    rtp_media_live_new(int ch, int st);
int rtp_media_live_free(struct rtp_media_t* m);

struct rtp_media_t* 
    rtp_media_file_new(int ch, int st, int64_t beg, int64_t end);
int rtp_media_file_free(struct rtp_media_t* m);

#endif /* !_rtp_media_h_ */
