/*
  Copyright (c), 2001-2024, Shenshu Tech. Co., Ltd.
 */

#ifndef CRYPTO_ECC_CURVE_H
#define CRYPTO_ECC_CURVE_H

#include "crypto_pke_struct.h"

const drv_pke_ecc_curve *get_ecc_curve(drv_pke_ecc_curve_type curve_type);

#endif