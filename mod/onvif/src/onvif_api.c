#include <assert.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <time.h>
#include <sys/stat.h>
#include <unistd.h>

#include "nvc.h"
#include "nvt.h"

#include "mod/codec/inc/codec.h"

#define MAX_DEST_LEN 1280

/************************************************************************************/
static NVC_PTZ_Preset_t *Preset = NULL;

static int s_parse_WH2resolution(uint16_t width, uint16_t height);

/***********************************************************************************/
int32_t nvc_init(void)
{
	return 0;
}
int32_t nvc_uninit(void)
{
	return 0;
}

int32_t nvc_probe(int type, int timeout, NVC_PROBER_CB *cb, void *user_args)
{
	int ret = 0;
	//NVC_discovery(type);
	if (cb == NULL)
	{
		printf("[%s][%s]----->>>> Error: Invalid parameter! cb = NULL\n", __FILE__, __func__);
		return -1;
	}
	NVC_Probe_t probe;
	memset(&probe, 0, sizeof(NVC_Probe_t));
	
	probe.type = type;
	ret = NVC_discovery(&probe, timeout, cb, user_args);
	if (ret != 0)
	{
		printf("[%s][%s]----->>>> Error: NVC_discovery return error! ret: %d\n", __FILE__, __func__, ret);
		return -1;
	}
   
	return 0;
}


char *onvif_url_parse(char *in, char *host, int* port, char* path, char *user, char *pwd)
{   
    char *p = strstr(in, "onvif://");

    if(!p) return in;
       
    p += strlen("onvif://");

    /* get onvif://xxx/ */
    char *h = strsep(&p, "/");
  
    if(!h) return in; 
    
    char *ip = NULL;   
    /* goto @ after */
    if(ip = strstr(h, "@"))
    {
        if(user && pwd)
        {        
            *ip = '\0';
             sscanf(h, "%[^:]:%s", user, pwd);
        }
        h = ip+strlen("@");
    }
    /* get ip */
    ip = strsep(&h, ":");
    
    if(ip){ if(host) strcpy(host, ip);}
   
    /* get port */
    if(h) 
        if(port) *port = atoi(h);
    else
        if(port) *port = 80;

    /* append /path */
    if(p){ if(path) strcpy(path, p);}

    return in;
}


void *nvc_dev_open(char *url, int timeout)
{
  int  port = 0;
  char ip[64]   = {0};
  char user[32] = {0};
  char pass[32] = {0};
  char path[64] = {0};
  char _url[256] = {0};
  strcpy(_url, url);
  onvif_url_parse(_url, ip, &port, path, user, pass);
  
	printf("[%s]--->>>> ip:[%s], port:%d, user:[%s], pass:[%s]\n"
	      , __func__, ip, port, user, pass);

	int ret = -1;
	NVC_Dev_t *dev_handle = (NVC_Dev_t *)calloc(1, sizeof(NVC_Dev_t));

	//dev_handle->profile_tn = (Profile_info_t *)calloc(2, sizeof(Profile_info_t));
	dev_handle->capa = (NVC_capa_t *)calloc(1, sizeof(NVC_capa_t));

	strcpy(dev_handle->ip, ip);
	dev_handle->port = port;
	if (NULL != user)
	{
		strcpy(dev_handle->user, user);
	}
	if (NULL != pass)
	{
		strcpy(dev_handle->passwd, pass);
	}

	dev_handle->timeout = timeout;

	dev_handle->capa->capa_type = All;	/**Get all capabilities*/
	dev_handle->capa->ana_capa = (Analytics_capa_t *)calloc(1, sizeof(Analytics_capa_t));
	dev_handle->capa->dev_capa = (Device_capa_t *)calloc(1, sizeof(Device_capa_t));
	dev_handle->capa->env_capa = (Events_capa_t *)calloc(1, sizeof(Events_capa_t));
	dev_handle->capa->img_capa = (Img_capa_t *)calloc(1, sizeof(Img_capa_t));
	dev_handle->capa->media_capa = (Media_capa_t *)calloc(1, sizeof(Media_capa_t));
	dev_handle->capa->PTZ_capa = (PTZ_capa_t *)calloc(1, sizeof(PTZ_capa_t));

	ret = NVC_Get_Capabilities(dev_handle);
	if (ret != 0)
	{
		//printf("[%s][%s][%d]--->>> NVC_Get_capabilities failed!\n, ret: %d\n",__FILE__, __func__, __LINE__, ret);
		nvc_dev_close(dev_handle);
		return NULL;
	}
	
	ret = NVC_Get_profiles(dev_handle);
	if (ret != 0)
	{
		//printf("[%s][%s][%d]--->>> NVC_Get_capabilities failed!\n, ret: %d\n",__FILE__, __func__, __LINE__, ret);
		nvc_dev_close(dev_handle);
		return NULL;
	}
  /**************** get ptz pantile's and zoom's space ****************/
  dev_handle->ptz_conf = calloc(1, sizeof(NVC_PTZ_Configution_t));
  ret = NVC_PTZ_Get_Configurations(dev_handle, dev_handle->ptz_conf);
  if (ret != 0)
  {
    //printf("[%s][%d]===> NVC_Get_Presets return Error! ret = %d\n", __func__, __LINE__, ret);
  }
  else
  {
    #if 0
    printf("[%s][%d]===>PTZ pantilt space: %s\n, zoom space: %s\n"
            , __func__, __LINE__
            , dev_handle->ptz_conf->ContinuousPanTiltSpace
            , dev_handle->ptz_conf->ContinuousZoomSpace);
    #endif
  }
  /**获取预置位*/
  dev_handle->Preset = calloc(1, sizeof(NVC_PTZ_Preset_t));
  //dev_handle->Preset->_preset = calloc(MAX_PRESET_NUM, sizeof(PTZ_Preset_t));
  ret = NVC_PTZ_Get_Presets(dev_handle, dev_handle->Preset);

  if (ret != 0)
  {
      //printf("[%s][%d]===> NVC_Get_Presets return Error! ret = %d\n", __func__, __LINE__, ret);
  }
  else
  {
      #if 0
      int i = 0;
      //printf("[%s][%d]===> preset->size: %d\n", __func__, __LINE__, dev_handle->Preset->_size);
      for (i = 0; i < dev_handle->Preset->_size; i++)
      {
          printf("[%s][%d]===> preset[%d].name: %s, token: %s\n"
              , __func__, __LINE__
              , i
              , dev_handle->Preset->_preset[i].name
              , dev_handle->Preset->_preset[i].token);

          printf("[%s][%d]===> preset[%d].speed ==> [ pantilt => x: %f, y: %f, zoom => x: %f ]\n"
              , __func__, __LINE__
              , i
              , dev_handle->Preset->_preset[i].speed.pantilt.x 
              , dev_handle->Preset->_preset[i].speed.pantilt.y
              , dev_handle->Preset->_preset[i].speed.zoom.x );
      }
      #endif
  }
#if 0
  ret = NVC_PTZ_Preset_tour(dev_handle, dev_handle->Preset);
  if(ret != 0)
  {
      printf("[%s][%d]===> NVC_PTZ_Preset_tour return Error! ret = %d\n", __func__, __LINE__, ret);
  }
#endif

  return dev_handle;
}

int32_t nvc_dev_close(void *dev)
{
	NVC_Dev_t *dev_handle = (NVC_Dev_t *)dev;
	if (NULL == dev_handle)
	{
		return 0;
	}
    #if 1
    if (dev_handle->Preset)
    {
        /*
        if (dev_handle->Preset->_preset)
        {
            free(dev_handle->Preset->_preset);
            dev_handle->Preset->_preset = NULL;
        }
        */
        free(dev_handle->Preset);
        dev_handle->Preset = NULL;
    }
    if (dev_handle->ptz_conf)
    {
        free(dev_handle->ptz_conf);
        dev_handle->ptz_conf = NULL;
    }
    #else
    if (Preset)
    {
        if (Preset->_preset)
        {
            free(Preset->_preset);
            Preset->_preset = NULL;
        }
        free(Preset);
        Preset = NULL;
    }
    #endif
	if (dev_handle->capa)
	{
		if (dev_handle->capa->ana_capa)
		{
			free(dev_handle->capa->ana_capa);
			dev_handle->capa->ana_capa = NULL;
		}
		if (dev_handle->capa->dev_capa)
		{
			free(dev_handle->capa->dev_capa);
			dev_handle->capa->dev_capa = NULL;
		}
		if (dev_handle->capa->env_capa)
		{
			free(dev_handle->capa->env_capa);
			dev_handle->capa->env_capa = NULL;
		}
		if (dev_handle->capa->img_capa)
		{
			free(dev_handle->capa->img_capa);
			dev_handle->capa->img_capa = NULL;
		}
		if (dev_handle->capa->media_capa)
		{
			free(dev_handle->capa->media_capa);
			dev_handle->capa->media_capa = NULL;
		}
		if (dev_handle->capa->PTZ_capa)
		{
			free(dev_handle->capa->PTZ_capa);
			dev_handle->capa->PTZ_capa = NULL;
		}
		free(dev_handle->capa);
		dev_handle->capa = NULL;
	}
	
	if (dev_handle->profile_tn)	/**此块空间在onvif.c中的getprofiles中calloc*/
	{
		free(dev_handle->profile_tn);
		dev_handle->profile_tn = NULL;
	}

	free(dev_handle);
	dev_handle = NULL;
	return 0;
}

