

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
#include "acodec.h"
#include "audio_aac_adp.h"
#include "audio_mp3_adp.h"

#ifdef HI_ACODEC_TYPE_TLV320AIC31
#include "tlv320aic31.h"
#endif

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* End of #ifdef __cplusplus */

#define ACODEC_FILE     "/dev/acodec"

#define AUDIO_ADPCM_TYPE ADPCM_TYPE_DVI4/* ADPCM_TYPE_IMA, ADPCM_TYPE_DVI4*/
#define G726_BPS MEDIA_G726_40K         /* MEDIA_G726_16K, MEDIA_G726_24K ... */

static AAC_TYPE_E     gs_enAacType = AAC_TYPE_AACLC;
static AAC_BPS_E     gs_enAacBps  = AAC_BPS_128K;
static AAC_TRANS_TYPE_E gs_enAacTransType = AAC_TRANS_TYPE_ADTS;

typedef struct tagSAMPLE_AENC_S
{
    HI_BOOL bStart;
    pthread_t stAencPid;
    HI_S32  AeChn;
    HI_S32  AdChn;
    FILE*    pfd;
    HI_BOOL bSendAdChn;
} SAMPLE_AENC_S;

typedef struct tagSAMPLE_AI_S
{
    HI_BOOL bStart;
    HI_S32  AiDev;
    HI_S32  AiChn;
    HI_S32  AencChn;
    HI_S32  AoDev;
    HI_S32  AoChn;
    HI_BOOL bSendAenc;
    HI_BOOL bSendAo;
    pthread_t stAiPid;
} SAMPLE_AI_S;

typedef struct tagSAMPLE_ADEC_S
{
    HI_BOOL bStart;
    HI_S32 AdChn;
    FILE* pfd;
    pthread_t stAdPid;
} SAMPLE_ADEC_S;

typedef struct tagSAMPLE_AO_S
{
    AUDIO_DEV AoDev;
    HI_BOOL bStart;
    pthread_t stAoPid;
} SAMPLE_AO_S;

static SAMPLE_AI_S   gs_stSampleAi[AI_DEV_MAX_NUM* AI_MAX_CHN_NUM];
static SAMPLE_AENC_S gs_stSampleAenc[AENC_MAX_CHN_NUM];
static SAMPLE_ADEC_S gs_stSampleAdec[ADEC_MAX_CHN_NUM];
static SAMPLE_AO_S   gs_stSampleAo[AO_DEV_MAX_NUM];

#ifdef HI_ACODEC_TYPE_TLV320AIC31
HI_S32 SAMPLE_Tlv320_CfgAudio(AIO_MODE_E enWorkmode, AUDIO_SAMPLE_RATE_E enSample)
{
    HI_S32 sample;
    HI_S32 vol = 0x100;
    Audio_Ctrl audio_ctrl;
    int s_fdTlv = -1;
    HI_BOOL bPCMmode = HI_FALSE;
    HI_BOOL bMaster = HI_TRUE;
    HI_BOOL bPCMStd = HI_FALSE;


    HI_BOOL b44100HzSeries = HI_FALSE;

    if (AUDIO_SAMPLE_RATE_8000 == enSample)
    {
        sample = AC31_SET_8K_SAMPLERATE;
    }
    else if (AUDIO_SAMPLE_RATE_12000 == enSample)
    {
        sample = AC31_SET_12K_SAMPLERATE;
    }
    else if (AUDIO_SAMPLE_RATE_11025 == enSample)
    {
        b44100HzSeries = HI_TRUE;
        sample = AC31_SET_11_025K_SAMPLERATE;
    }
    else if (AUDIO_SAMPLE_RATE_16000 == enSample)
    {
        sample = AC31_SET_16K_SAMPLERATE;
    }
    else if (AUDIO_SAMPLE_RATE_22050 == enSample)
    {
        b44100HzSeries = HI_TRUE;
        sample = AC31_SET_22_05K_SAMPLERATE;
    }
    else if (AUDIO_SAMPLE_RATE_24000 == enSample)
    {
        sample = AC31_SET_24K_SAMPLERATE;
    }
    else if (AUDIO_SAMPLE_RATE_32000 == enSample)
    {
        sample = AC31_SET_32K_SAMPLERATE;
    }
    else if (AUDIO_SAMPLE_RATE_44100 == enSample)
    {
        b44100HzSeries = HI_TRUE;
        sample = AC31_SET_44_1K_SAMPLERATE;
    }
    else if (AUDIO_SAMPLE_RATE_48000 == enSample)
    {
        sample = AC31_SET_48K_SAMPLERATE;
    }
    else
    {
        printf("SAMPLE_Tlv320_CfgAudio(), not support enSample:%d\n", enSample);
        return -1;
    }

    if (AIO_MODE_I2S_MASTER == enWorkmode)
    {
        bPCMmode = HI_FALSE;
        bMaster = HI_FALSE;
    }
    else if (AIO_MODE_I2S_SLAVE == enWorkmode)
    {
        bPCMmode = HI_FALSE;
        bMaster = HI_TRUE;
    }
    else if ((AIO_MODE_PCM_MASTER_NSTD == enWorkmode) || (AIO_MODE_PCM_MASTER_STD == enWorkmode))
    {
        bPCMmode = HI_TRUE;
        bMaster = HI_FALSE;
    }
    else if ((AIO_MODE_PCM_SLAVE_NSTD == enWorkmode) || (AIO_MODE_PCM_SLAVE_STD == enWorkmode))
    {
        bPCMmode = HI_TRUE;
        bMaster = HI_TRUE;
    }
    else
    {
        printf("SAMPLE_Tlv320_CfgAudio(), not support workmode:%d\n\n", enWorkmode);
    }

    s_fdTlv = open(TLV320_FILE, O_RDWR);
    if (s_fdTlv < 0)
    {
        printf("can't open tlv320,%s\n", TLV320_FILE);
        return -1;
    }

    audio_ctrl.chip_num = 0;
    if (ioctl(s_fdTlv, SOFT_RESET, &audio_ctrl))
    {
        printf("[Func]:%s [Line]:%d [Info]:%s\n", __FUNCTION__, __LINE__, "tlv320aic31 reset failed");
    }


    audio_ctrl.ctrl_mode = bMaster;
    audio_ctrl.if_44100hz_series = b44100HzSeries;
    audio_ctrl.sample = sample;
    audio_ctrl.sampleRate = (HI_U32)enSample;
    ioctl(s_fdTlv, SET_CTRL_MODE, &audio_ctrl);

    /* set transfer mode 0:I2S 1:PCM */
    audio_ctrl.trans_mode = bPCMmode;
    if (ioctl(s_fdTlv, SET_TRANSFER_MODE, &audio_ctrl))
    {
        printf("set tlv320aic31 trans_mode err\n");
        close(s_fdTlv);
        return -1;
    }

    /*set sample of DAC and ADC */
    if (ioctl(s_fdTlv, SET_DAC_SAMPLE, &audio_ctrl))
    {
        printf("ioctl err1\n");
        close(s_fdTlv);
        return -1;
    }

    if (ioctl(s_fdTlv, SET_ADC_SAMPLE, &audio_ctrl))
    {
        printf("ioctl err2\n");
        close(s_fdTlv);
        return -1;
    }

    /*set volume control of left and right DAC */
    audio_ctrl.if_mute_route = 0;
    audio_ctrl.input_level = 0;
    ioctl(s_fdTlv, LEFT_DAC_VOL_CTRL, &audio_ctrl);
    ioctl(s_fdTlv, RIGHT_DAC_VOL_CTRL, &audio_ctrl);

    /*Right/Left DAC Datapath Control */
    audio_ctrl.if_powerup = 1;/*Left/Right DAC datapath plays left/right channel input data*/
    ioctl(s_fdTlv, LEFT_DAC_POWER_SETUP, &audio_ctrl);
    if ((AIO_MODE_I2S_MASTER != enWorkmode) && (AIO_MODE_I2S_SLAVE != enWorkmode))
    {
        audio_ctrl.if_powerup = 0;
    }
    ioctl(s_fdTlv, RIGHT_DAC_POWER_SETUP, &audio_ctrl);

    /* config PCM standard mode and nonstandard mode */
    if ((AIO_MODE_PCM_MASTER_STD == enWorkmode) || (AIO_MODE_PCM_SLAVE_STD == enWorkmode))
    {
        bPCMStd = HI_TRUE;
        audio_ctrl.data_offset = 2;
        ioctl(s_fdTlv, SET_SERIAL_DATA_OFFSET, &audio_ctrl);
    }
    else if ((AIO_MODE_PCM_MASTER_NSTD == enWorkmode) || (AIO_MODE_PCM_SLAVE_NSTD == enWorkmode))
    {
        bPCMStd = HI_FALSE;
        audio_ctrl.data_offset = bPCMStd;
        ioctl(s_fdTlv, SET_SERIAL_DATA_OFFSET, &audio_ctrl);
    }
    else
    {;}

    /* (0:16bit 1:20bit 2:24bit 3:32bit) */
    audio_ctrl.data_length = 0;
    ioctl(s_fdTlv, SET_DATA_LENGTH, &audio_ctrl);

    /*DACL1 TO LEFT_LOP/RIGHT_LOP VOLUME CONTROL 82 92*/
    audio_ctrl.if_mute_route = 1;/* route*/
    audio_ctrl.input_level = vol; /*level control*/
    ioctl(s_fdTlv, DACL1_2_LEFT_LOP_VOL_CTRL, &audio_ctrl);
    ioctl(s_fdTlv, DACR1_2_RIGHT_LOP_VOL_CTRL, &audio_ctrl);

    /* LEFT_LOP/RIGHT_LOP OUTPUT LEVEL CONTROL 86 93*/
    audio_ctrl.if_mute_route = 1;
    audio_ctrl.if_powerup = 1;
    audio_ctrl.input_level = 0;
    ioctl(s_fdTlv, LEFT_LOP_OUTPUT_LEVEL_CTRL, &audio_ctrl);
    ioctl(s_fdTlv, RIGHT_LOP_OUTPUT_LEVEL_CTRL, &audio_ctrl);

    /* LEFT/RIGHT ADC PGA GAIN CONTROL 15 16*/
    audio_ctrl.if_mute_route = 0;
    audio_ctrl.input_level = 0;
    ioctl(s_fdTlv, LEFT_ADC_PGA_CTRL, &audio_ctrl);
    ioctl(s_fdTlv, RIGHT_ADC_PGA_CTRL, &audio_ctrl);

    /*INT2L TO LEFT/RIGTH ADCCONTROL 17 18*/
    audio_ctrl.input_level = 0;
    ioctl(s_fdTlv, IN2LR_2_LEFT_ADC_CTRL, &audio_ctrl);
    ioctl(s_fdTlv, IN2LR_2_RIGTH_ADC_CTRL, &audio_ctrl);

    /*IN1L_2_LEFT/RIGTH_ADC_CTRL 19 22*/
    /*audio_ctrl.input_level = 0xf;
    audio_ctrl.if_powerup = 1;
    printf("audio_ctrl.input_level=0x%x,audio_ctrl.if_powerup=0x%x\n",audio_ctrl.input_level,audio_ctrl.if_powerup);
    if (ioctl(s_fdTlv,IN1L_2_LEFT_ADC_CTRL,&audio_ctrl)==0)
        perror("ioctl err\n");
    getchar();
    printf("audio_ctrl.input_level=0x%x,audio_ctrl.if_powerup=0x%x\n",audio_ctrl.input_level,audio_ctrl.if_powerup);
    ioctl(s_fdTlv,IN1R_2_RIGHT_ADC_CTRL,&audio_ctrl);
    getchar();
    printf("set 19 22\n");*/

    audio_ctrl.if_mute_route = 1;
    audio_ctrl.input_level = 9;
    audio_ctrl.if_powerup = 1;
    ioctl(s_fdTlv, HPLOUT_OUTPUT_LEVEL_CTRL, &audio_ctrl);
    ioctl(s_fdTlv, HPROUT_OUTPUT_LEVEL_CTRL, &audio_ctrl);

    close(s_fdTlv);
    printf("Set aic31 ok: bMaster = %d, enWorkmode = %d, enSamplerate = %d\n",
           bMaster, enWorkmode, enSample);
    return 0;
}


