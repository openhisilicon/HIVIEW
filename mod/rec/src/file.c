#define _LARGEFILE64_SOURCE
#define _FILE_OFFSET_BITS 64
#include "file.h"

#include "inc/frm.h"

#define PATH_MAX 256
extern const struct mov_buffer_t* mov_file_buffer(void);

fd_av_t*
    fd_av_open(char *name)               /* 打开视频，不存在时创建文件 */
{
    fd_av_t *_hdl = (fd_av_t*)calloc(1, sizeof(fd_av_t));
    _hdl->vtrack = -1;
    _hdl->atrack = -1;
    _hdl->fd = fopen(name, "wb+");
    if(_hdl->fd == NULL)
    {
      free(_hdl);
      return NULL;
    }

	  _hdl->w = fmp4_writer_create(mov_file_buffer(), _hdl->fd, 0);
	  _hdl->s_length = 400*1024;  // init size;
	  _hdl->s_buffer = (uint8_t*)malloc(_hdl->s_length);
	  
	  return _hdl;
}
int fd_av_close(fd_av_t *fd)                     /* 关闭视频文件 */
{
  fd_av_t *_hdl = (fd_av_t*)fd;
  
	fmp4_writer_destroy(_hdl->w);
	fclose(_hdl->fd);
	
	free(_hdl->s_buffer);
	free(_hdl);
	return 0;
}

int fd_av_flush(fd_av_t *fd)                     /* 刷数据到磁盘 */
{
  //fd_av_t *_hdl = (fd_av_t*)fd;
  return 0;
}

