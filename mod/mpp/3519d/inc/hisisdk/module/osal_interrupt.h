/*
  Copyright (c), 2001-2024, Shenshu Tech. Co., Ltd.
 */

/**
 * @defgroup osal_interrupt osal_interrupt
 */
#ifndef __OSAL_INTERRUPT_H__
#define __OSAL_INTERRUPT_H__

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#define OSAL_CPU_ALL 0
#define OSAL_CPU_0 (1 << 1)
#define OSAL_CPU_1 (1 << 2)
#define OSAL_CPU_2 (1 << 3)
#define OSAL_CPU_3 (1 << 4)

typedef struct {
    void *tasklet;
    void (*handler)(unsigned long data);
    unsigned long data;
} osal_tasklet;

enum osal_irqreturn {
    OSAL_IRQ_NONE = (0 << 0),
    OSAL_IRQ_HANDLED = (1 << 0),
    OSAL_IRQ_WAKE_THREAD = (1 << 1),
};
typedef int (*osal_irq_handler)(int, void *);

/**
 * @ingroup osal_interrupt
 * @brief get irq_handler dev.
 *
 * @param param_dev [in] Parameters passed to the callback function.
 *
 * @return Returns the parameters that can be used directly.
 *
 * @par Support System:
 * linux liteos freertos seliteos.
 */
void *osal_irq_get_private_dev(void *param_dev);

/**
 * @ingroup osal_interrupt
 * @brief alloc an interrupt line.
 *
 * @param irq [in] Interrupt line to alloc.
 * @param handler [in] Function to be called when the IRQ occurs. Primary handler for threaded interrupts.
 * If NULL and thread_fn != NULL the default primary handler is installed.
 * @param thread_fn [in] Function called from the irq handler thread. If NULL, no irq thread is created.
 * @param name [in] An ascii name for the claiming device.
 * @param dev [in] A cookie passed back to the handler function.
 *
 * @attention In linux userspace, the type of the dev parameter must be (td_irq_arg *).
 *
 * @return OSAL_SUCCESS/OSAL_FAILURE
 *
 * @par Support System:
 * linux liteos seliteos freertos.
 */
int osal_irq_request(unsigned int irq, osal_irq_handler handler, osal_irq_handler thread_fn, const char *name,
    void *dev);

/**
 * @ingroup osal_interrupt
 * @brief free an interrupt allocd with request_irq.
 *
 * @param irq [in] Interrupt line to free.
 * @param dev [in] Device identity to free.
 *
 * @attention This function must not be called from interrupt context. In linux userspace,
 *            the parameter dev must be the same as the dev parameter of the osal_irq_request function.
 *
 * @par Support System:
 * linux liteos seliteos freertos.
 */
void osal_irq_free(unsigned int irq, void *dev);

/**
 * @ingroup osal_interrupt
 * @brief Set interrupts priority.
 *
 * @par Description:
 * Set interrupts priority.
 *
 * @attention
 * This function depends on the hardware implementation of the interrupt controller and CPU architecture.
 *
 * @return OSAL_SUCCESS/OSAL_FAILURE
 *
 * @par Support System:
 * liteos freertos.
 */
int osal_irq_set_priority(unsigned int irq, unsigned short priority);

/**
 * @ingroup osal_interrupt
 * @brief Setting the CPU Affinity of Interrupts.
 *
 * @par Description:
 * Setting the CPU Affinity of Interrupts.
 *
 * @param irq [in] irq number.
 * @param name [in] irq name.
 * @param cpu_mask [in] cpu_musk. one of the following: OSAL_CPU_ALL, OSAL_CPU_0, OSAL_CPU_1, OSAL_CPU_2, OSAL_CPU_3
 *
 * @par Support System:
 * linux liteos seliteos.
 */
int osal_irq_set_affinity(unsigned int irq, const char *name, int cpu_mask);

