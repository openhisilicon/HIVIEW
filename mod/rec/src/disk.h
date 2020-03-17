#ifndef __disk_h__
#define __disk_h__

/* gsf ´ÅÅÌ²Ù×÷ 
 * qq: 25088970 maohw
 */
#include "rec.h"

//#define DISK_FS_TYPE "ext4"
#define DISK_FS_TYPE "vfat"
#define DISK_SCAN_MNT_DIR "/tmp/scan_mnt"
#define DISK_FORMAT_MNT_DIR "/tmp/format_mnt"
#define DISK_CFG_FILENAME(mnt, p) sprintf(p, "%s/disk.cfg", mnt)


int disk_init(void);
int disk_uninit(void);

typedef struct gsf_disk_q_s
{
    void* uargs;
    int (*cb)(struct gsf_disk_q_s *owner, gsf_disk_t *disk);
}gsf_disk_q_t;

typedef struct gsf_disk_f_s
{
    gsf_disk_t *disk;
    void *uargs;
    int (*cb)(struct gsf_disk_f_s *owner, int percent);
}gsf_disk_f_t;

int disk_scan(gsf_disk_q_t *q);
int disk_format(gsf_disk_f_t *f);
int disk_mount(gsf_disk_t *d);


int disk_total(char *mnt); /* M */
int disk_left(char *mnt);  /* M */
int disk_percent(char *mnt);/* 0%-100% */

#endif //~__disk_h__