int fd_av_write(fd_av_t *fd, char *buf, int size, rec_rw_info_t *info)   /* 写视频文件 */
{  
	int ret = 0;
  fd_av_t *_hdl = (fd_av_t*)fd;

  if (_hdl->fd)
  {
      if (info->type == GSF_FRM_VIDEO_P 
          || info->type == GSF_FRM_VIDEO_I)
      {
          struct mpeg4_avc_t avc;
          struct mpeg4_hevc_t hevc;
          uint8_t extra_data[64 * 1024];
          
          if(size > _hdl->s_length)
          {
            _hdl->s_length = size;
            _hdl->s_buffer = realloc(_hdl->s_buffer, _hdl->s_length);
          }
          //printf("type:%d, enc:%d, s_length:%d\n", info->type, info->enc, _hdl->s_length);
          if(info->enc==GSF_ENC_H264)
          {
             size_t n = mpeg4_annexbtomp4(&avc, buf, size, _hdl->s_buffer, _hdl->s_length);
            
             if(_hdl->vtrack == -1)
             {
              if (!avc.chroma_format_idc || avc.nb_sps < 1 || avc.sps[0].bytes < 4) 
              {
                printf("GSF_ENC_H264 => video wait for key frame [idc:%d, nb_sps:%d, sps:%d]\n"
                      , avc.chroma_format_idc, avc.nb_sps, avc.sps[0].bytes);
                return 0;
              }
              printf("GSF_ENC_H264 => add vtrack info->v.w:%d, info->v.h:%d\n", info->v.w, info->v.h);
      				int extra_data_size = mpeg4_avc_decoder_configuration_record_save(&avc, extra_data, sizeof(extra_data));
      				assert(extra_data_size > 0); // check buffer length
      		    
      		    _hdl->vtrack = fmp4_writer_add_video(_hdl->w, MOV_OBJECT_H264, info->v.w, info->v.h, extra_data, extra_data_size);
            }
            if(_hdl->vtrack != -1)
            {
              //if(info->type == GSF_FRM_VIDEO_I) printf("video write vtrack.\n");
              ret = fmp4_writer_write(_hdl->w, _hdl->vtrack, _hdl->s_buffer, n, info->ms, info->ms, (info->type == GSF_FRM_VIDEO_I)? MOV_AV_FLAG_KEYFREAME : 0);
            }
          
          }
          else if(info->enc==GSF_ENC_H265)
          {
             size_t n = hevc_annexbtomp4(&hevc, buf, size, _hdl->s_buffer, _hdl->s_length);
             if(_hdl->vtrack == -1)
             {
              if (hevc.numOfArrays < 1)
              { 
                printf("GSF_ENC_H265 => wait for key frame\n");
                return 0; 
              }    
      				int extra_data_size = mpeg4_hevc_decoder_configuration_record_save(&hevc, extra_data, sizeof(extra_data));
      				assert(extra_data_size > 0); // check buffer length
      				printf("GSF_ENC_H265 => add vtrack info->v.w:%d, info->v.h:%d\n", info->v.w, info->v.h);
      				_hdl->vtrack = fmp4_writer_add_video(_hdl->w, MOV_OBJECT_HEVC, info->v.w, info->v.h, extra_data, extra_data_size);
            }
            if(_hdl->vtrack != -1)
            {
              ret = fmp4_writer_write(_hdl->w, _hdl->vtrack, _hdl->s_buffer, n, info->ms, info->ms, (info->type == GSF_FRM_VIDEO_I)? MOV_AV_FLAG_KEYFREAME : 0);
            }
          }
          else
          {
             if(_hdl->vtrack == -1)
             {
      				_hdl->vtrack = fmp4_writer_add_video(_hdl->w, MOV_OBJECT_JPEG, info->v.w, info->v.h, "null", strlen("null"));
             }
             ret = fmp4_writer_write(_hdl->w, _hdl->vtrack, buf, size, info->ms, info->ms, (info->type == GSF_FRM_VIDEO_I)? MOV_AV_FLAG_KEYFREAME : 0);
          }
      }
      else if(info->type == GSF_FRM_AUDIO_1)
      {
          if(info->enc==GSF_ENC_G711A
            ||info->enc==GSF_ENC_G711U)
          {
            if(_hdl->atrack == -1)
            {
              _hdl->atrack = fmp4_writer_add_audio(_hdl->w
                                  , (info->enc==GSF_ENC_G711A)?MOV_OBJECT_G711a:MOV_OBJECT_G711u
                                  , info->a.chs, info->a.bps, info->a.sp, NULL, 0);
            }
            
            if(_hdl->atrack != -1)
            {
              ret = fmp4_writer_write(_hdl->w, _hdl->atrack, buf, size, info->ms, info->ms, 0);
            }
          }
          else if(info->enc==GSF_ENC_AAC)
          {
            int rate = 1;
          	struct mpeg4_aac_t aac;
          	uint8_t extra_data[64 * 1024];
        		mpeg4_aac_adts_load((const uint8_t*)buf, size, &aac);
        		
        		if (-1 == _hdl->atrack)
        		{
        			int extra_data_size = mpeg4_aac_audio_specific_config_save(&aac, extra_data, sizeof(extra_data));
        			rate = mpeg4_aac_audio_frequency_to((enum mpeg4_aac_frequency)aac.sampling_frequency_index);
        			_hdl->atrack = fmp4_writer_add_audio(_hdl->w, MOV_OBJECT_AAC
                                  , aac.channel_configuration, 16, rate, extra_data, extra_data_size);
        		}
            if(-1 != _hdl->atrack)
            {
              unsigned char *_buf = buf;
          		int framelen = ((_buf[3] & 0x03) << 11) | (_buf[4] << 3) | (_buf[5] >> 5);
          		//printf("AAC framelen:%d\n", framelen);
          		ret = fmp4_writer_write(_hdl->w, _hdl->atrack, buf + 7, framelen - 7, info->ms, info->ms, 0);
	          }
          }
      }
      else
      {
        return -1;
      }
  }
  
	if (ret != 0)
		printf("info->type = %d, info->enc = %d, ret = %d\n", info->type, info->enc, ret);
		
  _hdl->size += size;
	return ret;
}

uint32_t fd_av_size(fd_av_t *fd)    /* 获取文件大小 */
{
  fd_av_t *_hdl = (fd_av_t*)fd;
  return _hdl->size;
}

static void mov_video_info(void* param, uint32_t track, uint8_t object, int width, int height, const void* extra, size_t bytes)
{
  fd_av_t *_hdl = (fd_av_t*)param;
  
  _hdl->vtrack = track;
  _hdl->vobject = object;
	if (MOV_OBJECT_H264 == object)
	{
		mpeg4_avc_decoder_configuration_record_load((const uint8_t*)extra, bytes, &_hdl->s_avc);
	}
	else if (MOV_OBJECT_HEVC == object)
	{
		mpeg4_hevc_decoder_configuration_record_load((const uint8_t*)extra, bytes, &_hdl->s_hevc);
	}
	else
	{
		;//assert(0);
	}
	
	_hdl->v.w = width;
	_hdl->v.h = height;
	_hdl->v.fps = 0;
}

