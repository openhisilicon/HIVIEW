#include "vpss_capture.h"


HI_S32 VpssCapture::VPSS_Restore(VPSS_GRP VpssGrp, VPSS_CHN VpssChn)
{
	HI_S32 s32Ret = HI_FAILURE;
	VPSS_CHN_ATTR_S stChnAttr;
	VPSS_EXT_CHN_ATTR_S stExtChnAttr;

	if (VB_INVALID_POOLID != stFrame.u32PoolId)
	{
		s32Ret = HI_MPI_VPSS_ReleaseChnFrame(VpssGrp, VpssChn, &stFrame);

		if (HI_SUCCESS != s32Ret)
		{
			printf("Release Chn Frame error!!!\n");
		}

		stFrame.u32PoolId = VB_INVALID_POOLID;
	}

	if (-1 != hHandle)
	{
		HI_MPI_VGS_CancelJob(hHandle);
		hHandle = -1;
	}

	if (HI_NULL != stMem.pVirAddr)
	{
		HI_MPI_SYS_Munmap((HI_VOID*)stMem.pVirAddr, u32BlkSize );
		stMem.u64PhyAddr = HI_NULL;
	}

	if (VB_INVALID_POOLID != stMem.hPool)
	{
		HI_MPI_VB_ReleaseBlock(stMem.hBlock);
		stMem.hPool = VB_INVALID_POOLID;
	}

	if (VB_INVALID_POOLID != hPool)
	{
		HI_MPI_VB_DestroyPool( hPool );
		hPool = VB_INVALID_POOLID;
	}

	if (HI_NULL != pUserPageAddr[0])
	{
		HI_MPI_SYS_Munmap(pUserPageAddr[0], u32Size);
		pUserPageAddr[0] = HI_NULL;
	}

	if (u32VpssDepthFlag)
	{
		if (VpssChn > 3)
		{
			s32Ret = HI_MPI_VPSS_GetExtChnAttr(VpssGrp, VpssChn, &stExtChnAttr);
		}
		else
		{
			s32Ret = HI_MPI_VPSS_GetChnAttr(VpssGrp, VpssChn, &stChnAttr);
		}

		if (s32Ret != HI_SUCCESS)
		{
			printf("get chn attr error!!!\n");
		}

		if (VpssChn > 3)
		{
			stExtChnAttr.u32Depth = u32OrigDepth;
			s32Ret = HI_MPI_VPSS_SetExtChnAttr(VpssGrp, VpssChn, &stExtChnAttr);
		}
		else
		{
			stChnAttr.u32Depth = u32OrigDepth;
			s32Ret = HI_MPI_VPSS_SetChnAttr(VpssGrp, VpssChn, &stChnAttr) ;
		}

		if (s32Ret != HI_SUCCESS)
		{
			printf("set depth error!!!\n");
		}

		u32VpssDepthFlag = 0;
	}
	return HI_SUCCESS;
}

void VpssCapture::VPSS_Chn_Dump_HandleSig(HI_S32 signo)
{
	if (u32SignalFlag)
	{
		exit(-1);
	}

	if (SIGINT == signo || SIGTERM == signo)
	{
		u32SignalFlag++;
		printf("\033[0;31mprogram termination abnormally!\033[0;39m\n");
		VPSS_Restore(VpssGrp, VpssChn);
    if(stDst.au64PhyAddr[0] && stDst.au64VirAddr[0])
    {
        printf("HI_MPI_SYS_MmzFree stDst.au64VirAddr: 0x%llx\n", stDst.au64VirAddr[0]);
        HI_MPI_SYS_MmzFree(stDst.au64PhyAddr[0], (HI_VOID *)stDst.au64VirAddr[0]);
        stDst.au64VirAddr[0] = 0;
    }
    u32SignalFlag--;
	}

	exit(-1);
}

