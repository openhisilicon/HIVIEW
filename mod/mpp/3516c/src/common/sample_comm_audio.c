/*
  Copyright (c), 2001-2024, Shenshu Tech. Co., Ltd.
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <pthread.h>
#include <errno.h>
#include <signal.h>
#include <sys/select.h>
#include "sample_comm.h"
#include "hi_audio_aac_adp.h"
#include "hi_audio_mp3_adp.h"
#include "hi_audio_opus_adp.h"
#ifdef OT_ACODEC_TYPE_INNER
#include "hi_acodec.h"
#endif
#ifdef OT_ACODEC_TYPE_ES8388
#include "es8388.h"
#endif
#ifdef OT_ACODEC_TYPE_NVP6158
#include "common.h"
#endif
#ifdef OT_ACODEC_TYPE_TP2828
#include "tp2802.h"
#endif

#define AUDIO_ADPCM_TYPE HI_ADPCM_TYPE_DVI4 /* ADPCM_TYPE_IMA, ADPCM_TYPE_DVI4 */
#define G726_BPS HI_MEDIA_G726_40K          /* MEDIA_G726_16K, MEDIA_G726_24K ... */

#define AUDIO_MICIN_GAIN_OPEN 1             /* should be 1 when micin */

static hi_aac_type g_aac_type = HI_AAC_TYPE_AACLC;
static hi_aac_bps g_aac_bps = HI_AAC_BPS_96K;
static hi_aac_transport_type g_aac_transport_type = HI_AAC_TRANSPORT_TYPE_ADTS;

static hi_opus_application g_opus_app = HI_OPUS_APPLICATION_VOIP;
static hi_opus_bps g_opus_bps = HI_OPUS_BPS_96K;

typedef struct {
    hi_bool start;
    pthread_t aenc_pid;
    hi_s32 ae_chn;
    hi_s32 ad_chn;
    FILE *fd;
    hi_bool send_ad_chn;
    
    hi_payload_type pt;
    void *uargs;
    int (*cb)(hi_aenc_chn ae_chn, hi_payload_type pt, hi_audio_stream* stream, void* uargs);
} sample_aenc;

typedef struct {
    hi_bool start;
    hi_s32 ai_dev;
    hi_s32 ai_chn;
    hi_s32 aenc_chn;
    hi_s32 ao_dev;
    hi_s32 ao_chn;
    hi_bool send_aenc;
    hi_bool send_ao;
    pthread_t ai_pid;
} sample_ai;

typedef struct {
    hi_bool start;
    hi_s32 ad_chn;
    FILE *fd;
    pthread_t ad_pid;
} sample_adec;

typedef struct {
    hi_audio_dev ao_dev;
    hi_bool start;
    pthread_t ao_pid;
} sample_ao;

static sample_ai g_sample_ai[HI_AI_DEV_MAX_NUM * HI_AI_MAX_CHN_NUM];
static sample_aenc g_sample_aenc[HI_AENC_MAX_CHN_NUM];
static sample_adec g_sample_adec[HI_ADEC_MAX_CHN_NUM];
static sample_ao g_sample_ao[HI_AO_DEV_MAX_NUM];

#ifdef OT_ACODEC_TYPE_ES8388
hi_s32 sample_es8388_get_clk_ratio(hi_audio_sample_rate sample_rate, hi_u32 *fsclk_ratio, hi_u32 *bclk_ratio)
{
    switch (sample_rate) {
        case HI_AUDIO_SAMPLE_RATE_8000:
            *fsclk_ratio = ES8388_FSCLK_DIV_1024;
            *bclk_ratio = ES8388_BCLK_DIV_4;
            break;

        case HI_AUDIO_SAMPLE_RATE_12000:
            *fsclk_ratio = ES8388_FSCLK_DIV_1024;
            *bclk_ratio = ES8388_BCLK_DIV_4;
            break;

        case HI_AUDIO_SAMPLE_RATE_16000:
            *fsclk_ratio = ES8388_FSCLK_DIV_512;
            *bclk_ratio = ES8388_BCLK_DIV_4;
            break;

        case HI_AUDIO_SAMPLE_RATE_24000:
            *fsclk_ratio = ES8388_FSCLK_DIV_512;
            *bclk_ratio = ES8388_BCLK_DIV_4;
            break;

        case HI_AUDIO_SAMPLE_RATE_32000:
            *fsclk_ratio = ES8388_FSCLK_DIV_256;
            *bclk_ratio = ES8388_BCLK_DIV_4;
            break;

        case HI_AUDIO_SAMPLE_RATE_48000:
            *fsclk_ratio = ES8388_FSCLK_DIV_256;
            *bclk_ratio = ES8388_BCLK_DIV_4;
            break;

        default:
            printf("[Func]:%s [Line]:%d [Info]:not support sample_rate:%d.\n",
                __FUNCTION__, __LINE__, sample_rate);
            return HI_FAILURE;
    }

    return HI_SUCCESS;
}

hi_s32 sample_es8388_soft_reset(int codec_fd)
{
    hi_s32 ret;
    ot_es8388_audio_ctrl audio_ctrl;
    audio_ctrl.chip_num = ES8388_CHIP_ID;

    ret = ioctl(codec_fd, OT_ES8388_SOFT_RESET, &audio_ctrl);
    if (ret != HI_SUCCESS) {
        printf("[Func]:%s [Line]:%d [Info]:%s\n", __FUNCTION__, __LINE__, "es8388 reset failed");
        return ret;
    }

    return HI_SUCCESS;
}

hi_s32 sample_es8388_set_input_ctrl(int codec_fd)
{
    hi_s32 ret;
    ot_es8388_audio_ctrl audio_ctrl;
    audio_ctrl.chip_num = ES8388_CHIP_ID;

    /* set input select */
    audio_ctrl.audio_in_select = ES8388_INPUT2; /* refer to hardware */
    ret = ioctl(codec_fd, OT_ES8388_SET_LEFT_INPUT_SELECT, &audio_ctrl);
    if (ret != HI_SUCCESS) {
        printf("[Func]:%s [Line]:%d [Info]:%s\n", __FUNCTION__, __LINE__, "es83888 set left input fail.");
        return ret;
    }
    audio_ctrl.audio_in_select = ES8388_INPUT2; /* refer to hardware */
    ret = ioctl(codec_fd, OT_ES8388_SET_RIGHT_INPUT_SELECT, &audio_ctrl);
    if (ret != HI_SUCCESS) {
        printf("[Func]:%s [Line]:%d [Info]:%s\n", __FUNCTION__, __LINE__, "es83888 set right input fail.");
        return ret;
    }

    /* power on input */
    audio_ctrl.if_powerup = ES8388_POWER_UP;
    ret = ioctl(codec_fd, OT_ES8388_SET_LFET_INPUT_POWER, &audio_ctrl);
    if (ret != HI_SUCCESS) {
        printf("[Func]:%s [Line]:%d [Info]:%s\n", __FUNCTION__, __LINE__, "es83888 set left in power fail.");
        return ret;
    }
    ret = ioctl(codec_fd, OT_ES8388_SET_RIGHT_INPUT_POWER, &audio_ctrl);
    if (ret != HI_SUCCESS) {
        printf("[Func]:%s [Line]:%d [Info]:%s\n", __FUNCTION__, __LINE__, "es83888 set right in power fail.");
        return ret;
    }

    /* power on ADC */
    audio_ctrl.if_powerup = ES8388_POWER_UP;
    ret = ioctl(codec_fd, OT_ES8388_SET_LFET_ADC_POWER, &audio_ctrl);
    if (ret != HI_SUCCESS) {
        printf("[Func]:%s [Line]:%d [Info]:%s\n", __FUNCTION__, __LINE__, "es83888 set left ADC power fail.");
        return ret;
    }
    ret = ioctl(codec_fd, OT_ES8388_SET_RIGHT_ADC_POWER, &audio_ctrl);
    if (ret != HI_SUCCESS) {
        printf("[Func]:%s [Line]:%d [Info]:%s\n", __FUNCTION__, __LINE__, "es83888 set right ADC power fail.");
        return ret;
    }

    return HI_SUCCESS;
}

hi_s32 sample_es8388_set_clk_mode(int codec_fd, hi_aio_mode work_mode, hi_audio_sample_rate sample_rate)
{
    hi_s32 ret;
    hi_u32 master_ctrl = ES8388_SLAVE_MODE;
    ot_es8388_audio_ctrl audio_ctrl;
    audio_ctrl.chip_num = ES8388_CHIP_ID;

    /* set master clk */
    if (work_mode == HI_AIO_MODE_I2S_SLAVE || work_mode == HI_AIO_MODE_PCM_SLAVE_STD ||
        work_mode == HI_AIO_MODE_PCM_SLAVE_NON_STD) {
        hi_u32 fsclk_ratio, bclk_ratio;
        master_ctrl = ES8388_MASTER_MODE;

        ret = sample_es8388_get_clk_ratio(sample_rate, &fsclk_ratio, &bclk_ratio);
        if (ret != HI_SUCCESS) {
            return ret;
        }

        audio_ctrl.clk_div = bclk_ratio;
        ret = ioctl(codec_fd, OT_ES8388_SET_MCLK_BCLK_RATIO, &audio_ctrl);
        if (ret != HI_SUCCESS) {
            printf("[Func]:%s [Line]:%d [Info]:%s\n", __FUNCTION__, __LINE__, "es83888 set ADC LRCLK ratio fail.");
            return ret;
        }

        audio_ctrl.clk_div = fsclk_ratio;
        ret = ioctl(codec_fd, OT_ES8388_SET_ADC_MCLK_LRCLK_RATIO, &audio_ctrl);
        if (ret != HI_SUCCESS) {
            printf("[Func]:%s [Line]:%d [Info]:%s\n", __FUNCTION__, __LINE__, "es83888 set ADC LRCLK ratio fail.");
            return ret;
        }

        audio_ctrl.clk_div = fsclk_ratio;
        ret = ioctl(codec_fd, OT_ES8388_SET_DAC_MCLK_LRCLK_RATIO, &audio_ctrl);
        if (ret != HI_SUCCESS) {
            printf("[Func]:%s [Line]:%d [Info]:%s\n", __FUNCTION__, __LINE__, "es83888 set DAC LRCLK ratio fail.");
            return ret;
        }
    }

    /* set master/slave */
    audio_ctrl.ctrl_mode = master_ctrl;
    ret = ioctl(codec_fd, OT_ES8388_SET_MASTER_MODE, &audio_ctrl);
    if (ret != HI_SUCCESS) {
        printf("[Func]:%s [Line]:%d [Info]:%s\n", __FUNCTION__, __LINE__, "es83888 set master/slave fail.");
        return ret;
    }

    return HI_SUCCESS;
}