HI_S32 SAMPLE_Tlv320_Disable()
{
    Audio_Ctrl audio_ctrl;
    int s_fdTlv = -1;
    HI_S32 s32Ret;

    s_fdTlv = open(TLV320_FILE, O_RDWR);
    if (s_fdTlv < 0)
    {
        printf("[Func]:%s [Line]:%d [Info]:%s\n", __FUNCTION__, __LINE__, "can't open /dev/tlv320aic31");
        return HI_FAILURE;
    }

    /* reset transfer mode 0:I2S 1:PCM */
    audio_ctrl.chip_num = 0;
    s32Ret = ioctl(s_fdTlv, SOFT_RESET, &audio_ctrl);
    if (HI_SUCCESS != s32Ret)
    {
        printf("[Func]:%s [Line]:%d [Info]:%s\n", __FUNCTION__, __LINE__, "tlv320aic31 reset failed");
    }
    close(s_fdTlv);

    return s32Ret;
}
#endif // end  HI_ACODEC_TYPE_TLV320AIC31


HI_S32 SAMPLE_INNER_CODEC_CfgAudio(AUDIO_SAMPLE_RATE_E enSample)
{
    HI_S32 fdAcodec = -1;
    HI_S32 ret = HI_SUCCESS;
    ACODEC_FS_E i2s_fs_sel = 0;
    int iAcodecInputVol = 0;
    ACODEC_MIXER_E input_mode = 0;

    fdAcodec = open(ACODEC_FILE, O_RDWR);
    if (fdAcodec < 0)
    {
        printf("%s: can't open Acodec,%s\n", __FUNCTION__, ACODEC_FILE);
        return HI_FAILURE;
    }
    if (ioctl(fdAcodec, ACODEC_SOFT_RESET_CTRL))
    {
        printf("Reset audio codec error\n");
    }

    switch (enSample)
    {
        case AUDIO_SAMPLE_RATE_8000:
            i2s_fs_sel = ACODEC_FS_8000;
            break;

        case AUDIO_SAMPLE_RATE_16000:
            i2s_fs_sel = ACODEC_FS_16000;
            break;

        case AUDIO_SAMPLE_RATE_32000:
            i2s_fs_sel = ACODEC_FS_32000;
            break;

        case AUDIO_SAMPLE_RATE_11025:
            i2s_fs_sel = ACODEC_FS_11025;
            break;

        case AUDIO_SAMPLE_RATE_22050:
            i2s_fs_sel = ACODEC_FS_22050;
            break;

        case AUDIO_SAMPLE_RATE_44100:
            i2s_fs_sel = ACODEC_FS_44100;
            break;

        case AUDIO_SAMPLE_RATE_12000:
            i2s_fs_sel = ACODEC_FS_12000;
            break;

        case AUDIO_SAMPLE_RATE_24000:
            i2s_fs_sel = ACODEC_FS_24000;
            break;

        case AUDIO_SAMPLE_RATE_48000:
            i2s_fs_sel = ACODEC_FS_48000;
            break;

        case AUDIO_SAMPLE_RATE_64000:
            i2s_fs_sel = ACODEC_FS_64000;
            break;

        case AUDIO_SAMPLE_RATE_96000:
            i2s_fs_sel = ACODEC_FS_96000;
            break;

        default:
            printf("%s: not support enSample:%d\n", __FUNCTION__, enSample);
            ret = HI_FAILURE;
            break;
    }

    if (ioctl(fdAcodec, ACODEC_SET_I2S1_FS, &i2s_fs_sel))
    {
        printf("%s: set acodec sample rate failed\n", __FUNCTION__);
        ret = HI_FAILURE;
    }

    input_mode = ACODEC_MIXER_IN0;
    if (ioctl(fdAcodec, ACODEC_SET_MIXER_MIC, &input_mode))
    {
        printf("%s: select acodec input_mode failed\n", __FUNCTION__);
        ret = HI_FAILURE;
    }
    if (0) /* should be 1 when micin */
    {
        /******************************************************************************************
        The input volume range is [-87, +86]. Both the analog gain and digital gain are adjusted.
        A larger value indicates higher volume.
        For example, the value 86 indicates the maximum volume of 86 dB,
        and the value -87 indicates the minimum volume (muted status).
        The volume adjustment takes effect simultaneously in the audio-left and audio-right channels.
        The recommended volume range is [+10, +56].
        Within this range, the noises are lowest because only the analog gain is adjusted,
        and the voice quality can be guaranteed.
        *******************************************************************************************/
        iAcodecInputVol = 30;
        if (ioctl(fdAcodec, ACODEC_SET_INPUT_VOL, &iAcodecInputVol))
        {
            printf("%s: set acodec micin volume failed\n", __FUNCTION__);
            return HI_FAILURE;
        }

    }

    close(fdAcodec);
    return ret;
}


