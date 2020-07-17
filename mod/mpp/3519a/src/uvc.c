/*************************************************************************
    > File Name: uvc.c
    > Author:AIOST
    > Mail: liangm@aiostchina.com
    > Created Time: 2020年07月13日 星期一 16时50分27秒
 ************************************************************************/
#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* End of #ifdef __cplusplus */

#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <unistd.h>
#include <pthread.h>
#include <signal.h>
#include <sys/mman.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <asm/types.h>		/* for videodev2.h */
#include <linux/videodev2.h>

#include "sample_comm.h"


#define CLEAR(x) memset (&(x), 0, sizeof (x))
#define MAX_CAPTURE			10000

typedef enum {
	IO_METHOD_READ,
	IO_METHOD_MMAP,
	IO_METHOD_USERPTR,
} io_method;

struct buffer {
	void *start;
	size_t length;
};

static char *dev_name = "/dev/video0";
static char *output_name = NULL;
static io_method io = IO_METHOD_MMAP;
static int fd = -1;
static struct buffer *buffers = NULL;
static unsigned int n_buffers = 0;
static unsigned int capture_count = 100;
//static unsigned int capture_interval = 10;
static unsigned int capture_idx = 0;
static unsigned int capture_fmt = V4L2_PIX_FMT_YUYV;
//static unsigned int no_cap = 0;
static unsigned int save_raw_image = 0;

#define DEFAULT_PREVIEW_WIDTH		1280
#define DEFAULT_PREVIEW_HEIGHT		512
static int pw = DEFAULT_PREVIEW_WIDTH;
static int ph = DEFAULT_PREVIEW_HEIGHT;

//#define DEFAULT_STILL_WIDTH		1280
//#define DEFAULT_STILL_HEIGHT		960
//static int sw = DEFAULT_STILL_WIDTH;
//static int sh = DEFAULT_STILL_HEIGHT;
//struct buffer *still_buffers = NULL;
//static unsigned int still_n_buffers = 0;



static void errno_exit(const char *s)
{
	fprintf(stderr, "%s error %d, %s\n", s, errno, strerror(errno));

	exit(EXIT_FAILURE);
}

static int xioctl(int fd, int request, void *arg)
{
	int r;

	do
		r = ioctl(fd, request, arg);
	while (-1 == r && EINTR == errno);

	return r;
}

static void dump_raw_to_file(void *data, int len, const char *suffix)
{
	int fd;
	int ret;
	char name[128];
	char *fcc;

	fcc = (char *)&capture_fmt;
	sprintf (name, "raw-%d%s.%c%c%c%c",
			capture_idx, suffix,
			fcc[0], fcc[1], fcc[2], fcc[3]);
	fd = open(name, O_CREAT|O_TRUNC|O_RDWR, 0660);

	//printf("Dumped %d bytes\n", len);
	while(len > 0)
	{
		ret = write (fd, data, len);
		if (ret < 0)
			break;
		len -= ret;
	}

	close(fd);
}

static void process_image(const struct v4l2_buffer *buf)
{
	if (save_raw_image)
		dump_raw_to_file(buffers[buf->index].start,
						 buf->bytesused, "");
	fputc('.', stdout);
	fflush(stdout);
}

static int read_frame(int fdYUV)
{
	struct v4l2_buffer buf;
	unsigned int i;

	switch (io) {
	case IO_METHOD_READ:
		if (-1 == read(fd, buffers[0].start, buffers[0].length)) {
			switch (errno) {
			case EAGAIN:
				return 0;

			case EIO:
				/* Could ignore EIO, see spec. */

				/* fall through */

			default:
				errno_exit("read");
			}
		}

		//process_image(buffers[0]);

		break;

	case IO_METHOD_MMAP:
		CLEAR(buf);

		buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
		buf.memory = V4L2_MEMORY_MMAP;

		if (-1 == xioctl(fd, VIDIOC_DQBUF, &buf)) {
			switch (errno) {
			case EAGAIN:
				return 0;

			case EIO:
				/* Could ignore EIO, see spec. */

				/* fall through */

			default:
				errno_exit("VIDIOC_DQBUF");
			}
		}

		assert(buf.index < n_buffers);

//		process_image(&buf);
		write(fdYUV, buffers[buf.index].start, buf.bytesused);
		capture_idx++;

		if (-1 == xioctl(fd, VIDIOC_QBUF, &buf))
			errno_exit("VIDIOC_QBUF");

		break;

	case IO_METHOD_USERPTR:
		CLEAR(buf);

		buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
		buf.memory = V4L2_MEMORY_USERPTR;

		if (-1 == xioctl(fd, VIDIOC_DQBUF, &buf)) {
			switch (errno) {
			case EAGAIN:
				return 0;

			case EIO:
				/* Could ignore EIO, see spec. */

				/* fall through */

			default:
				errno_exit("VIDIOC_DQBUF");
			}
		}

		for (i = 0; i < n_buffers; ++i)
			if (buf.m.userptr == (unsigned long)buffers[i].start
			    && buf.length == buffers[i].length)
				break;

		assert(i < n_buffers);

		process_image(&buf);

		if (-1 == xioctl(fd, VIDIOC_QBUF, &buf))
			errno_exit("VIDIOC_QBUF");

		break;
	}

	return 0;
}