int32_t nvc_media_url_get(void *dev, int ch,  int st, nvc_media_url_t* media)
{
  int retval = 0;
	NVC_Dev_t *dev_handle = (NVC_Dev_t *)dev;
	int st_type = st;
	
	nvc_media_url_t *nvc_url = media;
	NVC_Stream_Uri_t nvc_stream_uri;
	
	if (dev == NULL)
	{
		return -1;	
	}

	dev_handle->st_type = st_type;
	nvc_stream_uri.protocol = nvc_url->proto;
	nvc_stream_uri.trans_protocol = nvc_url->trans;

	if (strlen(dev_handle->capa->media_capa->url)== 0 
		|| strlen(dev_handle->profile_tn[dev_handle->st_type].profile_token) == 0)
	{
    return -2;
	}
	
	retval = NVC_GetStreamUri(dev_handle, &nvc_stream_uri);
	if (retval == -1)
	{
		return -3;
	}
	
	memset(nvc_url->url, 0, strlen(nvc_url->url));
	strncpy(nvc_url->url, nvc_stream_uri.url, strlen(nvc_stream_uri.url));
  return 0;
}

int32_t nvc_img_attr_get(void *dev, int ch, int st, nvc_image_attr_t *attr)
{
  int ret = 0;
  nvc_image_attr_t *img_parm = attr;
  NVC_Img_attr_t nvc_img;
 
  memset(&nvc_img, 0, sizeof(NVC_Img_attr_t));

	if (NULL == dev)
	{
		return -1;	
	}
	
	NVC_Dev_t *dev_handle = (NVC_Dev_t *)dev;
	dev_handle->st_type = st;
        
  ret = NVC_Get_Img_options(dev_handle, &nvc_img);

  ret = NVC_Get_Img_attr_info(dev_handle, &nvc_img);
  if (ret != 0)
  {
    return -1;	
  }

  img_parm->brightness = nvc_img.brightness;
  img_parm->contrast   = nvc_img.contrast;
  img_parm->saturation = nvc_img.saturation;
  img_parm->hue        = nvc_img.hue;
  img_parm->sharpness  = nvc_img.sharpness;
  
  return 0;
}

int32_t nvc_img_attr_set(void *dev, int ch, int st, nvc_image_attr_t *attr) 
{
    //printf("[%s][%d]=====================>>> in\n", __func__, __LINE__);
  int ret = 0;
  nvc_image_attr_t *img_parm = attr;
  NVC_Img_attr_t nvc_img;
  memset(&nvc_img, 0, sizeof(NVC_Img_attr_t));

	if (NULL == dev)
	{
		return -1;	
	}
	
	NVC_Dev_t *dev_handle = (NVC_Dev_t *)dev;
	dev_handle->st_type = st;

  nvc_img.brightness = img_parm->brightness;
  nvc_img.contrast   = img_parm->contrast;
  nvc_img.saturation = img_parm->saturation;
  nvc_img.hue        = img_parm->hue;
  nvc_img.sharpness  = img_parm->sharpness;

  ret = NVC_Get_Img_options(dev_handle, &nvc_img);
  
  ret = NVC_Set_Img_attr_info(dev_handle, &nvc_img);
  if (ret != 0)
	{
		return -1;	
	}
  return 0;
}

int32_t nvc_ptz_presets_get(void *dev, int ch, int st, nvc_preset_param_t *preset)
{
  int ret = 0;
  
  if (dev == NULL)
  {
      return -1;
  }

	NVC_Dev_t *dev_handle = (NVC_Dev_t *)dev;
	dev_handle->st_type = st;
    
  ret = NVC_PTZ_Get_Presets(dev_handle, dev_handle->Preset);
  if (ret != 0)
  {
      return -1;
  }

  if (dev_handle->Preset->_size <= MAX_PRESET_NUM)
  {
    int i = 0;    

    fprintf(stderr, "cruise_token :%s\n", dev_handle->Preset->ptz_tour_token);
	  memset(preset, 0, sizeof(nvc_preset_param_t));
    for (i = 0; i < dev_handle->Preset->_size; i++)
    {      
        if (strlen(dev_handle->Preset->_preset[i].name) > 1)
        {
            preset->preset_set[preset->num].enable = 1;
            preset->preset_set[preset->num].no     = i + 1;
		        sprintf(preset->preset_set[preset->num].name, ONVIF_PRESET_NAME_FMT, i+1);
            
            preset->num++;
        }
    }
  }
  
  return 0;
}

