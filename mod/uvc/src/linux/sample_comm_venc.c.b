/******************************************************************************
  A simple program of Hisilicon Hi35xx video encode implementation.
  Copyright (C), 2018, Hisilicon Tech. Co., Ltd.
******************************************************************************/

#include <pthread.h>
#include <unistd.h>

#include "sample_venc.h"
#include "sample_yuv.h"
#include "frame_cache.h"
#include "uvc_venc_glue.h"

#ifndef __HuaweiLite__
#include "log.h"
#include "config_svc.h"
#endif

static pthread_t __gs_VencPid;
static SAMPLE_VENC_GETSTREAM_PARA_S __gs_stPara;

#define v4l2_fourcc(a, b, c, d) \
    ((uint32_t)(a) | ((uint32_t)(b) << 8) | ((uint32_t)(c) << 16) | ((uint32_t)(d) << 24))

#define V4L2_PIX_FMT_YUYV v4l2_fourcc('Y', 'U', 'Y', 'V')  /* 16  YUV 4:2:2     */
#define V4L2_PIX_FMT_MJPEG v4l2_fourcc('M', 'J', 'P', 'G') /* Motion-JPEG   */
#define V4L2_PIX_FMT_H264 v4l2_fourcc('H', '2', '6', '4')  /* H264 with start codes */

encoder_property __encoder_property;

static PAYLOAD_TYPE_E change_to_mpp_format(uint32_t fcc)
{
    PAYLOAD_TYPE_E t;

    switch (fcc)
    {
    case V4L2_PIX_FMT_YUYV:
    case V4L2_PIX_FMT_MJPEG:
        t = PT_MJPEG;
        break;

    case V4L2_PIX_FMT_H264:
        t = PT_H264;
        break;

    default:
        t = PT_MJPEG;
        break;
    }

    return t;
}

static PIC_SIZE_E change_to_mpp_wh(int width)
{
    PIC_SIZE_E s = PIC_720P;

    switch (width)
    {
    case 640:
#ifndef __HuaweiLite__
        s = PIC_360P; /*640 x 360*/
#else
        s = PIC_VGA;
#endif
        break;
    case 1280:
        s = PIC_720P;
        break;
    case 1920:
        s = PIC_1080P;
        break;
    case 3840:
        s = PIC_3840x2160;
        break;
    default:
        s = PIC_720P;
        break;
    }

    return s;
}

static HI_S32 SAMPLE_COMM_VENC_GetFilePostfix(PAYLOAD_TYPE_E enPayload, char *szFilePostfix)
{
    if (PT_H264 == enPayload)
    {
        strcpy(szFilePostfix, ".h264");
    }
    else if (PT_H265 == enPayload)
    {
        strcpy(szFilePostfix, ".h265");
    }
    else if (PT_JPEG == enPayload)
    {
        strcpy(szFilePostfix, ".jpg");
    }
    else if (PT_MJPEG == enPayload)
    {
        strcpy(szFilePostfix, ".mjp");
    }
    else
    {
        SAMPLE_PRT("payload type err!\n");
        return HI_FAILURE;
    }
    return HI_SUCCESS;
}

static HI_VOID set_config_format(PAYLOAD_TYPE_E *format, int idx)
{
    format[idx] = change_to_mpp_format(__encoder_property.format);
}

static HI_VOID set_config_wh(PIC_SIZE_E *wh, int idx)
{
    wh[idx] = change_to_mpp_wh(__encoder_property.width);
}

HI_VOID set_user_config_format(PAYLOAD_TYPE_E *format, PIC_SIZE_E *wh, int *c)
{
    set_config_format(format, 0);
    set_config_wh(wh, 0);
    *c = 1;
}

int set_encoder_property(encoder_property *p)
{
    __encoder_property = *p;
    set_yuv_property();

    return 0;
}

int is_channel_yuv(int channel)
{
    if ((channel == 1) && (__encoder_property.format == V4L2_PIX_FMT_YUYV))
    {
        return 1;
    }

    return 0;
}

HI_S32 __SAMPLE_COMM_VENC_FORMAT(HI_VOID)
{
    if (__encoder_property.format == V4L2_PIX_FMT_YUYV)
    {
        return PIXEL_FORMAT_YVU_SEMIPLANAR_422;
    }
    else
    {
        return PIXEL_FORMAT_YVU_SEMIPLANAR_420;
    }
}

#ifndef __HuaweiLite__
/******************************************************************************
* funciton : save h264/h265/mjpeg stream.
******************************************************************************/
static HI_S32 __SAMPLE_COMM_VENC_SaveData(VENC_STREAM_S *pstStream)
{
    HI_U32 i = 0;
    VENC_PACK_S *pstData = HI_NULL;
    unsigned char *s = HI_NULL;
    unsigned int data_len = 0;
    unsigned int copy_size = 0;

    uvc_cache_t *uvc_cache = uvc_cache_get();
    frame_node_t *fnode = HI_NULL;

    if (uvc_cache)
    {
        get_node_from_queue(uvc_cache->free_queue, &fnode);
    }

    if (!fnode)
    {
        return HI_SUCCESS;
    }

    fnode->used = 0;

    for (i = 0; i < pstStream->u32PackCount; ++i)
    {
        pstData = &pstStream->pstPack[i];
        s = pstData->pu8Addr + pstData->u32Offset;
        data_len = pstData->u32Len - pstData->u32Offset;
        copy_size = data_len < (fnode->length - fnode->used) ? data_len : (fnode->length - fnode->used);

        if (copy_size > 0)
        {
            memcpy(fnode->mem + fnode->used, s, copy_size);
            fnode->used += copy_size;
        }
    }

    put_node_to_queue(uvc_cache->ok_queue, fnode);

    return HI_SUCCESS;
}

/******************************************************************************
* funciton : save stream
******************************************************************************/
static HI_S32 __SAMPLE_COMM_VENC_SaveStream(PAYLOAD_TYPE_E enType, FILE *pFd, VENC_STREAM_S *pstStream)
{
    HI_S32 s32Ret = HI_FAILURE;
#if (1 == UVC_SAVE_FILE)
    HI_S32 i = 0;
#endif

    if (PT_H264 == enType || PT_MJPEG == enType || PT_H265 == enType)
    {
        s32Ret = __SAMPLE_COMM_VENC_SaveData(pstStream);
    }

#if (1 == UVC_SAVE_FILE)
    for (i = 0; i < pstStream->u32PackCount; ++i)
    {
        fwrite(pstStream->pstPack[i].pu8Addr + pstStream->pstPack[i].u32Offset,
               pstStream->pstPack[i].u32Len - pstStream->pstPack[i].u32Offset, 1, pFd);

        fflush(pFd);
    }
#endif

    return s32Ret;
}

#else

static HI_S32 g_compsited = 0;
extern EVENT_CB_S g_frame_event;

HI_S32 __SAMPLE_COMM_VENC_SaveFile(PAYLOAD_TYPE_E enType, FILE *pFd, VENC_STREAM_BUF_INFO_S *pstStreamBuf, VENC_STREAM_S *pstStream)
{
    HI_U32 i = 0;
    HI_U32 j = 0;
    HI_U32 u32Left = 0;
    VENC_PACK_S *pstData = HI_NULL;
    unsigned char *s;
    unsigned int data_len, copy_size;

    uvc_cache_t *uvc_cache = uvc_cache_get();
    frame_node_t *fnode = HI_NULL;

    if (uvc_cache)
    {
        get_node_from_queue(uvc_cache->free_queue, &fnode);
    }

    if (!fnode) {
        //dprintf("get_node_from_queue fail !!\n");
        return HI_SUCCESS;
    }

    fnode->used = CACHE_MEM_START;
    fnode->offset = 0;

    if (g_compsited)
    {
        add_multi_payload_header(fnode, __encoder_property.format);
    }

    for (i = 0; i<pstStream->u32PackCount; i++)
    {
        pstData = &pstStream->pstPack[i];

        for (j = 0; j < MAX_TILE_NUM; ++j)
        {
            if((pstData->u64PhyAddr > pstStreamBuf->u64PhyAddr[j]) &&
                (pstData->u64PhyAddr <= pstStreamBuf->u64PhyAddr[j] + pstStreamBuf->u64BufSize[j]))
            {
                break;
            }
        }

        if (pstData->u64PhyAddr + pstData->u32Len >=
            pstStreamBuf->u64PhyAddr[j] + pstStreamBuf->u64BufSize[j])
        {
            if (pstData->u64PhyAddr + pstData->u32Offset >=
                pstStreamBuf->u64PhyAddr[j] + pstStreamBuf->u64BufSize[j])
            {
                /* physical address retrace in offset segment */
                s = (unsigned char *)(HI_UL)(pstStreamBuf->u64PhyAddr[j] + ((pstData->u64PhyAddr + pstData->u32Offset) - (pstStreamBuf->u64PhyAddr[j] + pstStreamBuf->u64BufSize[j])));
                data_len = pstData->u32Len - pstData->u32Offset;
                copy_size = data_len < (fnode->length - fnode->used) ? data_len : (fnode->length - fnode->used);

#if (1 == UVC_SAVE_FILE)
                fwrite((void *)s, data_len, 1, pFd);
#endif
                memcpy(fnode->mem + fnode->used, s, copy_size);
                fnode->used += copy_size;
            }
            else
            {
                /* physical address retrace in data segment */
                u32Left = (pstStreamBuf->u64PhyAddr[j] + pstStreamBuf->u64BufSize[j]) - pstData->u64PhyAddr;
                s = (unsigned char *)(HI_UL)(pstData->u64PhyAddr + pstData->u32Offset);
                data_len = u32Left - pstData->u32Offset;
                copy_size = data_len < (fnode->length - fnode->used) ? data_len : (fnode->length - fnode->used);

#if (1 == UVC_SAVE_FILE)
                fwrite((void *)s, data_len, 1, pFd);
#endif
                memcpy(fnode->mem + fnode->used, s, copy_size);
                fnode->used += copy_size;

                s = (unsigned char *)(HI_UL)pstStreamBuf->u64PhyAddr[j];
                data_len = pstData->u32Len - u32Left;
                copy_size = data_len < (fnode->length - fnode->used) ? data_len : (fnode->length - fnode->used);

#if (1 == UVC_SAVE_FILE)
                fwrite((void *)s, data_len, 1, pFd);
#endif
                memcpy(fnode->mem + fnode->used, s, copy_size);
                fnode->used += copy_size;
            }
        }
        else
        {
            /* physical address retrace does not happen */
            s = (unsigned char *)(HI_UL)(pstData->u64PhyAddr + pstData->u32Offset);
            data_len = pstData->u32Len - pstData->u32Offset;
            copy_size = data_len < (fnode->length - fnode->used) ? data_len : (fnode->length - fnode->used);

#if (1 == UVC_SAVE_FILE)
            fwrite ((void *)s, data_len, 1, pFd);
#endif
            memcpy(fnode->mem + fnode->used, s, copy_size);
            fnode->used += copy_size;
        }
    }

    if (g_compsited)
    {
        set_multi_payload_size(fnode);
    }

    put_node_to_queue(uvc_cache->ok_queue, fnode);

#if (1 == UVC_SAVE_FILE)
    fflush(pFd);
#endif

    LOS_EventWrite(&g_frame_event, 0x01);

    return HI_SUCCESS;
}
#endif /* end of #ifndef __HuaweiLite__ */

/******************************************************************************
* funciton : get stream from each channels and save them
******************************************************************************/
HI_VOID *__SAMPLE_COMM_VENC_GetVencStreamProc(HI_VOID *p)
{
    HI_S32 i;
    HI_S32 s32ChnTotal;
    VENC_CHN_ATTR_S stVencChnAttr;
    SAMPLE_VENC_GETSTREAM_PARA_S *pstPara;
    HI_S32 maxfd = 0;
    struct timeval TimeoutVal;
    fd_set read_fds;
    HI_U32 u32PictureCnt[VENC_MAX_CHN_NUM] = {0};
    HI_S32 VencFd[VENC_MAX_CHN_NUM];
#if (1 == UVC_SAVE_FILE)
    HI_CHAR aszFileName[VENC_MAX_CHN_NUM][64];
#endif
    FILE *pFile[VENC_MAX_CHN_NUM];
    char szFilePostfix[10];
    VENC_CHN_STATUS_S stStat;
    VENC_STREAM_S stStream;
    HI_S32 s32Ret;
    VENC_CHN VencChn;
    PAYLOAD_TYPE_E enPayLoadType[VENC_MAX_CHN_NUM];
    VENC_STREAM_BUF_INFO_S stStreamBufInfo[VENC_MAX_CHN_NUM];

    pstPara = (SAMPLE_VENC_GETSTREAM_PARA_S *)p;
    s32ChnTotal = pstPara->s32Cnt;

    /******************************************
     step 1:  check & prepare save-file & venc-fd
    ******************************************/
    if (s32ChnTotal >= VENC_MAX_CHN_NUM)
    {
        SAMPLE_PRT("input count invaild\n");
        return HI_NULL;
    }
    for (i = 0; i < s32ChnTotal; i++)
    {
        /* decide the stream file name, and open file to save stream */
        VencChn = pstPara->VeChn[i];
        s32Ret = HI_MPI_VENC_GetChnAttr(VencChn, &stVencChnAttr);
        if (s32Ret != HI_SUCCESS)
        {
            SAMPLE_PRT("HI_MPI_VENC_GetChnAttr chn[%d] failed with %#x!\n",
                       VencChn, s32Ret);
            return HI_NULL;
        }
        enPayLoadType[i] = stVencChnAttr.stVencAttr.enType;

        s32Ret = SAMPLE_COMM_VENC_GetFilePostfix(enPayLoadType[i], szFilePostfix);
        if (s32Ret != HI_SUCCESS)
        {
            SAMPLE_PRT("SAMPLE_COMM_VENC_GetFilePostfix [%d] failed with %#x!\n",
                       stVencChnAttr.stVencAttr.enType, s32Ret);
            return HI_NULL;
        }
#if (1 == UVC_SAVE_FILE)
        if (PT_JPEG != enPayLoadType[i])
        {
            snprintf(aszFileName[i], 32, "output/stream_chn%d%s", i, szFilePostfix);

            pFile[i] = fopen(aszFileName[i], "wb");
            if (!pFile[i])
            {
                SAMPLE_PRT("open file[%s] failed!\n",
                           aszFileName[i]);
                return HI_NULL;
            }
        }
#endif
        /* Set Venc Fd. */
        VencFd[i] = HI_MPI_VENC_GetFd(i);
        if (VencFd[i] < 0)
        {
            SAMPLE_PRT("HI_MPI_VENC_GetFd failed with %#x!\n",
                       VencFd[i]);
            return HI_NULL;
        }
        if (maxfd <= VencFd[i])
        {
            maxfd = VencFd[i];
        }

        s32Ret = HI_MPI_VENC_GetStreamBufInfo(i, &stStreamBufInfo[i]);
        if (HI_SUCCESS != s32Ret)
        {
            SAMPLE_PRT("HI_MPI_VENC_GetStreamBufInfo failed with %#x!\n", s32Ret);
            return (HI_VOID *)HI_FAILURE;
        }
    }

    /******************************************
     step 2:  Start to get streams of each channel.
    ******************************************/
    while (HI_TRUE == pstPara->bThreadStart)
    {
        FD_ZERO(&read_fds);
        for (i = 0; i < s32ChnTotal; i++)
        {
            FD_SET(VencFd[i], &read_fds);
        }

        TimeoutVal.tv_sec = 2;
        TimeoutVal.tv_usec = 0;
        s32Ret = select(maxfd + 1, &read_fds, HI_NULL, HI_NULL, &TimeoutVal);
        if (s32Ret < 0)
        {
            SAMPLE_PRT("select failed!\n");
            break;
        }
        else if (s32Ret == 0)
        {
            SAMPLE_PRT("get venc stream time out, exit thread\n");
            continue;
        }
        else
        {
            for (i = 0; i < s32ChnTotal; i++)
            {
                if (FD_ISSET(VencFd[i], &read_fds))
                {
                    /*******************************************************
                     step 2.1 : query how many packs in one-frame stream.
                    *******************************************************/
                    memset(&stStream, 0, sizeof(stStream));

                    s32Ret = HI_MPI_VENC_QueryStatus(i, &stStat);
                    if (HI_SUCCESS != s32Ret)
                    {
                        SAMPLE_PRT("HI_MPI_VENC_QueryStatus chn[%d] failed with %#x!\n", i, s32Ret);
                        break;
                    }

                    /*******************************************************
                     step 2.2 :suggest to check both u32CurPacks and u32LeftStreamFrames at the same time,for example:
                     if (0 == stStat.u32CurPacks || 0 == stStat.u32LeftStreamFrames)
                     {
                     SAMPLE_PRT("NOTE: Current frame is HI_NULL!\n");
                     continue;
                     }
                    *******************************************************/
                    if (0 == stStat.u32CurPacks)
                    {
                        SAMPLE_PRT("NOTE: Current frame is HI_NULL!\n");
                        continue;
                    }
                    /*******************************************************
                     step 2.3 : malloc corresponding number of pack nodes.
                    *******************************************************/
                    stStream.pstPack = (VENC_PACK_S *)malloc(sizeof(VENC_PACK_S) * stStat.u32CurPacks);
                    if (HI_NULL == stStream.pstPack)
                    {
                        SAMPLE_PRT("malloc stream pack failed!\n");
                        break;
                    }

                    /*******************************************************
                     step 2.4 : call mpi to get one-frame stream
                    *******************************************************/
                    stStream.u32PackCount = stStat.u32CurPacks;
                    s32Ret = HI_MPI_VENC_GetStream(i, &stStream, HI_TRUE);
                    if (HI_SUCCESS != s32Ret)
                    {
                        free(stStream.pstPack);
                        stStream.pstPack = HI_NULL;
                        SAMPLE_PRT("HI_MPI_VENC_GetStream failed with %#x!\n", s32Ret);
                        break;
                    }

                    /*******************************************************
                     step 2.5 : save frame to file
                    *******************************************************/
#if (1 == UVC_SAVE_FILE)
                    if (PT_JPEG == enPayLoadType[i])
                    {
                        snprintf(aszFileName[i], 32, "output/stream_chn%d_%d%s", i, u32PictureCnt[i], szFilePostfix);
                        pFile[i] = fopen(aszFileName[i], "wb");
                        if (!pFile[i])
                        {
                            SAMPLE_PRT("open file err!\n");
                            return HI_NULL;
                        }
                    }
#endif
#ifndef __HuaweiLite__
                    s32Ret = __SAMPLE_COMM_VENC_SaveStream(enPayLoadType[i], pFile[i], &stStream);
#else
                    s32Ret = __SAMPLE_COMM_VENC_SaveFile(enPayLoadType[i], pFile[i], &stStreamBufInfo[i], &stStream);
#endif
                    if (HI_SUCCESS != s32Ret)
                    {
                        free(stStream.pstPack);
                        stStream.pstPack = HI_NULL;
                        SAMPLE_PRT("save stream failed!\n");
                        break;
                    }

                    /*******************************************************
                     step 2.6 : release stream
                    *******************************************************/
                    s32Ret = HI_MPI_VENC_ReleaseStream(i, &stStream);
                    if (HI_SUCCESS != s32Ret)
                    {
                        SAMPLE_PRT("HI_MPI_VENC_ReleaseStream failed!\n");
                        free(stStream.pstPack);
                        stStream.pstPack = HI_NULL;
                        break;
                    }

                    /*******************************************************
                     step 2.7 : free pack nodes
                    *******************************************************/
                    free(stStream.pstPack);
                    stStream.pstPack = HI_NULL;
                    u32PictureCnt[i]++;
                    if (PT_JPEG == enPayLoadType[i])
                    {
                        fclose(pFile[i]);
                    }
                }
            }
        }
    }
    /*******************************************************
     step 3 : close save-file
    *******************************************************/
#if (1 == UVC_SAVE_FILE)
    for (i = 0; i < s32ChnTotal; i++)
    {
        if (PT_JPEG != enPayLoadType[i])
        {
            fclose(pFile[i]);
        }
    }
#endif
    return HI_NULL;
}

HI_S32 __SAMPLE_COMM_VENC_StartGetStream(VENC_CHN VeChn[], HI_S32 s32Cnt)
{
    HI_S32 i = 0;

    __gs_stPara.bThreadStart = HI_TRUE;
    __gs_stPara.s32Cnt = s32Cnt;
    for (i = 0; i < s32Cnt; ++i)
    {
        __gs_stPara.VeChn[i] = VeChn[i];
    }

    return pthread_create(&__gs_VencPid, 0, __SAMPLE_COMM_VENC_GetVencStreamProc, (HI_VOID *)&__gs_stPara);
}

/******************************************************************************
* funciton : stop get venc stream process.
******************************************************************************/
HI_S32 __SAMPLE_COMM_VENC_StopGetStream(HI_VOID)
{
    if (HI_TRUE == __gs_stPara.bThreadStart)
    {
        __gs_stPara.bThreadStart = HI_FALSE;
        pthread_join(__gs_VencPid, 0);
    }
    return HI_SUCCESS;
}
