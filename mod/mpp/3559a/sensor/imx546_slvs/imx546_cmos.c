/*
 * IMX546 SLVS-8ch libsns skeleton for Hi3559AV100 / HIVIEW.
 *
 * Provides stSnsImx546SlvsObj + I2C init from FPGA register_config.
 * AE/AWB CMOS tables are minimal stubs — replace with Sony setting list
 * + full HiSilicon AE template before production use.
 */
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "hi_comm_video.h"
#include "hi_sns_ctrl.h"
#include "hi_comm_sns.h"
#include "hi_ae_comm.h"
#include "hi_awb_comm.h"
#include "mpi_isp.h"
#include "mpi_ae.h"
#include "mpi_awb.h"

#include "imx546_sensor_ctl.h"
#include "imx546_reg_table.h"

#define IMX546_ID               (546)
#define IMX546_FULL_LINES_STD   (IMX546_VMAX_FPGA_DEFAULT)
#define IMX546_FULL_LINES_MAX   (0x0FFFFF)
#define IMX546_RES_WIDTH        (2840)
#define IMX546_RES_HEIGHT       (2840)

/* SHS / GAIN register indices in ISP_SNS_REGS_INFO */
#define IMX546_REG_SHS_L        (0)
#define IMX546_REG_SHS_M        (1)
#define IMX546_REG_SHS_H        (2)
#define IMX546_REG_GAIN_L       (3)
#define IMX546_REG_GAIN_H       (4)
#define IMX546_REG_NUM          (5)

static ISP_SNS_STATE_S g_astImx546[ISP_MAX_PIPE_NUM] = {{0}};
static ISP_SNS_COMMBUS_U g_aunImx546Bus[ISP_MAX_PIPE_NUM] = {
    [0 ...(ISP_MAX_PIPE_NUM - 1)] = { .s8I2cDev = 0 }
};

#define IMX546_STATE(pipe) (&g_astImx546[(pipe)])

extern HI_S32 imx546_set_bus(VI_PIPE ViPipe, HI_S8 s8I2cDev);

static HI_VOID cmos_sensor_global_init(VI_PIPE ViPipe)
{
    ISP_SNS_STATE_S *pstSns = IMX546_STATE(ViPipe);

    pstSns->bInit = HI_FALSE;
    pstSns->bSyncInit = HI_FALSE;
    pstSns->u8ImgMode = 0;
    pstSns->enWDRMode = WDR_MODE_NONE;
    pstSns->u32FLStd = IMX546_FULL_LINES_STD;
    pstSns->au32FL[0] = IMX546_FULL_LINES_STD;
    pstSns->au32FL[1] = IMX546_FULL_LINES_STD;
    memset(&pstSns->astRegsInfo[0], 0, sizeof(ISP_SNS_REGS_INFO_S));
    memset(&pstSns->astRegsInfo[1], 0, sizeof(ISP_SNS_REGS_INFO_S));
}

static HI_VOID cmos_sensor_init(VI_PIPE ViPipe)
{
    imx546_linear_8m60_slvs8ch_init(ViPipe);
    IMX546_STATE(ViPipe)->bInit = HI_TRUE;
}

static HI_VOID cmos_sensor_exit(VI_PIPE ViPipe)
{
    imx546_stream_off(ViPipe);
    imx546_i2c_exit(ViPipe);
    IMX546_STATE(ViPipe)->bInit = HI_FALSE;
}

static HI_S32 cmos_set_image_mode(VI_PIPE ViPipe, ISP_CMOS_SENSOR_IMAGE_MODE_S *pstMode)
{
    if (pstMode == HI_NULL) {
        return HI_FAILURE;
    }
    /* only one linear mode for now */
    if ((pstMode->u16Width == IMX546_RES_WIDTH) &&
        (pstMode->u16Height == IMX546_RES_HEIGHT)) {
        IMX546_STATE(ViPipe)->u8ImgMode = 0;
        return HI_SUCCESS;
    }
    printf("imx546: unsupported image mode %ux%u@%.1f\n",
           pstMode->u16Width, pstMode->u16Height, pstMode->f32Fps);
    return HI_FAILURE;
}

static HI_S32 cmos_set_wdr_mode(VI_PIPE ViPipe, HI_U8 u8Mode)
{
    if (u8Mode != WDR_MODE_NONE) {
        printf("imx546: only linear supported\n");
        return HI_FAILURE;
    }
    IMX546_STATE(ViPipe)->enWDRMode = WDR_MODE_NONE;
    return HI_SUCCESS;
}