/* config codec */
HI_S32 SAMPLE_COMM_AUDIO_CfgAcodec(AIO_ATTR_S* pstAioAttr)
{
    HI_S32 s32Ret = HI_SUCCESS;
    HI_BOOL bCodecCfg = HI_FALSE;

#ifdef HI_ACODEC_TYPE_INNER
    /*** INNER AUDIO CODEC ***/
    s32Ret = SAMPLE_INNER_CODEC_CfgAudio(pstAioAttr->enSamplerate);
    if (HI_SUCCESS != s32Ret)
    {
        printf("%s:SAMPLE_INNER_CODEC_CfgAudio failed\n", __FUNCTION__);
        return s32Ret;
    }
    bCodecCfg = HI_TRUE;
#endif

#ifdef HI_ACODEC_TYPE_TLV320AIC31
    /*** ACODEC_TYPE_TLV320 ***/
    s32Ret = SAMPLE_Tlv320_CfgAudio(pstAioAttr->enWorkmode, pstAioAttr->enSamplerate);
    if (HI_SUCCESS != s32Ret)
    {
        printf("%s: SAMPLE_Tlv320_CfgAudio failed\n", __FUNCTION__);
        return s32Ret;
    }
    bCodecCfg = HI_TRUE;
#endif

    if (!bCodecCfg)
    {
        printf("Can not find the right codec.\n");
        return HI_FALSE;
    }
    return HI_SUCCESS;
}

/******************************************************************************
* function : get frame from Ai, send it  to Aenc or Ao
******************************************************************************/
void* SAMPLE_COMM_AUDIO_AiProc(void* parg)
{
    HI_S32 s32Ret;
    HI_S32 AiFd;
    SAMPLE_AI_S* pstAiCtl = (SAMPLE_AI_S*)parg;
    AUDIO_FRAME_S stFrame;
    AEC_FRAME_S   stAecFrm;
    fd_set read_fds;
    struct timeval TimeoutVal;
    AI_CHN_PARAM_S stAiChnPara;

    s32Ret = HI_MPI_AI_GetChnParam(pstAiCtl->AiDev, pstAiCtl->AiChn, &stAiChnPara);
    if (HI_SUCCESS != s32Ret)
    {
        printf("%s: Get ai chn param failed\n", __FUNCTION__);
        return NULL;
    }

    stAiChnPara.u32UsrFrmDepth = 30;

    s32Ret = HI_MPI_AI_SetChnParam(pstAiCtl->AiDev, pstAiCtl->AiChn, &stAiChnPara);
    if (HI_SUCCESS != s32Ret)
    {
        printf("%s: set ai chn param failed\n", __FUNCTION__);
        return NULL;
    }

    FD_ZERO(&read_fds);
    AiFd = HI_MPI_AI_GetFd(pstAiCtl->AiDev, pstAiCtl->AiChn);
    FD_SET(AiFd, &read_fds);

    while (pstAiCtl->bStart)
    {
        TimeoutVal.tv_sec = 1;
        TimeoutVal.tv_usec = 0;

        FD_ZERO(&read_fds);
        FD_SET(AiFd, &read_fds);

        s32Ret = select(AiFd + 1, &read_fds, NULL, NULL, &TimeoutVal);
        if (s32Ret < 0)
        {
            break;
        }
        else if (0 == s32Ret)
        {
            printf("%s: get ai frame select time out\n", __FUNCTION__);
            break;
        }

        if (FD_ISSET(AiFd, &read_fds))
        {
            /* get frame from ai chn */
            memset(&stAecFrm, 0, sizeof(AEC_FRAME_S));
            s32Ret = HI_MPI_AI_GetFrame(pstAiCtl->AiDev, pstAiCtl->AiChn, &stFrame, &stAecFrm, HI_FALSE);
            if (HI_SUCCESS != s32Ret )
            {
#if 0
                printf("%s: HI_MPI_AI_GetFrame(%d, %d), failed with %#x!\n", \
                       __FUNCTION__, pstAiCtl->AiDev, pstAiCtl->AiChn, s32Ret);
                pstAiCtl->bStart = HI_FALSE;
                return NULL;
#else
                continue;
#endif
            }

            /* send frame to encoder */
            if (HI_TRUE == pstAiCtl->bSendAenc)
            {
                s32Ret = HI_MPI_AENC_SendFrame(pstAiCtl->AencChn, &stFrame, &stAecFrm);
                if (HI_SUCCESS != s32Ret )
                {
                    printf("%s: HI_MPI_AENC_SendFrame(%d), failed with %#x!\n", \
                           __FUNCTION__, pstAiCtl->AencChn, s32Ret);
                    pstAiCtl->bStart = HI_FALSE;
                    return NULL;
                }
            }

            /* send frame to ao */
            if (HI_TRUE == pstAiCtl->bSendAo)
            {
                s32Ret = HI_MPI_AO_SendFrame(pstAiCtl->AoDev, pstAiCtl->AoChn, &stFrame, 1000);
                if (HI_SUCCESS != s32Ret )
                {
                    printf("%s: HI_MPI_AO_SendFrame(%d, %d), failed with %#x!\n", \
                           __FUNCTION__, pstAiCtl->AoDev, pstAiCtl->AoChn, s32Ret);
                    pstAiCtl->bStart = HI_FALSE;
                    return NULL;
                }

            }

            /* finally you must release the stream */
            s32Ret = HI_MPI_AI_ReleaseFrame(pstAiCtl->AiDev, pstAiCtl->AiChn, &stFrame, &stAecFrm);
            if (HI_SUCCESS != s32Ret )
            {
                printf("%s: HI_MPI_AI_ReleaseFrame(%d, %d), failed with %#x!\n", \
                       __FUNCTION__, pstAiCtl->AiDev, pstAiCtl->AiChn, s32Ret);
                pstAiCtl->bStart = HI_FALSE;
                return NULL;
            }

        }
    }

    pstAiCtl->bStart = HI_FALSE;
    return NULL;
}

/******************************************************************************
* function : get stream from Aenc, send it  to Adec & save it to file
******************************************************************************/
void* SAMPLE_COMM_AUDIO_AencProc(void* parg)
{
    HI_S32 s32Ret;
    HI_S32 AencFd;
    SAMPLE_AENC_S* pstAencCtl = (SAMPLE_AENC_S*)parg;
    AUDIO_STREAM_S stStream;
    fd_set read_fds;
    struct timeval TimeoutVal;

    FD_ZERO(&read_fds);
    AencFd = HI_MPI_AENC_GetFd(pstAencCtl->AeChn);
    FD_SET(AencFd, &read_fds);

    while (pstAencCtl->bStart)
    {
        TimeoutVal.tv_sec = 1;
        TimeoutVal.tv_usec = 0;

        FD_ZERO(&read_fds);
        FD_SET(AencFd, &read_fds);

        s32Ret = select(AencFd + 1, &read_fds, NULL, NULL, &TimeoutVal);
        if (s32Ret < 0)
        {
            break;
        }
        else if (0 == s32Ret)
        {
            printf("%s: get aenc stream select time out\n", __FUNCTION__);
            break;
        }

        if (FD_ISSET(AencFd, &read_fds))
        {
            /* get stream from aenc chn */
            s32Ret = HI_MPI_AENC_GetStream(pstAencCtl->AeChn, &stStream, HI_FALSE);
            if (HI_SUCCESS != s32Ret )
            {
                printf("%s: HI_MPI_AENC_GetStream(%d), failed with %#x!\n", \
                       __FUNCTION__, pstAencCtl->AeChn, s32Ret);
                pstAencCtl->bStart = HI_FALSE;
                return NULL;
            }

            /* send stream to decoder and play for testing */
            if (HI_TRUE == pstAencCtl->bSendAdChn)
            {
                s32Ret = HI_MPI_ADEC_SendStream(pstAencCtl->AdChn, &stStream, HI_TRUE);
                if (HI_SUCCESS != s32Ret )
                {
                    printf("%s: HI_MPI_ADEC_SendStream(%d), failed with %#x!\n", \
                           __FUNCTION__, pstAencCtl->AdChn, s32Ret);
                    pstAencCtl->bStart = HI_FALSE;
                    return NULL;
                }
            }

            /* save audio stream to file */
            (HI_VOID)fwrite(stStream.pStream, 1, stStream.u32Len, pstAencCtl->pfd);

            fflush(pstAencCtl->pfd);

            /* finally you must release the stream */
            s32Ret = HI_MPI_AENC_ReleaseStream(pstAencCtl->AeChn, &stStream);
            if (HI_SUCCESS != s32Ret )
            {
                printf("%s: HI_MPI_AENC_ReleaseStream(%d), failed with %#x!\n", \
                       __FUNCTION__, pstAencCtl->AeChn, s32Ret);
                pstAencCtl->bStart = HI_FALSE;
                return NULL;
            }
        }
    }

    fclose(pstAencCtl->pfd);
    pstAencCtl->bStart = HI_FALSE;
    return NULL;
}

