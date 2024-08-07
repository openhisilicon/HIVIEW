/*
  Copyright (c), 2001-2024, Shenshu Tech. Co., Ltd.
 */

#ifndef OSAL_ERRNO_H_
#define OSAL_ERRNO_H_

/* Basic common errno 1~34 */
#define OSAL_EINTR (-4)  /* Interrupted system call */
#define OSAL_EINVAL (-22)  /* Interrupted system call */
#define OSAL_ETIME (-62)  /* Timer expired */
#define OSAL_EOVERFLOW (-75)  /* Value too large for defined data type */

#define OSAL_ERESTARTSYS 512

#define OSAL_NONEOS_DEFAULT_RET (-200) /* NoneOS's default return value */
#ifndef EOK
#define EOK 0
#endif

#endif /* OSAL_ERRNO_H_ */