hi_s32 sample_es8388_set_clk_polarity(int codec_fd, hi_aio_mode work_mode)
{
    hi_s32 ret;
    hi_u32 pcm_offset, clk_dir;

    ot_es8388_audio_ctrl audio_ctrl;
    audio_ctrl.chip_num = ES8388_CHIP_ID;

    if (work_mode == HI_AIO_MODE_PCM_MASTER_STD || work_mode == HI_AIO_MODE_PCM_SLAVE_STD) {
        /* PCM offset is 1 */
        pcm_offset = ES8388_PCM_OFFSET_2ND;
        clk_dir = ES8388_BCLK_DIR_INVERT;
    } else if (work_mode == HI_AIO_MODE_PCM_MASTER_NON_STD || work_mode == HI_AIO_MODE_PCM_SLAVE_NON_STD) {
        /* PCM offset is 0 */
        pcm_offset = ES8388_PCM_OFFSET_1ST;
        clk_dir = ES8388_BCLK_DIR_INVERT;
    } else {
        /* I2S use normal polarity */
        pcm_offset = ES8388_I2S_POLARITY_NORMAL;
        clk_dir = ES8388_BCLK_DIR_NORMAL;
    }

    /* set PCM offset or I2S polarity */
    audio_ctrl.clk_polarity = pcm_offset;
    ret = ioctl(codec_fd, OT_ES8388_SET_ADC_POLARITY_AND_OFFSET, &audio_ctrl);
    if (ret != HI_SUCCESS) {
        printf("[Func]:%s [Line]:%d [Info]:%s\n", __FUNCTION__, __LINE__, "es83888 set ADC polarity or offset fail.");
        return ret;
    }
    ret = ioctl(codec_fd, OT_ES8388_SET_DAC_POLARITY_AND_OFFSET, &audio_ctrl);
    if (ret != HI_SUCCESS) {
        printf("[Func]:%s [Line]:%d [Info]:%s\n", __FUNCTION__, __LINE__, "es83888 set DAC polarity or offset fail.");
        return ret;
    }

    /* set bclk dir */
    audio_ctrl.clk_polarity = clk_dir;
    ret = ioctl(codec_fd, OT_ES8388_SET_BCLK_DIR, &audio_ctrl);
    if (ret != HI_SUCCESS) {
        printf("[Func]:%s [Line]:%d [Info]:%s\n", __FUNCTION__, __LINE__, "es83888 set bclk dir fail.");
        return ret;
    }

    return HI_SUCCESS;
}

hi_s32 sample_es8388_set_clk_ctrl(int codec_fd, hi_aio_mode work_mode, hi_audio_sample_rate sample_rate)
{
    hi_s32 ret;

    ret = sample_es8388_set_clk_mode(codec_fd, work_mode, sample_rate);
    if (ret != HI_SUCCESS) {
        return ret;
    }

    ret = sample_es8388_set_clk_polarity(codec_fd, work_mode);
    if (ret != HI_SUCCESS) {
        return ret;
    }

    return HI_SUCCESS;
}

hi_s32 sample_es8388_set_data_ctrl(int codec_fd, hi_aio_mode work_mode, hi_audio_bit_width bit_width)
{
    hi_s32 ret;
    hi_u32 format_ctrl;
    hi_u32 width_ctrl;
    ot_es8388_audio_ctrl audio_ctrl;
    audio_ctrl.chip_num = ES8388_CHIP_ID;

    /* set data format */
    if (work_mode == HI_AIO_MODE_I2S_MASTER || work_mode == HI_AIO_MODE_I2S_SLAVE) {
        /* i2s */
        format_ctrl = ES8388_DATA_FORMAT_I2S;
    } else if (work_mode == HI_AIO_MODE_PCM_MASTER_STD || work_mode == HI_AIO_MODE_PCM_SLAVE_STD ||
               work_mode == HI_AIO_MODE_PCM_MASTER_NON_STD || work_mode == HI_AIO_MODE_PCM_SLAVE_NON_STD) {
        /* pcm */
        format_ctrl = ES8388_DATA_FORMAT_PCM;
    } else {
        printf("[Func]:%s [Line]:%d [Info]:not support work_mode:%d.\n",
            __FUNCTION__, __LINE__, work_mode);
        return HI_FAILURE;
    }
    audio_ctrl.data_format = format_ctrl;
    ret = ioctl(codec_fd, OT_ES8388_SET_ADC_DATA_FORMAT, &audio_ctrl);
    if (ret != HI_SUCCESS) {
        printf("[Func]:%s [Line]:%d [Info]:%s\n", __FUNCTION__, __LINE__, "es83888 set ADC format fail.");
        return ret;
    }
    ret = ioctl(codec_fd, OT_ES8388_SET_DAC_DATA_FORMAT, &audio_ctrl);
    if (ret != HI_SUCCESS) {
        printf("[Func]:%s [Line]:%d [Info]:%s\n", __FUNCTION__, __LINE__, "es83888 set DAC format fail.");
        return ret;
    }

    /* set data width */
    if (bit_width == HI_AUDIO_BIT_WIDTH_16) {
        width_ctrl = ES8388_DATA_LENGTH_16BIT;
    } else if (bit_width == HI_AUDIO_BIT_WIDTH_24) {
        width_ctrl = ES8388_DATA_LENGTH_24BIT;
    } else {
        printf("[Func]:%s [Line]:%d [Info]:not support bit_width:%d.\n", __FUNCTION__, __LINE__, bit_width);
        return HI_FAILURE;
    }
    audio_ctrl.data_length = width_ctrl;
    ret = ioctl(codec_fd, OT_ES8388_SET_ADC_DATA_WIDTH, &audio_ctrl);
    if (ret != HI_SUCCESS) {
        printf("[Func]:%s [Line]:%d [Info]:%s\n", __FUNCTION__, __LINE__, "es83888 set ADC width fail.");
        return ret;
    }
    ret = ioctl(codec_fd, OT_ES8388_SET_DAC_DATA_WIDTH, &audio_ctrl);
    if (ret != HI_SUCCESS) {
        printf("[Func]:%s [Line]:%d [Info]:%s\n", __FUNCTION__, __LINE__, "es83888 set DAC width fail.");
        return ret;
    }

    return HI_SUCCESS;
}

hi_s32 sample_es8388_set_output_ctrl(int codec_fd)
{
    hi_s32 ret;
    ot_es8388_audio_ctrl audio_ctrl;
    audio_ctrl.chip_num = ES8388_CHIP_ID;

    /* power on DAC */
    audio_ctrl.if_powerup = ES8388_POWER_UP;
    ret = ioctl(codec_fd, OT_ES8388_SET_LFET_DAC_POWER, &audio_ctrl);
    if (ret != HI_SUCCESS) {
        printf("[Func]:%s [Line]:%d [Info]:%s\n", __FUNCTION__, __LINE__, "es83888 set left DAC power fail.");
        return ret;
    }
    ret = ioctl(codec_fd, OT_ES8388_SET_RIGHT_DAC_POWER, &audio_ctrl);
    if (ret != HI_SUCCESS) {
        printf("[Func]:%s [Line]:%d [Info]:%s\n", __FUNCTION__, __LINE__, "es83888 set right DAC power fail.");
        return ret;
    }

    /* enable output */
    audio_ctrl.if_powerup = ES8388_OUT_ENABLE;
    ret = ioctl(codec_fd, OT_ES8388_SET_LFET_OUTPUT1_ENABLE, &audio_ctrl);
    if (ret != HI_SUCCESS) {
        printf("[Func]:%s [Line]:%d [Info]:%s\n", __FUNCTION__, __LINE__, "es83888 set left out1 enable fail.");
        return ret;
    }
    ret = ioctl(codec_fd, OT_ES8388_SET_RIGHT_OUTPUT1_ENABLE, &audio_ctrl);
    if (ret != HI_SUCCESS) {
        printf("[Func]:%s [Line]:%d [Info]:%s\n", __FUNCTION__, __LINE__, "es83888 set right out1 enable fail.");
        return ret;
    }
    ret = ioctl(codec_fd, OT_ES8388_SET_LFET_OUTPUT2_ENABLE, &audio_ctrl);
    if (ret != HI_SUCCESS) {
        printf("[Func]:%s [Line]:%d [Info]:%s\n", __FUNCTION__, __LINE__, "es83888 set left out2 enable fail.");
        return ret;
    }
    ret = ioctl(codec_fd, OT_ES8388_SET_RIGHT_OUTPUT2_ENABLE, &audio_ctrl);
    if (ret != HI_SUCCESS) {
        printf("[Func]:%s [Line]:%d [Info]:%s\n", __FUNCTION__, __LINE__, "es83888 set right out2 enable fail.");
        return ret;
    }

    return HI_SUCCESS;
}

