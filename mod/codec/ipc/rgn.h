#ifndef __rgn_h__
#define __rgn_h__

#include "codec.h"

typedef struct {
  int st_num;
}gsf_rgn_ini_t;

int gsf_rgn_init(gsf_rgn_ini_t *ini);

int gsf_rgn_osd_set(int ch, int idx, gsf_osd_t *osd);

#endif