/******************************************************************************
* function : get stream from file, and send it  to Adec
******************************************************************************/
void* SAMPLE_COMM_AUDIO_AdecProc(void* parg)
{
    HI_S32 s32Ret;
    AUDIO_STREAM_S stAudioStream;
    HI_U32 u32Len = 640;
    HI_U32 u32ReadLen;
    HI_S32 s32AdecChn;
    HI_U8* pu8AudioStream = NULL;
    SAMPLE_ADEC_S* pstAdecCtl = (SAMPLE_ADEC_S*)parg;
    FILE* pfd = pstAdecCtl->pfd;
    s32AdecChn = pstAdecCtl->AdChn;

    pu8AudioStream = (HI_U8*)malloc(sizeof(HI_U8) * MAX_AUDIO_STREAM_LEN);
    if (NULL == pu8AudioStream)
    {
        printf("%s: malloc failed!\n", __FUNCTION__);
        return NULL;
    }

    while (HI_TRUE == pstAdecCtl->bStart)
    {
        /* read from file */
        stAudioStream.pStream = pu8AudioStream;
        u32ReadLen = fread(stAudioStream.pStream, 1, u32Len, pfd);
        if (u32ReadLen <= 0)
        {
            s32Ret = HI_MPI_ADEC_SendEndOfStream(s32AdecChn, HI_FALSE);
            if (HI_SUCCESS != s32Ret)
            {
                printf("%s: HI_MPI_ADEC_SendEndOfStream failed!\n", __FUNCTION__);
            }
            (HI_VOID)fseek(pfd, 0, SEEK_SET);/*read file again*/
            continue;
        }

        /* here only demo adec streaming sending mode, but pack sending mode is commended */
        stAudioStream.u32Len = u32ReadLen;
        s32Ret = HI_MPI_ADEC_SendStream(s32AdecChn, &stAudioStream, HI_TRUE);
        if (HI_SUCCESS != s32Ret)
        {
            printf("%s: HI_MPI_ADEC_SendStream(%d) failed with %#x!\n", \
                   __FUNCTION__, s32AdecChn, s32Ret);
            break;
        }
    }

    free(pu8AudioStream);
    pu8AudioStream = NULL;
    fclose(pfd);
    pstAdecCtl->bStart = HI_FALSE;
    return NULL;
}

/******************************************************************************
* function : set ao volume
******************************************************************************/
void* SAMPLE_COMM_AUDIO_AoVolProc(void* parg)
{
    HI_S32 s32Ret;
    HI_S32 s32Volume;
    AUDIO_DEV AoDev;
    AUDIO_FADE_S stFade;
    SAMPLE_AO_S* pstAoCtl = (SAMPLE_AO_S*)parg;
    AoDev = pstAoCtl->AoDev;

    while (pstAoCtl->bStart)
    {
        for (s32Volume = 0; s32Volume <= 6; s32Volume++)
        {
            s32Ret = HI_MPI_AO_SetVolume( AoDev, s32Volume);
            if (HI_SUCCESS != s32Ret)
            {
                printf("%s: HI_MPI_AO_SetVolume(%d), failed with %#x!\n", \
                       __FUNCTION__, AoDev, s32Ret);
            }
            printf("\rset volume %d          ", s32Volume);
            fflush(stdout);
            sleep(2);
        }

        for (s32Volume = 5; s32Volume >= -15; s32Volume--)
        {
            s32Ret = HI_MPI_AO_SetVolume( AoDev, s32Volume);
            if (HI_SUCCESS != s32Ret)
            {
                printf("%s: HI_MPI_AO_SetVolume(%d), failed with %#x!\n", \
                       __FUNCTION__, AoDev, s32Ret);
            }
            printf("\rset volume %d          ", s32Volume);
            fflush(stdout);
            sleep(2);
        }

        for (s32Volume = -14; s32Volume <= 0; s32Volume++)
        {
            s32Ret = HI_MPI_AO_SetVolume( AoDev, s32Volume);
            if (HI_SUCCESS != s32Ret)
            {
                printf("%s: HI_MPI_AO_SetVolume(%d), failed with %#x!\n", \
                       __FUNCTION__, AoDev, s32Ret);
            }
            printf("\rset volume %d          ", s32Volume);
            fflush(stdout);
            sleep(2);
        }

        stFade.bFade         = HI_TRUE;
        stFade.enFadeInRate  = AUDIO_FADE_RATE_128;
        stFade.enFadeOutRate = AUDIO_FADE_RATE_128;

        s32Ret = HI_MPI_AO_SetMute(AoDev, HI_TRUE, &stFade);
        if (HI_SUCCESS != s32Ret)
        {
            printf("%s: HI_MPI_AO_SetVolume(%d), failed with %#x!\n", \
                   __FUNCTION__, AoDev, s32Ret);
        }
        printf("\rset Ao mute            ");
        fflush(stdout);
        sleep(2);

        s32Ret = HI_MPI_AO_SetMute(AoDev, HI_FALSE, NULL);
        if (HI_SUCCESS != s32Ret)
        {
            printf("%s: HI_MPI_AO_SetVolume(%d), failed with %#x!\n", \
                   __FUNCTION__, AoDev, s32Ret);
        }
        printf("\rset Ao unmute          ");
        fflush(stdout);
        sleep(2);
    }
    return NULL;
}

/******************************************************************************
* function : Create the thread to get frame from ai and send to ao
******************************************************************************/
HI_S32 SAMPLE_COMM_AUDIO_CreatTrdAiAo(AUDIO_DEV AiDev, AI_CHN AiChn, AUDIO_DEV AoDev, AO_CHN AoChn)
{
    SAMPLE_AI_S* pstAi = NULL;

    pstAi = &gs_stSampleAi[AiDev * AI_MAX_CHN_NUM + AiChn];
    pstAi->bSendAenc = HI_FALSE;
    pstAi->bSendAo = HI_TRUE;
    pstAi->bStart = HI_TRUE;
    pstAi->AiDev = AiDev;
    pstAi->AiChn = AiChn;
    pstAi->AoDev = AoDev;
    pstAi->AoChn = AoChn;

    pthread_create(&pstAi->stAiPid, 0, SAMPLE_COMM_AUDIO_AiProc, pstAi);

    return HI_SUCCESS;
}

/******************************************************************************
* function : Create the thread to get frame from ai and send to aenc
******************************************************************************/
HI_S32 SAMPLE_COMM_AUDIO_CreatTrdAiAenc(AUDIO_DEV AiDev, AI_CHN AiChn, AENC_CHN AeChn)
{
    SAMPLE_AI_S* pstAi = NULL;

    pstAi = &gs_stSampleAi[AiDev * AI_MAX_CHN_NUM + AiChn];
    pstAi->bSendAenc = HI_TRUE;
    pstAi->bSendAo = HI_FALSE;
    pstAi->bStart = HI_TRUE;
    pstAi->AiDev = AiDev;
    pstAi->AiChn = AiChn;
    pstAi->AencChn = AeChn;
    pthread_create(&pstAi->stAiPid, 0, SAMPLE_COMM_AUDIO_AiProc, pstAi);

    return HI_SUCCESS;
}

/******************************************************************************
* function : Create the thread to get stream from aenc and send to adec
******************************************************************************/
HI_S32 SAMPLE_COMM_AUDIO_CreatTrdAencAdec(AENC_CHN AeChn, ADEC_CHN AdChn, FILE* pAecFd)
{
    SAMPLE_AENC_S* pstAenc = NULL;

    if (NULL == pAecFd)
    {
        return HI_FAILURE;
    }

    pstAenc = &gs_stSampleAenc[AeChn];
    pstAenc->AeChn = AeChn;
    pstAenc->AdChn = AdChn;
    pstAenc->bSendAdChn = HI_TRUE;
    pstAenc->pfd = pAecFd;
    pstAenc->bStart = HI_TRUE;
    pthread_create(&pstAenc->stAencPid, 0, SAMPLE_COMM_AUDIO_AencProc, pstAenc);

    return HI_SUCCESS;
}

/******************************************************************************
* function : Create the thread to get stream from file and send to adec
******************************************************************************/
HI_S32 SAMPLE_COMM_AUDIO_CreatTrdFileAdec(ADEC_CHN AdChn, FILE* pAdcFd)
{
    SAMPLE_ADEC_S* pstAdec = NULL;

    if (NULL == pAdcFd)
    {
        return HI_FAILURE;
    }

    pstAdec = &gs_stSampleAdec[AdChn];
    pstAdec->AdChn = AdChn;
    pstAdec->pfd = pAdcFd;
    pstAdec->bStart = HI_TRUE;
    pthread_create(&pstAdec->stAdPid, 0, SAMPLE_COMM_AUDIO_AdecProc, pstAdec);

    return HI_SUCCESS;
}


/******************************************************************************
* function : Create the thread to set Ao volume
******************************************************************************/
HI_S32 SAMPLE_COMM_AUDIO_CreatTrdAoVolCtrl(AUDIO_DEV AoDev)
{
    SAMPLE_AO_S* pstAoCtl = NULL;

    pstAoCtl =  &gs_stSampleAo[AoDev];
    pstAoCtl->AoDev =  AoDev;
    pstAoCtl->bStart = HI_TRUE;
    pthread_create(&pstAoCtl->stAoPid, 0, SAMPLE_COMM_AUDIO_AoVolProc, pstAoCtl);

    return HI_SUCCESS;
}


