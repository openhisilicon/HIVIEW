/*
  Copyright (c), 2001-2022, Shenshu Tech. Co., Ltd.
 */

#ifndef __OT_OSAL__
#define __OT_OSAL__

#define OT_OSAL_VERSION    "1.0"

#include "osal_list.h"
#include "autoconf.h"

#define osal_gfp_kernel    0
#define osal_gfp_atomic    1
void *osal_vmalloc(unsigned long size);
void osal_vfree(const void *addr);
void *osal_kmalloc(unsigned long size, unsigned int osal_gfp_flag);
void osal_kfree(const void *addr);

/* atomic api */
typedef struct {
    void *atomic;
} osal_atomic_t;

#define OSAL_ATOMIC_INIT(i)  { (i) }

int osal_atomic_init(osal_atomic_t *atomic);
void osal_atomic_destroy(osal_atomic_t *atomic);
int osal_atomic_read(osal_atomic_t *v);
void osal_atomic_set(osal_atomic_t *v, int i);
int osal_atomic_inc_return(osal_atomic_t *v);
int osal_atomic_dec_return(osal_atomic_t *v);
int osal_atomic_fetch_and(int i, osal_atomic_t *atomic);
int osal_atomic_fetch_or(int i, osal_atomic_t *atomic);

typedef struct {
    void *atomic64;
} osal_atomic64_t;

int osal_atomic64_init(osal_atomic64_t *atomic);
void osal_atomic64_destroy(osal_atomic64_t *atomic);
long long osal_atomic64_read(osal_atomic64_t *v);
void osal_atomic64_set(osal_atomic64_t *v, long long i);
long long osal_atomic64_fetch_and(long long i, osal_atomic64_t *atomic);
long long osal_atomic64_fetch_or(long long i, osal_atomic64_t *atomic);

/* semaphore api */
#define EINTR              4
typedef struct osal_semaphore {
    void *sem;
} osal_semaphore_t;
int osal_sema_init(osal_semaphore_t *sem, int val);
int osal_down(osal_semaphore_t *sem);
int osal_down_interruptible(osal_semaphore_t *sem);
int osal_down_trylock(osal_semaphore_t *sem);
void osal_up(osal_semaphore_t *sem);
/* notice:must be called when kmod exit, other wise will lead to memory leak; */
void osal_sema_destroy(osal_semaphore_t *sem);

/* mutex api */
typedef struct osal_mutex {
    void *mutex;
} osal_mutex_t;
int osal_mutex_init(osal_mutex_t *mutex);
int osal_mutex_lock(osal_mutex_t *mutex);
int osal_mutex_lock_interruptible(osal_mutex_t *mutex);
void osal_mutex_unlock(osal_mutex_t *mutex);
/* notice:must be called when kmod exit, other wise will lead to memory leak; */
void osal_mutex_destroy(osal_mutex_t *mutex);

/* spin lock api */
typedef struct osal_spinlock {
    void *lock;
} osal_spinlock_t;
int osal_spin_lock_init(osal_spinlock_t *lock);
void osal_spin_lock(osal_spinlock_t *lock);
void osal_spin_unlock(osal_spinlock_t *lock);
void osal_spin_lock_irqsave(osal_spinlock_t *lock, unsigned long *flags);
void osal_spin_unlock_irqrestore(osal_spinlock_t *lock, const unsigned long *flags);
/* notice:must be called when kmod exit, other wise will lead to memory leak; */
void osal_spin_lock_destroy(osal_spinlock_t *lock);

/* wait api */
typedef int (*osal_wait_cond_func_t)(const void *param);

typedef struct osal_wait {
    void *wait;
} osal_wait_t;
#define ERESTARTSYS        512

unsigned long osal_msecs_to_jiffies(const unsigned int m);
int osal_wait_init(osal_wait_t *wait);
int osal_wait_uninterruptible(osal_wait_t *wait, osal_wait_cond_func_t func, const void *param);
int osal_wait_timeout_interruptible(osal_wait_t *wait, osal_wait_cond_func_t func, const void *param,
    unsigned long ms);
int osal_wait_timeout_uninterruptible(osal_wait_t *wait, osal_wait_cond_func_t func, const void *param,
    unsigned long ms);

#define osal_wait_event_interruptible(wait, func, param)                       \
    ({                                                                         \
        int __ret = 0;                                                         \
                                                                               \
        for (;;) {                                                             \
            if (func(param) != 0) {                                            \
                __ret = 0;                                                     \
                break;                                                         \
            }                                                                  \
            __ret = osal_wait_timeout_interruptible(wait, (func), param, 100); \
            if (__ret < 0) {                                                   \
                break;                                                         \
            }                                                                  \
        }                                                                      \
        __ret;                                                                 \
    })

#define osal_wait_event_uninterruptible(wait, func, param)          \
    ({                                                              \
        int __ret = 0;                                              \
                                                                    \
        for (;;) {                                                  \
            if (func(param) != 0) {                                 \
                __ret = 0;                                          \
                break;                                              \
            }                                                       \
            __ret = osal_wait_uninterruptible(wait, (func), param); \
            if (__ret < 0) {                                        \
                break;                                              \
            }                                                       \
        }                                                           \
        __ret;                                                      \
    })

#define osal_wait_event_timeout_interruptible(wait, func, param, timeout)        \
    ({                                                                           \
        int __ret = timeout;                                                     \
                                                                                 \
        if ((func(param) != 0) && ((timeout) == 0)) {                            \
            __ret = 1;                                                           \
        }                                                                        \
                                                                                 \
        for (;;) {                                                               \
            if (func(param) != 0) {                                              \
                __ret = (int)osal_msecs_to_jiffies(__ret);                       \
                break;                                                           \
            }                                                                    \
            __ret = osal_wait_timeout_interruptible(wait, (func), param, __ret); \
            if ((__ret == 0) || (__ret == -ERESTARTSYS)) {                       \
                break;                                                           \
            }                                                                    \
        }                                                                        \
        __ret;                                                                   \
    })

#define osal_wait_event_timeout_uninterruptible(wait, func, param, timeout)        \
    ({                                                                             \
        int __ret = timeout;                                                       \
                                                                                   \
        if ((func(param) != 0) && ((timeout) == 0)) {                              \
            __ret = 1;                                                             \
        }                                                                          \
                                                                                   \
        for (;;) {                                                                 \
            if (func(param) != 0) {                                                \
                __ret = (int)osal_msecs_to_jiffies(__ret);                         \
                break;                                                             \
            }                                                                      \
            __ret = osal_wait_timeout_uninterruptible(wait, (func), param, __ret); \
            if ((__ret == 0) || (__ret == -ERESTARTSYS)) {                         \
                break;                                                             \
            }                                                                      \
        }                                                                          \
        __ret;                                                                     \
    })

void osal_wakeup(osal_wait_t *wait);  /* same as wake_up_all */
void osal_wait_destroy(osal_wait_t *wait);

/* workqueue api */
typedef struct osal_work_struct {
    void *work;
    void (*func)(struct osal_work_struct *work);
} osal_work_struct_t;
typedef void (*osal_work_func_t)(struct osal_work_struct *work);

int osal_init_work(struct osal_work_struct *work, osal_work_func_t func);

#define OSAL_INIT_WORK(_work, _func)      \
    do {                                  \
        osal_init_work((_work), (_func)); \
    } while (0)

int osal_schedule_work(const struct osal_work_struct *work);
void osal_destroy_work(struct osal_work_struct *work);

/* schedule */
void osal_yield(void);

/* interrupt api */
enum osal_irqreturn {
    OSAL_IRQ_NONE = (0 << 0),
    OSAL_IRQ_HANDLED = (1 << 0),
    OSAL_IRQ_WAKE_THREAD = (1 << 1),
};

typedef int (*osal_irq_handler_t)(int, void *);
int osal_request_irq(unsigned int irq, osal_irq_handler_t handler, osal_irq_handler_t thread_fn,
    const char *name, void *dev);
void osal_free_irq(unsigned int irq, void *dev);
int osal_in_interrupt(void);

/* task api */
typedef struct osal_task {
    void *task_struct;
} osal_task_t;
typedef int (*threadfn_t)(void *data);
osal_task_t *osal_kthread_create(threadfn_t thread, void *data, const char *name);
void osal_kthread_destroy(osal_task_t *task, unsigned int stop_flag);
int osal_kthread_should_stop(void);

/* tasklet api */
typedef struct osal_tasklet {
    void *tasklet_struct;
}osal_tasklet_t;
int osal_tasklet_init(osal_tasklet_t *tasklet, void (*func)(unsigned long), unsigned long data);
void osal_tasklet_kill(osal_tasklet_t *tasklet);
void osal_tasklet_schedule(const osal_tasklet_t *tasklet);
int osal_get_current_tgid(void);
int osal_get_current_pid(void);
/* string api */
int osal_strcmp(const char *s1, const char *s2);
int osal_strncmp(const char *s1, const char *s2, int size);
char *osal_strstr(const char *s1, const char *s2);
int osal_strlen(const char *s);
int osal_strnlen(const char *s, int size);
char *osal_strsep(char **s, const char *ct);
int osal_memcmp(const void *cs, const void *ct, int count);

long osal_strtol(const char *cp, char **endp, unsigned int base);

/* addr translate */
void *osal_ioremap(unsigned long phys_addr, unsigned long size);
void *osal_ioremap_nocache(unsigned long phys_addr, unsigned long size);
void *osal_ioremap_cached(unsigned long phys_addr, unsigned long size);
void *osal_ioremap_wc(unsigned long phys_addr, unsigned long size);
void osal_iounmap(void *addr);

#define osal_readl(x) (*((volatile int *)(x)))
#define osal_writel(v, x) (*((volatile int *)(x)) = (v))

unsigned long osal_copy_from_user(void *to, const void *from, unsigned long n);
unsigned long osal_copy_to_user(void *to, const void *from, unsigned long n);

#define OSAL_VERIFY_READ   0
#define OSAL_VERIFY_WRITE  1
int osal_access_ok(int type, const void *addr, unsigned long size);

/* cache api */
void osal_flush_dcache_area(void *kvirt, unsigned long phys_addr, unsigned long length);

/* math */
unsigned long long osal_div_u64(unsigned long long dividend, unsigned int divisor);
long long osal_div_s64(long long dividend, int divisor);
unsigned long long osal_div64_u64(unsigned long long dividend, unsigned long long divisor);
long long osal_div64_s64(long long dividend, long long divisor);
unsigned long long osal_div_u64_rem(unsigned long long dividend, unsigned int divisor);
unsigned long long osal_div64_u64_rem(unsigned long long dividend, unsigned long long divisor);
unsigned int osal_random(void);

#define osal_min(x, y) ({          \
    __typeof__(x) _min1 = (x);     \
    __typeof__(y) _min2 = (y);     \
    (void) (&_min1 == &_min2);     \
    _min1 < _min2 ? _min1 : _min2; \
})

#define osal_abs(x) ({                \
    long ret;                         \
    if (sizeof(x) == sizeof(long)) {  \
        long __x = (x);               \
        ret = (__x < 0) ? -__x : __x; \
    } else {                          \
        int __x = (x);                \
        ret = (__x < 0) ? -__x : __x; \
    }                                 \
    ret;                              \
})

/* barrier */
void osal_isb(void);
void osal_dsb(void);
void osal_dmb(void);

/* debug */
int osal_printk(const char *fmt, ...) __attribute__((format(printf, 1, 2)));
#ifdef OT_DEBUG
void osal_panic(const char *fmt, const char *fun, int line, const char *);
#define OSAL_BUG() \
    do {           \
    } while (1)
#else
#define OSAL_BUG()
#endif

#define OSAL_ASSERT(expr)                       \
    do {                                        \
        if (!(expr)) {                          \
            osal_printk("\nASSERT failed at:\n" \
                        "  >Condition: %s\n",   \
                #expr);                         \
            OSAL_BUG();                         \
        }                                       \
    } while (0)

#define OSAL_BUG_ON(expr)                                                               \
    do {                                                                                \
        if (expr) {                                                                     \
            osal_printk("BUG: failure at %d/%s()!\n", __LINE__, __func__);              \
            OSAL_BUG();                                                                 \
        }                                                                               \
    } while (0)

#ifdef CONFIG_OT_LOG_TRACE_SUPPORT
#define osal_trace osal_printk
#else
#define osal_trace(str, fmt...)
#endif

/* proc */
#define OSAL_MAX_PROC_NAME_LEN  50
typedef struct osal_proc_dir_entry {
    char name[OSAL_MAX_PROC_NAME_LEN];
    void *proc_dir_entry;
    int (*open)(struct osal_proc_dir_entry *entry);
    int (*read)(struct osal_proc_dir_entry *entry);
    int (*write)(struct osal_proc_dir_entry *entry, const char *buf, int count, long long *);
    void *private;
    void *seqfile;
    struct osal_list_head node;
} osal_proc_entry_t;
osal_proc_entry_t *osal_create_proc_entry(const char *name, osal_proc_entry_t *parent);
osal_proc_entry_t *osal_proc_mkdir(const char *name, osal_proc_entry_t *parent);
void osal_remove_proc_entry(const char *name, osal_proc_entry_t *parent);
void osal_seq_printf(osal_proc_entry_t *entry, const char *fmt, ...) __attribute__((format(printf, 2, 3)));

#define OSAL_MAX_DEV_NAME_LEN   48

typedef struct osal_dev {
    char name[OSAL_MAX_DEV_NAME_LEN];
    void *dev;
    int minor;
    struct osal_fileops *fops;
} osal_dev_t;

typedef struct osal_vm {
    void *vm;
} osal_vm_t;

#define OSAL_POLLIN        0x0001U
#define OSAL_POLLPRI       0x0002U
#define OSAL_POLLOUT       0x0004U
#define OSAL_POLLERR       0x0008U
#define OSAL_POLLHUP       0x0010U
#define OSAL_POLLNVAL      0x0020U
#define OSAL_POLLRDNORM    0x0040U
#define OSAL_POLLRDBAND    0x0080U
#define OSAL_POLLWRNORM    0x0100U

typedef struct osal_poll {
    void *poll_table;
    void *data;
} osal_poll_t;

typedef struct osal_fileops {
    int (*open)(void *private_data);
    int (*read)(char *buf, int size, long *offset, void *private_data);
    int (*write)(const char *buf, int size, long *offset, void *private_data);
    long (*llseek)(long offset, int whence, void *private_data);
    int (*release)(void *private_data);
    long (*unlocked_ioctl)(unsigned int cmd, unsigned long arg, void *private_data);
    unsigned int (*poll)(osal_poll_t *osal_poll, void *private_data);
    int (*mmap)(osal_vm_t *vm, unsigned long start, unsigned long end, unsigned long vm_pgoff, void *private_data);
#ifdef CONFIG_COMPAT
    long (*compat_ioctl)(unsigned int cmd, unsigned long arg, void *private_data);
#endif
} osal_fileops_t;

#define OSAL_SEEK_SET      0
#define OSAL_SEEK_CUR      1
#define OSAL_SEEK_END      2

osal_dev_t *osal_createdev(const char *name);
int osal_destroydev(osal_dev_t *pdev);
int osal_registerdevice(const osal_dev_t *pdev);
void osal_deregisterdevice(const osal_dev_t *pdev);
void osal_poll_wait(osal_poll_t *table, const osal_wait_t *wait);
void osal_pgprot_noncached(osal_vm_t *vm);
void osal_pgprot_cached(osal_vm_t *vm);
void osal_pgprot_writecombine(osal_vm_t *vm);
int osal_remap_pfn_range(osal_vm_t *vm, unsigned long addr, unsigned long pfn, unsigned long size);

/* timer */
typedef struct osal_timer {
    void *timer;
    void (*function)(unsigned long);
    unsigned long data;
} osal_timer_t;

typedef struct osal_timeval {
    long tv_sec;
    long tv_usec;
} osal_timeval_t;

typedef struct osal_rtc_time {
    int tm_sec;
    int tm_min;
    int tm_hour;
    int tm_mday;
    int tm_mon;
    int tm_year;
    int tm_wday;
    int tm_yday;
    int tm_isdst;
} osal_rtc_time_t;

/* Return values for the timer callback function */
typedef enum {
    OSAL_HRTIMER_NORESTART, /* < The timer will not be restarted. */
    OSAL_HRTIMER_RESTART /* < The timer must be restarted. */
} OSAL_HRTIMER_RESTART_E;

/* hrtimer struct */
typedef struct osal_hrtimer {
    void *timer;
    OSAL_HRTIMER_RESTART_E (*function)(void *timer);
    unsigned long interval; /* Unit ms */
} osal_hrtimer_t;

int osal_hrtimer_create(osal_hrtimer_t *phrtimer);
int osal_hrtimer_start(const osal_hrtimer_t *phrtimer);
int osal_hrtimer_destroy(osal_hrtimer_t *phrtimer);

int osal_timer_init(osal_timer_t *timer);
int osal_set_timer(osal_timer_t *timer, unsigned long interval);  /* ms */
unsigned long osal_timer_get_private_data(const void *data);
int osal_del_timer(osal_timer_t *timer);
int osal_timer_destroy(osal_timer_t *timer);

void osal_msleep(unsigned int msecs);
void osal_udelay(unsigned int usecs);
void osal_mdelay(unsigned int msecs);

unsigned int osal_get_tickcount(void);
unsigned long long osal_sched_clock(void);
void osal_gettimeofday(osal_timeval_t *tv);
void osal_rtc_time_to_tm(unsigned long time, osal_rtc_time_t *tm);

#define OSAL_O_ACCMODE     00000003
#define OSAL_O_RDONLY      00000000
#define OSAL_O_WRONLY      00000001
#define OSAL_O_RDWR        00000002
#define OSAL_O_CREAT       00000100

void *osal_klib_fopen(const char *filename, int flags, int mode);
void osal_klib_fclose(void *filp);
int osal_klib_fwrite(const char *buf, int len, void *filp);
int osal_klib_fread(char *buf, unsigned int len, void *filp);

/* reboot */
struct osal_notifier_block {
    int (*notifier_call)(struct osal_notifier_block *nb, unsigned long action, void *data);
    void *notifier_block;
};

int osal_register_reboot_notifier(struct osal_notifier_block *nb);
int osal_unregister_reboot_notifier(struct osal_notifier_block *nb);

#include <stdarg.h>

#ifndef _OSAL_VA_LIST

#define _OSAL_VA_LIST
#define osal_va_list       va_list
#define osal_va_arg(ap, T) va_arg(ap, T)
#define osal_va_end(ap) va_end(ap)
#define osal_va_start(ap, A) va_start(ap, A)

#endif /* va_arg */

#define NULL_STRING        "NULL"

int osal_platform_driver_register(void *drv);
void osal_platform_driver_unregister(void *drv);
void *osal_platform_get_resource_byname(void *dev, unsigned int type,
                                        const char *name);
void *osal_platform_get_resource(void *dev, unsigned int type,
                                 unsigned int num);
int osal_platform_get_irq(void *dev, unsigned int num);
int osal_platform_get_irq_byname(void *dev, const char *name);

#define osal_module_driver(osal_driver, osal_register, osal_unregister, ...) \
    static int __init osal_driver##_init(void)                               \
    {                                                                        \
        return osal_register(&(osal_driver));                                \
    }                                                                        \
    module_init(osal_driver##_init);                                         \
    static void __exit osal_driver##_exit(void)                              \
    {                                                                        \
        osal_unregister(&(osal_driver));                                     \
    }                                                                        \
    module_exit(osal_driver##_exit);

#define osal_module_platform_driver(platform_driver)                   \
    osal_module_driver(platform_driver, osal_platform_driver_register, \
        osal_platform_driver_unregister)

#endif