int32_t nvc_ptz_ctl(void *dev, int ch, int st, nvc_ptz_ctl_t *ctl)
{
	int ret = 0;
	nvc_ptz_ctl_t *ptz_ctl = ctl;

	if (dev == NULL)
	{
		return -1;	
	}

	NVC_Dev_t *dev_handle = (NVC_Dev_t *)dev;
	dev_handle->st_type = st;

	switch (ptz_ctl->cmd)
	{
		case NVC_PTZ_UP_START: /**0x001*/
        {
          NVC_PTZ_Speed_t nvc_ptz_speed;
          nvc_ptz_speed.pantilt.x = 0.0;
          nvc_ptz_speed.pantilt.y = 0.0 + (ptz_ctl->speed / 10.0);
          nvc_ptz_speed.zoom.x = 0.0; 
          ret = NVC_PTZ_ContinusMove(dev_handle, &nvc_ptz_speed);
        }
			break;
		case NVC_PTZ_UP_STOP:
        {
          NVC_PTZ_Stop_t ptz_stop;
          ptz_stop.en_pant = __true;
          ptz_stop.en_zoom = __false;
          ret = NVC_PTZ_Stop(dev_handle, &ptz_stop);
		    }
        break;
		case NVC_PTZ_DOWN_START: /**0x003*/
        {
          NVC_PTZ_Speed_t nvc_ptz_speed;
    			nvc_ptz_speed.pantilt.x = 0.0;
    			nvc_ptz_speed.pantilt.y = 0.0 - (ptz_ctl->speed / 10.0);
    			nvc_ptz_speed.zoom.x = 0.0;
    			ret = NVC_PTZ_ContinusMove(dev_handle, &nvc_ptz_speed);
  		  }
        break;
		case NVC_PTZ_DOWN_STOP:
        {
          NVC_PTZ_Stop_t ptz_stop;
          ptz_stop.en_pant = __true;
          ptz_stop.en_zoom = __false;
          ret = NVC_PTZ_Stop(dev_handle, &ptz_stop);
		    }
        break;
		case NVC_PTZ_LEFT_START: /**0x005*/
        {
          NVC_PTZ_Speed_t nvc_ptz_speed;
    			nvc_ptz_speed.pantilt.x = 0.0 - (ptz_ctl->speed / 10.0);
    			nvc_ptz_speed.pantilt.y = 0.0;
    			nvc_ptz_speed.zoom.x = 0.0;
          ret = NVC_PTZ_ContinusMove(dev_handle, &nvc_ptz_speed);
		    }
        break;
		case NVC_PTZ_LEFT_STOP:
        {
          NVC_PTZ_Stop_t ptz_stop;
          ptz_stop.en_pant = __true;
          ptz_stop.en_zoom = __false;
          ret = NVC_PTZ_Stop(dev_handle, &ptz_stop);
		    }
        break;
		case NVC_PTZ_RIGHT_START: /**0x007*/
        {
          NVC_PTZ_Speed_t nvc_ptz_speed;
    			nvc_ptz_speed.pantilt.x = 0.0 + (ptz_ctl->speed / 10.0);
    			nvc_ptz_speed.pantilt.y = 0.0;
    			nvc_ptz_speed.zoom.x = 0.0;
          ret = NVC_PTZ_ContinusMove(dev_handle, &nvc_ptz_speed);
  		  }
        break;
		case NVC_PTZ_RIGHT_STOP:
        {
          NVC_PTZ_Stop_t ptz_stop;
          ptz_stop.en_pant = __true;
          ptz_stop.en_zoom = __false;
          ret = NVC_PTZ_Stop(dev_handle, &ptz_stop);
		    }
        break;
		case NVC_PTZ_LEFT_UP_START: /**0x009*/
        {
          NVC_PTZ_Speed_t nvc_ptz_speed;
    			nvc_ptz_speed.pantilt.x = 0.0 - (ptz_ctl->speed / 10.0);
    			nvc_ptz_speed.pantilt.y = 0.0 + (ptz_ctl->speed / 10.0);
    			nvc_ptz_speed.zoom.x = 0.0;
          ret = NVC_PTZ_ContinusMove(dev_handle, &nvc_ptz_speed);
		    }
        break;
		case NVC_PTZ_LEFT_UP_STOP:
        {
          NVC_PTZ_Stop_t ptz_stop;
          ptz_stop.en_pant = __true;
          ptz_stop.en_zoom = __false;
          ret = NVC_PTZ_Stop(dev_handle, &ptz_stop);
		    }
        break;
		case NVC_PTZ_RIGHT_UP_START: /**0x00b*/
        {
          NVC_PTZ_Speed_t nvc_ptz_speed;
    			nvc_ptz_speed.pantilt.x = 0.0 + (ptz_ctl->speed / 10.0);
    			nvc_ptz_speed.pantilt.y = 0.0 - (ptz_ctl->speed / 10.0);
    			nvc_ptz_speed.zoom.x = 0.0;
          ret = NVC_PTZ_ContinusMove(dev_handle, &nvc_ptz_speed);
		    }
        break;
		case NVC_PTZ_RIGHT_UP_STOP:
        {
          NVC_PTZ_Stop_t ptz_stop;
          ptz_stop.en_pant = __true;
          ptz_stop.en_zoom = __false;
          ret = NVC_PTZ_Stop(dev_handle, &ptz_stop);
		    }
        break;
		case NVC_PTZ_LEFT_DOWN_START: /**0x00d*/
        {
          NVC_PTZ_Speed_t nvc_ptz_speed;
    			nvc_ptz_speed.pantilt.x = 0.0 - (ptz_ctl->speed / 10.0);
    			nvc_ptz_speed.pantilt.y = 0.0 - (ptz_ctl->speed / 10.0);
    			nvc_ptz_speed.zoom.x = 0.0;
          ret = NVC_PTZ_ContinusMove(dev_handle, &nvc_ptz_speed);
		    }
        break;
		case NVC_PTZ_LEFT_DOWN_STOP:
        {
          NVC_PTZ_Stop_t ptz_stop;
          ptz_stop.en_pant = __true;
          ptz_stop.en_zoom = __false;
          ret = NVC_PTZ_Stop(dev_handle, &ptz_stop);
		    }
        break;
		case NVC_PTZ_RIGHT_DOWN_START: /**0x00f*/
        {
          NVC_PTZ_Speed_t nvc_ptz_speed;
    			nvc_ptz_speed.pantilt.x = 0.0 + (ptz_ctl->speed / 10.0);
    			nvc_ptz_speed.pantilt.y = 0.0 - (ptz_ctl->speed / 10.0);
    			nvc_ptz_speed.zoom.x = 0.0;
          ret = NVC_PTZ_ContinusMove(dev_handle, &nvc_ptz_speed);
		    }
        break;
		case NVC_PTZ_RITHT_DOWN_STOP:
        {
          NVC_PTZ_Stop_t ptz_stop;
          ptz_stop.en_pant = __true;
          ptz_stop.en_zoom = __false;
          ret = NVC_PTZ_Stop(dev_handle, &ptz_stop);
		    }
        break;
    case NVC_PTZ_PRESET_SET:
        {
          if (dev_handle->Preset->_size <= MAX_PRESET_NUM)
          {
              PTZ_Preset_t _preset;
              //memset(_preset.name, 0, sizeof(_preset.name));
              memset(&_preset, 0, sizeof(PTZ_Preset_t));
              strncpy(_preset.name,  ptz_ctl->name, sizeof(_preset.name));
              _preset.name[MAX_NAME_LEN - 1 ] = 0;
		          sprintf(_preset.token, "%d", ptz_ctl->val);
              
              ret = NVC_PTZ_Preset_set(dev_handle, &_preset);
              ret |= NVC_PTZ_Get_Presets(dev_handle, dev_handle->Preset);
          }
          else
          {
               ret = -1;
          }
        }
        break;
    case NVC_PTZ_PRESET_CALL:
        {
          int i = 0, nSet = 0;
          int preset_no = 0, preset_no_0 = 0, nRet = 0;
          char name_buf[MAX_NAME_LEN] = {0};
          for(i = 0; i < dev_handle->Preset->_size; i++)
          {
		        nSet = 0;					
            nSet = (strcmp(dev_handle->Preset->_preset[i].name, ptz_ctl->name) == 0) ? 1 : 0;
              
      			nRet = sscanf(dev_handle->Preset->_preset[i].name, "Preset%d", &preset_no);
      			if (0 < nRet)
      			{
      				nRet = sscanf(ptz_ctl->name, "PtzName_%d", &preset_no_0);
      				
      				if (0 < nRet && (preset_no_0 == preset_no))
      				{
      					nSet = 1;
      				}
      			}
		
		
		        if (1 == nSet)
            {
                ret = NVC_PTZ_Preset_call(dev_handle, &(dev_handle->Preset->_preset[i]));
                break;
            }
          }

          if (i == dev_handle->Preset->_size)
          {
          	PTZ_Preset_t preset_excp;
          	memset(&preset_excp, 0, sizeof(PTZ_Preset_t));
      			nRet = sscanf(ptz_ctl->name, "PtzName_%d", &preset_no_0);
      			if (nRet > 0 & preset_no_0 < MAX_PRESET_NUM)
      			{
      				sprintf(preset_excp.name, "PtzName_%d", preset_no_0);
      				sprintf(preset_excp.token, "%d", preset_no_0);
      				preset_excp.staytime = 0;
            }
          }
        }
        break;
    case NVC_PTZ_PRESET_DEL:
        {
          if (dev_handle->Preset->_size > 0)
          {
              int i = 0;
              for(i = 0; i < dev_handle->Preset->_size; i++)
              {
                  if (strcmp(dev_handle->Preset->_preset[i].name, ptz_ctl->name) == 0)
                  {
                      ret = NVC_PTZ_Preset_del(dev_handle, &(dev_handle->Preset->_preset[i]));
                      ret |= NVC_PTZ_Get_Presets(dev_handle, dev_handle->Preset);
                      break;
                  }
              }
          }
          else
          {
              printf("[%s][%d]===> Preset->size: %d, MAX_PRESET_NUM: %d, <= 0\n"
                  , __func__, __LINE__
                  , dev_handle->Preset->_size
                  , MAX_PRESET_NUM);
              ret = -1;
          }
        }
        break;
		case NVC_PTZ_ZOOM_ADD_START: /**0x014*/
        {
          NVC_PTZ_Speed_t nvc_ptz_speed;
    			nvc_ptz_speed.pantilt.x = 0.0;
    			nvc_ptz_speed.pantilt.y = 0.0;
    			nvc_ptz_speed.zoom.x = 0.0 + (ptz_ctl->speed / 10.0); 
          ret = NVC_PTZ_ContinusMove(dev_handle, &nvc_ptz_speed);
		    }
        break;
		case NVC_PTZ_ZOOM_ADD_STOP:
        {
          NVC_PTZ_Stop_t ptz_stop;
          ptz_stop.en_pant = __false;
          ptz_stop.en_zoom = __true;
          ret = NVC_PTZ_Stop(dev_handle, &ptz_stop);
		    }
        break;
		case NVC_PTZ_ZOOM_SUB_START: /**0x016*/
    		{
          NVC_PTZ_Speed_t nvc_ptz_speed;
          nvc_ptz_speed.pantilt.x = 0.0;
    			nvc_ptz_speed.pantilt.y = 0.0;
    			nvc_ptz_speed.zoom.x = 0.0 - (ptz_ctl->speed / 10.0); 
          ret = NVC_PTZ_ContinusMove(dev_handle, &nvc_ptz_speed);
    		}
        break;
		case NVC_PTZ_ZOOM_SUB_STOP:
        {
          NVC_PTZ_Stop_t ptz_stop;
          ptz_stop.en_pant = __false;
          ptz_stop.en_zoom = __true;
          ret = NVC_PTZ_Stop(dev_handle, &ptz_stop);
		    }
        break;
		default:
			  break;
	}
	
	if (ret != 0)
	{
		return -1;	
	}
	return 0;
}

int32_t nvc_snap_url_get(void *dev, int ch, int st, char *url)
{
  int ret = 0;
  NVC_Snapshot_t nvc_pic;
  memset(&nvc_pic, 0, sizeof(NVC_Snapshot_t));
  
  if (dev == NULL)
  {
      return -1;
  }

  NVC_Dev_t *dev_handle = (NVC_Dev_t *)dev;
  dev_handle->st_type = st;

  ret = NVC_Get_SnapshotUri(dev_handle, &nvc_pic);
  if (ret != 0)
  {
      return -1;
  }
  strncpy(url, nvc_pic.url, MAX_URL_LEN-1);
  
  return 0;
}

int32_t nvc_systime_set(void *dev, int ch, int st, nvc_time_t *t)
{
    int ret = 0;

    if (dev == NULL)
    {
        return -1;
    }

    NVC_Dev_t *dev_handle = (NVC_Dev_t *)dev;

    ret = NVC_Set_SystemTime(dev_handle, (NVC_time_t*)t);
    if(ret != 0)
    {
        return -1;
    }

    return 0;
}

///////////////////////////////////////////////////////////////

nvt_t *g_nvt = NULL;

///////////////////////////////////////////////////////////////

int get_dev_info(void);
int get_dev_profiles(int chs, onvif_profile_t *profile, int num, int flag);//get all profiles
int get_vsc_profiles(int chs, video_source_conf_t *info, int num);//video source configure
int get_asc_profiles(int chs, audio_source_conf_t *info, int num);//audio source configure
int get_vec_profiles(int chs, video_encoder_conf_t *info, int num);//video encoder configure
int get_aec_profiles(int chs, audio_encoder_conf_t *info, int num);//audio encoder configure
int get_meta_profiles(int chs, metadata_conf_t *info, int num);//metadata configure
int get_ptz_profiles(int chs, PTZ_conf_t *info, int num);//PTZ configure profiles
int get_dev_capability(device_capability_t *dev_cap, int streamtype);
int set_vsc_conf(int chs, video_source_conf_t *info);
int set_vec_conf(int chs, video_encoder_conf_t *info);
int set_aec_conf(int chs, audio_encoder_conf_t *info);
int get_recording_list(nvt_time_t starttime, nvt_time_t endtime);
int get_dev_time(systime_t *time);
int set_dev_time(systime_t *time);
int get_ntp_info(char *ntp1, char *ntp2);
int set_ntp_info(char *ntp1, char *ntp2);
int get_dev_netinfo(net_info_t *info);
int set_dev_netinfo(net_info_t *info);
int get_user_info(user_info_t *info);
int add_user_info(char *name, char *passwd);
int del_user_info(char *name);
int set_user_info(char *name, char *passwd);
int get_device_syslog(int type, char *syslog);
int device_reboot();
int get_dev_portinfo(port_info_t *info);
int set_dev_portinfo(port_info_t *info);
int get_curr_chs();
int set_imaging_conf(int chs, imaging_conf_t *img);
int get_imaging_conf(int chs, imaging_conf_t *img, int num);
int get_recording_info(recording_info_t *info, char *token);
int set_device_factorydefault(int mask);

int32_t nvt_start(void)
{
  system("route add -net 239.255.255.0 netmask 255.255.255.0 dev eth0");
  
  if (pthread_create(&g_nvt->discovery_id, NULL, start_nvt_discovery, NULL) < 0) 
  {
      return -1;
  }

  if(pthread_create(&g_nvt->serve_id, NULL, start_nvt_serve, NULL) < 0)
  {
      return -1;
  }
  printf("%s => start ok.\n", __func__);
  return 0;
}

int32_t nvt_stop(void)
{
  printf("%s => stop ...\n", __func__);
  stop_nvt_discovery();
  stop_nvt_serve();
}

int set_default_token(int ch_num)
{
    int num, i;
    char VSCname[SMALL_INFO_LENGTH];
    char VSCtoken[SMALL_INFO_LENGTH];
    char sourcetoken[SMALL_INFO_LENGTH];
    char VECname[SMALL_INFO_LENGTH];
    char VECtoken[SMALL_INFO_LENGTH];
    char pname[SMALL_INFO_LENGTH];
    char ptoken[SMALL_INFO_LENGTH];
    
    memset(VSCname, 0, SMALL_INFO_LENGTH);
    memset(VSCtoken, 0, SMALL_INFO_LENGTH);
    memset(sourcetoken, 0, SMALL_INFO_LENGTH);
    memset(VECname, 0, SMALL_INFO_LENGTH);
    memset(VECtoken, 0, SMALL_INFO_LENGTH);
    memset(pname, 0, SMALL_INFO_LENGTH);
    memset(ptoken, 0, SMALL_INFO_LENGTH);
    
    num = ch_num;
    num = num * 2;
    for(i = 0; i < num; i++)
    {
        sprintf(sourcetoken, "CH%02dVideoSourceToken", i/2);
        sprintf(VSCname, "CH%02dVideoConfigureName", i/2);
        sprintf(VSCtoken, "CH%02dVideoConfigureToken", i/2);

        if(i%2 == 0)
        {        
            sprintf(pname, "CH%02dProfileName", i/2);
            sprintf(ptoken, "CH%02dProfileToken", i/2);
            sprintf(VECname, "CH%02dVideoEncodeName", i/2);
            sprintf(VECtoken, "CH%02dVideoEncodeToken", i/2);
        }
        else
        {
            sprintf(pname, "CH%02dSubProfileName", i/2);
            sprintf(ptoken, "CH%02dSubProfileToken", i/2);
            sprintf(VECname, "CH%02dSubVideoEncodeName", i/2);
            sprintf(VECtoken, "CH%02dSubVideoEncodeToken", i/2);
        }

        strcpy(g_nvt->t.profiles[i].vsc->vname, VSCname);
        strcpy(g_nvt->t.profiles[i].vsc->vtoken, VSCtoken);
        strcpy(g_nvt->t.profiles[i].vsc->vsourcetoken, sourcetoken);
        strcpy(g_nvt->t.profiles[i].vec->name, VECname);
        strcpy(g_nvt->t.profiles[i].vec->token, VECtoken);
        strcpy(g_nvt->t.profiles[i].pname, pname);
        strcpy(g_nvt->t.profiles[i].ptoken, ptoken);
    }
    return 0;
}

int32_t nvt_init(nvt_parm_t *parm)
{
  int i;
  g_nvt = (nvt_t*)calloc(1, sizeof(nvt_t));
  if(!g_nvt)
      return -1;
  
  memcpy(&g_nvt->p, parm, sizeof(nvt_parm_t));

  g_nvt->t.dev = (device_info_t*)calloc(1, sizeof(device_info_t));
  
  g_nvt->t.nprofiles = MAX_PROFILES;
  g_nvt->t.profiles = (onvif_profile_t*)calloc(1, g_nvt->t.nprofiles * sizeof(onvif_profile_t));

  for(i = 0; i < MAX_PROFILES; i++)
  {
      g_nvt->t.profiles[i].vsc = (video_source_conf_t *)calloc(1, sizeof(video_source_conf_t));
      g_nvt->t.profiles[i].asc = (audio_source_conf_t *)calloc(1, sizeof(audio_source_conf_t));
      g_nvt->t.profiles[i].vec = (video_encoder_conf_t *)calloc(1, sizeof(video_encoder_conf_t));
      g_nvt->t.profiles[i].aec = (audio_encoder_conf_t *)calloc(1, sizeof(audio_encoder_conf_t));
      g_nvt->t.profiles[i].img = (imaging_conf_t *)calloc(1, sizeof(imaging_conf_t)); 
      //add by lyx
  	  g_nvt->t.profiles[i].PTZc = (PTZ_conf_t *)calloc(1, sizeof(PTZ_conf_t));
  }
  //get_dev_profiles(ALL_CHS,  g_nvt->t.profiles, 2, VSC | ASC | VEC | AEC);
  //get_dev_info();
  set_default_token(g_nvt->p.ch_num);
  return 0;
}
int32_t nvt_uninit(void)
{
  int i;    
  if(g_nvt)   
  {         
    for(i = 0; i < MAX_PROFILES; i++)   
    {   
        if(g_nvt->t.profiles[i].img) free(g_nvt->t.profiles[i].img);   
        if(g_nvt->t.profiles[i].vsc) free(g_nvt->t.profiles[i].vsc);     
        if(g_nvt->t.profiles[i].asc) free(g_nvt->t.profiles[i].asc);     
        if(g_nvt->t.profiles[i].vec) free(g_nvt->t.profiles[i].vec);      
        if(g_nvt->t.profiles[i].aec) free(g_nvt->t.profiles[i].aec);
    }       
    if(g_nvt->t.profiles) free(g_nvt->t.profiles);
    
    if(g_nvt->t.dev) free(g_nvt->t.dev);

    free(g_nvt);        
    g_nvt = NULL;   
  }
}

///////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////

int get_dev_info()
{
  strncpy(g_nvt->t.dev->hwid    , "hiview", INFO_LEN);
  strncpy(g_nvt->t.dev->serial  , "123456", INFO_LEN);
  strncpy(g_nvt->t.dev->sw      , "1.2.3" , INFO_LEN);
  return 0;
}

int get_curr_chs()
{ 
   return g_nvt->p.ch_num;
}

static int encoding_interval = 1;

int get_vsc_profiles(int chs, video_source_conf_t *info, int num)
{
    char name[SMALL_INFO_LENGTH];
    char token[SMALL_INFO_LENGTH];
    char sourcetoken[SMALL_INFO_LENGTH];
    int ret = 0;
    
    NET_IPV4 ip;
    char _IPAddr[LARGE_INFO_LENGTH] = {0};

    ip.int32 = net_get_ifaddr(ETH_NAME);
    sprintf(_IPAddr, "%d.%d.%d.%d", ip.str[0], ip.str[1], ip.str[2], ip.str[3]);

    memset(name, 0, sizeof(SMALL_INFO_LENGTH));
    memset(token, 0, sizeof(SMALL_INFO_LENGTH));
    memset(sourcetoken, 0, sizeof(SMALL_INFO_LENGTH));
    
    port_info_t port_info;
    ret = get_dev_portinfo(&port_info);
    if(ret < 0)
    {
      fprintf(stderr, "[get_vsc_profiles]====> not get port\n\n");
      port_info.rtsp_port = 554;//set default value
    }
    
    sprintf(sourcetoken, "CH%02dVideoSourceToken", chs/2);
    sprintf(name, "CH%02dVideoConfigureName", chs/2);
    sprintf(token, "CH%02dVideoConfigureToken", chs/2);
    strcpy(info->vname, name);
    strcpy(info->vtoken, token);
    strcpy(info->vsourcetoken, sourcetoken);
    sprintf(info->uri, "rtsp://%s:%d/%d/%d", _IPAddr, port_info.rtsp_port, chs/2, chs%2);

    info->vcount = num;
    info->x = 0;
    info->y = 0;
    info->width = 1920;
    info->heigth = 1080;
    
    return 0;
}

int get_imaging_conf(int chs, imaging_conf_t *img, int num)
{
  int ret = 0;
  GSF_MSG_DEF(gsf_img_csc_t, csc, sizeof(gsf_msg_t)+sizeof(gsf_img_csc_t));
  ret = GSF_MSG_SENDTO(GSF_ID_CODEC_IMGCSC, chs/2
                        , GET, 0, sizeof(gsf_img_csc_t)
                        , GSF_IPC_CODEC, 2000);
                        
  img->brightness = csc->u8Luma;
  img->ColorSaturation = csc->u8Satu;
  img->contrast = csc->u8Contr;
  img->sharpness = 50;
  img->IrCtFilter= 0;

  return ret;

}

int get_asc_profiles(int chs, audio_source_conf_t *info, int num)
{
    char name[SMALL_INFO_LENGTH];
    char token[SMALL_INFO_LENGTH];
    char sourcetoken[SMALL_INFO_LENGTH];

    memset(name, 0, sizeof(SMALL_INFO_LENGTH));
    memset(token, 0, sizeof(SMALL_INFO_LENGTH));
    memset(sourcetoken, 0, sizeof(SMALL_INFO_LENGTH));

    sprintf(name, "CH%02dAudioConfigureName", chs/2);
    sprintf(token, "CH%02dAudioConfigureToken", chs/2);
    sprintf(sourcetoken, "CH%02dAudioSourceToken", chs/2);
    strcpy(info->aname, name);
    strcpy(info->token, token);
    strcpy(info->sourcetoken, sourcetoken);
    info->count = num;
    return 0;
}

int get_vec_profiles(int chs, video_encoder_conf_t *info, int num)
{
    char name[SMALL_INFO_LENGTH];
    char token[SMALL_INFO_LENGTH];
    int ret = 0;
    
    memset(name, 0, sizeof(SMALL_INFO_LENGTH));
    memset(token, 0, sizeof(SMALL_INFO_LENGTH));
    printf("%s => GET chs:%d\n", __func__, chs);
    if(ret == 0)
    {
        if(chs%2 == 0)
        {
          
          GSF_MSG_DEF(gsf_venc_t, venc, sizeof(gsf_msg_t)+sizeof(gsf_venc_t));
          ret = GSF_MSG_SENDTO(GSF_ID_CODEC_VENC, chs/2, GET, 0, 0, GSF_IPC_CODEC, 2000);
          if(ret == 0)
          {  
            info->BitrateLimit  = venc->bitrate;
            info->gLength       = venc->gop;
            info->hProfile      = venc->profile;
            info->quality       = venc->qp;
            info->FrameRateLimit= venc->fps;
            info->width = venc->width;
            info->height = venc->height;
          }
          printf("%s => GET main ret:%d, fps:%d\n", __func__, ret, info->FrameRateLimit);
        }
        else
        {
          GSF_MSG_DEF(gsf_venc_t, venc, sizeof(gsf_msg_t)+sizeof(gsf_venc_t));
          ret = GSF_MSG_SENDTO(GSF_ID_CODEC_VENC, chs/2, GET, 1, 0, GSF_IPC_CODEC, 2000);
          if(ret == 0)
          {  
            info->BitrateLimit  = venc->bitrate;
            info->gLength       = venc->gop;
            info->hProfile      = venc->profile;
            info->quality       = venc->qp;
            info->FrameRateLimit= venc->fps;
            info->width = venc->width;
            info->height = venc->height;
          }
          printf("%s => GET second ret:%d, fps:%d\n", __func__, ret, info->FrameRateLimit);
        }
    }
    else
    {
        info->width = 1920;
        info->height = 1080;
        info->BitrateLimit = 2048;
        info->gLength = 1;
        info->hProfile = 0;
        info->quality = 1;
    }

    info->encoding = _H264;
    info->usecount = num;
    info->EncodingInterval = encoding_interval;
    info->MultiConf.autostart = 0;
    info->MultiConf.type = 0;
    info->MultiConf.port = g_nvt->p.port;
    info->MultiConf.ttl = 0;
    info->SessionTimeout = 720000;
    info->GovLength = 1;
    info->mProfile = SP;
    return 0;
}

static int audio_samplerate = 1;
static int audio_bitrate = 64;
int get_aec_profiles(int chs, audio_encoder_conf_t *info, int num)
{
    char name[SMALL_INFO_LENGTH];
    char token[SMALL_INFO_LENGTH];

    memset(name, 0, sizeof(SMALL_INFO_LENGTH));
    memset(token, 0, sizeof(SMALL_INFO_LENGTH));

    sprintf(name, "CH%02dAudioEncodeName", chs/2);
    sprintf(token, "CH%02dAudioEncodeToken", chs/2);

    strcpy(info->name, name);
    strcpy(info->token, token);
    info->encoding = _G711;
    info->bitrate = audio_bitrate;
    info->samplerate = audio_samplerate;
    info->MultiConf.autostart = 0;
    info->MultiConf.type = 0;
    info->MultiConf.port = g_nvt->p.port;
    info->MultiConf.ttl = 0;
    info->SessionTimeout = 72000;
    info->usecount = num;
    return 0;
}

int get_meta_profiles(int chs, metadata_conf_t *info, int num)
{
  return 0;
}
int get_ptz_profiles(int chs, PTZ_conf_t *info, int num)
{
  
  return 0;
}







static int firstGetParamFlag = 1;
static time_t lastGetParamTm;

int get_dev_profiles(int chs, onvif_profile_t *profile, int num, int flag)
{
    int i, t;
    char name[SMALL_INFO_LENGTH];
    char token[SMALL_INFO_LENGTH];
    time_t currTime;
    memset(name, 0, sizeof(SMALL_INFO_LENGTH));
    memset(token, 0, sizeof(SMALL_INFO_LENGTH));

    printf("[%s]=========> in  firstGetParamFlag=%d\n", __FUNCTION__, firstGetParamFlag);
    
    if(chs == ALL_CHS)
    {
        if(firstGetParamFlag == 1)
        {
            lastGetParamTm = time(((time_t *)NULL));
            firstGetParamFlag = 0;
        }
        else 
        {
            currTime = time(((time_t *)NULL));
            
            printf("[%s]=========> (currTime-lastGetParamTm)=%d \n",  __FUNCTION__, (currTime - lastGetParamTm));
            if((currTime - lastGetParamTm) < 6*60)
            {
                return 0;
            }
            else
            {
                lastGetParamTm = currTime;
            }
        }
    }
    
    printf("[%s]=========> qqqqqqqq\n", __FUNCTION__);
    
    if(chs == ALL_CHS)
    {
        num = get_curr_chs();
        num = num * 2;
        for(i = 0; i < num; i++)
        {
            profile[i].pfixed = __false;
            if(flag&VSC)
            {
                get_vsc_profiles(i, profile[i].vsc, num);
            }
            if(flag&ASC)
            {
                get_asc_profiles(i, profile[i].asc, num);
            }
            if(flag&VEC)
            {
                get_vec_profiles(i, profile[i].vec, num);
            }
            if(flag&AEC)
            {
                get_aec_profiles(i, profile[i].aec, num);
            }
        }
    }
    else
    {
        if(chs%2 == 0)
        {
            sprintf(name, "CH%02dProfileName", chs/2);
            sprintf(token, "CH%02dProfileToken", chs/2);
        }
        else
        {
            sprintf(name, "CH%02dSubProfileName", chs/2);
            sprintf(token, "CH%02dSubProfileToken", chs/2);
        }
    
        strcpy(profile->pname, name);
        strcpy(profile->ptoken, token);

        profile->pfixed = __false;
        get_vsc_profiles(chs, profile->vsc, num);
        get_asc_profiles(chs, profile->asc, num);
        get_vec_profiles(chs, profile->vec, num);
        get_aec_profiles(chs, profile->aec, num);
    }
    printf("[%s]=========> out\n", __FUNCTION__);
    return 0;
}

int set_vsc_conf(int chs, video_source_conf_t *info)
{
    int ret = 0;

    if(ret == 0)
    {
       if(chs%2 == 0)
       {
          ;
       }
       else
       {
          ;
       }
    }
    else
    {
        fprintf(stderr, "[%s] chs %d has no video stream\n", __FUNCTION__, chs/2);
    }
 
    return ret;
}

int set_vec_conf(int chs, video_encoder_conf_t *info)
{
    int ret = 0;
    
    if(ret == 0)
    {
        if(chs%2 == 0)
        {
          
          GSF_MSG_DEF(gsf_venc_t, venc, sizeof(gsf_msg_t)+sizeof(gsf_venc_t));
          ret = GSF_MSG_SENDTO(GSF_ID_CODEC_VENC, chs/2, GET, 0, 0, GSF_IPC_CODEC, 2000);
          if(ret == 0)
          {  
            venc->width = info->width;
            venc->height = info->height;
            venc->fps = info->FrameRateLimit;
            venc->gop = info->gLength;
            venc->bitrate = info->BitrateLimit;
            venc->profile = info->hProfile;
            venc->qp = info->quality;

            ret = GSF_MSG_SENDTO(GSF_ID_CODEC_VENC, 0, SET, 0, sizeof(gsf_venc_t), GSF_IPC_CODEC, 2000);
            printf("%s => SET main ret:%d\n", __func__, ret);
          }
        }
        else
        {
          GSF_MSG_DEF(gsf_venc_t, venc, sizeof(gsf_msg_t)+sizeof(gsf_venc_t));
          ret = GSF_MSG_SENDTO(GSF_ID_CODEC_VENC, chs/2, GET, 1, 0, GSF_IPC_CODEC, 2000);
          if(ret == 0)
          {  
            venc->width = info->width;
            venc->height = info->height;
            venc->fps = info->FrameRateLimit;
            venc->gop = info->gLength;
            venc->bitrate = info->BitrateLimit;
            venc->profile = info->hProfile;
            venc->qp = info->quality;

            ret = GSF_MSG_SENDTO(GSF_ID_CODEC_VENC, 0, SET, 1, sizeof(gsf_venc_t), GSF_IPC_CODEC, 2000);
            printf("%s => SET second ret:%d\n", __func__, ret);
          }
        }
    }
    else
    {
        fprintf(stderr, "[%s] chs %d has no video stream\n", __FUNCTION__, chs/2);
    }

    return ret;
}

int set_aec_conf(int chs, audio_encoder_conf_t *info)
{
    int ret = 0;
#if 0
    sdk_encode_t *videoconf;
    
    videoconf = ((sdk_msg_t*)msg_buf)->data;
    videoconf->main.audio_enc = info->encoding;
    audio_bitrate = info->bitrate;
    audio_samplerate = info->samplerate;
#endif
    return ret;   
}

int set_imaging_conf(int chs, imaging_conf_t *img)
{
  int ret = 0;
  GSF_MSG_DEF(gsf_img_csc_t, csc, sizeof(gsf_msg_t)+sizeof(gsf_img_csc_t));
  
  csc->bEnable = 1;
  csc->u8Hue = 50;
  csc->u8Luma = img->brightness;
  csc->u8Contr = img->contrast;
  csc->u8Satu = img->ColorSaturation;

  ret = GSF_MSG_SENDTO(GSF_ID_CODEC_IMGCSC, chs/2
                        , SET, 0, sizeof(gsf_img_csc_t)
                        , GSF_IPC_CODEC, 2000);
  return ret;
}

int get_dev_time(systime_t *_time)
{
    int ret = 0;
    
    time_t _t = time(NULL);
    struct tm _tm, _gm;
    gmtime_r(&_t, &_gm);
    localtime_r(&_t, &_tm);
    
    char *z = getenv("TZ");
    if(z && strlen(z))
    {
      char s = '+';
      int hour = 0, min = 0;
      sscanf(z, "UTC%c%d:%02d", &s, &hour, &min);
      sprintf(_time->TZ, "UTC%c%02d:%02d", s, hour, min);
    }
    
    _time->datetype = Manual;
    _time->daylightsaving = 0;
    _time->UTCDateTime.year   = _gm.tm_year + 1900;
    _time->UTCDateTime.month  = _gm.tm_mon + 1;
    _time->UTCDateTime.day    = _gm.tm_mday;
    _time->UTCDateTime.hour   = _gm.tm_hour;
    _time->UTCDateTime.minute = _gm.tm_min;
    _time->UTCDateTime.second = _gm.tm_sec;
    
    printf("%s => UTC [%04d-%02d-%02d %02d:%02d:%02d %s] ret:%d\n", __func__
          , _time->UTCDateTime.year, _time->UTCDateTime.month, _time->UTCDateTime.day
          , _time->UTCDateTime.hour, _time->UTCDateTime.minute,_time->UTCDateTime.second, _time->TZ
          , ret);    
    
    _time->LocalDateTime.year   = _tm.tm_year + 1900;
    _time->LocalDateTime.month  = _tm.tm_mon + 1;
    _time->LocalDateTime.day    = _tm.tm_mday;
    _time->LocalDateTime.hour   = _tm.tm_hour;
    _time->LocalDateTime.minute = _tm.tm_min;
    _time->LocalDateTime.second = _tm.tm_sec;
    
    printf("%s => LOCAL [%04d-%02d-%02d %02d:%02d:%02d %s] ret:%d\n", __func__
          , _time->LocalDateTime.year, _time->LocalDateTime.month, _time->LocalDateTime.day
          , _time->LocalDateTime.hour, _time->LocalDateTime.minute,_time->LocalDateTime.second, _time->TZ
          , ret);
    return 0;
}

