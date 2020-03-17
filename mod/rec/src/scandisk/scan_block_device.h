#ifndef __SCAN_BLOCK_DEVICE_H__
#define __SCAN_BLOCK_DEVICE_H__

#define DENT_FLGS_SLAVE				0x01
#define DENT_FLGS_IGNORE			0x02
#define DENT_FLGS_OSUSED			0x04
#define DENT_ALL_UNUSED		(DENT_FLGS_SLAVE|DENT_FLGS_IGNORE|DENT_FLGS_OSUSED)

typedef int (*disk_func)(char *name, void *u);
int scan_block_device(unsigned mask, disk_func f, void *u);

#endif	/* __SCAN_BLOCK_DEVICE_H__ */