static HI_S32 cmos_get_isp_default(VI_PIPE ViPipe, ISP_CMOS_DEFAULT_S *pstDef)
{
    if (pstDef == HI_NULL) {
        return HI_FAILURE;
    }
    memset(pstDef, 0, sizeof(ISP_CMOS_DEFAULT_S));
    pstDef->unKey.bit1Demosaic = 0;
    pstDef->stSensorMaxResolution.u32MaxWidth  = IMX546_RES_WIDTH;
    pstDef->stSensorMaxResolution.u32MaxHeight = IMX546_RES_HEIGHT;
    pstDef->stSensorMode.u32SensorID = IMX546_ID;
    pstDef->stSensorMode.u8SensorMode = IMX546_STATE(ViPipe)->u8ImgMode;
    return HI_SUCCESS;
}

static HI_S32 cmos_get_isp_black_level(VI_PIPE ViPipe, ISP_CMOS_BLACK_LEVEL_S *pstBlc)
{
    HI_S32 i;
    (void)ViPipe;
    if (pstBlc == HI_NULL) {
        return HI_FAILURE;
    }
    pstBlc->bUpdate = HI_FALSE;
    for (i = 0; i < ISP_BAYER_CHN_NUM; i++) {
        pstBlc->au16BlackLevel[i] = 0xF0; /* matches FPGA BLKLEVEL tendency */
    }
    return HI_SUCCESS;
}

static HI_VOID cmos_set_pixel_detect(VI_PIPE ViPipe, HI_BOOL bEnable)
{
    (void)ViPipe;
    (void)bEnable;
}

static HI_S32 cmos_get_sns_reg_info(VI_PIPE ViPipe, ISP_SNS_REGS_INFO_S *pstRegs)
{
    ISP_SNS_STATE_S *pstSns = IMX546_STATE(ViPipe);
    HI_S32 i;

    if (pstRegs == HI_NULL) {
        return HI_FAILURE;
    }

    if (pstSns->bSyncInit == HI_FALSE) {
        pstSns->astRegsInfo[0].enSnsType = ISP_SNS_I2C_TYPE;
        pstSns->astRegsInfo[0].unComBus.s8I2cDev = g_aunImx546Bus[ViPipe].s8I2cDev;
        pstSns->astRegsInfo[0].u8Cfg2ValidDelayMax = 2;
        pstSns->astRegsInfo[0].u32RegNum = IMX546_REG_NUM;

        for (i = 0; i < IMX546_REG_NUM; i++) {
            pstSns->astRegsInfo[0].astI2cData[i].bUpdate = HI_TRUE;
            pstSns->astRegsInfo[0].astI2cData[i].u8DevAddr = IMX546_I2C_ADDR;
            pstSns->astRegsInfo[0].astI2cData[i].u32AddrByteNum = 2;
            pstSns->astRegsInfo[0].astI2cData[i].u32DataByteNum = 1;
        }
        pstSns->astRegsInfo[0].astI2cData[IMX546_REG_SHS_L].u32RegAddr = 0x3240;
        pstSns->astRegsInfo[0].astI2cData[IMX546_REG_SHS_M].u32RegAddr = 0x3241;
        pstSns->astRegsInfo[0].astI2cData[IMX546_REG_SHS_H].u32RegAddr = 0x3242;
        pstSns->astRegsInfo[0].astI2cData[IMX546_REG_GAIN_L].u32RegAddr = 0x3514;
        pstSns->astRegsInfo[0].astI2cData[IMX546_REG_GAIN_H].u32RegAddr = 0x3515;

        pstSns->bSyncInit = HI_TRUE;
    } else {
        for (i = 0; i < IMX546_REG_NUM; i++) {
            if (pstSns->astRegsInfo[0].astI2cData[i].u32Data ==
                pstSns->astRegsInfo[1].astI2cData[i].u32Data) {
                pstSns->astRegsInfo[0].astI2cData[i].bUpdate = HI_FALSE;
            } else {
                pstSns->astRegsInfo[0].astI2cData[i].bUpdate = HI_TRUE;
            }
        }
    }

    memcpy(pstRegs, &pstSns->astRegsInfo[0], sizeof(ISP_SNS_REGS_INFO_S));
    memcpy(&pstSns->astRegsInfo[1], &pstSns->astRegsInfo[0], sizeof(ISP_SNS_REGS_INFO_S));
    return HI_SUCCESS;
}

/* ---- AE ---- */
static HI_S32 cmos_get_ae_default(VI_PIPE ViPipe, AE_SENSOR_DEFAULT_S *pstAe)
{
    ISP_SNS_STATE_S *pstSns = IMX546_STATE(ViPipe);

    if (pstAe == HI_NULL) {
        return HI_FAILURE;
    }
    memset(pstAe, 0, sizeof(AE_SENSOR_DEFAULT_S));

    pstAe->u32FullLinesStd = pstSns->u32FLStd;
    pstAe->u32FullLinesMax = IMX546_FULL_LINES_MAX;
    pstAe->u32FullLines = pstSns->au32FL[0];
    pstAe->u32MaxIntTime = pstSns->u32FLStd - 8;
    pstAe->u32MinIntTime = 2;
    pstAe->u32MaxIntTimeTarget = 65535;
    pstAe->u32MinIntTimeTarget = 2;
    pstAe->stIntTimeAccu.enAccuType = AE_ACCURACY_LINEAR;
    pstAe->stIntTimeAccu.f32Accuracy = 1;
    pstAe->stIntTimeAccu.f32Offset = 0;

    pstAe->u32MaxAgain = 3981; /* ~24dB placeholder */
    pstAe->u32MinAgain = 1024;
    pstAe->u32MaxAgainTarget = pstAe->u32MaxAgain;
    pstAe->u32MinAgainTarget = pstAe->u32MinAgain;
    pstAe->stAgainAccu.enAccuType = AE_ACCURACY_TABLE;
    pstAe->stAgainAccu.f32Accuracy = 1;

    pstAe->u32MaxDgain = 1024;
    pstAe->u32MinDgain = 1024;
    pstAe->u32MaxDgainTarget = 1024;
    pstAe->u32MinDgainTarget = 1024;
    pstAe->stDgainAccu.enAccuType = AE_ACCURACY_LINEAR;
    pstAe->stDgainAccu.f32Accuracy = 1.0 / 256;

    pstAe->u32MaxISPDgainTarget = 2048;
    pstAe->u32MinISPDgainTarget = 1024;
    pstAe->u32ISPDgainShift = 8;

    pstAe->f32Fps = 60.0; /* target; FPGA VMAX currently ~34fps */
    pstAe->u32LinesPer500ms = (HI_U32)(pstAe->u32FullLinesStd * pstAe->f32Fps / 2);
    pstAe->u8AeCompensation = 0x40;
    pstAe->u32InitExposure = 0x4E20;
    return HI_SUCCESS;
}

static HI_VOID cmos_fps_set(VI_PIPE ViPipe, HI_FLOAT f32Fps, AE_SENSOR_DEFAULT_S *pstAe)
{
    ISP_SNS_STATE_S *pstSns = IMX546_STATE(ViPipe);
    (void)f32Fps;
    if (pstAe == HI_NULL) {
        return;
    }
    /* TODO: recompute VMAX for true 60fps from datasheet */
    pstAe->f32Fps = f32Fps;
    pstAe->u32FullLinesStd = pstSns->u32FLStd;
    pstAe->u32MaxIntTime = pstSns->u32FLStd - 8;
    pstSns->au32FL[0] = pstSns->u32FLStd;
}

static HI_VOID cmos_slow_framerate_set(VI_PIPE ViPipe, HI_U32 u32FullLines, AE_SENSOR_DEFAULT_S *pstAe)
{
    ISP_SNS_STATE_S *pstSns = IMX546_STATE(ViPipe);
    if (u32FullLines > IMX546_FULL_LINES_MAX) {
        u32FullLines = IMX546_FULL_LINES_MAX;
    }
    pstSns->au32FL[0] = u32FullLines;
    pstAe->u32FullLines = u32FullLines;
    pstAe->u32MaxIntTime = u32FullLines - 8;
}

static HI_VOID cmos_inttime_update(VI_PIPE ViPipe, HI_U32 u32IntTime)
{
    ISP_SNS_STATE_S *pstSns = IMX546_STATE(ViPipe);
    HI_U32 u32SHS;

    /* Sony GS: SHS = VMAX - integration - offset; placeholder */
    if (pstSns->au32FL[0] > (u32IntTime + 1)) {
        u32SHS = pstSns->au32FL[0] - u32IntTime - 1;
    } else {
        u32SHS = 1;
    }
    pstSns->astRegsInfo[0].astI2cData[IMX546_REG_SHS_L].u32Data = (u32SHS >> 0) & 0xFF;
    pstSns->astRegsInfo[0].astI2cData[IMX546_REG_SHS_M].u32Data = (u32SHS >> 8) & 0xFF;
    pstSns->astRegsInfo[0].astI2cData[IMX546_REG_SHS_H].u32Data = (u32SHS >> 16) & 0x0F;
}