int set_dev_time(systime_t *_time)
{
  int ret = 0;
  
  GSF_MSG_DEF(gsf_time_t, t, sizeof(gsf_msg_t)+sizeof(gsf_time_t));
  
  struct tm e2;
    
  e2.tm_year = _time->UTCDateTime.year - 1900;
  e2.tm_mon  = _time->UTCDateTime.month - 1;
  e2.tm_mday = _time->UTCDateTime.day;
  e2.tm_hour = _time->UTCDateTime.hour;
  e2.tm_min  = _time->UTCDateTime.minute;
  e2.tm_sec  = _time->UTCDateTime.second;
    
  printf("%s => UTC [%04d-%02d-%02d %02d:%02d:%02d %s]\n", __func__
        , e2.tm_year + 1900, e2.tm_mon + 1, e2.tm_mday
        , e2.tm_hour, e2.tm_min,e2.tm_sec, _time->TZ);
  
  time_t _new = timegm(&e2);
  localtime_r(&_new, &e2);
  
  t->year   = e2.tm_year;
  t->month  = e2.tm_mon; 
  t->day    = e2.tm_mday;
  t->hour   = e2.tm_hour;
  t->minute = e2.tm_min; 
  t->second = e2.tm_sec;
  
  if(strlen(_time->TZ))
  { 
    char s = '+';
    int hour = 0, min = 0;
    sscanf(_time->TZ, "UTC%c%d:%02d", &s, &hour, &min);
    t->zone = (hour*60 + min) * (s=='+'?1:-1);
  }
  ret = GSF_MSG_SENDTO(GSF_ID_BSP_TIME, 0, SET, 0, sizeof(gsf_time_t), GSF_IPC_BSP, 2000);
  printf("%s => LOCAL [%04d-%02d-%02d %02d:%02d:%02d %s]\n", __func__
        , e2.tm_year + 1900, e2.tm_mon + 1, e2.tm_mday
        , e2.tm_hour, e2.tm_min,e2.tm_sec, _time->TZ);

  return ret;
}

int get_ntp_info(char *ntp1, char *ntp2)
{
  int ret = 0;
  GSF_MSG_DEF(gsf_ntp_t, ntp, sizeof(gsf_msg_t)+sizeof(gsf_ntp_t));
  ret = GSF_MSG_SENDTO(GSF_ID_BSP_NTP, 0, GET, 0, 0, GSF_IPC_BSP, 2000);
  if(strlen(ntp->server1))
    strcpy(ntp1, &ntp->server1[strlen("server ")]);
  if(strlen(ntp->server2))
    strcpy(ntp2, &ntp->server2[strlen("server ")]);
  return 0;
}
int set_ntp_info(char *ntp1, char *ntp2)
{
  int ret = 0;
  
  if(!strlen(ntp1) && !strlen(ntp2))
    return ret;
 
  GSF_MSG_DEF(gsf_ntp_t, ntp, sizeof(gsf_msg_t)+sizeof(gsf_ntp_t));
  ntp->prog = 600;
  if(strlen(ntp1))
    sprintf(ntp->server1, "server %s", ntp1);
  if(strlen(ntp2))
    sprintf(ntp->server2, "server %s", ntp2);  
  ret = GSF_MSG_SENDTO(GSF_ID_BSP_NTP, 0, SET, 0, sizeof(gsf_ntp_t), GSF_IPC_BSP, 2000);

  return 0;
}



int get_dev_netinfo(net_info_t *info)
{
    int ret = 0;
    GSF_MSG_DEF(gsf_eth_t, eth, sizeof(gsf_msg_t)+sizeof(gsf_eth_t));
    ret = GSF_MSG_SENDTO(GSF_ID_BSP_ETH, 0, GET, 0, 0, GSF_IPC_BSP, 2000);
    printf("%s => ret:%d\n", __func__, ret);
    if(ret < 0)
      return ret;
  	
    info->dhcp_status = eth->dhcp;
    strncpy(info->dns1, eth->dns1, sizeof(info->dns1));
    strncpy(info->dns2, eth->dns2, sizeof(info->dns2));
    strncpy(info->gateway, eth->gateway, sizeof(info->gateway));
    strncpy(info->if_name, "eth0", sizeof(info->if_name));
    strncpy(info->ip_addr, eth->ipaddr, sizeof(info->ip_addr));
    strncpy(info->mac, eth->mac, sizeof(info->mac));
    strncpy(info->mask, eth->netmask, sizeof(info->mask));
    info->dns_auto = eth->dhcp;

    return ret;
}

int get_dev_portinfo(port_info_t *info)
{
    int ret = 0;
    info->http_port = 80;
    info->rtsp_port = 554;
    info->http_enable = TRUE;
    info->rtsp_enable = TRUE;

    return ret;
}

int set_dev_netinfo(net_info_t *info)
{
    int ret = 0;
    GSF_MSG_DEF(gsf_eth_t, eth, sizeof(gsf_msg_t)+sizeof(gsf_eth_t));
    
    eth->dhcp = info->dhcp_status;
    strncpy(eth->gateway, info->gateway, sizeof(info->gateway));
    strncpy(eth->ipaddr, info->ip_addr, sizeof(info->ip_addr));
    strncpy(eth->netmask, info->mask, sizeof(info->mask));
    strncpy(eth->mac, info->mac, sizeof(info->mac));
    sprintf(eth->dns1, "%s", info->dns1);
    sprintf(eth->dns2, "%s", info->dns2);
    
    ret = GSF_MSG_SENDTO(GSF_ID_BSP_ETH, 0, SET, 0, sizeof(gsf_eth_t), GSF_IPC_BSP, 2000);
    printf("%s => ret:%d, info->dhcp_status:%d\n", __func__, ret, info->dhcp_status);
    return ret;
}

int set_dev_portinfo(port_info_t *info)
{
    int ret = 0;
#if 0
	  sdk_net_mng_cfg_t *net_info;         

    net_info->http_port = info->http_port;
    net_info->dvr_data_port = info->rtsp_port;
#endif
    return ret;
}


static int getuser(gsf_user_t *user, int num)
{
  GSF_MSG_DEF(gsf_msg_t, msg, 4*1024);
  int ret = GSF_MSG_SENDTO(GSF_ID_BSP_USER, 0, GET, 0, 0, GSF_IPC_BSP, 2000);
  gsf_user_t *userlist = (gsf_user_t*)__pmsg->data;
  int userlist_num = __pmsg->size/sizeof(gsf_user_t);
  printf("GET GSF_ID_BSP_USER To:%s, ret:%d, userlist num:%d\n"
        , GSF_IPC_BSP, ret, __pmsg->size/sizeof(gsf_user_t));

  if(ret < 0)
    return ret;

  if(user)
    memcpy(user, userlist, ((userlist_num > num)?num:userlist_num)*sizeof(gsf_user_t));

  return ret;
}

int get_user_info(user_info_t * info)
{
    int ret = 0;

    //only admin user for test;
    static gsf_user_t userlist[8];
    if(userlist[0].name[0] == '\0')
    {
      getuser(userlist, 8);
    }
      
    user_right_t *ur = (user_right_t*)info->buf;
    snprintf(ur->user_name, sizeof(ur->user_name)-1, "%s", userlist[0].name);
    snprintf(ur->user_pwd, sizeof(ur->user_pwd)-1, "%s", userlist[0].pwd);
    ur->local_right = ur->remote_right = userlist[0].caps;
    printf("userlist[0] name:[%s], pwd:[%s]\n", userlist[0].name, userlist[0].pwd);
    info->size = 1;

    return ret;
}

int add_user_info(char *name, char *passwd)
{
    int ret = 0;
#if 0
    sdk_user_right_t *info;
    info = ((sdk_msg_t*)msg_buf)->data;
    strcpy(info->user.user_name, name); 
    strcpy(info->user.user_pwd, passwd);
#endif
    return ret;
}

int del_user_info(char *name)
{
    int ret = 0, i = 0;
    
    // find user name;
    // delete user; 

    return ret;
}

int set_user_info(char *name, char *passwd)
{
    int ret = 0;

    return ret;
}

int get_device_syslog(int type, char *syslog)
{
    int ret = 0;
#if 0
    nvt_time_t *time_cfg;
    sdk_log_item_t *log_store;
    // query type log;
    if(ret == 0)
    {
        log_store = ((sdk_msg_t*)msg)->data;
        int size = (((sdk_msg_t*)msg)->size)/(sizeof(sdk_log_item_t));
        //convent struct to string 
        for(i = 0; i < size; i++)
        {
             sprintf(tmpinfo,"LogTime:%04d-%02d-%02d %02d:%02d:%02d, Username:%s, IP:%s, loginfo:"
                    ,log_store[i].time.year
                    ,log_store[i].time.mon
                    ,log_store[i].time.day
                    ,log_store[i].time.hour
                    ,log_store[i].time.min
                    ,log_store[i].time.sec
                    ,log_store[i].user
                    ,log_store[i].ip_addr);
            memcpy(loginfo, log_store[i].desc, sizeof(log_store[i].desc));
            strcat(syslog, tmpinfo);
            strcat(syslog, "\n");
        }
    }
#endif
    return 0;
}