// YUYVYUYV   ==> YYYYUVUV
static int fill_one_frame(HI_CHAR* data)
{
	struct v4l2_buffer buf;
	unsigned int i,j,k;
	int width  = DEFAULT_PREVIEW_WIDTH/2; 
	int height = DEFAULT_PREVIEW_HEIGHT;	
	unsigned char buf_start[width*height*2];

	memset(buf_start,0,width*height*2);
	
	CLEAR(buf);

	buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	buf.memory = V4L2_MEMORY_MMAP;

	if (-1 == xioctl(fd, VIDIOC_DQBUF, &buf)) {
		switch (errno) {
		case EAGAIN:
			return 0;
		case EIO:
			/* Could ignore EIO, see spec. */
			/* fall through */
		default:
			errno_exit("VIDIOC_DQBUF");
		}
	}
	
	assert(buf.index < n_buffers);

	for(int i = 0; i<height; i++) {
		memcpy(buf_start+i*width*2,buffers[buf.index].start+i*width*4, width*2);
	}

	for(i=0,j=0,k=0; i<buf.bytesused/2; i+=4,j+=2,k+=1) {
		data[j] = buf_start[i+1];
		data[j+1] = buf_start[i+3];
		data[k+width*height] = 0x80;	
	}

	capture_idx++;

	if (-1 == xioctl(fd, VIDIOC_QBUF, &buf))
		errno_exit("VIDIOC_QBUF");
}

static void mainloop(void)
{
	unsigned int count = capture_count;

	int fdYUV;
	fdYUV = open(output_name, O_CREAT | O_RDWR, 0660);
	if(fd<0) {
		printf("open test.yuv failed\n");
		return;
	}
	printf("Start capture %d frames\n", count);

	while (count-- > 0) {
		read_frame(fdYUV);

	}
	close(fdYUV);
}

static void stop_capturing(void)
{
	enum v4l2_buf_type type;

	switch (io) {
	case IO_METHOD_READ:
		/* Nothing to do. */
		break;

	case IO_METHOD_MMAP:
	case IO_METHOD_USERPTR:
		type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

		if (-1 == xioctl(fd, VIDIOC_STREAMOFF, &type))
			errno_exit("VIDIOC_STREAMOFF");

		break;
	}
}

static void start_capturing(void)
{
	unsigned int i;
	enum v4l2_buf_type type;

	switch (io) {
	case IO_METHOD_READ:
		/* Nothing to do. */
		break;

	case IO_METHOD_MMAP:
		for (i = 0; i < n_buffers; ++i) {
			struct v4l2_buffer buf;

			CLEAR(buf);

			buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
			buf.memory = V4L2_MEMORY_MMAP;
			buf.index = i;

			if (-1 == xioctl(fd, VIDIOC_QBUF, &buf))
				errno_exit("VIDIOC_QBUF");
		}

		type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

		if (-1 == xioctl(fd, VIDIOC_STREAMON, &type))
			errno_exit("VIDIOC_STREAMON");

		break;

	case IO_METHOD_USERPTR:
		for (i = 0; i < n_buffers; ++i) {
			struct v4l2_buffer buf;

			CLEAR(buf);

			buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
			buf.memory = V4L2_MEMORY_USERPTR;
			buf.index = i;
			buf.m.userptr = (unsigned long)buffers[i].start;
			buf.length = buffers[i].length;

			if (-1 == xioctl(fd, VIDIOC_QBUF, &buf))
				errno_exit("VIDIOC_QBUF");
		}

		type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

		if (-1 == xioctl(fd, VIDIOC_STREAMON, &type))
			errno_exit("VIDIOC_STREAMON");

		break;
	}
}