static void mov_audio_info(void* param, uint32_t track, uint8_t object, int channel_count, int bit_per_sample, int sample_rate, const void* extra, size_t bytes)
{
  fd_av_t *_hdl = (fd_av_t*)param;
  _hdl->atrack = track;
  _hdl->aobject = object;
  
  if (MOV_OBJECT_G711a == _hdl->aobject
      ||MOV_OBJECT_G711u == _hdl->aobject)
  {
    ;
  }
	else if (MOV_OBJECT_AAC == _hdl->aobject)
	{
  	_hdl->s_aac.profile = MPEG4_AAC_LC;
  	_hdl->s_aac.channel_configuration = channel_count;
  	_hdl->s_aac.sampling_frequency_index = mpeg4_aac_audio_frequency_from(sample_rate);
  }
  
  _hdl->a.sp  = sample_rate;
  _hdl->a.bps = bit_per_sample;
  _hdl->a.chs = channel_count;
}

static void onread(void* param, uint32_t track, const void* buffer, size_t bytes, int64_t pts, int64_t dts)
{
  fd_av_t *_hdl = (fd_av_t*)param;
  
  _hdl->type = 0;
  _hdl->pts  = pts;
    
  if(_hdl->vtrack == track)
  {
    if (MOV_OBJECT_H264 == _hdl->vobject)
    {
  		int n = mpeg4_mp4toannexb(&_hdl->s_avc, buffer, bytes, _hdl->out_buffer, *_hdl->out_size);
  		*_hdl->out_size = n;
  		_hdl->type = (_hdl->s_avc.chroma_format_idc)?GSF_FRM_VIDEO_I:GSF_FRM_VIDEO_P; // I.P
    }
    else if(MOV_OBJECT_HEVC == _hdl->vobject)
    {
  		int n = hevc_mp4toannexb(&_hdl->s_hevc, buffer, bytes, _hdl->out_buffer, *_hdl->out_size);
  		*_hdl->out_size = n;
  		_hdl->type = (_hdl->s_hevc.constantFrameRate)?GSF_FRM_VIDEO_I:GSF_FRM_VIDEO_P;  // I.P
    }
    else if(MOV_OBJECT_JPEG == _hdl->vobject)
    {
      int n = bytes;
      memcpy(_hdl->out_buffer, buffer, n);
      *_hdl->out_size = n;
      _hdl->type = GSF_FRM_VIDEO_I; // JPEG
    }
  }
  else if(_hdl->atrack == track)
  {
    _hdl->type = GSF_FRM_AUDIO_1; // A
    
    if (MOV_OBJECT_G711a == _hdl->aobject
        ||MOV_OBJECT_G711u == _hdl->aobject)
    {
      int n = bytes;
      memcpy(_hdl->out_buffer, buffer, n);
      *_hdl->out_size = n;
    }
    else if (MOV_OBJECT_AAC == _hdl->aobject)
    {
  		uint8_t adts[32];
  		int n = mpeg4_aac_adts_save(&_hdl->s_aac, bytes, adts, sizeof(adts));
  		
  		memcpy(_hdl->out_buffer, adts, n);
  		memcpy(_hdl->out_buffer+n, buffer, bytes);
  		*_hdl->out_size = n+bytes;
    }
  }
	else
	{
		;//assert(0);
	}
	
}

fd_av_t*
    fd_av_ropen(char *name, int type, rec_media_info_t *media) /* 打开视频，type: 用作扩展视频文件格式 */
{
  fd_av_t *_hdl = (fd_av_t*)calloc(1, sizeof(fd_av_t));
  _hdl->vtrack = -1;
  _hdl->atrack = -1;
  _hdl->fd = fopen(name, "rb");
  _hdl->r = mov_reader_create(mov_file_buffer(), _hdl->fd);
  _hdl->s_length = 400*1024;  // init size;
  _hdl->s_buffer = (uint8_t*)malloc(_hdl->s_length);
  
	struct mov_reader_trackinfo_t info = { mov_video_info, mov_audio_info };
	mov_reader_getinfo(_hdl->r, &info, _hdl);
  
	if (media)
	{
    media->venc = (_hdl->vobject == MOV_OBJECT_H264)?GSF_ENC_H264:
                    (_hdl->vobject == MOV_OBJECT_HEVC)?GSF_ENC_H265:
                    (_hdl->vobject == MOV_OBJECT_JPEG)?GSF_ENC_MJPEG:
                    GSF_ENC_BUTT;

    media->aenc = (_hdl->aobject == MOV_OBJECT_G711a)?GSF_ENC_G711A:
                    (_hdl->aobject == MOV_OBJECT_G711u)?GSF_ENC_G711U:
                    (_hdl->aobject == MOV_OBJECT_AAC)?GSF_ENC_AAC:
                    GSF_ENC_BUTT;
		
		media->v = _hdl->v;
		media->a = _hdl->a;
	}
  _hdl->size = fd_size((file_t*)_hdl);
  return _hdl;
}
int fd_av_rclose(fd_av_t *fd)                     /* 关闭视频文件 */
{
  fd_av_t *_hdl = (fd_av_t*)fd;
  
	mov_reader_destroy(_hdl->r);
	fclose(_hdl->fd);
	
	free(_hdl->s_buffer);
	free(_hdl);
	return 0;
}
int fd_av_rread(fd_av_t *fd, char *buf, int *size, rec_rw_info_t *info)    /* 读视频文件 */
{
  fd_av_t *_hdl = (fd_av_t*)fd;
  int ret = 0;
  
  _hdl->out_buffer = (uint8_t *)buf;
  _hdl->out_size  = size;
  
  while((ret = mov_reader_read(_hdl->r, _hdl->s_buffer, _hdl->s_length, onread, _hdl)) == ENOMEM)
  {
    _hdl->s_length *= 2; 
    _hdl->s_buffer = realloc(_hdl->s_buffer, _hdl->s_length);
  }
  
  info->type = _hdl->type;
  info->enc  = (_hdl->type == GSF_FRM_VIDEO_I || _hdl->type == GSF_FRM_VIDEO_P)?
                  (_hdl->vobject == MOV_OBJECT_H264)?GSF_ENC_H264:
                  (_hdl->vobject == MOV_OBJECT_HEVC)?GSF_ENC_H265:  
                  (_hdl->vobject == MOV_OBJECT_JPEG)?GSF_ENC_MJPEG: GSF_ENC_BUTT:
               (_hdl->type == GSF_FRM_AUDIO_1)?
                  (_hdl->aobject == MOV_OBJECT_G711a)?GSF_ENC_G711A:
                  (_hdl->aobject == MOV_OBJECT_G711u)?GSF_ENC_G711U:
                  (_hdl->aobject == MOV_OBJECT_AAC)?GSF_ENC_AAC:    GSF_ENC_BUTT:
                  GSF_ENC_BUTT;
  info->ms = _hdl->pts;
  if(info->type < GSF_FRM_AUDIO_1)
    info->v  = _hdl->v;
  else
    info->a  = _hdl->a;
  
  return ret;
}
int fd_av_rseek(fd_av_t *fd, int sec)             /* SEEK到指定的sec秒 */
{
  //mov_reader_seek
  return 0;
}
int fd_av_rsize(fd_av_t *fd)                      /* 获取文件大小 */
{
  fd_av_t *_hdl = (fd_av_t*)fd;
  return _hdl->size;
}


int fd_access(char *name)
{
    return (access(name, 0)!= -1)?1:0;
}

/* 目录操作 */
int fd_dir_mk(char *name)
{
    return mkdir(name, 0666);
}

int fd_dir_mv(char *src, char *dst)
{
    return rename(src, dst);
}

////////////////////////////////////

static int __rm_cb(char *name, int is_dir, void *u)
{
    if(is_dir)
    {
        if(rmdir(name) < 0)
        {
            printf("rmdir name:%s, errno:%d\n", name, errno);
        }
    }
    else
    {
        if(unlink(name) < 0)
        {
            printf("unlink name:%s, errno:%d\n", name, errno);
        }
    }
    return 0;
}