int device_reboot()
{
    fprintf(stderr, "[%s] reboot device...\n", __FUNCTION__);
    int ret = 0;
    return ret;
}


int set_device_factorydefault(int mask)
{
    int ret = 0;
   
    if(mask == 1)//soft
    {
      ;
    }
    else//hard
    {
      ;
    }

    return ret;
}

int fill_resolutions(device_capability_t *dev_cap, int stream_type, int i, int index)
{
    switch(stream_type)
    {
      case MAIN_STREAM:
        {
          dev_cap->videostream[i].resheight[index][0] = 2160;
          dev_cap->videostream[i].reswidth[index][0]  = 3840;
          dev_cap->videostream[i].resheight[index][1] = 1536;
          dev_cap->videostream[i].reswidth[index][1]  = 2592;
          dev_cap->videostream[i].resheight[index][2] = 1080;
          dev_cap->videostream[i].reswidth[index][2]  = 1920;
        }
        break;
      case SUB_STREAM:
        {
          dev_cap->videostream[i].resheight[index][0] = 1080;
          dev_cap->videostream[i].reswidth[index][0]  = 1920;
          dev_cap->videostream[i].resheight[index][1] = 720;
          dev_cap->videostream[i].reswidth[index][1]  = 1280;
          dev_cap->videostream[i].resheight[index][2] = 480;
          dev_cap->videostream[i].reswidth[index][2]  = 720;
        }
        break;
      default:
        break;
    }
}

int get_dev_capability(device_capability_t *dev_cap, int stream_type)
{
    int encodetype;
    int i;
 
    dev_cap->streamnums = get_curr_chs();
    dev_cap->streamnums = dev_cap->streamnums*2;
    encodetype = _H264;
    for(i = 0; i < dev_cap->streamnums; i++)
    {
        if(encodetype & _H264)
        {
            dev_cap->videostream[i].encodeformat |= _H264;
            dev_cap->videostream[i].maxframe[0] = 30;
            dev_cap->videostream[i].minframe[0] = 1;
            dev_cap->videostream[i].maxgovlen[0] = 200;
            dev_cap->videostream[i].mingovlen[0] = 1;
            dev_cap->videostream[i].maxinterval[0] = 3;
            dev_cap->videostream[i].mininterval[0] = 0;

            dev_cap->videostream[i].profileslevelnums = 3;
            dev_cap->videostream[i].profileslevelvalue[0] = 0;
            dev_cap->videostream[i].profileslevelvalue[1] = 1;
            dev_cap->videostream[i].profileslevelvalue[2] = 3;
            fill_resolutions(dev_cap, stream_type, i, 0);
        }
        else if(encodetype & _JPEG)
        {
            dev_cap->videostream[i].encodeformat |= _JPEG;
            dev_cap->videostream[i].maxframe[1] = 30;
            dev_cap->videostream[i].minframe[1] = 1;
            dev_cap->videostream[i].maxgovlen[1] = 200;
            dev_cap->videostream[i].mingovlen[1] = 1;
            dev_cap->videostream[i].maxinterval[1] = 3;
            dev_cap->videostream[i].mininterval[1] = 0;
            fill_resolutions(dev_cap, stream_type, i, 1);

        }
        else if(encodetype & _MPEG4)
        {
            dev_cap->videostream[i].encodeformat |= _MPEG4;
            dev_cap->videostream[i].maxframe[2] = 30;
            dev_cap->videostream[i].minframe[2] = 1;
            dev_cap->videostream[i].maxgovlen[2] = 200;
            dev_cap->videostream[i].mingovlen[2] = 1;
            dev_cap->videostream[i].maxinterval[2] = 3;
            dev_cap->videostream[i].mininterval[2] = 0;
            fill_resolutions(dev_cap, stream_type, i, 2);
        }
    }
}

time_t convert_time(nvt_time_t time)
{
    time_t to;
    struct tm from;

    from.tm_hour    = time.hour;
    from.tm_mday    = time.day;
    from.tm_min     = time.min;
    from.tm_year    = time.year - 1900;
    from.tm_sec     = time.sec;
    from.tm_mon     = time.mon - 1;

    to = mktime(&from);
    if(to < 0) 
    {
    	printf("mktime error! errno:%s\n", strerror(errno));
        return 0;
    }
    return to;
}   

int get_recording_list(nvt_time_t starttime, nvt_time_t endtime)
{
    int ret = 0, chs = 0;
    int i, j;
    int sum = 0;
    int count = 0;
    int res = 0;
    chs = get_curr_chs();
    
    memset(g_nvt->t.record, 0, sizeof(g_nvt->t.record));
#if 0
    for(i = 0; i < chs; i++)
    {
        memcpy(&cond->start_time, &starttime, sizeof(nvt_time_t ));
        memcpy(&cond->stop_time, &endtime, sizeof (nvt_time_t));

        if(ret == 0)
        {
            sum = (msg->size)/sizeof(sdk_record_item_t);
            sum = (sum > MAX_RECORDING)?MAX_RECORDING:sum;
            if(sum == 0)
              continue;
              
            for(j = 0; j < sum; j++)
            {
                sdk_record_item_t *record = (sdk_record_item_t *)(msg->data + sizeof(sdk_record_item_t)*j);
                g_nvt->t.record[i][j].start_time = convert_time(record->start_time);
                g_nvt->t.record[i][j].stop_time = convert_time(record->stop_time);
                sprintf(g_nvt->t.record[i][j].token, "chs_%d_%d_%d", i, g_nvt->t.record[i][j].start_time, g_nvt->t.record[i][j].stop_time);
            }  
            res = sum + res;
            sum = 0;
            g_nvt->t.nrecordings = res;
        }
        else if(ret < 0)
        {
            fprintf(stderr, "\n[%s] cannot get recording_list\n", __FUNCTION__);
        }
    }
#endif
    return 0;
}

int get_recording_info(recording_info_t *info, char *token)
{
  time_t starttm;
  time_t stoptm;
  int chs;
  char *_token = NULL;
  char *part = NULL;
  int i = 0;

  _token = strdup(token);
  if(!strchr(_token, '_'))
      chs = -1;
  part = strtok(_token, "_");
	while((part = strtok(NULL, "_")))
	{
    if(i == 0)
    {
        chs = atoi(part);
    }
    else if(i == 1)
    {
        info->start_time = atoi(part);
    }
    else if(i == 2)
    {
        info->stop_time = atoi(part);
    }
    i++;
	}
	free(_token);
  return chs;
}

int ptz_ctl(int chs, int action, int speed)
{
  int ret = 0;
  GSF_MSG_DEF(gsf_lens_t, lens, sizeof(gsf_msg_t)+sizeof(gsf_lens_t));

  switch(action)
  {
    case NVC_PTZ_ZOOM_ADD_START:
      lens->cmd = GSF_LENS_ZOOM;
      lens->arg1 = 1;
      lens->arg2 = speed;
      break;
    case NVC_PTZ_ZOOM_SUB_START:
      lens->cmd = GSF_LENS_ZOOM;
      lens->arg1 = 0;
      lens->arg2 = speed;
      break;
    case NVC_PTZ_ZOOM_ADD_STOP:
    case NVC_PTZ_ZOOM_SUB_STOP:
      lens->cmd = GSF_LENS_STOP;
      lens->arg1 = 0;
      lens->arg2 = 0;
      break;
    case NVC_PTZ_UP_STOP:
      lens->cmd = GSF_PTZ_STOP;
      lens->arg1 = 0;
      lens->arg2 = 0;
      break;
    case NVC_PTZ_UP_START:
      lens->cmd = GSF_PTZ_UP;
      lens->arg1 = 0;
      lens->arg2 = speed;
      break;  
    case NVC_PTZ_DOWN_START:
      lens->cmd = GSF_PTZ_DOWN;
      lens->arg1 = 0;
      lens->arg2 = speed;
      break;
    case NVC_PTZ_LEFT_START:
      lens->cmd = GSF_PTZ_LEFT;
      lens->arg1 = 0;
      lens->arg2 = speed;
      break;
    case NVC_PTZ_RIGHT_START:
      lens->cmd = GSF_PTZ_RIGHT;
      lens->arg1 = 0;
      lens->arg2 = speed;
      break;          
  }
  if(lens->cmd)
  {  
    ret = GSF_MSG_SENDTO(GSF_ID_CODEC_LENS, chs/2
                        , SET, 0, sizeof(gsf_lens_t)
                        , GSF_IPC_CODEC, 2000);
  }
  return ret;
}
