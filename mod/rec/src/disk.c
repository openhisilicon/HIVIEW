/* gsf ´ÅÅÌ²Ù×÷ */
#define _LARGEFILE64_SOURCE
#define _FILE_OFFSET_BITS 64
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <assert.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mount.h>
#include <unistd.h>
#include <errno.h>
#include <linux/fs.h>
#include <fcntl.h>


#include "list.h"

#include "disk.h"
#include "rec.h"
#include "scandisk.h"

#define TRUE 1
#define FALSE 0

typedef struct gsf_cfg_s    // 8byte
{
    int grp_id;             // ÅÌ×éID [0 - GFS_GRP_MAX_NUM]
    int disk_id;            // ´ÅÅÌID [1 - ...]
}gsf_cfg_t;



typedef struct disk_s
{
    struct list_head list;
    gsf_disk_t disk;
}disk_t;

typedef struct disk_mng_s 
{
    int max_disk_id;
    struct list_head disks;
}disk_mng_t;


static disk_mng_t mng;

int disk_init(void)
{
    mng.max_disk_id = 0;
    INIT_LIST_HEAD(&mng.disks);
    
    if(access(DISK_SCAN_MNT_DIR, 0) < 0)
        mkdir(DISK_SCAN_MNT_DIR, 0666);
        
    if(access(DISK_FORMAT_MNT_DIR, 0) < 0)
        mkdir(DISK_FORMAT_MNT_DIR, 0666);
    
    return 0;
}

int disk_uninit(void)
{
    disk_t *n, *nn;

    list_for_each_entry_safe(n, nn, &mng.disks, list)
    {
        list_del(&n->list);
        free(n);
    }
    return 0;
}


static int
valid_offset (int fd, long long offset)
{
  char ch;

  if (lseek64 (fd, offset, SEEK_SET) < 0)
    return FALSE;
  if (read (fd, &ch, 1) < 1)
    return FALSE;
  return TRUE;
}

static unsigned long long
count_blocks (char *filename)
{
  long long high, low;
  int fd;

  if ((fd = open (filename, O_RDONLY)) < 0)
    {
      perror (filename);
      exit (1);
    }

  /* first try SEEK_END, which should work on most devices nowadays */
  if ((low = lseek64(fd, 0, SEEK_END)) <= 0) {
      low = 0;
      for (high = 1; valid_offset (fd, high); high *= 2)
	  low = high;
      while (low < high - 1) {
	  const long long mid = (low + high) / 2;
	  if (valid_offset (fd, mid))
	      low = mid;
	  else
	      high = mid;
      }
      ++low;
  }

  close (fd);
  //printf("disk size %lld\n", low);
  return low / BLOCK_SIZE;
}

int scan_disk_cb(void *uargs, scandisk_info_t *info)
{
    printf("scan [dev:%s, bus:%d, hostno:%d, ma:%d, mi:%d, part:%d, target:%s]\n"
        , info->dev_name
        , info->bus_class
        , info->hostno
        , info->ma
        , info->mi
        , info->part_num
        , info->ptarget
        );
    
    disk_t *n;

    list_for_each_entry(n, &mng.disks, list)
    {
        if(strcmp(n->disk.dev_name, info->dev_name) == 0)
        {
            return 0;
        }
    }
    
    
    n = (disk_t *)calloc(1, sizeof(disk_t));
    n->disk.grp_id  = (~0);
    n->disk.disk_id = (~0);

    strncpy(n->disk.dev_name, info->dev_name, sizeof(n->disk.dev_name)-1);
    n->disk.part_num = info->part_num;
    n->disk.size = count_blocks(info->dev_name)/(1024*1024/BLOCK_SIZE);
    
    #if 0
    umount2(DISK_SCAN_MNT_DIR, MNT_FORCE);
    #else
    char um[256];
    sprintf(um, "umount %s", n->disk.dev_name);
    system(um);
    #endif
    
    if(mount(info->dev_name, DISK_SCAN_MNT_DIR, DISK_FS_TYPE, MS_NOATIME, NULL) == 0)
    {
        char filename[256];
        DISK_CFG_FILENAME(DISK_SCAN_MNT_DIR, filename);
        
        int fd;
        gsf_cfg_t cfg;
        memset(&cfg, 0, sizeof(gsf_cfg_t));
        
        if(access(filename, 0) == 0
            && (fd = open(filename, O_RDWR | O_CREAT, 0766)))
        {
            read(fd, &cfg, sizeof(gsf_cfg_t));
            
            n->disk.grp_id  = cfg.grp_id;
            n->disk.disk_id = cfg.disk_id;
            close(fd);
        }
        
        umount2(DISK_SCAN_MNT_DIR, MNT_FORCE);
    }
    else
    {
      printf("%s => err mount dev_name:%s, mnt_dir:%s, fs:%s.\n", __func__, info->dev_name, DISK_SCAN_MNT_DIR, DISK_FS_TYPE);
    }
    
    if(n->disk.disk_id > mng.max_disk_id)
        mng.max_disk_id = n->disk.disk_id;
 
    list_add(&n->list, &mng.disks);

    return 0;
}


