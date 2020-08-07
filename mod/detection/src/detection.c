#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <sys/timeb.h>
#include <sys/time.h>
#include <time.h>
#include <unistd.h>
//#include "inc/gsf.h"
#include "mpp.h"
#include "detection.h"
//#include "cfg.h"
// #include "msg_func.h"
// #include "rtsp-st.h"
#include "ins_nnie_c_interface.h"
#include "mod/bsp/inc/bsp.h"
#include "hi_ive.h"
#include "hi_comm_ive.h"
// #include "sample_comm_ive.h"
// #include "ivs_md.h"
void* svp_pub = NULL;
GSF_LOG_GLOBAL_INIT("DETECTION", 8 * 1024);

static int nnie_id = -1;
static int nnie_count = 0;

static void main_exit(int signal)
{
    //TODO;
    exit(0);
}

static void *detection_task(void *p)
{
    HI_S32 s32Ret, i = 0;
    VIDEO_FRAME_INFO_S stExtFrmInfo;
    HI_S32 s32VpssGrp = 0;
    HI_S32 as32VpssChn[] ={ VPSS_CHN0, VPSS_CHN1 };
    HI_S32 s32MilliSec = 1000;
    HI_U32 u32Size = 0;

    while (1) //(HI_FALSE == s_bStopSignal)
    {
        //usleep(10 * 1000);

        s32Ret = HI_MPI_VPSS_GetChnFrame(s32VpssGrp, as32VpssChn[1], &stExtFrmInfo, s32MilliSec);
        if (HI_SUCCESS != s32Ret)
        {
            SAMPLE_PRT("Error(%#x),HI_MPI_VPSS_GetChnFrame failed, VPSS_GRP(%d), VPSS_CHN(%d)!\n",
                s32Ret, s32VpssGrp, as32VpssChn[1]);
            continue;
        }

        if (PIXEL_FORMAT_YUV_SEMIPLANAR_420 == stExtFrmInfo.stVFrame.enPixelFormat || PIXEL_FORMAT_YVU_SEMIPLANAR_420 == stExtFrmInfo.stVFrame.enPixelFormat)
        {
            u32Size = (stExtFrmInfo.stVFrame.u32Stride[0]) * (stExtFrmInfo.stVFrame.u32Height) * 3 / 2;
        }
        else if (PIXEL_FORMAT_YUV_SEMIPLANAR_422 == stExtFrmInfo.stVFrame.enPixelFormat || PIXEL_FORMAT_YVU_SEMIPLANAR_422 == stExtFrmInfo.stVFrame.enPixelFormat)
        {
            u32Size = (stExtFrmInfo.stVFrame.u32Stride[0]) * (stExtFrmInfo.stVFrame.u32Height) * 2;
        }
        else if (PIXEL_FORMAT_YUV_400 == stExtFrmInfo.stVFrame.enPixelFormat)
        {
            u32Size = (stExtFrmInfo.stVFrame.u32Stride[0]) * (stExtFrmInfo.stVFrame.u32Height);
        }

        // HI_MPI_SYS_MmzFree(u64PhyAddr,u64PhyAddr);
        //printf("get chn bgr\n");

        HI_CHAR *pVirAddr = (HI_CHAR *)HI_MPI_SYS_Mmap(stExtFrmInfo.stVFrame.u64PhyAddr[0], u32Size);

        //printf("get chn frame width:%d,height:%d,enVideoFormat:%d  *** %d\n", stExtFrmInfo.stVFrame.u32Width, stExtFrmInfo.stVFrame.u32Height, stExtFrmInfo.stVFrame.enPixelFormat, u32Size);
        int count = 0;
        YOLOV3_Result* result = CNNIE_run_with_data_yuv(nnie_id, nnie_count++, pVirAddr, stExtFrmInfo.stVFrame.u32Height, stExtFrmInfo.stVFrame.u32Width, 2, 3, 0.2, 0.5, &count);

      
        char buf[sizeof(gsf_msg_t) + sizeof(gsf_detection_yolov3_t)];
        gsf_msg_t *msg = (gsf_msg_t*)buf;

        memset(msg, 0, sizeof(*msg));
        msg->id = GSF_EV_DETECTION_YOLO3;
        msg->ts = time(NULL)*1000;
        msg->sid = 0;
        msg->err = 0;
        msg->size = sizeof(gsf_detection_yolov3_t);
        
        gsf_detection_yolov3_t *detec_yolov3 = (gsf_detection_yolov3_t*)msg->data;
        
        detec_yolov3->pts = stExtFrmInfo.stVFrame.u64PTS/1000;
        detec_yolov3->w = 608; //stExtFrmInfo.stVFrame.u32Width;
        detec_yolov3->h = 608; //stExtFrmInfo.stVFrame.u32Height;
        detec_yolov3->cnt = count;

        for (size_t i = 0; i < count; i++)
        {
            //printf("%d %d %d %d %d %d\n", result[i].m_class, result[i].m_confidence, result[i].m_rect.m_iLeft, result[i].m_rect.m_iTop, result[i].m_rect.m_iRight, result[i].m_rect.m_iBottom);
       		detec_yolov3->result[i].rect[0] = result[i].m_rect.m_iLeft;
  			detec_yolov3->result[i].rect[1] = result[i].m_rect.m_iTop;
  			detec_yolov3->result[i].rect[2] = result[i].m_rect.m_iRight - result[i].m_rect.m_iLeft;
  			detec_yolov3->result[i].rect[3] = result[i].m_rect.m_iBottom - result[i].m_rect.m_iTop;
            detec_yolov3->result[i].cls = result[i].m_class;
            detec_yolov3->result[i].confidence = result[i].m_confidence;
        }
        nm_pub_send(svp_pub, (char*)msg, sizeof(*msg)+msg->size);

        CNNIE_free(result);
        HI_MPI_SYS_Munmap(pVirAddr, u32Size);
        HI_MPI_VPSS_ReleaseChnFrame(s32VpssGrp, as32VpssChn[1], &stExtFrmInfo);
    }
}
static void detection_callback(int nnie_id, int run_id, int cls_id, float confidence, float xmin, float ymin, float xmax, float ymax) {
    static int image_index = 0;
    static int frame_index = 0;
    if (run_id !=image_index)
    {
        // 新的
        frame_index = 0;

    }
    
  //printf("%d %d %.3f %.3f %.3f %.3f %.3f\n",run_id, cls_id, confidence, xmin, ymin, xmax, ymax);
    frame_index ++;
   image_index = run_id;
}
int main(int argc, char *argv[])
{
    signal(SIGPIPE, SIG_IGN);
    signal(SIGTERM, main_exit);
    signal(SIGINT, main_exit);


    svp_pub = nm_pub_listen(GSF_PUB_DETECTION);
    // if (argc < 2)
    // {
    //     printf("pls input: %s gb28181_parm.json\n", argv[0]);
    //     return -1;
    // }
    const char *image_path = "./data/nnie_image/test/000100_608x608.bgr";
    const char *model_path = "./data/nnie_model/detection/yolov3_my_inst.wk";
    nnie_id = CNNIE_init(model_path, 608, 608,NULL);


    // int file_length = 0;
    // FILE *fp = fopen(image_path, "rb");
    // if (fp == NULL)
    // {
    //     printf("open %s failed\n", image_path);
    //     return;
    // }

    // fseek(fp, 0L, SEEK_END);
    // file_length = ftell(fp);
    // fseek(fp, 0L, SEEK_SET);

    // unsigned char *data = (unsigned char *)malloc(sizeof(unsigned char) * file_length);

    // fread(data, file_length, 1, fp);

    // fclose(fp);
    int count = 0;
    YOLOV3_Result* result  = CNNIE_run_with_file(nnie_id, nnie_count++, image_path, 608, 608, 2, 3, 0.2, 0.5,&count); 
    printf("yolov3 NNIE count %d,%d\n",count,result); 
    for (size_t i = 0; i < count; i++)
    {
        printf("%d %d %d %d %d %d\n", result[i].m_class,result[i ].m_confidence, result[i].m_rect.m_iLeft, result[i].m_rect.m_iTop, result[i].m_rect.m_iRight, result[i].m_rect.m_iBottom);
    }
    printf("yolov3 NNIE ok\n");

    CNNIE_free(result);

    // free(data);
    
    detection_task(NULL);

    //CNNIE_finish(id);
    while (1)
    {
        sleep(6);
    }
    return 0;

  
}