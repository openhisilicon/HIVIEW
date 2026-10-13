#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <linux/i2c.h>
#include <linux/i2c-dev.h>

#include "hi_i2c.h"
#include "imx546_sensor_ctl.h"
#include "imx546_reg_table.h"

static int g_fd[ISP_MAX_PIPE_NUM] = {[0 ...(ISP_MAX_PIPE_NUM - 1)] = -1};
static int g_bus[ISP_MAX_PIPE_NUM] = {[0 ...(ISP_MAX_PIPE_NUM - 1)] = 0};

HI_S32 imx546_set_bus(VI_PIPE ViPipe, HI_S8 s8I2cDev)
{
    if (ViPipe >= ISP_MAX_PIPE_NUM) {
        return HI_FAILURE;
    }
    g_bus[ViPipe] = s8I2cDev;
    if (g_fd[ViPipe] >= 0) {
        imx546_i2c_exit(ViPipe);
    }
    return HI_SUCCESS;
}

HI_S32 imx546_i2c_init(VI_PIPE ViPipe)
{
    char dev[32];

    if (ViPipe >= ISP_MAX_PIPE_NUM) {
        return HI_FAILURE;
    }
    if (g_fd[ViPipe] >= 0) {
        return HI_SUCCESS;
    }

    snprintf(dev, sizeof(dev), "/dev/i2c-%d", g_bus[ViPipe]);
    g_fd[ViPipe] = open(dev, O_RDWR);
    if (g_fd[ViPipe] < 0) {
        printf("imx546: open %s failed\n", dev);
        return HI_FAILURE;
    }

    if (ioctl(g_fd[ViPipe], I2C_SLAVE_FORCE, IMX546_I2C_ADDR) < 0) {
        printf("imx546: I2C_SLAVE_FORCE 0x%02x failed\n", IMX546_I2C_ADDR);
        close(g_fd[ViPipe]);
        g_fd[ViPipe] = -1;
        return HI_FAILURE;
    }
    return HI_SUCCESS;
}

HI_S32 imx546_i2c_exit(VI_PIPE ViPipe)
{
    if (ViPipe < ISP_MAX_PIPE_NUM && g_fd[ViPipe] >= 0) {
        close(g_fd[ViPipe]);
        g_fd[ViPipe] = -1;
    }
    return HI_SUCCESS;
}

HI_S32 imx546_write_register(VI_PIPE ViPipe, HI_U32 addr, HI_U32 data)
{
    unsigned char buf[3];

    if (g_fd[ViPipe] < 0 && imx546_i2c_init(ViPipe) != HI_SUCCESS) {
        return HI_FAILURE;
    }

    buf[0] = (addr >> 8) & 0xff;
    buf[1] = addr & 0xff;
    buf[2] = data & 0xff;
    if (write(g_fd[ViPipe], buf, 3) != 3) {
        printf("imx546: write 0x%04x=0x%02x failed\n", addr, data & 0xff);
        return HI_FAILURE;
    }
    return HI_SUCCESS;
}

HI_S32 imx546_read_register(VI_PIPE ViPipe, HI_U32 addr)
{
    unsigned char buf[2];
    unsigned char val = 0;
    struct i2c_rdwr_ioctl_data rdwr;
    struct i2c_msg msg[2];

    if (g_fd[ViPipe] < 0 && imx546_i2c_init(ViPipe) != HI_SUCCESS) {
        return HI_FAILURE;
    }

    buf[0] = (addr >> 8) & 0xff;
    buf[1] = addr & 0xff;

    msg[0].addr  = IMX546_I2C_ADDR;
    msg[0].flags = 0;
    msg[0].len   = 2;
    msg[0].buf   = buf;
    msg[1].addr  = IMX546_I2C_ADDR;
    msg[1].flags = I2C_M_RD;
    msg[1].len   = 1;
    msg[1].buf   = &val;
    rdwr.msgs  = msg;
    rdwr.nmsgs = 2;

    if (ioctl(g_fd[ViPipe], I2C_RDWR, &rdwr) < 0) {
        return HI_FAILURE;
    }
    return val;
}

static HI_VOID imx546_write_array(VI_PIPE ViPipe, const IMX546_REG_S *regs, HI_U32 num)
{
    HI_U32 i;
    for (i = 0; i < num; i++) {
        imx546_write_register(ViPipe, regs[i].addr, regs[i].data);
    }
}

HI_VOID imx546_linear_8m60_slvs8ch_init(VI_PIPE ViPipe)
{
    printf("imx546: linear init (FPGA table, LANESEL_SLVS8ch=0x%02x)\n",
           IMX546_LANESEL_SLVS_8CH);
    printf("imx546: WARNING VMAX=0x%06x is FPGA default (~34fps), update for 60fps\n",
           IMX546_VMAX_FPGA_DEFAULT);

    imx546_i2c_init(ViPipe);
    imx546_write_array(ViPipe, g_imx546_linear_init,
                       (HI_U32)(sizeof(g_imx546_linear_init) / sizeof(g_imx546_linear_init[0])));
    usleep(20000);
    imx546_stream_on(ViPipe);
}

HI_VOID imx546_stream_on(VI_PIPE ViPipe)
{
    imx546_write_array(ViPipe, g_imx546_stream_on,
                       (HI_U32)(sizeof(g_imx546_stream_on) / sizeof(g_imx546_stream_on[0])));
}

HI_VOID imx546_stream_off(VI_PIPE ViPipe)
{
    imx546_write_array(ViPipe, g_imx546_stream_off,
                       (HI_U32)(sizeof(g_imx546_stream_off) / sizeof(g_imx546_stream_off[0])));
}
