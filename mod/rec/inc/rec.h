#ifndef __rec_h__
#define  __rec_h__

#ifdef __cplusplus
extern "C" {
#endif

#include "inc/gsf.h"

//for json cfg;
#include "mod/rec/inc/sjb_rec.h"

#define GSF_IPC_REC      "ipc:///tmp/rec_rep"

enum {
    GSF_ID_REC_QDISK = 1,
    GSF_ID_REC_FDISK = 2,
    GSF_ID_REC_QREC  = 3,
    GSF_ID_REC_START = 4,
    GSF_ID_REC_STOP  = 5,
    GSF_ID_REC_END
};

enum {
    GSF_REC_ERR = -1,
};

#ifdef __cplusplus
}
#endif

#endif