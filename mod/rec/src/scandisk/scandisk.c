/*
 * =====================================================================================
 *
 *       Filename:  scandisk.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  11/20/2012 05:48:35 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  
 *        Company:  sdk
 *         Modify:  
 *
 * =====================================================================================
 */
#include <string.h>
#include <dirent.h>
#include <errno.h>
#include <stdint.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <mntent.h>
#include <assert.h>
#include <sys/sysmacros.h>

#include "scan_block_device.h"
#include "scandisk.h"

#define PATH_MAX 256
#define MAX_DIR_NAME_LEN 256

#ifndef MKDEV
#define MKDEV(ma,mi)    ((ma)<<8 | (mi))
#endif

///#define printf(arg...) 

#define PROC_PART   "/proc/partitions"
#define PROC_MOUNTS "/proc/mounts"


typedef struct _scandisk_mng_s
{
    char sys_dev_name[MAX_DIR_NAME_LEN];
}scandisk_mng_t;

static scandisk_mng_t g_scandisk_mng;



static int _read_dir(char *pdirname, void *arg, int (*dodirname)(char *psubdirname, void *arg))
{
    DIR *hostdir = NULL;
    struct dirent *hostentry;
    int count = 0;
    int ret;

    hostdir = opendir(pdirname);
    if(hostdir == NULL)
    {
        printf("open dir %s err %s\n", SCSI_HOST, strerror(errno));
        return -1;
    }
    
    while((hostentry = readdir(hostdir)))
    {
        if(hostentry->d_name[0] == '.')
            continue;

        if(dodirname != NULL)
        {
            ret = dodirname(hostentry->d_name, arg);
            if(ret >= 0)
                count++;
        }
    }

    closedir(hostdir);

    return count;
}


static int _make_absolute_path(const char *path, char *retabspath)
{
#define MAXDEPTH 5

    char bufs[2][MAX_DIR_NAME_LEN], *buf = bufs[0], *next_buf = bufs[1];
    char cwd[MAX_DIR_NAME_LEN] = "";
    int buf_index = 1, len;
    int depth = 5;//MAXDEPTH;
    char *last_elem = NULL;
    struct stat st;

    strcpy(buf, path);

    while (depth--) {
        if (stat(buf, &st) || !S_ISDIR(st.st_mode)) {
            char *last_slash = strrchr(buf, '/');
            if (last_slash) {
                *last_slash = '\0';
                last_elem = strdup(last_slash + 1);
            } else {
                last_elem = strdup(buf);
                *buf = '\0';
            }
        }

        if (*buf) {
            if (!*cwd && !getcwd(cwd, sizeof(cwd)))
                printf ("Could not get current working directory");

            if (chdir(buf))
                printf ("Could not switch to '%s'", buf);
        }
        if (!getcwd(buf, PATH_MAX))
            printf ("Could not get current working directory");

        if (last_elem) {
            int len = strlen(buf);
            if (len + strlen(last_elem) + 2 > PATH_MAX)
                printf ("Too long path name: '%s/%s'",
                        buf, last_elem);
            buf[len] = '/';
            strcpy(buf + len + 1, last_elem);
            free(last_elem);
            last_elem = NULL;
        }

        if (!lstat(buf, &st) && S_ISLNK(st.st_mode)) {
            len = readlink(buf, next_buf, PATH_MAX);
            if (len < 0)
                printf ("Invalid symlink: %s", buf);
            next_buf[len] = '\0';
            buf = next_buf;
            buf_index = 1 - buf_index;
            next_buf = bufs[buf_index];
        } else
            break;
    }

    if (*cwd && chdir(cwd))
        printf ("Could not change back to '%s'", cwd);

    strcpy(retabspath, buf);
    ///printf("path %s abspath %s\n", path, retabspath);

    return 0;
}

static char *_analy_str(char dec, char **pstr)
{
    char *pos = NULL;

    while(**pstr)
    {
        if(dec == **pstr)
        {
            **pstr = 0;
            if(pos != NULL) 
            {
                (*pstr)++;
                break;
            }
        }
        else
        {
            if(pos == NULL)
                pos = *pstr;
        }
        (*pstr)++;
    }

    return pos;
}

static int get_dev_no(char *name, int *major, int *minor)
{
    int  len, fd;
    char temp[64];
    char file_name[MAX_DIR_NAME_LEN];

    snprintf(file_name, sizeof(file_name), "/sys/block/%s/dev", name);
    fd = open(file_name, O_RDONLY);
    if(fd < 0)
    {
        printf("open file %s err %s\n", temp, strerror(errno));
        exit(-1);
        return -1;
    }
    len = read(fd, temp, sizeof(temp));
    close(fd);
    if (len < 1)
    {
        exit(-1);
        printf("read %s len %d err %s\n", file_name, len, strerror(errno));
        return -1;
    }

    ///printf("read file %s %s\n", file_name, temp);
    if (sscanf(temp, "%d:%d", major, minor) != 2)
        return -1;

    return 0;
}

static int _analy_disk_dirname(char *name, void *arg)
{
    int *ppart_num;
    char *pstr;
    int partno;

    ppart_num = ((int*)arg);
    if((pstr = strstr(name, "sd")) != NULL) 
    {
        partno = atoi(name+3); 
        printf("analy disk partno %d partname %s\n",  partno, name);
    }
    else if((pstr = strstr(name, "mmc")) != NULL)
    {
        partno = atoi(name+5);
    }
    else
        return -1;

    (*ppart_num)++;

    return 0;
}

char *default_disk_feature[]=
{
    "mmc",
    "sd",
    NULL
};


