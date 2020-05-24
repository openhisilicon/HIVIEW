#ifndef __nvr_live_h__
#define __nvr_live_h__

#include "msg_func.h"

int live_mon();

int live_chsrc_modify(int i, gsf_chsrc_t *chsrc);
int live_clear_rtsp();
int live_clear_onvif();

int live_get_shmid(int layout, int voch[GSF_CODEC_NVR_CHN]
                , int st, gsf_shmid_t shmid[GSF_CODEC_NVR_CHN]);
                
#endif