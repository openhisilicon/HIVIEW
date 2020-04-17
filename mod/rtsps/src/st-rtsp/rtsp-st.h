#ifndef _rtsp_st_h_
#define _rtsp_st_h_

struct rtsp_st_ini_t
{
  unsigned short server_port;
};
int rtsp_st_init();


struct rtsp_st_ctl_t
{
  int id; //RTSP_CTL_ 
  int size;
  unsigned char data[0];
};

int rtsp_st_ctl(struct rtsp_st_ctl_t *ctl);


#endif //!_rtsp_st_h_