static void uninit_device(void)
{
	unsigned int i;

	switch (io) {
	case IO_METHOD_READ:
		free(buffers[0].start);
		break;

	case IO_METHOD_MMAP:
		for (i = 0; i < n_buffers; ++i)
			if (-1 == munmap(buffers[i].start, buffers[i].length))
				errno_exit("munmap");
		break;

	case IO_METHOD_USERPTR:
		for (i = 0; i < n_buffers; ++i)
			free(buffers[i].start);
		break;
	}

	free(buffers);
}

static void init_read(unsigned int buffer_size)
{
	buffers = calloc(1, sizeof(*buffers));

	if (!buffers) {
		fprintf(stderr, "Out of memory\n");
		exit(EXIT_FAILURE);
	}

	buffers[0].length = buffer_size;
	buffers[0].start = malloc(buffer_size);

	if (!buffers[0].start) {
		fprintf(stderr, "Out of memory\n");
		exit(EXIT_FAILURE);
	}
}

static void init_mmap(void)
{
	struct v4l2_requestbuffers req;

	CLEAR(req);

	req.count = 4;
	req.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	req.memory = V4L2_MEMORY_MMAP;

	if (-1 == xioctl(fd, VIDIOC_REQBUFS, &req)) {
		if (EINVAL == errno) {
			fprintf(stderr, "%s does not support "
				"memory mapping\n", dev_name);
			exit(EXIT_FAILURE);
		} else {
			errno_exit("VIDIOC_REQBUFS");
		}
	}

	if (req.count < 2) {
		fprintf(stderr, "Insufficient buffer memory on %s\n", dev_name);
		exit(EXIT_FAILURE);
	}

	buffers = calloc(req.count, sizeof(*buffers));

	if (!buffers) {
		fprintf(stderr, "Out of memory\n");
		exit(EXIT_FAILURE);
	}

	for (n_buffers = 0; n_buffers < req.count; ++n_buffers) {
		struct v4l2_buffer buf;

		CLEAR(buf);

		buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
		buf.memory = V4L2_MEMORY_MMAP;
		buf.index = n_buffers;

		if (-1 == xioctl(fd, VIDIOC_QUERYBUF, &buf))
			errno_exit("VIDIOC_QUERYBUF");

		buffers[n_buffers].length = buf.length;
		buffers[n_buffers].start = mmap(NULL /* start anywhere */ ,
						buf.length,
						PROT_READ | PROT_WRITE
						/* required */ ,
						MAP_SHARED /* recommended */ ,
						fd, buf.m.offset);

#if 1
		printf("Preview buffer: %p, len: %d, raw: 0x%x\n",
				buffers[n_buffers].start,
				(int)buffers[n_buffers].length,
				buf.m.offset);
#endif

		if (MAP_FAILED == buffers[n_buffers].start)
			errno_exit("mmap");
	}
}

static void init_userp(unsigned int buffer_size)
{
	struct v4l2_requestbuffers req;
	unsigned int page_size;

	page_size = getpagesize();
	buffer_size = (buffer_size + page_size - 1) & ~(page_size - 1);

	CLEAR(req);

	req.count = 4;
	req.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	req.memory = V4L2_MEMORY_USERPTR;

	if (-1 == xioctl(fd, VIDIOC_REQBUFS, &req)) {
		if (EINVAL == errno) {
			fprintf(stderr, "%s does not support "
				"user pointer i/o\n", dev_name);
			exit(EXIT_FAILURE);
		} else {
			errno_exit("VIDIOC_REQBUFS");
		}
	}

	buffers = calloc(4, sizeof(*buffers));

	if (!buffers) {
		fprintf(stderr, "Out of memory\n");
		exit(EXIT_FAILURE);
	}

	for (n_buffers = 0; n_buffers < 4; ++n_buffers) {
		buffers[n_buffers].length = buffer_size;
		buffers[n_buffers].start = memalign( /* boundary */ page_size,
						    buffer_size);

		if (!buffers[n_buffers].start) {
			fprintf(stderr, "Out of memory\n");
			exit(EXIT_FAILURE);
		}
	}
}