static HI_VOID cmos_gains_update(VI_PIPE ViPipe, HI_U32 u32Again, HI_U32 u32Dgain)
{
    ISP_SNS_STATE_S *pstSns = IMX546_STATE(ViPipe);
    HI_U32 gain = u32Again; /* dB-code placeholder; map via again_calc_table */
    (void)u32Dgain;
    pstSns->astRegsInfo[0].astI2cData[IMX546_REG_GAIN_L].u32Data = (gain >> 0) & 0xFF;
    pstSns->astRegsInfo[0].astI2cData[IMX546_REG_GAIN_H].u32Data = (gain >> 8) & 0xFF;
}

static HI_VOID cmos_again_calc_table(VI_PIPE ViPipe, HI_U32 *pu32AgainLin, HI_U32 *pu32AgainDb)
{
    (void)ViPipe;
    if ((pu32AgainLin == HI_NULL) || (pu32AgainDb == HI_NULL)) {
        return;
    }
    /* linear 1024-based -> pass-through dB code stub */
    *pu32AgainDb = (*pu32AgainLin > 1024) ? ((*pu32AgainLin - 1024) / 16) : 0;
}

static HI_VOID cmos_dgain_calc_table(VI_PIPE ViPipe, HI_U32 *pu32DgainLin, HI_U32 *pu32DgainDb)
{
    (void)ViPipe;
    if ((pu32DgainLin == HI_NULL) || (pu32DgainDb == HI_NULL)) {
        return;
    }
    *pu32DgainDb = *pu32DgainLin;
}

/* ---- AWB ---- */
static HI_S32 cmos_get_awb_default(VI_PIPE ViPipe, AWB_SENSOR_DEFAULT_S *pstAwb)
{
    (void)ViPipe;
    if (pstAwb == HI_NULL) {
        return HI_FAILURE;
    }
    memset(pstAwb, 0, sizeof(AWB_SENSOR_DEFAULT_S));
    pstAwb->u16WbRefTemp = 5000;
    pstAwb->au16GainOffset[0] = 0x1A5;
    pstAwb->au16GainOffset[1] = 0x100;
    pstAwb->au16GainOffset[2] = 0x100;
    pstAwb->au16GainOffset[3] = 0x1C5;
    pstAwb->as32WbPara[0] = 52;
    pstAwb->as32WbPara[1] = 85;
    pstAwb->as32WbPara[2] = -95;
    pstAwb->as32WbPara[3] = 201460;
    pstAwb->as32WbPara[4] = 128;
    pstAwb->as32WbPara[5] = -151050;
    pstAwb->u16GoldenRgain = 0;
    pstAwb->u16GoldenBgain = 0;
    pstAwb->u8AWBRunInterval = 4;
    return HI_SUCCESS;
}

