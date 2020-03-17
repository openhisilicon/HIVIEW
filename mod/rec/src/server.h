#ifndef __server_h__
#define __server_h__

#include "rec.h"
#include "file.h"
#include "fw/tindex/inc/tindex.h"

int ser_init(void);
int ser_query_disk(gsf_disk_t disks[8]);
int ser_query_file(gsf_rec_q_t *q, gsf_file_t files[1024]);
int ser_cfg_update(gsf_rec_cfg_t *cfg);
int ser_open_vod(gsf_file_t *file, gsf_rec_media_t *media);

#endif //!__server_h__