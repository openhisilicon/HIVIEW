/*
  Copyright (c), 2001-2022, Shenshu Tech. Co., Ltd.
 */

#ifndef OT_SPI_H
#define OT_SPI_H

#include "ot_type.h"
#ifdef __LITEOS__
#include <spi.h>
#endif

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* End of #ifdef __cplusplus */

#ifndef __LITEOS__

/* User space versions of kernel symbols for SPI clocking modes,
 * matching <linux/spi/spi.h>
 */
#define OT_SPI_CPHA                 0x01
#define OT_SPI_CPOL                 0x02

#define OT_SPI_MODE_0               (0 | 0)
#define OT_SPI_MODE_1               (0 | OT_SPI_CPHA)
#define OT_SPI_MODE_2               (OT_SPI_CPOL | 0)
#define OT_SPI_MODE_3               (OT_SPI_CPOL | OT_SPI_CPHA)

#define OT_SPI_CS_HIGH              0x04
#define OT_SPI_LSB_FIRST            0x08
#define OT_SPI_3WIRE                0x10
#define OT_SPI_LOOP                 0x20
#define OT_SPI_NO_CS                0x40
#define OT_SPI_READY                0x80

/* IOCTL commands */
#define OT_SPI_IOC_MAGIC            'k'

/*
 * struct spi_ioc_transfer - describes a single SPI transfer
 * @tx_buf: Holds pointer to userspace buffer with transmit data, or null.
 *    If no data is provided, zeroes are shifted out.
 * @rx_buf: Holds pointer to userspace buffer for receive data, or null.
 * @len: Length of tx and rx buffers, in bytes.
 * @speed_hz: Temporary override of the device's bitrate.
 * @bits_per_word: Temporary override of the device's wordsize.
 * @delay_usecs: If nonzero, how long to delay after the last bit transfer
 *    before optionally deselecting the device before the next transfer.
 * @cs_change: True to deselect device before starting the next transfer.
 *
 * This structure is mapped directly to the kernel spi_transfer structure;
 * the fields have the same meanings, except of course that the pointers
 * are in a different address space (and may be of different sizes in some
 * cases, such as 32-bit i386 userspace over a 64-bit x86_64 kernel).
 * Zero-initialize the structure, including currently unused fields, to
 * accommodate potential future updates.
 *
 * SPI_IOC_MESSAGE gives userspace the equivalent of kernel spi_sync().
 * Pass it an array of related transfers, they'll execute together.
 * Each transfer may be half duplex (either direction) or full duplex.
 *
 *    struct spi_ioc_transfer mesg[4];
 *    ...
 *    status : ioctl(fd, SPI_IOC_MESSAGE(4), mesg);
 *
 * So for example one transfer might send a nine bit command (right aligned
 * in a 16-bit word), the next could read a block of 8-bit data before
 * terminating that command by temporarily deselecting the chip; the next
 * could send a different nine bit command (re-selecting the chip), and the
 * last transfer might write some register values.
 * */
 /* If the contents of 'struct spi_ioc_transfer' ever change
 * incompatibly, then the ioctl number (currently 0) must change;
 * ioctls with constant size fields get a bit more in the way of
 * error checking than ones (like this) where that field varies.
 *
 * NOTE: struct layout is the same in 64bit and 32bit userspace.
 * */
typedef struct {
    td_u64 tx_buf;
    td_u64 rx_buf;

    td_u32 len;
    td_u32 speed_hz;

    td_u16 delay_usecs;
    td_u8 bits_per_word;
    td_u8 cs_change;
    td_u32 pad;
} ot_spi_ioc_transfer;

/* not all platforms use <asm-generic/ioctl.h> or _IOC_TYPECHECK() ... */
#define ot_spi_msgsize(n) \
    ((((n) * (sizeof(ot_spi_ioc_transfer))) < (1 << _IOC_SIZEBITS)) \
            ? ((n) * (sizeof(ot_spi_ioc_transfer))) \
            : 0)
#define ot_spi_ioc_message(n) _IOW(OT_SPI_IOC_MAGIC, 0, char[ot_spi_msgsize(n)])

/* Read / Write of SPI mode (OT_SPI_MODE_0..OT_SPI_MODE_3) */
#define OT_SPI_IOC_RD_MODE          _IOR(OT_SPI_IOC_MAGIC, 1, td_u8)
#define OT_SPI_IOC_WR_MODE          _IOW(OT_SPI_IOC_MAGIC, 1, td_u8)

/* Read / Write SPI bit justification */
#define OT_SPI_IOC_RD_LSB_FIRST     _IOR(OT_SPI_IOC_MAGIC, 2, td_u8)
#define OT_SPI_IOC_WR_LSB_FIRST     _IOW(OT_SPI_IOC_MAGIC, 2, td_u8)

/* Read / Write SPI device word length (1..N) */
#define OT_SPI_IOC_RD_BITS_PER_WORD _IOR(OT_SPI_IOC_MAGIC, 3, td_u8)
#define OT_SPI_IOC_WR_BITS_PER_WORD _IOW(OT_SPI_IOC_MAGIC, 3, td_u8)

/* Read / Write SPI device default max speed hz */
#define OT_SPI_IOC_RD_MAX_SPEED_HZ  _IOR(OT_SPI_IOC_MAGIC, 4, td_u32)
#define OT_SPI_IOC_WR_MAX_SPEED_HZ  _IOW(OT_SPI_IOC_MAGIC, 4, td_u32)

#endif

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */

#endif /* OT_SPI_H */
