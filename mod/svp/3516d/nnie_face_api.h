#ifndef __NNIE_FACE_H__
#define __NNIE_FACE_H__


#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */

#define NNIE_FACE_NUM_MAX  16
typedef struct nnie_rect
{
	float x;
	float y;
}nnie_rect;

typedef struct NNIE_FD_INFO
{
	nnie_rect ul;	//face up left
	nnie_rect lr;	//face low right
	nnie_rect keyPoint[5]; // key point	
}NNIE_FD_INFO;

typedef struct NNIE_FD_RESULT
{
	unsigned int fd_num;
	NNIE_FD_INFO fd[NNIE_FACE_NUM_MAX];	
}NNIE_FD_RESULT;

typedef struct NNIE_IMG_RETINAFACE
{
    unsigned long long  au64PhyAddr[3]; /* RW;The physical address of the image */
	unsigned long long  au64VirAddr[3]; /* RW;The virtual address of the image */
	unsigned int  au32Stride[3];  /* RW;The stride of the image */
	unsigned int  u32Width;       /* RW;The width of the image */    
	unsigned int  u32Height;      /* RW;The height of the image */
    unsigned int  enType; /* RW;The type of the image */
}NNIE_IMG_RETINAFACE;

/******************************************************************************
* function : Retinaface Detector func
******************************************************************************/

void NNIE_FACE_DETECTOR_INIT(char *pcModelName, float threshold, int isLog);
void NNIE_FACE_DETECTOR_GET(char *pcSrcFile);
void NNIE_FACE_DETECTOR_GET_NEW(NNIE_IMG_RETINAFACE *Frame,NNIE_FD_RESULT *result);


void NNIE_FACE_DETECTOR_RELEASE(void);

/******************************************************************************
* function : Face Recognition func
******************************************************************************/
void NNIE_FACE_EXTRACTOR_INIT(char *pcModelName);
void NNIE_FACE_NNIE_EXTRACTOR_GET(char *pcSrcFile, float *feature_buff);
void NNIE_FACE_EXTRACTOR_RELEASE(void);

/******************************************************************************
* function : Face Pose func
******************************************************************************/
void NNIE_FACE_PFPLD_INIT(char *pcModelName);
void NNIE_FACE_PFPLD_GET(char *pcSrcFile, float *landmarks_buff, float *angles_buff);
void NNIE_FACE_PFPLD_RELEASE(void);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */


#endif /* __NNIE_FACE_H__ */