/******************************************************************************
* function : Destory the thread to get frame from ai and send to ao or aenc
******************************************************************************/
HI_S32 SAMPLE_COMM_AUDIO_DestoryTrdAi(AUDIO_DEV AiDev, AI_CHN AiChn)
{
    SAMPLE_AI_S* pstAi = NULL;

    pstAi = &gs_stSampleAi[AiDev * AI_MAX_CHN_NUM + AiChn];
    if (pstAi->bStart)
    {
        pstAi->bStart = HI_FALSE;
        //pthread_cancel(pstAi->stAiPid);
        pthread_join(pstAi->stAiPid, 0);
    }


    return HI_SUCCESS;
}

/******************************************************************************
* function : Destory the thread to get stream from aenc and send to adec
******************************************************************************/
HI_S32 SAMPLE_COMM_AUDIO_DestoryTrdAencAdec(AENC_CHN AeChn)
{
    SAMPLE_AENC_S* pstAenc = NULL;

    pstAenc = &gs_stSampleAenc[AeChn];
    if (pstAenc->bStart)
    {
        pstAenc->bStart = HI_FALSE;
        //pthread_cancel(pstAenc->stAencPid);
        pthread_join(pstAenc->stAencPid, 0);
    }


    return HI_SUCCESS;
}

/******************************************************************************
* function : Destory the thread to get stream from file and send to adec
******************************************************************************/
HI_S32 SAMPLE_COMM_AUDIO_DestoryTrdFileAdec(ADEC_CHN AdChn)
{
    SAMPLE_ADEC_S* pstAdec = NULL;

    pstAdec = &gs_stSampleAdec[AdChn];
    if (pstAdec->bStart)
    {
        pstAdec->bStart = HI_FALSE;
        //pthread_cancel(pstAdec->stAdPid);
        pthread_join(pstAdec->stAdPid, 0);
    }


    return HI_SUCCESS;
}

/******************************************************************************
* function : Destory the thread to set Ao volume
******************************************************************************/
HI_S32 SAMPLE_COMM_AUDIO_DestoryTrdAoVolCtrl(AUDIO_DEV AoDev)
{
    SAMPLE_AO_S* pstAoCtl = NULL;

    pstAoCtl =  &gs_stSampleAo[AoDev];
    if (pstAoCtl->bStart)
    {
        pstAoCtl->bStart = HI_FALSE;
        pthread_cancel(pstAoCtl->stAoPid);
        pthread_join(pstAoCtl->stAoPid, 0);
    }


    return HI_SUCCESS;
}

/******************************************************************************
* function : Ao bind Adec
******************************************************************************/
HI_S32 SAMPLE_COMM_AUDIO_AoBindAdec(AUDIO_DEV AoDev, AO_CHN AoChn, ADEC_CHN AdChn)
{
    MPP_CHN_S stSrcChn, stDestChn;

    stSrcChn.enModId = HI_ID_ADEC;
    stSrcChn.s32DevId = 0;
    stSrcChn.s32ChnId = AdChn;
    stDestChn.enModId = HI_ID_AO;
    stDestChn.s32DevId = AoDev;
    stDestChn.s32ChnId = AoChn;

    return HI_MPI_SYS_Bind(&stSrcChn, &stDestChn);
}

/******************************************************************************
* function : Ao unbind Adec
******************************************************************************/
HI_S32 SAMPLE_COMM_AUDIO_AoUnbindAdec(AUDIO_DEV AoDev, AO_CHN AoChn, ADEC_CHN AdChn)
{
    MPP_CHN_S stSrcChn, stDestChn;

    stSrcChn.enModId = HI_ID_ADEC;
    stSrcChn.s32ChnId = AdChn;
    stSrcChn.s32DevId = 0;
    stDestChn.enModId = HI_ID_AO;
    stDestChn.s32DevId = AoDev;
    stDestChn.s32ChnId = AoChn;

    return HI_MPI_SYS_UnBind(&stSrcChn, &stDestChn);
}

/******************************************************************************
* function : Ao bind Ai
******************************************************************************/
HI_S32 SAMPLE_COMM_AUDIO_AoBindAi(AUDIO_DEV AiDev, AI_CHN AiChn, AUDIO_DEV AoDev, AO_CHN AoChn)
{
    MPP_CHN_S stSrcChn, stDestChn;

    stSrcChn.enModId = HI_ID_AI;
    stSrcChn.s32ChnId = AiChn;
    stSrcChn.s32DevId = AiDev;
    stDestChn.enModId = HI_ID_AO;
    stDestChn.s32DevId = AoDev;
    stDestChn.s32ChnId = AoChn;

    return HI_MPI_SYS_Bind(&stSrcChn, &stDestChn);
}

/******************************************************************************
* function : Ao unbind Ai
******************************************************************************/
HI_S32 SAMPLE_COMM_AUDIO_AoUnbindAi(AUDIO_DEV AiDev, AI_CHN AiChn, AUDIO_DEV AoDev, AO_CHN AoChn)
{
    MPP_CHN_S stSrcChn, stDestChn;

    stSrcChn.enModId = HI_ID_AI;
    stSrcChn.s32ChnId = AiChn;
    stSrcChn.s32DevId = AiDev;
    stDestChn.enModId = HI_ID_AO;
    stDestChn.s32DevId = AoDev;
    stDestChn.s32ChnId = AoChn;

    return HI_MPI_SYS_UnBind(&stSrcChn, &stDestChn);
}

/******************************************************************************
* function : Aenc bind Ai
******************************************************************************/
HI_S32 SAMPLE_COMM_AUDIO_AencBindAi(AUDIO_DEV AiDev, AI_CHN AiChn, AENC_CHN AeChn)
{
    MPP_CHN_S stSrcChn, stDestChn;

    stSrcChn.enModId = HI_ID_AI;
    stSrcChn.s32DevId = AiDev;
    stSrcChn.s32ChnId = AiChn;
    stDestChn.enModId = HI_ID_AENC;
    stDestChn.s32DevId = 0;
    stDestChn.s32ChnId = AeChn;

    return HI_MPI_SYS_Bind(&stSrcChn, &stDestChn);
}

/******************************************************************************
* function : Aenc unbind Ai
******************************************************************************/
HI_S32 SAMPLE_COMM_AUDIO_AencUnbindAi(AUDIO_DEV AiDev, AI_CHN AiChn, AENC_CHN AeChn)
{
    MPP_CHN_S stSrcChn, stDestChn;

    stSrcChn.enModId = HI_ID_AI;
    stSrcChn.s32DevId = AiDev;
    stSrcChn.s32ChnId = AiChn;
    stDestChn.enModId = HI_ID_AENC;
    stDestChn.s32DevId = 0;
    stDestChn.s32ChnId = AeChn;

    return HI_MPI_SYS_UnBind(&stSrcChn, &stDestChn);
}

/******************************************************************************
* function : Start Ai
******************************************************************************/
HI_S32 SAMPLE_COMM_AUDIO_StartAi(AUDIO_DEV AiDevId, HI_S32 s32AiChnCnt,
                                 AIO_ATTR_S* pstAioAttr, AUDIO_SAMPLE_RATE_E enOutSampleRate,
                                 HI_BOOL bResampleEn, HI_VOID* pstAiVqeAttr, HI_U32 u32AiVqeType)
{
    HI_S32 i;
    HI_S32 s32Ret;

    s32Ret = HI_MPI_AI_SetPubAttr(AiDevId, pstAioAttr);
    if (s32Ret)
    {
        printf("%s: HI_MPI_AI_SetPubAttr(%d) failed with %#x\n", __FUNCTION__, AiDevId, s32Ret);
        return s32Ret;
    }

    s32Ret = HI_MPI_AI_Enable(AiDevId);
    if (s32Ret)
    {
        printf("%s: HI_MPI_AI_Enable(%d) failed with %#x\n", __FUNCTION__, AiDevId, s32Ret);
        return s32Ret;
    }

    for (i = 0; i < s32AiChnCnt>>pstAioAttr->enSoundmode; i++)
    {
        s32Ret = HI_MPI_AI_EnableChn(AiDevId, i);
        if (s32Ret)
        {
            printf("%s: HI_MPI_AI_EnableChn(%d,%d) failed with %#x\n", __FUNCTION__, AiDevId, i, s32Ret);
            return s32Ret;
        }

        if (HI_TRUE == bResampleEn)
        {
            s32Ret = HI_MPI_AI_EnableReSmp(AiDevId, i, enOutSampleRate);
            if (s32Ret)
            {
                printf("%s: HI_MPI_AI_EnableReSmp(%d,%d) failed with %#x\n", __FUNCTION__, AiDevId, i, s32Ret);
                return s32Ret;
            }
        }

        if (NULL != pstAiVqeAttr)
        {
            HI_BOOL bAiVqe = HI_TRUE;
            switch (u32AiVqeType)
            {
                case 0:
                    s32Ret = HI_SUCCESS;
                    bAiVqe = HI_FALSE;
                    break;
                case 1:
                    s32Ret = HI_MPI_AI_SetRecordVqeAttr(AiDevId, i, (AI_RECORDVQE_CONFIG_S *)pstAiVqeAttr);
                    break;
                case 2:
                    s32Ret = HI_MPI_AI_SetTalkVqeAttr(AiDevId, i, 1, 0, (AI_TALKVQE_CONFIG_S *)pstAiVqeAttr);
                    break;
                default:
                    s32Ret = HI_FAILURE;
                    break;
            }

            if (s32Ret)
            {
                printf("%s: SetAiVqe%d(%d,%d) failed with %#x\n", __FUNCTION__, u32AiVqeType, AiDevId, i, s32Ret);
                return s32Ret;
            }

            if (bAiVqe)
            {
                s32Ret = HI_MPI_AI_EnableVqe(AiDevId, i);
                if (s32Ret)
                {
                    printf("%s: HI_MPI_AI_EnableVqe(%d,%d) failed with %#x\n", __FUNCTION__, AiDevId, i, s32Ret);
                    return s32Ret;
                }
            }
        }
    }

    return HI_SUCCESS;
}

