/******************************************************************************

  Copyright (C), 2001-2011, Hisilicon Tech. Co., Ltd.

 ******************************************************************************
  File Name     : aenc_aac_adp.c
  Version       : Initial Draft
  Author        : Hisilicon multimedia software group
  Created       : 2011/02/26
  Description   : 
  History       :
  1.Date        : 2011/02/26
    Author      : n00168968
    Modification: Created file

******************************************************************************/


#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include "audio_aac_adp.h"

//#define DUMP_AACENC
#ifdef DUMP_AACENC
FILE *pfdin_ENC = NULL;
FILE *pfdout_ENC = NULL;
#endif

//#define DUMP_AACDEC
#ifdef DUMP_AACDEC
FILE *pfdin_DEC = NULL;
FILE *pfdout_DEC = NULL;
FILE *pfdout_DEC_L = NULL;
#endif

int cnt_aenc = 100000;
int cnt_adec = 100000;

static HI_S32 AencCheckAACAttr(const AENC_ATTR_AAC_S *pstAACAttr)
{  
    if (pstAACAttr->enBitWidth != AUDIO_BIT_WIDTH_16)
    {
        printf("[Func]:%s [Line]:%d [Info]:%s\n", 
            __FUNCTION__, __LINE__, "invalid bitwidth for AAC encoder");
        return HI_ERR_AENC_ILLEGAL_PARAM;
    }
    
    if (pstAACAttr->enSoundMode >= AUDIO_SOUND_MODE_BUTT)
    {
        printf("[Func]:%s [Line]:%d [Info]:%s\n", 
            __FUNCTION__, __LINE__, "invalid sound mode for AAC encoder");
        return HI_ERR_AENC_ILLEGAL_PARAM;
    }

	if ((pstAACAttr->enAACType == AAC_TYPE_EAACPLUS) && (pstAACAttr->enSoundMode != AUDIO_SOUND_MODE_STEREO))
	{
		printf("[Func]:%s [Line]:%d [Info]:%s\n", 
            __FUNCTION__, __LINE__, "invalid sound mode for AAC encoder");
        return HI_ERR_AENC_ILLEGAL_PARAM;
	}

	if (pstAACAttr->enTransType == AAC_TRANS_TYPE_ADTS)
	{
		if ((pstAACAttr->enAACType == AAC_TYPE_AACLD)
			|| (pstAACAttr->enAACType == AAC_TYPE_AACELD))
		{
			printf("[Func]:%s [Line]:%d [Info]:%s\n", 
            	__FUNCTION__, __LINE__, "AACLD or AACELD not support AAC_TRANS_TYPE_ADTS");
        	return HI_ERR_AENC_ILLEGAL_PARAM;
		}
	}
   
    return HI_SUCCESS;
}

static HI_S32 AdecCheckAACAttr(const ADEC_ATTR_AAC_S *pstAACAttr)
{  
    if (pstAACAttr->enTransType >= AAC_TRANS_TYPE_BUTT)
    {
        printf("[Func]:%s [Line]:%d [Info]:%s\n", 
            	__FUNCTION__, __LINE__, "invalid enTransType for AAC decoder");
        	return HI_ERR_ADEC_ILLEGAL_PARAM;
    }
    
    return HI_SUCCESS;
}


static HI_S32 AencCheckAACLCConfig(AACENC_CONFIG *pconfig)
{
	HI_S32 s32MinBitRate = 0;
    HI_S32 s32MaxBitRate = 0;
    HI_S32 s32RecommemdRate = 0;

	if(pconfig->coderFormat == AACLC)
    {
        if(pconfig->nChannelsOut != pconfig->nChannelsIn) 
        {
    	    printf("AACLC nChannelsOut(%d) in not equal to nChannelsIn(%d)\n", pconfig->nChannelsOut, pconfig->nChannelsIn);
    	    return HI_ERR_AENC_ILLEGAL_PARAM;
        }
    	
        if(pconfig->sampleRate == 32000)
        {
            s32MinBitRate = (pconfig->nChannelsIn == 1) ? 32000 : 32000;
            s32MaxBitRate = (pconfig->nChannelsIn == 1) ? 192000 : 320000;
            s32RecommemdRate = (pconfig->nChannelsIn == 1) ? 48000 : 128000;
    	    if(pconfig->bitRate < s32MinBitRate ||  pconfig->bitRate > s32MaxBitRate)
    	    {
    		    printf("AACLC 32000 Hz bitRate(%d) should be %d ~ %d, recommed %d\n",pconfig->bitRate,s32MinBitRate,s32MaxBitRate,s32RecommemdRate);
    		    return HI_ERR_AENC_ILLEGAL_PARAM;
    	    }
        }
        else if(pconfig->sampleRate == 44100)
        {
    	    s32MinBitRate = (pconfig->nChannelsIn == 1) ? 48000 : 48000;
            s32MaxBitRate = (pconfig->nChannelsIn == 1) ? 265000 : 320000;
            s32RecommemdRate = (pconfig->nChannelsIn == 1) ? 64000 : 128000;
    	    if(pconfig->bitRate < s32MinBitRate ||  pconfig->bitRate > s32MaxBitRate)
    	    {
    		    printf("AACLC 44100 Hz bitRate(%d) should be %d ~ %d, recommed %d\n",pconfig->bitRate,s32MinBitRate,s32MaxBitRate,s32RecommemdRate);
    		    return HI_ERR_AENC_ILLEGAL_PARAM;
    	    }
        }
        else if(pconfig->sampleRate == 48000)
        {
    	    s32MinBitRate = (pconfig->nChannelsIn == 1) ? 48000 : 48000;
            s32MaxBitRate = (pconfig->nChannelsIn == 1) ? 288000 : 320000;
            s32RecommemdRate = (pconfig->nChannelsIn == 1) ? 64000 : 128000;
    	    if(pconfig->bitRate < s32MinBitRate ||  pconfig->bitRate > s32MaxBitRate)
    	    {
    		    printf("AACLC 48000 Hz bitRate(%d) should be %d ~ %d, recommed %d\n",pconfig->bitRate,s32MinBitRate,s32MaxBitRate,s32RecommemdRate);
    		    return HI_ERR_AENC_ILLEGAL_PARAM;
    	    }
        }
        else if(pconfig->sampleRate == 16000)
        {
    	    s32MinBitRate = (pconfig->nChannelsIn == 1) ? 24000 : 24000;
            s32MaxBitRate = (pconfig->nChannelsIn == 1) ? 96000 : 192000;
            s32RecommemdRate = (pconfig->nChannelsIn == 1) ? 48000 : 48000;
    	    if(pconfig->bitRate < s32MinBitRate ||  pconfig->bitRate > s32MaxBitRate)
    	    {
    		    printf("AACLC 16000 Hz bitRate(%d) should be %d ~ %d, recommed %d\n",pconfig->bitRate,s32MinBitRate,s32MaxBitRate,s32RecommemdRate);
    		    return HI_ERR_AENC_ILLEGAL_PARAM;
    	    }
        }
        else if(pconfig->sampleRate == 8000)
        {
    	    s32MinBitRate = (pconfig->nChannelsIn == 1) ? 16000 : 16000;
            s32MaxBitRate = (pconfig->nChannelsIn == 1) ? 48000 : 96000;
            s32RecommemdRate = (pconfig->nChannelsIn == 1) ? 24000 : 32000;
    	    if(pconfig->bitRate < s32MinBitRate ||  pconfig->bitRate > s32MaxBitRate)
    	    {
    		    printf("AACLC 8000 Hz bitRate(%d) should be %d ~ %d, recommed %d\n",pconfig->bitRate,s32MinBitRate,s32MaxBitRate,s32RecommemdRate);
    		    return HI_ERR_AENC_ILLEGAL_PARAM;
    	    }
        }
        else if(pconfig->sampleRate == 24000)
        {
    	    s32MinBitRate = (pconfig->nChannelsIn == 1) ? 32000 : 32000;
            s32MaxBitRate = (pconfig->nChannelsIn == 1) ? 144000 : 288000;
            s32RecommemdRate = (pconfig->nChannelsIn == 1) ? 48000 : 48000;
    	    if(pconfig->bitRate < s32MinBitRate ||  pconfig->bitRate > s32MaxBitRate)
    	    {
    		    printf("AACLC 24000 Hz bitRate(%d) should be %d ~ %d, recommed %d\n",pconfig->bitRate,s32MinBitRate,s32MaxBitRate,s32RecommemdRate);
    		    return HI_ERR_AENC_ILLEGAL_PARAM;
    	    }
        }
        else if(pconfig->sampleRate == 22050)
        {
    	    s32MinBitRate = (pconfig->nChannelsIn == 1) ? 32000 : 32000;
            s32MaxBitRate = (pconfig->nChannelsIn == 1) ? 132000 : 265000;
            s32RecommemdRate = (pconfig->nChannelsIn == 1) ? 64000 : 48000;
    	    if(pconfig->bitRate < s32MinBitRate ||  pconfig->bitRate > s32MaxBitRate)
    	    {
    		    printf("AACLC 22050 Hz bitRate(%d) should be %d ~ %d, recommed %d\n",pconfig->bitRate,s32MinBitRate,s32MaxBitRate,s32RecommemdRate);
    		    return HI_ERR_AENC_ILLEGAL_PARAM;
    	    }
        }
        else
        {
    	    printf("AACLC invalid samplerate(%d)\n",pconfig->sampleRate);
    	    return HI_ERR_AENC_ILLEGAL_PARAM;
        }
    }
	else
	{
		return HI_ERR_AENC_ILLEGAL_PARAM;
	}

	return HI_SUCCESS;
}

