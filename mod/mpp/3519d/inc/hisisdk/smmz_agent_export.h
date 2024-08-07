/*
  Copyright (c), 2001-2024, Shenshu Tech. Co., Ltd.
 */
#ifndef SMMZ_AGENT_H
#define SMMZ_AGENT_H
#include <linux/types.h>

#ifdef __cplusplus
extern "C" {
#endif /* end of #ifdef __cplusplus */

#define MMZ_NAME_LEN 32

int get_smmb_buf_id(int fd);

phys_addr_t smmb_phyaddr(int buf_id, pid_t pid);
#ifdef __cplusplus
}
#endif /* end of #ifdef __cplusplus */

#endif