/******************************************************************************
* function : Stop Ai
******************************************************************************/
HI_S32 SAMPLE_COMM_AUDIO_StopAi(AUDIO_DEV AiDevId, HI_S32 s32AiChnCnt,
                                HI_BOOL bResampleEn, HI_BOOL bVqeEn)
{
    HI_S32 i;
    HI_S32 s32Ret;

    for (i = 0; i < s32AiChnCnt; i++)
    {
        if (HI_TRUE == bResampleEn)
        {
            s32Ret = HI_MPI_AI_DisableReSmp(AiDevId, i);
            if (HI_SUCCESS != s32Ret)
            {
                printf("[Func]:%s [Line]:%d [Info]:%s\n", __FUNCTION__, __LINE__, "failed");
                return s32Ret;
            }
        }

        if (HI_TRUE == bVqeEn)
        {
            s32Ret = HI_MPI_AI_DisableVqe(AiDevId, i);
            if (HI_SUCCESS != s32Ret)
            {
                printf("[Func]:%s [Line]:%d [Info]:%s\n", __FUNCTION__, __LINE__, "failed");
                return s32Ret;
            }
        }

        s32Ret = HI_MPI_AI_DisableChn(AiDevId, i);
        if (HI_SUCCESS != s32Ret)
        {
            printf("[Func]:%s [Line]:%d [Info]:%s\n", __FUNCTION__, __LINE__, "failed");
            return s32Ret;
        }
    }

    s32Ret = HI_MPI_AI_Disable(AiDevId);
    if (HI_SUCCESS != s32Ret)
    {
        printf("[Func]:%s [Line]:%d [Info]:%s\n", __FUNCTION__, __LINE__, "failed");
        return s32Ret;
    }

    return HI_SUCCESS;
}