static HI_S32 AencCheckEAACConfig(AACENC_CONFIG *pconfig)
{
	HI_S32 s32MinBitRate = 0;
    HI_S32 s32MaxBitRate = 0;
    HI_S32 s32RecommemdRate = 0;

	if(pconfig->coderFormat == EAAC)
    {
        if(pconfig->nChannelsOut != pconfig->nChannelsIn) 
        {
    	    printf("EAAC nChannelsOut(%d) is not equal to nChannelsIn(%d)\n", pconfig->nChannelsOut, pconfig->nChannelsIn);
    	    return HI_ERR_AENC_ILLEGAL_PARAM;
        }
    	
        if(pconfig->sampleRate == 32000)
        {
    	    s32MinBitRate = (pconfig->nChannelsIn == 1) ? 32000 : 32000;
            s32MaxBitRate = (pconfig->nChannelsIn == 1) ? 64000 : 128000;
            s32RecommemdRate = (pconfig->nChannelsIn == 1) ? 48000 : 64000;
    	    if(pconfig->bitRate < s32MinBitRate ||  pconfig->bitRate > s32MaxBitRate)
    	    {
    		    printf("EAAC 32000 Hz bitRate(%d) should be %d ~ %d, recommed %d\n",pconfig->bitRate,s32MinBitRate,s32MaxBitRate,s32RecommemdRate);
    		    return HI_ERR_AENC_ILLEGAL_PARAM;
    	    }
        }
        else if(pconfig->sampleRate == 44100)
        {
    	    s32MinBitRate = (pconfig->nChannelsIn == 1) ? 32000 : 32000;
            s32MaxBitRate = (pconfig->nChannelsIn == 1) ? 64000 : 128000;
            s32RecommemdRate = (pconfig->nChannelsIn == 1) ? 48000 : 64000;
    	    if(pconfig->bitRate < s32MinBitRate ||  pconfig->bitRate > s32MaxBitRate)
    	    {
    		    printf("EAAC 44100 Hz bitRate(%d) should be %d ~ %d, recommed %d\n",pconfig->bitRate,s32MinBitRate,s32MaxBitRate,s32RecommemdRate);
    		    return HI_ERR_AENC_ILLEGAL_PARAM;
    	    }
        }
        else if(pconfig->sampleRate == 48000)
        {
    	    s32MinBitRate = (pconfig->nChannelsIn == 1) ? 32000 : 32000;
            s32MaxBitRate = (pconfig->nChannelsIn == 1) ? 64000 : 128000;
            s32RecommemdRate = (pconfig->nChannelsIn == 1) ? 48000 : 64000;
    	    if(pconfig->bitRate < s32MinBitRate ||  pconfig->bitRate > s32MaxBitRate)
    	    {
    		    printf("EAAC 48000 Hz bitRate(%d) should be %d ~ %d, recommed %d\n",pconfig->bitRate,s32MinBitRate,s32MaxBitRate,s32RecommemdRate);
    		    return HI_ERR_AENC_ILLEGAL_PARAM;
    	    }
        }
        else if(pconfig->sampleRate == 16000)
        {
    	    s32MinBitRate = (pconfig->nChannelsIn == 1) ? 24000 : 24000;
            s32MaxBitRate = (pconfig->nChannelsIn == 1) ? 48000 : 96000;
            s32RecommemdRate = (pconfig->nChannelsIn == 1) ? 32000 : 32000;
    	    if(pconfig->bitRate < s32MinBitRate ||  pconfig->bitRate > s32MaxBitRate)
    	    {
    		    printf("EAAC 16000 Hz bitRate(%d) should be %d ~ %d, recommed %d\n",pconfig->bitRate,s32MinBitRate,s32MaxBitRate,s32RecommemdRate);
    		    return HI_ERR_AENC_ILLEGAL_PARAM;
    	    }
        }
        else if(pconfig->sampleRate == 22050)
        {
    	    s32MinBitRate = (pconfig->nChannelsIn == 1) ? 32000 : 32000;
            s32MaxBitRate = (pconfig->nChannelsIn == 1) ? 64000 : 128000;
            s32RecommemdRate = (pconfig->nChannelsIn == 1) ? 48000 : 64000;
    	    if(pconfig->bitRate < s32MinBitRate ||  pconfig->bitRate > s32MaxBitRate)
    	    {
    		    printf("EAAC 22050 Hz bitRate(%d) should be %d ~ %d, recommed %d\n",pconfig->bitRate,s32MinBitRate,s32MaxBitRate,s32RecommemdRate);
    		    return HI_ERR_AENC_ILLEGAL_PARAM;
    	    }
        }
        else if(pconfig->sampleRate == 24000)
        {
    	    s32MinBitRate = (pconfig->nChannelsIn == 1) ? 32000 : 32000;
            s32MaxBitRate = (pconfig->nChannelsIn == 1) ? 64000 : 128000;
            s32RecommemdRate = (pconfig->nChannelsIn == 1) ? 48000 : 64000;
    	    if(pconfig->bitRate < s32MinBitRate ||  pconfig->bitRate > s32MaxBitRate)
    	    {
    		    printf("EAAC 24000 Hz bitRate(%d) should be %d ~ %d, recommed %d\n",pconfig->bitRate,s32MinBitRate,s32MaxBitRate,s32RecommemdRate);
    		    return HI_ERR_AENC_ILLEGAL_PARAM;
    	    }
        }
        else
        {
    	    printf("EAAC invalid samplerate(%d)\n",pconfig->sampleRate);
    	    return HI_ERR_AENC_ILLEGAL_PARAM;
        }
    }
	else
	{
		return HI_ERR_AENC_ILLEGAL_PARAM;
	}

	return HI_SUCCESS;
}