int VpssCapture::YUV2Mat(VIDEO_FRAME_S* pVBuf, cv::Mat& img)
{
	HI_S32 ret;
	IVE_SRC_IMAGE_S stSrc;
	IVE_HANDLE IveHandle;
	IVE_CSC_CTRL_S stCscCtrl;
	HI_BOOL bInstant = HI_TRUE;
	unsigned char *pImage;

	stSrc.au64PhyAddr[0] = pVBuf->u64PhyAddr[0];
	stSrc.au64PhyAddr[1] = pVBuf->u64PhyAddr[1];
	stSrc.au64PhyAddr[2] = pVBuf->u64PhyAddr[2];
	stSrc.au64VirAddr[0] = pVBuf->u64VirAddr[0];
	stSrc.au64VirAddr[1] = pVBuf->u64VirAddr[1];
	stSrc.au64VirAddr[2] = pVBuf->u64VirAddr[2];
	stSrc.au32Stride[0]  = pVBuf->u32Stride[0];
	stSrc.au32Stride[1]  = pVBuf->u32Stride[1];
	stSrc.au32Stride[2]  = pVBuf->u32Stride[2];
	stSrc.u32Width  = pVBuf->u32Width;
	stSrc.u32Height = pVBuf->u32Height;

	switch (pVBuf->enPixelFormat)
	{
		case PIXEL_FORMAT_YVU_PLANAR_420:
			stSrc.enType = IVE_IMAGE_TYPE_YUV420P;
			break ;
		case PIXEL_FORMAT_YVU_SEMIPLANAR_420:
			stSrc.enType = IVE_IMAGE_TYPE_YUV420SP;
			break ;
		default:
			printf("unsupported PixelFormat yet, %d\n", pVBuf->enPixelFormat);
			break ;
	}

	stDst.au32Stride[0]  = pVBuf->u32Width;
	stDst.au32Stride[1]  = pVBuf->u32Width;
	stDst.au32Stride[2]  = pVBuf->u32Width;
	stDst.u32Width  = pVBuf->u32Width;
	stDst.u32Height = pVBuf->u32Height;
	stDst.enType = IVE_IMAGE_TYPE_U8C3_PACKAGE;

	if(img.cols != pVBuf->u32Width || img.rows != pVBuf->u32Height || !stDst.au64VirAddr[0])
	{
		ret = HI_MPI_SYS_MmzAlloc_Cached(&stDst.au64PhyAddr[0],
				(HI_VOID **)&stDst.au64VirAddr[0],
				NULL,HI_NULL,
				stDst.u32Height*stDst.u32Width*3);
		if (HI_FAILURE == ret)
		{
			printf("MmzAlloc_Cached failed!\n");
			return HI_FAILURE;
		}
		printf("cv_mat refto MmzAlloc_Cached OK!\n");
		dst_mat = cv::Mat(pVBuf->u32Height, pVBuf->u32Width, CV_8UC3, (void*)stDst.au64VirAddr[0]);
		img = cv::Mat(dst_mat);
	}

	stDst.au64PhyAddr[1] = stDst.au64PhyAddr[0] + stDst.au32Stride[0];
	stDst.au64PhyAddr[2] = stDst.au64PhyAddr[1] + stDst.au32Stride[1];
	stDst.au64VirAddr[1] = stDst.au64VirAddr[0] + stDst.au32Stride[0];
	stDst.au64VirAddr[2] = stDst.au64VirAddr[1] + stDst.au32Stride[1]; 

	stCscCtrl.enMode = IVE_CSC_MODE_PIC_BT601_YUV2RGB;
	ret = HI_MPI_IVE_CSC(&IveHandle, &stSrc, &stDst, &stCscCtrl, bInstant); 
	if (HI_FAILURE == ret)
	{
		printf("YUV Convert to RGB failed!\n");
		return HI_FAILURE;
	}

	pImage = (unsigned char *)stDst.au64VirAddr[0];
	if(NULL == pImage)
	{
		printf( "stDst.au64VirAddr[0] is null!\n");
		return HI_FAILURE;    
	}
	
	return 0;
}



IVE_DST_IMAGE_S VpssCapture::stDst;
VPSS_GRP VpssCapture::VpssGrp = 0;
VPSS_CHN VpssCapture::VpssChn = 0;
HI_U32 VpssCapture::u32OrigDepth = 0;