#ifdef HI_ACODEC_TYPE_HDMI
HI_S32 SAMPLE_COMM_AUDIO_StartHdmi(AIO_ATTR_S *pstAioAttr)
{
    HI_S32 s32Ret;
    HI_HDMI_ATTR_S stHdmiAttr;
    HI_HDMI_ID_E enHdmi = HI_HDMI_ID_0;
    VO_PUB_ATTR_S stPubAttr;
    VO_DEV VoDev = 0;

    stPubAttr.u32BgColor = 0x000000ff;
    stPubAttr.enIntfType = VO_INTF_HDMI;
    stPubAttr.enIntfSync = VO_OUTPUT_1080P30;

    if(HI_SUCCESS != SAMPLE_COMM_VO_StartDev(VoDev, &stPubAttr))
    {
        printf("[Func]:%s [Line]:%d [Info]:%s\n", __FUNCTION__, __LINE__, "failed");
        return HI_FAILURE;
    }

    s32Ret = SAMPLE_COMM_VO_HdmiStart(stPubAttr.enIntfSync);
    if(HI_SUCCESS != s32Ret)
    {
        printf("[Func]:%s [Line]:%d [Info]:%s\n", __FUNCTION__, __LINE__, "failed");
        return HI_FAILURE;
    }

    s32Ret = HI_MPI_HDMI_Stop(enHdmi);
    if(HI_SUCCESS != s32Ret)
    {
        printf("[Func]:%s [Line]:%d [Info]:%s\n", __FUNCTION__, __LINE__, "failed");
        return HI_FAILURE;
    }

    s32Ret = HI_MPI_HDMI_GetAttr(enHdmi, &stHdmiAttr);
    if(HI_SUCCESS != s32Ret)
    {
        printf("[Func]:%s [Line]:%d [Info]:%s\n", __FUNCTION__, __LINE__, "failed");
        return HI_FAILURE;
    }

    stHdmiAttr.bEnableAudio = HI_TRUE;        /**< if enable audio */
    stHdmiAttr.enVidInMode  = HI_HDMI_VIDEO_MODE_YCBCR444;
    stHdmiAttr.enVidOutMode = HI_HDMI_VIDEO_MODE_YCBCR444;
    stHdmiAttr.enSoundIntf  = HI_HDMI_SND_INTERFACE_I2S; /**< source of HDMI audio, HI_HDMI_SND_INTERFACE_I2S suggested.the parameter must be consistent with the input of AO*/
    stHdmiAttr.enSampleRate = pstAioAttr->enSamplerate;        /**< sampling rate of PCM audio,the parameter must be consistent with the input of AO */
    stHdmiAttr.u8DownSampleParm = HI_FALSE;    /**< parameter of downsampling  rate of PCM audio, default :0 */

    stHdmiAttr.enBitDepth = 8 * (pstAioAttr->enBitwidth+1);   /**< bitwidth of audio,default :16,the parameter must be consistent with the config of AO */
    stHdmiAttr.u8I2SCtlVbit = 0;        /**< reserved, should be 0, I2S control (0x7A:0x1D) */

    stHdmiAttr.bEnableAviInfoFrame = HI_TRUE; /**< if enable  AVI InfoFrame*/
    stHdmiAttr.bEnableAudInfoFrame = HI_TRUE;; /**< if enable AUDIO InfoFrame*/

    s32Ret = HI_MPI_HDMI_SetAttr(enHdmi, &stHdmiAttr);
    if(HI_SUCCESS != s32Ret)
    {
        printf("[Func]:%s [Line]:%d [Info]:%s\n", __FUNCTION__, __LINE__, "failed");
        return HI_FAILURE;
    }

    s32Ret = HI_MPI_HDMI_Start(enHdmi);
    if(HI_SUCCESS != s32Ret)
    {
        printf("[Func]:%s [Line]:%d [Info]:%s\n", __FUNCTION__, __LINE__, "failed");
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

HI_S32 SAMPLE_COMM_AUDIO_StopHdmi(HI_VOID)
{
    HI_S32 s32Ret;
    VO_DEV VoDev = 0;

    s32Ret =  SAMPLE_COMM_VO_HdmiStop();
    s32Ret |= HI_MPI_VO_Disable(VoDev);
    if(HI_SUCCESS != s32Ret)
    {
        printf("%s: HI_MPI_VO_Disable failed with %#x!\n", __FUNCTION__, s32Ret);
        return HI_FAILURE;
    }

    return s32Ret;
}
#endif

/******************************************************************************
* function : Start Ao with VQE
******************************************************************************/
HI_S32 SAMPLE_COMM_AUDIO_StartAoWithVQE(AUDIO_DEV AoDevId, HI_S32 s32AoChnCnt,
                                 AIO_ATTR_S* pstAioAttr, AUDIO_SAMPLE_RATE_E enInSampleRate,
                                 HI_BOOL bResampleEn, AO_VQE_CONFIG_S* pstAoVqeAttr, HI_U32 u32AiVqeType)
{
    HI_S32 i;
    HI_S32 s32Ret;

    if (SAMPLE_AUDIO_INNER_HDMI_AO_DEV == AoDevId)
    {
#ifdef HI_ACODEC_TYPE_HDMI
        pstAioAttr->u32ClkSel = 0;

        SAMPLE_COMM_AUDIO_StartHdmi(pstAioAttr);
#endif
    }

    s32Ret = HI_MPI_AO_SetPubAttr(AoDevId, pstAioAttr);
    if (HI_SUCCESS != s32Ret)
    {
        printf("%s: HI_MPI_AO_SetPubAttr(%d) failed with %#x!\n", __FUNCTION__, \
               AoDevId, s32Ret);
        return HI_FAILURE;
    }

    s32Ret = HI_MPI_AO_Enable(AoDevId);
    if (HI_SUCCESS != s32Ret)
    {
        printf("%s: HI_MPI_AO_Enable(%d) failed with %#x!\n", __FUNCTION__, AoDevId, s32Ret);
        return HI_FAILURE;
    }

    for (i = 0; i < s32AoChnCnt; i++)
    {
        s32Ret = HI_MPI_AO_EnableChn(AoDevId, i/(pstAioAttr->enSoundmode + 1));
        if (HI_SUCCESS != s32Ret)
        {
            printf("%s: HI_MPI_AO_EnableChn(%d) failed with %#x!\n", __FUNCTION__, i, s32Ret);
            return HI_FAILURE;
        }

        if (HI_TRUE == bResampleEn)
        {
            s32Ret = HI_MPI_AO_DisableReSmp(AoDevId, i);
            s32Ret |= HI_MPI_AO_EnableReSmp(AoDevId, i, enInSampleRate);
            if (HI_SUCCESS != s32Ret)
            {
                printf("%s: HI_MPI_AO_EnableReSmp(%d,%d) failed with %#x!\n", __FUNCTION__, AoDevId, i, s32Ret);
                return HI_FAILURE;
            }
        }

        if (2 == u32AiVqeType && HI_NULL != pstAoVqeAttr)
        {
            s32Ret = HI_MPI_AO_SetVqeAttr(AoDevId, i, pstAoVqeAttr);
            if (HI_SUCCESS != s32Ret)
            {
                printf("%s: HI_MPI_AO_SetVqeAttr(%d,%d) failed with %#x!\n", __FUNCTION__, AoDevId, i, s32Ret);
                return HI_FAILURE;
            }
            s32Ret = HI_MPI_AO_EnableVqe(AoDevId, i);
            if (HI_SUCCESS != s32Ret)
            {
                printf("%s: HI_MPI_AO_EnableVqe(%d,%d) failed with %#x!\n", __FUNCTION__, AoDevId, i, s32Ret);
                return HI_FAILURE;
            }
        }
    }

    s32Ret = HI_MPI_AO_EnableChn(AoDevId, AO_SYSCHN_CHNID);
    if (HI_SUCCESS != s32Ret)
    {
        printf("%s: HI_MPI_AO_EnableChn(%d) failed with %#x!\n", __FUNCTION__, i, s32Ret);
        return HI_FAILURE;
    }

#if 0
    if (HI_TRUE == bResampleEn)
    {
        s32Ret = HI_MPI_AO_DisableReSmp(AoDevId, AO_SYSCHN_CHNID);
        s32Ret |= HI_MPI_AO_EnableReSmp(AoDevId, AO_SYSCHN_CHNID, enInSampleRate);
        if (HI_SUCCESS != s32Ret)
        {
            printf("%s: HI_MPI_AO_EnableReSmp(%d,%d) failed with %#x!\n", __FUNCTION__, AoDevId, AO_SYSCHN_CHNID, s32Ret);
            return HI_FAILURE;
        }
    }
#endif

    return HI_SUCCESS;
}

/******************************************************************************
* function : Start Ao
******************************************************************************/
HI_S32 SAMPLE_COMM_AUDIO_StartAo(AUDIO_DEV AoDevId, HI_S32 s32AoChnCnt,
                                 AIO_ATTR_S* pstAioAttr, AUDIO_SAMPLE_RATE_E enInSampleRate, HI_BOOL bResampleEn)
{
    HI_S32 i;
    HI_S32 s32Ret;

    if (SAMPLE_AUDIO_INNER_HDMI_AO_DEV == AoDevId)
    {
#ifdef HI_ACODEC_TYPE_HDMI
        pstAioAttr->u32ClkSel = 0;

        SAMPLE_COMM_AUDIO_StartHdmi(pstAioAttr);
#endif
    }

    s32Ret = HI_MPI_AO_SetPubAttr(AoDevId, pstAioAttr);
    if (HI_SUCCESS != s32Ret)
    {
        printf("%s: HI_MPI_AO_SetPubAttr(%d) failed with %#x!\n", __FUNCTION__, \
               AoDevId, s32Ret);
        return HI_FAILURE;
    }

    s32Ret = HI_MPI_AO_Enable(AoDevId);
    if (HI_SUCCESS != s32Ret)
    {
        printf("%s: HI_MPI_AO_Enable(%d) failed with %#x!\n", __FUNCTION__, AoDevId, s32Ret);
        return HI_FAILURE;
    }

    for (i = 0; i < s32AoChnCnt>>pstAioAttr->enSoundmode; i++)
    {
        s32Ret = HI_MPI_AO_EnableChn(AoDevId, i);
        if (HI_SUCCESS != s32Ret)
        {
            printf("%s: HI_MPI_AO_EnableChn(%d) failed with %#x!\n", __FUNCTION__, i, s32Ret);
            return HI_FAILURE;
        }

        if (HI_TRUE == bResampleEn)
        {
            s32Ret = HI_MPI_AO_DisableReSmp(AoDevId, i);
            s32Ret |= HI_MPI_AO_EnableReSmp(AoDevId, i, enInSampleRate);
            if (HI_SUCCESS != s32Ret)
            {
                printf("%s: HI_MPI_AO_EnableReSmp(%d,%d) failed with %#x!\n", __FUNCTION__, AoDevId, i, s32Ret);
                return HI_FAILURE;
            }
        }

    }

    s32Ret = HI_MPI_AO_EnableChn(AoDevId, AO_SYSCHN_CHNID);
    if (HI_SUCCESS != s32Ret)
    {
        printf("%s: HI_MPI_AO_EnableChn(%d) failed with %#x!\n", __FUNCTION__, i, s32Ret);
        return HI_FAILURE;
    }

#if 0
    if (HI_TRUE == bResampleEn)
    {
        s32Ret = HI_MPI_AO_DisableReSmp(AoDevId, AO_SYSCHN_CHNID);
        s32Ret |= HI_MPI_AO_EnableReSmp(AoDevId, AO_SYSCHN_CHNID, enInSampleRate);
        if (HI_SUCCESS != s32Ret)
        {
            printf("%s: HI_MPI_AO_EnableReSmp(%d,%d) failed with %#x!\n", __FUNCTION__, AoDevId, AO_SYSCHN_CHNID, s32Ret);
            return HI_FAILURE;
        }
    }
#endif

    return HI_SUCCESS;
}

/******************************************************************************
* function : Stop Ao
******************************************************************************/
HI_S32 SAMPLE_COMM_AUDIO_StopAo(AUDIO_DEV AoDevId, HI_S32 s32AoChnCnt, HI_BOOL bResampleEn)
{
    HI_S32 i;
    HI_S32 s32Ret;

    for (i = 0; i < s32AoChnCnt; i++)
    {
        if (HI_TRUE == bResampleEn)
        {
            s32Ret = HI_MPI_AO_DisableReSmp(AoDevId, i);
            if (HI_SUCCESS != s32Ret)
            {
                printf("%s: HI_MPI_AO_DisableReSmp failed with %#x!\n", __FUNCTION__, s32Ret);
                return s32Ret;
            }
        }

        s32Ret = HI_MPI_AO_DisableChn(AoDevId, i);
        if (HI_SUCCESS != s32Ret)
        {
            printf("%s: HI_MPI_AO_DisableChn failed with %#x!\n", __FUNCTION__, s32Ret);
            return s32Ret;
        }
    }

    s32Ret = HI_MPI_AO_DisableChn(AoDevId, AO_SYSCHN_CHNID);
    if (HI_SUCCESS != s32Ret)
    {
        printf("%s: HI_MPI_AO_DisableChn(%d) failed with %#x!\n", __FUNCTION__, i, s32Ret);
        return s32Ret;
    }

    s32Ret = HI_MPI_AO_Disable(AoDevId);
    if (HI_SUCCESS != s32Ret)
    {
        printf("%s: HI_MPI_AO_Disable failed with %#x!\n", __FUNCTION__, s32Ret);
        return s32Ret;
    }

    if (SAMPLE_AUDIO_INNER_HDMI_AO_DEV == AoDevId)
    {

#ifdef HI_ACODEC_TYPE_HDMI
        s32Ret = SAMPLE_COMM_AUDIO_StopHdmi();
        if (HI_SUCCESS != s32Ret)
        {
            printf("%s: SAMPLE_COMM_AUDIO_StopHdmi failed with %#x!\n", __FUNCTION__, s32Ret);
            return s32Ret;
        }

#endif
    }

    return HI_SUCCESS;
}

/******************************************************************************
* function : Start Aenc
******************************************************************************/
HI_S32 SAMPLE_COMM_AUDIO_StartAenc(HI_S32 s32AencChnCnt, AIO_ATTR_S *pstAioAttr, PAYLOAD_TYPE_E enType)
{
    AENC_CHN AeChn;
    HI_S32 s32Ret, i;
    AENC_CHN_ATTR_S stAencAttr;
    AENC_ATTR_ADPCM_S stAdpcmAenc;
    AENC_ATTR_G711_S stAencG711;
    AENC_ATTR_G726_S stAencG726;
    AENC_ATTR_LPCM_S stAencLpcm;
    AENC_ATTR_AAC_S  stAencAac;

    /* set AENC chn attr */

    stAencAttr.enType = enType;
    stAencAttr.u32BufSize = 30;
    stAencAttr.u32PtNumPerFrm = pstAioAttr->u32PtNumPerFrm;

    if (PT_ADPCMA == stAencAttr.enType)
    {
        stAencAttr.pValue       = &stAdpcmAenc;
        stAdpcmAenc.enADPCMType = AUDIO_ADPCM_TYPE;
    }
    else if (PT_G711A == stAencAttr.enType || PT_G711U == stAencAttr.enType)
    {
        stAencAttr.pValue       = &stAencG711;
    }
    else if (PT_G726 == stAencAttr.enType)
    {
        stAencAttr.pValue       = &stAencG726;
        stAencG726.enG726bps    = G726_BPS;
    }
    else if (PT_LPCM == stAencAttr.enType)
    {
        stAencAttr.pValue = &stAencLpcm;
    }
    else if (PT_AAC == stAencAttr.enType)
    {

        stAencAttr.pValue = &stAencAac;
        stAencAac.enAACType = gs_enAacType;
        stAencAac.enBitRate = gs_enAacBps;
        stAencAac.enBitWidth = AUDIO_BIT_WIDTH_16;
        stAencAac.enSmpRate = pstAioAttr->enSamplerate;
        stAencAac.enSoundMode = pstAioAttr->enSoundmode;
        stAencAac.enTransType = gs_enAacTransType;
        stAencAac.s16BandWidth = 0;
    }
    else
    {
        printf("%s: invalid aenc payload type:%d\n", __FUNCTION__, stAencAttr.enType);
        return HI_FAILURE;
    }

    for (i = 0; i < s32AencChnCnt; i++)
    {
        AeChn = i;

        /* create aenc chn*/
        s32Ret = HI_MPI_AENC_CreateChn(AeChn, &stAencAttr);
        if (HI_SUCCESS != s32Ret)
        {
            printf("%s: HI_MPI_AENC_CreateChn(%d) failed with %#x!\n", __FUNCTION__,
                   AeChn, s32Ret);
            return s32Ret;
        }
    }

    return HI_SUCCESS;
}

/******************************************************************************
* function : Stop Aenc
******************************************************************************/
HI_S32 SAMPLE_COMM_AUDIO_StopAenc(HI_S32 s32AencChnCnt)
{
    HI_S32 i;
    HI_S32 s32Ret;

    for (i = 0; i < s32AencChnCnt; i++)
    {
        s32Ret = HI_MPI_AENC_DestroyChn(i);
        if (HI_SUCCESS != s32Ret)
        {
            printf("%s: HI_MPI_AENC_DestroyChn(%d) failed with %#x!\n", __FUNCTION__,
                   i, s32Ret);
            return s32Ret;
        }

    }

    return HI_SUCCESS;
}

/******************************************************************************
* function : Destory the all thread
******************************************************************************/
HI_S32 SAMPLE_COMM_AUDIO_DestoryAllTrd(void)
{
    HI_U32 u32DevId, u32ChnId;

    for (u32DevId = 0; u32DevId < AI_DEV_MAX_NUM; u32DevId ++)
    {
        for (u32ChnId = 0; u32ChnId < AI_MAX_CHN_NUM; u32ChnId ++)
        {
            if(HI_SUCCESS != SAMPLE_COMM_AUDIO_DestoryTrdAi(u32DevId, u32ChnId))
            {
                printf("%s: SAMPLE_COMM_AUDIO_DestoryTrdAi(%d,%d) failed!\n", __FUNCTION__,
                   u32DevId, u32ChnId);
                return HI_FAILURE;
            }
        }
    }

    for (u32ChnId = 0; u32ChnId < AENC_MAX_CHN_NUM; u32ChnId ++)
    {
        if (HI_SUCCESS != SAMPLE_COMM_AUDIO_DestoryTrdAencAdec(u32ChnId))
        {
            printf("%s: SAMPLE_COMM_AUDIO_DestoryTrdAencAdec(%d) failed!\n", __FUNCTION__,
               u32ChnId);
            return HI_FAILURE;
        }
    }

    for (u32ChnId = 0; u32ChnId < ADEC_MAX_CHN_NUM; u32ChnId ++)
    {
        if (HI_SUCCESS != SAMPLE_COMM_AUDIO_DestoryTrdFileAdec(u32ChnId))
        {
            printf("%s: SAMPLE_COMM_AUDIO_DestoryTrdFileAdec(%d) failed!\n", __FUNCTION__,
               u32ChnId);
            return HI_FAILURE;
        }
    }

    for (u32ChnId = 0; u32ChnId < AO_DEV_MAX_NUM; u32ChnId ++)
    {
        if (HI_SUCCESS != SAMPLE_COMM_AUDIO_DestoryTrdAoVolCtrl(u32ChnId))
        {
            printf("%s: SAMPLE_COMM_AUDIO_DestoryTrdAoVolCtrl(%d) failed!\n", __FUNCTION__,
               u32ChnId);
            return HI_FAILURE;
        }
    }


    return HI_SUCCESS;
}


/******************************************************************************
* function : Start Adec
******************************************************************************/
HI_S32 SAMPLE_COMM_AUDIO_StartAdec(ADEC_CHN AdChn, PAYLOAD_TYPE_E enType)
{
    HI_S32 s32Ret;
    ADEC_CHN_ATTR_S stAdecAttr;
    ADEC_ATTR_ADPCM_S stAdpcm;
    ADEC_ATTR_G711_S stAdecG711;
    ADEC_ATTR_G726_S stAdecG726;
    ADEC_ATTR_LPCM_S stAdecLpcm;
    ADEC_ATTR_AAC_S stAdecAac;
    ADEC_ATTR_MP3_S  pstMp3Attr;

    stAdecAttr.enType = enType;
    stAdecAttr.u32BufSize = 20;
    stAdecAttr.enMode = ADEC_MODE_STREAM;/* propose use pack mode in your app */

    if (PT_ADPCMA == stAdecAttr.enType)
    {
        stAdecAttr.pValue = &stAdpcm;
        stAdpcm.enADPCMType = AUDIO_ADPCM_TYPE ;
    }
    else if (PT_G711A == stAdecAttr.enType || PT_G711U == stAdecAttr.enType)
    {
        stAdecAttr.pValue = &stAdecG711;
    }
    else if (PT_G726 == stAdecAttr.enType)
    {
        stAdecAttr.pValue = &stAdecG726;
        stAdecG726.enG726bps = G726_BPS ;
    }
    else if (PT_LPCM == stAdecAttr.enType)
    {
        stAdecAttr.pValue = &stAdecLpcm;
        stAdecAttr.enMode = ADEC_MODE_PACK;/* lpcm must use pack mode */
    }
    else if (PT_AAC == stAdecAttr.enType)
    {
        stAdecAttr.pValue = &stAdecAac;
        stAdecAttr.enMode = ADEC_MODE_STREAM;   /* aac should be stream mode */
        stAdecAac.enTransType = gs_enAacTransType;
    }
    else if (PT_MP3 == stAdecAttr.enType)
    {
        stAdecAttr.pValue = &pstMp3Attr;
    }
    else
    {
        printf("%s: invalid aenc payload type:%d\n", __FUNCTION__, stAdecAttr.enType);
        return HI_FAILURE;
    }

    /* create adec chn*/
    s32Ret = HI_MPI_ADEC_CreateChn(AdChn, &stAdecAttr);
    if (HI_SUCCESS != s32Ret)
    {
        printf("%s: HI_MPI_ADEC_CreateChn(%d) failed with %#x!\n", __FUNCTION__, \
               AdChn, s32Ret);
        return s32Ret;
    }
    return 0;
}

/******************************************************************************
* function : Stop Adec
******************************************************************************/
HI_S32 SAMPLE_COMM_AUDIO_StopAdec(ADEC_CHN AdChn)
{
    HI_S32 s32Ret;

    s32Ret = HI_MPI_ADEC_DestroyChn(AdChn);
    if (HI_SUCCESS != s32Ret)
    {
        printf("%s: HI_MPI_ADEC_DestroyChn(%d) failed with %#x!\n", __FUNCTION__,
               AdChn, s32Ret);
        return s32Ret;
    }

    return HI_SUCCESS;
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */

