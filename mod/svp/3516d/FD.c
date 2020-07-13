#include<stdio.h>
#include<string.h>
#include <sys/timeb.h>
#include <sys/time.h>
#include <time.h>
#include <unistd.h>
#include "svp.h"
#include "sample_comm_ive.h"
#include "FD.h"
#include "nnie_face_api.h"
#include "basetype.h"

static pthread_t s_hMdThread = 0;
static HI_BOOL s_bStopSignal = HI_FALSE;
static void* fd_task(void* p);
extern int tracker_id(int FrameIndex, RESULT_BAG *result_bag, RESULT_BAG *result_out);
 extern void * svp_pub;
 
int saveBMPFile(unsigned char* src, int width, int height, const char* name)
{
	BMPHeader hdr;
	BMPInfoHeader infoHdr;
    int ret = 0;
 
	FILE* fp = NULL;
    if(NULL == src) 
    {
		return (-1);
    }
 
    fp = fopen(name,"wb");
	if(NULL == fp)
    {
		printf("saveBMPFile: Err: Open!\n");
		return (-1);
	}
 
	infoHdr.size	= sizeof(BMPInfoHeader);
	infoHdr.width	= width;
	infoHdr.height	= 0 - height;
	infoHdr.planes	= 1;
	infoHdr.bitsPerPixel	= 24;
	infoHdr.compression		= 0;
	infoHdr.imageSize		= width*height;
	infoHdr.xPelsPerMeter	= 0;
	infoHdr.yPelsPerMeter	= 0;
	infoHdr.clrUsed			= 0;
	infoHdr.clrImportant	= 0;
 
	hdr.type	= 0x4D42;
	hdr.size	= 54 + infoHdr.imageSize*3;
	hdr.reserved1	= 0;
	hdr.reserved2	= 0;
	hdr.offset	= 54;
 
 
    //BGR2RGB
    int i;
    int temp;
    for(i = 0;i < width*height;i++)
    {
        temp = src[i*3];
        src[i*3] = src[i*3 + 2];
        src[i*3 + 2] = temp;
    }
 
	fwrite(&hdr, sizeof(hdr), 1, fp);
  	fwrite(&infoHdr, sizeof(infoHdr), 1, fp);
 	fwrite(src, sizeof(unsigned char), width*height*3, fp); //TODO:
    fflush(fp);
 
	if(ferror(fp)){
		printf("saveBMPFile: Err: Unknown!***\n");
	}
 
	fclose(fp);
    fp = NULL;
 
	return 0;
}


int fd_start()
{
	printf("fd_init");
	char *pcModelName = "/userdata/data/nnie_model/face/mnet_640_inst.wk";
	float threshold = 0.6;
	int isLog = 0;
	NNIE_FACE_DETECTOR_INIT(pcModelName, threshold, isLog);
	return pthread_create(&s_hMdThread, NULL, fd_task, NULL);
}

int fd_stop()
{
	s_bStopSignal = HI_TRUE;
	return pthread_join(s_hMdThread, NULL);
	NNIE_FACE_DETECTOR_RELEASE();
}

