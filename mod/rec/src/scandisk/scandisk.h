/*
 * =====================================================================================
 *
 *       Filename:  scandisk.h
 *
 *    Description:  scan disk
 *
 *        Version:  1.0
 *        Created:  11/20/2012 05:38:38 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  maohw (), maohongwei@gmail.com
 *        Company:  gsf
 *         Modify:  
 *
 * =====================================================================================
 */
#ifndef scan_disk_h
#define scan_disk_h

#define MKDISKNO(ma, mi) ((ma)<<16|(mi))
#define SCSI_HOST       "/sys/class/scsi_host/"
#define SYS_BLOCK       "/sys/block/"
#define SYS_SCSI_DEV    "/sys/bus/scsi/devices/"
#define MMC_HOST        "/sys/class/mmc_host"

typedef enum _DISK_BUS_CLASS {     
      DISK_BUS_CLASS_UNKNOW = 0
    , DISK_BUS_CLASS_PCI  = 1
    , DISK_BUS_CLASS_USB  = 2
    , DISK_BUS_CLASS_IPSAN = 3
    , DISK_BUS_CLASS_MMC  = 4
    , DISK_BUS_CLASS_NFS  = 5
}DISK_BUS_CLASS_E;


typedef struct _scandisk_info_s
{
    char *dev_name;
    int bus_class;
    int hostno;
    int disk_no;
    int ma;
    int mi;
    int part_num;
    char *ptarget;
}scandisk_info_t;

typedef struct _scandisk_op_s
{
    void *uargs;
    int (*cb)(void *uargs, scandisk_info_t *info);
}scandisk_op_t;

int scan_disk(scandisk_op_t *pscandisk);

#endif


