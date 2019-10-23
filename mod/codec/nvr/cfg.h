#ifndef __cfg_h__
#define __cfg_h__

#include "codec.h"


extern gsf_codec_nvr_t codec_nvr;

int json_parm_load(char *filename, gsf_codec_nvr_t *cfg);
int json_parm_save(char *filename, gsf_codec_nvr_t *cfg);

#endif