HI_S32 VpssCapture::s32MilliSec = -1;
HI_U32 VpssCapture::u32VpssDepthFlag = 0;
HI_U32 VpssCapture::u32SignalFlag = 0;
VIDEO_FRAME_INFO_S VpssCapture::stFrame;
HI_U32  VpssCapture::u32BlkSize = 0;
VGS_HANDLE VpssCapture::hHandle = -1;
DUMP_MEMBUF_S VpssCapture::stMem = {0};
VB_POOL VpssCapture::hPool  = VB_INVALID_POOLID;
HI_U32 VpssCapture::u32Size = 0;
HI_CHAR* VpssCapture::pUserPageAddr[2] = {HI_NULL, HI_NULL};
unsigned int VpssCapture::frame_id = 0;


int VpssCapture::init(int VpssGrp, int VpssChn)
{
	HI_CHAR szPixFrm[10];
	HI_U32 u32Depth = 2;
	HI_S32 s32Times = 10;
	VIDEO_FRAME_INFO_S stFrmInfo;
	VGS_TASK_ATTR_S stTask;
	HI_U32 u32LumaSize = 0;
	HI_U32 u32PicLStride = 0;
	HI_U32 u32PicCStride = 0;
	HI_U32 u32Width = 0;
	HI_U32 u32Height = 0;
	HI_S32 i = 0;
	HI_S32 s32Ret;
	VPSS_CHN_ATTR_S stChnAttr;
	VPSS_EXT_CHN_ATTR_S stExtChnAttr;
	HI_U32 u32BitWidth;
	VB_POOL_CONFIG_S stVbPoolCfg;

  // signal(SIGINT, VPSS_Chn_Dump_HandleSig);
  // signal(SIGTERM, VPSS_Chn_Dump_HandleSig);

	if (VpssChn > VPSS_CHN3)
	{
		s32Ret = HI_MPI_VPSS_GetExtChnAttr(VpssGrp, VpssChn, &stExtChnAttr);
		u32OrigDepth = stExtChnAttr.u32Depth;
	}
	else
	{
		s32Ret = HI_MPI_VPSS_GetChnAttr(VpssGrp, VpssChn, &stChnAttr);
		u32OrigDepth = stChnAttr.u32Depth;
	}

	if (s32Ret != HI_SUCCESS)
	{
		printf("get chn:%d attr error!!!\n", VpssChn);
		return -1;
	}

	if (VpssChn > VPSS_CHN3)
	{
		stExtChnAttr.u32Depth = u32Depth;
		s32Ret = HI_MPI_VPSS_SetExtChnAttr(VpssGrp, VpssChn, &stExtChnAttr);
	}
	else
	{
		stChnAttr.u32Depth = u32Depth;
		s32Ret = HI_MPI_VPSS_SetChnAttr(VpssGrp, VpssChn, &stChnAttr) ;
	}
	if (s32Ret != HI_SUCCESS)
	{
		printf("set depth error!!!\n");
		VPSS_Restore(VpssGrp, VpssChn);
		return -1;
	}
	u32VpssDepthFlag = 1;

	memset(&stFrame, 0, sizeof(stFrame));
	stFrame.u32PoolId = VB_INVALID_POOLID;
	while (HI_MPI_VPSS_GetChnFrame(VpssGrp, VpssChn, &stFrame, s32MilliSec) != HI_SUCCESS)
	{
		s32Times--;
		if (0 >= s32Times)
		{
			printf("get frame error for 10 times,now exit !!!\n");
			VPSS_Restore(VpssGrp, VpssChn);
			return -1;
		}
		usleep(40*1000);
	}

	if (VIDEO_FORMAT_LINEAR != stFrame.stVFrame.enVideoFormat)
	{
		printf("only support linear frame dump!\n");
		HI_MPI_VPSS_ReleaseChnFrame(VpssGrp, VpssChn, &stFrame);
		stFrame.u32PoolId = VB_INVALID_POOLID;
		return -1;
	}

	switch (stFrame.stVFrame.enPixelFormat)
	{
		case PIXEL_FORMAT_YVU_SEMIPLANAR_420:
		case PIXEL_FORMAT_YUV_SEMIPLANAR_420:
			snprintf(szPixFrm, 10, "P420");
			break;

		case PIXEL_FORMAT_YVU_SEMIPLANAR_422:
		case PIXEL_FORMAT_YUV_SEMIPLANAR_422:
			snprintf(szPixFrm, 10, "P422");
			break;

		default:
			snprintf(szPixFrm, 10, "P400");
			break;
	}
	
	this->VpssGrp = VpssGrp;
	this->VpssChn = VpssChn;
	return 0;
}

