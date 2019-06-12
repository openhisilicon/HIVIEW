#ifndef __cfg_h__
#define __cfg_h__

#include "codec.h"


extern gsf_codec_parm_t codec_parm;

int jscon_parm_load(char *filename, gsf_codec_parm_t *cfg);
int jscon_parm_save(char *filename, gsf_codec_parm_t *cfg);

#endif