int __dir_each(char *name, int (*cb)(char *name, int is_dir, void *u), void *u)
{
    char dirbuf[PATH_MAX];
    DIR *dp = NULL;
    struct dirent *ep;
    struct stat buf;
    
    off_t file_size = 0;
    //unsigned long long file_size = 0;
    
    if ((dp = opendir(name)) == NULL)
    {
		//printf("!!!!!!!!!!!!!!!!!!!!! opendir %s == NULL\n", name);   	
        return file_size;
    }
    
    while ((ep = readdir(dp)) != NULL)
    {
        snprintf(dirbuf, PATH_MAX, "%s/%s", name, ep->d_name);
        if (strcmp(ep->d_name, ".") == 0 || strcmp(ep->d_name, "..") == 0)
            continue;
        if (lstat(dirbuf, &buf) != 0)
        {
            goto __return;
        }

        if (S_ISDIR(buf.st_mode))
        {
            if ((file_size += __dir_each(dirbuf, cb, u)) < -1)
            {
				continue;
                //  goto __return;
            }
            
            if(cb)
            {
                if(cb(dirbuf, 1, u))
                    goto __return;
            }
        }
        else
        {
            file_size += buf.st_size;
            if(cb)
            {
                if(cb(dirbuf, 0, u))
                    goto __return;
            }
        }
    }
    
__return:
    if(dp)
        closedir(dp);

	//printf("!!!!!!!!!!!!!!!!!!!!! return file_size = %d\n", file_size);	 	
    return file_size;
}


int fd_dir_rm(char *name)
{
    off_t dir_size = __dir_each(name, __rm_cb, NULL);
    rmdir(name);
    return dir_size/(1024*1024);
}


int fd_dir_each(char *name, int (*cb)(char *name, int is_dir, void *u), void *u)
{
    off_t dir_size = __dir_each(name, cb, u);
    return (dir_size>>20);//dir_size/(1024*1024);
}

////////////////////////////////////
/* 文件读写 */
file_t*
    fd_open(char *name, int type)
{
    file_t *_hdl = (file_t*)calloc(1, sizeof(file_t));

    if(fd_access(name))
        _hdl->fd = fopen(name, "rb+");
    else
        _hdl->fd = fopen(name, "wb+");
    return _hdl;
}

int fd_close(file_t *fd)
{
    file_t *_hdl = (file_t*)fd;
    if(_hdl)
    {
		if (_hdl->fd > 0)
        	fclose(_hdl->fd);
		free(_hdl);
    }
    return 0;
}

int fd_flush(file_t *fd)
{
    file_t *_hdl = (file_t*)fd;
    if(_hdl && _hdl->fd > 0)
    {
        fflush(_hdl->fd);
    }
    return 0;
}

int fd_seek(file_t *fd, int offset, int origin)
{
    file_t *_hdl = (file_t*)fd;
    int ret = -EINVAL;
    if(_hdl && _hdl->fd > 0)
    {
        ret = fseek(_hdl->fd, offset, origin);
    }
    return ret;
}


int fd_write(file_t *fd, char *buf, int size)
{
    file_t *_hdl = (file_t*)fd;
    int ret = -EINVAL;
    if(_hdl && _hdl->fd > 0)
    {
        ret = fwrite(buf, 1, size, _hdl->fd);
    }
    return ret;
}

int fd_read(file_t *fd, char *buf, int size)
{
    file_t *_hdl = (file_t*)fd;
    int ret = -EINVAL;
    if(_hdl && _hdl->fd > 0)
    {
        ret = fread(buf, 1, size, _hdl->fd);
    }
    return ret;
}

int fd_rm(char *name)
{
    return unlink(name);
}

int fd_size(file_t *fd)
{
  file_t *_hdl = (file_t*)fd;
  int ret = -EINVAL;
  if(_hdl && _hdl->fd > 0)
  {
    struct stat st;
    if(fstat(fileno(_hdl->fd), &st) == 0)
      ret = st.st_size;
  }
  return ret;
}

int fd_stat(char* filename, uint32_t *size, uint32_t *mtime, uint16_t *mtime_ms)
{
  struct stat st;
  int ret = stat(filename, &st);
  if(ret == 0)
  {
    *size  = st.st_size;
    *mtime = st.st_mtime;
    if(mtime_ms) *mtime_ms = st.st_mtim.tv_nsec/1000000;
  }
  return ret;
}




/////////////////////////////////////