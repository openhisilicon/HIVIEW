/*
  Copyright (c), 2001-2024, Shenshu Tech. Co., Ltd.
 */
#ifndef _MMZ_TEE_MPI_H
#define _MMZ_TEE_MPI_H
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif /* end of #ifdef __cplusplus */

#define TEE_TYPE_MMZ_MASK (1 << 4)
#define TEE_IOCTL_PARAM_ATTR_TYPE_MMZ_INPUT (TEE_TYPE_MMZ_MASK | TEE_IOCTL_PARAM_ATTR_TYPE_MEMREF_INPUT)
#define TEE_IOCTL_PARAM_ATTR_TYPE_MMZ_OUTPUT (TEE_TYPE_MMZ_MASK | TEE_IOCTL_PARAM_ATTR_TYPE_MEMREF_OUTPUT)
#define TEE_IOCTL_PARAM_ATTR_TYPE_MMZ_INOUT (TEE_TYPE_MMZ_MASK | TEE_IOCTL_PARAM_ATTR_TYPE_MEMREF_INOUT)

#define OT_MMZ_NAME_LEN 32
struct smmz_agent_args {
    char mmz_name[OT_MMZ_NAME_LEN];
    int buf_fd;
    ssize_t size;
    void *vaddr;
    unsigned long phy_addr;
    int npuclass;
};

#define IOC_SMMZ_AGENT_ALLOC_SMMZ _IOWR('m', 11, struct smmz_agent_args)

#define IOC_SMMZ_AGENT_ALLOC_SMMZ_NPUCLASS _IOWR('m', 14, struct smmz_agent_args)

#define IOC_SMMZ_AGENT_GET_SIZE _IOWR('m', 25, struct smmz_agent_args)
#define IOC_SMMZ_AGENT_GET_PHY_ADDR _IOWR('m', 26, struct smmz_agent_args)

int smmb_alloc_from_sec_zone(const char *name, ssize_t size);
int smmb_alloc_from_sec_zone_npuclass(int npuclass, ssize_t size);
ssize_t smmb_get_size(int buf_fd);
unsigned long smmb_get_phy_addr(int buf_fd);
int smmb_free(int buf_fd);
void smmz_agent_close(void);
int smmz_agent_init(void);
#ifdef __cplusplus
}
#endif /* end of #ifdef __cplusplus */

#endif
