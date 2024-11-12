/*
  Copyright (c), 2001-2024, Shenshu Tech. Co., Ltd.
 */

#ifndef _OSAL_IOCTL_H
#define _OSAL_IOCTL_H

#ifndef _IOC_NRBITS
#define _IOC_NRBITS    8
#endif

#ifndef _IOC_TYPEBITS
#define _IOC_TYPEBITS  8
#endif

#ifndef _IOC_SIZEBITS
#define _IOC_SIZEBITS  14
#endif

#ifndef _IOC_DIRBITS
#define _IOC_DIRBITS   2
#endif

#ifndef _IOC_NRMASK
#define _IOC_NRMASK    ((1 << _IOC_NRBITS) - 1)
#endif

#ifndef _IOC_TYPEMASK
#define _IOC_TYPEMASK  ((1 << _IOC_TYPEBITS) - 1)
#endif

#ifndef _IOC_SIZEMASK
#define _IOC_SIZEMASK  ((1 << _IOC_SIZEBITS) - 1)
#endif

#ifndef _IOC_DIRMASK
#define _IOC_DIRMASK   ((1 << _IOC_DIRBITS) - 1)
#endif

#ifndef _IOC_NRSHIFT
#define _IOC_NRSHIFT    0
#endif

#ifndef _IOC_TYPESHIFT
#define _IOC_TYPESHIFT   (_IOC_NRSHIFT + _IOC_NRBITS)
#endif

#ifndef _IOC_SIZESHIFT
#define _IOC_SIZESHIFT   (_IOC_TYPESHIFT + _IOC_TYPEBITS)
#endif

#ifndef _IOC_DIRSHIFT
#define _IOC_DIRSHIFT    (_IOC_SIZESHIFT + _IOC_SIZEBITS)
#endif

#ifndef _IOC_NONE
#define _IOC_NONE      0U
#endif

#ifndef _IOC_WRITE
#define _IOC_WRITE     1U
#endif

#ifndef _IOC_READ
#define _IOC_READ      2U
#endif

#ifndef _IOC
#define _IOC(dir, type, nr, size) \
    (((dir) << _IOC_DIRSHIFT) |   \
    ((type) << _IOC_TYPESHIFT) |  \
    ((nr) << _IOC_NRSHIFT) |      \
    ((size) << _IOC_SIZESHIFT))
#endif

#ifdef __CHECKER__
#define _IOC_TYPECHECK(t) (sizeof(t))
#else
/* provoke compile error for invalid uses of size argument */
extern unsigned int __invalid_size_argument_for_IOC;

#ifndef _IOC_TYPECHECK
#define _IOC_TYPECHECK(t)               \
    ((sizeof(t) == sizeof(t[1]) &&      \
    sizeof(t) < (1 << _IOC_SIZEBITS)) ? \
    sizeof(t) :                         \
    __invalid_size_argument_for_IOC)
#endif // #ifndef _IOC_TYPECHECK

#endif // #ifdef __CHECKER__

/* used to create numbers */
#ifndef _IO
#define _IO(type, nr)             _IOC(_IOC_NONE, (type), (nr), 0)
#endif

#ifndef _IOR
#define _IOR(type, nr, size)      _IOC(_IOC_READ, (type), (nr), (_IOC_TYPECHECK(size)))
#endif

#ifndef _IOW
#define _IOW(type, nr, size)      _IOC(_IOC_WRITE, (type), (nr), (_IOC_TYPECHECK(size)))
#endif

#ifndef _IOWR
#define _IOWR(type, nr, size)     _IOC(_IOC_READ | _IOC_WRITE, (type), (nr), (_IOC_TYPECHECK(size)))
#endif

#ifndef _IOR_BAD
#define _IOR_BAD(type, nr, size)  _IOC(_IOC_READ, (type), (nr), sizeof(size))
#endif

#ifndef _IOW_BAD
#define _IOW_BAD(type, nr, size)  _IOC(_IOC_WRITE, (type), (nr), sizeof(size))
#endif

#ifndef _IOWR_BAD
#define _IOWR_BAD(type, nr, size) _IOC(_IOC_READ | _IOC_WRITE, (type), (nr), sizeof(size))
#endif

/* used to decode ioctl numbers.. */
#ifndef _IOC_DIR
#define _IOC_DIR(nr)    (((nr) >> _IOC_DIRSHIFT) & _IOC_DIRMASK)
#endif

#ifndef _IOC_TYPE
#define _IOC_TYPE(nr)   (((nr) >> _IOC_TYPESHIFT) & _IOC_TYPEMASK)
#endif

#ifndef _IOC_NR
#define _IOC_NR(nr)     (((nr) >> _IOC_NRSHIFT) & _IOC_NRMASK)
#endif

#ifndef _IOC_SIZE
#define _IOC_SIZE(nr)   (((nr) >> _IOC_SIZESHIFT) & _IOC_SIZEMASK)
#endif

#ifndef IOC_IN
#define IOC_IN        (_IOC_WRITE << _IOC_DIRSHIFT)
#endif
#ifndef IOC_OUT
#define IOC_OUT       (_IOC_READ << _IOC_DIRSHIFT)
#endif

#ifndef IOC_INOUT
#define IOC_INOUT     ((_IOC_WRITE | _IOC_READ) << _IOC_DIRSHIFT)
#endif

#ifndef IOCSIZE_MASK
#define IOCSIZE_MASK  (_IOC_SIZEMASK << _IOC_SIZESHIFT)
#endif

#ifndef IOCSIZE_SHIFT
#define IOCSIZE_SHIFT (_IOC_SIZESHIFT)
#endif

#endif /* _OSAL_IOCTL_H */