/* ---- ISP_SNS_OBJ ---- */
static HI_S32 imx546_register_callback(VI_PIPE ViPipe, ALG_LIB_S *pstAeLib, ALG_LIB_S *pstAwbLib)
{
    HI_S32 s32Ret;
    ISP_SENSOR_REGISTER_S stIspReg;
    AE_SENSOR_REGISTER_S  stAeReg;
    AWB_SENSOR_REGISTER_S stAwbReg;
    ISP_SNS_ATTR_INFO_S   stSnsAttr = { .eSensorId = IMX546_ID };

    if ((pstAeLib == HI_NULL) || (pstAwbLib == HI_NULL)) {
        return HI_FAILURE;
    }

    cmos_sensor_global_init(ViPipe);

    memset(&stIspReg, 0, sizeof(stIspReg));
    stIspReg.stSnsExp.pfn_cmos_sensor_init = cmos_sensor_init;
    stIspReg.stSnsExp.pfn_cmos_sensor_exit = cmos_sensor_exit;
    stIspReg.stSnsExp.pfn_cmos_sensor_global_init = cmos_sensor_global_init;
    stIspReg.stSnsExp.pfn_cmos_set_image_mode = cmos_set_image_mode;
    stIspReg.stSnsExp.pfn_cmos_set_wdr_mode = cmos_set_wdr_mode;
    stIspReg.stSnsExp.pfn_cmos_get_isp_default = cmos_get_isp_default;
    stIspReg.stSnsExp.pfn_cmos_get_isp_black_level = cmos_get_isp_black_level;
    stIspReg.stSnsExp.pfn_cmos_set_pixel_detect = cmos_set_pixel_detect;
    stIspReg.stSnsExp.pfn_cmos_get_sns_reg_info = cmos_get_sns_reg_info;

    s32Ret = HI_MPI_ISP_SensorRegCallBack(ViPipe, &stSnsAttr, &stIspReg);
    if (s32Ret != HI_SUCCESS) {
        printf("imx546: ISP_SensorRegCallBack failed %#x\n", s32Ret);
        return s32Ret;
    }

    memset(&stAeReg, 0, sizeof(stAeReg));
    stAeReg.stSnsExp.pfn_cmos_get_ae_default = cmos_get_ae_default;
    stAeReg.stSnsExp.pfn_cmos_fps_set = cmos_fps_set;
    stAeReg.stSnsExp.pfn_cmos_slow_framerate_set = cmos_slow_framerate_set;
    stAeReg.stSnsExp.pfn_cmos_inttime_update = cmos_inttime_update;
    stAeReg.stSnsExp.pfn_cmos_gains_update = cmos_gains_update;
    stAeReg.stSnsExp.pfn_cmos_again_calc_table = cmos_again_calc_table;
    stAeReg.stSnsExp.pfn_cmos_dgain_calc_table = cmos_dgain_calc_table;

    s32Ret = HI_MPI_AE_SensorRegCallBack(ViPipe, pstAeLib, &stSnsAttr, &stAeReg);
    if (s32Ret != HI_SUCCESS) {
        printf("imx546: AE_SensorRegCallBack failed %#x\n", s32Ret);
        return s32Ret;
    }

    memset(&stAwbReg, 0, sizeof(stAwbReg));
    stAwbReg.stSnsExp.pfn_cmos_get_awb_default = cmos_get_awb_default;

    s32Ret = HI_MPI_AWB_SensorRegCallBack(ViPipe, pstAwbLib, &stSnsAttr, &stAwbReg);
    if (s32Ret != HI_SUCCESS) {
        printf("imx546: AWB_SensorRegCallBack failed %#x\n", s32Ret);
        return s32Ret;
    }
    return HI_SUCCESS;
}

static HI_S32 imx546_unregister_callback(VI_PIPE ViPipe, ALG_LIB_S *pstAeLib, ALG_LIB_S *pstAwbLib)
{
    HI_S32 s32Ret;
    if ((pstAeLib == HI_NULL) || (pstAwbLib == HI_NULL)) {
        return HI_FAILURE;
    }
    s32Ret  = HI_MPI_ISP_SensorUnRegCallBack(ViPipe, IMX546_ID);
    s32Ret |= HI_MPI_AE_SensorUnRegCallBack(ViPipe, pstAeLib, IMX546_ID);
    s32Ret |= HI_MPI_AWB_SensorUnRegCallBack(ViPipe, pstAwbLib, IMX546_ID);
    return s32Ret;
}

static HI_S32 imx546_set_bus_info(VI_PIPE ViPipe, ISP_SNS_COMMBUS_U unBus)
{
    g_aunImx546Bus[ViPipe].s8I2cDev = unBus.s8I2cDev;
    imx546_set_bus(ViPipe, unBus.s8I2cDev);
    return HI_SUCCESS;
}

static HI_VOID imx546_standby(VI_PIPE ViPipe)
{
    imx546_stream_off(ViPipe);
}

static HI_VOID imx546_restart(VI_PIPE ViPipe)
{
    imx546_stream_on(ViPipe);
}

static HI_S32 imx546_write_reg_obj(VI_PIPE ViPipe, HI_S32 s32Addr, HI_S32 s32Data)
{
    return imx546_write_register(ViPipe, (HI_U32)s32Addr, (HI_U32)s32Data);
}

static HI_S32 imx546_read_reg_obj(VI_PIPE ViPipe, HI_S32 s32Addr)
{
    return imx546_read_register(ViPipe, (HI_U32)s32Addr);
}

ISP_SNS_OBJ_S stSnsImx546SlvsObj = {
    .pfnRegisterCallback   = imx546_register_callback,
    .pfnUnRegisterCallback = imx546_unregister_callback,
    .pfnSetBusInfo         = imx546_set_bus_info,
    .pfnStandby            = imx546_standby,
    .pfnRestart            = imx546_restart,
    .pfnMirrorFlip         = HI_NULL,
    .pfnWriteReg           = imx546_write_reg_obj,
    .pfnReadReg            = imx546_read_reg_obj,
    .pfnSetInit            = HI_NULL,
};
