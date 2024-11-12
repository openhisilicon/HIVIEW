/*
  Copyright (c), 2001-2024, Shenshu Tech. Co., Ltd.
 */

/**
 * @defgroup osal_drvbox osal_drvbox
 */
#ifndef __OSAL_DRVBOX_H__
#define __OSAL_DRVBOX_H__

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/**
 * @ingroup osal_drvbox
 * @brief Switch region from usr box to driver box.
 *
 * @par Description:
 * This API is used to switch executing region from usr box to driver box, and change pmp/mpu configuration respectivly.
 *
 * @attention
 * This api can only be invoked in common usr lib.
 *
 * @param    drvId    [in] used to check the invoking permission of the driver.
 *
 * @retval   DRV_BOX_INVALID_SWITCH  No need to do the switching process due to the limitation.
 * @retval   DRV_BOX_SWITCH_FAIL     Permission rejected.
 * @retval   LOS_OK                  Box region switching success.
 *
 * @par Support System:
 * seliteos.
 */
unsigned int osal_drvmgr_switch_to_drvbox(unsigned int drv_id);

/**
 * @ingroup osal_drvbox
 * @brief Switch region from driver box to usr box.
 *
 * @par Description:
 * This API is used to switch box running region from driver box to usr box,
 * and change pmp/mpu configuration respectivly.
 *
 * @attention
 * This api can only be invoked in common usr lib space.
 *
 * @par Support System:
 * seliteos.
 */
void osal_drvmgr_switch_to_usrbox(void);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif
#endif /* __OSAL_DRVBOX_H__ */