static HI_S32 AencCheckEAACPLUSConfig(AACENC_CONFIG *pconfig)
{
	HI_S32 s32MinBitRate = 0;
    HI_S32 s32MaxBitRate = 0;
    HI_S32 s32RecommemdRate = 0;

	if(pconfig->coderFormat == EAACPLUS)
    {
        if(pconfig->nChannelsOut != 2 || pconfig->nChannelsIn != 2) 
        {
            printf("EAACPLUS nChannelsOut(%d) and nChannelsIn(%d) should be 2\n",pconfig->nChannelsOut, pconfig->nChannelsIn);
            return HI_ERR_AENC_ILLEGAL_PARAM;
        }
        	
        if(pconfig->sampleRate == 32000)
        {
        	s32MinBitRate = 16000;
            s32MaxBitRate = 64000;
            s32RecommemdRate = 32000;
    	    if(pconfig->bitRate < s32MinBitRate ||  pconfig->bitRate > s32MaxBitRate)
    	    {
    		    printf("EAACPLUS 32000 Hz bitRate(%d) should be %d ~ %d, recommed %d\n",pconfig->bitRate,s32MinBitRate,s32MaxBitRate,s32RecommemdRate);
    		    return HI_ERR_AENC_ILLEGAL_PARAM;
    	    }
        }
        else if(pconfig->sampleRate == 44100)
        {
        	s32MinBitRate = 16000;
            s32MaxBitRate = 64000;
            s32RecommemdRate = 48000;
    	    if(pconfig->bitRate < s32MinBitRate ||  pconfig->bitRate > s32MaxBitRate)
    	    {
    		    printf("EAACPLUS 44100 Hz bitRate(%d) should be %d ~ %d, recommed %d\n",pconfig->bitRate,s32MinBitRate,s32MaxBitRate,s32RecommemdRate);
    		    return HI_ERR_AENC_ILLEGAL_PARAM;
    	    }
        }
        else if(pconfig->sampleRate == 48000)
        {
        	s32MinBitRate = 16000;
            s32MaxBitRate = 64000;
            s32RecommemdRate = 48000;
    	    if(pconfig->bitRate < s32MinBitRate ||  pconfig->bitRate > s32MaxBitRate)
    	    {
    		    printf("EAACPLUS 48000 Hz bitRate(%d) should be %d ~ %d, recommed %d\n",pconfig->bitRate,s32MinBitRate,s32MaxBitRate,s32RecommemdRate);
    		    return HI_ERR_AENC_ILLEGAL_PARAM;
    	    }
        }
        else if(pconfig->sampleRate == 16000)
        {
        	s32MinBitRate = 16000;
            s32MaxBitRate = 48000;
            s32RecommemdRate = 32000;
    	    if(pconfig->bitRate < s32MinBitRate ||  pconfig->bitRate > s32MaxBitRate)
    	    {
    		    printf("EAACPLUS 16000 Hz bitRate(%d) should be %d ~ %d, recommed %d\n",pconfig->bitRate,s32MinBitRate,s32MaxBitRate,s32RecommemdRate);
    		    return HI_ERR_AENC_ILLEGAL_PARAM;
    	    }
        }
        else if(pconfig->sampleRate == 22050)
        {
        	s32MinBitRate = 16000;
            s32MaxBitRate = 64000;
            s32RecommemdRate = 32000;
    	    if(pconfig->bitRate < s32MinBitRate ||  pconfig->bitRate > s32MaxBitRate)
    	    {
    		    printf("EAACPLUS 22050 Hz bitRate(%d) should be %d ~ %d, recommed %d\n",pconfig->bitRate,s32MinBitRate,s32MaxBitRate,s32RecommemdRate);
    		    return HI_ERR_AENC_ILLEGAL_PARAM;
    	    }
        }
        else if(pconfig->sampleRate == 24000)
        {
        	s32MinBitRate = 16000;
            s32MaxBitRate = 64000;
            s32RecommemdRate = 32000;
    	    if(pconfig->bitRate < s32MinBitRate ||  pconfig->bitRate > s32MaxBitRate)
    	    {
    		    printf("EAACPLUS 24000 Hz bitRate(%d) should be %d ~ %d, recommed %d\n",pconfig->bitRate,s32MinBitRate,s32MaxBitRate,s32RecommemdRate);
    		    return HI_ERR_AENC_ILLEGAL_PARAM;
    	    }
        }
        else
        {
        	printf("EAACPLUS invalid samplerate(%d)\n",pconfig->sampleRate);
        	return HI_ERR_AENC_ILLEGAL_PARAM;
        }
        	
    }
	else
	{
		return HI_ERR_AENC_ILLEGAL_PARAM;
	}

	return HI_SUCCESS;
}

static HI_S32 AencCheckAACLDConfig(AACENC_CONFIG *pconfig)
{
	HI_S32 s32MinBitRate = 0;
    HI_S32 s32MaxBitRate = 0;
    HI_S32 s32RecommemdRate = 0;

	if(pconfig->coderFormat == AACLD)
    {
        if(pconfig->nChannelsOut != pconfig->nChannelsIn) 
        {
    	    printf("AACLD nChannelsOut(%d) in not equal to nChannelsIn(%d)\n", pconfig->nChannelsOut, pconfig->nChannelsIn);
    	    return HI_ERR_AENC_ILLEGAL_PARAM;
        }
    	
        if(pconfig->sampleRate == 32000)
        {
    	    s32MinBitRate = (pconfig->nChannelsIn == 1) ? 48000 : 64000;
            s32MaxBitRate = (pconfig->nChannelsIn == 1) ? 320000 : 320000;
            s32RecommemdRate = (pconfig->nChannelsIn == 1) ? 64000 : 128000;
    	    if(pconfig->bitRate < s32MinBitRate ||  pconfig->bitRate > s32MaxBitRate)
    	    {
    		    printf("AACLD 32000 Hz bitRate(%d) should be %d ~ %d, recommed %d\n",pconfig->bitRate,s32MinBitRate,s32MaxBitRate,s32RecommemdRate);
    		    return HI_ERR_AENC_ILLEGAL_PARAM;
    	    }
        }
        else if(pconfig->sampleRate == 44100)
        {
    	    s32MinBitRate = (pconfig->nChannelsIn == 1) ? 64000 : 44000;
            s32MaxBitRate = (pconfig->nChannelsIn == 1) ? 320000 : 320000;
            s32RecommemdRate = (pconfig->nChannelsIn == 1) ? 128000 : 256000;
    	    if(pconfig->bitRate < s32MinBitRate ||  pconfig->bitRate > s32MaxBitRate)
    	    {
    		    printf("AACLD 44100 Hz bitRate(%d) should be %d ~ %d, recommed %d\n",pconfig->bitRate,s32MinBitRate,s32MaxBitRate,s32RecommemdRate);
    		    return HI_ERR_AENC_ILLEGAL_PARAM;
    	    }
        }
        else if(pconfig->sampleRate == 48000)
        {
    	    s32MinBitRate = (pconfig->nChannelsIn == 1) ? 64000 : 64000;
            s32MaxBitRate = (pconfig->nChannelsIn == 1) ? 320000 : 320000;
            s32RecommemdRate = (pconfig->nChannelsIn == 1) ? 128000 : 256000;
    	    if(pconfig->bitRate < s32MinBitRate ||  pconfig->bitRate > s32MaxBitRate)
    	    {
    		    printf("AACLD 48000 Hz bitRate(%d) should be %d ~ %d, recommed %d\n",pconfig->bitRate,s32MinBitRate,s32MaxBitRate,s32RecommemdRate);
    		    return HI_ERR_AENC_ILLEGAL_PARAM;
    	    }
        }
        else if(pconfig->sampleRate == 16000)
        {
    	    s32MinBitRate = (pconfig->nChannelsIn == 1) ? 24000 : 32000;
            s32MaxBitRate = (pconfig->nChannelsIn == 1) ? 192000 : 320000;
            s32RecommemdRate = (pconfig->nChannelsIn == 1) ? 48000 : 96000;
    	    if(pconfig->bitRate < s32MinBitRate ||  pconfig->bitRate > s32MaxBitRate)
    	    {
    		    printf("AACLD 16000 Hz bitRate(%d) should be %d ~ %d, recommed %d\n",pconfig->bitRate,s32MinBitRate,s32MaxBitRate,s32RecommemdRate);
    		    return HI_ERR_AENC_ILLEGAL_PARAM;
    	    }
        }
        else if(pconfig->sampleRate == 8000)
        {
    	    s32MinBitRate = (pconfig->nChannelsIn == 1) ? 16000 : 16000;
            s32MaxBitRate = (pconfig->nChannelsIn == 1) ? 96000 : 192000;
            s32RecommemdRate = (pconfig->nChannelsIn == 1) ? 24000 : 48000;
    	    if(pconfig->bitRate < s32MinBitRate ||  pconfig->bitRate > s32MaxBitRate)
    	    {
    		    printf("AACLD 8000 Hz bitRate(%d) should be %d ~ %d, recommed %d\n",pconfig->bitRate,s32MinBitRate,s32MaxBitRate,s32RecommemdRate);
    		    return HI_ERR_AENC_ILLEGAL_PARAM;
    	    }
        }
        else if(pconfig->sampleRate == 24000)
        {
    	    s32MinBitRate = (pconfig->nChannelsIn == 1) ? 32000 : 48000;
            s32MaxBitRate = (pconfig->nChannelsIn == 1) ? 256000 : 320000;
            s32RecommemdRate = (pconfig->nChannelsIn == 1) ? 64000 : 128000;
    	    if(pconfig->bitRate < s32MinBitRate ||  pconfig->bitRate > s32MaxBitRate)
    	    {
    		    printf("AACLD 24000 Hz bitRate(%d) should be %d ~ %d, recommed %d\n",pconfig->bitRate,s32MinBitRate,s32MaxBitRate,s32RecommemdRate);
    		    return HI_ERR_AENC_ILLEGAL_PARAM;
    	    }
        }
        else if(pconfig->sampleRate == 22050)
        {
    	    s32MinBitRate = (pconfig->nChannelsIn == 1) ? 32000 : 48000;
            s32MaxBitRate = (pconfig->nChannelsIn == 1) ? 256000 : 320000;
            s32RecommemdRate = (pconfig->nChannelsIn == 1) ? 48000 : 96000;
    	    if(pconfig->bitRate < s32MinBitRate ||  pconfig->bitRate > s32MaxBitRate)
    	    {
    		    printf("AACLD 22050 Hz bitRate(%d) should be %d ~ %d, recommed %d\n",pconfig->bitRate,s32MinBitRate,s32MaxBitRate,s32RecommemdRate);
    		    return HI_ERR_AENC_ILLEGAL_PARAM;
    	    }
        }
        else
        {
    	    printf("AACLD invalid samplerate(%d)\n",pconfig->sampleRate);
    	    return HI_ERR_AENC_ILLEGAL_PARAM;
        }
            
    }
	else
	{
		return HI_ERR_AENC_ILLEGAL_PARAM;
	}

	return HI_SUCCESS;
}

static HI_S32 AencCheckAACELDConfig(AACENC_CONFIG *pconfig)
{
	HI_S32 s32MinBitRate = 0;
    HI_S32 s32MaxBitRate = 0;
    HI_S32 s32RecommemdRate = 0;

	if(pconfig->coderFormat == AACELD)
    {
        if(pconfig->nChannelsOut != pconfig->nChannelsIn) 
        {
    	    printf("AACELD nChannelsOut(%d) in not equal to nChannelsIn(%d)\n", pconfig->nChannelsOut, pconfig->nChannelsIn);
    	    return HI_ERR_AENC_ILLEGAL_PARAM;
        }
    	
        if(pconfig->sampleRate == 32000)
        {
    	    s32MinBitRate = (pconfig->nChannelsIn == 1) ? 32000 : 64000;
            s32MaxBitRate = (pconfig->nChannelsIn == 1) ? 320000 : 320000;
            s32RecommemdRate = (pconfig->nChannelsIn == 1) ? 64000 : 128000;
    	    if(pconfig->bitRate < s32MinBitRate ||  pconfig->bitRate > s32MaxBitRate)
    	    {
    		    printf("AACELD 32000 Hz bitRate(%d) should be %d ~ %d, recommed %d\n",pconfig->bitRate,s32MinBitRate,s32MaxBitRate,s32RecommemdRate);
    		    return HI_ERR_AENC_ILLEGAL_PARAM;
    	    }
        }
        else if(pconfig->sampleRate == 44100)
        {
    	    s32MinBitRate = (pconfig->nChannelsIn == 1) ? 96000 : 192000;
            s32MaxBitRate = (pconfig->nChannelsIn == 1) ? 320000 : 320000;
            s32RecommemdRate = (pconfig->nChannelsIn == 1) ? 128000 : 256000;
    	    if(pconfig->bitRate < s32MinBitRate ||  pconfig->bitRate > s32MaxBitRate)
    	    {
    		    printf("AACELD 44100 Hz bitRate(%d) should be %d ~ %d, recommed %d\n",pconfig->bitRate,s32MinBitRate,s32MaxBitRate,s32RecommemdRate);
    		    return HI_ERR_AENC_ILLEGAL_PARAM;
    	    }
        }
        else if(pconfig->sampleRate == 48000)
        {
    	    s32MinBitRate = (pconfig->nChannelsIn == 1) ? 96000 : 192000;
            s32MaxBitRate = (pconfig->nChannelsIn == 1) ? 320000 : 320000;
            s32RecommemdRate = (pconfig->nChannelsIn == 1) ? 128000 : 256000;
    	    if(pconfig->bitRate < s32MinBitRate ||  pconfig->bitRate > s32MaxBitRate)
    	    {
    		    printf("AACELD 48000 Hz bitRate(%d) should be %d ~ %d, recommed %d\n",pconfig->bitRate,s32MinBitRate,s32MaxBitRate,s32RecommemdRate);
    		    return HI_ERR_AENC_ILLEGAL_PARAM;
    	    }
        }
        else if(pconfig->sampleRate == 16000)
        {
    	    s32MinBitRate = (pconfig->nChannelsIn == 1) ? 16000 : 32000;
            s32MaxBitRate = (pconfig->nChannelsIn == 1) ? 256000 : 320000;
            s32RecommemdRate = (pconfig->nChannelsIn == 1) ? 48000 : 96000;
    	    if(pconfig->bitRate < s32MinBitRate ||  pconfig->bitRate > s32MaxBitRate)
    	    {
    		    printf("AACELD 16000 Hz bitRate(%d) should be %d ~ %d, recommed %d\n",pconfig->bitRate,s32MinBitRate,s32MaxBitRate,s32RecommemdRate);
    		    return HI_ERR_AENC_ILLEGAL_PARAM;
    	    }
        }
        else if(pconfig->sampleRate == 8000)
        {
    	    s32MinBitRate = (pconfig->nChannelsIn == 1) ? 32000 : 64000;
            s32MaxBitRate = (pconfig->nChannelsIn == 1) ? 96000 : 192000;
            s32RecommemdRate = (pconfig->nChannelsIn == 1) ? 32000 : 64000;
    	    if(pconfig->bitRate < s32MinBitRate ||  pconfig->bitRate > s32MaxBitRate)
    	    {
    		    printf("AACELD 8000 Hz bitRate(%d) should be %d ~ %d, recommed %d\n",pconfig->bitRate,s32MinBitRate,s32MaxBitRate,s32RecommemdRate);
    		    return HI_ERR_AENC_ILLEGAL_PARAM;
    	    }
        }
        else if(pconfig->sampleRate == 24000)
        {
    	    s32MinBitRate = (pconfig->nChannelsIn == 1) ? 24000 : 32000;
            s32MaxBitRate = (pconfig->nChannelsIn == 1) ? 256000 : 320000;
            s32RecommemdRate = (pconfig->nChannelsIn == 1) ? 64000 : 128000;
    	    if(pconfig->bitRate < s32MinBitRate ||  pconfig->bitRate > s32MaxBitRate)
    	    {
    		    printf("AACELD 24000 Hz bitRate(%d) should be %d ~ %d, recommed %d\n",pconfig->bitRate,s32MinBitRate,s32MaxBitRate,s32RecommemdRate);
    		    return HI_ERR_AENC_ILLEGAL_PARAM;
    	    }
        }
        else if(pconfig->sampleRate == 22050)
        {
    	    s32MinBitRate = (pconfig->nChannelsIn == 1) ? 24000 : 32000;
            s32MaxBitRate = (pconfig->nChannelsIn == 1) ? 256000 : 320000;
            s32RecommemdRate = (pconfig->nChannelsIn == 1) ? 48000 : 96000;
    	    if(pconfig->bitRate < s32MinBitRate ||  pconfig->bitRate > s32MaxBitRate)
    	    {
    		    printf("AACELD 22050 Hz bitRate(%d) should be %d ~ %d, recommed %d\n",pconfig->bitRate,s32MinBitRate,s32MaxBitRate,s32RecommemdRate);
    		    return HI_ERR_AENC_ILLEGAL_PARAM;
    	    }
        }
        else
        {
    	    printf("AACELD invalid samplerate(%d)\n",pconfig->sampleRate);
    	    return HI_ERR_AENC_ILLEGAL_PARAM;
        }
            
    }
	else
	{
		return HI_ERR_AENC_ILLEGAL_PARAM;
	}

	return HI_SUCCESS;
}

HI_S32 AencAACCheckConfig(AACENC_CONFIG *pconfig)
{
    HI_S32 s32Ret = 0;

    if(NULL == pconfig)
    {
    	printf("[Func]:%s [Line]:%d [Info]:%s\n", __FUNCTION__, __LINE__, "pconfig is null");
        return HI_ERR_AENC_NULL_PTR;
    }

    if(pconfig->coderFormat != AACLC && pconfig->coderFormat!= EAAC && pconfig->coderFormat != EAACPLUS && pconfig->coderFormat!= AACLD && pconfig->coderFormat != AACELD)
    {
        printf("aacenc coderFormat(%d) invalid\n",pconfig->coderFormat);
		return HI_ERR_AENC_ILLEGAL_PARAM;
    }


    if(pconfig->quality != AU_QualityExcellent && pconfig->quality!= AU_QualityHigh && pconfig->quality != AU_QualityMedium && pconfig->quality != AU_QualityLow) 
    {
        printf("aacenc quality(%d) invalid\n",pconfig->quality);
		return HI_ERR_AENC_ILLEGAL_PARAM;
    }

    if(pconfig->bitsPerSample != 16) 
    {
        printf("aacenc bitsPerSample(%d) should be 16\n",pconfig->bitsPerSample);
		return HI_ERR_AENC_ILLEGAL_PARAM;
    }

	
	if (pconfig->transtype < 0 || pconfig->transtype > 2)
	{
		printf("invalid transtype(%d), not in [0, 2]\n", pconfig->transtype);
		return HI_ERR_AENC_ILLEGAL_PARAM;
	}

	if(pconfig->bandWidth != 0 && (pconfig->bandWidth < 1000 || pconfig->bandWidth > pconfig->sampleRate / 2))
    {
	    printf("AAC bandWidth(%d) should be 0, or 1000 ~ %d\n",pconfig->bandWidth,pconfig->sampleRate / 2);
	    return HI_ERR_AENC_ILLEGAL_PARAM;
	    
    }

    if (pconfig->coderFormat == AACLC)
    {
		s32Ret = AencCheckAACLCConfig(pconfig);
	}
    else if(pconfig->coderFormat == EAAC)
    {
		s32Ret = AencCheckEAACConfig(pconfig);
	}
    else if(pconfig->coderFormat == EAACPLUS)
    {
		s32Ret = AencCheckEAACPLUSConfig(pconfig);
	}    
    #if 1
    else if(pconfig->coderFormat == AACLD)
    {
		s32Ret = AencCheckAACLDConfig(pconfig);
	}   
    else if(pconfig->coderFormat == AACELD)
    {
		s32Ret = AencCheckAACELDConfig(pconfig);
	}
    #endif


    return s32Ret;
}

HI_S32 OpenAACEncoder(HI_VOID *pEncoderAttr, HI_VOID **ppEncoder)
{
    AENC_AAC_ENCODER_S *pstEncoder = NULL;
    AENC_ATTR_AAC_S *pstAttr = NULL;
    HI_S32 s32Ret;
    AACENC_CONFIG  config;

    HI_ASSERT(pEncoderAttr != NULL); 
    HI_ASSERT(ppEncoder != NULL);

    /* check attribute of encoder */
    pstAttr = (AENC_ATTR_AAC_S *)pEncoderAttr;
    s32Ret = AencCheckAACAttr(pstAttr);
    if (s32Ret)
    {
    	printf("[Func]:%s [Line]:%d s32Ret:0x%x.#########\n", __FUNCTION__, __LINE__, s32Ret);
        return s32Ret;
    }

    /* allocate memory for encoder */
    pstEncoder = (AENC_AAC_ENCODER_S *)malloc(sizeof(AENC_AAC_ENCODER_S));
    if(NULL == pstEncoder)
    {
        printf("[Func]:%s [Line]:%d [Info]:%s\n", __FUNCTION__, __LINE__, "no memory");
        return HI_ERR_AENC_NOMEM;
    }
    memset(pstEncoder, 0, sizeof(AENC_AAC_ENCODER_S));
    *ppEncoder = (HI_VOID *)pstEncoder;        

    /* set default config to encoder */
    s32Ret = AACInitDefaultConfig(&config);
    if (s32Ret)
    {
		free(pstEncoder);
    	printf("[Func]:%s [Line]:%d s32Ret:0x%x.#########\n", __FUNCTION__, __LINE__, s32Ret);
        return s32Ret;
    }
    
    config.coderFormat   = (AuEncoderFormat)pstAttr->enAACType;
    config.bitRate       = pstAttr->enBitRate;
    config.bitsPerSample = 8*(1<<(pstAttr->enBitWidth)); 
    config.sampleRate    = pstAttr->enSmpRate;
    config.bandWidth     = pstAttr->s16BandWidth;//config.sampleRate/2;     
    config.transtype     = (AACENCTransportType)pstAttr->enTransType;  

	if (AUDIO_SOUND_MODE_MONO == pstAttr->enSoundMode && AAC_TYPE_EAACPLUS != pstAttr->enAACType)
	{
		config.nChannelsIn   = 1;
		config.nChannelsOut  = 1; 
	}
	else
	{
		config.nChannelsIn   = 2;
		config.nChannelsOut  = 2; 
	}
    
    config.quality       = AU_QualityHigh;
    
    //printf("~~~~~~~~AENC AAC-TYPE:%d, trans-type:%d, bindWidth:%d ~~~~~~~~~\n",config.coderFormat,config.transtype,config.bandWidth);
    s32Ret = AencAACCheckConfig(&config);
    if (s32Ret)
    {
		free(pstEncoder);
    	printf("[Func]:%s [Line]:%d #########\n", __FUNCTION__, __LINE__);
        return HI_ERR_AENC_ILLEGAL_PARAM;
    }

    /* create encoder */
    s32Ret = AACEncoderOpen(&pstEncoder->pstAACState, &config);
    if (s32Ret)
    {
		free(pstEncoder);
    	printf("[Func]:%s [Line]:%d s32Ret:0x%x.#########\n", __FUNCTION__, __LINE__, s32Ret);
        return s32Ret;
    }

    memcpy(&pstEncoder->stAACAttr, pstAttr, sizeof(AENC_ATTR_AAC_S));

#ifdef DUMP_AACENC
    HI_CHAR NAMEin[256];
    HI_CHAR NAMEout[256];

    snprintf(NAMEin, 256, "AACENC_sin_T%d_t%d.raw", config.coderFormat,config.transtype);
    snprintf(NAMEout, 256, "AACENC_sout_T%d_t%d.aac", config.coderFormat,config.transtype);
    pfdin_ENC= fopen(NAMEin, "w+");
    pfdout_ENC= fopen(NAMEout, "w+");
    cnt_aenc = 10000;
#endif
        
    return HI_SUCCESS;
}

HI_S32 EncodeAACFrm(HI_VOID *pEncoder, const AUDIO_FRAME_S *pstData,
                    HI_U8 *pu8Outbuf,HI_U32 *pu32OutLen)
{
    HI_S32 s32Ret = HI_SUCCESS;
    AENC_AAC_ENCODER_S *pstEncoder = NULL;
    HI_U32 u32PtNums;
    HI_S32 i;     
    HI_S16 aData[AACENC_BLOCKSIZE*2*MAX_CHANNELS];
    HI_S16 s16Len = 0;

    HI_U32 u32WaterLine;    

    HI_ASSERT(pEncoder != NULL);
    HI_ASSERT(pstData != NULL);
    HI_ASSERT(pu8Outbuf != NULL); 
    HI_ASSERT(pu32OutLen != NULL);

    pstEncoder = (AENC_AAC_ENCODER_S *)pEncoder;
   
    if (AUDIO_SOUND_MODE_STEREO == pstEncoder->stAACAttr.enSoundMode)
    {
		/* whether the sound mode of frame and channel is match  */
		if(AUDIO_SOUND_MODE_STEREO != pstData->enSoundmode)
		{
			printf("[Func]:%s [Line]:%d [Info]:%s\n", 
            	__FUNCTION__, __LINE__, "AAC encode receive a frame which not match its Soundmode");
			return HI_ERR_AENC_ILLEGAL_PARAM;
		}
    }

    /*WaterLine, equals to the frame sample frame of protocol*/
    if (pstEncoder->stAACAttr.enAACType == AAC_TYPE_AACLC)
    {
        u32WaterLine = AACLC_SAMPLES_PER_FRAME;
    }
    else if (pstEncoder->stAACAttr.enAACType == AAC_TYPE_EAAC
        || pstEncoder->stAACAttr.enAACType == AAC_TYPE_EAACPLUS)
    {
        u32WaterLine = AACPLUS_SAMPLES_PER_FRAME;
    }
    else if (pstEncoder->stAACAttr.enAACType == AAC_TYPE_AACLD
        || pstEncoder->stAACAttr.enAACType == AAC_TYPE_AACELD)
    {
        u32WaterLine = AACLD_SAMPLES_PER_FRAME;
    }
    else
	{
        printf("[Func]:%s [Line]:%d [Info]:%s\n", __FUNCTION__, __LINE__, "invalid AAC coder type");
        return HI_ERR_AENC_ILLEGAL_PARAM;;
	}
    /* calculate point number */
    u32PtNums = pstData->u32Len/(pstData->enBitwidth + 1);

    /*if frame sample larger than protocol sample, reject to receive, or buffer will be overflow*/
    if (u32PtNums != u32WaterLine)
    {
        printf("[Func]:%s [Line]:%d [Info]:invalid u32PtNums%d for AACType:%d\n", 
            __FUNCTION__, __LINE__, u32PtNums, pstEncoder->stAACAttr.enAACType);
        return HI_ERR_AENC_ILLEGAL_PARAM;;
    }

    /* AAC encoder need interleaved data,here change LLLRRR to LRLRLR. 
       AACLC will encode 1024*2 point, and AACplus encode 2048*2 point*/
    if (AUDIO_SOUND_MODE_STEREO == pstEncoder->stAACAttr.enSoundMode)
    {
		s16Len = u32WaterLine;
        for (i = s16Len-1; i >= 0 ; i--)
        {                
            aData[2*i] = *((HI_S16 *)pstData->pVirAddr[0] + i);
            aData[2*i+1] = *((HI_S16 *)pstData->pVirAddr[1] + i);
        }
 
    }
    else/* if inbuf is momo, copy left to right */
    {
        HI_S16 *temp = (HI_S16 *)pstData->pVirAddr[0];

		s16Len = u32WaterLine;
        for (i = s16Len-1; i >= 0 ; i--)
        {        
        	aData[i] = *(temp + i);
        }

    }

#ifdef DUMP_AACENC
    //if (cnt_aenc > 0)
    {
    fwrite((HI_U8 *)aData, 1, (AUDIO_SOUND_MODE_STEREO == pstEncoder->stAACAttr.enSoundMode)?u32count*2:u32count, pfdin_ENC);
    }
#endif
  
    s32Ret = AACEncoderFrame(pstEncoder->pstAACState, aData, pu8Outbuf, (HI_S32*)pu32OutLen);
    if (HI_SUCCESS != s32Ret)
    {
        printf("[Func]:%s [Line]:%d [Info]:%s\n", __FUNCTION__, __LINE__, "AAC encode failed");
    }
    
#ifdef DUMP_AACENC
    //printf("~~~~~~~~~strlen:%d~~~~~~\n", *((HI_S32*)pu32OutLen));
    //if (cnt_aenc > 0)
    {
    HI_U8 *pu8TestNum = pu8Outbuf + 1;
    //printf("Enc:(%02x %02x), ~~~~~~~~~strlen:%d~~~~~~\n", *pu8Outbuf, *pu8TestNum, *((HI_S32*)pu32OutLen));
    fwrite((HI_U8 *)pu8Outbuf, 1, *((HI_S32*)pu32OutLen), pfdout_ENC);
    cnt_aenc --;
    }
#endif
    return s32Ret;
}

