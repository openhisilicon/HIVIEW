/*
  Copyright (c), 2001-2024, Shenshu Tech. Co., Ltd.
 */

#ifndef CRYPTO_TYPE_H
#define CRYPTO_TYPE_H

#if defined(CONFIG_CRYPTO_CHIP_HI3519DV500)
#include "ot_type.h"
#else
#include "td_type.h"
#endif

#if defined(CONFIG_CRYPTO_CHIP_HI3892WV100)
#define TD_SUCCESS 0
#define TD_FAILURE 0xFFFFFFFF
#endif

#include "crypto_common_def.h"
#include "crypto_common_macro.h"
#include "crypto_errno.h"

#if defined(__UBOOT__)
#endif

#endif