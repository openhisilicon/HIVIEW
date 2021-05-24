#include <stdlib.h>
#include<stdio.h>
#include<string.h>
#include <sys/timeb.h>
#include <sys/time.h>
#include <time.h>
#include <unistd.h>

#include "aface.h"

#include "sample_comm_ive.h"
#include "svp.h"
#include "db_api.h"

extern void* svp_pub;

static pthread_t s_hMdThread = 0;
static HI_BOOL s_bStopSignal = HI_FALSE;
static void* pHand = NULL;
static void* tracker_task(void* p);

int person_start(char *home_path)
{
  int VpssGrp = 0;
  int VpssChn = 1;
  char ModelPath[256];
  sprintf(ModelPath, "%s/model", home_path);
  printf("ModelPath:[%s]\n", ModelPath);

	struct FuncEnable funcEnable = {0};
	funcEnable.face_detect = true;
	funcEnable.face_match = true;
	funcEnable.age = true;
	funcEnable.gender = true;
	
	pHand = aface_init(ModelPath, &funcEnable);
	if(pHand == NULL)
	{
		printf("tracker initialize falied!\n");
		return -1;
	}
  
  s_bStopSignal = HI_FALSE;
  return pthread_create(&s_hMdThread, NULL, tracker_task, NULL);
}

int person_stop()
{
  if(s_hMdThread)
  {
    s_bStopSignal = HI_TRUE;
    pthread_join(s_hMdThread, NULL);
    s_hMdThread = 0;
  }
}

static pub_send(gsf_svp_yolos_t *boxs)
{
  int i;
  char buf[sizeof(gsf_msg_t) + sizeof(gsf_svp_yolos_t)];
  gsf_msg_t *msg = (gsf_msg_t*)buf;
  
  memset(msg, 0, sizeof(*msg));
  msg->id = GSF_EV_SVP_YOLO;
  msg->ts = time(NULL)*1000;
  msg->sid = 0;
  msg->err = 0;
  msg->size = sizeof(gsf_svp_yolos_t);
  
  gsf_svp_yolos_t *yolos = (gsf_svp_yolos_t*)msg->data;
   
  *yolos = *boxs;
  nm_pub_send(svp_pub, (char*)msg, sizeof(*msg)+msg->size);
}


#include "bmp_utils.h"

static IVE_SRC_IMAGE_S stDst;
static HI_U64 __au64PhyAddr;
static HI_U64 __au64VirAddr;
static int yuv2bmp(VIDEO_FRAME_S* pVBuf, char *buffer, int x, int y, int w, int h)
{
	HI_S32 ret;
	IVE_SRC_IMAGE_S stSrc;
	IVE_HANDLE IveHandle;
	IVE_CSC_CTRL_S stCscCtrl;
	HI_BOOL bInstant = HI_TRUE;
	
	if(w && h)
	{
  	x = ALIGN((x-200>0)?x-200:0, 16);
  	y = ALIGN((y-200>0)?y-200:0, 16);
  	w = ALIGN((x+w+256>1920)?1920-x:w+256, 16);
  	h = ALIGN((y+h+256>1080)?1080-y:h+256, 16);
	}
	
	stSrc.au64PhyAddr[0] = pVBuf->u64PhyAddr[0] + x + y*pVBuf->u32Stride[0];
	stSrc.au64PhyAddr[1] = pVBuf->u64PhyAddr[1] + x + y/2*pVBuf->u32Stride[1];
	stSrc.au64PhyAddr[2] = pVBuf->u64PhyAddr[2] + x + y/2*pVBuf->u32Stride[2];
	
	stSrc.au64VirAddr[0] = pVBuf->u64VirAddr[0] + x + y*pVBuf->u32Stride[0];
	stSrc.au64VirAddr[1] = pVBuf->u64VirAddr[1] + x + y/2*pVBuf->u32Stride[1];
	stSrc.au64VirAddr[2] = pVBuf->u64VirAddr[2] + x + y/2*pVBuf->u32Stride[2];
	
	stSrc.au32Stride[0]  = pVBuf->u32Stride[0];
	stSrc.au32Stride[1]  = pVBuf->u32Stride[1];
	stSrc.au32Stride[2]  = pVBuf->u32Stride[2];
	stSrc.u32Width  = w?w:pVBuf->u32Width;
	stSrc.u32Height = h?h:pVBuf->u32Height;
	
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
	
  if(!__au64VirAddr)//if(stDst.u32Width != pVBuf->u32Width || stDst.u32Height != pVBuf->u32Height)
	{
	  if(__au64VirAddr)
	  {
  	  HI_MPI_SYS_MmzFree(__au64PhyAddr, (HI_VOID *)__au64VirAddr);
  	  __au64VirAddr = 0;
	  }
	  
  	ret = HI_MPI_SYS_MmzAlloc_Cached(&__au64PhyAddr,
    			(HI_VOID **)&__au64VirAddr,
    			NULL,HI_NULL,
    			//pVBuf->u32Height*pVBuf->u32Width*3 + 64);
          1920*1080*3 + 64);
  	if (HI_FAILURE == ret)
  	{
  		//printf("MmzAlloc_Cached failed!, size:%d\n", pVBuf->u32Height*pVBuf->u32Width*3);
  		printf("MmzAlloc_Cached failed!, size:%d\n", 1920*1080*3);
  		return HI_FAILURE;
  	}
  	printf("MmzAlloc_Cached OK!\n");
  }
  
	stDst.au32Stride[0]  = stSrc.u32Width;//pVBuf->u32Width;
	stDst.au32Stride[1]  = stSrc.u32Width;//pVBuf->u32Width;
	stDst.au32Stride[2]  = stSrc.u32Width;//pVBuf->u32Width;
	stDst.u32Width  = stSrc.u32Width;//pVBuf->u32Width;
	stDst.u32Height = stSrc.u32Height;//pVBuf->u32Height;
	
	stDst.enType    = IVE_IMAGE_TYPE_U8C3_PACKAGE;
	
	stDst.au64PhyAddr[0] = __au64PhyAddr + 64;
	stDst.au64PhyAddr[1] = stDst.au64PhyAddr[0] + stDst.au32Stride[0];
	stDst.au64PhyAddr[2] = stDst.au64PhyAddr[1] + stDst.au32Stride[1];
	
	stDst.au64VirAddr[0] = __au64VirAddr + 64;
	stDst.au64VirAddr[1] = stDst.au64VirAddr[0] + stDst.au32Stride[0];
	stDst.au64VirAddr[2] = stDst.au64VirAddr[1] + stDst.au32Stride[1]; 

	stCscCtrl.enMode = IVE_CSC_MODE_PIC_BT601_YUV2RGB;
	ret = HI_MPI_IVE_CSC(&IveHandle, &stSrc, &stDst, &stCscCtrl, bInstant);
	if (HI_FAILURE == ret)
	{
		printf("YUV Convert to RGB failed!\n");
		return HI_FAILURE;
	}

  write_bmp_file2(buffer, (unsigned char*)__au64VirAddr, stDst.u32Width, stDst.u32Height);

  return 0;
}

typedef struct {
  char name[32];
  float features[512];
}feature_item_t;

typedef struct {
  int cnt;
  int size;
  feature_item_t item[0];
}feature_items_t;

static feature_items_t *feature_items;

static int face_load_cb(db_row_t *row, void *_u)
{
  if(1)
  {
    db_row_t *__row = row;
    printf("row->face[uuid:%llu, name:%s, filepath:%s, features(0x%02x%02x%02x%02x)]\n"
          , __row->face.uuid, __row->face.name, __row->face.filepath
          , __row->face.features[0], __row->face.features[1], __row->face.features[2], __row->face.features[3]);
  }
  // add to mem;
  if(feature_items->cnt < feature_items->size)
  {
    strncpy(feature_items->item[feature_items->cnt].name, row->face.name, sizeof(feature_items->item[0].name)-1);
    feature_items->item[feature_items->cnt].name[sizeof(feature_items->item[0].name)-1] = '\0';
    memcpy(feature_items->item[feature_items->cnt].features, row->face.features, sizeof(feature_items->item[0].features));
    feature_items->cnt++;
  }
  return 0;
}

static db_hdl_t* db_hdl;

int person_face_load(char *db_file)
{
  if(!feature_items)
  {
    feature_items = malloc(sizeof(feature_items_t)+sizeof(feature_item_t)*512);
    feature_items->size = 512;
    feature_items->cnt  = 0;
  }
  
  db_hdl = db_open(db_file);
  if(db_hdl == NULL)
    return -1;
    
  int ret = db_select(db_hdl, DB_FORM_FACE, 0, 0xffffffff, face_load_cb, NULL);
  return ret;
}

int person_face_add(char* filename)
{
  int ret = 0;
  db_row_t row;
  row.type = DB_FORM_FACE;
  
  if(db_hdl == NULL)
    return -1;
  
  char _filename[256] = {0};
  strncpy(_filename, filename, sizeof(_filename)-1);
  char *face_info = basename(_filename);
  
  char uuid[128] = {0};
  char name[256] = {0};

  // /tmp/uuid.name.jpg
  sscanf(face_info, "%[^.].%[^.].", uuid, name);

  // extract_features
  float features[512];
  int gender = 0, age = 0, expression = 0;
  ret = aface_features(pHand, filename, features, &gender, &age, &expression);
  if(ret < 0)
  {
    return ret;
  }
 
  row.face.uuid = strtoll(uuid, NULL, 10);
  row.face.date = time(NULL);
  row.face.name = name;
  row.face.age  = age;
  row.face.gender = gender;
  row.face.filepath = filename; // string;
  row.face.features = features; // blob_size: sizeof(float)*512;
  if(1)
  {
    db_row_t *__row = &row;
    printf("row->face[uuid:%llu, name:%s, filepath:%s, features(0x%02x%02x%02x%02x)]\n"
          , __row->face.uuid, __row->face.name, __row->face.filepath
          , __row->face.features[0], __row->face.features[1], __row->face.features[2], __row->face.features[3]);
  }
  ret = db_instet(db_hdl, &row);
  if(ret < 0)
  {
    ret = db_update(db_hdl, &row);
    if(ret < 0)
      return ret;
  }
  
  //add to mem;
  if(feature_items->cnt < feature_items->size)
  {
    strncpy(feature_items->item[feature_items->cnt].name, row.face.name, sizeof(feature_items->item[0].name)-1);
    feature_items->item[feature_items->cnt].name[sizeof(feature_items->item[0].name)-1] = '\0';
    memcpy(feature_items->item[feature_items->cnt].features, row.face.features, sizeof(feature_items->item[0].features));
    feature_items->cnt++;
  }
  
  return ret;
}

static void* tracker_task(void* p)
{
  HI_S32 s32Ret, i = 0;
  HI_S32 VpssGrp = 0;
  HI_S32 VpssChn = 1;
  HI_S32 s32MilliSec = 1000;
  HI_U32 u32Size = 0;
  VIDEO_FRAME_INFO_S stFrame;
  
  struct timespec ts1, ts2;
  clock_gettime(CLOCK_MONOTONIC, &ts1);
  
  int gender = 0, age = 0, expression = 0;
  
  person_face_load("/app/face/face.db");
  
  while (HI_FALSE == s_bStopSignal)
  {
    //usleep(33*1000);
    
    s32Ret = HI_MPI_VPSS_GetChnFrame(VpssGrp, VpssChn, &stFrame, s32MilliSec);
    if(HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("Error(%#x),HI_MPI_VPSS_GetChnFrame failed, VPSS_GRP(%d), VPSS_CHN(%d)!\n",
            s32Ret,VpssGrp,VpssChn);
        continue;
    }
    
    clock_gettime(CLOCK_MONOTONIC, &ts1);
    
    int minsize = 40;
		struct FaceInfo faceBoxs = {0};
		int face_count = aface_detect(pHand, &stFrame.stVFrame, minsize, &faceBoxs);
		//printf("get face count is : %d.\n",face_count);
		if(face_count < 0)
		{
			printf("tracker detect failed!\n");
			HI_MPI_VPSS_ReleaseChnFrame(VpssGrp, VpssChn, &stFrame);
			break;
		}
		
		clock_gettime(CLOCK_MONOTONIC, &ts2);
		//printf("detect cost: %d ms\n", (ts2.tv_sec*1000 + ts2.tv_nsec/1000000) - (ts1.tv_sec*1000 + ts1.tv_nsec/1000000));
		ts1 = ts2;
		
		
		#define WRITE_BMP_FILE

    //
  	{
  	  #ifdef WRITE_BMP_FILE
  		s32Ret = yuv2bmp(&stFrame.stVFrame, "/tmp/face.bmp", 0, 0, 0, 0);
  		#else
  		s32Ret = yuv2bmp(&stFrame.stVFrame, NULL, 0, 0, 0, 0);
  		#endif
      clock_gettime(CLOCK_MONOTONIC, &ts2);
  		//printf("yuv2bmp cost: %d ms\n", (ts2.tv_sec*1000 + ts2.tv_nsec/1000000) - (ts1.tv_sec*1000 + ts1.tv_nsec/1000000));
  		ts1 = ts2;
  	}
  	HI_MPI_VPSS_ReleaseChnFrame(VpssGrp, VpssChn, &stFrame);
		
		gsf_svp_yolos_t yolos = {0};
    yolos.pts = 0;
    yolos.w = stDst.u32Width;
    yolos.h = stDst.u32Height;

		float x = 0, y = 0, w = 0, h = 0;
		for(size_t iFace = 0; iFace < face_count; iFace ++)
		{
			x = faceBoxs.x[iFace];
			y = faceBoxs.y[iFace];
			w = faceBoxs.w[iFace];
			h = faceBoxs.h[iFace];
			int confidence = faceBoxs.confidence[iFace];
			//printf("face %d,confidence %d, bbox:[%.2f,%.2f,%.2f,%.2f].\n",iFace,confidence, x, y, w, h);

  		{
    		float features[512] = {0};
    		#ifdef WRITE_BMP_FILE
    		s32Ret = aface_features_by_bbox(pHand, "/tmp/face.bmp", x, y, w, h, features, &gender, &age, &expression);
    		//s32Ret = aface_features(pHand, "/tmp/face.bmp", features, &gender, &age, &expression);
    		#else
    		s32Ret = aface_features_by_bbox_buffer(pHand, __au64VirAddr, stDst.u32Width*stDst.u32Height*3 + 64, x, y, w, h, features, &gender, &age, &expression);
        //s32Ret =  aface_features_by_buffer(pHand, __au64VirAddr, stDst.u32Width*stDst.u32Height*3 + 64, features, &gender, &age, &expression);
        #endif
        clock_gettime(CLOCK_MONOTONIC, &ts2);
        //printf("extract cost: %d ms\n", (ts2.tv_sec*1000 + ts2.tv_nsec/1000000) - (ts1.tv_sec*1000 + ts1.tv_nsec/1000000));
    		ts1 = ts2;
    		
        yolos.box[iFace].rect[0] = x;
        yolos.box[iFace].rect[1] = y;
        yolos.box[iFace].rect[2] = w;
        yolos.box[iFace].rect[3] = h;
    		yolos.cnt++;
    		
    		for(i = 0; i < feature_items->cnt; i++)
    		{
    		  float distance = aface_feature_dist(pHand, features, feature_items->item[i].features);
    		  if (distance > 0.8)
    		  {
    		    printf("iFace:%d, features face s32Ret:%d, name:%s, distance: %.2f\n"
    		          , iFace, s32Ret, feature_items->item[i].name, distance);
    		    strncpy(yolos.box[iFace].label, feature_items->item[i].name, sizeof(yolos.box[iFace].label)-1);
    		    break;
    		  }
    		}
    		
        clock_gettime(CLOCK_MONOTONIC, &ts2);
        //printf("distance cost: %d ms\n", (ts2.tv_sec*1000 + ts2.tv_nsec/1000000) - (ts1.tv_sec*1000 + ts1.tv_nsec/1000000));
    		ts1 = ts2;
        
  		}
		}
		pub_send(&yolos);
  }
  
  gsf_svp_yolos_t yolos = {0};
  pub_send(&yolos);
  
	aface_destroy(pHand);
	if(__au64VirAddr)
	{
  	HI_MPI_SYS_MmzFree(__au64PhyAddr, (HI_VOID *)__au64VirAddr);
  	__au64VirAddr = 0;
  }
  return NULL;
}
