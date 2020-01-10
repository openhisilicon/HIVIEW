#ifndef __gsf_urldec_h__
#define __gsf_urldec_h__

char* gsf_url_encode(char const *s, int len, int *new_length);
int   gsf_url_decode(char *str, int len);

#endif //__gsf_urldec_h__