hi_s32 sample_es8388_set_input_volume(int codec_fd)
{
    hi_s32 ret;
    ot_es8388_audio_ctrl audio_ctrl;
    audio_ctrl.chip_num = ES8388_CHIP_ID;

    /* set PGA gain */
    audio_ctrl.volume = ES8388_PGA_GAIN_0DB;
    ret = ioctl(codec_fd, OT_ES8388_SET_LFET_INPUT_GAIN, &audio_ctrl);
    if (ret != HI_SUCCESS) {
        printf("[Func]:%s [Line]:%d [Info]:%s\n", __FUNCTION__, __LINE__, "es83888 set left gain fail.");
        return ret;
    }
    ret = ioctl(codec_fd, OT_ES8388_SET_RIGHT_INPUT_GAIN, &audio_ctrl);
    if (ret != HI_SUCCESS) {
        printf("[Func]:%s [Line]:%d [Info]:%s\n", __FUNCTION__, __LINE__, "es83888 set right gain fail.");
        return ret;
    }

    /* set ADC volume : 0dB */
    audio_ctrl.volume = ES8388_ADC_VOLUME_0DB;
    ret = ioctl(codec_fd, OT_ES8388_SET_LFET_ADC_VOLUME, &audio_ctrl);
    if (ret != HI_SUCCESS) {
        printf("[Func]:%s [Line]:%d [Info]:%s\n", __FUNCTION__, __LINE__, "es83888 set left ADC vol fail.");
        return ret;
    }
    ret = ioctl(codec_fd, OT_ES8388_SET_RIGHT_ADC_VOLUME, &audio_ctrl);
    if (ret != HI_SUCCESS) {
        printf("[Func]:%s [Line]:%d [Info]:%s\n", __FUNCTION__, __LINE__, "es83888 set right ADC vol fail.");
        return ret;
    }

    /* set ADC unmute */
    audio_ctrl.if_mute = ES8388_MUTE_DISABLE;
    ret = ioctl(codec_fd, OT_ES8388_SET_ADC_MUTE, &audio_ctrl);
    if (ret != HI_SUCCESS) {
        printf("[Func]:%s [Line]:%d [Info]:%s\n", __FUNCTION__, __LINE__, "es83888 set ADC unmute fail.");
        return ret;
    }

    return HI_SUCCESS;
}

hi_s32 sample_es8388_set_output_volume(int codec_fd)
{
    hi_s32 ret;
    ot_es8388_audio_ctrl audio_ctrl;
    audio_ctrl.chip_num = ES8388_CHIP_ID;

    /* set output volume : 0dB */
    audio_ctrl.volume = ES8388_OUTPUT_VOLUME_0DB;
    ret = ioctl(codec_fd, OT_ES8388_SET_LFET_OUTPUT1_VOLUME, &audio_ctrl);
    if (ret != HI_SUCCESS) {
        printf("[Func]:%s [Line]:%d [Info]:%s\n", __FUNCTION__, __LINE__, "es83888 set left out1 vol fail.");
        return ret;
    }
    ret = ioctl(codec_fd, OT_ES8388_SET_RIGHT_OUTPUT1_VOLUME, &audio_ctrl);
    if (ret != HI_SUCCESS) {
        printf("[Func]:%s [Line]:%d [Info]:%s\n", __FUNCTION__, __LINE__, "es83888 set right out1 vol fail.");
        return ret;
    }
    ret = ioctl(codec_fd, OT_ES8388_SET_LFET_OUTPUT2_VOLUME, &audio_ctrl);
    if (ret != HI_SUCCESS) {
        printf("[Func]:%s [Line]:%d [Info]:%s\n", __FUNCTION__, __LINE__, "es83888 set left out2 vol fail.");
        return ret;
    }
    ret = ioctl(codec_fd, OT_ES8388_SET_RIGHT_OUTPUT2_VOLUME, &audio_ctrl);
    if (ret != HI_SUCCESS) {
        printf("[Func]:%s [Line]:%d [Info]:%s\n", __FUNCTION__, __LINE__, "es83888 set right out2 vol fail.");
        return ret;
    }

    /* set DAC volume : 0dB */
    audio_ctrl.volume = ES8388_DAC_VOLUME_0DB;
    ret = ioctl(codec_fd, OT_ES8388_SET_LFET_DAC_VOLUME, &audio_ctrl);
    if (ret != HI_SUCCESS) {
        printf("[Func]:%s [Line]:%d [Info]:%s\n", __FUNCTION__, __LINE__, "es83888 set left DAC vol fail.");
        return ret;
    }
    ret = ioctl(codec_fd, OT_ES8388_SET_RIGHT_DAC_VOLUME, &audio_ctrl);
    if (ret != HI_SUCCESS) {
        printf("[Func]:%s [Line]:%d [Info]:%s\n", __FUNCTION__, __LINE__, "es83888 set right DAC vol fail.");
        return ret;
    }

    /* set DAC unmute */
    audio_ctrl.if_mute = ES8388_MUTE_DISABLE;
    ret = ioctl(codec_fd, OT_ES8388_SET_DAC_MUTE, &audio_ctrl);
    if (ret != HI_SUCCESS) {
        printf("[Func]:%s [Line]:%d [Info]:%s\n", __FUNCTION__, __LINE__, "es83888 set DAC unmute fail.");
        return ret;
    }

    return HI_SUCCESS;
}

hi_s32 sample_es8388_cfg_audio(hi_aio_mode work_mode, hi_audio_sample_rate sample_rate, hi_audio_bit_width bit_width)
{
    hi_s32 ret;
    int es8388_fd = -1;

    es8388_fd = open(ES8388_FILE, O_RDWR);
    if (es8388_fd < 0) {
        printf("can't open es8388(%s).\n", ES8388_FILE);
        return HI_FAILURE;
    }

    /* soft reset */
    ret = sample_es8388_soft_reset(es8388_fd);
    if (ret != HI_SUCCESS) {
        goto es8388_err;
    }

    /* set input ctrl */
    ret = sample_es8388_set_input_ctrl(es8388_fd);
    if (ret != HI_SUCCESS) {
        goto es8388_err;
    }

    /* set clk */
    ret = sample_es8388_set_clk_ctrl(es8388_fd, work_mode, sample_rate);
    if (ret != HI_SUCCESS) {
        goto es8388_err;
    }

    /* set data */
    ret = sample_es8388_set_data_ctrl(es8388_fd, work_mode, bit_width);
    if (ret != HI_SUCCESS) {
        goto es8388_err;
    }

    /* set output ctrl */
    ret = sample_es8388_set_output_ctrl(es8388_fd);
    if (ret != HI_SUCCESS) {
        goto es8388_err;
    }

    /* set input volume */
    ret = sample_es8388_set_input_volume(es8388_fd);
    if (ret != HI_SUCCESS) {
        goto es8388_err;
    }

    /* set output volume */
    ret = sample_es8388_set_output_volume(es8388_fd);
    if (ret != HI_SUCCESS) {
        goto es8388_err;
    }

    close(es8388_fd);
    printf("Set es8388 ok: work_mode = %d, sample_rate = %d, bit_width = %d\n", work_mode, sample_rate, bit_width);
    return HI_SUCCESS;

es8388_err:
    close(es8388_fd);
    return ret;
}

hi_s32 sample_es8388_disable(hi_void)
{
    hi_s32 ret;
    int es8388_fd = -1;

    es8388_fd = open(ES8388_FILE, O_RDWR);
    if (es8388_fd < 0) {
        printf("can't open es8388(%s).\n", ES8388_FILE);
        return HI_FAILURE;
    }

    /* soft reset */
    ret = sample_es8388_soft_reset(es8388_fd);

    close(es8388_fd);

    return ret;
}
#endif

#ifdef OT_ACODEC_TYPE_INNER
static hi_s32 inner_codec_get_i2s_fs(hi_audio_sample_rate sample_rate, hi_acodec_fs *i2s_fs)
{
    hi_acodec_fs i2s_fs_sel;

    switch (sample_rate) {
        case HI_AUDIO_SAMPLE_RATE_8000:
            i2s_fs_sel = HI_ACODEC_FS_8000;
            break;

        case HI_AUDIO_SAMPLE_RATE_16000:
            i2s_fs_sel = HI_ACODEC_FS_16000;
            break;

        case HI_AUDIO_SAMPLE_RATE_32000:
            i2s_fs_sel = HI_ACODEC_FS_32000;
            break;

        case HI_AUDIO_SAMPLE_RATE_11025:
            i2s_fs_sel = HI_ACODEC_FS_11025;
            break;

        case HI_AUDIO_SAMPLE_RATE_22050:
            i2s_fs_sel = HI_ACODEC_FS_22050;
            break;

        case HI_AUDIO_SAMPLE_RATE_44100:
            i2s_fs_sel = HI_ACODEC_FS_44100;
            break;

        case HI_AUDIO_SAMPLE_RATE_12000:
            i2s_fs_sel = HI_ACODEC_FS_12000;
            break;

        case HI_AUDIO_SAMPLE_RATE_24000:
            i2s_fs_sel = HI_ACODEC_FS_24000;
            break;

        case HI_AUDIO_SAMPLE_RATE_48000:
            i2s_fs_sel = HI_ACODEC_FS_48000;
            break;

        case HI_AUDIO_SAMPLE_RATE_64000:
            i2s_fs_sel = HI_ACODEC_FS_64000;
            break;

        case HI_AUDIO_SAMPLE_RATE_96000:
            i2s_fs_sel = HI_ACODEC_FS_96000;
            break;

        default:
            printf("%s: not support sample_rate:%d\n", __FUNCTION__, sample_rate);
            return HI_FAILURE;
    }

    *i2s_fs = i2s_fs_sel;
    return HI_SUCCESS;
}