static void* fd_task(void* p)
{
	HI_S32 s32Ret, i = 0;
	VIDEO_FRAME_INFO_S stExtFrmInfo;
	HI_S32 s32VpssGrp = 0;
	HI_S32 as32VpssChn[] = {VPSS_CHN0, VPSS_CHN1};
	HI_S32 s32MilliSec = 1000;
	HI_U32 u32Size = 0;
	int framecnt = 0;
	
	IVE_CSC_CTRL_S stCscCtrl;
	IVE_DST_IMAGE_S stDstData;    
	IVE_SRC_IMAGE_S stSrcData;       
	IVE_HANDLE hIveHandle;    
	HI_BOOL bInstant = HI_TRUE;    
	HI_BOOL bFinish = HI_FALSE;    
	HI_BOOL bBlock = HI_TRUE;
	stCscCtrl.enMode = IVE_CSC_MODE_VIDEO_BT601_YUV2RGB;
	

	while (HI_FALSE == s_bStopSignal)
	{
		usleep(10*1000);
		
		s32Ret = HI_MPI_VPSS_GetChnFrame(s32VpssGrp, as32VpssChn[1], &stExtFrmInfo, s32MilliSec);
		if(HI_SUCCESS != s32Ret)
		{
		  	SAMPLE_PRT("Error(%#x),HI_MPI_VPSS_GetChnFrame failed, VPSS_GRP(%d), VPSS_CHN(%d)!\n",
		      s32Ret,s32VpssGrp, as32VpssChn[1]);
		  	continue;
		}
		if (PIXEL_FORMAT_YUV_SEMIPLANAR_420 == stExtFrmInfo.stVFrame.enPixelFormat
		|| PIXEL_FORMAT_YVU_SEMIPLANAR_420 == stExtFrmInfo.stVFrame.enPixelFormat)    
		{        
			u32Size = (stExtFrmInfo.stVFrame.u32Stride[0]) * (stExtFrmInfo.stVFrame.u32Height) * 3 / 2;        
		}    
		else if(PIXEL_FORMAT_YUV_SEMIPLANAR_422 == stExtFrmInfo.stVFrame.enPixelFormat
		|| PIXEL_FORMAT_YVU_SEMIPLANAR_422 == stExtFrmInfo.stVFrame.enPixelFormat)    
		{        
		  	u32Size = (stExtFrmInfo.stVFrame.u32Stride[0]) * (stExtFrmInfo.stVFrame.u32Height) * 2;        
		}  
		else if(PIXEL_FORMAT_YUV_400 == stExtFrmInfo.stVFrame.enPixelFormat)    
		{        
		  	u32Size = (stExtFrmInfo.stVFrame.u32Stride[0]) * (stExtFrmInfo.stVFrame.u32Height);   
		} 

		//HI_CHAR* pVirAddr = (HI_CHAR*) HI_MPI_SYS_Mmap(stExtFrmInfo.stVFrame.u64PhyAddr[0], u32Size); 



		struct timespec ts1, ts2;
		clock_gettime(CLOCK_MONOTONIC, &ts1);

		stSrcData.enType = IVE_IMAGE_TYPE_YUV420SP;   //骞夸箟鍥惧儚鐨勭墿鐞嗗湴鍧�鏁扮粍        
		stSrcData.au64PhyAddr[0] = stExtFrmInfo.stVFrame.u64PhyAddr[0];  //0        
		stSrcData.au64PhyAddr[1] = stExtFrmInfo.stVFrame.u64PhyAddr[1];  //        
		stSrcData.au64VirAddr[0] = stExtFrmInfo.stVFrame.u64VirAddr[0];  //0        
		stSrcData.au64VirAddr[1] = stExtFrmInfo.stVFrame.u64VirAddr[1];  //        
		stSrcData.au32Stride[0] = stExtFrmInfo.stVFrame.u32Stride[0];  //640        
		stSrcData.au32Stride[1] = stExtFrmInfo.stVFrame.u32Stride[1];  //640          
		stSrcData.u32Width   = stExtFrmInfo.stVFrame.u32Width; //640       
		stSrcData.u32Height  = stExtFrmInfo.stVFrame.u32Height; //640         

		stDstData.enType = IVE_IMAGE_TYPE_U8C3_PLANAR;        
		stDstData.u32Width   = stSrcData.u32Width;        
		stDstData.u32Height  = stSrcData.u32Height;        
		stDstData.au32Stride[0]  = stSrcData.au32Stride[0]; //640         
		stDstData.au32Stride[1]  = stSrcData.au32Stride[0]; 
		stDstData.au32Stride[2]  = stSrcData.au32Stride[0];
		u32Size = stDstData.au32Stride[0] * stDstData.u32Height;

		s32Ret = HI_MPI_SYS_MmzAlloc_Cached(&stDstData.au64PhyAddr[0], (HI_VOID**)&stDstData.au64VirAddr[0], "User", HI_NULL, u32Size*3);  //640*640脳3	
		if(HI_SUCCESS != s32Ret) 	   
		{					   
			HI_MPI_SYS_MmzFree(stSrcData.au64PhyAddr[0],(HI_VOID*)stSrcData.au64VirAddr[0]);				
			printf("HI_MPI_SYS_MmzAlloc_Cached Failed!");				  
			return NULL;		  
		} 		
		
		memset((HI_VOID*)stDstData.au64VirAddr[0], 0, stDstData.u32Height*stDstData.au32Stride[0]*3);		
		s32Ret = HI_MPI_SYS_MmzFlushCache(stDstData.au64PhyAddr[0], (HI_VOID*)stDstData.au64VirAddr[0], stDstData.u32Height*stDstData.au32Stride[0]*3);		  if(HI_SUCCESS != s32Ret)		  { 					   HI_MPI_SYS_MmzFree(stDstData.au64PhyAddr[0], (HI_VOID*)stDstData.au64VirAddr[0]);				 return NULL;		 }

		stDstData.au64PhyAddr[1] = stDstData.au64PhyAddr[0] + u32Size;
		stDstData.au64PhyAddr[2] = stDstData.au64PhyAddr[0] + u32Size * 2;
		stDstData.au64VirAddr[1] = stDstData.au64VirAddr[0] + u32Size;
		stDstData.au64VirAddr[2] = stDstData.au64VirAddr[0] + u32Size *2;

		s32Ret = HI_MPI_IVE_CSC(&hIveHandle,&stSrcData,&stDstData,&stCscCtrl,bInstant);

		if(HI_SUCCESS != s32Ret)
		{       
		 printf("s32Ret:%d \n", s32Ret);
		        HI_MPI_SYS_MmzFree(stDstData.au64PhyAddr[0], (HI_VOID*)stDstData.au64VirAddr[0]);
		        return NULL;
		}

		NNIE_IMG_RETINAFACE frame;
		frame.au64PhyAddr[0] = stDstData.au64PhyAddr[0];
		frame.au64VirAddr[0] = stDstData.au64VirAddr[0];

		NNIE_FD_RESULT result;
		memset(&result,0,sizeof(NNIE_FD_RESULT));
		
		NNIE_FACE_DETECTOR_GET_NEW(&frame,&result);		
		clock_gettime(CLOCK_MONOTONIC, &ts2);

		/*printf(" cost:%d ms\n"
		    , (ts2.tv_sec*1000 + ts2.tv_nsec/1000000) - (ts1.tv_sec*1000 + ts1.tv_nsec/1000000));*/

		RESULT_BAG result_bag;
		RESULT_BAG result_out;
		memset(&result_bag,0,sizeof(RESULT_BAG));
		memset(&result_out,0,sizeof(RESULT_BAG));
        
		for(i = 0; i < result.fd_num; i++)
		{
			printf("result rect: %d, %f, %f, %f, %f\n", i ,result.fd[i].ul.x,result.fd[i].ul.y,result.fd[i].lr.x,result.fd[i].lr.y);
			//key point
			printf("result lds:  %f, %f\n", result.fd[i].keyPoint[0].x,result.fd[i].keyPoint[0].y);
			printf("result lds:  %f, %f\n", result.fd[i].keyPoint[1].x,result.fd[i].keyPoint[1].y);
			printf("result lds:  %f, %f\n", result.fd[i].keyPoint[2].x,result.fd[i].keyPoint[2].y);
			printf("result lds:  %f, %f\n", result.fd[i].keyPoint[3].x,result.fd[i].keyPoint[3].y);
			printf("result lds:  %f, %f\n", result.fd[i].keyPoint[4].x,result.fd[i].keyPoint[4].y);
			/*触边处理*/
			result.fd[i].ul.x = result.fd[i].ul.x<0 ? 0:result.fd[i].ul.x;
			result.fd[i].ul.y = result.fd[i].ul.y<0 ? 0: result.fd[i].ul.y;
			result.fd[i].lr.x = result.fd[i].lr.x>640 ? 640: result.fd[i].lr.x;
			result.fd[i].lr.y = result.fd[i].lr.y>640 ? 640: result.fd[i].lr.y;

			
			result_bag.obj[i].centerx = (result.fd[i].ul.x + result.fd[i].lr.x) / 2;
			result_bag.obj[i].centery = (result.fd[i].ul.y + result.fd[i].lr.y) / 2;	
			result_bag.obj[i].heigth = result.fd[i].lr.y - result.fd[i].ul.y;
			result_bag.obj[i].width = result.fd[i].lr.x - result.fd[i].ul.x;
			
		}
		

		result_bag.obj_num = result.fd_num;
		if(result_bag.obj_num > 0)
		{
			framecnt ++;
		}
		/* 跟踪一定要每帧调用 */
		tracker_id(framecnt,&result_bag,&result_out);
		//printf("RESULT num %d\n", result_out.obj_num);
		
		if(result_bag.obj_num > 0)
		{
			char buf[sizeof(gsf_msg_t) + sizeof(NNIE_FD_RESULT)];
			gsf_msg_t *msg = (gsf_msg_t*)buf;

			memset(msg, 0, sizeof(*msg));
			msg->id = GSF_EV_SVP_FD;
			msg->ts = time(NULL)*1000;
			msg->sid = 0;
			msg->err = 0;
			msg->size = sizeof(gsf_svp_fds_t);

			gsf_svp_fds_t *fds = (gsf_svp_fds_t*)msg->data;
			fds->pts = stExtFrmInfo.stVFrame.u64PTS/1000;
			fds->w = stExtFrmInfo.stVFrame.u32Width;
			fds->h = stExtFrmInfo.stVFrame.u32Height;
			fds->cnt = result_out.obj_num;

			for(i=0;i<result_out.obj_num;i++)
			{
				fds->result[i].rect[0] = result_out.obj[i].centerx - (result_out.obj[i].width/2);
				fds->result[i].rect[1] = result_out.obj[i].centery - (result_out.obj[i].heigth/2);
				fds->result[i].rect[2] = result_out.obj[i].width;
				fds->result[i].rect[3] = result_out.obj[i].heigth;
				fds->result[i].id = result_out.obj[i].ID;
			}
			if(fds->cnt > 0)
			{
				nm_pub_send(svp_pub, (char*)msg, sizeof(*msg)+msg->size);
			}

			for(i = 0; i < result_out.obj_num; i++)
			{
			  printf("RESULT rect: %d, %d, %d, %d, %d\n", result_out.obj[i].ID ,result_out.obj[i].width,result_out.obj[i].heigth,result_out.obj[i].centerx,result_out.obj[i].centery);
			}
		}



	  //HI_MPI_SYS_Munmap(pVirAddr, u32Size);
		HI_MPI_SYS_MmzFree(stDstData.au64PhyAddr[0], (HI_VOID*)stDstData.au64VirAddr[0]);
		HI_MPI_VPSS_ReleaseChnFrame(s32VpssGrp, as32VpssChn[1], &stExtFrmInfo);

	}
}