static void init_device(void)
{
	struct v4l2_capability cap;
	struct v4l2_cropcap cropcap;
	struct v4l2_crop crop;
	struct v4l2_format fmt;
	//char *fcc;
	unsigned int min;

	if (-1 == xioctl(fd, VIDIOC_QUERYCAP, &cap)) {
		if (EINVAL == errno) {
			fprintf(stderr, "%s is no V4L2 device\n", dev_name);
			exit(EXIT_FAILURE);
		} else {
			errno_exit("VIDIOC_QUERYCAP");
		}
	}

	if (!(cap.capabilities & V4L2_CAP_VIDEO_CAPTURE)) {
		fprintf(stderr, "%s is no video capture device\n", dev_name);
		exit(EXIT_FAILURE);
	}

	switch (io) {
	case IO_METHOD_READ:
		if (!(cap.capabilities & V4L2_CAP_READWRITE)) {
			fprintf(stderr, "%s does not support read i/o\n",
				dev_name);
			exit(EXIT_FAILURE);
		}

		break;

	case IO_METHOD_MMAP:
	case IO_METHOD_USERPTR:
		printf("init_device ok!!!\n");
		if (!(cap.capabilities & V4L2_CAP_STREAMING)) {
			fprintf(stderr, "%s does not support streaming i/o\n",
				dev_name);
			exit(EXIT_FAILURE);
		}

		break;
	}

	/* Select video input, video standard and tune here. */

	CLEAR(cropcap);

	cropcap.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

	if (0 == xioctl(fd, VIDIOC_CROPCAP, &cropcap)) {
		crop.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
		crop.c = cropcap.defrect;	/* reset to default */

		if (-1 == xioctl(fd, VIDIOC_S_CROP, &crop)) {
			switch (errno) {
			case EINVAL:
				/* Cropping not supported. */
				break;
			default:
				/* Errors ignored. */
				break;
			}
		}
	} else {
		/* Errors ignored. */
	}

	CLEAR(fmt);

	fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	fmt.fmt.pix.width = pw;
	fmt.fmt.pix.height = ph;
	fmt.fmt.pix.pixelformat = capture_fmt;
	//fmt.fmt.pix.field = V4L2_FIELD_INTERLACED;
	fmt.fmt.pix.field = V4L2_FIELD_NONE;

	if (-1 == xioctl(fd, VIDIOC_S_FMT, &fmt))
		errno_exit("VIDIOC_S_FMT");

	/* Note VIDIOC_S_FMT may change width and height. */

	/* Buggy driver paranoia. */
	min = fmt.fmt.pix.width * 2;
	if (fmt.fmt.pix.bytesperline < min)
		fmt.fmt.pix.bytesperline = min;
	min = fmt.fmt.pix.bytesperline * fmt.fmt.pix.height;
	if (fmt.fmt.pix.sizeimage < min)
		fmt.fmt.pix.sizeimage = min;

	switch (io) {
	case IO_METHOD_READ:
		init_read(fmt.fmt.pix.sizeimage);
		break;

	case IO_METHOD_MMAP:
		init_mmap();
		break;

	case IO_METHOD_USERPTR:
		init_userp(fmt.fmt.pix.sizeimage);
		break;
	}
}

static void close_device(void)
{
	if (-1 == close(fd))
		errno_exit("close");

	fd = -1;
}

static void open_device(void)
{
	struct stat st;

	if (-1 == stat(dev_name, &st)) {
		fprintf(stderr, "Cannot identify '%s': %d, %s\n",
			dev_name, errno, strerror(errno));
		exit(EXIT_FAILURE);
	}

	if (!S_ISCHR(st.st_mode)) {
		fprintf(stderr, "%s is no device\n", dev_name);
		exit(EXIT_FAILURE);
	}
	/* Use blocked IO */
	fd = open(dev_name, O_RDWR, 0);

	if (-1 == fd) {
		fprintf(stderr, "Cannot open '%s': %d, %s\n",
			dev_name, errno, strerror(errno));
		exit(EXIT_FAILURE);
	}

	printf("open_device ok!!!!\n");
}


HI_VOID* SEND_YUV_TO_VENC(HI_VOID* pArgs)
{
	HI_U64  phyAddr;
	HI_U8	*pVirAddr;
    VENC_CHN VencChn3 = 3;
	VENC_CHN VencChn4 = 4;
	//VENC_CHN VencChn5 = 5;
	HI_S32 s32Ret = HI_SUCCESS;
    VB_POOL hPool  = VB_INVALID_POOLID;
	VB_BLK  vbBlk  = VB_INVALID_HANDLE;
	HI_U32 u32Cnt  = 0;
    HI_U32 u32OutWidth  = DEFAULT_PREVIEW_WIDTH/2;
	HI_U32 u32OutHeight = DEFAULT_PREVIEW_HEIGHT;
	HI_U32 u32OutStride = DEFAULT_PREVIEW_WIDTH/2;
    HI_U32 u32BlkSize   = u32OutWidth*u32OutHeight*3/2;
	HI_U64 u64pts = 0;
	
	open_device();
	init_device();
	start_capturing();

    printf("SAMPLE_COMM_VENC_StartGetStream!\n");
    while(1)
    { 
		VIDEO_FRAME_INFO_S stFrmInfo;
        u32Cnt ++;
        
		while((vbBlk = HI_MPI_VB_GetBlock(hPool, u32BlkSize,NULL)) == VB_INVALID_HANDLE)
        {
			;
        }
  
        phyAddr = HI_MPI_VB_Handle2PhysAddr(vbBlk);
        pVirAddr = (HI_U8 *) HI_MPI_SYS_Mmap(phyAddr, u32BlkSize);
        if(pVirAddr == NULL)
        {
            SAMPLE_PRT("Mem dev may not open\n");
        }
        memset(&stFrmInfo.stVFrame, 0, sizeof(VIDEO_FRAME_S));
        stFrmInfo.stVFrame.u64PhyAddr[0] = phyAddr;
        stFrmInfo.stVFrame.u64PhyAddr[1] = stFrmInfo.stVFrame.u64PhyAddr[0] + u32OutStride * u32OutHeight;
        stFrmInfo.stVFrame.u64PhyAddr[2] = stFrmInfo.stVFrame.u64PhyAddr[1] + u32OutStride * u32OutHeight;
        stFrmInfo.stVFrame.u64VirAddr[0] = (HI_U64)(HI_UL)pVirAddr;
        stFrmInfo.stVFrame.u64VirAddr[1] = stFrmInfo.stVFrame.u64VirAddr[0] + u32OutStride * u32OutHeight;
        stFrmInfo.stVFrame.u64VirAddr[2] = stFrmInfo.stVFrame.u64VirAddr[1] + u32OutStride * u32OutHeight;
        stFrmInfo.stVFrame.u32Width      = u32OutWidth;
        stFrmInfo.stVFrame.u32Height     = u32OutHeight;
        stFrmInfo.stVFrame.u32Stride[0]  = u32OutStride;  
        stFrmInfo.stVFrame.u32Stride[1]  = u32OutStride; 
        stFrmInfo.stVFrame.u32Stride[2]  = u32OutStride; 
        stFrmInfo.stVFrame.u32TimeRef    = (u32Cnt * 2);
		stFrmInfo.stVFrame.u64PTS        = u32Cnt * 40;//u64pts;
        stFrmInfo.stVFrame.enPixelFormat = SAMPLE_PIXEL_FORMAT;
        stFrmInfo.stVFrame.enField = VIDEO_FIELD_FRAME;
        stFrmInfo.u32PoolId = HI_MPI_VB_Handle2PoolId(vbBlk);
		
		fill_one_frame(pVirAddr);
		
		s32Ret = HI_MPI_VENC_SendFrame(VencChn3, &stFrmInfo, 0);
		if(s32Ret != HI_SUCCESS) {
			printf("HI_MPI_VENC_SendFrame0 = %X\n",s32Ret);
		}
		s32Ret = HI_MPI_VENC_SendFrame(VencChn4, &stFrmInfo, 0);
		if(s32Ret != HI_SUCCESS) {
			printf("HI_MPI_VENC_SendFrame1 = %X\n",s32Ret);
		}
		//s32Ret = HI_MPI_VENC_SendFrame(VencChn2, &stFrmInfo, 0);
		//if(s32Ret != HI_SUCCESS) {
		//	printf("HI_MPI_VENC_SendFrame2 = %X\n",s32Ret);
		//}

        HI_MPI_SYS_Munmap( pVirAddr, u32BlkSize );
        HI_MPI_VB_ReleaseBlock(vbBlk);
    }
}

HI_VOID SAMPLE_SEND_YUV_TO_VENC(HI_VOID)
{
	pthread_t uvc_Thread = 0;
	pthread_create(&uvc_Thread, 0, SEND_YUV_TO_VENC, NULL);
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */
