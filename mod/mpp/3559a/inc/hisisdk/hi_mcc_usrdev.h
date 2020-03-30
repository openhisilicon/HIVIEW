/*
 *    This file defines data structures and interfaces for 
 *    hisilicon Multiple chips communication.
 *
 *    Copyright (C) 2008 hisilicon , chanjinn@hauwei.com
 *
 *    This program is free software; you can redistribute it and/or modify
 *    it under the terms of the GNU General Public License as published by
 *    the Free Software Foundation; either version 2 of the License, or
 *    (at your option) any later version.
 *
 *    This program is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License
 *    along with this program; if not, write to the Free Software
 *    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 *
 *    MCC is short for Multi-Chips-Communication
 *
 *    Create by Hisicon, 2011.1.1
 */

#include <linux/ioctl.h>

#ifndef __HI_MCC_USERDEV_HEAD__
#define __HI_MCC_USERDEV_HEAD__

#define MCC_USR_MODULE_NAME		"MCC_USR"
#define MCC_USR_VERSION			__DATE__", "__TIME__

#define HISI_MAX_MAP_DEV		0x1f

#define NORMAL_MESSAGE_SIZE		128

#define MAXIMAL_MESSAGE_SIZE		0x1000

struct hi_mcc_handle_attr {
	int target_id;
	int port;
	int priority;

	/* to store remote device ids */
	int remote_id[HISI_MAX_MAP_DEV];
};

#define	HI_IOC_MCC_BASE			'M'

/* Create a new mcc handle. A file descriptor is only used once for one mcc handle. */
#define HI_MCC_IOC_CONNECT		_IOW(HI_IOC_MCC_BASE, 1, struct hi_mcc_handle_attr)

#define HI_MCC_IOC_CHECK		_IOW(HI_IOC_MCC_BASE, 2, struct hi_mcc_handle_attr)

#define HI_MCC_IOC_GET_LOCAL_ID		_IOW(HI_IOC_MCC_BASE, 4, struct hi_mcc_handle_attr) 

#define HI_MCC_IOC_GET_REMOTE_ID	_IOW(HI_IOC_MCC_BASE, 5, struct hi_mcc_handle_attr)

#define HI_MCC_IOC_ATTR_INIT		_IOW(HI_IOC_MCC_BASE, 6, struct hi_mcc_handle_attr)

#ifdef __KERNEL__
#include <linux/module.h>
#include <linux/kernel.h>
#include <asm/ioctl.h>
#include <linux/wait.h>

#define HI_MCC_DEBUG		4
#define HI_MCC_INFO		3
#define HI_MCC_ERR		2
#define HI_MCC_FATAL		1
#define HI_MCC_DBG_LEVEL  	2
#define hi_mcc_trace(level, s, params...) do{ if(level <= HI_MCC_DBG_LEVEL)\
	printk(KERN_INFO "[%s, %d]: " s "\n", __FUNCTION__, __LINE__, ##params);\
}while(0)

struct hios_mcc_handle {
	unsigned long pci_handle;   /* pci handle */
	struct list_head mem_list;  /* mem list  */
	wait_queue_head_t wait;
};

typedef struct hios_mcc_handle  hios_mcc_handle_t;

/*
 * recvfrom_notify sample  
 *
 *int myrecv_notify(void *handle ,void *buf, unsigned int data_len)
 *{		        ~~~~~~~
 *	struct hios_mcc_handle hios_handle;
 *	hios_mcc_handle_opt opt;
 *	unsigned long cus_data;
 *	hios_handle.handle = (unsigned long) handle
 *					     ~~~~~~
 *	hios_mcc_getopt(&hios_handle, &opt);
 *	cus_data = opt.data;
 *	...
 *}
 */

struct hios_mcc_handle_opt {
	int (*recvfrom_notify)(void *handle,
			void *buf,
			unsigned int data_len);

	unsigned long data;
};

typedef struct hios_mcc_handle_opt hios_mcc_handle_opt_t;

hios_mcc_handle_t *hios_mcc_open(struct hi_mcc_handle_attr *attr);

void hios_mcc_handle_attr_init(struct hi_mcc_handle_attr *attr);

int hios_mcc_sendto(hios_mcc_handle_t *handle, const void *buf, unsigned int len);

int hios_mcc_sendto_user(hios_mcc_handle_t *handle, const void *buf, unsigned int len, int flag);

int hios_mcc_close(hios_mcc_handle_t *handle);

int hios_mcc_getopt(hios_mcc_handle_t *handle, hios_mcc_handle_opt_t *opt);

int hios_mcc_setopt(hios_mcc_handle_t *handle, const hios_mcc_handle_opt_t *opt);

int hios_mcc_getlocalid(hios_mcc_handle_t *handle);

int hios_mcc_getremoteids(int ids[],hios_mcc_handle_t *handle);

int hios_mcc_check_remote(int remote_id,hios_mcc_handle_t *handle);

int hios_mcc_register(void);

#endif  /* __KERNEL__ */

#endif  /* __HI_MCC_USERDEV_HEAD__ */
