#ifndef __rgn_h__
#define __rgn_h__

#include "codec.h"
#include "fw/libfont/inc/gsf_font_draw.h"
#include "fw/libfont/inc/gsf_character_conv.h"

typedef struct {
  int ch_num;
  int st_num;
}gsf_rgn_ini_t;

int gsf_rgn_init(gsf_rgn_ini_t *ini);

int gsf_rgn_osd_set(int ch, int idx, gsf_osd_t *osd);

int gsf_rgn_vmask_set(int ch, int idx, gsf_vmask_t *vmask);

int gsf_rgn_osd_set_ex(int ch, int idx, gsf_osd_t* osds, int osd_count);
#endif // __rgn_h__