HI_S32 CloseAACEncoder(HI_VOID *pEncoder)
{
    AENC_AAC_ENCODER_S *pstEncoder = NULL;

    HI_ASSERT(pEncoder != NULL);
    pstEncoder = (AENC_AAC_ENCODER_S *)pEncoder;

    AACEncoderClose(pstEncoder->pstAACState); 

    free(pstEncoder);

#ifdef DUMP_AACENC
    if (pfdin_ENC)
    {
        fclose(pfdin_ENC);
        pfdin_ENC= NULL;
    }

    if (pfdout_ENC)
    {
        fclose(pfdout_ENC);
        pfdout_ENC = NULL;
    }
#endif
    return HI_SUCCESS;
}

HI_S32 OpenAACDecoder(HI_VOID *pDecoderAttr, HI_VOID **ppDecoder)
{
    ADEC_AAC_DECODER_S *pstDecoder = NULL;
    ADEC_ATTR_AAC_S *pstAttr = NULL;
    HI_S32 s32Ret;
    
    HI_ASSERT(pDecoderAttr != NULL); 
    HI_ASSERT(ppDecoder != NULL);

    pstAttr = (ADEC_ATTR_AAC_S *)pDecoderAttr;

    /* check attribute of encoder */
    s32Ret = AdecCheckAACAttr(pstAttr);
    if (s32Ret)
    {
    	printf("[Func]:%s [Line]:%d s32Ret:0x%x.#########\n", __FUNCTION__, __LINE__, s32Ret);
        return s32Ret;
    }

    /* allocate memory for decoder */
    pstDecoder = (ADEC_AAC_DECODER_S *)malloc(sizeof(ADEC_AAC_DECODER_S));
    if(NULL == pstDecoder)
    {
        printf("[Func]:%s [Line]:%d [Info]:%s\n", __FUNCTION__, __LINE__, "no memory");
        return HI_ERR_ADEC_NOMEM;
    }
    memset(pstDecoder, 0, sizeof(ADEC_AAC_DECODER_S));
    *ppDecoder = (HI_VOID *)pstDecoder;

    /* create decoder */
    //printf("~~~~~~~~ADEC trans type:%d ~~~~~~~~~\n", pstAttr->enTransType);
    pstDecoder->pstAACState = AACInitDecoder((AACDECTransportType)pstAttr->enTransType);
    if (!pstDecoder->pstAACState)
    {
        free(pstDecoder);
        printf("[Func]:%s [Line]:%d [Info]:%s\n", __FUNCTION__, __LINE__, "AACInitDecoder failed");
        return HI_ERR_ADEC_DECODER_ERR;
    }

    memcpy(&pstDecoder->stAACAttr, pstAttr, sizeof(ADEC_ATTR_AAC_S));

#ifdef DUMP_AACDEC
    HI_CHAR NAMEin[256];
    HI_CHAR NAMEout[256];
    HI_CHAR NAMEout_L[256];

    snprintf(NAMEin, 256, "AACDEC_sin_t%d.aac", pstAttr->enTransType);
    snprintf(NAMEout, 256, "AACDEC_sout_t%d.raw", pstAttr->enTransType);
    snprintf(NAMEout_L, 256, "AACDEC_sout_t%d_L.raw", pstAttr->enTransType);
    
    pfdin_DEC= fopen(NAMEin, "w+");
    pfdout_DEC= fopen(NAMEout, "w+");
    pfdout_DEC_L = fopen(NAMEout_L, "w+");
    cnt_adec = 10000;
#endif
    return HI_SUCCESS;
}

HI_S32 DecodeAACFrm(HI_VOID *pDecoder, HI_U8 **pu8Inbuf,HI_S32 *ps32LeftByte,
                        HI_U16 *pu16Outbuf,HI_U32 *pu32OutLen,HI_U32 *pu32Chns)
{
    HI_S32 s32Ret = HI_SUCCESS;
    ADEC_AAC_DECODER_S *pstDecoder = NULL;
    HI_S32 s32Samples, s32FrmLen, s32SampleBytes;
    AACFrameInfo aacFrameInfo;
    
    HI_ASSERT(pDecoder != NULL);
    HI_ASSERT(pu8Inbuf != NULL);
    HI_ASSERT(ps32LeftByte != NULL); 
    HI_ASSERT(pu16Outbuf != NULL);  
    HI_ASSERT(pu32OutLen != NULL);  
    HI_ASSERT(pu32Chns != NULL);

    *pu32Chns = 1;/*voice encoder only one channle */

    pstDecoder = (ADEC_AAC_DECODER_S *)pDecoder;
    
    s32FrmLen = AACDecodeFindSyncHeader(pstDecoder->pstAACState, pu8Inbuf, ps32LeftByte);            
    if (s32FrmLen < 0)
    {
        printf("[Func]:%s [Line]:%d [Info]:%s\n", __FUNCTION__, __LINE__, "AAC decoder can't find sync header");
        return HI_ERR_ADEC_BUF_LACK;
    }

#ifdef DUMP_AACDEC
    HI_U32 u32PreLen = *ps32LeftByte;
#endif
    
    /*Notes: pInbuf will updated*/ 
    s32Ret = AACDecodeFrame(pstDecoder->pstAACState, pu8Inbuf, ps32LeftByte, (HI_S16 *)pu16Outbuf);
    if (s32Ret)
    {
        printf("aac decoder failed!, s32Ret:0x%x\n", s32Ret);
        return s32Ret;
    }
    #if 0
    if (s32Ret)
    {
        //printf("------------[Func]:%s [Line]:%d  [Ret]:%x  [Inbuf]:0x%x  [LeftByte]:%d->%d [Info]:%s\n", 
        //    __FUNCTION__, __LINE__, s32Ret, pu8Inbuf,s32LefteByte, *ps32LeftByte, "AAC decode failed");
        return s32Ret;
    }  
    else
    {
        //printf("------------[Func]:%s [Line]:%d  [Ret]:%x  [Inbuf]:0x%x  [LeftByte]:%d->%d [Info]:%s\n", 
        //    __FUNCTION__, __LINE__, s32Ret, pu8Inbuf,s32LefteByte, *ps32LeftByte, "AAC decode failed");
        
    }
    #endif
    AACGetLastFrameInfo(pstDecoder->pstAACState, &aacFrameInfo);
    aacFrameInfo.nChans = ((aacFrameInfo.nChans != 0) ? aacFrameInfo.nChans : 1);
    /* samples per frame of one sound track*/
    s32Samples = aacFrameInfo.outputSamps/aacFrameInfo.nChans;

    if ((s32Samples!=AACLC_SAMPLES_PER_FRAME)
		&& (s32Samples!=AACPLUS_SAMPLES_PER_FRAME)
		&& (s32Samples!=AACLD_SAMPLES_PER_FRAME))
    {
    	printf("aac decoder failed!\n");
		return HI_ERR_ADEC_DECODER_ERR;
	}
	
    s32SampleBytes = s32Samples * sizeof(HI_U16);
    *pu32Chns = aacFrameInfo.nChans;
    *pu32OutLen = s32SampleBytes;

#ifdef DUMP_AACDEC
    //if (cnt_adec > 0)
    {   
        //printf("Dec: ~~~~~~~~(%d, %d)~~~~~~~\n", aacFrameInfo.outputSamps, *pu32OutLen);
        fwrite((HI_U8 *)pu8BaseBuf, 1, (u32PreLen-(*ps32LeftByte)), pfdin_DEC);
        fwrite((HI_U8 *)pu16Outbuf, 1, (*pu32OutLen) * (*pu32Chns), pfdout_DEC);
        fwrite((HI_U8 *)pu16Outbuf, 1, (*pu32OutLen), pfdout_DEC_L);
        cnt_adec --;
    }
#endif
    
    /* NOTICE: our audio frame format is same as AAC decoder L/L/L/... R/R/R/...*/    
#if 0       
    /*change data format of AAC encoder to our AUDIO_FRAME format */
    if (MAX_AUDIO_FRAME_LEN != s32SampleBytes)
    {
        /* after decoder, 1024 left samples , then 1024 right samples*/
        memmove((HI_U8*)pOutbuf + MAX_AUDIO_FRAME_LEN, 
            (HI_U8*)pOutbuf + s32SampleBytes, s32SampleBytes);
    }
#endif  
    
    return s32Ret;
}