int disk_scan(gsf_disk_q_t *q)
{
    scandisk_op_t op;
    op.uargs = &mng;
    op.cb = scan_disk_cb;
    scan_disk(&op);
 
    disk_t *n;

    list_for_each_entry(n, &mng.disks, list)
    {
        q->cb(q, &n->disk);
    }
    
    return 0;
}

#if (defined __i386__ || defined __x86_64__)
    //#define _XOPEN_SOURCE 600
    //#include <fcntl.h>
#else
    #if 0
      #include <sys/syscall.h>
      #ifdef SYS_fallocate
      int posix_fallocate(int fd, int offset, int len)
      {
          return syscall(SYS_fallocate, fd, 0, offset, 0, len, 0);
      }
      #endif
    #endif
#endif

static int mkfs_percent(int percent, gsf_disk_f_t *f)
{
  if(f->cb)
  {
    f->cb(f, percent);
  }
  return 0;
}

int disk_mkvfat(char *partname, int (*cb)(int percent, void *u), void *u)
{
  int p = 10;
  char cmd[256] = {0};
  sprintf(cmd, "mkfs.vfat %s", partname);
  printf("%s => [%s]\n", __func__, cmd);
  FILE *fd = popen(cmd, "r");
  
  if(cb)
  {
    cb(p+=10, u);
  }
  
  memset(cmd, 0, sizeof(cmd));
  while(fgets(cmd, sizeof(cmd), fd))
  {
    //printf("mkfs.vfat:[%s]", cmd);
    if(cb)
    {
      if(strstr(cmd, "done"))cb(p+=10, u);
    }
    memset(cmd, 0, sizeof(cmd));
  }
  
  if(cb)
  {
    cb(p+=10, u);
  }
  
  return pclose(fd);
}


int disk_mkext4(char *partname, int (*cb)(int percent, void *u), void *u)
{
  int p = 10;
  char cmd[256] = {0};
  sprintf(cmd, "mkfs.ext4 -E lazy_itable_init=1,lazy_journal_init=1 -F %s", partname);
  printf("%s => [%s]\n", __func__, cmd);
  FILE *fd = popen(cmd, "r");
  
  if(cb)
  {
    cb(p+=10, u);
  }
  
  memset(cmd, 0, sizeof(cmd));
  while(fgets(cmd, sizeof(cmd), fd))
  {
    //printf("mkfs.ext4:[%s]", cmd);
    if(cb)
    {
      if(strstr(cmd, "done"))cb(p+=10, u);
    }
    memset(cmd, 0, sizeof(cmd));
  }
  
  if(cb)
  {
    cb(p+=10, u);
  }
  
  return pclose(fd);
}