int VpssCapture::get_rgbframe(cv::Mat &frame, VIDEO_FRAME_S *pVBuf)
{
	if (HI_MPI_VPSS_GetChnFrame(VpssGrp, VpssChn, &stFrame, s32MilliSec) != HI_SUCCESS)
	{
		printf("Get frame fail \n");
		usleep(1000);
		return -1;
	}

	bool bSendToVgs = ((COMPRESS_MODE_NONE != stFrame.stVFrame.enCompressMode) || (VIDEO_FORMAT_LINEAR != stFrame.stVFrame.enVideoFormat));

	if (bSendToVgs){
		return -1;
	}

	if (DYNAMIC_RANGE_SDR8 == stFrame.stVFrame.enDynamicRange)
	{
		int ret = YUV2Mat(&stFrame.stVFrame, frame);	
	//	*pVBuf = stFrame.stVFrame;
		//static int i = 0;
		//std::string imgpath = "rgb-" + std::to_string(i) + ".jpg";
		//cv::imwrite(imgpath, frame);
		//i++;
		//memcpy(pVBuf, &stFrame.stVFrame, sizeof(stFrame.stVFrame));
		if(ret < 0) return -1;
	}
	//printf("Get frame %d!!\n", frame_id);
	/* release frame after using */
	HI_S32 s32Ret = HI_MPI_VPSS_ReleaseChnFrame(VpssGrp, VpssChn, &stFrame);

	if (HI_SUCCESS != s32Ret)
	{
		printf("Release frame error ,now exit !!!\n");
		VPSS_Restore(VpssGrp, VpssChn);
		return -1;
	}
	frame_id ++;

	return 0;
}

int VpssCapture::get_frame(cv::Mat &frame)
{
	if (HI_MPI_VPSS_GetChnFrame(VpssGrp, VpssChn, &stFrame, s32MilliSec) != HI_SUCCESS)
	{
		printf("Get frame fail \n");
		usleep(1000);
		return -1;
	}

	bool bSendToVgs = ((COMPRESS_MODE_NONE != stFrame.stVFrame.enCompressMode) || (VIDEO_FORMAT_LINEAR != stFrame.stVFrame.enVideoFormat));

	if (bSendToVgs){
		return -1;
	}

	if (DYNAMIC_RANGE_SDR8 == stFrame.stVFrame.enDynamicRange)
	{
		int ret = YUV2Mat(&stFrame.stVFrame, frame);	
		if(ret < 0) return -1;
	}
	//printf("Get frame %d!!\n", frame_id);
	/* release frame after using */
	HI_S32 s32Ret = HI_MPI_VPSS_ReleaseChnFrame(VpssGrp, VpssChn, &stFrame);

	if (HI_SUCCESS != s32Ret)
	{
		printf("Release frame error ,now exit !!!\n");
		VPSS_Restore(VpssGrp, VpssChn);
		return -1;
	}
	frame_id ++;

	return 0;
}

int VpssCapture::destroy()
{
	stFrame.u32PoolId = VB_INVALID_POOLID;
	VPSS_Restore(VpssGrp, VpssChn);
	HI_MPI_SYS_MmzFree(stDst.au64PhyAddr[0], (HI_VOID *)stDst.au64VirAddr[0]);
	stDst.au64VirAddr[0] = 0;
	return 0;
}