hi_s32 sample_inner_codec_cfg_audio(hi_audio_sample_rate sample_rate)
{
    hi_s32 ret;
    hi_s32 fd_acodec = -1;
    hi_acodec_fs i2s_fs_sel;
    hi_acodec_mixer input_mode;

    fd_acodec = open(ACODEC_FILE, O_RDWR);
    if (fd_acodec < 0) {
        printf("%s: can't open audio codec,%s\n", __FUNCTION__, ACODEC_FILE);
        return HI_FAILURE;
    }

    ret = ioctl(fd_acodec, HI_ACODEC_SOFT_RESET_CTRL);
    if (ret != HI_SUCCESS) {
        printf("reset audio codec error\n");
        goto cfg_fail;
    }

    ret = inner_codec_get_i2s_fs(sample_rate, &i2s_fs_sel);
    if (ret != HI_SUCCESS) {
        goto cfg_fail;
    }

    ret = ioctl(fd_acodec, HI_ACODEC_SET_I2S1_FS, &i2s_fs_sel);
    if (ret != HI_SUCCESS) {
        printf("%s: set acodec sample rate failed\n", __FUNCTION__);
        goto cfg_fail;
    }

    /* refer to hardware, demo board is pseudo-differential (IN_D), socket board is single-ended (IN1) */
    input_mode = HI_ACODEC_MIXER_IN_D;
    ret = ioctl(fd_acodec, HI_ACODEC_SET_MIXER_MIC, &input_mode);
    if (ret != HI_SUCCESS) {
        printf("%s: select acodec input_mode failed\n", __FUNCTION__);
        goto cfg_fail;
    }

#if AUDIO_MICIN_GAIN_OPEN
    /*
     * The input volume range is [-78, 80]. Both the analog gain and digital gain are adjusted.
     * A larger value indicates higher volume.
     * For example, the value 80 indicates the maximum volume of 80 dB,
     * and the value -78 indicates the minimum volume (muted status).
     * The volume adjustment takes effect simultaneously in the audio-left and audio-right channels.
     * The recommended volume range is [20, 50].
     * Within this range, the noises are lowest because only the analog gain is adjusted,
     * and the voice quality can be guaranteed.
     */
    int acodec_input_vol;

    acodec_input_vol = 40; /* 30dB */
    ret = ioctl(fd_acodec,  HI_ACODEC_SET_INPUT_VOLUME, &acodec_input_vol);
    if (ret != HI_SUCCESS) {
        printf("%s: set acodec micin volume failed\n", __FUNCTION__);
        goto cfg_fail;
    }
#endif

    printf("set inner audio codec ok: sample_rate = %d.\n", sample_rate);

cfg_fail:
    close(fd_acodec);
    return ret;
}
#endif