int file_is_exist(char *pfile_name)
{
    int ret;
    struct stat t_stat;

    ret = stat(pfile_name, &t_stat);
    if(ret < 0)
        return 0;
    else 
        return 1;
}

static int _analy_block_dev_dirname(char *name, void *arg)
{
    int hostno = -1;
    int usbno = 0;
    int mmcno = 0;
    int disk_no = -1;
    int hostbustype;
    char dirbuf[MAX_DIR_NAME_LEN];
    char abspath[MAX_DIR_NAME_LEN];
    char *curpos, *pstrpos;
    //int ret;
    char target[8] = {0};
    //char scheduler_buf_cmd[64];
    ///int i;
    int ma, mi;
    ///int flag;
    int part_num;
    scandisk_op_t *pscandisk = (scandisk_op_t *)arg;
    scandisk_info_t scandisk_info;

    sprintf(dirbuf, SYS_BLOCK"%s/device", name);
    if(file_is_exist(dirbuf))
    {
        _make_absolute_path(dirbuf, abspath);
        //printf("block device %s analy  abspath %s\n", dirbuf, abspath);
        curpos = abspath;
        hostbustype = DISK_BUS_CLASS_UNKNOW;
        while((pstrpos = _analy_str('/', &curpos)))
        {
            if(strstr(pstrpos, "host"))
            {
                hostno = atoi(pstrpos+4);
            }

            if(strstr(pstrpos, "pci") != NULL)
            {
                hostbustype = DISK_BUS_CLASS_PCI;
            }

            if(strstr(pstrpos, "usb") != NULL)
            {
                hostbustype = DISK_BUS_CLASS_USB;
                usbno = atoi(pstrpos+3);
            }

            if(strstr(pstrpos, "session") != NULL)
            {
                hostbustype = DISK_BUS_CLASS_IPSAN;
            }

            if(strstr(pstrpos, "mmc") != NULL)
            {
                hostbustype = DISK_BUS_CLASS_MMC;
                mmcno = atoi(pstrpos+3);
            }

            /*local disk pos */
            if(strstr(pstrpos, "target") != NULL)
            {
                memcpy(target, pstrpos+6, 5);
                target[5] = 0;
            }
        }

        if(hostbustype == DISK_BUS_CLASS_USB)
        {
            hostno = usbno;
        }
        else if(hostbustype == DISK_BUS_CLASS_MMC) 
        {
            hostno = mmcno; 
        }
    }
    //    sprintf(scheduler_buf_cmd, "echo \"%s\">/sys/block/%s/queue/scheduler", STOR_SYS_IO_SCHEDULER, name);
    //    system(scheduler_buf_cmd);

    sprintf(dirbuf, SYS_BLOCK"%s", name);

    get_dev_no(name, &ma, &mi);
    ///printf("read dir %s major %d minor %d\n", dirbuf, ma, mi);

    disk_no = MKDISKNO(ma, mi);//mi / BLOCK_MAIN_DEV_NO_MUL;
    part_num = 0;
    _read_dir(dirbuf, &part_num, _analy_disk_dirname); 
    ///printf("disk no %d part num %d\n", disk_no, part_num);

    sprintf(dirbuf, "/dev/%s", name);
    scandisk_info.dev_name      = dirbuf;
    scandisk_info.bus_class     = hostbustype;
    scandisk_info.hostno        = hostno;
    scandisk_info.disk_no       = disk_no;
    scandisk_info.ma            = ma;
    scandisk_info.mi            = mi;
    scandisk_info.part_num      = part_num;
    scandisk_info.ptarget       = target;
    pscandisk->cb(pscandisk->uargs, &scandisk_info);
        //dirbuf, hostbustype
         //   , hostno, disk_no, ma, mi, part_num, target);

    return 0;
}

int get_sys_dev_name(char *retsys_dev_name)
{
    struct mntent mtent;
    FILE *mountTable = setmntent(PROC_MOUNTS, "r");
    char getmntent_buf[512];

    while (getmntent_r(mountTable, &mtent, getmntent_buf
                , sizeof(getmntent_buf)))
    {
#ifdef DEBUG
            printf("%s on %s type %s (%s)\n", mtent.mnt_fsname,
                    mtent.mnt_dir, mtent.mnt_type,
                    mtent.mnt_opts);
#endif
            if(strcmp(mtent.mnt_dir, "/") == 0 && strstr(mtent.mnt_fsname, "/dev") != NULL)
            {
                _make_absolute_path(mtent.mnt_fsname, retsys_dev_name);
                retsys_dev_name[strlen("/dev/sda")] = 0;
                printf("ret sys dev name %s\n", retsys_dev_name);
                break;
            }
    }
    
    endmntent(mountTable);

    return 0;
}



int scandisk_init()
{
    ///int i;

    memset(&g_scandisk_mng, 0, sizeof(g_scandisk_mng));
    get_sys_dev_name(g_scandisk_mng.sys_dev_name);

    return 0;
}



char *scandisk_get_sys_dev_name()
{
    return g_scandisk_mng.sys_dev_name;
}



typedef struct __analy_scan_disk_s
{
    scandisk_op_t *pscandisk;
}_analy_scan_disk_t;


static inline int _analy_scan_disk_res(char *name, void *u)
{
    //_analy_scan_disk_t *panaly_scan_disk;

   return _analy_block_dev_dirname(name, u);
     
}

int scan_disk(scandisk_op_t *pscandisk)
{
    return scan_block_device(DENT_ALL_UNUSED, _analy_scan_disk_res, (void*)pscandisk);
}