HI_S32 GetAACFrmInfo(HI_VOID *pDecoder, HI_VOID *pInfo)
{
    ADEC_AAC_DECODER_S *pstDecoder = NULL;
    AACFrameInfo aacFrameInfo;
    AAC_FRAME_INFO_S *pstAacFrm = NULL;
    
    HI_ASSERT(pDecoder != NULL);
    HI_ASSERT(pInfo != NULL);
    
    pstDecoder = (ADEC_AAC_DECODER_S *)pDecoder;
    pstAacFrm = (AAC_FRAME_INFO_S *)pInfo;
            
    AACGetLastFrameInfo(pstDecoder->pstAACState, &aacFrameInfo);

    pstAacFrm->s32Samplerate = aacFrameInfo.sampRateOut;
    pstAacFrm->s32BitRate = aacFrameInfo.bitRate;
    pstAacFrm->s32Profile = aacFrameInfo.profile;
    pstAacFrm->s32TnsUsed = aacFrameInfo.tnsUsed;
    pstAacFrm->s32PnsUsed = aacFrameInfo.pnsUsed;

    return HI_SUCCESS;
}


HI_S32 CloseAACDecoder(HI_VOID *pDecoder)
{
    ADEC_AAC_DECODER_S *pstDecoder = NULL;

    HI_ASSERT(pDecoder != NULL);
    pstDecoder = (ADEC_AAC_DECODER_S *)pDecoder;

    AACFreeDecoder(pstDecoder->pstAACState); 

    free(pstDecoder);
	
	#ifdef DUMP_AACDEC
    if (pfdin_DEC)
    {
        fclose(pfdin_DEC);
        pfdin_DEC = NULL;
    }

    if (pfdout_DEC)
    {
        fclose(pfdout_DEC);
        pfdout_DEC = NULL;
    }

    if (pfdout_DEC_L)
    {
        fclose(pfdout_DEC_L);
        pfdout_DEC_L = NULL;
    }
#endif

    return HI_SUCCESS;
}

HI_S32 ResetAACDecoder(HI_VOID *pDecoder)
{
    ADEC_AAC_DECODER_S *pstDecoder = NULL;

    HI_ASSERT(pDecoder != NULL);
    pstDecoder = (ADEC_AAC_DECODER_S *)pDecoder;

    AACFreeDecoder(pstDecoder->pstAACState); 

    /* create decoder */
    pstDecoder->pstAACState = AACInitDecoder((AACDECTransportType)pstDecoder->stAACAttr.enTransType);
    if (!pstDecoder->pstAACState)
    {
        printf("[Func]:%s [Line]:%d [Info]:%s\n", __FUNCTION__, __LINE__, "AACResetDecoder failed");
        return HI_ERR_ADEC_DECODER_ERR;
    }
    
    return HI_SUCCESS;
}



HI_S32 HI_MPI_AENC_AacInit(HI_VOID)
{
    HI_S32 s32Handle, s32Ret;
    AENC_ENCODER_S stAac;
    
    stAac.enType = PT_AAC;
    snprintf(stAac.aszName, sizeof(stAac.aszName), "Aac");
    stAac.u32MaxFrmLen = MAX_AAC_MAINBUF_SIZE;
    stAac.pfnOpenEncoder = OpenAACEncoder;
    stAac.pfnEncodeFrm = EncodeAACFrm;
    stAac.pfnCloseEncoder = CloseAACEncoder;
    s32Ret = HI_MPI_AENC_RegeisterEncoder(&s32Handle, &stAac);
    if (s32Ret)
    {
        return s32Ret;
    }

    stAac.enType = PT_AACLC;
    snprintf(stAac.aszName, sizeof(stAac.aszName), "Aaclc");
    stAac.u32MaxFrmLen = MAX_AAC_MAINBUF_SIZE;
    stAac.pfnOpenEncoder = OpenAACEncoder;
    stAac.pfnEncodeFrm = EncodeAACFrm;
    stAac.pfnCloseEncoder = CloseAACEncoder;
    s32Ret = HI_MPI_AENC_RegeisterEncoder(&s32Handle, &stAac);
    if (s32Ret)
    {
        return s32Ret;
    }

    return HI_SUCCESS;
}

HI_S32 HI_MPI_ADEC_AacInit(HI_VOID)
{
    HI_S32 s32Handle, s32Ret;

    ADEC_DECODER_S stAac;
    
    stAac.enType = PT_AAC;
    snprintf(stAac.aszName, sizeof(stAac.aszName), "Aac");
    stAac.pfnOpenDecoder = OpenAACDecoder;
    stAac.pfnDecodeFrm = DecodeAACFrm;
    stAac.pfnGetFrmInfo = GetAACFrmInfo;
    stAac.pfnCloseDecoder = CloseAACDecoder;
    stAac.pfnResetDecoder = ResetAACDecoder;
    s32Ret = HI_MPI_ADEC_RegeisterDecoder(&s32Handle, &stAac);
    if (s32Ret)
    {
        return s32Ret;
    }

    stAac.enType = PT_AACLC;
    snprintf(stAac.aszName, sizeof(stAac.aszName), "Aaclc");
    stAac.pfnOpenDecoder = OpenAACDecoder;
    stAac.pfnDecodeFrm = DecodeAACFrm;
    stAac.pfnGetFrmInfo = GetAACFrmInfo;
    stAac.pfnCloseDecoder = CloseAACDecoder;
    stAac.pfnResetDecoder = ResetAACDecoder;
    s32Ret = HI_MPI_ADEC_RegeisterDecoder(&s32Handle, &stAac);
    if (s32Ret)
    {
        return s32Ret;
    }
    
    return HI_SUCCESS;
}