/**
 * @ingroup osal_interrupt
 * @brief enable handling of an irq.
 *
 * @par Description:
 * Undoes the effect of one call to disable_irq().
 * If this matches the last disable, processing of interrupts on this IRQ line is re-enabled.
 *
 * @param irq [in] Interrupt to enable.
 *
 * @par Support System:
 * linux liteos seliteos freertos.
 */
void osal_irq_enable(unsigned int irq);

/**
 * @ingroup osal_interrupt
 * @brief disable an irq and wait for completion.
 *
 * @par Description:
 * Disable the selected interrupt line.
 *
 * @param irq [in] Interrupt to disable.
 *
 * @par Support System:
 * linux liteos seliteos freertos.
 */
void osal_irq_disable(unsigned int irq);

/**
 * @ingroup osal_interrupt
 * @brief Disable all interrupts.
 *
 * @par Description:
 * This API is used to disable all IRQ and FIQ interrupts in the CPSR.
 *
 * @return CPSR value before all interrupts are disabled.
 *
 * @par Support System:
 * liteos seliteos nonos freertos.
 */
unsigned int osal_irq_lock(void);

/**
 * @ingroup osal_interrupt
 * @brief Enable all interrupts.
 *
 * @par Description:
 * This API is used to enable all IRQ and FIQ interrupts in the CPSR.
 *
 * @return CPSR value after all interrupts are enabled.
 *
 * @par Support System:
 * liteos seliteos nonos freertos.
 */
unsigned int osal_irq_unlock(void);

/**
 * @ingroup osal_interrupt
 * @brief Restore interrupts.
 *
 * @par Description:
 * This API is used to restore the CPSR value obtained before all interrupts are disabled by #osal_irq_lock.
 *
 * @param irq_status [in] Interrupt to restore.
 *
 * @attention
 * This API can be called only after all interrupts are disabled, and the input parameter value should be
 * the value returned by osal_irq_lock.
 *
 * @par Support System:
 * liteos seliteos nonos freertos.
 */
void osal_irq_restore(unsigned int irq_status);

/**
 * @ingroup osal_interrupt
 * @brief Clear the pending status of other interrupts.
 *
 * @par Description:
 * This API is used to clear the pending status of other interrupts.
 *
 * @param vector [in] interrupt vector.
 *
 * @par Support System:
 * liteos freertos.
 */
unsigned int osal_irq_clear(unsigned int vector);

/**
 * @ingroup osal_interrupt
 * @brief Check whether the current interrupt is in interrupt.
 *
 * @par Description:
 * Check whether the current interrupt is in the context of hard interrupts, soft interrupts, and unmaskable interrupts.
 *
 * @return false/true
 *
 * @par Support System:
 * linux liteos nonos freertos.
 */
int osal_in_interrupt(void);

/**
 * @ingroup osal_interrupt
 * @brief initialize tasklet.
 *
 * @par Description:
 * initialize tasklet.
 *
 * @attention
 * Before invoking this interface,
 * assign values to the handler and data members of osal_tasklet and leave the tasklet member tasklet empty.
 *
 * @param tasklet [in/out] The tasklet to be initialized.
 *
 * @par Support System:
 * linux liteos seliteos.
 */
int osal_tasklet_init(osal_tasklet *tasklet);

/**
 * @ingroup osal_interrupt
 * @brief schedule tasklet.
 *
 * @par Support System:
 * linux liteos seliteos.
 */
int osal_tasklet_schedule(osal_tasklet *tasklet);

/**
 * @ingroup osal_interrupt
 * @brief Close tasklet.
 *
 * @par Description:
 * Close tasklet.
 *
 * @par Support System:
 * linux liteos seliteos.
 */
int osal_tasklet_kill(osal_tasklet *tasklet);

/**
 * @ingroup osal_interrupt
 * @brief Update tasklet.
 *
 * @par Description:
 * Update tasklet.
 *
 * @par Support System:
 * linux liteos seliteos.
 */
int osal_tasklet_update(osal_tasklet *tasklet);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif
#endif /* __OSAL_INTERRUPT_H__ */