int disk_format(gsf_disk_f_t *f)
{
    gsf_disk_t *disk = f->disk;
    char filename[256] = {0};
    
    disk->disk_id = (disk->disk_id != (~0))?disk->disk_id:++mng.max_disk_id;
    disk->grp_id = (disk->grp_id != (~0))?disk->grp_id:0;
    
    mkfs_percent(1, f);
    
    if(strlen(disk->dev_name))
    {
        if(strstr(DISK_FS_TYPE, "ext4"))
        {
          disk->part_num = 0;
          disk_mkext4(disk->dev_name, (int(*)(int,void*))mkfs_percent, f);
        }
        else if(strstr(DISK_FS_TYPE, "vfat"))
        {
          disk->part_num = 0;
          disk_mkvfat(disk->dev_name, (int(*)(int,void*))mkfs_percent, f);
        }

        // mount;
        sprintf(disk->mnt_dir, "%s", DISK_FORMAT_MNT_DIR);
        umount2(disk->mnt_dir, MNT_FORCE);
        if(mount(disk->dev_name, disk->mnt_dir, DISK_FS_TYPE, MS_NOATIME, NULL) < 0)
        {
          printf("%s => err mount dev_name:%s, mnt_dir:%s, fs:%s.\n", __func__, disk->dev_name, disk->mnt_dir, DISK_FS_TYPE);
          return -1;
        }
    }
    
    if(!strlen(disk->mnt_dir))
    {
        return -2;
    }
    
    { // cfg;
        DISK_CFG_FILENAME(disk->mnt_dir, filename);
        int fd = open(filename, O_RDWR | O_CREAT, 0766);
        gsf_cfg_t cfg;
        cfg.disk_id = disk->disk_id;
        cfg.grp_id = disk->grp_id;
        write(fd, &cfg, sizeof(gsf_cfg_t));
        close(fd);
        mkfs_percent(99, f);
    }
    
    sync();
    
    if(strlen(disk->dev_name))
    {
        umount2(disk->mnt_dir, MNT_FORCE);
    }
    
    mkfs_percent(100, f);
    return 0;
}



int disk_mount(gsf_disk_t *d)
{
  if(!strlen(d->mnt_dir))
  {
    return -2;
  }
  
  if(strlen(d->dev_name))
  {
    umount2(d->mnt_dir, MNT_FORCE);
    if(mount(d->dev_name, d->mnt_dir, DISK_FS_TYPE, MS_NOATIME, NULL) < 0)
    {
      printf("%s => err mount dev_name:%s, mnt_dir:%s, fs:%s.\n", __func__, d->dev_name, d->mnt_dir, DISK_FS_TYPE);
      return -1;
    }
  }
  return 0;
}

#include <sys/statvfs.h>

/* ´ÅÅÌ²Ù×÷ */
int disk_total(char *mnt)
{
	int ret = 0;
	struct statvfs stat_buf;
	memset(&stat_buf, 0, sizeof(stat_buf));
	
	ret = statvfs(mnt, &stat_buf);
  if(ret <0)
	{
    printf("disk_total statvfs mnt:%s, errno:%d\n", mnt, errno);
    return -1;
  }

  unsigned long long size = stat_buf.f_bsize;
  size *= stat_buf.f_blocks;
  size /= (1024*1024);

  printf("disk_total statvfs mnt:%s, total:%lld M\n", mnt, size);

	return size;
}

int disk_left(char *mnt)
{
	int ret = 0;
	struct statvfs stat_buf;
	memset(&stat_buf, 0, sizeof(stat_buf));

	ret = statvfs(mnt, &stat_buf);
  if(ret <0)
	{
    printf("disk_left statvfs mnt:%s, errno:%d\n", mnt, errno);
    return -1;
  }

  unsigned long long size = stat_buf.f_bsize;
  size *= stat_buf.f_bavail;
  size /= (1024*1024);
  
  printf("disk_left statvfs mnt:%s, left:%lld M\n", mnt, size);

	return size;
}



int disk_percent(char *mnt)
{
	int ret = 0;
	struct statvfs stat_buf;
	memset(&stat_buf, 0, sizeof(stat_buf));

	ret = statvfs(mnt, &stat_buf);
  if(ret <0)
	{
    printf("disk_left statvfs mnt:%s, errno:%d\n", mnt, errno);
    return -1;
  }

  double percent = (stat_buf.f_blocks - stat_buf.f_bavail);
  percent /= stat_buf.f_blocks;
  percent *= 100;
  printf("disk_percent statvfs mnt:%s, percent:%d%\n", mnt, (int)percent);
	return percent;
}