/* config codec */
hi_s32 sample_comm_audio_cfg_acodec(const hi_aio_attr *aio_attr)
{
    hi_bool codec_cfg = HI_FALSE;

#ifdef OT_ACODEC_TYPE_ES8388
    hi_s32 ret = sample_es8388_cfg_audio(aio_attr->work_mode, aio_attr->sample_rate, aio_attr->bit_width);
    if (ret != HI_SUCCESS) {
        printf("%s: sample_es8388_cfg_audio failed\n", __FUNCTION__);
        return ret;
    }

    codec_cfg = HI_TRUE;
#endif

#ifdef OT_ACODEC_TYPE_INNER
    /* INNER AUDIO CODEC */
    hi_s32 ret = sample_inner_codec_cfg_audio(aio_attr->sample_rate);
    if (ret != HI_SUCCESS) {
        printf("%s:sample_inner_codec_cfg_audio failed\n", __FUNCTION__);
        return ret;
    }
    codec_cfg = HI_TRUE;
#endif

    if (codec_cfg == HI_FALSE) {
        printf("can not find the right codec.\n");
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

static hi_s32 audio_ai_get_frame_and_send(sample_ai *ai_ctl)
{
    hi_s32 ret;
    hi_audio_frame frame = {0};
    hi_aec_frame aec_frm = {0};

    /* get frame from ai chn */
    ret = hi_mpi_ai_get_frame(ai_ctl->ai_dev, ai_ctl->ai_chn, &frame, &aec_frm, HI_FALSE);
    if (ret != HI_SUCCESS) {
        /* continue */
        return HI_SUCCESS;
    }

    /* send frame to encoder */
    if (ai_ctl->send_aenc == HI_TRUE) {
        ret = hi_mpi_aenc_send_frame(ai_ctl->aenc_chn, &frame, &aec_frm);
        if (ret != HI_SUCCESS) {
            printf("%s: hi_mpi_aenc_send_frame(%d), failed with %#x!\n", __FUNCTION__, ai_ctl->aenc_chn, ret);
            hi_mpi_ai_release_frame(ai_ctl->ai_dev, ai_ctl->ai_chn, &frame, &aec_frm);
            return HI_FAILURE;
        }
    }

    /* send frame to ao */
    if (ai_ctl->send_ao == HI_TRUE) {
        ret = hi_mpi_ao_send_frame(ai_ctl->ao_dev, ai_ctl->ao_chn, &frame, 1000); /* 1000:1000ms */
        if (ret != HI_SUCCESS) {
            printf("%s: hi_mpi_ao_send_frame(%d, %d), failed with %#x!\n", __FUNCTION__, ai_ctl->ao_dev, ai_ctl->ao_chn,
                ret);
            hi_mpi_ai_release_frame(ai_ctl->ai_dev, ai_ctl->ai_chn, &frame, &aec_frm);
            return HI_FAILURE;
        }
    }

    /* finally you must release the stream */
    ret = hi_mpi_ai_release_frame(ai_ctl->ai_dev, ai_ctl->ai_chn, &frame, &aec_frm);
    if (ret != HI_SUCCESS) {
        printf("%s: hi_mpi_ai_release_frame(%d, %d), failed with %#x!\n", __FUNCTION__, ai_ctl->ai_dev, ai_ctl->ai_chn,
            ret);
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

/* get frame from ai, send it to aenc or ao */
void *sample_comm_audio_ai_proc(void *parg)
{
    hi_s32 ret;
    hi_s32 ai_fd;
    sample_ai *ai_ctl = (sample_ai *)parg;
    fd_set read_fds;
    struct timeval timeout_val;
    hi_ai_chn_param ai_chn_para;

    ret = hi_mpi_ai_get_chn_param(ai_ctl->ai_dev, ai_ctl->ai_chn, &ai_chn_para);
    if (ret != HI_SUCCESS) {
        printf("%s: get ai chn param failed\n", __FUNCTION__);
        return NULL;
    }

    ai_chn_para.usr_frame_depth = SAMPLE_AUDIO_AI_USER_FRAME_DEPTH;

    ret = hi_mpi_ai_set_chn_param(ai_ctl->ai_dev, ai_ctl->ai_chn, &ai_chn_para);
    if (ret != HI_SUCCESS) {
        printf("%s: set ai chn param failed\n", __FUNCTION__);
        return NULL;
    }

    FD_ZERO(&read_fds);
    ai_fd = hi_mpi_ai_get_fd(ai_ctl->ai_dev, ai_ctl->ai_chn);
    if (ai_fd < 0) {
        printf("%s: get ai fd failed\n", __FUNCTION__);
        return NULL;
    }
    FD_SET(ai_fd, &read_fds);

    while (ai_ctl->start) {
        timeout_val.tv_sec = 1;
        timeout_val.tv_usec = 0;

        FD_ZERO(&read_fds);
        FD_SET(ai_fd, &read_fds);

        ret = select(ai_fd + 1, &read_fds, NULL, NULL, &timeout_val);
        if (ret < 0) {
            break;
        } else if (ret == 0) {
            printf("%s: get ai frame select time out\n", __FUNCTION__);
            break;
        }

        if (FD_ISSET(ai_fd, &read_fds)) {
            /* get ai frame, send and release */
            ret = audio_ai_get_frame_and_send(ai_ctl);
            if (ret != HI_SUCCESS) {
                break;
            }
        }
    }

    ai_ctl->start = HI_FALSE;
    return NULL;
}

static hi_s32 audio_aenc_get_stream_and_send(sample_aenc *aenc_ctl)
{
    hi_s32 ret;
    hi_audio_stream stream = { 0 };

    /* get stream from aenc chn */
    ret = hi_mpi_aenc_get_stream(aenc_ctl->ae_chn, &stream, HI_FALSE);
    if (ret != HI_SUCCESS) {
        printf("%s: hi_mpi_aenc_get_stream(%d), failed with %#x!\n", __FUNCTION__, aenc_ctl->ae_chn, ret);
        return HI_FAILURE;
    }

    /* send stream to decoder and play for testing */
    if (aenc_ctl->send_ad_chn == HI_TRUE) {
        ret = hi_mpi_adec_send_stream(aenc_ctl->ad_chn, &stream, HI_TRUE);
        if (ret != HI_SUCCESS) {
            printf("%s: hi_mpi_adec_send_stream(%d), failed with %#x!\n", __FUNCTION__, aenc_ctl->ad_chn, ret);
            hi_mpi_aenc_release_stream(aenc_ctl->ae_chn, &stream);
            return HI_FAILURE;
        }
    }

    if(aenc_ctl->fd)
    {  
      /* save audio stream to file */
      (hi_void)fwrite(stream.stream, 1, stream.len, aenc_ctl->fd);
      (hi_void)fflush(aenc_ctl->fd);
    }
    if(aenc_ctl->cb)
    {
      aenc_ctl->cb(aenc_ctl->ae_chn, aenc_ctl->pt, &stream, aenc_ctl->uargs);
    }
    
    /* finally you must release the stream */
    ret = hi_mpi_aenc_release_stream(aenc_ctl->ae_chn, &stream);
    if (ret != HI_SUCCESS) {
        printf("%s: hi_mpi_aenc_release_stream(%d), failed with %#x!\n", __FUNCTION__, aenc_ctl->ae_chn, ret);
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

/* get stream from aenc, send it to adec & save it to file */
void *sample_comm_audio_aenc_proc(void *parg)
{
    hi_s32 ret;
    hi_s32 aenc_fd;
    sample_aenc *aenc_ctl = (sample_aenc *)parg;
    fd_set read_fds;
    struct timeval timeout_val;

    FD_ZERO(&read_fds);
    aenc_fd = hi_mpi_aenc_get_fd(aenc_ctl->ae_chn);
    if (aenc_fd < 0) {
        printf("%s: get aenc fd failed\n", __FUNCTION__);
        goto get_fd_fail;
    }
    FD_SET(aenc_fd, &read_fds);

    while (aenc_ctl->start) {
        timeout_val.tv_sec = 1;
        timeout_val.tv_usec = 0;

        FD_ZERO(&read_fds);
        FD_SET(aenc_fd, &read_fds);

        ret = select(aenc_fd + 1, &read_fds, NULL, NULL, &timeout_val);
        if (ret < 0) {
            break;
        } else if (ret == 0) {
            printf("%s: get aenc stream select time out\n", __FUNCTION__);
            break;
        }

        if (FD_ISSET(aenc_fd, &read_fds)) {
            /* get stream from aenc chn, send and release */
            ret = audio_aenc_get_stream_and_send(aenc_ctl);
            if (ret != HI_SUCCESS) {
                break;
            }
        }
    }

get_fd_fail:
    if(aenc_ctl->fd)
    {  
      (hi_void)fclose(aenc_ctl->fd);
      aenc_ctl->fd = HI_NULL;
    }
    aenc_ctl->start = HI_FALSE;
    return NULL;
}

/* get stream from file, and send it  to adec */
void *sample_comm_audio_adec_proc(void *parg)
{
    hi_s32 ret;
    hi_audio_stream audio_stream;
    const hi_u32 len = 640; /* 640: stream length */
    hi_u32 read_len;
    hi_s32 adec_chn;
    hi_u8 *audio_stream_tmp = NULL;
    sample_adec *adec_ctl = (sample_adec *)parg;
    FILE *fd = adec_ctl->fd;
    adec_chn = adec_ctl->ad_chn;

    audio_stream_tmp = (hi_u8 *)malloc(sizeof(hi_u8) * HI_MAX_AUDIO_STREAM_LEN);
    if (audio_stream_tmp == NULL) {
        printf("%s: malloc failed!\n", __FUNCTION__);
        goto stream_malloc_fail;
    }

    while (adec_ctl->start == HI_TRUE) {
        /* read from file */
        audio_stream.stream = audio_stream_tmp;
        read_len = fread(audio_stream.stream, 1, len, fd);
        if (read_len <= 0) {
            ret = hi_mpi_adec_send_end_of_stream(adec_chn, HI_FALSE);
            if (ret != HI_SUCCESS) {
                printf("%s: hi_mpi_adec_send_end_of_stream failed!\n", __FUNCTION__);
            }
            (hi_void)fseek(fd, 0, SEEK_SET); /* read file again */
            continue;
        }

        /* here only demo adec streaming sending mode, but pack sending mode is commended */
        audio_stream.len = read_len;
        ret = hi_mpi_adec_send_stream(adec_chn, &audio_stream, HI_TRUE);
        if (ret != HI_SUCCESS) {
            printf("%s: hi_mpi_adec_send_stream(%d) failed with %#x!\n", __FUNCTION__, adec_chn, ret);
            break;
        }
    }

    free(audio_stream_tmp);
    audio_stream_tmp = NULL;

stream_malloc_fail:
    (hi_void)fclose(adec_ctl->fd);
    adec_ctl->fd = HI_NULL;
    adec_ctl->start = HI_FALSE;
    return NULL;
}

/* set ao volume */
void *sample_comm_audio_ao_vol_proc(void *parg)
{
    hi_s32 ret;
    hi_s32 volume;
    hi_audio_fade fade;
    sample_ao *ao_ctl = (sample_ao *)parg;
    hi_audio_dev ao_dev = ao_ctl->ao_dev;

    while (ao_ctl->start) {
        for (volume = 0; volume <= 6; volume++) { /* 0,6:test range */
            ret = hi_mpi_ao_set_volume(ao_dev, volume);
            if (ret != HI_SUCCESS) {
                printf("%s: hi_mpi_ao_set_volume(%d), failed with %#x!\n", __FUNCTION__, ao_dev, ret);
            }
            printf("\rset volume %d          ", volume);
            sleep(2); /* 2:2s */
        }

        for (volume = 5; volume >= -15; volume--) { /* -15,5:test range */
            ret = hi_mpi_ao_set_volume(ao_dev, volume);
            if (ret != HI_SUCCESS) {
                printf("%s: hi_mpi_ao_set_volume(%d), failed with %#x!\n", __FUNCTION__, ao_dev, ret);
            }
            printf("\rset volume %d          ", volume);
            sleep(2); /* 2:2s */
        }

        for (volume = -14; volume <= 0; volume++) { /* -14,0:test range */
            ret = hi_mpi_ao_set_volume(ao_dev, volume);
            if (ret != HI_SUCCESS) {
                printf("%s: hi_mpi_ao_set_volume(%d), failed with %#x!\n", __FUNCTION__, ao_dev, ret);
            }
            printf("\rset volume %d          ", volume);
            sleep(2); /* 2:2s */
        }

        fade.fade = HI_TRUE;
        fade.fade_in_rate = HI_AUDIO_FADE_RATE_128;
        fade.fade_out_rate = HI_AUDIO_FADE_RATE_128;

        ret = hi_mpi_ao_set_mute(ao_dev, HI_TRUE, &fade);
        if (ret != HI_SUCCESS) {
            printf("%s: hi_mpi_ao_set_volume(%d), failed with %#x!\n", __FUNCTION__, ao_dev, ret);
        }
        printf("\rset ao mute            ");
        sleep(2); /* 2:2s */

        ret = hi_mpi_ao_set_mute(ao_dev, HI_FALSE, NULL);
        if (ret != HI_SUCCESS) {
            printf("%s: hi_mpi_ao_set_volume(%d), failed with %#x!\n", __FUNCTION__, ao_dev, ret);
        }
        printf("\rset ao unmute          ");
        sleep(2); /* 2:2s */
    }

    return NULL;
}

/* create the thread to get frame from ai and send to ao */
hi_s32 sample_comm_audio_create_thread_ai_ao(hi_audio_dev ai_dev, hi_ai_chn ai_chn,
    hi_audio_dev ao_dev, hi_ao_chn ao_chn)
{
    sample_ai *ai = NULL;

    if ((ai_dev >= HI_AI_DEV_MAX_NUM) || (ai_dev < 0) ||
        (ai_chn >= HI_AI_MAX_CHN_NUM) || (ai_chn < 0)) {
        printf("%s: ai_dev = %d, ai_chn = %d error.\n", __FUNCTION__, ai_dev, ai_chn);
        return HI_FAILURE;
    }

    ai = &g_sample_ai[ai_dev * HI_AI_MAX_CHN_NUM + ai_chn];
    ai->send_aenc = HI_FALSE;
    ai->send_ao = HI_TRUE;
    ai->start = HI_TRUE;
    ai->ai_dev = ai_dev;
    ai->ai_chn = ai_chn;
    ai->ao_dev = ao_dev;
    ai->ao_chn = ao_chn;

    pthread_create(&ai->ai_pid, 0, sample_comm_audio_ai_proc, ai);

    return HI_SUCCESS;
}

/* create the thread to get frame from ai and send to aenc */
hi_s32 sample_comm_audio_create_thread_ai_aenc(hi_audio_dev ai_dev, hi_ai_chn ai_chn, hi_aenc_chn ae_chn)
{
    sample_ai *ai = NULL;

    if ((ai_dev >= HI_AI_DEV_MAX_NUM) || (ai_dev < 0) ||
        (ai_chn >= HI_AI_MAX_CHN_NUM) || (ai_chn < 0)) {
        printf("%s: ai_dev = %d, ai_chn = %d error.\n", __FUNCTION__, ai_dev, ai_chn);
        return HI_FAILURE;
    }

    ai = &g_sample_ai[ai_dev * HI_AI_MAX_CHN_NUM + ai_chn];
    ai->send_aenc = HI_TRUE;
    ai->send_ao = HI_FALSE;
    ai->start = HI_TRUE;
    ai->ai_dev = ai_dev;
    ai->ai_chn = ai_chn;
    ai->aenc_chn = ae_chn;
    pthread_create(&ai->ai_pid, 0, sample_comm_audio_ai_proc, ai);

    return HI_SUCCESS;
}


hi_s32 sample_comm_audio_set_aenc_cb(hi_aenc_chn ae_chn, hi_payload_type pt, int (*cb)(hi_aenc_chn ae_chn, hi_payload_type pt, hi_audio_stream* stream, void* uargs), void *uargs)
{
  sample_aenc *aenc = &g_sample_aenc[ae_chn];
  aenc->pt = pt;
  aenc->cb = cb;
  aenc->uargs = uargs;
  return 0;
}


/* create the thread to get stream from aenc and send to adec */
hi_s32 sample_comm_audio_create_thread_aenc_adec(hi_aenc_chn ae_chn, hi_adec_chn ad_chn, FILE *aenc_fd)
{
    sample_aenc *aenc = NULL;

#if 0
    if (aenc_fd == NULL) {
        return HI_FAILURE;
    }
#endif

    if ((ae_chn >= HI_AENC_MAX_CHN_NUM) || (ae_chn < 0)) {
        printf("%s: ae_chn = %d error.\n", __FUNCTION__, ae_chn);
        return HI_FAILURE;
    }

    aenc = &g_sample_aenc[ae_chn];
    aenc->ae_chn = ae_chn;
    aenc->ad_chn = ad_chn;
    aenc->send_ad_chn = (aenc_fd == NULL)?HI_FALSE:HI_TRUE;
    aenc->fd = aenc_fd;
    aenc->start = HI_TRUE;
    pthread_create(&aenc->aenc_pid, 0, sample_comm_audio_aenc_proc, aenc);

    return HI_SUCCESS;
}

/* create the thread to get stream from file and send to adec */
hi_s32 sample_comm_audio_create_thread_file_adec(hi_adec_chn ad_chn, FILE *adec_fd)
{
    sample_adec *adec = NULL;

    if (adec_fd == NULL) {
        return HI_FAILURE;
    }

    if ((ad_chn >= HI_ADEC_MAX_CHN_NUM) || (ad_chn < 0)) {
        printf("%s: ad_chn = %d error.\n", __FUNCTION__, ad_chn);
        return HI_FAILURE;
    }

    adec = &g_sample_adec[ad_chn];
    adec->ad_chn = ad_chn;
    adec->fd = adec_fd;
    adec->start = HI_TRUE;
    pthread_create(&adec->ad_pid, 0, sample_comm_audio_adec_proc, adec);

    return HI_SUCCESS;
}

/* create the thread to set ao volume */
hi_s32 sample_comm_audio_create_thread_ao_vol_ctrl(hi_audio_dev ao_dev)
{
    sample_ao *ao_ctl = NULL;

    if ((ao_dev >= HI_AO_DEV_MAX_NUM) || (ao_dev < 0)) {
        printf("%s: ao_dev = %d error.\n", __FUNCTION__, ao_dev);
        return HI_FAILURE;
    }

    ao_ctl = &g_sample_ao[ao_dev];
    ao_ctl->ao_dev = ao_dev;
    ao_ctl->start = HI_TRUE;
    pthread_create(&ao_ctl->ao_pid, 0, sample_comm_audio_ao_vol_proc, ao_ctl);

    return HI_SUCCESS;
}

/* destroy the thread to get frame from ai and send to ao or aenc */
hi_s32 sample_comm_audio_destroy_thread_ai(hi_audio_dev ai_dev, hi_ai_chn ai_chn)
{
    sample_ai *ai = NULL;

    if ((ai_dev >= HI_AI_DEV_MAX_NUM) || (ai_dev < 0) ||
        (ai_chn >= HI_AI_MAX_CHN_NUM) || (ai_chn < 0)) {
        printf("%s: ai_dev = %d, ai_chn = %d error.\n", __FUNCTION__, ai_dev, ai_chn);
        return HI_FAILURE;
    }

    ai = &g_sample_ai[ai_dev * HI_AI_MAX_CHN_NUM + ai_chn];
    if (ai->start) {
        ai->start = HI_FALSE;
        pthread_join(ai->ai_pid, 0);
    }

    return HI_SUCCESS;
}

/* destroy the thread to get stream from aenc and send to adec */
hi_s32 sample_comm_audio_destroy_thread_aenc_adec(hi_aenc_chn ae_chn)
{
    sample_aenc *aenc = NULL;

    if ((ae_chn >= HI_AENC_MAX_CHN_NUM) || (ae_chn < 0)) {
        printf("%s: ae_chn = %d error.\n", __FUNCTION__, ae_chn);
        return HI_FAILURE;
    }

    aenc = &g_sample_aenc[ae_chn];
    if (aenc->start) {
        aenc->start = HI_FALSE;
        pthread_join(aenc->aenc_pid, 0);
    }

    if (aenc->fd != HI_NULL) {
        fclose(aenc->fd);
        aenc->fd = HI_NULL;
    }

    return HI_SUCCESS;
}

/* destroy the thread to get stream from file and send to adec */
hi_s32 sample_comm_audio_destroy_thread_file_adec(hi_adec_chn ad_chn)
{
    sample_adec *adec = NULL;

    if ((ad_chn >= HI_ADEC_MAX_CHN_NUM) || (ad_chn < 0)) {
        printf("%s: ad_chn = %d error.\n", __FUNCTION__, ad_chn);
        return HI_FAILURE;
    }

    adec = &g_sample_adec[ad_chn];
    if (adec->start) {
        adec->start = HI_FALSE;
        pthread_join(adec->ad_pid, 0);
    }

    if (adec->fd != HI_NULL) {
        fclose(adec->fd);
        adec->fd = HI_NULL;
    }

    return HI_SUCCESS;
}

/* destroy the thread to set ao volume */
hi_s32 sample_comm_audio_destroy_thread_ao_vol_ctrl(hi_audio_dev ao_dev)
{
    sample_ao *ao_ctl = NULL;

    if ((ao_dev >= HI_AO_DEV_MAX_NUM) || (ao_dev < 0)) {
        printf("%s: ao_dev = %d error.\n", __FUNCTION__, ao_dev);
        return HI_FAILURE;
    }

    ao_ctl = &g_sample_ao[ao_dev];
    if (ao_ctl->start) {
        ao_ctl->start = HI_FALSE;
        pthread_cancel(ao_ctl->ao_pid);
        pthread_join(ao_ctl->ao_pid, 0);
    }

    return HI_SUCCESS;
}

/* ao bind adec */
hi_s32 sample_comm_audio_ao_bind_adec(hi_audio_dev ao_dev, hi_ao_chn ao_chn, hi_adec_chn ad_chn)
{
    hi_mpp_chn src_chn, dest_chn;

    src_chn.mod_id = HI_ID_ADEC;
    src_chn.dev_id = 0;
    src_chn.chn_id = ad_chn;
    dest_chn.mod_id = HI_ID_AO;
    dest_chn.dev_id = ao_dev;
    dest_chn.chn_id = ao_chn;

    return hi_mpi_sys_bind(&src_chn, &dest_chn);
}

/* ao unbind adec */
hi_s32 sample_comm_audio_ao_unbind_adec(hi_audio_dev ao_dev, hi_ao_chn ao_chn, hi_adec_chn ad_chn)
{
    hi_mpp_chn src_chn, dest_chn;

    src_chn.mod_id = HI_ID_ADEC;
    src_chn.chn_id = ad_chn;
    src_chn.dev_id = 0;
    dest_chn.mod_id = HI_ID_AO;
    dest_chn.dev_id = ao_dev;
    dest_chn.chn_id = ao_chn;

    return hi_mpi_sys_unbind(&src_chn, &dest_chn);
}

/* ao bind ai */
hi_s32 sample_comm_audio_ao_bind_ai(hi_audio_dev ai_dev, hi_ai_chn ai_chn, hi_audio_dev ao_dev, hi_ao_chn ao_chn)
{
    hi_mpp_chn src_chn, dest_chn;

    src_chn.mod_id = HI_ID_AI;
    src_chn.chn_id = ai_chn;
    src_chn.dev_id = ai_dev;
    dest_chn.mod_id = HI_ID_AO;
    dest_chn.dev_id = ao_dev;
    dest_chn.chn_id = ao_chn;

    return hi_mpi_sys_bind(&src_chn, &dest_chn);
}

/* ao unbind ai */
hi_s32 sample_comm_audio_ao_unbind_ai(hi_audio_dev ai_dev, hi_ai_chn ai_chn, hi_audio_dev ao_dev, hi_ao_chn ao_chn)
{
    hi_mpp_chn src_chn, dest_chn;

    src_chn.mod_id = HI_ID_AI;
    src_chn.chn_id = ai_chn;
    src_chn.dev_id = ai_dev;
    dest_chn.mod_id = HI_ID_AO;
    dest_chn.dev_id = ao_dev;
    dest_chn.chn_id = ao_chn;

    return hi_mpi_sys_unbind(&src_chn, &dest_chn);
}

/* aenc bind ai */
hi_s32 sample_comm_audio_aenc_bind_ai(hi_audio_dev ai_dev, hi_ai_chn ai_chn, hi_aenc_chn ae_chn)
{
    hi_mpp_chn src_chn, dest_chn;

    src_chn.mod_id = HI_ID_AI;
    src_chn.dev_id = ai_dev;
    src_chn.chn_id = ai_chn;
    dest_chn.mod_id = HI_ID_AENC;
    dest_chn.dev_id = 0;
    dest_chn.chn_id = ae_chn;

    return hi_mpi_sys_bind(&src_chn, &dest_chn);
}

/* aenc unbind ai */
hi_s32 sample_comm_audio_aenc_unbind_ai(hi_audio_dev ai_dev, hi_ai_chn ai_chn, hi_aenc_chn ae_chn)
{
    hi_mpp_chn src_chn, dest_chn;

    src_chn.mod_id = HI_ID_AI;
    src_chn.dev_id = ai_dev;
    src_chn.chn_id = ai_chn;
    dest_chn.mod_id = HI_ID_AENC;
    dest_chn.dev_id = 0;
    dest_chn.chn_id = ae_chn;

    return hi_mpi_sys_unbind(&src_chn, &dest_chn);
}

static hi_s32 sample_comm_audio_start_ai_vqe(hi_audio_dev ai_dev_id, hi_ai_chn ai_chn,
    const sample_comm_ai_vqe_param *ai_vqe_param, hi_audio_dev ao_dev_id)
{
    hi_s32 ret;

    if (ai_vqe_param->ai_vqe_attr != NULL) {
        hi_bool ai_vqe = HI_TRUE;
        switch (ai_vqe_param->ai_vqe_type) {
            case SAMPLE_AUDIO_VQE_TYPE_NONE:
                ret = HI_SUCCESS;
                ai_vqe = HI_FALSE;
                break;
            case SAMPLE_AUDIO_VQE_TYPE_RECORD:
                ret = hi_mpi_ai_set_record_vqe_attr(ai_dev_id, ai_chn,
                    (hi_ai_record_vqe_cfg *)ai_vqe_param->ai_vqe_attr);
                break;
            case SAMPLE_AUDIO_VQE_TYPE_TALK:
                ret = hi_mpi_ai_set_talk_vqe_attr(ai_dev_id, ai_chn, ao_dev_id, ai_chn,
                    (hi_ai_talk_vqe_cfg *)ai_vqe_param->ai_vqe_attr);
                break;
            case SAMPLE_AUDIO_VQE_TYPE_TALKV2:
                ret = hi_mpi_ai_set_talk_vqe_v2_attr(ai_dev_id, ai_chn, ao_dev_id, ai_chn,
                    (hi_ai_talk_vqe_v2_cfg *)ai_vqe_param->ai_vqe_attr);
                break;
            default:
                ret = HI_FAILURE;
                break;
        }
        if (ret != HI_SUCCESS) {
            printf("%s: set_ai_vqe%d(%d,%d) failed with %#x\n", __FUNCTION__, ai_vqe_param->ai_vqe_type,
                ai_dev_id, ai_chn, ret);
            return ret;
        }

        if (ai_vqe == HI_TRUE) {
            ret = hi_mpi_ai_enable_vqe(ai_dev_id, ai_chn);
            if (ret) {
                printf("%s: hi_mpi_ai_enable_vqe(%d,%d) failed with %#x\n", __FUNCTION__, ai_dev_id, ai_chn, ret);
                return ret;
            }
        }
    }

    return HI_SUCCESS;
}

/* start ai */
hi_s32 sample_comm_audio_start_ai(hi_audio_dev ai_dev_id, hi_u32 ai_chn_cnt, hi_aio_attr *aio_attr,
    const sample_comm_ai_vqe_param *ai_vqe_param, hi_audio_dev ao_dev_id)
{
    hi_s32 i;
    hi_s32 ret;
    hi_u32 chn_cnt;

    ret = hi_mpi_ai_set_pub_attr(ai_dev_id, aio_attr);
    if (ret) {
        printf("%s: hi_mpi_ai_set_pub_attr(%d) failed with %#x\n", __FUNCTION__, ai_dev_id, ret);
        return ret;
    }

    ret = hi_mpi_ai_enable(ai_dev_id);
    if (ret) {
        printf("%s: hi_mpi_ai_enable(%d) failed with %#x\n", __FUNCTION__, ai_dev_id, ret);
        return ret;
    }

    chn_cnt = ai_chn_cnt >> ((hi_u32)aio_attr->snd_mode);
    for (i = 0; i < (hi_s32)chn_cnt; i++) {
        ret = hi_mpi_ai_enable_chn(ai_dev_id, i);
        if (ret) {
            printf("%s: hi_mpi_ai_enable_chn(%d,%d) failed with %#x\n", __FUNCTION__, ai_dev_id, i, ret);
            return ret;
        }

        if (ai_vqe_param->resample_en == HI_TRUE) {
            ret = hi_mpi_ai_enable_resample(ai_dev_id, i, ai_vqe_param->out_sample_rate);
            if (ret) {
                printf("%s: hi_mpi_ai_enable_re_smp(%d,%d) failed with %#x\n", __FUNCTION__, ai_dev_id, i, ret);
                return ret;
            }
        }

        ret = sample_comm_audio_start_ai_vqe(ai_dev_id, i, ai_vqe_param, ao_dev_id);
        if (ret != HI_SUCCESS) {
            return ret;
        }
    }

    return HI_SUCCESS;
}

/* stop ai */
hi_s32 sample_comm_audio_stop_ai(hi_audio_dev ai_dev_id, hi_u32 ai_chn_cnt, hi_bool resample_en, hi_bool vqe_en)
{
    hi_s32 i;
    hi_s32 ret;

    for (i = 0; i < (hi_s32)ai_chn_cnt; i++) {
        if (resample_en == HI_TRUE) {
            ret = hi_mpi_ai_disable_resample(ai_dev_id, i);
            if (ret != HI_SUCCESS) {
                printf("[func]:%s [line]:%d [info]:%s\n", __FUNCTION__, __LINE__, "failed");
                return ret;
            }
        }

        if (vqe_en == HI_TRUE) {
            ret = hi_mpi_ai_disable_vqe(ai_dev_id, i);
            if (ret != HI_SUCCESS) {
                printf("[func]:%s [line]:%d [info]:%s\n", __FUNCTION__, __LINE__, "failed");
                return ret;
            }
        }

        ret = hi_mpi_ai_disable_chn(ai_dev_id, i);
        if (ret != HI_SUCCESS) {
            printf("[func]:%s [line]:%d [info]:%s\n", __FUNCTION__, __LINE__, "failed");
            return ret;
        }
    }

    ret = hi_mpi_ai_disable(ai_dev_id);
    if (ret != HI_SUCCESS) {
        printf("[func]:%s [line]:%d [info]:%s\n", __FUNCTION__, __LINE__, "failed");
        return ret;
    }

    return HI_SUCCESS;
}

/* start ao */
hi_s32 sample_comm_audio_start_ao(hi_audio_dev ao_dev_id, hi_u32 ao_chn_cnt, hi_aio_attr *aio_attr,
    hi_audio_sample_rate in_sample_rate, hi_bool resample_en)
{
    hi_s32 i;
    hi_s32 ret;
    hi_u32 chn_cnt;

    ret = hi_mpi_ao_set_pub_attr(ao_dev_id, aio_attr);
    if (ret != HI_SUCCESS) {
        printf("%s: hi_mpi_ao_set_pub_attr(%d) failed with %#x!\n", __FUNCTION__, ao_dev_id, ret);
        return HI_FAILURE;
    }

    ret = hi_mpi_ao_enable(ao_dev_id);
    if (ret != HI_SUCCESS) {
        printf("%s: hi_mpi_ao_enable(%d) failed with %#x!\n", __FUNCTION__, ao_dev_id, ret);
        return HI_FAILURE;
    }

    chn_cnt = ao_chn_cnt >> ((hi_u32)aio_attr->snd_mode);
    for (i = 0; i < (hi_s32)chn_cnt; i++) {
        ret = hi_mpi_ao_enable_chn(ao_dev_id, i);
        if (ret != HI_SUCCESS) {
            printf("%s: hi_mpi_ao_enable_chn(%d) failed with %#x!\n", __FUNCTION__, i, ret);
            return HI_FAILURE;
        }

        if (resample_en == HI_TRUE) {
            ret = hi_mpi_ao_disable_resample(ao_dev_id, i);
            if (ret != HI_SUCCESS) {
                printf("%s: hi_mpi_ao_disable_resample (%d,%d) failed with %#x!\n", __FUNCTION__, ao_dev_id, i, ret);
                return HI_FAILURE;
            }

            ret = hi_mpi_ao_enable_resample(ao_dev_id, i, in_sample_rate);
            if (ret != HI_SUCCESS) {
                printf("%s: hi_mpi_ao_enable_resample(%d,%d) failed with %#x!\n", __FUNCTION__, ao_dev_id, i, ret);
                return HI_FAILURE;
            }
        }
    }
#if 0
    ret = hi_mpi_ao_enable_chn(ao_dev_id, HI_AO_SYS_CHN_ID);
    if (ret != HI_SUCCESS) {
        printf("%s: hi_mpi_ao_enable_chn(%d) failed with %#x!\n", __FUNCTION__, i, ret);
        return HI_FAILURE;
    }
#endif
    return HI_SUCCESS;
}

/* stop ao */
hi_s32 sample_comm_audio_stop_ao(hi_audio_dev ao_dev_id, hi_u32 ao_chn_cnt, hi_bool resample_en)
{
    hi_s32 i;
    hi_s32 ret;

    for (i = 0; i < (hi_s32)ao_chn_cnt; i++) {
        if (resample_en == HI_TRUE) {
            ret = hi_mpi_ao_disable_resample(ao_dev_id, i);
            if (ret != HI_SUCCESS) {
                printf("%s: hi_mpi_ao_disable_re_smp failed with %#x!\n", __FUNCTION__, ret);
                return ret;
            }
        }

        ret = hi_mpi_ao_disable_chn(ao_dev_id, i);
        if (ret != HI_SUCCESS) {
            printf("%s: hi_mpi_ao_disable_chn failed with %#x!\n", __FUNCTION__, ret);
            return ret;
        }
    }
#if 0
    ret = hi_mpi_ao_disable_chn(ao_dev_id, HI_AO_SYS_CHN_ID);
    if (ret != HI_SUCCESS) {
        printf("%s: hi_mpi_ao_disable_chn(%d) failed with %#x!\n", __FUNCTION__, i, ret);
        return HI_FAILURE;
    }
#endif
    ret = hi_mpi_ao_disable(ao_dev_id);
    if (ret != HI_SUCCESS) {
        printf("%s: hi_mpi_ao_disable failed with %#x!\n", __FUNCTION__, ret);
        return ret;
    }

    return HI_SUCCESS;
}

static hi_void aenc_attr_opus_get_param(const hi_aio_attr *aio_attr, hi_aenc_attr_opus *aenc_opus)
{
    aenc_opus->bit_rate = g_opus_bps;
    aenc_opus->app = g_opus_app;
    aenc_opus->bit_width = HI_AUDIO_BIT_WIDTH_16;
    aenc_opus->sample_rate = aio_attr->sample_rate;
    aenc_opus->snd_mode = aio_attr->snd_mode;
}

static hi_void aenc_attr_mp3_get_param(const hi_aio_attr *aio_attr, hi_aenc_attr_mp3 *aenc_mp3)
{
    aenc_mp3->sample_rate = aio_attr->sample_rate;
    aenc_mp3->bit_width = HI_AUDIO_BIT_WIDTH_16;
    aenc_mp3->sound_mode = aio_attr->snd_mode;
    aenc_mp3->bit_rate = HI_MP3_BPS_128K;
    aenc_mp3->quality = 7; /* 7 : default quality */
}

static hi_void aenc_attr_aac_get_param(const hi_aio_attr *aio_attr, hi_aenc_attr_aac *aenc_aac)
{
    aenc_aac->aac_type = g_aac_type;
    aenc_aac->bit_rate = g_aac_bps;
    aenc_aac->bit_width = HI_AUDIO_BIT_WIDTH_16;
    aenc_aac->sample_rate = aio_attr->sample_rate;
    aenc_aac->snd_mode = aio_attr->snd_mode;
    aenc_aac->transport_type = g_aac_transport_type;
    aenc_aac->band_width = 0;
}

/* start aenc */
hi_s32 sample_comm_audio_start_aenc(hi_u32 aenc_chn_cnt, const hi_aio_attr *aio_attr, hi_payload_type type)
{
    hi_aenc_chn ae_chn;
    hi_s32 ret, i, j;
    hi_aenc_chn_attr aenc_attr;
    hi_aenc_attr_adpcm adpcm_aenc;
    hi_aenc_attr_g711 aenc_g711;
    hi_aenc_attr_g726 aenc_g726;
    hi_aenc_attr_lpcm aenc_lpcm;
    hi_aenc_attr_aac aenc_aac;
    hi_aenc_attr_mp3 aenc_mp3;
    hi_aenc_attr_opus aenc_opus;

    /* set AENC chn attr */
    aenc_attr.type = type;
    aenc_attr.buf_size = 30; /* 30:size */
    aenc_attr.point_num_per_frame = aio_attr->point_num_per_frame;

    if (aenc_attr.type == HI_PT_ADPCMA) {
        aenc_attr.value = &adpcm_aenc;
        adpcm_aenc.adpcm_type = AUDIO_ADPCM_TYPE;
    } else if ((aenc_attr.type == HI_PT_G711A) || (aenc_attr.type == HI_PT_G711U)) {
        aenc_attr.value = &aenc_g711;
    } else if (aenc_attr.type == HI_PT_G726) {
        aenc_attr.value = &aenc_g726;
        aenc_g726.g726bps = G726_BPS;
    } else if (aenc_attr.type == HI_PT_LPCM) {
        aenc_attr.value = &aenc_lpcm;
    } else if (aenc_attr.type == HI_PT_AAC) {
        aenc_attr.value = &aenc_aac;
        aenc_attr_aac_get_param(aio_attr, &aenc_aac);
    } else if (aenc_attr.type == HI_PT_MP3) {
        aenc_attr.value = &aenc_mp3;
        aenc_attr_mp3_get_param(aio_attr, &aenc_mp3);
    } else if (aenc_attr.type == HI_PT_OPUS) {
        aenc_attr.value = &aenc_opus;
        aenc_attr_opus_get_param(aio_attr, &aenc_opus);
    } else {
        printf("%s: invalid aenc payload type:%d\n", __FUNCTION__, aenc_attr.type);
        return HI_FAILURE;
    }

    for (i = 0; i < (hi_s32)aenc_chn_cnt; i++) {
        ae_chn = i;
        /* create aenc chn */
        ret = hi_mpi_aenc_create_chn(ae_chn, &aenc_attr);
        if (ret != HI_SUCCESS) {
            printf("%s: hi_mpi_aenc_create_chn(%d) failed with %#x!\n", __FUNCTION__, ae_chn, ret);
            for (j = 0; j < (hi_s32)ae_chn; j++) {
                (hi_void)hi_mpi_aenc_destroy_chn((hi_aenc_chn)j);
            }
            return ret;
        }
    }

    return HI_SUCCESS;
}

/* stop aenc */
hi_s32 sample_comm_audio_stop_aenc(hi_u32 aenc_chn_cnt)
{
    hi_s32 i;
    hi_s32 ret;

    for (i = 0; i < (hi_s32)aenc_chn_cnt; i++) {
        ret = hi_mpi_aenc_destroy_chn(i);
        if (ret != HI_SUCCESS) {
            printf("%s: hi_mpi_aenc_destroy_chn(%d) failed with %#x!\n", __FUNCTION__, i, ret);
            return ret;
        }
    }

    return HI_SUCCESS;
}

/* destroy the all thread */
hi_s32 sample_comm_audio_destroy_all_thread(void)
{
    hi_u32 dev_id, chn_id;

    for (dev_id = 0; dev_id < HI_AI_DEV_MAX_NUM; dev_id++) {
        for (chn_id = 0; chn_id < HI_AI_MAX_CHN_NUM; chn_id++) {
            if (sample_comm_audio_destroy_thread_ai(dev_id, chn_id) != HI_SUCCESS) {
                printf("%s: sample_comm_audio_destroy_thread_ai(%d,%d) failed!\n", __FUNCTION__, dev_id, chn_id);
                return HI_FAILURE;
            }
        }
    }

    for (chn_id = 0; chn_id < HI_AENC_MAX_CHN_NUM; chn_id++) {
        if (sample_comm_audio_destroy_thread_aenc_adec(chn_id) != HI_SUCCESS) {
            printf("%s: sample_comm_audio_destroy_thread_aenc_adec(%d) failed!\n", __FUNCTION__, chn_id);
            return HI_FAILURE;
        }
    }

    for (chn_id = 0; chn_id < HI_ADEC_MAX_CHN_NUM; chn_id++) {
        if (sample_comm_audio_destroy_thread_file_adec(chn_id) != HI_SUCCESS) {
            printf("%s: sample_comm_audio_destroy_thread_file_adec(%d) failed!\n", __FUNCTION__, chn_id);
            return HI_FAILURE;
        }
    }

    for (chn_id = 0; chn_id < HI_AO_DEV_MAX_NUM; chn_id++) {
        if (sample_comm_audio_destroy_thread_ao_vol_ctrl(chn_id) != HI_SUCCESS) {
            printf("%s: sample_comm_audio_destroy_thread_ao_vol_ctrl(%d) failed!\n", __FUNCTION__, chn_id);
            return HI_FAILURE;
        }
    }

    return HI_SUCCESS;
}

static hi_void adec_attr_opus_get_param(const hi_aio_attr *aio_attr, hi_adec_attr_opus *adec_opus)
{
    adec_opus->sample_rate = aio_attr->sample_rate;
    adec_opus->snd_mode = aio_attr->snd_mode;
}

/* start adec */
hi_s32 sample_comm_audio_start_adec(hi_u32 adec_chn_cnt, const hi_aio_attr *aio_attr, hi_payload_type type)
{
    hi_s32 i, j, ret;
    hi_adec_chn ad_chn;
    hi_adec_chn_attr adec_attr;
    hi_adec_attr_adpcm adpcm;
    hi_adec_attr_g711 adec_g711;
    hi_adec_attr_g726 adec_g726;
    hi_adec_attr_lpcm adec_lpcm;
    hi_adec_attr_aac adec_aac;
    hi_adec_attr_mp3 adec_mp3;
    hi_adec_attr_opus adec_opus;

    adec_attr.type = type;
    adec_attr.buf_size = 20; /* 20: adec buf size */
    adec_attr.mode = HI_ADEC_MODE_STREAM; /* default use stream mode */

    if (adec_attr.type == HI_PT_ADPCMA) {
        adec_attr.value = &adpcm;
        adpcm.adpcm_type = AUDIO_ADPCM_TYPE;
    } else if ((adec_attr.type == HI_PT_G711A) || (adec_attr.type == HI_PT_G711U)) {
        adec_attr.value = &adec_g711;
    } else if (adec_attr.type == HI_PT_G726) {
        adec_attr.value = &adec_g726;
        adec_g726.g726bps = G726_BPS;
    } else if (adec_attr.type == HI_PT_LPCM) {
        adec_attr.value = &adec_lpcm;
        adec_attr.mode = HI_ADEC_MODE_PACK; /* lpcm must use pack mode */
    } else if (adec_attr.type == HI_PT_AAC) {
        adec_attr.value = &adec_aac;
        adec_aac.transport_type = g_aac_transport_type;
    } else if (adec_attr.type == HI_PT_MP3) {
        adec_attr.value = &adec_mp3;
    } else if (adec_attr.type == HI_PT_OPUS) {
        adec_attr.value = &adec_opus;
        adec_attr_opus_get_param(aio_attr, &adec_opus);
    } else {
        printf("%s: invalid aenc payload type:%d\n", __FUNCTION__, adec_attr.type);
        return HI_FAILURE;
    }

    /* create adec chn */
    for (i = 0; i < (hi_s32)adec_chn_cnt; i++) {
        ad_chn = i;
        ret = hi_mpi_adec_create_chn(ad_chn, &adec_attr);
        if (ret != HI_SUCCESS) {
            printf("%s: hi_mpi_adec_create_chn(%d) failed with %#x!\n", __FUNCTION__, ad_chn, ret);
            for (j = 0; j < (hi_s32)ad_chn; j++) {
                (hi_void)hi_mpi_adec_destroy_chn((hi_adec_chn)j);
            }
            return ret;
        }
    }
    return HI_SUCCESS;
}

/* stop adec */
hi_s32 sample_comm_audio_stop_adec(hi_adec_chn ad_chn)
{
    hi_s32 ret;

    ret = hi_mpi_adec_destroy_chn(ad_chn);
    if (ret != HI_SUCCESS) {
        printf("%s: hi_mpi_adec_destroy_chn(%d) failed with %#x!\n", __FUNCTION__, ad_chn, ret);
        return ret;
    }

    return HI_SUCCESS;
}

/* audio system init */
hi_s32 sample_comm_audio_init(hi_void)
{
    hi_s32 ret;

    hi_mpi_audio_exit();

    ret = hi_mpi_audio_init();
    if (ret != HI_SUCCESS) {
        sample_print("hi_mpi_audio_init failed!\n");
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

/* audio system exit */
hi_void sample_comm_audio_exit(hi_void)
{
    hi_mpi_audio_exit();
    return;
}
