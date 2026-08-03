/*
 * IMX546 register init extracted from:
 *   imx546_2mod_trig_power-zy-rx2376-2272-v3.3/hdl/register_config.sv
 *
 * Reference board notes:
 *   - Control bus: I2C, 7-bit addr 0x36 (7'b0110110)
 *   - INCK: 37.125 MHz
 *   - ADBIT: 12-bit (0x3200=0x14)
 *   - FPGA path used SLVS-EC RX + crop ~2272x2272
 *   - Default VMAX in table is 0x222E (~34 fps per their comment), NOT 60 fps
 *
 * For Hi3559A SLVS 8ch @ 8M12bit60:
 *   - Must set LANESEL for classic SLVS 8ch (FPGA 0x3904=0x03 is for their EC path)
 *   - Must set VMAX/HMAX from Sony SLVS-8ch 60fps setting list
 *   - Full recording size target: 2840x2840
 */
#ifndef __IMX546_REG_TABLE_H__
#define __IMX546_REG_TABLE_H__

#include "hi_type.h"

#define IMX546_I2C_ADDR          (0x36)   /* 7-bit */
#define IMX546_INCK_MHZ          (37.125)

/* TODO: confirm with datasheet for classic SLVS 8ch */
#ifndef IMX546_LANESEL_SLVS_8CH
#define IMX546_LANESEL_SLVS_8CH  (0x02)
#endif

/* FPGA default VMAX (bring-up start). Replace for 60fps. */
#define IMX546_VMAX_FPGA_DEFAULT (0x00222E)
#define IMX546_HMAX_FPGA_DEFAULT (0x037A)

typedef struct {
    HI_U16 addr;
    HI_U8  data;
} IMX546_REG_S;

/* Linear standby->config sequence (before STANDBY=0 / XMSTA=0) */
static const IMX546_REG_S g_imx546_linear_init[] = {
    {0x3000, 0x01}, /* STANDBY */

    /* INCKSEL 37.125MHz */
    {0x3014, 0x05},
    {0x3015, 0x91},
    {0x3016, 0x50},
    {0x3018, 0x20},
    {0x3019, 0x02},
    {0x301B, 0x1D},
    {0x303C, 0x02},

    {0x30D0, 0x28}, /* VOP_VBLK_HWIDTH */
    {0x30D1, 0x0B},
    {0x30D2, 0x28}, /* FINFO_HWIDTH */
    {0x30D3, 0x0B},

    /* VMAX / HMAX — update for target fps */
    {0x30D4, (IMX546_VMAX_FPGA_DEFAULT >> 0) & 0xFF},
    {0x30D5, (IMX546_VMAX_FPGA_DEFAULT >> 8) & 0xFF},
    {0x30D6, (IMX546_VMAX_FPGA_DEFAULT >> 16) & 0xFF},
    {0x30D8, (IMX546_HMAX_FPGA_DEFAULT >> 0) & 0xFF},
    {0x30D9, (IMX546_HMAX_FPGA_DEFAULT >> 8) & 0xFF},
    {0x30DC, 0x01}, /* FREQ */

    {0x30E2, 0x04}, /* GMRWT */
    {0x30E3, 0x14}, /* GMTWT */
    {0x30E5, 0x02}, /* GAINDLY */
    {0x30E6, 0x0A}, /* GSDLY */

    {0x3200, 0x14}, /* ADBIT 12bit */
    {0x321C, 0x40},
    {0x3220, 0x40},
    {0x3226, 0x40},
    {0x3227, 0xD0},
    {0x3233, 0x50},
    {0x323C, 0x19}, /* LLBLANK */
    {0x323E, 0x33},

    {0x3400, 0x0A}, /* trigger related (FPGA: fast trigger) — disable for free-run if needed */
    {0x3430, 0x01}, /* ODBIT */

    {0x3502, 0x08}, /* GAIN_RTS */
    {0x3521, 0x7D},
    {0x3546, 0x1F},
    {0x35B4, 0xF0}, /* BLKLEVEL */

    /* Lane select: override FPGA EC value for SLVS 8ch */
    {0x3904, IMX546_LANESEL_SLVS_8CH},
    {0x3A0C, 0x0B},
};

static const IMX546_REG_S g_imx546_stream_on[] = {
    {0x3000, 0x00}, /* STANDBY = 0 */
    {0x3010, 0x00}, /* XMSTA = start */
};

static const IMX546_REG_S g_imx546_stream_off[] = {
    {0x3010, 0x01}, /* XMSTA = stop */
    {0x3000, 0x01}, /* STANDBY = 1 */
};

#endif /* __IMX546_REG_TABLE_H__ */
