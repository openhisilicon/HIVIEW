
STRUCT(gsf_src_t)
{
  FIELD(bool, en);     // enable
  STRING(host,256);    // onvif://user:pwd@192.168.1.60:80
  
  FIELD(int,  transp); // tcp/udp/mc
  STRING(st1, 256);    // rtsp://user:pwd@192.168.1.60:554/st1
  STRING(st2, 256);    // rtsp://user:pwd@192.168.1.60:554/st2
};

STRUCT(gsf_codec_nvr_t)
{
  STRING(devname, 256);
  ARRAY(gsf_src_t, ch, 16);
};